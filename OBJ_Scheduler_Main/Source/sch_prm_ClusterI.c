#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "sch_prm_cluster.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int				Sch_User_Path_In[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
int				Sch_User_Path_Out[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
int				Sch_User_Slot_Out[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
int				Sch_User_Order_In[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
//------------------------------------------------------------------------------------------
int				Sch_SUPPLY_ID_DATA = 0; // 2007.09.05
int				Sch_SUPPLY_ID_DATA_EXPEDITE = -99999999; // 2008.12.18
int				Sch_PRCS_ID_DATA = 0; // 2013.05.06
//------------------------------------------------------------------------------------------
int				Sch_SUPPLY_ID_MODE[ MAX_CASSETTE_SIDE ];
//------------------------------------------------------------------------------------------
int				Sch_SUPPLY_ORDERMODE_OFFSET[ MAX_CASSETTE_SIDE ]; // 2016.08.26
//------------------------------------------------------------------------------------------
void _i_SCH_CLUSTER_Set_User_PathIn( int side , int SchPointer , int data ) { Sch_User_Path_In[side][ SchPointer ] = data; }
int  _i_SCH_CLUSTER_Get_User_PathIn( int side , int SchPointer ) { return Sch_User_Path_In[side][ SchPointer ]; }
//
void _i_SCH_CLUSTER_Set_User_PathOut( int side , int SchPointer , int data ) { Sch_User_Path_Out[side][ SchPointer ] = data; }
int  _i_SCH_CLUSTER_Get_User_PathOut( int side , int SchPointer ) { return Sch_User_Path_Out[side][ SchPointer ]; }
//
void _i_SCH_CLUSTER_Set_User_SlotOut( int side , int SchPointer , int data ) { Sch_User_Slot_Out[side][ SchPointer ] = data; }
int  _i_SCH_CLUSTER_Get_User_SlotOut( int side , int SchPointer ) { return Sch_User_Slot_Out[side][ SchPointer ]; }
//
void _i_SCH_CLUSTER_Set_User_OrderIn( int side , int SchPointer , int data ) { Sch_User_Order_In[side][ SchPointer ] = data; }
int  _i_SCH_CLUSTER_Get_User_OrderIn( int side , int SchPointer ) { return Sch_User_Order_In[side][ SchPointer ]; }
//----------------------------------------------------
void SCHEDULER_CONTROL_Inf_Reset_SupplyID() { // 2007.09.05
//	Sch_SUPPLY_ID_DATA = 0; // 2013.05.06
	Sch_SUPPLY_ID_DATA = 1; // 2013.05.06
	Sch_SUPPLY_ID_DATA_EXPEDITE = -99999999; // 2008.12.18
	Sch_PRCS_ID_DATA = 1; // 2013.05.06
}
//
void SCHEDULER_CONTROL_Inf_Mode_SupplyID( int side , int mode ) { // 2008.12.18
	Sch_SUPPLY_ID_MODE[side] = mode;
}
//
void _i_SCH_CLUSTER_Make_SupplyID( int side , int SchPointer ) { // 2007.09.05
	if ( Sch_SUPPLY_ID_MODE[side] == 0 ) {
		//
		if ( _i_SCH_CLUSTER_Get_Ex_OrderMode( side , SchPointer ) != 0 ) { // 2016.08.26
			_i_SCH_CLUSTER_Set_SupplyID( side , SchPointer , Sch_SUPPLY_ORDERMODE_OFFSET[side] );
			Sch_SUPPLY_ORDERMODE_OFFSET[side]++;
		}
		else {
			_i_SCH_CLUSTER_Set_SupplyID( side , SchPointer , Sch_SUPPLY_ID_DATA );
			Sch_SUPPLY_ID_DATA++;
		}
		//
	}
	else { // 2008.12.18
		_i_SCH_CLUSTER_Set_SupplyID( side , SchPointer , Sch_SUPPLY_ID_DATA_EXPEDITE );
		Sch_SUPPLY_ID_DATA_EXPEDITE++;
	}
}
//
void SCHEDULER_CONTROL_Check_OrderMode_SupplyID( int side ) { // 2016.08.26
	int i , c;
	//
	c = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_Ex_OrderMode( side , i ) != 0 ) c++;
		//
	}
	//
	Sch_SUPPLY_ORDERMODE_OFFSET[side] = Sch_SUPPLY_ID_DATA;
	//
	if ( c > 0 ) {
		Sch_SUPPLY_ID_DATA = Sch_SUPPLY_ID_DATA + MAX_CASSETTE_SLOT_SIZE + c;
	}
	//
}
//

int _i_SCH_PRM_GET_MODULE_GROUP( int ch );

void _i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( int side , int SchPointer , int ch ) { // 2013.05.06
	int data;
	//
	data = ( Sch_PRCS_ID_DATA * 1000 );
	//
	if ( data < 0 ) {
		Sch_PRCS_ID_DATA = 0;
		data = 0;
	}
	//
	_i_SCH_CLUSTER_Set_Ex_PrcsID( side , SchPointer , data + ( ch * 10 ) + ( _i_SCH_PRM_GET_MODULE_GROUP(ch) + 1 ) );
	//
	Sch_PRCS_ID_DATA++;
	//
}

//==========================================================================================================
//==========================================================================================================
//==========================================================================================================
//==========================================================================================================





