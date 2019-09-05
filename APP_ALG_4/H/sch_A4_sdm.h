#ifndef SCH_A4_SDM_H
#define SCH_A4_SDM_H

//===================
void INIT_SCHEDULER_AL4_SUB_SDM_PART_DATA( int apmode , int side );

int SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S1_TAG( int ch );
int SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S2_TAG( int ch );
int SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( int ch );

void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( int pointer , int style , int id , char *recipe , int mc );
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_PRE_RECIPE( int pointer , int style , int id );

void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( int pointer ,int style ,  int id , char *recipe , int mc );
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_RUN_RECIPE( int pointer , int style , int id );

void SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( int pointer , int style , int id , char *recipe , int mc );
BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_POST_RECIPE( int pointer , int style , int id );

void SCHEDULER_CONTROL_Get_SDM_4_Status_String( int side , int bmch , char *data ); // 2003.05.29
//
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2( int side , int pointer ); // 2005.08.03
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2_RESET( int side , int pointer ); // 2005.08.03
BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER_DELETE( int side , int pointer ); // 2005.12.05

BOOL SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_ONLY_WAFER( int side , int ch ); // 2006.01.14

BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT0( int side , int ch ); // 2005.12.19
BOOL SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT( int side , int ch , int *pointer , int *slot );
int  SCHEDULER_CONTROL_Get_SDM_4_SUPPLY_WAFER_SLOT( int side , int ch , int *pointer , int *slot );
int  SCHEDULER_CONTROL_Get_SDM_4_NOTHING_WAFER_SLOT( int side , int ch , int pointer ); // 2006.07.21
//
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( int pointer , BOOL end ); // 2005.08.01
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( int ch ); // 2005.08.04
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( int ch ); // 2005.09.21

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_GO_WHERE( int pointer , int *ch );
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_RUN( int pointer );
int  SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( int pointer );
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( int pointer );
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_END_REJECT( int pointer ); // 2006.09.02

int  SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( int pointer );
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE_NOT_FINISH( int pointer ); // 2006.01.17
int  SCHEDULER_CONTROL_Get_SDM_4_CHAMER_SIDE_STYLE( int pointer );
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( int ch );
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN_SIDE( int side );

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( int ch );
int  SCHEDULER_CONTROL_Get_SDM_4_CHAMER_SIDE_LOTFIRST( int ch );
int  SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( int side , int ch , int *pt , int option , int first , BOOL *wait , BOOL checkmode );
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_LOTFIRST( int side , int ch , int pt );

int  SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( int side , int ch , int switchp , int option , int index );
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_PM_ENDMONITOR( int side , int ch , int *pointer , int index );

BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTAPPEND( int side , int ch );

BOOL SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_ENDFINISH( int ch ); // 2006.09.02
void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_ENDFINISH( int ch ); // 2006.09.02
void SCHEDULER_CONTROL_Reg_SDM_4_CHAMER_ENDFINISH( int ch ); // 2006.09.02
BOOL SCHEDULER_CONTROL_Run_SDM_4_CHAMER_ENDFINISH( int ch ); // 2006.09.02
void SCHEDULER_CONTROL_Reset_SDM_4_CHAMER_ENDFINISH( int ch ); // 2006.09.02

int  SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTEND( int side , int ch , int *pointer , int spdata ); // 2005.07.29
int  SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( int side , int ch , int endpick , int pt , int *pointer , int spdata ); // 2005.07.29

BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( int side , int ch );

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( int side , int pointer );

int  SCHEDULER_CONTROL_Change_SDM_4_Dummy_Side( int orgside , int side );
BOOL SCHEDULER_CONTROL_Wait_SDM_4_Dummy_Chamber( int ch ); // 2006.01.26

void SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( int ch , BOOL endalso );

BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PLACE_PM_SKIP_CHECK( int pointer );
BOOL SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CHECK( int pointer , int *dummode );
int  SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( int side , int ch , BOOL Cycle , int *pointer ); // 2003.08.23
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_CLEAR( int pointer );
BOOL SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_END_RESET( int ch ); // 2005.08.26
//===========================================================================================

#endif

