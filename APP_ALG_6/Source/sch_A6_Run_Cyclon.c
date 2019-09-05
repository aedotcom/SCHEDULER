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
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A6_default.h"
#include "sch_A6_subBM.h"
#include "sch_A6_equip.h"
#include "sch_A6_sub.h"
#include "sch_A6_init.h"
//================================================================================
extern BOOL MODULE_LAST_SWITCH_6[MAX_CHAMBER]; // 2013.07.25
//================================================================================
int Scheduler_BM_Get_First_for_CYCLON( int tms ); // 2010.04.26
int Scheduler_PM_Get_First_for_CYCLON( int tms , BOOL );

int SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( int pmc , int index , int mode );

int SCHEDULING_CHECK_STOCK_Processing( int tms , int bmc , BOOL inmode , int );

//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================
int SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( int tms , int bmc , int pmc ) { // 2014.12.24
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 1 ) return FALSE;
	//
	if ( ( bmc < 0 ) && ( pmc < 0 ) ) return TRUE;
	//
	if ( bmc <= 0 ) bmc = Scheduler_BM_Get_First_for_CYCLON( tms );
	if ( pmc <= 0 ) pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( SIZE_OF_CYCLON_PM_DEPTH( pmc ) == _SCH_PRM_GET_MODULE_SIZE( bmc ) ) return TRUE;
	//
	return FALSE;
	//
}
//


void SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PICKING( int t , int pmc , int s , int p ) { // 2013.11.26
	int i , j , a , l;
	int bmsl[MAX_CASSETTE_SLOT_SIZE] , bmc;
	int bmsp[MAX_CASSETTE_SLOT_SIZE];
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return;
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return;
	if ( pmc <= 0 ) return;
	//
	bmc = 0;
	a = 0;
	//
	for ( i = p ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( a >= SIZE_OF_CYCLON_PM_DEPTH(pmc) ) break;
		//
		if ( _SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue;
		//
		if ( i != p ) {
			if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) != _SCH_CLUSTER_Get_ClusterIndex( s , i ) ) break;
		}
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( pmc == _SCH_CLUSTER_Get_PathProcessChamber( s , i , 0 , j ) ) break;
		}
		if ( j == MAX_PM_CHAMBER_DEPTH ) break;
		//
		a++;
		//
//		if ( _SCH_CLUSTER_Get_PathProcessParallel( s , i , 0 ) == NULL ) continue; // 2014.02.03
		if ( _SCH_CLUSTER_Get_PathProcessDepth( s , i , 0 ) == NULL ) continue; // 2014.02.03
		//
//		l = _SCH_CLUSTER_Get_PathProcessParallel( s , i , 0 )[ pmc - PM1 ]; // 2014.02.03
//		l = _SCH_CLUSTER_Get_PathProcessDepth( s , i , 0 )[ pmc - PM1 ]; // 2014.02.03 // 2015.04.10
		l = _SCH_CLUSTER_Get_PathProcessDepth2( s , i , 0 , ( pmc - PM1 ) ); // 2014.02.03 // 2015.04.10
		//
//		if ( ( l > 100 ) && ( l <= ( 100 + MAX_CYCLON_PM_DEPTH ) ) ) { // 2014.02.03
		if ( ( l > 0 ) && ( l <= SIZE_OF_CYCLON_PM_DEPTH(pmc) ) ) { // 2014.02.03
			//
			for ( j = 0 ; j < bmc ; j++ ) {
//				if ( bmsp[j] == ( l - 100 ) ) { // 2014.02.03
				if ( bmsp[j] == l ) { // 2014.02.03
					i = MAX_CASSETTE_SLOT_SIZE;
					break;
				}
			}
			//
			if ( j == bmc ) {
				bmsl[bmc] = i;
//				bmsp[bmc] = l - 100; // 2014.02.03
				bmsp[bmc] = l; // 2014.02.03
				bmc++;
			}
			//
		}
		//
	}
	//
	if ( bmc > 1 ) {
		for ( i = 0 ; i < ( bmc - 1 ) ; i++ ) {
			for ( j = (i+1) ; j < bmc ; j++ ) {
				//
				if ( bmsp[i] <= bmsp[j] ) continue;
				//
				l = bmsp[j];
				bmsp[j] = bmsp[i];
				bmsp[i] = l;
			}
		}
		//
		for ( i = 0 ; i < bmc ; i++ ) {
			//
//			_SCH_CLUSTER_Get_PathProcessParallel( s , bmsl[i] , 0 )[ pmc - PM1 ] = bmsp[i] + 100; // 2014.02.03
//			_SCH_CLUSTER_Get_PathProcessDepth( s , bmsl[i] , 0 )[ pmc - PM1 ] = bmsp[i]; // 2014.02.03 // 2015.04.09
			_SCH_CLUSTER_Set_PathProcessDepth2( s , bmsl[i] , 0 , pmc - PM1 , bmsp[i] ); // 2014.02.03 // 2015.04.09
			//
		}
	}
	//
}

void SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( int tms , int ch ) { // 2013.11.26
	int i , j , s , p , l , pmc , z;
	int bmsl[MAX_CASSETTE_SLOT_SIZE] , bmc;
	int bmsp[MAX_CASSETTE_SLOT_SIZE];
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return;
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 1 ) return;
	//
	bmc = 0;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	z = _SCH_PRM_GET_MODULE_SIZE( ch );
	//
	for ( i = 1 ; i <= z ; i++ ) {
		if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( ch , i );
			p = _SCH_MODULE_Get_BM_POINTER( ch , i );
			//			
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) continue;
			//
//			if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , 0 ) == NULL ) continue; // 2014.02.03
			if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , 0 ) == NULL ) continue; // 2014.02.03
			//
//			l = _SCH_CLUSTER_Get_PathProcessParallel( s , p , 0 )[ pmc - PM1 ]; // 2014.02.03
//			l = _SCH_CLUSTER_Get_PathProcessDepth( s , p , 0 )[ pmc - PM1 ]; // 2014.02.03 // 2015.04.10
			l = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , 0 , ( pmc - PM1 ) ); // 2014.02.03 // 2015.04.10
			//
//			if ( ( l > 100 ) && ( l <= ( 100 + MAX_CYCLON_PM_DEPTH ) ) ) { // 2014.02.03
			if ( ( l > 0 ) && ( l <= SIZE_OF_CYCLON_PM_DEPTH(pmc) ) ) { // 2014.02.03
				bmsl[bmc] = i;
//				bmsp[bmc] = l - 100; // 2014.02.03
				bmsp[bmc] = l; // 2014.02.03
				bmc++;
			}
			//
		}
	}
	//
	for ( i = 0 ; i < ( bmc - 1 ) ; i++ ) {
		for ( j = (i+1) ; j < bmc ; j++ ) {
			//
			if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , bmsl[i] ) , _SCH_MODULE_Get_BM_POINTER( ch , bmsl[i] ) ) <= _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , bmsl[j] ) , _SCH_MODULE_Get_BM_POINTER( ch , bmsl[j] ) ) ) continue;
			//
			l = bmsl[j];
			bmsl[j] = bmsl[i];
			bmsl[i] = l;
		}
	}
	//
	for ( i = 0 ; i < ( bmc - 1 ) ; i++ ) {
		for ( j = (i+1) ; j < bmc ; j++ ) {
			//
			if ( bmsp[i] <= bmsp[j] ) continue;
			//
			l = bmsp[j];
			bmsp[j] = bmsp[i];
			bmsp[i] = l;
		}
	}
	//
	for ( i = 0 ; i < bmc ; i++ ) {
		//
		s = _SCH_MODULE_Get_BM_SIDE( ch , bmsl[i] );
		p = _SCH_MODULE_Get_BM_POINTER( ch , bmsl[i] );
		//
//		_SCH_CLUSTER_Get_PathProcessParallel( s , p , 0 )[ pmc - PM1 ] = bmsp[i] + 100; // 2014.02.03
//		_SCH_CLUSTER_Get_PathProcessDepth( s , p , 0 )[ pmc - PM1 ] = bmsp[i]; // 2014.02.03 // 2015.04.09
		_SCH_CLUSTER_Set_PathProcessDepth2( s , p , 0 , pmc - PM1 , bmsp[i] ); // 2014.02.03 // 2015.04.09
		//
	}
}



//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================
int SCHEDULING_MOVE_OPERATION( int tms , int mode , int side , int pt , int mdl , int arm , int wfr , int slot , int switching , int swmode ) {
	int pmc;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.07.25
		//
		if ( ( mdl >= BM1 ) && ( mdl < ( BM1 + 4 ) ) ) {
			//
			if ( Scheduler_BM_Get_First_for_CYCLON( tms ) != -1 ) { // 2013.09.03
				//
				pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
				//
				if ( ( pmc >= PM1 ) && ( pmc < AL ) ) {
					//
					if ( MODULE_LAST_SWITCH_6[pmc] ) {
						//
						MODULE_LAST_SWITCH_6[pmc] = FALSE;
						//
						_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Start at %s%cWHTM%dCLOSESTART PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
						//
						if ( SCHEDULING_EQ_GATE_CLOSE_STYLE_6( tms , pmc ) != SYS_SUCCESS ) {
							//
							_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Fail at %s%cWHTM%dCLOSEFAIL PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
							//
							return SYS_ABORTED;
						}
						//
						_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Success at %s%cWHTM%dCLOSESUCCESS PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
						//
					}
				}
				//
			}
			//
		}
	}
	else if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) { // 2013.11.11
		//
		if ( ( mdl >= BM1 ) && ( mdl < ( BM1 + 4 ) ) ) {
			//
			for ( pmc = PM1 ; pmc < AL ; pmc++ ) {
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( pmc ) == tms ) {
					//
					if ( _SCH_PRM_GET_MODULE_MODE( pmc ) ) {
						//
						if ( MODULE_LAST_SWITCH_6[pmc] ) {
							//
							MODULE_LAST_SWITCH_6[pmc] = FALSE;
							//
							_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Start at %s%cWHTM%dCLOSESTART PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
							//
							if ( SCHEDULING_EQ_GATE_CLOSE_STYLE_6( tms , pmc ) != SYS_SUCCESS ) {
								//
								_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Fail at %s%cWHTM%dCLOSEFAIL PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
								//
								return SYS_ABORTED;
							}
							//
							_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Close Success at %s%cWHTM%dCLOSESUCCESS PM%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 , pmc - PM1 + 1 , 9 , wfr );
							//
						}
						//
					}
					//
				}
				//
			}
			//
		}
	}
	//-----------------------------------------------------------------------
	/*
	//  2013.09.03
	if ( ( mode % 10 ) == MACRO_PLACE ) { // 2013.08.30
		if ( Scheduler_BM_Get_First_for_CYCLON( tms ) == -1 ) { // 2013.09.02
			MODULE_LAST_SWITCH_6[mdl] = FALSE;
		}
		else {
			MODULE_LAST_SWITCH_6[mdl] = switching; // 2013.07.25
		}
	}
	else {
		MODULE_LAST_SWITCH_6[mdl] = switching; // 2013.07.25
	}
	*/
	//
	MODULE_LAST_SWITCH_6[mdl] = switching; // 2013.09.03
	//
	//-----------------------------------------------------------------------
	//
	switch( tms ) {
	case 0 :	return _SCH_MACRO_TM_OPERATION_0( mode , side , pt , mdl , arm , wfr , slot , switching , -1 , FALSE , swmode , -1 , -1 );	break;
	case 1 :	return _SCH_MACRO_TM_OPERATION_1( mode , side , pt , mdl , arm , wfr , slot , switching , -1 , FALSE , swmode , -1 , -1 );	break;
	case 2 :	return _SCH_MACRO_TM_OPERATION_2( mode , side , pt , mdl , arm , wfr , slot , switching , -1 , FALSE , swmode , -1 , -1 );	break;
	case 3 :	return _SCH_MACRO_TM_OPERATION_3( mode , side , pt , mdl , arm , wfr , slot , switching , -1 , FALSE , swmode , -1 , -1 );	break;
	}
	return SYS_ABORTED;
	//
}

//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( int tms ) {
	int pmc;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( pmc , TRUE , -1 ) ) return FALSE;
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ) return FALSE;
	//
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_PM_WAFER_NONE( int tms ) {
	int pmc;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) > 0 ) return FALSE;
	//
	return TRUE;
}

BOOL Stock_Have_Return( int bm ) { // 2013.04.11
	int i , s, p;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( bm , i );
			p = _SCH_MODULE_Get_BM_POINTER( bm , i );
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) == BUFFER_OUTWAIT_STATUS ) {
				if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) return TRUE;
			}
		}
	}
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( int tms , BOOL pmdischeck , BOOL wfrcheck , int chkbm ) {
	int StockBm;
	//
	StockBm = Scheduler_BM_Get_First_for_CYCLON( tms );
	//
	if ( StockBm < BM1 ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , -1 ) <= 0 ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_OUTWAIT_STATUS ) <= 0 ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
	//
	if ( chkbm >= BM1 ) {
		if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( chkbm ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( chkbm , BUFFER_INWAIT_STATUS ) <= 0 ) {
				return FALSE;
			}
		}
	}
	//
	if ( pmdischeck ) {
		//
		if ( !SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) return FALSE;
		//
	}
	//
	if ( wfrcheck ) {
		if ( !SCHEDULER_CONTROL_Chk_PM_WAFER_NONE( tms ) ) return FALSE;
	}
	//
	return TRUE;
}



BOOL SCHEDULER_CONTROL_Chk_ST6_PM_CURR_SIDE( int pmc , int side ) {
	int i;
	for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
				if ( _SCH_MODULE_Get_PM_SIDE( pmc , i ) == side ) {
					return TRUE;
				}
				else {
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}

//
// 2014.09.23
//
BOOL SCHEDULER_CONTROL_Chk_ST6_PM_GET_SIDE_PT( int pmc , int *side , int *pt , int *wsa , BOOL *firstnotuse , int *stock ) {
	int i;
	//
	*firstnotuse = FALSE;
	//
	for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
		if ( ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) && ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) ) { // temp
			//
			*side = _SCH_MODULE_Get_PM_SIDE( pmc , i );
			*pt   = _SCH_MODULE_Get_PM_POINTER( pmc , i );
			*wsa  = _SCH_MODULE_Get_PM_WAFER( pmc , i );
			//
			*stock = _SCH_CLUSTER_Get_Stock( *side , *pt );
			//
			return TRUE;
		}
		else {
			if ( i == 0 ) {
				*firstnotuse = TRUE;
			}
		}
	}
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( int pmc , int index , int mode ) {
	int i , c;
	if      ( mode == -2 ) { // free // 2014.12.24
		c = 0;
		if ( index <= 0 ) {
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 100 ) { // temp
						c++;
					}
				}
				else {
					c++;
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , index - 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) == 100 ) { // temp
					c++;
				}
			}
			else {
				c++;
			}
		}
		return c;
	}
	else if ( mode == -1 ) { // all
		c = 0;
		if ( index == -1 ) {
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
						c++;
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , index - 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) != 100 ) { // temp
					c++;
				}
			}
		}
		return c;
	}
	else if ( mode == 0 ) { // Not Process
		c = 0;
		if ( index == -1 ) {
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
						if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 0 ) c++;
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , index - 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) != 100 ) { // temp
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) == 0 ) c++;
				}
			}
		}
		return c;
	}
	else { // processed
		c = 0;
		if ( index == -1 ) {
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
						if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 0 ) c++;
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , index - 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) != 100 ) { // temp
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , index - 1 ) != 0 ) c++;
				}
			}
		}
		return c;
	}
	return 0;
}

void SCHEDULER_CONTROL_Chk_ST6_PM_RUN_MODE( int pmc ) {
	int i;
	for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
			_SCH_MODULE_Set_PM_STATUS( pmc , i , 1 );
		}
	}
}


int SCHEDULER_CONTROL_ST6_MOVE_ACTION( int side , int pmc , int pmindex , int option ) {
	int Res;
	//
	Res = SCH_CYCLON2_Set_PM_MOVE_ACTION( pmc , pmindex ); // 2013.01.25
	//
	if ( Res == -1 ) {
		_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Fail(D) to %d%cWHPM%dMOVEFAIL PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 , pmc - PM1 + 1 , pmindex , option , 9 );
		return SYS_ABORTED;
	}
	if ( Res == 1 ) {
		return SYS_SUCCESS;
	}
	//
	_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Start to %d%cWHPM%dMOVESTART PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 , pmc - PM1 + 1 , pmindex , option , 9 );
	Res = SCHEDULING_EQ_PM_MOVE_CONTROL_FOR_STYLE_6( pmc , pmindex , 0 , 0 , 0 , 0 , 0 , 0 , 1 );
	//
	return Res;
}

int SCHEDULER_CONTROL_ST6_MOVE_STATUS( int side , int pmc , int pmindex , BOOL wait , int option ) {
	int Res;

	Res = SCH_CYCLON2_Get_PM_RESULT_INFO( pmc ); // 2013.01.25
	//
	if ( Res == SYS_RUNNING ) {
		//
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
			//
			if ( pmindex == -1 ) {
				pmindex = SCH_CYCLON2_Get_PM_SLOT_INFO( pmc );
			}
			else {
				if ( pmindex != SCH_CYCLON2_Get_PM_SLOT_INFO( pmc ) ) {
					_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Fail(S) to %d%cWHPM%dMOVEFAIL PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 , pmc - PM1 + 1 , pmindex , option , 9 );
					return SYS_ABORTED;
				}
			}
			//
		}
		//
		while( TRUE ) {
			//
			Res = SCHEDULING_EQ_PM_MOVE_CONTROL_FOR_STYLE_6( pmc , pmindex , 0 , 0 , 0 , 0 , 0 , 0 , 2 );
			//
			switch( Res ) {
			case SYS_SUCCESS :
				//
				SCH_CYCLON2_Set_PM_MOVE_RESULT( pmc , SYS_SUCCESS ); // 2013.01.25
				//
				_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Success to %d%cWHPM%dMOVESUCCESS PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 , pmc - PM1 + 1 , pmindex , option , 9 );
				//
				return Res;
				break;
			case SYS_ABORTED :
			case SYS_ERROR :
				//
				SCH_CYCLON2_Set_PM_MOVE_RESULT( pmc , SYS_ABORTED ); // 2013.01.25
				//
				_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Fail to %d%cWHPM%dMOVEFAIL PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 , pmc - PM1 + 1 , pmindex , option , 9 );
				return Res;
				break;
			}
			//
			if ( !wait ) return Res;
			//
			Sleep(1);
			//
		}
	}

	return Res;
}

