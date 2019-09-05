#ifndef SCH_CASSETTE_RESULT_H
#define SCH_CASSETTE_RESULT_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Cassette Result Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_CASSETTE_LAST_RESULT_INCOMPLETE_FAIL( int side );
//void SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( int side , int Slot , int data ); // 2011.12.14
void SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( int side , int pointer , BOOL always , int data ); // 2011.12.14
void SCHEDULER_CASSETTE_LAST_RESULT_SET( int side , int PathIn , int Slot , int ch , int depth , int data , BOOL Gj );
void SCHEDULER_CASSETTE_LAST_RESULT2_SET( int side , int pointer , int ch , int depth , int data , BOOL Gj );
void SCHEDULER_CASSETTE_LAST_RESULT_INIT_AFTER_CM_PICK( int cs , int s , int side , int pt );
void SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( int mode , int station , int slot , int srccass , int srcslot , int TMATM , int Arm ); // 2008.02.26
void SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int TMATM );
void SCHEDULER_CASSETTE_WAFER_RESULT2_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int s , int p , int TMATM );
int  SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( int srccass , int srcslot , int station , int subslot , int data , int s , int p ); // 2003.10.21

BOOL SCHEDULER_CONTROL_Set_CASSETTE_SKIP( int side , int slot , BOOL data ); // 2005.07.19

void _SCH_CASSETTE_ReSet_For_Move( int tside , int p2 , int side , int p ); // 2011.04.18

void INIT_SCHEDULER_CASSETTE_RESULT_DATA( int apmode , int side );

//===============================================================
int  _i_SCH_CASSETTE_LAST_RESULT_GET( int cs , int s );
int  _i_SCH_CASSETTE_LAST_RESULT2_GET( int s , int p );

int  _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( int cs , int s ); // 2007.07.13
int  _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( int s , int p ); // 2011.04.18
void _i_SCH_CASSETTE_LAST_RESET( int cs , int s );
void _i_SCH_CASSETTE_LAST_RESET2( int s , int p ); // 2011.04.21

int  _i_SCH_CASSETTE_Chk_SKIP( int side ); // 2005.07.19

BOOL _i_SCH_CASSETTE_Chk_SKIP_WITH_POINTER( int side , int pt ); // 2009.04.29

//===============================================================

#endif

