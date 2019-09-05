#include "default.h"

#include "EQ_Enum.h"

#include "resource.h"
#include "System_Tag.h"
#include "DLL_Interface.h"
#include "Sch_sub.h"
#include "Equip_Handling.h"
#include "Robot_Handling.h"
#include "User_Parameter.h"
#include "sch_prm_FBTPM.h"
#include "sch_CassetteSupply.h"
#include "sch_prm_cluster.h"
#include "Sch_Multi_alic.h"
#include "Timer_Handling.h"

#include "INF_sch_CommonUser.h"

//------------------------------------------------------------------------------------------
// _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() : Infinite Scenario
//
// Complete Scenario(Inside) 가 없는 경우 (  -1 - -10 )
// Complete Scenario(Inside) 가 있는 경우 ( -11 - -20 )
//
// % 10 Operation
//												CompleteScenario
// -1 (A) : Cooler Slot Timing Matching			Last Place(All) to IC Time
// -2 (B) : Full Pick IC to Place CM			Last Place(All) to IC Time
// -3 (C) : Cooler Slot Ordering Matching		Last Supply Time
// -4 (D) : Cooler Slot By Slot Matching		Last Supply Time
// -5 (E) : Cooler Slot Ordering Matching		Last Place to IC Time			// 2011.04.28
// -6 (F) : Cooler Slot By Slot Matching		Last Place to IC Time			// 2011.04.28
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//int				MFIC_STATION_SIDE[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.21
//int				MFIC_STATION_POINTER[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.21
//int				MFIC_STATION_WAFER[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.21

BOOL			MFIC_MULTI_CHECK = FALSE; // 2018.01.18

int				MFIC_STATION_SIDE[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.21
int				MFIC_STATION_POINTER[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ];
int				MFIC_STATION_WAFER[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ];
int				MFIC_STATION_WAFER_B[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ]; // 2012.03.22
//
int				MFIC_STATION_TIMER_CONTROL[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ]; // 2011.04.27

int				MFIC_LAST_PLACED_SLOT = 0; // 2007.09.17

int				MFIC_LAST_PLACED_SIDE_SLOT[MAX_CASSETTE_SIDE]; // 2018.01.18

//int				MFAL_STATION_SIDE; // 2008.08.29
//int				MFAL_STATION_POINTER; // 2008.08.29
//int				MFAL_STATION_WAFER; // 2008.08.29
//int				MFAL_STATION_BM; // 2008.08.29
int				MFAL_STATION_SIDE[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.08.29
int				MFAL_STATION_POINTER[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.08.29
int				MFAL_STATION_WAFER[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.08.29
int				MFAL_STATION_WAFER_B[ MAX_CASSETTE_SLOT_SIZE ]; // 2012.03.22
int				MFAL_STATION_BM[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.08.29
//------------------------------------------------------------------------------------------
int _i_SCH_MODULE_Get_MFIC_LAST_PLACED_SLOT() {
	return MFIC_LAST_PLACED_SLOT;
}
//------------------------------------------------------------------------------------------
BOOL _i_SCH_MODULE_Need_Do_Multi_FAL() {
	if ( ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) && _i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) ) return TRUE;
	return FALSE;
}
BOOL _i_SCH_MODULE_Need_Do_Multi_TAL() {
	if ( ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) && _i_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) ) return TRUE;
	return FALSE;
}
BOOL _i_SCH_MODULE_Need_Do_Multi_FIC() { // 2005.09.06
	if ( ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) || ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) ) && _i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) return TRUE; // 2010.12.22
	return FALSE;
}
//------------------------------------------------------------------------------------------


void _i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( int slot , int side , int pointer , int bm ) {
	MFAL_STATION_SIDE[slot-1] = side;
	MFAL_STATION_POINTER[slot-1] = pointer;
	MFAL_STATION_BM[slot-1] = bm;
}
void _i_SCH_MODULE_Set_MFALS_SIDE( int slot , int side ) {
	MFAL_STATION_SIDE[slot-1] = side;
}
void _i_SCH_MODULE_Set_MFALS_POINTER( int slot , int pointer ) {
	MFAL_STATION_POINTER[slot-1] = pointer;
}
void _i_SCH_MODULE_Set_MFALS_WAFER( int slot , int wafer ) {
	MFAL_STATION_WAFER[slot-1] = wafer;
	if ( wafer > 0 ) MFAL_STATION_WAFER_B[slot-1] = wafer;
}
void _i_SCH_MODULE_Set_MFALS_WAFER_B( int slot , int wafer ) {
	MFAL_STATION_WAFER_B[slot-1] = wafer;
}
void _i_SCH_MODULE_Set_MFALS_BM( int slot , int data ) {
	MFAL_STATION_BM[slot-1] = data;
}

int _i_SCH_MODULE_Get_MFALS_SIDE( int slot ) {
	return MFAL_STATION_SIDE[slot-1];
}
int _i_SCH_MODULE_Get_MFALS_POINTER( int slot ) {
	return MFAL_STATION_POINTER[slot-1];
}
int _i_SCH_MODULE_Get_MFALS_BM( int slot ) {
	return MFAL_STATION_BM[slot-1];
}
int _i_SCH_MODULE_Get_MFALS_WAFER( int slot ) {
	return MFAL_STATION_WAFER[slot-1];
}
int _i_SCH_MODULE_Get_MFALS_WAFER_B( int slot ) {
	return MFAL_STATION_WAFER_B[slot-1];
}


//==========================================================================================================
BOOL _MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( int side , int place_side , int place_pt ) { // 2011.11.30
	//
	switch ( _i_SCH_CLUSTER_Get_Extra_Flag( place_side , place_pt , 1 ) ) {
	case 3 : // left(0)
	case 7 : // left(0) // 2014.08.26
		if ( 0 != side ) return FALSE;
		break;
	case 4 : // right(1)
	case 8 : // right(1) // 2014.08.26
		if ( 1 != side ) return FALSE;
		break;
	}
	//
	return TRUE;
	//
}
//==========================================================================================================


BOOL SCHEDULER_CONTROL_Check_MFIC_Supply_Impossible( int side ) { // 2011.07.20
	int i , c , runc , fc;
	int s , oside1 , oside2 , supply_ic_pos , L_side , R_side;
	//
	if ( !_i_SCH_MODULE_Need_Do_Multi_FIC() ) return FALSE;
	//
	c = _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME();
	if ( c >= 0 ) return FALSE;
	//
	c = -c;
	switch( c % 10 ) {
	case 3 :
	case 4 :
	case 5 :
	case 6 :
	case 7 :
	case 8 :
	case 9 :
		break;
	default :
		return FALSE;
		break;
	}
	//====================================================================================================
	// 2011.11.30
	//
	c = 0;
	supply_ic_pos = -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
		//
		fc = _i_SCH_CLUSTER_Get_Extra_Flag( side , i , 1 );
		//
//		if ( fc != 2 ) { // 2014.08.26
		if ( ( fc != 2 ) && ( fc != 6 ) ) { // 2014.08.26
			//
			c = 1;
			//
//			if      ( fc == 3 ) { // 2014.08.26
			if      ( ( fc == 3 ) || ( fc == 7 ) ) { // 2014.08.26
				supply_ic_pos = 0;
			}
//			else if ( fc == 4 ) { // 2014.08.26
			else if ( ( fc == 4 ) || ( fc == 8 ) ) { // 2014.08.26
				supply_ic_pos = 1;
			}
			break;
		}
		//
	}
	//
	if ( c == 0 ) return FALSE;
	//====================================================================================================
