#ifndef SCH_A0_SUB_BM_H
#define SCH_A0_SUB_BM_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling BM Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_ALL( int ch );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL( int ch );
//
int  SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( int ch , int checksts );
int  SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( int ch );

int  SCHEDULER_CONTROL_Chk_BM_FREE_DBL( int ch ); // 2015.07.02








//
#endif

