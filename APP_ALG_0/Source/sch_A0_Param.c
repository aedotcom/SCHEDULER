//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_sch_prm.h"
#include "INF_User_Parameter.h"
//================================================================================


int  SIGNAL_APPEND_END[ MAX_CASSETTE_SIDE + 1 ]; // 2005.10.25

time_t Cassette_Supply_Timer_STYLE_0;

int	 FM_SchedulerOutFirst_STYLE_0[ MAX_CASSETTE_SIDE ];
int	 TM_Pick_BM_Signal_STYLE_0[ MAX_TM_CHAMBER_DEPTH ]; // 2007.02.01
int	 TM_MidCount_STYLE_0[ MAX_TM_CHAMBER_DEPTH ];
int	 TM_LastMoveSignal_STYLE_0[ MAX_TM_CHAMBER_DEPTH ];
int	 PM_MidCount_STYLE_0[ MAX_TM_CHAMBER_DEPTH ];

int	 BUFFER_STATION_SWITCH_WHAT_STYLE_0[ MAX_CASSETTE_SIDE ];
int  BUFFER_STATION_SWITCH_7_CROSS_STYLE_0[ MAX_CASSETTE_SIDE ]; // 2003.11.28
//int  BUFFER_STATION_SWITCH_USE_STYLE_0[ MAX_CHAMBER ]; // 2008.11.24
int  BUFFER_STATION_SWITCH_SEPARATE_SIDE_STYLE_0; // 2007.01.05
int  BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0; // 2007.01.05
int  BUFFER_STATION_SWITCH_SEPARATE_COUNT_STYLE_0; // 2007.01.05
int	 BUFFER_STATION_SWITCH_SEPARATE_SCH_STYLE_0; // 2007.01.24
int	 BUFFER_STATION_SWITCH_SEPARATE_NCH_STYLE_0; // 2007.01.24
int	 BUFFER_STATION_SWITCH_SEPARATE_SCNT_STYLE_0; // 2007.01.24
//

time_t Process_Progress_Out_Timer_STYLE_0[ MAX_CHAMBER ];

int paththruinit = 0;

//------------------------------------------------------------------------------------------
int  BUFFER_BATCH_SWITCH_BM_ONLY[ MAX_CASSETTE_SIDE ]; // 2010.01.01
//------------------------------------------------------------------------------------------
BOOL			TM_INTERLOCK_USE[MAX_TM_CHAMBER_DEPTH];
//------------------------------------------------------------------------------------------
int				MULTI_GROUP_PLACE[ MAX_CHAMBER ]; // 2006.02.01
int				MULTI_GROUP_PICK[ MAX_CHAMBER ]; // 2006.02.01
//
int				MULTI_GROUP_TMS[ MAX_TM_CHAMBER_DEPTH ]; // 2013.03.19
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int				MODULE_BUFFER_SINGLE_MODE = 0;
int				MODULE_BUFFER_USE_MODE = 0;
int				MODULE_BUFFER_SWITCH_MODE = 0;
//------------------------------------------------------------------------------------------
int				FULLY_RETURN_MODE = 0; // 2006.02.01
int				FULLY_RETURN_CH[ MAX_CHAMBER ]; // 2006.02.01
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void KPLT0_INIT_CASSETTE_TIME_SUPPLY() {
	Cassette_Supply_Timer_STYLE_0 = 0;
}

void KPLT0_RESET_CASSETTE_TIME_SUPPLY() {
	time( &Cassette_Supply_Timer_STYLE_0 );
}
//
BOOL KPLT0_CHECK_CASSETTE_TIME_SUPPLY( long timevalue ) {
	time_t finish;
	long l;
	if ( Cassette_Supply_Timer_STYLE_0 == 0 ) return TRUE;
	if ( timevalue == 0 ) return TRUE;
	time( &finish );
	l = (long) difftime( finish , Cassette_Supply_Timer_STYLE_0 );
	if ( l < 0 ) return FALSE;
	if ( l >= timevalue ) return TRUE;
	return FALSE;
}

void KPLT0_SET_PROCESS_TIME_OUT_INIT() {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Process_Progress_Out_Timer_STYLE_0[ i ] = 0;
	}
}