//	c = ( _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) <= MAX_CASSETTE_SLOT_SIZE ) ? 0 : 1; // 2018.01.18
	c = ( !MFIC_MULTI_CHECK ) ? 0 : 1; // 2018.01.18
	//
	runc = 0;
	fc = 0; // 2011.10.11
	//
	for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( _i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) {
			//
			fc++; // 2011.10.11
			//
			continue;
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
			if ( ( _i_SCH_MODULE_Get_FM_OutCount( i ) <= 0 ) && ( _i_SCH_MODULE_Get_TM_OutCount( i ) <= 0 ) ) continue;
		}
		else {
			if ( _i_SCH_MODULE_Get_TM_OutCount( i ) <= 0 ) continue;
		}
		//
		runc++;
		//
		if ( runc >= ( c + 1 ) ) return TRUE;
		//
	}
	//
	if ( ( fc + runc ) > ( c + 1 ) ) return TRUE; // 2011.10.11
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.09.04
		//
		if ( c == 1 ) {
			//
			//
			oside1 = -1;
			oside2 = -1;
			//
			L_side = 0;
			R_side = 0;
			//
			for ( i = 0; i < _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; i++ ) {
				//
				if ( _i_SCH_MODULE_Get_MFIC_WAFER( i + 1 ) <= 0 ) continue;
				//
				s = _i_SCH_MODULE_Get_MFIC_SIDE( i + 1 );
				//
				if ( s == side ) return FALSE;
				//
				if ( i < ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) / 2 ) ) L_side++;
				else                                                R_side++;
				//
				if ( oside1 == -1 ) {
					oside1 = s;
				}
				else {
					if ( oside1 != s ) {
						if ( oside2 == -1 ) {
							oside2 = s;
						}
						else if ( oside2 != s ) {
							oside2 = s;
						}
					}
				}
				//
			}
			//
			if ( runc > 0 ) { // 2011.11.30
				//
				if ( ( supply_ic_pos != -1 ) && ( oside1 == -1 ) && ( oside2 == -1 ) ) return TRUE;
				//
				if ( ( supply_ic_pos == 0 ) && ( L_side > 0 ) ) return TRUE;
				//
				if ( ( supply_ic_pos == 1 ) && ( R_side > 0 ) ) return TRUE;
				//
			}
			//
			if ( ( _i_SCH_MODULE_Get_FM_OutCount( side ) - _i_SCH_MODULE_Get_FM_InCount( side ) ) >= ( _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( side ) + 1 ) ) {
				//
				if ( ( oside1 == -1 ) || ( oside2 == -1 ) ) return FALSE;
				//
				return TRUE;
				//
			}
			//
		}
		//
	}
	//
	return FALSE;
}


//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_MFIC_RUN_TIMER( int slot , int slot2  , int targettime , BOOL targetsetonly ) {
	KPLT_RESET_MFIC_RUN_TIMER( slot , slot2  , targettime , targetsetonly );
}
//====================================================================================================================
void SCHEDULER_CONTROL_Check_MFIC_Completed_Machine( int data ) { // 2007.07.16
	int i , j , c , r , rok , cc1 , cc2;
	int cs[MAX_CASSETTE_SIDE];
	//
	if ( data >= -10 ) return;
	//
	if ( ( data == -13 ) || ( data == -14 ) ) {
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
//			if ( !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue;
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue; // 2012.02.14
			//
			/*
			// 2013.12.11
			for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
				//
				if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
				//
				if ( MFIC_STATION_SIDE[ j ] != i ) continue;
				//
				if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue; // 2011.04.27
				//
				MFIC_STATION_TIMER_CONTROL[ j ] = 1; // 2011.04.27
				//
				if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
					KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE );
				}
				else {
					KPLT_RESET_MFIC_CURRTRG_TIMER( j + 1 , TRUE );
				}
				//
			}
			//
			*/

			// 2013.12.11
			//
			while ( TRUE ) {
				//
				rok = -1;
				//
				for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
					//
					if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
					//
					if ( MFIC_STATION_SIDE[ j ] != i ) continue;
					//
					if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue; // 2011.04.27
					//
					if ( rok == -1 ) {
						rok = j;
					}
					else {
						if ( _i_SCH_CLUSTER_Get_SupplyID( i , MFIC_STATION_POINTER[ j ] ) < _i_SCH_CLUSTER_Get_SupplyID( i , MFIC_STATION_POINTER[ rok ] ) ) {
							rok = j;
						}
					}
					//
				}
				//
				if ( rok == -1 ) {
					break;
				}
				else {
					//
					MFIC_STATION_TIMER_CONTROL[ rok ] = 1; // 2011.04.27
					//
					if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
						KPLT_RESET_MFIC_RUN_TIMER( rok + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE );
					}
					else {
						KPLT_RESET_MFIC_CURRTRG_TIMER( rok + 1 , TRUE );
					}
				}
			}

		}
	}
	else if ( ( data == -15 ) || ( data == -16 ) ) {
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue; // 2011.06.02
			//
			if ( !_i_SCH_MODULE_Chk_TM_Free_Status2( i , 1 ) ) continue;
			//
			c = 0;
			//
			for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
				if ( MFIC_STATION_WAFER[ j ] > 0 ) {
					if ( MFIC_STATION_SIDE[ j ] == i ) {
						c++;
					}
				}
			}
			//
