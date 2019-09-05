#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "sch_sub.h"
#include "User_Parameter.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_prm.h" // 2017.07.26
#include "iolog.h"

#include "INF_sch_CommonUser.h"

//-------------------------------------------
int				Side_Check_Flag[ MAX_CASSETTE_SIDE ];
int				Side_Check_Last_Supplied_Flag[ MAX_CASSETTE_SIDE ]; // 2003.04.23
int				Side_Check_Target_Cluster_Count[ MAX_CASSETTE_SIDE ]; // 2003.10.10
int				Side_Check_Current_Run_Count[ MAX_CASSETTE_SIDE ]; // 2003.10.10
//
int				Side_Check_Set_Action_Count[ MAX_CASSETTE_SIDE ]; // 2016.03.09
//-------------------------------------------

void _SCH_CASSETTE_Set_Count_Action( int s , BOOL ); // 2018.10.01

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Side Scheck Control Use Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_Side_Check_Flag( int side ) {
	return Side_Check_Flag[side];
}
int SCHEDULER_CONTROL_Get_Side_Check_Supply_Flag( int side ) {
	return Side_Check_Last_Supplied_Flag[side];
}
int SCHEDULER_CONTROL_Get_Side_Check_Current_Run_Count( int side ) {
	return Side_Check_Current_Run_Count[side];
}
//
BOOL _i_SCH_CASSETTE_Check_Side_Monitor( int side ) {
	if ( side >= 100 ) { // 2014.06.27
		if ( Side_Check_Last_Supplied_Flag[side-100] == 1 ) return TRUE;
		return FALSE;
	}
	else {
		if ( Side_Check_Flag[side] > 3 ) return TRUE;
		Side_Check_Flag[side]++;
		return FALSE;
	}
}
//

void _i_SCH_CASSETTE_Reset_Side_Monitor( int side , int init ) {
/*
	// 2010.11.11
	Side_Check_Flag[side] = 0;
//	Side_Check_Last_Supplied_Flag[side] = FALSE; // 2008.06.22
	Side_Check_Last_Supplied_Flag[side] = init; // 2008.06.22
	if ( init ) Side_Check_Current_Run_Count[side] = 0; // 2003.10.10
*/
	//
	if      ( init == 0 ) { // 2010.11.11 // Done
		Side_Check_Flag[side] = 0;
		Side_Check_Last_Supplied_Flag[side] = 0; // 2008.06.22
	}
	else if ( init == 1 ) { // Reset
		Side_Check_Flag[side] = 0;
		//
		// 2014.06.19
//		Side_Check_Last_Supplied_Flag[side] = 1; // 2008.06.22 // 2014.06.19
		//
		Side_Check_Last_Supplied_Flag[side] = -1; // 2008.06.22 // 2014.06.19
		//
		Side_Check_Current_Run_Count[side] = 0; // 2003.10.10
		//
		Side_Check_Set_Action_Count[side] = 0; // 2016.03.09
		//
	}
	else { // 2010.11.11
		//
		Side_Check_Set_Action_Count[side] = 0; // 2016.03.09
		//
		_SCH_CASSETTE_Set_Count_Action( side , FALSE ); // 2018.10.01
		//
	}
}
//
void SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( int side , int count ) { // 2003.10.10
	if ( count <= 0 ) {
		Side_Check_Target_Cluster_Count[side] = 0;
	}
	else {
		Side_Check_Target_Cluster_Count[side] = count;
	}
	//
	if ( Side_Check_Target_Cluster_Count[side] <= 0 ) _SCH_CASSETTE_Set_Count_Action( side , FALSE ); // 2016.03.09 // 2018.10.01
	//
	Side_Check_Set_Action_Count[side] = 0; // 2016.03.09
	//
}
//
void SCHEDULER_CONTROL_ReCheck_Side_Monitor_Cluster_Count_Zero( int side ) { // 2016.03.09
	//
	if ( Side_Check_Target_Cluster_Count[side] > 0 ) return;
	//
	Side_Check_Set_Action_Count[side]++;
	//
//	if ( Side_Check_Set_Action_Count[side] < 256 ) return; // 2018.09.28
	if ( Side_Check_Set_Action_Count[side] < 32 ) return; // 2018.09.28
	//
	_SCH_CASSETTE_Set_Count_Action( side , FALSE ); // 2018.10.01
	//
	Side_Check_Set_Action_Count[side] = 0;
	//
}
//
//
//void SCHEDULER_CONTROL_Dec_Side_Monitor_Cluster_Count( int side ) { // 2006.06.22 // 2016.03.09
//	if ( Side_Check_Target_Cluster_Count[side] > 0 ) Side_Check_Target_Cluster_Count[side]--; // 2016.03.09
//} // 2016.03.09
//
int _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( int side ) { // 2003.10.21
	return Side_Check_Target_Cluster_Count[side];
}
//