//
long KPLT0_GET_PROCESS_TIME_OUT_WAIT( int chamber ) {
	long l;
	time_t finish;
	time( &finish );
	l = (long) difftime( finish , Process_Progress_Out_Timer_STYLE_0[chamber] );
	if ( l < 0 ) l = 0;
	return l;
}
//
void KPLT0_SET_PROCESS_TIME_OUT( int chamber ) {
	time( &Process_Progress_Out_Timer_STYLE_0[chamber] );
}
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( int side , int data ) { BUFFER_BATCH_SWITCH_BM_ONLY[side] = data; }
int  SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( int side ) { return BUFFER_BATCH_SWITCH_BM_ONLY[side]; }

// 0 : LP->LP
// 1 : LL->LL
// 2 : LP->LL
// 3 : LL->LP



void SCHEDULER_Set_PATHTHRU_MODE( int data ) { paththruinit = data; }
int  SCHEDULER_Get_PATHTHRU_MODE() { return paththruinit; }


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int  SIGNAL_MODE_APPEND_END_GET( int CHECKING_SIDE ) {	return SIGNAL_APPEND_END[CHECKING_SIDE];	} // 2005.10.25
void SIGNAL_MODE_APPEND_END_SET( int CHECKING_SIDE , int data ) {	SIGNAL_APPEND_END[CHECKING_SIDE] = data;	} // 2005.10.25

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_FM_OUT_FIRST( int side , int data ) { FM_SchedulerOutFirst_STYLE_0[side] = data; }
int  SCHEDULER_Get_FM_OUT_FIRST( int side ) { return FM_SchedulerOutFirst_STYLE_0[side]; }

void SCHEDULER_Set_TM_MidCount( int TMATM , int data ) { TM_MidCount_STYLE_0[ TMATM ] = data; }
int  SCHEDULER_Get_TM_MidCount( int TMATM ) { return TM_MidCount_STYLE_0[ TMATM ]; }

void SCHEDULER_Set_TM_Pick_BM_Signal( int TMATM , int data ) { TM_Pick_BM_Signal_STYLE_0[ TMATM ] = data; } // 2007.02.01
int  SCHEDULER_Get_TM_Pick_BM_Signal( int TMATM ) { return TM_Pick_BM_Signal_STYLE_0[ TMATM ]; } // 2007.02.01


void SCHEDULER_Set_TM_LastMove_Signal( int TMATM , int Data ) { TM_LastMoveSignal_STYLE_0[TMATM] = Data; }
int  SCHEDULER_Get_TM_LastMove_Signal( int TMATM ) { return( TM_LastMoveSignal_STYLE_0[TMATM] ); }

void SCHEDULER_Set_PM_MidCount( int tms , int data ) { PM_MidCount_STYLE_0[tms] = data; }
int  SCHEDULER_Get_PM_MidCount( int tms ) { return PM_MidCount_STYLE_0[tms]; }


void SCHEDULER_CONTROL_Cls_BM_Switch_Use() {
//	int i; // 2008.11.24
//	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2008.11.24
//		BUFFER_STATION_SWITCH_USE_STYLE_0[i] = -1; // 2008.11.24
//	} // 2008.11.24
	BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0 = FALSE;
	BUFFER_STATION_SWITCH_SEPARATE_COUNT_STYLE_0 = 0;
}


void SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( int side , int ch ) { BUFFER_STATION_SWITCH_WHAT_STYLE_0[side] = ch; }
int	 SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( int side ) { return BUFFER_STATION_SWITCH_WHAT_STYLE_0[side]; }

void SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( int side , int data ) { BUFFER_STATION_SWITCH_7_CROSS_STYLE_0[side] = data; } // 2003.11.28
int  SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( int side ) { return BUFFER_STATION_SWITCH_7_CROSS_STYLE_0[side]; } // 2003.11.28

//void SCHEDULER_CONTROL_Set_BM_Switch_UseSide( int ch , int data ) { BUFFER_STATION_SWITCH_USE_STYLE_0[ch] = data; } // 2008.11.24
//int  SCHEDULER_CONTROL_Get_BM_Switch_UseSide( int ch ) { return BUFFER_STATION_SWITCH_USE_STYLE_0[ch]; } // 2008.11.24