//			if ( c != _i_SCH_MODULE_Get_TM_OutCount( i ) ) continue; // 2011.06.02
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				if ( c != ( _i_SCH_MODULE_Get_FM_OutCount( i ) - _i_SCH_MODULE_Get_FM_InCount( i ) ) ) continue; // 2011.06.02
			}
			else {
				if ( c != ( _i_SCH_MODULE_Get_TM_OutCount( i ) - _i_SCH_MODULE_Get_TM_InCount( i ) ) ) continue; // 2011.06.02
			}
			//
			for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
				//
				if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
				//
				if ( MFIC_STATION_SIDE[ j ] != i ) continue;
				//
				if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue;
				//
				MFIC_STATION_TIMER_CONTROL[ j ] = 1;
				//
				if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
					KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE );
				}
				else {
					KPLT_RESET_MFIC_CURRTRG_TIMER( j + 1 , TRUE );
				}
				//
			}
			//
		}
	}
	else if ( ( data == -17 ) || ( data == -18 ) ) { // 2011.06.28
		//
		/*
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue;
			//
			for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
				//
				if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
				//
				if ( MFIC_STATION_SIDE[ j ] != i ) continue;
				//
				if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue;
				//
				MFIC_STATION_TIMER_CONTROL[ j ] = 1;
				//
				if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
					KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE );
				}
				else {
					KPLT_RESET_MFIC_CURRTRG_TIMER( j + 1 , TRUE );
				}
				//
			}
			//
		}
		*/
		//
		// 2011.09.15
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			cs[i] = -1;
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue;
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_READY ) {
					//
					if ( cs[i] == -1 ) {
						cs[i] = _i_SCH_CLUSTER_Get_SupplyID( i , j );
					}
					else {
						if ( cs[i] < _i_SCH_CLUSTER_Get_SupplyID( i , j ) ) {
							cs[i] = _i_SCH_CLUSTER_Get_SupplyID( i , j );
						}
					}
					//
				}
				//
			}
			//
		}
		//
		c = -1;
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( cs[i] == -1 ) continue;
			//
			if ( c == -1 ) {
				c = i;
			}
			else {
				if ( cs[i] < cs[c] ) {
					c = i;
				}
			}
		}
		//
		if ( c != -1 ) { // 2011.09.22
			//
			while ( TRUE ) {
				//
				r = 0;
				rok = 0;
				//
				cc1 = 0;
				cc2 = 0;
				//
				for ( i = 0 ; i < 2 ; i++ ) {
					//
					for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
						if ( MFIC_STATION_WAFER[ j ] > 0 ) {
							if ( MFIC_STATION_SIDE[ j ] == c ) {
								if ( i == 0 )	cc1++;
								else			cc2++;
							}
						}
					}
					//
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
						if ( _i_SCH_MODULE_Get_FM_WAFER( j ) > 0 ) {
							if ( _i_SCH_MODULE_Get_FM_SIDE( j ) == c ) {
								if ( i == 0 )	r++;
								else			rok++;
							}
						}
					}
					//
					if ( i == 0 ) {
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
							r = _i_SCH_MODULE_Get_FM_OutCount( c ) - _i_SCH_MODULE_Get_FM_InCount( c ) - ( cc1 + r );
						}
						else {
							r = _i_SCH_MODULE_Get_TM_InCount( c ) - _i_SCH_MODULE_Get_TM_InCount( c ) - ( cc1 + r );
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
							rok = _i_SCH_MODULE_Get_FM_OutCount( c ) - _i_SCH_MODULE_Get_FM_InCount( c ) - ( cc2 + rok );
						}
						else {
							rok = _i_SCH_MODULE_Get_TM_InCount( c ) - _i_SCH_MODULE_Get_TM_InCount( c ) - ( cc2 + rok );
						}
					}
					//
				}
				//
				if ( ( cc1 == cc2 ) && ( r == rok ) ) break;
				//
				Sleep(1);
				//
			}
			//
			if ( ( r > 0 ) && ( cc1 <= 2 ) ) {
				rok = 1;
			}
			else {
				rok = 0;
			}
			//
		}
		else {
			rok = 0;
		}
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( i ) ) continue;
			//
			if ( ( c != -1 ) && ( c != i ) ) {
				//
				for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
					//
					if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
					//
					if ( MFIC_STATION_SIDE[ j ] != i ) continue;
					//
					if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) {
						//
						KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , data , FALSE );
						//
						MFIC_STATION_TIMER_CONTROL[ j ] = 0;
						//
					}
					//
				}
				//
				continue;
			}
			//
			for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
				//
				if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
				//
				if ( MFIC_STATION_SIDE[ j ] != i ) continue;
				//
				if ( rok == 1 ) { // 2011.09.22
					//
					if ( MFIC_STATION_TIMER_CONTROL[ j ] != 1 ) continue;
					//
					KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , data , FALSE );
					//
					MFIC_STATION_TIMER_CONTROL[ j ] = 0;
					//
				}
				else {
					//
					if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue;
					//
					MFIC_STATION_TIMER_CONTROL[ j ] = 1;
					//
					if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
						KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE );
					}
					else {
						KPLT_RESET_MFIC_CURRTRG_TIMER( j + 1 , TRUE );
					}
					//
				}
			}
			//
		}
		//
	}
	else {
		r = 0;
		rok = 0;
		//
		for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				r++;
				if ( _i_SCH_MODULE_Chk_TM_Free_Status2( i , 1 ) ) {
					rok++;
					cs[i] = 1;
				}
				else {
					cs[i] = 2;
				}
			}
			else {
				cs[i] = 0;
			}
		}
		//
		if (
//			( ( rok >= 1 ) && ( ( _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) <= MAX_CASSETTE_SLOT_SIZE ) || ( r == 1 ) ) ) || // 2018.01.18
//			( ( rok >= 2 ) && ( _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) > MAX_CASSETTE_SLOT_SIZE ) ) // 2018.01.18
			( ( rok >= 1 ) && ( ( !MFIC_MULTI_CHECK ) || ( r == 1 ) ) ) || // 2018.01.18
			( ( rok >= 2 ) && ( MFIC_MULTI_CHECK ) ) // 2018.01.18
			) {
			for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( cs[i] == 1 ) {
					//
					c = 0;
					//
					for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
						if ( MFIC_STATION_WAFER[ j ] > 0 ) {
							if ( MFIC_STATION_SIDE[ j ] == i ) {
								c++;
							}
						}
					}
					//
					if ( c != _i_SCH_MODULE_Get_TM_OutCount( i ) ) return;
					//
				}
			}
			for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( cs[i] != 1 ) continue;
				//
				for ( j = 0; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
					//
					if ( MFIC_STATION_WAFER[ j ] <= 0 ) continue;
					//
					if ( MFIC_STATION_SIDE[ j ] != i ) continue;
					//
					if ( MFIC_STATION_TIMER_CONTROL[ j ] == 1 ) continue; // 2011.04.27
					//
					MFIC_STATION_TIMER_CONTROL[ j ] = 1; // 2011.04.27
					//
					if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() >= 5 ) && ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) ) { // 2011.04.27
						KPLT_RESET_MFIC_RUN_TIMER( j + 1 , 0 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() , FALSE ); // 2011.04.27
					}
					else {
						KPLT_RESET_MFIC_CURRTRG_TIMER( j + 1 , TRUE );
					}
					//
				}
				//
			}
		}
	}
}
//====================================================================================================================
int _i_SCH_MODULE_Get_MFIC_Completed_Wafer_Sub( int side , int *side2 , int *slot , int *slot2 ) { // 2005.09.06
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_FNAME()[0] != 0 ) {
		if ( _i_SCH_MODULE_Get_MFIC_WAFER( 1 ) > 0 ) {
			if ( side == -1 ) { // 2009.02.16
				*slot = 1;
				*slot2 = 0;
				*side2 = _i_SCH_MODULE_Get_MFIC_SIDE( 1 );
				return EQUIP_COOLING_STATUS_FOR_FM_MIC( _i_SCH_MODULE_Get_MFIC_SIDE( 1 ) , 1 );
			}
			else {
				if ( _i_SCH_MODULE_Get_MFIC_SIDE( 1 ) == side ) {
					*slot = 1;
					*slot2 = 0;
					*side2 = side; // 2006.11.14
					return EQUIP_COOLING_STATUS_FOR_FM_MIC( side , 1 );
				}
			}
		}
	}
	else {
		if ( !_SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE( side , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , 0 ) ) {
			SCHEDULER_CONTROL_Check_MFIC_Completed_Machine( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() ); // 2007.07.16
		}
		if ( _i_SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( TRUE , side , side2 , slot , slot2 ) > 0 ) return SYS_SUCCESS;
	}
	return SYS_RUNNING;
}

int _i_SCH_MODULE_Get_MFIC_Completed_Wafer_FDHC( int side , int *side2 , int *slot , int *slot2 ) { // 2005.09.06
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Res = _i_SCH_MODULE_Get_MFIC_Completed_Wafer_Sub( side , side2 , slot , slot2 );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
}

