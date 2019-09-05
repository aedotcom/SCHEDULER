#ifndef SCH_A0_SUB_H
#define SCH_A0_SUB_H


//------------------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_FEM_TM_EX_STYLE_0; // 2006.06.26
extern CRITICAL_SECTION CR_MULTI_GROUP_PLACE_STYLE_0[MAX_CHAMBER]; // 2006.02.08 // 2006.03.27
extern CRITICAL_SECTION CR_MULTI_GROUP_PICK_STYLE_0[MAX_CHAMBER]; // 2006.02.08
extern CRITICAL_SECTION CR_FEM_PICK_SUBAL_STYLE_0; // 2007.10.04
extern CRITICAL_SECTION CR_SINGLEBM_STYLE_0;
extern CRITICAL_SECTION CR_MULTI_GROUP_TMS_STYLE_0[MAX_TM_CHAMBER_DEPTH]; // 2013.03.19

extern CRITICAL_SECTION CR_FEM_TM_DENYCHECK_STYLE_0; // 2014.12.26
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCH_PM_GET_FIRST_SIDE_PT( int ch , int *s , int *p ); // 2014.01.10 [7]
BOOL SCH_PM_IS_EMPTY( int ch ); // 2014.01.10 [14]
BOOL SCH_PM_IS_WILL_EMPTY( int ch , int pickslot ); // 2014.01.10 [3]

BOOL SCH_PM_HAS_PICKING( int ch , int *depth ); // 2014.01.10 [19]
BOOL SCH_PM_IS_PICKING( int ch ); // 2014.01.10 [10]
BOOL SCH_PM_IS_PLACING( int ch ); // 2018.10.20
BOOL SCH_PM_HAS_PLACING_EXPECT( int ch , int s , int p , int d ); // 2018.10.20

BOOL SCH_PM_HAS_ONLY_PICKING( int ch , int *depth ); // 2014.01.10 [14]

BOOL SCH_PM_HAS_ABSENT( int ch , int s , int p , int d , int *depth ); // 2014.01.10 [4]
BOOL SCH_PM_IS_ABSENT( int ch , int s , int p , int d ); // 2014.01.10 [88]
BOOL SCH_PM_IS_ABSENT2( int ch , int s , int p , int d , BOOL *WaitingMultiwafer ); // 2014.01.10 [88]
//
BOOL SCH_PM_IS_FREE( int ch ); // 2014.01.10 [17]
//
BOOL SCH_PM_IS_ALL_PLACED_PROCESSING( int ch , BOOL Normal_After_Place , BOOL defwait , BOOL *multiprcs ); // 2014.01.10 [8]

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Interlock Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_Chk_Last_Finish_Status_TM_0( int side , int SchPointer , int finc );

int  SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( int TMATM , int chamber , int Side , int Pointer , int dop ); // 2003.05.22
BOOL SCHEDULING_CHECK_INTERLOCK_PM_EXT_DENY_FOR_MDL( int TMATM , int chamber , int ext ); // 2004.03.15
int  SCHEDULING_CHECK_INTERLOCK_DENY_PICK_POSSIBLE( int TMATM , int chamber , int Side , int Pointer , int dop ); // 2003.05.22

void SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( int sidex , int ch );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Sub Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_FEM_Running_for_Style_0( int CHECKING_SIDE );

int  Scheduler_Main_Running_TM_1_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_2_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_3_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_4_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_5_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_6_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_7_for_Style_0( int CHECKING_SIDE );
int  Scheduler_Main_Running_TM_8_for_Style_0( int CHECKING_SIDE );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING(); // 2004.08.14
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Check_Process_2Module_Same_Body(); // 2007.10.07
int SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly(); // 2007.11.04
BOOL SCHEDULER_CONTROL_Check_Process_2Module_Skip( int ch ); // 2015.11.10
void SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( int *ch ); // 2007.10.29
void SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( BOOL , int side , int tms , int arm , int *ch , int * ); // 2007.10.29
BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_Pick_PM( int side , int ch , int * ); // 2007.10.29
BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PROCESS( int side , int ch , int * ); // 2007.10.29
BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_CURR_is_OLD( int oldch , int oldtme , int currch , int currtme ); // 2008.01.08
void SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_SEPARATE_MIXING( int tms , int side ); // 2008.01.08


