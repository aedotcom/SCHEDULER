#ifndef SCH_SUB_H
#define SCH_SUB_H

#include "lottype.h"

BOOL _SCH_SUB_NOCHECK_RUNNING( int side ); // 2017.10.10

int SCHEDULER_GET_RECIPE_LOCKING( int side ); // 2014.02.08

int BUTTON_SET_FLOW_MTLOUT_STATUS( int side , int cm , BOOL , int data ); // 2013.06.08

//--
int  Scheduler_Main_Waiting( int CHECKING_SIDE , int PreChecking  , int *pralskip , int lc , int *result );
//--
int  Scheduler_Main_Running_Sub( int CHECKING_SIDE , int TMATM );
//--

// 2016.12.09
int _SCH_Get_SIM_TIME1( int side , int ch );
int _SCH_Get_SIM_TIME2( int side , int ch );
int _SCH_Get_SIM_TIME3( int side , int ch );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Check Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( int Chamber , int HW , int Check , int abflag , int multipm ); // 2003.05.26 // 2003.08.09
void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( int Chamber , BOOL PMMode , int NextEnable , int abflag , int multipm ); // 2003.05.26
//void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( int Chamber , int multipm , BOOL wfrcls ); // 2006.09.06 // 2017.02.17
void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( int Chamber , int multipm , BOOL wfrcls , BOOL force ); // 2006.09.06 // 2017.02.17
BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( int side , BOOL , int * ); // 2011.06.13

void SCHEDULER_CONTROL_Data_Initialize_Part( int , int );
//
void SCHEDULER_CONTROL_Switch_Scheduler_Data( int side );
int  SCHEDULER_CONTROL_Check_Scheduler_Data_for_Run( int side , int RunAll , int ss , int es , int CPJOB , int ch1 , int ch2 );
int  SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( int side , int ch );

void SCHEDULER_CONTROL_MapData_Make_Verification_Info( int side , int cass1 , int cass2 , CASSETTEVerTemplate *MAPPING_INFO , int mapbuf );
void SCHEDULER_CONTROL_RunData_Make_Verification_Info( int side , int id , CLUSTERStepTemplate * , CLUSTERStepTemplate4 * );
void SCHEDULER_CONTROL_RunData_Make_Verification_Info2( int side , int id , CLUSTERStepTemplate2 * , CLUSTERStepTemplate4 * );


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Set_Change_Disappear_Status( int side , int pointer ); // 2006.03.03
//
int SCHEDULING_CHECK_Change_DataBase_PM_Enable_other_Disable( int CHECKING_SIDE , char *slotmodestring , char *ordermodestring , int pm , char *pmstr , int ReadyNotCheck ); // 2005.11.25
//
int  SCHEDULING_CHECK_Increase_for_FM( int side , int trgslot , int *pointer ); // 2003.10.07

int  SCHEDULER_CONTROL_PM_Next_Place_Impossible( int ch ); // 2007.03.12
//

BOOL SCH_Inside_ThisIs_BM_OtherChamber( int ch , int mode ); // 2009.01.21
int  SCH_Inside_ThisIs_BM_OtherChamberD( int ch , int mode ); // 2009.09.23

int  SCH_Inside_ThisIs_DummyMethod( int ch , int mode , int wh , int s , int p ); // 2017.09.11

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int  _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( int tms , int side , BOOL EndTh );
int  _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( int tms , int side , BOOL EndTh );

void _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( int side , BOOL EndTh );
void _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( int side , BOOL EndTh );

int  _i_SCH_SUB_PRECHECK_WAITING_FOR_THREAD_TM( int side , int tmside );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  _i_SCH_SUB_Main_End_Check_Finished( int side ); // 2005.03.15
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Check Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL _i_SCH_SUB_Check_ClusterIndex_CPJob_Same( int side , int SchPointer , int side2 , int SchPointer2 );
BOOL _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( int side , int SchPointer , int ch );


//------------------------------------------------------------------------------------------
// Need Interface
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_SUB_Wait_Finish_Complete( int CHECKING_SIDE , int mode ); // 2003.05.22

void _i_SCH_SUB_Reset_Scheduler_Data( int side );

BOOL _i_SCH_SUB_Check_Complete_Return( int side ); // 2004.12.03

BOOL _i_SCH_SUB_Run_Impossible_Condition( int side , int pt , int PlaceChamber ); // 2007.03.20

BOOL _i_SCH_SUB_Simple_Supply_Possible_FDHC( int side ); // 2006.11.30

BOOL _i_SCH_SUB_Simple_Supply_Possible_Sub( int side , BOOL ); // 2006.11.30

BOOL _i_SCH_SUB_Remain_for_CM( int side , BOOL log );
//
BOOL _i_SCH_SUB_Remain_for_FM_FDHC( int side );
BOOL _i_SCH_SUB_Remain_for_FM_Sub( int side , BOOL log );

BOOL _i_SCH_SUB_FM_Current_No_Way_Supply( int side , int pt , int pmc ); // 2008.07.28

BOOL _i_SCH_SUB_FM_Another_No_More_Supply( int side , int pmc , int side2 , int pt2_or_clidx );

BOOL _i_SCH_SUB_FM_Current_No_More_Supply_FDHC( int side , int *pt , int pmc , int *retcm , int side2 , int pt2_or_clidx );

BOOL _i_SCH_SUB_GET_OUT_CM_AND_SLOT( int FM_Side , int FM_Pointer , int FM_Slot , int *FM_CO , int *FM_OSlot ); // 2007.07.11

BOOL _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( int FM_Side , int FM_Pointer , int FM_Slot , int *FM_CO , int *FM_OSlot , BOOL *fail ); // 2011.05.03

void _i_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( int fms , int FM_Slot , int *FM_Slot_Real , int *FM_TSlot_Real );

BOOL _i_SCH_SUB_PLACE_CM_for_ERROROUT( int CHECKING_SIDE1 , int Pointer , int *ch1 , int *sl1 , int CHECKING_SIDE2 , int Pointer2 , int *ch2 , int *sl2 );
//
BOOL _i_SCH_SUB_DISAPPEAR_OPERATION( int group , int bmoption );
//
BOOL _i_SCH_SUB_FMTMSIDE_OPERATION( int side , int where ); // 2009.05.06
//
int  _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( int Result , int mode ); // 2007.07.25

BOOL _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( int Finger , int Slot );

BOOL _i_SCH_SUB_START_PARALLEL_SKIP_CHECK( int runside , int checkside , int ranpos , BOOL precheck ); // 2003.02.05

int  _i_SCH_SUB_RECIPE_READ_OPERATION( int side );

int  _i_SCH_SUB_RECIPE_READ_CLUSTER( int side , int pt , int mdlcheck , int incm , int slot , char *clusterRecipefile ); // 2010.11.09

int  _i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK( int ch , int sub ); // 2015.06.11

int  _i_SCH_SUB_REMAIN_FORCE_IN_DELETE_FDHC( int side , int cm ); // 2016.07.15


#endif