BOOL _i_SCH_MODULE_Chk_MFIC_Yes_for_Get_FDHC( int side ) {
	int FM_Side2 , ICsts , ICsts2;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	if ( _i_SCH_MODULE_Get_MFIC_Completed_Wafer_Sub( side , &FM_Side2 , &ICsts , &ICsts2 ) != SYS_RUNNING ) {
		//
		_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
		//
		return TRUE;
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return FALSE;
}

BOOL _i_SCH_MODULE_Chk_MFIC_LOCKING( int side ) {
	if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( _i_SCH_MODULE_Need_Do_Multi_FIC() && ( _i_SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) ) return TRUE;
	}
	else {
		if ( !_i_SCH_MODULE_Need_Do_Multi_FIC() ) return FALSE;
		if ( _i_SCH_MODULE_Chk_MFIC_FREE_COUNT() >= 1 ) return FALSE;
		if ( !_i_SCH_MODULE_Chk_MFIC_Yes_for_Get_FDHC( side ) ) return TRUE;
	}
	return FALSE;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi FIC Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_MFIC_SIDE_POINTER( int slot , int side , int pointer ) { MFIC_STATION_SIDE[slot-1] = side; MFIC_STATION_POINTER[slot-1] = pointer; }
void _i_SCH_MODULE_Set_MFIC_SIDE( int slot , int side ) { MFIC_STATION_SIDE[slot-1] = side; }
int  _i_SCH_MODULE_Get_MFIC_SIDE( int slot ) { return MFIC_STATION_SIDE[slot-1]; }
void _i_SCH_MODULE_Set_MFIC_POINTER( int slot , int pointer ) { MFIC_STATION_POINTER[slot-1] = pointer; }
int  _i_SCH_MODULE_Get_MFIC_POINTER( int slot ) { return MFIC_STATION_POINTER[slot-1]; }

void _i_SCH_MODULE_Set_MFIC_WAFER( int slot , int wafer ) {
	//===========================================================================================
	MFIC_STATION_TIMER_CONTROL[slot-1] = 0; // 2011.04.27
	//===========================================================================================
	MFIC_STATION_WAFER[slot-1] = wafer;
	//===========================================================================================
	// 2007.09.17
	//===========================================================================================
	if ( wafer > 0 ) {
		MFIC_STATION_WAFER_B[slot-1] = wafer; // 2012.03.20
		MFIC_LAST_PLACED_SLOT = slot;
		//
		if ( ( MFIC_STATION_SIDE[slot-1] >= 0 ) && ( MFIC_STATION_SIDE[slot-1] < MAX_CASSETTE_SIDE ) ) { // 2018.01.18
			MFIC_LAST_PLACED_SIDE_SLOT[MFIC_STATION_SIDE[slot-1]] = slot;
		}
		//
	}
	//===========================================================================================
}
int  _i_SCH_MODULE_Get_MFIC_WAFER( int slot ) { return MFIC_STATION_WAFER[slot-1]; }

void _i_SCH_MODULE_Set_MFIC_WAFER_B( int slot , int wafer ) { MFIC_STATION_WAFER_B[slot-1] = wafer; }
int  _i_SCH_MODULE_Get_MFIC_WAFER_B( int slot ) { return MFIC_STATION_WAFER_B[slot-1]; }

int  _i_SCH_MODULE_Get_MFIC_TIMECONTROL( int slot ) { return MFIC_STATION_TIMER_CONTROL[slot-1]; }

//======================================================================

void INIT_SCHEDULER_MFALIC_DATA( int apmode , int side ) {
	int i;
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) {
//			_i_SCH_MODULE_Set_MFIC_WAFER( i , 0 ); // 2007.08.03 // 2008.08.29
			_i_SCH_MODULE_Set_MFIC_WAFER( i + 1 , 0 ); // 2007.08.03 // 2008.08.29
			_i_SCH_MODULE_Set_MFIC_WAFER_B( i + 1 , 0 ); // 2012.03.22
		}
		MFIC_LAST_PLACED_SLOT = 0; // 2007.09.17
		//
//		_i_SCH_MODULE_Set_MFAL_WAFER( 0 ); // 2008.08.29
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			_i_SCH_MODULE_Set_MFALS_WAFER( i + 1 , 0 );
			_i_SCH_MODULE_Set_MFALS_WAFER_B( i + 1 , 0 ); // 2012.03.22
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2018.01.18
			MFIC_LAST_PLACED_SIDE_SLOT[i] = -1;
		}
		//
		// 2018.01.18
		//
		if ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) <= _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) {
			MFIC_MULTI_CHECK = FALSE;
		}
		else {
			MFIC_MULTI_CHECK = TRUE;
		}
		//
	}
}

//======================================================================

int  _i_SCH_MODULE_Chk_MFIC_HAS_COUNT() {
	int i , k = 0;
	for ( i = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i >= 1 ; i-- ) {
		if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) k++;
	}
	return k;
}

//======================================================================

int  _i_SCH_MODULE_Chk_MFIC_FREE_COUNT() {
	int i , k = 0;
	for ( i = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i >= 1 ; i-- ) {
		if ( MFIC_STATION_WAFER[ i - 1 ] <= 0 ) k++;
	}
	return k;
}

//======================================================================

int _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( int s , int p ) { // 2007.09.17
	if ( s == -1 ) return -1;
	if ( p == -1 ) return -1;
	if ( p >= 100 ) return -1;
	return _i_SCH_CLUSTER_Get_SlotIn( s , p );
}


//======================================================================
int  _i_SCH_MODULE_Chk_MFIC_FREE2_COUNT( int s1 , int p1 , int s2 , int p2 ) {
	int c , s;
//	if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
	if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -8 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
		c = 0;
		if ( s1 != -1 ) {
			s = _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( s1 , p1 );
			if ( s != -1 ) {
				if ( MFIC_STATION_WAFER[ s - 1 ] <= 0 ) c++;
			}
		}
		if ( s2 != -1 ) {
			s = _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( s2 , p2 );
			if ( s != -1 ) {
				if ( MFIC_STATION_WAFER[ s - 1 ] <= 0 ) c++;
			}
		}
		//===================================================================================================================
		// 2007.12.14
		//===================================================================================================================
//		if ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) > MAX_CASSETTE_SLOT_SIZE ) { // 2018.01.18
		if ( MFIC_MULTI_CHECK ) { // 2018.01.18
			if ( c == 0 ) {
				if ( s1 != -1 ) {
					s = _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( s1 , p1 );
					if ( s != -1 ) {
						if ( MFIC_STATION_WAFER[ s - 1 + ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) / 2 ) ] <= 0 ) c++;
					}
				}
				if ( s2 != -1 ) {
					s = _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( s2 , p2 );
					if ( s != -1 ) {
						if ( MFIC_STATION_WAFER[ s - 1 + ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) / 2 ) ] <= 0 ) c++;
					}
				}
			}
		}
		//================================================================================================================================
		return c;
	}
	return _i_SCH_MODULE_Chk_MFIC_FREE_COUNT();
}

//======================================================================

int  _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( int sl , int el , int *s1 , int *s2 , int refslot1 , int refslot2 ) {
	int i , k = 0 , l , ss , es;
	time_t finish; // 2006.02.03

	time( &finish ); // 2006.02.03

	*s1 = 0;
	*s2 = 0;

	//====================================================================================
	// 2006.02.03
	//====================================================================================
//	for ( i = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i >= 1 ; i-- ) {
//		if ( MFIC_STATION_WAFER[ i - 1 ] <= 0 ) {
//			*s2 = *s1;
//			*s1 = i;
//			k++;
//		}
//	}
	//====================================================================================
	for ( l = 0 ; l < 2 ; l++ ) { // 2007.09.17
		//==========================================================================
		// 2007.09.17
		//==========================================================================
		if ( l == 0 ) {
//			if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
			if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -7 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
				ss = MFIC_LAST_PLACED_SLOT + 1;
				if ( ss < sl ) ss = sl; // 2009.07.16
				es = el;
			}
//			else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
			else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -8 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
				if ( ( refslot1 != -1 ) && ( refslot2 != -1 ) && ( refslot1 != refslot2 ) ) {
					ss = refslot1;
					es = refslot1;
				}
				else if ( ( refslot1 != -1 ) && ( refslot2 == -1 ) ) {
					ss = refslot1;
					es = refslot1;
				}
				else {
printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "= Unmatched 1 (%d,%d)=\n" , refslot1 , refslot2 );
printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "====================================================\n" );
					ss = sl;
					es = el;
				}
			}
			else {
				ss = sl;
				es = el;
			}
		}
		else {
//			if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
			if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -7 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
				ss = sl;
				es = MFIC_LAST_PLACED_SLOT;
				if ( es > el ) es = el; // 2009.07.16
			}
//			else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
			else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -8 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
				if ( ( refslot1 != -1 ) && ( refslot2 != -1 ) && ( refslot1 != refslot2 ) ) {
					ss = refslot2;
					es = refslot2;
				}
				else if ( ( refslot2 != -1 ) && ( refslot1 == -1 ) ) {
					ss = refslot2;
					es = refslot2;
				}
				else {
					return k; // 2007.10.15


printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "= Unmatched 2 (%d,%d)=\n" , refslot1 , refslot2 );
printf( "====================================================\n" );
printf( "====================================================\n" );
printf( "====================================================\n" );

					ss = sl;
					es = el;
				}
			}
			else {
				ss = sl;
				es = el;
			}
		}
		//==========================================================================
