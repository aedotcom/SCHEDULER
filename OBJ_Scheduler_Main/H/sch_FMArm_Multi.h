#ifndef SCH_FMARM_MULTI_H
#define SCH_FMARM_MULTI_H

void INIT_SCHEDULER_FM_ARM_MULTI_DATA( int , int );

void _i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( BOOL retmode );
void _i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM_POST( int *finc , BOOL endok );

void _i_SCH_FMARMMULTI_CASSETTE_SET_AFTER_PLACE_CM( BOOL retmode );
void _i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( int cm );
void _i_SCH_FMARMMULTI_LASTSET_AFTER_PLACE_CM();

//

void _i_SCH_FMARMMULTI_DATA_Init();
BOOL _i_SCH_FMARMMULTI_DATA_Append( int side , int pointer , int slot , int bmslot );
BOOL _i_SCH_FMARMMULTI_DATA_Exist( int index , int *s , int *p , int *cs , int *bs ); // 2007.07.11

void _i_SCH_FMARMMULTI_DATA_SHIFTING_A_AND_APPEND( int side , int pointer , int slot , int bmslot , int mode , int *rs , int *rp , int *rw ); // 2007.08.31

void _i_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( int ch , int slot , int mode );
void _i_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( int slot );

void _i_SCH_FMARMMULTI_FA_SUBSTRATE( int ch , int mslot , int Result );

void _i_SCH_FMARMMULTI_PICK_FROM_CM_POST( int mode , int opt );

void _i_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( int slot );

void _i_SCH_FMARMMULTI_DATA_SET_FROM_FIC( int fms , int ICsts_Real , int k ); // 2007.08.01
void _i_SCH_FMARMMULTI_DATA_SET_FROM_CM_FDHC( int CHECKING_SIDE , int fms , int FM_Slot_Real , int k ); // 2007.08.01
void _i_SCH_FMARMMULTI_DATA_SET_FROM_BM( int fms , int FM_Buffer0 , int FM_Slot_Real0 , int k , int mode ); // 2007.08.01

#endif