//
//
/*
//
// 2016.05.03
//
BOOL _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( int side , BOOL EndCheck , int sup_style , BOOL usermode ) { // 2003.04.23
	int i , j , f , ss , es;
	int fr , frp;

	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
		//----------------------------------------------------------------
		if ( _SCH_COMMON_SIDE_SUPPLY( side , EndCheck , sup_style , &i ) ) return i;
		//----------------------------------------------------------------
	}
	else { // 2010.09.13
		if ( sup_style >= 0 ) {
			return TRUE;
		}
		else {
			//----------------------------------------------------------------
			if ( _SCH_COMMON_SIDE_SUPPLY( side , EndCheck , sup_style , &i ) ) return i;
			//----------------------------------------------------------------
		}
	}
	//----------------------------------------------------------------
	if ( usermode ) return TRUE; // 2012.02.16
	//----------------------------------------------------------------
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) {
		return TRUE;
	}
	else { // 2008.12.24
		if ( sup_style == 2 ) {
//			_i_SCH_SUB_Remain_for_CM( i ); // 2009.03.03
			_i_SCH_SUB_Remain_for_CM( side ); // 2009.03.03
			j = _i_SCH_MODULE_Get_TM_DoPointer( side );
			if ( j < MAX_CASSETTE_SLOT_SIZE ) {
				if ( _i_SCH_CLUSTER_Get_SupplyID( side , j ) < 0 ) return TRUE;
			}
		}
	}
	//----------------------------------------------------------------
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance // 2003.10.13
		//
		SCHEDULER_CONTROL_ReCheck_Side_Monitor_Cluster_Count_Zero( side ); // 2016.03.09
		//
		if ( Side_Check_Target_Cluster_Count[side] <= 0 ) {
			//
			Side_Check_Current_Run_Count[side] = 0;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
				else if ( i != side ) {
//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
					if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
				}
			}
			return FALSE;
		}
		else { // 2009.04.02
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				//
				if ( Side_Check_Target_Cluster_Count[i] > 0 ) break;
				//
			}
			if ( i == MAX_CASSETTE_SIDE ) return TRUE;
			//
			// 2014.12.22
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				//
				if ( _i_SCH_MODULE_Get_FM_SupplyCount( side ) == 0 ) {
					//
					if ( Side_Check_Last_Supplied_Flag[ side ] == 1 ) {
						//
						f = 0;
						//
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							//
							if ( i != side ) {
								//
								if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
								//
								if ( Side_Check_Last_Supplied_Flag[i] == 1 ) continue;
								//
								if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue;
								//
								if ( Side_Check_Current_Run_Count[i] > 0 ) break;
								//
								if ( _i_SCH_MODULE_Get_FM_SupplyCount( i ) == 0 ) continue;
								//
								f = 1;
								//
							}
						}
						//
						if ( i == MAX_CASSETTE_SIDE ) {
							//
							if ( f == 1 ) {
								//
								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
									if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 0;
									else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = 1;
								}
								//
							}
							//
						}
						//
					}
					else { // 2015.06.05
						//
						if ( Side_Check_Current_Run_Count[side] <= 0 ) {
							//
							f = -1;
							//
							for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
								//
								if ( i != side ) {
									//
									if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									//
									if ( Side_Check_Last_Supplied_Flag[i] == 1 ) continue;
									//
									if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue;
									//
									if ( Side_Check_Current_Run_Count[i] > 0 ) continue;
									//
									if ( _i_SCH_MODULE_Get_FM_SupplyCount( i ) > 0 ) continue;
									//
									if ( f == -1 ) {
										//
										if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( side ) ) {
											f = i;
										}
										//
									}
									else {
										//
										if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( f ) ) {
											f = i;
										}
										//
									}
									//
								}
							}
							//
							if ( f != -1 ) {
								//
								Side_Check_Last_Supplied_Flag[f] = 0;
								Side_Check_Last_Supplied_Flag[side] = 1;
								//
							}
							//
						}
						//
					}
				}
			}
			//
		}
	}
	//----------------------------------------------------------------
	// 2014.06.26
	//
	f = 0;
	fr = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( Side_Check_Last_Supplied_Flag[i] == -1 ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				f++;
			}
		}
		else if ( Side_Check_Last_Supplied_Flag[i] == 1 ) {
			fr = 1;
		}
	}
	//
	if ( f > 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( Side_Check_Last_Supplied_Flag[i] == -1 ) {
				if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
					Side_Check_Last_Supplied_Flag[i] = ( fr == 0 );
				}
			}
		}
	}
	//----------------------------------------------------------------
	f = -1;
	fr = 0;
	frp = -1; // 2005.01.13
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( Side_Check_Last_Supplied_Flag[i] == 1 ) {
//			if ( _i_SCH_SYSTEM_USING_GET(i) < 10 ) { // 2005.07.29
//			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29 // 2008.08.27
//				Side_Check_Last_Supplied_Flag[i] = FALSE; // 2008.08.27
//				Side_Check_Current_Run_Count[i] = 0; // 2003.10.10 // 2008.08.27
//			} // 2008.08.27
//			else { // 2008.08.27
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29 // 2008.08.27
				fr++;
				if ( EndCheck ) { // 2004.05.24
					if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) {
						Side_Check_Last_Supplied_Flag[i] = 0;
						Side_Check_Current_Run_Count[i] = 0;
						continue;
					}
					else {
						if      ( sup_style == 1 ) { // FEM
//							if ( !_i_SCH_SUB_Remain_for_FM( i ) ) { // 2011.10.25
							if ( !_i_SCH_SUB_Remain_for_FM_Sub( i ) ) { // 2011.10.25
								Side_Check_Last_Supplied_Flag[i] = 0;
								Side_Check_Current_Run_Count[i] = 0;
								continue;
							}
						}
						else if ( sup_style == 0 ) { // 2005.01.06 // CM
							if ( !_i_SCH_SUB_Remain_for_CM( i ) ) {
								Side_Check_Last_Supplied_Flag[i] = 0;
								Side_Check_Current_Run_Count[i] = 0;
								continue;
							}
						}
						else if ( sup_style == 2 ) { // 2006.09.07 // Batch
//							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) < 4 ) { // 2008.08.25
							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) < 999999 ) { // 2008.08.25
								Side_Check_Last_Supplied_Flag[i] = 0;
								Side_Check_Current_Run_Count[i] = 0;
								continue;
							}
							else {
								if ( !_i_SCH_SUB_Remain_for_CM( i ) ) {
									Side_Check_Last_Supplied_Flag[i] = 0;
									Side_Check_Current_Run_Count[i] = 0;
									continue;
								}
							}
						}
					}
				}
				if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // 2003.10.13
					if ( Side_Check_Target_Cluster_Count[i] <= 0 ) { // 2003.10.14
						Side_Check_Last_Supplied_Flag[i] = 0;
						Side_Check_Current_Run_Count[i] = 0; // 2003.10.10
					}
					else {
						//====================================================================================
						// 2007.11.27
						//====================================================================================
						if ( Side_Check_Current_Run_Count[i] > 0 ) {
							if ( Side_Check_Current_Run_Count[i] < Side_Check_Target_Cluster_Count[i] ) {
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						//====================================================================================
						f = i;
					}
				}
				else {
					f = i;
				}
			}
		}
//		else { // 2005.01.06 // 2014.06.26
		else if ( Side_Check_Last_Supplied_Flag[i] == 0 ) { // 2005.01.06 // 2014.06.26
//			if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) { // 2005.07.29
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				fr++;
				if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) {
					if ( Side_Check_Target_Cluster_Count[i] <= 0 ) {
						frp = i;
					}
				}
			}
		}
	}
	//
	if ( f == -1 ) {
		if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // 2005.01.06
			if ( fr >= 2 ) {
				if ( frp == -1 ) return TRUE; // 2005.01.13
				f = frp;
			}
			else {
				return TRUE;
			}
		}
		else {
			return TRUE;
		}
	}
	//
	for ( j = 0 ; j < 2 ; j++ ) {
		if ( j == 0 ) {
			ss = f + 1;
			es = MAX_CASSETTE_SIDE;
		}
		else {
			ss = 0;
			es = f + 1;
		}
		for ( i = ss ; i < es ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( EndCheck ) { // 2004.05.24
					if ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) {
						if      ( sup_style == 1 ) {
//							if ( _i_SCH_SUB_Remain_for_FM( i ) ) { // 2011.10.25
							if ( _i_SCH_SUB_Remain_for_FM_Sub( i ) ) { // 2011.10.25
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						else if ( sup_style == 0 ) { // 2005.01.06
							if ( _i_SCH_SUB_Remain_for_CM( i ) ) {
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						else if ( sup_style == 2 ) { // 2006.09.07
//							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) >= 4 ) { // 2008.08.25
							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) >= 999999 ) { // 2008.08.25
								if ( _i_SCH_SUB_Remain_for_CM( i ) ) {
									if ( i == side ) return TRUE;
									else             return FALSE;
								}
							}
						}
					}
				}
				else {
					if ( i == side ) return TRUE;
					else             return FALSE;
				}
			}
		}
	}
	return TRUE;
}
//
//
//
*/

