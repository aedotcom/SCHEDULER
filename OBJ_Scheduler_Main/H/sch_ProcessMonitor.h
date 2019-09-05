#ifndef SCH_PRCMON_H
#define SCH_PRCMON_H

//----------------------------------------------------------------------------
void INIT_PROCESS_MONITOR_DATA( int , int );
//----------------------------------------------------------------------------
void PROCESS_MONITOR_Set_MONITORING_TIME( int timedata );
//------------------------------------------------------------------------------------------
void PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( int data );
//void PROCESS_MONITOR_Set_MONITORING_WHERE_LOG_SIDE_AT_MODULE( int data ); // 2005.02.16
//void PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( BOOL data ); // 2019.04.05
void PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( int data ); // 2019.04.05
//BOOL PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB(); // 2019.04.05
int PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB(); // 2019.04.05
//------------------------------------------------------------------------------------------
void PROCESS_MONITOR_Set_Status( int side , int chamber , int status , BOOL append , int pathin );
int  PROCESS_MONITOR_Get_Status( int side , int chamber );
//
BOOL PROCESS_MONITOR_Set_Finish_Status( int side , int chamber , int status ); // 2011.03.02
//
void PROCESS_MONITOR_Set_Last( int side , int chamber , int status );
int  PROCESS_MONITOR_Get_Last( int side , int chamber );

void PROCESS_MAKE_SLOT_FOLDER( int side , int pt , char *data ); // 2012.02.18

void PROCESS_MONITOR_Set_Pointer( int side , int chamber , int pointer , int no , int no2 , int chp , int chs );
int  PROCESS_MONITOR_Get_Pointer( int side , int chamber );
int  PROCESS_MONITOR_Get_Pointer1( int side , int chamber );
int  PROCESS_MONITOR_Get_Pointer2( int side , int chamber );

void PROCESS_MONITOR_Set_Recipe( int side , int chamber , char *recipe );
char *PROCESS_MONITOR_Get_Recipe( int side , int chamber );

int  PROCESS_MONITOR_Run_and_Get_Status( int chamber );
int  PROCESS_MONITOR_Reset_Status( int chamber ); // 2018.10.31

BOOL PROCESS_MONITOR_Get_Remain_Predict_Time( int ch , int wafer , int *tdata , int willch ); // 2006.02.10

//void PROCESS_MONITOR_Setting1( int ch , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , BOOL append ); // 2006.01.12 // 2014.02.13
void PROCESS_MONITOR_Setting1( int ch , int chp , int chs , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , BOOL append , BOOL wfrnotcheck ); // 2006.01.12 // 2014.02.13
void PROCESS_MONITOR_Setting2( int ch , int chp , int chs , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , int side2 , int pointer2 , BOOL wfrnotcheck ); // 2007.04.19
void PROCESS_MONITOR_Setting2_One( int ch , int chp , int chs , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , int side2 , int pointer2 , BOOL wfrnotcheck ); // 2009.09.24
//void PROCESS_MONITOR_SettingM( int ch , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , int firstnotuse ); // 2007.05.02 // 2014.02.13
void PROCESS_MONITOR_SettingM( int ch , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , int firstnotuse , BOOL wfrnotcheck ); // 2007.05.02 // 2014.02.13
void PROCESS_MONITOR_Setting_with_WfrNotCheck( int ch , int chp , int chs , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last ); // 2007.10.11
void PROCESS_MONITOR_Setting_with_Dummy( int ch , int status , int side , int pointer , int incm , int no1 , int no2 , char *recipe , int last , BOOL wfrnotcheck , int dummyonly ); // 2006.01.12
BOOL PROCESS_MONITOR_SPAWN_APPENDING( int side , int pt , int ch , int pathin , int wid , int arm , char *recipe , int mode , int puttime , char *NextPM , int MinTime , int index , int status , int no1 , int no2 , int last , BOOL append ); // 2011.03.02
BOOL PROCESS_MONITOR_HAS_APPENDING( int ch ); // 2017.09.19

int PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( int chamber );
char *PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( int chamber ); // 2007.02.22
void PROCESS_MONITOR_Reset_Last_ProcessJob_Data( int chamber ); // 2011.07.18
BOOL PROCESS_MONITOR_Set_Last_ProcessJob_Data( int chamber , int side , int pointer ); // 2011.07.18
BOOL PROCESS_MONITOR_Set_Last_ProcessJob_PPID( int chamber , int side , char *ppid ); // 2011.07.18

BOOL PROCESS_MONITOR_Last_And_Current_CPName_is_Different( int side , int pointer , int chamber ); // 2007.02.22
//BOOL PROCESS_MONITOR_SameSide_and_CPName_is_Different( int side1 , int pointer1 , int side2 , int pointer2 ); // 2007.05.28 // 2014.02.03
BOOL PROCESS_MONITOR_CPName_is_Different( int side1 , int pointer1 , int side2 , int pointer2 ); // 2007.05.28 // 2014.02.03

void INIT_PROCESS_INTERFACE_MODULE_DATA( int , int );

int  PROCESS_INTERFACE_MODULE_GET_TIME( int ch ); // 2018.06.07
BOOL PROCESS_INTERFACE_MODULE_GET_INFO( int ch , int index , int *s , int *m , int *p , int *w );
void PROCESS_INTERFACE_MODULE_APPEND_DATA( int ch , int side , int pt , int w );
void PROCESS_INTERFACE_MODULE_CLEAR_INFO( int side , int ch );

void PROCESS_MONITOR_LOG_READY( int side , int Ch , int Option );
void PROCESS_MONITOR_LOG_END( int side , int Ch , BOOL OnlyOne );
void PROCESS_MONITOR_LOG( int ch );

void PROCESS_MONITOR_LOG_READY_for_PRJOB( char *CPJobName , char *PRJobName );
void PROCESS_MONITOR_LOG_END_for_PRJOB( int id , char *CPJobName , char *PRJobName );

void PROCESS_MONITOR_STATUS();
void PROCESS_MONITOR_STATUS_Abort_Signal_Set( int side , BOOL Always );
void PROCESS_MONITOR_STATUS_Abort_Signal_Reset( int side );

BOOL PROCESS_MONITOR_STATUS_for_Abort_Remain( int side ); // 2003.12.01

void PROCESS_PRJOB_End_Set_Delay_Time( int timedata ); // 2005.04.06

void PROCESS_DATALOG_Init_BUFFER_SIZE(); // 2007.10.23
void PROCESS_DATALOG_Set_BUFFER_SIZE( int ch , int data ); // 2007.10.23
int  PROCESS_DATALOG_Get_BUFFER_SIZE( int ch ); // 2007.10.23


#endif

