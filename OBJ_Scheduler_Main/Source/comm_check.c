#include "default.h"

#include "EQ_Enum.h"

#include "IOLog.h"
#include "User_Parameter.h"
#include "Equip_Handling.h"
#include "Sch_estimate.h"
#include "Sch_Prm.h"
#include "System_tag.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_ProcessMonitor.h"
#include "sch_estimate.h"

void FA_MDL_STATUS_IO_SET( int ch );
void EQUIP_COOLING_TIMER_UPDATE_FOR_FM_MIC();

int MSG_READYEND[MAX_CHAMBER]; // 2010.09.21

CRITICAL_SECTION CR_COMMCHK_RE; // 2010.09.21

enum {
	MSG_NA				,	// 0
	MSG_START			,	// 1
	MSG_END_RUNNING		,	// 2
	MSG_END_DONE		,	// 3
	MSG_READY_RUNNING	,	// 4
	MSG_READY_DONE		,	// 5
	MSG_READY_FAIL		,	// 6
};

enum {
	CSTS_UNUSE         ,	// 0
	CSTS_LOCAL         ,	// 1
	CSTS_DISCON        ,	// 2
	CSTS_ENABLE        ,	// 3
	CSTS_DISABLE       ,	// 4
	CSTS_DISABLEHW     ,	// 5
	CSTS_ENABLEPM      ,	// 6
	//
	CSTS_CIU_ENABLE    ,	// 7
	CSTS_CIU_DISABLE_WY,	// 8
	CSTS_CIU_DISABLE_WN,	// 9
	CSTS_CIU_ENABLEPM  ,	// 10
	CSTS_CIU_DISHW_WY  ,	// 11
	CSTS_CIU_DISHW_WN  ,	// 12
	//
	CSTS_ENABLE_ABORT  ,	// 13
	CSTS_DISABLE_ABORT ,	// 14
	CSTS_DISHW_ABORT   ,	// 15
	CSTS_ENABLEPM_ABORT,	// 16
};

enum {
	MSTS_USE         ,	// 0
	MSTS_CTCUSE      ,	// 1
	MSTS_DISABLE     ,	// 2
	MSTS_DISABLEHW   ,	// 3
	MSTS_ABORT       ,	// 4
};

//---------------------------------------------+--------------------+--------------------+
//  Status                                     |  Option            | ENUM               |
//---------------------------------------------+--------------------+--------------------+
//  0 : Unuse                                  |  Normal        or  | CSTS_UNUSE         |
//  1 : Local                                  |  Expand        or  | CSTS_LOCAL         |
//  2 : Remote  : Disconnect                   |  Expand+Abort      | CSTS_DISCON        |
//  3 : Remote  : Enable(Connect)              |                    | CSTS_ENABLE        |
//  4 : Loc.Rem : Disable                      |                    | CSTS_DISABLE       |
//  5 : Loc.Rem : Disable HW                   |                    | CSTS_DISABLEHW     |
//  6 : Loc.Rem : Enable PM                    |                    | CSTS_ENABLEPM      |
//---------------------------------------------+--------------------+--------------------+
//  7 : Loc.Rem : CTC Use(Enable)              |  Expand        or  | CSTS_CIU_ENABLE    |
//  8 : Loc.Rem : CTC Use(Disable)(WaferYes)   |  Expand+Abort      | CSTS_CIU_DISABLE_WY|
//  9 : Loc.Rem : CTC Use(Disble)(WaferNo)     |                    | CSTS_CIU_DISABLE_WN|
// 10 : Loc.Rem : CTC Use(Enable PM)           |                    | CSTS_CIU_ENABLEPM  |
// 11 : Loc.Rem : CTC Use(Disable HW)(WaferYes)|                    | CSTS_CIU_DISHW_WY  |
// 12 : Loc.Rem : CTC Use(Disble HW)(WaferNo)  |                    | CSTS_CIU_DISHW_WN  |
//---------------------------------------------+--------------------+--------------------+
// 13 : Loc.Rem : Enable.Abort                 |  Expand+Abort      | CSTS_ENABLE_ABORT  |
// 14 : Loc.Rem : Disable.Abort                |                    | CSTS_DISABLE_ABORT |
// 15 : Loc.Rem : Disble HW.Abort              |                    | CSTS_DISHW_ABORT   |
// 16 : Loc.Rem : Enable PM.Abort              |                    | CSTS_ENABLEPM_ABORT|
//---------------------------------------------+--------------------+--------------------+
int CURRECNT_CONNECTION_STATUS[MAX_CHAMBER]; // 2005.10.21
//
void Get_Connection_Status_Init() { // 2005.10.21
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
//		CURRECNT_CONNECTION_STATUS[i] = 1; // 2005.11.29
		CURRECNT_CONNECTION_STATUS[i] = 99; // 2005.11.29
		//
		MSG_READYEND[i] = 0; // 2010.09.21
	}
	//
	InitializeCriticalSection( &CR_COMMCHK_RE ); // 2010.09.21

}
//
int Get_Connection_Status( int ch ) { // 2005.10.21
	return CURRECNT_CONNECTION_STATUS[ch];
}
//