BOOL SCHEDULER_CONTROL_First_Supply_Fail( int side ) { // 2017.07.26
	int i , pt , m;
	//
	if ( _i_SCH_PRM_GET_OPTIMIZE_MODE() != 1 ) return FALSE;
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		pt = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
	}
	else {
		pt = _i_SCH_MODULE_Get_TM_DoPointer( side );
	}
	//
	if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return TRUE;
	//
	m = _i_SCH_CLUSTER_Get_PathRange( side , pt );
	//
	if      ( m < 0 ) return TRUE;
	else if ( m == 0 ) return FALSE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , i );
		//
		if ( m > 0 ) {
			//
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , m ) == MUF_01_USE ) return FALSE;
			//
		}
	}
	//
	return TRUE;
	//
}


//
// 2016.05.03
//
BOOL _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( int side , BOOL EndCheck , int sup_style , BOOL usermode , BOOL log ) { // 2003.04.23
	int i , j , f , ss , es;
	int fr , frp , TrgCount;

//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13 // 2017.10.10
	if ( !_SCH_SUB_NOCHECK_RUNNING( side ) ) { // 2010.09.13 // 2017.10.10
		//
		if ( !log ) {
			//----------------------------------------------------------------
			if ( _SCH_COMMON_SIDE_SUPPLY( side , EndCheck , sup_style , &i ) ) return i;
			//----------------------------------------------------------------
		}
		//
	}
	else { // 2010.09.13
		if ( sup_style >= 0 ) {
			return TRUE;
		}
		else {
			//
			if ( !log ) {
				//----------------------------------------------------------------
				if ( _SCH_COMMON_SIDE_SUPPLY( side , EndCheck , sup_style , &i ) ) return i;
				//----------------------------------------------------------------
			}
			//
		}
	}
	//----------------------------------------------------------------
	if ( usermode ) return TRUE; // 2012.02.16
	//----------------------------------------------------------------
	//
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) return TRUE;
	//
	//
	//
	// 2016.12.05
	//
	//
	if ( !log ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			//
			if ( _i_SCH_MODULE_Get_FM_OutCount( side ) == 0 ) {
				//
				if ( Side_Check_Current_Run_Count[side] > 0 ) {
					//
					Side_Check_Current_Run_Count[side] = 0;
					//
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
						else if ( i != side ) {
							if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0;
						}
					}
					//
				}
			}
			//
		}
		else {
			//
			if ( _i_SCH_MODULE_Get_TM_OutCount( side ) == 0 ) {
				//
				if ( Side_Check_Current_Run_Count[side] > 0 ) {
					//
					Side_Check_Current_Run_Count[side] = 0;
					//
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
						else if ( i != side ) {
							if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0;
						}
					}
					//
				}
				//
			}
			//
		}
		//
	}
	//
	//
	if ( sup_style == 2 ) {
//			_i_SCH_SUB_Remain_for_CM( i ); // 2009.03.03
		//
		_i_SCH_SUB_Remain_for_CM( side , log ); // 2009.03.03
		//
		j = _i_SCH_MODULE_Get_TM_DoPointer( side );
		if ( j < MAX_CASSETTE_SLOT_SIZE ) {
			if ( _i_SCH_CLUSTER_Get_SupplyID( side , j ) < 0 ) return TRUE;
		}
	}
	//
	//----------------------------------------------------------------
