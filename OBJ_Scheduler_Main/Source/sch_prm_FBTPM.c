#include "default.h"

//================================================================================
#include "EQ_Enum.h"
#include "IO_Part_Data.h"
#include "IOLog.h"
#include "system_tag.h"
#include "User_Parameter.h"
#include "Sch_Prm.h"
#include "sch_Sub.h"
#include "sch_prm_Cluster.h"
#include "Sch_ProcessMonitor.h"

//#include "iolog.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int				MODULE_MOVE_WAIT_CONTROL[ MAX_CHAMBER ]; // 0:NORMAL 1:MOVE-X 2:MOVE-O // 2013.09.03

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
// 2010.11.16
int				BUFFER_STATION_SIDE[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE ];
int				BUFFER_STATION_POINTER[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE ];

int				BUFFER_STATION_FULL_MODE[ MAX_CHAMBER ];

int				BUFFER_STATION_STATUS[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE ];
int				BUFFER_STATION_WAFER[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE ];
*/

// 2010.11.16
int				BUFFER_STATION_SIDE[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE + 1 ];
int				BUFFER_STATION_POINTER[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE + 1 ];

int				BUFFER_STATION_FULL_MODE[ MAX_CHAMBER ];

int				BUFFER_STATION_STATUS[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE + 1 ];
int				BUFFER_STATION_WAFER[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE + 1 ];
int				BUFFER_STATION_WAFER_B[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE + 1 ]; // 2012.03.22

int				BUFFER_MidCount;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_FM_Do_Pointer; // 2011.10.25
//------------------------------------------------------------------------------------------
int				FM_SchedulerDoingPointer[ MAX_CASSETTE_SIDE ];
int				FM_SchedulerLastOutPointer[ MAX_CASSETTE_SIDE ];

BOOL			FM_SchedulerHWait[ MAX_CASSETTE_SIDE ];
int				FM_SchedulerSwWait[ MAX_CASSETTE_SIDE ];

int				FM_Runinig_Flag[ MAX_CASSETTE_SIDE ];
//-------------------------------------------
int				FM_OutCount[ MAX_CASSETTE_SIDE ];
int				FM_InCount[ MAX_CASSETTE_SIDE ];

int				FM_MidCount;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int				FM_MOVING_SIDE[ MAX_CHAMBER ];
int				FM_MOVING_POINTER[ MAX_CHAMBER ];
int				FM_MOVING_WAFER[ MAX_CHAMBER ];
int				FM_MOVING_WAFER_B[ MAX_CHAMBER ]; // 2012.03.22
int				FM_MOVING_MODE[ MAX_CHAMBER ];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int				PM_MOVING_SIDE[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2007.04.30
int				PM_MOVING_WAFER[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ];
int				PM_MOVING_WAFER_B[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2012.03.22
int				PM_MOVING_STATUS[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2007.04.30
int				PM_MOVING_POINTER[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2007.04.30
int				PM_MOVING_PATHORDER[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2007.04.30
int				PM_MOVING_PICKLOCK[ MAX_CHAMBER ][ MAX_PM_SLOT_DEPTH ]; // 2014.01.10 // PM_LOCK_NONE:PM_LOCK_ALL:PM_LOCK_PLACE:PM_LOCK_PICK
int				PM_MOVING_PICKLOCK2[ MAX_CHAMBER ]; // 2018.10.20

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int				TM_MOVING_SIDE[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int				TM_MOVING_SIDE2[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ]; // 2007.04.18
int				TM_MOVING_WAFER[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int				TM_MOVING_WAFER_B[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ]; // 2012.03.22
int				TM_MOVING_POINTER[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int				TM_MOVING_POINTER2[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int 			TM_MOVING_PATHORDER[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int 			TM_MOVING_TYPE[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
int 			TM_MOVING_OUTCH[ MAX_TM_CHAMBER_DEPTH ][ MAX_FINGER_TM ];
//

int				TM_DOUBLE_COUNT_WAFER[ MAX_CASSETTE_SIDE ];
//
int				TM_OutCounter[ MAX_CASSETTE_SIDE ];
int				TM_InCounter[ MAX_CASSETTE_SIDE ];

//
int				TM_SwitchSignal[ MAX_TM_CHAMBER_DEPTH ];
int				TM_MoveSignal[ MAX_TM_CHAMBER_DEPTH ];

int				TM_SchedulerDoingPointer[ MAX_CASSETTE_SIDE ];


//------------------------------------------------------------------------------------------
//
// 2015.08.26
//
int				_TM_PICKPLACING_CHAMBER[ MAX_CHAMBER ];

int				_TM_WILLPLACING_CHAMBER[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM][ MAX_CHAMBER ];
int				_TM_WILLPLACING_MAX_TM;
int				_TM_WILLPLACING_MAX_ARM;

int				_TM_PLACING_TH_CHAMBER[ MAX_CHAMBER ];

void SCH_TM_PICKPLACING_INIT() { // 2015.09.18
	int i , j , k;
	//
	_TM_WILLPLACING_MAX_TM = 0;
	_TM_WILLPLACING_MAX_ARM = 0;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			for ( k = 0 ; k < MAX_CHAMBER ; k++ ) {
				_TM_WILLPLACING_CHAMBER[i][j][k] = 0;
			}
		}
	}
}


extern int EVENT_FLAG_LOG; // 2016.05.09

BOOL _i_SCH_MODULE_GROUP_TPM_POSSIBLE( int grp , int Chamber , int mode ); // 2016.07.28
BOOL _i_SCH_MODULE_GROUP_TBM_POSSIBLE( int grp , int Chamber , int mode , int mdchk ); // 2016.07.28


void SCH_TM_PICKPLACING_SET( int ch , int pick , int tms , int arm , int side , int pointer ) {
	int i , k , m , pd , pr , s;
	switch( pick ) {
	case 10 : // place_reject // 2017.02.08
		//
		_TM_PICKPLACING_CHAMBER[ ch ] = 2;
		//
		break;

	case 11 : // pick_reject // 2017.02.08
		//
		break;

	case 0 : // place
		//
		if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
			if ( _TM_PICKPLACING_CHAMBER[ ch ] != 1 ) {
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
						_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING SET [S=%d][TMS=%d] %s : PLACE(0)%d->1%cMDLPPCHG\n" , s , tms , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _TM_PICKPLACING_CHAMBER[ ch ] , 9 );
					}
				}
			}
		}
		//
		_TM_PICKPLACING_CHAMBER[ ch ] = 1;
		//
		// 2015.09.18
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_TM_WILLPLACING_CHAMBER[tms][arm][i] = 0;
		}
		//
		break;
	case 1 : // pick
		//
		if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
			if ( _TM_PICKPLACING_CHAMBER[ ch ] != 2 ) {
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
						_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING SET [S=%d][TMS=%d] %s : PICK(1)%d->2%cMDLPPCHG\n" , s , tms , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _TM_PICKPLACING_CHAMBER[ ch ] , 9 );
					}
				}
			}
		}
		//
		_TM_PLACING_TH_CHAMBER[ ch ] = 0; // 2017.01.02
		//
		_TM_PICKPLACING_CHAMBER[ ch ] = 2;
		//
		// 2015.09.18
		//
		if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) break;
		//
		if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) break;
		//
		if ( _i_SCH_SYSTEM_USING_GET( side ) < 9 ) break;
		//
		pd = _i_SCH_CLUSTER_Get_PathDo( side , pointer );
		pr = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
		//
		for ( i = pd ; i < pr ; i++ ) {
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , i , k );
				//
				if ( m > 0 ) {
					//
					if ( tms > _TM_WILLPLACING_MAX_TM  ) _TM_WILLPLACING_MAX_TM = tms;
					if ( arm > _TM_WILLPLACING_MAX_ARM ) _TM_WILLPLACING_MAX_ARM = arm;
					//
					if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
						if ( _TM_WILLPLACING_CHAMBER[tms][arm][m] != 1 ) {
							for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
								if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
									_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING SET [S=%d][TMS=%d] %s : PICK(1):WILLPLACING %d->1(%d:%d)%cMDLPPCHG\n" , s , tms , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( m ) , _TM_WILLPLACING_CHAMBER[tms][arm][m] , tms , arm , 9 );
								}
							}
						}
					}
					//
					_TM_WILLPLACING_CHAMBER[tms][arm][m] = 1;
					//
				}
			}
		}
		//
		break;
		//
	case 2 : // 2017.01.02 // place2
		//
		_TM_PLACING_TH_CHAMBER[ ch ] = 1; // 2017.01.02
		//
		break;

	default : // loopreset
		if ( ch == -1 ) {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
//				if ( _TM_PICKPLACING_CHAMBER[ i ] == 2 ) { // 2016.03.22
				if ( _TM_PICKPLACING_CHAMBER[ i ] >= 2 ) { // 2016.03.22
					//
					// 2016.07.28
					//
					if ( ( i >= PM1 ) && ( i < AL ) ) {
						if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , i , G_PICK ) && !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , i , G_PLACE ) ) continue;
					}
					else if ( ( i >= BM1 ) && ( i < TM ) ) {
						if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , i , G_PICK , G_MCHECK_ALL ) && !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , i , G_PLACE , G_MCHECK_ALL ) ) continue;
					}
					//
					if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
						if ( _TM_PICKPLACING_CHAMBER[i] != 0 ) {
							for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
								if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
									_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING SET [S=%d][TMS=%d] %s : LOOP(-1) %d->0%cMDLPPCHG\n" , s , tms , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _TM_PICKPLACING_CHAMBER[i] , 9 );
								}
							}
						}
					}
					//
					_TM_PICKPLACING_CHAMBER[ i ] = 0;
				}
				//
				_TM_PLACING_TH_CHAMBER[ i ] = 0; // 2017.01.02
				//
			}
		}
		else {
//			if ( _TM_PICKPLACING_CHAMBER[ ch ] == 2 ) { // 2016.03.22
			if ( _TM_PICKPLACING_CHAMBER[ ch ] >= 2 ) { // 2016.03.22
				//
				// 2016.07.28
				//
				if ( ( ch >= PM1 ) && ( ch < AL ) ) {
					if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , ch , G_PICK ) && !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , ch , G_PLACE ) ) break;
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
					if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , ch , G_PICK , G_MCHECK_ALL ) && !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , ch , G_PLACE , G_MCHECK_ALL ) ) break;
				}
				//
				if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
					if ( _TM_PICKPLACING_CHAMBER[ch] != 0 ) {
						for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
							if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
								_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING SET [S=%d][TMS=%d] %s : LOOP(CH) %d->0%cMDLPPCHG\n" , s , tms , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _TM_PICKPLACING_CHAMBER[ch] , 9 );
							}
						}
					}
				}
				//
				_TM_PICKPLACING_CHAMBER[ ch ] = 0;
			}
			//
			_TM_PLACING_TH_CHAMBER[ ch ] = 0; // 2017.01.02
			//
		}
		break;
	}
}
//
// 2015.09.18
//
//BOOL SCH_TM_PICKPLACING_GET( int ch ) {
//	if ( _TM_PICKPLACING_CHAMBER[ ch ] > 0 ) return TRUE;
//	return FALSE;
//	//
//}

