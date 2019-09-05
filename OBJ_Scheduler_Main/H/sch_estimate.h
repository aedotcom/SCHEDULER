#ifndef SCH_ESTIMATE_H
#define SCH_ESTIMATE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULING_LOT_PREDICT_TIME_CHECK( int idx );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int GET_RUN_LD_CONTROL( int mode );

//=======================================================================================================================================

void EST_BPM_RUN_INIT();


void EST_CM_BEGIN( int cm );
void EST_CM_END( int cm );
void EST_CM_PLACE_START( int cm );
void EST_CM_PLACE_END( int cm );
void EST_CM_PICK_START( int cm );
void EST_CM_PICK_END( int cm );

void EST_PM_BEGIN( int pm );
void EST_PM_PRE_PROCESS_START( int pm );
void EST_PM_PRE_PROCESS_END( int pm );
void EST_PM_PLACE_START( int pm );
void EST_PM_PLACE_END( int pm );
void EST_PM_PROCESS_START( int pm );
void EST_PM_PROCESS_END( int pm );
void EST_PM_PICK_START( int pm );
void EST_PM_PICK_END( int pm );
void EST_PM_POST_PROCESS_START( int pm );
void EST_PM_POST_PROCESS_END( int pm );
void EST_PM_END( int pm );

void EST_BM_BEGIN( int bm );
void EST_BM_FM_SIDE_START( int bm );
void EST_BM_FM_SIDE_END( int bm );
void EST_BM_TM_SIDE_START( int bm );
void EST_BM_TM_SIDE_END( int bm );
void EST_BM_PLACE_START( int bm , int tms );
void EST_BM_PLACE_END( int bm , int tms );
void EST_BM_PICK_START( int bm , int tms );
void EST_BM_PICK_END( int bm , int tms );

void EST_TM_RUN_START();
void EST_TM_PLACE_START( int tmside );
void EST_TM_PLACE_END( int tmside );
void EST_TM_PICK_START( int tmside );
void EST_TM_PICK_END( int tmside );

void EST_FM_RUN_START();
void EST_FM_PLACE_START();
void EST_FM_PLACE_END();
void EST_FM_PICK_START();
void EST_FM_PICK_END();
//=======================================================================================
//void EST_REPORT();


#endif

