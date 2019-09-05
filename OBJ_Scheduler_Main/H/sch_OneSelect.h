#ifndef SCH_ONESELECT_H
#define SCH_ONESELECT_H

void INIT_SCHEDULER_CONTROL_ONESELECT_DATA( int , int );

int  _i_SCH_ONESELECT_Get_LastSelectPM();
void _i_SCH_ONESELECT_Set_LastSelectPM( int data ); // 2007.11.12
void _i_SCH_ONESELECT_Reset_First_What( BOOL check , int side , int pointer );
int  _i_SCH_ONESELECT_Get_First_One_What( BOOL check , int side , int pointer , BOOL indexsel , BOOL retbmmode , int *mdl );

#endif