void SCHEDULER_CONTROL_Set_BM_Switch_Separate( int data , int side , int count , int sch , int nch ) {
	if      ( data == 1 ) {
		BUFFER_STATION_SWITCH_SEPARATE_COUNT_STYLE_0 = count;
		BUFFER_STATION_SWITCH_SEPARATE_SIDE_STYLE_0 = side;
		BUFFER_STATION_SWITCH_SEPARATE_SCH_STYLE_0 = sch;
		BUFFER_STATION_SWITCH_SEPARATE_NCH_STYLE_0 = nch;
		BUFFER_STATION_SWITCH_SEPARATE_SCNT_STYLE_0 = 0;
		//
		BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0 = data;
	}
	else if ( data == 2 ) {
		BUFFER_STATION_SWITCH_SEPARATE_SCNT_STYLE_0 = count;
		//
		BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0 = data;
	}
	else if ( data == 0 ) {
		BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0 = data;
		//
		BUFFER_STATION_SWITCH_SEPARATE_COUNT_STYLE_0 = 0;
		BUFFER_STATION_SWITCH_SEPARATE_SIDE_STYLE_0 = 0;
		BUFFER_STATION_SWITCH_SEPARATE_SCH_STYLE_0 = 0;
		BUFFER_STATION_SWITCH_SEPARATE_NCH_STYLE_0 = 0;
		BUFFER_STATION_SWITCH_SEPARATE_SCNT_STYLE_0 = 0;
	}
	else {
		BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0 = data;
	}
}
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() { return BUFFER_STATION_SWITCH_SEPARATE_TAG_STYLE_0; }
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateCount() { return BUFFER_STATION_SWITCH_SEPARATE_COUNT_STYLE_0; }
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() { return BUFFER_STATION_SWITCH_SEPARATE_SIDE_STYLE_0; }
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() { return BUFFER_STATION_SWITCH_SEPARATE_SCH_STYLE_0; }
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch() { return BUFFER_STATION_SWITCH_SEPARATE_NCH_STYLE_0; }
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSCnt() { return BUFFER_STATION_SWITCH_SEPARATE_SCNT_STYLE_0; }


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Get_Prm_MODULE_MOVE_GROUP( int Chamber ) { // 2006.05.25
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) > 0 ) return TRUE;
	if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) > 0 ) return TRUE;
	return FALSE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi Group Return Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_MULTI_GROUP_PLACE( int ch , int gp ) { // 2006.02.08
	int i;
	if ( ch == -1 ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			MULTI_GROUP_PLACE[i] = gp;
		}
	}
	else {
		MULTI_GROUP_PLACE[ch] = gp;
	}
}

int  SCHEDULER_Get_MULTI_GROUP_PLACE( int ch ) { // 2006.02.08
	return MULTI_GROUP_PLACE[ch];

}

void SCHEDULER_Set_MULTI_GROUP_PICK( int ch , int gp ) { // 2006.02.08
	int i;
	if ( ch == -1 ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			MULTI_GROUP_PICK[i] = gp;
		}
	}
	else {
		MULTI_GROUP_PICK[ch] = gp;
	}
}

int  SCHEDULER_Get_MULTI_GROUP_PICK( int ch ) { // 2006.02.08
	return MULTI_GROUP_PICK[ch];
}

//

void SCHEDULER_Init_MULTI_GROUP_TMS() { // 2013.03.19
	int i;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) MULTI_GROUP_TMS[i] = FALSE;
}

void SCHEDULER_Set_MULTI_GROUP_TMS( int id ) { // 2013.03.19
	MULTI_GROUP_TMS[id] = TRUE;
}

void SCHEDULER_ReSet_MULTI_GROUP_TMS( int id ) { // 2013.03.19
	MULTI_GROUP_TMS[id] = FALSE;
}

