#ifndef SCH_SDM_H
#define SCH_SDM_H

//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Save_SDM_ins_DATA();
void SCHEDULER_CONTROL_Read_SDM_ins_DATA();
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_LOTFIRST_MODULE( int ch , int data );
void SCHEDULER_CONTROL_Set_SDM_ins_LOTEND_MODULE( int ch , int data );
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_MODULE( int pointer , int data );
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( int pointer , int style , int id , char *recipe );
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( int pointer , int style , int id , char *recipe );
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE( int pointer , int style , int id , char *recipe );
//------------------------------------------------------------------------------------------
//void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RESET_COUNT( int pointer ); // 2012.02.15
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Pre_Inc( int side ); // 2015.07.30
//------------------------------------------------------------------------------------------
void _i_SCH_SDM_Set_MODE_INC_EVENT( int data ); // 2015.01.30
//------------------------------------------------------------------------------------------
void _i_SCH_SDM_Set_CHAMBER_INC_COUNT_SUB( int pointer ); // 2015.01.30
//------------------------------------------------------------------------------------------
void _i_SCH_SDM_Set_CHAMBER_INC_COUNT( int pointer );
//------------------------------------------------------------------------------------------
int  _i_SCH_SDM_Get_LOTFIRST_OPTION( int ch );
//------------------------------------------------------------------------------------------
int  _i_SCH_SDM_Get_LOTEND_OPTION( int ch );
//------------------------------------------------------------------------------------------
char *_i_SCH_SDM_Get_RECIPE( int , int slot , int style , int mode );
//------------------------------------------------------------------------------------------
int  _i_SCH_SDM_Get_RUN_CHAMBER( int slot );
//------------------------------------------------------------------------------------------
int  _i_SCH_SDM_Get_RUN_USECOUNT( int slot );
//------------------------------------------------------------------------------------------
void _i_SCH_SDM_Set_CHAMBER_RESET_COUNT( int slot ); // 2012.02.15
//------------------------------------------------------------------------------------------

#endif

