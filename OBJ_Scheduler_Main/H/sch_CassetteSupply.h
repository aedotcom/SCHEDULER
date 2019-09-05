#ifndef SCH_CASSETTE_SUPPLY_H
#define SCH_CASSETTE_SUPPLY_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Side Scheck Control Use Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_Side_Check_Flag( int side );
int SCHEDULER_CONTROL_Get_Side_Check_Supply_Flag( int side );
int SCHEDULER_CONTROL_Get_Side_Check_Current_Run_Count( int side );
//
void SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( int side , int count ); // 2003.10.10
//void SCHEDULER_CONTROL_Dec_Side_Monitor_Cluster_Count( int side ); // 2006.06.22 // 2016.03.09

BOOL _i_SCH_CASSETTE_Check_Side_Monitor( int side );
//
void _i_SCH_CASSETTE_Reset_Side_Monitor( int side , int init );
BOOL _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_FDHC( int side , BOOL EndCheck , int sup_style );
BOOL _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( int side , BOOL EndCheck , int sup_style , BOOL usermode , BOOL log );
void _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( int side , int incwafercount );
//
int  _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( int side ); // 2003.10.21

#endif

