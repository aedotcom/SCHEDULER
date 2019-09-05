#ifndef SCH_A0_SUB_F_SW_H
#define SCH_A0_SUB_F_SW_H


BOOL Scheduler_Main_Waiting_for_Switch( int CHECKING_SIDE );
void Scheduler_FEM_Running_for_Switch( int CHECKING_SIDE );

BOOL Scheduler_Switch_Side_Check_for_TM_Place( int side , int ch );

BOOL SCHEDULER_CONTROL_Chk_BM_HAS_OUT_CHECK_SWITCH( int ch , int mode ); // 2006.09.26
BOOL SCHEDULER_CONTROL_Chk_BM_HAS_OUT_WAFER( int mode ); // 2014.01.27

int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( int ch , int side , int mode , int *s1 , int orderoption , int *ips , int *ipt , int tmw );
int  SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_SINGLESWAP( int ch , int side , int *s1 , int orderoption , int *ips , int *ipt , BOOL dblcheck );

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( int side , BOOL All , int ch , BOOL middleswap ); // 2004.03.22

int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCHALL( int side , int ch , int cnt , BOOL mdncheck ); // 2007.01.24
int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( int side , int ch , BOOL middleswap , BOOL dblchk , int tmside , int otherarm ); // 2004.03.22
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP2( int side , int ch , BOOL *k );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( int side , int mode , BOOL All , int ch );
int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP1( int side , int ch ); // 2004.11.29
int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP2( int side , int ch , BOOL pickmode , int tmside );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( int side , int ch , BOOL *k );

void SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_RETURN_BATCH_ALL( int side , int ch ); // 2006.08.02
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_OK_OUT_BATCH_ALL( int side , int ch ); // 2006.08.02

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( int ch , BOOL middleswap , BOOL outcheck );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( int side , int ch , BOOL middleswap , BOOL freeonly );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( int side , int ch , BOOL middleswap ); // 2013.10.01
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_SINGLSWAP( int side , int ch , int *k );
int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( int ch , int md );
//BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( int ch , int oldch ); // 2018.08.22
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( int ch , int oldch , BOOL othercheck ); // 2018.08.22
int  SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( int ch , int mode , int *rs , int *rp , BOOL midswap ); // 2006.12.22
BOOL SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_MIDDLE_SWAP( int bmc ); // 2008.03.31
BOOL SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_SINGLE_SWAP( int bmc ); // 2011.04.28

void SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( int bmc , BOOL inmode , int *sc , int *lc ); // 2006.02.22

BOOL SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( int side , int *ch , int addcnt , int willbm , int ); // 2013.10.01

BOOL SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( int side , int bmc , int mode ); // 2004.03.10
BOOL SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( int side , int bmc ); // 2004.11.29
//int  SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( int side , int bmc , int nextmode ); // 2004.06.15 // 2016.10.20
int  SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( int side , int tmside , int bmc , int nextmode ); // 2004.06.15 // 2016.10.20
void SCHEDULING_CHECK_BM_Change_FMSIDE_for_Switch( int tmside , int side ); // 2004.10.15

void SCHEDULING_SET_FEM_Pick_Deny_PM_LOCK_DENY_CONTROL( int );
BOOL SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( int supplyside , int pointer , BOOL check ); // 2004.11.05
void SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( int side , int TMATM , int bmc ); // 2004.11.26
BOOL SCHEDULING_CHECK_AllBM_Pump_and_hasPlaceSpace_for_Switch_SingleSwap(); // 2004.12.06
void SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Pump_for_Switch_SingleSwap1( int side , int bmc ); // 2017.02.08

int  SCHEDULING_CHECK_BM_HAS_DualExt_for_Switch( int ch , int side , int slot , int dualext , int armb , int *res );

int  SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( int ch , int *s1 , int *s2 , int orderoption , BOOL dblchk ); // 2006.02.22
int  SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( int ch , int *s , int *p , int *w , int *fs , int orderoption ); // 2007.05.03
int  SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( int ch , int *s1 , int *s2 , int orderoption ); // 2006.02.22

int  SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( int ch , int *s1 , int *s2 , int orderoption , BOOL swap ); // 2017.07.11

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( int ch );
BOOL SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( int ch );
BOOL SCHEDULER_CONTROL_Chk_BM_OUT_IS_LAST_FOR_MIDDLESWAP( int ch );
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_OUT_MIDDLESWAP( int tms , int ch );
int  SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( int side , int *ch , int *ch2 , int beforebm , int mode , BOOL allcheck );
BOOL SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( int side , BOOL , int *rFM_Buffer , int mode ); // 2007.09.11
BOOL SCHEDULER_CONTROL_Chk_BM_CROSS_OUT_MIDDLESWAP_ONLY_PLACE_PICK_PLACE_NOT_PICK( int ch , int ins1 , int ins2 , int *outs1 , int *outs2 );
BOOL SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( int bmc , BOOL ); // 2014.03.06
BOOL SCHEDULER_CONTROL_Chk_AL2_FREE(); // 2014.05.14

BOOL SCHEDULER_CONTROL_Chk_BM_IN_HAS_SPACE_FULL_SWITCH( int ch , int mode ); // 2014.01.23

BOOL SCHEDULING_CHECK_TM_GetOut_BM_Conflict_Check_for_Switch( int side ); // 2006.05.24

BOOL Scheduler_Separate_Check_for_Batch_All( int side ); // 2007.01.23


BOOL Scheduler_Supply_Waiting_for_Batch_All( int side , BOOL mode , int bch );
BOOL Scheduler_FEM_Other_GetOut_Possible_SingleSwap( int side , int bmc , int *cside );
BOOL Scheduler_FEM_Will_Out_Check_for_Switch( int side , int bm );
BOOL Scheduler_FEM_GetIn_Properly_Check_for_Switch( int side , int ch );
BOOL Scheduler_FEM_GetOut_Properly_Check_for_Switch( int side , int ch );


int Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch( int side , int *sider , int *sl , int *pt , int check_side , int check_pt );

BOOL SCHEDULER_CONTROL_Chk_SWAP_PRE_PICK_POSSIBLE_in_MIDDLESWAP( int bmd );

int SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( int mode , int Side , int Pointer , int OtherYesBM );

int SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( int side , int bmc ); // 2007.05.21 // 2014.06.10

BOOL SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( int side , int bmc ); // 2007.05.03

BOOL SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( int tms , int bmch , BOOL outonly );

int SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( int cmpick , int bmch , int s1 , int p1 , int w1 , int s2 , int p2 , int w2 , int *ps1 , int *ps2 , BOOL nextcross );

int SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( int CHECKING_SIDE , int bmch1 , int s1 , int p1 , int w1 , int bmch2 , int s2 , int p2 , int w2 , int *log ); // 2007.05.01

void SCHEDULER_CONTROL_INIT_SWITCH_PART_DATA( int , int );

#endif

