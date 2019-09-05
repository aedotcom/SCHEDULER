#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "System_Tag.h"
#include "User_Parameter.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
//-------------------------------------------
BOOL			DisableDisappear[ MAX_CASSETTE_SIDE ]; // 2003.12.10
//-------------------------------------------
int				Last_Check_Flag[ MAX_CASSETTE_SIDE ];
//-------------------------------------------
BOOL			FIRST_MODULE_CONTROL;
BOOL			FIRST_MODULE_EXPEDITE[ MAX_CHAMBER ];
BOOL			SECOND_MODULE_EXPEDITE[ MAX_CHAMBER ];
//------------------------------------------------------------------------------------------
int				GLOBAL_STOP[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2005.07.19
int				GLOBAL_STOP_FLAG[ MAX_CASSETTE_SIDE ]; // 2005.07.19
//------------------------------------------------------------------------------------------
int				Mdl_Use_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER];
int				Mdl_Run_Flag[MAX_CHAMBER];
int				Mdl_Run_AbortWait_Flag[MAX_CHAMBER]; // 2003.06.12
int				Mdl_Spd_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2006.11.07
//
//BOOL			Chamber_Intlks_Flag[MAX_CHAMBER]; // 2003.02.12
BOOL			Chamber_Intlks_Flag_Pre[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2003.02.12
BOOL			Chamber_Intlks_Flag_Run[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2003.02.12
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// DISPPEAR Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Get_Disable_Disappear( int side ) { // 2003.12.10
	return DisableDisappear[ side ];
}
void SCHEDULER_CONTROL_Set_Disable_Disappear( int side , BOOL data ) { // 2003.12.10
	DisableDisappear[ side ] = data;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// First Time Delay Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_FIRST_MODULE_CONTROL_SET( BOOL Use ) {
	FIRST_MODULE_CONTROL = Use;
}

void SCHEDULER_FIRST_MODULE_EXPEDITE_SET( int ch , BOOL Use ) {
	FIRST_MODULE_EXPEDITE[ch] = Use;
}

void SCHEDULER_SECOND_MODULE_EXPEDITE_SET( int ch , BOOL Use ) {
	SECOND_MODULE_EXPEDITE[ch] = Use;
}

BOOL _i_SCH_MODULE_FIRST_CONTROL_GET() {
	return FIRST_MODULE_CONTROL;
}

BOOL _i_SCH_MODULE_FIRST_EXPEDITE_GET( int ch ) {
	return FIRST_MODULE_EXPEDITE[ch];
}

BOOL _i_SCH_MODULE_SECOND_EXPEDITE_GET( int ch ) {
	return SECOND_MODULE_EXPEDITE[ch];
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Module Status Control Use Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int EVENT_FLAG_LOG; // 2015.05.26
void _i_SCH_LOG_LOT_PRINTF( int mode , LPSTR , ... );
char *_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( int data );

void _i_SCH_MODULE_Set_Mdl_Spd_Flag( int side , int ch , int data ) { Mdl_Spd_Flag[side][ch] = data; }
int  _i_SCH_MODULE_Get_Mdl_Spd_Flag( int side , int ch ) { return Mdl_Spd_Flag[side][ch]; }

//void _i_SCH_MODULE_Set_Mdl_Run_Flag( int ch , int data ) { Mdl_Run_Flag[ch] = data; } // 2015.05.26
//void _i_SCH_MODULE_Inc_Mdl_Run_Flag( int ch ) { Mdl_Run_Flag[ch]++; } // 2015.05.26
//void _i_SCH_MODULE_Dec_Mdl_Run_Flag( int ch ) { Mdl_Run_Flag[ch]--; } // 2015.05.26

void _i_SCH_MODULE_Set_Mdl_Run_Flag( int ch , int data ) {
	int i; // 2015.05.26
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
		if ( Mdl_Run_Flag[ch] != data ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( _i_SCH_SYSTEM_USING_GET( i ) != 0 ) {
					_i_SCH_LOG_LOT_PRINTF( i , "Mdl_Run_Flag SET [S=%d] %s : %d->%d <%d>%cMDLCNTCHG\n" , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Mdl_Run_Flag[ch] , data , Mdl_Use_Flag[i][ch] , 9 );
				}
			}
		}
	}
	//
	Mdl_Run_Flag[ch] = data;
} // 2015.05.26
void _i_SCH_MODULE_Inc_Mdl_Run_Flag( int ch ) {
	int i; // 2015.05.26
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) != 0 ) {
				_i_SCH_LOG_LOT_PRINTF( i , "Mdl_Run_Flag INC [S=%d] %s : %d->%d <%d>%cMDLCNTCHG\n" , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Mdl_Run_Flag[ch] , Mdl_Run_Flag[ch]+1 , Mdl_Use_Flag[i][ch] , 9 );
			}
		}
	}
	//
	Mdl_Run_Flag[ch]++;
	//
} // 2015.05.26
void _i_SCH_MODULE_Dec_Mdl_Run_Flag( int ch ) {
	int i; // 2015.05.26
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) != 0 ) {
				_i_SCH_LOG_LOT_PRINTF( i , "Mdl_Run_Flag DEC [S=%d] %s : %d->%d <%d>%cMDLCNTCHG\n" , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Mdl_Run_Flag[ch] , Mdl_Run_Flag[ch]-1 , Mdl_Use_Flag[i][ch] , 9 );
			}
		}
	}
	//
	Mdl_Run_Flag[ch]--;
	//
} // 2015.05.26