int SCHEDULER_CONTROL_ST6_MOVE_WAITING( int side , int pmc , int *pmcall , int option ) { // 2008.01.23
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) return SYS_SUCCESS; // 2013.01.25
	//
	if ( *pmcall == -1 ) return SYS_SUCCESS;
	//
	while( TRUE ) {
		switch( SCHEDULING_EQ_PM_MOVE_CONTROL_FOR_STYLE_6( *pmcall / 100 , *pmcall % 100 , 0 , 0 , 0 , 0 , 0 , 0 , 2 ) ) {
		case SYS_SUCCESS :
			//
			SCH_CYCLON2_Set_PM_MOVE_RESULT( *pmcall / 100 , SYS_SUCCESS ); // 2013.01.25
			//
			_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Success to %d%cWHPM%dMOVESUCCESS PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( *pmcall / 100 ) , *pmcall % 100 , 9 , ( *pmcall / 100 ) - PM1 + 1 , ( *pmcall / 100 ) - PM1 + 1 , *pmcall % 100 , option , 9 );
			*pmcall = -1;
			return SYS_SUCCESS;
			break;
		case SYS_ABORTED :
		case SYS_ERROR :
			//
			SCH_CYCLON2_Set_PM_MOVE_RESULT( *pmcall / 100 , SYS_ABORTED ); // 2013.01.25
			//
			_SCH_LOG_LOT_PRINTF( side , "%s Handling Move Fail to %d%cWHPM%dMOVEFAIL PM%d:%d:%d%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( *pmcall / 100 ) , *pmcall % 100 , 9 , ( *pmcall / 100 ) - PM1 + 1 , ( *pmcall / 100 ) - PM1 + 1 , *pmcall % 100 , option , 9 );
			*pmcall = -1;
			return SYS_ABORTED;
			break;
		}
		Sleep(1);
	}
	//
	*pmcall = -1;
	//
	return SYS_SUCCESS;
}



int SCHEDULER_CONTROL_ST6_GET_SLOT( int pmc , int side , int pt ) {
	int l;

//	if ( _SCH_CLUSTER_Get_PathProcessParallel( side , pt , 0 ) == NULL ) return 0; // 2014.02.03
	if ( _SCH_CLUSTER_Get_PathProcessDepth( side , pt , 0 ) == NULL ) return 0; // 2014.02.03

//	l = strlen( _SCH_CLUSTER_Get_PathProcessParallel( side , pt , 0 ) ); // 2008.09.16
//	if ( l > ( pmc - PM1 ) ) { // 2008.09.16
//		l = _SCH_CLUSTER_Get_PathProcessParallel( side , pt , 0 )[ pmc - PM1 ]; // 2014.02.03
//		l = _SCH_CLUSTER_Get_PathProcessDepth( side , pt , 0 )[ pmc - PM1 ]; // 2014.02.03 // 2015.04.10
		l = _SCH_CLUSTER_Get_PathProcessDepth2( side , pt , 0 , ( pmc - PM1 ) ); // 2014.02.03 // 2015.04.10
//		if ( ( l > 100 ) && ( l <= ( 100 + MAX_CYCLON_PM_DEPTH ) ) ) { // 2014.02.03
		if ( ( l > 0 ) && ( l <= SIZE_OF_CYCLON_PM_DEPTH(pmc) ) ) { // 2014.02.03
//			return l - 100; // 2014.02.03
			return l; // 2014.02.03
		}
//	} // 2008.09.16
	return 0;
}

BOOL SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( int pmc ) { // 2008.01.23
	int i , res , sts , x , allres;
	//
	allres = FALSE; // 2010.10.20
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.02.15
		//
		for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) { // 2010.09.13
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 100 ) continue; // temp // 2013.06.13
				if ( _SCH_CLUSTER_Get_PathIn( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) break;
				if ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) != 3 ) break;
			}
		}
		if ( i == SIZE_OF_CYCLON_PM_DEPTH(pmc) ) return TRUE; // 2010.09.13
	}
	//
	for ( x = 0 ; x < MAX_CASSETTE_SIDE ; x++ ) { // 2010.10.20
		//
		res = FALSE;
		//
		for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
				//
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 100 ) continue; // temp // 2013.06.13
				//
				if ( _SCH_MODULE_Get_PM_SIDE( pmc , i ) != x ) continue; // 2010.10.20
				//
				if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) == PATHDO_WAFER_RETURN ) {
					sts = _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) );
					res = TRUE;
					break;
				}
			}
		}
		if ( res ) {
			//
			allres = TRUE; // 2010.10.20
			//
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.02.15
				for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
					if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
						//
						if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 100 ) continue; // temp // 2013.06.13
						//
						if ( _SCH_MODULE_Get_PM_SIDE( pmc , i ) != x ) continue; // 2010.10.20
						//
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) != PATHDO_WAFER_RETURN ) {
							_SCH_CLUSTER_Set_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , PATHDO_WAFER_RETURN );
							_SCH_CLUSTER_Set_PathStatus( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , sts );
						}
					}
				}
			}
			else {
				for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
					if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
						//
						if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 100 ) continue; // temp // 2013.06.13
						//
						if ( _SCH_MODULE_Get_PM_SIDE( pmc , i ) != x ) continue; // 2010.10.20
						//
						if ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) >= 4 ) continue;
						//
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) != PATHDO_WAFER_RETURN ) {
							_SCH_CLUSTER_Set_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , PATHDO_WAFER_RETURN );
							_SCH_CLUSTER_Set_PathStatus( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , sts );
						}
					}
				}
			}
			//
		}
	}
	//
//	return res; // 2010.10.20
	return allres; // 2010.10.20
}

int SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( int rside , int tms , int mode , int bmc , int pmc , int side , int pt ) {
	int i , l;
	int pms[MAX_CASSETTE_SLOT_SIZE];
	//===================================================================================================
	// mode
	// 0 : side / pt
	// 1 : pick pm
	// 2 : pick pm : Disable
	// e : free
	//===================================================================================================
	if ( mode == 0 ) {
		l = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , side , pt );
		if ( l != 0 ) return l;
	}
	//===================================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) pms[i] = 0;
	//===================================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.25
		//
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
			if ( ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmc , i ) ) { // 2010.09.09
				//
				if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( bmc , i ) , _SCH_MODULE_Get_BM_POINTER( bmc , i ) ) != PATHDO_WAFER_RETURN ) { // 2008.01.23
					l = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , _SCH_MODULE_Get_BM_SIDE( bmc , i ) , _SCH_MODULE_Get_BM_POINTER( bmc , i ) );
					pms[l] = 1;
				}
			}
		}
		//
	}
	else {
		//
		SCH_CYCLON2_GROUP_GET_PM_SLOT_INFO( pmc , pms );
		//
	}
	//===================================================================================================
	if ( SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( tms , -1 , -1 ) ) { // 2014.12.24
		//
		if ( mode == 0 ) {
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) <= 0 ) {
					if ( pms[ i + 1 ] != 1 ) {
						return i + 1;
					}
				}
			}
		}
		//
	}
	//
	for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
			if ( mode == 2 ) { // 2008.01.16
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp // 2013.06.13
					if ( pms[ i + 1 ] != 1 ) {
						return i + 1;
					}
				}
			}
			else {
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
					if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 0 ) { // processed
						if ( pms[ i + 1 ] != 1 ) {
							return i + 1;
						}
					}
					else { // 2008.01.23
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) == PATHDO_WAFER_RETURN ) { // 2008.01.23
							if ( pms[ i + 1 ] != 1 ) {
								return i + 1;
							}
						}
					}
				}
			}
		}
	}
	//
	//===================================================================================================
	if ( mode != 2 ) { // 2008.01.16
		for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) <= 0 ) {
				if ( pms[ i + 1 ] != 1 ) {
					return i + 1;
				}
			}
		}
	}
	//===================================================================================================
	return -1;
}


BOOL CYCLON_TM_ACT_LOCKED( int side , int tmside ); // 2012.09.03


//
//
// 2018.10.02
//
//
BOOL SCH_BM_FORCE_FMSIDE( int side , int tms , int pmc ) {
	int bmch , i;
	int s , p , ss , es;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) return FALSE;
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 2 ) == 0 ) return FALSE;
	//
	bmch = tms + BM1;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmch , FALSE , -1 ) ) return FALSE;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION ) return FALSE;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch ) != 0 ) return FALSE;
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_INWAIT_STATUS ) return FALSE;
			//
			s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return FALSE;
			//
		}
	}
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
			//
			_SCH_MODULE_Set_BM_STATUS( bmch , i , BUFFER_OUTWAIT_STATUS );
			if ( _SCH_CLUSTER_Get_PathIn( s , p ) != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
				SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
			}
			else {
				_SCH_CLUSTER_Set_PathDo( s , p , 1 );
			}
		}
	}
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmch , -1 , TRUE , tms + 1 , 4004 );
	//
	return TRUE;
}
//

int SCHEDULER_MODULE_RUN_ENABLE( int run_side , int TMSide , int pmc ) { // 2008.10.01
	int i;
	//
	_SCH_SUB_Chk_GLOBAL_STOP( run_side ); // 2012.03.30
	//
	while( TRUE ) {
		//
		if ( !CYCLON_TM_ACT_LOCKED( run_side , TMSide ) ) { // 2012.09.03
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ) return 1;
		}
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( pmc , TRUE , -1 ) ) {
			//
			//===================================================================================================
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.02.15
				//
				for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
					if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
						if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) != 100 ) { // temp
							if ( _SCH_MODULE_Get_PM_STATUS( pmc , i ) == 0 ) { // Not processed
								if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) != PATHDO_WAFER_RETURN ) {
									if ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) < 4 ) {
										_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , -1 );
										SCH_CYCLON2_GROUP_SUPPLY_STOP( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) );
										_SCH_MODULE_Set_PM_STATUS( pmc , i , 1 );
									}
								}
							}
						}
					}
				}
				//
			}
			//===================================================================================================
			//
			return 0;
		}
		else { // 2018.10.02
			//
			if ( SCH_BM_FORCE_FMSIDE( run_side , TMSide , pmc ) ) return 1;
			//
		}
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return 0;
		//
		_SCH_SYSTEM_LEAVE_TM_CRITICAL( TMSide );
		Sleep(1); \
		_SCH_SYSTEM_ENTER_TM_CRITICAL( TMSide );
		//
		if ( _SCH_MODULE_Chk_FM_Finish_Status( run_side ) ) { // 2011.04.22
			if ( _SCH_MODULE_Chk_FM_Free_Status( run_side ) ) {
				return 2;
			}
		}
		//
	}
	return 1;
}


int SCHEDULER_CONTROL_ST6_GET_TM_ARM( int pmc , BOOL loading ) {
	int tms;
	//
	// 2014.12.24
	//
	tms = _SCH_PRM_GET_MODULE_GROUP( pmc );
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
		//
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) return TB_STATION;
		//
		return TA_STATION;
		//
	}
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( pmc ) ) {
	case 0 :
		switch( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( pmc ) ) {
		case 0 :
		case 1 :
			if ( loading )	return TA_STATION;
			else			return TB_STATION;
			break;
		case 2 :
			if ( loading )	return TB_STATION;
			else			return TA_STATION;
			break;
		}
		break;
	case 1 :
		if ( loading )	return TB_STATION;
		else			return TA_STATION;
		break;
	case 2 :
		if ( loading )	return TA_STATION;
		else			return TB_STATION;
		break;
	}
	if ( loading )	return TA_STATION;
	else			return TB_STATION;
}


BOOL SCHEDULER_CONTROL_NEW_SUPPLY_POSSIBLE( int side , int bmc , int pmc ) { // 2008.07.28
	int i , ret_rcm , ret_pt;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i == side ) continue;
		//
		if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , 0 ) ) {
			if ( !_SCH_SUB_FM_Current_No_Way_Supply( i , ret_pt , pmc ) ) {
				//
				if ( _SCH_CLUSTER_Get_PathIn( i , ret_pt ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2012.02.16
					if ( _SCH_IO_GET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( i , ret_pt ) , _SCH_CLUSTER_Get_SlotIn( i , ret_pt ) ) <= 0 ) {
						continue;
					}
				}
				//
				return TRUE;
			}
		}
		//
	}
	return FALSE;
}




int SCHEDULER_CONTROL_BM_WAFER_HAS_RETURN_WAFER( int bmc ) { // 2010.04.26
	int i;
	if ( bmc == -1 ) return 0;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) return i;
	}
	return 0;
}




int SCHEDULER_CONTROL_NEXT_PROCESSING( int run_side , int tms , int bmcbuf , int orgbmc , int pmc , int side , int pt , int firstnotuse ) { // 2010.04.26
	int bmc; // 2010.06.25
	int l , i , j , range , pathdo , Function_Result , Arm , hasnext , renasts;
	//
	int wfr_wafer[MAX_PM_SLOT_DEPTH];
	int wfr_side[MAX_PM_SLOT_DEPTH];
	int wfr_point[MAX_PM_SLOT_DEPTH];

	range  = _SCH_CLUSTER_Get_PathRange( side , pt );
	pathdo = _SCH_CLUSTER_Get_PathDo( side , pt );
	if ( range <= pathdo ) return 1;
	//
//	if ( bmc == -1 ) return TRUE; // 2010.06.25
	if ( bmcbuf == -1 ) {
		//
		bmc = orgbmc;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return 0;
		//
	}
	else {
		//
		bmc = bmcbuf;
		//
	}
	//
	for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
		wfr_side[i]  = _SCH_MODULE_Get_PM_SIDE( pmc , i );
		wfr_point[i] = _SCH_MODULE_Get_PM_POINTER( pmc , i );
		wfr_wafer[i] = _SCH_MODULE_Get_PM_WAFER( pmc , i );
	}
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) < SIZE_OF_CYCLON_PM_DEPTH(pmc) ) { // singlemode
		for ( l = 1 ; l < range ; l++ ) {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				Arm = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , l , j );
				if ( Arm == pmc ) break;
			}
			//
			if ( j == MAX_PM_CHAMBER_DEPTH ) {
				for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
					if ( wfr_wafer[j] > 0 ) _SCH_CLUSTER_Inc_PathDo( wfr_side[j] , wfr_point[j] );
				}
				continue;
			}
			//
			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
			if ( renasts == 2 ) return 2;
			if ( renasts == 0 ) return 1;
			//
			// Wait Process Finished
			//
			while( TRUE ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == 0 ) {
					break;
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) < 0 ) {
					//
					_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted(101) Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 );
					//
					return 0;
				}
				//
				Sleep(1);
				//
			}
			//
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				//
				renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
				if ( renasts == 2 ) return 2;
				if ( renasts == 0 ) return 1;
				//
				if ( wfr_wafer[i] <= 0 ) continue;
				//
				Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );
				//
				Function_Result = SCHEDULER_CONTROL_ST6_MOVE_ACTION( wfr_side[i] , pmc , i + 1 , 11 );
				//
				if ( Function_Result != SYS_SUCCESS ) return 0;
				//
				Function_Result = SCHEDULER_CONTROL_ST6_MOVE_STATUS( wfr_side[i] , pmc , i + 1 , TRUE , 12 );
				if ( Function_Result != SYS_SUCCESS ) return 0;
				//
				while( TRUE ) { // 2011.04.04
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) return 1;
					//
					// pick from pm
					//
					Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 10 , wfr_side[i] , wfr_point[i] , pmc , Arm , wfr_wafer[i] , i + 1 , TRUE , 0 );
					//
					if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					break; // 2011.04.04
					//
				}
				//
				while( TRUE ) { // 2011.04.04
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) return 1;
					//
					// place to bm
					//
					Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 10 , wfr_side[i] , wfr_point[i] , bmc , Arm , wfr_wafer[i] , 1 , TRUE , 0 );
					//
					if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					break; // 2011.04.04
					//
				}
				//
				//
				// bm TMSIDE
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , run_side , bmc , wfr_wafer[i] , TRUE , tms + 1 , 4010 );
				//
				while( TRUE ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) == 0 ) {
						break;
					}
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) < 0 ) {
						return 0;
					}
					//
					Sleep(1);
					//
				}
				//
				renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
				if ( renasts == 2 ) return 2;
				if ( renasts == 0 ) {
					//
					_SCH_MODULE_Set_PM_WAFER( pmc , i , 0 );
					//
					_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , i+1 , wfr_side[i] , wfr_point[i] );
					_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , i+1 , wfr_wafer[i] , BUFFER_INWAIT_STATUS );
					//
					return 1;
				}
				//
				hasnext = 0;
				for ( j = i + 1 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
					if ( wfr_wafer[j] > 0 ) {
						hasnext = j;
						break;
					}
				}
				//
				Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );
				//
				while( TRUE ) { // 2011.04.04
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) return 1;
					//
					// pick from bm
					//
					Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 110 , wfr_side[i] , wfr_point[i] , bmc , Arm , wfr_wafer[i] , 1 , hasnext > 0 , 0 );
					//
					if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					_SCH_CLUSTER_Inc_PathDo( wfr_side[i] , wfr_point[i] );
					//
					break; // 2011.04.04
				}
				//
				while( TRUE ) { // 2011.04.04
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) return 1;
					//
					// place to pm
					//
					Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 110 , wfr_side[i] , wfr_point[i] , pmc , Arm , wfr_wafer[i] , i + 1 , hasnext > 0 , 0 );
					//
					if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					if ( ( _SCH_CLUSTER_Get_SwitchInOut( wfr_side[i] , wfr_point[i] ) % 2 ) == 0 ) // 2013.08.01
						_SCH_CLUSTER_Set_SwitchInOut( wfr_side[i] , wfr_point[i] , 2 );
					else
						_SCH_CLUSTER_Set_SwitchInOut( wfr_side[i] , wfr_point[i] , 3 );
					//
					break; // 2011.04.04
				}
			}
			//
			if ( SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc ) ) return 1;
			if ( _SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , pmc , Arm , firstnotuse , 619 ) < 0 ) return 0;
			//
		}
	}
	else {
		for ( l = 1 ; l < range ; l++ ) {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				Arm = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , l , j );
				if ( Arm == pmc ) break;
			}
			//
			if ( j == MAX_PM_CHAMBER_DEPTH ) {
				for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
					if ( wfr_wafer[j] > 0 ) _SCH_CLUSTER_Inc_PathDo( wfr_side[j] , wfr_point[j] );
				}
				continue;
			}
			//
			//
			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
			if ( renasts == 2 ) return 2;
			if ( renasts == 0 ) return 1;
			//
			//
			// Wait Process Finished
			//
			while( TRUE ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == 0 ) {
					break;
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) < 0 ) {
					//
					_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted(102) Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 );
					//
					return 0;
				}
				//
				Sleep(1);
				//
			}
			//
			Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );
			//
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				//
				if ( wfr_wafer[i] > 0 ) {
					//
					Function_Result = SCHEDULER_CONTROL_ST6_MOVE_ACTION( wfr_side[i] , pmc , i + 1 , 13 );
					//
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					break;
				}
				//
			}
			//
			for ( ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				//
				// pick from pm
				// place to bm
				//
				if ( wfr_wafer[i] > 0 ) {
					//
					hasnext = 0;
					//
					for ( j = i + 1 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
						if ( wfr_wafer[j] > 0 ) {
							hasnext = j;
							break;
						}
					}
					//
					Function_Result = SCHEDULER_CONTROL_ST6_MOVE_STATUS( wfr_side[i] , pmc , i + 1 , TRUE , 14 );
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) {
						//
						for ( j = 0 ; j < i ; j++ ) {
							//
							if ( wfr_wafer[j] > 0 ) {
								//
								_SCH_MODULE_Set_PM_WAFER( pmc , j , 0 );
								//
								_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , j+1 , wfr_side[j] , wfr_point[j] );
								_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , j+1 , wfr_wafer[j] , BUFFER_INWAIT_STATUS );
								//
							}
							//
						}
						return 1;
					}
					//
					while( TRUE ) { // 2011.04.04
						//
						renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
						if ( renasts == 2 ) return 2;
						if ( renasts == 0 ) return 1;
						//
						// pick
						//
						Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 210 , wfr_side[i] , wfr_point[i] , pmc , Arm , wfr_wafer[i] , i + 1 , hasnext > 0 , 0 );
						//
						if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
						break; // 2011.04.04
					}
					//
					if ( hasnext > 0 ) {
						//
						Function_Result = SCHEDULER_CONTROL_ST6_MOVE_ACTION( wfr_side[hasnext] , pmc , hasnext + 1 , 15 );
						//
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
					}
					//
					while( TRUE ) { // 2011.04.04
						//
						renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
						if ( renasts == 2 ) return 2;
						if ( renasts == 0 ) return 1;
						//
						// place
						//
						Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 210 , wfr_side[i] , wfr_point[i] , bmc , Arm , wfr_wafer[i] , i + 1 , hasnext > 0 , 0 );
						//
						if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
						break; // 2011.04.04
					}
					//
				}
				//
			}
			//
			// bm TMSIDE
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , run_side , bmc , -1 , TRUE , tms + 1 , 4010 );
			//
			while( TRUE ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) == 0 ) {
					break;
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) < 0 ) {
					return 0;
				}
				//
				Sleep(1);
				//
			}
			//
			// pick from bm
			// place to pm
			//
			//
			Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );
			//
			for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				//
				if ( wfr_wafer[i] > 0 ) {
					//
					Function_Result = SCHEDULER_CONTROL_ST6_MOVE_ACTION( wfr_side[i] , pmc , i + 1 , 16 );
					//
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					break;
				}
				//
			}
			//
			for ( ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) {
				//
				// pick from bm
				// place to pm
				//
				if ( wfr_wafer[i] > 0 ) {
					//
					hasnext = 0;
					//
					for ( j = i + 1 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
						if ( wfr_wafer[j] > 0 ) {
							hasnext = j;
							break;
						}
					}
					//
					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
					if ( renasts == 2 ) return 2;
					if ( renasts == 0 ) {
						//
						for ( j = i ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
							//
							if ( wfr_wafer[j] > 0 ) {
								//
								_SCH_MODULE_Set_PM_WAFER( pmc , j , 0 );
								//
								_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , j+1 , wfr_side[j] , wfr_point[j] );
								_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , j+1 , wfr_wafer[j] , BUFFER_INWAIT_STATUS );
								//
							}
							//
						}
						return 1;
					}
					//
					while( TRUE ) { // 2011.04.04
						//
						renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
						if ( renasts == 2 ) return 2;
						if ( renasts == 0 ) return 1;
						//
						// pick
						//
						Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 310 , wfr_side[i] , wfr_point[i] , bmc , Arm , wfr_wafer[i] , i + 1 , hasnext > 0 , 0 );
						//
						if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
						_SCH_CLUSTER_Inc_PathDo( wfr_side[i] , wfr_point[i] );
						//
						break; // 2011.04.04
					}
					//
					Function_Result = SCHEDULER_CONTROL_ST6_MOVE_STATUS( wfr_side[i] , pmc , i + 1 , TRUE , 17 );
					if ( Function_Result != SYS_SUCCESS ) return 0;
					//
					while( TRUE ) { // 2011.04.04
						//
						renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
						if ( renasts == 2 ) return 2;
						if ( renasts == 0 ) return 1;
						//
						// place
						//
						Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 310 , wfr_side[i] , wfr_point[i] , pmc , Arm , wfr_wafer[i] , i + 1 , hasnext > 0 , 0 );
						//
						if ( Function_Result == SYS_ERROR ) continue; // 2011.04.04
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
						if ( ( _SCH_CLUSTER_Get_SwitchInOut( wfr_side[i] , wfr_point[i] ) % 2 ) == 0 ) // 2013.08.01
							_SCH_CLUSTER_Set_SwitchInOut( wfr_side[i] , wfr_point[i] , 2 );
						else
							_SCH_CLUSTER_Set_SwitchInOut( wfr_side[i] , wfr_point[i] , 3 );
						//
						break; // 2011.04.04
					}
					//
					if ( hasnext > 0 ) {
						//
						Function_Result = SCHEDULER_CONTROL_ST6_MOVE_ACTION( wfr_side[hasnext] , pmc , hasnext + 1 , 18 );
						//
						if ( Function_Result != SYS_SUCCESS ) return 0;
						//
					}
					//
				}
				//
			}
			//
			// Process
			//
			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc ); // 2011.04.22
			if ( renasts == 2 ) return 2;
			if ( renasts == 0 ) return 1;
			//
			if ( SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc ) ) return 1;
			if ( _SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , pmc , Arm , firstnotuse , 619 ) < 0 ) return 0;
			//
		}
	}
	//
	return 1;
}


