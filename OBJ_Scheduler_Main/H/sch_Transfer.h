#ifndef SCH_TRANSFER_H
#define SCH_TRANSFER_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_Check( int ch );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_Check_until_Process_Finished();
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_InterlockFinger_Check_for_Sub( int Mode , int tms , int Ch , int finger , int skippickch );
//------------------------------------------------------------------------------------------
void Transfer_Cass_Info_Operation_Before_Pick( int Ch , int Slot );
void Transfer_Cass_Info_Operation_After_Place( int Ch , int Slot );
//------------------------------------------------------------------------------------------

#endif
