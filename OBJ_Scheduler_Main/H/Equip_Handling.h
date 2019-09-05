#ifndef EQUIP_HANDLING_H
#define EQUIP_HANDLING_H

//================================================================================
extern int _EQUIP_RUNNING_TAG; // 2008.04.17
//extern int  _EQUIP_RUNNING2_TAG[MAX_CHAMBER]; // 2012.10.31
//================================================================================
extern int Address_FUNCTION_PROCESS[MAX_CASSETTE_SIDE][MAX_CHAMBER];
extern int Address_FUNCTION_INTERFACE[MAX_CHAMBER];
extern int Address_FUNCTION_INTERFACE_FAL;
extern int Address_FUNCTION_INTERFACE_FIC; // 2005.09.06
extern int Address_FUNCTION_INTERFACE_FM2; // 2007.06.20
extern int Address_FUNCTION_INTERFACE_SUB_CALL_END[MAX_CHAMBER]; // 2006.10.24
//================================================================================
//BOOL EQUIP_RUNNING_CHECK( long timeover ); // 2008.04.18 // 2018.09.04
BOOL EQUIP_RUNNING_CHECK( long timeover , BOOL *notRun ); // 2008.04.18 // 2018.09.04
void EQUIP_RUNNING_STRING_INFO( BOOL debugrun , char *data ); // 2008.04.18

//================================================================================

BOOL EQUIP_MESAGE_NOTUSE_GATE( BOOL MaintInfUse , int tm , int ch ); // 2013.04.26
BOOL EQUIP_MESAGE_NOTUSE_PREPRECV( BOOL MaintInfUse , int tm , int ch ); // 2013.04.26
BOOL EQUIP_MESAGE_NOTUSE_PREPSEND( BOOL MaintInfUse , int tm , int ch ); // 2013.04.26
BOOL EQUIP_MESAGE_NOTUSE_POSTRECV( BOOL MaintInfUse , int tm , int ch ); // 2013.04.26
BOOL EQUIP_MESAGE_NOTUSE_POSTSEND( BOOL MaintInfUse , int tm , int ch ); // 2013.04.26

//================================================================================
void EQUIP_INIT_BEFORE_READFILE();
void EQUIP_INIT_AFTER_READFILE();

int  EQUIP_ADDRESS_FM( int fms ); // 2007.07.24
int  EQUIP_FM_GET_SYNCH_DATA( int fms );
BOOL EQUIP_INTERFACE_INVALID_STATION( int Chamber );

BOOL EQUIP_INTERFACE_FUNCTION_CONNECT( int Chamber );
BOOL EQUIP_PROCESS_FUNCTION_CONNECT( int Chamber );

void EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( int Side , char *data );
void EQUIP_INTERFACE_STRING_APPEND_TRANSFER( int Side , char *data );
void EQUIP_INTERFACE_STRING_APPEND_MESSAGE( BOOL MaintInfUse , char *MsgAppchar ); // 2013.04.26

int  EQUIP_STATUS_PROCESS_FOR_LOG( int Side , int Chamber );
int  EQUIP_READ_FUNCTION_FOR_LOG( int Chamber );

int  EQUIP_READ_FUNCTION( int Chamber );
int  EQUIP_WAIT_FUNCTION( int Chamber );
//
int  EQUIP_RUN_FUNCTION( int Chamber , char *data );
void EQUIP_RUN_FUNCTION_ABORT( int Chamber );
BOOL EQUIP_RUN_SET_FUNCTION( int Chamber , char *data );

