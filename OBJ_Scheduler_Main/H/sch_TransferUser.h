#ifndef SCH_TRANSFER_USER_H
#define SCH_TRANSFER_USER_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void Transfer_User_Code( int Mode );
int  Transfer_User_LL_Code( int Mode );
int  Transfer_User_FEM_Code( int Mode );

int  Transfer_User_Running_LL_Code( int Mode , int tmatm , int Finger , int Src_Station , int Src_Slot , int Trg_Station , int Trg_Slot );
int  Transfer_User_Running_FEM_Code( int Mode , int Use , int Finger , int Src_Station , int Src_Slot , int Src_Slot2 , int Trg_Station , int Trg_Slot , int Trg_Slot2 , int *BM_Station , int *BM_Slot , int *BM_Slot2 , int *BM_PSlot , int *BM_PSlot2 , int st , int et , BOOL DoArmB2 , int *rcode );

#endif

