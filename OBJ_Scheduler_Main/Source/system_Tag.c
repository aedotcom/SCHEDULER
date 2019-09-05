#include "default.h"

#include "EQ_Enum.h"

#include "IO_Part_data.h"
#include "System_Tag.h"
#include "User_Parameter.h"
#include "sch_prm_Log.h"
#include "MultiJob.h"
//------------------------------------------------------------------------------------------
int  System_Run_Alg_Style = 0;
int  System_Gui_Alg_Style = 0; // 2008.01.09
int  System_Sub_Alg_Style = 0; // 2005.05.18
//
int  System_Run_Time = 0; // 2007.10.23
//
int  System_Use[MAX_CASSETTE_SIDE+1];
int  System_Use2[MAX_CASSETTE_SIDE+1]; // FEM Control
int  System_TM_Use[MAX_TM_CHAMBER_DEPTH][MAX_CASSETTE_SIDE+1];
int  System_Begin[MAX_CASSETTE_SIDE+1]; // 2007.05.04
int  System_RunTag_Set[MAX_CASSETTE_SIDE+1]; // 2011.04.22
int  System_Prep[MAX_CASSETTE_SIDE+1]; // 2008.04.23
//
int  System_Dummy[MAX_CASSETTE_SIDE];
int  System_Blank[MAX_CASSETTE_SIDE]; // 2011.04.20
int  System_Restart[MAX_CASSETTE_SIDE]; // 2011.09.23
int  System_DB_Update[MAX_CASSETTE_SIDE]; // 2011.04.20
int  System_Finish[MAX_CASSETTE_SIDE]; // 2011.04.27
int  System_FA[MAX_CASSETTE_SIDE];
int  System_CPJOB[MAX_CASSETTE_SIDE];
int  System_CPJOB_ID[MAX_CASSETTE_SIDE];
int  System_Mode[MAX_CASSETTE_SIDE];
int  System_Mode_Buffer[MAX_CASSETTE_SIDE];
int  System_Mode_First[MAX_CASSETTE_SIDE];
int  System_Mode_First_Buffer[MAX_CASSETTE_SIDE];
int  System_RID[MAX_CASSETTE_SIDE];
int  System_Lasting[MAX_CASSETTE_SIDE];
int	 System_Order[MAX_CASSETTE_SIDE];
int  System_Cancel[MAX_CASSETTE_SIDE];
int  System_EndCheck[MAX_CASSETTE_SIDE];
int  System_ThdCheck[MAX_CASSETTE_SIDE]; // 2003.05.23
int  System_PMMODE[MAX_CASSETTE_SIDE]; // 2005.07.06
int  System_MOVEMODE[MAX_CASSETTE_SIDE]; // 2013.09.03
int  System_IN_Module[MAX_CASSETTE_SIDE]; // 2010.01.19
int  System_OUT_Module[MAX_CASSETTE_SIDE]; // 2010.01.19
int  System_RunCheck;
int  System_RunCheck_FM; // 2008.01.11
int  System_ESOURCEID[MAX_CASSETTE_SIDE]; // 2019.02.26

BOOL System_SIDE_CLOSING[MAX_CASSETTE_SIDE]; // 2013.10.02

int  System_APPEND_END[MAX_CASSETTE_SIDE]; // 2010.09.13

int  System_EQUIP_RUNNING_TAG[MAX_CHAMBER]; // 2012.10.31

DWORD System_Set_Timer[MAX_CASSETTE_SIDE]; // 2010.12.02
time_t System_Ref_Timer[MAX_CASSETTE_SIDE]; // 2014.02.03

int System_OLD_SIDE_FOR_MTLOUT[MAX_CASSETTE_SIDE]; // 2014.06.26
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SIGNAL_ABORT[ MAX_CASSETTE_SIDE	+ 1 ];
int  SIGNAL_PAUSE[ MAX_CASSETTE_SIDE	+ 1 ];
BOOL SIGNAL_RESUME[ MAX_CASSETTE_SIDE + 1 ];
BOOL SIGNAL_WAITR[ MAX_CASSETTE_SIDE + 1 ];
int  SIGNAL_LOOP[ MAX_CASSETTE_SIDE + 1 ];
int  SIGNAL_END[ MAX_CASSETTE_SIDE + 1 ];
//
int  SIGNAL_END_BUF1[ MAX_CASSETTE_SIDE + 1 ]; // 2016.08.12
int  SIGNAL_END_BUF2[ MAX_CASSETTE_SIDE + 1 ]; // 2016.08.12