extern int EVENT_FLAG_LOG; // 2016.05.09

void Set_Connection_Status( int ch , int data , int logdata ) { // 2005.10.21
	int i; // 2015.05.26
	//
	if ( ( CURRECNT_CONNECTION_STATUS[ch] == 99 ) && ( data == 0 ) ) return; // 2005.11.29
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
		if ( CURRECNT_CONNECTION_STATUS[ch] != data ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( _i_SCH_SYSTEM_USING_GET( i ) != 0 ) {
					_i_SCH_LOG_LOT_PRINTF( i , "CommStatus SET [S=%d] %s : %d->%d(%d)%cCOMMSTSCHG\n" , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , CURRECNT_CONNECTION_STATUS[ch] , data , logdata , 9 );
				}
			}
		}
	}
	//
	CURRECNT_CONNECTION_STATUS[ch] = data;

}
//
//BOOL Comm_Run_Check_Sub( int ch , BOOL tagonly ) { // 2003.07.04 // 2015.09.18
int Comm_Run_Check_Sub( int ch , BOOL tagonly ) { // 2003.07.04 // 2015.09.18
	int i;
	//
//	BOOL tmpick; // 2015.08.26 // 2015.09.18
	int tmpick; // 2015.08.26 // 2015.09.18
	//
	tmpick = SCH_TM_PICKPLACING_GET( ch ); // 2015.08.26
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( _i_SCH_SYSTEM_USING_GET( i ) >= 9 ) { // 2003.08.13 (Here is must 9:2005.07.29)
			//
//			if ( SCHEDULER_CONTROL_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) { // 2007.04.12
//			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_71_USE_to_PREND_EF_XLP ) ) { // 2007.04.12 // 2008.09.22
//			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_71_USE_to_PREND_EF_XLP ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2007.04.12 // 2008.09.22 // 2008.09.23
			//
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_71_USE_to_PREND_EF_XLP ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) < MUF_90_USE_to_XDEC_FROM ) ) ) { // 2007.04.12 // 2008.09.22 // 2008.09.23
//				return TRUE; // 2015.09.18
				return 1; // 2015.09.18
			}
			//
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_04_USE_to_PREND_EF_LP ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) <= MUF_08_USE_to_PRENDNA_DECTAG ) ) { // 2015.08.26
				//
//				if ( tmpick ) return TRUE; // 2015.09.18
				//
				if      ( tmpick == 1 ) return 11; // 2015.09.18
				else if ( tmpick == 2 ) return 12; // 2015.09.18
				else if ( tmpick == 3 ) return 13; // 2015.09.18
				else if ( tmpick == 4 ) return 14; // 2015.09.18
				else if ( tmpick == 5 ) return 15; // 2015.09.18
				else if ( tmpick >= 6 ) return 16; // 2015.09.18
				//
			}
			//
			//========================================================================================================
			//
			if ( tagonly ) { // 2010.09.15
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_99_USE_to_DISABLE ) return TRUE; // 2015.09.18
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_99_USE_to_DISABLE ) return 2; // 2015.09.18
			}
			else { // 2016.05.13
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_99_USE_to_DISABLE ) {
					//
					if ( _i_SCH_MODULE_GET_USE_ENABLE( ch , ( _i_SCH_MODULE_Get_PM_WAFER( ch , -1 ) > 0 ) , -1 ) ) {
						return 4;
					}
					else {
						//
						if      ( tmpick == 1 ) return 21;
						else if ( tmpick == 2 ) return 22;
						else if ( tmpick == 3 ) return 23;
						//
//						else if ( tmpick == 4 ) return 24; // 2016.10.20
						//
						else if ( tmpick == 5 ) return 25;
						else if ( tmpick >= 6 ) return 26;
						//
					}
				}
			}
			//
			//========================================================================================================
			//
		}
	}
	//
	//========================================================================================================
	//
//	if ( tagonly ) return FALSE; // 2010.10.05 // 2015.09.18
	if ( tagonly ) return -1; // 2010.10.05 // 2015.09.18
	//
	//========================================================================================================
	//
	//----------------------------------------------------------------------------------------
	// 2008.05.09
	//----------------------------------------------------------------------------------------
