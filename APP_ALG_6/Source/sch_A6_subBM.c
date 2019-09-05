//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_User_Parameter.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
//================================================================================


#include "sch_A6_sub.h"
//------------------------------------------------------------------------------------------
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) return FALSE;
	}
	return TRUE;
}

//==========================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) return FALSE;
	}
	return TRUE;
}


//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( int ch , int checksts ) {
	int i , k = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( checksts == -1 ) {
				k++;
			}
			else {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == checksts ) {
					k++;
				}
			}
		}
	}
	return k;
}

//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( int ch ) {
	int i , k = 99999999 , s;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				s = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
				//
				if ( k == 99999999 ) {
					k = s;
				}
				else if ( s < k ) {
					k = s;
				}
			}
		}
	}
	return k;
}

//==========================================================================================================================
int SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( int ch , int slot ) { // 2010.09.09
	if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( ch , slot ) , _SCH_MODULE_Get_BM_POINTER( ch , slot ) ) == PATHDO_WAFER_RETURN ) return TRUE;
	return FALSE;
}

//==========================================================================================================================
int SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( int ch , int checksts ) {
	int i , k = 0;
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( checksts == -1 ) {
					k++;
				}
				else {
					if ( checksts == BUFFER_INWAIT_STATUS ) {
						if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == checksts ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( ch , i ) ) { // 2010.09.09
								k++;
							}
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( ch , i ) ) { // 2010.09.09
							k++;
						}
						else {
							if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == checksts ) {
								k++;
							}
						}
					}
				}
			}
		}
		return k;
	}
	else {
		return SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( ch , checksts );
	}
}

//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_FREE_COUNT_STYLE_6( int ch ) {
	int i , k = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) k++;
	}
	return k;
}

//==========================================================================================================================

/*
// 2015.03.13
BOOL SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_and_DUMMY_STYLE_6( int ch ) { // 2015.01.12
	int i , s , p;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				s = _SCH_MODULE_Get_BM_SIDE( ch , i );
				p = _SCH_MODULE_Get_BM_POINTER( ch , i );
				//
				if ( _SCH_CLUSTER_Get_PathIn( s , p ) >= BM1 ) return TRUE;
				//
			}
		}
	}
	return FALSE;
}
*/


// 2015.03.13
BOOL SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_and_DUMMY_STYLE_6( int ch ) { // 2015.01.12
	int i , s , p;
	BOOL hasd , hasn;
	//
	return FALSE; // 2015.03.13

	hasd = FALSE;
	hasn = FALSE;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				s = _SCH_MODULE_Get_BM_SIDE( ch , i );
				p = _SCH_MODULE_Get_BM_POINTER( ch , i );
				//
				if ( _SCH_CLUSTER_Get_PathIn( s , p ) >= BM1 ) {
					hasd = TRUE;
				}
				else {
					hasn = TRUE;
				}
				//
			}
		}
	}
	//
	if ( hasn && hasd ) return TRUE;
	//
	return FALSE;
}