int  _i_SCH_MODULE_Get_Mdl_Run_Flag( int ch ) { return Mdl_Run_Flag[ch]; }



//void _i_SCH_MODULE_Set_Mdl_Use_Flag( int side , int ch , int data ) { Mdl_Use_Flag[side][ch] = data; } // 2015.05.26

void _i_SCH_MODULE_Set_Mdl_Use_Flag( int side , int ch , int data ) { // 2015.05.26
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 2 ) ) {
		_i_SCH_LOG_LOT_PRINTF( side , "Mdl_Use_Flag CHG [S=%d] %s : %d->%d <%d>%cMDLSTSCHG\n" , side , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Mdl_Use_Flag[side][ch] , data , Mdl_Run_Flag[ ch ] , 9 );
	}
	//
	Mdl_Use_Flag[side][ch] = data;
	//
}

int  _i_SCH_MODULE_Get_Mdl_Use_Flag( int side , int ch ) { return Mdl_Use_Flag[side][ch]; }



//void SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( int ch , BOOL data ) { Chamber_Intlks_Flag[ch] = data; }
//BOOL SCHEDULER_CONTROL_Get_Chamber_Use_Interlock( int ch ) { return Chamber_Intlks_Flag[ch]; }
void SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( int ch , int data ) { Mdl_Run_AbortWait_Flag[ch] = data; } // 2003.06.13
int  SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( int ch ) { return Mdl_Run_AbortWait_Flag[ch]; } // 2003.06.13
//
void SCHEDULER_CONTROL_Init_Mdl_Run_AbortWait_Flag() {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Mdl_Run_AbortWait_Flag[i] = 0;
	}
} // 2007.05.09

void SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( int side , int ch , BOOL data ) { Chamber_Intlks_Flag_Pre[side][ch] = data; }
BOOL SCHEDULER_CONTROL_Get_Chamber_Use_Interlock_Pre( int side , int ch ) { return Chamber_Intlks_Flag_Pre[side][ch]; }

void _i_SCH_MODULE_Set_Use_Interlock_Run( int side , int ch , BOOL data ) { Chamber_Intlks_Flag_Run[side][ch] = data; }
BOOL _i_SCH_MODULE_Get_Use_Interlock_Run( int side , int ch ) { return Chamber_Intlks_Flag_Run[side][ch]; }

