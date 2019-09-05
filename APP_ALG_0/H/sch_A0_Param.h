#ifndef SCH_A0_PARAM_H
#define SCH_A0_PARAM_H


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum {
	BATCHALL_SWITCH_LP_2_LP	,
	BATCHALL_SWITCH_LL_2_LL	,
	BATCHALL_SWITCH_LP_2_LL	,
	BATCHALL_SWITCH_LL_2_LP
};

void SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( int side , int data );
int  SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( int side );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_PATHTHRU_MODE( int data );
int  SCHEDULER_Get_PATHTHRU_MODE();

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void KPLT0_INIT_CASSETTE_TIME_SUPPLY();
void KPLT0_RESET_CASSETTE_TIME_SUPPLY();
BOOL KPLT0_CHECK_CASSETTE_TIME_SUPPLY( long timevalue );
//---
long KPLT0_GET_PROCESS_TIME_OUT_WAIT( int chamber );
void KPLT0_SET_PROCESS_TIME_OUT( int chamber );
void KPLT0_SET_PROCESS_TIME_OUT_INIT();

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SIGNAL_MODE_APPEND_END_GET( int ); // 2005.10.25
void SIGNAL_MODE_APPEND_END_SET( int , int ); // 2005.10.25
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Interlock Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_TM_Pick_BM_Signal( int TMATM , int data );
int  SCHEDULER_Get_TM_Pick_BM_Signal( int TMATM );

void SCHEDULER_Set_TM_MidCount( int TMATM , int data );
int  SCHEDULER_Get_TM_MidCount( int TMATM );

void SCHEDULER_Set_FM_OUT_FIRST( int side , int data );
int  SCHEDULER_Get_FM_OUT_FIRST( int side );

void SCHEDULER_Set_TM_LastMove_Signal( int TMATM , int Data );
int  SCHEDULER_Get_TM_LastMove_Signal( int TMATM );

void SCHEDULER_Set_PM_MidCount( int tms , int data );
int  SCHEDULER_Get_PM_MidCount( int tms );

void SCHEDULER_CONTROL_Cls_BM_Switch_Use();

void SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( int side , int ch );
int	 SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( int side );

void SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( int side , int data );
int  SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( int side );

//void SCHEDULER_CONTROL_Set_BM_Switch_UseSide( int ch , int data ); // 2008.11.24
//int  SCHEDULER_CONTROL_Get_BM_Switch_UseSide( int ch ); // 2008.11.24

void SCHEDULER_CONTROL_Set_BM_Switch_Separate( int data , int side , int count , int sch , int nch );
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode();
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateCount();
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide();
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch();
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch();
int  SCHEDULER_CONTROL_Get_BM_Switch_SeparateSCnt();

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Get_Prm_MODULE_MOVE_GROUP( int Chamber ); // 2006.05.25

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi Group Place Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_MULTI_GROUP_PLACE( int ch , int gp ); // 2006.02.08
int  SCHEDULER_Get_MULTI_GROUP_PLACE( int ch ); // 2006.02.08
//
void SCHEDULER_Set_MULTI_GROUP_PICK( int ch , int gp ); // 2006.02.08
int  SCHEDULER_Get_MULTI_GROUP_PICK( int ch ); // 2006.02.08

void SCHEDULER_Init_MULTI_GROUP_TMS(); // 2013.03.19
void SCHEDULER_Set_MULTI_GROUP_TMS( int id ); // 2013.03.19
void SCHEDULER_ReSet_MULTI_GROUP_TMS( int id ); // 2013.03.19
int  SCHEDULER_Get_MULTI_GROUP_TMS( int id ); // 2013.03.19
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Get_Prm_MODULE_BUFFER_SINGLE_MODE();
int Get_Prm_MODULE_BUFFER_SWITCH_MODE();
int Get_Prm_MODULE_BUFFER_USE_MODE();

void Set_Prm_MODULE_BUFFER_SINGLE_MODE( int data );
void Set_Prm_MODULE_BUFFER_SWITCH_MODE( int data );
void Set_Prm_MODULE_BUFFER_USE_MODE( int data );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Fully Return Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_FULLY_RETURN_MODE( int Use ); // 2006.02.01
int  SCHEDULER_Get_FULLY_RETURN_MODE(); // 2006.02.01
void SCHEDULER_Set_FULLY_RETURN_CH( int ch , int data ); // 2006.02.01
int  SCHEDULER_Get_FULLY_RETURN_CH( int ch ); // 2006.02.01


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// TM Interlock Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Set_RunPrm_TM_INTERLOCK_USE();
BOOL Get_RunPrm_TM_INTERLOCK_USE( int tmid );
BOOL Get_RunPrm_TM_INTERLOCK_USE_CHECK( int tmid , int PickCh , int AnoCh );


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Set_RunPrm_FM_PM_CONTROL_USE(); // 2014.11.09
BOOL Get_RunPrm_FM_PM_CONTROL_USE(); // 2014.11.09
int Get_RunPrm_FM_PM_START_MODULE(); // 2014.11.09

BOOL Get_RunPrm_FM_PM_SEL_PM( int seledpm , int checkpm ); // 2014.11.25
void Set_RunPrm_FM_PM_SEL_PM( int pm ); // 2014.11.25


#endif