//
// 2015.09.18
//
int SCH_TM_PICKPLACING_GET( int ch ) {
	//
	int i , j;
	//
	if ( _TM_PICKPLACING_CHAMBER[ ch ] > 0 ) return _TM_PICKPLACING_CHAMBER[ ch ];
	//
	for ( i = 0 ; i <= _TM_WILLPLACING_MAX_TM ; i++ ) {
		for ( j = 0 ; j <= _TM_WILLPLACING_MAX_ARM ; j++ ) {
//			if ( _TM_WILLPLACING_CHAMBER[i][j][ch] != 0 ) return 3; // 2016.03.22
			if ( _TM_WILLPLACING_CHAMBER[i][j][ch] != 0 ) return 4; // 2016.03.22
		}
	}
	//
	return 0;
	//
}

int _i_SCH_SYSTEM_PLACING_TH_GET( int ch ) { // 2017.01.02
	return _TM_PLACING_TH_CHAMBER[ ch ];
}

//void SCH_TM_PICKPLACING_PRCS( int ch ) { // 2016.03.22
void SCH_TM_PICKPLACING_PRCS( int ch , BOOL post ) { // 2016.03.22
	int s;
	//
	if ( _TM_PICKPLACING_CHAMBER[ ch ] == 0 ) {
		//
		if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
				if ( _i_SCH_SYSTEM_USING_GET( s ) != 0 ) {
					_i_SCH_LOG_LOT_PRINTF( s , "TM_PICKPLACING PRCS [S=%d] %s : 0->%d%cMDLPPCHG\n" , s , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , post ? 3 : 1 , 9 );
				}
			}
		}
		//
//		_TM_PICKPLACING_CHAMBER[ ch ] = 1; // 2016.03.22
		_TM_PICKPLACING_CHAMBER[ ch ] = post ? 3 : 1; // 2016.03.22
	}
	//
	_TM_PLACING_TH_CHAMBER[ ch ] = 0; // 2017.01.02
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FEM Station Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_FM_SIDE_WAFER( int arm , int side , int wafer ) {
	FM_MOVING_SIDE[ arm ] = side;
	FM_MOVING_WAFER[ arm ] = wafer;
	if ( wafer > 0 ) FM_MOVING_WAFER_B[ arm ] = wafer; // 2012.03.22
}
//
void _i_SCH_MODULE_Set_FM_SIDE( int arm , int side ) { FM_MOVING_SIDE[ arm ] = side; }
//
void _i_SCH_MODULE_Set_FM_WAFER( int arm , int wafer ) { // 2013.01.11
	FM_MOVING_WAFER[ arm ] = wafer;
	if ( wafer > 0 ) FM_MOVING_WAFER_B[ arm ] = wafer;
}
//
int  _i_SCH_MODULE_Get_FM_SIDE( int arm ) { return FM_MOVING_SIDE[ arm ]; }
int  _i_SCH_MODULE_Get_FM_WAFER( int arm ) { return FM_MOVING_WAFER[ arm ]; }