BOOL PMC_HAS_NEXT_BM_WAIT( int pmc , int bmc , int side , int pt ) {
	if ( pmc <= 0 ) return FALSE;
	if ( bmc > 0 ) return FALSE;
	if ( _SCH_CLUSTER_Get_PathRange( side , pt ) <= _SCH_CLUSTER_Get_PathDo( side , pt ) ) return FALSE;
	return TRUE;
}

//

int BM_WAIT_CHECK_6[MAX_CHAMBER]; // 2018.10.02
int PM_WAIT_LAST_STS_6[MAX_CHAMBER]; // 2018.10.02
int PM_WAIT_FUNCTION_ADDRESS[MAX_CHAMBER]; // 2018.10.02


BOOL SCHEDULER_CONTROL_Chk_PM_WAIT_FAILED( int ch ) {
	switch ( PM_WAIT_LAST_STS_6[ch] ) {
	case -1 : // All Stop
		return FALSE;
		break;
	case -2 : // Cluster - Continue , Process - Continue
		break;
	case -3 : // Cluster - Continue , Process - Disable
		break;
	case -4 : // Cluster - Out , Process - Continue
		break;
	case -5 : // Cluster - Out , Process - Disable
		break;
	default :
		return FALSE;
		break;
	}
	//
	return TRUE;
}


void SCHEDULER_CONTROL_Chk_BM_REMAP_FOR_RUN( int ch ) {
	int i , s , p;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_OUTWAIT_STATUS ) continue;
			//
			_SCH_MODULE_Set_BM_STATUS( ch , i , BUFFER_INWAIT_STATUS );
			//
			s = _SCH_MODULE_Get_BM_SIDE( ch , i );
			p = _SCH_MODULE_Get_BM_POINTER( ch , i );
			//
			_SCH_CLUSTER_Set_PathDo( s , p , 0 );
			_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_SUPPLY );
			//
			if ( _SCH_CLUSTER_Get_PathIn( s , p ) >= BM1 ) {
				_SCH_CLUSTER_Set_SwitchInOut( s , p , 1 );
			}
			else {
				_SCH_CLUSTER_Set_SwitchInOut( s , p , 0 );
			}
			//
		}
	}
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_FMWAIT_STYLE_6( int tms , int ch ) { // 2018.10.02
	//
	int pmc;
	char Buffer[64];
	//
	if ( BM_WAIT_CHECK_6[ch] == 1 ) return TRUE;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( pmc <= 0 ) return FALSE;
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -2 ) {
		//
		sprintf( Buffer , "SCHEDULER_USER_PM%d_AL6" , pmc - PM1 + 1 );
		//
		PM_WAIT_FUNCTION_ADDRESS[pmc] = _FIND_FROM_STRING( _K_F_IO + 1 , Buffer );
		//
	}
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -1 ) return FALSE;
	//
	return TRUE;
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_FORCE_FMSIDE_STYLE_6( int tms , int ch , BOOL *Abort ) { // 2018.10.02
	//
	int pmc;
	char Buffer[64];
	//
	*Abort = FALSE;
	//
	if ( BM_WAIT_CHECK_6[ch] == -1 ) {
		*Abort = TRUE;
		return FALSE;
	}
	//
	if ( BM_WAIT_CHECK_6[ch] != 1 ) return FALSE;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( pmc <= 0 ) return FALSE;
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -2 ) {
		//
		sprintf( Buffer , "SCHEDULER_USER_PM%d_AL6" , pmc - PM1 + 1 );
		//
		PM_WAIT_FUNCTION_ADDRESS[pmc] = _FIND_FROM_STRING( _K_F_IO + 1 , Buffer );
		//
	}
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -1 ) return FALSE;
	//
	switch( _dREAD_FUNCTION( PM_WAIT_FUNCTION_ADDRESS[pmc] ) ) {
	case SYS_ERROR : // 재공급
		//
		_SCH_MACRO_RESET_PROCESSING_INFO( pmc );	// 2018.10.31
		//
		SCHEDULER_CONTROL_Chk_BM_REMAP_FOR_RUN( ch );
		BM_WAIT_CHECK_6[ch] = 0;
		return FALSE;
		break;
	case SYS_ABORTED : // 중지
		BM_WAIT_CHECK_6[ch] = -1;
		*Abort = TRUE;
		return FALSE;
		break;
	case SYS_SUCCESS : // 회수
		BM_WAIT_CHECK_6[ch] = 0;
		return TRUE;
		break;
	default : // Wait
		return FALSE;
		break;
	}
	//
	return FALSE;
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( int tms , int ch , BOOL *Abort ) { // 2018.10.02
	//
	int pmc , i , s , p;
	BOOL f;
	char Buffer[64];
	//
	if ( Abort != NULL ) *Abort = FALSE;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( pmc <= 0 ) return FALSE;
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -2 ) {
		//
		sprintf( Buffer , "SCHEDULER_USER_PM%d_AL6" , pmc - PM1 + 1 );
		//
		PM_WAIT_FUNCTION_ADDRESS[pmc] = _FIND_FROM_STRING( _K_F_IO + 1 , Buffer );
		//
	}
	//
//	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -1 ) return FALSE; // 2018.11.15
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] != -1 ) {
		//
		if ( BM_WAIT_CHECK_6[ch] == -1 ) {
			if ( Abort != NULL ) *Abort = TRUE;
			return TRUE;
		}
		//
		if ( BM_WAIT_CHECK_6[ch] == 1 ) {
			//
			switch( _dREAD_FUNCTION( PM_WAIT_FUNCTION_ADDRESS[pmc] ) ) {
			case SYS_ERROR : // 재공급
				//
				_SCH_MACRO_RESET_PROCESSING_INFO( pmc );	// 2018.10.31
				//
				SCHEDULER_CONTROL_Chk_BM_REMAP_FOR_RUN( ch );
				BM_WAIT_CHECK_6[ch] = 0;
				return TRUE;
				break;
			case SYS_ABORTED : // 중지
				BM_WAIT_CHECK_6[ch] = -1;
				if ( Abort != NULL ) *Abort = TRUE;
				return TRUE;
				break;
			case SYS_SUCCESS : // 회수
				BM_WAIT_CHECK_6[ch] = 0;
				return FALSE;
				break;
			default : // Wait
				return TRUE;
				break;
			}
		}
		//
	}
	//
	if ( !SCHEDULER_CONTROL_Chk_PM_WAIT_FAILED( pmc ) ) return FALSE;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) != BUFFER_TM_STATION ) return FALSE;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) != 0 ) return FALSE;
	//
	f = FALSE;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			//
			f = TRUE;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
			//
			s = _SCH_MODULE_Get_BM_SIDE( ch , i );
			p = _SCH_MODULE_Get_BM_POINTER( ch , i );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return FALSE;
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 0 ) return FALSE;
			//
			_SCH_MULTIJOB_SET_CJPJJOB_RESULT( s , p , 0 , 2 ); // 2018.11.15
			//
		}
	}
	//
	if ( !f ) return FALSE;
	//
	if ( PM_WAIT_FUNCTION_ADDRESS[pmc] == -1 ) return FALSE; // 2018.11.15
	//
	if ( _dREAD_FUNCTION( PM_WAIT_FUNCTION_ADDRESS[pmc] ) != SYS_RUNNING ) {
		//
		sprintf( Buffer , "WAITING BM%d" , ch - BM1 + 1 );
		//
		_dRUN_SET_FUNCTION( PM_WAIT_FUNCTION_ADDRESS[pmc] , Buffer );
		//
	}
	//
	BM_WAIT_CHECK_6[ch] = 1;
	//
	return TRUE;
	//
}

BOOL SCHEDULING_CHECK_CYCLON_LOCKING_MAKE_FREE2( int side , int tmside , int RealStock ) { // 2012.04.08
	int i , bm;
	BOOL Abort;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) return TRUE; // 2018.10.02
	//
	// 2018.10.02
	//
	if ( SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_FORCE_FMSIDE_STYLE_6( tmside , tmside + BM1 , &Abort ) ) {
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , tmside + BM1 , -1 , TRUE , tmside + 1 , 1288 );
		//
		return !Abort;
	}
	//
	if ( Abort ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		if ( i == 0 ) {
			bm = tmside + BM1;
		}
		else {
			bm = tmside + BM1 + 2;
		}
		//
//		if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bm , FALSE , TRUE , TRUE ) ) continue; // 2013.04.12 // 2013.05.02
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_TM_STATION ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) != 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm , BUFFER_INWAIT_STATUS ) > 0 ) continue;
		//
		if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( bm ) ) {
			if ( RealStock <= 0 ) {
				continue;
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RealStock , BUFFER_OUTWAIT_STATUS ) > 0 ) continue;
			}
			//
			if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bm , FALSE , TRUE , TRUE ) ) continue; // 2013.04.12 // 2013.05.02
			//
		}
		//
		if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tmside , bm , &Abort ) ) { // 2018.10.02
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bm , -1 , TRUE , tmside + 1 , 1284 );
			//
		}
		//
		if ( Abort ) return FALSE;
		//
	}
	//
	return TRUE;
}

int SCHEDULING_CHECK_CYCLON_LOCKING_MAKE_FREE( int tmside , int side , int bmc , int pmc ) { // 2011.03.03
	//
	if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( bmc ) ) {
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bmc , BUFFER_INWAIT_STATUS ) <= 0 ) {
			//
			if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tmside , bmc , NULL ) ) { // 2018.10.02
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , tmside + 1 , 1233 );
				//
			}
			//
			return 1;
		}
	}
	//
	if ( pmc == -1 ) return 0;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( tmside , 0 ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tmside , 1 ) > 0 ) ) return 0;
	if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) > 0 ) return 0;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmc , BUFFER_INWAIT_STATUS ) > 0 ) return 0;
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmc , BUFFER_OUTWAIT_STATUS ) <= 0 ) return 0;
	//
	if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tmside , bmc , NULL ) ) { // 2018.10.02
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , tmside + 1 , 1234 );
	}
	//
	return 1;
	//
}


BOOL CYCLON_TM_ACT_LOCK[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];


void CYCLON_TM_ACT_LOCK_INIT( int side ) { // 2012.09.03
	int i;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		CYCLON_TM_ACT_LOCK[side][i] = FALSE;
	}
	//
}
//
BOOL CYCLON_TM_ACT_LOCKED( int side , int tmside ) { // 2012.09.03
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( CYCLON_TM_ACT_LOCK[i][tmside] ) return TRUE;
	}
	//
	return FALSE;
}

void CYCLON_TM_SET_LOCKING( int side , int tmside , BOOL data ) { // 2012.09.03
	CYCLON_TM_ACT_LOCK[side][tmside] = data;
}


int Sch_Check_TM_BM_PreCheck( int run_side , int tms , int RunBm , int *runpmc , int fmskip , int StockBm , int RealStock , int StockReturn ) {
	//
	BOOL Abort;
	int i , Result = 0 , pmc , pmc2 , slot , morerun;
	//
	if ( !SCHEDULING_CHECK_CYCLON_LOCKING_MAKE_FREE2( run_side , tms , RealStock ) ) return -1; // 2013.04.08
	//
	if ( fmskip ) return 0; // 2013.01.29
	//
	pmc = *runpmc;
	//
	morerun = FALSE;
	//
	if ( StockReturn ) morerun = TRUE; // 2013.03.29
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) == BUFFER_TM_STATION ) {

		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) == 0 ) {

			Result = SCHEDULING_CHECK_CYCLON_LOCKING_MAKE_FREE( tms , run_side , RunBm , pmc );
			//
			if ( Result == -1 ) {
				//
				_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 9 ....%cWHTMFAIL\n" ,  tms + 1 , 9 );
				//
				return -1;
			}
			if ( Result == 1 ) return 1;

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );

			switch ( SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( RunBm , TRUE ) ) {
			case -1 :
				_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( RunBm ) , 9 , tms + 1 );
				return -1;
				break;
			case 0 :
				//
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103-31 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result ); // 2018.10.02
				if ( pmc != -1 ) {

					pmc2 = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
					//
					if ( pmc2 != -1 ) {
						//
						i = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc2 );
						//
						if ( i == 2 ) return 1;
						//
					}
					//
					if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &slot , TRUE ) <= 0 ) {

						if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

							// [START] BM Venting
							if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02

								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4002 );

							}
							//
							if ( Abort ) return -1;
							//
							// [END] BM Venting
						}
						//
