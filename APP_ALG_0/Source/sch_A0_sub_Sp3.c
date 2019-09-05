//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_Multireg.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_sub.h"
#include "sch_A0_init.h"
#include "sch_A0_sub_sp3.h"
//================================================================================



//=======================================================================================
int SCHEDULER_CONTROL_REMAP_AFTER_PICKCM_A0SUB3( int side , int pt , int tms , int tmarm ) {
	int i , fs , m , cd , cid;
	char strdata[256];
	Scheduling_Regist	Scheduler_Reg;
	//=========================================================================
	if ( !_SCH_PRM_GET_MODULE_MODE( side + CM1 ) ) return 1;
	//=========================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( _SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) return 2;
		if ( _SCH_CLUSTER_Get_PathDo( side , i ) == PATHDO_WAFER_RETURN ) return 3;
	}
	//=========================================================================
	for ( fs = 0 ; fs < MAX_CASSETTE_SIDE ; fs++ ) {
		if ( fs != side ) {
			if ( !_SCH_PRM_GET_MODULE_MODE( fs + CM1 ) ) {
				if ( _SCH_SYSTEM_USING_GET( fs ) <= 0 ) break;
			}
		}
	}
	//=========================================================================
	if ( fs == MAX_CASSETTE_SIDE ) {
		_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(4) %d:%d:%d/NA\n" , side , pt , tmarm );
		return 4;
	}
	//=========================================================================
	_SCH_SYSTEM_LEAVE_TM_CRITICAL( tms ); // 2006.03.10
	//=========================================================================
