#ifndef SCH_A4_SUB_H
#define SCH_A4_SUB_H


extern CRITICAL_SECTION CR_SINGLEBM_STYLE_4;
extern CRITICAL_SECTION CR_MULTI_BM_STYLE_4;

void Scheduler_FEM_Running_for_Style_4( int CHECKING_SIDE );

int Scheduler_Main_Running_TM_1_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_2_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_3_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_4_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_5_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_6_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_7_for_Style_4( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_8_for_Style_4( int CHECKING_SIDE );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( int ch );
int SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( int ch ); // 2006.01.12

BOOL SCHEDULER_CONTROL_PROCESS_MONITOR_Run_with_Properly_Status( int ch , int exppart , BOOL notruncheck ); // 2006.11.28

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Lot Special Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( int currside , int currpt , int willch , int *sp_old , int *sp_new ); // 2005.07.20
void SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( int currside , int currpt , int willch ); // 2005.07.20
void SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( int data , int willch );
//----------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( int side , int pt , int ch );
void SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( int ch , int data ); // 2006.01.24
int  SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( int mode , int side , int ch );

//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_ARM_WHAT( int tms , int mode );
BOOL SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( int tms ); // 2005.06.28


BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( int ch , BOOL freecheck );
int SCHEDULING_Possible_BM_for_STYLE_4( int side , int bbb , int Slot , int Slot2 , int Pointer , int Pointer2 , int *bmchamber , int *os , int *os2 , BOOL CMFirst , int mdlckeckskip );
BOOL SCHEDULER_CONTROL_Other_BM_Go_Pump_Possible_for_STYLE_4( int bch );

BOOL SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( int side , int pt , int bmc , int cldx_side , int cldx_pt );

BOOL SCHEDULING_More_Supply_Check_for_STYLE_4( int ch , int pmcheck );

BOOL SCHEDULING_Possible_Process_for_STYLE_4( int side , int pointer , int ch );

BOOL SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4( int side , int pointer , int nccheck , int grpcheck ); // 2006.03.10

int SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( int side , int pch , BOOL check );

int SCHEDULER_CONTROL_BM_VENTING_PART_0( int side , BOOL fmside , int bch , int logoffset );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_Side , int *FM_Side2 , int doublemode , BOOL clcheck , int s2 , int p2 );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_CONTROL_BM_PROCESS_MONITOR_DATA_for_STYLE_4( int , int );

BOOL SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( int ch ); // 2005.09.09

int  SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Res_for_STYLE_4( int side , int ch ); // 2005.09.30

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( int ch , int mp , int *n ); // 2007.01.06

BOOL SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4();

int  SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( int ); // 2005.09.09

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( int ch , BOOL ventmode );

int  SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( int nch , BOOL ventmode );

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( int ch , BOOL ventmode );

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( int ch , BOOL ventmode );

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( int ch , BOOL ventmode ); // 2007.05.11

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( int ch , BOOL ventmode );

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_Other_Wait_RealRun_for_STYLE_4( int ch , BOOL ventmode ); // 2005.10.05

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( int ch , BOOL ventmode ); // 2006.01.11

int  SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( int pmc , int *p ); // 2006.01.17

BOOL SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( int s1 , int p1 , int s2 , int p2 ); // 2006.02.18

BOOL SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( int side , int Chamber );

int SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_for_STYLE_4( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber );

int  SCHEDULER_CONTROL_Pre_Force_Flag_End_for_STYLE_4_Part( int CHECKING_SIDE , int Chamber ); // 2006.01.13

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( int ch , int *s1 , int orderoption );

BOOL SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( int ch , int *s1 , int orderoption );

#endif