void _i_SCH_MODULE_Set_FM_WAFER_B( int arm , int wafer ) { FM_MOVING_WAFER_B[ arm ] = wafer; } // 2012.03.22
int  _i_SCH_MODULE_Get_FM_WAFER_B( int arm ) { return FM_MOVING_WAFER_B[ arm ]; } // 2012.03.22

void _i_SCH_MODULE_Set_FM_POINTER( int arm , int data ) { FM_MOVING_POINTER[ arm ] = data; }
int  _i_SCH_MODULE_Get_FM_POINTER( int arm ) { return FM_MOVING_POINTER[ arm ]; }

void _i_SCH_MODULE_Set_FM_MODE( int arm , int data ) { // 2007.09.12
	if ( data != -1 ) {
		//---------------------------------------------------------------------
		FM_MOVING_MODE[ arm ] = data;
		//---------------------------------------------------------------------
		if ( arm == TA_STATION ) {
			if ( FM_MOVING_MODE[ TB_STATION ] == FMWFR_PICK_CM_DONE_AL_GOBM ) FM_MOVING_MODE[ TB_STATION ] = FMWFR_PICK_CM_DONE_AL;
		}
		else {
			if ( FM_MOVING_MODE[ TA_STATION ] == FMWFR_PICK_CM_DONE_AL_GOBM ) FM_MOVING_MODE[ TA_STATION ] = FMWFR_PICK_CM_DONE_AL;
		}
	}
	//---------------------------------------------------------------------
}
int  _i_SCH_MODULE_Get_FM_MODE( int arm ) { return FM_MOVING_MODE[ arm ]; } // 2007.09.12
//Pick CM           = 0		FMWFR_PICK_CM_NEED_AL
//Pick AL(Place BM) = 1		FMWFR_PICK_CM_DONE_AL
//Pick BM           = 2		FMWFR_PICK_BM_NEED_IC
//Pick IC(Place CM) = 3		FMWFR_PICK_BM_DONE_IC
//							FMWFR_PICK_CM_DONE_AL_GOBM

void _i_SCH_MODULE_Set_FM_POINTER_MODE( int arm , int data , int mode ) {
	FM_MOVING_POINTER[ arm ] = data;
	_i_SCH_MODULE_Set_FM_MODE( arm , mode );
}

void _i_SCH_MODULE_Set_FM_Runinig_Flag( int side , int data ) { FM_Runinig_Flag[ side ] = data; }
int	 _i_SCH_MODULE_Get_FM_Runinig_Flag( int side ) { return FM_Runinig_Flag[ side ]; }

void _i_SCH_MODULE_Set_FM_OutCount( int side , int data ) { FM_OutCount[side] = data; }
void _i_SCH_MODULE_Inc_FM_OutCount( int side ) { FM_OutCount[side]++; }
int  _i_SCH_MODULE_Get_FM_OutCount( int side ) { return FM_OutCount[side]; }

void _i_SCH_MODULE_Set_FM_InCount( int side , int data ) { FM_InCount[side] = data; }
void _i_SCH_MODULE_Inc_FM_InCount( int side ) { FM_InCount[side]++; }
int  _i_SCH_MODULE_Get_FM_InCount( int side ) { return FM_InCount[side]; }

void _i_SCH_MODULE_Set_FM_LastOutPointer( int side , int data ) { FM_SchedulerLastOutPointer[side] = data; }
int  _i_SCH_MODULE_Get_FM_LastOutPointer( int side ) { return( FM_SchedulerLastOutPointer[side] ); }

// 2011.10.25
/*
void _i_SCH_MODULE_Set_FM_DoPointer( int side , int data ) { FM_SchedulerDoingPointer[side] = data; }
void _i_SCH_MODULE_Inc_FM_DoPointer( int side ) { FM_SchedulerDoingPointer[side]++; }
int  _i_SCH_MODULE_Get_FM_DoPointer( int side ) { return( FM_SchedulerDoingPointer[side] ); }
//
void _i_SCH_MODULE_Set_FM_End_Status( int side ) { FM_SchedulerDoingPointer[side] = MAX_CASSETTE_SLOT_SIZE; }
//
BOOL _i_SCH_MODULE_Chk_FM_Finish_Status( int side ) { if ( FM_SchedulerDoingPointer[side] >= MAX_CASSETTE_SLOT_SIZE ) return TRUE; return FALSE; }
//
*/
void _i_SCH_MODULE_Enter_FM_DoPointer_Sub() {
	EnterCriticalSection( &CR_FM_Do_Pointer );
}
void _i_SCH_MODULE_Leave_FM_DoPointer_Sub() {
	LeaveCriticalSection( &CR_FM_Do_Pointer );
}

void _i_SCH_MODULE_Set_FM_DoPointer_Sub( int side , int data ) {
	FM_SchedulerDoingPointer[side] = data;
}
void _i_SCH_MODULE_Inc_FM_DoPointer_Sub( int side ) {
	FM_SchedulerDoingPointer[side]++;
}
int  _i_SCH_MODULE_Get_FM_DoPointer_Sub( int side ) {
	return( FM_SchedulerDoingPointer[side] );
}
//
void _i_SCH_MODULE_Set_FM_End_Status_Sub( int side ) {
	FM_SchedulerDoingPointer[side] = MAX_CASSETTE_SLOT_SIZE;
}
//
BOOL _i_SCH_MODULE_Chk_FM_Finish_Status_Sub( int side ) {
	if ( FM_SchedulerDoingPointer[side] >= MAX_CASSETTE_SLOT_SIZE ) return TRUE;
	return FALSE;
}
//
//==================================================================================================================
void _i_SCH_MODULE_Set_FM_DoPointer_FDHC( int side , int data ) {
	EnterCriticalSection( &CR_FM_Do_Pointer );
	_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , data );
	LeaveCriticalSection( &CR_FM_Do_Pointer );
}
void _i_SCH_MODULE_Inc_FM_DoPointer_FDHC( int side ) {
	EnterCriticalSection( &CR_FM_Do_Pointer );
	_i_SCH_MODULE_Inc_FM_DoPointer_Sub( side );
	LeaveCriticalSection( &CR_FM_Do_Pointer );
}
int  _i_SCH_MODULE_Get_FM_DoPointer_FDHC( int side ) {
	int Res;
	EnterCriticalSection( &CR_FM_Do_Pointer );
	Res = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
	LeaveCriticalSection( &CR_FM_Do_Pointer );
	return Res;
}
//
void _i_SCH_MODULE_Set_FM_End_Status_FDHC( int side ) {
	EnterCriticalSection( &CR_FM_Do_Pointer );
	_i_SCH_MODULE_Set_FM_End_Status_Sub( side );
	LeaveCriticalSection( &CR_FM_Do_Pointer );
}
//
BOOL _i_SCH_MODULE_Chk_FM_Finish_Status_FDHC( int side ) {
	BOOL Res;
	EnterCriticalSection( &CR_FM_Do_Pointer );
	Res = _i_SCH_MODULE_Chk_FM_Finish_Status_Sub( side );
	LeaveCriticalSection( &CR_FM_Do_Pointer );
	return Res;
}
//