//						Sleep(1); // 2013.01.29
						//
						_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103-2 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );
						//
						return 1;
					}
				}
				//
			case 1 :
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103-3 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );
				if ( pmc == -1 ) {

					pmc2 = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );

					if ( pmc2 != -1 ) {

						i = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc2 );
						//
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103-32 [pmc2=%d][RunBm=%d][i=%d]\n" , tms + 1 , pmc2 , RunBm , i ); // 2018.10.02
						//
						if ( i == 2 ) return 1;
					}

					for ( i = PM1 ; i < AL ; i++ ) {
						if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
							if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( i , -1 , -1 ) > 0 ) {
								break;
							}
						}
					}
					//
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 103-33 [pmc2=%d][RunBm=%d][i=%d]\n" , tms + 1 , pmc2 , RunBm , i ); // 2018.10.02
					//
					if ( i == AL ) {
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) <= 0 ) ) {
							//
							if ( ( pmc2 != -1 ) && ( _SCH_MODULE_GET_USE_ENABLE( pmc2 , FALSE , -1 ) ) ) { // 2019.05.29 ynsong
								//
								if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

									if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , RunBm ) ) { // 2013.03.29

										// [START] BM Venting
										if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4001 );
										}
										//
										if ( Abort ) return -1;
										//
										// [END] BM Venting
									}

								}
							}

//							Sleep(1); // 2013.01.29

							_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 104 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );

							if ( morerun ) return 0; // 2013.03.29

							return 1;
						}
					}
					else {
						if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &slot , TRUE ) <= 0 ) {
							//
							if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) { /* 2012.03.08 */
								// [START] BM Venting
//								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4002 ); // 2018.10.02
								if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4003 ); // 2018.10.02
								}
								//
								if ( Abort ) return -1;
								//
								// [END] BM Venting
							}

//							Sleep(1); // 2013.01.29

							_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 105 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );

							if ( morerun ) return 0; // 2013.03.29

							return 1;
						}
						//=========================================================================================================================
						pmc = i;
						*runpmc = i;
						//=========================================================================================================================
					}
				}
				break;
			}
		}
		else {
//			Sleep(1); // 2013.01.29

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 107 [pmc=%d][RunBm=%d][Result=%d][morerun=%d]\n" , tms + 1 , pmc , RunBm , Result , morerun );

			if ( morerun ) return 0;
			//
			return 1;
		}
	}
	else {
//		Sleep(1); // 2013.01.29

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 108 [pmc=%d][RunBm=%d][Result=%d][morerun=%d]\n" , tms + 1 , pmc , RunBm , Result , morerun );
		//
		if ( morerun ) return 0;
		//
		return 1;
	}

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 110 [pmc=%d][RunBm=%d][Result=%d]\n" , tms + 1 , pmc , RunBm , Result );

	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) {
		_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( RunBm ) , 9 , tms + 1 );
		return -1;
	}
	//
	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 111 [pmc=%d][RunBm=%d][Result=%d][morerun=%d]\n" , tms + 1 , pmc , RunBm , Result , morerun );
	//
	if ( morerun ) return 0;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_TM_STATION ) {
//		Sleep(1); // 2013.01.29
		return 1;
	}

	return 0;
}


int Sch_Check_TM_PLACE_BM_Unload_Run( int run_side , BOOL pre , int tms , int RunBm , int pmc , int StockBm , int RealBM , int RealStock , int *prcsing , int *MoveRun ) {
	BOOL Abort;
	int Arm , Result = 0 , renasts , CHECKING_SIDE = 0 , BM_Slot , pt , wsa , wsb , Sav_Int1 , SCH_Return , Function_Result;
	//
	//
	if ( pre ) {
		//
		if ( pmc == -1 ) return 0;
		//
		Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );
		//
	}
	else {
		if ( pmc != -1 ) {
			//
			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			//
			if ( renasts == 2 ) return 1;

			if ( renasts == 1 ) {
				//
				Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );

			}
			else {

				*prcsing = TRUE;

				if ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) > 0 ) {
					Arm = 0;
				}
				else if ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) > 0 ) {
					Arm = 1;
				}
				else {
					Arm = 0;
				}
			}
		}
		else {

			*prcsing = TRUE;

			if ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) > 0 ) {
				Arm = 0;
			}
			else if ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) > 0 ) {
				Arm = 1;
			}
			else {
				Arm = 0;
			}
		}
	}

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 212 [%d][pre=%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pre , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );

	if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) {

		CHECKING_SIDE = _SCH_MODULE_Get_TM_SIDE( tms , Arm );

		Result = 1;

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 213 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );

		if ( pmc != -1 ) {
			//
			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			//
			if ( renasts == 2 ) return 1;
		}
		else {
			if ( CHECKING_SIDE != run_side ) Result = 0;
		}

		if ( Result == 1 ) {
			//
			if ( pre ) {
				if ( ( renasts != 0 ) && ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( tms , Arm ) ) != PATHDO_WAFER_RETURN ) ) {
					return 0;
				}
			}
			//
			if ( SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( CHECKING_SIDE , tms , RunBm , &BM_Slot , &SCH_Return , TRUE ) == 1 ) {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 214 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );

				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) > 0 ) {

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 215 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );

					if ( pre ) {

//						Sleep(1); // 2013.01.29

						return 1;
					}

					if ( pmc == -1 ) {
						//
						if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 109 ) == SYS_ABORTED ) return -1;

//						Sleep(1); // 2013.01.29

						return 1;
					}

					renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );

					if ( renasts == 2 ) return 1;

					if ( renasts == 0 ) {
						if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 110 ) == SYS_ABORTED ) return -1;
						//
//						Sleep(1); // 2013.01.29
						//
						return 1;
					}
					//
					Result = 1;
				}
				else {
					//

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 216 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );

					if ( pre ) {
						Result = 0;
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
							if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 ) {
								Result = 0;
							}
							else {
								Result = 1;
							}
						}
						else { // 2013.01.25
							if ( StockBm != -1 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
									Result = 0;
								}
								else {
									Result = 1;
								}
							}
							else {
								if ( !SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( _SCH_MODULE_Get_TM_SIDE( tms , Arm ) , _SCH_MODULE_Get_TM_POINTER( tms , Arm ) , pmc ) ) {
									Result = 0;
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 ) {
										Result = 0;
									}
									else {
										Result = 1;
									}
								}
							}
						}
					}
				}
				//
				if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_TM_STATION ) return 0; // 2013.03.31
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) return -1; // 2013.03.31
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) != 0 ) return 0; // 2013.03.31
				//
				pt = _SCH_MODULE_Get_TM_POINTER( tms , Arm );
				wsa = _SCH_MODULE_Get_TM_WAFER( tms , Arm );
				wsb = BM_Slot;
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END );
				//
				Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 510 , CHECKING_SIDE , pt , RunBm , Arm , wsa , wsb , Result , 0 );
				//
				if ( Function_Result == SYS_ABORTED ) {
					return -1;
				}
				else if ( Function_Result == SYS_ERROR ) {
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
				}
				if ( Function_Result == SYS_SUCCESS ) {
					_SCH_MODULE_Set_BM_SIDE_POINTER( RunBm , BM_Slot , CHECKING_SIDE , pt );
					_SCH_MODULE_Set_BM_WAFER_STATUS( RunBm , BM_Slot , _SCH_MODULE_Get_TM_WAFER( tms , Arm ) , BUFFER_OUTWAIT_STATUS );
					_SCH_MODULE_Set_TM_WAFER( tms , Arm , 0 );
					_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE );

					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.02.15
						if ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) < 4 ) {
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
							SCH_CYCLON2_GROUP_SUPPLY_STOP( CHECKING_SIDE , pt );
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 216-2 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );
						}
					}
					else {
						if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 0 ) || ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 2 ) ) {
							if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
								_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 216-3 [%d][pmc=%d][RunBm=%d][Result=%d][Arm=%d][pr=%d][mr=%d]\n" , tms + 1, CHECKING_SIDE , pmc , RunBm , Result , Arm , *prcsing , *MoveRun );
							}
						}
					}
				}
				//
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.03.31
					//
					if ( RunBm == RealBM ) {
						//
						if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
							//
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( RunBm ) ) {
								//
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
									// [START] BM Venting
									if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4091 );
									}
									//
									if ( Abort ) return -1;
									//
									// [END] BM Venting
								}
							}
							else {
								if ( Scheduler_BM_Get_First_for_CYCLON( tms ) >= BM1 ) {
									if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
										if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , -1 ) ) {
											// [START] BM Venting
											if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4092 );
											}
											//
											if ( Abort ) return -1;
											//
											// [END] BM Venting
										}
									}
									else {
										if ( !SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_BM( CHECKING_SIDE , pt , Scheduler_BM_Get_First_for_CYCLON( tms ) ) ) {
											// [START] BM Venting
											if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4093 );
											}
											//
											if ( Abort ) return -1;
											//
											// [END] BM Venting
										}
									}
								}
								else {
									if ( !SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( CHECKING_SIDE , pt , pmc ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
											// [START] BM Venting
											if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 4094 );
											}
											//
											if ( Abort ) return -1;
											//
											// [END] BM Venting
										}
									}
								}
							}
						}
					}
					else if ( RunBm == RealStock ) {
						//
						SCHEDULING_CHECK_STOCK_Processing( tms , RunBm , FALSE , pmc ); // 2013.06.26
						//
					}
				}
				//
			}

			/* [END] PLACE BM	Arm (Unoading) BM_Slot */
		}
	}
	return 0;
}

int SCHEDULING_CHECK_BM_Stock_Supply_Check_for_CYCLON( int tms , int side , int pt , BOOL freeonly , int pmc , int StockBm , int *StockSlot , int *StockMode ) {
	int i;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE; // 2013.01.25
	//
	if ( StockBm == -1 ) return FALSE;
	//
	*StockSlot = 0;
	*StockMode = -1;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( StockBm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( StockBm , i ) > 0 ) {
			//
			if ( freeonly ) continue;
			//
			if ( pmc >= PM1 ) { // 2013.04.01
				if ( SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( _SCH_MODULE_Get_BM_SIDE( StockBm , i ) , _SCH_MODULE_Get_BM_POINTER( StockBm , i ) , pmc ) ) {
					//
					*StockMode = -1;
					//
					return TRUE;
				}
			}
			//
			if ( _SCH_MODULE_Get_BM_STATUS( StockBm , i ) == BUFFER_INWAIT_STATUS ) {
				//
				if ( side >= 0 ) {
					if ( _SCH_CLUSTER_Get_Stock( _SCH_MODULE_Get_BM_SIDE( StockBm , i ) , _SCH_MODULE_Get_BM_POINTER( StockBm , i ) ) != _SCH_CLUSTER_Get_Stock( side , pt ) ) {
						//
						*StockMode = -1;
						//
						break;
					}
				}
				//
			}
			else if ( _SCH_MODULE_Get_BM_STATUS( StockBm , i ) == BUFFER_OUTWAIT_STATUS ) {
				//
				if ( !SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) { // 2013.05.02
					//
					if ( *StockSlot == 0 ) {
						//
						*StockSlot = i;
						//
						if ( side >= 0 ) {
							*StockMode = 0; // swap
						}
						else {
							*StockMode = 2; // out
						}
						//
					}
				}
				//
			}
			//
		}
		else {
			if ( freeonly ) {
				//
				*StockSlot = i;
				*StockMode = 1; // in
				//
				return TRUE;
			}
			else {
				if ( side >= 0 ) {
					if ( *StockSlot == 0 ) {
						*StockSlot = i;
						*StockMode = 1; // in
					}
				}
			}
		}
	}
	//
	if ( freeonly ) return FALSE;
	//
	return TRUE;
}


BOOL SCHEDULING_PICK_ACTION( int tms , int arm , int side , int pt , int bm , int slot , int Switch , int log ) {
	int Function_Result;
	//
	Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + log , side , pt , bm , arm , _SCH_CLUSTER_Get_SlotIn( side , pt ) , slot , Switch , 0 );
	//
	if ( Function_Result == SYS_ABORTED ) return FALSE;
	//
	_SCH_MACRO_TM_DATABASE_OPERATION( tms , MACRO_PICK , bm , arm , slot , 0 , side , pt , 0 , 0 , 0 , 0 );
	//
	return TRUE;
}

BOOL SCHEDULING_PLACE_ACTION( int tms , int arm , int side , int pt , int bm , int slot , int sts , int Switch , int log ) {
	int Function_Result;
	//
	Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + log , side , pt , bm , arm , _SCH_CLUSTER_Get_SlotIn( side , pt ) , slot , Switch , 0 );
	//
	if ( Function_Result == SYS_ABORTED ) return FALSE;
	//
	_SCH_MACRO_TM_DATABASE_OPERATION( tms , MACRO_PLACE , bm , arm , slot , 0 , side , pt , 0 , 0 , sts , sts );
	//
	if ( ( bm >= PM1 ) && ( bm < AL ) ) { // 2013.08.01
		if ( ( _SCH_CLUSTER_Get_SwitchInOut( side , pt ) % 2 ) == 0 )
			_SCH_CLUSTER_Set_SwitchInOut( side , pt , 2 );
		else
			_SCH_CLUSTER_Set_SwitchInOut( side , pt , 3 );
	}
	//
	return TRUE;
}

int SW_CHECK_REMAIN( int bm , int slot , int sts ) {
	int i;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
		//
		if ( i == slot ) continue;
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) > 0 ) {
			if ( sts == -1 ) {
				return 1;
			}
			if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) == sts ) {
				return 1;
			}
		}
	}
	return 0;
}

int SW_CHECK_ALL_NOT_STS( int bm , int slot , int sts ) {
	int i;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
		//
		if ( i == slot ) continue;
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) != sts ) {
				return 1;
			}
		}
		else {
			return 1;
		}
	}
	return 0;
}

int SCHEDULING_CHECK_STOCK_Not_Prepared( int bmc ) { // 2013.06.26
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return 0;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) < 0 ) return -1;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) != 0 ) return 1;
	return 0;
}

int SCHEDULING_CHECK_STOCK_Processing( int tms , int bmc , BOOL inmode , int pmc ) { // 2013.06.26
	int i , s , p , l , c;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return 1;
	//
	c = 0;
	s = -1;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i >= 1 ; i-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
		//
		if ( inmode ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) != BUFFER_INWAIT_STATUS ) return 2;
		}
		else {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) != BUFFER_OUTWAIT_STATUS ) return 3;
		}
		//
		if ( s == -1 ) {
			s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
			l = _SCH_CLUSTER_Get_Stock( s , p ) / 100;
		}
		else {
			s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
			if ( l != ( _SCH_CLUSTER_Get_Stock( s , p ) / 100 ) ) return 4;
		}
		//
		c++;
		//
	}
	//
	if ( s == -1 ) return 5;
	//
	if ( inmode ) {
		if ( SCH_CYCLON2_GROUP_COUNT( tms , l ) != c ) return 6;
	}
	else {
		if ( SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( s , p , pmc ) ) return 7;
	}
	//
	if ( inmode ) {
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "_SCH_MACRO_SPAWN_WAITING_BM_OPERATION <TM> [BM%d]\n" , bmc - BM1 + 1 );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );	Sleep(3000);

		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , s , bmc , -1 , FALSE , tms + 1 , 7231 );
	}
	else {
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "_SCH_MACRO_SPAWN_WAITING_BM_OPERATION <FM> [BM%d]\n" , bmc - BM1 + 1 );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );
//printf( "==============================================================\n" );	Sleep(3000);

		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s , bmc , -1 , FALSE , tms + 1 , 7232 );
	}
	//
	return 0;
}


BOOL SCHEDULING_CHECK_BM_Stock_Supply_Action_for_CYCLON( int tms , int side , int pt , int pmc , int RunBm , int BMSlot , int StockBm , int StockSlot , int StockMode , int StockReturn , int log ) {
	BOOL Abort;
	int Arm , Arm2;
	int s , p , sw , c;
	//
	if ( pmc == -1 ) return TRUE;
	//
	switch ( SCHEDULING_CHECK_STOCK_Not_Prepared( StockBm ) ) { // 2013.06.26
	case -1 :
		return FALSE;
		break;
	case 1 :
		return TRUE;
		break;
	}
	//
	c = 0;
	//
	if      ( StockMode == 0 ) { // swap
		//
		Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );
		//
		if ( Arm == TA_STATION ) Arm2 = TB_STATION;	else Arm2 = TA_STATION;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) return TRUE;
		if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm2 ) > 0 ) return TRUE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( StockBm , StockSlot );
		p = _SCH_MODULE_Get_BM_POINTER( StockBm , StockSlot );
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		if ( !SCHEDULING_PICK_ACTION( tms , Arm , side , pt , RunBm , BMSlot , 1 , 1100 + log ) ) return FALSE;

		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		if ( !SCHEDULING_PICK_ACTION( tms , Arm2 , s , p , StockBm , StockSlot , 1 , 1200 + log ) ) return FALSE;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//
		/*
		// 2013.05.02
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
			sw = 0;
		}
		else {
			sw = SW_CHECK_ALL_NOT_STS( StockBm , StockSlot , BUFFER_INWAIT_STATUS );
		}
		*/
		// 2013.05.02
		sw = SW_CHECK_ALL_NOT_STS( StockBm , StockSlot , BUFFER_INWAIT_STATUS );
		//
		if ( !SCHEDULING_PLACE_ACTION( tms , Arm , side , pt , StockBm , StockSlot , BUFFER_INWAIT_STATUS , sw , 1300 + log ) ) return FALSE;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		//
		SCHEDULING_CHECK_STOCK_Processing( tms , StockBm , TRUE , 0 ); // 2013.06.26
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
			sw = 0;
		}
		else {
			sw = SW_CHECK_ALL_NOT_STS( RunBm , BMSlot , BUFFER_OUTWAIT_STATUS );
		}
		//
		if ( !SCHEDULING_PLACE_ACTION( tms , Arm2 , s , p , RunBm , BMSlot , BUFFER_OUTWAIT_STATUS , sw , 1400 + log ) ) return FALSE;
		//
		c = RunBm;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		//
	}
	else if ( StockMode == 1 ) { // in
		//
		Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) return TRUE;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//
		sw = SW_CHECK_REMAIN( RunBm , BMSlot , BUFFER_INWAIT_STATUS );
		//
		if ( !SCHEDULING_PICK_ACTION( tms , Arm , side , pt , RunBm , BMSlot , sw , 2100 + log ) ) return FALSE;

		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//
		sw = SW_CHECK_ALL_NOT_STS( StockBm , StockSlot , BUFFER_INWAIT_STATUS );
		//
		if ( !SCHEDULING_PLACE_ACTION( tms , Arm , side , pt , StockBm , StockSlot , BUFFER_INWAIT_STATUS , sw , 2200 + log ) ) return FALSE;

		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//
		SCHEDULING_CHECK_STOCK_Processing( tms , StockBm , TRUE , 0 ); // 2013.06.26
		//
	}
	else if ( StockMode == 2 ) { // out
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_TM_STATION ) return TRUE; // 2013.03.31
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) return FALSE; // 2013.03.31
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) != 0 ) return TRUE; // 2013.03.31
		//
		Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) return TRUE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( StockBm , StockSlot );
		p = _SCH_MODULE_Get_BM_POINTER( StockBm , StockSlot );
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		//
		sw = SW_CHECK_REMAIN( StockBm , StockSlot , -1 );
		//
		if ( !SCHEDULING_PICK_ACTION( tms , Arm , s , p , StockBm , StockSlot , sw , 3100 + log ) ) return FALSE;

		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
			sw = 0;
		}
		else {
			sw = SW_CHECK_ALL_NOT_STS( RunBm , BMSlot , BUFFER_OUTWAIT_STATUS );
		}
		//
		if ( !SCHEDULING_PLACE_ACTION( tms , Arm , s , p , RunBm , BMSlot , BUFFER_OUTWAIT_STATUS , sw , 3200 + log ) ) return FALSE;
		//
		c = RunBm;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( s ) ) || ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) ) return FALSE;
		//
	}
	//
	if ( c >= BM1 ) { // 2013.04.01
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.03.31
			if ( ( StockBm >= BM1 ) && ( RunBm >= BM1 ) ) {
				//
				if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
					//
					if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( RunBm ) ) {
						//
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
							// [START] BM Venting
							if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , RunBm , -1 , TRUE , tms + 1 , 4191 );
							}
							//
							if ( Abort ) return -1;
							//
							// [END] BM Venting
						}
					}
					else {
						if ( Scheduler_BM_Get_First_for_CYCLON( tms ) >= BM1 ) {
							if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
								if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , -1 ) ) {
									// [START] BM Venting
									if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , RunBm , -1 , TRUE , tms + 1 , 4192 );
									}
									//
									if ( Abort ) return -1;
									//
									// [END] BM Venting
								}
							}
							else {
								if ( !SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_BM( s , p , StockBm ) ) {
									// [START] BM Venting
									if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , RunBm , -1 , TRUE , tms + 1 , 4193 );
									}
									//
									if ( Abort ) return -1;
									//
									// [END] BM Venting
								}
							}
						}
					}
				}
			}
			//
		}
		//

	}
	//
	return TRUE;
}


