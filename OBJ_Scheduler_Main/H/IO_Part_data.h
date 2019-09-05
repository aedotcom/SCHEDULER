#ifndef IO_PART_DATA_H
#define IO_PART_DATA_H

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
int    IO_ADD_READ_DIGITAL( int address );
void   IO_ADD_WRITE_DIGITAL( int address , int data );
void   IO_ADD_WRITE_DIGITAL_TH( int address , int data );
double IO_ADD_READ_ANALOG( int address );
void   IO_ADD_WRITE_ANALOG( int address , double data );
void   IO_ADD_WRITE_STRING( int address , char *data );
void   IO_ADD_READ_STRING( int address , char *data );
//BOOL   IO_ADD_APPEND_DIGITAL( int address , int count ); // 2011.01.06
int    IO_ADD_READ_DIGITAL_NAME( int *address , LPSTR list , ... );
BOOL   IO_ADD_WRITE_DIGITAL_TH_NAME( int *address , BOOL app , int count , LPSTR list , ... ); // 2011.01.06

//===================================================================================
//===================================================================================

void IO_DATA_INIT_BEFORE_READFILE();
void IO_DATA_INIT_AFTER_READFILE();

//===================================================================================
BOOL WAFER_SOURCE_IO_CHECK( int ch , int depth );
//===================================================================================

BOOL WAFER_STATUS_IN_ALL_CASSETTE( int cass1 , int cass2 , char *String_Read_Buffer );
//---------------------------------------------------------------------------------------
//----------------------------------------------------------------------------
void WAFER_TIMER_SET_FIC( int Slot , int data ); // 2005.12.01
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  WAFER_STATUS_IN_FM( int fms , int Finger );
int  WAFER_STATUS_IN_TM( int ATM , int Finger );
int  WAFER_STATUS_IN_TM2( int ATM , int Finger );
//
void WAFER_STATUS_SET_FM( int fms , int Chamber , int data );
void WAFER_STATUS_SET_TM( int ATM , int Chamber , int data );
void WAFER_STATUS_SET_TM2( int ATM , int Chamber , int data );
//
int  WAFER_SOURCE_IN_FM( int fms , int Finger );
int  WAFER_SOURCE_IN_TM( int ATM , int Finger );
int  WAFER_SOURCE_IN_TM2( int ATM , int Finger );
//
int  WAFER_SOURCE_IN_FM_E( int fms , int Finger ); // 2018.09.05
int  WAFER_SOURCE_IN_TM_E( int ATM , int Finger ); // 2018.09.05
int  WAFER_SOURCE_IN_TM2_E( int ATM , int Finger ); // 2018.09.05
//
void WAFER_SOURCE_SET_FM( int fms , int Chamber , int data ); // 2018.09.05
void WAFER_SOURCE_SET_TM( int ATM , int Chamber , int data ); // 2018.09.05
void WAFER_SOURCE_SET_TM2( int ATM , int Chamber , int data ); // 2018.09.05
//
void WAFER_SOURCE_SET_FM_E( int fms , int Chamber , int data );
void WAFER_SOURCE_SET_TM_E( int ATM , int Chamber , int data );
void WAFER_SOURCE_SET_TM2_E( int ATM , int Chamber , int data );
//
void WAFER_RESULT_SET_FM( int fms , int Finger , int data );
int  WAFER_RESULT_IN_FM( int fms , int Finger );
//
//void WAFER_RESULT_SET_TM( int ATM , int Finger , int data ); // 2015.05.27
//int  WAFER_RESULT_IN_TM( int ATM , int Finger ); // 2015.05.27

void WAFER_RESULT_SET_TM( int ATM , int Finger , int slot , int data ); // 2015.05.27
int  WAFER_RESULT_IN_TM( int ATM , int Finger , int slot ); // 2015.05.27
//---------------------------------------------------------------------------------------

void WAFER_MSET_CM( int cm , int data );
void WAFER_MSET_BM( int bm , int data );
void WAFER_MSET_PM( int pm , int data );
void WAFER_MSET_TM( int data );
void WAFER_MSET_FM( int data );

void WAFER_MSWAP_CM( int cm1 , int cm2 );

//---------------------------------------------------------------------------------------
void BUTTON_SET_TR_CONTROL( int data );
int  BUTTON_GET_TR_CONTROL();
//----------------------------------------------------------------------------
void BUTTON_SET_HANDOFF_CONTROL( int Chamber , int data );
int  BUTTON_GET_HANDOFF_CONTROL( int Chamber );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BUTTON_SET_FLOW_STATUS( int index , int data );
void BUTTON_SET_FLOW_IO_STATUS( int index , int data );
void BUTTON_SET_FLOW_STATUS_EQUAL( int index );
int  BUTTON_GET_FLOW_IO_STATUS( int index ); // 2011.04.14
//---------------------------------------------------------------------------
void BUTTON_SET_FLOW_STATUS_VAR( int index , int data );
int  BUTTON_GET_FLOW_STATUS_VAR( int index );
//----------------------------------------------------------------------------
void ROBOT_SET_ARM_DISPLAY_STYLE( int ATM , int i , int data );
//----------------------------------------------------------------------------
int    ROBOT_GET_SYNCH_STATUS( int ATM );