BOOL _i_SCH_MODULE_Chk_FM_Free_Status( int side ) { if ( FM_OutCount[side] == FM_InCount[side] ) return TRUE; return FALSE; }
//
void _i_SCH_MODULE_Set_FM_HWait( int side , BOOL data ) { FM_SchedulerHWait[side] = data; }
BOOL _i_SCH_MODULE_Get_FM_HWait( int side ) { return FM_SchedulerHWait[side]; }
//
void _i_SCH_MODULE_Set_FM_SwWait( int side , int data ) { FM_SchedulerSwWait[side] = data; }
int  _i_SCH_MODULE_Get_FM_SwWait( int side ) { return FM_SchedulerSwWait[side]; }
//
void _i_SCH_MODULE_Set_FM_MidCount( int data ) { FM_MidCount = data; }
int  _i_SCH_MODULE_Get_FM_MidCount() { return FM_MidCount; }

int _i_SCH_MODULE_Get_FM_SupplyCount( int side ) { return( FM_OutCount[side] - FM_InCount[side] ); } // 2014.12.22
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Buffer Station Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//#include "iolog.h"

//void _i_SCH_MODULE_Set_BM_FULL_MODE( int ch , int Data ) { BUFFER_STATION_FULL_MODE[ch] = Data; }
void _i_SCH_MODULE_Set_BM_FULL_MODE_ONLY( int ch , int Data ) { BUFFER_STATION_FULL_MODE[ch] = Data; }
void _i_SCH_MODULE_Set_BM_FULL_MODE( int ch , int Data ) {
//	char Buffer[256];

//	if ( BUFFER_STATION_FULL_MODE[ch] != Data ) {
//		sprintf( Buffer , "[BM%d] %d => %d\n" , ch - BM1 + 1 , BUFFER_STATION_FULL_MODE[ch] , Data );
//		_IO_MSGEVENT_PRINTF( "BM_MODE" , Buffer );
//	}

	BUFFER_STATION_FULL_MODE[ch] = Data;
	//
	_SCH_IO_SET_MODULE_FLAG_DATA( ch , Data ); // 2013.03.19
	//
}
int  _i_SCH_MODULE_Get_BM_FULL_MODE( int ch ) { return BUFFER_STATION_FULL_MODE[ch]; }

void _i_SCH_MODULE_Set_BM_SIDE_POINTER( int ch , int slot , int side , int pointer ) { BUFFER_STATION_SIDE[ch][slot] = side; BUFFER_STATION_POINTER[ch][slot] = pointer; }
void _i_SCH_MODULE_Set_BM_SIDE( int ch , int slot , int side ) { BUFFER_STATION_SIDE[ch][slot] = side; }
int  _i_SCH_MODULE_Get_BM_SIDE( int ch , int slot ) { return BUFFER_STATION_SIDE[ch][slot]; }
void _i_SCH_MODULE_Set_BM_POINTER( int ch , int slot , int pointer ) { BUFFER_STATION_POINTER[ch][slot] = pointer; }
int  _i_SCH_MODULE_Get_BM_POINTER( int ch , int slot ) { return BUFFER_STATION_POINTER[ch][slot]; }

void _i_SCH_MODULE_Set_BM_WAFER_STATUS( int ch , int slot , int wafer , int status ) {
	BUFFER_STATION_WAFER[ch][slot] = wafer;
//	BUFFER_STATION_STATUS[ch][slot] = status; // 2012.03.28
	if ( ( wafer > 0 ) || ( status >= 0 ) ) BUFFER_STATION_STATUS[ch][slot] = status; // 2012.03.28
	if ( wafer > 0 ) BUFFER_STATION_WAFER_B[ch][slot] = wafer;
}
void _i_SCH_MODULE_Set_BM_STATUS( int ch , int slot , int status ) {
	BUFFER_STATION_STATUS[ch][slot] = status;
}
int  _i_SCH_MODULE_Get_BM_STATUS( int ch , int slot ) {
	return BUFFER_STATION_STATUS[ch][slot];
}
int  _i_SCH_MODULE_Get_BM_WAFER( int ch , int slot ) {
	return BUFFER_STATION_WAFER[ch][slot];
}

void _i_SCH_MODULE_Set_BM_MidCount( int data ) { BUFFER_MidCount = data; }
int  _i_SCH_MODULE_Get_BM_MidCount() { return BUFFER_MidCount; }