int Sch_Check_TM_PICK_BM_Load_Run( int run_side , int tms , int RunBm , int pmc , int StockBm , int fmskip , int StockReturn , int *MoveRun ) {
	BOOL Abort;
	int pt , side , BM_Slot , Result , Arm , Sav_Int1 , Sav_Int2 , wsa , wsb , swmode , Function_Result , StockSlot , StockMode;
	int pmidx; // 2014.12.24
	//
//	pt = -1; // 2013.01.25
	//
	if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt , &BM_Slot , &side ) > 0 ) {

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 142 [pmc=%d][RunBm=%d][mr=%d][%d,%d,%d]\n" , tms + 1, pmc , RunBm , *MoveRun , side , pt , BM_Slot );

		if ( pmc == -1 ) {
			//
			if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 101 ) == SYS_ABORTED ) return -1;
			//
//			Sleep(1); // 2013.01.29
			//
			return 1;
		}
		//
		if ( SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( tms , -1 , -1 ) ) { // 2014.12.24
			//
			pmidx = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 0 , RunBm , pmc , side , pt );
			//
			if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , pmidx , -2 ) <= 0 ) return 0;
			//
		}
		//
		if ( SCHEDULING_CHECK_BM_Stock_Supply_Check_for_CYCLON( tms , side , pt , FALSE , pmc , StockBm , &StockSlot , &StockMode ) ) { // Stock Swap

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 143 [pmc=%d][RunBm=%d][mr=%d][%d,%d,%d]\n" , tms + 1, pmc , RunBm , *MoveRun , side , pt , BM_Slot );
		
			if ( !SCHEDULING_CHECK_BM_Stock_Supply_Action_for_CYCLON( tms , side , pt , pmc , RunBm , BM_Slot , StockBm , StockSlot , StockMode , StockReturn , 10 ) ) {

				return -1;

			}
		
		}
		else { // NoStock/defstock
			//
			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 144 [pmc=%d][RunBm=%d][mr=%d][%d,%d,%d]\n" , tms + 1, pmc , RunBm , *MoveRun , side , pt , BM_Slot );
			//
			Result = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			
			if ( Result == 2 ) return 1;

			if ( ( Result == 0 ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot ) , _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot ) ) == PATHDO_WAFER_RETURN ) ) {
				//
				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 102 ) == SYS_ABORTED ) return -1;
				//
//				Sleep(1); // 2013.01.29
				//
				return 1;
			}

			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) > 0 ) {

				return 1;

			}
			else {
				//
//				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) < 0 ) { // 2013.04.08
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) { // 2013.04.08
					//
					_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted(103) Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , tms + 1 );
					//
					return -1; // 2013.04.03
				}
				//

				// [START] PICK BM Arm (Loading) slot

				side = _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot );
				pt   = _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot );
				//
//				if ( SCH_CYCLON2_GROUP_WAIT_ANOTHER_WAFER_IN_PM( tms , side , pt , pmc , MAX_CYCLON_PM_DEPTH ) ) { // 2013.05.28 // 2014.02.03
//					return 1; // 2014.02.03
//				} // 2014.02.03
				//
				//
				if ( SCH_CYCLON2_GROUP_WAIT_ANOTHER_WAFER_IN_PM( tms , side , pt , pmc , SIZE_OF_CYCLON_PM_DEPTH(pmc) ) ) { // 2013.05.28 // 2014.02.03
					//
//					SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , RunBm ); // 2014.02.03 // 2014.07.10
					SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( RunBm ); // 2014.02.03 // 2014.07.10
					//
				}
				else {
					//
//					SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , 0 ); // 2014.02.03 // 2014.07.10
					SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( RunBm ); // 2014.02.03 // 2014.07.10
					//
					SCH_CYCLON2_GROUP_SET_SUPPLY_TM( tms , side , pt ); // 2013.01.25
					//
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) > 1 ) {
						Result = 1;
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 ) {
							Result = 0;
						}
						else {
							Result = 1;
						}
					}

					Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );

					if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) <= 0 ) {

						Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( side , pt );
						_SCH_CLUSTER_Set_PathStatus( side , pt , SCHEDULER_STARTING );
						_SCH_CLUSTER_Inc_PathDo( side , pt );
						wsa = _SCH_CLUSTER_Get_SlotIn( side , pt );
						wsb = BM_Slot;
						swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 );
						Sav_Int2 = _SCH_SUB_Get_Last_Status( side );
						if ( _SCH_SUB_Chk_Last_Out_Status( side , pt ) ) {
							_SCH_SUB_Set_Last_Status( side , 2 );
						}

						Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 520 , side , pt , RunBm , Arm , wsa , wsb , Result , swmode );

						if ( Function_Result == SYS_ABORTED ) {
							return -1;
						}
						else if ( Function_Result == SYS_ERROR ) {
							_SCH_CLUSTER_Set_PathStatus( side , pt , Sav_Int1 );
							_SCH_CLUSTER_Dec_PathDo( side , pt );
							_SCH_SUB_Set_Last_Status( side , Sav_Int2 );
						}
						if ( Function_Result == SYS_SUCCESS ) {
							_SCH_MODULE_Set_TM_PATHORDER( tms , Arm , 0 );
							_SCH_MODULE_Set_TM_TYPE( tms , Arm , SCHEDULER_MOVING_IN );
							_SCH_MODULE_Set_TM_SIDE_POINTER( tms , Arm , side , pt , side , 0 );
							_SCH_MODULE_Set_TM_WAFER( tms , Arm , _SCH_MODULE_Get_BM_WAFER( RunBm , BM_Slot ) );
							_SCH_MODULE_Set_BM_WAFER_STATUS( RunBm , BM_Slot , 0 , BUFFER_READY_STATUS );
							_SCH_MODULE_Inc_TM_OutCount( side );
							_SCH_MODULE_Inc_TM_DoPointer( side );
						}

					}

				}
			}

			// [END] PICK BM Arm (Loading) slot
		}

		if ( !fmskip ) {
	//		pt = -1; // 2013.01.25

			if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt , &wsa , &side ) <= 0 ) {

				if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 ) {

					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_OUTWAIT_STATUS ) > 0 ) {

						if ( _SCH_MODULE_Get_TM_WAFER( tms , SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE ) ) <= 0 ) {

							if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

								if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , RunBm ) ) { // 2013.03.29

									// [START] BM Venting

									if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 11 );
									}
									//
									if ( Abort ) return -1;
									//

									// [END] BM Venting
								}
							} 

						}
					}
				}
			}
		}

	}
	else {

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 145 [pmc=%d][RunBm=%d][mr=%d]\n" , tms + 1, pmc , RunBm , *MoveRun );

		if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &BM_Slot , FALSE ) <= 0 ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 146 [pmc=%d][RunBm=%d][mr=%d]\n" , tms + 1, pmc , RunBm , *MoveRun );

			if ( pmc == -1 ) {
				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 103 ) == SYS_ABORTED ) return -1;
				//
//				Sleep(1); // 2013.01.29
				//
				return 1;
			}

			Result = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			//
			if ( Result == 2 ) return 1;

			if ( Result == 0 ) {

				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 104 ) == SYS_ABORTED ) return -1;
				//
//				Sleep(1); // 2013.01.29
				//
				if ( SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , -1 ) ) return 0; // 2013.03.31
				//
				return 1;
			}

//			Sleep(1); // 2013.01.29

			//
			if ( SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , -1 ) ) return 0; // 2013.03.31
			//
			return 1;
		}
		else {

			while ( TRUE ) {
				//
				if ( SCHEDULING_CHECK_BM_Stock_Supply_Check_for_CYCLON( tms , -1 , -1 , FALSE , pmc , StockBm , &StockSlot , &StockMode ) ) { // Stock Swap

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 147 [pmc=%d][RunBm=%d][mr=%d][%d,%d,%d]\n" , tms + 1, pmc , RunBm , *MoveRun , side , pt , BM_Slot );
				
					if ( StockMode == -1 ) break;

					if ( StockReturn ) break;
					//
					if ( !SCHEDULING_CHECK_BM_Stock_Supply_Action_for_CYCLON( tms , -1 , -1 , pmc , RunBm , BM_Slot , StockBm , StockSlot , StockMode , StockReturn , 20 ) ) {

						return -1;

					}
					//
					if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &BM_Slot , FALSE ) <= 0 ) break;
					//
				}
				else {
					break;
				}
			}

		}
		//
	}
	return 0;
}


int Sch_Check_TM_PM_BM_Pre_Cond_W_Check( int run_side , int tms , int RunBm , int pmc , int StockBm , int fmskip , int *MoveRun , int *notfirst , int *pmindex ) {
	int side , pt , BM_Slot , renasts;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
		//
		if ( StockBm != -1 ) return 0;
		//
	}
	//
//	pt = -1; // 2013.01.25
	//
	if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt , &BM_Slot , &side ) > 0 ) {
		//
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 115-1 [pmc=%d][RunBm=%d][mr=%d][nf=%d][pt=%d][BM_Slot=%d][side=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , pt , BM_Slot , side );

		if ( side != run_side ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 113 [pmc=%d][RunBm=%d][mr=%d][nf=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst );

			return 1;
		}
		//
		if ( pmc == -1 ) {
//			Sleep(1); // 2013.01.29
			//
			return 1;
		}
		//
		renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
		//
		if ( renasts == 2 ) return 1;

		if ( ( renasts == 0 ) || ( _SCH_CLUSTER_Get_PathDo( side , pt ) == PATHDO_WAFER_RETURN ) ) {
//			Sleep(1); // 2013.01.29
			return 1;
		}
	}
	else {
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 115-2 [pmc=%d][RunBm=%d][mr=%d][nf=%d][pt=%d][BM_Slot=%d][side=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , pt , BM_Slot , side );
		if ( pmc != -1 ) {
			if ( !SCHEDULER_CONTROL_Chk_ST6_PM_CURR_SIDE( pmc , run_side ) ) {
				//
				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 114 [pmc=%d][RunBm=%d][mr=%d][nf=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst );
				//
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.03.30
					//
					if ( StockBm == -1 ) {
						if ( SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , TRUE , -1 ) ) return 0;
					}
					//
				}
				//
				return 1;
			}
		}
	}

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 115 [pmc=%d][RunBm=%d][mr=%d][nf=%d][pt=%d][BM_Slot=%d][side=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , pt , BM_Slot , side );

	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
		//
		if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) <= 0 ) {
			//
			if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) <= 0 ) ) {
				//
//				Sleep(1); // 2013.01.29
				//
				return 1;
			}
			//
		}
		//
	}

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 120 [pmc=%d][RunBm=%d][mr=%d][nf=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst );

	if ( pmc != -1 ) {

		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {

			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			if ( renasts == 2 ) return 1;

			if ( renasts == 0 ) {
				*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 2 , RunBm , pmc , 0 , 0 );
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 121-1 [pmc=%d][RunBm=%d][mr=%d][nf=%d][*pmindex=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , *pmindex );
			}
			else {
				*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 1 , RunBm , pmc , 0 , 0 );
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 121-2 [pmc=%d][RunBm=%d][mr=%d][nf=%d][*pmindex=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , *pmindex );
			}
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot ) , _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot ) ) == PATHDO_WAFER_RETURN ) {
//				Sleep(1); // 2013.01.29
				return 1;
			}
			*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 0 , RunBm , pmc , _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot ) , _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot ) );
_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 121-3 [pmc=%d][RunBm=%d][mr=%d][nf=%d][*pmindex=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , *pmindex );
		}

		if ( *pmindex == -1 ) {
			//
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) return 0; // 2013.02.08
			//
			_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 999 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
			return -1;
		}

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 121 [pmc=%d][RunBm=%d][mr=%d][nf=%d][*pmindex=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst , *pmindex );

		if ( _SCH_MODULE_GET_USE_ENABLE( pmc , TRUE , -1 ) ) {

			while ( TRUE ) {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 122 [pmc=%d][RunBm=%d][mr=%d][nf=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst );

				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return -1;

				if ( !notfirst ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) <= 0 ) {
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) {
							//
							_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 1 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
							//
							return -1;
							//
						}
						//
						if ( *pmindex != -1 ) { // 2013.02.15
							//
							*notfirst = TRUE;
							//
							if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 21 ) != SYS_SUCCESS ) return -1; // 2013.01.25
							//
							if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , *pmindex , 21 ) != SYS_SUCCESS ) return -1;
							//
							*MoveRun = ( pmc * 100 ) + *pmindex;
							//
						}
					}
				}
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) <= 0 ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) {
						//
						_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted Because 2 %s Abort%cWHBM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( RunBm ) , 9 , tms + 1 );
						//
						return -1;
					}
					//
					break;
				}
				//
				Sleep(1);
				//
			}
		}
	}
	return 0;
}




int Sch_Check_TM_PM_BM_GO_TMSIDE_W_Check1( int run_side , int tms , int RunBm , int pmc , int StockBm , int fmskip , int *MoveRun , int *notfirst , int *pmindex ) {
	BOOL Abort;
	int retmv , renasts , Arm , dm;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
		//
		if ( StockBm != -1 ) return 0;
		//
		if ( SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , TRUE , -1 ) ) return 0; // 2013.03.30
		//
	}
	//
	if ( pmc != -1 ) {
		//
		retmv = FALSE;
		//
		while ( TRUE ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 152 [pmc=%d][RunBm=%d][mr=%d][nf=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *notfirst );

			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return -1;

			if ( !retmv ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) <= 0 ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) {
						//
						_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 4 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
						//
						return -1;
					}

					Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );

					if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) {
						//
						*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 0 , RunBm , pmc , _SCH_MODULE_Get_TM_SIDE( tms , Arm ) , _SCH_MODULE_Get_TM_POINTER( tms , Arm ) );
						//
					}
					//
					if ( *pmindex != -1 ) { // 2013.02.15
						//
						if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 22 ) != SYS_SUCCESS ) return -1; // 2013.01.25
						//
						if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , *pmindex , 22 ) != SYS_SUCCESS ) return -1;
						//
						*MoveRun = ( pmc * 100 ) + *pmindex;
						//
						break;
					}
					//
				}

				renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
				if ( renasts == 2 ) return 1;

				if ( renasts == 0 ) {
					//
					if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 105 ) == SYS_ABORTED ) return -1;
					//
//					Sleep(1); // 2013.01.29
					//
					return 1;
					//
				}

				if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) <= 0 ) ) {

					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , -1 ) <= 0 ) {

						if ( SCH_CYCLON2_GROUP_GET_NEED_DUMMY( tms , &dm ) ) { // 2013.05.02
							//
							*notfirst = FALSE;
							//
							return 2;
							//
						}

						if ( fmskip ) { // 2013.01.29
							//
							if ( SCHEDULER_CONTROL_NEW_SUPPLY_POSSIBLE( run_side , RunBm , pmc ) ) {
								//
								*notfirst = FALSE;
								//
								return 2;
								//
							}
							//
						}
						else {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) == BUFFER_TM_STATION ) {
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) return -1; // 2013.04.03
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) == 0 ) {

									if ( SCHEDULER_CONTROL_NEW_SUPPLY_POSSIBLE( run_side , RunBm , pmc ) ) {

										if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

											// [START] BM Venting
											if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 5001 );
											}
											//
											if ( Abort ) return -1;
											//
											// [END] BM Venting

										}
										//
										retmv = TRUE;
									}
								}
							}
						}
					}
				}
			}
			else {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) == BUFFER_TM_STATION ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) < 0 ) return -1; // 2013.04.03
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RunBm ) == 0 ) {
						//
						*notfirst = FALSE;
						//
						break;
					}

				}
			}

			CYCLON_TM_SET_LOCKING( run_side , tms , TRUE );

			_SCH_SYSTEM_LEAVE_TM_CRITICAL( tms );
			//
			Sleep(1);
			//
			_SCH_SYSTEM_ENTER_TM_CRITICAL( tms );

			CYCLON_TM_SET_LOCKING( run_side , tms , FALSE );
			//
		}
		//
		if ( retmv ) return 2;
		//
	}
	//
	return 0;
	//
}


BOOL Sch_TM_Properly_Slot_Place( int tms , int arm , int pmc , int pmdepth ) { // 2013.01.25
	int s , p , l;
	s = _SCH_MODULE_Get_TM_SIDE( tms , arm );
	p = _SCH_MODULE_Get_TM_POINTER( tms , arm );
	//
	l = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s , p );
	if ( l == 0 ) return TRUE;
	if ( l == pmdepth ) return TRUE;
	//
	return FALSE;
}