//
//int  SCHEDULING_CHECK_for_Enable_PICK_from_FM( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_Side , int *FM_Side2 , BOOL TM_Double , int Single_Mode , BOOL *cpreject , int *returnpointer ); // 2008.11.01
int  SCHEDULING_CHECK_for_Enable_PICK_from_CM( int TMATM , int CHECKING_SIDE , int *GetChamber , int *NextChamber , int *MovingType , int *MovingOrder , int *MovingFinger , int *ArmIntlks , BOOL Always , BOOL GetOutSkip , BOOL GetPickLast );
//int  SCHEDULING_CHECK_for_Enable_PICK_from_BM( int TMATM , int CHECKING_SIDE , int *GetChamber , int *GetSlot , int *NextChamber , int *MovingType , int *MovingOrder , int MoveOnly , int SwitchMode , int swbm , int *schpt , int *JumpMode , BOOL GetOutSkip , BOOL GetPickLast , BOOL fullcheck ); // 2013.11.12
int  SCHEDULING_CHECK_for_Enable_PICK_from_BM( int TMATM , int CHECKING_SIDE , int ChkGetCh , int *GetChamber , int *GetSlot , int *NextChamber , int *MovingType , int *MovingOrder , int MoveOnly , int SwitchMode , int swbm , int *schpt , int *JumpMode , BOOL GetOutSkip , BOOL GetPickLast , BOOL fullcheck , int sk1 , int sk2 , int sk3 , int sk4 , int sk5 , int skb1 , int skb2 ); // 2013.11.12
int  SCHEDULING_CHECK_for_Enable_PICK_from_PM( int TMATM , int CHECKING_SIDE , int Chamber , int pmslot , int *MovingType , int *MovingOrder , int *next_chamber , BOOL FMMode , BOOL MoveOnly , int *WaitMode , int prcsgap , int remaintime , int swbm , BOOL *EndMode , BOOL *wy , int *finger , int *halfpick , int srcheck );
int  SCHEDULING_CHECK_for_Enable_PLACE_From_TM( int TMATM , int CHECKING_SIDE , int Finger , int *NextChamber , int *NextSlot , int *MovingType , int *MovingOrder , BOOL FMMode , BOOL MoveOnly , int swbm , int *JumpMode , int putfirstch , int *retmode , int putfixch );

int  SCHEDULING_CHECK_for_Enable_PICK_from_PM_MG( int TMATM , int CHECKING_SIDE , int Chamber , int pmslot , int *MovingType , int *MovingOrder , int *next_chamber , BOOL FMMode , BOOL MoveOnly , int *WaitMode , int prcsgap , int remaintime , int swbm , BOOL *EndMode , BOOL *wy , int *finger , int *halfpick , int srcheck , int *prret );
BOOL SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( int TMATM , int mode );

int  SCHEDULING_CHECK_Enable_PICK_from_PM_for_GETPR( int TMATM , BOOL fmmode , BOOL *Running , BOOL *bmpickx , int *bmoutf );
int  SCHEDULING_CHECK_Enable_MOVE_BM_for_GETPR( int TMATM );
int  SCHEDULING_CHECK_Disable_PICK_from_CMBM_OUT_for_GETPR( int TMATM , int side , int pointer , int rbcount ); // 2002.11.04

int  SCHEDULING_CHECK_Enable_PICK_CM_or_PLACE_to_BM_at_SINGLE_MODE( int TMATM , int CHECKING_SIDE , int Slot );
int  SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( int TMATM , int CHECKING_SIDE , int mode , int NextChamber );
//
BOOL SCHEDULING_CHECK_TM_Has_More_Space( int side , int TMATM , int finger , int bmc ); // 2003.11.07
int  SCHEDULING_CHECK_NonTM_Wafer_PM_Next_Place_Switch_Possible( int tms , int getch , int getslot , int s , int p , int trgch , BOOL mainprcspickok ); // 2007.12.12

//
BOOL SCHEDULING_CHECK_TM_All_Free_Status( int ); // 2012.08.30
BOOL SCHEDULING_CHECK_TM_Another_Free_Status( int side , BOOL fmcheck , BOOL Swapping , int bmc );
BOOL SCHEDULING_CHECK_TM_Another_No_More_Supply( int side ); // 2004.11.23
//
BOOL SCHEDULING_CHECK_CM_Another_No_More_Supply( int side , int pt ); // 2005.01.03

int  SCHEDULING_CHECK_FIXED_GROUPPING_FLOWDATA( int side , int pt , int pd , int tmatm ); // 2006.02.11
int  SCHEDULING_CHECK_REMAKE_GROUPPING_FLOWDATA( int tmatm , int arm , int side , int pointer , int ch ); // 2006.01.26
BOOL SCHEDULING_CHECK_REMAKE_GROUPPING_POSSIBLE( int side , int pt , int sind ); // 2006.01.26