//		for ( i = sl ; i <= el ; i++ ) { // 2005.11.28 // 2007.09.17
		for ( i = ss ; i <= es ; i++ ) { // 2005.11.28 // 2007.09.17
			if ( MFIC_STATION_WAFER[ i - 1 ] <= 0 ) {
				if      ( *s1 == 0 ) {
					*s1 = i;
				}
				else if ( *s2 == 0 ) {
					//===============================================================
					// 2007.09.19
					//===============================================================
//					if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
					if      ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -3 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -5 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -7 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -13 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -15 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -17 ) ) { // 2014.07.02
						*s2 = i;
						return 2;
					}
//					else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
					else if ( ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -4 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -6 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -8 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -14 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -16 ) || ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -18 ) ) { // 2014.07.02
						*s2 = i;
						return 2;
					}
					//===============================================================
	//				if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() < 0 ) { // 2007.07.05 // 2007.09.12
	//					*s2 = i;
	//					return 2;
	//				}
					//===============================================================
					switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) {
					case 1 :
						*s2 = *s1;
						*s1 = i;
						break;
					case 0 :
						*s2 = i;
						break;
					case -1 :
						*s2 = i;
						break;
					}
				}
				else {
					switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) {
					case 1 :
						*s2 = *s1;
						*s1 = i;
						break;
					case 0 :
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) {
						case 1 :
							*s2 = i;
							break;
						}
						break;
					case -1 :
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) {
						case 1 :
							*s2 = i;
							break;
						}
						break;
					}
				}
				k++;
			}
		}
		//====================================================================================
		if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() >=  0 ) break; // 2007.09.17
		if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -1 ) break; // 2007.09.17
		if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -2 ) break; // 2007.09.17
		//====================================================================================
	}
	return k;
}


int  _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( int *s1 , int *s2 , int refslot1 , int refslot2 ) {
	return _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s1 , s2 , refslot1 , refslot2 );
}


int  _i_SCH_MODULE_Chk_MFIC_FREE_TYPE2_SLOT( int refslot , int offset , int *s1 , int *s2 ) {
	int i , k = 0;
	time_t finish;

	time( &finish );

	*s1 = 0;
	*s2 = 0;

	//====================================================================================
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
		if ( ( ( i - 1 ) % offset ) == ( refslot - 1 ) ) {
			if ( MFIC_STATION_WAFER[ i - 1 ] <= 0 ) {
				if ( *s1 == 0 ) {
					*s1 = i;
				}
				else if ( *s2 == 0 ) {
					switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) {
					case 1 :
						*s2 = *s1;
						*s1 = i;
						break;
					case 0 :
						*s2 = i;
						break;
					case -1 :
						*s2 = i;
						break;
					}
				}
				else {
					switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) {
					case 1 :
						*s2 = *s1;
						*s1 = i;
						break;
					case 0 :
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) {
						case 1 :
							*s2 = i;
							break;
						}
						break;
					case -1 :
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) {
						case 1 :
							*s2 = i;
							break;
						}
						break;
					}
				}
				k++;
			}
		}
	}
	//====================================================================================
	return k;
}


//==========================================================================================================
BOOL  _i_SCH_MODULE_Chk_MFIC_MULTI_FREE( int sl , int count ) {
	int i , c , s , t;

	if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2007.10.15
		//======================================================================================================
		_i_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , sl , &s , &t );
		//======================================================================================================
	}
	else {
		s = sl;
	}
	//
	c = 0;
	//
	for ( i = s ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
		if ( MFIC_STATION_WAFER[ i - 1 ] <= 0 ) {
			c++;
			if ( c == count ) return TRUE;
		}
		else {
			return FALSE;
		}
	}
	return FALSE;
}

//==========================================================================================================
BOOL  _i_SCH_MODULE_Chk_MFIC_MULTI_FINISH( int sl , int count ) {
	int i , c , cc , cs , FM_CO , FM_OSlot;
	time_t finish;

	time( &finish );

	c = 0;
	cc = -1;
	cs = -1;
	for ( i = sl ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
		if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
			//
			if ( !KPLT_CHECK_MFIC_RUN_TIMER1( i , finish ) ) return FALSE;
			//
			_i_SCH_SUB_GET_OUT_CM_AND_SLOT( MFIC_STATION_SIDE[ i - 1 ] , MFIC_STATION_POINTER[ i - 1 ] , 0 , &FM_CO , &FM_OSlot );
			//
			if ( cs == -1 ) {
				cc = FM_CO;
				cs = FM_OSlot;
			}
			else {
				if ( cc != FM_CO ) return FALSE;
				if ( ( cs + c ) != FM_OSlot ) return FALSE;
			}
		}
		c++;
		if ( c == count ) return TRUE;
	}
	return FALSE;
}


