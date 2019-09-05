//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================
#include "sch_A4_sub.h"
//================================================================================


void USER_REVISION_HISTORY_AL4_PART( int mode , char *data , int count ) {

	sprintf( data , "2015/02/05/1200(%s %s)" , __DATE__ , __TIME__ );	//

//	strcpy( data , "2015/02/05/1200" );	// AT:BM Pick Only Delay Bug Update
//	strcpy( data , "2014/01/15/1200" );	// AT:Diff Check Bug
//	strcpy( data , "2011/12/13/1600" );	// GE:PICK IC ARM A BUG Update
//	strcpy( data , "2011/03/23/1800" );	// AT:PRE VENT Bug Update
//	strcpy( data , "2011/01/28/1800" );	// AT:PRE VENT for return
//	strcpy( data , "2010/12/09/2300" );	// AT:PRE VENT for return
//	strcpy( data , "2010/11/29/1800" );	// AT:PRE VENT for return
//	strcpy( data , "2010/10/01/1800" );	// MH:Case Problem2
//	strcpy( data , "2010/09/03/1200" );	// MH:Case Problem
//	strcpy( data , "2009/09/18/1200" );	// AT:BM PMONEBODY PRE Process Problem
}








extern int  SDM_4_LOTEND_SIGNAL[ MAX_CHAMBER ];
extern int  SDM_4_LOTFIRST_SIGNAL[ MAX_CHAMBER ];
extern int  SDM_4_LOTFIRST_SIDE[ MAX_CHAMBER ];
//
extern int  SDM_4_LOTCYCLE_SIGNAL[ MAX_CHAMBER ];
extern int  SDM_4_LOTCYCLE_SIDE[ MAX_CHAMBER ];
//
extern int  SDM_4_RUN_STATUS[ MAX_CASSETTE_SLOT_SIZE ];
extern int  SDM_4_RUN_SIDE[ MAX_CASSETTE_SLOT_SIZE ];
extern int  SDM_4_RUN_ACTION[ MAX_CASSETTE_SLOT_SIZE ];

extern char SDM_4_PRE_RECIPE[ MAX_CASSETTE_SLOT_SIZE ][3][65];
extern char SDM_4_RUN_RECIPE[ MAX_CASSETTE_SLOT_SIZE ][3][65];
//
extern int sch4_RERUN_END_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_END_S1_ETC[ MAX_CHAMBER ]; // 2005.07.29

extern int sch4_RERUN_END_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_END_S2_ETC[ MAX_CHAMBER ]; // 2005.07.29

extern int sch4_RERUN_FST_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_FST_S1_ETC[ MAX_CHAMBER ]; // 2005.07.29

extern int sch4_RERUN_FST_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_FST_S2_ETC[ MAX_CHAMBER ]; // 2005.07.29

extern int sch4_RERUN_END_S3_TAG[ MAX_CHAMBER ]; // 2005.11.11

extern int  sch4_t3_logdata[MAX_BM_CHAMBER_DEPTH];
extern int	sch4_BM_PROCESS_MONITOR[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_CHECKING[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_MODE[ MAX_CHAMBER ];
//
//------------------------------------------------------------------------------------------
extern int				SDM_4_LOTSPECIAL_TAG[ MAX_CHAMBER ]; // 2005.07.20




void USER_SYSTEM_LOG_AL4( char *filename , int side ) {

//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
// Include Start
//###################################################################################################
	int i;
	FILE *fpt;

	if ( side != -1 ) return;

	fpt = fopen( filename , "a" );

	if ( fpt == NULL ) return;

	fprintf( fpt , "LOTEND_SIGNAL    " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTEND_SIGNAL[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "LOTFIRST_SIGNAL  " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTFIRST_SIGNAL[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "LOTFIRST_SIDE    " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTFIRST_SIDE[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "LOTCYCLE_SIGNAL  " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTCYCLE_SIGNAL[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "LOTCYCLE_SIDE    " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTCYCLE_SIDE[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "LOTSPECIAL_TAG   " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , SDM_4_LOTSPECIAL_TAG[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "RUN_STATUS       " );	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) fprintf( fpt , "[%02d]" , SDM_4_RUN_STATUS[i] ); fprintf( fpt , "\n" );
	fprintf( fpt , "RUN_SIDE         " );	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) fprintf( fpt , "[%02d]" , SDM_4_RUN_SIDE[i] ); fprintf( fpt , "\n" );
	fprintf( fpt , "RUN_ACTION       " );	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) fprintf( fpt , "[%02d]" , SDM_4_RUN_ACTION[i] ); fprintf( fpt , "\n" );

	fprintf( fpt , "\n" );

	fprintf( fpt , "RERUN_END_S1_TAG,RERUN_END_S1_ETC " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d,%02d]" , sch4_RERUN_END_S1_TAG[i] , sch4_RERUN_END_S1_ETC[i] );
	}
	fprintf( fpt , "\n" );
	
	fprintf( fpt , "RERUN_END_S2_TAG,RERUN_END_S2_ETC " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d,%02d]" , sch4_RERUN_END_S2_TAG[i] , sch4_RERUN_END_S2_ETC[i] );
	}
	fprintf( fpt , "\n" );
	
	fprintf( fpt , "RERUN_END_S3_TAG " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d]" , sch4_RERUN_END_S3_TAG[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "RERUN_FST_S1_TAG,RERUN_FST_S1_ETC " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d,%02d]" , sch4_RERUN_FST_S1_TAG[i] , sch4_RERUN_FST_S1_ETC[i] );
	}
	fprintf( fpt , "\n" );
	
	fprintf( fpt , "RERUN_FST_S2_TAG,RERUN_FST_S2_ETC " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%02d,%02d]" , sch4_RERUN_FST_S2_TAG[i] , sch4_RERUN_FST_S2_ETC[i] );
	}
	fprintf( fpt , "\n" );
	
	fprintf( fpt , "------------------------------------------------------------------\n" );
	fprintf( fpt , "sch4_BM_PROCESS_MONITOR[Monitor,Check,Mode] " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%d,%d,%d]" , sch4_BM_PROCESS_MONITOR[i] , sch4_BM_PROCESS_CHECKING[i] , sch4_BM_PROCESS_MODE[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "------------------------------------------------------------------\n" );
	fprintf( fpt , "sch4_t3_logdata " );
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		fprintf( fpt , "[%d]" , sch4_t3_logdata[i] );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "------------------------------------------------------------------\n" );
	fprintf( fpt , "ONEBODY %d,%d\n" , SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) , SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) );

	fclose( fpt );

//###################################################################################################
// Include End
//###################################################################################################
#endif
//###################################################################################################

}