void SCHEDULING_CHECK_Chk_BM_HAS_IN_ANOTHER_WAFER_for_DOUBLE( int ch , int side , int s , int *side2 , int *pointer , int *s1 , BOOL swmode , BOOL doubleSlotFixmode , BOOL oddpick , BOOL xcr );
int  SCHEDULING_CHECK_Get_BM_FREE_IN_CHAMBER_FOR_PICK_BM( int side , int TMATM );

int  SCHEDULING_CHECK_for_Check_PLACE_Deadlock( int TMATM , int side );

BOOL SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( int TMATM , int side , int arm , int Next_Chamber ); // 2006.03.27

BOOL SCHEDULER_CONTROL_Next_All_Stock_and_Nothing_to_Go( int TMATM , int Side , int Pointer , int Chamber ); // 2006.04.11

BOOL SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( int TMATM , int Side , int Pointer , int mode ); // 2006.04.11

//BOOL SCHEDULER_CONTROL_Stock_Other_Next_Disable_Before_Pick_PM( int tmatm , int Side , int Pointer ); // 2006.04.15

BOOL SCHEDULING_CHECK_OTHER_GROUP_HAS_CLEAR_POSSIBLE( int TMATM , int Chamber , int s , int p , int n ); // 2006.05.25

void SCHEDULING_CHECK_MULTI_PLACE_GROUP_RESET( int tmside , int ch ); // 2006.06.14

BOOL SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( int TMATM , int Finger );

//void SCHEDULING_CHECK_PRE_END_RUN( int TMATM , int CHECKING_SIDE ); // 2006.07.20 // 2018.11.29

BOOL SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( int ch , BOOL inout ); // 2007.06.28
int  SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( int ch , int *s1 , int *s2 , int orderoption );
int  SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( int whatm , int option ); // 2003.12.10
//int  SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( int ch , int slot0 , int *slot2 ); // 2007.07.11 // 2018.06.18
int  SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( int ch , int slot0 , int order , int *slot2 ); // 2007.07.11 // 2018.06.18
int  SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( int ch , int side , int *pointer , int *s1 , BOOL Switch , int orderoption , int *oc );
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE_POINTER( int ch , int side , int *pointer , int *s1 , BOOL Switch , int orderoption , int *oc );
int  SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( int ch , int side ); // 2002.03.27
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( int ch , int side , int *s1 , int *s2 , int runstyle , int orderoption );
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( int ch , int side , int *s , int *p , int runstyle , int orderoption );
int  SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE( int ch , int side , BOOL Switch );
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK( int ch , int s1 , BOOL dblcheck );
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( int ch , int *s1 , int orderoption );

int SCHEDULER_CONTROL_Chk_Get_BM_Count( int ch , int sts ); // 2018.10.20

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULING_CHECK_PICK_FROM_CM_POSSIBLE_FOR_SWITCHMESSAGE( int side );
int  SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT();
//
BOOL SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( int side , int tmside , int ch , int swmd , int jmpmode , int cntarmminus , BOOL bmonly );

int  SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( int tmside , BOOL , BOOL , int ); // 2007.08.29

int  SCHEDULING_CHECK_Switch_PLACE_From_TM( int TMATM , int CHECKING_SIDE , int Finger , int PickChamber );
//
int  SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( int TMATM , int CHECKING_SIDE , int PickChamber , int PickSlot , int NextChamber , BOOL *switchable ); // 2007.01.15

BOOL SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_0( int side , int *ch , int *slot , int *uc , int skipslot ); // 2007.03.19

BOOL SCHEDULING_ThisIs_BM_OtherChamber( int ch , int mode ); // 2009.01.21

BOOL SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( int mode , int newch , int oldch ); // 2012.09.21
BOOL SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( BOOL check , int newch , int oldch ); // 2014.04.07

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS( int tms , int s , int p , int pathdo , int pick_bm , int mode , BOOL prcscheck ); // 2013.07.13
int  SCHEDULER_CONTROL_DUALTM_for_GET_PROPERLY_ARM_WHEN_PICK_FROM_BM_POSSIBLE( int tms , int s , int p , int pathdo , int pick_bm ); // 2013.07.13
BOOL SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS_CURR_TM( int tms , int s , int p , int pathdo , int pick_bm , BOOL prcscheck ); // 2016.12.13

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULING_CHECK_Chk_OTHERGROUPBM_WAITING_HASRETURN( int side , int tmside , BOOL only , int ch , int swmd ); // 2018.05.08

#endif