void _i_SCH_MODULE_Set_Use_Interlock_Map( int side ) {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) Chamber_Intlks_Flag_Run[side][i] = Chamber_Intlks_Flag_Pre[side][i];
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Global Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SUB_Chk_First_Out_Status( int side , int SchPointer ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			if ( !SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , i ) , 1 ) ) { // 2010.09.13
				if ( SchPointer == i ) return TRUE;
				else return FALSE;
			}
		}
	}
	return FALSE;
}
//
BOOL _i_SCH_SUB_Chk_Last_Out_Status( int side , int SchPointer ) {
	int i;
	for ( i = SchPointer + 1 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			//
//			if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue; // 2011.04.18 // 2011.05.11
			//
//			return FALSE; // 2007.05.18
//			if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2009.02.09
			if ( !SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , i ) , 1 ) ) { // 2009.02.09
				return FALSE; // 2007.05.18
			}
		}
	}
	return TRUE;
}
//
void _i_SCH_SUB_Set_Last_Status( int side , BOOL data ) {
	Last_Check_Flag[ side ] = data;
}
//
BOOL _i_SCH_SUB_Get_Last_Status( int side ) {
	return Last_Check_Flag[ side ];
}

BOOL SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( int side , int SchPointer , int finc ) {
	int i , dmocnt;
//	for ( i = SchPointer + 1 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2004.06.02
//		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) return FALSE; // 2004.06.02
//	} // 2004.06.02
	//
	//-----------------------------------------------------------------------------------------------------------------------
	// 2009.02.12
	//-----------------------------------------------------------------------------------------------------------------------
	if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , SchPointer ) , 1 ) ) return FALSE;
	i    = _i_SCH_CLUSTER_Get_PathRun( side , SchPointer , 0 , 2 );
	if ( i < _i_SCH_CLUSTER_Get_PathRun( side , SchPointer , 10 , 2 ) ) return FALSE;
	//-----------------------------------------------------------------------------------------------------------------------
	dmocnt = 0;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2004.06.02
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( SchPointer != i ) { // 2004.06.02
			if ( !SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , i ) , 1 ) ) { // 2009.02.12
				if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) == PATHDO_WAFER_RETURN ) return FALSE; // 2004.06.02
//				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) > _i_SCH_CLUSTER_Get_PathDo( side , i ) ) return FALSE; // 2004.06.02 // 2018.11.12
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) > _i_SCH_CLUSTER_Get_PathDo( side , i ) ) { // 2004.06.02 // 2018.11.12
					if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_CM_END ) { // 2018.11.12
						return FALSE;
					}
				}
			}
			else { // 2009.02.12
				switch( _i_SCH_CLUSTER_Get_PathStatus( side , i ) ) {
				case SCHEDULER_READY			:
				case SCHEDULER_CM_END			:
					break;
				default							:
					dmocnt++;
					break;
				}
			}
		}
	} // 2004.06.02
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
//	if ( _i_SCH_MODULE_Get_FM_InCount( side ) >= _i_SCH_MODULE_Get_FM_OutCount( side ) ) { // 2005.06.28
		if ( ( _i_SCH_MODULE_Get_FM_InCount( side ) + finc + dmocnt ) >= _i_SCH_MODULE_Get_FM_OutCount( side ) ) { // 2005.06.28
			return TRUE;
		}
	}
	else {
		if ( ( _i_SCH_MODULE_Get_TM_InCount( side ) + finc + dmocnt ) >= _i_SCH_MODULE_Get_TM_OutCount( side ) ) { // 2005.06.28
			return TRUE;
		}
	}
	return FALSE;
}
//

//=======================================================================================
//=======================================================================================
/*
// 2014.11.28
void SCHEDULER_CONTROL_Set_GLOBAL_STOP( int SlotMode , int mdl_jc , int slot_jp , BOOL data ) { // 2007.11.08
	int s , p;
	//
	if      ( SlotMode == 0 ) { // cj // 2012.04.01
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( s , p ) != mdl_jc ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) >= 0 ) {
					if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) != slot_jp ) continue;
				}
				//
				GLOBAL_STOP_FLAG[ s ] = data;
				GLOBAL_STOP[ s ][ p ] = data;
				//
			}
		}
	}
	else if ( SlotMode == 1 ) { // slot
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
				if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) != mdl_jc ) continue;
				if ( slot_jp > 0 ) {
					if ( _i_SCH_CLUSTER_Get_SlotIn( s , p ) != slot_jp ) continue;
				}
				//
				GLOBAL_STOP_FLAG[ s ] = data;
				GLOBAL_STOP[ s ][ p ] = data;
				//
			}
		}
	}
	else {
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			GLOBAL_STOP_FLAG[ s ] = data;
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				GLOBAL_STOP[ s ][ p ] = data;
			}
		}
	}
	//
}
*/

