#ifndef ROBOT_ANIMATION_H
#define ROBOT_ANIMATION_H

//-----------------------------------------------------------------------
// Common
//-----------------------------------------------------------------------
BOOL   ROBOT_ANIMATION_SUB_GO_TARGET_POSITION( double Current , double Target , double Range , double *Result );
BOOL   ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( double Current , double Target , double Range );
//-----------------------------------------------------------------------
// TM
//-----------------------------------------------------------------------
int  ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( int tms , int runtype , BOOL End , BOOL *update );
BOOL ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( int tms , int MODE , int STATION , int TMSTATION , int SLOT , int SLOT2 , int DEPTH , int DEPTH2 , int SYNCH , int SEPERATE , BOOL MAINTINF , BOOL animuse );
void ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( int tms , int MODE , int STATION , int TMSTATION ); // 2008.12.10

//-----------------------------------------------------------------------
// FM
//-----------------------------------------------------------------------
int  ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( int fms , int runtype , BOOL End , BOOL *update );
BOOL ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( int fms , int MODE , int STATION , int SLOT1 , int SLOT2 , int SYNCH , BOOL MaintInfUse , BOOL AnumUse );
void ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( int fms , int MODE , int STATION , int SLOT1 , int SLOT2 );

//-----------------------------------------------------------------------
// ETC
//-----------------------------------------------------------------------
void ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE( int ch , BOOL *update , int mode , int tms );

#endif