//	_SCH_SUB_Wait_Finish_Complete( fs , 4 ); // 2010.07.29
	_SCH_SUB_Wait_Finish_Complete( fs , 9 ); // 2010.07.29
	//=========================================================================
	_SCH_SYSTEM_ENTER_TM_CRITICAL( tms ); // 2006.03.10
	//=========================================================================
	m = _SCH_SYSTEM_FA_GET( side );
	cd = _SCH_SYSTEM_CPJOB_GET( side );
	cid = _SCH_SYSTEM_CPJOB_ID_GET( side );
	//
	if ( m != 1 ) {
		_SCH_IO_SET_IN_PATH( fs , _SCH_IO_GET_IN_PATH( side ) );
		_SCH_IO_SET_OUT_PATH( fs , _SCH_IO_GET_OUT_PATH( side ) );
		_SCH_IO_SET_START_SLOT( fs , _SCH_IO_GET_START_SLOT( side ) );
		_SCH_IO_SET_END_SLOT( fs , _SCH_IO_GET_END_SLOT( side ) );
		_SCH_IO_GET_RECIPE_FILE( side , strdata );	_SCH_IO_SET_RECIPE_FILE( fs , strdata );
		_SCH_IO_SET_LOOP_COUNT( fs , 1 );
	}
	else {
//		Scheduler_Reg.RunIndex  = SYSTEM_RID_GET( side ); // 2008.02.26
		Scheduler_Reg.RunIndex  = _SCH_IO_GET_IN_PATH( side ) - 1; // 2008.02.26
		Scheduler_Reg.CassIn    = _SCH_IO_GET_IN_PATH( side ) - 1;
		Scheduler_Reg.CassOut   = _SCH_IO_GET_OUT_PATH( side ) - 1;
		Scheduler_Reg.SlotStart = _SCH_IO_GET_START_SLOT( side );
		Scheduler_Reg.SlotEnd   = _SCH_IO_GET_END_SLOT( side );
		Scheduler_Reg.LoopCount = 1;
		strcpy( Scheduler_Reg.JobName , _SCH_SYSTEM_GET_JOB_ID( side ) );
		_SCH_IO_GET_RECIPE_FILE( side , strdata );
		strcpy( Scheduler_Reg.LotName , strdata );
		strcpy( Scheduler_Reg.MidName , _SCH_SYSTEM_GET_MID_ID( side ) );

		if ( !_SCH_MULTIREG_SET_REGIST_DATA( fs , &Scheduler_Reg ) ) {
			_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(5) %d:%d:%d/%d:%d:%d:%d\n" , side , pt , tmarm , fs , m , cd , cid );
			return 5;
		}
	}
	//--------------------------------------------------------------------------------------
	_SCH_SYSTEM_FA_SET( fs , m );
	_SCH_SYSTEM_CPJOB_SET( fs , cd );
	_SCH_SYSTEM_CPJOB_ID_SET( fs , cid );
	//--------------------------------------------------------------------------------------
	_SCH_SYSTEM_USING_SET( fs , 1 );
	//--------------------------------------------------------------------------------------
	_SCH_SUB_Reset_Scheduler_Data( fs );
	//--------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		_SCH_CLUSTER_Clear_PathProcessData( fs , i , TRUE );
		_SCH_CLUSTER_Copy_Cluster_Data( fs , i , side , i , 0 );
	}
	//--------------------------------------------------------------------------------------
	_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( fs , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) );
	//--------------------------------------------------------------------------------------
	_SCH_MODULE_Set_FM_OutCount( fs , 0 );
	_SCH_MODULE_Set_FM_InCount( fs , 0 );
	_SCH_MODULE_Set_TM_OutCount( fs , 0 );
	_SCH_MODULE_Set_TM_InCount( fs , 0 );
	_SCH_MODULE_Set_TM_DoubleCount( fs , 0 );

	_SCH_MODULE_Set_FM_DoPointer( fs , 0 );
	_SCH_MODULE_Set_TM_DoPointer( fs , 0 );
	//--------------------------------------------------------------------------------------
	if ( !_SCH_SUB_Main_Handling_Code_Spawn( fs + 100 ) ) {
		if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( fs );
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) _SCH_CLUSTER_Clear_PathProcessData( fs , i , FALSE );
		_SCH_SYSTEM_USING_SET( fs , 0 );
		_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(6) %d:%d:%d/%d:%d:%d:%d\n" , side , pt , tmarm , fs , m , cd , cid );
		return 6;
	}
	//--------------------------------------------------------------------------------------
	while( TRUE ) {
		Sleep(100);
		//-------------------------------------------
		if ( _SCH_SYSTEM_USING_GET( fs ) >= 10 ) break;
		//-------------------------------------------
		if ( MANAGER_ABORT() ) {
			if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( fs );
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) _SCH_CLUSTER_Clear_PathProcessData( fs , i , FALSE );
			_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(7) %d:%d:%d/%d:%d:%d:%d\n" , side , pt , tmarm , fs , m , cd , cid );
			return 7;
		}
		//-------------------------------------------
		if ( _SCH_SYSTEM_USING_GET( fs ) <= 0 ) {
			if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( fs );
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) _SCH_CLUSTER_Clear_PathProcessData( fs , i , FALSE );
			_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(8) %d:%d:%d/%d:%d:%d:%d\n" , side , pt , tmarm , fs , m , cd , cid );
			return 8;
		}
		//-------------------------------------------
	}
	//--------------------------------------------------------------------------------------
	_SCH_MODULE_Set_FM_OutCount( fs , _SCH_MODULE_Get_FM_OutCount( side ) );
	_SCH_MODULE_Set_FM_InCount( fs , _SCH_MODULE_Get_FM_InCount( side ) );
	_SCH_MODULE_Set_TM_OutCount( fs , _SCH_MODULE_Get_TM_OutCount( side ) );
	_SCH_MODULE_Set_TM_InCount( fs , _SCH_MODULE_Get_TM_InCount( side ) );
	_SCH_MODULE_Set_TM_DoubleCount( fs , _SCH_MODULE_Get_TM_DoubleCount( side ) );
	//--------------------------------------------------------------------------------------
	if ( ( m != 0 ) && ( cd ) ) {
		if ( !_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS( cid , fs , side ) ) {
			_IO_CIM_PRINTF( "CASSETTE_REMAP_AFTER_PICKCM(DS3) FAIL(9) %d:%d:%d/%d:%d:%d:%d\n" , side , pt , tmarm , fs , m , cd , cid );
			return 9;
		}
	}
	//--------------------------------------------------------------------------------------
	_SCH_SYSTEM_FA_SET( side , 0 );
	_SCH_SYSTEM_CPJOB_SET( side , 0 );
	_SCH_SYSTEM_CPJOB_ID_SET( side , 0 );
	//
	_SCH_MODULE_Set_FM_OutCount( side , 0 );
	_SCH_MODULE_Set_FM_InCount( side , 0 );
	_SCH_MODULE_Set_TM_OutCount( side , 0 );
	_SCH_MODULE_Set_TM_InCount( side , 0 );
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) _SCH_CLUSTER_Clear_PathProcessData( side , i , FALSE );
	//
	_SCH_MODULE_Set_TM_SIDE( tms , tmarm , fs );
	return 0;
}