void _i_SCH_MODULE_Set_BM_WAFER_B( int ch , int slot , int wafer ) { // 2012.03.22
	BUFFER_STATION_WAFER_B[ch][slot] = wafer;
}
int  _i_SCH_MODULE_Get_BM_WAFER_B( int ch , int slot ) { // 2012.03.22
	return BUFFER_STATION_WAFER_B[ch][slot];
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// TM Station Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_TM_DoPointer( int side , int data ) { TM_SchedulerDoingPointer[side] = data; }
void _i_SCH_MODULE_Inc_TM_DoPointer( int side ) { TM_SchedulerDoingPointer[side]++; }
int  _i_SCH_MODULE_Get_TM_DoPointer( int side ) { return( TM_SchedulerDoingPointer[side] ); }


void _i_SCH_MODULE_Set_TM_InCount( int side , int data ) { TM_InCounter[side] = data; }
void _i_SCH_MODULE_Inc_TM_InCount( int side ) { TM_InCounter[side]++; }
int  _i_SCH_MODULE_Get_TM_InCount( int side ) { return TM_InCounter[side]; }


/*
void _i_SCH_MODULE_Set_TM_InCount( int side , int data ) {

_i_SCH_LOG_LOT_PRINTF( side , "TEST LOG TM_InCount(%d) Set1 (%d)\tTEST_LOG\t\n" , side , data );
	
	TM_InCounter[side] = data;

_i_SCH_LOG_LOT_PRINTF( side , "TEST LOG TM_InCount(%d) Set2 (%d)\tTEST_LOG\t\n" , side , TM_InCounter[side] );

}
void _i_SCH_MODULE_Inc_TM_InCount( int side ) {
_i_SCH_LOG_LOT_PRINTF( side , "TEST LOG TM_InCount(%d) Inc1 (%d)\tTEST_LOG\t\n" , side , TM_InCounter[side] );
	TM_InCounter[side]++;
_i_SCH_LOG_LOT_PRINTF( side , "TEST LOG TM_InCount(%d) Inc2 (%d)\tTEST_LOG\t\n" , side , TM_InCounter[side] );
}
int  _i_SCH_MODULE_Get_TM_InCount( int side ) { return TM_InCounter[side]; }
*/


void _i_SCH_MODULE_Set_TM_OutCount( int side , int data ) { TM_OutCounter[side] = data; }
void _i_SCH_MODULE_Inc_TM_OutCount( int side ) { TM_OutCounter[side]++; }
int  _i_SCH_MODULE_Get_TM_OutCount( int side ) { return TM_OutCounter[side]; }

int _i_SCH_MODULE_Get_TM_SupplyCount( int side ) { return( TM_OutCounter[side] - TM_InCounter[side] ); } // 2014.12.22

void _i_SCH_MODULE_Set_TM_End_Status( int side ) { TM_SchedulerDoingPointer[side] = MAX_CASSETTE_SLOT_SIZE; }

BOOL _i_SCH_MODULE_Chk_TM_Finish_Status( int side ) { if ( TM_SchedulerDoingPointer[side] >= MAX_CASSETTE_SLOT_SIZE ) return TRUE; return FALSE; }
BOOL _i_SCH_MODULE_Chk_TM_Free_Status( int side ) { if ( TM_InCounter[side] == TM_OutCounter[side] ) return TRUE; return FALSE; }
BOOL _i_SCH_MODULE_Chk_TM_Free_Status2( int side , int check ) {
	if ( ( check != 0 ) || ( TM_OutCounter[side] > 0 ) ) {
		if ( TM_InCounter[side] == TM_OutCounter[side] ) return TRUE;
	}
	return FALSE;
}
//
void _i_SCH_MODULE_Set_TM_Switch_Signal( int TMATM , int Data ) { TM_SwitchSignal[TMATM] = Data; }
int  _i_SCH_MODULE_Get_TM_Switch_Signal( int TMATM ) { return( TM_SwitchSignal[TMATM] ); }
//
void _i_SCH_MODULE_Set_TM_Move_Signal( int TMATM , int Data ) { TM_MoveSignal[TMATM] = Data; }
int  _i_SCH_MODULE_Get_TM_Move_Signal( int TMATM ) { return( TM_MoveSignal[TMATM] ); }
//
void _i_SCH_MODULE_Set_TM_SIDE_POINTER( int TMATM , int finger , int side , int pointer , int side2 , int pointer2 ) {
	TM_MOVING_SIDE[TMATM][ finger ] = side;
	TM_MOVING_SIDE2[TMATM][ finger ] = side2; // 2007.04.18
	TM_MOVING_POINTER[TMATM][ finger ] = pointer;
	TM_MOVING_POINTER2[TMATM][ finger ] = pointer2;
}

void _i_SCH_MODULE_Set_TM_SIDE_POINTER1( int TMATM , int finger , int side , int pointer ) {
	TM_MOVING_SIDE[TMATM][ finger ] = side;
	TM_MOVING_POINTER[TMATM][ finger ] = pointer;
}

void _i_SCH_MODULE_Set_TM_SIDE_POINTER2( int TMATM , int finger , int side2 , int pointer2 ) {
	TM_MOVING_SIDE2[TMATM][ finger ] = side2;
	TM_MOVING_POINTER2[TMATM][ finger ] = pointer2;
}

void _i_SCH_MODULE_Set_TM_SIDE( int TMATM , int finger , int side ) { TM_MOVING_SIDE[TMATM][ finger ] = side; }
void _i_SCH_MODULE_Set_TM_POINTER( int TMATM , int finger , int pointer ) { TM_MOVING_POINTER[TMATM][ finger ] = pointer; }
//
int  _i_SCH_MODULE_Get_TM_SIDE( int TMATM , int finger ) { return TM_MOVING_SIDE[TMATM][ finger ]; }
int  _i_SCH_MODULE_Get_TM_SIDE2( int TMATM , int finger ) { return TM_MOVING_SIDE2[TMATM][ finger ]; } // 2007.04.18

int  _i_SCH_MODULE_Get_TM_POINTER( int TMATM , int finger ) { return TM_MOVING_POINTER[TMATM][ finger ]; }
int  _i_SCH_MODULE_Get_TM_POINTER2( int TMATM , int finger ) { return TM_MOVING_POINTER2[TMATM][ finger ]; }
//
int  _i_SCH_MODULE_Get_TM_PATHORDER( int TMATM , int finger ) { return TM_MOVING_PATHORDER[TMATM][ finger ]; }
void _i_SCH_MODULE_Set_TM_PATHORDER( int TMATM , int finger , int data ) { TM_MOVING_PATHORDER[TMATM][ finger ] = data; }
//
int  _i_SCH_MODULE_Get_TM_TYPE( int TMATM , int finger ) { return TM_MOVING_TYPE[TMATM][ finger ]; }
void _i_SCH_MODULE_Set_TM_TYPE( int TMATM , int finger , int data ) { TM_MOVING_TYPE[TMATM][ finger ] = data; }
//
int  _i_SCH_MODULE_Get_TM_WAFER( int TMATM , int finger ) { return TM_MOVING_WAFER[TMATM][ finger ]; }
void _i_SCH_MODULE_Set_TM_WAFER( int TMATM , int finger , int data ) {
	TM_MOVING_WAFER[TMATM][ finger ] = data;
	if ( data > 0 ) TM_MOVING_WAFER_B[TMATM][ finger ] = data;
}
//
int  _i_SCH_MODULE_Get_TM_OUTCH( int TMATM , int finger ) { return TM_MOVING_OUTCH[TMATM][ finger ]; }
void _i_SCH_MODULE_Set_TM_OUTCH( int TMATM , int finger , int data ) { TM_MOVING_OUTCH[TMATM][ finger ] = data; }
//
void _i_SCH_MODULE_Set_TM_DoubleCount( int side , int data ) { TM_DOUBLE_COUNT_WAFER[ side ] = data; }
void _i_SCH_MODULE_Inc_TM_DoubleCount( int side ) { TM_DOUBLE_COUNT_WAFER[ side ]++; }
void _i_SCH_MODULE_Dec_TM_DoubleCount( int side ) { TM_DOUBLE_COUNT_WAFER[ side ]--; }
int  _i_SCH_MODULE_Get_TM_DoubleCount( int side ) { return TM_DOUBLE_COUNT_WAFER[ side ]; }

int  _i_SCH_MODULE_Get_TM_WAFER_B( int TMATM , int finger ) { return TM_MOVING_WAFER_B[TMATM][ finger ]; } // 2012.03.22
void _i_SCH_MODULE_Set_TM_WAFER_B( int TMATM , int finger , int data ) { TM_MOVING_WAFER_B[TMATM][ finger ] = data; } // 2012.03.22
//

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// PM Station Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_PM_SIDE_POINTER( int chamber , int side , int pointer , int side2 , int pointer2 ) {
	PM_MOVING_SIDE[ chamber ][0] = side;
	PM_MOVING_POINTER[ chamber ][0] = pointer;
	PM_MOVING_SIDE[ chamber ][1] = side2; // 2007.04.18
	PM_MOVING_POINTER[ chamber ][1] = pointer2;
}
void _i_SCH_MODULE_Set_PM_SIDE( int chamber , int depth , int side ) { PM_MOVING_SIDE[ chamber ][depth] = side; }
void _i_SCH_MODULE_Set_PM_POINTER( int chamber , int depth , int pointer ) { PM_MOVING_POINTER[ chamber ][depth] = pointer; }
//
void _i_SCH_MODULE_Set_PM_PATHORDER( int chamber , int depth , int data ) { PM_MOVING_PATHORDER[ chamber ][depth] = data; }
//
//void _i_SCH_MODULE_Set_PM_PICKLOCK( int chamber , int depth , int data ) { PM_MOVING_PICKLOCK[ chamber ][depth] = data; } // 2014.01.10 // 2018.10.20
void _i_SCH_MODULE_Set_PM_PICKLOCK( int chamber , int depth , int data ) { // 2018.10.20
	if ( depth < 0 ) {
		PM_MOVING_PICKLOCK2[ chamber ] = data;
	}
	else {
		PM_MOVING_PICKLOCK[ chamber ][depth] = data;
	}
}
//
int  _i_SCH_MODULE_Get_PM_SIDE( int chamber , int depth ) { return PM_MOVING_SIDE[ chamber ][depth]; }
int  _i_SCH_MODULE_Get_PM_POINTER( int chamber , int depth ) { return PM_MOVING_POINTER[ chamber ][depth]; }
//
int  _i_SCH_MODULE_Get_PM_PATHORDER( int chamber , int depth ) { return PM_MOVING_PATHORDER[ chamber ][depth]; }
//
//int  _i_SCH_MODULE_Get_PM_PICKLOCK( int chamber , int depth ) { return PM_MOVING_PICKLOCK[ chamber ][depth]; } // 2014.01.10 // 2018.10.20
int  _i_SCH_MODULE_Get_PM_PICKLOCK( int chamber , int depth ) { // 2018.10.20
	if ( depth < 0 ) {
		return PM_MOVING_PICKLOCK2[ chamber ];
	}
	else {
		return PM_MOVING_PICKLOCK[ chamber ][depth];
	}
}
//
//int  _i_SCH_MODULE_Get_PM_WAFER( int chamber , int depth ) { return PM_MOVING_WAFER[ chamber ][ depth ]; } // 2011.12.06
int  _i_SCH_MODULE_Get_PM_WAFER( int chamber , int depth ) { // 2011.12.06
	int i;
	if ( ( depth >= 0 ) && ( depth < MAX_PM_SLOT_DEPTH ) ) return PM_MOVING_WAFER[ chamber ][ depth ];
	for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
		if ( PM_MOVING_WAFER[ chamber ][i] > 0 ) return 1;
	}
	return 0;
}
void _i_SCH_MODULE_Set_PM_WAFER( int chamber , int depth , int data ) {
	PM_MOVING_WAFER[ chamber ][ depth ] = data;
	if ( data > 0 ) PM_MOVING_WAFER_B[ chamber ][ depth ] = data;
}