void   ROBOT_SET_SYNCH_STATUS( int ATM , int data );

void   ROBOT_SET_EXTEND_POSITION( int ATM , int md , double data );
double ROBOT_GET_EXTEND_POSITION( int ATM , int md );

void   ROBOT_SET_ROTATE_POSITION( int ATM , double data );
double ROBOT_GET_ROTATE_POSITION( int ATM );

void   ROBOT_SET_UPDOWN_POSITION( int ATM , double data );
double ROBOT_GET_UPDOWN_POSITION( int ATM );

void   ROBOT_SET_MOVE_POSITION( int ATM , double data );
double ROBOT_GET_MOVE_POSITION( int ATM );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void   ROBOT_FM_SET_ARM_DISPLAY_STYLE( int fms , int i , int data );
//----------------------------------------------------------------------------
int    ROBOT_FM_GET_SYNCH_STATUS( int fms );
void   ROBOT_FM_SET_SYNCH_STATUS( int fms , int data );

void   ROBOT_FM_SET_POSITION( int fms , int mode , double data );
double ROBOT_FM_GET_POSITION( int fms , int mode );
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_SET_JID_NAME( int index , char *data );
void IO_GET_JID_NAME( int index , char *data );
void IO_SET_MID_NAME( int index , char *data );
void IO_GET_MID_NAME( int index , char *data );
void IO_GET_MID_NAME_FROM_READ( int index , char *data );
void IO_SET_MID_NAME_FROM_READ( int index , char *data );
void IO_GET_JID_NAME_FROM_READ( int index , char *data );
void IO_CLEAR_JID_NAME_FROM_READ( int index );
void IO_CLEAR_MID_NAME_FROM_READ( int index );
void IO_GET_NID_NAME_FROM_READ( int index , char *data );
int  IO_GET_PAUSE_STATUS();
void IO_SET_PAUSE_STATUS( int data );
//---------------------------------------------------------------------------------------
//----------------------------------------------------------------------------
void IO_MDL_STATUS_SET( int chamber , int data );
void IO_MDL_SWITCH_SET( int chamber , int data );
void IO_AGV_STATUS_SET( int chamber , int data );
void IO_MAP_STATUS_SET( int chamber , int data );
void IO_HANDOFF_STATUS_SET( int chamber , int data );
void IO_RUNTIME_IN_STATUS_SET( int chamber , int data );
void IO_RUNTIME_OUT_STATUS_SET( int chamber , int data );
void IO_PM_SIDE_SET( int chamber , int data );
void IO_MODULE_SIZE_SET( int chamber , int data );
void IO_SCH_RUNTYPE_SET( int data );
void IO_RUN_STATUS_SET( int data );
void IO_TR_STATUS_SET( int index , int data );
void IO_CPJOB_LOGBASED_SET( int data );
void IO_FM_ARM_STYLE_SET( int data );
void IO_TM_ARM_STYLE_SET( int tms , int data ); // 2014.11.20
void IO_SCH_CARRIER_GROUP_SET( int data ); // 2003.02.05
void IO_SCH_WAFER_SUPPLY_MODE_SET( int data ); // 2003.06.12
void IO_SCH_LOT_SUPPLY_MODE_SET( int data ); // 2007.10.15
void IO_SCH_CPJOB_MODE_SET( int data ); // 2008.09.29
void IO_SCH_CLUSTER_INCLUDE_SET( int data ); // 2005.08.02
void IO_SCH_COOLING_TIME_SET( int data ); // 2005.11.29
void IO_SCH_COOLING_TIME_SET2( int data ); // 2011.06.01
void IO_SCH_BM_SCHEDULING_FACTOR( int data ); // 2015.03.25
void IO_DOUBLE_SIDE_SET( int chamber , int data ); // 2015.05.27

void IO_SDM_DUMMY_LOTFIRST( int ch , int data );
void IO_SDM_DUMMY_LOTEND( int ch , int data );
BOOL IO_SDM_DUMMY_MODE( int pt , int data );
BOOL IO_SDM_DUMMY_COUNT( int pt , int style , int data );
BOOL IO_SDM_DUMMY_RECIPE( int wh , int pt , int style , int id , char *data ); // 2004.11.17
//
//---------------------------------------------------------------------------------------
void IO_WAFER_DATA_SET_TO_FM( int fms , int finger , int ssrc , int ssts , int sres , int ssre ); // 2007.08.10
void IO_WAFER_DATA_SET_TO_BM( int ch , int slot , int ssrc , int sslt , int sres , int ssre ); // 2007.07.25
void IO_WAFER_DATA_SET_TO_TM( int tms , int finger , int sub , int ssrc , int ssts , int sres , int ssre ); // 2007.08.10

