#ifndef SCH_EIL_H
#define SCH_EIL_H


//=======================================================================================================================
//=======================================================================================================================
//=======================================================================================================================


int  _EIL_FMMODE_RUN_CONTROL_TM( int side );
int  _EIL_FMMODE_RUN_CONTROL_FM( int side );

//

void _EIL_FMMODE_EQ_BEGIN_END_RUN( int Chamber , BOOL End , char *appendstring , int Disable );
int  _EIL_FMMODE_EQ_BEGIN_END_STATUS( int Chamber );

//

int  _EIL_FMMODE_MACRO_CHECK_PROCESSING( int ch );
int  _EIL_FMMODE_MACRO_SPAWN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata , char *elsestr );
int  _EIL_FMMODE_MACRO_RUN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata );

BOOL _EIL_FMMODE_MACRO_FM_OPERATION( int fms , int mode , int ch1 , int slot1 , int tslot1 , int side1 , int pointer1 , int ch2 , int slot2 , int tslot2 , int side2 , int pointer2 );

//=======================================================================================================================
//=======================================================================================================================
//=======================================================================================================================


void _EIL_BEFORE_ENTER_CONTROL_TM( int side , int mode );

void _EIL_BEFORE_RUN_CONTROL_TM_1( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_2( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_3( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_4( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_5( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_6( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_7( int side );
void _EIL_BEFORE_RUN_CONTROL_TM_8( int side );

void _EIL_IO_NOTUSE_STATUS(); // 2012.09.14

int _EIL_COMMON_ENTER_CONTROL_FEM( int side , int mode );
int _EIL_COMMON_RUN_CONTROL_FEM( int side );

void INIT_SCHEDULER_EIL_DATA( int apmode , int side );

int _EIL_EVENT_RECEIVED_OPERATION( char *Command , int bm , int slot , char *message , char *eventall );
//
void _EIL_LOG_LOT_PRINTF( int M , int Slot , LPSTR list , ... );


int _i_EIL_METHOD_REMOVE_TO_EIL( int bm , int slot , BOOL );
int _i_EIL_OPERATE_REQUEST_TM_TO_EIL( int bm );
int _i_EIL_METHOD_INSERT_TO_EIL( int bm , int slot , int incm0 , int wafer , char *clusterRecipefile , char *informationfile , BOOL );
int _i_EIL_SIDE_BEGIN( int side , char *jobname , char *midname , char *recipename , char *elsestr );
int _i_EIL_SIDE_END( int side , char *message );
void _i_EIL_SET_FINISH( int side );

char *EIL_Get_SLOT_FOLDER( int side , int pt );
int  EIL_Get_One_Blank_Wafer_Slot( int s , int *rs , BOOL tmside ); // 2011.05.17
void EIL_CHECK_ENABLE_CONTROL( int ch , BOOL pmmode , int mode ); // 2011.07.25
BOOL EIL_Get_READY_RECIPE_NAME( int side , int pt , int ch , char *processname );

#endif