BOOL SCHEDULING_CHECK_BM_HAS_IN_MORE_SUPPLY_for_CYCLON( int tms , int side , int pt , int pmc ) { // 2013.05.16
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	if ( Scheduler_BM_Get_First_for_CYCLON( tms ) >= BM1 ) return FALSE;
	//
	return SCH_CYCLON2_GROUP_SET_SUPPLY_MORE_PM( tms , side , pt , pmc , SIZE_OF_CYCLON_PM_DEPTH(pmc) );
	//
}

int Sch_Check_TM_PLACE_PM_Load_Run( int run_side , int tms , int RunBm , int pmc , int pmindex ) {
	int Result , Arm , side , pt , wsa , wsb , Function_Result;
	//
	if ( pmc == -1 ) return 0;

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 182 [pmc=%d][RunBm=%d][pmindex=%d]\n" , tms + 1 , pmc , RunBm , pmindex );

	if ( _SCH_MODULE_Get_TM_WAFER( tms , SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE ) ) <= 0 ) return 0;

	_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 184 [pmc=%d][RunBm=%d][pmindex=%d]\n" , tms + 1 , pmc , RunBm , pmindex );

	Result = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
	if ( Result == 2 ) return 1;
	if ( Result != 1 ) return 0;

	Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );

	if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) <= 0 ) return 0;

	//
	if ( SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( tms , -1 , -1 ) ) { // 2014.12.24
		//
		side = _SCH_MODULE_Get_TM_SIDE( tms , Arm );
		pt   = _SCH_MODULE_Get_TM_POINTER( tms , Arm );
		//
		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 184-2 [pmc=%d][RunBm=%d][pmindex=%d][%d,%d,%d,%d]\n" , tms + 1 , pmc , RunBm , pmindex , side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) , _SCH_CLUSTER_Get_PathRange( side , pt ) );
		//
		if ( _SCH_CLUSTER_Get_PathDo( side , pt ) > _SCH_CLUSTER_Get_PathRange( side , pt ) ) return 0;
		//
		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 184-3 [pmc=%d][RunBm=%d][pmindex=%d]\n" , tms + 1 , pmc , RunBm , pmindex );
		//
	}

	if ( _SCH_MODULE_Get_PM_WAFER( pmc , pmindex - 1 ) > 0 ) return 0;

	// [START] PLACE PM Arm (Loading) pmindex

	if ( !Sch_TM_Properly_Slot_Place( tms , Arm , pmc , pmindex ) ) { // 2013.01.25
		//
		_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted Because %s:%d Invalid Slot Place%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , pmindex , 9 , pmc - PM1 + 1 );
		//
		return -1;
		//
	}
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
		//
		if ( pmindex != SCH_CYCLON2_Get_PM_SLOT_INFO( pmc ) ) {
			//
			if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 1001 ) != SYS_SUCCESS ) return -1;
			//
//			if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , -1 , 1001 ) != SYS_SUCCESS ) return -1; // 2013.04.11
			if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , pmindex , 1001 ) != SYS_SUCCESS ) return -1; // 2013.04.11
			//
		}
		//
		if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , pmindex , TRUE , 1001 ) != SYS_SUCCESS ) return -1;
	}
	//
	if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) > 0 ) {
		Result = 1;
	}
	else {
//		pt = -1; // 2013.01.25
		if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt , &wsa , &side ) <= 0 ) {
			if ( SCHEDULING_CHECK_BM_HAS_IN_MORE_SUPPLY_for_CYCLON( tms , _SCH_MODULE_Get_TM_SIDE( tms , Arm ) , _SCH_MODULE_Get_TM_POINTER( tms , Arm ) , pmc ) ) { // 2013.05.16
				Result = 1;
			}
			else {
				Result = 0;
			}
		}
		else {
			Result = 1;
		}
	}

	_SCH_MODULE_Set_PM_STATUS( pmc , pmindex - 1 , 0 );

	side = _SCH_MODULE_Get_TM_SIDE( tms , Arm );
	pt   = _SCH_MODULE_Get_TM_POINTER( tms , Arm );
	wsa  = _SCH_MODULE_Get_TM_WAFER( tms , Arm );
	wsb  = pmindex;
	//
	switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) ) {
	case -1 :
		_SCH_LOG_LOT_PRINTF( side , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
		return -1;
		break;
	}
	//
	Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 540 , side , pt , pmc , Arm , wsa , wsb , Result , 0 );
	//
	if ( Function_Result == SYS_ABORTED ) {
		return -1;
	}
	else if ( Function_Result == SYS_ERROR ) {
	}
	//
	if ( Function_Result == SYS_SUCCESS ) {
		//
		_SCH_MODULE_Set_PM_SIDE( pmc , pmindex - 1 , side );
		_SCH_MODULE_Set_PM_POINTER( pmc , pmindex - 1 , pt );
		_SCH_MODULE_Set_PM_WAFER( pmc , pmindex - 1 , _SCH_MODULE_Get_TM_WAFER( tms , Arm ) );
		_SCH_MODULE_Set_TM_WAFER( tms , Arm , 0 );
		//
		if ( ( _SCH_CLUSTER_Get_SwitchInOut( side , pt ) % 2 ) == 0 )
			_SCH_CLUSTER_Set_SwitchInOut( side , pt , 2 );
		else
			_SCH_CLUSTER_Set_SwitchInOut( side , pt , 3 );
		//
	}

	// [END] PLACE PM Arm (Loading) pmindex
	return 0;
}


int Sch_Check_TM_PM_Process_Run( int run_side , int tms , int RunBm , int pmc , int StockBm , int *MoveRun , int *pmindex , int *prcsing , int *NextProcess_pmc , int *NextProcess_side , int *NextProcess_pt , int *NextProcess_firstnotuse ) {
	//
	int i , renasts , pt2 , BM_Slot2 , side2 , Arm , wsa , firstnotuse , CHECKING_SIDE , pt , stock , ok;
	//
	//
//	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25 // 2013.04.01
//		//
//		if ( StockBm != -1 ) return 0;
//		//
//	}
	//
	if ( pmc != -1 ) {

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 192 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

		renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
		if ( renasts == 2 ) return 1;
		if ( renasts != 1 ) return 0;

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 193 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

//		pt2 = -1; // 2013.01.25

		ok = FALSE;
		//
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25 // 2013.04.01
			//
			if ( StockBm != -1 ) {
				ok = TRUE;
			}
			else {
				if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt2 , &BM_Slot2 , &side2 ) <= 0 ) ok = TRUE;
			}
		//
		}
		else {
			if ( SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( RunBm , &pt2 , &BM_Slot2 , &side2 ) <= 0 ) ok = TRUE;
		}
		//
		if ( ok ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 194 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

			if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 ) {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 195 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

				*prcsing = TRUE;

				if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 0 ) > 0 ) {

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 196 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

					if ( SCH_CYCLON2_GROUP_PROCESS_POSSIBLE( tms , pmc ) ) {

						SCHEDULER_CONTROL_Chk_ST6_PM_RUN_MODE( pmc );

						if ( !SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc ) ) {

							// [START] process

							Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );

							if ( SCHEDULER_CONTROL_Chk_ST6_PM_GET_SIDE_PT( pmc , &CHECKING_SIDE , &pt , &wsa , &firstnotuse , &stock ) ) {

								_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 197 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d][%d,%d,%d,%d,%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex , CHECKING_SIDE , pt , wsa , firstnotuse , stock );

								if ( !SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc ) ) {

									_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 198 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

									SCH_CYCLON2_GROUP_SET_PROCESS( tms , pmc , stock );

									if ( _SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , pmc , Arm , firstnotuse , 611 ) < 0 ) {
										//
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Process Recipe Fail at %s(%d)%cPROCESS_FAIL PM%d:%d::6001\n" , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , wsa , 9 , pmc - PM1 + 1 , wsa );
										//
										return -1;
									}
									//
									if ( firstnotuse ) {
										_SCH_MODULE_Set_PM_SIDE( pmc , 0 , CHECKING_SIDE );
										_SCH_MODULE_Set_PM_POINTER( pmc , 0 , pt );
										_SCH_MODULE_Set_PM_WAFER( pmc , 0 , wsa );
										_SCH_MODULE_Set_PM_STATUS( pmc , 0 , 100 );
									}

									if ( !firstnotuse ) {
										if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 0 ) || ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 2 ) )
											_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 4 );
										else
											_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 5 );
									}
//									for ( i = 1 ; i <= 5 ; i++ ) { // 2014.09.23
									for ( i = 1 ; i < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; i++ ) { // 2014.09.23
										if ( _SCH_MODULE_Get_PM_WAFER( pmc , i ) > 0 ) {
											if ( ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) == 0 ) || ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) ) == 2 ) )
												_SCH_CLUSTER_Set_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , 4 );
											else
												_SCH_CLUSTER_Set_SwitchInOut( _SCH_MODULE_Get_PM_SIDE( pmc , i ) , _SCH_MODULE_Get_PM_POINTER( pmc , i ) , 5 );
										}
									}
									//
									if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.29
										//
										*NextProcess_pmc = pmc;
										*NextProcess_side = CHECKING_SIDE;
										*NextProcess_pt = pt;
										*NextProcess_firstnotuse = firstnotuse;
										//
									}
								}
							}
							// [END] process
						}
					}
				}
			}
			else {
				// 2013.01.29
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) <= 0 ) { // 2013.01.29
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) {
						//
						_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 12 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
						//
						return -1;
						//
					}
				}
				else {
					return 0;
				}
				//
				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 201 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

				*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 1 , RunBm , pmc , 0 , 0 );

				if ( *pmindex == -1 ) {
					//
					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) return 0; // 2013.02.08
					//
					_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 998 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
					//
					return -1;
				}
				//
				if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 24 ) != SYS_SUCCESS ) return -1; // 2013.01.25
				//
				if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , *pmindex , 24 ) != SYS_SUCCESS ) return -1;
				//
				MoveRun = ( pmc * 100 ) + pmindex;
				//
			}
		}
		else {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 202 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

			if ( pmc == -1 ) {
				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 106 ) == SYS_ABORTED ) return -1;
//				Sleep(1); // 2013.01.29
				return 1;
			}

			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			if ( renasts == 2 ) return 1;
			if ( renasts == 0 ) {
				//
				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 107 ) == SYS_ABORTED ) return -1;
//				Sleep(1); // 2013.01.29
				return 1;
			}

			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot2 ) ) == PATHDO_WAFER_RETURN ) {
				if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 108 ) == SYS_ABORTED ) return -1;
				//
//				Sleep(1); // 2013.01.29
				//
				return 1;
			}

			// 2013.01.29
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) <= 0 ) { // 2013.01.29
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) {
					//
					_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 12 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
					//
					return -1;
					//
				}
			}
			else {
				return 0;
			}
			//
			Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE );

			if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) {
				//
				*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 0 , RunBm , pmc , _SCH_MODULE_Get_TM_SIDE( tms , Arm ) , _SCH_MODULE_Get_TM_POINTER( tms , Arm ) );
				//
			}
			else {
				//
				*pmindex = SCHEDULER_CONTROL_ST6_PM_SLOT_CHECK( run_side , tms , 0 , RunBm , pmc , _SCH_MODULE_Get_BM_SIDE( RunBm , BM_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( RunBm , BM_Slot2 ) );
				//
			}

			if ( *pmindex == -1 ) {
				//
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) return 0; // 2013.02.08
				//
				_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 997 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
				//
				return -1;
			}

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 203 [pmc=%d][RunBm=%d][mr=%d][pr=%d][pi=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , *prcsing , *pmindex );

			//
			if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 25 ) != SYS_SUCCESS ) return -1; // 2013.01.25
			//
			if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , *pmindex , 25 ) != SYS_SUCCESS ) return -1;
			//
			*MoveRun = ( pmc * 100 ) + *pmindex;
			//
		}
	}
	//
	return 0;
	//
}








int Sch_Check_TM_PM_BM_GO_TMSIDE_Check2( int run_side , int tms , int RunBm , int pmc , int StockBm , int fmskip , int prcsing , int *MoveRun , int NextProcess_pmc , int NextProcess_side , int NextProcess_pt , int NextProcess_firstnotuse ) {
	BOOL Abort;
	int renasts , BM_Slot , prcheck , cond;
	//
	if ( fmskip ) return 0; // 2013.01.29
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.29
		//
		if ( prcsing && ( pmc != -1 ) && !PMC_HAS_NEXT_BM_WAIT( NextProcess_pmc , StockBm , NextProcess_side , NextProcess_pt ) ) {
			prcheck = TRUE;
		}
		else {
			prcheck = FALSE;
		}
		//
	}
	else {
		if ( pmc != -1 ) {
			prcheck = TRUE;
		}
		else {
			prcheck = FALSE;
		}
	}
	//
	if ( prcheck ) {

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 222 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

		if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 223 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

			renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
			//
			if ( renasts == 2 ) return 1;

			if ( renasts == 1 ) {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 223-1 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) { // 2013.01.25

					cond = 0;
					//
					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.02.14
						if ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_OUTWAIT_STATUS ) > 0 ) || ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) <= 0 ) ) cond = 1;
					}
					else { // 2013.02.14
						if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) <= 0 ) {
							cond = 1;
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_OUTWAIT_STATUS ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT_STYLE_6( RunBm ) <= 0 ) {
									cond = 1;
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) <= 0 )
										cond = 1;
									else
										cond = -1;
								}
							}
						}
					}
					//
					if ( !SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( tms , RunBm , pmc ) ) { // 2014.12.24
						//
						if ( cond == 1 ) {

							if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
								// [START] BM Venting
								if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 12 );
								}
								//
								if ( Abort ) return -1;
								//
								// [END] BM Venting
							}
						}
						else if ( cond == 0 )  {

							_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 224 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

							if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( RunBm , FALSE , TRUE , TRUE ) ) {

								_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 225 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

	//							Sleep(100); // 2013.01.29
								Sleep(1); // 2013.01.29

								if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return -1;

								if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( RunBm , FALSE , TRUE , TRUE ) ) {
									//
									if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

										// [START] BM Venting
										if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 13 );
										}
										//
										if ( Abort ) return -1;
										//
										// [END] BM Venting
									}
								}
							}
						}
					}
				}
				//
			}
			else {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 223-2 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) > 0 ) {

					if ( SCHEDULER_CONTROL_ST6_MOVE_WAITING( run_side , pmc , MoveRun , 111 ) == SYS_ABORTED ) return -1;
					//
//					Sleep(1); // 2013.01.29

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 226 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

					return 1;
				}

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 227 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

				if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , -1 ) <= 0 ) {

					_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 228 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) <= 0 ) ) {

						if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

							if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , RunBm ) ) { // 2013.03.29

								// [START] BM Venting
								if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 1201 );
								}
								//
								if ( Abort ) return -1;
								//
								// [END] BM Venting

							}

						}
					}
					else {
						if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &BM_Slot , TRUE ) <= 0 ) {

							if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
								// [START] BM Venting
								if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 1202 );
								}
								//
								if ( Abort ) return -1;
								//
								// [END] BM Venting
							}
						}
					}
				}
				else {
					if ( SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( tms , RunBm , &BM_Slot , TRUE ) <= 0 ) {

						if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {

							// [START] BM Venting
							if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 1203 );
							}
							//
							if ( Abort ) return -1;
							//
							// [END] BM Venting
						}
					}
				}
			}
		}
	}
	else {

		_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 229 [pmc=%d][RunBm=%d][mr=%d][pr=%d][st=%d]\n" , tms + 1 , pmc , RunBm , *MoveRun , prcsing , StockBm );

		if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) <= 0 ) ) {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( RunBm ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( RunBm , BUFFER_INWAIT_STATUS ) <= 0 ) {
						//
						if ( _SCH_MODULE_Get_BM_FULL_MODE( RunBm ) != BUFFER_FM_STATION ) {
							
							if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , FALSE , RunBm ) ) { // 2013.03.29
								// [START] BM Venting
								if ( !SCHEDULER_CONTROL_Chk_BM_FMWAIT_AND_RUNNED_STYLE_6( tms , RunBm , &Abort ) ) { // 2018.10.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , run_side , RunBm , -1 , TRUE , tms + 1 , 1204 );
								}
								//
								if ( Abort ) return -1;
								//
								// [END] BM Venting
							}
						}
					}
				}
			}
		}
	}
	//
	if ( NextProcess_pmc > 0 ) { /* 2010.04.26 */ \
		//
		renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , NextProcess_pmc );
		//
		if ( renasts == 2 ) return 1;

		if ( renasts == 1 ) {
			switch( SCHEDULER_CONTROL_NEXT_PROCESSING( run_side , tms , StockBm , RunBm , NextProcess_pmc , NextProcess_side , NextProcess_pt , NextProcess_firstnotuse ) ) {
			case 0 :
				_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 900 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( NextProcess_pmc ) , 9 , NextProcess_pmc - PM1 + 1 );
				return -1;
			case 1 :
				break;
			default :
				return 1;
			}
		}
	}
	//
	return 0;
}



int SCH_GET_PM_SLOT( int pmc , int mode ) {
	int j;
	//
	for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 100 ) { // temp
				//
				if ( mode == -1 ) return j+1;
				//
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) == mode ) return j+1;
				//
			}
		}
	}
	return -1;
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( int bm , int pm , int pmindex , BOOL *haswfr , BOOL dischk ) { // 2013.05.02
	int i , s , p , w , w2 , l , l2;
	//
	*haswfr = FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	if ( pmindex < 0 ) return FALSE;
	if ( _SCH_MODULE_Get_PM_WAFER( pm , pmindex ) <= 0 ) return FALSE;
	//
	*haswfr = TRUE;
	//
	s = _SCH_MODULE_Get_PM_SIDE( pm , pmindex );
	p = _SCH_MODULE_Get_PM_POINTER( pm , pmindex );
	w = _SCH_CLUSTER_Get_SwitchInOut( s , p );
	l = _SCH_CLUSTER_Get_Stock( s , p ) / 100; // 2013.06.26
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) continue;
		//
		if ( !dischk ) { // 2013.06.26
			if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) != BUFFER_OUTWAIT_STATUS ) continue; // 2013.06.26
		}
		//
		s = _SCH_MODULE_Get_BM_SIDE( bm , i );
		p = _SCH_MODULE_Get_BM_POINTER( bm , i );
		w2 = _SCH_CLUSTER_Get_SwitchInOut( s , p );
		l2 = _SCH_CLUSTER_Get_Stock( s , p ) / 100; // 2013.06.26
		//
