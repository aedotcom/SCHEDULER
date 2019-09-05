#include "default.h"

#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "system_Tag.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
//================================================================================
//================================================================================
int	sch_FM_LAST_SELECTED_PM = -1;

//================================================================================
//================================================================================

void INIT_SCHEDULER_CONTROL_ONESELECT_DATA( int apmode , int side ) {
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		sch_FM_LAST_SELECTED_PM = -1;
	}
}

//================================================================================
//================================================================================
int  _i_SCH_ONESELECT_Get_LastSelectPM() {
	return sch_FM_LAST_SELECTED_PM;
}
//================================================================================
//================================================================================
void _i_SCH_ONESELECT_Set_LastSelectPM( int data ) { // 2007.11.12
	sch_FM_LAST_SELECTED_PM = data;
}
//================================================================================
//================================================================================

void _i_SCH_ONESELECT_Reset_First_What( BOOL check , int side , int pointer ) { // 2006.03.10
//	int i , m;
	if ( !check ) return;
	//
	_i_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( side , pointer , 0 ); // 2008.01.21
/*
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i );
		if ( m > 0 ) {
			if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , 0 , i , m );
			}
			else {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , 0 , i , -m );
			}
		}
		else if ( m < 0 ) {
			if ( _i_SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , 0 , i , -m );
			}
			else {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , 0 , i , m );
			}
		}
		_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , 0 );
	}
*/
	//====================================================================================================================
}

int _i_SCH_ONESELECT_Get_First_One_What( BOOL check , int side , int pointer , BOOL indexsel , BOOL retbmmode , int *mdl ) {
	int i , j , m , c;

	*mdl = -1;
	//====================================================================================================================
	if ( check ) { // 2010.03.06
		if ( _i_SCH_CLUSTER_Get_PathRange( side , pointer ) >= MAX_CLUSTER_DEPTH ) check = FALSE;
	}
	//====================================================================================================================
	if ( check ) {
		//====================================================================================================================
		// Buffering2
		//====================================================================================================================
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , m );
		}
		//====================================================================================================================
		for ( j = 0 ; j < 2 ; j++ ) {
			//
			for ( i = 0 , c = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
				//
				if ( m > 0 ) {
					if ( c == 0 ) {
						if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
							if ( indexsel ) {
								if ( ( j == 1 ) || ( i > sch_FM_LAST_SELECTED_PM ) ) {
									c = 1;
									if ( retbmmode ) {
										*mdl = m - PM1 + BM1;
									}
									else {
										*mdl = m;
									}
									sch_FM_LAST_SELECTED_PM = i;
									break;
								}
							}
							else {
								if ( ( j == 1 ) || ( m > sch_FM_LAST_SELECTED_PM ) ) {
									c = 1;
									if ( retbmmode ) {
										*mdl = m - PM1 + BM1;
									}
									else {
										*mdl = m;
									}
									sch_FM_LAST_SELECTED_PM = m;
									break;
								}
							}
						}
					}
				}
			}
			if ( c == 1 ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
					if ( retbmmode ) {
						if ( ( m - PM1 + BM1 ) != *mdl ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , 0 , i );
						}
					}
					else {
						if ( m != *mdl ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , 0 , i );
						}
					}
				}
				return 1;
			}
		}
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , 0 );
		}
		//====================================================================================================================
	}
	//====================================================================================================================
	else {
		c = 0;
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
			if ( m > 0 ) {
				if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
					c++;
					if ( *mdl == -1 ) {
						if ( retbmmode ) {
							*mdl = m - PM1 + BM1;
						}
						else {
							*mdl = m;
						}
					}
				}
			}
		}
		return c;
	}
	//====================================================================================================================
	return 0;
}