// 2013.11.28
void SCHEDULER_CONTROL_Set_GLOBAL_STOP( int SlotMode , int mdl_jc , int slot_jp , int data ) {
	int s , p;
	//
	if      ( SlotMode == 0 ) { // cj // 2012.04.01
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( s , p ) != mdl_jc ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) >= 0 ) {
					if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) != slot_jp ) continue;
				}
				//
//				GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
//				GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
				//
				GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
				GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
				//
			}
		}
	}
	else if ( SlotMode == 1 ) { // slot
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
				if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) != mdl_jc ) continue;
				if ( slot_jp > 0 ) {
					if ( _i_SCH_CLUSTER_Get_SlotIn( s , p ) != slot_jp ) continue;
				}
				//
//				GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
//				GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
				//
				GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
				GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
				//
			}
		}
	}
	else {
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			GLOBAL_STOP_FLAG[ s ] = FALSE;
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				GLOBAL_STOP[ s ][ p ] = FALSE;
			}
		}
	}
	//
}

void SCHEDULER_CONTROL_Set_GLOBAL_STOP_TEST() { // 2015.06.01 XXXX
	//
	GLOBAL_STOP[ 0 ][ 2 ] = 1;
	GLOBAL_STOP_FLAG[ 0 ] = TRUE;
	//
		if ( _i_SCH_SYSTEM_USING_RUNNING( 0 ) ) {
			_i_SCH_SYSTEM_MODE_LOOP_SET( 0 , 2 );
		}
	//
}

void SCHEDULER_CONTROL_Set_GLOBAL_STOP_FOR_JOB( int jc , int jp , int data ) { // 2014.05.21
	int s , p;
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( s , p ) != jc ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) >= 0 ) {
				if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) != jp ) continue;
			}
			//
//			GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
//			GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
			//
			GLOBAL_STOP[ s ][ p ] = data; // 2015.06.01
			GLOBAL_STOP_FLAG[ s ] = TRUE; // 2015.06.01
			//
			if ( data == 1 ) {
				if ( _i_SCH_SYSTEM_USING_RUNNING( s ) ) {
					_i_SCH_SYSTEM_MODE_LOOP_SET( s , 2 );
				}
			}
			//
		}
	}
	//
}