//	if ( PROCESS_MONITOR_Run_and_Get_Status( ch ) > 0 ) return TRUE; // 2015.09.18
	if ( PROCESS_MONITOR_Run_and_Get_Status( ch ) > 0 ) return 3; // 2015.09.18
	//----------------------------------------------------------------------------------------
//	if ( Get_RunPrm_RUNNING_CLUSTER() ) {
//		if ( SCHEDULER_CONTROL_Get_Mdl_Run_Flag( ch ) > 0 ) {
//			return TRUE;
//		}
//	}
	//
//	return FALSE; // 2015.09.18
	//
	//
	if      ( tmpick == 1 ) return -11; // 2015.09.18
	else if ( tmpick == 2 ) return -12; // 2015.09.18
	else if ( tmpick == 3 ) return -13; // 2015.09.18
	else if ( tmpick == 4 ) return -14; // 2015.09.18
	else if ( tmpick == 5 ) return -15; // 2015.09.18
	else if ( tmpick >= 6 ) return -16; // 2015.09.18
	//
	return 0;
}

//BOOL Comm_Run_Check( int ch ) { // 2015.09.18
//	return Comm_Run_Check_Sub( ch , FALSE );
//}

BOOL Comm_Run_Check( int ch , int *res ) { // 2015.09.18
	*res = Comm_Run_Check_Sub( ch , FALSE );
	if ( *res > 0 ) return TRUE;
	return FALSE;
}