int  _i_SCH_MODULE_Get_PM_STATUS( int chamber , int depth ) { return PM_MOVING_STATUS[ chamber ][ depth ]; }
void _i_SCH_MODULE_Set_PM_STATUS( int chamber , int depth , int data ) { PM_MOVING_STATUS[ chamber ][ depth ] = data; }

int  _i_SCH_MODULE_Get_PM_WAFER_B( int chamber , int depth ) { // 2012.03.22
	int i;
	if ( ( depth >= 0 ) && ( depth < MAX_PM_SLOT_DEPTH ) ) return PM_MOVING_WAFER_B[ chamber ][ depth ];
	for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
		if ( PM_MOVING_WAFER_B[ chamber ][i] > 0 ) return 1;
	}
	return 0;
}
void _i_SCH_MODULE_Set_PM_WAFER_B( int chamber , int depth , int data ) { // 2012.03.22
	PM_MOVING_WAFER_B[ chamber ][ depth ] = data;
}
//=======================================================================================
//=======================================================================================
void _i_SCH_MODULE_Init_FBTPMData( int side ) {
	_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , 0 );
	_i_SCH_MODULE_Set_FM_OutCount( side , 0 );
	_i_SCH_MODULE_Set_FM_InCount( side , 0 );
	//
	_i_SCH_MODULE_Set_TM_DoPointer( side , 0 );
	_i_SCH_MODULE_Set_TM_InCount( side , 0 );
	_i_SCH_MODULE_Set_TM_OutCount( side , 0 );
	//
	_i_SCH_MODULE_Set_TM_DoubleCount( side , 0 );
}
//
void INIT_SCHEDULER_PRM_FBTPM_DATA( int apmode , int side ) {
	int i , j;
	//==============================================================================================
	if ( apmode == 0 ) { // 2011.10.25
		InitializeCriticalSection( &CR_FM_Do_Pointer );
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2013.09.03
			MODULE_MOVE_WAIT_CONTROL[ i ] = 0;
		}
		//
	}
	//==============================================================================================
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//
		SCH_TM_PICKPLACING_INIT(); // 2015.09.18
		//
		//==============================================================================================
		_i_SCH_MODULE_Set_FM_MidCount( 0 );
		//
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
//			_i_SCH_MODULE_Set_FM_SIDE_WAFER( j , 0 , 0 ); // 2013.01.11
			_i_SCH_MODULE_Set_FM_WAFER( j , 0 ); // 2013.01.11
			_i_SCH_MODULE_Set_FM_WAFER_B( j , 0 );
		}
		//==============================================================================================
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j + 1 , 0 , BUFFER_READY_STATUS );
				_i_SCH_MODULE_Set_BM_WAFER_B( i , j + 1 , 0 );
			}
//			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_WAIT_STATION ); // 2013.03.19
			_i_SCH_MODULE_Set_BM_FULL_MODE_ONLY( i , BUFFER_WAIT_STATION ); // 2013.03.19
		}
		_i_SCH_MODULE_Set_BM_MidCount( 0 );
		//==============================================================================================
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			_i_SCH_MODULE_Set_TM_Switch_Signal( i , -1 );
			_i_SCH_MODULE_Set_TM_Move_Signal( i , -1 );
		}
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				_i_SCH_MODULE_Set_TM_WAFER( i , j , 0 );
				_i_SCH_MODULE_Set_TM_WAFER_B( i , j , 0 );
			}
		}
		//==============================================================================================
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( i , -1 , 0 ); // 2018.10.20
			//
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				_i_SCH_MODULE_Set_PM_WAFER( i , j , 0 );
				_i_SCH_MODULE_Set_PM_WAFER_B( i , j , 0 );
				//
				_i_SCH_MODULE_Set_PM_PICKLOCK( i , j , 0 ); // 2014.01.10
				//
			}
		}
		//==============================================================================================
		// 2015.08.26
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_TM_PICKPLACING_CHAMBER[ i ] = 0;
			//
			_TM_PLACING_TH_CHAMBER[ i ] = 0; // 2017.01.02
			//
		}
		//==============================================================================================
	}
}
//----------------------------------------------------------------------------
int _i_SCH_MODULE_GET_MOVE_MODE( int Chamber ) { // 2013.09.03
	return MODULE_MOVE_WAIT_CONTROL[ Chamber ];
}
void _i_SCH_MODULE_SET_MOVE_MODE( int Chamber , int data ) { // 2013.09.03
	MODULE_MOVE_WAIT_CONTROL[ Chamber ] = data;
}
//----------------------------------------------------------------------------
int _i_SCH_MODULE_GET_USE_ENABLE_Sub( int Chamber , BOOL Always , int side ) {
	int i , c;
	//
	if ( Always ) {
		switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
		case 0 :
			return( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) );
		default :
			if ( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) ) {
				switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) ) {
				case 0 : // Disable
					if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_WAIT ) {
						return FALSE;
					}
					if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_ABORTWAIT ) {
						return FALSE;
					}
					//
					if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
					//
					return TRUE;
				case 1 : // Enable
