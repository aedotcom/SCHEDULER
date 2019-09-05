#ifndef ROBOT_HANDLING_H
#define ROBOT_HANDLING_H

//===============================================================
void ROBOT_LAST_ACTION_CONTROL( int TMATM , int mode , int arm , int ch ); // 2013.03.26
//===============================================================
int  ROBOT_Get_FAST_FINGER_AT_STATION( int TMATM , int a1 , int a2 , int i , double curpos , double curmv , BOOL *same );
//===============================================================
void ROBOT_ARM_SET_CONFIGURATION( int TMATM );
//===============================================================
void ROBOT_FM_ARM_SET_CONFIGURATION();
//===============================================================
//===============================================================
int  _i_SCH_ROBOT_GET_FINGER_FREE_COUNT( int TMATM );
int  _i_SCH_ROBOT_GET_FAST_PICK_FINGER( int TMATM , int Station );
int  _i_SCH_ROBOT_GET_FAST_PLACE_FINGER( int TMATM , int Station );
BOOL _i_SCH_ROBOT_GET_FINGER_HW_USABLE( int TMATM , int arm );
//===============================================================
BOOL _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( int arm );
//===============================================================
int  _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( int pickarm ); // 2013.01.11
//===============================================================

#endif