CRITICAL_SECTION SIGNAL_DISAPPEAR_CR;
BOOL SIGNAL_DISAPPEAR[128];
//
int  SIGNAL_PAUSE_SW;
//
int  SIGNAL_FMTMSIDE[MAX_CHAMBER];
int  SIGNAL_FMTMSIDE_in[MAX_CHAMBER];
char *SIGNAL_FMTMRECIPE[MAX_CHAMBER];
int  SIGNAL_FMTMSIDE_pmskippreside[MAX_CHAMBER]; // 2014.12.18
int  SIGNAL_FMTMSIDE_extmode[MAX_CHAMBER]; // 2018.06.22
//
int  SIGNAL_NO_PICK_FROM_CM[ MAX_CASSETTE_SIDE ]; // 2016.12.05
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SYSTEM_RUN_TIME_GET() { // 2007.10.23
	System_Run_Time++;
	return System_Run_Time;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SYSTEM_RUN_ALG_STYLE_SET( int id , int id2 ) {
	System_Run_Alg_Style = id;
	if ( id2 >= 0 ) {
		System_Gui_Alg_Style = id2;
	}
	else {
		System_Gui_Alg_Style = id;
	}
}
int  SYSTEM_RUN_ALG_STYLE_GET() { return System_Run_Alg_Style; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SYSTEM_GUI_ALG_STYLE_SET( int id ) { System_Gui_Alg_Style = id; }
int  SYSTEM_GUI_ALG_STYLE_GET() { return System_Gui_Alg_Style; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SYSTEM_SUB_ALG_STYLE_SET( int id ) { System_Sub_Alg_Style = id; } // 2005.05.18
int  SYSTEM_SUB_ALG_STYLE_GET() { return System_Sub_Alg_Style; } // 2005.05.18
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
char System_LogDirectory[MAX_CASSETTE_SIDE][256];
void SYSTEM_LOTLOGDIRECTORY_SET( int Side , char *data ) {	strncpy( System_LogDirectory[Side] , data , 255 ); }
char *SYSTEM_LOTLOGDIRECTORY_GET( int Side ) {	return System_LogDirectory[Side]; }
*/
MAKE_SYSTEM_CHAR_PARAMETER2( LOTLOGDIRECTORY , MAX_CASSETTE_SIDE )

MAKE_SYSTEM_CHAR_PARAMETER2( LOTLOGDIRECTORY_LOOP , MAX_CASSETTE_SIDE )

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  _i_SCH_SYSTEM_BLANK_GET( int Side ) { return System_Blank[Side]; }
void _i_SCH_SYSTEM_BLANK_SET( int Side , int data ) { System_Blank[Side] = data; }

int  _i_SCH_SYSTEM_RESTART_GET( int Side ) { return System_Restart[Side]; }
void _i_SCH_SYSTEM_RESTART_SET( int Side , int data ) { System_Restart[Side] = data; }

int  _i_SCH_SYSTEM_DBUPDATE_GET( int Side ) { return System_DB_Update[Side]; }
void _i_SCH_SYSTEM_DBUPDATE_SET( int Side , int data ) { System_DB_Update[Side] = data; }

int  _i_SCH_SYSTEM_FINISH_GET( int Side ) { return System_Finish[Side]; }
void _i_SCH_SYSTEM_FINISH_SET( int Side , int data ) { System_Finish[Side] = data; }

int  _i_SCH_SYSTEM_FA_GET( int Side ) { return System_FA[Side]; }
void _i_SCH_SYSTEM_FA_SET( int Side , int data ) { System_FA[Side] = data; }

int  _i_SCH_SYSTEM_CPJOB_GET( int Side ) { return System_CPJOB[Side]; }
void _i_SCH_SYSTEM_CPJOB_SET( int Side , int data ) { System_CPJOB[Side] = data; }

int  _i_SCH_SYSTEM_CPJOB_ID_GET( int Side ) { return System_CPJOB_ID[Side]; }
void _i_SCH_SYSTEM_CPJOB_ID_SET( int Side , int data ) { System_CPJOB_ID[Side] = data; }

int  _i_SCH_SYSTEM_USING_GET( int Side ) { return System_Use[Side]; }
void _i_SCH_SYSTEM_USING_SET( int Side , int data ) {
	//
	System_Use[Side] = data;
	//-------------------------------------
	// 2011.04.22
	//
	if ( data >= 100 ) { // 2012.01.29
	}
//	if ( data >= 10 ) { // 2012.01.29
	else if ( data >= 10 ) { // 2012.01.29
		if ( System_RunTag_Set[Side] <= 0 ) System_RunTag_Set[Side] = 1;
	}
	else { 
		System_RunTag_Set[Side] = 0;
	}
	//-------------------------------------
}

void _i_SCH_SYSTEM_RUN_TAG_SET( int Side , int data ) { // 2011.04.22
	System_RunTag_Set[Side] = data;
}
int  _i_SCH_SYSTEM_RUN_TAG_GET( int Side ) { // 2011.04.22
	return System_RunTag_Set[Side];
}

int  SYSTEM_BEGIN_GET( int Side ) { return System_Begin[Side]; } // 2007.05.04
void SYSTEM_BEGIN_SET( int Side , int data ) { System_Begin[Side] = data; } // 2007.05.04

int  SYSTEM_PREPARE_GET( int Side ) { return System_Prep[Side]; } // 2008.04.23
void SYSTEM_PREPARE_SET( int Side , int data ) { System_Prep[Side] = data; } // 2008.04.23

int  _i_SCH_SYSTEM_TM_GET( int TMATM , int Side ) { return System_TM_Use[TMATM][Side]; }
void _i_SCH_SYSTEM_TM_SET( int TMATM , int Side , int data ) { System_TM_Use[TMATM][Side] = data; }

int  SYSTEM_DUMMY_GET( int Side ) { return System_Dummy[Side]; }
void SYSTEM_DUMMY_SET( int Side , int data ) { System_Dummy[Side] = data; }

int  _i_SCH_SYSTEM_LASTING_GET( int Side ) { return System_Lasting[Side]; }
void _i_SCH_SYSTEM_LASTING_SET( int Side , int data ) { System_Lasting[Side] = data; }

int  _i_SCH_SYSTEM_USING2_GET( int Side ) { return System_Use2[Side]; }
void _i_SCH_SYSTEM_USING2_SET( int Side , int data ) { System_Use2[Side] = data; }

int  _i_SCH_SYSTEM_MODE_GET( int Side ) { return System_Mode[Side]; }
void _i_SCH_SYSTEM_MODE_SET( int Side , int data ) { System_Mode[Side] = data; }

int  SYSTEM_MODE_BUFFER_GET( int Side ) { return System_Mode_Buffer[Side]; }
void SYSTEM_MODE_BUFFER_SET( int Side , int data ) { System_Mode_Buffer[Side] = data; }

int  SYSTEM_MODE_FIRST_GET( int Side ) { return System_Mode_First[Side]; } // 2005.03.10
void SYSTEM_MODE_FIRST_SET( int Side , int data ) { System_Mode_First[Side] = data; } // 2005.03.10

void SYSTEM_MODE_FIRST_SET_BUFFER( int Side , int data ) { System_Mode_First_Buffer[Side] = data; } // 2005.06.15
int  SYSTEM_MODE_FIRST_GET_BUFFER( int Side ) { return System_Mode_First_Buffer[Side]; } // 2005.06.15

int  SYSTEM_RID_GET( int Side ) { return System_RID[Side]; }
void SYSTEM_RID_SET( int Side , int data ) { System_RID[Side] = data; }

int  _i_SCH_SYSTEM_RUNORDER_GET( int Side ) { return System_Order[Side]; }
void _i_SCH_SYSTEM_RUNORDER_SET( int Side , int data ) { System_Order[Side] = data; }

int  SYSTEM_CANCEL_DISABLE_GET( int Side ) { return System_Cancel[Side]; }
void SYSTEM_CANCEL_DISABLE_SET( int Side , int data ) { System_Cancel[Side] = data; }

int  SYSTEM_ENDCHECK_GET( int Side ) { return System_EndCheck[Side]; }
void SYSTEM_ENDCHECK_SET( int Side , int data ) { System_EndCheck[Side] = data; }

int  SYSTEM_THDCHECK_GET( int Side ) { return System_ThdCheck[Side]; }
void SYSTEM_THDCHECK_SET( int Side , int data ) { System_ThdCheck[Side] = data; }

int  _i_SCH_SYSTEM_RUNCHECK_GET() { return System_RunCheck; }
void _i_SCH_SYSTEM_RUNCHECK_SET( int data ) { System_RunCheck = data; }

int  _i_SCH_SYSTEM_RUNCHECK_FM_GET() { return System_RunCheck_FM; } // 2008.01.11
void _i_SCH_SYSTEM_RUNCHECK_FM_SET( int data ) { System_RunCheck_FM = data; } // 2008.01.11

int  _i_SCH_SYSTEM_PMMODE_GET( int Side ) { return System_PMMODE[Side]; }
void _i_SCH_SYSTEM_PMMODE_SET( int Side , int data ) { System_PMMODE[Side] = data; }

int  _i_SCH_SYSTEM_MOVEMODE_GET( int Side ) { return System_MOVEMODE[Side]; } // 2013.09.03
void _i_SCH_SYSTEM_MOVEMODE_SET( int Side , int data ) { System_MOVEMODE[Side] = data; } // 2013.09.03

int  SYSTEM_IN_MODULE_GET( int Side ) { return System_IN_Module[Side]; } // 2010.01.19
void SYSTEM_IN_MODULE_SET( int Side , int data ) { System_IN_Module[Side] = data; } // 2010.01.19

int  SYSTEM_OUT_MODULE_GET( int Side ) { return System_OUT_Module[Side]; } // 2010.01.19
void SYSTEM_OUT_MODULE_SET( int Side , int data ) { System_OUT_Module[Side] = data; } // 2010.01.19

int  _i_SCH_SYSTEM_APPEND_END_GET( int CHECKING_SIDE ) {	return System_APPEND_END[CHECKING_SIDE];	} // 2010.09.13
void _i_SCH_SYSTEM_APPEND_END_SET( int CHECKING_SIDE , int data ) {	System_APPEND_END[CHECKING_SIDE] = data;	} // 2010.09.13

DWORD SYSTEM_RUN_TIMER_GET( int Side ) { return ( GetTickCount() - System_Set_Timer[Side] ); } // 2010.12.02

void  SYSTEM_RUN_TIMER_SET( int Side ) { // 2010.12.02
	System_Set_Timer[Side] = GetTickCount();
	time( &(System_Ref_Timer[Side]) ); // 2014.02.04
}

BOOL SYSTEM_PRGS_TIME_GET( int Side , long timeclock ,char *data ) { // 2014.02.04
	struct tm *Pres_Time;
	time_t newtime;
	//
	strcpy( data , "" );
	//
	if ( timeclock != 0 ) {
		//
		newtime = System_Ref_Timer[Side] + ( ( timeclock - System_Set_Timer[Side] ) / 1000 );
		Pres_Time = localtime( &newtime );
		//
		sprintf( data , "%04d/%02d/%02d %02d:%02d:%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon+1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
		//
		return TRUE;
	}
	return FALSE;
}

int  _i_SCH_SYSTEM_EQUIP_RUNNING_GET( int ch ) {	return System_EQUIP_RUNNING_TAG[ch];	} // 2012.10.31
void _i_SCH_SYSTEM_EQUIP_RUNNING_SET( int ch , int data ) {	System_EQUIP_RUNNING_TAG[ch] = data;	} // 2012.10.31


BOOL _SCH_SYSTEM_SIDE_CLOSING_GET( int Side ) { // 2013.10.02
	return System_SIDE_CLOSING[Side];
}

void _SCH_SYSTEM_SIDE_CLOSING_SET( int Side , BOOL data ) { // 2013.10.02
	System_SIDE_CLOSING[Side] = data;
}


int  _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_GET( int Side ) { // 2014.06.26
	return System_OLD_SIDE_FOR_MTLOUT[Side];
}
void _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_SET( int Side , int data ) { // 2014.06.26
	System_OLD_SIDE_FOR_MTLOUT[Side] = data;
}


int  _i_SCH_SYSTEM_ESOURCE_ID_GET( int Side ) { return System_ESOURCEID[Side]; } // 2019.02.26
void _i_SCH_SYSTEM_ESOURCE_ID_SET( int Side , int data ) { System_ESOURCEID[Side] = data; } // 2019.02.26

//===================================================================================================
// 2005.07.29
//===================================================================================================
BOOL _i_SCH_SYSTEM_USING_RUNNING( int Side ) {
	if ( ( _i_SCH_SYSTEM_USING_GET( Side ) <= 0 ) || ( _i_SCH_SYSTEM_USING_GET( Side ) >= 100 ) ) return FALSE;
	//
//	if ( _i_SCH_SYSTEM_USING_GET( Side ) >= 9  ) return TRUE; // 2005.08.03
	if ( _i_SCH_SYSTEM_USING_GET( Side ) >= 10 ) return TRUE; // 2005.08.03
	//
//	if ( _i_SCH_SYSTEM_MODE_GET( Side ) == 0 ) return TRUE; // 2005.08.17
	//
	return FALSE;
}
//===================================================================================================
//===================================================================================================
BOOL _i_SCH_SYSTEM_USING_STARTING( int Side ) {
//	if ( _i_SCH_SYSTEM_USING_GET( Side ) > 0 ) return TRUE; // 2010.02.09
	if ( ( _i_SCH_SYSTEM_USING_GET( Side ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( Side ) < 100 ) ) return TRUE; // 2010.02.09
	return FALSE;
}
//=======================================================================================
BOOL _i_SCH_SYSTEM_USING_STARTING_ONLY( int Side ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != Side ) {
//			if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) return FALSE; // 2010.02.09
			if ( ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( i ) < 100 ) ) return FALSE; // 2010.02.09
		}
	}
	return TRUE;
}
//=======================================================================================
BOOL _i_SCH_SYSTEM_USING_ANOTHER_RUNNING( int Side ) { // 2003.11.10
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != Side ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) return TRUE;
		}
	}
	return FALSE;
}
//=======================================================================================
BOOL _SCH_SYSTEM_NO_PICK_FROM_CM_GET( int Side ) { // 2016.12.05
	return SIGNAL_NO_PICK_FROM_CM[Side];
}