//	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance // 2003.10.13 // 2016.05.03
		//
		//
		if ( !log ) {
			SCHEDULER_CONTROL_ReCheck_Side_Monitor_Cluster_Count_Zero( side ); // 2016.03.09
		}
		//
		//
//		if ( Side_Check_Target_Cluster_Count[side] <= 0 ) { // 2017.07.26
		if ( ( Side_Check_Target_Cluster_Count[side] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(side) ) { // 2017.07.26
			//
			if ( !log ) {
				//
				Side_Check_Current_Run_Count[side] = 0;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
					else if ( i != side ) {
	//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
						if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
					}
				}
				//
			}
			//
			return FALSE;
		}
		else { // 2009.04.02
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				//
//				if ( Side_Check_Target_Cluster_Count[i] > 0 ) break; // 2017.07.26
				if ( ( Side_Check_Target_Cluster_Count[i] > 0 ) && !SCHEDULER_CONTROL_First_Supply_Fail(i) ) break; // 2017.07.26
				//
			}
			if ( i == MAX_CASSETTE_SIDE ) return TRUE;
			//
			// 2014.12.22
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				//
//				if ( _i_SCH_MODULE_Get_FM_SupplyCount( side ) == 0 ) { // 2016.06.09
				if ( _i_SCH_MODULE_Get_FM_OutCount( side ) == 0 ) { // 2016.06.09
					//
					//
					// 2016.06.09
					//
					// 다른 Side 에서도 공급이 안되어 있을때 다른 Side 먼저 공급이 필요할때
					//
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						//
						if ( i != side ) {
							//
							if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
							//
							if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) continue; // 2016.06.09
							//
							if ( !_i_SCH_SUB_Remain_for_FM_Sub( i , log ) ) continue; // 2016.06.09
							//
//							if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue; // 2017.07.26
							if ( ( Side_Check_Target_Cluster_Count[i] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(i) ) continue; // 2017.07.26
							//
							if ( Side_Check_Current_Run_Count[i] > 0 ) continue;
							//
//							if ( _i_SCH_MODULE_Get_FM_SupplyCount( i ) > 0 ) continue; // 2016.06.09
							if ( _i_SCH_MODULE_Get_FM_OutCount( i ) > 0 ) continue; // 2016.06.09
							//
							if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( side ) ) {
								//
								//
								if ( !log ) {
									//
									//
									// 2016.11.18
									//
									for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
										if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
										else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = 0;
									}
									//
								}
								//
								return FALSE;
								//
							}
							//
						}
					}
					//
					//
					// 새로 시작한 SIDE를 바로 참여하기 위한 부분
					//
					//