int  EQUIP_PICK_FROM_CHAMBER_0( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_0( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_1( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_1( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_2( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_2( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_3( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_3( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_4( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_4( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_5( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_5( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_6( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_6( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_7( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_7( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PICK_FROM_CHAMBER_8( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
int  EQUIP_PLACE_TO_CHAMBER_8( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror );
//
int  EQUIP_PICK_FROM_CHAMBER_TT( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order );
int  EQUIP_PLACE_TO_CHAMBER_TT( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order );
//
int  EQUIP_PICK_FROM_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual );
int  EQUIP_PLACE_TO_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual );
//
// 2013.01.11
int  EQUIP_SWAP_CHAMBER_0( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_1( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_2( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_3( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_4( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_5( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_6( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_7( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
int  EQUIP_SWAP_CHAMBER_8( int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse );
//
int  EQUIP_SWAP_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , int forcealigncool , int PrepPartSkip , int order , BOOL pmdual );

int  EQUIP_STATUS_PROCESS( int Side , int Chamber );
int  EQUIP_STATUS_PROCESS_OF_OPTION( int Side , int Chamber , int *failout , int *finishout , int *disappear , int *fail_scenario , int *success_result , int *fail_result , int *abort_result , int *pre_time , int *prcs_time , int *post_time );
void EQUIP_RUN_PROCESS_ABORT( int Side , int Chamber );
int  EQUIP_PROCESS_ITEM_CHANGE_EVENT( int Side , int Chamber , int Slot , int Step , char *Item , char *Data , char *Else );
int  EQUIP_WAIT_PROCESS( int Side , int Chamber );
//
BOOL EQUIP_MAIN_PROCESS_SKIP_RUN( int Chamber ); // 2006.01.06
BOOL EQUIP_POST_PROCESS_SKIP_RUN( int Chamber ); // 2006.01.06
BOOL EQUIP_POST_PROCESS_FORCE_RUN( int Chamber ); // 2006.01.06

int  EQUIP_BEGIN_END_STATUS( int Chamber );
void EQUIP_BM_END_RUN_SUB_CALL( int Chamber , int Disable ); // 2006.10.24
//
int  EQUIP_CARRIER_HANDOFF( BOOL InMode , int Chamber1 , int Chamber2 );
int  EQUIP_CARRIER_MAPPING( int side , BOOL msgoff , BOOL FAMode , BOOL InMode , BOOL SkipCheck , int Chamber1 , int Chamber2 , BOOL Ch1Run , BOOL Ch2Run , int StartSlot , int EndSlot , BOOL Manual , int * );
int  EQUIP_CARRIER_HANDOFF_OUT( int Chamber );
//
int  EQUIP_ROBOT_FM_MOVE_CONTROL( int fms , int side , int Mode , int Chamber , int Slot , int Slot2 , BOOL MaintInfUse );
//
int  EQUIP_ROBOT_MOVE_CONTROL_0( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_0( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_1( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_1( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_2( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_2( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_3( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_3( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_4( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_4( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_5( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_5( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_6( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_6( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_7( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_7( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_ROBOT_MOVE_CONTROL_8( int Mode , int side , int Chamber , int Finger , int Slot , int Depth );
int  EQUIP_ROBOT_CONTROL_8( int Mode , int side , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse );
//
int  EQUIP_ROBOT_CONTROL_MAINT_INF( int TMATM , int Mode , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order );

//== Append FEM

int  EQUIP_HOME_FROM_FMBM( int fms , int side , int chamber , int slot1 , int slot2 );
//
int  EQUIP_ROBOT_FROM_FMBM( int fms , int mode , int side , int chamber , int slot1 , int slot2 , BOOL MaintInfUse );
//
int  EQUIP_PICK_FROM_FMBM( int fms , int side , int chamber , int slot1 , int slot2 , BOOL FullRun , int SrcCas , int order , BOOL MaintInfUse );
int  EQUIP_PLACE_TO_FMBM( int fms , int side , int chamber , int slot1 , int slot2 , int FullRun , int SrcCas , int order , BOOL MaintInfUse );

int  EQUIP_PLACE_AND_PICK_WITH_FALIC( int fms , int side , int chamber , int slot1 , int slot2 , int SrcCas , int swch , int PickPlaceMode , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int side1 , int side2 , int pointer , int pointer2 , BOOL MaintInfUse );
int  EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( int chamber , int slot1 , int slot2 , int SrcCas , int swch , int PickPlaceMode );
//
int  EQUIP_CLEAR_CHAMBER( int chamber , int finger , int slot );
//
int  EQUIP_ALIGNING_SPAWN_FOR_TM_MAL( int CHECKING_SIDE , int Finger , int Slot , int SrcCas ); // 2004.09.08
int  EQUIP_ALIGNING_STATUS_FOR_TM_MAL( int CHECKING_SIDE ); // 2004.09.08
//
//int  EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( int CHECKING_SIDE , int Slot1 , int Slot2 , int SrcCas , int swch , int outmode ); // 2017.09.11
int  EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( int CHECKING_SIDE , int Slot1 , int Slot2 , int SrcCas , int swch , int outmode , int s1 , int p1 , int s2 , int p2 ); // 2017.09.11
int  EQUIP_ALIGNING_STATUS_FOR_FM_MAL( int CHECKING_SIDE );
//
int  EQUIP_COOLING_SPAWN_FOR_FM_MIC( int CHECKING_SIDE , int icslot1 , int icslot2 , int Slot1 , int Slot2 , int SrcCas , int s1 , int p1 , int s2 , int p2 ); // 2005.09.05
int  EQUIP_COOLING_STATUS_FOR_FM_MIC( int CHECKING_SIDE , int icslot ); // 2005.09.05
//
void EQUIP_COOLING_TIMER_UPDATE_FOR_FM_MIC(); // 2005.12.01
//================================================================================
//================================================================================
//================================================================================
void EQUIP_SPAWN_WAITING_BM( int Side , int Chamber , int Mode , int slot , int bmfirstslot );
int  EQUIP_RUN_WAITING_BM( int Side , int Chamber , int mode , int slot , int bmfirstslot , BOOL MaintInfUse );

//== Append Different Style

int  EQUIP_PM_MOVE_CONTROL( int Chamber , int i1 , int i2 , int i3 , int i4 , int i5 , int mode , int tms , int appstyle );

//== Append Different Style for AL0/SUB2

int  EQUIP_INTERFACE_PICKPLACE_CANCEL( int tmside , int mode , int Chamber , int Finger , int Slot , int Depth , int Transfer , int SrcCas , int abortwhere );
void EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( int mode , int Chamber , int Slot1 , int Slot2 , int Transfer , int SrcCas , int abortwhere ); // 2007.12.12

//===========================================================================================================================================================================

int  _i_SCH_EQ_PROCESS_FUNCTION_STATUS( int side , int ch , BOOL waitfinish );
int  _i_SCH_EQ_INTERFACE_FUNCTION_STATUS( int ch , BOOL waitfinish );
int  _i_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( int ch , int *addr );
//
int  _i_SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( int Chamber , int *prcs_time , int *post_time );
int  _i_SCH_EQ_PROCESS_SKIPFORCE_RUN( int mode , int ch );
BOOL _i_SCH_EQ_SPAWN_PROCESS_SUB( int Side , int Pointer , int Chamber , int StartChamber , int Slot , int Finger , char *Recipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex , int *rid ); // 2015.04.23
BOOL _i_SCH_EQ_SPAWN_PROCESS( int Side , int Pointer , int Chamber , int StartChamber , int Slot , int Finger , char *Recipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex );
BOOL _i_SCH_EQ_SPAWN_EVENT_PROCESS( int Side , int Pointer , int Chamber , int StartChamber , int Slot , int Finger , char *Recipe , int mode , int PutTime , char *NextPM , int MinTime , int index );
int  _i_SCH_EQ_PICK_FROM_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , BOOL Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual );
int  _i_SCH_EQ_PLACE_TO_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual );
int  _i_SCH_EQ_PICK_FROM_FMBM( int fms , int side , int chamber , int slot1 , int slot2 , BOOL FullRun , int SrcCas , int order );
int  _i_SCH_EQ_PLACE_TO_FMBM( int fms , int side , int chamber , int slot1 , int slot2 , int FullRun , int SrcCas , int order );
int  _i_SCH_EQ_PICK_FROM_TM_MAL( int CHECKING_SIDE , int Finger , int Slot , int SrcCas , int order );
void _i_SCH_EQ_BEGIN_END_RUN( int Chamber , BOOL End , char *appendstring , int Disable );
int  _i_SCH_EQ_ROBOT_FROM_FMBM( int fms , int mode , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 );
int  _i_SCH_EQ_ROBOT_FROM_CHAMBER( int tms , int mode , int CHECKING_SIDE , int Chamber , int Finger , int Slot ); // 2012.04.18
//
int  _i_SCH_EQ_SWAP_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual ); // 2013.01.11

int  _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( int Chamber ); // 2011.04.05

#endif