void _SCH_SYSTEM_NO_PICK_FROM_CM_SET( int Side , BOOL data ) { // 2016.12.05
	SIGNAL_NO_PICK_FROM_CM[Side] = data;
}
//===================================================================================================
//===================================================================================================
void SIGNAL_MODE_RESET( int CHECKING_SIDE ) {
	SIGNAL_ABORT[CHECKING_SIDE] = FALSE;
	SIGNAL_PAUSE[CHECKING_SIDE] = 0;
	SIGNAL_RESUME[CHECKING_SIDE] = FALSE;
	SIGNAL_WAITR[CHECKING_SIDE] = FALSE;
	SIGNAL_END[CHECKING_SIDE]   = 0;
	//
	SIGNAL_END_BUF1[CHECKING_SIDE]   = 0; // 2016.08.12
	SIGNAL_END_BUF2[CHECKING_SIDE]   = 0; // 2016.08.12
	//
}
//
void _i_SCH_SYSTEM_FLAG_RESET( int CHECKING_SIDE ) { // 2012.08.29
	int i;
	if ( CHECKING_SIDE == -1 ) {
		SIGNAL_MODE_RESET( TR_CHECKING_SIDE );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_GET(i) <= 0 ) SIGNAL_MODE_RESET( i );
		}
	}
	else {
		SIGNAL_MODE_RESET( CHECKING_SIDE );
	}
}