int SCHEDULER_CONTROL_Get_GLOBAL_STOP_Other_Pointer( int side , int pointer , int *rside ) { // 2011.07.21
	int i , j , s1 , s2 , p1 , p2 , w1 , w2;
	//
	if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() <= 0 ) return -2;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -3;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return -4;
	//
	//=========================================================================================================================================================
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
		//
		for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			//
			if ( _i_SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
			//
			if ( _i_SCH_MODULE_Get_BM_SIDE( i , j ) != side ) continue;
			if ( _i_SCH_MODULE_Get_BM_POINTER( i , j ) != pointer ) continue;
			//
			if ( ( j - 1 ) % 2 == 0 ) {
				if ( _i_SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) { // 2015.06.01
					*rside = _i_SCH_MODULE_Get_BM_SIDE( i , j + 1 );
					return _i_SCH_MODULE_Get_BM_POINTER( i , j + 1 );
				}
				//
				j++; // 2015.06.01
				//
			}
			else {
				if ( _i_SCH_MODULE_Get_BM_WAFER( i , j - 1 ) > 0 ) { // 2015.06.01
					*rside = _i_SCH_MODULE_Get_BM_SIDE( i , j - 1 );
					return _i_SCH_MODULE_Get_BM_POINTER( i , j - 1 );
				}
			}
		}
	}
	//=========================================================================================================================================================
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( TM + i ) ) continue;
		//
		//=========================================================================================================================================================
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			//
			if ( j >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
			//
			if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , j ) ) continue;
			//
			if ( _i_SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) continue;
			//
			s1 = _i_SCH_MODULE_Get_TM_SIDE( i , j );
			s2 = _i_SCH_MODULE_Get_TM_SIDE2( i , j );
			//
			p1 = _i_SCH_MODULE_Get_TM_POINTER( i , j );
			p2 = _i_SCH_MODULE_Get_TM_POINTER2( i , j );
			//
			w1 = _i_SCH_MODULE_Get_TM_WAFER( i , j ) % 100;
			w2 = _i_SCH_MODULE_Get_TM_WAFER( i , j ) / 100;
			//
			if      ( ( w1 > 0 ) && ( s1 == side ) && ( p1 == pointer ) ) {
				if ( w2 > 0 ) { // 2012.04.01
					*rside = s2;
					return p2;
				}
			}
			else if ( ( w2 > 0 ) && ( s2 == side ) && ( p2 == pointer ) ) {
				if ( w1 > 0 ) { // 2012.04.01
					*rside = s1;
					return p1;
				}
			}
		}
		//=========================================================================================================================================================
	}
	//=========================================================================================================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
		//
		for ( j = 0 ; j < 2 ; j++ ) {
			//
			if ( _i_SCH_MODULE_Get_PM_WAFER( i , j ) <= 0 ) continue;
			//
			if ( _i_SCH_MODULE_Get_PM_SIDE( i , j ) != side ) continue;
			if ( _i_SCH_MODULE_Get_PM_POINTER( i , j ) != pointer ) continue;
			//
			if ( ( j % 2 ) == 0 ) {
				if ( _i_SCH_MODULE_Get_PM_WAFER( i , j + 1 ) > 0 ) { // 2015.06.01
					*rside = _i_SCH_MODULE_Get_PM_SIDE( i , j + 1 );
					return _i_SCH_MODULE_Get_PM_POINTER( i , j + 1 );
				}
			}
			else {
				if ( _i_SCH_MODULE_Get_PM_WAFER( i , j - 1 ) > 0 ) { // 2015.06.01
					*rside = _i_SCH_MODULE_Get_PM_SIDE( i , j - 1 );
					return _i_SCH_MODULE_Get_PM_POINTER( i , j - 1 );
				}
			}
		}
	}
	return -1;
}
//=======================================================================================
/*
// 2013.11.28
void _i_SCH_SUB_Chk_GLOBAL_STOP_FDHC( int side ) { // 2007.11.08
	int i , j , p , ss , op;
	//
//	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25 // 2012.05.04
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( ( side != -1 ) && ( i != side ) ) continue;
		//
		if ( !GLOBAL_STOP_FLAG[ i ] ) continue;
		//
		GLOBAL_STOP_FLAG[ i ] = FALSE;
		//
		_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2012.05.04
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( GLOBAL_STOP[ i ][ j ] ) {
				//
				if ( _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( i , &p , FALSE , FALSE , j ) ) {
					//
					op = SCHEDULER_CONTROL_Get_GLOBAL_STOP_Other_Pointer( i , p , &ss );
					//
					if ( op >= 0 ) _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op );
					//
				}
				//
				GLOBAL_STOP[ i ][ j ] = FALSE;
			}
			//
		}
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			_i_SCH_SUB_Remain_for_CM( i );
		}
		else {
			_i_SCH_SUB_Remain_for_FM_Sub( i );
		}
		//
		_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2012.05.04
		//
	}
	//
//	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25 // 2012.05.04
	//
}
*/

// 2013.11.28

