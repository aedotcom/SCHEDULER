#ifndef SCH_A6_SUB_H
#define SCH_A6_SUB_H

void Scheduler_FEM_Running_for_Style_6( int CHECKING_SIDE );

int Scheduler_Main_Running_TM_1_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_2_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_3_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_4_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_5_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_6_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_7_for_Style_6( int CHECKING_SIDE );
int Scheduler_Main_Running_TM_8_for_Style_6( int CHECKING_SIDE );

int Scheduler_TM_Running_CYCLON_0_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_1_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_2_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_3_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_4_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_5_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_6_for_Style_6( int CHECKING_SIDE );
int Scheduler_TM_Running_CYCLON_7_for_Style_6( int CHECKING_SIDE );

int SIZE_OF_CYCLON_PM_DEPTH( int ch ); // 2014.11.14

BOOL SCHEDULING_ThisIs_BM_OtherChamber6( int ch , int mode ); // 2014.07.08

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
int SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_6( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_Mode , int *next_FM_sDoPointer , int *next_FM_DoPointer , int *next_BM , BOOL BOnly , BOOL *forcediv , BOOL *hasinvalid );
int SCHEDULING_CHECK_Enable_PICK_from_FM_ArmBOnly_for_STYLE_6( int TMATM , int side , int *next_BM , int *FM_Slot , int *FM_Pointer , int *next_FM_DoPointer , int * );

int SCHEDULING_CHECK_Enable_PICK_from_FM_Properly_Group_for_STYLE_6( int TMATM , int side , int pt , int bm , int clidx ); // 2005.09.14

int  SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_FOR_6( int TMATM , int CHECKING_SIDE , int Finger , int PickChamber );
int  SCHEDULING_CHECK_Switch_PLACE_From_Fixed_TM_and_OtherDisable_FOR_6( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber ); // 2005.08.17
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6(); // 2007.04.30

BOOL SCHEDULER_CONTROL_Chk_TM_PICKPLACE_DENY_FOR_6( int tms );
BOOL SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( int tms , BOOL zeroreturn );
BOOL SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( int bmc );
BOOL SCHEDULER_CONTROL_Chk_NEED_SEPARATE_PLACE_FOR_6( int bmc );
void SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( int mode , int bmc , int *ss , int *es );
void SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( int mode , int bmc , int *ss , int *es ); // 2014.12.24
int  SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( int mode , int bmc );
BOOL SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( int bmc , int pmc );
//int  SCHEDULER_CONTROL_Chk_FM_BMOUT_DIRECT_RETURN_FOR_6(); // 2014.11.13
int  SCHEDULER_CONTROL_Chk_FM_BMOUT_DIRECT_RETURN_FOR_6( int ); // 2014.11.13

int  SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( int side , int bmch , int *slotstart , int *slotend );
int  SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_CYCLON_FOR_6( int side , int bmch , int *slotstart , int *slotend , int *c , int *s );
BOOL SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( int bmch , int side , int pt , int offset );
BOOL SCHEDULER_CONTROL_Chk_Will_Do_PM_POST_FOR_6( int side , int tms , int bmch , int slot );
BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_PM_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( int bmch ); // 2010.07.27
BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPMBMIN_FOR_6( int bmch , BOOL dmchk );
BOOL SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_Slot1_getout_wafer_from_BM_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( int bmch );
int  SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( int bmch , int *fs , int *fs2 , BOOL );
BOOL SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_TM_ArmCheck_FOR_6( int bmc , int s1 , int p1 , int s2 , int p2 );
//int  SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( int side , int bmch , int skipch , int skipslot , BOOL sdcheck ); // 2014.08.22
int  SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( int *side , int bmch , int skipch , int skipslot , BOOL sdcheck ); // 2014.08.22
BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_OtherSlot_first_FOR_6( int tmside , int bmch , int slot ); // 2014.06.18
BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( int side , int tms , int bmch , BOOL SWCheck );
BOOL SCHEDULER_CONTROL_Chk_BM_has_just_one_for_supply_FOR_6( int side , int bmch );
BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_one_out_full_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( int bmch , BOOL chk , BOOL , BOOL );
int  SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_Other_CM_FOR_6( int bmch , int *prerunside ); // 2015.12.01
BOOL SCHEDULER_CONTROL_Chk_FMs_Target_BM_is_Yes_FOR_6( int bmch );
BOOL SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_free_FOR_6( int bmch , int slot , int *pm , BOOL *dissts );
//int  SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( int tmside , int bmch , int barm , int S , int P , int *arm ); // 2007.07.27 // 2014.07.18
int  SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( int tmside , int barm , int S , int P , int *arm ); // 2007.07.27 // 2014.07.18
BOOL SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_ArmIntlks_OK_FOR_6( int tmside , int bmch , int slot , int *arm ); // 2007.07.27
BOOL SCHEDULER_CONTROL_Chk_TMs_Target_PM_is_free_FOR_6( int tms , int pm );
BOOL SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( int tch , int pch , int S , int P );
BOOL SCHEDULER_CONTROL_Chk_Next_PM_Go_Impossible_FOR_6( int tch , int ch , int armcnt , int dis );
int  SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( int side , int tmatm , int bm , int *slot , int *ret , BOOL slotonly );
int  SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm2_FOR_6( int *side , int tmatm , int bm , int *slot , int *ret , BOOL slotonly ); // 2015.02.13
BOOL SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( int bmch , BOOL runcheck );
int  SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( int side , int bmmodex , int Slotmode , int intlockmode , int *rbm , int *Justoneuse , BOOL runcheck );
int  SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_FOR_6( int side , int bm , int Slotmode , int *slot2 );
//
// 2014.07.10
//BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( int side , int tmside );
//BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( int side , int tmside );
//BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( int side , int tmside );
BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( int side , int tmside , int bmc );
BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( int side , int tmside , int bmc );
BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( int side , int tmside , int bmc );
//
BOOL SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_MIC_FOR_6( int , int , int , int , int , int ); // 2007.11.06
BOOL SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_Lock_FOR_6( int tmatm , BOOL *alltmout ); // 2013.10.30

BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_curr_out_last_one_FOR_6( int side , int bmch , BOOL * ); // 2007.10.02 // 2013.10.30
BOOL SCHEDULER_CONTROL_Chk_BM_has_all_Out_FOR_6( int bmch ); // 2013.11.15

int SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Cyclon_FOR_6( int bmc , int mode , int s1 , int p1 , int w1 , int s2 , int p2 , int w2 ); // 2007.05.01

BOOL SCHEDULER_CONTROL_Chk_Place_BM_Force_Waiting_FOR_6( int bmc , int side , int p1 , int w1 , int p2 , int w2 , BOOL *PickOtherWait ); // 2013.11.27

int SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( int bmch , BOOL cycl ); // 2008.01.16

BOOL SCHEDULER_CONTROL_Chk_BM_TM_PM_HAS_WAFER_CYCLON_STYLE_6( int bmc , BOOL all );

int SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( int ch , int *pointer , int *s1 , int *realside );

int SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( int tms , int ch , int *s1 , BOOL fmsidecheck );

BOOL SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_6( int side , int *ch , int *slot , int *uc , int skipslot ); // 2007.03.19

BOOL SCHEDULING_CHECK_BM_HAS_OUT_SUPPLYID( int ch , int ch2 ); // 2014.10.03

#endif

