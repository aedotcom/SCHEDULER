//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_system_tag.h"
#include "INF_sch_sdm.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A4_default.h"
#include "sch_A4_Sub.h"
//================================================================================



int sch4_RERUN_END_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
int sch4_RERUN_END_S1_ETC[ MAX_CHAMBER ]; // 2005.07.29

int sch4_RERUN_END_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
int sch4_RERUN_END_S2_ETC[ MAX_CHAMBER ]; // 2005.07.29

int sch4_RERUN_FST_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
int sch4_RERUN_FST_S1_ETC[ MAX_CHAMBER ]; // 2005.07.29

int sch4_RERUN_FST_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
int sch4_RERUN_FST_S2_ETC[ MAX_CHAMBER ]; // 2005.07.29

int sch4_RERUN_END_S3_TAG[ MAX_CHAMBER ]; // 2005.11.11

//------------------------------------------------------------------------------------------
int  SDM_4_LOTEND_SIGNAL[ MAX_CHAMBER ];
int  SDM_4_LOTFIRST_SIGNAL[ MAX_CHAMBER ];
int  SDM_4_LOTFIRST_SIDE[ MAX_CHAMBER ];
//
int  SDM_4_LOTCYCLE_SIGNAL[ MAX_CHAMBER ];
int  SDM_4_LOTCYCLE_SIDE[ MAX_CHAMBER ];
//
int  SDM_4_LOTENDFINISH_TAG[ MAX_CHAMBER ]; // 2006.09.02
//
int  SDM_4_RUN_STATUS[ MAX_CASSETTE_SLOT_SIZE ];
int  SDM_4_RUN_SIDE[ MAX_CASSETTE_SLOT_SIZE ];
int  SDM_4_RUN_ACTION[ MAX_CASSETTE_SLOT_SIZE ];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S1_TAG( int ch ) {
	return sch4_RERUN_FST_S1_TAG[ ch ];
}
int SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S2_TAG( int ch ) {
	return sch4_RERUN_FST_S2_TAG[ ch ];
}
int SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( int ch ) {
	return sch4_RERUN_END_S3_TAG[ ch ];
}

//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_AL4_SUB_SDM_PART_DATA( int apmode , int side ) {
	int i;

	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//-----------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			SDM_4_RUN_STATUS[ i ] = 0;
			SDM_4_RUN_ACTION[ i ] = 0; // 2006.01.26
		}
		//-----------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			SDM_4_LOTFIRST_SIGNAL[ i ] = FALSE;
			SDM_4_LOTEND_SIGNAL[ i ] = FALSE;
			//
			SDM_4_LOTENDFINISH_TAG[ i ] = FALSE; // 2006.09.02
			//
			sch4_RERUN_END_S2_TAG[ i ] = -1;
			sch4_RERUN_FST_S2_TAG[ i ] = -1;
			sch4_RERUN_END_S1_TAG[ i ] = -1;
			sch4_RERUN_FST_S1_TAG[ i ] = -1;
			//
			sch4_RERUN_END_S3_TAG[ i ] = FALSE; // 2005.11.11
		}
		//-----------------------------------------------
	}
}