//===================================================================================================
//===================================================================================================

// 2009.04.23
int  SIGNAL_MODE_RECIPE_READ[ MAX_CASSETTE_SIDE ];
char SIGNAL_MODE_RECIPE_READ_FILE[ MAX_CASSETTE_SIDE ][64];

BOOL SIGNAL_MODE_RECIPE_READ_GET( int side , int *filetype , int *callstyle , char *filename ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( SIGNAL_MODE_RECIPE_READ[ side ] == -1 ) return FALSE;
	*filetype  = SIGNAL_MODE_RECIPE_READ[ side ] / 100;
	*callstyle = SIGNAL_MODE_RECIPE_READ[ side ] % 100;
	strcpy( filename , SIGNAL_MODE_RECIPE_READ_FILE[ side ] );
	return TRUE;
}

BOOL SIGNAL_MODE_RECIPE_READ_CLEAR( int side ) {
	int i;
	if ( side == -1 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) SIGNAL_MODE_RECIPE_READ[ i ] = -1;
	}
	else {
		if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
		SIGNAL_MODE_RECIPE_READ[ side ] = -1;
	}
	return TRUE;
}

BOOL SIGNAL_MODE_RECIPE_READ_SET( int side , int filetype , int callstyle , char *filename ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( filename[0] == 0 ) return FALSE;
	if ( ( filetype < 0 ) || ( filetype > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	if ( ( callstyle < 0 ) || ( callstyle > 99 ) ) return FALSE;
	strncpy( SIGNAL_MODE_RECIPE_READ_FILE[ side ] , filename , 63 );
	SIGNAL_MODE_RECIPE_READ[ side ] = ( filetype * 100 ) + callstyle;
	return TRUE;
}

//===================================================================================================
//===================================================================================================
int  SIGNAL_MODE_FMTMSIDE_GET( BOOL ins , int ch ) {
	if ( ( ch < BM1 ) || ( ch >= BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		if ( ( ch < PM1 ) || ( ch >= PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			return -1;
		}
	}
	//
	if ( ins ) {
		return SIGNAL_FMTMSIDE_in[ch];
	}
	else {
		return SIGNAL_FMTMSIDE[ch];
	}
}

char *SIGNAL_MODE_FMTMRECIPE_GET( int ch ) {
	if ( ( ch < BM1 ) || ( ch >= BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		if ( ( ch < PM1 ) || ( ch >= PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			return NULL;
		}
	}
	return SIGNAL_FMTMRECIPE[ch];
}

int  SIGNAL_MODE_FMTMPRESKIPSIDE_GET( int ch ) { // 2014.12.18
	if ( ( ch < BM1 ) || ( ch >= BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		if ( ( ch < PM1 ) || ( ch >= PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			return -1;
		}
	}
	//
	return SIGNAL_FMTMSIDE_pmskippreside[ch];
}

int  SIGNAL_MODE_FMTMEXTMODE_GET( int ch ) { // 2018.06.22
	if ( ( ch < BM1 ) || ( ch >= BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		if ( ( ch < PM1 ) || ( ch >= PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			return 0;
		}
	}
	//
	return SIGNAL_FMTMSIDE_extmode[ch];
}


void SIGNAL_MODE_FMTMSIDE_INIT( BOOL init ) {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		SIGNAL_FMTMSIDE[i] = -1;
		SIGNAL_FMTMSIDE_in[i] = 0;
		SIGNAL_FMTMSIDE_pmskippreside[i] = -1; // 2014.12.18
		SIGNAL_FMTMSIDE_extmode[i] = 0; // 2018.06.22
		//
		if ( init ) SIGNAL_FMTMRECIPE[i] = NULL;
	}
}

//void SIGNAL_MODE_FMTMSIDE_SET( int ch , int mode , char *rcp , int PMSKIPPRESIDE ) { // 2018.06.22
void SIGNAL_MODE_FMTMSIDE_SET( int ch , int mode , char *rcp , int PMSKIPPRESIDE , int extmode ) { // 2018.06.22
	if ( ( ch < BM1 ) || ( ch >= BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		if ( ( ch < PM1 ) || ( ch >= PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			return;
		}
	}
	//
	SIGNAL_FMTMSIDE_pmskippreside[ch] = PMSKIPPRESIDE; // 2014.12.18
	SIGNAL_FMTMSIDE_extmode[ch] = extmode; // 2018.06.22
	//
	if ( mode >= 1000 ) {
		SIGNAL_FMTMSIDE_in[ch] = mode % 1000;
	}
	else {
		SIGNAL_FMTMSIDE[ch] = mode;
		if ( mode != -1 ) {
			if ( !STR_MEM_MAKE_COPY2( &(SIGNAL_FMTMRECIPE[ ch ]) , rcp ) ) {
				_IO_CIM_PRINTF( "SIGNAL_MODE_FMTMSIDE_SET Memory Allocate Error[%d,%d]\n" , ch , mode );
			}
		}
		else {
			SIGNAL_FMTMSIDE_in[ch] = 0;
		}
	}
}


int  _i_SCH_SYSTEM_HAS_USER_PROCESS_GET( int ch ) { // 2017.07.17
	return SIGNAL_MODE_FMTMSIDE_GET( FALSE , ch ); // 2017.07.17
}

//===================================================================================================
//===================================================================================================
int  SIGNAL_MODE_DISAPPEAR_GET( int index ) {
	if ( ( index < 0 ) || ( index >= 128 ) ) {
		if ( SIGNAL_DISAPPEAR[127] == 0 ) return FALSE;
		return TRUE;
	}
	if ( SIGNAL_DISAPPEAR[index] == 0 ) return FALSE;
	return TRUE;
}
//
void SIGNAL_MODE_DISAPPEAR_SET() {
	int i;
	for ( i = 0 ; i < 128 ; i++ ) SIGNAL_DISAPPEAR[i] = 1;
}
//
void SIGNAL_MODE_DISAPPEAR_ENTER() {
	EnterCriticalSection( &SIGNAL_DISAPPEAR_CR );
}
//
void SIGNAL_MODE_DISAPPEAR_INIT( int ap ) {
	int i;
	if ( ap == 0 ) InitializeCriticalSection( &SIGNAL_DISAPPEAR_CR );
	for ( i = 0 ; i < 128 ; i++ ) SIGNAL_DISAPPEAR[i] = 0;
}
//
void SIGNAL_MODE_DISAPPEAR_CLEAR( int index ) {
	if ( ( index < 0 ) || ( index >= 128 ) ) SIGNAL_DISAPPEAR[127] = 0;
	else                                     SIGNAL_DISAPPEAR[index] = 0;
	LeaveCriticalSection( &SIGNAL_DISAPPEAR_CR );
}
//
int  _i_SCH_SYSTEM_MODE_END_GET( int CHECKING_SIDE ) {	return SIGNAL_END[CHECKING_SIDE];	}
//void _i_SCH_SYSTEM_MODE_END_SET( int CHECKING_SIDE , int data ) {	SIGNAL_END[CHECKING_SIDE] = data;	} // 2016.08.12
void _i_SCH_SYSTEM_MODE_END_SET( int CHECKING_SIDE , int data ) {
	//
	SIGNAL_END[CHECKING_SIDE] = data;
	//
	if ( ( data > 0 ) && ( data <= 9 ) ) { // 2016.08.12
		if ( SIGNAL_END_BUF1[CHECKING_SIDE] < 10000000 ) {
			SIGNAL_END_BUF1[CHECKING_SIDE] = ( SIGNAL_END_BUF1[CHECKING_SIDE] * 10 ) + data;
		}
		else {
			if ( SIGNAL_END_BUF2[CHECKING_SIDE] < 10000000 ) {
				SIGNAL_END_BUF2[CHECKING_SIDE] = ( SIGNAL_END_BUF2[CHECKING_SIDE] * 10 ) + data;
			}
		}
	}
	//
}

int  _i_SCH_SYSTEM_MODE_END_BUF1_GET( int CHECKING_SIDE ) {	return SIGNAL_END_BUF1[CHECKING_SIDE];	} // 2016.08.12
int  _i_SCH_SYSTEM_MODE_END_BUF2_GET( int CHECKING_SIDE ) {	return SIGNAL_END_BUF2[CHECKING_SIDE];	} // 2016.08.12

BOOL _i_SCH_SYSTEM_MODE_RESUME_GET( int CHECKING_SIDE ) {	return SIGNAL_RESUME[CHECKING_SIDE];	}
void _i_SCH_SYSTEM_MODE_RESUME_SET( int CHECKING_SIDE , BOOL data ) {	SIGNAL_RESUME[CHECKING_SIDE] = data;	}

int  _i_SCH_SYSTEM_PAUSE_GET( int CHECKING_SIDE ) {	return SIGNAL_PAUSE[CHECKING_SIDE];	}
void _i_SCH_SYSTEM_PAUSE_SET( int CHECKING_SIDE , int data ) {	SIGNAL_PAUSE[CHECKING_SIDE] = data;	}

int _i_SCH_SYSTEM_PAUSE_SW_GET() {	return SIGNAL_PAUSE_SW;	}
void _i_SCH_SYSTEM_PAUSE_SW_SET( int data ) {	SIGNAL_PAUSE_SW = data;	}

//

BOOL _i_SCH_SYSTEM_MODE_WAITR_GET( int CHECKING_SIDE ) {	return SIGNAL_WAITR[CHECKING_SIDE];	}
void _i_SCH_SYSTEM_MODE_WAITR_SET( int CHECKING_SIDE , BOOL data ) {	SIGNAL_WAITR[CHECKING_SIDE] = data;	}
int  _i_SCH_SYSTEM_MODE_LOOP_GET( int CHECKING_SIDE ) {	return SIGNAL_LOOP[CHECKING_SIDE];	}
void _i_SCH_SYSTEM_MODE_LOOP_SET( int CHECKING_SIDE , int data ) {	SIGNAL_LOOP[CHECKING_SIDE] = data;	}
void _i_SCH_SYSTEM_MODE_ABORT_SET( int CHECKING_SIDE ) {	SIGNAL_ABORT[CHECKING_SIDE] = TRUE;	}


void SIGNAL_MODE_ABORT_RESET( int CHECKING_SIDE ) {	SIGNAL_ABORT[CHECKING_SIDE] = FALSE;	}
//BOOL SIGNAL_MODE_ABORT_GET( int CHECKING_SIDE ) { // 2016.01.12
//	if ( MANAGER_ABORT() ) return TRUE; // 2007.07.23
//	return SIGNAL_ABORT[CHECKING_SIDE];
//}

int SIGNAL_MODE_ABORT_GET( int CHECKING_SIDE ) { // 2016.01.12
	if ( MANAGER_ABORT() ) {
		if ( SIGNAL_ABORT[CHECKING_SIDE] ) return 3;
		return 2;
	}
	//
	if ( SIGNAL_ABORT[CHECKING_SIDE] ) return 1;
	return 0;
}

/*
//  2012.01.08
int _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( int CHECKING_SIDE ) { // 2007.08.09
	int i;
	//
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return -1; 
	//
	if ( SIGNAL_PAUSE[CHECKING_SIDE] ) {
		if ( CHECKING_SIDE < MAX_CASSETTE_SIDE )
			_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSED );
		else
			BUTTON_SET_TR_CONTROL( CTC_PAUSED );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != CHECKING_SIDE ) {
				//
				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && SIGNAL_PAUSE[i] ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED );
				//
			}
		}
		return 1;
	}
	if ( SIGNAL_PAUSE_SW != 0 ) return 1;
	return 0;
}
*/

// 2012.01.08
int _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( int CHECKING_SIDE ) {
	int i;
	//
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return -1;  // 2016.01.12
	if ( ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return -1;  // 2016.01.12
	//
	if ( SIGNAL_PAUSE[CHECKING_SIDE] != 0 ) {
		//
		if ( CHECKING_SIDE < MAX_CASSETTE_SIDE )
			_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSED );
		else
			BUTTON_SET_TR_CONTROL( CTC_PAUSED );
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != CHECKING_SIDE ) {
				//
//				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] != 0 ) ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2012.07.12
				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 1 ) ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2012.07.12
				//
			}
		}
		return 1;
	}
	if ( SIGNAL_PAUSE_SW != 0 ) return 1;
	return 0;
}

BOOL SIGNAL_PAUSE_ABORT_SUB_CHECK( int CHECKING_SIDE , BOOL Using_Check ) {
	BOOL i , Run = FALSE;

//	while( SIGNAL_PAUSE[CHECKING_SIDE] || ( SIGNAL_PAUSE_SW != 0 ) ) { // 2011.12.29
	while( ( SIGNAL_PAUSE[CHECKING_SIDE] == 1 ) || ( SIGNAL_PAUSE_SW != 0 ) ) { // 2011.12.29
//		if ( SIGNAL_PAUSE[CHECKING_SIDE] ) { // 2004.06.29 // 2011.12.29
		if ( SIGNAL_PAUSE[CHECKING_SIDE] == 1 ) { // 2004.06.29 // 2011.12.29
			if ( !Run ) {
				if ( CHECKING_SIDE < MAX_CASSETTE_SIDE )
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSED );
				else
					BUTTON_SET_TR_CONTROL( CTC_PAUSED );
				Run = TRUE;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != CHECKING_SIDE ) {
					//
//					if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && SIGNAL_PAUSE[i] ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2011.12.29
					if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 1 ) ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2011.12.29
					//
				}
			}
			if ( MANAGER_ABORT() ) return FALSE;
			if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
			if ( Using_Check ) {
				if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return TRUE;
			}
		}
		Sleep(1);
	}
	if ( MANAGER_ABORT() ) return FALSE;
	if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
	if ( Run ) {
//		if ( CHECKING_SIDE < 4 ) { // 2012.08.29
		if ( CHECKING_SIDE < MAX_CASSETTE_SIDE ) { // 2012.08.29
			if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 10 ) {
				_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
			}
			else {
				//-----------------------------
				// 2011.04.22
				//-----------------------------
				//_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING );
				//
				if ( _i_SCH_SYSTEM_RUN_TAG_GET( CHECKING_SIDE ) > 0 ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
				}
				else {
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING );
				}
				//-----------------------------
			}
		}
		else {
			BUTTON_SET_TR_CONTROL( CTC_RUNNING );
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != CHECKING_SIDE ) {
//				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && !SIGNAL_PAUSE[i] ) { // 2011.12.29
				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 0 ) ) { // 2011.12.29
					if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) {
						_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
					}
					else {
						//-----------------------------
						// 2011.04.22
						//-----------------------------
						//_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_WAITING );
						//
						if ( _i_SCH_SYSTEM_RUN_TAG_GET( i ) > 0 ) {
							_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
						}
						else {
							_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_WAITING );
						}
						//-----------------------------
					}
				}
			}
		}
	}
	if ( Using_Check ) {
		if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return TRUE;
	}
	return TRUE;
}
//
BOOL _i_SCH_SYSTEM_PAUSE_ABORT_CHECK( int CHECKING_SIDE ) {	return SIGNAL_PAUSE_ABORT_SUB_CHECK( CHECKING_SIDE , FALSE );	}
BOOL _i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( int CHECKING_SIDE ) {	return SIGNAL_PAUSE_ABORT_SUB_CHECK( CHECKING_SIDE , TRUE );	}

