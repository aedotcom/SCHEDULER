#ifndef SCH_TRANSFER_MAINTINF_H
#define SCH_TRANSFER_MAINTINF_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Transfer_Maint_Inf_Code_Interface_Data_Setting( int check , int index , int TMATM , int Chamber , int Finger , int Slot , int Depth , int ArmASlot , int ArmBSlot , int SrcCass , int BMChamber , int TransferID , int order );

void Transfer_Maint_Inf_Code_for_Low_Level( int Signal );
BOOL Transfer_Maint_Inf_Code_End( int index );
void Transfer_Maint_Inf_Code_End_IO_Set( int index , int result );
void Transfer_Maint_Inf_Code_for_Init();

#endif