int  SCHEDULER_Get_MULTI_GROUP_TMS( int id ) { // 2013.03.19
	return MULTI_GROUP_TMS[id];
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int Get_Prm_MODULE_BUFFER_SINGLE_MODE() {
	return MODULE_BUFFER_SINGLE_MODE;
}

int Get_Prm_MODULE_BUFFER_SWITCH_MODE() {
	return MODULE_BUFFER_SWITCH_MODE;
}

int Get_Prm_MODULE_BUFFER_USE_MODE() {
	return MODULE_BUFFER_USE_MODE;
}

void Set_Prm_MODULE_BUFFER_SINGLE_MODE( int data ) {
	MODULE_BUFFER_SINGLE_MODE = data;
}

void Set_Prm_MODULE_BUFFER_SWITCH_MODE( int data ) {
	MODULE_BUFFER_SWITCH_MODE = data;
}

void Set_Prm_MODULE_BUFFER_USE_MODE( int data ) {
	MODULE_BUFFER_USE_MODE = data;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Fully Return Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_FULLY_RETURN_MODE( int Use ) { // 2006.02.01
	int i;
	if ( Use == 0 ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			FULLY_RETURN_CH[i] = 0;
		}
	}
	if ( Use == 1 ) {
		if ( FULLY_RETURN_MODE == 0 ) FULLY_RETURN_MODE = 1;
	}
	else {
		FULLY_RETURN_MODE = Use;
	}
}
//
int SCHEDULER_Get_FULLY_RETURN_MODE() { // 2006.02.01
	return FULLY_RETURN_MODE;
}
//
void SCHEDULER_Set_FULLY_RETURN_CH( int ch , int data ) { // 2006.02.01
	FULLY_RETURN_CH[ch] = data;
	if ( data ) SCHEDULER_Set_FULLY_RETURN_MODE( 1 );
}
//
int SCHEDULER_Get_FULLY_RETURN_CH( int ch ) { // 2006.02.01
	return FULLY_RETURN_CH[ch];
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FM.PM Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL RunPrm_FM_PM_CONTROL_USE;
int  RunPrm_FM_PM_START_MODULE;
int  RunPrm_FM_PM_SEL_TIME[MAX_CHAMBER];

void Set_RunPrm_FM_PM_CONTROL_USE() { // 2014.11.09
	int k;
	//
	RunPrm_FM_PM_CONTROL_USE = FALSE;
	RunPrm_FM_PM_START_MODULE = PM1;
	//
	for ( k = PM1 ; k < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; k++ ) {
		//
		RunPrm_FM_PM_SEL_TIME[k] = GetTickCount();
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( k ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( k ) != -1 ) continue;
		//
		RunPrm_FM_PM_START_MODULE = k;
		RunPrm_FM_PM_CONTROL_USE = TRUE;
		//
		_SCH_SUB_Make_ENABLE_TO_ENABLEN( 3 ); // 2016.02.19
		//
		return;
		//
	}
	//
}

BOOL Get_RunPrm_FM_PM_CONTROL_USE() { // 2014.11.09
	return RunPrm_FM_PM_CONTROL_USE;
}

int Get_RunPrm_FM_PM_START_MODULE() { // 2014.11.09
	return RunPrm_FM_PM_START_MODULE;
}

BOOL Get_RunPrm_FM_PM_SEL_PM( int seledpm , int checkpm ) { // 2014.11.25
	int t;
	//
	t = GetTickCount();
	//
	if ( ( t - RunPrm_FM_PM_SEL_TIME[seledpm] ) < ( t - RunPrm_FM_PM_SEL_TIME[checkpm] ) ) return TRUE;
	//
	return FALSE;
}

void Set_RunPrm_FM_PM_SEL_PM( int pm ) { // 2014.11.25
	RunPrm_FM_PM_SEL_TIME[pm] = GetTickCount();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// TM Interlock Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Set_RunPrm_TM_INTERLOCK_USE() { // 2002.04.27
	int i , j;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		TM_INTERLOCK_USE[i] = FALSE;
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( i , j + PM1 ) > 0 ) {
				TM_INTERLOCK_USE[i] = TRUE;
				break;
			}
		}
	}
}
//----------------------------------------------------------------------------
BOOL Get_RunPrm_TM_INTERLOCK_USE( int tmid ) { // 2002.04.27
	return TM_INTERLOCK_USE[tmid]; 
}
//----------------------------------------------------------------------------
BOOL Get_RunPrm_TM_INTERLOCK_USE_CHECK( int tmid , int PickCh , int AnoCh ) { // 2002.04.27
	int i , j;
	if ( !TM_INTERLOCK_USE[tmid] ) return TRUE;
	i = _SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( tmid , PickCh );
	if ( i <= 0 ) return TRUE;
	j = _SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( tmid , AnoCh  );
	if ( j <= 0 ) return TRUE;
	if ( i == j ) return FALSE;
	return TRUE;
}