/*
//
//
// 2016.11.04
//
BOOL _i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( int CHECKING_SIDE ) { // 2012.07.12
	BOOL i , Run = FALSE;
	//
	if ( SIGNAL_PAUSE[CHECKING_SIDE] != 2 ) return TRUE;
	//
	if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 0 ) return TRUE;
	//
	while( ( SIGNAL_PAUSE[CHECKING_SIDE] != 0 ) || ( SIGNAL_PAUSE_SW != 0 ) ) {
		if ( SIGNAL_PAUSE[CHECKING_SIDE] != 0 ) {
			if ( !Run ) {
				if ( CHECKING_SIDE < MAX_CASSETTE_SIDE )
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSED );
				else
					BUTTON_SET_TR_CONTROL( CTC_PAUSED );
				Run = TRUE;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != CHECKING_SIDE ) {
					//
					if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 1 ) ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2011.12.29
					//
				}
			}
			if ( MANAGER_ABORT() ) return FALSE;
			if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
		}
		Sleep(1);
	}
	if ( MANAGER_ABORT() ) return FALSE;
	if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
	if ( Run ) {
		if ( CHECKING_SIDE < 4 ) {
			if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 10 ) {
				_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
			}
			else {
				if ( _i_SCH_SYSTEM_RUN_TAG_GET( CHECKING_SIDE ) > 0 ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
				}
				else {
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING );
				}
				//-----------------------------
			}
		}
		else {
			BUTTON_SET_TR_CONTROL( CTC_RUNNING );
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != CHECKING_SIDE ) {
				if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 0 ) ) {
					if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) {
						_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
					}
					else {
						if ( _i_SCH_SYSTEM_RUN_TAG_GET( i ) > 0 ) {
							_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
						}
						else {
							_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_WAITING );
						}
						//-----------------------------
					}
				}
			}
		}
	}
	return TRUE;
}
//
*/
//
// 2016.11.04
//
BOOL _i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( int CHECKING_SIDE , int pt ) {
	BOOL i , Run;
	//
	while ( TRUE ) {
		//
		//-------------------------------------------------------------------------------------------------
		//
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return FALSE;
		//
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , pt , 0 ) == 0 ) {
				Sleep(1);
				continue;
			}
		}
		//
		//-------------------------------------------------------------------------------------------------
		//
		if ( SIGNAL_PAUSE[CHECKING_SIDE] != 2 ) return TRUE;
		//
		if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 0 ) return TRUE;
		//
		//-------------------------------------------------------------------------------------------------
		//
		Run = FALSE;
		//
		while( ( SIGNAL_PAUSE[CHECKING_SIDE] != 0 ) || ( SIGNAL_PAUSE_SW != 0 ) ) {
			if ( SIGNAL_PAUSE[CHECKING_SIDE] != 0 ) {
				if ( !Run ) {
					if ( CHECKING_SIDE < MAX_CASSETTE_SIDE )
						_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSED );
					else
						BUTTON_SET_TR_CONTROL( CTC_PAUSED );
					Run = TRUE;
				}
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( i != CHECKING_SIDE ) {
						//
						if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 1 ) ) _SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_PAUSED ); // 2011.12.29
						//
					}
				}
				if ( MANAGER_ABORT() ) return FALSE;
				if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
			}
			Sleep(1);
		}
		//
		if ( MANAGER_ABORT() ) return FALSE;
		if ( SIGNAL_ABORT[CHECKING_SIDE] ) return FALSE;
		//
		if ( Run ) {
			if ( CHECKING_SIDE < 4 ) {
				if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 10 ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
				}
				else {
					if ( _i_SCH_SYSTEM_RUN_TAG_GET( CHECKING_SIDE ) > 0 ) {
						_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
					}
					else {
						_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING );
					}
					//-----------------------------
				}
			}
			else {
				BUTTON_SET_TR_CONTROL( CTC_RUNNING );
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != CHECKING_SIDE ) {
					if ( ( _i_SCH_SYSTEM_USING_GET(i) > 0 ) && ( SIGNAL_PAUSE[i] == 0 ) ) {
						if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) {
							_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
						}
						else {
							if ( _i_SCH_SYSTEM_RUN_TAG_GET( i ) > 0 ) {
								_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_RUNNING );
							}
							else {
								_SCH_IO_SET_MAIN_BUTTON_MC( i , CTC_WAITING );
							}
							//-----------------------------
						}
					}
				}
			}
		}
		//
		//-------------------------------------------------------------------------------------------------
		//
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , pt , 0 ) == 0 ) {
				Sleep(1);
				continue;
			}
		}
		//
		//-------------------------------------------------------------------------------------------------
		//
		break;
		//
		//-------------------------------------------------------------------------------------------------
		//
	}
	return TRUE;
}
//