void SCHEDULER_CONTROL_Get_SDM_4_Status_String( int side , int bmch , char *data ) { // 2003.05.29
	int i;

	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		strcpy( data , "OFF" );
		return;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		strcpy( data , "OFF" );
		return;
	}
	//
	sprintf( data , "ON %d %d %d " , _SCH_SDM_Get_LOTFIRST_OPTION( bmch - BM1 + PM1 ) , _SCH_SDM_Get_LOTEND_OPTION( bmch - BM1 + PM1 ) , _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) );
	//-------------------------------------------
	// 0 : NotUse
	// 1 : This Chamber Use,WaferYes
	// 2 : This Chamber Use,WaferNo
	// 3 : Other Chamber Use,WaferYes
	// 4 : Other Chamber Use,WaferNo
	//-------------------------------------------
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) == 0 ) {
			strcat( data , "0" );
		}
		else {
			if ( _SCH_SDM_Get_RUN_CHAMBER( i ) == ( bmch - BM1 + PM1 ) ) {
				if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) > 0 ) {
					strcat( data , "1" );
				}
				else {
					strcat( data , "2" );
				}
			}
			else {
				if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) > 0 ) {
					strcat( data , "3" );
				}
				else {
					strcat( data , "4" );
				}
			}
		}
	}
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Wait_SDM_4_Dummy_Chamber( int ch ) { // 2006.01.26
	int i;
	if ( SDM_4_LOTEND_SIGNAL[ch] ) return TRUE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] != 0 )	return TRUE;
	}
	return FALSE;
}
//
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( int pointer , int style , int id , char *recipe , int mc ) {
	strncpy( recipe , _SCH_SDM_Get_RECIPE( 2 , pointer%100 , style , id ) , mc );
}
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( int pointer , int style , int id ) {
	if ( strlen( _SCH_SDM_Get_RECIPE( 2 , pointer%100 , style , id ) ) > 0 ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( int pointer ,int style ,  int id , char *recipe , int mc ) {
	strncpy( recipe , _SCH_SDM_Get_RECIPE( 0 , pointer%100 , style , id ) , mc );
}
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( int pointer , int style , int id ) {
	if ( strlen( _SCH_SDM_Get_RECIPE( 0 , pointer%100 , style , id ) ) > 0 ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( int pointer , int style , int id , char *recipe , int mc ) {
	strncpy( recipe , _SCH_SDM_Get_RECIPE( 1 , pointer%100 , style , id ) , mc );
}
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_POST_RECIPE( int pointer , int style , int id ) {
	if ( strlen( _SCH_SDM_Get_RECIPE( 1 , pointer%100 , style , id ) ) > 0 ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_LOTEND_POSSIBLE( int ch , int ectpt ) { // 2006.02.01
	int i;
	if ( !SDM_4_LOTEND_SIGNAL[ch] ) return TRUE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( i != ( ectpt % 100 ) ) {
			if ( ( _SCH_SDM_Get_RUN_CHAMBER(i) == ch ) && ( SDM_4_RUN_STATUS[i] != 0 ) ) return FALSE;
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( int ch ) { // 2005.08.04
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		if ( SDM_4_RUN_STATUS[ i ] == 3 ) SDM_4_RUN_STATUS[ i ] = 0;
	}
}
//
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( int pointer , BOOL end ) { // 2005.08.01
	if ( end ) {
		SDM_4_RUN_ACTION[pointer%100] = 3;
		SDM_4_RUN_STATUS[pointer%100] = 2;
	}
	else { // first
		SDM_4_RUN_ACTION[pointer%100] = 3;
		SDM_4_RUN_STATUS[pointer%100] = 1;
	}
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( int ch ) { // 2005.09.21
	int i;
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	//----------------------------------------------------------
	if ( sch4_RERUN_END_S2_TAG[ch] != -1 ) return TRUE; // 2005.11.10
	//----------------------------------------------------------
//###################################################################################################
#endif
//###################################################################################################
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		if ( SDM_4_RUN_ACTION[ i ] == 3 ) {
			if ( SDM_4_RUN_STATUS[ i ] == 2 ) return TRUE;
			if ( SDM_4_RUN_STATUS[ i ] == 12 ) return TRUE;
			if ( SDM_4_RUN_STATUS[ i ] == 4 ) return TRUE; // 2005.11.10
			if ( SDM_4_RUN_STATUS[ i ] == 14 ) return TRUE; // 2005.11.10
		}
	}
	return FALSE;
}


void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( int ch , BOOL endalso ) { // 2005.09.23
	SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
	if ( endalso ) SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2006.01.26
}

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_GO_WHERE( int pointer , int *ch ) {
	*ch = _SCH_SDM_Get_RUN_CHAMBER(pointer%100);
	if ( SDM_4_RUN_STATUS[pointer%100] == 0 ) return FALSE;
//	if ( SDM_4_RUN_ACTION[pointer%100] == 0 ) return FALSE;
	return TRUE;
}
//
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_RUN( int pointer ) {
	switch( SDM_4_RUN_STATUS[pointer%100] ) {
	case 0 :
		break;
	case 1 : // first
		SDM_4_RUN_STATUS[pointer%100] = 11;
		break;
	case 2 : // last
		SDM_4_RUN_STATUS[pointer%100] = 12;
		break;
	case 3 : // cycle
		SDM_4_RUN_STATUS[pointer%100] = 13;
		break;
	case 4 : // end-cycle
		SDM_4_RUN_STATUS[pointer%100] = 14;
		break;
	}
}
//
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( int pointer ) {
	switch( SDM_4_RUN_STATUS[pointer%100] ) {
	case 0 :
		break;
	case 11 : // first
		SDM_4_RUN_STATUS[pointer%100] = 21;
		break;
	case 12 : // last
		SDM_4_RUN_STATUS[pointer%100] = 22;
		break;
	case 13 : // cycle
		SDM_4_RUN_STATUS[pointer%100] = 23;
		break;
	case 14 : // end-cycle
		SDM_4_RUN_STATUS[pointer%100] = 24;
		break;

	case 1 : // 2005.07.28 // first
		if ( SDM_4_RUN_ACTION[pointer%100] == 2 ) { // S1 only
			SDM_4_RUN_STATUS[pointer%100] = 0;
		}
		break;
	case 2 : // 2005.07.28 // last
		if ( SDM_4_RUN_ACTION[pointer%100] == 2 ) { // S1 only
			SDM_4_RUN_STATUS[pointer%100] = 0;
		}
		break;
	case 3 : // 2005.07.28 // cycle
		if ( SDM_4_RUN_ACTION[pointer%100] == 2 ) { // S1 only
			SDM_4_RUN_STATUS[pointer%100] = 0;
		}
		break;
	case 4 : // 2005.07.28 // end-cycle
		if ( SDM_4_RUN_ACTION[pointer%100] == 2 ) { // S1 only
			SDM_4_RUN_STATUS[pointer%100] = 0;
		}
		break;
	}
}
//
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_END_REJECT( int pointer ) { // 2006.09.02
	if ( pointer < 100 ) return FALSE;
	if ( SDM_4_RUN_STATUS[pointer%100] == 0 ) return TRUE;
	return FALSE;
}
//
int SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( int pointer ) {
	int i , ch;
	if ( ( SDM_4_RUN_STATUS[pointer%100] == 2 ) || ( SDM_4_RUN_STATUS[pointer%100] == 12 ) || ( SDM_4_RUN_STATUS[pointer%100] == 22 ) || ( SDM_4_RUN_STATUS[pointer%100] == 4 ) || ( SDM_4_RUN_STATUS[pointer%100] == 14 ) || ( SDM_4_RUN_STATUS[pointer%100] == 24 ) ) {
		ch = _SCH_SDM_Get_RUN_CHAMBER( pointer%100 );
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
			if ( ( SDM_4_RUN_STATUS[i] == 2 ) || ( SDM_4_RUN_STATUS[i] == 12 ) || ( SDM_4_RUN_STATUS[i] == 22 ) || ( SDM_4_RUN_STATUS[i] == 4 ) || ( SDM_4_RUN_STATUS[i] == 14 ) || ( SDM_4_RUN_STATUS[i] == 24 ) ) {
				SDM_4_RUN_STATUS[i] = 0;
			}
		}
//		SDM_4_LOTEND_SIGNAL[ch] = FALSE;
		return 1;
	}
	else {
		if ( SDM_4_RUN_STATUS[pointer%100] == 0 ) return 2;
		SDM_4_RUN_STATUS[pointer%100] = 0;
		return 0;
	}
}

//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1 // 2003.08.23
			if ( SDM_4_RUN_STATUS[ i ] == 1 ) return TRUE;
			if ( SDM_4_RUN_STATUS[ i ] == 2 ) return TRUE;
			if ( SDM_4_RUN_STATUS[ i ] == 3 ) return TRUE;
			if ( SDM_4_RUN_STATUS[ i ] == 4 ) return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN_SIDE( int side ) {
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE; // 2007.07.05
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( SDM_4_RUN_STATUS[ i ] == 2 ) {
			if ( SDM_4_RUN_SIDE[ i ] == side ) return TRUE;
		}
		if ( SDM_4_RUN_STATUS[ i ] == 4 ) {
			if ( SDM_4_RUN_SIDE[ i ] == side ) return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_INIT( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		SDM_4_RUN_STATUS[ i ] = 0;
	}
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		if ( SDM_4_RUN_STATUS[ i ] != 0 ) return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN2( int ch , int *index ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) != ch ) continue;
		*index = i;
		if ( SDM_4_RUN_STATUS[ i ] != 0 ) return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( int pointer ) {
	return SDM_4_RUN_STATUS[ pointer % 100 ] % 10;
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_SDM_4_CHAMER_SIDE_STYLE( int pointer ) {
	return SDM_4_RUN_SIDE[ pointer % 100 ];
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTFIRST_WAFER( int side , int ch ) {
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER( i ) == ch ) {
			if ( SDM_4_RUN_STATUS[i] == 1 ) return TRUE;
		}
		else {
			if ( SDM_4_RUN_STATUS[i] == 1 ) {
//				if ( _SCH_SYSTEM_USING_GET( SDM_4_RUN_SIDE[i] ) >= 9 ) { // 2005.07.29
				if ( _SCH_SYSTEM_USING_RUNNING( SDM_4_RUN_SIDE[i] ) ) { // 2005.07.29
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER( int side ) {
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1 // 2003.08.23
			if ( SDM_4_RUN_STATUS[i] == 3 ) return TRUE;
			if ( SDM_4_RUN_STATUS[i] == 4 ) return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2( int side , int pointer ) { // 2005.08.03
	if ( SDM_4_RUN_STATUS[pointer%100] == 3 ) return TRUE;
	if ( SDM_4_RUN_STATUS[pointer%100] == 4 ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2_RESET( int side , int pointer ) { // 2005.08.03
	SDM_4_RUN_STATUS[pointer%100] = 0;
	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER_DELETE( int side , int pointer ) { // 2005.12.05
	if ( ( SDM_4_RUN_STATUS[pointer%100] % 10 ) == 3 ) {
		SDM_4_RUN_STATUS[pointer%100] = 0;
		return TRUE;
	}
	if ( ( SDM_4_RUN_STATUS[pointer%100] % 10 ) == 4 ) {
		SDM_4_RUN_STATUS[pointer%100] = 0;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_WAFER( int side , int ch ) {
	int i , spd;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return TRUE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return TRUE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_ACTION[i] == 0 ) continue; // 2006.02.05
		if ( SDM_4_RUN_STATUS[i] == 2 ) return TRUE;
		if ( SDM_4_RUN_STATUS[i] == 12 ) return TRUE;
		if ( SDM_4_RUN_STATUS[i] == 4 ) return TRUE;
		if ( SDM_4_RUN_STATUS[i] == 14 ) return TRUE;
	}
	//
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , -1 , ch );
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] == 3 ) {
			//================================================================================
			// 2005.03.15
			//================================================================================
			if ( SDM_4_RUN_ACTION[i] == 2 ) { // S1 only
				if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
					if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
						SDM_4_RUN_STATUS[i] = 0;
						return FALSE;
					}
				}
				else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
					SDM_4_RUN_STATUS[i] = 0;
					return FALSE;
				}
			}
			else {
				if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
					if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
						return FALSE;
					}
				}
				else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
					return FALSE;
				}
			}
			//================================================================================
			SDM_4_RUN_STATUS[i] = 4;
			return TRUE;
		}
		if ( SDM_4_RUN_STATUS[i] == 13 ) {
			SDM_4_RUN_STATUS[i] = 14;
			return TRUE;
		}
	}
	return FALSE;
}
//
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_ONLY_WAFER( int side , int ch ) { // 2006.01.14
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) == ch ) {
			if ( SDM_4_RUN_STATUS[i] ==  1 ) return FALSE;
			if ( SDM_4_RUN_STATUS[i] == 11 ) return FALSE;
			if ( SDM_4_RUN_STATUS[i] ==  3 ) return FALSE;
			if ( SDM_4_RUN_STATUS[i] == 13 ) return FALSE;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT0( int side , int ch ) {
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return TRUE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return TRUE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] == 0 ) {
			if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) > 0 ) return TRUE;
		}
	}
	return FALSE;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( int side , int ch , int *pointer , int *slot ) {
	int i , mx = -1 , s = -1;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) <= 0 ) continue;
		if ( SDM_4_RUN_STATUS[i] == 0 ) { // ready
			if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
				mx = _SCH_SDM_Get_RUN_USECOUNT(i);
				s = i;
			}
		}
	}
	if ( s == -1 ) return FALSE;
	*pointer = s + 100;
	*slot = s + 1;
	return TRUE;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_END_WAFER_SLOT( int side , int ch , int *pointer , int *slot ) { // 2006.01.14
	int i , mx = -1 , s = -1;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( ( SDM_4_RUN_STATUS[i] == 22 ) || ( SDM_4_RUN_STATUS[i] == 24 ) ) { // back end
			if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
				mx = _SCH_SDM_Get_RUN_USECOUNT(i);
				s = i;
			}
		}
	}
	if ( s == -1 ) return FALSE;
	*pointer = s + 100;
	*slot = s + 1;
	return TRUE;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT2( int side , int ch , int *pointer , int *slot ) {
	int i , mx = -1 , s = -1;
	if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , slot ) ) {
		return TRUE;
	}
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] == 0 ) { // ready
			if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
				mx = _SCH_SDM_Get_RUN_USECOUNT(i);
				s = i;
			}
		}
	}
	if ( s == -1 ) return FALSE;
	*pointer = s + 100;
	*slot = s + 1;
	return TRUE;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT3( int side , int ch , int pointer , BOOL zero ) {
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	//
	if ( _SCH_SDM_Get_RUN_CHAMBER(pointer%100) != ch ) return FALSE;
	if ( zero ) {
		if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pointer%100) + 1 ) <= 0 ) return FALSE;
		if ( SDM_4_RUN_STATUS[pointer%100] == 0 ) return TRUE;
	}
	else {
		if ( SDM_4_RUN_STATUS[pointer%100] != 0 ) return TRUE;
	}
	return FALSE;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT4( int side , int ch , int *pointer , int *slot ) {
	int i , mx = -1 , s = -1;
	if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , slot ) ) {
		return TRUE;
	}
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] != 0 ) { // ready
			if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
				mx = _SCH_SDM_Get_RUN_USECOUNT(i);
				s = i;
			}
		}
	}
	if ( s == -1 ) return FALSE;
	*pointer = s + 100;
	*slot = s + 1;
	return TRUE;
}
//
int SCHEDULER_CONTROL_Get_SDM_4_NOTHING_WAFER_SLOT( int side , int ch , int pointer ) { // 2006.07.21
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return -11;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return -12;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( pointer >= 100 ) { // 2007.05.22
			if ( i == ( pointer % 100 ) ) continue;
		}
		if ( SDM_4_RUN_STATUS[i] != 0 ) return -13-(i*100);
		if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) > 0 ) return -14-(i*100);
	}
	return 0;
}
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHANGE_END_WAFER( int side , int ch , int option , int *trgpt , int *trgsts ) {
	int i , spd;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	if ( option == 0 ) return FALSE;
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , -1 , ch );
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( option == 1 ) {
			if ( SDM_4_RUN_STATUS[i] == 3 ) {
				//================================================================================
				// 2005.03.15
				//================================================================================
				if ( SDM_4_RUN_ACTION[i] == 2 ) { // S1 only
					if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
						if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
							SDM_4_RUN_STATUS[i] = 0;
							return FALSE;
						}
					}
					else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
						SDM_4_RUN_STATUS[i] = 0;
						return FALSE;
					}
				}
				else {
					if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
						if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
							return FALSE;
						}
					}
					else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
						return FALSE;
					}
				}
				//================================================================================
				*trgpt = i;
				*trgsts = 4;
				return TRUE;
			}
		}
		else if ( option == 2 ) {
			if ( SDM_4_RUN_STATUS[i] == 3 ) {
				//================================================================================
				// 2005.03.15
				//================================================================================
				if ( SDM_4_RUN_ACTION[i] == 2 ) { // S1 only
					if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
						if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
							SDM_4_RUN_STATUS[i] = 0;
							return FALSE;
						}
					}
					else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
						SDM_4_RUN_STATUS[i] = 0;
						return FALSE;
					}
				}
				else {
					if ( ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 0 /* On */ ) || ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 1 /* On(D) */ ) ) {
						if ( !SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spd , SDM4_PTYPE_LOTEND ) ) {
							return FALSE;
						}
					}
					else if ( _SCH_SDM_Get_LOTEND_OPTION( ch ) == 3 /* On(S1) */ ) {
						return FALSE;
					}
				}
				//================================================================================
				*trgpt = i;
				*trgsts = 4;
				return TRUE;
			}
			else if ( SDM_4_RUN_STATUS[i] == 13 ) {
				*trgpt = i;
				*trgsts = 14;
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
int SCHEDULER_CONTROL_Get_SDM_4_SUPPLY_WAFER_SLOT( int side , int ch , int *pointer , int *slot ) {
	int i , mx = -1 , s = -1 , md = -1;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return -1;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return -1;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) <= 0 ) continue;
		//---------------------------------------------------------------------------------------
		if ( SDM_4_RUN_STATUS[i] == 2 ) {
			if ( side != SDM_4_RUN_SIDE[i] ) {
				if ( !_SCH_SYSTEM_USING_RUNNING( SDM_4_RUN_SIDE[i] ) ) SDM_4_RUN_SIDE[i] = side; // 2005.07.29
			}
		}
		//------------------------------------------------------------------------------------------
		if ( SDM_4_RUN_SIDE[i] != side ) continue;
		//---------------------------------------------------------------------------------------
		if ( SDM_4_RUN_STATUS[i] == 1 ) { // first
			if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1
				if ( ( md == -1 ) || ( md == 2 ) || ( md == 3 ) ) {
					md = 1;
					mx = _SCH_SDM_Get_RUN_USECOUNT(i);
					s = i;
				}
				else {
					if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
						mx = _SCH_SDM_Get_RUN_USECOUNT(i);
						s = i;
					}
				}
			}
		}
		else if ( SDM_4_RUN_STATUS[i] == 2 ) { // end
			if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1
				if ( ( md == -1 ) || ( md == 3 ) ) {
					md = 2;
					mx = _SCH_SDM_Get_RUN_USECOUNT(i);
					s = i;
				}
				else if ( md == 2 ) {
					if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
						mx = _SCH_SDM_Get_RUN_USECOUNT(i);
						s = i;
					}
				}
			}
		}
		else if ( SDM_4_RUN_STATUS[i] == 3 ) { // cycle
			if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1 // 2003.08.23
				if ( md == -1 ) {
					md = 3;
					mx = _SCH_SDM_Get_RUN_USECOUNT(i);
					s = i;
				}
				else if ( md == 3 ) {
					if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
						mx = _SCH_SDM_Get_RUN_USECOUNT(i);
						s = i;
					}
				}
			}
		}
		else if ( SDM_4_RUN_STATUS[i] == 4 ) { // end-cycle
			if ( ( SDM_4_RUN_ACTION[i] != 0 ) && ( SDM_4_RUN_ACTION[i] != 2 ) ) { // not s1 // 2003.08.23
				if ( ( md == -1 ) || ( md == 3 ) ) {
					md = 2;
					mx = _SCH_SDM_Get_RUN_USECOUNT(i);
					s = i;
				}
				else if ( md == 2 ) {
					if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
						mx = _SCH_SDM_Get_RUN_USECOUNT(i);
						s = i;
					}
				}
			}
		}
	}
	if ( s == -1 ) return -1;
	*pointer = s + 100;
	*slot = s + 1;
	return md;
}
//
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( int side , int ch ) {
	int pt , i , spd;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
		return TRUE;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
		return TRUE;
	}
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] == 2 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_RUN_STATUS[i] = 4;
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE;
		} // end
		if ( SDM_4_RUN_STATUS[i] == 12 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_RUN_STATUS[i] = 14;
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE;
		} // end
	}
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] == 3 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE; // cycle
		}
		if ( SDM_4_RUN_STATUS[i] == 13 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE; // cycle
		}
		if ( SDM_4_RUN_STATUS[i] == 4 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE; // end-cycle
		}
		if ( SDM_4_RUN_STATUS[i] == 14 ) {
			//---------------------------------------------------------------
			SDM_4_RUN_SIDE[ i ] = side; // 2005.08.25
			//---------------------------------------------------------------
			SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
			return TRUE; // end-cycle
		}
	}
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTRUN , side , ch );
	if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , &pt , &i ) ) {
		if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pt , spd , SDM4_PTYPE_LOTRUN ) ) { // 2003.08.23
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spd , SDM4_PTYPE_LOTRUN ) ) { // 2003.08.23
				SDM_4_RUN_SIDE[ pt % 100 ] = side;
				SDM_4_RUN_ACTION[ pt % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ pt % 100 ] = 3; // 1:First 2:Last 3:Cycle
				SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
				return TRUE;
			}
			else { // 2003.08.23
				SDM_4_RUN_SIDE[ pt % 100 ] = side;
				SDM_4_RUN_ACTION[ pt % 100 ] = 2; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ pt % 100 ] = 3; // 1:First 2:Last 3:Cycle
				SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
				return TRUE;
			}
		}
		else { // 2003.08.23
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spd , SDM4_PTYPE_LOTRUN ) ) { // 2003.08.23
				SDM_4_RUN_SIDE[ pt % 100 ] = side;
				SDM_4_RUN_ACTION[ pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ pt % 100 ] = 3; // 1:First 2:Last 3:Cycle
				SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
				return TRUE;
			}
			else { // 2003.08.23
				return FALSE; // 2003.08.23
			}
		}
	}
	SDM_4_LOTCYCLE_SIDE[ ch ] = side;
	SDM_4_LOTCYCLE_SIGNAL[ ch ] = TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( int ch ) {
	return SDM_4_LOTFIRST_SIGNAL[ ch ];
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_SDM_4_CHAMER_SIDE_LOTFIRST( int ch ) {
	return SDM_4_LOTFIRST_SIDE[ ch ];
}
//------------------------------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( int side , int ch , int *pt , int option , int First , BOOL *wait , BOOL checkmode ) {
	int i , spd;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
		*wait = FALSE;
		return -1;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
		*wait = FALSE;
		return -2;
	}
	//
	if ( First == 0 ) {
		if ( !SDM_4_LOTFIRST_SIGNAL[ ch ] ) {
			if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTFIRST_WAFER( side , ch ) ) {
				*wait = TRUE;
				return -3;
			}
			if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER( side ) ) {
				*wait = TRUE;
				return -4;
			}
			if ( SDM_4_LOTCYCLE_SIGNAL[ ch ] ) {
				if ( SDM_4_LOTCYCLE_SIDE[ ch ] == side ) {
					if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( side , ch ) ) {
						*wait = TRUE;
						return -5;
					}
				}
				else {
					if ( !_SCH_SYSTEM_USING_RUNNING( SDM_4_LOTCYCLE_SIDE[ ch ] ) ) { // 2005.07.29
						if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( side , ch ) ) {
							*wait = TRUE;
							return -6;
						}
					}
				}
			}
			*wait = FALSE;
			return -7;
		}
	}
	else if ( First == 1 ) {
		if ( !checkmode	) SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE; // 2005.11.11
	}
	else if ( First == 2 ) {
		SDM_4_LOTCYCLE_SIGNAL[ ch ] = FALSE;
		SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
		SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
		SCHEDULER_CONTROL_Get_SDM_4_CHAMER_INIT( ch );
	}
	*wait = SDM_4_LOTFIRST_SIGNAL[ ch ];
	//
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , side , ch ); // 2005.10.26
	//
	//===============================================================================================================
	// 2005.10.26
	//===============================================================================================================
	if ( First != 0 ) {
		if ( !checkmode	) SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( spd , ch ); // 2005.11.11
	}
	else {
		//===========================================================================
		// 2006.07.22
		//===========================================================================
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) {
			*wait = FALSE;
			return -333;
		}
		//===========================================================================
	}
	//===============================================================================================================
	//
	switch( _SCH_SDM_Get_LOTFIRST_OPTION( ch ) ) {
	case 1 : // On
		if ( option == 0 ) {
			if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pt , &i ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						//-----------------------------------------------------------------------------------
						if ( checkmode ) return -8; // 2005.11.11
						//-----------------------------------------------------------------------------------
						SDM_4_RUN_SIDE[ *pt % 100 ] = side;
						SDM_4_RUN_ACTION[ *pt % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -9;
					}
					else { // 2003.08.23
						//-----------------------------------------------------------------------------------
						if ( checkmode ) return 1; // 2005.11.11
						//-----------------------------------------------------------------------------------
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
						return 2; // 2003.08.23
					}
				}
				else { // 2003.08.23
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -10; // 2005.11.11
					//-----------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ *pt % 100 ] = side;
						SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -11;
					}
					else { // 2003.08.23
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
						return -12;
					}
				}
			}
			else {
				//-----------------------------------------------------------------------------------
				if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -13; // 2006.01.24
					//-----------------------------------------------------------------------------------
					SDM_4_LOTFIRST_SIDE[ ch ] = side;
					SDM_4_LOTFIRST_SIGNAL[ ch ] = TRUE;
					return -14;
				}
				else {
					if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pt ) == 2 ) { // 2006.01.17 // 2006.01.23
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
								//-----------------------------------------------------------------------------------
								if ( checkmode ) return -108; // 2005.11.11
								//-----------------------------------------------------------------------------------
								SDM_4_RUN_SIDE[ *pt % 100 ] = side;
								SDM_4_RUN_ACTION[ *pt % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
								SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
								return -109;
							}
							else { // 2003.08.23
								//-----------------------------------------------------------------------------------
								if ( checkmode ) return 101; // 2005.11.11
								//-----------------------------------------------------------------------------------
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
								return 102; // 2003.08.23
							}
						}
						else { // 2003.08.23
							//-----------------------------------------------------------------------------------
							if ( checkmode ) return -110; // 2005.11.11
							//-----------------------------------------------------------------------------------
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
								SDM_4_RUN_SIDE[ *pt % 100 ] = side;
								SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
								SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
								return -111;
							}
							else { // 2003.08.23
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
								return -112;
							}
						}
					}
					else {
						//-----------------------------------------------------------------------------------
						if ( checkmode ) return -15; // 2006.01.24
						//-----------------------------------------------------------------------------------
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -15;
					}
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pt , &i ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return 3; // 2005.11.11
					//-----------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ *pt % 100 ] = side;
						SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return 4;
					}
					else { // 2003.08.23
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
						return 5; // 2003.08.23
					}
				}
				else { // 2003.08.23
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -16; // 2005.11.11
					//-----------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ *pt % 100 ] = side;
						SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -17; // 2003.08.23
					}
					else { // 2003.08.23
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
						return -18; // 2003.08.23
					}
				}
			}
			else {
				//-----------------------------------------------------------------------------------
//				if ( checkmode ) return -19; // 2005.11.11 // 2006.01.24
				//-----------------------------------------------------------------------------------
				if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -19; // 2006.01.24
					//-----------------------------------------------------------------------------------
					SDM_4_LOTFIRST_SIDE[ ch ] = side;
					SDM_4_LOTFIRST_SIGNAL[ ch ] = TRUE;
					return -20;
				}
				else {
					if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pt ) == 2 ) { // 2006.01.17 // 2006.01.23
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
							//-----------------------------------------------------------------------------------
							if ( checkmode ) return 103; // 2005.11.11
							//-----------------------------------------------------------------------------------
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
								SDM_4_RUN_SIDE[ *pt % 100 ] = side;
								SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
								SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
								return 104;
							}
							else { // 2003.08.23
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
								return 105; // 2003.08.23
							}
						}
						else { // 2003.08.23
							//-----------------------------------------------------------------------------------
							if ( checkmode ) return -116; // 2005.11.11
							//-----------------------------------------------------------------------------------
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
								SDM_4_RUN_SIDE[ *pt % 100 ] = side;
								SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
								SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
								return -117; // 2003.08.23
							}
							else { // 2003.08.23
								SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
								SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
								return -118; // 2003.08.23
							}
						}
					}
					else {
						//-----------------------------------------------------------------------------------
						if ( checkmode ) return -21; // 2006.01.24
						//-----------------------------------------------------------------------------------
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -21;
					}
				}
			}
		}
		break;
	case 2 : // On(D)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pt , &i ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -22; // 2005.11.11
					//-----------------------------------------------------------------------------------
					SDM_4_RUN_SIDE[ *pt % 100 ] = side;
					SDM_4_RUN_ACTION[ *pt % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
					return -23;
				}
				else { // 2003.08.23
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return 6; // 2005.11.11
					//-----------------------------------------------------------------------------------
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
					return 7; // 2003.08.23
				}
			}
			else {
				//-----------------------------------------------------------------------------------
				if ( checkmode ) return -24; // 2005.11.11
				//-----------------------------------------------------------------------------------
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ *pt % 100 ] = side;
					SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
					return -25;
				}
				else { // 2003.08.23
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
					return -26;
				}
			}
		}
		else {
			//-----------------------------------------------------------------------------------
//			if ( checkmode ) return -27; // 2005.11.11 // 2006.01.24
			//-----------------------------------------------------------------------------------
			if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
				//-----------------------------------------------------------------------------------
				if ( checkmode ) return -27; // 2006.01.24
				//-----------------------------------------------------------------------------------
				SDM_4_LOTFIRST_SIDE[ ch ] = side;
				SDM_4_LOTFIRST_SIGNAL[ ch ] = TRUE;
				return -28;
			}
			else {
				if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pt ) == 2 ) { // 2006.01.17 // 2006.01.23
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
							//-----------------------------------------------------------------------------------
							if ( checkmode ) return -122; // 2005.11.11
							//-----------------------------------------------------------------------------------
							SDM_4_RUN_SIDE[ *pt % 100 ] = side;
							SDM_4_RUN_ACTION[ *pt % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
							SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
							SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
							SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
							return -123;
						}
						else { // 2003.08.23
							//-----------------------------------------------------------------------------------
							if ( checkmode ) return 106; // 2005.11.11
							//-----------------------------------------------------------------------------------
							SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
							SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
							return 107; // 2003.08.23
						}
					}
					else {
						//-----------------------------------------------------------------------------------
						if ( checkmode ) return -24; // 2005.11.11
						//-----------------------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
							SDM_4_RUN_SIDE[ *pt % 100 ] = side;
							SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
							SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
							SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
							SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
							return -125;
						}
						else { // 2003.08.23
							SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
							SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
							return -126;
						}
					}
				}
				else {
					//-----------------------------------------------------------------------------------
					if ( checkmode ) return -29; // 2006.01.24
					//-----------------------------------------------------------------------------------
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
					return -29;
				}
			}
		}
		break;
	case 3 : // On(S1)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT2( side , ch , pt , &i ) ) { // 2003.08.23
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
				//-----------------------------------------------------------------------------------
				if ( checkmode ) return 8; // 2005.11.11
				//-----------------------------------------------------------------------------------
				SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
				SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
				return 9;
			}
			else {
				//-----------------------------------------------------------------------------------
				if ( checkmode ) return -30; // 2005.11.11
				//-----------------------------------------------------------------------------------
				SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
				SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
				return -31;
			}
		}
		else {
			//-----------------------------------------------------------------------------------
			if ( checkmode ) return -32; // 2005.11.11
			//-----------------------------------------------------------------------------------
			SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
			SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
			return -33;
		}
		break;
	case 4 : // On(S2)
		//-----------------------------------------------------------------------------------
		if ( checkmode ) return -34; // 2005.11.11
		//-----------------------------------------------------------------------------------
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pt , &i ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
				SDM_4_RUN_SIDE[ *pt % 100 ] = side;
				SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
				SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
				SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
				return -35;
			}
			else { // 2003.08.23
				SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
				SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
				return -36;
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
				SDM_4_LOTFIRST_SIDE[ ch ] = side;
				SDM_4_LOTFIRST_SIGNAL[ ch ] = TRUE;
				return -37;
			}
			else {
				if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pt ) == 2 ) { // 2006.01.17 // 2006.01.23
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pt , spd , SDM4_PTYPE_LOTFIRST ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ *pt % 100 ] = side;
						SDM_4_RUN_ACTION[ *pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ *pt % 100 ] = 1; // 1:First 2:Last 3:Cycle
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
						return -135;
					}
					else { // 2003.08.23
						SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE; // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ ch ] = FALSE; // 2003.08.23
						return -136;
					}
				}
				else {
					SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
					SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
					return -38;
				}
			}
		}
		break;
	default : // off
		//-----------------------------------------------------------------------------------
		if ( checkmode ) return -39; // 2005.11.11
		//-----------------------------------------------------------------------------------
		SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
		SDM_4_LOTEND_SIGNAL[ ch ] = FALSE;
		return -40;
		break;
	}
	return -99;
}
//
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_MAKE_LOTFIRST_TO_LOTEND( int ch ) { // 2004.09.07
	switch( _SCH_SDM_Get_LOTEND_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		SDM_4_LOTEND_SIGNAL[ch] = FALSE;
		break;
	case 3 : // On(S1)
		SDM_4_LOTEND_SIGNAL[ch] = FALSE;
		break;
	case 4 : // On(S2)
		SDM_4_LOTEND_SIGNAL[ch] = FALSE;
		break;
	default : // off
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		break;
	}
}
//------------------------------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_LOTFIRST( int side , int ch , int pt ) {
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		return FALSE;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		return FALSE;
	}
	//
	SDM_4_LOTFIRST_SIGNAL[ ch ] = FALSE;
	//
	SCHEDULER_CONTROL_Set_SDM_4_CHAMER_MAKE_LOTFIRST_TO_LOTEND( ch ); // 2004.09.07
	//
	switch( _SCH_SDM_Get_LOTFIRST_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		SDM_4_RUN_SIDE[ pt % 100 ] = side;
		SDM_4_RUN_ACTION[ pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
		SDM_4_RUN_STATUS[ pt % 100 ] = 11; // 1:First 2:Last 3:Cycle
		return TRUE;
		break;
	case 3 : // On(S1)
		return TRUE;
		break;
	case 4 : // On(S2)
		SDM_4_RUN_SIDE[ pt % 100 ] = side;
		SDM_4_RUN_ACTION[ pt % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
		SDM_4_RUN_STATUS[ pt % 100 ] = 11; // 1:First 2:Last 3:Cycle
		return FALSE;
		break;
	default : // off
		return FALSE;
		break;
	}
	return FALSE;
}
//
int SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( int side , int ch , int switchp , int checkstatus , int index ) {
	int pointer , slot , spd;
	int trgpt , trgsts;

	if ( SDM_4_LOTEND_SIGNAL[ch] ) return 1;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return 2;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return 3;
	}
	if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_WAFER( side , ch ) ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return 4;
	}
	if ( checkstatus != 0 ) {
		if ( !SCHEDULER_CONTROL_Get_SDM_4_CHANGE_END_WAFER( side , ch , checkstatus , &trgpt , &trgsts ) ) {
			trgpt = -1;
		}
	}
	else {
		trgpt = -1;
	}
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , side , ch );
	switch( _SCH_SDM_Get_LOTEND_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		if ( trgpt == -1 ) {
			if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , &pointer , &slot ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ pointer % 100 ]   = side;
						SDM_4_RUN_ACTION[ pointer % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
					else { // 2003.08.23
						SDM_4_RUN_SIDE[ pointer % 100 ]   = side;
						SDM_4_RUN_ACTION[ pointer % 100 ] = 2; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
				}
				else { // 2003.08.23
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ pointer % 100 ]   = side;
						SDM_4_RUN_ACTION[ pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
					else { // 2003.08.23
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
				}
			}
			else {
				if ( switchp == -1 ) {
					if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( switchp , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( switchp , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
							SDM_4_RUN_SIDE[ switchp % 100 ]   = side;
							SDM_4_RUN_ACTION[ switchp % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
							SDM_4_RUN_STATUS[ switchp % 100 ] = 2; // 1:First 2:Last 3:Cycle
							SDM_4_LOTEND_SIGNAL[ch] = TRUE;
						}
						else { // 2003.08.23
							SDM_4_RUN_SIDE[ switchp % 100 ]   = side;
							SDM_4_RUN_ACTION[ switchp % 100 ] = 2; // 1:s1.s2 2:s1 3:s2
							SDM_4_RUN_STATUS[ switchp % 100 ] = 2; // 1:First 2:Last 3:Cycle
							SDM_4_LOTEND_SIGNAL[ch] = TRUE;
						}
					}
					else { // 2003.08.23
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( switchp , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
							SDM_4_RUN_SIDE[ switchp % 100 ]   = side;
							SDM_4_RUN_ACTION[ switchp % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
							SDM_4_RUN_STATUS[ switchp % 100 ] = 2; // 1:First 2:Last 3:Cycle
							SDM_4_LOTEND_SIGNAL[ch] = TRUE;
						}
						else { // 2003.08.23
							SDM_4_LOTEND_SIGNAL[ch] = TRUE;
						}
					}
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( trgpt , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( trgpt , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ trgpt ]   = side;
					SDM_4_RUN_ACTION[ trgpt ] = 1; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ trgpt ] = trgsts; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
				else { // 2003.08.23
					SDM_4_RUN_SIDE[ trgpt ]   = side;
					SDM_4_RUN_ACTION[ trgpt ] = 2; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ trgpt ] = trgsts; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
			}
			else { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( trgpt , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ trgpt ]   = side;
					SDM_4_RUN_ACTION[ trgpt ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ trgpt ] = trgsts; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
				else { // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
			}
		}
		break;
	case 3 : // On(S1)
		break;
	case 4 : // On(S2)
		if ( trgpt == -1 ) {
			if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , &pointer , &slot ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ pointer % 100 ]   = side;
					SDM_4_RUN_ACTION[ pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
				else {
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				}
			}
			else {
				if ( switchp == -1 ) {
					if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( switchp , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
						SDM_4_RUN_SIDE[ switchp % 100 ]   = side;
						SDM_4_RUN_ACTION[ switchp % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
						SDM_4_RUN_STATUS[ switchp % 100 ] = 2; // 1:First 2:Last 3:Cycle
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
					else {
						SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					}
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( trgpt , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				SDM_4_RUN_SIDE[ trgpt ]   = side; // 2004.09.07
				SDM_4_RUN_ACTION[ trgpt ] = 3; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ trgpt ] = trgsts; // 1:First 2:Last 3:Cycle
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
			else {
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
		}
		break;
	default : // off
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		break;
	}
	return 5;
}
//
int SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( int side , int ch , BOOL Cycle , int *pointer ) { // 2003.08.23
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		//==============================================================
//		if ( SDM_4_RUN_SIDE[i] != side ) continue; // 2005.08.25
		//==============================================================
		//---------------------------------------------------------------------------------------
		if ( Cycle ) {
//			if ( SDM_4_RUN_STATUS[i] == 3 ) { // cycle // 2005.08.25
			if ( ( SDM_4_RUN_STATUS[i] == 3 ) || ( SDM_4_RUN_STATUS[i] == 4 ) ) { // cycle // 2005.08.25
				if ( SDM_4_RUN_ACTION[i] == 2 ) { // s1
					*pointer = i;
					//==============================================================
					if ( SDM_4_RUN_SIDE[i] == side ) return 1; // 2005.08.25
					//==============================================================
					//==============================================================
					SDM_4_RUN_SIDE[i] = side; // 2005.08.25
					//==============================================================
					return 2;
				}
			}
		}
		else {
			if ( ( SDM_4_RUN_STATUS[i] == 2 ) || ( SDM_4_RUN_STATUS[i] == 4 ) ) { // end
				if ( SDM_4_RUN_ACTION[i] == 2 ) { // s1
					*pointer = i;
					//==============================================================
					if ( SDM_4_RUN_SIDE[i] == side ) return 1; // 2005.08.25
					//==============================================================
					//==============================================================
					SDM_4_RUN_SIDE[i] = side; // 2005.08.25
					//==============================================================
					return 2;
				}
			}
		}
	}
	return 0;
}
//

BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_PM_ENDMONITOR( int side , int ch , int *pointer , int index ) {
	int slot , spd;

	if ( SDM_4_LOTEND_SIGNAL[ch] ) {
		if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( side , ch , FALSE , pointer ) >= 1 ) { // 2003.08.26
			SDM_4_RUN_ACTION[ *pointer % 100 ] = 0; // 1:s1.s2 2:s1 3:s2 // 2003.08.26
			SDM_4_RUN_STATUS[ *pointer % 100 ] = 0; // 1:First 2:Last 3:Cycle // 2003.08.26
			return TRUE; // 2003.08.26
		} // 2003.08.26
		return FALSE;
	}
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return FALSE;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return FALSE;
	}
	if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_WAFER( side , ch ) ) {
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		return FALSE;
	}
	spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , side , ch );
	switch( _SCH_SDM_Get_LOTEND_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , &slot ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ *pointer % 100 ]   = side;
					SDM_4_RUN_ACTION[ *pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ *pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return TRUE;
				}
				else { // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return TRUE;
				}
			}
			else { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_SIDE[ *pointer % 100 ]   = side;
					SDM_4_RUN_ACTION[ *pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ *pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return FALSE;
				}
				else { // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return FALSE;
				}
			}
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
		}
		break;
	case 3 : // On(S1)
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT2( side , ch , pointer , &slot ) ) { // 2003.08.23
			return TRUE;
		}
		else {
			return FALSE;
		}
		return TRUE;
		break;
	case 4 : // On(S2)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , &slot ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				SDM_4_RUN_SIDE[ *pointer % 100 ]   = side;
				SDM_4_RUN_ACTION[ *pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ *pointer % 100 ] = 2; // 1:First 2:Last 3:Cycle
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
			else { // 2003.08.23
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
			return FALSE;
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN( ch ) ) {
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
		}
		break;
	default : // off
		SDM_4_LOTEND_SIGNAL[ch] = TRUE;
		break;
	}
	return FALSE;
}
//
int SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTEND( int side , int ch , int *pointer , int spdata ) { // 2005.07.29
	int slot;

	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		*pointer = -1;
		return 0;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		*pointer = -2;
		return 0;
	}
	//============================================================================
	// 2006.08.30
	//============================================================================
	if ( SDM_4_LOTEND_SIGNAL[ch] ) {
		*pointer = 0;
		return 0;
	}
	//============================================================================
	//============================================================================
	switch( _SCH_SDM_Get_LOTEND_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , &slot ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
					// spawn Process + Pick
					return 2; // s1.s2
				}
				else {
					// spawn Process
					return 1; // s1
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
					// Pick
					return 3; // s2
				}
				else {
					// nothing
					*pointer = -3; // 2006.08.30
					return 0;
				}
			}
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN2( ch , pointer ) ) {
				if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pointer ) == 2 ) { // 2006.01.17
					if ( *pointer < 100 ) *pointer = *pointer + 100;
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
							// spawn Process + Pick(Wait)
							return 4; // s1.s2
						}
						else {
							// spawn Process
							return 1; // s1
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
							// Pick(Wait)
							return 5; // s2
						}
						else {
							// nothing
							*pointer = -4; // 2006.08.30
							return 0;
						}
					}
				}
				else {
					// nothing
					*pointer = -5; // 2006.08.30
					return 0;
				}
			}
			else {
				if ( *pointer < 100 ) *pointer = *pointer + 100;
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
						// spawn Process + Pick(Wait)
						return 4; // s1.s2
					}
					else {
						// spawn Process
						return 1; // s1
					}
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
						// Pick(Wait)
						return 5; // s2
					}
					else {
						// nothing
						*pointer = -6; // 2006.08.30
						return 0;
					}
				}
			}
		}
		break;
	case 3 : // On(S1)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT2( side , ch , pointer , &slot ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
				// spawn Process
				return 1; // s1
			}
			else {
				*pointer = -7; // 2006.08.30
				return 0;
			}
		}
		else {
			*pointer = -8; // 2006.08.30
			return 0;
		}
		break;
	case 4 : // On(S2)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( side , ch , pointer , &slot ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
				// Pick
				return 3; // s2
			}
			else {
				// nothing
				*pointer = -9; // 2006.08.30
				return 0;
			}
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_HAVE_RUN2( ch , pointer ) ) {
				if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( ch , pointer ) == 2 ) { // 2006.01.17
					if ( *pointer < 100 ) *pointer = *pointer + 100;
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
						// Pick(Wait)
						return 5; // s2
					}
					else {
						// nothing
						*pointer = -10; // 2006.08.30
						return 0;
					}
				}
				else {
					// nothing
					*pointer = -11; // 2006.08.30
					return 0;
				}
			}
			else {
				if ( *pointer < 100 ) *pointer = *pointer + 100;
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( *pointer , spdata , SDM4_PTYPE_LOTEND ) ) {
					// Pick(Wait)
					return 5; // s2
				}
				else {
					// nothing
					*pointer = -12; // 2006.08.30
					return 0;
				}
			}
		}
		break;
	default : // off
		*pointer = -13; // 2006.08.30
		return 0;
		break;
	}
	*pointer = -14; // 2006.08.30
	return 0;
}
//=================================================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( int side , int ch , int mode , int pt , int *rpointer , int *rmode , int spdata ) { // 2005.10.25
	int i , mx = -1 , ns = -1 , nm;

	if ( ( mode != 2 ) && ( mode != 3 ) && ( mode != 4 ) && ( mode != 5 ) ) return FALSE;
	//-------------------------------------------------------------------------------------------
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( ( pt % 100 ) != i ) {
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) == ch ) {
				if ( SDM_4_RUN_STATUS[i] == 0 ) {
					if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) > 0 ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spdata , SDM4_PTYPE_LOTFIRST ) ) {
							if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
								mx = _SCH_SDM_Get_RUN_USECOUNT(i);
								ns = i + 100;
								if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( i , spdata , SDM4_PTYPE_LOTFIRST ) ) {
									nm = 2;
								}
								else {
									nm = 3;
								}
							}
						}
					}
				}
			}
		}
	}
	//-------------------------------------------------------------------------------------------
	if ( ns == -1 ) {
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
			if ( ( pt % 100 ) != i ) {
				if ( _SCH_SDM_Get_RUN_CHAMBER(i) == ch ) {
					if ( SDM_4_RUN_STATUS[i] > 0 ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( i , spdata , SDM4_PTYPE_LOTFIRST ) ) {
							if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
								mx = _SCH_SDM_Get_RUN_USECOUNT(i);
								ns = i + 100;
								if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( i , spdata , SDM4_PTYPE_LOTFIRST ) ) {
									nm = 2;
								}
								else {
									nm = 3;
								}
							}
						}
					}
				}
			}
		}
	}
	//-------------------------------------------------------------------------------------------
	if ( ns == -1 ) return FALSE;
	*rpointer = ns;
	*rmode = nm;
	return TRUE;
}
//
//

int SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( int side , int ch , int endpick , int ptx , int *pointer , int spdata ) {
	int rp , rm , pt;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return 0;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return 0;
	//
	if ( endpick == 2 ) { // 2005.10.25 // Nothing
		if ( !SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT4( side , ch , &pt , &rm ) ) return 0;
	}
	else {
		pt = ptx;
	}
	switch( _SCH_SDM_Get_LOTFIRST_OPTION( ch ) ) {
	case 1 : // On
	case 2 : // On(D)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT3( side , ch , pt , TRUE ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
					//===================================================================================================
					// 2005.10.25
					//===================================================================================================
					if ( endpick == 1 ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 2 , pt , &rp , &rm , spdata ) ) {
							*pointer = rp;
							return rm;
						}
					}
					//===================================================================================================
					// spawn Process + Pick
					*pointer = pt;
					return 2; // s1.s2
				}
				else {
					// spawn Process
					*pointer = pt;
					return 1; // s1
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
					//===================================================================================================
					// 2005.10.25
					//===================================================================================================
					if ( endpick == 1 ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 3 , pt , &rp , &rm , spdata ) ) {
							*pointer = rp;
							return rm;
						}
					}
					//===================================================================================================
					// Pick
					*pointer = pt;
					return 3; // s2
				}
				else {
					// nothing
					*pointer = pt;
					return 0;
				}
			}
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT3( side , ch , pt , FALSE ) ) {
				if ( endpick == 2 ) { // 2006.02.07
					// nothing
					*pointer = pt;
					return 0; // 2006.02.07
				}
			}
