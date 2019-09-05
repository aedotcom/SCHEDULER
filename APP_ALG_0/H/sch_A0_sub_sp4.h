#ifndef SCH_A0_SUB_SP4_H
#define SCH_A0_SUB_SP4_H

//=======================================================================================
void SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB4( int , int );
//=======================================================================================
void SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( int , int );
//=======================================================================================
BOOL SCHEDULER_CONTROL_END_WAIT_A0SUB4( int side , int mode , int ch );
//=======================================================================================
BOOL SCHEDULER_CONTROL_END_CHECK_A0SUB4( int side , int mode );
//=======================================================================================
BOOL SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( int bmc , int side , int pointer );
//=======================================================================================
BOOL SCHEDULER_CONTROL_LOTPRE_EXTRACT( BOOL remap , int side , int ch , int clx , char *rcpname , char *rcpname_post , int mode );
//=======================================================================================
BOOL SCHEDULER_CONTROL_PLACE_PICK_ACTION_WITH_ERROR_A0SUB4( int tms , int side , int pt , int ch , int arm , int wid , int depid , int scm );
//=======================================================================================
void SCHEDULER_CONTROL_PLACE_PICK_REPORT_WITH_ERROR_A0SUB4( int tms , int ch , int , int pt , int arm , int ord );
//=======================================================================================
void SCHEDULER_CONTROL_SET_MAL_CHAMBER_A0SUB4( int ch );
//=======================================================================================
int  SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( int side , int pt , int mode );
//=======================================================================================
//BOOL SCHEDULER_CONTROL_PICK_FROM_FM_DUMMYWAITCHECK_A0SUB4( int side , int pt ); // 2011.05.11 // 2011.06.13
//=======================================================================================
BOOL SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( int TMATM , int bch , int side , int pointer , int mode ); // 2009.08.26
//=======================================================================================
BOOL SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_AFTER_PICK_A0SUB4( int side , int p , int FM_Buffer );
//=======================================================================================
void SCHEDULER_CONTROL_CHECK_RUN_START_RESTORE_A0SUB4( int side );
//=======================================================================================
void SCHEDULER_CONTROL_CHECK_RUN_AGAIN_RESTORE_A0SUB4( int side );
//=======================================================================================
BOOL SCHEDULER_CONTROL_TUNING_GET_MORE_APPEND_DATA_A0SUB4( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index ,
						char *AppendStr );
//=======================================================================================

#endif