void _i_SCH_SUB_Chk_GLOBAL_STOP_FDHC( int side ) { // 2007.11.08
	int i , j , p , ss , op;
	//
//	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25 // 2012.05.04
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( ( side != -1 ) && ( i != side ) ) continue;
		//
		if ( !GLOBAL_STOP_FLAG[ i ] ) continue;
		//
		GLOBAL_STOP_FLAG[ i ] = FALSE;
		//
		if ( !_i_SCH_SYSTEM_USING_STARTING( i ) ) { // 2018.09.10
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				GLOBAL_STOP[ i ][ j ] = 0;
			}
			//
			continue;
		}
		//
		_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2012.05.04
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( GLOBAL_STOP[ i ][ j ] > 0 ) {
				//
				if ( _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( i , &p , FALSE , FALSE , j , GLOBAL_STOP[ i ][ j ] == 2 ) ) {
					//
					op = SCHEDULER_CONTROL_Get_GLOBAL_STOP_Other_Pointer( i , p , &ss );
					//
					if ( op >= 0 ) {
						//
						//_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op , GLOBAL_STOP[ i ][ j ] == 2 ); // 2014.07.17
						//
						// 2014.07.17
						//
						if ( ( ss == i ) || GLOBAL_STOP_FLAG[ ss ] ) {
							//
							if      ( GLOBAL_STOP[ ss ][ op ] == 1 ) {
								//
								_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op , FALSE );
								//
								GLOBAL_STOP[ ss ][ op ] = 0;
								//
							}
							else if ( GLOBAL_STOP[ ss ][ op ] == 2 ) {
								//
								_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op , TRUE );
								//
								GLOBAL_STOP[ ss ][ op ] = 0;
								//
							}
							else {
								//
								_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op , TRUE );
								//
							}
						}
						else {
							//
							_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( ss , &p , FALSE , FALSE , op , TRUE );
							//
						}
						//
					}
					//
				}
				//
				GLOBAL_STOP[ i ][ j ] = 0;
			}
			//
		}
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			_i_SCH_SUB_Remain_for_CM( i , FALSE );
		}
		else {
			_i_SCH_SUB_Remain_for_FM_Sub( i , FALSE );
		}
		//
		_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2012.05.04
		//
	}
	//
//	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25 // 2012.05.04
	//
}



int SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER = -1; // 2010.10.20
int SCHEDULER_FORCE_MAKE_FMSIDE_TAG = 0;
//int  SCHEDULER_FORCE_MAKE_FMSIDE_DATA = 0; // 2010.10.20
int  SCHEDULER_FORCE_MAKE_FMSIDE_DATA[MAX_CHAMBER]; // 2010.10.20

void _i_SCH_SUB_Make_FORCE_FMSIDE( int mdl ) {
	int i;
	if      ( mdl == 0 ) {
		SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER = -1; // 2010.10.20
		//
		SCHEDULER_FORCE_MAKE_FMSIDE_TAG = 0;
//		SCHEDULER_FORCE_MAKE_FMSIDE_DATA = 0; // 2010.10.20
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			SCHEDULER_FORCE_MAKE_FMSIDE_DATA[i] = 0; // 2010.10.20
		}
	}
	else if ( mdl == -1 ) { // 2010.10.20
		SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER = -1; // 2010.10.20
		//
		SCHEDULER_FORCE_MAKE_FMSIDE_TAG = 1;
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			SCHEDULER_FORCE_MAKE_FMSIDE_DATA[i] = 0;
		}
	}
	else if ( mdl >= 1000 ) { // 2010.10.20
		mdl = ( mdl % 1000 );
		if ( mdl < MAX_CHAMBER ) {
			SCHEDULER_FORCE_MAKE_FMSIDE_DATA[mdl] = 0; // 2010.10.20
		}
	}
	else {
//		SCHEDULER_FORCE_MAKE_FMSIDE_DATA = mdl; // 2010.10.20
//		SCHEDULER_FORCE_MAKE_FMSIDE_TAG = 1; // 2010.10.20
		if ( ( mdl >= 0 ) && ( mdl < MAX_CHAMBER ) ) {
			SCHEDULER_FORCE_MAKE_FMSIDE_DATA[mdl] = 1; // 2010.10.20
		}
	}
}