//		if ( dischk ) { // 2013.06.26 // 2013.08.01
			if ( ( w >= 4 ) && ( w2 < 4 ) ) return TRUE;
			if ( ( w2 >= 4 ) && ( w < 4 ) ) return TRUE;
//		} // 2013.08.01
//		else { // 2013.07.12
			if ( l != l2 ) return TRUE; // 2013.06.26
			//
//		} // 2013.07.12
		//
	}
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_STOCK_PM_SAME_OUT_STYLE_6( int bm , int pmc ) { // 2013.09.02
	int i , x , s , p , wx , lx , w , l , w2 , l2;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	wx = -1;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( bm , i );
		p = _SCH_MODULE_Get_BM_POINTER( bm , i );
		w2 = _SCH_CLUSTER_Get_SwitchInOut( s , p );
		l2 = _SCH_CLUSTER_Get_Stock( s , p ) / 100;
		//
		if ( wx == -1 ) {
			//
			wx = w2;
			lx = l2;
			//
			for ( x = 0 ; x < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; x++ ) {
				//
				if ( _SCH_MODULE_Get_PM_WAFER( pmc , x ) <= 0 ) continue;
				//
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , x ) == 100 ) continue;
				//
				s = _SCH_MODULE_Get_PM_SIDE( pmc , x );
				p = _SCH_MODULE_Get_PM_POINTER( pmc , x );
				w = _SCH_CLUSTER_Get_SwitchInOut( s , p );
				l = _SCH_CLUSTER_Get_Stock( s , p ) / 100;
				//
				if ( ( wx == w ) && ( lx == l ) ) break;
				//
			}
			//
			if ( x == SIZE_OF_CYCLON_PM_DEPTH(pmc) ) return FALSE;
			//
		}
		else {
			if ( ( wx != w2 ) || ( lx != l2 ) ) return FALSE;
		}
		//
	}
	//
	if ( wx == -1 ) return FALSE;
	//
	return TRUE;
	//
}


int Sch_Check_TM_PICK_ST_OUT_Unload_Run( int run_side , int tms , int StockBm , int RunBm , int pmc , int pmindex ) { // 2013.04.11
	int Stockfree , StockBmSlot , sm , BMSlot , i , s , p , haswfr;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return 0;
	//
	if ( StockBm == -1 ) return 0;
	//
	StockBmSlot = 0;
	Stockfree = FALSE;
	sm = 0;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( StockBm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( StockBm , i ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( StockBm , i );
			p = _SCH_MODULE_Get_BM_POINTER( StockBm , i );
			//
			if ( _SCH_MODULE_Get_BM_STATUS( StockBm , i ) == BUFFER_OUTWAIT_STATUS ) {
				if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) {
					if ( sm != 1 ) { // 2013.05.02
						StockBmSlot = i;
						sm = 1;
					}
//					break; // 2013.05.02
				}
				else { // 2013.05.02
					/*
					// 2013.08.01
					if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( StockBm , pmc , pmindex - 1 , &haswfr , TRUE ) ) {
							if ( sm == 0 ) {
								sm = 2;
								StockBmSlot = i;
							}
						}
					}
					*/
					//
					// 2013.08.01
					if ( SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( StockBm , pmc , pmindex - 1 , &haswfr , SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) ) {
						if ( sm == 0 ) {
							sm = 2;
							StockBmSlot = i;
						}
					}
					//
				}
				//
				if ( StockBmSlot == 0 ) StockBmSlot = i;
				//
			}
			else { // 2013.07.09
				if ( !SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
					return 0;
				}
			}
		}
		else {
			Stockfree = TRUE;
		}
	}
	//
	if ( StockBmSlot == 0 ) return 0;
	//
	if ( sm == 0 ) {
		if ( Stockfree ) return 0;
	}
	else { // 2013.05.20
		if ( Stockfree ) {
			/*
			// 2013.08.01
			if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
				if ( !SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( StockBm , pmc , pmindex - 1 , &haswfr , TRUE ) ) {
					if ( haswfr ) return 0;
				}
			}
			*/
			// 2013.08.01
			if ( !SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( StockBm , pmc , pmindex - 1 , &haswfr , SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) ) {
				if ( haswfr ) return 0;
			}
			//
		}
	}
	//
	if ( Stockfree ) { // 2013.09.02
		if ( SCHEDULER_CONTROL_Chk_BM_STOCK_PM_SAME_OUT_STYLE_6( StockBm , pmc ) ) {
			return 0;
		}
	}
	//
	BMSlot = 0;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( RunBm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( RunBm , i ) <= 0 ) {
			BMSlot = i;
			break;
		}
	}
	//
	if ( BMSlot == 0 ) return 0;
	//
	if ( !SCHEDULING_CHECK_BM_Stock_Supply_Action_for_CYCLON( tms , run_side , 0 , Scheduler_PM_Get_First_for_CYCLON( tms , TRUE ) , RunBm , BMSlot , StockBm , StockBmSlot , 2 , TRUE , 30 ) ) return -1;
	//
	return 0;
}


int Sch_Check_TM_PICK_PM_ST_Unload_Run( int run_side , int tms , int StockBm , int pmc , int pmindex , int RealBm , int RealStock , int StockReturn ) {
	int renasts , StockBmSlot , Result , CHECKING_SIDE , pt , wsa , wsb , Arm , Sav_Int1 , swmode , Function_Result , picking , haswfr;
	int j;
	//
	picking = 0;
	//
	if ( pmc != -1 ) {
		//
		renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
		if ( renasts == 2 ) return 1;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( pmc , TRUE , -1 ) ) return 0;
		//
		Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) return 0;
		//
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.25
			StockBmSlot = SCHEDULER_CONTROL_BM_WAFER_HAS_RETURN_WAFER( StockBm );
			//
			if ( StockBmSlot == 0 ) {
				picking = 1;
			}
			else {
				//
				if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ) return 0;
				//
				picking = 2;
			}
			//
		}
		else {
			if ( StockReturn ) { // 2013.03.30
				StockBm = Scheduler_BM_Get_First_for_CYCLON( tms );
				StockBmSlot = SCHEDULER_CONTROL_BM_WAFER_HAS_RETURN_WAFER( StockBm );
				//
				if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ) return 0;
				//
				picking = 3;
			}
			else {
				//
				StockBmSlot = 0;
				//
				picking = 1;
				//
				if ( RealStock != -1 ) {
					if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
						//
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( RealStock ) ) {
							//
							StockBm = RealStock;
							StockBmSlot = SCHEDULER_CONTROL_BM_WAFER_HAS_RETURN_WAFER( RealStock );
							//
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ) return 0;
							//
							picking = 3;
						}
						//
					}
				}
				//
			}
		}
		//
	}
	else {
		//
		if ( StockReturn ) { // 2013.03.30
			//
			StockBm = Scheduler_BM_Get_First_for_CYCLON( tms );
			StockBmSlot = SCHEDULER_CONTROL_BM_WAFER_HAS_RETURN_WAFER( StockBm );
			//
//			Arm = TA_STATION; // 2015.01.28
			//
			//
			// 2015.01.28
			//
			if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
				if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
					Arm = TB_STATION;
				}
				else {
					Arm = TA_STATION;
				}
			}
			else {
				Arm = TA_STATION;
			}
			//
			if ( _SCH_MODULE_Get_TM_WAFER( tms , Arm ) > 0 ) return 0;
			//
			if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ) return 0;
			//
			picking = 3;
			//
		}
		//
	}
	//

	if ( picking == 3 ) {
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( RealBm ) != BUFFER_TM_STATION ) return 0;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RealBm ) < 0 ) return -1; // 2013.04.03
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( RealBm ) != 0 ) return 0;
		//
		CHECKING_SIDE = _SCH_MODULE_Get_BM_SIDE( StockBm , StockBmSlot );
		pt = _SCH_MODULE_Get_BM_POINTER( StockBm , StockBmSlot );
		//
		if ( SCHEDULING_CHECK_BM_Stock_Supply_Check_for_CYCLON( tms , -1 , -1 , TRUE , -1 , RealBm , &wsa , &wsb ) ) { // Stock Swap
	
			if ( !SCHEDULING_CHECK_BM_Stock_Supply_Action_for_CYCLON( tms , CHECKING_SIDE , pt , Scheduler_PM_Get_First_for_CYCLON( tms , TRUE ) , RealBm , wsa , StockBm , StockBmSlot , 2 , TRUE , 40 ) ) {

				return -1;

			}
		
		}
		//
	}
	else if ( picking == 2 ) {

		// [START] PICK BM_STOCK Arm (Unloading) StockBmSlot

		Result = 0;

		CHECKING_SIDE = _SCH_MODULE_Get_BM_SIDE( StockBm , StockBmSlot );
		pt = _SCH_MODULE_Get_BM_POINTER( StockBm , StockBmSlot );

		wsa = _SCH_MODULE_Get_BM_WAFER( StockBm , StockBmSlot );
		wsb = StockBmSlot;

		Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING );
		_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
		swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 );
		//
		Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 520 , CHECKING_SIDE , pt , StockBm , Arm , wsa , wsb , Result , swmode );
		//
		if ( Function_Result == SYS_ABORTED ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Fail from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( StockBm ) , wsb , wsa , Arm + 'A' , 9 , tms + 1 , StockBm - BM1 + 1 , wsb , wsa , Arm + 'A' , 9 , wsa );
			_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , StockBm , 1 , Arm , wsa , wsa );
			return -1;
		}
		else if ( Function_Result == SYS_ERROR ) {
			_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
			_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
		}
		if ( Function_Result == SYS_SUCCESS ) {
			_SCH_MODULE_Set_TM_SIDE_POINTER( tms , Arm , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
			_SCH_MODULE_Set_TM_WAFER( tms , Arm , wsa );
			_SCH_MODULE_Set_BM_WAFER_STATUS( StockBm , StockBmSlot , 0 , BUFFER_READY_STATUS );
		}

		// [END] PICK BM_STOCK Arm (Unloading) StockBmSlot
	}
	else {
		if ( picking == 1 ) { // 2013.03.30
			//
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
				//
				Result = SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc );
				//
				Result = ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , pmindex , ( !Result ) && _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ? 1 : -1 ) > 0 );
				//
				if ( Result ) { // 2014.12.24
					//
					if ( SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( tms , -1 , -1 ) ) { // 2014.12.24
						//
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT_STYLE_6( RealBm ) <= 0 ) Result = FALSE;
						//
					}
					//
				}
				//
			}
			else { // 2013.01.29
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) <= 0 ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) == -1 ) {
						//
						_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 13 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
						//
						return -1;
						//
					}
				}
				else {
					return 0;
				}
				//
/*
// 2013.04.01
				if ( StockBm == -1 ) {
					//
					Result = SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc );

					Result = ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , pmindex , ( !Result ) && _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ? 1 : -1 ) > 0 );
					//
				}
				else {
					Result = FALSE;
				}
*/
				if ( StockBm == -1 ) {
					//
					Result = SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc );

					Result = ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , pmindex , ( !Result ) && _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ? 1 : -1 ) > 0 );
					//
				}
				else {
					//
					if ( Stock_Have_Return( StockBm ) ) return 0; // 2013.04.11
					//
					if ( SCHEDULING_CHECK_BM_Stock_Supply_Check_for_CYCLON( tms , -1 , -1 , TRUE , -1 , StockBm , &StockBmSlot , &wsb ) ) { // Stock Swap
						// 2013.04.01
						Result = SCHEDULER_CONTROL_ST6_PM_HAS_RETURN_WAFER_BEFORE_PROCESS( pmc );
						//
						if ( pmindex == -1 ) {
							if ( ( !Result ) && _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ) { // 1(Process)
								pmindex = SCH_GET_PM_SLOT( pmc , 1 );
							}
							else { // -1(all)
								pmindex = SCH_GET_PM_SLOT( pmc , -1 );
							}
						}
						//
						if ( pmindex == -1 ) {
							Result = FALSE;
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( StockBm , BUFFER_INWAIT_STATUS ) > 0 ) {
								Result = FALSE;
							}
							else {
								//
								picking = 9;
								//
								Result = ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , pmindex , ( !Result ) && _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ? 1 : -1 ) > 0 );
								//
								if ( Result ) { // 2013.06.26
									//
									switch ( SCHEDULING_CHECK_STOCK_Not_Prepared( StockBm ) ) { // 2013.06.26
									case -1 :
										return -1;
										break;
									case 1 :
										return 0;
										break;
									}
									//
								}
								//
							}
						}
					}
					else {
						Result = FALSE;
					}
				}
			}
			//
			if ( Result ) { // 2013.03.31
				//
				if ( RealStock != -1 ) {
					//
//					if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) { // 2013.06.26
						//
//						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_6( RealStock ) ) Result = FALSE; // 2013.05.02
					if ( SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( RealStock , pmc , pmindex - 1 , &haswfr , SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) ) {
						//
						Result = FALSE; // 2013.05.02
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ) { // 2013.07.24
							//
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) <= 0 ) ) {
								//
								for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
									//
									if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) <= 0 ) continue;
									//
									if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) == 100 ) continue; // temp
									//
									if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 1 ) {
										if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_PM_SIDE( pmc , j ) , _SCH_MODULE_Get_PM_POINTER( pmc , j ) ) != PATHDO_WAFER_RETURN ) {
											continue;
										}
									}
									//
									if ( !SCHEDULER_CONTROL_Chk_BM_STOCK_PM_DIFF_OUT_STYLE_6( RealStock , pmc , j , &haswfr , SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) ) {
										pmindex = j+1;
										Result = TRUE;
										break;
									}
									//
								}
							}
						}
						//
					}
						//
//					} // 2013.06.26
					//
				}
			}
			//
			if ( Result ) {

				_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 173 [pmc=%d][stBm=%d][pi=%d]\n" , tms + 1 , pmc , StockBm , pmindex );

				if ( _SCH_MODULE_Get_PM_STATUS( pmc , 0 ) == 100 ) {
					_SCH_MODULE_Set_PM_WAFER( pmc , 0 , 0 );
					_SCH_MODULE_Set_PM_STATUS( pmc , 0 , 1 );
				}

				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
					//
					if ( pmindex != SCH_CYCLON2_Get_PM_SLOT_INFO( pmc ) ) {
						//
						if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , -1 , TRUE , 1002 ) != SYS_SUCCESS ) return -1;
						//
//						if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , -1 , 1002 ) != SYS_SUCCESS ) return -1; // 2013.04.11
						if ( SCHEDULER_CONTROL_ST6_MOVE_ACTION( run_side , pmc , pmindex , 1002 ) != SYS_SUCCESS ) return -1; // 2013.04.11
						//
					}
					//
					if ( SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , pmindex , TRUE , 1002 ) != SYS_SUCCESS ) return -1;
					//
				}
				//
				// [START] PICK PM Arm (Unloading) pmindex

				renasts = SCHEDULER_MODULE_RUN_ENABLE( run_side , tms , pmc );
				if ( renasts == 2 ) return 1;

				if ( renasts == 1 ) {
					if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) > 1 ) {
						Result = 1;
					}
					else {
						if ( _SCH_MODULE_Get_TM_WAFER( tms , SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , TRUE ) ) > 0 ) {
							Result = 1;
						}
						else {
							Result = 0;
						}
					}
				}
				else {
//					Result = 0; // 2013.05.02
					// 2013.05.02
					if ( SCHEDULER_CONTROL_Chk_ST6_PM_HAS_COUNT( pmc , -1 , 1 ) > 1 ) {
						Result = 1;
					}
					else {
						Result = 0;
					}
				}

				CHECKING_SIDE = _SCH_MODULE_Get_PM_SIDE( pmc , pmindex - 1 );
				pt = _SCH_MODULE_Get_PM_POINTER( pmc , pmindex - 1 );
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) ) {
				case -1 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
					return -1;
					break;
				}
				wsa = _SCH_MODULE_Get_PM_WAFER( pmc , pmindex - 1 );
				wsb = pmindex;
				Arm = SCHEDULER_CONTROL_ST6_GET_TM_ARM( pmc , FALSE );
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING );
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 );
				//
				PM_WAIT_LAST_STS_6[pmc] = _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ); // 2018.10.02
				//
				Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PICK + 530 , CHECKING_SIDE , pt , pmc , Arm , wsa , wsb , Result , swmode );
				//
				if ( Function_Result == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Fail from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , wsb , wsa , Arm + 'A' , 9 , tms + 1 , pmc - PM1 + 1 , wsb , wsa , Arm + 'A' , 9 , wsa );
					_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , pmc , 1 , Arm , wsa , wsa );
					return -1;
				}
				else if ( Function_Result == SYS_ERROR ) {
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
				}
				//
				if ( Function_Result == SYS_SUCCESS ) {
					//
					_SCH_MODULE_Set_TM_SIDE_POINTER( tms , Arm , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
					_SCH_MODULE_Set_TM_WAFER( tms , Arm , _SCH_MODULE_Get_PM_WAFER( pmc , pmindex - 1 ) );
					_SCH_MODULE_Set_PM_WAFER( pmc , pmindex - 1 , 0 );
					//
					SCH_CYCLON2_GROUP_AFTER_PICK_PM( tms , CHECKING_SIDE , pt , pmc , SIZE_OF_CYCLON_PM_DEPTH(pmc) ); // 2013.05.28
					//
				}
				//
				// [END] PICK PM Arm (Unloading) pmindex

				//
				if ( picking == 9 ) {
					//
					Function_Result = SCHEDULING_MOVE_OPERATION( tms , MACRO_PLACE + 590 , CHECKING_SIDE , pt , StockBm , Arm , wsa , StockBmSlot , 0 , 0 );
					//
					if ( Function_Result == SYS_ABORTED ) {
						return -1;
					}
					else if ( Function_Result == SYS_ERROR ) {
					}
					if ( Function_Result == SYS_SUCCESS ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( StockBm , StockBmSlot , CHECKING_SIDE , pt );
						_SCH_MODULE_Set_BM_WAFER_STATUS( StockBm , StockBmSlot , _SCH_MODULE_Get_TM_WAFER( tms , Arm ) , BUFFER_OUTWAIT_STATUS );
						_SCH_MODULE_Set_TM_WAFER( tms , Arm , 0 );
						//
						SCHEDULING_CHECK_STOCK_Processing( tms , StockBm , FALSE , pmc ); // 2013.06.26
						//
					}
					//
				}
				//
			}
		}
	}
	return 0;
}