//			else { // 2006.02.07
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
						//===================================================================================================
						// 2005.10.25
						//===================================================================================================
						if ( endpick == 1 ) {
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 4 , pt , &rp , &rm , spdata ) ) {
								*pointer = rp;
								return rm;
							}
						}
						//===================================================================================================
						// spawn Process + Pick(Wait)
						*pointer = pt;
						return 4; // s1.s2
					}
					else {
						// spawn Process
						*pointer = pt;
						return 1; // s1
					}
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
						//===================================================================================================
						// 2005.10.25
						//===================================================================================================
						if ( endpick == 1 ) {
							if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 5 , pt , &rp , &rm , spdata ) ) {
								*pointer = rp;
								return rm;
							}
						}
						//===================================================================================================
						// Pick(Wait)
						*pointer = pt;
						return 5; // s2
					}
					else {
						// nothing
						*pointer = pt;
						return 0;
					}
				}
//		} // 2006.02.07
		}
		break;
	case 3 : // On(S1)
		if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
			// spawn Process
			*pointer = pt;
			return 1; // s1
		}
		else {
			// nothing
			*pointer = pt;
			return 0;
		}
		break;
	case 4 : // On(S2)
		if ( SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT3( side , ch , pt , TRUE ) ) {
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
				//===================================================================================================
				// 2005.10.25
				//===================================================================================================
				if ( endpick == 1 ) {
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 3 , pt , &rp , &rm , spdata ) ) {
						*pointer = rp;
						return rm;
					}
				}
				//===================================================================================================
				// Pick
				*pointer = pt;
				return 3; // s2
			}
			else {
				// nothing
				*pointer = pt;
				return 0;
			}
		}
		else {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT3( side , ch , pt , FALSE ) ) {
				if ( endpick == 2 ) { // 2006.02.07
					// nothing
					*pointer = pt;
					return 0;
				} // 2006.02.07
			}
