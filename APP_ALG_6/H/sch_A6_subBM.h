#ifndef SCH_A6_SUB_BM_H
#define SCH_A6_SUB_BM_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling BM Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( int ch );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( int ch );
//
int  SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( int ch , int checksts );
int  SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( int ch , int checksts );
int  SCHEDULER_CONTROL_Chk_BM_FREE_COUNT_STYLE_6( int ch );
int  SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( int ch , int slot ); // 2010.09.09


int SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( int ch );

BOOL SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_and_DUMMY_STYLE_6( int ch ); // 2015.01.12






//
#endif