int Sch_Check_TM_PM_Move_W_Run( int run_side , int tms , int pmc , int pmindex , int StockBm , int *MoveRun ) {
	int Res;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.29
		//
		if ( StockBm != -1 ) return 0;
		//
	}
	//
	if ( pmc != -1 ) {

		while ( TRUE ) {

			_SCH_LOG_DEBUG_PRINTF( run_side , tms + 1 , "TM%d 162 [pmc=%d][pi=%d]\n" , tms + 1 , pmc , pmindex );

			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return -1;

			Res = SCHEDULER_CONTROL_ST6_MOVE_STATUS( run_side , pmc , pmindex , FALSE , 23 );

			if      ( ( Res == SYS_ERROR ) || ( Res == SYS_ABORTED ) ) {
				//
				*MoveRun = -1;
				//
				_SCH_LOG_LOT_PRINTF( run_side , "TM%d Scheduling Aborted 5 Because %s Move Fail%cWHPM%dMOVEFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( pmc ) , 9 , pmc - PM1 + 1 );
				//
				return -1;
			}
			else if ( Res == SYS_SUCCESS ) {

				*MoveRun = -1;

				break;
			}

			CYCLON_TM_SET_LOCKING( run_side , tms , TRUE );

			_SCH_SYSTEM_LEAVE_TM_CRITICAL( tms );

			Sleep(1);

			_SCH_SYSTEM_ENTER_TM_CRITICAL( tms );

			CYCLON_TM_SET_LOCKING( run_side , tms , FALSE );

		}
	}
	//
	return 0;
}

int SCH_GET_MODULE_INFO_TAG[MAX_TM_CHAMBER_DEPTH];


void SCH_GET_MODULE_INFO_TAG_CHECK( int tms , int mode , int bm1 , int bm2 , int *RunBM ) {
	if ( mode == 0 ) {
		if ( SCH_GET_MODULE_INFO_TAG[tms] == 0 ) {
			SCH_GET_MODULE_INFO_TAG[tms] = 1;
			*RunBM = bm1;
		}
		else {
			SCH_GET_MODULE_INFO_TAG[tms] = 0;
			*RunBM = bm2;
		}
	}
	else if ( mode == 1 ) {
		if ( SCH_GET_MODULE_INFO_TAG[tms] == 0 ) {
			*RunBM = bm2;
		}
		else {
			*RunBM = bm1;
		}
	}
	else {
		if ( ( mode % 2 ) == 0 ) {
			*RunBM = bm1;
		}
		else {
			*RunBM = bm2;
		}
	}
}


int SCH_GET_MODULE_INFO( int tms , int mode , int *RunBM , int *pmc , int *StockBm , int *RealBM , int *RealStock , int *stockreturn ) { // 2013.01.29
	int Res , i , s , p , pdis;
	//
	*stockreturn = FALSE;
	//
	*StockBm = Scheduler_BM_Get_First_for_CYCLON( tms );
	//
	*pmc = Scheduler_PM_Get_First_for_CYCLON( tms , FALSE );
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( tms + BM1 , FALSE , -1 ) && _SCH_MODULE_GET_USE_ENABLE( tms + BM1 + 2 , FALSE , -1 ) ) { // 2013.11.22
			//
			if ( ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 ) == BUFFER_TM_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 + 2 ) == BUFFER_TM_STATION ) ) {
				/*
				// 2014.02.03
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( tms + BM1 ) > SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( tms + BM1 + 2 ) ) {
					*RunBM = tms + BM1 + 2;
					Res = 21;
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 , BUFFER_OUTWAIT_STATUS ) < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 + 2 , BUFFER_OUTWAIT_STATUS ) ) {
						*RunBM = tms + BM1 + 2;
						Res = 22;
					}
					else {
						*RunBM = tms + BM1;
						Res = 23;
					}
				}
				*/
				//
				// 2014.02.03
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 , BUFFER_OUTWAIT_STATUS ) < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 + 2 , BUFFER_OUTWAIT_STATUS ) ) {
					//
//					if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms ) == (tms + BM1 + 2) ) { // 2014.07.10
					if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms + BM1 + 2 ) ) { // 2014.07.10
						//
						SCH_GET_MODULE_INFO_TAG_CHECK( tms , mode , tms + BM1 + 2 , tms + BM1 , RunBM );
						//
						Res = 21;
						//
					}
					else {
						SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
						*RunBM = tms + BM1 + 2;
						Res = 21;
					}
					//
				}
				else if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 , BUFFER_OUTWAIT_STATUS ) > SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 + 2 , BUFFER_OUTWAIT_STATUS ) ) {
//					if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms ) == (tms + BM1) ) { // 2014.07.10
					if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms + BM1 ) ) { // 2014.07.10
						//
						SCH_GET_MODULE_INFO_TAG_CHECK( tms , mode , tms + BM1 , tms + BM1 + 2 , RunBM );
						//
						Res = 22;
						//
					}
					else {
						SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
						*RunBM = tms + BM1;
						Res = 22;
					}
				}
				else {
					if ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 , BUFFER_INWAIT_STATUS ) > 0 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( tms + BM1 + 2 , BUFFER_INWAIT_STATUS ) > 0 ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( tms + BM1 ) > SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_ID_STYLE_6( tms + BM1 + 2 ) ) {
							//
//							if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms ) == (tms + BM1 + 2) ) { // 2014.07.10
							if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms + BM1 + 2 ) ) { // 2014.07.10
								//
								SCH_GET_MODULE_INFO_TAG_CHECK( tms , mode , tms + BM1 + 2 , tms + BM1 , RunBM );
								//
								Res = 23;
								//
							}
							else {
								SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
								*RunBM = tms + BM1 + 2;
								Res = 23;
							}
						}
						else {
//							if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms ) == (tms + BM1) ) { // 2014.07.10
							if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( tms + BM1 ) ) { // 2014.07.10
								//
								SCH_GET_MODULE_INFO_TAG_CHECK( tms , mode , tms + BM1 , tms + BM1 + 2 , RunBM );
								//
								Res = 24;
								//
							}
							else {
								SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
								*RunBM = tms + BM1;
								Res = 24;
							}
						}
					}
					else { // 2014.02.03
						//
						SCH_GET_MODULE_INFO_TAG_CHECK( tms , mode , tms + BM1 , tms + BM1 + 2 , RunBM );
						//
						Res = 25;
						//
					}
				}
				//
			}
			else if ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 ) == BUFFER_TM_STATION ) {
				SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
				*RunBM = tms + BM1;
				Res = 24;
			}
			else if ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 + 2 ) == BUFFER_TM_STATION ) {
				SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
				*RunBM = tms + BM1 + 2;
				Res = 25;
			}
			else {
				SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
				*RunBM = tms + BM1;
				Res = 26;
			}
		}
		else if ( _SCH_MODULE_GET_USE_ENABLE( tms + BM1 , FALSE , -1 ) ) { // 2013.11.22
			SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
			//
			*RunBM = tms + BM1;
			Res = 26;
			//
		}
		else if ( _SCH_MODULE_GET_USE_ENABLE( tms + BM1 + 2 , FALSE , -1 ) ) { // 2013.11.22
			SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
			//
			*RunBM = tms + BM1 + 2;
			Res = 25;
			//
		}
		else { // 2013.12.10
			SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
			//
			*RunBM = tms + BM1;
			Res = 27;
			//
		}
	}
	else {
		//
		SCH_GET_MODULE_INFO_TAG[tms] = 0; // 2014.02.03
		//
		*RunBM = tms + BM1;
		Res = 31;
	}
	//
	*RealBM = *RunBM;
	*RealStock = *StockBm;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) {
		//
		if ( *StockBm != -1 ) {
			//
			_SCH_MODULE_Set_BM_FULL_MODE( *StockBm , BUFFER_TM_STATION );
			//
			pdis = SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms );
			//
			//===================================================================================================
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( *StockBm ) ; i++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( *StockBm , i ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( *StockBm , i );
					p = _SCH_MODULE_Get_BM_POINTER( *StockBm , i );
					//
					if ( pdis ) {
						if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) {
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
							SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
							_SCH_MODULE_Set_BM_STATUS( *StockBm , i , BUFFER_OUTWAIT_STATUS );
						}
					}
					else {
						if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
							_SCH_MODULE_Set_BM_STATUS( *StockBm , i , BUFFER_OUTWAIT_STATUS );
						}
					}
				}
			}
			//
			if ( pdis ) {
				//===================================================================================================
				if ( _SCH_MODULE_GET_USE_ENABLE( tms + BM1 , FALSE , -1 ) ) { // 2013.11.22
					//
					if ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 ) == BUFFER_TM_STATION ) {
						//===================================================================================================
						for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( tms + BM1 ) ; i++ ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( tms + BM1 , i ) > 0 ) {
								//
								s = _SCH_MODULE_Get_BM_SIDE( tms + BM1 , i );
								p = _SCH_MODULE_Get_BM_POINTER( tms + BM1 , i );
								//
								if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) {
									_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
									SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
									_SCH_MODULE_Set_BM_STATUS( tms + BM1 , i , BUFFER_OUTWAIT_STATUS );
								}
							}
						}
						//===================================================================================================
					}
					//
				}
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( tms + BM1 + 2 , FALSE , -1 ) ) { // 2013.11.22
					if ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 + 2 ) == BUFFER_TM_STATION ) {
						//===================================================================================================
						for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( tms + BM1 + 2 ) ; i++ ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( tms + BM1 + 2 , i ) > 0 ) {
								//
								s = _SCH_MODULE_Get_BM_SIDE( tms + BM1 + 2 , i );
								p = _SCH_MODULE_Get_BM_POINTER( tms + BM1 + 2 , i );
								//
								if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) {
									_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
									SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
									_SCH_MODULE_Set_BM_STATUS( tms + BM1 + 2 , i , BUFFER_OUTWAIT_STATUS );
								}
							}
						}
						//===================================================================================================
					}
					//
				}
				//===================================================================================================
			}
			//
			if ( SCH_CYCLON2_GROUP_STOCK_POSSIBLE( tms , *pmc , *StockBm , SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( *StockBm , FALSE , TRUE , FALSE ) ) > 0 ) {
				//
				*RunBM = *StockBm;
				//
				*StockBm = -1;
				//
				Res = 11;
			}
			else {
				if ( *pmc == -1 ) {
					//
					if ( SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( tms , TRUE , TRUE , -1 ) ) {
						//
						*stockreturn = TRUE;
						//
						Res = 12;
						//
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( tms ) ) {
							if ( !SCHEDULER_CONTROL_Chk_PM_WAFER_NONE( tms ) ) {
								//
								*RunBM = *StockBm;
								//
								*StockBm = -1;
								//
								*pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
								//
								Res = 13;
								//
							}
						}
					}
					//
				}
			}
		}
	}
	//
	return Res;
}


#define MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( ZZZ ) int Scheduler_TM_Running_CYCLON_##ZZZ##_for_Style_6( int run_side ) { \
\
	int RunBm , StockBm , RealBM , RealStock , StockReturn; \
	int pmc = 0; \
\
	int RunBm2 , StockBm2 , RealBM2 , RealStock2 , StockReturn2; \
	int pmc2; \
\
	int notfirst = 0; \
	int prcsing = 0; \
	int pmindex = 0; \
	int MoveRun; \
\
	int Result = 0; \
\
	int NextProcess_pmc; \
	int NextProcess_side; \
	int NextProcess_pt; \
	int NextProcess_firstnotuse; \
\
	int gck; \
\
	_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 100 [pmc=%d] START\n" , ##ZZZ## + 1 ,  pmc ); /* 2018.10.02 */ \
\
	if ( CYCLON_TM_ACT_LOCKED( run_side , ##ZZZ## ) ) return SYS_SUCCESS; /* 2012.09.03 */ \
\
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return SYS_ABORTED; \
\
	_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 101 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 ,  pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
	MoveRun = -1; \
	NextProcess_pmc = 0; \
	notfirst = FALSE; \
\
	pmindex = -1; \
\
	Result = SCH_GET_MODULE_INFO( ##ZZZ## , 0 , &RunBm , &pmc , &StockBm , &RealBM , &RealStock , &StockReturn ); \
\
\
	SCH_CYCLON2_Set_MDL_SELECT( ##ZZZ## , pmc , RunBm , StockBm , Result ); \
\
\
	_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 102 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d][RunBm=%d][StockBm=%d][RealBM=%d][RealStock=%d][StockReturn=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result , RunBm , StockBm , RealBM , RealStock , StockReturn ); \
\
\
\
\
	Result = Sch_Check_TM_BM_PreCheck( run_side , ##ZZZ## , RunBm , &pmc , ( RunBm == RealStock ) , StockBm , RealStock , StockReturn ); \
\
	if ( Result == -1 ) return SYS_ABORTED; \
	if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
	_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 112 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
\
	Result = Sch_Check_TM_PLACE_BM_Unload_Run( run_side , TRUE , ##ZZZ## , RunBm , pmc , StockBm , RealBM , RealStock , &prcsing , &MoveRun ); \
\
	if ( Result == -1 ) return SYS_ABORTED; \
	if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
\
\
\
\
\
\
\
	Result = Sch_Check_TM_PM_BM_Pre_Cond_W_Check( run_side , ##ZZZ## , RunBm , pmc , StockBm , ( RunBm == RealStock ) , &MoveRun , &notfirst , &pmindex ); \
\
	if ( Result == -1 ) return SYS_ABORTED; \
	if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
	gck = 1; \
\
	while ( TRUE ) { \
\
		if ( _SCH_MACRO_FM_FINISHED_CHECK( run_side , TRUE ) ) return SYS_SUCCESS; \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 131 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		_SCH_MACRO_INTERRUPT_PART_CHECK( run_side , TRUE , -1 ); \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 132 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		if ( StockBm != -1 ) { \
\
			if ( SCH_CYCLON2_GROUP_STOCK_POSSIBLE( ##ZZZ## , pmc , StockBm , SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( StockBm , FALSE , TRUE , FALSE ) ) > 0 ) { \
\
				if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( ##ZZZ## , TRUE , FALSE , -1 ) ) { \
					return SYS_SUCCESS; \
				} \
			} \
\
		} \
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 133 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		SCH_GET_MODULE_INFO( ##ZZZ## , gck , &RunBm2 , &pmc2 , &StockBm2 , &RealBM2 , &RealStock2 , &StockReturn2 ); \
\
		gck++; \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 134 [pmc2=%d][RunBm2=%d][StockBm2=%d][RealBM2=%d][RealStock2=%d][StockReturn2=%d]\n" , ##ZZZ## + 1 , pmc2 , RunBm2 , StockBm2, RealBM2 , RealStock2 , StockReturn2 ); \
\
		if ( RunBm2   != RunBm   ) return SYS_SUCCESS; \
\
		if ( pmc2 != -1 ) { /* 2013.02.15 */ \
			if ( pmc2 != pmc     ) return SYS_SUCCESS; \
		} \
\
		if ( StockBm2 != StockBm ) return SYS_SUCCESS; \
\
		Result = Sch_Check_TM_PICK_ST_OUT_Unload_Run( run_side , ##ZZZ## , RealStock , RealBM , pmc , pmindex ); /* 2013.04.11 */ \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 141 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return SYS_ABORTED; \
\
\
\
\
\
\
\
		Result = Sch_Check_TM_PICK_BM_Load_Run( run_side , ##ZZZ## , RunBm , pmc , StockBm , ( RunBm == RealStock ) , StockReturn , &MoveRun ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 151 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result  ); \
\
\
		if ( !notfirst ) { \
\
			notfirst = TRUE; \
\
\
\
\
			Result = Sch_Check_TM_PM_BM_GO_TMSIDE_W_Check1( run_side , ##ZZZ## , RunBm , pmc , StockBm , ( RunBm == RealStock ) , &MoveRun , &notfirst , &pmindex ); \
\
			if ( Result == -1 ) return SYS_ABORTED; \
			if ( Result ==  1 ) return SYS_SUCCESS; \
			if ( Result ==  2 ) continue; \
\
\
\
\
\
\
		} \
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 161 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
\
		Result = Sch_Check_TM_PM_Move_W_Run( run_side , ##ZZZ## , pmc , pmindex , StockBm , &MoveRun ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 171 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
		Result = Sch_Check_TM_PICK_PM_ST_Unload_Run( run_side , ##ZZZ## , StockBm , pmc , pmindex , RealBM , RealStock , StockReturn ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 181 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return SYS_ABORTED; \
\
\
\
\
\
		Result = Sch_Check_TM_PLACE_PM_Load_Run( run_side , ##ZZZ## , RunBm , pmc , pmindex ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 191 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return SYS_ABORTED; \
\
\
\
\
\
\
		prcsing = FALSE; \
\
\
\
		Result = Sch_Check_TM_PM_Process_Run( run_side , ##ZZZ## , RunBm , pmc , StockBm , &MoveRun , &pmindex , &prcsing , &NextProcess_pmc , &NextProcess_side , &NextProcess_pt , &NextProcess_firstnotuse ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( run_side ) ) || ( _SCH_SYSTEM_USING_GET( run_side ) <= 0 ) ) return SYS_ABORTED; \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 211 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
\
		Result = Sch_Check_TM_PLACE_BM_Unload_Run( run_side , FALSE , ##ZZZ## , RunBm , pmc , StockBm , RealBM , RealStock , &prcsing , &MoveRun ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 221 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
\
\
\
\
\
\
		Result = Sch_Check_TM_PM_BM_GO_TMSIDE_Check2( run_side , ##ZZZ## , RunBm , pmc , StockBm , ( RunBm == RealStock ) , prcsing , &MoveRun , NextProcess_pmc , NextProcess_side , NextProcess_pt , NextProcess_firstnotuse ); \
\
		if ( Result == -1 ) return SYS_ABORTED; \
		if ( Result ==  1 ) return SYS_SUCCESS; \
\
\
\
\
\
\
\
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 231 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
\
		if ( pmc == -1 ) break; /* 2008.01.16 */ \
\
		Result = SCHEDULER_MODULE_RUN_ENABLE( run_side , ##ZZZ## , pmc ); /* 2011.04.22 */ \
		if ( Result == 2 ) return SYS_SUCCESS; \
\
		if ( Result == 0 ) break; /* 2008.01.16 */ /* 2008.10.01 */ \
\
		if ( prcsing ) break; \
\
		if ( !_SCH_SYSTEM_USING_RUNNING( run_side ) ) break; \
\
		_SCH_LOG_DEBUG_PRINTF( run_side , ##ZZZ## + 1 , "TM%d 232 [pmc=%d][pmindex=%d][notfirst=%d][prcsing=%d][Result=%d]\n" , ##ZZZ## + 1 , pmc , pmindex , notfirst, prcsing , Result ); \
	} \
	return SYS_SUCCESS; \
}

MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 0 );

MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 1 );
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 2 );
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 4 );
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 6 );
MAKE_SCHEDULER_MAIN_SUB_TM_CYCLON_for_6( 7 );
#endif
#endif

