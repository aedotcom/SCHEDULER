#ifndef SCH_MAIN_H
#define SCH_MAIN_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Scheduler_InfoInit();
//
int  Scheduler_InfoCheck( int CHECKING_SIDE , int FAMODE , int CPJOB , int PreChecking , BOOL );
int  Scheduler_InfoRegCheck( int CHECKING_SIDE , int EI , char *Lot , int SS , int ES , int LC );
int  Scheduler_InfoRegistCheck( int CHECKING_SIDE , int EI , char *Job , char *Lot , char *Mid , int SS , int ES , int LC , char *etc , BOOL QA , int FA );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_Run_Main_Handling_Code( int CHECKING_SIDE );
int Scheduler_Run_Main_Handling_Code_CheckOnly( int data , char *errstr ); // 2004.06.16
//------------------------------------------------------------------------------------------
void RunTime_Value_Init_Before_Readfile();
void RunTime_Value_Init_Before_Readfile2();
BOOL RunTime_Value_Init_After_Readfile();

//int  Scheduler_CONTROL_RECIPE_SETTING( int CHECKING_SIDE , char *RunName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int lc ); // 2014.05.13
int  Scheduler_CONTROL_RECIPE_SETTING( int CHECKING_SIDE , char *RunName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int *lc ); // 2014.05.13
int  Scheduler_CONTROL_RECIPE_SETTING_AFTER_MAPPING( int CHECKING_SIDE , BOOL );
int  Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( int , int , int , int side , int pointer , int ch , int Slot , char *clusterRecipefile , char *grpname );
int  Scheduler_CONTROL_RECIPE_SETTING_FROM_LOT_RECIPE( int UserMode , int side , char *lotRecipefile );
int  Scheduler_CONTROL_CHECK_AFTER_MODE_SETTING( int rs , int side , int mode ); // 2011.05.25
//
int  Scheduler_Main_Running( int CHECKING_SIDE , int TMATM ); // 2002.07.10
BOOL Scheduler_Main_Wait_Finish_Complete( int CHECKING_SIDE , int mode ); // 2003.05.22
void Scheduler_Main_Other_End_Check_Monitor(); // 2002.07.10
void Scheduler_Main_Running_th_Sub( int data ); // 2002.07.10

//===================================================================================================

int Scheduler_Run_Gui_And_Job_Code( int CHECKING_SIDE , BOOL mappingskip , int StartStyle , int movemode ); // 2008.04.23



#endif