//					if ( Side_Check_Last_Supplied_Flag[ side ] == 1 ) { // 2016.06.09
						//
					//
					if ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 0 ) { // 2016.06.09
					//
						if ( _i_SCH_SUB_Remain_for_FM_Sub( side , log ) ) { // 2016.06.09
							//
							f = 0;
							//
							for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
								//
								if ( i != side ) {
									//
									if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									//
									if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) continue; // 2016.06.09
									//
									if ( !_i_SCH_SUB_Remain_for_FM_Sub( i , log ) ) continue; // 2016.06.09
									//
	//								if ( Side_Check_Last_Supplied_Flag[i] == 1 ) continue; // 2016.06.09
									//
//									if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue; // 2017.07.26
									if ( ( Side_Check_Target_Cluster_Count[i] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(i) ) continue; // 2017.07.26
									//
									if ( Side_Check_Current_Run_Count[i] > 0 ) break;
									//
	//								if ( _i_SCH_MODULE_Get_FM_SupplyCount( i ) == 0 ) continue; // 2016.06.09
									if ( _i_SCH_MODULE_Get_FM_OutCount( i ) == 0 ) continue; // 2016.06.09
									//
									f = 1;
									//
								}
							}
							//
							if ( i == MAX_CASSETTE_SIDE ) {
								//
								if ( f == 1 ) {
									//
	//								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
	//									if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 0;
	//									else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = 1;
	//								}
									//
									return TRUE;
								}
								//
							}
						}
					}


						//
//					} // 2016.06.09
//					else { // 2015.06.05
						//
						/*
						//
						// 2016.06.08
						//
						if ( Side_Check_Current_Run_Count[side] <= 0 ) {
							//
							f = -1;
							//
							for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
								//
								if ( i != side ) {
									//
									if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									//
									if ( Side_Check_Last_Supplied_Flag[i] == 1 ) continue;
									//
									if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue;
									//
									if ( Side_Check_Current_Run_Count[i] > 0 ) continue;
									//
									if ( _i_SCH_MODULE_Get_FM_SupplyCount( i ) > 0 ) continue;
									//
									if ( f == -1 ) {
										//
										if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( side ) ) {
											f = i;
										}
										//
									}
									else {
										//
										if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( f ) ) {
											f = i;
										}
										//
									}
									//
								}
							}
							//
							if ( f != -1 ) {
								//
								Side_Check_Last_Supplied_Flag[f] = 0;
								Side_Check_Last_Supplied_Flag[side] = 1;
								//
							}
							//
							}
							//
						}
						//
						*/
//					}
				}
				else { // 2016.06.09
					//
					if ( Side_Check_Current_Run_Count[side] == 0 ) {
						//
						f = 0;
						//
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							//
							if ( i != side ) {
								//
								if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
								//
								if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) continue; // 2016.06.09
								//
								if ( !_i_SCH_SUB_Remain_for_FM_Sub( i , log ) ) continue; // 2016.06.09
								//
//								if ( Side_Check_Target_Cluster_Count[i] <= 0 ) continue; // 2017.07.26
								if ( ( Side_Check_Target_Cluster_Count[i] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(i) ) continue; // 2017.07.26
								//
								if ( Side_Check_Current_Run_Count[i] > 0 ) break;
								//
								if ( _i_SCH_MODULE_Get_FM_OutCount( i ) != 0 ) continue;
								//
								f = 1;
								//
							}
						}
						//
						if ( i == MAX_CASSETTE_SIDE ) {
							//
							if ( f == 1 ) {
								//
								//
								//
								if ( !log ) {
									//
									//
									// 2016.11.18
									//
									for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
										if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
										else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = 0;
									}
									//
								}
								//
								return FALSE;
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
//	} // 2016.05.03
	//----------------------------------------------------------------
	// 2014.06.26
	//
	f = 0;
	fr = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( Side_Check_Last_Supplied_Flag[i] == -1 ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				f++;
			}
		}
		else if ( Side_Check_Last_Supplied_Flag[i] == 1 ) {
			fr = 1;
		}
	}
	//
	//
	if ( !log ) {
		//
		if ( f > 0 ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( Side_Check_Last_Supplied_Flag[i] == -1 ) {
					if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
						Side_Check_Last_Supplied_Flag[i] = ( fr == 0 );
					}
				}
			}
		}
		//
	}
	//----------------------------------------------------------------
	f = -1;
	fr = 0;
	frp = -1; // 2005.01.13
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( Side_Check_Last_Supplied_Flag[i] == 1 ) {
//			if ( _i_SCH_SYSTEM_USING_GET(i) < 10 ) { // 2005.07.29
//			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29 // 2008.08.27
//				Side_Check_Last_Supplied_Flag[i] = FALSE; // 2008.08.27
//				Side_Check_Current_Run_Count[i] = 0; // 2003.10.10 // 2008.08.27
//			} // 2008.08.27
//			else { // 2008.08.27
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29 // 2008.08.27
				fr++;
				if ( EndCheck ) { // 2004.05.24
					if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) {
						//
						if ( !log ) {
							//
							Side_Check_Last_Supplied_Flag[i] = 0;
							Side_Check_Current_Run_Count[i] = 0;
							//
						}
						//
						continue;
					}
					else {
						if      ( sup_style == 1 ) { // FEM
//							if ( !_i_SCH_SUB_Remain_for_FM( i ) ) { // 2011.10.25
							if ( !_i_SCH_SUB_Remain_for_FM_Sub( i , log ) ) { // 2011.10.25
								//
								if ( !log ) {
									//
									Side_Check_Last_Supplied_Flag[i] = 0;
									Side_Check_Current_Run_Count[i] = 0;
									//
								}
								continue;
							}
						}
						else if ( sup_style == 0 ) { // 2005.01.06 // CM
							if ( !_i_SCH_SUB_Remain_for_CM( i , log ) ) {
								//
								if ( !log ) {
									//
									Side_Check_Last_Supplied_Flag[i] = 0;
									Side_Check_Current_Run_Count[i] = 0;
									//
								}
								//
								continue;
							}
						}
						else if ( sup_style == 2 ) { // 2006.09.07 // Batch
//							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) < 4 ) { // 2008.08.25
							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) < 999999 ) { // 2008.08.25
								//
								if ( !log ) {
									//
									Side_Check_Last_Supplied_Flag[i] = 0;
									Side_Check_Current_Run_Count[i] = 0;
									//
								}
								//
								continue;
							}
							else {
								if ( !_i_SCH_SUB_Remain_for_CM( i , log ) ) {
									//
									if ( !log ) {
										//
										Side_Check_Last_Supplied_Flag[i] = 0;
										Side_Check_Current_Run_Count[i] = 0;
										//
									}
									//
									continue;
								}
							}
						}
					}
				}
