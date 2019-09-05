#ifndef SCH_CASSMAP_H
#define SCH_CASSMAP_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int Scheduler_Get_Max_Slot( int side , int in , int out , int opt , BOOL * ); // 2010.12.17

void Scheduler_Carrier_Mapping( int CHECKING_SIDE );
int  Scheduler_Mapping_Real_Part( int CHECKING_SIDE , BOOL First , int ch1 , int ch2 , int ss , int es );
int  Scheduler_Mapping_Check_Part( int CHECKING_SIDE , int Count , int ch1 , int ch2 , int ss , int es , int CPJOB );


void Scheduler_Mapping_Buffer_Data_Reset( int side );
int  Scheduler_Mapping_Buffer_Data_Get1( int side , int i );
int  Scheduler_Mapping_Buffer_Data_Get2( int side , int i );
void Scheduler_Mapping_Buffer_Data_Set( int side , int i , int mode , int data );

#endif

