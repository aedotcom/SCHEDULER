#ifndef SCH_PREPRCS_H
#define SCH_PREPRCS_H

//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_CONTROL_FIRSTRUNPRE_DATA( int , int );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Global Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag( int side , int ch );
int  SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag( int side , int ch );
//
void SCHEDULER_GLOBAL_Set_Chamber_Pre_NotUse_Flag( int ch , int data );
int  SCHEDULER_GLOBAL_Get_Chamber_Pre_NotUse_Flag( int ch );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( int side , int data ); // 2003.11.11
void _i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( int side , int ch ); // 2005.02.17
void _i_SCH_PREPRCS_FirstRunPre_Clear_PathProcessData( int side , int ch ); // 2005.12.20
BOOL _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( int side , int ch ); // 2003.11.11
void _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( int side , int ch ); // 2003.11.11
//
void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse( int side , char *data ); // 2005.10.27
void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse2( int side , char *data ); // 2007.09.07

//------------------------------------------------------------------------------------------

#endif