//				if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // 2003.10.13 // 2016.05.03
//					if ( Side_Check_Target_Cluster_Count[i] <= 0 ) { // 2003.10.14 // 2017.07.26
					if ( ( Side_Check_Target_Cluster_Count[i] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(i) ) { // 2017.07.26
						//
						if ( !log ) {
							//
							Side_Check_Last_Supplied_Flag[i] = 0;
							Side_Check_Current_Run_Count[i] = 0; // 2003.10.10
							//
						}
						//
					}
					else {
						//====================================================================================
						// 2007.11.27
						//====================================================================================
						if ( Side_Check_Current_Run_Count[i] > 0 ) {
							//
							//
							// 2016.05.03
							//
							if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance
								TrgCount = Side_Check_Target_Cluster_Count[i];
							}
							else {
								TrgCount = 1;
							}
							//
//							if ( Side_Check_Current_Run_Count[i] < Side_Check_Target_Cluster_Count[i] ) { // 2016.05.03
							if ( Side_Check_Current_Run_Count[i] < TrgCount ) { // 2016.05.03
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						//====================================================================================
						f = i;
					}
//				} // 2016.05.03
//				else { // 2016.05.03
//					f = i; // 2016.05.03
//				} // 2016.05.03
			}
		}
//		else { // 2005.01.06 // 2014.06.26
		else if ( Side_Check_Last_Supplied_Flag[i] == 0 ) { // 2005.01.06 // 2014.06.26
//			if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) { // 2005.07.29
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				fr++;
//				if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // 2016.05.03
//					if ( Side_Check_Target_Cluster_Count[i] <= 0 ) { // 2017.07.26
					if ( ( Side_Check_Target_Cluster_Count[i] <= 0 ) || SCHEDULER_CONTROL_First_Supply_Fail(i) ) { // 2017.07.26
						frp = i;
					}
//				} // 2016.05.03
			}
		}
	}
	//
	if ( f == -1 ) {
		//
//		if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // 2005.01.06 // 2016.05.03
			if ( fr >= 2 ) {
				if ( frp == -1 ) return TRUE; // 2005.01.13
				f = frp;
			}
			else {
				return TRUE;
			}
//		} // 2016.05.03
//		else { // 2016.05.03
//			return TRUE; // 2016.05.03
//		} // 2016.05.03
	}
	//
	for ( j = 0 ; j < 2 ; j++ ) {
		if ( j == 0 ) {
			ss = f + 1;
			es = MAX_CASSETTE_SIDE;
		}
		else {
			ss = 0;
			es = f + 1;
		}
		for ( i = ss ; i < es ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( EndCheck ) { // 2004.05.24
					if ( _i_SCH_SYSTEM_MODE_END_GET( i ) == 0 ) {
						if      ( sup_style == 1 ) {
//							if ( _i_SCH_SUB_Remain_for_FM( i ) ) { // 2011.10.25
							if ( _i_SCH_SUB_Remain_for_FM_Sub( i , log ) ) { // 2011.10.25
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						else if ( sup_style == 0 ) { // 2005.01.06
							if ( _i_SCH_SUB_Remain_for_CM( i , log ) ) {
								if ( i == side ) return TRUE;
								else             return FALSE;
							}
						}
						else if ( sup_style == 2 ) { // 2006.09.07
//							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) >= 4 ) { // 2008.08.25
							if ( _i_SCH_MODULE_Get_FM_SwWait( i ) >= 999999 ) { // 2008.08.25
								if ( _i_SCH_SUB_Remain_for_CM( i , log ) ) {
									if ( i == side ) return TRUE;
									else             return FALSE;
								}
							}
						}
					}
				}
				else {
					if ( i == side ) return TRUE;
					else             return FALSE;
				}
			}
		}
	}
	return TRUE;
}
//
BOOL _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_FDHC( int side , BOOL EndCheck , int sup_style ) { // 2003.04.23
	BOOL Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , EndCheck , sup_style , FALSE , FALSE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	return Res;
}
//