void _i_SCH_SYSTEM_INFO_LOG( char *data , char *disp ) { // 2013.02.20
	SCHEDULING_DATA_STRUCTURE_LOG( data , disp , -1 , 0 , 0 );
	SCHEDULING_DATA_STRUCTURE_JOB_LOG( data , disp );
}


BOOL SIGNAL_MODE_WAITR_CHECK( int CHECKING_SIDE ) { // 2005.09.15
	if ( SIGNAL_WAITR[CHECKING_SIDE] ) return TRUE;
	//
	if ( SIGNAL_END[CHECKING_SIDE] == 2 ) return TRUE;
	if ( SIGNAL_END[CHECKING_SIDE] == 4 ) return TRUE;
	if ( SIGNAL_END[CHECKING_SIDE] == 6 ) return TRUE;
	return FALSE;
}

void INIT_SIGNAL_MODE_DATA( int apmode , int side ) {
	int i;
	//
	if ( apmode == 0 ) { // 2016.12.05
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _SCH_SYSTEM_NO_PICK_FROM_CM_SET( i , FALSE );
		//------------------------------------------------------------------------------------
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _SCH_SYSTEM_SIDE_CLOSING_SET( i , FALSE ); // 2013.10.02
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_FLAG_RESET( i ); // 2012.01.12
		//------------------------------------------------------------------------------------
		_i_SCH_SYSTEM_PAUSE_SW_SET( 0 ); // 2004.06.29
		//------------------------------------------------------------------------------------
		SIGNAL_MODE_DISAPPEAR_INIT( apmode );
		//------------------------------------------------------------------------------------
		SIGNAL_MODE_FMTMSIDE_INIT( apmode == 0 ); // 2009.05.06
		//------------------------------------------------------------------------------------
		SIGNAL_MODE_RECIPE_READ_CLEAR( -1 ); // 2009.04.23
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_APPEND_END_SET( i , FALSE ); // 2010.11.30
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( i , FALSE ); // 2012.10.31
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_SET( i , -1 ); // 2014.06.26
		//------------------------------------------------------------------------------------
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Job Id / Mid Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
char			JOB_ID_STRING[MAX_CASSETTE_SIDE][ 64 + 1 ];
char			MID_ID_STRING[MAX_CASSETTE_SIDE][ 64 + 1 ];
//------------------------------------------------------------------------------------------
void  SCHEDULER_JOB_ID_CONTROL_REGIST( int side , char *str );
char *SCHEDULER_JOB_ID_CONTROL_READ( int side );
void  SCHEDULER_MID_ID_CONTROL_REGIST( int side , char *str );
char *SCHEDULER_MID_ID_CONTROL_READ( int side );

void SCHEDULER_JOB_ID_CONTROL_REGIST( int side , char *str ) {
	strncpy( JOB_ID_STRING[side] , str , 64 );
}

char *SCHEDULER_JOB_ID_CONTROL_READ( int side ) {
	return JOB_ID_STRING[side];
}
void SCHEDULER_MID_ID_CONTROL_REGIST( int side , char *str ) {
	strncpy( MID_ID_STRING[side] , str , 64 );
}

char *SCHEDULER_MID_ID_CONTROL_READ( int side ) {
	return MID_ID_STRING[side];
}
*/

MAKE_SYSTEM_CHAR_PARAMETER2( JOB_ID , MAX_CASSETTE_SIDE )
MAKE_SYSTEM_CHAR_PARAMETER2( MID_ID , MAX_CASSETTE_SIDE )

//=======================================================================================
//=======================================================================================

MAKE_SYSTEM_CHAR_PARAMETER2( MODULE_NAME , MAX_CHAMBER )

MAKE_SYSTEM_CHAR_PARAMETER2( MODULE_NAME_for_SYSTEM , MAX_CHAMBER )

/*
char _chamber_String[MAX_CHAMBER][9];
char _chamber_String_System[MAX_CHAMBER][9];

void SYSTEM_SET_MODULE_STRING( int ch , char *data ) {
	strncpy( _chamber_String[ch] , data , 8 );
	_chamber_String[ch][8] = 0;
}
char *_i_SCH_SYSTEM_GET_MODULE_NAME( int ch ) {
	return _chamber_String[ch];
}
char *_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( int ch ) {
	return _chamber_String_System[ch];
}
*/
//=======================================================================================
/*void SYSTEM_TAG_DATA_INIT() {
	int i;
	//
	_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE + 1 ; i++ ) {
		_i_SCH_SYSTEM_USING_SET( i , 0 );
		_i_SCH_SYSTEM_USING2_SET( i , 0 );
		//
		SYSTEM_ENDCHECK_SET( i , 0 );
		//
		SYSTEM_THDCHECK_SET( i , FALSE ); // 2003.05.23
	}
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		strcpy( _chamber_String_System[i] , "" );
		strcpy( _chamber_String[i] , "" );
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		sprintf( _chamber_String_System[i+1] , "CM%d" , i + 1 );
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		sprintf( _chamber_String_System[i+MAX_CASSETTE_SIDE+1] , "PM%d" , i + 1 );
	}
	//
//	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 1 + MAX_PM_CHAMBER_DEPTH ] , "AL " ); // 2006.11.16
	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 1 + MAX_PM_CHAMBER_DEPTH ] , "AL" ); // 2006.11.16
//	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH ] , "IC " ); // 2006.11.16
	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH ] , "IC" ); // 2006.11.16
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		sprintf( _chamber_String_System[i+MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2] , "BM%d" , i + 1 );
	}
	//
	sprintf( _chamber_String_System[MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2+MAX_BM_CHAMBER_DEPTH] , "TM" );
	//
	for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		sprintf( _chamber_String_System[i+MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2+MAX_BM_CHAMBER_DEPTH] , "TM%d" , i + 1 );
	}
	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH ] , "FM" );
	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH + 1 ] , "FAL" );
	strcpy( _chamber_String_System[ MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH + 2 ] , "FIC" );

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		strcpy( _chamber_String[i] , _chamber_String_System[i] );
	}
}
*/

//=======================================================================================
void SYSTEM_TAG_DATA_INIT() {
	int i;
	char Buffer[8];
	//
	_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
	_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 ); // 2008.01.11
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE + 1 ; i++ ) {
		_i_SCH_SYSTEM_USING_SET( i , 0 );
		_i_SCH_SYSTEM_USING2_SET( i , 0 );
		//
		SYSTEM_PREPARE_SET( i , 0 ); // 2008.04.23
		//
		SYSTEM_ENDCHECK_SET( i , 0 );
		//
		SYSTEM_THDCHECK_SET( i , FALSE ); // 2003.05.23
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		sprintf( Buffer , "CM%d" , i + 1 );
		_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( i + 1 , Buffer );
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		sprintf( Buffer , "PM%d" , i + 1 );
		_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( i+MAX_CASSETTE_SIDE+1 , Buffer );
	}
	//
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE + 1 + MAX_PM_CHAMBER_DEPTH , "AL" );
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH , "IC" );
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		sprintf( Buffer , "BM%d" , i + 1 );
		_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( i+MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2 , Buffer );
	}
	//
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2+MAX_BM_CHAMBER_DEPTH , "TM" );
	//
	for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		sprintf( Buffer , "TM%d" , i + 1 );
		_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( i+MAX_CASSETTE_SIDE+1+MAX_PM_CHAMBER_DEPTH+2+MAX_BM_CHAMBER_DEPTH , Buffer );
	}
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH , "FM" );
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH + 1 , "FAL" );
	_i_SCH_SYSTEM_SET_MODULE_NAME_for_SYSTEM( MAX_CASSETTE_SIDE + 2 + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 + MAX_TM_CHAMBER_DEPTH + 2 , "FIC" );

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		_i_SCH_SYSTEM_SET_MODULE_NAME( i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
	}
}