//==========================================================================================================
int  _i_SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( int Placewaferside1 , int Placewaferpt1 , int Placewaferside2 , int Placewaferpt2 , int *s1 , int *s2 , int multiextarm ) {
	int i , ss , es , div , NextSide_Skip_Check , Select_Count_Side1 , Select_Count_Side2 , Select_Side , Present_Count_in_Side1 , Present_Count_in_Side2 , Absent_Count_in_Side1 , Absent_Count_in_Side2 , xs , mode , bufint;
	//================================================================================================================================
//	if ( _i_SCH_PRM_GET_MODULE_SIZE(F_IC) <= MAX_CASSETTE_SLOT_SIZE ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) ); // 2018.01.18
	if ( !MFIC_MULTI_CHECK ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) ); // 2018.01.18
	//================================================================================================================================
	if ( ( Placewaferside1 == -1 ) && ( Placewaferside2 == -1 ) ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
	//================================================================================================================================
	div = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) / 2;
	//================================================================================================================================
	ss = 0;
	es = 0;
	//
	Select_Side = 0;
	NextSide_Skip_Check = 0;
	//
	Select_Count_Side1 = 0;
	Select_Count_Side2 = 0;
	Present_Count_in_Side1 = 0;
	Present_Count_in_Side2 = 0;
	Absent_Count_in_Side1 = 0;
	Absent_Count_in_Side2 = 0;
	//================================================================================================================================
	mode = _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME();
	//
	if ( // 2007.10.15
		( mode == -3 ) ||
		( mode == -4 ) ||
		( mode == -5 ) ||
		( mode == -6 ) ||
		( mode == -7 ) || // 2014.07.02
		( mode == -8 ) || // 2014.07.02
		( mode == -13 ) ||
		( mode == -14 ) ||
		( mode == -15 ) ||
		( mode == -16 ) ||
		( mode == -17 ) ||
		( mode == -18 ) 
	) {
		if ( ( Placewaferside1 != -1 ) || ( Placewaferside2 != -1 ) ) {
			//
			if ( ( Placewaferside1 != -1 ) && ( Placewaferside2 != -1 ) && ( Placewaferside1 != Placewaferside2 ) ) {
				//
//printf( "====================================================\n" );
//printf( "====================================================\n" );
//printf( "====================================================\n" );
//printf( "= Unmatched 2 =\n" );
//printf( "====================================================\n" );
//printf( "====================================================\n" );
//printf( "====================================================\n" );

				if ( multiextarm > 0 ) {
					return 0;
				}
				//---------------------------------------------------------------
				// 2009.03.04
				//---------------------------------------------------------------
				Select_Side = -1;
				for ( i = 1 ; i <= div ; i++ ) {
					if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
						Present_Count_in_Side1++;
						if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) {
							Select_Side = 0;
							break;
						}
					}
					else {
						Absent_Count_in_Side1++;
					}
				}
				if ( Select_Side == -1 ) {
					for ( i = div + 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
						if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
							Present_Count_in_Side2++;
							if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) {
								Select_Side = 1;
								break;
							}
						}
						else {
							Absent_Count_in_Side2++;
						}
					}
				}
				//-------
				if ( Select_Side == -1 ) { // 2018.01.18
					//
					if ( ( Present_Count_in_Side1 <= 0 ) && ( Present_Count_in_Side2 <= 0 ) ) {
						//
						if ( ( Placewaferside1 >= 0 ) && ( Placewaferside1 < MAX_CASSETTE_SIDE ) ) {
							//
							if      ( ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] >= 1 ) && ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] <= div ) ) {
								Select_Side = 0;
							}
							else if ( ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] >= (div + 1) ) && ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ) ) {
								Select_Side = 1;
							}
							else {
								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
									//
									if ( i == Placewaferside1 ) continue;
									//
									if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									//
									if      ( ( MFIC_LAST_PLACED_SIDE_SLOT[i] >= 1 ) && ( MFIC_LAST_PLACED_SIDE_SLOT[i] <= div ) ) {
										Select_Side = 1;
										break;
									}
									else if ( ( MFIC_LAST_PLACED_SIDE_SLOT[i] >= (div + 1) ) && ( MFIC_LAST_PLACED_SIDE_SLOT[i] <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ) ) {
										Select_Side = 0;
										break;
									}
									//
								}
							}
						}
						//
					}
				}
				//-------
				if ( Select_Side == -1 ) {
					//
					if ( Present_Count_in_Side1 <= 0 ) {
						//
						if ( _MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( 0 , Placewaferside1 , Placewaferpt1 ) ) Select_Side = 0; // 2011.11.30
						//
					}
					//
					if ( Select_Side == -1 ) {
						//
						if ( Present_Count_in_Side2 <= 0 ) {
							//
							if ( _MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( 1 , Placewaferside1 , Placewaferpt1 ) ) Select_Side = 1; // 2011.11.30
							//
						}
						//
					}
					//
				}
				//-------
				NextSide_Skip_Check = FALSE;
				//
				if ( Select_Side != -1 ) {
					if ( !_MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( Select_Side , Placewaferside2 , Placewaferpt2 ) ) NextSide_Skip_Check = TRUE;
				}
				//-------
				if      ( Select_Side == 0 ) {
					*s2 = 0;
					xs = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , -1 );
					if ( xs <= 0 ) return 0;
					//
					if ( NextSide_Skip_Check ) return 1;
					//
					xs = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) + div , -1 );
					if ( xs <= 0 ) return 1;
					return 2;
				}
				else if ( Select_Side == 1 ) {
					*s2 = 0;
					xs = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) + div , -1 );
					if ( xs <= 0 ) return 0;
					//
					if ( NextSide_Skip_Check ) return 1;
					//
					xs = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) , -1 );
					if ( xs <= 0 ) return 1;
					return 2;
				}
				else {
					return 0;
				}
				//-------
//				return 0; // 아직 적용 안함
				//-------
			}
			else if ( Placewaferside1 != -1 ) {
				Select_Side = -1;
				for ( i = 1 ; i <= div ; i++ ) {
					if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
						Present_Count_in_Side1++;
						if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) {
							Select_Side = 0;
							break;
						}
					}
					else {
						Absent_Count_in_Side1++;
					}
				}
				if ( Select_Side == -1 ) {
					for ( i = div + 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
						if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
							Present_Count_in_Side2++;
							if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) {
								Select_Side = 1;
								break;
							}
						}
						else {
							Absent_Count_in_Side2++;
						}
					}
				}
				//-------
				if ( Select_Side == -1 ) { // 2018.01.18
					//
					if ( ( Present_Count_in_Side1 <= 0 ) && ( Present_Count_in_Side2 <= 0 ) ) {
						//
						if ( ( Placewaferside1 >= 0 ) && ( Placewaferside1 < MAX_CASSETTE_SIDE ) ) {
							//
							if      ( ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] >= 1 ) && ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] <= div ) ) {
								Select_Side = 0;
							}
							else if ( ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] >= (div + 1) ) && ( MFIC_LAST_PLACED_SIDE_SLOT[Placewaferside1] <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ) ) {
								Select_Side = 1;
							}
							else {
								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
									//
									if ( i == Placewaferside1 ) continue;
									//
									if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									//
									if      ( ( MFIC_LAST_PLACED_SIDE_SLOT[i] >= 1 ) && ( MFIC_LAST_PLACED_SIDE_SLOT[i] <= div ) ) {
										Select_Side = 1;
										break;
									}
									else if ( ( MFIC_LAST_PLACED_SIDE_SLOT[i] >= (div + 1) ) && ( MFIC_LAST_PLACED_SIDE_SLOT[i] <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ) ) {
										Select_Side = 0;
										break;
									}
									//
								}
							}
						}
					}
				}
				//-------
				if ( Select_Side == -1 ) {
					//
					if ( Present_Count_in_Side1 <= 0 ) {
						//
						if ( _MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( 0 , Placewaferside1 , Placewaferpt1 ) ) Select_Side = 0; // 2011.11.30
						//
					}
					//
					if ( Select_Side == -1 ) {
						//
						if ( Present_Count_in_Side2 <= 0 ) {
							//
							if ( _MFIC_FREE_TYPE3_POSSIBLE_SIDECHECK( 1 , Placewaferside1 , Placewaferpt1 ) ) Select_Side = 1; // 2011.11.30
							//
						}
						//
					}
					//
				}
				//-------
				if      ( Select_Side == 0 ) {
					return _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
				}
				else if ( Select_Side == 1 ) {
					return _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) + div , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) + div );
				}
				else {
					return 0;
				}
				//-------
			}
		}
	}
	//================================================================================================================================
	else {
		if ( ( Placewaferside1 != -1 ) && ( Placewaferside2 != -1 ) && ( Placewaferside1 != Placewaferside2 ) ) {
			for ( i = 1 ; i <= div ; i++ ) {
				if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) Select_Count_Side1++;
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside2 ) Select_Count_Side2++;
				}
				else {
					Absent_Count_in_Side1++;
				}
			}
			for ( i = div + 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
				if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) Present_Count_in_Side1++;
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside2 ) Present_Count_in_Side2++;
				}
				else {
					Absent_Count_in_Side2++;
				}
			}
			//
			if ( ( Absent_Count_in_Side1 > 0 ) && ( Absent_Count_in_Side2 > 0 ) ) {
				if      ( ( Select_Count_Side1 >  0 ) && ( Present_Count_in_Side1 >  0 ) ) {
					if      ( ( Select_Count_Side2 >  0 ) && ( Present_Count_in_Side2 >  0 ) ) {
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						return 2;
					}
					else if ( ( Select_Count_Side2 >  0 ) && ( Present_Count_in_Side2 == 0 ) ) {
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						return 2;
					}
					else if ( ( Select_Count_Side2 == 0 ) && ( Present_Count_in_Side2 >  0 ) ) {
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						return 2;
					}
					else {
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						return 2;
					}
				}
				else if ( ( Select_Count_Side1 >  0 ) && ( Present_Count_in_Side1 == 0 ) ) {
					i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					return 2;
				}
				else if ( ( Select_Count_Side1 == 0 ) && ( Present_Count_in_Side1 >  0 ) ) {
					i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( 1 , div , s2 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( div + 1 , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) , s1 , &bufint , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					if ( i <= 0 ) return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					return 2;
				}
				else {
					return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
				}
			}
			else {
				return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
			}
		}
		else {
			for ( i = 1 ; i <= div ; i++ ) {
				if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
					Present_Count_in_Side1++;
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) Select_Count_Side1++;
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside2 ) Select_Count_Side2++;
				}
				else {
					Absent_Count_in_Side1++;
				}
			}
			for ( i = div + 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
				if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
					Present_Count_in_Side2++;
//					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) Select_Count_Side2++; // ?? 2011.11.30
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside1 ) Select_Count_Side1++; // ?? 2011.11.30
					if ( MFIC_STATION_SIDE[ i - 1 ] == Placewaferside2 ) Select_Count_Side2++;
				}
				else {
					Absent_Count_in_Side2++;
				}
			}
			//================================================================================================================================
			if ( ( Select_Count_Side1 == 0 ) && ( Select_Count_Side2 == 0 ) ) {
				if      ( Present_Count_in_Side1 == 0 ) {
					ss = 1;
					es = div;
				}
				else if ( Present_Count_in_Side2 == 0 ) {
					ss = div + 1;
					es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
				}
				else {
					if      ( ( Absent_Count_in_Side1 == 0 ) && ( Absent_Count_in_Side2 == 0 ) ) {
						return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					}
					else if ( ( Absent_Count_in_Side1 >  0 ) && ( Absent_Count_in_Side2 == 0 ) ) {
						ss = 1;
						es = div;
					}
					else if ( ( Absent_Count_in_Side1 == 0 ) && ( Absent_Count_in_Side2 >  0 ) ) {
						ss = div + 1;
						es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
					}
					else {
						if ( ( ( Placewaferside1 == -1 ) || ( Placewaferside2 == -1 ) ) ) {
							return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						}
						else {
							if      ( ( Absent_Count_in_Side1 > Absent_Count_in_Side2 ) && ( Absent_Count_in_Side1 >= 2 ) ) {
								ss = 1;
								es = div;
							}
							else if ( ( Absent_Count_in_Side2 > Absent_Count_in_Side1 ) && ( Absent_Count_in_Side2 >= 2 ) ) {
								ss = div + 1;
								es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
							}
							else {
								return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
							}
						}
					}
				}
			}
			else if ( ( Select_Count_Side1 >  0 ) && ( Select_Count_Side2 == 0 ) ) {
				if ( Absent_Count_in_Side1 == 0 ) {
					return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , -1 , -1 );
				}
				else {
					if ( ( ( Placewaferside1 == -1 ) || ( Placewaferside2 == -1 ) ) ) {
						ss = 1;
						es = div;
					}
					else {
						if ( Absent_Count_in_Side1 == 1 ) {
							return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						}
						else {
							ss = 1;
							es = div;
						}
					}
				}
			}
			else if ( ( Select_Count_Side1 == 0 ) && ( Select_Count_Side2 >  0 ) ) {
				if ( Absent_Count_in_Side2 == 0 ) {
					return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
				}
				else {
					if ( ( ( Placewaferside1 == -1 ) || ( Placewaferside2 == -1 ) ) ) {
						ss = div + 1;
						es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
					}
					else {
						if ( Absent_Count_in_Side2 == 1 ) {
							return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						}
						else {
							ss = div + 1;
							es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
						}
					}
				}
			}
			else {
				if ( ( ( Placewaferside1 == -1 ) || ( Placewaferside2 == -1 ) ) ) {
					if ( Absent_Count_in_Side1 >= Absent_Count_in_Side2 ) {
						ss = 1;
						es = div;
					}
					else {
						ss = div + 1;
						es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
					}
				}
				else {
					if ( Absent_Count_in_Side1 > Absent_Count_in_Side2 ) {
						if ( Absent_Count_in_Side1 >= 2 ) {
							ss = 1;
							es = div;
						}
						else {
							return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						}
					}
					else if ( Absent_Count_in_Side2 > Absent_Count_in_Side1 ) {
						if ( Absent_Count_in_Side2 >= 2 ) {
							ss = div + 1;
							es = _i_SCH_PRM_GET_MODULE_SIZE(F_IC);
						}
						else {
							return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
						}
					}
					else {
						return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
					}
				}
			}
		}
	}
	//================================================================================================================================
	i = _i_SCH_MODULE_Chk_MFIC_FREE_SUB_SLOT( ss , es , s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
	if ( i > 0 ) return i;
	return _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( s1 , s2 , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside1 , Placewaferpt1 ) , _i_SCH_MODULE_Chk_MFIC_GET_SLOTIN( Placewaferside2 , Placewaferpt2 ) );
	//================================================================================================================================
}