//
//
/*
// 2016.05.03
//
void _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( int side , int wfrcnt0 ) { // 2003.04.23
	int i;
	int wfrcnt;
//	int t; // 2005.01.06

	//----------------------------------------------------------------
	wfrcnt = wfrcnt0 % 1000;
	//----------------------------------------------------------------
	_i_SCH_CASSETTE_Reset_Side_Monitor( side , 0 ); // 2007.11.27
	//----------------------------------------------------------------
	if ( _SCH_COMMON_SIDE_SUPPLY( side , wfrcnt , -99 , &i ) ) return;
	//----------------------------------------------------------------
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) return;
	//----------------------------------------------------------------
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance // 2003.10.13
		//
		SCHEDULER_CONTROL_ReCheck_Side_Monitor_Cluster_Count_Zero( side ); // 2016.03.09
		//
		if ( Side_Check_Target_Cluster_Count[side] <= 0 ) { // 2003.10.13
			//
			Side_Check_Current_Run_Count[side] = 0;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
				else if ( i != side ) {
//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
					if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
				}
			}
			return;
		}
		//----------------------------------------------------------------
		// 2005.01.06
		//----------------------------------------------------------------
/ *
		t = 999999; // 2003.10.10
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2003.10.10
			if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) {
				if ( Side_Check_Target_Cluster_Count[i] > 0 ) { // 2003.10.13
					if ( t > Side_Check_Target_Cluster_Count[i] ) {
						t = Side_Check_Target_Cluster_Count[i];
					}
				}
			}
		}
		if ( t == 999999 ) t = 1;
		//----------------------------------------------------------------
		t = Side_Check_Target_Cluster_Count[side] - t; // 2003.10.10
		//----------------------------------------------------------------
		Side_Check_Current_Run_Count[side]++; // 2003.10.10
		if ( Side_Check_Current_Run_Count[side] > t ) { // 2003.10.10
			Side_Check_Current_Run_Count[side] = 0; // 2003.10.10
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = TRUE;
				else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = FALSE;
			}
		}
* /
//		Side_Check_Current_Run_Count[side]++; // 2007.03.15
		Side_Check_Current_Run_Count[side] += wfrcnt; // 2007.03.15
		if ( Side_Check_Current_Run_Count[side] >= Side_Check_Target_Cluster_Count[side] ) {
			Side_Check_Current_Run_Count[side] = 0;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
				else if ( i != side ) {
//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
					if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
				}
			}
		}
		//----------------------------------------------------------------
//_i_SCH_LOG_LOT_PRINTF( side , "Side_Monitor_SupplyDone [%d]\n" , wfrcnt0 );
		//----------------------------------------------------------------
		//----------------------------------------------------------------
	}
	else {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
			else if ( i != side ) {
//				Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
				if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
			}
		}
	}
}

//
//
*/
//
//
// 2016.05.03
//
//void _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( int side , int wfrcnt0 ) { // 2003.04.23
int _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone_Sub( int side , int wfrcnt0 ) { // 2016.06.09
	int i , TrgCount;
	int wfrcnt;
//	int t; // 2005.01.06

	//----------------------------------------------------------------
	wfrcnt = wfrcnt0 % 1000;
	//----------------------------------------------------------------
	_i_SCH_CASSETTE_Reset_Side_Monitor( side , 0 ); // 2007.11.27
	//----------------------------------------------------------------
	if ( _SCH_COMMON_SIDE_SUPPLY( side , wfrcnt , -99 , &i ) ) return 1;
	//----------------------------------------------------------------
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) return 2;
	//----------------------------------------------------------------
//	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance // 2003.10.13 // 2016.05.03
		//
		SCHEDULER_CONTROL_ReCheck_Side_Monitor_Cluster_Count_Zero( side ); // 2016.03.09
		//
		if ( Side_Check_Target_Cluster_Count[side] <= 0 ) { // 2003.10.13
			//
			Side_Check_Current_Run_Count[side] = 0;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
				else if ( i != side ) {
//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
					if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
				}
			}
			return 3;
		}
		//----------------------------------------------------------------
		// 2005.01.06
		//----------------------------------------------------------------
/*
		t = 999999; // 2003.10.10
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2003.10.10
			if ( _i_SCH_SYSTEM_USING_GET(i) >= 10 ) {
				if ( Side_Check_Target_Cluster_Count[i] > 0 ) { // 2003.10.13
					if ( t > Side_Check_Target_Cluster_Count[i] ) {
						t = Side_Check_Target_Cluster_Count[i];
					}
				}
			}
		}
		if ( t == 999999 ) t = 1;
		//----------------------------------------------------------------
		t = Side_Check_Target_Cluster_Count[side] - t; // 2003.10.10
		//----------------------------------------------------------------
		Side_Check_Current_Run_Count[side]++; // 2003.10.10
		if ( Side_Check_Current_Run_Count[side] > t ) { // 2003.10.10
			Side_Check_Current_Run_Count[side] = 0; // 2003.10.10
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = TRUE;
				else if ( i != side ) Side_Check_Last_Supplied_Flag[i] = FALSE;
			}
		}
*/
//		Side_Check_Current_Run_Count[side]++; // 2007.03.15
		//
		//
		// 2016.05.03
		//
		if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 4 ) { // Balance
			TrgCount = Side_Check_Target_Cluster_Count[side];
		}
		else {
			TrgCount = 1;
		}
		//
		Side_Check_Current_Run_Count[side] += wfrcnt; // 2007.03.15
		//