BOOL _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( int *mdl ) {
	int i;
	//
	if      ( SCHEDULER_FORCE_MAKE_FMSIDE_TAG == 1 ) {
		*mdl = -1;
		return TRUE;
	}
	else {
		for ( i = (SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER+1) ; i < MAX_CHAMBER ; i++ ) {
			if ( SCHEDULER_FORCE_MAKE_FMSIDE_DATA[i] != 0 ) {
				*mdl = i;
				SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER = i;
				return TRUE;
			}
		}
		for ( i = 0 ; i < (SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER+1) ; i++ ) {
			if ( SCHEDULER_FORCE_MAKE_FMSIDE_DATA[i] != 0 ) {
				*mdl = i;
				SCHEDULER_FORCE_MAKE_FMSIDE_CHECK_ORDER = i;
				return TRUE;
			}
		}
	}
	//
	*mdl = 0;
	return FALSE;
	//
//	*mdl = SCHEDULER_FORCE_MAKE_FMSIDE_DATA; // 2010.10.20
//	if ( *mdl == 0 ) return FALSE; // 2010.10.20
//	return SCHEDULER_FORCE_MAKE_FMSIDE_TAG; // 2010.10.20
}




int SCHEDULER_ENABLE_TO_ENABLEN = 0; // 2015.04.03


void _i_SCH_SUB_Make_ENABLE_TO_ENABLEN( int data ) { // 2015.04.03
	switch( data ) {
	case 0 :	SCHEDULER_ENABLE_TO_ENABLEN	= 0;	break;
	case 1 :	SCHEDULER_ENABLE_TO_ENABLEN	= 1;	break;
	case 2 :	SCHEDULER_ENABLE_TO_ENABLEN	= 2;	break;
	case 3 :	SCHEDULER_ENABLE_TO_ENABLEN	= 3;	break;
	case 10 :	SCHEDULER_ENABLE_TO_ENABLEN	= 10;	break;
	case 11 :	SCHEDULER_ENABLE_TO_ENABLEN	= 11;	break;
	case 12 :	SCHEDULER_ENABLE_TO_ENABLEN	= 12;	break;
	case 13 :	SCHEDULER_ENABLE_TO_ENABLEN	= 13;	break;
	}
}


int  _i_SCH_SUB_Get_ENABLE_TO_ENABLEN() { // 2015.04.03
	switch( SCHEDULER_ENABLE_TO_ENABLEN ) {
	case 0 :	return 0;	break;
	case 1 :	return 1;	break;
	case 2 :	return 2;	break;
	case 3 :	return 3;	break;
	case 10 :	return 0;	break;
	case 11 :	return 1;	break;
	case 12 :	return 2;	break;
	case 13 :	return 3;	break;
	}
	return 0;
}

void  _i_SCH_SUB_Chk_ENABLE_TO_ENABLEN() { // 2015.04.03
	switch( SCHEDULER_ENABLE_TO_ENABLEN ) {
	case 10 :	SCHEDULER_ENABLE_TO_ENABLEN = 0;	break;
	case 11 :	SCHEDULER_ENABLE_TO_ENABLEN = 0;	break;
	case 12 :	SCHEDULER_ENABLE_TO_ENABLEN = 0;	break;
	case 13 :	SCHEDULER_ENABLE_TO_ENABLEN = 0;	break;
	}
}


void INIT_SCHEDULER_PRM_DATA( int apmode , int side ) {
	int i , j;
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Init_Mdl_Run_AbortWait_Flag(); // 2007.05.09
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
			for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( j , i , MUF_00_NOTUSE );
				_i_SCH_MODULE_Set_Use_Interlock_Run( j , i , FALSE ); // 2003.02.12
				//
				SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( j , i , FALSE ); // 2003.02.12
			}
		}
		//=============================================
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) SCHEDULER_FIRST_MODULE_EXPEDITE_SET( i , FALSE );
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) SCHEDULER_SECOND_MODULE_EXPEDITE_SET( i , FALSE );
		//=============================================
		_i_SCH_SUB_Make_FORCE_FMSIDE( 0 );
		//=============================================
		_i_SCH_SUB_Chk_ENABLE_TO_ENABLEN(); // 2015.04.03
		//=============================================
	}
}


