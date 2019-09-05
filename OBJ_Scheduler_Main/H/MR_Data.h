#ifndef MRDATA_H
#define MRDATA_H

//
// Option	:	Auto	Manual
// Count	:	0 - ?
//
enum
{
	MR_PRE_0_READY ,
	//
	// When Start
	//
	MR_PRE_1_REQUEST ,				//  -> REQ
	MR_PRE_2_REQUEST_and_WAIT ,		//  -> REQ+Wait
	//
	// After Unload 
	//
	MR_PRE_3_WAIT ,					//  -> Ready
	//
};


void SCHMRDATA_INIT();

BOOL SCH_MR_UNLOAD_LOCK_START( int side ); // 2012.04.01
void SCH_MR_UNLOAD_LOCK_END( int side ); // 2012.04.01
void SCH_MR_UNLOAD_LOCK_SET_PLACE( int side , BOOL ); // 2012.04.01
void SCH_MR_UNLOAD_LOCK_SET_PICK( int side , BOOL ); // 2012.04.01
BOOL SCH_MR_UNLOAD_LOCK_GET_PICKPLACE( int side ); // 2012.04.01
//=============================

void SCHMRDATA_ReSet_For_Move( int tside , int tpt , int side , int pt );

//=============================

BOOL SCHMRDATA_Data_Setting_for_outcm( int cm , int side , int pt , int log );

//=============================

BOOL SCHMRDATA_Data_Setting_for_LoadUnload( int cm , int mode );

//=============================

void SCHMRDATA_Data_Setting_for_PickPlacing( int mode , int side , int pt , int rbm , int arm , int mdl , int slot );

//=============================

void SCHMRDATA_Data_Setting_for_PreStarting( int cm , int side );

void SCHMRDATA_Data_Setting_for_Starting( int side );

void SCHMRDATA_Data_Setting_for_Finishing( int cm );

//=============================

BOOL SCHMRDATA_Data_IN_Unload_Request_After_Place( int side , int cm ); // 2016.10.31
BOOL SCHMRDATA_Data_IN_Unload_Request_After_Pick( int side , int pt , int cm ); // 2016.10.31
BOOL SCHMRDATA_Data_IN_Wait_Reset( int side , int cm ); // 2016.10.31
BOOL SCHMRDATA_Data_IN_Wait_Set( int side , int cm , int Count , BOOL Manual ); // 2016.10.31


#endif