void IO_WAFER_DATA_GET_TO_FM( int fms , int finger , int *ssrc , int *ssts , int *ssre ); // 2007.08.10
void IO_WAFER_DATA_GET_TO_BM( int ch , int slot , int *ssrc , int *ssts , int *ssre ); // 2007.08.10
void IO_WAFER_DATA_GET_TO_TM( int tms , int finger , int sub , int *ssrc , int *ssts , int *ssre ); // 2007.08.10

void IO_WAFER_DATA_SET_TO_CHAMBER( int ch , int slot , int ssrc , int sslt , int sres , int ssre ); // 2007.07.25
void IO_WAFER_DATA_GET_TO_CHAMBER( int ch , int slot , int *ssrc , int *ssts , int *ssre ); // 2007.08.10

//BOOL IOINLTKS_FOR_PICK_POSSIBLE( int ch ); // 2010.07.09
//BOOL IOINLTKS_FOR_PLACE_POSSIBLE( int ch ); // 2010.07.09
//BOOL IOINLTKS_FOR_PICK_POSSIBLE( int ch , int bmpos ); // 2010.07.09
//BOOL IOINLTKS_FOR_PLACE_POSSIBLE( int ch , int bmpos ); // 2010.07.09

BOOL _SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( int ch , int bmpos ); // 2010.07.09
BOOL _SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( int ch , int bmpos ); // 2010.07.09
BOOL _SCH_IO_SET_INLTKS_FOR_PICK_POSSIBLE( int ch , int bmpos , int data ); // 2012.11.27
BOOL _SCH_IO_SET_INLTKS_FOR_PLACE_POSSIBLE( int ch , int bmpos , int data ); // 2012.11.27
//---------------------------------------------------------------------------------------
void _SCH_IO_SET_MAIN_BM_MODE( int index , int data ); // 2010.02.19
BOOL _SCH_IO_GET_MAIN_BM_MODE( int index ); // 2010.02.19

void _SCH_IO_SET_MAIN_BUTTON_ONLY( int index , int data );
void _SCH_IO_SET_MAIN_BUTTON_EQUAL( int index );

void _i_SCH_IO_SET_MAIN_BUTTON( int index , int data );
int  _i_SCH_IO_GET_MAIN_BUTTON( int index );

void _SCH_IO_SET_MAIN_BUTTON_MC( int index , int data );
int  _SCH_IO_GET_MAIN_BUTTON_MC( int index );
//===================================================================================
void _i_SCH_IO_SET_END_BUTTON( int index , int data );
//===================================================================================
int  _i_SCH_IO_GET_IN_PATH( int index );
void _i_SCH_IO_SET_IN_PATH( int index , int data );
int  _i_SCH_IO_GET_OUT_PATH( int index );
void _i_SCH_IO_SET_OUT_PATH( int index , int data );
int  _i_SCH_IO_GET_START_SLOT( int index );
void _i_SCH_IO_SET_START_SLOT( int index , int data );
int  _i_SCH_IO_GET_END_SLOT( int index );
void _i_SCH_IO_SET_END_SLOT( int index , int data );
int  _i_SCH_IO_GET_LOOP_COUNT( int index );
void _i_SCH_IO_SET_LOOP_COUNT( int index , int data );
void _i_SCH_IO_GET_RECIPE_FILE( int index , char *data );
void _i_SCH_IO_SET_RECIPE_FILE( int index , char *data );

int  _i_SCH_IO_GET_START_INDEX( int index ); // 2015.10.12
void _i_SCH_IO_SET_START_INDEX( int index , int data ); // 2015.10.12
int  _i_SCH_IO_GET_END_INDEX( int index ); // 2015.10.12
void _i_SCH_IO_SET_END_INDEX( int index , int data ); // 2015.10.12
//===================================================================================
void _i_SCH_IO_SET_RECIPE_PRCS_FILE( int Chamber , char *data );
//===================================================================================
BOOL _i_SCH_IO_GET_WID_NAME_FROM_READ( int index , int mode , char *data );
//===================================================================================
int  _i_SCH_IO_GET_MODULE_STATUS( int Chamber , int Slot );
void _i_SCH_IO_SET_MODULE_STATUS( int Chamber , int Slot , int data );

int  _i_SCH_IO_GET_MODULE_SOURCE( int Chamber , int Slot );
void _i_SCH_IO_SET_MODULE_SOURCE( int Chamber , int Slot , int data );
//
int  _i_SCH_IO_GET_MODULE_SOURCE_E( int Chamber , int Slot ); // 2018.09.05
void _i_SCH_IO_SET_MODULE_SOURCE_E( int Chamber , int Slot , int data ); // 2018.09.05
//
int  _i_SCH_IO_GET_MODULE_RESULT( int Chamber , int Slot );
void _i_SCH_IO_SET_MODULE_RESULT( int Chamber , int Slot , int data );
//===================================================================================
void _SCH_IO_SET_MODULE_FLAG_DATA( int Chamber , int data ); // 2013.03.19
void _SCH_IO_SET_MODULE_FLAG_DATA3( int Chamber , int data , int data2 , int data3 ); // 2016.12.13
//===================================================================================
BOOL _IO_SET_CHAMBER_INLTKS( int mode , int tmatm , int ch , int data ); // 2013.11.21


#endif