//					if ( ( Chamber < PM1 ) || ( Chamber >= ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) return TRUE; // 2005.07.06
//					if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2005.07.06
//					if ( !_i_SCH_SYSTEM_PMMODE_GET( side ) ) return TRUE; // 2005.07.06
//					return FALSE; // 2005.07.06
//					if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.04.15 // 2013.03.20
						if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_WAIT ) {
							return FALSE;
						}
						if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_ABORTWAIT ) {
							return FALSE;
						}
//					} // 2013.03.20
					//
					if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
					//
					return TRUE;
				case 2 : // DisableHW
					return FALSE;
				case 3 : // EnablePM
//					if ( ( Chamber < PM1 ) || ( Chamber >= ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) return TRUE; // 2005.07.06
//					if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2005.07.06
//					if ( _i_SCH_SYSTEM_PMMODE_GET( side ) ) return TRUE; // 2005.07.06
//					return FALSE; // 2005.07.06
//					if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.04.15 // 2013.03.20
						if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_WAIT ) {
							return FALSE;
						}
						if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) == ABORTWAIT_FLAG_ABORTWAIT ) {
							return FALSE;
						}
//					} // 2013.03.20
					//
					if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
					//
					return TRUE;
				default :
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
			break;
		}
	}
	else {
		//
		// 2018.10.20
		//
		if ( ( side != -2 ) && ( ( side / 1000 ) != 1 ) ) {
			//
			if ( ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) || ( ( Chamber >= BM1 ) && ( Chamber < TM ) ) ) {
				c = 0;
				for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( Chamber ) ; i++ ) {
					if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( Chamber , i + 1 ) ) continue;
					c = 1;
					break;
				}
				//
				if ( c == 0 ) return FALSE;
				//
			}
			//
		}
		//
		switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
		case 0 :
			return( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) );
		default :
			if ( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) ) {
				switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) ) {
				case 0 : // Disable
					return FALSE;
				case 1 : // Enable
					if ( ( Chamber < PM1 ) || ( Chamber >= ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) return TRUE; // 2005.07.06
					if ( ( (side%1000) < 0 ) || ( (side%1000) >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2005.07.06
					//
//					if ( !_i_SCH_SYSTEM_PMMODE_GET( (side%1000) ) ) return TRUE; // 2005.07.06 // 2013.09.03
					//
					if ( !_i_SCH_SYSTEM_PMMODE_GET( (side%1000) ) ) { // 2013.09.03
						//
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( (side%1000) ) == 0 ) { // 2013.09.03
							if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 0 ) return TRUE; // 2013.09.03
						}
						else {
							if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 2 ) return TRUE; // 2013.09.03
						}
						//
					}
					//
					return FALSE; // 2005.07.06
				case 2 : // DisableHW
					return FALSE;
				case 3 : // EnablePM
					if ( ( Chamber < PM1 ) || ( Chamber >= ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) return TRUE; // 2005.07.06
					if ( ( (side%1000) < 0 ) || ( (side%1000) >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2005.07.06
					//
//					if ( _i_SCH_SYSTEM_PMMODE_GET( (side%1000) ) ) return TRUE; // 2005.07.06 // 2013.09.03
					//
					if ( _i_SCH_SYSTEM_PMMODE_GET( (side%1000) ) ) { // 2013.09.03
						//
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( (side%1000) ) == 0 ) { // 2013.09.03
							if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 0 ) return TRUE; // 2013.09.03
						}
						else {
							if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 2 ) return TRUE; // 2013.09.03
						}
						//
					}
					//
					return FALSE; // 2005.07.06
				default :
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
			break;
		}
	}
	return TRUE;
}

int _i_SCH_MODULE_GET_USE_ENABLE( int Chamber , BOOL Always , int side ) {
	int res;
	EnterCriticalSection( &CR_WAIT ); // 2009.07.09
	res = _i_SCH_MODULE_GET_USE_ENABLE_Sub( Chamber , Always , side );
	LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
	return res;
}


//==========================================================================================
// 2006.01.04
//==========================================================================================
/*
//
// 2014.09.24
//
//BOOL _i_SCH_MODULE_GROUP_FM_POSSIBLE( int Chamber , int mode , int prcs ) {
	//
	if ( prcs != -2 ) { // 2007.12.12
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , -1 ) ) return FALSE;
	}
	//
	if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
	//
	if      ( mode == G_PICK ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( prcs != -1 ) { // 2007.07.03
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( FM , -1 ) ) return FALSE;
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( Chamber , -1 ) ) return FALSE;
		}
		//=======================================================================
		if ( Chamber < PM1 ) return TRUE; // 2007.02.01
		//=======================================================================
//		if ( Chamber == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE; // 2007.03.20 // 2009.02.09
//		if ( Chamber == _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE; // 2007.03.20 // 2009.02.09
		if ( SCH_Inside_ThisIs_BM_OtherChamber( Chamber , 0 ) ) { // 2009.02.099
			if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
			return TRUE;
		}
		//=======================================================================
		switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
		case 1 :
			if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
			return TRUE;
		case 0 :
			return FALSE;
		}
		if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == 0 ) {
//			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_OUT_MODE ) { // 2008.04.28
			if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_OUT_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_SWITCH_MODE ) ) { // 2008.04.28 // 2009.02.
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
		}
	}
	else if ( mode == G_PLACE ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( prcs != -1 ) { // 2007.07.03
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( FM , -1 ) ) return FALSE;
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( Chamber , -1 ) ) return FALSE;
		}
		//=======================================================================
		if ( Chamber < PM1 ) return TRUE; // 2007.02.01
		//=======================================================================
//		if ( Chamber == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE; // 2007.03.20 // 2009.02.01
//		if ( Chamber == _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE; // 2007.03.20 // 2009.02.01
		if ( SCH_Inside_ThisIs_BM_OtherChamber( Chamber , 0 ) ) { // 2009.02.01
			if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
			return TRUE;
		}
		//=======================================================================
		switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
		case 1 :
			if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
			return TRUE;
		case 0 :
			return FALSE;
		}
		if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == 0 ) {
//			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_IN_MODE ) { // 2008.04.28
			if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_IN_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_SWITCH_MODE ) ) { // 2008.04.28
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
		}
	}
	return FALSE;
}
*/