//			else { // 2006.02.07
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pt , spdata , SDM4_PTYPE_LOTFIRST ) ) {
					//===================================================================================================
					// 2005.10.25
					//===================================================================================================
					if ( endpick == 1 ) {
						if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_LOTFIRST_AT_PR_OTHER( side , ch , 5 , pt , &rp , &rm , spdata ) ) {
							*pointer = rp;
							return rm;
						}
					}
					//===================================================================================================
					// Pick(Wait)
					*pointer = pt;
					return 5; // s2
				}
				else {
					// nothing
					*pointer = pt;
					return 0;
				}
//			} // 2006.02.07
		}
		break;
	default : // off
		// nothing
		*pointer = pt;
		return 0;
		break;
	}
	*pointer = pt;
	return 0;
}
//
//
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( int side , int pointer ) {
	int ch , spd;

	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) {
		return FALSE;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) {
		return FALSE;
	}
	ch = _SCH_SDM_Get_RUN_CHAMBER( pointer % 100 );
	//====================================================================================================
	// 2006.02.01
	//====================================================================================================
	//if ( SDM_4_LOTEND_SIGNAL[ch] ) return FALSE; // 2006.02.01
	//=========================================================================================
	if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_LOTEND_POSSIBLE( ch , pointer ) ) return FALSE; // 2006.02.01
	//=========================================================================================