//==========================================================================================================

int  _i_SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( BOOL sidenotfixed , int side , int *side2 , int *s1 , int *s2 ) {
	int i , k = 0;
	time_t finish;

	time( &finish );

	*s1 = 0;
	*s2 = 0;

//	for ( i = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i >= 1 ; i-- ) { // 2005.11.28
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) { // 2005.11.28
		if ( sidenotfixed || ( MFIC_STATION_SIDE[ i - 1 ] == side ) ) {
			if ( MFIC_STATION_WAFER[ i - 1 ] > 0 ) {
				if ( KPLT_CHECK_MFIC_RUN_TIMER1( i , finish ) ) { // 2005.11.28
					if ( *s1 == 0 ) { // 2005.11.28
						*s1 = i;
					}
					else if ( *s2 == 0 ) { // 2005.11.28
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) { // 2005.11.28
						case 1 :
							*s2 = *s1;
							*s1 = i;
							break;
						case 0 :
//							*s2 = i; // 2013.12.11
							if ( _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ i-1 ] , MFIC_STATION_POINTER[ i-1 ] ) < _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ *s1-1 ] , MFIC_STATION_POINTER[ *s1-1 ] ) ) { // 2013.12.11
								*s2 = *s1;
								*s1 = i;
							}
							else {
								*s2 = i;
							}
							break;
						case -1 :
							*s2 = i;
							break;
						}
					}
					else { // 2005.11.28
						switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s1 , finish ) ) { // 2005.11.28
						case 1 :
							*s2 = *s1;
							*s1 = i;
							break;
						case 0 :
							if ( _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ i-1 ] , MFIC_STATION_POINTER[ i-1 ] ) < _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ *s1-1 ] , MFIC_STATION_POINTER[ *s1-1 ] ) ) { // 2013.12.11
								*s2 = *s1;
								*s1 = i;
							}
							else {
								switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) { // 2006.02.03
								case 1 :
									*s2 = i;
									break;
								case 0 : // 2013.12.11
									if ( _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ i-1 ] , MFIC_STATION_POINTER[ i-1 ] ) < _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ *s2-1 ] , MFIC_STATION_POINTER[ *s2-1 ] ) ) { // 2013.12.11
										*s2 = i;
									}
									break;
								}
							}
							break;
						case -1 :
							switch( KPLT_CHECK_MFIC_HOWLONG_TIMER( i , *s2 , finish ) ) { // 2005.11.28
							case 1 :
								*s2 = i;
								break;
							case 0 : // 2013.12.11
								if ( _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ i-1 ] , MFIC_STATION_POINTER[ i-1 ] ) < _i_SCH_CLUSTER_Get_SupplyID( MFIC_STATION_SIDE[ *s2-1 ] , MFIC_STATION_POINTER[ *s2-1 ] ) ) { // 2013.12.11
									*s2 = i;
								}
								break;
							}
							break;
						}
					}
					k++;
				}
			}
		}
	}
	//======================================
	// 2005.11.28
	//======================================
	if ( *s1 != 0 ) {
		if ( side != -1 ) { // 2007.08.27
			if ( MFIC_STATION_SIDE[ *s1 - 1 ] != side ) return 0;
		}
	}
	if ( *s2 != 0 ) {
		*side2 = MFIC_STATION_SIDE[ *s2 - 1 ];
	}
	//======================================
	return k;
}