BOOL Comm_Check_Dot_Abort( int ch ) {
	if ( ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() == 2 ) || ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() == 4 ) ) {
		if (
			( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORT ) ||
			( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) ) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Comm_Check_Dot_Use( int ch ) {
	if ( ( ( ch >= CM1 ) && ( ch < PM1 ) ) || ( ( ch >= BM1 ) && ( ch <= FM ) ) ) { // 2006.12.05
		if ( ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() == 1 ) || ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() == 2 ) ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() ) {
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) > 0 ) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void Set_MSG_READYEND_Tag( int ch , int data ) {
	//
	if ( MSG_READYEND[ch] != data ) {
		switch( data ) {
		case MSG_NA :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_NA(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_START :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_START(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_END_RUNNING :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_END_RUNNING(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_END_DONE :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_END_DONE(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_READY_RUNNING :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_READY_RUNNING(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_READY_DONE :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_READY_DONE(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		case MSG_READY_FAIL :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=MSG_READY_FAIL(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		default :
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[OLD=%d] to [NEW=???(%d)] in %s\n" , MSG_READYEND[ch] , data , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			break;
		}
	}
	//
	MSG_READYEND[ch] = data;
}

BOOL _MSG_READYEND_READY_ACTION_BEFORE_ENABLE( int ch ) { // 2010.09.21
	//
	if ( ch < CM1 ) return TRUE; // 2012.12.06
	//
	if ( _i_SCH_PRM_GET_MAINT_MESSAGE_STYLE() == 0 ) return TRUE;
	if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) return TRUE;
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( ch ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return TRUE;
	//
	_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] {Enter} in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
	//
	if ( MSG_READYEND[ch] == MSG_READY_FAIL ) MSG_READYEND[ch] = MSG_END_DONE;
	//
	while( TRUE ) {
		//
		EnterCriticalSection( &CR_COMMCHK_RE );
		//
		if ( MANAGER_ABORT() ) {
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] {Leave-Aborted} in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			LeaveCriticalSection( &CR_COMMCHK_RE );
			//
			return FALSE;
		}
		//
		switch ( MSG_READYEND[ch] ) {
		case MSG_END_DONE :
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] Starting in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			_i_SCH_EQ_BEGIN_END_RUN( ch , FALSE , "" , 3 );
			//
			Set_MSG_READYEND_Tag( ch , MSG_READY_RUNNING );
			break;
			//
		case MSG_READY_RUNNING :
			break;
			//

		case MSG_READY_DONE :
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] Success in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			Set_MSG_READYEND_Tag( ch , MSG_NA );
			//
			LeaveCriticalSection( &CR_COMMCHK_RE );
			//
			return TRUE;
			break;

		case MSG_READY_FAIL :
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] Fail in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			LeaveCriticalSection( &CR_COMMCHK_RE );
			//
			return FALSE;
			break;

		default :
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] {Leave-%d} in %s\n" , MSG_READYEND[ch] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			LeaveCriticalSection( &CR_COMMCHK_RE );
			//
			return TRUE;
			break;
		}
		//
		LeaveCriticalSection( &CR_COMMCHK_RE );
		//
		Sleep(1);
	}
	//
	return TRUE;
}


BOOL _MSG_READYEND_ACTION( int ch , int *vctc ) { // 2010.09.21
	//
	if ( _i_SCH_PRM_GET_MAINT_MESSAGE_STYLE() == 0 ) return TRUE;
	if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) return TRUE;
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( ch ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return TRUE;
	//
	EnterCriticalSection( &CR_COMMCHK_RE ); // 2010.09.21
	//
	switch ( MSG_READYEND[ch] ) {
	case MSG_NA :
		switch ( *vctc ) {
		case CSTS_CIU_ENABLE :
		case CSTS_CIU_ENABLEPM :
			Set_MSG_READYEND_Tag( ch , MSG_START );
			break;
		}
		break;

	case MSG_END_DONE :
//		if ( !Comm_Run_Check_Sub( ch , TRUE ) ) { // 2010.10.05 // 2015.09.18
		if ( Comm_Run_Check_Sub( ch , TRUE ) <= 0 ) { // 2010.10.05 // 2015.09.18
			Set_MSG_READYEND_Tag( ch , MSG_NA );
		}
		else {
			switch ( *vctc ) {
			case CSTS_CIU_DISABLE_WN : // 2010.10.08
				//
				*vctc = CSTS_DISABLE; // 2010.10.08
				//
				break;
				//
			case CSTS_ENABLE :
			case CSTS_ENABLEPM :
				Set_MSG_READYEND_Tag( ch , MSG_NA );
				break;
			}
		}
		break;
		//
	case MSG_READY_DONE :
	case MSG_READY_FAIL :
		switch ( *vctc ) {
		case CSTS_ENABLE :
		case CSTS_ENABLEPM :
			Set_MSG_READYEND_Tag( ch , MSG_NA );
			break;
		}

		break;
		//
	case MSG_START :
		//
		switch ( *vctc ) {
		case CSTS_CIU_ENABLE :
		case CSTS_CIU_DISABLE_WY :
		case CSTS_CIU_DISABLE_WN :
		case CSTS_CIU_ENABLEPM :
		case CSTS_CIU_DISHW_WY :
		case CSTS_CIU_DISHW_WN :
			break;
			//
		case CSTS_DISABLE :	
			//
			*vctc = CSTS_CIU_DISABLE_WN;
			//
			_IO_CONSOLE_PRINTF( "MSG_READYEND\t[END] Starting in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
			//
			_i_SCH_EQ_BEGIN_END_RUN( ch , TRUE , "" , 3 );
			//
			Set_MSG_READYEND_Tag( ch , MSG_END_RUNNING );
			break;

		default :
			Set_MSG_READYEND_Tag( ch , MSG_NA );
			break;
		}
		break;
		//
	case MSG_END_RUNNING :
		//
		switch ( *vctc ) {
		case CSTS_CIU_ENABLE :
		case CSTS_CIU_DISABLE_WY :
		case CSTS_CIU_DISABLE_WN :
		case CSTS_CIU_ENABLEPM :
		case CSTS_CIU_DISHW_WY :
		case CSTS_CIU_DISHW_WN :
			break;
			//
		case CSTS_DISABLE :	
			//
			switch( EQUIP_BEGIN_END_STATUS( ch ) ) {
			case SYS_SUCCESS :
				//
				_IO_CONSOLE_PRINTF( "MSG_READYEND\t[END] Success(C) in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
				//
				Set_MSG_READYEND_Tag( ch , MSG_END_DONE );
				//
				break;
				//
			case SYS_RUNNING :
				*vctc = CSTS_CIU_DISABLE_WN;
				break;
				//
			default :
				//
				_IO_CONSOLE_PRINTF( "MSG_READYEND\t[END] Fail(C) in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
				//
				_i_SCH_PRM_SET_MODULE_MODE_for_SW( ch , 2 );
				//
				Set_MSG_READYEND_Tag( ch , MSG_END_DONE );
				//
				break;
			}
			//
			break;

		default :
			Set_MSG_READYEND_Tag( ch , MSG_NA );
			break;
		}
		break;
		//
	case MSG_READY_RUNNING :
		//
		switch ( *vctc ) {
		case CSTS_DISABLE :	
			//
			switch( EQUIP_BEGIN_END_STATUS( ch ) ) {
			case SYS_SUCCESS :
				//
				_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] Success(C) in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
				//
				Set_MSG_READYEND_Tag( ch , MSG_READY_DONE );
				//
				break;
				//
			case SYS_RUNNING :
				break;
				//
			default :
				//
				_IO_CONSOLE_PRINTF( "MSG_READYEND\t[READY] Fail(C) in %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) );
				//
				Set_MSG_READYEND_Tag( ch , MSG_READY_FAIL );
				//
				break;
			}
			//
			break;

		default :
			Set_MSG_READYEND_Tag( ch , MSG_NA );
			break;
		}
		break;
		//
	}
	//
	LeaveCriticalSection( &CR_COMMCHK_RE ); // 2010.09.21
	//
	return TRUE;
}
//
int CommChk_OldStatus[MAX_CHAMBER];
int CommChk_SelStatus[MAX_CHAMBER];



void _i_SCH_COMMSTATUS_GET( int ch , int *c1 , int *c2 , int *c3 ) {
	EnterCriticalSection( &CR_COMMCHK_RE );
	*c1 = CommChk_OldStatus[ch];
	*c2 = CommChk_SelStatus[ch];
	*c3 = MSG_READYEND[ch];
	LeaveCriticalSection( &CR_COMMCHK_RE );
}



void Main_Thread_Communication_Check() {
	int i , j , x , v_ctc , v_mdl , CommStatus , rio[MAX_CHAMBER] , sio[MAX_CHAMBER]; // , old[MAX_CHAMBER]; // 2010.10.05
	BOOL wiock[MAX_CHAMBER]; // 2003.08.08
	BOOL ck[MAX_CHAMBER];
	char Buffer[1024];
	char Buffer2[64];
	int ReadMode;
	int RunCheckRes[MAX_CHAMBER]; // 2015.09.18

	//==============================================================================
	//==============================================================================
	//==============================================================================
	Get_Connection_Status_Init(); // 2005.10.21
	//==============================================================================
	//==============================================================================
	//==============================================================================
	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( ( i == AL ) || ( i == IC ) ) continue;
		if ( strlen( _i_SCH_PRM_GET_MODULE_COMMSTATUS_IO_NAME( i ) ) <= 0 ) {
			sprintf( Buffer , "%s.CommStatus" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
			_i_SCH_PRM_SET_MODULE_COMMSTATUS_IO_NAME( i , Buffer );
		}
	}
	//==============================================================================
	//==============================================================================
	//==============================================================================
	Sleep( 5000 );
	//==============================================================================
	//==============================================================================
	//==============================================================================
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { ck[i] = FALSE; CommChk_OldStatus[i] = 0; CommChk_SelStatus[i] = 0; wiock[i] = FALSE; }
	//==============================================================================
	//==============================================================================
	//==============================================================================
	for ( i = CM1 ; i <= FM ; i++ ) {
		//
		if ( ( i == AL ) || ( i == IC ) ) continue;
		//
		rio[i] = -1;
		sio[i] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.%s_CommStatus" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
		//
		if ( sio[i] >= 0 ) {
			//
			ck[i] = TRUE;
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) && _i_SCH_PRM_GET_MODULE_SERVICE_MODE( i ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
				_dWRITE_DIGITAL( sio[i] , 2 , &CommStatus );
				rio[i] = _FIND_FROM_STRING( _K_D_IO , _i_SCH_PRM_GET_MODULE_COMMSTATUS_IO_NAME( i ) );
				if ( rio[i] < 0 ) {
					ck[i] = FALSE;
				}
				else { // 2003.08.08
					if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
						_dDIGITAL_SHAPE( rio[i] , &CommStatus , &j , &v_ctc , &v_mdl , Buffer );
						if ( CommStatus == 4 ) wiock[i] = TRUE;
					}
				}
			}
		}
	}
	//==============================================================================
	//==============================================================================
	//==============================================================================
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { if ( ck[i] ) break; }
	if ( i == MAX_CHAMBER ) { _endthread();	return; }
	//==============================================================================
	//==============================================================================
	// 2008.10.17
	//==============================================================================
	ReadMode = _i_SCH_PRM_GET_DFIX_LOT_LOG_MODE();
	//
//	if ( ReadMode != 0 ) { // 2010.11.16
	if ( ( ReadMode % 2 ) != 0 ) { // 2010.11.16
		//
		j = 0;
		//
		strcpy( Buffer , "T500" );
		//
		for ( i = CM1 ; i <= FM ; i++ ) {
			//
			if ( ( i == AL ) || ( i == IC ) ) continue; // 2009.04.10
			//
			if ( rio[i] < 0 ) continue;
			//
			sprintf( Buffer2 , "D%d" , rio[i] );
			strcat( Buffer , Buffer2 );
			j++;
		}
		//
		if ( j > 0 ) _dGROUP_IO_MONITORING( Buffer );
	}
	//==============================================================================
	while ( TRUE ) {
		for ( i = CM1 ; i <= FM ; i++ ) {
			//=====================
			if ( !ck[i] ) continue;
			//=====================
			//
			RunCheckRes[i] = -99; // 2015.09.18
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				//========================================================================
				Set_Connection_Status( i , 2 , 0 ); // 2005.10.21
				//========================================================================
				_dWRITE_DIGITAL( sio[i] , 0 , &CommStatus );
				ck[i] = FALSE;
				//
				MSG_READYEND[i] = 0; // 2010.09.21
				//
			}
			else {
				if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( i ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) {
					//========================================================================
					Set_Connection_Status( i , 1 , 0 ); // 2005.10.21
					//========================================================================
					v_ctc = CSTS_LOCAL;
					if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
						switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
						case 0 : // Disable
							v_ctc = CSTS_DISABLE;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( ( i >= PM1 ) && ( i < AL ) ) {
									if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
//										if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) { // 2011.11.18
										if ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) <= 0 ) { // 2011.11.18
											v_ctc = CSTS_CIU_DISABLE_WN;
										}
										else {
											v_ctc = CSTS_CIU_DISABLE_WY;
										}
									}
									else {
										if ( Comm_Check_Dot_Abort( i ) ) v_ctc = CSTS_DISABLE_ABORT;
									}
								}
							}
							break;
						case 1 : // Enable
							v_ctc = CSTS_LOCAL;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( ( i >= PM1 ) && ( i < AL ) ) {
									if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
										v_ctc = CSTS_CIU_ENABLE;
									}
									else {
										if ( Comm_Check_Dot_Abort( i ) ) v_ctc = CSTS_ENABLE_ABORT;
									}
								}
								else {
									if ( Comm_Check_Dot_Use( i ) ) v_ctc = CSTS_CIU_ENABLE;
								}
							}
							break;
						case 2 : // Disable H/W
							v_ctc = CSTS_DISABLEHW;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( ( i >= PM1 ) && ( i < AL ) ) {
									if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
//										if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) { // 2011.11.18
										if ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) <= 0 ) { // 2011.11.18
											v_ctc = CSTS_CIU_DISHW_WN;
										}
										else {
											v_ctc = CSTS_CIU_DISHW_WY;
										}
									}
									else {
										if ( Comm_Check_Dot_Abort( i ) ) v_ctc = CSTS_DISHW_ABORT;
									}
								}
							}
							break;
						case 3 : // Enable PM
							v_ctc = CSTS_ENABLEPM;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( ( i >= PM1 ) && ( i < AL ) ) {
									if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
										v_ctc = CSTS_CIU_ENABLEPM;
									}
									else {
										if ( Comm_Check_Dot_Abort( i ) ) v_ctc = CSTS_ENABLEPM_ABORT;
									}
								}
								else {
									if ( Comm_Check_Dot_Use( i ) ) v_ctc = CSTS_CIU_ENABLEPM;
								}
							}
							break;
						}
					}
					//-------------------------------------------------------------------------------
					if ( CommChk_OldStatus[i] != v_ctc ) {
						_dWRITE_DIGITAL( sio[i] , v_ctc , &CommStatus );
						CommChk_OldStatus[i] = v_ctc;
						CommChk_SelStatus[i] = v_ctc; // 2010.10.05
					}
					//-------------------------------------------------------------------------------
				}
				else {
//					if ( ReadMode == 0 ) { // 2010.11.16
					if ( ( ReadMode % 2 ) == 0 ) { // 2010.11.16
						j = _dREAD_DIGITAL( rio[i] , &CommStatus );
					}
					else { // 2008.10.17
						j = _dREAD_MEM_DIGITAL( rio[i] , Buffer );
					}
					//
					v_mdl = -1;
					//========================================================================
					Set_Connection_Status( i , j , RunCheckRes[i] ); // 2005.10.21
					//========================================================================
					if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
						switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
						case 0 : // Disable
							v_ctc = CSTS_DISABLE;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( j == 0 ) {
									v_ctc = CSTS_DISCON;
								}
								else {
									if ( ( i >= PM1 ) && ( i < AL ) ) {
										v_mdl = MSTS_DISABLE;
										if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
											v_mdl = MSTS_CTCUSE;
//											if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) { // 2011.11.18
											if ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) <= 0 ) { // 2011.11.18
												v_ctc = CSTS_CIU_DISABLE_WN;
											}
											else {
												v_ctc = CSTS_CIU_DISABLE_WY;
											}
										}
										else {
											if ( Comm_Check_Dot_Abort( i ) ) {	v_ctc = CSTS_DISABLE_ABORT;	v_mdl = MSTS_ABORT;	}
										}
									}
								}
							}
							break;
						case 1 : // Enable
							if ( j == 0 ) {
								v_ctc = CSTS_DISCON;
							}
							else {
								v_ctc = CSTS_ENABLE;
								if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
									if ( ( i >= PM1 ) && ( i < AL ) ) {
										v_mdl = MSTS_USE;
										if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
											v_ctc = CSTS_CIU_ENABLE;
											v_mdl = MSTS_CTCUSE;
										}
										else {
											if ( Comm_Check_Dot_Abort( i ) ) {	v_ctc = CSTS_ENABLE_ABORT;	v_mdl = MSTS_ABORT;	}
										}
									}
									else {
										if ( Comm_Check_Dot_Use( i ) ) v_ctc = CSTS_CIU_ENABLE;
									}
								}
							}
							break;
						case 2 : // Disable H/W
							v_ctc = CSTS_DISABLEHW;
							if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
								if ( j == 0 ) {
									v_ctc = CSTS_DISCON;
								}
								else {
									if ( ( i >= PM1 ) && ( i < AL ) ) {
										v_mdl = MSTS_DISABLEHW;
										if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
											v_mdl = MSTS_CTCUSE;
//											if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) { // 2011.11.18
											if ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) <= 0 ) { // 2011.11.18
												v_ctc = CSTS_CIU_DISHW_WN;
											}
											else {
												v_ctc = CSTS_CIU_DISHW_WY;
											}
										}
										else {
											if ( Comm_Check_Dot_Abort( i ) ) {	v_ctc = CSTS_DISHW_ABORT;	v_mdl = MSTS_ABORT;	}
										}
									}
								}
							}
							break;
						case 3 : // Enable PM
							if ( j == 0 ) {
								v_ctc = CSTS_DISCON;
							}
							else {
								v_ctc = CSTS_ENABLEPM;
								if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
									if ( ( i >= PM1 ) && ( i < AL ) ) {
										v_mdl = MSTS_USE;
										if ( Comm_Run_Check( i , &(RunCheckRes[i]) ) ) { // 2003.07.04 // 2015.09.18
											v_ctc = CSTS_CIU_ENABLEPM;
											v_mdl = MSTS_CTCUSE;
										}
										else {
											if ( Comm_Check_Dot_Abort( i ) ) {	v_ctc = CSTS_ENABLEPM_ABORT;	v_mdl = MSTS_ABORT;	}
										}
									}
									else {
										if ( Comm_Check_Dot_Use( i ) ) v_ctc = CSTS_CIU_ENABLEPM;
									}
								}
							}
							break;
						}
					}
					else {
						if ( j == 0 ) {
							v_ctc = CSTS_DISCON;
						}
						else {
							v_ctc = CSTS_ENABLE;
						}
						if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) {
							if ( ( i >= PM1 ) && ( i < AL ) ) {
								v_mdl = MSTS_USE;
							}
						}
						//
					}
					//-------------------------------------------------------------------------------
					x = v_ctc; // 2010.10.05
					//-------------------------------------------------------------------------------
					_MSG_READYEND_ACTION( i , &v_ctc );
					//-------------------------------------------------------------------------------
					if ( CommChk_OldStatus[i] != v_ctc ) {
						//
						_dWRITE_DIGITAL( sio[i] , v_ctc , &CommStatus );
						//
						//
						if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								if ( _i_SCH_SYSTEM_USING_GET( j ) != 0 ) {
									_i_SCH_LOG_LOT_PRINTF( j , "CommStatus SET2 [S=%d] %s : %d->%d(%d)(%d)%cCOMMSTSCHG\n" , j , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , CommChk_OldStatus[i] , v_ctc , x , RunCheckRes[i] , 9 );
								}
							}
						}
						//
						//
						_IO_CONSOLE_PRINTF( "MSG_READYEND\t[COMMSTATUS] %d to %d(S=%d) int %s (%d)\n" , CommChk_OldStatus[i] , v_ctc , x , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , RunCheckRes[i] ); // 2015.09.18
						//
						CommChk_SelStatus[i] = x;
						CommChk_OldStatus[i] = v_ctc;
						//
						if ( ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) && ( wiock[i] ) ) { // 2003.07.04 // 2003.08.08
							if ( v_mdl != -1 ) {
//								_dWRITE_DIGITAL( rio[i] , v_mdl , &CommStatus ); // 2008.10.17
								_dWRITE_DIGITAL_TH2( rio[i] , v_mdl ); // 2008.10.17
							}
						}
					}
					else {
						if ( CommChk_SelStatus[i] != x ) {
							//
							_IO_CONSOLE_PRINTF( "MSG_READYEND\t[SELSTATUS] %d to %d(C=%d) int %s (%d)\n" , CommChk_SelStatus[i] , x , v_ctc , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , RunCheckRes[i] ); // 2015.09.18
							//
						}
						CommChk_SelStatus[i] = x;
					}
					//-------------------------------------------------------------------------------
				}
			}
		}
		//================================================================================================================
		// 2005.10.21
		//================================================================================================================
		if ( _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() != 0 ) { // 2005.11.08
			//
			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
				//
				j = 0;
				//
				if ( Get_RunPrm_RUNNING_CLUSTER() ) { // 2011.09.21
					if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) {
						j = 2;
					}
				}
				else {
					j = 1;
				}
				//
				if ( j != 0 ) {
					//
					for ( i = CM1 ; i <= FM ; i++ ) {
						//=====================
						if ( !ck[i] ) continue;
						//=====================
						if ( Get_Connection_Status( i ) != 0 ) {
							//
							if ( j == 2 ) {
								//
								if ( Get_Connection_Status( i ) != 99 ) continue;
								//
							}
							else {
								continue;
							}
							//
						}
						//=====================
						if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
						//=====================
						if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( i ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) continue;
						//=====================
						if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) == 0 ) continue; // Disable
						if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) == 2 ) continue; // Disable HW
						//
						//=====================
						// Make Disable
						//=====================
						// OPTION	CTCRUN(EIL)	WAIT    STATUS
						//
						// 0		DH(1)		D(0)	ABORT
						// 1		D(0)		D(0)	ABORT
						// 2		DH(1)		DH(2)	ABORT
						// 3		D(0)		DH(2)	ABORT
						//
						// 4		X			D(0)	ERROR
						// 5		X			DH(2)	ERROR
						//
						x = 0; // 2016.02.15
						//
						switch ( _i_SCH_PRM_GET_MODULE_COMMSTATUS2_MODE() ) { // 2012.07.21
						case 4 :	v_mdl = 1;	v_ctc = 0;	x = 1;	break; // 2016.02.15
						case 5 :	v_mdl = 1;	v_ctc = 2;	x = 1;	break; // 2016.02.15
							//
						case 1 :	v_mdl = 0;	v_ctc = 0;	break;
						case 2 :	v_mdl = 1;	v_ctc = 2;	break;
						case 3 :	v_mdl = 0;	v_ctc = 2;	break;
						default :	v_mdl = 1;	v_ctc = 0;	break;
						}
						//
						if ( j == 1 ) {
							_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , v_ctc );
							//=====================
							FA_MDL_STATUS_IO_SET( i ); // 2005.11.29
							//=====================
							if ( v_ctc == 0 ) {
								_IO_CIM_PRINTF( "Module %s changed to Disable Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
							}
							else {
								_IO_CIM_PRINTF( "Module %s changed to DisableHW Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
							}
							//=====================
						}
						else { // run
							//
							if ( x == 0 ) { // 2016.02.15
								//
								if ( ( i >= PM1 ) && ( i < AL ) ) { // 2011.09.21
									//=====================
									SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( i , v_mdl , 0 , ABORTWAIT_FLAG_NONE , 0 );
									//=====================
									FA_MDL_STATUS_IO_SET( i ); // 2005.11.29
									//=====================
									if ( v_mdl == 0 ) {
										_IO_CIM_PRINTF( "Module %s changed to Disable Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
									}
									else {
										_IO_CIM_PRINTF( "Module %s changed to DisableHW Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
									}
									//=====================
								}
								else {
									_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , v_ctc );
									//=====================
									FA_MDL_STATUS_IO_SET( i ); // 2005.11.29
									//=====================
									if ( v_ctc == 0 ) {
										_IO_CIM_PRINTF( "Module %s changed to Disable2 Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
									}
									else {
										_IO_CIM_PRINTF( "Module %s changed to DisableHW2 Mode because Offline Status happened\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
									}
									//=====================
								}
								//
							}
							//
						}
						//
					}
				}
				//
			}
		}
		//================================================================================================================
		// 2005.12.01
		//================================================================================================================
		EQUIP_COOLING_TIMER_UPDATE_FOR_FM_MIC();
		//================================================================================================================
		//================================================================================================================
		// 2006.08.11
		//================================================================================================================
		SCHEDULING_LOT_PREDICT_TIME_CHECK( -1 );
		//================================================================================================================
//		if ( ReadMode == 0 ) { // 2010.11.16
		if ( ( ReadMode % 2 ) == 0 ) { // 2010.11.16
			Sleep(500);
		}
		else {
			Sleep(250);
		}
		//================================================================================================================
	}
	_endthread();
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