//
// 2014.09.24
//
BOOL _i_SCH_MODULE_GROUP_FM_POSSIBLE_EX( int Chamber , int mode , int prcs , BOOL fmchk ) {
	//
	if ( prcs != -2 ) { // 2007.12.12
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , -1 ) ) return FALSE;
	}
	//
	if ( fmchk ) {
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( FM , FALSE , -1 ) ) return FALSE; // 2018.11.08
	}
	//
	if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
	//
	if      ( mode == G_PICK ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( prcs != -1 ) { // 2007.07.03
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( FM , -1 ) ) return FALSE;
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( Chamber , -1 ) ) return FALSE;
		}
		//=======================================================================
		if ( Chamber < PM1 ) return TRUE; // 2007.02.01
		//=======================================================================
		if ( ( Chamber >= PM1 ) && ( Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { // 2014.09.24
			//=======================================================================
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
			case 1 :
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			case 0 :
				return FALSE;
			}
			//=======================================================================
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == -1 ) {
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//=======================================================================
		}
		else {
			//
			if ( SCH_Inside_ThisIs_BM_OtherChamber( Chamber , 0 ) ) { // 2009.02.099
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//=======================================================================
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
			case 1 :
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			case 0 :
				return FALSE;
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == 0 ) {
				if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_OUT_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_SWITCH_MODE ) ) { // 2008.04.28 // 2009.02.
					if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
					return TRUE;
				}
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) < 0 ) { // 2018.12.12
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//
		}
	}
	else if ( mode == G_PLACE ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( prcs != -1 ) { // 2007.07.03
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( FM , -1 ) ) return FALSE;
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( Chamber , -1 ) ) return FALSE;
		}
		//=======================================================================
		if ( Chamber < PM1 ) return TRUE; // 2007.02.01
		//=======================================================================
		if ( ( Chamber >= PM1 ) && ( Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { // 2014.09.24
			//=======================================================================
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
			case 1 :
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			case 0 :
				return FALSE;
			}
			//=======================================================================
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == -1 ) {
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//=======================================================================
		}
		else {
			if ( SCH_Inside_ThisIs_BM_OtherChamber( Chamber , 0 ) ) { // 2009.02.01
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//=======================================================================
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , Chamber ) ) {
			case 1 :
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			case 0 :
				return FALSE;
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == 0 ) {
				if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_IN_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Chamber ) == BUFFER_SWITCH_MODE ) ) { // 2008.04.28
					if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
					return TRUE;
				}
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) < 0 ) { // 2018.12.12
				if ( prcs == 1 ) return ( PROCESS_MONITOR_Run_and_Get_Status( Chamber ) <= 0 );
				return TRUE;
			}
			//
		}
	}
	return FALSE;
}

BOOL _i_SCH_MODULE_GROUP_FM_POSSIBLE( int Chamber , int mode , int prcs ) {
	return _i_SCH_MODULE_GROUP_FM_POSSIBLE_EX( Chamber , mode , prcs , TRUE );
}

BOOL _i_SCH_MODULE_GROUP_TBM_POSSIBLE_EX( int grp , int Chamber , int mode , int mdchk , BOOL tmchk ) {
	//
	if ( tmchk ) {
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( TM + grp , FALSE , -1 ) ) return FALSE; // 2018.11.08
	}
	//
	if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
	//
	if      ( mode == G_PICK ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( TM + grp , grp ) ) return FALSE;
		if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( Chamber , grp ) ) return FALSE;
		//==============================================================
		if ( Chamber < PM1 ) return TRUE; // 2010.09.01
		//==============================================================
		switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( grp , Chamber ) ) {
		case 1 :
			return TRUE;
		case 0 :
			return FALSE;
		}
		//
		if     ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp ) {
//			if ( mdchk >= G_MCHECK_SAME ) { // 2008.04.28
			if ( ( mdchk == G_MCHECK_SAME ) || ( mdchk == G_MCHECK_ALL ) ) { // 2008.04.28
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp , Chamber ) == BUFFER_IN_MODE ) return TRUE;
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp , Chamber ) == BUFFER_SWITCH_MODE ) return TRUE; // 2008.04.28
			}
			else {
				return TRUE;
			}
		}
		else if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp + 1 ) {
			if ( mdchk == G_MCHECK_ALL ) {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp + 1 , Chamber ) == BUFFER_OUT_MODE ) return TRUE;
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp + 1 , Chamber ) == BUFFER_SWITCH_MODE ) return TRUE; // 2011.03.15
			}
		}
	}
	else if ( mode == G_PLACE ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( TM + grp , grp ) ) return FALSE;
		if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( Chamber , grp ) ) return FALSE;
		//==============================================================
		if ( Chamber < PM1 ) return TRUE; // 2010.09.01
		//==============================================================
		switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( grp , Chamber ) ) {
		case 1 :
			return TRUE;
		case 0 :
			return FALSE;
		}
		if     ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp ) {
//			if ( mdchk >= G_MCHECK_SAME ) { // 2008.04.28
			if ( ( mdchk == G_MCHECK_SAME ) || ( mdchk == G_MCHECK_ALL ) ) { // 2008.04.28
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp , Chamber ) == BUFFER_OUT_MODE ) return TRUE;
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp , Chamber ) == BUFFER_SWITCH_MODE ) return TRUE; // 2008.04.28
			}
			else {
				return TRUE;
			}
		}
		else if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp + 1 ) {
			if ( mdchk == G_MCHECK_ALL ) {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp + 1 , Chamber ) == BUFFER_IN_MODE ) return TRUE;
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( grp + 1 , Chamber ) == BUFFER_SWITCH_MODE ) return TRUE; // 2011.03.15
			}
		}
	}
	else {
		if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp ) return TRUE;
	}
	return FALSE;
}

BOOL _i_SCH_MODULE_GROUP_TBM_POSSIBLE( int grp , int Chamber , int mode , int mdchk ) {
	return _i_SCH_MODULE_GROUP_TBM_POSSIBLE_EX( grp , Chamber , mode , mdchk , TRUE );
}

BOOL _i_SCH_MODULE_GROUP_TPM_POSSIBLE_EX( int grp , int Chamber , int mode , BOOL tmchk ) {
	//
	if ( tmchk ) {
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( TM + grp , FALSE , -1 ) ) return FALSE; // 2018.11.08
	}
	//
	if ( MODULE_MOVE_WAIT_CONTROL[Chamber] == 1 ) return FALSE; // 2013.09.03
	//
	if      ( mode == G_PICK ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( TM + grp , grp ) ) return FALSE;
		if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( Chamber , grp ) ) return FALSE;
		//==============================================================
		if ( Chamber < PM1 ) return TRUE; // 2010.09.01
		//==============================================================
		switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( grp , Chamber ) ) {
		case 1 :
			return TRUE;
		case 0 :
			return FALSE;
		}
	}
	else if ( mode == G_PLACE ) {
		//==============================================================
		// 2007.01.23
		//==============================================================
		if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( TM + grp , grp ) ) return FALSE;
		if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( Chamber , grp ) ) return FALSE;
		//==============================================================
		if ( Chamber < PM1 ) return TRUE; // 2010.09.01
		//==============================================================
		switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( grp , Chamber ) ) {
		case 1 :
			return TRUE;
		case 0 :
			return FALSE;
		}
	}
	//
	if ( _i_SCH_PRM_GET_MODULE_GROUP( Chamber ) == grp ) return TRUE;
	//
	return FALSE;
}


BOOL _i_SCH_MODULE_GROUP_TPM_POSSIBLE( int grp , int Chamber , int mode ) {
	return _i_SCH_MODULE_GROUP_TPM_POSSIBLE_EX( grp , Chamber , mode , TRUE );
}