//		if ( Side_Check_Current_Run_Count[side] >= Side_Check_Target_Cluster_Count[side] ) { // 2016.05.03
		//
		if ( Side_Check_Current_Run_Count[side] >= TrgCount ) { // 2016.05.03
			//
			Side_Check_Current_Run_Count[side] = 0;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1;
				else if ( i != side ) {
//					Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26
					if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26
				}
			}
		}
		else {
			return 4;
		}
		//----------------------------------------------------------------
//_i_SCH_LOG_LOT_PRINTF( side , "Side_Monitor_SupplyDone [%d]\n" , wfrcnt0 );
		//----------------------------------------------------------------
		//----------------------------------------------------------------
//	} // 2016.05.03
//	else { // 2016.05.03
//		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2016.05.03
//			if      ( i == side ) Side_Check_Last_Supplied_Flag[i] = 1; // 2016.05.03
//			else if ( i != side ) { // 2016.05.03
////				Side_Check_Last_Supplied_Flag[i] = FALSE; // 2014.06.26 // 2016.05.03
//				if ( Side_Check_Last_Supplied_Flag[i] == 1 ) Side_Check_Last_Supplied_Flag[i] = 0; // 2014.06.26 // 2016.05.03
//			} // 2016.05.03
//		} // 2016.05.03
//	} // 2016.05.03


		return 0;
}

extern int EVENT_FLAG_LOG;

void _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( int side , int wfrcnt0 ) { // 2016.06.09
	int i , Res;
	char Buf[64];
	char Buffer[512];

	//----------------------------------------------------------------
	//
	Res = _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone_Sub( side , wfrcnt0 );
	//
	if ( ( EVENT_FLAG_LOG == 1 ) || ( EVENT_FLAG_LOG == 3 ) ) {
		//
		strcpy( Buffer , "" );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) == 0 ) {
				strcpy( Buf , "{X}" );
			}
			else {
				sprintf( Buf , "{%d,%d,%d,%d}" , Side_Check_Flag[i] , Side_Check_Last_Supplied_Flag[i] , Side_Check_Target_Cluster_Count[i] , Side_Check_Current_Run_Count[i] );
			}
			strcat( Buffer , Buf );
		}
		//
		if ( ( wfrcnt0 / 1000 ) > 0 ) {
			_i_SCH_LOG_LOT_PRINTF( side , "SupplyDone SET [COUNT=%d][RES=%d][%s][LOG=%d]%cSUPPLYDONE\n" , wfrcnt0 % 1000 , Res , Buffer , wfrcnt0 / 1000 , 9 );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( side , "SupplyDone SET [COUNT=%d][RES=%d][%s]%cSUPPLYDONE\n" , wfrcnt0 , Res , Buffer , 9 );
		}
		//
	}
	//
}


void _SCH_CASSETTE_Set_Count_Action( int s , BOOL always ) { // 2018.10.10
	int i , j , k , m;
	int ch_buffer[ MAX_PM_CHAMBER_DEPTH ];
	BOOL use1ch; // 2018.01.18
	//
	use1ch = FALSE; // 2018.01.18
	//
	j = 0;
	//
//	if ( _i_SCH_SYSTEM_USING_RUNNING( s ) ) { // 2018.10.10
	if ( always || _i_SCH_SYSTEM_USING_RUNNING( s ) ) { // 2018.10.10
		//-----------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
		//-----------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) <= 0 ) continue;
			if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) >= SCHEDULER_BM_END ) {
				if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_RETURN_REQUEST ) continue;
			}
			//
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) {
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//==========================================================
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
					//==========================================================
					if ( m > 0 ) {
						if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) {
							//
							use1ch = TRUE; // 2018.01.18
							//
							if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) {
								ch_buffer[ m - PM1 ] = TRUE;
							}
							//
						}
					}
					//==========================================================
				}
			}
		}
		//=============================================================================
		j = 0;
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) if ( ch_buffer[ i ] ) j++;
		//=============================================================================
	}
	//====================================================================================================================================
//	SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , j ); // 2016.03.09
	//
	// 2016.03.09
	//
	if ( j <= 0 ) {
//		Side_Check_Target_Cluster_Count[s] = 0; // 2018.01.18
		Side_Check_Target_Cluster_Count[s] = use1ch ? 1 : 0; // 2018.01.18
	}
	else {
		Side_Check_Target_Cluster_Count[s] = j;
	}
	//
	//====================================================================================================================================
}