int  _i_SCH_MODULE_Chk_MFIC_RUN_TIMER( int slot , int mode , time_t curtime ) { // 2005.12.01
	int ct;
	int target;
	//
	if ( mode == 0 ) return 0;
	if ( mode == 5 ) return 0; // 2011.04.27
	//
	if ( MFIC_STATION_WAFER[ slot - 1 ] > 0 ) {
		//
		ct = KPLT_CHECK_MFIC_RUN_TIMER2( slot , curtime , &target );
		//
//		if ( target <= 0 ) return 0; // 2018.02.23
		//
		switch( mode ) {
		case 1 : // dec
		case 6 : // dec // 2011.04.27
			//
			if ( target <= 0 ) return 0; // 2018.02.23
			//
			if ( ( target - ct ) < 0 ) return 0;
			return ( target - ct );
			break;
		case 2 : // inc
		case 7 : // inc // 2011.04.27
			//
			if ( target <= 0 ) return ct; // 2018.02.23
			//
			if ( ct > target ) return target;
			return ct;
			break;
		case 3 : // dec(%)
		case 8 : // dec(%) // 2011.04.27
			//
			if ( target <= 0 ) return 0; // 2018.02.23
			//
			if ( ( target - ct ) < 0 ) return 0;
			return ( ( ( target - ct ) * 100 ) / target );
			break;
		case 4 : // inc(%)
		case 9 : // inc(%) // 2011.04.27
			//
			if ( target <= 0 ) return 0; // 2018.02.23
			//
			if ( ct > target ) return 100;
			return ( ( ct * 100 ) / target );
			break;
		default : // ??
			return 0;
			break;
		}
	}
	return 0;
}


void _i_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( int side , int pointer , int bm ) { MFAL_STATION_SIDE[0] = side; MFAL_STATION_POINTER[0] = pointer; MFAL_STATION_BM[0] = bm; }
//void _i_SCH_MODULE_Set_MFAL_SIDE_POINTER( int side , int pointer ) { MFAL_STATION_SIDE = side; MFAL_STATION_POINTER = pointer; }
void _i_SCH_MODULE_Set_MFAL_SIDE( int side ) { MFAL_STATION_SIDE[0] = side; }
int  _i_SCH_MODULE_Get_MFAL_SIDE() { return MFAL_STATION_SIDE[0]; }
void _i_SCH_MODULE_Set_MFAL_POINTER( int pointer ) { MFAL_STATION_POINTER[0] = pointer; }
int  _i_SCH_MODULE_Get_MFAL_POINTER() { return MFAL_STATION_POINTER[0]; }
int  _i_SCH_MODULE_Get_MFAL_BM() { return MFAL_STATION_BM[0]; }
void _i_SCH_MODULE_Set_MFAL_BM( int data ) { MFAL_STATION_BM[0] = data; }

void _i_SCH_MODULE_Set_MFAL_WAFER( int wafer ) { MFAL_STATION_WAFER[0] = wafer; }
int  _i_SCH_MODULE_Get_MFAL_WAFER() { return MFAL_STATION_WAFER[0]; }




//==================================================================================================================
//==================================================================================================================
//==================================================================================================================
//==================================================================================================================
//==================================================================================================================
//==================================================================================================================
//==================================================================================================================




BOOL APIENTRY Gui_IDD_MCOOL_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int elapsed , target;
	char Buffer[64];
	char *szString[] = { "ID" , "Wafer" , "Side" , "Pointer" , "Target" , "Elapsed" , "Display" , "LastStartTime" };
	int    szSize[8] = {  35  ,      60 ,     60 ,        60 ,       60 ,        60 ,        60 ,             200 };
	LPNMHDR lpnmh;
	LV_DISPINFO *lpdi;
	time_t finish;
	static int tid;
	//

	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		KillTimer( hdlg , tid );
		break;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MoveCenterWindow( hdlg );
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 8 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE );
		//
		tid = SetTimer( hdlg , 3000 , 500 , NULL );
		return TRUE;

	case WM_TIMER:
		InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
		return TRUE;


	case WM_NOTIFY :
		lpnmh = (LPNMHDR) lParam;
		switch( lpnmh->code ) {
		case LVN_GETDISPINFO :
			lpdi = (LV_DISPINFO *) lParam;
			//
			Buffer[0] = 0; // 2016.02.02
			//
			switch( lpdi->item.iSubItem ) {
			case 0 : // ID
				sprintf( Buffer , "%d" , lpdi->item.iItem + 1 );
				break;

			case 1 : // Wafer
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					sprintf( Buffer , "%d" , MFIC_STATION_WAFER[ lpdi->item.iItem ] );
				}
				break;

			case 2 : // Side
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					sprintf( Buffer , "%c" , MFIC_STATION_SIDE[ lpdi->item.iItem ] + 'A' );
				}
				break;

			case 3 : // Pointer
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					sprintf( Buffer , "%d" , MFIC_STATION_POINTER[ lpdi->item.iItem ] );
				}
				break;

			case 4 : // Target
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					time( &finish );
					elapsed = KPLT_CHECK_MFIC_RUN_TIMER2( lpdi->item.iItem + 1 , finish , &target );
					if ( target < 0 ) {
						if      ( target == -1 ) {
							sprintf( Buffer , "-INF-" );
						}
						else if ( target == -2 ) {
							sprintf( Buffer , "-INF.A-" );
						}
						else {
							sprintf( Buffer , "-INF.?-" );
						}
					}
					else {
						sprintf( Buffer , "%d" , target );
					}
				}
				break;

			case 5 : // Elapsed
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					time( &finish );
					elapsed = KPLT_CHECK_MFIC_RUN_TIMER2( lpdi->item.iItem + 1 , finish , &target );
					if ( target < 0 ) {
						sprintf( Buffer , "%d" , elapsed );
					}
					else {
						if ( elapsed >= target ) {
							sprintf( Buffer , "-End-" );
						}
						else {
							sprintf( Buffer , "%d" , elapsed );
						}
					}
				}
				break;

			case 6 : // Display
				if ( MFIC_STATION_WAFER[ lpdi->item.iItem ] <= 0 ) {
					sprintf( Buffer , "" );
				}
				else {
					if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() == 0 ) {
						sprintf( Buffer , "" );
					}
					else {
						time( &finish );
						sprintf( Buffer , "%d" , _i_SCH_MODULE_Chk_MFIC_RUN_TIMER( lpdi->item.iItem + 1 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() , finish ) );
					}
				}
				break;

			case 7 : // Last Start Time
				KPLT_CHECK_MFIC_START_TIME( lpdi->item.iItem + 1 , Buffer );
				break;

			}
			//
//			lstrcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
			//
			strcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
			//
			return TRUE;

		case NM_DBLCLK :


			return TRUE;

		case LVN_ITEMCHANGED :


			return TRUE;

		case LVN_KEYDOWN :


			return TRUE;
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;

		case IDRETRY : // all end
			KPLT_RESET_MFIC_CURRTRG_TIMER( -1 , TRUE );
			return TRUE;

		case IDABORT : // select end
			target = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
			if ( target >= 0 ) {
				while ( TRUE ) {
					KPLT_RESET_MFIC_CURRTRG_TIMER( target + 1 , TRUE );
					target = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , target , LVNI_SELECTED );
					if ( target < 0 ) break;
				}
			}
			return TRUE;

		case IDYES : // all +30
			KPLT_RESET_MFIC_CURRTRG_TIMER( -1 , FALSE );
			return TRUE;

		case IDNO : // select +30
			target = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
			if ( target >= 0 ) {
				while ( TRUE ) {
					KPLT_RESET_MFIC_CURRTRG_TIMER( target + 1 , FALSE );
					target = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , target , LVNI_SELECTED );
					if ( target < 0 ) break;
				}
			}
			return TRUE;

		case IDOK :
			InvalidateRect( hdlg , NULL , TRUE );
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
