#ifndef SCH_PRM_H
#define SCH_PRM_H

//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_PRM_DATA( int apmode , int side );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// DISPPEAR Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_Disable_Disappear( int side ); // 2003.12.10
void SCHEDULER_CONTROL_Set_Disable_Disappear( int side , BOOL data ); // 2003.12.10
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// First Time Delay Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void SCHEDULER_FIRST_MODULE_CONTROL_SET( BOOL Use );
void SCHEDULER_FIRST_MODULE_EXPEDITE_SET( int ch , BOOL Use );
void SCHEDULER_SECOND_MODULE_EXPEDITE_SET( int ch , BOOL Use );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Module Status Control Use Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_Mdl_Use_Flag( int side , int ch , int data );
int  _i_SCH_MODULE_Get_Mdl_Use_Flag( int side , int ch );

void _i_SCH_MODULE_Set_Mdl_Spd_Flag( int side , int ch , int data ); // 2006.11.07
int  _i_SCH_MODULE_Get_Mdl_Spd_Flag( int side , int ch ); // 2006.11.07

void _i_SCH_MODULE_Set_Mdl_Run_Flag( int ch , int data );
void _i_SCH_MODULE_Dec_Mdl_Run_Flag( int ch );
int  _i_SCH_MODULE_Get_Mdl_Run_Flag( int ch );
void _i_SCH_MODULE_Inc_Mdl_Run_Flag( int ch );


void SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( int ch , int data ); // 2003.06.13
int  SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( int ch ); // 2003.06.13
void SCHEDULER_CONTROL_Init_Mdl_Run_AbortWait_Flag(); // 2007.05.09

void SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( int side , int ch , BOOL data );
BOOL SCHEDULER_CONTROL_Get_Chamber_Use_Interlock_Pre( int side , int ch );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Global Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( int side , int SchPointer , int finc );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void SCHEDULER_CONTROL_Set_GLOBAL_STOP( int SlotMode , int cm_jc , int slot_jp , BOOL data ); // 2007.11.08 // 2013.11.28
void SCHEDULER_CONTROL_Set_GLOBAL_STOP( int SlotMode , int cm_jc , int slot_jp , int data ); // 2007.11.08 // 2013.11.28
void SCHEDULER_CONTROL_Set_GLOBAL_STOP_FOR_JOB( int jc , int jp , int data ); // 2014.05.21
//----------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
BOOL _i_SCH_MODULE_FIRST_CONTROL_GET();
BOOL _i_SCH_MODULE_FIRST_EXPEDITE_GET( int ch );
BOOL _i_SCH_MODULE_SECOND_EXPEDITE_GET( int ch );
//------------------------------------------------------------------------------------------

void _i_SCH_MODULE_Set_Use_Interlock_Run( int side , int ch , BOOL data );
BOOL _i_SCH_MODULE_Get_Use_Interlock_Run( int side , int ch );

void _i_SCH_MODULE_Set_Use_Interlock_Map( int side );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Global Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SUB_Chk_First_Out_Status( int side , int SchPointer );
BOOL _i_SCH_SUB_Chk_Last_Out_Status( int side , int SchPointer );

void _i_SCH_SUB_Set_Last_Status( int side , BOOL data );
BOOL _i_SCH_SUB_Get_Last_Status( int side );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void _i_SCH_SUB_Chk_GLOBAL_STOP_FDHC( int side ); // 2007.11.08
//----------------------------------------------------------------------------

void _i_SCH_SUB_Make_FORCE_FMSIDE( int mdl );
BOOL _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( int *mdl );

void _i_SCH_SUB_Make_ENABLE_TO_ENABLEN( int data ); // 2015.04.03
int  _i_SCH_SUB_Get_ENABLE_TO_ENABLEN(); // 2015.04.03


#endif

