#ifndef TIMER_HANDLING_H
#define TIMER_HANDLING_H

void INIT_TIMER_KPLT_DATA( int , int );

void KPLT_PROCESS_TIME_INIT( int chamber , int t1 , int t2 );
//
//void KPLT_PROCESS_TIME_RESET( int ch ); // 2005.04.19 // 2008.04.18
//======================================================================
int  KPLT_GET_MFIC_TRG_TIMER( int slot ); // 2011.05.19
void KPLT_RESET_MFIC_RUN_TIMER( int slot , int slot2  , int targettime , BOOL targetsetonly );
BOOL KPLT_CHECK_MFIC_RUN_TIMER1( int slot , time_t finish );
int  KPLT_CHECK_MFIC_RUN_TIMER2( int slot , time_t finish , int *target ); // 2005.12.01
int  KPLT_CHECK_MFIC_HOWLONG_TIMER( int slot1 , int slot2 , time_t finish );
void KPLT_RESET_MFIC_CURRTRG_TIMER( int slot1 , BOOL ); // 2007.07.03
void KPLT_CHECK_MFIC_START_TIME( int slot , char *target ); // 2007.07.04
//
void KPLT_RESET_MFAL_RUN_TIMER();
BOOL KPLT_CHECK_MFAL_RUN_TIMER( long timevalue );
//
//void KPLT_RESET_BM_RUN_TIMER( int ch , long timevalue ); // 2016.12.09
//BOOL KPLT_CHECK_BM_RUN_TIMER( int ch ); // 2016.12.09
//
void KPLT_RESET_BM_RUN_MS_TIMER( int ch , int targetvalue ); // 2016.12.09
BOOL KPLT_CHECK_BM_RUN_MS_TIMER( int ch ); // 2016.12.09
//
void KPLT_Current_Time( char *buffer );
//---
void KPLT_Local_Time_for_File( BOOL sysmode , char *buffer , int Sep );

//=================================================================================================================
void KPLT_START_FM_PICK_ROBOT_TIME( int fms , int mode );
void KPLT_END_FM_PICK_ROBOT_TIME( int fms , int mode );
long KPLT_GET_FM_PICK_ROBOT_TIME( int fms , int mode );
//===========
void KPLT_START_FM_PLACE_ROBOT_TIME( int fms , int mode );
void KPLT_END_FM_PLACE_ROBOT_TIME( int fms , int mode );
long KPLT_GET_FM_PLACE_ROBOT_TIME( int fms , int mode );
//=================================================================================================================
void KPLT_START_TM_PICK_ROBOT_TIME( int tms , int mode );
void KPLT_END_TM_PICK_ROBOT_TIME( int tms , int mode );
long KPLT_GET_TM_PICK_ROBOT_TIME( int tms , int mode );
//===========
void KPLT_START_TM_PLACE_ROBOT_TIME( int tms , int mode );
void KPLT_END_TM_PLACE_ROBOT_TIME( int tms , int mode );
long KPLT_GET_TM_PLACE_ROBOT_TIME( int tms , int mode );
//=================================================================================================================
unsigned long KPLT_GET_LOT_PROGRESS_TIME( int ch ); // 2008.09.27
//======================================================================
long _i_SCH_TIMER_GET_PROCESS_TIME_START(   int mode , int chamber , BOOL );
long _i_SCH_TIMER_GET_PROCESS_TIME_END(     int mode , int chamber , BOOL );
long _i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( int mode , int chamber );
long _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN(  int mode , int chamber );
long _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( int mode , int chamber );
long _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN_for_PE( int mode , int chamber ); // 2013.04.01
long _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( int mode , int chamber ); // 2012.09.26
long _i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( int mode , int chamber ); // 2014.11.14
//======================================================================
void _i_SCH_TIMER_SET_PROCESS_TIME_START( int mode , int chamber );
void _i_SCH_TIMER_SET_PROCESS_TIME_END(   int mode , int chamber , BOOL Success );
void _i_SCH_TIMER_SET_PROCESS_TIME_SKIP(  int mode , int chamber ); // 2006.02.10
void _i_SCH_TIMER_SET_PROCESS_TIME_TARGET( int mode , int chamber , int trgtime ); // 2006.02.09
//======================================================================
void _i_SCH_TIMER_SET_ROBOT_TIME_START( int TMATM , int finger );
void _i_SCH_TIMER_SET_ROBOT_TIME_END( int TMATM , int finger );
//======================================================================
long _i_SCH_TIMER_GET_ROBOT_TIME_REMAIN( int TMATM , int finger );
long _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( int TMATM , int finger );
//======================================================================



#endif
