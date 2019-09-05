#ifndef IO_PART_LOG_H
#define IO_PART_LOG_H

//==============================================================================================================
void LOG_MTL_INIT();
//==============================================================================================================
void LOG_MTL_STATUS( int chamber , char *data );
//==============================================================================================================
void LOG_MTL_SET_DONE_CARR_COUNT( int side , int data );
void LOG_MTL_SET_DONE_WFR_COUNT( int side , int data );
void LOG_MTL_SET_TARGET_WAFER_COUNT( int side , int data );
int  LOG_MTL_GET_DONE_WFR_COUNT( int side );
int  LOG_MTL_GET_DONE_CARR_COUNT( int side );
//==============================================================================================================
void LOG_MTL_ADD_PROCESS_COUNT( int chamber , int count );
void LOG_MTL_ADD_FAIL_COUNT( int chamber , int count );
void LOG_MTL_ADD_CASS_COUNT( int chamber );
//==============================================================================================================
void LOG_MTL_MOVE_DATA_COUNT( int tside , int sside ); // 2012.03.20
//==============================================================================================================
void _i_SCH_IO_MTL_ADD_PICK_COUNT( int chamber , int count );
void _i_SCH_IO_MTL_ADD_PLACE_COUNT( int chamber , BOOL done , int side , int count );
//==============================================================================================================
void _i_SCH_IO_MTL_SAVE();
//==============================================================================================================

#endif