//	if ( SDM_4_RUN_STATUS[ pointer % 100 ] == 13 ) { // 1:First 2:Last 3:Cycle // 2005.03.10
	if ( ( SDM_4_RUN_STATUS[ pointer % 100 ] == 13 ) || ( ( SDM_4_RUN_STATUS[ pointer % 100 ] == 3 ) && ( SDM_4_RUN_ACTION[ pointer % 100 ] == 2 ) ) ) { // 1:First 2:Last 3:Cycle // 2005.03.10
		spd = SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , -1 , ch );
		switch( _SCH_SDM_Get_LOTEND_OPTION( ch ) ) {
		case 1 : // On
		case 2 : // On(D)
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_ACTION[ pointer % 100 ] = 1; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ pointer % 100 ] = 14; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return FALSE;
				}
				else { // 2003.08.23
					SDM_4_RUN_ACTION[ pointer % 100 ] = 2; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ pointer % 100 ] = 14; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return TRUE;
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
					SDM_4_RUN_ACTION[ pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
					SDM_4_RUN_STATUS[ pointer % 100 ] = 14; // 1:First 2:Last 3:Cycle
					SDM_4_LOTEND_SIGNAL[ch] = TRUE;
					return FALSE;
				}
				else { // 2003.08.23
					SDM_4_LOTEND_SIGNAL[ch] = TRUE; // 2003.08.26
					return FALSE; // 2003.08.26
				}
			}
			break;
		case 3 : // On(S1)
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				SDM_4_RUN_ACTION[ pointer % 100 ] = 2; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ pointer % 100 ] = 14; // 1:First 2:Last 3:Cycle
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
			}
			else {
				SDM_4_LOTEND_SIGNAL[ch] = TRUE; // 2003.08.26
				return FALSE; // 2003.08.26
			}
			return TRUE;
			break;
		case 4 : // On(S2)
			if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( pointer , spd , SDM4_PTYPE_LOTEND ) ) { // 2003.08.23
				SDM_4_RUN_ACTION[ pointer % 100 ] = 3; // 1:s1.s2 2:s1 3:s2
				SDM_4_RUN_STATUS[ pointer % 100 ] = 14; // 1:First 2:Last 3:Cycle
				SDM_4_LOTEND_SIGNAL[ch] = TRUE;
				return FALSE;
			}
			else {
				SDM_4_LOTEND_SIGNAL[ch] = TRUE; // 2003.08.26
				return FALSE; // 2003.08.26
			}
			break;
		default : // off
			SDM_4_LOTEND_SIGNAL[ch] = TRUE; // 2003.08.26
			return FALSE; // 2003.08.26
			break;
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_ENDFINISH( int ch ) { // 2006.09.02
	int i;
	if ( ( !SDM_4_LOTFIRST_SIGNAL[ch] ) && ( SDM_4_LOTEND_SIGNAL[ch] ) ) {
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
			if ( SDM_4_RUN_STATUS[i] != 0 ) {
				if ( SDM_4_RUN_STATUS[i] != 22 ) {
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_ENDFINISH( int ch ) { // 2006.09.02
	SDM_4_LOTEND_SIGNAL[ch] = FALSE;
	SDM_4_LOTFIRST_SIGNAL[ch] = TRUE;
	//
	SDM_4_LOTENDFINISH_TAG[ch] = FALSE;
}

void SCHEDULER_CONTROL_Reset_SDM_4_CHAMER_ENDFINISH( int ch ) { // 2006.09.02
	SDM_4_LOTENDFINISH_TAG[ch] = FALSE;
}

void SCHEDULER_CONTROL_Reg_SDM_4_CHAMER_ENDFINISH( int ch ) { // 2006.09.02
	SDM_4_LOTENDFINISH_TAG[ch] = TRUE;
}

BOOL SCHEDULER_CONTROL_Run_SDM_4_CHAMER_ENDFINISH( int ch ) { // 2006.09.02
	return SDM_4_LOTENDFINISH_TAG[ch];
}

BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTAPPEND( int side , int ch ) {
	int i;

//printf( ">>> LA [side = %d][ch = %d]===============\n" , side , ch );

	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return TRUE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return TRUE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( _SCH_SDM_Get_RUN_CHAMBER(i) != ch ) continue;
		if ( SDM_4_RUN_STATUS[i] ==  2 ) {
			SDM_4_RUN_STATUS[i] = 0;
			SDM_4_LOTEND_SIGNAL[ch] = FALSE;
			return TRUE; // 2006.09.02
//printf( ">>> LA 2->0 [side = %d][ch = %d]===============\n" , side , ch );
//SCHEDULER_CONTROL_Set_SDM_4_PRINTF();
		}
		if ( SDM_4_RUN_STATUS[i] == 12 ) {
			SDM_4_RUN_ACTION[i] = 0; //
			SDM_4_LOTEND_SIGNAL[ch] = FALSE;
			return TRUE; // 2006.09.02
//printf( ">>> LA 12->0 [side = %d][ch = %d]===============\n" , side , ch );
//SCHEDULER_CONTROL_Set_SDM_4_PRINTF();
		}
		if ( SDM_4_RUN_STATUS[i] ==  4 ) {
			SDM_4_RUN_STATUS[i] = 3;
			SDM_4_LOTEND_SIGNAL[ch] = FALSE;
			return TRUE; // 2006.09.02
//printf( ">>> LA 4->3 [side = %d][ch = %d]===============\n" , side , ch );
//SCHEDULER_CONTROL_Set_SDM_4_PRINTF();
		}
		if ( SDM_4_RUN_STATUS[i] == 14 ) {
			SDM_4_RUN_STATUS[i] = 13;
			SDM_4_LOTEND_SIGNAL[ch] = FALSE;
			return TRUE; // 2006.09.02
//printf( ">>> LA 14->13 [side = %d][ch = %d]===============\n" , side , ch );
//SCHEDULER_CONTROL_Set_SDM_4_PRINTF();
		}
	}
	return FALSE; // 2006.09.02
}
//
//

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PLACE_PM_SKIP_CHECK( int pointer ) {
	switch( ( SDM_4_RUN_STATUS[ pointer % 100 ] % 10 ) ) {
	case 0 :
		return TRUE;
		break;
	case 1 : // first
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 0 ) return TRUE;
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 2 ) return TRUE;
		break;
	case 2 : // end
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 0 ) return TRUE;
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 2 ) return TRUE;
		break;
	case 3 : // cycle
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 0 ) return TRUE;
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 2 ) return TRUE;
		break;
	case 4 : // end-cycle
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 0 ) return TRUE;
		if ( SDM_4_RUN_ACTION[ pointer % 100 ] == 2 ) return TRUE;
		break;
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CHECK( int pointer , int *dummode ) {
	switch( ( SDM_4_RUN_STATUS[ pointer % 100 ] % 10 ) ) {
	case 0 :
		break;
	case 1 : // first
		*dummode = 1;
		if ( ( SDM_4_RUN_ACTION[ pointer % 100 ] != 0 ) && ( SDM_4_RUN_ACTION[ pointer % 100 ] != 3 ) ) return TRUE;
		break;
	case 2 : // last
//	case 4 : // last // 2005.08.25
		*dummode = 2;
		if ( ( SDM_4_RUN_ACTION[ pointer % 100 ] != 0 ) && ( SDM_4_RUN_ACTION[ pointer % 100 ] != 3 ) ) return TRUE;
		break;
	case 3 : // cycle
		*dummode = 0;
		if ( ( SDM_4_RUN_ACTION[ pointer % 100 ] != 0 ) && ( SDM_4_RUN_ACTION[ pointer % 100 ] != 3 ) ) return TRUE;
		break;
	case 4 : // last/cycle
		*dummode = 99;
		if ( ( SDM_4_RUN_ACTION[ pointer % 100 ] != 0 ) && ( SDM_4_RUN_ACTION[ pointer % 100 ] != 3 ) ) return TRUE;
		break;
	}
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_CLEAR( int pointer ) {
	switch( ( SDM_4_RUN_STATUS[ pointer % 100 ] % 10 ) ) {
	case 0 :
		break;
	default :
		switch( SDM_4_RUN_ACTION[ pointer % 100 ] ) {
		case 0 :
			SDM_4_RUN_ACTION[ pointer % 100 ] = 0;
			break;
		case 1 : // s1,s2
			SDM_4_RUN_ACTION[ pointer % 100 ] = 3;
			break;
		case 2 : // s1
			SDM_4_RUN_ACTION[ pointer % 100 ] = 0;
			SDM_4_RUN_STATUS[ pointer % 100 ] = 0;
			break;
		case 3 : // s2
			SDM_4_RUN_ACTION[ pointer % 100 ] = 3;
			break;
		}
		break;
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_END_RESET( int ch ) { // 2005.08.26
	SDM_4_LOTEND_SIGNAL[ch] = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Change_SDM_4_Dummy_Side( int orgside , int side ) { // 2004.09.07
	int i;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;

	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( orgside == -1 ) {
			SDM_4_RUN_SIDE[i] = side;
		}
		else {
			if ( SDM_4_RUN_SIDE[i] == orgside )	SDM_4_RUN_SIDE[i] = side;
		}
	}
	return TRUE;
}
//
