#ifndef FILE_READ_INIT_H
#define FILE_READ_INIT_H


//-------------------------------------------------------------
char *SCH_Get_String_Table( int index ); // 2004.12.15
//-------------------------------------------------------------
BOOL FILE_MESSAGE_SETUP( char *Filename ); // 2004.12.05

int  FILE_SYSTEM_SETUP_FOR_GET_ALG( char *Filename );

BOOL FILE_SYSTEM_SETUP( char *Filename );

BOOL FILE_SCHEDULE_SETUP( char *Filename );

BOOL FILE_FILE_SETUP( char *Filename );

BOOL FILE_ROBOT_SETUP( char *Filename );
BOOL FILE_ROBOT2_SETUP( char *Filename );

//BOOL FILE_SCHEDULE_SETUP_SAVE( char *Filename ); // 2017.02.09
BOOL FILE_SCHEDULE_SETUP_SAVE( char *Filename , BOOL all ); // 2017.02.09

BOOL FILE_ROBOT_SETUP_SAVE( char *Filename );

BOOL FILE_PARAM_SM_SETUP(); // 2013.08.22

void FILE_PARAM_SM_SCHEDULER_SETTING2( int mode , int id ); // 2013.08.22

#endif