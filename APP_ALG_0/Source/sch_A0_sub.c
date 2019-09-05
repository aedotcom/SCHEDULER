//================================================================================
#include "INF_default.h"
#include "INF_CimSeqnc.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_default.h"
#include "sch_A0_subBM.h"
#include "sch_A0_param.h"
#include "sch_A0_init.h"
#include "sch_A0_sub.h"
#include "sch_A0_sub_sp2.h"
#include "sch_A0_sub_sp4.h"
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_CommonUser.h"
//================================================================================

#define		CROSS_OVER_SIMPLE_CHECK		TRUE		// 2012.10.30

//================================================================================
extern BOOL MULTI_ALL_PM_FULLSWAP;

BOOL Scheduler_Deadlock_Rounding_Possible_Before_CM_Pick_for_SingleArm( int s , int p ); // 2018.07.12

int SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM_SUB( int s , int p , int WillPathDo , int pickch ); // 2017.09.07

int  SCHEDULER_FM_All_No_More_Supply(); // 2017.02.10
//
BOOL SCHEDULER_FM_Current_No_More_Supply( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Current_No_More_Supply2( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply( int Side , int option ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply2( int Side , int option , int option2 ); // 2014.11.07
BOOL SCHEDULER_FM_FM_Finish_Status( int Side ); // 2014.11.07
int SCHEDULING_CHECK_Enable_Place_To_BM( int TMATM , int Side , int Pointer , int Finger , int *RChamber , int *RSlot , int plcgroup , int bmmode , BOOL MoveOnly , int swbm , int putprfirstch , int retmode , BOOL dtmothersupply , int *incnt );
int SCHEDULING_CHECK_for_Enable_PLACE_TO_BM_FOR_OUT( int TMATM , int side , int pickch , int placech , int swbm , int Count , BOOL MoveOnly ); // 2014.10.07

int  SCHEDULER_CONTROL_Special_Case_Checking( int mode , int tmside , int ch , int Side , int Pointer , int offset , int *stock , BOOL placecheck ); // 2006.04.15
BOOL SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( int , int , BOOL );

BOOL SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( int tmside , int side , int *bmc );

int SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( int s , int p , BOOL whenalldis , BOOL dataonly , int log ); // 2016.02.19

CRITICAL_SECTION CR_SINGLEBM_STYLE_0;

CRITICAL_SECTION CR_MULTI_GROUP_PLACE_STYLE_0[MAX_CHAMBER]; // 2006.02.08 // 2006.03.27
CRITICAL_SECTION CR_MULTI_GROUP_PICK_STYLE_0[MAX_CHAMBER]; // 2006.02.08
CRITICAL_SECTION CR_FEM_PICK_SUBAL_STYLE_0; // 2007.10.04
//
CRITICAL_SECTION CR_FEM_TM_EX_STYLE_0; // 2006.06.26
CRITICAL_SECTION CR_MULTI_GROUP_TMS_STYLE_0[MAX_TM_CHAMBER_DEPTH]; // 2013.03.19

CRITICAL_SECTION CR_FEM_TM_DENYCHECK_STYLE_0; // 2014.12.26

//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
BOOL SCH_PM_Check_Impossible_Place_MultiPM( int Side , int Pointer , int pm ) { // 2016.05.30
	int clx , i , s , p;
	//
	clx = _SCH_CLUSTER_Get_ClusterIndex( Side , Pointer );
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( pm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_PM_WAFER( pm,i ) <= 0 ) continue;
		if ( _SCH_MODULE_Get_PM_PICKLOCK( pm,i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_PM_SIDE( pm,i );
		p = _SCH_MODULE_Get_PM_POINTER( pm,i );
		//
		if ( clx != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) return TRUE;
	}
	//
	return FALSE;
}


/*
//
// 2018.12.06
//
BOOL SCH_PM_GET_FIRST_SIDE_PT( int ch , int *s , int *p ) { // 2014.01.10
	int i;
	switch( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 0 ) ) { // 2018.04.03
	case 0 : // F-IN F-OUT
	case 1 : // F-IN L-OUT
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			*s = _SCH_MODULE_Get_PM_SIDE( ch , i );
			*p = _SCH_MODULE_Get_PM_POINTER( ch , i );
			//
			return TRUE;
		}
		break;
	default :
		for ( i = ( _SCH_PRM_GET_MODULE_SIZE( ch ) - 1 ) ; i >= 0 ; i-- ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			*s = _SCH_MODULE_Get_PM_SIDE( ch , i );
			*p = _SCH_MODULE_Get_PM_POINTER( ch , i );
			//
			return TRUE;
		}
		break;
	}
	return FALSE;
}
*/
//
//
// 2018.12.06
//
BOOL SCH_PM_GET_FIRST_SIDE_PT( int ch , int *s , int *p ) { // 2014.01.10
	int i , sl , pr;
	//
	sl = -1;
	//
	switch( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 0 ) ) { // 2018.04.03
	case 0 : // F-IN F-OUT
	case 1 : // F-IN L-OUT
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			if ( sl == -1 ) {
				sl = i;
				pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 ) < _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , sl + 1 ) ) {
					sl = i;
					pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
				}
			}
			//
		}
		//
		break;
		//
	default :
		for ( i = ( _SCH_PRM_GET_MODULE_SIZE( ch ) - 1 ) ; i >= 0 ; i-- ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			if ( sl == -1 ) {
				sl = i;
				pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 ) < _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , sl + 1 ) ) {
					sl = i;
					pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
				}
			}
			//
		}
		//
		break;
	}
	//
	if ( sl == -1 ) return FALSE;
	//
	*s = _SCH_MODULE_Get_PM_SIDE( ch , sl );
	*p = _SCH_MODULE_Get_PM_POINTER( ch , sl );
	//
	return TRUE;
}

BOOL SCH_PM_IS_EMPTY( int ch ) { // 2014.01.10
	int i;
	//
	BOOL hasempty = FALSE; // 2018.04.03
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) return FALSE;
		//
		if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
		//
		hasempty = TRUE; // 2018.04.03
		//
	}
	//
	return hasempty;
}

BOOL SCH_PM_IS_WILL_EMPTY( int ch , int pickslot ) { // 2014.01.27
	int i;
	//
	BOOL hasempty = FALSE; // 2018.04.03
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		//
		if ( i == pickslot ) {
			//
			hasempty = TRUE; // 2018.05.03
			//
			continue;
		}
		//
		if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) return FALSE;
		//
		if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
		//
		hasempty = TRUE; // 2018.04.03
		//
	}
	//
	return hasempty;
}

//
/*
//
// 2018.12.06
//
BOOL SCH_PM_HAS_PICKING( int ch , int *depth ) { // 2014.01.10
	int i;
	switch( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 0 ) ) { // 2018.04.03
	case 0 : // F-IN F-OUT
	case 2 : // L-IN F-OUT
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) continue;
				*depth = i;
				return TRUE;
			}
		}
		break;
	default :
		for ( i = ( _SCH_PRM_GET_MODULE_SIZE( ch ) - 1 ) ; i >= 0 ; i-- ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) continue;
				*depth = i;
				return TRUE;
			}
		}
		break;
	}
	//
	return FALSE;
}
//
*/
//
//
// 2018.12.06
//
BOOL SCH_PM_HAS_PICKING( int ch , int *depth ) { // 2014.01.10
	int i , sl , pr;
	//
	sl = -1;
	//
	switch( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 0 ) ) { // 2018.04.03
	case 0 : // F-IN F-OUT
	case 2 : // L-IN F-OUT
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) continue;
			//
			if ( sl == -1 ) {
				sl = i;
				pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 ) < _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , sl + 1 ) ) {
					sl = i;
					pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
				}
			}
			//
		}
		//
		break;
		//
	default :
		for ( i = ( _SCH_PRM_GET_MODULE_SIZE( ch ) - 1 ) ; i >= 0 ; i-- ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) continue;
			//
			if ( sl == -1 ) {
				sl = i;
				pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 ) < _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , sl + 1 ) ) {
					sl = i;
					pr = _SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( ch , i + 1 );
				}
			}
			//
		}
		//
		break;
	}
	//
	if ( sl == -1 ) return FALSE;
	//
	*depth = sl;
	//
	return TRUE;
}
//
//
BOOL SCH_PM_HAS_ONLY_PICKING( int ch , int *depth ) { // 2014.01.10
	if ( !SCH_PM_IS_FREE( ch ) && ( SCH_PM_HAS_PICKING( ch , depth ) ) ) return TRUE;
	return FALSE;
}

BOOL SCH_PM_IS_PICKING( int ch ) { // 2014.01.10
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) continue;
			return TRUE;
		}
	}
	//
	return FALSE;
}

BOOL SCH_PM_IS_PLACING( int ch ) { // 2018.10.20
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) return TRUE;
		}
	}
	//
	return FALSE;
}


BOOL SCH_PM_HAS_PLACING_EXPECT( int ch , int s , int p , int d ) { // 2018.10.20
	int i , l;

	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
		return TRUE;
	}
	else {
		//
		if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE; // 2018.12.06
		//
		if ( s < 0 ) {
			l = 0;
		}
		else {
			//
			if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , d ) == NULL ) {
				l = 0;
			}
			else {
				l = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , d , (ch - PM1) );
			}
		}
		//
		if ( ( l > 0 ) && ( l <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , l - 1 ) <= 0 ) return TRUE;
		}
		else {
			for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				//
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
				//
				if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) return TRUE;
				//
			}
		}
	}
	//
	return FALSE;
}

/*
int c_slot( int p ) {
	switch( p ) {
	case 0 :	return 3;
	case 1 :	return 2;
	case 2 :	return 1;
	case 3 :	return 3;
	case 4 :	return 2;
	case 5 :	return 1;
	case 6 :	return 3;
	case 7 :	return 2;
	case 8 :	return 1;
	case 9 :	return 3;
	case 10 :	return 2;
	case 11 :	return 1;
	case 12 :	return 3;
	case 13 :	return 2;
	case 14 :	return 1;
	case 15 :	return 3;
	case 16 :	return 2;
	case 17 :	return 1;
	case 18 :	return 3;
	case 19 :	return 2;
	case 20 :	return 1;
	case 21 :	return 3;
	case 22 :	return 2;
	case 23 :	return 1;
	case 24 :	return 3;
	case 25 :	return 2;
	case 26 :	return 1;
	case 27 :	return 3;
	case 28 :	return 2;
	case 29 :	return 1;
	}
	return 0;
}
*/

BOOL SCH_PM_HAS_ABSENT( int ch , int s , int p , int d , int *depth ) { // 2014.01.10
	int i , l;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) {
			//
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 1 ) ) { // 2018.04.03
				//
				*depth = 0;
				return TRUE;
				//
			}
			//
		}
	}
	else {
		//
		if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , d ) == NULL ) {
			l = 0;
//l = c_slot( p );
		}
		else {
//			l = _SCH_CLUSTER_Get_PathProcessDepth( s , p , d )[ ch - PM1 ]; // 2015.04.10
			l = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , d , (ch - PM1) ); // 2015.04.10
		}
		//
		if ( ( l > 0 ) && ( l <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , l-1 ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , l ) ) { // 2018.04.03
					//
					*depth = l-1;
					return TRUE;
					//
				}
				//
			}
		}
		else {
			switch( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 0 ) ) { // 2018.04.03
			case 0 : // F-IN F-OUT
			case 1 : // F-IN L-OUT
				for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
						//
						*depth = i;
						return TRUE;
					}
				}
				break;
			default :
				for ( i = ( _SCH_PRM_GET_MODULE_SIZE( ch ) - 1 ) ; i >= 0 ; i-- ) {
					if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
						//
						*depth = i;
						return TRUE;
					}
				}
				break;
			}
		}
	}
	//
	return FALSE;
}

BOOL SCH_PM_IS_ABSENT( int ch , int s , int p , int d ) { // 2014.01.10
	int i , l;
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) {
			//
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 1 ) ) { // 2018.04.03
				//
				return TRUE;
				//
			}
			//
		}
	}
	else {
		//
		if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , d ) == NULL ) {
			l = 0;
//l = c_slot( p );
		}
		else {
//			l = _SCH_CLUSTER_Get_PathProcessDepth( s , p , d )[ ch - PM1 ]; // 2015.04.10
			l = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , d , ( ch - PM1 ) ); // 2015.04.10
		}
		//
		if ( ( l > 0 ) && ( l <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , l-1 ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , l ) ) { // 2018.04.03
					//
					return TRUE;
					//
				}
				//
			}
		}
		else {
			for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
					//
					return TRUE;
				}
			}
		}
		//
	}
	//
	return FALSE;
}


BOOL SCH_PM_IS_ABSENT2( int ch , int s , int p , int d , BOOL *WaitingMultiwafer ) { // 2018.05.08
	int i , l , f;
	//
	*WaitingMultiwafer = FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) {
			//
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , 1 ) ) {
				//
				return TRUE;
				//
			}
			//
		}
	}
	else {
		//
		if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , d ) == NULL ) {
			l = 0;
		}
		else {
			l = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , d , ( ch - PM1 ) );
		}
		//
		if ( ( l > 0 ) && ( l <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , l-1 ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , l ) ) { // 2018.04.03
					//
					return TRUE;
					//
				}
				//
			}
			//
			// 2018.10.20
			//
			for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
					if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) != 0 ) {
						*WaitingMultiwafer = TRUE;
						break;
					}
				}
			}
			//
		}
		else {
			//
			f = 0;
			//
			for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
					//
					f++;
					//
				}
				else {
					if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) != 0 ) *WaitingMultiwafer = TRUE;
				}
			}
			//
			if ( f > 0 ) return TRUE;
			//
		}
		//
	}
	//
	return FALSE;
}


BOOL SCH_PM_IS_FREE( int ch ) { // 2014.01.10
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
			//
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03
			//
			return TRUE;
		}
	}
	//
	return FALSE;
}


long SCH_PM_TIMER[MAX_CHAMBER];

BOOL SCH_PM_IS_ALL_PLACED_PROCESSING( int ch , BOOL Normal_After_Place , BOOL defwait , BOOL *multiprcs ) { // 2014.01.10
	int i , l , c;
	//
	int notuse[MAX_PM_SLOT_DEPTH]; // 2018.10.20
	//
	//
	// 2018.10.20
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		notuse[i] = _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 );
	}
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 8 ) == 1 ) {
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) <= 0 ) continue;
			//
			if ( notuse[i] ) return FALSE;
			//
		}
	}
	//
	//
	if ( Normal_After_Place ) {
		//
		*multiprcs = ( _SCH_PRM_GET_MODULE_SIZE( ch ) > 1 );
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) {
				//
//				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) continue; // 2018.04.03 // 2018.10.20
				if ( notuse[i] ) continue; // 2018.04.03 // 2018.10.20
				//
				return FALSE;
				//
			}
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) <= 0 ) return FALSE;
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) { // 2018.10.20
			if ( notuse[i] != _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) return FALSE;
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 0 );
		}
		//
	}
	else {
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) { // 2018.10.20
			if ( notuse[i] != _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) return FALSE;
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			//
//			if ( *multiprcs ) { // 2018.12.19
			if ( *multiprcs && !defwait ) { // 2018.12.19
				_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 0 );
			}
			else {
				//
				c = _SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME();
				//
				if ( defwait ) { // 2018.12.07
					if ( c < 30 ) c = 30;
				}
				//
				if ( c > 0 ) {
					//
					l = _SCH_MODULE_Get_PM_PICKLOCK( ch , i );
					//
					if      ( l == 1 ) {
						SCH_PM_TIMER[ch] = GetTickCount();
						_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 2 );
					}
					else if ( l == 2 ) {
						if ( (signed) ( GetTickCount() - SCH_PM_TIMER[ch] ) >= ( c * 1000 ) ) {
//							_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 0 ); // 2016.05.30
							_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 3 ); // 2016.05.30
						}
					}
					//
				}
				else { // 2018.05.08
					//
					l = _SCH_MODULE_Get_PM_PICKLOCK( ch , i );
					//
					if      ( l == 1 ) {
						_SCH_MODULE_Set_PM_PICKLOCK( ch , i , 3 );
					}
				}
				//
			}
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
//			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) return FALSE; // 2016.05.30
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) != 3 ) return FALSE;
			}
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) { // 2016.05.30
			if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , i ) == 3 ) _SCH_MODULE_Set_PM_PICKLOCK( ch , i , 0 );
		}
		//
	}
	return TRUE;
}

//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================
//================================================================================================================================================================================

BOOL SCHEDULING_ThisIs_BM_OtherChamber( int ch , int mode ) { // 2009.01.21
	if ( ( mode == 0 ) || ( mode == 1 ) ) {
		if ( ch == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE;
	}
	if ( ( mode == 0 ) || ( mode == 2 ) ) {
		if ( ch == _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE;
	}
	if ( ( ch >= BM1 ) && ( ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE ) return TRUE;
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE2 ) return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2012.09.21
BOOL SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( int mode , int newch , int oldch ) {
	// mode
	// 0 : BUFFER_FM_STATION
	// 1 : BUFFER_TM_STATION
	//
	if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( newch ) ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( oldch ) ) return FALSE;
	//
	if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO2 ) { // 2012.12.05
		if ( mode == 0 ) {
			if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 0/1:오랬동안 사용하지 않은 BM부터 Place
				return FALSE;
			}
		}
	}
	//
	if ( _SCH_TIMER_GET_PROCESS_TIME_START( mode , newch ) < _SCH_TIMER_GET_PROCESS_TIME_START( mode , oldch ) ) {
		return TRUE;
	}
	//
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_Last_Finish_Status_TM_0( int side , int SchPointer , int finc ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2004.06.02
		if ( SchPointer != i ) { // 2004.06.02
			if ( _SCH_CLUSTER_Get_PathDo( side , i ) == PATHDO_WAFER_RETURN ) return FALSE; // 2004.06.02
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) > _SCH_CLUSTER_Get_PathDo( side , i ) ) return FALSE; // 2004.06.02
		}
	} // 2004.06.02
	if ( ( _SCH_MODULE_Get_TM_InCount( side ) + finc ) >= _SCH_MODULE_Get_TM_OutCount( side ) ) { // 2005.06.28
		return TRUE;
	}
	return FALSE;
}
//

//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
int SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() { // 2007.10.07
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 4 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 5 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 6 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 7 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 8 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 9 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 10 ) return 3; // 2015.10.27
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 11 ) return 3; // 2015.10.27
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 12 ) return 3; // 2015.10.27
	return 0;
}


int SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() { // 2007.10.07
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 2 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 5 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 8 ) return 1;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 3 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 6 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 9 ) return 2;
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 11 ) return 1; // 2015.10.27
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 12 ) return 2; // 2015.10.27
	return 0;
}

BOOL SCHEDULER_CONTROL_Check_Process_2Module_Skip( int ch ) { // 2015.11.10
	//
	if ( ( ( ch - PM1 ) % 2 ) == 0 ) return FALSE;
	//
	switch( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) {
	case 4 :
	case 5 :
	case 6 :
	case 10 :
	case 11 :
	case 12 :
		return TRUE;
	}
	//
	return FALSE;
}


//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_PathProcessChamber( int s , int p , int pd , int k ) { // 2015.11.10
	int m;
	//
	m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , pd , k );
	//
	if ( m > 0 ) {
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( m ) ) return 0; // 2015.11.10
	}
	//
	return m;
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Check_2Module_OK( int ch , BOOL mode , int side ) { // 2008.03.18
	if ( !_SCH_MODULE_GET_USE_ENABLE( ch , mode , side ) ) return -1;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) return 0;
	return 1;
}

BOOL SCHEDULER_CONTROL_Check_2Module_OverTime_Wait( int ch ) { // 2008.03.18
	long l;
	time_t finish;
	if ( _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( ch ) <= 0 ) return FALSE;
	//
	time( &finish );
	l = (long) difftime( finish , _SCH_TIMER_GET_PROCESS_TIME_END( 0 , ch ) );
	if ( l <= 0 ) return FALSE;
	//
	if ( _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( ch ) > l ) return TRUE;
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Check_2Module_OverTime_Gap( int ch , int finish_ch ) { // 2008.09.26
	long l;
	time_t finish;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) return FALSE;
	if ( _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( ch ) <= 0 ) return FALSE;
	//
	time( &finish );
	l = (long) difftime( finish , _SCH_TIMER_GET_PROCESS_TIME_END( 1 , finish_ch ) );
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) return FALSE;
	//
	if ( l <= 0 ) return FALSE;
	//
	if ( _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( ch ) <= l ) return TRUE;
	//
	return FALSE;
}

void SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( BOOL check , int side , int tms , int arm , int *ch , int *halfret ) { // 2007.10.29
	int res , res2;
	BOOL sep; // 2016.05.03
	//
	*halfret = -1;
	//
	if ( *ch == 0 ) return;
	//
	sep = FALSE; // 2016.05.03
	//
	switch ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) {
	case 1 : // 2008.04.03
//	case 3 : // 2010.10.27 // 2016.05.03
		//
		if ( ( ( *ch - PM1 ) % 2 ) != 0 ) *ch = 0; // 2015.11.10
		//
		return;
		break;
		//
	case 2 :
		sep = TRUE; // 2016.05.03
		break;

	case 3 : // 2016.05.03
		break;

	default :
		return;
	}
	if ( *ch > 0 ) {
		if ( ( ( *ch - PM1 ) % 2 ) != 0 ) { // pm2
			//
			if ( !sep ) { // 2016.05.03
				if ( check ) {
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) <= 0 ) {
						*ch = 0;
						return;
					}
				}
				*ch = *ch - 1;
			}
			else {
				//=========================================================================
				res = SCHEDULER_CONTROL_Check_2Module_OK( *ch - 1 , FALSE , side );
				res2 = SCHEDULER_CONTROL_Check_2Module_OK( *ch , FALSE , side );
				//
				if ( ( ( res >= 0 ) && ( res2 < 0 ) ) || ( ( res < 0 ) && ( res2 >= 0 ) ) ) {
					//
					Sleep(1);
					//
					res = SCHEDULER_CONTROL_Check_2Module_OK( *ch - 1 , FALSE , side );
					res2 = SCHEDULER_CONTROL_Check_2Module_OK( *ch , FALSE , side );
					//
				}
				//--------------------------------------------------------------
				// 2008.10.15
				//--------------------------------------------------------------
				if ( check ) {
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) <= 0 ) { // 2008.10.23
						*ch = 0;
						return;
					}
					if ( ( res == 0 ) && ( res2 == 1 ) ) {
						if ( SCHEDULER_CONTROL_Check_2Module_OverTime_Gap( *ch - 1 , *ch ) ) {
							res = -1;
						}
					}
					//
					/*
					// 2008.10.23
					if ( ( res == 0 ) && ( res2 == 1 ) ) {
						if ( SCHEDULER_CONTROL_Check_2Module_OverTime_Gap( *ch - 1 , *ch ) ) {
							res = -1;
						}
						else {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) <= 0 ) {
								*ch = 0;
								return;
							}
						}
					}
					else {
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) <= 0 ) {
							*ch = 0;
							return;
						}
					}
					*/
				}
				//--------------------------------------------------------------
				//
				if ( res == -1 ) { // disable
					//
					if ( res2 == -1 ) { // disable
						*ch = 0;
						return;
					}
					else {
						//
						if ( check && ( res2 == 0 ) ) { *ch = 0; return; } // 2008.03.25
						//
						if ( check ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
								*halfret = 1;
							}
						}
						return;
					}
				}
				else {
	//				if ( check && ( res  == 0 ) ) { *ch = 0; return; } // 2008.03.25 // 2008.09.26
					if ( check && ( res2 == 0 ) ) { *ch = 0; return; } // 2008.03.25
					//
					if ( check && ( res == 0 ) ) { // 2008.09.26
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
							if ( ( ( _SCH_MODULE_Get_PM_WAFER( *ch - 1 , 0 ) % 100 ) <= 0 ) &&( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) % 100 ) <= 0 ) ) {
								if ( !SCHEDULER_CONTROL_Check_2Module_OverTime_Gap( *ch - 1 , *ch ) ) {
									*ch = 0;
									return;
								}
							}
						}
					}
					//
					if (
						( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) <= 0 ) &&
						( ( _SCH_MODULE_Get_PM_WAFER( *ch - 1 , 0 ) / 100 ) <= 0 ) &&
						( ( _SCH_MODULE_Get_PM_WAFER( *ch - 1 , 0 ) % 100 ) > 0 )
						) {
						//=========================================================================
						if ( check ) { // 2008.01.24
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
								*halfret = 1;
							}
							if ( res == 0 ) { // 2008.09.26
								*halfret = 1;
							}
						}
						//=========================================================================
						return;
					}
					//
					if ( check ) { // 2008.01.07
						if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) / 100 ) > 0 ) {
							*ch = 0;
							return;
						}
						if ( ( _SCH_MODULE_Get_PM_WAFER( *ch - 1 , 0 ) / 100 ) > 0 ) {
							*ch = 0;
							return;
						}
						//=========================================================================
						if (
							( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) % 100 ) <= 0 ) &&
							( ( _SCH_MODULE_Get_PM_WAFER( *ch - 1 , 0 ) % 100 ) <= 0 )
							) { // 2008.09.26
							if ( res == 0 ) {
								if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
									*halfret = 1;
								}
								return;
							}
						}
						//=========================================================================
					}
					//
				}
				//=========================================================================
				*ch = *ch - 1;
			}
		}
		else { // pm1
			//
			if ( !sep ) return; // 2016.05.03
			//
			res = SCHEDULER_CONTROL_Check_2Module_OK( *ch + 1 , FALSE , side );
			//
			if ( res == -1 ) { // 2008.03.18
				if ( check ) {
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) > 0 ) {
						*halfret = 2;
					}
				}
			}
			else { // 2008.01.10
				//
//				if ( check && ( res == 0 ) ) { *ch = 0; return; } // 2008.03.25 // 2008.09.26
				//
				if ( check && ( res == 0 ) ) { // 2008.09.26
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) > 0 ) {
						if ( ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) / 100 ) <= 0 ) && ( ( _SCH_MODULE_Get_PM_WAFER( *ch + 1 , 0 ) / 100 ) <= 0 ) ) {
							if ( !SCHEDULER_CONTROL_Check_2Module_OverTime_Gap( *ch + 1 , *ch ) ) {
								*ch = 0;
								return;
							}
							else { // 2008.10.15
								*halfret = 2;
								return;
							}
						}
					}
				}
				//
				if ( check ) {
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) > 0 ) {
						if ( res == 0 ) { // 2008.09.26 prcsing
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) % 100 ) > 0 ) {
									*ch = 0;
									return;
								}
								*halfret = 2;
							}
							else {
								*ch = 0;
								return;
							}
						}
						else {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) % 100 ) > 0 ) {
									*ch = 0;
									return;
								}
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) / 100 ) > 0 ) {
									*halfret = 2;
								}
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch + 1 , 0 ) / 100 ) > 0 ) {
									*halfret = 2;
								}
							}
							else {
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) / 100 ) > 0 ) {
									*ch = 0;
									return;
								}
								if ( ( _SCH_MODULE_Get_PM_WAFER( *ch + 1 , 0 ) / 100 ) > 0 ) {
									*ch = 0;
									return;
								}
							}
						}
					}
					else {
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_PM_WAFER( *ch , 0 ) % 100 ) > 0 ) {
								*ch = 0;
								return;
							}
						}
						else {
							*ch = 0;
							return;
						}
					}
				}
			}
		}
	}
}


void SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( int *ch ) { // 2007.10.29
	BOOL h;
	SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( FALSE , -1 , 0 , 0 , ch , &h );
}


BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_Pick_PM( int side , int ch , int *halfpick ) { // 2007.10.29
	//
	if ( ch <= 0 ) return FALSE;
	//
	/*
	// 2015.11.10
	//
	switch ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) {
	case 0 :
	case 1 :
	case 2 :
	case 3 :
	case 4 : // 2008.04.03
	case 5 : // 2008.04.03
	case 6 : // 2008.04.03
	case 10 : // 2015.10.27
	case 11 : // 2015.10.27
	case 12 : // 2015.10.27
		return TRUE;
		break;
	}
	//
	*/
	//
	// 2015.11.10
	//
	switch ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) {
	case 1 : // 2008.04.03
	case 3 : // 2015.10.27
		//
		if ( ( ( ch - PM1 ) % 2 ) != 0 ) return FALSE; // 2015.11.10
		//
		return TRUE;
		break;
	case 2 :
		break;
	default :
		return TRUE;
		break;
	}
	//
	//
	if ( ( ( ch - PM1 ) % 2 ) != 0 ) { // PM2
		//===================================================================================
		// 2007.11.09
		//===================================================================================
		if (
			( ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) / 100 ) > 0 ) &&
			( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 )
			) {
			return TRUE;
		}
		else if (
			( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) <= 0 ) &&
			( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 )
			) {
			return TRUE;
		}
		else if ( // 2008.01.10
			( ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) % 100 ) > 0 ) &&
			( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 )
			) {
			return TRUE;
		}
		else {
			if (
				( ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) / 100 ) > 0 ) &&
				( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) <= 0 )
				) {
				if ( SCHEDULER_CONTROL_Check_2Module_OK( ch - 1 , TRUE , side ) != 1 ) { // 2008.03.18
//				if ( !_SCH_MODULE_GET_USE_ENABLE( ch - 1 , TRUE , side ) ) { // 2008.03.18
					//===============================================================================================
					// Separating
					//===============================================================================================
					if ( ( ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) % 100 ) > 0 ) ) {
						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , _SCH_MODULE_Get_PM_SIDE( ch - 1 , 1 ) , _SCH_MODULE_Get_PM_POINTER( ch - 1 , 1 ) , 0 , 0 );
					}
					else {
						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , _SCH_MODULE_Get_PM_SIDE( ch - 1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( ch - 1 , 0 ) , 0 , 0 );
					}
					//===============================================================================================
					_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , _SCH_MODULE_Get_PM_PATHORDER( ch - 1 , 0 ) );
					_SCH_MODULE_Set_PM_WAFER( ch , 0 , ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) / 100 ) * 100 );
					//===============================================================================================
//					_SCH_MODULE_Inc_TM_OutCount( _SCH_MODULE_Get_PM_SIDE( ch , 0 ) ); // 2008.06.10
					//===============================================================================================
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	else { // PM1
		//===================================================================================
		// 2007.11.09
		//===================================================================================
		if ( SCHEDULER_CONTROL_Check_2Module_OK( ch + 1 , TRUE , side ) != 1 ) { // 2008.03.18
//		if ( !_SCH_MODULE_GET_USE_ENABLE( ch + 1 , TRUE , side ) ) { // 2008.03.18
			if (
				( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 ) &&
				( ( _SCH_MODULE_Get_PM_WAFER( ch + 1 , 0 ) / 100 ) <= 0 )
				) {
				//===============================================================================================
				*halfpick = TRUE;
				//===============================================================================================
				// Separating
				//===============================================================================================
				if ( ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) % 100 ) > 0 ) ) {
					_SCH_MODULE_Set_PM_SIDE_POINTER( ch + 1 , _SCH_MODULE_Get_PM_SIDE( ch , 1 ) , _SCH_MODULE_Get_PM_POINTER( ch , 1 ) , 0 , 0 );
				}
				else {
					_SCH_MODULE_Set_PM_SIDE_POINTER( ch + 1 , _SCH_MODULE_Get_PM_SIDE( ch , 0 ) , _SCH_MODULE_Get_PM_POINTER( ch , 0 ) , 0 , 0 );
				}
				//===============================================================================================
				_SCH_MODULE_Set_PM_PATHORDER( ch + 1 , 0 , _SCH_MODULE_Get_PM_PATHORDER( ch , 0 ) );
				_SCH_MODULE_Set_PM_WAFER( ch + 1 , 0 , ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) * 100 );
				//===============================================================================================
//				_SCH_MODULE_Inc_TM_OutCount( _SCH_MODULE_Get_PM_SIDE( ch + 1 , 0 ) ); // 2008.06.10
				//===============================================================================================
			}
			else {
				if (
					( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 ) &&
					( ( _SCH_MODULE_Get_PM_WAFER( ch + 1 , 0 ) / 100 ) > 0 )
					) {
					//===============================================================================================
					if ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) % 100 ) <= 0 ) return FALSE;
					//===============================================================================================
					*halfpick = TRUE;
					//===============================================================================================
				}
			}
		}
		else {
			if (
				( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) > 0 ) &&
				( ( _SCH_MODULE_Get_PM_WAFER( ch + 1 , 0 ) / 100 ) > 0 )
				) {
				//===============================================================================================
				*halfpick = TRUE;
				//===============================================================================================
			}
		}
		//===================================================================================
	}
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PROCESS( int side , int ch , int *m ) { // 2007.10.29
	//
	/*
	// 2013.05.06
	switch ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) {
	case 0 :
	case 1 :
	case 2 :
	case 3 :
	case 4 : // 2008.04.03
	case 5 : // 2008.04.03
	case 6 : // 2008.04.03
		*m = -2;
		return ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 );
		break;
	}
	*/
	//
	// 2013.05.06
	switch ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) {
	case 1 :
	case 3 : // 2015.10.27
		//
		*m = -3;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) return TRUE;
		if ( ( ( ch - PM1 ) % 2 ) == 0 ) { // PM1
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + 1 ) > 0 ) return TRUE;
		}
		else {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch - 1 ) > 0 ) return TRUE;
		}
		return FALSE;
		break;
	case 2 :
		break;
	default :
		*m = -2;
		return ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 );
		break;
	}
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) {
		if ( ( ( ch - PM1 ) % 2 ) == 0 ) { // PM1
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + 1 ) > 0 ) {
				*m = 0;
			}
			else {
				*m = 1;
			}
		}
		else { // pm2
			*m = -1;
		}
		return TRUE;
	}
	else {
		if ( ( ( ch - PM1 ) % 2 ) == 0 ) { // pm1
			if ( !_SCH_MODULE_GET_USE_ENABLE( ch + 1 , TRUE , side ) ) {
				*m = -2;
				return FALSE;
			}
			else {
				if ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) / 100 ) <= 0 ) {
					*m = -1;
					return FALSE;
				}
				else {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + 1 ) > 0 ) {
						if ( SCHEDULER_CONTROL_Check_2Module_OverTime_Wait( ch ) ) { // 2008.03.18
							*m = -3;
							return FALSE;
						}
						else {
							*m = 2;
							return TRUE;
						}
					}
					else {
						*m = -1;
						return FALSE;
					}
				}
			}
		}
		else { // pm2
			if ( !_SCH_MODULE_GET_USE_ENABLE( ch - 1 , TRUE , side ) ) {
				*m = -2;
				return FALSE;
			}
			else {
				if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) {
					*m = -1;
					return FALSE;
				}
				else {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch - 1 ) > 0 ) {
						if ( SCHEDULER_CONTROL_Check_2Module_OverTime_Wait( ch ) ) { // 2008.03.18
							*m = -3;
							return FALSE;
						}
						else {
							*m = 2;
							return TRUE;
						}
					}
					else {
						*m = -1;
						return FALSE;
					}
				}
			}
		}
	}
	*m = -3;
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_INVALID_MAKE_FMSIDE_AND_RETURN( int side , int tms ) { // 2008.01.09
	int i , j , xoff , s , p , crm , sc , lc;
	int old = -1 , oldtag;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return FALSE; // 2012.09.03
	//
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) return FALSE;
	//
	if ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2013.03.09
		crm = TRUE;
	}
	else {
		crm = FALSE;
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) <= 0 ) return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) <= 0 ) return FALSE;
	//
	if ( _SCH_MODULE_Get_TM_TYPE( tms , 0 ) != SCHEDULER_MOVING_OUT ) return FALSE;
	if ( _SCH_MODULE_Get_TM_TYPE( tms , 1 ) != SCHEDULER_MOVING_OUT ) return FALSE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) ) return FALSE;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return FALSE;
			//
			//==============================================================
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.03.09
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc );
			}
			else {
				sc = 1;
				lc = _SCH_PRM_GET_MODULE_SIZE( i );
			}
			//==============================================================
			if ( crm ) {
				xoff = _SCH_PRM_GET_MODULE_SIZE( i ) / 2;
			}
			else {
				xoff = 1;
			}
			//==============================================================
			for ( j = sc ; j <= ( crm ? xoff : lc ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j + xoff ) <= 0 ) {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j+xoff ) ) { // 2018.04.03
							//
							return FALSE;
							//
						}
						//
					}
				}
				if ( !crm ) j++;
			}
			//
			for ( j = sc ; j <= ( crm ? xoff : lc ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
					//
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) { // 2018.04.03
						//
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) % 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) / 100 ) <= 0 ) {
								return FALSE;
							}
						}
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) % 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) / 100 ) <= 0 ) {
								return FALSE;
							}
						}
					}
				}
				if ( !crm ) j++;
			}
			//
			if ( crm ) {
				for ( j = xoff + 1 ; j <= lc ; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
						//
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) / 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) % 100 ) <= 0 ) {
								return FALSE;
							}
						}
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) / 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) % 100 ) <= 0 ) {
								return FALSE;
							}
						}
					}
				}
			}
			//
			for ( j = sc ; j <= lc ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( i , j );
					p = _SCH_MODULE_Get_BM_POINTER( i , j );
					if ( ( s >= 0 ) && ( p >= 0 ) && ( p < 100 ) ) {
						if ( old == -1 ) {
							old = i;
							oldtag = _SCH_CLUSTER_Get_SupplyID( s , p );
						}
						else {
							if ( oldtag < _SCH_CLUSTER_Get_SupplyID( s , p ) ) {
								old = i;
								oldtag = _SCH_CLUSTER_Get_SupplyID( s , p );
							}
						}
					}
				}
			}
		}
		//
	}
	//
	if ( old == -1 ) return FALSE;
	//
	for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( old ) ; j++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( old , j ) > 0 ) {
			//
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( old , j ) , _SCH_MODULE_Get_BM_POINTER( old , j ) , -1 );
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_BM_SIDE( old , j ) , _SCH_MODULE_Get_BM_POINTER( old , j ) , FALSE , TRUE , 31 ); // 2016.02.19
			//
			_SCH_MODULE_Set_BM_STATUS( old , j , BUFFER_OUTWAIT_STATUS );
		}
	}
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , old , 1 , TRUE , 1 , 1504 );
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_FORCE_MAKE_FMSIDE( int side , int tms ) { // 2008.01.10
	int i;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return FALSE; // 2012.09.03
	//
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) return FALSE;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) > 0 ) return FALSE;
	//
	if ( SCHEDULER_FM_Current_No_More_Supply2( side ) ) {
		if ( SCHEDULER_FM_Another_No_More_Supply( side , -2 ) ) return FALSE;
	}
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) {
			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) return FALSE;
		}
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) continue;
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , 1 , 1506 );
		}
	}
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_CURR_is_OLD( int oldch , int oldtme , int currch , int currtme ) { // 2008.01.08
	int s , p , oldsp , cursp;
	//-------------------------------------------------------------------------
	if ( ( ( oldch - PM1 ) % 2 ) == 0 ) { // pm1
		if      ( _SCH_MODULE_Get_PM_WAFER( oldch , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( oldch , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( oldch , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				oldsp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else if ( _SCH_MODULE_Get_PM_WAFER( oldch + 1 , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( oldch + 1 , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( oldch + 1 , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				oldsp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else {
			return ( oldtme > currtme );
		}
	}
	else { // pm2
		if      ( _SCH_MODULE_Get_PM_WAFER( oldch , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( oldch , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( oldch , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				oldsp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else if ( _SCH_MODULE_Get_PM_WAFER( oldch - 1 , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( oldch - 1 , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( oldch - 1 , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				oldsp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else {
			return ( oldtme > currtme );
		}
	}
	//-------------------------------------------------------------------------
	if ( ( ( currch - PM1 ) % 2 ) == 0 ) { // pm1
		if      ( _SCH_MODULE_Get_PM_WAFER( currch , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( currch , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( currch , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				cursp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else if ( _SCH_MODULE_Get_PM_WAFER( currch + 1 , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( currch + 1 , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( currch + 1 , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				cursp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else {
			return ( oldtme > currtme );
		}
	}
	else { // pm2
		if      ( _SCH_MODULE_Get_PM_WAFER( currch , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( currch , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( currch , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				cursp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else if ( _SCH_MODULE_Get_PM_WAFER( currch - 1 , 0 ) > 0 ) {
			s = _SCH_MODULE_Get_PM_SIDE( currch - 1 , 0 );
			p = _SCH_MODULE_Get_PM_POINTER( currch - 1 , 0 );
			if ( ( s < 0 ) || ( p < 0 ) || ( p >= 100 ) ) {
				return ( oldtme > currtme );
			}
			else {
				cursp = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
		}
		else {
			return ( oldtme > currtme );
		}
	}
	//-------------------------------------------------------------------------
	if ( oldsp > cursp ) return TRUE;
	//-------------------------------------------------------------------------
	return FALSE;
}


void SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_SEPARATE_MIXING( int tms , int side ) { // 2008.01.08
	int i , sd;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i + 1 , FALSE , -1 ) ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i + 1 ) <= 0 ) {
						//
						if ( ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) % 100 ) > 0 ) {
							if ( ( _SCH_MODULE_Get_PM_WAFER( i + 1 , 0 ) / 100 ) > 0 ) {
								sd = _SCH_MODULE_Get_PM_SIDE( i + 1 , 0 );
								//
								_SCH_MODULE_Set_PM_WAFER( i , 0 , ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) % 100 ) + ( ( _SCH_MODULE_Get_PM_WAFER( i + 1 , 0 ) / 100 ) * 100 ) );
								_SCH_MODULE_Set_PM_SIDE_POINTER( i , _SCH_MODULE_Get_PM_SIDE( i , 0 ) , _SCH_MODULE_Get_PM_POINTER( i , 0 ) , _SCH_MODULE_Get_PM_SIDE( i + 1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( i + 1 , 0 ) );
								_SCH_MODULE_Set_PM_WAFER( i + 1 , 0 , 0 );
								//
							}
						}
						//
					}
				}
			}
		}
		i++;
	}
	//
	if ( !SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_INVALID_MAKE_FMSIDE_AND_RETURN( side , tms ) ) { // 2008.01.09
		SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_FORCE_MAKE_FMSIDE( side , tms ); // 2008.01.10
	}
	//
}

//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
//=====================================================================================================================================================================================================================================================================
int SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( int TMATM , int chamber , int Side , int Pointer , int dop ) { // 2003.05.22
	int ChkTrg , m , k , x;
	x = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,chamber );
	if ( x != 0 ) {
//		*curr = _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,chamber );
		return x;
	}
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , dop , k );
		if ( ChkTrg > 0 ) {
			m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg );
			if ( m == 0 ) return 0;
			if ( x != 0 ) {
				if ( x != m ) return 0;
			}
			x = m;
		}
	}
	return x;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_INTERLOCK_PM_EXT_DENY_FOR_MDL( int TMATM , int chamber , int ext ) { // 2004.03.15
	if ( ext == 0 ) return TRUE;
	if ( ext != _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,chamber ) ) {
		return TRUE;
	}
	return FALSE;
}
//=======================================================================================
int SCHEDULING_CHECK_INTERLOCK_DENY_PICK_POSSIBLE( int TMATM , int chamber , int Side , int Pointer , int dop ) { // 2003.05.22
	int i , j , Side2 , Pointer2 , ChkTrg , ChkTrg2;
	int pmslot;

	if ( dop >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return TRUE;

	if ( !SCH_PM_HAS_PICKING( chamber , &pmslot ) ) return TRUE; // 2014.01.10

//	Side2    = _SCH_MODULE_Get_PM_SIDE( chamber , 0 ); // 2014.01.10
//	Pointer2 = _SCH_MODULE_Get_PM_POINTER( chamber , 0 ); // 2014.01.10
	//
	Side2    = _SCH_MODULE_Get_PM_SIDE( chamber , pmslot ); // 2014.01.10
	Pointer2 = _SCH_MODULE_Get_PM_POINTER( chamber , pmslot ); // 2014.01.10
	//
	if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) >= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) return TRUE;

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) , i );
		if ( ChkTrg > 0 ) {
//			if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg ) == TMATM ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , dop , j );
					if ( ChkTrg2 > 0 ) {
//						if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
							if ( ChkTrg != ChkTrg2 ) return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}
//=======================================================================================
void SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( int sidex , int ch ) {
	int i , side;
	for ( side = 0 ; side < MAX_CASSETTE_SIDE ; side++ ) { // 2003.02.13
		if ( ch == -1 ) {
	//		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( i + PM1 , FALSE ); // 2003.02.12
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) _SCH_MODULE_Set_Use_Interlock_Run( side , i + PM1 , FALSE ); // 2003.02.12
		}
		else {
	//		SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( ch , FALSE ); // 2003.02.12
			_SCH_MODULE_Set_Use_Interlock_Run( side , ch , FALSE ); // 2003.02.12
		}
	}
}

//=======================================================================================

//BOOL SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( int ch , BOOL inout ) { // 2007.06.28 // 2015.02.16
BOOL SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( int ch , BOOL inout ) { // 2007.06.28 // 2015.02.16
	int i;
	BOOL in , out;
	in = FALSE;
	out = FALSE;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
			//
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
				//
				if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
							return FALSE;
						}
					}
					else {
						if ( _SCH_MODULE_Get_BM_STATUS( ch , i+1 ) == BUFFER_OUTWAIT_STATUS ) out = TRUE;
						if ( _SCH_MODULE_Get_BM_STATUS( ch , i+1 ) == BUFFER_INWAIT_STATUS ) in = TRUE;
					}
				}
				else {
					return FALSE;
				}
				//
			}
		}
		else {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) out = TRUE;
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) in = TRUE;
			//
			if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i+1 ) == BUFFER_OUTWAIT_STATUS ) out = TRUE;
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i+1 ) == BUFFER_INWAIT_STATUS ) in = TRUE;
				}
			}
			//
		}
		i++;
	}
	//
	if ( inout ) {
		if ( in && out ) return TRUE;
		return FALSE;
	}
	//
	return TRUE;
}


int SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( int ch , int *s1 , int *s2 , int orderoption ) {
	int i , k = 0;
	//
	*s1 = 0;
	*s2 = 0;
	if ( orderoption == 2 ) { // 2003.06.02
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					//
					if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( _SCH_PRM_GET_MODULE_GROUP( ch )  ) > 0 ) { // 2016.12.09
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
							i++; // 2018.04.03
							continue;
						}
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) {
							i++;
							continue; // 2018.04.03
						}
						//
					}
					//
					*s2 = *s1;
					*s1 = i;
					k++;
				}
				//
			}
			//
			if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( _SCH_PRM_GET_MODULE_GROUP( ch )  ) > 0 ) i++; // 2016.12.09
			//
		}
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					//
					if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( _SCH_PRM_GET_MODULE_GROUP( ch )  ) > 0 ) { // 2016.12.09
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) > 0 ) {
							i--; // 2018.04.03
							continue;
						}
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i - 1 ) ) {
							i--;
							continue; // 2018.04.03
						}
						//
					}
					//
		//		if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_READY_STATUS ) {
					*s2 = *s1;
					*s1 = i;
					k++;
					//
				}
			}
			//
			if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( _SCH_PRM_GET_MODULE_GROUP( ch )  ) > 0 ) i--; // 2016.12.09
			//
		}
	}
	return k;
}

//==========================================================================================================================
//int SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( int ch , int slot0 , int *slot2 ) { // 2007.07.11 // 2018.06.18
int SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( int ch , int slot0 , int order , int *slot2 ) { // 2007.07.11 // 2018.06.18
	int slot , crs;

	if ( ( slot0 % 2 ) == 0 ) { // 2,4,6...
//		crs = TRUE; // 2007.10.04 // 2018.06.18
		crs = ( order != 2 ); // 2007.10.04 // 2018.06.18
		slot = slot0 - 1;
	}
	else { // 1,3,5...
//		crs = FALSE; // 2007.10.04 // 2018.06.18
		crs = ( order == 2 ); // 2007.10.04 // 2018.06.18
		slot = slot0 + 1;
	}
	if ( slot < 1 ) return 0;
	if ( slot > _SCH_PRM_GET_MODULE_SIZE( ch ) ) return 0;
	if ( _SCH_MODULE_Get_BM_WAFER( ch , slot ) <= 0 ) return 0;
	if ( _SCH_MODULE_Get_BM_STATUS( ch , slot ) != BUFFER_OUTWAIT_STATUS ) return 0; // 2008.01.04
	if ( _SCH_MODULE_Get_BM_SIDE( ch , slot ) == _SCH_MODULE_Get_BM_SIDE( ch , slot0 ) ) return 0;
	if ( *slot2 <= 0 ) { // 2007.11.05
		*slot2 = slot;
	}
	if ( crs ) return 2; // 2007.10.04
	return 1;
}


int SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( int ch , int side , int *pointer , int *s1 , BOOL Switch , int orderoption , int *outcount ) {
	int i , k = 0 , no = -1;
	*outcount = 0; // 2004.12.06
	*s1 = 0;
	if ( Switch ) {
		//=====================================================================================================================================
		//=====================================================================================================================================
		// 2004.12.06
		//=====================================================================================================================================
		//=====================================================================================================================================
		if ( side != -1 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) > BUFFER_SWITCH_BATCH_PART ) { // 2013.03.09
				if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2009.03.03
					//
					k = 0;
					//
					for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
							if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
								k++;
								k++;
							}
						}
						else if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
							if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
								k++;
								k++;
							}
						}
						i++;
					}
					//
					*outcount = k;
					//
					k = 0;
				}
				else {
					*outcount = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_OUTWAIT_STATUS );
				}
			}
		}
		//=====================================================================================================================================
		//=====================================================================================================================================
		if ( orderoption == 2 ) { // 2003.06.02
			/*
			//
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
				for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
//								no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30 // 2016.08.25
//								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , 1 ); // 2016.08.25
//								*s1 = i; // 2016.08.25
//								k++; // 2016.08.25
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , 1 );
									*s1 = i;
								}
								else {
									if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , 1 );
										*s1 = i;
									}
								}
								k++;
								//
							}
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
//								no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30 // 2016.08.25
//								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2016.08.25
//								*s1 = i; // 2016.08.25
//								k++; // 2016.08.25
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									//
									if ( MULTI_ALL_PM_FULLSWAP ) { // 2019.01.07
										if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
										}
									}
									else {
										//
										if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
										}
										//
									}
								}
								k++;
								//
							}
						}
					}
				}
//			} // 2016.12.12
		}
		else if ( orderoption == 1 ) { // 2003.06.16
			//
			/*
			//
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
//								no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30 // 2016.08.25
//								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2016.08.25
//								*s1 = i; // 2016.08.25
//								k++; // 2016.08.25
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
										*s1 = i;
									}
								}
								k++;
								//
							}
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
//								no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30 // 2016.08.25
//								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2016.08.25
//								*s1 = i; // 2016.08.25
//								k++; // 2016.08.25
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									if ( MULTI_ALL_PM_FULLSWAP ) { // 2019.01.07
										if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
										}
									}
									//
									else {
										if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
										}
									}
								}
								k++;
								//
							}
						}
					}
				}
//			} // 2016.12.12
		}
		else {
			//
			/*
			//
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
								// 2016.08.25
								//
//								if ( no == -1 ) {
//									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//									*s1 = i;
//									k++;
//								}
//								else {
//									if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
//										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//										*s1 = i;
//									}
//									k++;
//								}
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
										//
										if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
											//
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
											//
										}
									}
								}
								k++;
								//
							}
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
							if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
								//
								// 2016.08.25
								//
//								if ( no == -1 ) {
//									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//									*s1 = i;
//									k++;
//								}
//								else {
//									if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
//										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//										*s1 = i;
//									}
//									k++;
//								}
								//
								// 2016.08.25
								//
								if ( no == -1 ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									if ( MULTI_ALL_PM_FULLSWAP ) { // 2019.01.07
										if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
										}
									}
									else {
										if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
											//
											if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( ( i % 2 ) == 1 ) ) { // 2017.07.17
												//
												no = _SCH_MODULE_Get_BM_POINTER( ch , i );
												if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
												*s1 = i;
												//
											}
											else {
												if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
													//
													no = _SCH_MODULE_Get_BM_POINTER( ch , i );
													if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
													*s1 = i;
													//
												}
												//
											}
										}
									}
								}
								k++;
								//
							}
						}
					}
				}
//			} // 2016.12.12
		}
	}
	else {
		if ( orderoption == 2 ) {
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
//				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							//
							// 2016.08.25
							//
//							no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30
//							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//							*s1 = i;
//							k++;
							//
							//
							// 2016.08.25
							//
							if ( no == -1 ) {
								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*s1 = i;
							}
							else {
								if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
							}
							k++;
							//
						}
					}
				}
			}
		}
		else if ( orderoption == 1 ) {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							//
							// 2016.08.25
							//
//							no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30
//							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//							*s1 = i;
//							k++;
							//
							// 2016.08.25
							//
							if ( no == -1 ) {
								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*s1 = i;
							}
							else {
								if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
							}
							k++;
							//
						}
					}
				}
			}
		}
		else {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.01.05
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							//
							// 2016.08.25
							//
//							if ( no == -1 ) {
//								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//								*s1 = i;
//								k++;
//							}
//							else {
//								if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
//									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
//									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
//									*s1 = i;
//								}
//								k++;
//							}
							//
							// 2016.08.25
							//
							if ( no == -1 ) {
								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*s1 = i;
							}
							else {
								if ( _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) >= _SCH_CLUSTER_Get_Ex_OrderMode( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
									//
									if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( ( i % 2 ) == 1 ) ) { // 2017.07.17
										//
										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
										*s1 = i;
										//
									}
									else {
										if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
											//
											no = _SCH_MODULE_Get_BM_POINTER( ch , i );
											if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
											*s1 = i;
											//
										}
									}
								}
							}
							k++;
							//
						}
					}
				}
			}
		}
	}
	if ( k > 0 ) *pointer = no;
	return k;
}

//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_HAS_INOUT_SIDE_POINTER( int ch , int side , int tms , int *pointer , int *s1 , int orderoption , int *outcount ) { // 2013.03.18
	int i , k = 0 , no = -1;
	//
	*outcount = 0;
	//
	*s1 = 0;
	//
	//=====================================================================================================================================
	if ( orderoption == 2 ) {
		//
		/*
		// 2016.12.12
		//
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2013.03.09
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							no = _SCH_MODULE_Get_BM_POINTER( ch , i );
							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , 1 );
							*s1 = i;
							k++;
						}
					}
				}
			}
		}
		else {
		//
		*/
		//
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							no = _SCH_MODULE_Get_BM_POINTER( ch , i );
							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
							*s1 = i;
							k++;
						}
					}
				}
			}
//		} // 2016.12.12
	}
	else if ( orderoption == 1 ) {
		//
		/*
		// 2016.12.12
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2013.03.09
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							no = _SCH_MODULE_Get_BM_POINTER( ch , i );
							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
							*s1 = i;
							k++;
						}
					}
				}
			}
		}
		else {
		//
		*/
		//
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							no = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2003.09.30
							if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
							*s1 = i;
							k++;
						}
					}
				}
			}
//		} // 2016.12.12
	}
	else {
		/*
		//
		// 2016.12.12
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( ch ) ) <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							if ( no == -1 ) {
								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*s1 = i;
								k++;
							}
							else {
								if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								k++;
							}
						}
					}
				}
			}
		}
		else {
		*/
		//

			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2007.01.05
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
						if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) {
							if ( no == -1 ) {
								no = _SCH_MODULE_Get_BM_POINTER( ch , i );
								if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*s1 = i;
								k++;
							}
							else {
								if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( ( i % 2 ) == 1 ) ) { // 2017.07.17
									no = _SCH_MODULE_Get_BM_POINTER( ch , i );
									if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*s1 = i;
								}
								else {
									if ( _SCH_MODULE_Get_BM_POINTER( ch , i ) < no ) {
										no = _SCH_MODULE_Get_BM_POINTER( ch , i );
										if ( side == -1 ) *outcount = _SCH_MODULE_Get_BM_SIDE( ch , i );
										*s1 = i;
									}
								}
								k++;
							}
						}
					}
				}
			}
//		} // 2016.12.12
	}
	//
	if ( k > 0 ) *pointer = no;
	//
	return k;
}

//==========================================================================================================================


// 2007.11.06
BOOL SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( int ch , int side ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

//==========================================================================================================================


int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( int ch , int side , int *s1 , int *s2 , int RunStyle , int orderoption ) {
//	int i , k = 0 , w1 = 0 , w2 = 0 , sc , lc; // 2009.06.18
	int i , k = 0 , w1 = -1 , w2 = -1 , sc , lc; // 2009.06.18
	int os , supid; // 2013.10.01
	*s1 = 0;
	*s2 = 0;
	if ( RunStyle == 1 ) { // Switch
		if ( orderoption == 2 ) { // 2003.06.02
			//
			/*
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
				for ( i = 1  ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s2 = *s1;
							*s1 = i;
							k++;
						}
					}
				}
			}
			else {
			*/
			//
				for ( i = 1  ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s2 = *s1;
							*s1 = i;
							k++;
						}
					}
				}
//			} // 2016.12.12
		}
		else {
			/*
			//
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s2 = *s1;
							*s1 = i;
							k++;
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s2 = *s1;
							*s1 = i;
							k++;
						}
					}
				}
//			} // 2016.12.12
		}
	}
	else if ( RunStyle == 2 ) { // Single
		if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) {
			if ( ( _SCH_MODULE_Get_BM_WAFER( ch , 1 ) > 0 ) && ( ( _SCH_MODULE_Get_BM_STATUS( ch , 1 ) == BUFFER_OUTWAIT_STATUS ) || ( _SCH_CLUSTER_Get_PathDo( side , _SCH_MODULE_Get_BM_POINTER( ch , 1 ) ) == PATHDO_WAFER_RETURN ) ) ) {
				k++;
				*s1 = 1;
			}
		}
	}
	else if ( RunStyle == 3 ) { // Switch-MiddleSwap
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc ); // 2006.02.22
		if ( orderoption == 2 ) {
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
			}
		}
		else {
			for ( i = lc ; i >= sc ; i-- ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
			}
		}
		//====================================================================================================================
		// 2007.04.02
		//====================================================================================================================
		if ( k == 0 ) {
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
			}
		}
		//====================================================================================================================
	}
	else if ( RunStyle == 4 ) { // Switch-FullSwap // 2007.11.23
		//
		os = FALSE; // 2013.10.01
		supid = -1; // 2013.10.01
		//
		if ( orderoption == 2 ) { // 2003.06.02
			for ( i = 1  ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2013.10.01
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
				else { // 2013.10.01
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						if ( !os ) {
							os = TRUE;
							supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
						}
						else {
							if ( supid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
								supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
							}
						}
					}
				}
			}
		}
		else {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) { // 2013.10.01
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
				else { // 2013.10.01
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						if ( !os ) {
							os = TRUE;
							supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
						}
						else {
							if ( supid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
								supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
							}
						}
					}
				}
			}
		}
		//
		if ( os ) {
			if ( k > 0 ) {
				if ( ( *s1 != 0 ) && ( *s2 != 0 ) ) {
					if ( supid < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
						if ( supid < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s2 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s2 ) ) ) {
							k = 0;
						}
					}
				}
				else if ( *s1 != 0 ) {
					if ( supid < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s1 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s1 ) ) ) {
						k = 0;
					}
				}
				else if ( *s2 != 0 ) {
					if ( supid < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , *s2 ) , _SCH_MODULE_Get_BM_POINTER( ch , *s2 ) ) ) {
						k = 0;
					}
				}
			}
		}
		//
	}
	else if ( RunStyle == 10 ) { // INS / OUTS // 2009.06.18
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
				if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
					k++;
//					if      ( ( w1 == 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w1 ) ) { // 2009.06.18
					if      ( ( w1 == -1 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w1 ) ) { // 2009.06.18
						*s2 = *s1;
						*s1 = i;
						w2 = w1;
						w1 = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
//					else if ( ( w2 == 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w2 ) ) { // 2009.06.18
					else if ( ( w2 == -1 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w2 ) ) { // 2009.06.18
						*s2 = i;
						w2 = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
				}
			}
		}
	}
	else {
		if ( orderoption == 2 ) { // 2003.06.02
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
						*s2 = *s1;
						*s1 = i;
					}
				}
			}
		}
		else if ( orderoption == 1 ) { // 2003.06.16
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
						*s2 = *s1;
						*s1 = i;
					}
				}
			}
		}
		else {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
//						if      ( ( w1 == 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w1 ) ) { // 2009.06.18
						if      ( ( w1 == -1 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w1 ) ) { // 2009.06.18
							*s2 = *s1;
							*s1 = i;
							w2 = w1;
							w1 = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
//						else if ( ( w2 == 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w2 ) ) { // 2009.06.18
						else if ( ( w2 == -1 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w2 ) ) { // 2009.06.18
							*s2 = i;
							w2 = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
					}
				}
			}
		}
	}
	return k;
}

int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( int ch , int side , int *s , int *p , int RunStyle , int orderoption ) {
	int i , k = 0 , w1 = 0  , sc , lc;
	*s = 0;
	if ( RunStyle == 1 ) { // Switch
		if ( orderoption == 2 ) { // 2003.06.02
			//
			/*
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
				for ( i = 1  ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s = i;
							*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
							k++;
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = 1  ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s = i;
							*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
							k++;
						}
					}
				}
//			} // 2016.12.12
		}
		else {
			/*
			//
			// 2016.12.12
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s = i;
							*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
							k++;
						}
					}
				}
			}
			else {
			//
			*/
			//
				for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
					if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
							*s = i;
							*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
							k++;
						}
					}
				}
//			} // 2016.12.12
		}
	}
	else if ( RunStyle == 2 ) { // Single
		if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) ) {
			if ( ( _SCH_MODULE_Get_BM_WAFER( ch , 1 ) > 0 ) && ( ( _SCH_MODULE_Get_BM_STATUS( ch , 1 ) == BUFFER_OUTWAIT_STATUS ) || ( _SCH_CLUSTER_Get_PathDo( side , _SCH_MODULE_Get_BM_POINTER( ch , 1 ) ) == PATHDO_WAFER_RETURN ) ) ) {
				k++;
				*s = 1;
				*p = _SCH_MODULE_Get_BM_POINTER( ch , 1 );
			}
		}
	}
	else if ( RunStyle == 3 ) { // Switch-MiddleSwap
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc ); // 2006.02.22
		if ( orderoption == 2 ) {
			for ( i = sc ; i <= lc ; i++ ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
						*s = i;
						*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
						k++;
					}
				}
			}
		}
		else {
			for ( i = lc ; i >= sc ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_READY_STATUS ) {
						*s = i;
						*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
						k++;
					}
				}
			}
		}
	}
	else {
		if ( orderoption == 2 ) { // 2003.06.02
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
						*s = i;
						*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
				}
			}
		}
		else if ( orderoption == 1 ) { // 2003.06.16
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
						*s = i;
						*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
				}
			}
		}
		else {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) ) {
					if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
						k++;
						if      ( ( w1 == 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) <= w1 ) ) {
							*s = i;
							*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
							w1 = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
					}
				}
			}
		}
	}
	return k;
}

//==========================================================================================================================
int SCHEDULER_CONTROL_Chk_Get_BM_Count( int ch , int sts ) {
	int i , k = 0;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) continue;
		if ( sts < 0 ) {
			k++;
		}
		else {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == sts ) {
				k++;
			}
		}
	}
	//
	return k;
}

//==========================================================================================================================
int SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE( int ch , int side , BOOL Switch ) {
	int i , k = 0;
	if ( Switch ) {
		/*
		// 2016.12.12
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) == side ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
						k++;
					}
				}
			}
		}
		else {
		//
		*/
		//
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
						k++;
					}
				}
			}
//		} // 2016.12.12
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
					k++;
				}
			}
		}
	}
	return k;
}

//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK( int ch , int s1 , BOOL dblcheck ) { // 2016.12.09
	if ( s1 <= 0 ) return 0;
	if ( s1 > _SCH_PRM_GET_MODULE_SIZE( ch ) ) return 0;
//	if ( _SCH_MODULE_Get_BM_STATUS( ch , s1 ) == BUFFER_READY_STATUS ) {
	if ( _SCH_MODULE_Get_BM_WAFER( ch , s1 ) <= 0 ) {
		//
		if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , s1 ) ) return 0; // 2018.04.03
		//
		if ( dblcheck ) { // 2016.12.09
			if ( _SCH_MODULE_Get_BM_WAFER( ch , ( ( s1 % 2 ) == 0 ) ? s1-1 : s1+1 ) > 0 ) return 0;
			//
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , ( ( s1 % 2 ) == 0 ) ? s1-1 : s1+1 ) ) return 0; // 2018.04.03
			//
		}
		//
		return 1;
	}
	return 0;
}

//==========================================================================================================================
int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( int ch , int *s1 , int orderoption ) {
	int i , k = 0;
	*s1 = 0;
	if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( _SCH_PRM_GET_MODULE_SIZE( ch ) > 2 ) ) { // 2007.06.28
		//=====================================================================================
		// 2007.06.28
		//=====================================================================================
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) { // 2018.04.03
								*s1 = i;
								k++;
								k++;
								if ( orderoption != 2 ) break;
							}
						}
					}
					else {
						*s1 = i;
						k++;
						if ( orderoption != 2 ) break;
					}
				}
			}
			i++;
		}
		//=====================================================================================
	}
	else {
		if ( orderoption == 2 ) { // 2003.06.02
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s1 = i;
					k++;
				}
			}
		}
		else {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		//		if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_READY_STATUS ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s1 = i;
					k++;
				}
			}
		}
	}
	return k;
}

//==========================================================================================================================




int  SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( int whatm , int option ) {
	if ( option == 0 ) return 0;
	if      ( whatm == CHECKORDER_FOR_FM_PICK ) { // fmside
		if ( ( option == 2 ) || ( option == 4 ) || ( option == 6 ) || ( option == 8 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_FM_PLACE ) { // fmside
		if ( ( option == 2 ) || ( option == 4 ) || ( option == 6 ) || ( option == 8 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_TM_PICK ) { // fmside
		if ( ( option == 2 ) || ( option == 3 ) || ( option == 6 ) || ( option == 7 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_TM_PLACE ) { // fmside
		if ( ( option == 2 ) || ( option == 3 ) || ( option == 6 ) || ( option == 7 ) ) return 2;
	}
	else {
		return 0;
	}
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ = 0; // 2019.04.25
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_MODE; // 2019.04.25
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_SWAPPING; // 2019.04.25
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_MODE[MAX_BM_CHAMBER_DEPTH]; // 2019.04.25
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_FULLRUN[MAX_BM_CHAMBER_DEPTH]; // 2019.04.25
int  SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_LASTORDER[MAX_BM_CHAMBER_DEPTH]; // 2019.04.25

void SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_CHECK() { // 2019.04.25
	int i;
	//
	if ( SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ != 0 ) return;
	//
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_MODE = Get_Prm_MODULE_BUFFER_SWITCH_MODE();
	//
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_SWAPPING = _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING();
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_MODE[i] = _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , BM1 + i );
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_FULLRUN[i] = _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM1 + i );
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_LASTORDER[i] = _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BM1 + i );
		//
	}
	//
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ = 1;
	//
}

void SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_SET() { // 2019.04.25
	int i;
	//
	if ( SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ != 2 ) return;
	//
	Set_Prm_MODULE_BUFFER_SWITCH_MODE( SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_MODE );
	//
	_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_SWITCH_SWAPPING );
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		//
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + i , SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_MODE[i] );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + i , SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_FULLRUN[i] );
		_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( BM1 + i , SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_LASTORDER[i] );
		//
	}
	//
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ = 1;
	//
}

void SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE() { // 2019.04.25
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_DEFAULT_READ = 2;
}

BOOL SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING() { // 2004.08.14
	int b1 , b2 , xm = 0; // 2007.03.27
	//
	SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_CHECK(); // 2019.04.25
	//
	switch( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() ) {
	case 0 :
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_SET(); // 2019.04.25
		//
		return TRUE;
		break;
	case 4 : // b
		xm = 4; // 2007.03.27
		break;
	case 5 : // f
		xm = 5; // 2007.03.27
		break;
	case 6 : // s
		xm = 6; // 2007.03.27
		break;
	case 8 : // m // 2006.02.25
		xm = 7; // 2007.03.27
		break;
	case 9 : // pt1 // 2015.03.02
		xm = 8;
		break;
	case 10 : // pt2 // 2015.03.02
		xm = 9;
		break;
	}
	//====================================================================
	if ( xm == 0 ) {
		//
//		if ( SCHEDULER_Get_PATHTHRU_MODE() == 0 ) return FALSE; // 2015.03.02
		//
		b1 = _SCH_MODULE_GET_USE_ENABLE( BM1 , FALSE , -1 );
		b2 = _SCH_MODULE_GET_USE_ENABLE( BM1 + 1 , FALSE , -1 );
		//
		if ( !b1 && !b2 ) return TRUE;
		//
		if ( ( b1 && !b2 ) || ( !b1 && b2 ) ) {
			switch( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() ) {
			case 1 : // pt <-> b
				xm = 4;
				break;
			case 2 : // pt <-> f
				xm = 5;
				break;
			case 3 : // pt <-> s
				xm = 6;
				break;
			case 7 : // pt <-> m
				xm = 7;
				break;
			}
		}
		else {
			if      ( SCHEDULER_Get_PATHTHRU_MODE() == 1 ) { // BM1-IN Dir
				xm = 2;
			}
			else if ( SCHEDULER_Get_PATHTHRU_MODE() == 2 ) { // BM1-IN Full
				xm = 0;
			}
			else if ( SCHEDULER_Get_PATHTHRU_MODE() == 3 ) { // BM2-IN Dir
				xm = 3;
			}
			else if ( SCHEDULER_Get_PATHTHRU_MODE() == 4 ) { // BM2-IN Full
				xm = 1;
			}
			else { // 2015.03.02
				xm = 0;
			}
		}
	}
	//====================================================================
	// 2007.03.17
	//====================================================================
	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) return FALSE;
	//====================================================================
	switch( xm ) {
	case 0 : // paththru(A-Full)
	case 8 : // paththru(A-Full) // 2015.03.02
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_IN_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_OUT_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 1 : // paththru(B-Full)
	case 9 : // paththru(B-Full) // 2015.03.02
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_OUT_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_IN_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 2 : // paththru(A-Direct)
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_IN_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_OUT_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , FALSE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , FALSE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 3 : // paththru(B-Direct)
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_OUT_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_IN_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , FALSE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , FALSE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 4 : // BatchSwap
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_BATCH_ALL );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		for ( b1 = BM1 ; b1 < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; b1++ ) {
			if ( _SCH_PRM_GET_MODULE_MODE( b1 ) ) {
				if ( _SCH_PRM_GET_MODULE_SIZE( b1 ) < _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) {
					_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_BATCH_PART );
					break;
				}
			}
		}
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 5 : // FullSwap
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_FULLSWAP );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 6 : // SingleSwap
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_SINGLESWAP );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	case 7 : // MiddleSwap
		//
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
		//
		_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_MIDDLESWAP );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_MODE( 0 , BM1 + 1 , BUFFER_SWITCH_MODE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 , TRUE );
		_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( BM1 + 1 , TRUE );
		//
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING_DEFAULT_UPDATE(); // 2019.04.25
		//
		break;
	default :
		return FALSE;
		break;
	}
	//====================================================================
	return TRUE;
}




BOOL SCHEDULER_CONTROL_Check_Arm_Place_BM_Has_Space( BOOL fmmode , int TMATM , int Arm , int swbm , BOOL prccheck ) { // 2007.10.10
	int i , j , k;

	if ( fmmode && ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) ) return FALSE; // 2007.10.17
	//
	if ( _SCH_MODULE_Get_TM_TYPE( TMATM , Arm ) != SCHEDULER_MOVING_OUT ) return FALSE; // 2007.10.17
	//
	if ( fmmode ) { // 2007.10.17
		if ( ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) || ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) ) {
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , Arm ) == SCHEDULER_MOVING_OUT ) return TRUE;
		}
		else {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
				if ( swbm == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//-----------------------------------------------------------------------------------------------------------------
						// 2008.03.20
						//-----------------------------------------------------------------------------------------------------------------
//						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
//							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { 
//								if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) ) { 
//									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return TRUE;
//								}
//							}
//						}
//						else { // 2007.10.17
//							return TRUE; // 2007.10.17
//						} // 2007.10.17
						//-----------------------------------------------------------------------------------------------------------------
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { 
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
								if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) ) { 
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return TRUE;
								}
							}
							else { // 2007.10.17
//								return TRUE; // 2007.10.17 // 2013.03.18
							} // 2007.10.17
						}
						//-----------------------------------------------------------------------------------------------------------------
					}
				}
				else {
					if ( _SCH_MODULE_GET_USE_ENABLE( swbm , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( swbm ) == TMATM ) ) {
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( swbm ) || ( _SCH_MODULE_Get_BM_FULL_MODE( swbm ) == BUFFER_TM_STATION ) ) {
							if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( swbm ) <= 0 ) ) { 
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( swbm , &j , &k , 0 ) > 0 ) return TRUE;
							}
						}
						else { // 2007.10.17
//							return TRUE; // 2007.10.17 // 2013.03.18
						} // 2007.10.17
					}
				}
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
				if ( swbm == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//-----------------------------------------------------------------------------------------------------------------
						// 2008.03.20
						//-----------------------------------------------------------------------------------------------------------------
//						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
//							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
//								if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) ) { 
//									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 ) > 0 ) return TRUE;
//								}
//							}
//						}
//						else { // 2007.10.17
//							return TRUE; // 2007.10.17
//						} // 2007.10.17
						//-----------------------------------------------------------------------------------------------------------------
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
								if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) ) { 
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , FALSE ) > 0 ) return TRUE;
								}
							}
							else { // 2007.10.17
//								return TRUE; // 2007.10.17 // 2013.03.18
							} // 2007.10.17
						}
						//-----------------------------------------------------------------------------------------------------------------
					}
				}
				else {
					if ( _SCH_MODULE_GET_USE_ENABLE( swbm , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( swbm ) == TMATM ) ) {
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( swbm ) || ( _SCH_MODULE_Get_BM_FULL_MODE( swbm ) == BUFFER_TM_STATION ) ) {
							if ( !prccheck || ( _SCH_MACRO_CHECK_PROCESSING_INFO( swbm ) <= 0 ) ) { 
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( swbm , &j , &k , 0 , FALSE ) > 0 ) return TRUE;
							}
						}
						else { // 2007.10.17
//							return TRUE; // 2007.10.17 // 2013.03.18
						} // 2007.10.17
					}
				}
			}
			else {
//				if ( _SCH_MODULE_Get_TM_TYPE( TMATM , Arm ) == SCHEDULER_MOVING_OUT ) return TRUE; // 2007.10.17
				return TRUE; // 2007.10.17
			}
		}
	}
	return FALSE;
}
//============================================================================================================


BOOL SCHEDULER_CONTROL_Check_BM_Has_Space_forSW( int TMATM ) { // 2007.10.10
	int i , j , k;

	if ( ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) || ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) ) return TRUE;

	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { 
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return TRUE;
				}
				else {
					return TRUE;
				}
			}
			//-----------------------------------------------------------------------------------------------------------------
		}
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , FALSE ) > 0 ) return TRUE;
				}
				else {
					return TRUE;
				}
			}
			//-----------------------------------------------------------------------------------------------------------------
		}
	}
	return FALSE;
}
//============================================================================================================


BOOL GatewayBM_waiting( int TMATM , int Side , int Pointer , int offset ) { // 2018.10.20
	int i , j;
	int pd , cx;
	BOOL ts , to;
	//
	if ( !MULTI_ALL_PM_FULLSWAP ) return FALSE;
	//
	pd = _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + offset;
	//
	ts = FALSE;
	to = FALSE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		j = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , pd - 1 , i );
		//
		if ( j > 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , j , G_PLACE ) ) {
				ts = TRUE;
			}
			else {
				to = TRUE;
			}
		}
		//
	}
	//
	if ( !ts || !to ) return FALSE;
	//
	cx = _SCH_CLUSTER_Get_ClusterIndex( Side , Pointer );
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) {
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) {
				//
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( (TMATM+1) , i ) == BUFFER_SWITCH_MODE ) {
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
					//
					for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
						//
						if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
						//
						if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_INWAIT_STATUS ) continue;
						//
						if ( _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) ) != cx ) continue;
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) - 1 ) != pd ) continue;
						//
						return TRUE;
						//
					}
				}
			}
		}
	}
	//
	return FALSE;
	//
}



int SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( int TMATM , int arm0 , int pickch1 , int pickch2 , int trgprcswfrchk ) { // 2007.09.17
	// trgprcswfrchk
	// -1 : trgnotcheck
	//  0 : wx px
	//  1 : wo px
	//  2 : wx po
	//  3 : wo po
	int arm , sa , ea , i , Side2 , Pointer2 , ChkTrg2;
	BOOL BM_Waitiing; // 2018.10.20

	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) ) return 2;
	//
	if      ( arm0 == TA_STATION ) {
		sa = TA_STATION;
		ea = TA_STATION;
	}
	else if ( arm0 == TB_STATION ) {
		sa = TB_STATION;
		ea = TB_STATION;
	}
	else {
		sa = TA_STATION;
		ea = TB_STATION;
	}
	for ( arm = sa ; arm <= ea ; arm++ ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) > 0 ) ) {
			Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
			Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );

			BM_Waitiing = GatewayBM_waiting( TMATM , Side2 , Pointer2 , -1 ); // 2018.10.20

			if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) != PATHDO_WAFER_RETURN ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , i );
					//-------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
					//-------------------------------------------------------------------------------------
					if ( ChkTrg2 > 0 ) {
						if ( !BM_Waitiing  && _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
							if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg2 , FALSE , Side2 ) ) {
								if ( !_SCH_MODULE_Get_Use_Interlock_Run( Side2 , ChkTrg2 ) ) {
									//
									if ( ( pickch1 == -1 ) && ( pickch2 == -1 ) && ( trgprcswfrchk == -1 ) ) return arm;
									//
									if ( pickch1 != -1 ) {
										if ( ChkTrg2 == pickch1 ) return arm;
									}
									if ( pickch2 != -1 ) {
										if ( ChkTrg2 == pickch2 ) return arm;
									}
									if ( ( trgprcswfrchk >= 0 ) && ( trgprcswfrchk <= 3 ) ) {
//										if ( ( trgprcswfrchk == 0 ) || ( trgprcswfrchk == 2 ) || ( _SCH_MODULE_Get_PM_WAFER( ChkTrg2 , 0 ) <= 0 ) ) { // 2014.01.10
										if ( ( trgprcswfrchk == 0 ) || ( trgprcswfrchk == 2 ) || ( SCH_PM_IS_ABSENT( ChkTrg2 , Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 ) ) ) { // 2014.01.10
											if ( ( trgprcswfrchk == 0 ) || ( trgprcswfrchk == 1 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg2 ) <= 0 ) ) {
												if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) {
													if ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg2 ) == -1 ) {
														return arm;
													}
												}
												else {
													return arm;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return -1;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_PM_Has_Return_Wafer( int TMATM ) { // 2013.03.19
	int i;
	int pmslot;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//------------------------------------------------------------------------------------------------
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		//------------------------------------------------------------------------------------------------
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue; // 2014.01.11
		if ( !SCH_PM_HAS_PICKING( i , &pmslot ) ) continue; // 2014.01.10
		//------------------------------------------------------------------------------------------------
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//------------------------------------------------------------------------------------------------
		return TRUE;
		//------------------------------------------------------------------------------------------------
	}
	//
	return FALSE;
}

BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( int TMATM , BOOL cannotgo ) { // 2007.09.06 // 2013.03.26
	int k , m , s , p , arm , w , pd , pr , reread , Res;
	//
	while ( TRUE ) {
		//
		reread = FALSE;
		//
		for ( arm = TA_STATION ; arm <= TB_STATION ; arm++ ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) {
				//
				w = _SCH_MODULE_Get_TM_WAFER( TMATM , arm );
				s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
				p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
				//
				if ( w > 0 ) {
					//
					if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) {
						reread = TRUE;
						break;
					}
					if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) {
						reread = TRUE;
						break;
					}
					//
					pd = _SCH_CLUSTER_Get_PathDo( s , p );
					pr = _SCH_CLUSTER_Get_PathRange( s , p );
					//
					if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) == SCHEDULER_MOVING_OUT ) || ( pd > pr ) ) {
						Res = TRUE;
					}
					else {
						//
						if ( cannotgo ) {
							//
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								//
								m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd - 1 , k );
								//
								if ( m > 0 ) break;
								//
							}
							//
							if ( k == MAX_PM_CHAMBER_DEPTH ) {
								Res = TRUE;
							}
							else {
								Res = FALSE;
							}
							//
						}
						else {
							Res = FALSE;
						}
						//
					}
					//
					if ( w != _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) ) {
						reread = TRUE;
						break;
					}
					if ( s != _SCH_MODULE_Get_TM_SIDE( TMATM , arm ) ) {
						reread = TRUE;
						break;
					}
					if ( p != _SCH_MODULE_Get_TM_POINTER( TMATM , arm ) ) {
						reread = TRUE;
						break;
					}
					if ( pd != _SCH_CLUSTER_Get_PathDo( s , p ) ) {
						reread = TRUE;
						break;
					}
					if ( pr != _SCH_CLUSTER_Get_PathRange( s , p ) ) {
						reread = TRUE;
						break;
					}
					//
					if ( Res ) return TRUE;
					//============================================================================================================
				}
			}
		}
		//
		if ( !reread ) break;
	}
	return FALSE;
}

int SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( int TMATM , int arm0 , int NextPlacech , int mode , BOOL hasnotwaferr ) { // 2006.02.10
	int s , p , k , arm , nc , f;
	int arms , arme;
	int gofind; // 2014.07.18
	//
	if ( arm0 == -1 ) {
		arms = TA_STATION;
		arme = TB_STATION;
	}
	else {
		arms = arm0;
		arme = arm0;
	}
	//
	if ( ( mode != 3 ) && ( mode != 4 ) ) {
		if ( NextPlacech != -1 ) { // 2007.03.07
//			if ( _SCH_MODULE_Get_PM_WAFER( NextPlacech , 0 ) <= 0 ) { // 2014.01.10
			if ( hasnotwaferr ) { // 2014.01.10
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( NextPlacech ) <= 0 ) return 1;
			}
		}
	}
	//
	f = 0;
	gofind = 0; // 2014.07.18
	//
	for ( arm = arms ; arm <= arme ; arm++ ) {
		//
		if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) > 0 ) ) {
			//============================================================================================================
			if ( ( mode == 3 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) == SCHEDULER_MOVING_OUT ) ) return 2;
			if ( ( mode == 4 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) == SCHEDULER_MOVING_OUT ) ) return 0;
			//============================================================================================================
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) {
				//============================================================================================================
				s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
				p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
				//============================================================================================================
				if      ( mode == 3 ) {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return 2;
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 1 ) return 0;
				}
				else if ( mode == 4 ) {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return 0;
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 1 ) return 0;
				}
				//============================================================================================================
//				if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) { // 2007.09.05
				if ( ( _SCH_CLUSTER_Get_PathDo( s , p ) > 0 ) && ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) ) { // 2007.09.05
					//
					if ( ( mode == 3 ) || ( mode == 4 ) || ( mode == 6 ) ) f = 1;
					//
					if ( gofind == 0 ) gofind = 1;
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						nc = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
						//
						//-------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &nc ); // 2007.10.29
						//-------------------------------------------------------------------------------------
						if ( nc > 0 ) {
							//
							gofind = 2; // 2014.07.18
							//
							if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nc , G_PLACE ) ) {
								//
								if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2017.12.11
									if ( _SCH_MODULE_GET_USE_ENABLE( nc , FALSE , s ) ) return 0;
								}
								//
								continue; // 2016.10.30
							}
							//
							if ( mode == 3 ) {
								switch ( mode ) {
								case 3 :
								case 4 :
									if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nc , G_PLACE ) ) continue;
									if ( !_SCH_MODULE_GET_USE_ENABLE( nc , FALSE , s ) ) continue;
									if ( nc != NextPlacech ) {
										f = 2;
//										if ( _SCH_MODULE_Get_PM_WAFER( nc , 0 ) > 0 ) continue; // 2014.01.10
										if ( !SCH_PM_IS_ABSENT( nc , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) continue; // 2014.01.10
									}
									break;
								}
								return 0;
							}
							else {
								if ( nc != NextPlacech ) {
									//
									f = 1;
									//
									switch ( mode ) {
									case 0 :
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( nc ) > 0 ) continue;
										break;
									case 1 :
//										if ( _SCH_MODULE_Get_PM_WAFER( nc , 0 ) > 0 ) continue; // 2014.01.10
										if ( !SCH_PM_IS_ABSENT( nc , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) continue; // 2014.01.10
										break;
									case 2 :
//										if ( _SCH_MODULE_Get_PM_WAFER( nc , 0 ) > 0 ) continue; // 2014.01.10
										if ( !SCH_PM_IS_ABSENT( nc , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) continue; // 2014.01.10
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( nc ) > 0 ) continue;
										break;
									case 3 :
									case 4 :
										if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nc , G_PLACE ) ) continue;
										if ( !_SCH_MODULE_GET_USE_ENABLE( nc , FALSE , s ) ) continue;
										break;
									case 5 :
										if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nc , G_PLACE ) ) continue;
										if ( !_SCH_MODULE_GET_USE_ENABLE( nc , FALSE , s ) ) continue;
										break;
									case 6 :
										if ( !_SCH_MODULE_GET_USE_ENABLE( nc , FALSE , s ) ) continue;
										break;
									}
									return 0;
								}
							}
						}
					}
				}
			}
		}
	}
	//
	if ( ( mode == 5 ) && ( f == 0 ) && ( gofind == 1 ) ) return 1; // 2014.07.18
	//
	return f;
}
//=======================================================================================
/*
//
// 2017.11.01 확인 후 사용예정(무한 TM-LL 반복 문제) 2017.12.07 수정
//
BOOL SCHEDULING_CHECK_for_TM_ARM_Place_ArmIntlks_Impossible( int TMATM , int arm ) { // 2017.02.13
	int s , p , pd , k , m , f;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( pd > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	f = 0;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd - 1 , k );
		//
		if ( m <= 0 ) continue;
		//
		f = 1;
		//
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,m ) ) {
		case 0 :
			return FALSE;
		case 1 :
			if ( arm == TA_STATION ) return FALSE;
			break;
		case 2 :
			if ( arm == TB_STATION ) return FALSE;
			break;
		}
		//
	}
	//
	if ( f == 0 ) return FALSE;
	//
	return TRUE;
}
//
*/
//
// 2017.11.01 확인 후 사용예정(무한 TM-LL 반복 문제) 2017.12.07 적용
//
BOOL SCHEDULING_CHECK_for_TM_ARM_Place_ArmIntlks_Impossible( int TMATM , int arm ) { // 2017.02.13
//	int s , p , pd , k , m , f; // 2017.11.01
	int s , p , pd , k , m; // 2017.11.01
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( pd > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
//	f = 0; // 2017.11.01
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd - 1 , k );
		//
		if ( m <= 0 ) continue;
		//
//		f = 1; // 2017.11.01
		//
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,m ) ) {
		case 0 :
			return FALSE;
		case 1 :
			if ( arm == TA_STATION ) return FALSE;
			break;
		case 2 :
			if ( arm == TB_STATION ) return FALSE;
			break;
		}
		//
	}
	//
//	if ( f == 0 ) return FALSE; // 2017.11.01
	//
	return TRUE;
}

BOOL SCHEDULING_CHECK_for_TM_ARM_Place_MultiGroup_Impossible( int TMATM , int arm ) { // 2018.11.07
	int s , p , pd , k , m , j , sl;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( pd > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd - 1 , k );
		//
		if ( m <= 0 ) continue;
		//
		for ( j = TMATM ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			//
			if ( !_SCH_PRM_GET_MODULE_MODE( TM + j ) ) continue; // 2019.01.30
			//
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( j , m , G_PLACE ) ) {
				//
				// 2018.12.06
				//
				if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , pd - 1 ) != NULL ) { // 2018.12.06
					//
					sl = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , pd - 1 , (m - PM1) );
					//
					if ( sl > 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( m , sl ) ) continue;
						//
					}
					//
				}
				//
				//
				return FALSE;
			}
			//
		}
		//
	}
	//
	return TRUE;
}

//=======================================================================================
BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Next_Chamber( int TMATM , int Nextch ) { // 2004.12.15
	int s , p , k , arm;
	for ( arm = TA_STATION ; arm <= TB_STATION ; arm++ ) {
		if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) > 0 ) ) {
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) {
				s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
				p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k ) == Nextch ) return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Next_Group( int TMATM ) { // 2018.06.12
	int s , p , k , arm , m;
	for ( arm = TA_STATION ; arm <= TB_STATION ; arm++ ) {
		if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) > 0 ) ) {
			//
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) {
				//
				s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
				p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
				//
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
						//
						if ( m > 0 ) {
							//
							if ( _SCH_PRM_GET_MODULE_GROUP( m ) > TMATM ) {
								return TRUE;
							}
							//
						}
						//
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULING_CHECK_for_BM_Has_Next_Group( int TMATM , int side ) { // 2018.06.12
	//
	int s , p , k , i , j , m;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( j = _SCH_PRM_GET_MODULE_SIZE( i ) ; j >= 1 ; j-- ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
			if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_INWAIT_STATUS ) continue;
			//
			s = _SCH_MODULE_Get_BM_SIDE( i , j );
			p = _SCH_MODULE_Get_BM_POINTER( i , j );
			//
			if ( TMATM == 0 ) { // 2018.09.19
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
			}
			else { // 2018.09.19
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
			}
			//
//			if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) { // 2018.09.19
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					if ( TMATM == 0 ) { // 2018.09.19
						m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
					}
					else { // 2018.09.19
						m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
					}
					//
					if ( m > 0 ) {
						//
						if ( _SCH_PRM_GET_MODULE_GROUP( m ) > TMATM ) {
							return TRUE;
						}
						//
					}
					//
				}
//			}
		}
	}
	return FALSE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Supply_Wafer( int TMATM , int arm ) { // 2014.04.07
	//
	int s , p;
	//
	if ( ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) || ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) <= 0 ) ) return FALSE;
	//
	if ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) == SCHEDULER_MOVING_OUT ) return FALSE;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	return TRUE;
}

//=======================================================================================
int SCHEDULING_CHECK_NonTM_Wafer_PM_Next_Place_Switch_Possible( int tms , int getch , int getslot , int side , int pt , int trgch , BOOL mainprcspickok ) { // 2007.12.12
	int ps , pp , pk;
	int pmslot;
	//======================================================================================================
	if ( ( trgch < PM1 ) || ( trgch >= AL ) ) return 1;
	//======================================================================================================
	if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , trgch , G_PLACE ) ) return 0;
	if ( !_SCH_MODULE_GET_USE_ENABLE( trgch , FALSE , -1 ) ) return 0;
	//======================================================================================================
//	if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) { // 2014.01.10
	if ( SCH_PM_HAS_ONLY_PICKING( trgch , &pmslot ) ) { // 2014.01.10
		//======================================================================================================
		if ( !mainprcspickok ) { // 2011.11.30
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return 0;
		}
		//======================================================================================================
//		ps = _SCH_MODULE_Get_PM_SIDE( trgch , 0 ); // 2014.01.10
//		pp = _SCH_MODULE_Get_PM_POINTER( trgch , 0 ); // 2014.01.10
		//
		ps = _SCH_MODULE_Get_PM_SIDE( trgch , pmslot ); // 2014.01.10
		pp = _SCH_MODULE_Get_PM_POINTER( trgch , pmslot ); // 2014.01.10
		//
		if ( ( pp < 100 ) && ( pp >= 0 ) ) {
			if ( _SCH_CLUSTER_Get_PathDo( ps , pp ) <= _SCH_CLUSTER_Get_PathRange( ps , pp ) ) {
				for ( pk = 0 ; pk < MAX_PM_CHAMBER_DEPTH ; pk++ ) {
					if ( trgch == SCHEDULER_CONTROL_Get_PathProcessChamber( ps , pp , _SCH_CLUSTER_Get_PathDo( ps , pp ) - 1 , pk ) ) {
						if ( !_SCH_CLUSTER_Check_HasProcessData_Post( ps , pp , _SCH_CLUSTER_Get_PathDo( ps , pp ) - 1 , pk ) ) break;
					}
				}
				if ( pk == MAX_PM_CHAMBER_DEPTH ) return 0;
			}
		}
	}
	else {
		//======================================================================================================
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return 0;
		//======================================================================================================
	}
	//======================================================================================================
	if ( ( getch >= CM1 ) && ( getch < PM1 ) ) { // 2011.11.25
		//======================================================================================================
		ps = side;
		pp = pt;
		//======================================================================================================
	}
	else {
		if ( ( getch < BM1 ) || ( getch >= TM ) ) return 2;
		//======================================================================================================
		ps = _SCH_MODULE_Get_BM_SIDE( getch , getslot );
		pp = _SCH_MODULE_Get_BM_POINTER( getch , getslot );
		//======================================================================================================
	}
	//======================================================================================================
	if ( _SCH_CLUSTER_Get_PathDo( ps , pp ) > _SCH_CLUSTER_Get_PathRange( ps , pp ) ) return 3;
	//======================================================================================================
//	if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) { // 2014.01.10
	if ( !SCH_PM_IS_EMPTY( trgch ) ) { // 2014.01.10
		if ( !_SCH_MACRO_HAS_PRE_PROCESS_OPERATION( ps , pp , trgch ) ) {
			return 4;
		}
	}
	else {
//		switch ( _SCH_MACRO_PRE_PROCESS_OPERATION( ps , pp , ( tms * 100 ) + 0 , trgch ) ) { // 2008.12.07
		switch ( _SCH_MACRO_PRE_PROCESS_OPERATION( ps , pp , ( tms * 100 ) + 0 , ( tms == 0 ) ? trgch + 100 : trgch ) ) { // 2008.12.07
		case -1 :
		case 2 :
			return 5;
			break;
		default :
			return 0;
			break;
		}
	}
	//======================================================================================================
	return 0;
}
//=======================================================================================
int SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_7( int mode , int tmatm , int jptype , int count , int side , int pointer ) { // 2006.07.13
	int k , oc , offset , ChkTrg;
	//========================================================================================
	if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) >= PATHDO_WAFER_RETURN ) return TRUE;
	//========================================================================================
	if      ( mode == 0 ) { // cm
		offset = 0;
	}
	else if ( mode == 1 ) { // pm
		offset = _SCH_CLUSTER_Get_PathDo( side , pointer );
	}
	else { // bm
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= 0 ) {
			offset = 0;
		}
		else {
			if ( tmatm == 0 ) {
				if ( jptype == 1 ) {
					offset = _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
				}
				else {
					offset = _SCH_CLUSTER_Get_PathDo( side , pointer );
				}
			}
			else {
				offset = _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
			}
		}
	}
	//========================================================================================
	oc = 0;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pointer , offset , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ChkTrg > 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , ChkTrg , G_PLACE ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , side ) ) {
					if ( _SCH_SUB_Run_Impossible_Condition( side , pointer , ChkTrg ) ) {
						oc++;
					}
				}
			}
		}
	}
	//========================================================================================
	if ( oc == 1 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pointer , offset , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , ChkTrg , G_PLACE ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , side ) ) {
						if ( _SCH_SUB_Run_Impossible_Condition( side , pointer , ChkTrg ) ) {
							if ( count == 1 ) {
								if ( SCHEDULING_CHECK_for_TM_ARM_Has_Next_Chamber( tmatm , ChkTrg ) ) return FALSE;
							}
						}
					}
				}
			}
		}
	}
	//========================================================================================
	else if ( oc >= 2 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pointer , offset , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , ChkTrg , G_PLACE ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , side ) ) {
						if ( _SCH_SUB_Run_Impossible_Condition( side , pointer , ChkTrg ) ) {
							//
//							if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) > 0 ) return FALSE; // 2014.01.10
							if ( !SCH_PM_IS_ABSENT( ChkTrg , side , pointer , offset ) ) return FALSE; // 2014.01.10
							//
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
								if ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) return FALSE;
							}
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg ) > 0 ) return FALSE;
							if ( count == 1 ) {
								if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Next_Chamber( tmatm , ChkTrg ) ) return FALSE;
							}
						}
					}
				}
			}
		}
	}
	//========================================================================================
	return TRUE;
}

//=======================================================================================
int SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_17( int mode , int tmatm , int jptype , int count , int side , int pointer ) { // 2006.07.13
	int i , k , offset , ChkTrg , wc1 , wc2 , wc3 , wy1 , wy2 , wy3 , crx , trg;
	int chc[MAX_CHAMBER];
	int chi[MAX_CHAMBER];
	//========================================================================================
	if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) >= PATHDO_WAFER_RETURN ) return TRUE;
	//========================================================================================
	trg = 6;
	//========================================================================================
	if      ( mode == 0 ) { // cm
		offset = 0;
	}
	else if ( mode == 1 ) { // pm
		offset = _SCH_CLUSTER_Get_PathDo( side , pointer );
	}
	else { // bm
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= 0 ) {
			offset = 0;
		}
		else {
			if ( tmatm == 0 ) {
				if ( jptype == 1 ) {
					offset = _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
				}
				else {
					offset = _SCH_CLUSTER_Get_PathDo( side , pointer );
				}
			}
			else {
				offset = _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
			}
		}
	}
	//========================================================================================
	if ( ( _SCH_CLUSTER_Get_PathRange( side , pointer ) - offset ) <= trg ) return TRUE;
	//========================================================================================
	wc1 = wc2 = wc3 = wy1 = wy2 = wy3 = 0;
	crx = 0;

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		chc[i] = 0;
		chi[i] = -1;
	}

	for ( i = offset ; i < _SCH_CLUSTER_Get_PathRange( side , pointer ) ; i++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pointer , i , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if      ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
					wc1++;
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , side , pointer , i ) ) { // 2014.01.10
						if ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) == -1 ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg ) <= 0 ) {
								wy1++;
							}
						}
					}
				}
				else if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
					wc2++;
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , side , pointer , i ) ) { // 2014.01.10
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg ) <= 0 ) {
							wy2++;
						}
					}
				}
				else {
					//==============================================================================
					wc3++;
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , side , pointer , i ) ) { // 2014.01.10
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg ) <= 0 ) {
							wy3++;
						}
					}
					//==============================================================================
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( tmatm , ChkTrg ) ) {
						if ( crx == 1 ) crx = 2;
					}
					else {
						if ( crx == 0 ) crx = 1;
						if ( crx == 2 ) crx = 3;
					}
					//==============================================================================
					if ( chi[ChkTrg] != crx ) chc[ChkTrg]++;
					chi[ChkTrg] = crx;
					//==============================================================================
				}
			}
		}
	}
	//========================================================================================
	if ( ( mode == 1 ) && ( crx != 3 ) ) return TRUE;
	//========================================================================================
	if ( ( wc1 + wc2 + wc3 ) <= trg ) return FALSE;
	//========================================================================================
	if ( ( wc1 > 2 ) && ( wy1 < 2 ) ) return FALSE;
	if ( ( wc2 > 2 ) && ( wy2 < 2 ) ) return FALSE;
	if ( ( wc3 > 2 ) && ( wy3 < 2 ) ) return FALSE;
	//========================================================================================
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( chc[i] > 1 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( SCH_PM_IS_PICKING( i ) ) return FALSE; // 2014.01.10
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return FALSE;
		}
	}
	//========================================================================================
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_LAST_OPERATION_AT_MDL( int chamber ) { // 2005.03.29
	int Side , Pointer;
	int pmslot;
	//
//	if ( _SCH_MODULE_Get_PM_WAFER( chamber , 0 ) <= 0 ) return FALSE; // 2014.01.10
	if ( !SCH_PM_HAS_PICKING( chamber , &pmslot ) ) return FALSE; // 2014.01.10
	//
//	Pointer = _SCH_MODULE_Get_PM_POINTER( chamber , 0 ); // 2014.01.10
	Pointer = _SCH_MODULE_Get_PM_POINTER( chamber , pmslot ); // 2014.01.10
	if ( Pointer >= 100 ) return TRUE;
//	Side    = _SCH_MODULE_Get_PM_SIDE( chamber , 0 ); // 2014.01.10
	Side    = _SCH_MODULE_Get_PM_SIDE( chamber , pmslot ); // 2014.01.10
	if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) <= _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) return TRUE;
	return FALSE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( int tmatm , int trgch ) { // 2015.02.03
	int s , p , pd , k;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 0 ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( tmatm , 0 );
		p = _SCH_MODULE_Get_TM_POINTER( tmatm , 0 );
	}
	else if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 1 ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( tmatm , 1 );
		p = _SCH_MODULE_Get_TM_POINTER( tmatm , 1 );
	}
	else {
		return FALSE;
	}
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
	//
	if ( pd >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		if ( trgch == SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k ) ) return TRUE;
		//
	}
	//
	return FALSE;
}

int SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE( int tmatm , int Count , int getch , int getslot , int side , int pointer , int pathDo , int trgch , BOOL check7 ) { // 2006.07.21
	//
	// 2015.02.03
	//
	switch ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( getch ) ) {
	case 1 :
	case 2 :
	case 3 :
	case 4 :
	case 5 :
		//
		if ( SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( tmatm , trgch ) ) return FALSE; // 2015.02.03
		//
		break;
	case 7 :
		if ( check7 ) {
			//
			if ( SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( tmatm , trgch ) ) return FALSE; // 2015.02.03
			//
		}
		break;
	case 8 :
	case 9 :
	case 10 :
		if ( !SCHEDULING_CHECK_LAST_OPERATION_AT_MDL( trgch ) ) {
			//
			if ( SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( tmatm , trgch ) ) return FALSE; // 2015.02.03
			//
		}
		break;
	case 11 :
	case 12 :
	case 13 :
		if ( Count != 2 ) {
			//
			if ( SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( tmatm , trgch ) ) return FALSE; // 2015.02.03
			//
		}
		break;
	}
	//================================================================================================
	switch ( _SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( trgch ) ) {
	case 1 :
	case 2 :
	case 3 :
		//
		if ( SCHEDULING_CHECK_Arm_Has_Next_GO_TRGCH( tmatm , trgch ) ) return FALSE; // 2015.02.03
		//
		break;
	}
	//================================================================================================
	//================================================================================================
	//================================================================================================
	//================================================================================================
	switch ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( getch ) ) {
	case 1 :
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		//
		break;
	case 2 :
//		if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
		if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
		else {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//				if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
				if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
			}
		}
		break;
	case 3 :
//		if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
		if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
		else {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//				if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
				if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
			}
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		break;
	case 4 :
	case 5 :
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) <= 0 ) { // 2014.01.10
			if ( SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) { // 2014.01.10
				return FALSE;
			}
		}
		break;
	case 7 :
		if ( check7 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
			else {
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//					if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
				}
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
			if ( Count == 1 ) {
				if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Next_Chamber( tmatm , trgch ) ) return FALSE;
			}
		}
		break;
	case 8 :
		if ( !SCHEDULING_CHECK_LAST_OPERATION_AT_MDL( trgch ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		}
		break;
	case 9 :
		if ( !SCHEDULING_CHECK_LAST_OPERATION_AT_MDL( trgch ) ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
			else {
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//					if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
				}
			}
		}
		break;
	case 10 :
		if ( !SCHEDULING_CHECK_LAST_OPERATION_AT_MDL( trgch ) ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
			else {
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//					if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
				}
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		}
		break;
	case 11 :
		if ( Count != 2 ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		}
		break;
	case 12 :
		if ( Count != 2 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
			else {
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//					if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
				}
			}
		}
		break;
	case 13 :
		if ( Count != 2 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
			else {
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//					if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
				}
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		}
		break;
	case 14 :
		if ( Count != 2 ) {
			if ( tmatm == 0 ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
			}
			else {
				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( tmatm , -1 , trgch , 1 , SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) != 0 ) return FALSE;
			}
		}
		break;
	case 15 :
		if ( Count != 2 ) {
			if ( tmatm == 0 ) {
//				if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
				if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
				else {
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//						if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
						if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
					}
				}
			}
			else {
				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( tmatm , -1 , trgch , 1 , SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) != 0 ) return FALSE;
			}
		}
		break;
	case 16 :
		if ( Count != 2 ) {
			if ( tmatm == 0 ) {
//				if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
				if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
				else {
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//						if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
						if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
					}
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
			}
			else {
				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( tmatm , -1 , trgch , 1 , SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) != 0 ) return FALSE;
			}
		}
		break;
	case 18 : // 2007.12.12

//printf( "====================[A]==\n" );

		if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO2 ) { // 2011.11.29
			//
			if ( ( Count == 1 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tmatm , 0 ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( tmatm , 1 ) > 0 ) ) ) return FALSE;
			//
		}

//printf( "====================[B][PM%d]==\n" , trgch - PM1 + 1 );
//{
//	int res;
//		res = SCHEDULING_CHECK_NonTM_Wafer_PM_Next_Place_Switch_Possible( tmatm , getch , getslot , trgch );
//printf( "====================[C][BM%d][%d][res=%d]==\n" , getch - BM1 + 1 , getslot , res );
//		if ( res == 0 ) return FALSE;

		if ( SCHEDULING_CHECK_NonTM_Wafer_PM_Next_Place_Switch_Possible( tmatm , getch , getslot , side , pointer , trgch , ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) ) == 0 ) return FALSE;

//}
		break;
	}
	//================================================================================================
	switch ( _SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( trgch ) ) {
	case 1 :
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		break;
	case 2 :
//		if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
		if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
		else {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//				if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
				if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
			}
		}
		break;
	case 3 :
//		if ( _SCH_MODULE_Get_PM_WAFER( trgch , 0 ) > 0 ) return FALSE; // 2014.01.10
		if ( !SCH_PM_IS_ABSENT( trgch , side , pointer , pathDo ) ) return FALSE; // 2014.01.10
		else {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trgch ) >= 2 ) {
//				if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != tmatm ) ) return FALSE; // 2007.01.02
				if ( SCHEDULER_Get_MULTI_GROUP_PLACE( trgch ) != -1 ) return FALSE; // 2007.01.02
			}
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( trgch ) > 0 ) return FALSE;
		break;
	}
	//================================================================================================
	return TRUE;
}
//=======================================================================================
int SCHEDULING_CHECK_for_PICKPLACE_from_PM_NEXT_FULLY_REMAP( BOOL pickmode , int TMATM , int Side , int Pointer , int ch ) { // 2006.02.02
	int k , d , ChkTrg , offset;

	if ( SCHEDULER_Get_FULLY_RETURN_MODE() == 0 ) return -1;

	if ( ch != -1 ) {
		if ( SCHEDULER_Get_FULLY_RETURN_CH( ch ) == 1 ) return 0;
	}
	//
	d = 0;
	//
	offset = pickmode ? 0 : 1;
	//
	//=====================================================================================
	// 2006.07.10
	//=====================================================================================
	if ( pickmode ) {
		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == 1 ) { // First Chamber
			if ( SCHEDULER_Get_FULLY_RETURN_CH( ch ) == 2 ) { // 2006.07.20
				_SCH_CLUSTER_Inc_PathDo( Side , Pointer );
			}
		}
	}
	//=====================================================================================

	while ( TRUE ) {
		if ( ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - offset ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			if ( d != 0 ) return 0;
			return -5;
		}
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - offset , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
						if ( SCHEDULER_Get_FULLY_RETURN_CH( ChkTrg ) != 1 ) {
							if ( d != 0 ) return 0;
							return -6;
						}
					}
				}
			}
		}
		d++;
		//=============================================
		_SCH_CLUSTER_Inc_PathDo( Side , Pointer );
		//=============================================
	}
	return 0;
}
//=======================================================================================

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//=======================================================================================
/*
//
2008.11.01
int SCHEDULING_CHECK_for_Enable_PICK_from_FM( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_HSide , int *FM_HSide2 , BOOL TM_Double , int Single_Mode , BOOL *cpreject , int *returnpointer ) {
	int fh , fh2 , op , op2;
	int single , xr;

	*cpreject = FALSE; // 2004.05.11

	if ( !_SCH_SUB_Check_Complete_Return( side ) ) return -3;
	if ( !_SCH_SUB_Remain_for_FM( side ) ) return -3;

	*returnpointer = _SCH_MODULE_Get_FM_DoPointer( side ); // 2004.05.11

	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_FM_DoPointer( side ) , 0 ) == 0 ) {
		*cpreject = TRUE; // 2004.05.11
		return -21;
	}

	if ( !TM_Double ) {
		xr = 0; // 2006.02.07
		_SCH_MODULE_Set_FM_HWait( side , TRUE );
		*FM_HSide  = 0;
		*FM_HSide2 = 0;
//		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2007.08.28
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( Single_Mode != 2 ) ) { // 2007.08.28
			if ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2006.02.07
				//=========================================================================
				// 2006.02.07
				//=========================================================================
//				if ( Single_Mode ) { // 2007.08.28
				if ( Single_Mode != 0 ) { // 2007.08.28
					xr = 1;
				}
				else {
					*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
					*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
					//
					fh			 = _SCH_CLUSTER_Get_PathIn( side , *FM_Pointer );
					//
					op = 0;
					op2 = *FM_Pointer + 1;
					//
					while( TRUE ) {
						while ( _SCH_SUB_Run_Impossible_Condition( side , op2 , -1 ) ) {
							op2++;
							if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) break;
						}
						if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) {
							xr = 1;
							break;
						}
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
								xr = 2;
								break;
							}
							if ( ( *FM_Slot + op + 1 ) != ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
								xr = 2;
								break;
							}
							op++;
							if ( op >= _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) break;
						}
						else { // 2006.04.25
							if ( ( ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) > *FM_Slot ) &&
								( ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) <= ( *FM_Slot + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) ) ) {
								if ( _SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
									xr = 2;
									break;
								}
							}
						}
						op2++;
					}
				}
				//
				if ( xr != 0 ) {
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
						*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
						_SCH_MODULE_Inc_FM_DoPointer( side );
						*FM_Slot  = 0;
					}
					else {
						if ( xr == 2 ) {
							return -21;
						}
						else {
							_SCH_MODULE_Inc_FM_DoPointer( side );
							*FM_Slot2 = 0;
						}
					}
				}
				else {
					_SCH_MODULE_Inc_FM_DoPointer( side );
					*FM_Slot2 = 0;
				}
				//
				xr = 1;
				//=========================================================================
			}
			else {
				*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
				*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
				_SCH_MODULE_Inc_FM_DoPointer( side );
				*FM_Slot2 = 0;
				//
				if ( Single_Mode != 0 ) xr = 1; // 2007.08.28
				//
			}
		}
//		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2007.08.28
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( Single_Mode != 1 ) ) { // 2007.08.28
			*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
			*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
			_SCH_MODULE_Inc_FM_DoPointer( side );
			*FM_Slot  = 0;
			//
			if ( Single_Mode != 0 ) xr = 1; // 2007.08.28
			//
		}
		else {
			return -4;
		}
		//===============================================================================
		// Change location // 2003.06.05
		//===============================================================================
		if ( !_SCH_SUB_Remain_for_FM( side ) ) return 3; // 2006.11.28
		//===============================================================================
//		if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 1; // 2006.02.07
		if ( ( xr != 0 ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 1; // 2006.02.07
//		if ( Single_Mode ) return 6; // 2007.08.28
		if ( Single_Mode != 0 ) return 6; // 2007.08.28
		//===============================================================================
		if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_FM_DoPointer( side ) , 0 ) == 0 ) return 3;
		//===============================================================================
		if ( _SCH_CLUSTER_Get_PathIn( side , *FM_Pointer ) == _SCH_CLUSTER_Get_PathIn( side , _SCH_MODULE_Get_FM_DoPointer( side ) ) ) {
			*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
			*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
			_SCH_MODULE_Inc_FM_DoPointer( side );
			//======================================================================================
			// 2003.06.05
			//======================================================================================
			_SCH_SUB_Remain_for_FM( side );
			//======================================================================================
		}
		return 4;
	}
	else {
		single = FALSE;
		op	= _SCH_MODULE_Get_FM_DoPointer( side );
		fh = _SCH_CLUSTER_Get_PathHSide( side , op );
		_SCH_MODULE_Inc_FM_DoPointer( side );

		if ( _SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) { // 2003.01.03
			single = TRUE; // 2003.01.03
		} // 2003.01.03
		else {
			while ( _SCH_SUB_Run_Impossible_Condition( side , _SCH_MODULE_Get_FM_DoPointer( side ) , -1 ) ) {
				_SCH_MODULE_Inc_FM_DoPointer( side );
				if ( _SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) {
					single = TRUE;
					break;
				}
				if ( _SCH_CLUSTER_Get_PathHSide( side , _SCH_MODULE_Get_FM_DoPointer( side ) ) != HANDLING_ALL_SIDE ) {
					single = TRUE;
				}
			}
		}
		//=============================================================================================
		// 2007.07.10
		//=============================================================================================
//		if ( Single_Mode ) { // 2007.08.28
		if ( ( Single_Mode == 1 ) || ( Single_Mode == 3 ) ) { // 2007.08.28
			*FM_Pointer		= op;
			*FM_Slot        = _SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide		= fh;
			*FM_Slot2		= 0;
			_SCH_MODULE_Set_FM_HWait( side , FALSE );
			return 11;
		}
		else if ( Single_Mode == 2 ) { // 2007.08.28
			*FM_Pointer2	= op;
			*FM_Slot2       = _SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide2		= fh;
			*FM_Slot		= 0;
			_SCH_MODULE_Set_FM_HWait( side , FALSE );
			return 11;
		}
		//=============================================================================================
		if ( _SCH_MODULE_Get_FM_DoPointer( side ) < MAX_CASSETTE_SLOT_SIZE ) {
			op2 = _SCH_MODULE_Get_FM_DoPointer( side );
			fh2 = _SCH_CLUSTER_Get_PathHSide( side , op2 );
			if      ( fh2 == HANDLING_B_SIDE_ONLY ) {
				single = TRUE;
			}
			else if ( fh2 == HANDLING_A_SIDE_ONLY ) {
				single = TRUE;
			}
		}
		else {
			if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_FM_DoPointer( side ) , 0 ) == 0 ) single = TRUE;
		}
		//
		if      ( fh == HANDLING_B_SIDE_ONLY ) {
			single = TRUE;
		}
		else if ( fh == HANDLING_A_SIDE_ONLY ) {
			single = TRUE;
		}
		else if ( fh == HANDLING_END_SIDE ) {
			if ( _SCH_MODULE_Get_FM_HWait( side ) ) {
				fh = HANDLING_B_SIDE_ONLY;
			}
			else {
				fh = HANDLING_A_SIDE_ONLY;
			}
			single = TRUE;
		}
		else {
			if ( _SCH_MODULE_Get_FM_HWait( side ) ) {
				fh = HANDLING_B_SIDE_ONLY;
				single = TRUE;
			}
			else {
				fh = HANDLING_A_SIDE_ONLY;
			}
		}

		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _SCH_CLUSTER_Get_SlotIn( side , op ) ) ) {
				_SCH_MODULE_Set_FM_HWait( side , FALSE );
				*FM_Pointer2	= op;
				*FM_Slot2       = _SCH_CLUSTER_Get_SlotIn( side , op );
				*FM_HSide2		= fh;
				if ( single ) {
					*FM_Slot	= 0;
				}
				else {
					if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
						*FM_Slot		= 0;
						_SCH_MODULE_Set_FM_HWait( side , TRUE );
					}
					else {
						*FM_Pointer	= op2;
						*FM_Slot	= _SCH_CLUSTER_Get_SlotIn( side , op2 );
						*FM_HSide	= HANDLING_B_SIDE_ONLY;
						_SCH_MODULE_Inc_FM_DoPointer( side );
					}
				}
			}
			else {
				_SCH_MODULE_Set_FM_HWait( side , FALSE );
				*FM_Pointer		= op;
				*FM_Slot        = _SCH_CLUSTER_Get_SlotIn( side , op );
				*FM_HSide		= fh;
				if ( single ) {
					*FM_Slot2	= 0;
				}
				else {
					if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
						*FM_Slot2		= 0;
						_SCH_MODULE_Set_FM_HWait( side , TRUE );
					}
					else {
						*FM_Pointer2	= op2;
						*FM_Slot2	    = _SCH_CLUSTER_Get_SlotIn( side , op2 );
						*FM_HSide2		= HANDLING_B_SIDE_ONLY;
						_SCH_MODULE_Inc_FM_DoPointer( side );
					}
				}
			}
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			*FM_Pointer		= op;
			*FM_Slot        = _SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide		= fh;
			*FM_Slot2		= 0;
			if ( single ) 	_SCH_MODULE_Set_FM_HWait( side , FALSE );
			else			_SCH_MODULE_Set_FM_HWait( side , TRUE );
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			*FM_Pointer2	= op;
			*FM_Slot2	    = _SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide2		= fh;
			*FM_Slot		= 0;
			if ( single ) 	_SCH_MODULE_Set_FM_HWait( side , FALSE );
			else			_SCH_MODULE_Set_FM_HWait( side , TRUE );
		}
	}
	return 5;
}
*/
//=======================================================================================
BOOL SCHEDULING_CHECK_for_MOVE_ENABLE( int TMATM , int Wantgo , int WantgoNext , int CurrChamber ) {
	int i , j , m , n , Side2 , Pointer2 , ChkSrc2 , ChkTrg2;
	BOOL dono , find;
	int pmslot;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
//		if ( ( CurrChamber != i ) && ( _SCH_PRM_GET_MODULE_xGROUP( i ) == TMATM ) ) {

		if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) ) ) continue; // 2008.09.18

		if ( ( CurrChamber != i ) && ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) ) {
			//
			if ( SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) { // 2014.01.10
				//
//				Side2    = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//				Pointer2 = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
				//
				Side2    = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2014.01.10
				Pointer2 = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2014.01.10
				//
//				if ( ( Pointer2 >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) ) { // 2014.01.10
				if ( ( Pointer2 >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , pmslot ) > 0 ) ) { // 2014.01.10
					//
					if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
						dono = TRUE;
						find = FALSE;
						ChkSrc2 = i;
						for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
							ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) , n );
							//-------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
							//-------------------------------------------------------------------------------------
							if ( ChkTrg2 > 0 ) {
								//if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
									//
									if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) ) ) break; // 2008.09.18
									//
									if ( ( WantgoNext != ChkSrc2 ) || ( Wantgo != ChkTrg2 ) ) dono = FALSE;
									find = TRUE;
								}
							}
						}
						if ( dono && find ) return FALSE;
						if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) + 1 < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
							for ( j = _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) + 1 ; j < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ; j++ ) {
								dono = TRUE;
								find = FALSE;
								for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
									ChkSrc2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j - 1 , m );
									//-------------------------------------------------------------------------------------
									SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc2 ); // 2007.10.29
									//-------------------------------------------------------------------------------------
									if ( ChkSrc2 > 0 ) {
										//if ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc2 ) == TMATM ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc2 , G_PLACE ) ) {
											//
											if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) {
												j = 9999; // 2008.09.18
												break; // 2008.09.18
											}
											//
											for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
												ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j , n );
												//-------------------------------------------------------------------------------------
												SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
												//-------------------------------------------------------------------------------------
												if ( ChkTrg2 > 0 ) {
													//if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
														//
														if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) ) ) break; // 2008.09.18
														//
														if ( ( WantgoNext != ChkSrc2 ) || ( Wantgo != ChkTrg2 ) ) dono = FALSE;
														find = TRUE;
													}
												}
											}
										}
									}
								}
								if ( dono && find ) return FALSE;
							}
						}
					}
				}
			}
		}
	}
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
		Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		if ( ( Pointer2 >= 0 ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , i ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) > 0 ) ) {
			if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
				for ( j = _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 ; j < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ; j++ ) {
					if ( j > 0 ) {
						dono = TRUE;
						find = FALSE;
						for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
							ChkSrc2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j - 1 , m );
							//-------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc2 ); // 2007.10.29
							//-------------------------------------------------------------------------------------
							if ( ChkSrc2 > 0 ) {
								//if ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc2 ) == TMATM ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc2 , G_PLACE ) ) {
									//
									if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) {
										j = 9999; // 2008.09.18
										break; // 2008.09.18
									}
									//
									for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
										ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j , n );
										//-------------------------------------------------------------------------------------
										SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
										//-------------------------------------------------------------------------------------
										if ( ChkTrg2 > 0 ) {
											//if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
												//
												if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) ) ) break; // 2008.09.18
												//
												if ( ( WantgoNext != ChkSrc2 ) || ( Wantgo != ChkTrg2 ) ) dono = FALSE;
												find = TRUE;
											}
										}
									}
								}
							}
						}
						if ( dono && find ) return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NOT_CROSS_OVER( int TMATM , int CurrChamber , int Side , int Pointer , int freearmcount ) {
	int i , j , m , n , Side2 , Pointer2 , ChkSrc2 , ChkTrg2 , t;
	BOOL dono , find , nch;
	int xdone = 0;
	int pmslot;

	//------------------------------------------------------------------------------------------------
	// 2012.10.30
	//------------------------------------------------------------------------------------------------
	if ( CROSS_OVER_SIMPLE_CHECK ) {
		if ( freearmcount >= 2 ) return TRUE;
	}
	//------------------------------------------------------------------------------------------------

	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//------------------------------------------------------------------------------------------------
//		if ( _SCH_PRM_GET_MODULE_xGROUP( i ) != TMATM ) continue; // 2002.05.30
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;

		//------------------------------------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) ) ) continue; // 2008.07.16

		//------------------------------------------------------------------------------------------------
		// 2002.05.28
		//------------------------------------------------------------------------------------------------
		nch = FALSE;
		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , j );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( ChkTrg2 > 0 ) { // 2008.07.16
					//
//					if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) ) ) continue; // 2008.07.16
					//
					if ( ChkTrg2 == i ) {
	//						if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
							if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) >= 2 ) ) ) break; // 2008.09.18
							//
							nch = TRUE;
							//
							break;
						}
					}
				}
			}
		}
		//------------------------------------------------------------------------------------------------
		if ( CurrChamber != i ) {
			//
//			Side2    = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//			Pointer2 = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
			//
			if ( SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) { // 2014.01.10
				Side2    = _SCH_MODULE_Get_PM_SIDE( i , pmslot );
				Pointer2 = _SCH_MODULE_Get_PM_POINTER( i , pmslot );
			}
			else { // 2014.01.10
				Pointer2 = -1;
			}
			//
			if ( ( Pointer2 >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) ) {
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
					//------------------------------------------------------------------------------------------------
					// 2012.10.30
					//------------------------------------------------------------------------------------------------
					if ( CROSS_OVER_SIMPLE_CHECK ) {
						t = _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) + 1;
					}
					else {
						t = _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 );
					}
					//------------------------------------------------------------------------------------------------
//					for ( j = _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) ; j < _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ; j++ ) { // 2012.10.30
					for ( j = _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) ; j < t ; j++ ) { // 2012.10.30
						dono = TRUE;
						find = FALSE;
						if ( j == _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) ) { // 2002.05.25
							//
							ChkSrc2 = i;
//								if ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc2 ) == TMATM ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc2 , G_PICK ) ) {
								//
//								if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) continue; // 2008.07.16
								if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) break; // 2008.09.18
								//
								for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
//									ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j , n );
									ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j , n );		// 2008.12.12 Hakjun Kim
									//-------------------------------------------------------------------------------------
									SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
									//-------------------------------------------------------------------------------------
									if ( ChkTrg2 > 0 ) {
//											if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
											if ( nch && ( xdone != 1 ) ) { // 2002.05.28
//													if ( _SCH_CLUSTER_Check_HasProcessData_Post( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , _SCH_MODULE_Get_PM_PATHORDER( i , 0 ) ) ) { // 2002.05.28 // 2006.01.06
												if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) { // 2007.01.15
													xdone = 1;
												}
												else {
													if ( CROSS_OVER_SIMPLE_CHECK ) { // 2012.10.31
														xdone = 1;
													}
													else {
														if ( _SCH_CLUSTER_Check_HasProcessData_Post( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , _SCH_MODULE_Get_PM_PATHORDER( i , 0 ) ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , i ) ) { // 2002.05.28 // 2006.01.06
															if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PICK ) ) { // 2006.01.29
																if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg2 , 0 ) > 0 ) {
																	xdone = 2;
																}
																else {
																	xdone = 1;
																}
															}
															else { // 2006.01.29
																xdone = 1; // 2006.01.29
															}
														}
														else {
															xdone = 1;
														}
													}
												}
											}
											//
											if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) || ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) < 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) < 2 ) ) ) { // 2008.07.16
												if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , ChkSrc2 , ChkTrg2 , CurrChamber ) ) dono = FALSE;
											}
											else {
												dono = FALSE;
											}
											//
											find = TRUE;
										}
									}
								}
							}
						}
						else {
							for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
								ChkSrc2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j - 1 , m );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc2 ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( ChkSrc2 > 0 ) {
//										if ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc2 ) == TMATM ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc2 , G_PLACE ) ) {
										//
//										if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) continue; // 2008.07.16
										if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) && ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkSrc2 ) >= 2 ) ) ) {
											j = 9999; // 2008.09.18
											break; // 2008.09.18
										}
										//
										for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
//											ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , j , n );
											ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j , n );		// 2008.12.12 Hakjun Kim
											//-------------------------------------------------------------------------------------
											SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
											//-------------------------------------------------------------------------------------
											if ( ChkTrg2 > 0 ) {
//													if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg2 ) == TMATM ) {
												if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg2 , G_PLACE ) ) {
													//
													if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) || ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) < 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg2 ) < 2 ) ) ) { // 2008.07.16
														if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , ChkSrc2 , ChkTrg2 , CurrChamber ) ) dono = FALSE;
													}
													else {
														dono = FALSE; // 2008.07.16
													}
													//
													find = TRUE;
												}
											}
										}
									}
								}
							}
						}
						if ( dono && find ) return FALSE;
					}
				}
				else { // 2002.05.28
					if ( nch && ( xdone != 1 ) ) { // 2002.05.28
						xdone = 1;
					}
				}
			}
//			else if ( ( Pointer2 >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) ) { // 2002.05.28 // 2014.01.11
			else { // 2002.05.28 // 2014.01.11
				if ( nch && ( xdone != 1 ) ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
						xdone = 1;
					}
				}
			}
		}
	}
	if ( xdone == 2 ) return FALSE; // 2002.05.28
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( int TMATM , int Side , int Pointer , int freearmcount ) {
	int j , k , l , ChkSrc , ChkTrg , t;
	BOOL dono , find;
	//------------------------------------------------------------------------------------------------
	// 2012.10.30
	//------------------------------------------------------------------------------------------------
	if ( CROSS_OVER_SIMPLE_CHECK ) {
		if ( freearmcount >= 2 ) return TRUE;
	}
	//------------------------------------------------------------------------------------------------
	if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) > 1 ) {
		//
		// 2012.10.30
		//
		if ( CROSS_OVER_SIMPLE_CHECK ) {
			t = 1;
		}
		else {
			t = _SCH_CLUSTER_Get_PathRange( Side , Pointer ) - 1;
		}
		//
//		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) - 1 ; j++ ) { // 2012.10.30
		for ( j = 0 ; j < t ; j++ ) { // 2012.10.30
			dono = TRUE;
			find = FALSE;
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j , k );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( ChkSrc > 0 ) {
//						if ( ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc ) == TMATM ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) {
//					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) { // 2014.01.10
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( SCH_PM_IS_ABSENT( ChkSrc , Side , Pointer , j ) ) ) { // 2014.01.10
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkSrc ) <= 0 ) { // 2002.05.25
							for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
								ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j + 1 , l );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( ChkTrg > 0 ) {
//										if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg ) == TMATM ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
										if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , ChkSrc , ChkTrg , -1 ) ) dono = FALSE;
										find = TRUE;
									}
								}
							}
						}
						else { // 2002.05.25 // for ATTO 200mm Problem
							find = TRUE; // 2002.05.25
						}
					}
				}
			}
			if ( dono && find ) return FALSE;
		}
	}
	//--------------------------------------------------------------
	// 2001.07.27 Single Buffer , Single PM Deadlock Point (insert)
	//--------------------------------------------------------------
	// Begin
	else {
		dono = TRUE;
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , 0 , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkSrc > 0 ) {
//					if ( ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc ) == TMATM ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) {
//				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) { // 2014.01.10
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( SCH_PM_IS_ABSENT( ChkSrc , Side , Pointer , 0  ) ) ) { // 2014.01.10
					dono = FALSE;
				}
			}
		}
		if ( dono ) return FALSE;
	}
	// End
	//--------------------------------------------------------------
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NOT_CROSS_OVER_WHEN_PICK_BM_ONEFREE_SINGLEMODE( int TMATM , int Side , int Pointer ) {
	//--------------------------------------------------------------
	// 2001.07.01 Check Speed (Seperate & insert Function)
	//--------------------------------------------------------------
	int j , k , l , ChkSrc , ChkTrg;
	BOOL dono , find;
	if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) > 1 ) {
		j = 0;
//		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) - 1 ; j++ ) {
			dono = TRUE;
			find = FALSE;
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j , k );
				//-------------------------------------------------------------------------------------
				if ( ChkSrc > 0 ) {
//					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) { // 2014.01.10
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( SCH_PM_IS_ABSENT( ChkSrc , Side , Pointer , j ) ) ) { // 2014.01.10
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkSrc ) <= 0 ) { // 2002.05.25
							for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
								ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , j + 1 , l );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( ChkTrg > 0 ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
										if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , ChkSrc , ChkTrg , -1 ) ) dono = FALSE;
										find = TRUE;
									}
								}
							}
						}
						else { // 2002.05.25 // for 200mm Problem
							find = TRUE; // 2002.05.25
						}
					}
				}
			}
			if ( dono && find ) return FALSE;
//		}
	}
	//--------------------------------------------------------------
	// 2001.07.27 Single Buffer , Single PM Deadlock Point (insert)
	//--------------------------------------------------------------
	// Begin
	else {
		dono = TRUE;
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , 0 , k );
			//-------------------------------------------------------------------------------------
			if ( ChkSrc > 0 ) {
//				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( _SCH_MODULE_Get_PM_WAFER( ChkSrc , 0 ) <= 0 ) ) { // 2014.01.10
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) && ( SCH_PM_IS_ABSENT( ChkSrc , Side , Pointer , 0 ) ) ) { // 2014.01.10
					dono = FALSE;
				}
			}
		}
		if ( dono ) return FALSE;
	}
	// End
	//--------------------------------------------------------------
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NOT_CROSS_OVER_WHEN_PICK_CM_DUALFREE( int TMATM ) { // 2004.12.20
	int i , Side , Pointer;
	int pmslot;
	/*
	// 2014.01.10
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
//		if ( _SCH_PRM_GET_MODULE_xGROUP( i ) != TMATM ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		Side    = _SCH_MODULE_Get_PM_SIDE( i , 0 );
		Pointer = _SCH_MODULE_Get_PM_POINTER( i , 0 );
		if ( Pointer < 100 ) {
			//
			if ( ( Pointer >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , Side ) ) {
						if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , Side ) ) {
							return FALSE;
						}
					}
				}
			}
		}
	}
	*/
	//
	// 2014.01.10
	//
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		//
		if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue;
		//
		Side    = _SCH_MODULE_Get_PM_SIDE( i , pmslot );
		Pointer = _SCH_MODULE_Get_PM_POINTER( i , pmslot );
		//
		if ( Pointer < 100 ) {
			//
			if ( ( Pointer >= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( i , pmslot ) > 0 ) ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , Side ) ) {
						if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , Side ) ) {
							return FALSE;
						}
					}
				}
			}
		}
	}
	//
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_PM_ALL_FREE( int TMATM , int placepm ) {
	int i , tdata , cdata;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
		// Double Arm Part
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
	//		if ( _SCH_PRM_GET_MODULE_xGROUP( i ) == TMATM ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PLACE ) ) {
				//
//				if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) return FALSE; // 2014.01.10
				if ( !SCH_PM_IS_EMPTY( i ) ) return FALSE; // 2014.01.10
				//
			}
		}
	}
	else { // 2008.06.11
		// single arm일때...
//		_SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( placepm , FALSE , &tdata , -1 );           //  comment 2008.06.18
		tdata = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1, placepm );
		//
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
			if ( i == placepm ) continue;

			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PLACE ) ) {
				// Place할 Chamber가 아닌 다른 Chamber에 Wafer가 있는지 확인한다.

				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) { // 2008.06.26
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
					if ( !SCH_PM_IS_EMPTY( i ) ) { // 2014.01.10
						//
						// Place할 Chamber가 아닌 다른 Chamber의 공정이 끝나있으면 BM에서 Pick을 하지 않는다.
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 )	
							return FALSE;					

						// 
						// _SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( i , TRUE , &cdata , -1 );          //  comment 2008.06.18
						cdata = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0, i );
						if ( cdata < tdata ) return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID2( int TMATM , int CurrChamber , int NextChamber ) {
	int s , p , n;
	int pmslot;
	//
//	if ( _SCH_MODULE_Get_PM_WAFER( CurrChamber , 0 ) <= 0 ) return TRUE; // 2014.01.10
	//
	if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , NextChamber , G_PLACE ) ) return TRUE;
	//
	if ( !SCH_PM_HAS_ONLY_PICKING( CurrChamber , &pmslot ) ) return TRUE; // 2014.01.10
	//
//	s = _SCH_MODULE_Get_PM_SIDE( CurrChamber , 0 ); // 2014.01.10
//	p = _SCH_MODULE_Get_PM_POINTER( CurrChamber , 0 ); // 2014.01.10
	//
	s = _SCH_MODULE_Get_PM_SIDE( CurrChamber , pmslot ); // 2014.01.10
	p = _SCH_MODULE_Get_PM_POINTER( CurrChamber , pmslot ); // 2014.01.10
	//
	if ( p >= 100 ) return FALSE;
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE; // 2007.11.05
//	if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE; // 2007.11.03 // 2007.11.05
	//
	for ( n = 0 ; n < MAX_PM_CHAMBER_DEPTH ; n++ ) {
		if ( NextChamber == SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , n ) ) return FALSE;
	}
	return TRUE;
}


BOOL SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( int TMATM , int GetChamber , int NextBMChamber , int JumpMode , BOOL swithmode , int spcnt , int getcnt ) {
	int Side , Pointer , i , j , ChkTrg , c , f , d;
	if ( JumpMode != 1 ) return FALSE;
	if ( GetChamber == NextBMChamber ) return FALSE;
	//
	if ( spcnt != -1 ) { // 2008.06.20
		if ( spcnt > getcnt ) {
			if ( ( GetChamber >= BM1 ) && ( NextBMChamber >= BM1 ) ) return FALSE;
		}
	}
	//
	if ( swithmode ) {
		Pointer = -1;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( NextBMChamber , -1 , &Pointer , &ChkTrg , swithmode , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( NextBMChamber ) ) , &Side ) <= 0 ) return FALSE;
		if ( ( Pointer < 0 ) || ( Pointer >= 100 ) ) return FALSE;
		if ( TMATM == 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return FALSE;
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) > _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return FALSE;
		}
		//
		c = 0;
		f = 0; // 2008.04.24
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( TMATM == 0 ) {
				ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , i ); // 2007.02.02
			}
			else {
				ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , i ); // 2007.02.02
			}
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				f++; // 2008.04.24
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
					if ( GetChamber == ChkTrg ) return FALSE; // 2007.02.05
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
					//
					if ( TMATM == 0 ) {
						if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) ) { // 2014.01.10
							return FALSE;
						}
						else { // 2007.05.22
							if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID2( TMATM , ChkTrg , GetChamber ) ) {
								return FALSE;
							}
						}
					}
					else {
						if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) { // 2014.01.10
							return FALSE;
						}
						else { // 2007.05.22
							if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID2( TMATM , ChkTrg , GetChamber ) ) {
								return FALSE;
							}
						}
					}
					//
				}
				else {
					c++;
				}
			}
		}
		if ( c > 0 ) return FALSE;
		if ( f == 0 ) return FALSE; // 2008.04.24
		//
		//
		// 2018.06.26
		//
		if ( TMATM == 0 ) {
			//
			c = _SCH_CLUSTER_Get_PathRange( Side , Pointer );
			//
			for ( f = ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 ) ; f < c ; f++ ) {
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , f , i );
					//
					//-------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg );
					//-------------------------------------------------------------------------------------
					if ( ChkTrg == GetChamber ) return FALSE;
					//
				}
			}
		}
		//
	}
	else {
		if ( spcnt != -1 ) { // 2008.06.25
			if ( spcnt >= getcnt ) {
				if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) && ( NextBMChamber >= BM1 ) ) {
					if ( TMATM != 0 ) return FALSE; // 2008.07.04
					if ( _SCH_MODULE_GET_USE_ENABLE( GetChamber , TRUE , -1 ) ) {
						if ( !_SCH_MODULE_GET_USE_ENABLE( GetChamber , FALSE , -1 ) ) {
							return FALSE;
						}
					}

				}
			}
		}
		//
		f = 0;
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( i == NextBMChamber ) continue;
			if ( i == GetChamber ) continue;
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_IN_MODE ) continue;
			Pointer = -1;
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , -1 , &Pointer , &ChkTrg , swithmode , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &Side ) <= 0 ) continue;
			if ( ( Pointer < 0 ) || ( Pointer >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
			f++;
			if ( TMATM == 0 ) {
				if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) continue;
			}
			else {
				if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) > _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) continue;
			}
			c = 0;
			//=================================================
			d = 0; // 2007.09.06
			//=================================================
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( TMATM == 0 ) {
					ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , j ); // 2007.02.02
				}
				else {
					ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , j ); // 2007.02.02
				}
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( ChkTrg > 0 ) {
					//=================================================
					d = 1; // 2007.09.06
					//=================================================
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
						if ( GetChamber == ChkTrg ) return FALSE; // 2007.02.05
//						if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
						//
						if ( TMATM == 0 ) {
							if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) ) { // 2014.01.10
								return FALSE;
							}
							else { // 2007.05.22
								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID2( TMATM , ChkTrg , GetChamber ) ) {
									return FALSE;
								}
							}
						}
						else {
							if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) { // 2014.01.10
								return FALSE;
							}
							else { // 2007.05.22
								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID2( TMATM , ChkTrg , GetChamber ) ) {
									return FALSE;
								}
							}
						}
					}
					else {
						c++;
					}
				}
			}
			//=================================================
			if ( d == 0 ) return FALSE; // 2007.09.06
			//=================================================
			if ( c > 0 ) return FALSE;
		}
		if ( f == 0 ) return FALSE;
		//
		//-----------------------------------------------------------------------------
/*
		if ( spcnt != -1 ) { // 2008.06.27
			if ( spcnt >= getcnt ) {
				if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) ) {
					return FALSE;
				}
			}
		}
*/
		//-----------------------------------------------------------------------------
		//
	}
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_for_NEXT_GO_READY( int TMATM , int Side , int Pointer , int Nextbm ) {
	int i , j , k , NextChamber , bmd;
	BOOL find = FALSE;
	BOOL Off = FALSE;

	if ( Nextbm == 0 ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) return FALSE;
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) return FALSE;
		}
		if ( ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , i );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &NextChamber ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( NextChamber > 0 ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , NextChamber , G_PLACE ) ) {
						find = TRUE;
//						if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( NextChamber ) <= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( NextChamber , 0 ) <= 0 ) ) { // 2014.01.10
						if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( NextChamber ) <= 0 ) && ( SCH_PM_IS_ABSENT( NextChamber , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) ) { // 2014.01.10
							Off = TRUE;
						}
					}
				}
			}
		}
		if ( find ) {
			return Off;
		}
		return TRUE;
	}
	else if ( Nextbm == 1 ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) return FALSE;
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) return FALSE;
		}
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
					bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	else if ( Nextbm == 2 ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_S_MODE ) {
					bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	else if ( Nextbm == 3 ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) {
					bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}
//=======================================================================================
int SCHEDULING_CHECK_for_Enable_PICK_from_CM( int TMATM , int CHECKING_SIDE , int *GetChamber , int *NextChamber , int *MovingType , int *MovingOrder , int *MovingFinger , int *ArmIntlks , BOOL Always , BOOL GetOutSkip , BOOL GetPickLast ) {
	int SchPointer , Count , k , m , n;

	if ( GetPickLast ) return -98; // 2004.11.23

	*MovingFinger = -1;
	*ArmIntlks = 0; // 2007.09.06
	//-----------------
	SchPointer = -1; // 2006.04.04

	//========================================================================================
	// 2006.04.04
	//========================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2006.04.04
		for ( m = 0 ; m < MAX_CASSETTE_SLOT_SIZE ; m++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , m ) >= CM1 ) { // 2011.01.18
				if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , m ) < PM1 ) {
					if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , m ) , FALSE , CHECKING_SIDE ) ) {
						continue;
					}
				}
			}
			//
			if      ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , m ) < 0 ) {
			}
			else if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , m ) > SCHEDULER_SUPPLY ) {
			}
			else {
				if ( _SCH_CLUSTER_Get_Optimize( CHECKING_SIDE , m ) != -1 ) {
					if ( SchPointer == -1 ) {
						SchPointer = m;
					}
					else {
						if ( _SCH_CLUSTER_Get_Optimize( CHECKING_SIDE , SchPointer ) > _SCH_CLUSTER_Get_Optimize( CHECKING_SIDE , m ) ) {
							SchPointer = m;
						}
					}
				}
			}
		}
	}
	//========================================================================================
	if ( SchPointer == -1 ) { // 2006.04.04
		for ( ; _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) < MAX_CASSETTE_SLOT_SIZE ; ) {
			if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) < 0 ) _SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
			else if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) > SCHEDULER_SUPPLY ) _SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
			else
				break;
		}
	}
	else { // 2006.04.04
		_SCH_MODULE_Set_TM_DoPointer( CHECKING_SIDE , SchPointer ); // 2006.04.04
	}
	//================================================================

	SchPointer = _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE );
	if ( SchPointer >= MAX_CASSETTE_SLOT_SIZE ) return -2;

	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 0 ) == 0 ) return -21;

	//-- 2002.03.06
	k = _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer )	);
	if ( k > 0 ) {
//		if ( _SCH_MODULE_Get_PM_WAFER( ( k - 1 ) + PM1 , 0 ) > 0 ) { // 2014.01.10
		if ( !SCH_PM_IS_FREE( ( k - 1 ) + PM1 ) ) { // 2014.01.10
			return -31;
		}
	}
	//================================================================================================================================
	// 2006.04.27
	//================================================================================================================================
	if ( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) ) >= 7 ) {
		if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) return -32;
	}
	//================================================================================================================================
	// 2004.11.23
	//================================================================================================================================
	Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM );
	if ( !GetOutSkip ) {
		if ( SCHEDULING_CHECK_Disable_PICK_from_CMBM_OUT_for_GETPR( TMATM , CHECKING_SIDE , SchPointer , Count ) ) return -99;
	}
	//================================================================================================================================
	if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) < 0 ) {
		return -3;
	}
	else if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) == 0 ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) , FALSE , -1 ) ) return -491; // 2019.01.08
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) , G_PICK ) ) return -492; // 2019.01.08
		//
//		Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ); // 2004.11.23
		if ( Count == 0 ) return -4;
		if ( Count == 1 ) { // 2004.01.30
			switch( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) ) ) {
			case 1 :
			case 3 :
			case 4 :
			case 6 :
				return -44;
				break;
			}
		}
		*MovingOrder = 0;
		*GetChamber  = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer );
		*NextChamber = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , SchPointer );
		*MovingType  = SCHEDULER_MOVING_OUT;
		return 1;
	}
	else {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) , FALSE , -1 ) ) return -491; // 2019.01.08
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) , G_PICK ) ) return -492; // 2019.01.08
		//
		if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , SchPointer ) > SCHEDULER_SUPPLY ) return -5;
//		Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ); // 2004.11.23
		if ( Count == 0 ) return -6;
		if ( Count == 1 ) { // 2004.01.30
			switch( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer ) ) ) {
			case 1 :
			case 3 :
			case 4 :
			case 6 :
				return -44;
				break;
			}
		}
		//
		if ( !Scheduler_Deadlock_Rounding_Possible_Before_CM_Pick_for_SingleArm( CHECKING_SIDE , SchPointer ) ) { // 2018.07.12
			return -441;
		}
		//
		*MovingOrder = 0;
		*GetChamber  = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , SchPointer );
		*MovingType  = SCHEDULER_MOVING_IN;
		//
		//=====================================================================================================================================
		// 2004.12.14
		//=====================================================================================================================================
		if ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( *GetChamber ) == 7 ) { // 2004.12.14 // 2004.12.28
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_7( 0 , TMATM , 0 , Count , CHECKING_SIDE , SchPointer ) ) { // 2006.07.13
				return -201;
			}
		}
		else if ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( *GetChamber ) == 17 ) { // 2006.07.13
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_17( 0 , TMATM , 0 , Count , CHECKING_SIDE , SchPointer ) ) { // 2006.07.13
				return -202;
			}
		}
		//=====================================================================================================================================
		// 2004.12.20
		//=====================================================================================================================================
		if ( Count == 2 ) {
			if ( !SCHEDULING_CHECK_for_NOT_CROSS_OVER_WHEN_PICK_CM_DUALFREE( TMATM ) ) return -205;
		}
		//=====================================================================================================================================
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			*MovingFinger = -1;
			*MovingOrder = k;
			*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , SchPointer , 0 , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( NextChamber ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( *NextChamber > 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , *NextChamber , G_PLACE ) ) {
					//=============================================================================================================
					if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE( TMATM , Count , *GetChamber , 1 , CHECKING_SIDE , SchPointer , 0 , *NextChamber , FALSE ) ) { // 2006.07.21 // 2011.11.25
						continue;
					}
					//=============================================================================================================
					if ( _SCH_SUB_Run_Impossible_Condition( CHECKING_SIDE , SchPointer , *NextChamber ) ) {
						if ( ( 0 + 1 ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) { // 2003.05.31
							m = SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( TMATM , *NextChamber , CHECKING_SIDE , SchPointer , ( 0 + 1 ) ); // 2003.05.31
						} // 2003.05.31
						else {
							m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,*NextChamber ); // 2003.05.31
						}
						n = 1;
						*ArmIntlks = 0; // 2007.09.06
						if ( m != 0 ) { // 2003.02.05
							if      ( ( m == 1 ) && ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) <= 0 ) ) {
								n = 0; 
								*MovingFinger = TA_STATION;
							}
							else if ( ( m == 2 ) && ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) <= 0 ) ) {
								n = 0; 
								*MovingFinger = TB_STATION;
							}
							else {
								//======================================================================================
								// 2007.09.06
								//======================================================================================
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
									switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
									case 6 :
									case 7 :
										if ( Count == 1 ) {
											if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
												if      ( ( m == 1 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) ) {
													n = 0; 
													m = 2;
													*MovingFinger = TB_STATION;
													*ArmIntlks = 2; // 2007.09.06
												}
												else if ( ( m == 2 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) ) {
													n = 0; 
													m = 1;
													*MovingFinger = TA_STATION;
													*ArmIntlks = 1; // 2007.09.06
												}
											}
										}
										break;
									}
								}
								//======================================================================================
							}
						} // 2003.02.05
						else {
							n = 0;
						}
						if ( n == 0 ) { // 2003.02.05
							if ( Count == 2 ) {
								if ( m == 0 ) { // 2003.05.31
									if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER( TMATM , -1 , CHECKING_SIDE , SchPointer , Count ) ) return 22;
									return -7;
								}
							}
							else { // 2002.05.20
								if ( Always ) return 10;
							}
							//==========================================================================================================
							// 2007.09.29
							//==========================================================================================================
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2006.04.04
								switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
								case 6 :
								case 7 :
									if ( ( m == 0 ) || ( m == 1 ) ) {
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) != SCHEDULER_MOVING_OUT ) ) {
											if ( SCHEDULER_CONTROL_Chk_ARMINLTKS_DATA_A0SUB2( _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION ) ) ) {
												return 1011;
											}
										}
									}
									if ( ( m == 0 ) || ( m == 2 ) ) {
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) != SCHEDULER_MOVING_OUT ) ) {
											if ( SCHEDULER_CONTROL_Chk_ARMINLTKS_DATA_A0SUB2( _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION ) ) ) {
												return 1012;
											}
										}
									}
									break;
								}
							}
							//==========================================================================================================
//							if ( _SCH_MODULE_Get_PM_WAFER( *NextChamber , 0 ) > 0 ) { // 2014.01.10
							if ( !SCH_PM_IS_ABSENT( *NextChamber , CHECKING_SIDE , SchPointer , 0 ) ) { // 2014.01.10
								//
								if ( ( m == 0 ) || ( m == 1 ) ) {
//									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
								}
								if ( ( m == 0 ) || ( m == 2 ) ) {
//									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
								}
								if ( ( m != 0 ) && ( Count == 2 ) ) {
									if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,*NextChamber ) == 0 ) { // 2003.06.02
										if ( SCHEDULING_CHECK_INTERLOCK_DENY_PICK_POSSIBLE( 0 , *NextChamber , CHECKING_SIDE , SchPointer , ( 0 + 1 ) ) ) { // 2003.06.03
											return 77;
										}
									}
									else if ( m == 1 ) { // 2003.07.31
										if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,*NextChamber ) != 1 ) {
											return 78;
										}
									}
									else if ( m == 2 ) { // 2003.07.31
										if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,*NextChamber ) != 2 ) {
											return 79;
										}
									}
								}
							}
							else {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( *NextChamber ) > 0 ) { // 2002.06.28
									if ( ( m == 0 ) || ( m == 1 ) ) {
//										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
									}
									if ( ( m == 0 ) || ( m == 2 ) ) {
//										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
									}
									//
									if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( *GetChamber ) == 3 ) { // 2012.04.02
										if ( Count == 2 ) {
											return 121;
										}
										else {
											if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , SchPointer , Count ) ) return 122;
											if ( SCHEDULING_CHECK_REMAKE_GROUPPING_POSSIBLE( CHECKING_SIDE , SchPointer , 0 ) ) return 123;
										}
									}
									//
								}
								else { // 2002.06.28
//									if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( CHECKING_SIDE , *NextChamber ) ) { // 2003.11.11 // 2017.07.17
									if ( ( -1 != _SCH_SYSTEM_HAS_USER_PROCESS_GET( *NextChamber ) ) || _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( CHECKING_SIDE , *NextChamber ) ) { // 2003.11.11 // 2017.07.17
										if ( ( m == 0 ) || ( m == 1 ) ) {
//											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
											if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) return 31; // 2004.05.13
										}
										if ( ( m == 0 ) || ( m == 2 ) ) {
//											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
											if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) return 41; // 2004.05.13
										}
									}
									else {
										if ( _SCH_CLUSTER_Check_Possible_UsedPre( CHECKING_SIDE , SchPointer , 0 , k , *NextChamber , FALSE ) ) { // 2002.07.10
											if ( ( m == 0 ) || ( m == 1 ) ) {
//												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 3; // 2006.04.11
												if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) return 31; // 2004.05.13
											}
											if ( ( m == 0 ) || ( m == 2 ) ) {
//												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 4; // 2006.04.11
												if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) return 41; // 2004.05.13
											}
										}
										else {
											if ( ( m == 0 ) || ( m == 1 ) ) {
//												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) { // 2006.04.11
//													if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) return 5; // 2006.04.11
//												} // 2006.04.11
												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) return 6; // 2006.04.11
											}
											if ( ( m == 0 ) || ( m == 2 ) ) {
//												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) { // 2006.04.11
//													if ( _SCH_MODULE_Get_TM_TYPE( TMATM ,TB_STATION ) == SCHEDULER_MOVING_OUT ) return 6; // 2006.04.11
//												} // 2006.04.11
												if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) return 6; // 2006.04.11
											}
// Move Down
//											if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , SchPointer ) ) return 7; // 2006.02.10
//											if ( SCHEDULING_CHECK_REMAKE_GROUPPING_POSSIBLE( CHECKING_SIDE , SchPointer , 0 ) ) return 114; // 2006.01.26 // 2006.02.10
										}
									}
									// Move Here
									if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , SchPointer , Count ) ) return 7; // 2006.02.10
									if ( SCHEDULING_CHECK_REMAKE_GROUPPING_POSSIBLE( CHECKING_SIDE , SchPointer , 0 ) ) return 114; // 2006.01.26 // 2006.02.10
								}
							}
						}
					}
				}
			}
		}
	}
	return -1;
}
//=======================================================================================
int SCHEDULING_CHECK_Enable_PICK_CM_or_PLACE_to_BM_at_SINGLE_MODE( int TMATM , int CHECKING_SIDE , int Slot ) {
	int i , Count , k , SchPointer , NextChamber , k2 , NextChamber2;

	if ( Slot < 0 ) {
		if ( !_SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) return -3; // 2006.11.28

		SchPointer  = _SCH_MODULE_Get_FM_DoPointer( CHECKING_SIDE );
	}
	else {
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
				if ( _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) == Slot ) {
					SchPointer  = i;
					break;
				}
			}
		}
		if ( i == MAX_CASSETTE_SLOT_SIZE ) return -10;
	}
	if ( SchPointer >= MAX_CASSETTE_SLOT_SIZE ) return -2;
	if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) < 0 ) return -3;
	if ( SCHEDULER_Get_TM_MidCount( TMATM ) == 11 ) return -4;
	Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM );
	if ( Count == 0 ) return -6;
	//
	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 0 ) == 0 ) return -21;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , SchPointer , 0 , k );
		//-------------------------------------------------------------------------------------
		if ( NextChamber > 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , NextChamber , G_PLACE ) ) {
				if ( _SCH_SUB_Run_Impossible_Condition( CHECKING_SIDE , SchPointer , NextChamber ) ) {
					if ( Count == 2 ) {
						if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER( TMATM , -1 , CHECKING_SIDE , SchPointer , Count ) ) return 2;
						return -7;
					}
					if ( ( Count == 1 ) && ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , 0 ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , 1 ) ) ) { // 2002.11.11
//						if ( _SCH_MODULE_Get_PM_WAFER( NextChamber , 0 ) > 0 ) { // 2014.01.10
						if ( !SCH_PM_IS_ABSENT( NextChamber , CHECKING_SIDE , SchPointer , 0 ) ) { // 2014.01.10
							if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) > 1 ) { // 2004.06.07
								for ( k2 = 0 ; k2 < MAX_PM_CHAMBER_DEPTH ; k2++ ) {
									NextChamber2 = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , SchPointer , 1 , k2 );
									//-------------------------------------------------------------------------------------
									if ( NextChamber2 > 0 ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , NextChamber2 , G_PLACE ) ) {
//											if ( _SCH_MODULE_Get_PM_WAFER( NextChamber2 , 0 ) <= 0 ) { // 2014.01.10
											if ( SCH_PM_IS_ABSENT( NextChamber2 , CHECKING_SIDE , SchPointer , 1 ) ) { // 2014.01.10
												return 9;
											}
										}
									}
								}
							}
							return -8;
						}
					}
					if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , SchPointer , Count ) ) return 7;
				}
			}
		}
	}
	return -1;
}
//=======================================================================================
BOOL Sch_NextPrev_Free_Pick_Check_for_Place( int tms , int mdchk , int Count , int sc , int ch , BOOL MoveOnly ) { // 2007.01.04
	int plccheck = 0;
	//
	if ( mdchk == 1 ) {
		if ( ( ch >= BM1 ) && ( ch < TM ) ) { // 2007.09.06
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( ch ) != tms ) {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) == BUFFER_FM_STATION ) {
					}
					else {
						return FALSE;
					}
				}
				else {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) == BUFFER_TM_STATION ) {
					}
					else {
						return FALSE;
					}
				}
			}
		}
	}
	//
	switch ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( sc ) ) {
	case 11 : // Process Free(A1)
	case 14 : // Process Free(A1.G0)
		if ( Count != 1 ) break;
	case  1 : // Process Free
	case  8 : // Process Free(LX)
		plccheck = 1;
		break;
	case 12 : // Wafer Free(A1)
	case 15 : // Wafer Free(A1.G0)
		if ( Count != 1 ) break;
	case  2 : // Wafer Free
	case  9 : // Wafer Free(LX)
		plccheck = 2;
		break;
	case 13 : // Process+Wafer Free(A1)
	case 16 : // Process+Wafer Free(A1.G0)
		if ( Count != 1 ) break;
	case  3 : // Process+Wafer Free
	case  7 : // Process+Wafer Free(ALL)
	case 10 : // Process+Wafer Free(LX)
		plccheck = 3;
		break;
	}
	//
	if ( ch != -1 ) {
		switch ( _SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( ch ) ) {
		case  1 : // Process Free
			if      ( plccheck == 0 ) plccheck = 1;
			else if ( plccheck == 2 ) plccheck = 3;
			break;
		case  2 : // Wafer Free
			if      ( plccheck == 0 ) plccheck = 2;
			else if ( plccheck == 1 ) plccheck = 3;
			break;
		case  3 : // Process + Wafer Free
			plccheck = 3;
		}
	}
	//
	if ( mdchk == 2 ) {
		if ( ( plccheck == 2 ) || ( plccheck == 3 ) ) return FALSE;
		return TRUE;
	}
	if ( ( ch >= BM1 ) && ( ch < TM ) ) { // 2007.09.06
		if ( ( plccheck != 0 ) && ( mdchk == 0 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( ch ) != tms ) {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) == BUFFER_FM_STATION ) {
					}
					else {
						return FALSE;
					}
				}
				else {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) == BUFFER_TM_STATION ) {
					}
					else {
						return FALSE;
					}
				}
			}
		}
	}
	if ( ( plccheck == 1 ) || ( plccheck == 3 ) ) {
		if ( !MoveOnly ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) return FALSE;
		}
	}

	return TRUE;
}


int SCHEDULING_CHECK_for_Get_Mdl_Size( BOOL sw , int ch ) {
	if ( sw ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
			return _SCH_PRM_GET_MODULE_SIZE( ch ) / 2;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
			return _SCH_PRM_GET_MODULE_SIZE( ch ) / 2;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
			return _SCH_PRM_GET_MODULE_SIZE( ch );
		}
		else {
			return _SCH_PRM_GET_MODULE_SIZE( ch );
		}
	}
	else {
		return _SCH_PRM_GET_MODULE_SIZE( ch );
	}
}	
//=======================================================================================
int SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( int TMATM , int CHECKING_SIDE , int Chkpt , int Count , int SwitchModeOrg , int swbm , int JumpMode , int GetChamber , int *NextChamber , int offset , BOOL getchkpossible , BOOL MoveOnly ) {
	int i , j , k , crch , find , fs , getcnt , spcnt , plccbm , SwitchMode , getinsize , cbmcnt , retmode;
	int PathDo , code;
	//==============================================================================================================
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return 1;
	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) return -1;
	//
	//==============================================================================================================
	//
	if ( _SCH_PRM_GET_MODULE_GROUP( GetChamber ) != TMATM ) return 2; // 2018.11.23
	//
	//==============================================================================================================
	plccbm = *NextChamber;
	retmode = FALSE; // 2007.09.28
	//==============================================================================================================
	//
	code = 0;
	//
	if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) {
		if ( offset == 0 ) {
			if      ( SwitchModeOrg == 1 ) {
				SwitchMode = TRUE;
				getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , BUFFER_INWAIT_STATUS );
				getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( TRUE , GetChamber ); // 2007.02.16
				//
				code = 1;
				//
			}
			else if ( SwitchModeOrg == 2 ) {
				if ( TMATM == 0 ) {
					if ( JumpMode == 1 ) {
						SwitchMode = TRUE;
						getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , -1 );
						getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , GetChamber ); // 2007.02.16
						//
						code = 2;
						//
					}
					else {
						SwitchMode = FALSE;
						getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , BUFFER_INWAIT_STATUS );
						getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( TRUE , GetChamber ); // 2007.02.16
						//
						code = 3;
						//
					}
				}
				else {
					SwitchMode = FALSE;
					getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , -1 );
					getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , GetChamber ); // 2007.02.16
					//
					code = 4;
					//
				}
			}
			else {
				SwitchMode = FALSE;
				getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , -1 );
				getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , GetChamber ); // 2007.02.16
				//
				code = 5;
				//
			}
		}
		else {
			SwitchMode = SwitchModeOrg;
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , GetChamber ) == BUFFER_SWITCH_MODE ) {
				getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , BUFFER_INWAIT_STATUS ) + offset;
				getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( TRUE , GetChamber ); // 2007.02.16
				//
				code = 6;
				//
			}
			else {
				getcnt = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( GetChamber , -1 ) + offset;
				getinsize = SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , GetChamber ); // 2007.02.16
				//
				code = 7;
				//
			}
		}
	}
	else if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) ) {
		SwitchMode = SwitchModeOrg;
		getcnt = 1;
		getinsize = 1;
		//
		code = 8;
		//
	}
	else {
		return 2;
	}
	//==============================================================================================================
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[A=BM1,%d(%d)%d,%d,%d,%d,%d,%d]" , TMATM , offset , SwitchMode , getcnt , Chkpt , swbm , JumpMode , plccbm );
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[A=BM2,%d(%d)%d,%d,%d,%d,%d,%d]" , TMATM , offset , SwitchMode , getcnt , Chkpt , swbm , JumpMode , plccbm );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[A=BM3,%d(%d)%d,%d,%d,%d,%d,%d]" , TMATM , offset , SwitchMode , getcnt , Chkpt , swbm , JumpMode , plccbm );
	//==============================================================================================================
	if ( Chkpt != -1 ) {
		find = FALSE;
		//
		if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) == 0 ) { // 2011.07.04
			retmode = TRUE;
			if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) {
				getcnt--;
				//
				code += 10;
				//
			}
			else {
				//
				code += 20;
				//
			}
		}
		else if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) < PATHDO_WAFER_RETURN ) {
			if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) {
				if ( offset == 0 ) {
					if ( ( TMATM == 0 ) && ( JumpMode != 1 ) ) {
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
							find = TRUE;
							//
							code += 10;
							//
						}
						else {
							//
							code += 20;
							//
						}
					}
					else {
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
							find = TRUE;
							//
							code += 30;
							//
						}
						else {
							//
							code += 40;
							//
						}
					}
				}
				else {
					if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
						find = TRUE;
						//
						code += 50;
						//
					}
					else {
						//
						code += 60;
						//
					}
				}
			}
			else if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) ) {
				if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
					find = TRUE;
					//
					code += 70;
					//
				}
				else {
					//
					code += 80;
					//
				}
			}
		}
		//=============================================================================================================
		// 2007.09.06
		//=============================================================================================================
		else if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) == PATHDO_WAFER_RETURN ) {
			retmode = TRUE; // 2007.09.28
			if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) {
				getcnt--;
				//
				code += 10;
				//
			}
			else {
				//
				code += 20;
				//
			}
		}
		//=============================================================================================================
		if ( find ) {
			find = FALSE;
			fs   = FALSE;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				crch = 0;
				//
				if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) {
					if ( offset == 0 ) {
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= 0 ) {
							PathDo = 0;
							crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , PathDo , k );
						}
						else {
							if ( ( TMATM == 0 ) && ( JumpMode != 1 ) ) {
								PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt );
								crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , PathDo , k );
							}
							else {
								PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1;
								crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , PathDo , k );
							}
						}
					}
					else {
						PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1;
						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , PathDo , k );
					}
				}
				else if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) ) {
					PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt );
					crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) , k );
				}
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &crch ); // 2007.10.29
				//-------------------------------------------------------------------------------------
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[B=%d]" , crch );
				if ( crch > 0 ) {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[C=%d]" , crch );
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , crch , G_PLACE ) ) {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[D=%d]" , crch );
//						if ( _SCH_MODULE_Get_PM_WAFER( crch , 0 ) <= 0 ) { // 2014.01.10
						if ( SCH_PM_IS_ABSENT( crch , CHECKING_SIDE , Chkpt , PathDo ) ) { // 2014.01.10
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[D1=%d]" , crch );
							return 3;
						}
						else {
							find = TRUE;
						}
					}
					else {
						//
						fs = TRUE;
						//
					}
				}
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[E0=%d,%d]" , crch , fs );
			}
			if ( find ) {
				if ( !fs ) return -2;
			}
			//=====================================================================================
			if ( ( GetChamber >= PM1 ) && ( GetChamber < AL ) ) return 4;
			//=====================================================================================
		}
	}
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[E1=BM1,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[E1=BM2,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[E1=BM3,%d,%d,%d,RET=%d]" , SwitchMode , getcnt , Chkpt , retmode );
	//==============================================================================================================
	if ( !getchkpossible ) return -3; // 2007.02.15
	//==============================================================================================================
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[E2=BM1,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[E2=BM2,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[E2=BM3,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[E=%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
	cbmcnt = 0; // 2007.09.28

	if ( SwitchMode ) {
		if ( swbm != 0 ) {
			if ( TMATM != 0 ) { // 2013.05.03
				i = swbm;
			}
			else {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) {
					i = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE );
				}
				else {
					i = swbm;
				}
			}
			//
			*NextChamber = i;
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[F1=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//				if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 0 , TRUE ) ) { // 2008.06.20
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[F2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.06.26
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) { // 2018.06.26
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.11.05
							spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , FALSE );
							if ( spcnt > 0 ) { 
								//
								if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
								//
								if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) != 0 ) return 62; // 2015.08.17
								//
								if ( spcnt >= getcnt ) {
	//if ( GetChamber == BM1 + 4 - 1 ) printf( "[F3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
									//
									if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 0 , TRUE , spcnt , getcnt ) ) { // 2008.06.20
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 5;
										//
										code += 100;
										//
									} // 2008.06.20
									else {
										//
										code += 200;
										//
									}
	//if ( GetChamber == BM1 + 4 - 1 ) printf( "[F4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
								}
								else { // 2008.06.20
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( i ) ) {
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 5;
										//
										code += 300;
										//
									}
									else {
										//
										code += 400;
										//
									}
								}
							}
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
						if ( i != swbm ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
								if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != CHECKING_SIDE ) return -4;
							}
						}
						spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
						if ( spcnt > 0 ) {
							if ( spcnt >= getcnt ) {
								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 0 , TRUE , spcnt , getcnt ) ) { // 2008.06.20
									if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 6;
								} // 2008.06.20
							}
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.05.22
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
							spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
							if ( spcnt > 0 ) {
								//
								if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
								//
								if ( spcnt >= getcnt ) {
									if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 0 , TRUE , spcnt , getcnt ) ) { // 2008.06.20
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 7;
									} // 2008.06.20
								}
							}
						}
					}
					else {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[G1=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
						spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
						if ( spcnt > 0 ) {
							//
							if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
							//
							if ( spcnt >= getcnt ) {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[G2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
								//
								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 0 , TRUE , spcnt , getcnt ) ) { // 2008.06.20
									if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 8;
								} // 2008.06.20
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[G3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
							}
						}
						else {
							if ( ( retmode ) && ( GetChamber == i ) && ( Count != 0 ) ) { // 2011.06.23
//								cbmcnt--; // 2013.04.10
								return 81; // 2013.04.10
							}
						}
					}
//				} // 2008.06.20
			}
			else { // 2017.12.12
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					return 88;
				}
			}
		}
		else {
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				//=========================================================================================================================
				// 2007.05.18
				//=========================================================================================================================
//				if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue; // 2007.05.22
				//=========================================================================================================================
				if ( ( plccbm != -1 ) && ( plccbm != i ) ) continue;
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2011.06.23
				//
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_SWITCH_MODE ) continue; // 2007.01.17
				//
				*NextChamber = i;
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {

					cbmcnt++; // 2007.09.28

//if ( GetChamber == BM1 + 4 - 1 ) printf( "[H1=BM%d,%d,%d,%d]" , i - BM1 + 1 , SwitchMode , getcnt , spcnt );
//if ( i == BM1 + 2 - 1 ) printf( "[H1=BM%d,%d,%d,%d]" , i - BM1 + 1 , SwitchMode , getcnt , spcnt );
//					if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , TRUE ) ) { // 2008.06.20
						//
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[H2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( i == BM1 + 2 - 1 ) printf( "[H2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.06.26
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) { // 2018.06.26
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[H2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( i == BM1 + 2 - 1 ) printf( "[H21=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.11.05
								spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , FALSE );
								if ( spcnt > 0 ) {
									//
									if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
									//
									if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) != 0 ) return 62; // 2015.08.17
									//
									if ( spcnt >= getcnt ) {
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[H3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( i == BM1 + 2 - 1 ) printf( "[H3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
										if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , TRUE , spcnt , getcnt ) ) { // 2008.06.20
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 9;
											//
											code += 100;
											//
										} // 2008.06.20
										else {
											//
											code += 200;
											//
										}
//if ( GetChamber == BM1 + 4 - 1 ) printf( "[H4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( i == BM1 + 2 - 1 ) printf( "[H4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
									}
									else { // 2008.06.20
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( i ) ) {
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 9;
											//
											code += 300;
											//
										}
										else {
											//
											code += 400;
											//
										}
									}
								}
							}
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
							if ( i != swbm ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
									if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != CHECKING_SIDE ) return -5;
								}
							}
							spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
							if ( spcnt > 0 ) {
								if ( spcnt >= getcnt ) {
									if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , TRUE , spcnt , getcnt ) ) { // 2008.06.20
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 10;
									} // 2008.06.20
								}
							}
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.05.22
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
								spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
								if ( spcnt > 0 ) {
									//
									if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
									//
									if ( spcnt >= getcnt ) {
										if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , TRUE , spcnt , getcnt ) ) { // 2008.06.20
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 11;
										} // 2008.06.20
									}
								}
							}
						}
						else {
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[I1=%d,%d,%d][i=%d][cbmcnt=%d]" , SwitchMode , getcnt , spcnt , i , cbmcnt );
							spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[I2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
							if ( spcnt > 0 ) {
								//
								if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return 61; // 2013.05.07
								//
								if ( spcnt >= getcnt ) {
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[I3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
									//
									if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , TRUE , spcnt , getcnt ) ) { // 2008.06.20
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 12;
									} // 2008.06.20
									//
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[I4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
								}
							}
							else {
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[I5=%d,%d,%d][retmode=%d][Count=%d]" , SwitchMode , getcnt , spcnt , retmode , Count );
								if ( ( retmode ) && ( GetChamber == i ) && ( Count != 0 ) ) { // 2011.06.23
//									cbmcnt--; // 2011.07.04
//									cbmcnt = 0; // 2011.07.04 // 2013.10.21
									return 82; // 2013.10.21
								}
							}
						}
//					} // 2008.06.20
				}
				else { // 2017.12.12
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
						return 88;
					}
				}
			}
			//===============================================================================
			// 2007.09.28
			//===============================================================================
			if ( cbmcnt == 0 ) return 21;
			//===============================================================================
		}
	}
	else {
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				if ( ( plccbm != -1 ) && ( plccbm != i ) ) continue;
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2011.06.23
				//
				*NextChamber = i;
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {

					if (
						( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) ||
						( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE )
						) {

						cbmcnt++; // 2007.09.28

//if ( GetChamber == BM1 + 1 - 1 ) printf( "[J1=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
						if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , JumpMode , FALSE , -1 , -1 ) ) {
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[J2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[J3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );

//							spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ); // 2015.08.25
							//
							spcnt = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , 0 ); // 2015.08.25
							//
							if ( spcnt > 0 ) {
								if ( spcnt >= getcnt ) {
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[J4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
									if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 13;
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[J5=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
								}
							}
						}
					}
				}
			}
			//===============================================================================
			// 2007.09.28
			//===============================================================================
			if ( cbmcnt == 0 ) return 22;
			//===============================================================================
		}
		else {
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // first ordering 
				return 14;
			}
			else {
				if ( JumpMode == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( ( plccbm != -1 ) && ( plccbm != i ) ) continue;
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2011.06.23
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_SWITCH_MODE ) continue; // 2007.01.17
						//
						*NextChamber = i;
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) { // 2007.01.17

//if ( GetChamber == BM1 + 1 - 1 ) printf( "[K1=%d,%d,%d]" , SwitchMode , getcnt , i );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[K1=%d,%d,%d]" , SwitchMode , getcnt , i );

							if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) {

								cbmcnt++; // 2007.09.28
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[K2=%d,%d,%d]" , SwitchMode , getcnt , i );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[K2=%d,%d,%d]" , SwitchMode , getcnt , i );

//								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE ) ) { // 2008.06.20
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[K3=%d,%d,%d]" , SwitchMode , getcnt , i );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[K3=%d,%d,%d]" , SwitchMode , getcnt , i );

									spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );

									if ( spcnt > 0 ) {
										if ( spcnt >= getcnt ) {
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[K4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[K4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
											if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE , spcnt , getcnt ) ) { // 2008.06.20
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 15;
											} // 2008.06.20
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[K5=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[K5=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
										}
										else { // 2007.02.16
											if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE , -1 , -1 ) ) { // 2008.06.20
												if ( getinsize > SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , i ) ) {
													if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 16;
												}
												else { // 2007.10.16
													if ( ( GetChamber >= BM1 ) && ( GetChamber < TM ) ) { // 2007.10.16
														if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( GetChamber ) ) { // 2007.10.16
															if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 16; // 2007.10.16
														}
														else {
															//======================================================================
															// 2007.11.07
															//======================================================================
															if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
																if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
																	if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 16;
																}
															}
															//======================================================================
														}
													}
												}
											} // 2008.06.20
										}
									}
//								} // 2008.06.20
							}
						}
					}
					//===============================================================================
					// 2007.09.28
					//===============================================================================
					if ( cbmcnt == 0 ) return 23;
					//===============================================================================
				}
				else {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L0=%d,%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt , plccbm );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L0=%d,%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt , plccbm );
						//
						if ( ( plccbm != -1 ) && ( plccbm != i ) ) continue;
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2011.06.23
						//
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L01=%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L01=%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt );
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_SWITCH_MODE ) continue; // 2007.01.17
						//
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L02=%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L02=%d,%d,%d,%d]" , i , SwitchMode , getcnt , spcnt );
						*NextChamber = i;
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) {

//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L1=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L1=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
							if (
								( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) ||
								( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE )
								) {

								cbmcnt++; // 2007.09.28

//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L2=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//								if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE ) ) { // 2008.06.20
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L3=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
									spcnt = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 );
									if ( spcnt > 0 ) {
										if ( spcnt >= getcnt ) {
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[L4=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
											if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE , spcnt , getcnt ) ) { // 2008.06.20
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , GetChamber , i , MoveOnly ) ) return 17;
											} // 2008.06.20
//if ( GetChamber == PM1 + 4 - 1 ) printf( "[L5=%d,%d,%d]" , SwitchMode , getcnt , spcnt );
										}
										else { // 2007.02.16
											if ( !SCHEDULING_CHECK_for_NEXT_NOT_CROSS_INVALID( TMATM , GetChamber , i , 1 , FALSE , -1 , -1 ) ) { // 2008.06.20
												if ( getinsize > SCHEDULING_CHECK_for_Get_Mdl_Size( FALSE , i ) ) {
													if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 18;
												}
												else { // 2007.03.08
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
														if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , GetChamber , i , MoveOnly ) ) return 19;
													}
												}
											} // 2008.06.20
										}
									}
//								} // 2008.06.20
							}
						}
					}
					//===============================================================================
					// 2007.09.28
					//===============================================================================
					if ( cbmcnt == 0 ) return 24;
					//===============================================================================
				}
			}
		}
	}
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[Z=%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 1 - 1 ) printf( "[Z=BM1,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 3 - 1 ) printf( "[Z=BM2,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//if ( GetChamber == BM1 + 2 - 1 ) printf( "[Z=BM2,%d,%d,%d]" , SwitchMode , getcnt , Chkpt );
//	return -99; // 2018.06.26
	//
	if ( code >= 0 ) return -9; // 2018.06.26
	//
	return -code; // 2018.06.26
}


//int SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( int TMATM , int CHECKING_SIDE , int Chkpt , int Count , int SwitchModeOrg , int swbm , int JumpMode , int GetChamber , int *NextChamber , int offset , BOOL getchkpossible , BOOL MoveOnly ) {
//	int Res;
//	Res = SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm_xxx( TMATM , CHECKING_SIDE , Chkpt , Count , SwitchModeOrg , swbm , JumpMode , GetChamber , NextChamber , offset , getchkpossible , MoveOnly );
//	printf( "=======================[%d]\n" , Res );
//	return Res;
//}
/*
// 2013.08.01
int SCHEDULING_CHECK_PICK_from_BM_INTLKS( int TMATM , int bmc ) { // 2008.06.22
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,bmc ) ) {
	case 1 :
		if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) > 0 ) return FALSE;
		break;
	case 2 :
		if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) > 0 ) return FALSE;
		break;
	}
	return TRUE;
}
*/

// 2013.08.01
int SCHEDULING_CHECK_PICK_from_BM_INTLKS( int TMATM , int bmc , BOOL diffgrp , int s , int p ) { // 2008.06.22
	int cf;
	//
	if ( diffgrp ) {
		//
		cf = SCHEDULER_CONTROL_DUALTM_for_GET_PROPERLY_ARM_WHEN_PICK_FROM_BM_POSSIBLE( TMATM , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , bmc );
		//
	}
	else {
		//
		cf = _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,bmc );
		//
	}
	//
	switch( cf ) {
	case 1 :
		if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) > 0 ) return FALSE;
		break;
	case 2 :
		if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) > 0 ) return FALSE;
		break;
	}
	//
	return TRUE;
}


int SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( int bch , BOOL pickmode ) { // 2008.08.27
	int i , k , s , p , m , sc , lc , w , cr , or , TMATM = 0;
	int pm[MAX_PM_CHAMBER_DEPTH];
	//

	if ( bch < BM1 ) return 1;
	if ( bch >= TM ) return 2;

	if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( bch , G_PLACE , 0 ) ) return 3;

	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return 4;
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) return 5;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return 6;
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bch , TRUE , &sc , &lc ); // in
	//
	w = 0;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) pm[k] = 0;
	//
	for ( i = sc ; i <= lc ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bch , i ) <= 0 ) continue;

		s = _SCH_MODULE_Get_BM_SIDE( bch , i );
		p = _SCH_MODULE_Get_BM_POINTER( bch , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
			//
			w++;
			//
			cr = 0;
			or = 0;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
				//
				if ( m > 0 ) {
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return 7; // 2014.01.10
					if ( SCH_PM_IS_ABSENT( m , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) ) ) return 7; // 2014.01.10
					//
					pm[m-PM1] = 1;
					//
					if      ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
						if ( pickmode ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) > 0 ) {
								cr++;
							}
						}
						else {
							cr++;
						}
					}
					else if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
						or++;
					}
				}
			}
			//
			if ( ( !pickmode ) && ( or <= 0 ) ) return -11;
			//
			if ( ( cr <= or ) || ( or <= 0 ) ) return 8;
			//
		}
	}
	//
	if ( w < 2 ) return 9;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( pm[i] == 1 ) continue;
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i + PM1 , FALSE , -1 ) ) {
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i + PM1 , G_PLACE ) ) {
				if ( _SCH_MODULE_Get_Mdl_Run_Flag( i + PM1 ) > 0 ) return -12;
			}
		}
	}
	//
	return 10;
}

//int SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( int bch , BOOL pickmode ) { // 2008.08.27
//	int res;
//	res = SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK_SUB( bch , pickmode );
//	printf( "--[%d][M=%d] is %d\n" , bch , pickmode , res );
//	return res;
//}

BOOL SCHEDULING_CHECK_for_DOUBLE_OtherSlot_Go_Same_PM( int tms , int GetChamber , int GetSlot , int pch , int pathdo ) { // 2009.03.04
	int k , s , p , crch;
	//
	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) <= 0 ) return TRUE; // 2009.03.12 // 2013.03.19
	//
	if ( ( GetSlot % 2 ) != 0 ) {
		if ( ( GetSlot + 1 ) > _SCH_PRM_GET_MODULE_SIZE( GetChamber ) ) return TRUE;
		if ( _SCH_MODULE_Get_BM_WAFER( GetChamber , GetSlot + 1 ) <= 0 ) return TRUE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( GetChamber , GetSlot + 1 );
		p = _SCH_MODULE_Get_BM_POINTER( GetChamber , GetSlot + 1 );
	}
	else {
		if ( ( GetSlot - 1 ) < 1 ) return TRUE;
		if ( _SCH_MODULE_Get_BM_WAFER( GetChamber , GetSlot - 1 ) <= 0 ) return TRUE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( GetChamber , GetSlot - 1 );
		p = _SCH_MODULE_Get_BM_POINTER( GetChamber , GetSlot - 1 );
	}
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= PATHDO_WAFER_RETURN ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//		crch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k ); // 2013.03.19
		crch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pathdo , k ); // 2013.03.19
		if ( crch == pch ) return TRUE;
	}
	//
	return FALSE;
	//
}

//BOOL SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( int side , int TMATM , int arm , int getch ) { // 2009.08.22 // 2015.06.15
BOOL SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( int side , int TMATM , int arm , int getch , BOOL oneslotonly ) { // 2009.08.22 // 2015.06.15
	int i;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( getch ) > 1 ) {
		//
		if ( oneslotonly ) return FALSE; // 2015.06.15
		//
		return TRUE;
	}
	//
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( getch , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( getch ) == TMATM ) ) {
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( TMATM,arm ) , getch ) != MUF_00_NOTUSE ) {
				if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , getch , G_PLACE , G_MCHECK_SAME ) ) {
					//
					if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
						if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , getch , _SCH_MODULE_Get_TM_SIDE( TMATM,arm ) , _SCH_MODULE_Get_TM_POINTER( TMATM,arm ) , BUFFER_OUTWAIT_STATUS ) ) { // 2009.08.26
							//
							return TRUE;
						}
					}
					else {
						return TRUE;
					}
					//
				}
				//
			}
		}
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( i == getch ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( TMATM,arm ) , i ) == MUF_00_NOTUSE ) continue;
			//
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
				if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , _SCH_MODULE_Get_TM_SIDE( TMATM,arm ) , _SCH_MODULE_Get_TM_POINTER( TMATM,arm ) , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
			}
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
				if ( _SCH_MODULE_Get_BM_WAFER( i , 1 ) > 0 ) continue;
			}
			//
			return TRUE;
		}
		//
		return FALSE;
	}
	return TRUE;
}



BOOL SCHEDULING_CHECK_PICK_from_BM_1_Slot( int TMATM , int GetSide , int GetChamber ) {
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) return TRUE;
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ) != 1 ) return TRUE;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
//		if ( !SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( GetSide , TMATM , TA_STATION , GetChamber ) ) return FALSE; // 2015.06.15
		if ( !SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( GetSide , TMATM , TA_STATION , GetChamber , FALSE ) ) return FALSE; // 2015.06.15
	}
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
//		if ( !SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( GetSide , TMATM , TB_STATION , GetChamber ) ) return FALSE; // 2015.06.15
		if ( !SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( GetSide , TMATM , TB_STATION , GetChamber , FALSE ) ) return FALSE; // 2015.06.15
	}
	//
	return TRUE;
}

BOOL BM_OUT_IN_FIRST( int oldch , int newch ) { // 2018.10.20
	int i , oldsp , newsp;
	//
	if ( !MULTI_ALL_PM_FULLSWAP ) return FALSE;
	//
	oldsp = -1;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( oldch ) ; i >= 1 ; i-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( oldch , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( oldch , i ) == BUFFER_INWAIT_STATUS ) {
			if ( oldsp == -1 ) {
				oldsp = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( oldch , i ) , _SCH_MODULE_Get_BM_POINTER( oldch , i ) );
			}
			else {
				if ( oldsp > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( oldch , i ) , _SCH_MODULE_Get_BM_POINTER( oldch , i ) ) ) {
					oldsp = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( oldch , i ) , _SCH_MODULE_Get_BM_POINTER( oldch , i ) );
				}
			}
		}
		//
	}
	//
	if ( oldsp < 0 ) return FALSE;
	//
	newsp = -1;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( newch ) ; i >= 1 ; i-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( newch , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( newch , i ) == BUFFER_INWAIT_STATUS ) {
			if ( newsp == -1 ) {
				newsp = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( newch , i ) , _SCH_MODULE_Get_BM_POINTER( newch , i ) );
			}
			else {
				if ( newsp > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( newch , i ) , _SCH_MODULE_Get_BM_POINTER( newch , i ) ) ) {
					newsp = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( newch , i ) , _SCH_MODULE_Get_BM_POINTER( newch , i ) );
				}
			}
		}
		//
	}
	//
	if ( newsp < 0 ) return FALSE;
	//
	if ( oldsp > newsp ) return TRUE;
	//
	return FALSE;
	//
}



//int SCHEDULING_CHECK_for_Switch_PICK_from_BM_Check( int TMATM , int CHECKING_SIDE , int *RChamber , int *RSlot , int *Rpt , int pickgroup , int bmmode , int Count , int MoveOnly , int SwitchMode , int swbm , BOOL GetPickLast , int Skip1 , int Skip2 , int Skip3 , int Skip4 , int Skip5 ) { // 2016.05.12
int SCHEDULING_CHECK_for_Switch_PICK_from_BM_Check( int TMATM , int CHECKING_SIDE , int *RChamber , int *RSlot , int *Rpt , int pickgroup , int bmmode , int Count , int MoveOnly , int SwitchMode , int swbm , BOOL GetPickLast , int Skip1 , int Skip2 , int Skip3 , int Skip4 , int Skip5 , int Skip_Pick_BM1 , int Skip_Pick_BM2 ) { // 2016.05.12
	int i , j , SchPointer , k , oc , ChkSrc , ChkSlot , Chkpt , Chkoc , chk;
	int nx;

	ChkSrc  = -1;
	ChkSlot = 1;
	k = 999999;

	SchPointer = -1;
	//------------------------------------
	Chkoc   = -1; // 2004.12.06
	//------------------------------------
	// 2002.01.29
	if ( swbm == 0 ) { // 2002.10.29
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( i == Skip1 ) continue; // 2013.12.06
			if ( i == Skip2 ) continue; // 2013.12.06
			if ( i == Skip3 ) continue; // 2013.12.06
			if ( i == Skip4 ) continue; // 2013.12.06
			if ( i == Skip5 ) continue; // 2013.12.06
			//
			if ( i == Skip_Pick_BM1 ) continue; // 2016.05.12
			if ( i == Skip_Pick_BM2 ) continue; // 2016.05.12
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == pickgroup ) && !GetPickLast ) { // 2007.01.15 // 2007.02.02
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
				//
				if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , ( TMATM == pickgroup ) ? G_MCHECK_SAME : G_MCHECK_ALL ) ) continue; // 2008.05.26
				//
//				if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i ) ) continue; // 2008.06.22 // 2013.08.01
				//
				if ( SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( i , TRUE ) < 0 ) continue; // 2008.08.27
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != bmmode ) ) continue; // 2006.05.26
				//
				if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) && ( MoveOnly == 0 ) && ( _SCH_PRM_GET_Getting_Priority( i ) >= 0 ) ) continue; // 2007.01.02
				//
				SchPointer = -1;
				//
				if ( TMATM == pickgroup ) {
					chk = SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , CHECKING_SIDE , &SchPointer , &j , TRUE , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &oc );
				}
				else {
					chk = SCHEDULER_CONTROL_Chk_BM_HAS_INOUT_SIDE_POINTER( i , CHECKING_SIDE , TMATM , &SchPointer , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &oc );
				}
				//
//				if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , ( TMATM != pickgroup ) , CHECKING_SIDE , SchPointer ) ) continue; // 2013.08.01 // 2014.10.15
				//
				if ( chk > 0 ) {
					//
					if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , ( TMATM != pickgroup ) , CHECKING_SIDE , SchPointer ) ) continue; // 2013.08.01 // 2014.10.15
					//
					nx = -1;
					if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , SchPointer , Count , SwitchMode , swbm , 0 , i , &nx , 0 , TRUE , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
						//
						if ( !SCHEDULING_CHECK_PICK_from_BM_1_Slot( TMATM , CHECKING_SIDE , i ) ) continue; // 2009.09.03
						//
						if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 1 ) != 0 ) {
							if ( k > _SCH_PRM_GET_Getting_Priority( i ) ) {
								ChkSrc  = i;
								ChkSlot = j;
								Chkpt   = SchPointer;
								k		= _SCH_PRM_GET_Getting_Priority( i );
								//
								//------------------------------------
								Chkoc   = oc; // 2004.12.06
								//------------------------------------
							}
							else if ( k == _SCH_PRM_GET_Getting_Priority( i ) ) {
								if ( ( TMATM != pickgroup )	&& ( BM_OUT_IN_FIRST( ChkSrc , i ) ) ) { // 2018.10.20
									//
									ChkSrc  = i;
									ChkSlot = j;
									Chkpt   = SchPointer;
									k		= _SCH_PRM_GET_Getting_Priority( i );
									//
									//------------------------------------
									Chkoc   = oc; // 2004.12.06
									//------------------------------------
									//
								}
								else {
									if ( ( Chkoc == -1 ) || ( Chkoc == oc ) ) { // 2004.12.06 // 2008.12.19
										//if ( Chkpt > SchPointer ) { // 2008.11.22
										if ( _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , Chkpt ) > _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , SchPointer ) ) {// 2008.11.22
											ChkSrc  = i;
											ChkSlot = j;
											Chkpt   = SchPointer;
											k		= _SCH_PRM_GET_Getting_Priority( i );
											//
											//------------------------------------
											Chkoc   = oc; // 2004.12.06
											//------------------------------------
										}
									}
									else { // 2004.12.06
										if ( Chkoc < oc ) {
											ChkSrc  = i;
											ChkSlot = j;
											Chkpt   = SchPointer;
											k		= _SCH_PRM_GET_Getting_Priority( i );
											//
											//------------------------------------
											Chkoc   = oc; // 2004.12.06
											//------------------------------------
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		if ( _SCH_MODULE_GET_USE_ENABLE( swbm , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( swbm ) == pickgroup ) && !GetPickLast ) { // 2007.02.02
			if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , swbm , G_PICK , ( TMATM == pickgroup ) ? G_MCHECK_SAME : G_MCHECK_ALL ) ) { // 2008.05.26
				//
//				if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , swbm ) ) { // 2008.06.22 // 2013.08.01
					//
				if ( ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 999999 ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(pickgroup) != BUFFER_SWITCH_BATCH_ALL ) ) { // 2008.08.25 // 2009.03.12 // 2013.03.09
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( swbm ) || ( _SCH_MODULE_Get_BM_FULL_MODE( swbm ) == bmmode ) ) { // 2006.05.26
						if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( swbm ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( swbm ) < 0 ) ) { // 2007.01.02
							//
							SchPointer = -1;
							//
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( swbm , CHECKING_SIDE , &SchPointer , &j , TRUE , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( swbm ) ) , &oc ) > 0 ) {
								//
								if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , swbm , ( TMATM != pickgroup ) , CHECKING_SIDE , SchPointer ) ) { // 2013.08.01
									//
									nx = -1;
									if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , SchPointer , Count , SwitchMode , swbm , 0 , swbm , &nx , 0 , TRUE , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
										if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 1 ) != 0 ) {
											if ( k > _SCH_PRM_GET_Getting_Priority( swbm ) ) {
												ChkSrc  = swbm;
												ChkSlot = j;
												Chkpt   = SchPointer;
												k		= _SCH_PRM_GET_Getting_Priority( swbm );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//
	if ( ChkSrc > 0 ) {
		*RChamber  = ChkSrc;
		*RSlot     = ChkSlot;
		*Rpt	   = Chkpt;
		return TRUE;
	}
	//
	return FALSE;
	//
}

//=======================================================================================
int SCHEDULING_CHECK_for_BPM_ARM_INTLKS( int TMATM , int Chamber , int Side , int Pointer , BOOL optcheck ) { // 2013.11.27
	int k , pickrm , ChkTrg , pd , pr;
	BOOL gofind; // 2015.08.10
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,Chamber ) ) {
	case 1 :
		pickrm = 1;
		break;
	case 2 :
		pickrm = 2;
		break;
	default :
		//
		if ( !optcheck ) return 0;
		//
		pickrm = 0;
		//
		break;
	}
	//
	pd = _SCH_CLUSTER_Get_PathDo( Side , Pointer );
	pr = _SCH_CLUSTER_Get_PathRange( Side , Pointer );
	//
	if ( pd >= pr ) {
		//
		return 1;
	}
	//
	gofind = FALSE; // 2015.08.10
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , pd , k );
		//
		if ( ChkTrg > 0 ) {
			//
			gofind = TRUE; // 2015.08.10
			//
			//===========================================================================
			if ( Chamber == ChkTrg ) return 0; // 2014.07.04
			//===========================================================================
			//
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg ) ) {
			case 1 :
				//
				if ( optcheck ) return -1;
				//
				if ( pickrm == 1 ) {
					//
					if ( Chamber == ChkTrg ) return 0;
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) return 0; // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , pd ) ) return 0; // 2014.01.10
					//
					if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ChkTrg ) != pickrm ) return 0; // 2015.03.25
					//
				}
				break;
			case 2 :
				//
				if ( optcheck ) return -1;
				//
				if ( pickrm == 2 ) {
					//
					if ( Chamber == ChkTrg ) return 0;
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) return 0; // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , pd ) ) return 0; // 2014.01.10
					//
					if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ChkTrg ) != pickrm ) return 0; // 2015.03.25
					//
				}
				break;
			default :
				//
				if ( optcheck ) return 0;
				//
				if ( Chamber == ChkTrg ) return 0;
				//
//				if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) return 0; // 2014.01.10
				if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , pd ) ) return 0; // 2014.01.10
				//
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) return 0;
				//
				if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PICK ) ) return 0;
				//
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ChkTrg ) ) {
				case 1 :
					if ( pickrm != 1 ) return 0;
					break;
				case 2 :
					if ( pickrm != 2 ) return 0;
					break;
				default :
					return 0;
					break;
				}
				//
				break;
			}
			//
		}
	}
	//
	if ( !gofind ) return 1; // 2015.08.10
	//
	return -1;
	//
}

void SCHEDULING_CHECK_CHINTLKS_PICLPLACE_DATA( int TMATM , int ch , int *curr ) { // 2015.06.03
	//
	if      ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ch ) == 1 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ch ) == 2 ) ) {
		//
		if      ( *curr == -1 ) {
			*curr = 1;
		}
		else if ( *curr != 1 ) {
			*curr = 0;
		}
		//
	}
	else if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ch ) == 2 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ch ) == 1 ) ) {
		//
		if      ( *curr == -1 ) {
			*curr = 2;
		}
		else if ( *curr != 2 ) {
			*curr = 0;
		}
		//
	}
	else {
		//
		*curr = 0;
		//
	}
	//
}

int SCHEDULING_CHECK_PICK_BM_ReJect_PM_CHINTLKS( int TMATM , int side , int pt ) { // 2015.06.03
	int i , j , pmslot , s , p , ChkTrg , j2 , ChkTrg2;
	int chintlk_has_out , chintlk_has_next , chintlk_has_next2;
	//
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) != 0 ) return FALSE;
	//
	chintlk_has_out = -1;
	chintlk_has_next = -1;
	chintlk_has_next2 = -1;
	//
	for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
		//
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( i ) ) continue; // 2015.11.10
		//
		if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) continue;
		//
		s = _SCH_MODULE_Get_PM_SIDE( i , pmslot );
		p = _SCH_MODULE_Get_PM_POINTER( i , pmslot );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) { // last for out
			//
			SCHEDULING_CHECK_CHINTLKS_PICLPLACE_DATA( TMATM , i , &chintlk_has_out );
			//
		}
		else {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , 0 , j );
				//
				if ( ChkTrg > 0 ) {
					//
					if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( ChkTrg ) ) continue; // 2015.11.10
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , side ) ) {
						//
						if ( ChkTrg == i ) return FALSE;
						//
					}
				}
			}
			//
			SCHEDULING_CHECK_CHINTLKS_PICLPLACE_DATA( TMATM , i , &chintlk_has_next );
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , j );
				//
				if ( ChkTrg > 0 ) {
					//
					if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( ChkTrg ) ) continue; // 2015.11.10
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) {
						//
						for ( j2 = 0 ; j2 < MAX_PM_CHAMBER_DEPTH ; j2++ ) {
							//
							ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , 0 , j2 );
							//
							if ( ChkTrg2 > 0 ) {
								//
								if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( ChkTrg2 ) ) continue; // 2015.11.10
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg2 , FALSE , side ) ) {
									//
									if ( ChkTrg2 == ChkTrg ) return FALSE;
									//
								}
							}
						}
						//
						SCHEDULING_CHECK_CHINTLKS_PICLPLACE_DATA( TMATM , ChkTrg , &chintlk_has_next2 );
						//
					}
				}
			}
		}
	}
	//
	//============================================================================================================
	if ( ( chintlk_has_out == 1 ) && ( chintlk_has_next == 2 ) && ( chintlk_has_next2 == chintlk_has_out ) ) return TRUE;
	if ( ( chintlk_has_out == 2 ) && ( chintlk_has_next == 1 ) && ( chintlk_has_next2 == chintlk_has_out ) ) return TRUE;
	//============================================================================================================
	//
	return FALSE;
}


/*
//
// 2017.10.22
//
BOOL SCHEDULING_CHECK_for_PARALLEL_Possible_PM( int TMATM , int Side , int Pointer , int PathDo , int idx2 , int ch ) {
	int i , k , m , selid , selidx , PathRange;
	BOOL find_e , find_d;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_CLUSTER_Get_PathProcessParallel( Side , Pointer , PathDo ) == NULL ) return TRUE;
	//
	PathRange = _SCH_CLUSTER_Get_PathRange( Side , Pointer );
	//
	if ( ( PathDo + 1 ) >= PathRange ) return TRUE;
	//
	selid = _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , PathDo , ( ch - PM1 ) );
	//
	if ( ( selid <= 1 ) || ( selid >= 100 ) ) return TRUE;
	//
	for ( i = ( PathDo + 1 ) ; i < PathRange ; i++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathProcessParallel( Side , Pointer , i ) == NULL ) continue;
		//
		find_e = FALSE;
		find_d = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , i , k );
			//
			if ( m < 0 ) {
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , -m ) != MUF_01_USE ) {
					find_d = TRUE;
				}
				else {
					if ( selid == _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( (-m) - PM1 ) ) ) {
						find_d = TRUE;
					}
				}
				//
			}
			else if ( m > 0 ) {
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , m ) != MUF_01_USE ) {
					find_d = TRUE;
				}
				else {
					//
					selidx = _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( m - PM1 ) );
					//
					if ( ( selidx <= 1 ) || ( selidx >= 100 ) ) {
						find_e = TRUE;
					}
					else {
						if ( selid == selidx ) {
							find_e = TRUE;
						}
					}
				}
				//
			}
			//
		}
		//
		if ( !find_e && find_d ) return FALSE;
		//
	}
	//
	return TRUE;
}
*/
//
// 2017.10.22
//
BOOL SCHEDULING_CHECK_for_PARALLEL_Possible_PM( int TMATM , int Side , int Pointer , int PathDo , int idx2 , int ch ) {
	int i , k , m , selid , selidx , PathRange;
	BOOL find_e , find_d;
	int ena_pm_count;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_CLUSTER_Get_PathProcessParallel( Side , Pointer , PathDo ) == NULL ) return TRUE;
	//
	PathRange = _SCH_CLUSTER_Get_PathRange( Side , Pointer );
	//
	if ( ( PathDo + 1 ) >= PathRange ) return TRUE;
	//
	selid = _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , PathDo , ( ch - PM1 ) );
	//
	if ( ( selid <= 1 ) || ( selid >= 100 ) ) return TRUE;
	//
	ena_pm_count = 0; // 2017.10.22
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { // 2017.10.22
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , PathDo , k );
		//
		if ( m > 0 ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , m ) == MUF_01_USE ) {
				//
				if ( m == ch ) {
					//
					if ( ena_pm_count != -1 ) {
						ena_pm_count++;
					}
					//
				}
				else {
					//
					ena_pm_count = -1;
					//
				}
				//
			}
			//
		}
	}
	//
	for ( i = ( PathDo + 1 ) ; i < PathRange ; i++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathProcessParallel( Side , Pointer , i ) == NULL ) continue;
		//
		find_e = FALSE;
		find_d = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , i , k );
			//
			if ( m < 0 ) {
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , -m ) == MUF_98_USE_to_DISABLE_RAND ) { // 2017.10.22
					//
					if ( selid == _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( (-m) - PM1 ) ) ) {
						//
						if ( ena_pm_count == 1 ) {
							//





//							if ( SCH_PM_IS_FREE( -m ) ) { // 2017.10.23
//								//
//								find_d = TRUE;
//								//
//							}





							//
						}
						else {
							find_d = TRUE;
						}
						//
					}
					else {
						find_d = TRUE;
					}
					//
				}
				else {
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , -m ) != MUF_01_USE ) {
						find_d = TRUE;
					}
					else {
						if ( selid == _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( (-m) - PM1 ) ) ) {
							find_d = TRUE;
						}
					}
				}
				//
			}
			else if ( m > 0 ) {
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , m ) != MUF_01_USE ) {
					//
//					find_d = TRUE; // 2017.10.24
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , m ) == MUF_98_USE_to_DISABLE_RAND ) {
						//
						if ( selid == _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( m - PM1 ) ) ) {
							//
							if ( ena_pm_count == 1 ) {
								//
							}
							else {
								find_d = TRUE;
							}
							//
						}
						else {
							find_d = TRUE;
						}
					}
					else {
						find_d = TRUE;
					}
					//
				}
				else {
					//
					selidx = _SCH_CLUSTER_Get_PathProcessParallel2( Side , Pointer , i , ( m - PM1 ) );
					//
					if ( ( selidx <= 1 ) || ( selidx >= 100 ) ) {
						find_e = TRUE;
					}
					else {
						if ( selid == selidx ) {
							find_e = TRUE;
						}
					}
				}
				//
			}
			//
		}
		//
		if ( !find_e && find_d ) return FALSE;
		//
	}
	//
	return TRUE;
}


BOOL OtherArm_Place_Curr_Get_BM( int tms , int Chkjp , int GetChamber ) { // 2018.10.20
	int s , p , k , m , d;
	BOOL BM_Waitiing;
	//
	if      ( _SCH_MODULE_Get_TM_WAFER( tms,TA_STATION ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( tms,TA_STATION );
		p = _SCH_MODULE_Get_TM_POINTER( tms,TA_STATION );
	}
	else if ( _SCH_MODULE_Get_TM_WAFER( tms,TB_STATION ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( tms,TB_STATION );
		p = _SCH_MODULE_Get_TM_POINTER( tms,TB_STATION );
	}
	else {
		return FALSE;
	}
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
	//
	if ( Chkjp == 0 ) {
		//
		// ret
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( GetChamber ) && ( _SCH_MODULE_Get_BM_FULL_MODE( GetChamber ) != BUFFER_TM_STATION ) ) return FALSE;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( GetChamber ) > 0 ) return FALSE;
		//
		if ( d < _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
		//
	}
	else {
		//
		// sup
		//
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( GetChamber ) && ( _SCH_MODULE_Get_BM_FULL_MODE( GetChamber ) != BUFFER_FM_STATION ) ) return FALSE;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( GetChamber ) > 0 ) return FALSE;
		//
		if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
		//
		BM_Waitiing = GatewayBM_waiting( tms , s , p , -1 );
		//
		if ( BM_Waitiing ) return TRUE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m  = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
			//
			if ( m > 0 ) {
				//
				if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tms , m , G_PLACE ) ) return TRUE;
				//
			}
		}
		//
		return FALSE;
		//
	}
	//
	return TRUE;
	//
}


//int SCHEDULING_CHECK_for_Enable_PICK_from_BM( int TMATM , int CHECKING_SIDE , int *GetChamber , int *GetSlot , int *NextChamber , int *MovingType , int *MovingOrder , int MoveOnly , int SwitchModeOrg , int swbm , int *schpt , int *JumpMode , BOOL GetOutSkip , BOOL GetPickLast , BOOL fullcheck ) { /* 2013.11.12 */
//int SCHEDULING_CHECK_for_Enable_PICK_from_BM( int TMATM , int CHECKING_SIDE , int ChkGetCh , int *GetChamber , int *GetSlot , int *NextChamber , int *MovingType , int *MovingOrder , int MoveOnly , int SwitchModeOrg , int swbm , int *schpt , int *JumpMode , BOOL GetOutSkip , BOOL GetPickLast , BOOL fullcheck , int Skip1 , int Skip2 , int Skip3 , int Skip4 , int Skip5 ) { /* 2013.11.12 */ // 2016.05.12
int SCHEDULING_CHECK_for_Enable_PICK_from_BM( int TMATM , int CHECKING_SIDE , int ChkGetCh , int *GetChamber , int *GetSlot , int *NextChamber , int *MovingType , int *MovingOrder , int MoveOnly , int SwitchModeOrg , int swbm , int *schpt , int *JumpMode , BOOL GetOutSkip , BOOL GetPickLast , BOOL fullcheck , int Skip1 , int Skip2 , int Skip3 , int Skip4 , int Skip5 , int Skip_Pick_BM1 , int Skip_Pick_BM2 ) { /* 2013.11.12 */ // 2016.05.12
	int i , j , jx , SchPointer , Count , k , oc , ChkSrc , ChkSlot , Chkpt , Chkjp;
	BOOL find;
	int m , Place_PM_Count_All , PlusOneGroupPlaceCount;
	int bmd , crch;
	int SwitchMode , nx , PathDo;
	int xs , xp; // 2013.12.22
	int errorcode; // 2014.10.28
	int Count_Intlks; // 2015.08.11
	int sbm , ebm , bma , bmb; // 2016.12.13
	BOOL BM_Waitiing; // 2018.10.20
	//
	errorcode = 0; // 2014.10.28
	//
	//======================================================================
	// 2006.12.21
	//======================================================================
	if ( TMATM == 0 ) {
		if      ( SwitchModeOrg == 1 ) {
			SwitchMode = 1;
		}
		else if ( SwitchModeOrg == 2 ) {
			SwitchMode = 2;
		}
		else {
			SwitchMode = 0;
		}
	}
	else {
		if      ( SwitchModeOrg == 1 ) {
			SwitchMode = 1;
		}
		else if ( SwitchModeOrg == 2 ) {
			SwitchMode = 0;
		}
		else {
			SwitchMode = 0;
		}
		//
		swbm = 0; // 2013.05.03
		//
	}
	//======================================================================
	// 2007.05.29
	//======================================================================
	*GetChamber = -1;
	*GetSlot = -1;
	*NextChamber = -1;
	*MovingType = -1;
	*MovingOrder = -1;
	*JumpMode = -1;
	//======================================================================

	if ( TMATM == 0 ) {
		for ( ; _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) < MAX_CASSETTE_SLOT_SIZE ; ) {
			if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) < 0 ) {
				_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
			}
//			else if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) > SCHEDULER_SUPPLY ) // 2004.12.03
			else if (
				( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) > SCHEDULER_SUPPLY ) && // 2004.12.03
				( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) ) != SCHEDULER_RETURN_REQUEST ) // 2004.12.03
				) {// 2004.12.03
				_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
			}
			else {
				break;
			}
		}
//		SchPointer = _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE );
//		if ( SchPointer >= MAX_CASSETTE_SLOT_SIZE ) return -2;
	}
//	else {
		SchPointer = -1;
//	}
	//============================================================================================
	Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM );
	if ( Count == 0 ) return -11;

	Chkpt   = _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE );
	// Update Insert below Line 2002.01.02
	if ( Chkpt < MAX_CASSETTE_SLOT_SIZE ) {
		ChkSrc  = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , Chkpt );
		//
//		if ( ChkSrc >= PM1 ) { // 2007.03.20
		if ( ( ChkSrc >= PM1 ) && ( ChkSrc < AL ) ) { // 2007.03.20
			if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , Chkpt , 2 ) != 0 ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkSrc ) <= 0 ) {
//					&& ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc ) == TMATM ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PICK ) ) {
						*MovingOrder = 0;
						*GetChamber  = ChkSrc;
						*GetSlot     = 1;
						*NextChamber = 0;
						*MovingType  = SCHEDULER_MOVING_IN;
						*schpt		 = Chkpt;
						*JumpMode	 = 0;
						return 11;
					}
				}
			}
		}
	}

	ChkSrc  = -1;
	ChkSlot = 1;
	Chkjp   = 0;
	k = 999999;

	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
		i = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
		if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( i ) < 0 ) ) { // 2007.01.02
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , CHECKING_SIDE , &SchPointer , &j , FALSE , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &oc ) > 0 ) {
				if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , SchPointer ) >= PATHDO_WAFER_RETURN ) return -88;
				if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 1 ) == 0 ) return -21;
				if ( k > _SCH_PRM_GET_Getting_Priority( i ) ) {
					k		= _SCH_PRM_GET_Getting_Priority( i );
					ChkSrc  = i;
					ChkSlot = j;
					Chkpt   = SchPointer;
					Chkjp	= 0;
				}
			}
		}
	}
	else {
		if ( ( SwitchMode == 1 ) || ( ( SwitchMode == 2 ) && ( TMATM == 0 ) ) ) { // 2006.12.21
			//
			if ( swbm != 0 ) {
				if ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
						return -22;
					}
//					else if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == 0 ) ) { // 2013.05.03
					else if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == 0 ) ) ) { // 2013.05.03
						return -22;
					}
				}
			}
			//
			if ( SCHEDULING_CHECK_for_Switch_PICK_from_BM_Check( TMATM , CHECKING_SIDE , &i , &j , &SchPointer , TMATM , BUFFER_TM_STATION , Count , MoveOnly , SwitchMode , swbm , GetPickLast , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ) ) { // 2016.05.12
				//
				if ( Count == 1 ) { // 2013.03.18
					if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , SchPointer ) > _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) {
						if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) { // 2015.11.27
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.08
									//
									errorcode = (SchPointer*1000000) + (i*1000) + (j*10) + 1; // 2014.10.28
									//
									i = 0;
									//
								}
								//
							}
							//
						}
					}
				}
				if ( i > 0 ) {
					ChkSrc  = i;
					ChkSlot = j;
					Chkpt   = SchPointer;
					k		= _SCH_PRM_GET_Getting_Priority( i );
					Chkjp	= 0;
				}
			}
			//
			if ( !GetOutSkip ) { // 2013.05.15
				//
				if ( ( ChkSrc < 0 ) || ( ChkGetCh != ChkSrc ) ) { // 2013.11.12
					//
					if ( SCHEDULING_CHECK_for_Switch_PICK_from_BM_Check( TMATM , CHECKING_SIDE , &i , &j , &SchPointer , TMATM + 1 , BUFFER_FM_STATION , Count , MoveOnly , SwitchMode , /*swbm*/ 0 , GetPickLast , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ) ) { // 2016.05.12
						if ( Count == 1 ) { // 2013.03.18
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , SchPointer ) >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) {
								if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
									//
									errorcode = (SchPointer*1000000) + (i*1000) + (j*10) + 2; // 2014.10.28
									//
									i = 0;
									//
								}
							}
						}
						if ( i > 0 ) {
							//
							errorcode = (SchPointer*1000000) + (i*1000) + (j*10) + 3; // 2014.10.28
							//
							if ( ( k > _SCH_PRM_GET_Getting_Priority( i ) ) || ( ( Chkjp == 0 ) && ( k == _SCH_PRM_GET_Getting_Priority( i ) ) ) ) {
								//
								ChkSrc  = i;
								ChkSlot = j;
								Chkpt   = _SCH_MODULE_Get_BM_POINTER( i , j );
								k		= _SCH_PRM_GET_Getting_Priority( i );
								Chkjp	= 1;
							}
							//
						}
					}
					//
				}
				//
			}
			//
		}
		//
	//			else { // Non Switch // 2006.12.21
		if ( SwitchMode != 1 ) { // 2006.12.21
			//
			if ( ( ChkSrc < 0 ) || ( ChkGetCh != ChkSrc ) ) { // 2013.11.12
				//
				if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) != GROUP_HAS_SWITCH_NONE ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( i == Skip1 ) continue; // 2013.12.06
						if ( i == Skip2 ) continue; // 2013.12.06
						if ( i == Skip3 ) continue; // 2013.12.06
						if ( i == Skip4 ) continue; // 2013.12.06
						if ( i == Skip5 ) continue; // 2013.12.06
						//
						if ( i == Skip_Pick_BM1 ) continue; // 2016.05.12
						if ( i == Skip_Pick_BM2 ) continue; // 2016.05.12
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
							//
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
							//
	//						if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i ) ) continue; // 2008.06.22 // 2013.08.01
							//
							if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) { // 2006.05.25
								//
	//							if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i ) ) continue; // 2008.06.22 // 2013.08.01
								//
								if ( ( SwitchMode != 0 ) && ( ( SwitchMode != 2 ) || ( TMATM == 0 ) ) ) continue; // 2006.12.21
								//=========================================================================================================================================
								bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
								//
								if ( ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( i ) < 0 ) ) && !GetPickLast ) { // 2007.01.02
									if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) { // 2006.05.25
										SchPointer = -1;
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , CHECKING_SIDE , &SchPointer , &j , FALSE , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &oc ) > 0 ) {
											//
											if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , FALSE , CHECKING_SIDE , SchPointer ) ) { // 2013.08.01
												//
												nx = -1;
												//
												if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , SchPointer , Count , SwitchMode , swbm , 0 , i , &nx , 0 , ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
													if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 2 ) != 0 ) {
														if ( SCHEDULING_CHECK_for_NEXT_GO_READY( TMATM , CHECKING_SIDE , SchPointer , 2 ) ) {
															if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
																if ( k > _SCH_PRM_GET_Getting_Priority( i ) ) {
																	ChkSrc  = i;
																	ChkSlot = j;
																	Chkpt   = SchPointer;
																	k		= _SCH_PRM_GET_Getting_Priority( i );
																	Chkjp	= 4;
																}
																else if ( k == _SCH_PRM_GET_Getting_Priority( i ) ) { // 2006.05.25
																	// if ( Chkpt > SchPointer ) { // 2008.11.22
																	if ( _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , Chkpt ) > _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , SchPointer ) ) { // 2008.11.22
																		ChkSrc  = i;
																		ChkSlot = j;
																		Chkpt   = SchPointer;
																		k		= _SCH_PRM_GET_Getting_Priority( i );
																		Chkjp	= 4;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
									else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_S_MODE ) {
										//
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( i , CHECKING_SIDE , &j , &jx , 10 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											//
											if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , FALSE , CHECKING_SIDE , j ) ) { // 2013.08.01
												//
												if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , 2 ) != 0 ) {
													if ( SCHEDULING_CHECK_for_NEXT_GO_READY( TMATM , CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( i , j ) , 0 ) ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
															if ( k > _SCH_PRM_GET_Getting_Priority( i ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																Chkpt   = _SCH_MODULE_Get_BM_POINTER( i , j );
																k		= _SCH_PRM_GET_Getting_Priority( i );
																Chkjp	= 2;
															}
														}
													}
												}
											}
										}
									}
								}
								//
								if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) && ( MoveOnly == 0 ) ) {
									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( i , CHECKING_SIDE , &j , &jx , 10 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											//
											if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , FALSE , CHECKING_SIDE , j ) ) { // 2013.08.01
												//
												if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , 2 ) != 0 ) {
													if ( SCHEDULING_CHECK_for_NEXT_GO_READY( TMATM , CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( i , j ) , 1 ) ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
															if ( ( Chkjp == 0 ) || ( Chkjp == 4 ) || ( k > _SCH_PRM_GET_Getting_Priority( i ) ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																Chkpt   = _SCH_MODULE_Get_BM_POINTER( i , j );
																k		= _SCH_PRM_GET_Getting_Priority( i );
																Chkjp	= 3;
															}
														}
													}
												}
												//
											}
											//
										}
									}
								}
								//=========================================================================================================================================
							}
							else if ( !GetOutSkip && ( _SCH_PRM_GET_MODULE_GROUP( i ) == ( TMATM+1 ) ) && !Get_Prm_MODULE_MOVE_GROUP(i) && ( MoveOnly != 1 ) ) { // 2006.05.25 // 2007.05.30
								bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
								if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( i ) < 0 ) ) { // 2007.01.02
									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_MODE ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , CHECKING_SIDE , &j , &jx , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											//
											if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , TRUE , CHECKING_SIDE , j ) ) { // 2013.08.01
												//
												nx = -1;
												if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , jx , Count , SwitchMode , swbm , 1 , i , &nx , 0 , ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
													if ( !SCHEDULING_CHECK_Disable_PICK_from_CMBM_OUT_for_GETPR( TMATM , CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , Count ) ) {
														if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , 2 ) != 0 ) {
															if ( SCHEDULING_CHECK_for_NEXT_GO_READY( TMATM , CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( i , j ) , 3 ) ) {
																if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
																	if ( ( Chkjp == 0 ) || ( k > _SCH_PRM_GET_Getting_Priority( i ) ) ) {
																		ChkSrc  = i;
																		ChkSlot = j;
																		Chkpt   = _SCH_MODULE_Get_BM_POINTER( i , j );
																		k		= _SCH_PRM_GET_Getting_Priority( i );
																		Chkjp	= 1;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				else {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( i == Skip1 ) continue; // 2013.12.06
						if ( i == Skip2 ) continue; // 2013.12.06
						if ( i == Skip3 ) continue; // 2013.12.06
						if ( i == Skip4 ) continue; // 2013.12.06
						if ( i == Skip5 ) continue; // 2013.12.06
						//
						if ( i == Skip_Pick_BM1 ) continue; // 2016.05.12
						if ( i == Skip_Pick_BM2 ) continue; // 2016.05.12
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
							//
	//						if ( !SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i ) ) continue; // 2008.06.22 // 2013.08.01
							//
							if ( ( Chkjp == 0 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) { // 2006.05.25
								//
								if ( ( SwitchMode == 0 ) || ( ( SwitchMode == 2 ) && ( TMATM != 0 ) ) ) { // 2006.12.21
									//
									bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
									//
									if ( ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( i ) < 0 ) ) && !GetPickLast ) { // 2007.01.02
										if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) { // 2006.05.25
											SchPointer = -1;
											//
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , CHECKING_SIDE , &SchPointer , &j , FALSE , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &oc ) > 0 ) {
												//
												if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , FALSE , CHECKING_SIDE , SchPointer ) ) { // 2013.08.01
													//
													nx = -1;
													if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , SchPointer , Count , SwitchMode , swbm , 0 , i , &nx , 0 , ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
														if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , SchPointer , 2 ) != 0 ) {
															if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
																if ( k > _SCH_PRM_GET_Getting_Priority( i ) ) {
																	ChkSrc  = i;
																	ChkSlot = j;
																	Chkpt   = SchPointer;
																	k		= _SCH_PRM_GET_Getting_Priority( i );
																	Chkjp	= 0;
																}
																else if ( k == _SCH_PRM_GET_Getting_Priority( i ) ) { // 2006.05.25
																	// if ( Chkpt > SchPointer ) { // 2008.11.22
																	if ( _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , Chkpt ) > _SCH_CLUSTER_Get_SupplyID( CHECKING_SIDE , SchPointer ) ) { // 2008.11.22
																		ChkSrc  = i;
																		ChkSlot = j;
																		Chkpt   = SchPointer;
																		k		= _SCH_PRM_GET_Getting_Priority( i );
																		Chkjp	= 0;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
							else if ( !GetOutSkip && ( _SCH_PRM_GET_MODULE_GROUP( i ) == ( TMATM+1 ) ) && !Get_Prm_MODULE_MOVE_GROUP(i) && ( MoveOnly != 1 ) ) { // 2006.05.25 // 2007.05.30
								bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
								if ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( MoveOnly != 0 ) || ( _SCH_PRM_GET_Getting_Priority( i ) < 0 ) ) { // 2007.01.02
									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_MODE ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , CHECKING_SIDE , &j , &jx , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											//
											if ( SCHEDULING_CHECK_PICK_from_BM_INTLKS( TMATM , i , TRUE , CHECKING_SIDE , j ) ) { // 2013.08.01
												//
												nx = -1;
												if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , jx , Count , SwitchMode , swbm , 1 , i , &nx , 0 , ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) , ( MoveOnly != 0 ) ) > 0 ) { // 2007.01.17
													if ( SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( nx , FALSE ) >= 0 ) { // 2008.08.27
														if ( !SCHEDULING_CHECK_Disable_PICK_from_CMBM_OUT_for_GETPR( TMATM , CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , Count ) ) {
															if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER(i,j) , 2 ) != 0 ) {
																if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
																	if ( ( k > _SCH_PRM_GET_Getting_Priority( i ) ) || ( ( Chkjp == 0 ) && ( k == _SCH_PRM_GET_Getting_Priority( i ) ) ) ) { // 2007.01.04
																		ChkSrc  = i;
																		ChkSlot = j;
																		Chkpt   = _SCH_MODULE_Get_BM_POINTER( i , j );
																		k		= _SCH_PRM_GET_Getting_Priority( i );
																		Chkjp	= 1;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
							else {
							}
						}
					}
				}
			}
		}
	}
	//
	if ( ChkSrc < 0 ) {
		//
		*MovingOrder = errorcode; // 2014.10.28
		//
		return -3;
	}
	//===========================================================================================================
	Count_Intlks = Count; // 2015.08.11
	//===========================================================================================================
	if ( ( TMATM == 0 ) && ( Chkjp == 0 ) ) { // 2013.11.27
		//
		// 2015.06.03
		//
		if ( Count == 2 ) {
			if ( SCHEDULING_CHECK_PICK_BM_ReJect_PM_CHINTLKS( TMATM , CHECKING_SIDE , Chkpt ) ) {
				return -17;
			}
		}
		//
		switch( SCHEDULING_CHECK_for_BPM_ARM_INTLKS( TMATM , ChkSrc , CHECKING_SIDE , Chkpt , FALSE ) ) {
		case 1 :
			Count = 1;
			break;
		case -1 :
			return -19;
			break;
		}
	}
	//============================================================================================
	if ( !fullcheck ) { // 2003.12.01
		*MovingOrder = 0;
		*GetChamber  = ChkSrc;
		*GetSlot     = ChkSlot;
		*MovingType  = SCHEDULER_MOVING_IN;
		*schpt		 = Chkpt;
		*JumpMode	 = Chkjp;
		*NextChamber = 0;
		return 111;
	}
	//============================================================================================
	if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) < 0 ) {
		return -4;
	}
	if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) == 0 ) {
		if ( Chkjp <= 1 ) {
			*MovingOrder = 0;
			*GetChamber  = ChkSrc;
			*GetSlot     = ChkSlot;
			*NextChamber = 0;
			*MovingType  = SCHEDULER_MOVING_OUT;
			*schpt		 = Chkpt;
			*JumpMode	 = Chkjp;
			//
			//----------------------------------------------------------------------------------------------------------------
			// 2009.04.07
			//----------------------------------------------------------------------------------------------------------------
			if ( ( SwitchMode == 1 ) || ( ( SwitchMode == 2 ) && ( TMATM == 0 ) ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
					//----------------------------------------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != 0 ) {
//						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2013.05.03
						if ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2013.05.03
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) ) {
								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
									if ( i == CHECKING_SIDE ) continue;
									if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
									if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return -186;
								}
							}
						}
					}
					//----------------------------------------------------------------------------------------------------------------
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			return 1;
		}
		return -31;
	}
	if ( ( TMATM == 0 ) && ( Chkjp == 0 ) ) {
//			if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , Chkpt ) > SCHEDULER_SUPPLY ) return -6; // 2004.12.03
//			if ( ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , Chkpt ) > SCHEDULER_SUPPLY ) && ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , Chkpt ) != SCHEDULER_RETURN_REQUEST ) ) return -6; // 2004.12.03 // 2007.11.08
		if ( ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , Chkpt ) > SCHEDULER_SUPPLY ) && ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) != PATHDO_WAFER_RETURN ) ) return -6; // 2004.12.03 // 2007.11.08
	}
	*MovingOrder = 0;
	*GetChamber  = ChkSrc;
	*GetSlot     = ChkSlot;
	*MovingType  = SCHEDULER_MOVING_IN;
	*schpt		 = Chkpt;
	*JumpMode	 = Chkjp;
	*NextChamber = 0;
	if ( Chkjp <= 1 ) {
		//============================================================================================================
		// 2004.12.14 // 2004.12.28
		//============================================================================================================
		if ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( ChkSrc ) == 7 ) { // 2004.12.14
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_7( 2 , TMATM , Chkjp , Count , CHECKING_SIDE , Chkpt ) ) { // 2006.07.13
				return -201;
			}
		}
		else if ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( ChkSrc ) == 17 ) { // 2006.07.13
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_17( 2 , TMATM , Chkjp , Count , CHECKING_SIDE , Chkpt ) ) { // 2006.07.13
				return -202;
			}
		}
		//============================================================================================================
		find = 0;
		//
		Place_PM_Count_All = 0; // 2004.01.28
		PlusOneGroupPlaceCount = 0; // 2016.12.13
		//
		crch = 0; // 2006.12.06
		//===================================================================================================================
		// 2007.02.05
		//===================================================================================================================
		if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) < 3 ) { // 2017.01.04
			//
			if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) < PATHDO_WAFER_RETURN ) {
				if ( ( TMATM == 0 ) && ( Chkjp != 1 ) ) {
					if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
						find = 1;
					}
				}
				else {
					if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) {
						find = 1;
					}
				}
			}
			//
		}
		//===================================================================================================================
		if ( find != 0 ) {
			//
			BM_Waitiing = GatewayBM_waiting( TMATM , CHECKING_SIDE , Chkpt , ( TMATM == 0 ) ? 0 : -1 ); // 2018.10.20
			//
			find = 0; /* (0)Place_PM_Not_Find */
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				*MovingOrder = k;
				//
				//===================================================================================================================
				// 2007.02.05
				//===================================================================================================================
				if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= 0 ) {
					PathDo = 0;
//					crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , 0 , k ); // 2013.03.19
				}
				else {
					if ( ( TMATM == 0 ) && ( Chkjp != 1 ) ) {
						PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt );
//						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) , k ); // 2013.03.19
					}
					else {
						PathDo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1;
//						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 , k ); // 2013.03.19
					}
				}
				//
				crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , PathDo , k ); // 2013.03.19
				//
				//===================================================================================================================
				// 2008.07.30
				//===================================================================================================================
				if ( crch > 0 ) {
					if ( !_SCH_MODULE_GET_USE_ENABLE( crch , FALSE , CHECKING_SIDE ) ) crch = 0;
				}
				//===================================================================================================================
				// 2009.03.04
				//===================================================================================================================
				if ( crch > 0 ) {
					if ( !SCHEDULING_CHECK_for_DOUBLE_OtherSlot_Go_Same_PM( TMATM , *GetChamber , *GetSlot , crch , PathDo ) ) crch = 0;
				}
				//===================================================================================================================
				// 2016.01.06
				//===================================================================================================================
				if ( crch > 0 ) {
					if ( !SCHEDULING_CHECK_for_PARALLEL_Possible_PM( TMATM , CHECKING_SIDE , Chkpt , PathDo , k , crch ) ) crch = 0;
				}
				//===================================================================================================================
				// 2016.05.30
				//===================================================================================================================
				if ( crch > 0 ) {
					if ( SCH_PM_Check_Impossible_Place_MultiPM( CHECKING_SIDE , Chkpt , crch ) ) crch = 0;
				}
				//===================================================================================================================
/*
// 2007.02.05
				if ( TMATM == 0 ) {
					if ( Chkjp == 1 ) {
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= 0 ) { // 2006.12.06
//								*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , 0 , k ); // 2006.12.06
							crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , 0 , k ); // 2006.12.06
						}
						else {
//								*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 , k ); // 2006.12.06
							crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 , k ); // 2006.12.06
						}
					}
					else {
//							*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) , k ); // 2006.12.06
						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) , k ); // 2006.12.06
					}
				}
				else {
					if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) <= 0 ) { // 2006.07.10
//							*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , 0 , k ); // 2006.12.06
						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , 0 , k ); // 2006.12.06
					}
					else {
//							*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 , k ); // 2006.12.06
						crch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , Chkpt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 , k ); // 2006.12.06
					}
				}
*/
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &crch ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				//===================================================================================================================
//				if ( *NextChamber > 0 ) { // 2006.12.06
				if ( crch > 0 ) { // 2006.12.06
					*NextChamber = crch; // 2006.12.06
					Place_PM_Count_All++; // 2004.01.28
//					if ( _SCH_PRM_GET_MODULE_xGROUP( *NextChamber ) == TMATM ) {
					if ( !BM_Waitiing && _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , *NextChamber , G_PLACE ) ) {
						//=============================================================================================================
						if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE( TMATM , Count , ChkSrc , ChkSlot , CHECKING_SIDE , Chkpt , PathDo , *NextChamber , FALSE ) ) { // 2006.07.21
							continue;
						}
						//=============================================================================================================
						if ( TMATM == 0 ) {
							if ( Chkjp == 1 ) {
								if ( ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 + 1 ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) { // 2003.05.22
									m = SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( TMATM , *NextChamber , CHECKING_SIDE , SchPointer , ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 + 1 ) ); // 2003.05.22
								} // 2003.05.22
								else {
									m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,*NextChamber ); // 2003.02.05
								}
							}
							else {
								if ( ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) + 1 ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) { // 2003.05.22
									m = SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( TMATM , *NextChamber , CHECKING_SIDE , SchPointer , ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) + 1 ) ); // 2003.05.22
								} // 2003.05.22
								else {
									m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,*NextChamber ); // 2003.02.05
								}
							}
						}
						else {
							if ( ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 + 1 ) < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , SchPointer ) ) { // 2003.05.22
								m = SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( TMATM , *NextChamber , CHECKING_SIDE , SchPointer , ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) - 1 + 1 ) ); // 2003.05.22
							} // 2003.05.22
							else {
								m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,*NextChamber ); // 2003.02.05
							}
						}
						//
						if ( m != 0 ) { // 2003.02.05
//							if ( _SCH_MODULE_Get_PM_WAFER( *NextChamber , 0 ) <= 0 ) { // 2003.05.21 // 2014.01.10
							if ( SCH_PM_IS_ABSENT( *NextChamber , CHECKING_SIDE , Chkpt , PathDo ) ) { // 2014.01.10
								if      ( ( m == 1 ) && ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) <= 0 ) ) m = 0; 
								else if ( ( m == 2 ) && ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) <= 0 ) ) m = 0; 
							}
							else { // 2004.03.15
								if ( SCHEDULING_CHECK_INTERLOCK_PM_EXT_DENY_FOR_MDL( TMATM , *NextChamber , m ) ) {
									if ( Count == 2 ) {
										m = 0;
									}
									else {
										if      ( ( m == 1 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) ) {
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
												m = 0;
											}
										}
										else if ( ( m == 2 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) ) {
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
												m = 0;
											}
										}
									}
								}
							}
						} // 2003.02.05
						if ( m != 0 ) continue; // 2003.02.05
		// Move below 3 line to here 2001.11.14
						//
						find = 1 /* (1)Place_PM_Current_Find */ ;
						//
						if ( _SCH_SUB_Run_Impossible_Condition( CHECKING_SIDE , Chkpt , *NextChamber ) ) {
		//					find = 1;
							if ( Count == 2 ) {
								if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER( TMATM , -1 , CHECKING_SIDE , Chkpt , Count ) ) return 2;
								return -8;
							}
							else { // 2002.05.20
								//
//								if ( _SCH_MODULE_Get_PM_WAFER( *NextChamber , 0 ) > 0 ) { // 2014.01.10
								if ( !SCH_PM_IS_ABSENT( *NextChamber , CHECKING_SIDE , Chkpt , PathDo ) ) { // 2014.01.10
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
//										if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TA_STATION , *GetChamber ) ) { // 2009.08.22 // 2015.06.15
										if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TA_STATION , *GetChamber , FALSE ) ) { // 2009.08.22 // 2015.06.15
											return 3;
										}
									}
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
//										if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TB_STATION , *GetChamber ) ) { // 2009.08.22 // 2015.06.15
										if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TB_STATION , *GetChamber , FALSE ) ) { // 2009.08.22 // 2015.06.15
											return 4;
										}
									}
								}
								else if ( _SCH_MACRO_CHECK_PROCESSING_INFO( *NextChamber ) <= 0 ) {
			//					else { //2002.02.16
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) {
										if ( _SCH_MODULE_Get_TM_TYPE( TMATM,TA_STATION ) == SCHEDULER_MOVING_OUT ) {
//											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TA_STATION , *GetChamber ) ) { // 2009.08.22 // 2015.06.15
											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TA_STATION , *GetChamber , FALSE ) ) { // 2009.08.22 // 2015.06.15
												return 5;
											}
										}
									}
									else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
										if ( _SCH_MODULE_Get_TM_TYPE( TMATM,TB_STATION ) == SCHEDULER_MOVING_OUT ) {
//											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TB_STATION , *GetChamber ) ) { // 2009.08.22 // 2015.06.15
											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TB_STATION , *GetChamber , FALSE ) ) { // 2009.08.22 // 2015.06.15
												return 6;
											}
										}
									}
									//
									if ( MULTI_ALL_PM_FULLSWAP ) {
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) ) {
											return -77;
										}
									}
									//
									//-----Old Style Update 2001.07.04
									//if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , Chkpt ) ) return 7;
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
										if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_WHEN_PICK_BM_ONEFREE_SINGLEMODE( TMATM , CHECKING_SIDE , Chkpt ) ) return 7;
									}
									else {
										if ( SCHEDULING_CHECK_for_NOT_CROSS_OVER_PICK_CMBM_OUT( TMATM , CHECKING_SIDE , Chkpt , Count ) ) return 7;
									}
								}
								// 2002.02.19
								else if ( _SCH_MACRO_CHECK_PROCESSING_INFO( *NextChamber ) > 0 ) {
									if ( SCHEDULING_CHECK_for_PM_ALL_FREE( TMATM , *NextChamber ) ) {
										if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) <= 0 ) { // 2013.05.02
											return 11;
										}
									}
									else { // 2015.06.15
										//
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TA_STATION , *GetChamber , TRUE ) ) {
												return 13;
											}
										}
										if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM,TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
											if ( SCHEDULING_CHECK_for_TM_WAFER_RETURN_PLACE_TO_BM_POSSIBLE( CHECKING_SIDE , TMATM , TB_STATION , *GetChamber , TRUE ) ) {
												return 14;
											}
										}
										//
									}
								}
								//
								if ( MULTI_ALL_PM_FULLSWAP ) {
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) ) {
										return -78;
									}
								}
								//
							}
						}
					}
//					else if ( _SCH_PRM_GET_MODULE_xGROUP( *NextChamber ) != TMATM ) {
					else {
						//
						if ( find == 0 ) find = 2; /* (2)Place_PM_OtherOnly_Find */
						//
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM + 1 , *NextChamber , G_PLACE ) ) { //  // 2016.12.13
							PlusOneGroupPlaceCount++;
						}
					}
				}
			}
			//
			if ( find == 0 ) { // 2016.07.22
				if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 2 ) {
					return -84;
				}
			}
			//
		}
		//
		if      ( find == 0 /* (0)Place_PM_Not_Find */ ) {
			//
			if ( Place_PM_Count_All == 0 ) { // 2004.01.28
				//----------------------------------------------------------------------------------------------------------------
				// 2009.03.24
				//----------------------------------------------------------------------------------------------------------------
				if ( ( SwitchMode == 1 ) || ( ( SwitchMode == 2 ) && ( TMATM == 0 ) ) ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
						//----------------------------------------------------------------------------------------------------------------
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) >= PATHDO_WAFER_RETURN ) {
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != 0 ) {
//								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2013.05.03
								if ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2013.05.03
									if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) ) {
										for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
											if ( i == CHECKING_SIDE ) continue;
											if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
											if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return -86;
										}
									}
								}
							}

						}
						//----------------------------------------------------------------------------------------------------------------
					}
				}
				//----------------------------------------------------------------------------------------------------------------
				//
				if ( Count == 1 ) {
					//
					if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.10.20
							//----------------------------------------------------------------------------------------------------------------
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) >= PATHDO_WAFER_RETURN ) return 87; // 2004.09.09
							//----------------------------------------------------------------------------------------------------------------
						}
						//----------------------------------------------------------------------------------------------------------------
						// 2004.12.01
						//----------------------------------------------------------------------------------------------------------------
						//----------------------------------------------------------------------------------------------------------------
						if ( TMATM != 0 ) { // 2013.03.18
							if ( Chkjp == 0 ) { // 2016.12.09
								if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
									return -89;
								}
							}
						}
						//
						if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO2 ) { // 2007.09.29
							if ( _SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE ) <= 0 ) {
								if ( _SCH_CASSETTE_LAST_RESULT2_GET( CHECKING_SIDE,Chkpt ) == -2 ) {
									switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
									case 6 :
									case 7 :
										return 88;
										break;
									default : // 2016.07.22
										//
										if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) return 88;
										if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) return 88; // 2018.04.10
										//
										// 2016.12.07 PathReturn이 안된 경우 TM에서 Place BM할때 아래 조건으로 못하게 하는데 여기서 Pick BM 을 해버리면 TM이 Wafer를 2개 들고 대기만 하고 있게 되고 후에 PM을 Enable하게 되면 갈수 있는 PM에 Wafer가 있을 경우 Deadlock이 발생 하여 추가.
										//
										if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) < 3 ) { // 2016.12.28
											if ( ( TMATM == 0 ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
												return -85;
											}
										}
										//
										break;
									}
								}
							}
							else { // 2010.12.08
								if ( ( SwitchMode == 1 ) || ( ( SwitchMode == 2 ) && ( TMATM == 0 ) ) ) {
									//
									if ( ( TMATM != 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2019.01.30
										//
										if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Chkpt ) >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Chkpt ) ) { // 2013.03.18
											if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
												return -91;
											}
										}
										//
									}
									//
									switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
									case 6 :
									case 7 :
										if ( _SCH_SUB_FM_Current_No_Way_Supply( CHECKING_SIDE , Chkpt , -2 ) ) {
											return 89;
										}
										break;
									default : // 2016.07.22
										//
	//									if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) { // 2018.04.10
										if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) { // 2018.04.10
											if ( _SCH_SUB_FM_Current_No_Way_Supply( CHECKING_SIDE , Chkpt , -2 ) ) {
												return 89;
											}
										}
										//
										break;
									}
								}
							}
						}
					}
					//----------------------------------------------------------------------------------------------------------------
					// 2007.01.04
					//----------------------------------------------------------------------------------------------------------------
					if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
						*NextChamber = -1;
//						if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , -1 , Count , SwitchMode , swbm , *JumpMode , *GetChamber , NextChamber , 0 , TRUE , ( MoveOnly != 0 ) ) > 0 ) return 97; // 2018.09.17
//						return -97; // 2018.09.17
						if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , -1 , Count , SwitchMode , swbm , *JumpMode , *GetChamber , NextChamber , 0 , TRUE , ( MoveOnly != 0 ) ) <= 0 ) return -97; // 2018.09.17
					}
					//----------------------------------------------------------------------------------------------------------------
					// 2005.12.14
					// 2004.05.13 이전에는 없던 내용. 추가 함으로써 Track(Multi TM)에서 동작에 문제 발생.
					// 조치는 Switch Mode에서만 return -87 되도록 PathThru에서는 제거(맞는지 추후 확인 필요)
					//----------------------------------------------------------------------------------------------------------------
					//return -87; // 2004.05.13
//						if ( SwitchMode	) return -87; // 2004.05.13 // 2005.12.14 // 2006.12.17
					if ( ( SwitchMode == 1 ) || ( ( SwitchMode == 2 ) && ( TMATM == 0 ) ) ) {
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.10.20
							if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17
								//
								if ( SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , CHECKING_SIDE , &i ) ) { // 2014.08.16
									// (Multi TM)에서 Lot별로 BM을 한개만 사용할시 TM2에서 Pick 할수 있도록 추가
									//
		//							return 86; // 2016.07.22
									//
									if ( TMATM != 0 ) return 86; // 2016.07.22
									//
								}
								//
							}
						}
						//
						// 2014.10.07
						//
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							//
							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) {
								//
								switch( SCHEDULING_CHECK_for_Enable_PLACE_TO_BM_FOR_OUT( TMATM , CHECKING_SIDE , *GetChamber , i , swbm , Count , MoveOnly ) ) {
								case 1 :
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.10.20
										if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return 85; // 2018.09.17
									}
									else { // 2018.10.20
										if ( Chkjp == 0 ) {
											if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return 85; // 2018.09.17
										}
									}
									//
//									return 85; // 2016.07.22
									//
									// 2016.07.22
									//
									if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) >= 3 ) { // 2016.12.28
										return 83;
									}
									else {
										if ( Count_Intlks <= 1 ) {
//											if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) { // 2017.04.06
											if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE ) ) { // 2017.04.06
												return 85;
											}
											else { // 2016.12.09
												if ( Chkjp == 1 ) return 84;
											}
										}
										else {
											return 85;
										}
									}
									break;
								}
								//
							}
							//
						}
						//
						//
						if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return -87; // 2018.09.17
						//
						//
						// return -87; // 2004.05.13 // 2005.12.14 // 2006.12.17 // 2015.08.11
						if ( Count_Intlks <= 1 ) {
//							return -87; // 2015.08.11 // 2018.05.08
							if ( Chkjp == 0 ) return -87; // 2018.05.08
						}
					}
					//----------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------
					// 2007.01.15
					// 모든 Module Disable 시 여기서 문제가 됨
					// 조치는 모두 return -87 되도록(일단 Track(Multi TM))은 추후 생각하기로 함
					//----------------------------------------------------------------------------------------------------------------
//					return -87; // 2007.01.15 // 2015.08.11
//					if ( Count_Intlks <= 1 ) return -87; // 2007.01.15 // 2015.08.11 // 2016.12.09
					if ( Count_Intlks <= 1 ) { // 2016.12.09
						if ( Chkjp == 0 ) return -83;
					}
					//
					if ( !OtherArm_Place_Curr_Get_BM( TMATM , Chkjp , *GetChamber ) ) { // 2018.10.20
						return -831;
					}
					//
				}
				return 8; // 2004.01.28
			} // 2004.01.28
			else { // 2004.01.28
				//
				//
				if ( TMATM == 0 ) { // 2018.10.20
					//
					if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
							//
							if ( Chkjp == 0 ) {
								//
								if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
									return 832;
								}
							}
						}
						//
					}
				}
				//
				//
				return -88; // 2004.01.28
				//
			} // 2004.01.28
			//return 8; // 2004.01.28
		}
		else if ( find == 2 /* (2)Place_PM_OtherOnly_Find */ ) {
			//
			if ( Count == 1 ) {
/*
// 2013.11.13
				if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) ) { // 2003.09.15
					if ( ( TMATM != 0 ) || ( SwitchMode != 2 ) ) { // 2006.12.22
						return -9; // 2003.09.15
					}
				} // 2003.09.15
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) { // 2006.05.25
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
								return 9;
							}
						}
					}
				}
*/
				//
				// 2013.11.13
				//
				//
				// 2016.12.13
				sbm = BM1;
				ebm = ( BM1 + MAX_BM_CHAMBER_DEPTH );
				//
//				if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 4 ) == 0 ) { // 0:SeparateSupply , 1:AllSupply // 2018.09.17
				if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 4 ) == 1 ) { // 0:AllSupply , 1:SeparateSupply // 2018.09.17
					//
					//
					if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17
						//
						if ( ( TMATM + 2 ) < MAX_TM_CHAMBER_DEPTH ) {
							//
							if ( _SCH_PRM_GET_MODULE_MODE( TM + TMATM + 2 ) ) {
								//
								bma = 0;
								bmb = 0;
								//
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) {
										//
										if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) {
											//
											if      ( bma == 0 ) bma = i;
											else if ( bmb == 0 ) bmb = i;
											else                 bma = -1;
											//
										}
										//
									}
								}
								//
								if ( ( bma > 0 ) && ( bmb > 0 ) ) {
									if ( PlusOneGroupPlaceCount <= 0 ) {
										sbm = bmb;
										ebm = bmb+1;
									}
									else {
										sbm = bma;
										ebm = bma+1;
									}
								}
							}
						}
						//
					}
				}
				//
//				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2016.12.13
				for ( i = sbm ; i < ebm ; i++ ) { // 2016.12.13
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2014.06.27
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
								return 9;
							}
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( (TMATM+1) , i ) == BUFFER_SWITCH_MODE ) { // 2013.11.13
							//
							if ( SCHEDULING_CHECK_Enable_Place_To_BM( TMATM , CHECKING_SIDE , Chkpt , -1 , &k , &j , (TMATM+1) , BUFFER_FM_STATION , MoveOnly , i , -1 , FALSE , FALSE , NULL ) ) {
								//
//								return 11; // 2013.12.22
								return 11; // 2013.12.22 // 2014.02.04
								//
								// 2013.12.22
								if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
									return 11;
								}
								else {
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) {
										//
										/*
										// 2014.07.17
										if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) % 100 ) > 0 ) {
											xs = _SCH_MODULE_Get_TM_SIDE( TMATM,TA_STATION );
											xp = _SCH_MODULE_Get_TM_POINTER( TMATM,TA_STATION );
										}
										else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) / 100 ) > 0 ) {
											xs = _SCH_MODULE_Get_TM_SIDE2( TMATM,TA_STATION );
											xp = _SCH_MODULE_Get_TM_POINTER2( TMATM,TA_STATION );
										}
										else {
											return 11;
										}
										*/
										//
										// 2014.07.17
										xs = _SCH_MODULE_Get_TM_SIDE( TMATM,TA_STATION );
										xp = _SCH_MODULE_Get_TM_POINTER( TMATM,TA_STATION );
										//
									}
									else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
										//
										/*
										// 2014.07.17
										if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) % 100 ) > 0 ) {
											xs = _SCH_MODULE_Get_TM_SIDE( TMATM,TB_STATION );
											xp = _SCH_MODULE_Get_TM_POINTER( TMATM,TB_STATION );
										}
										else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) / 100 ) > 0 ) {
											xs = _SCH_MODULE_Get_TM_SIDE2( TMATM,TB_STATION );
											xp = _SCH_MODULE_Get_TM_POINTER2( TMATM,TB_STATION );
										}
										else {
											return 11;
										}
										*/
										//
										// 2014.07.17
										xs = _SCH_MODULE_Get_TM_SIDE( TMATM,TB_STATION );
										xp = _SCH_MODULE_Get_TM_POINTER( TMATM,TB_STATION );
										//
									}
									else {
										return 11;
									}
									//
									if ( !SCHEDULING_CHECK_Enable_Place_To_BM( TMATM , xs , xp , -1 , &k , &j , (TMATM+1) , BUFFER_FM_STATION , MoveOnly , i , -1 , FALSE , FALSE , NULL ) ) {
										if ( SCHEDULING_CHECK_Enable_Place_To_BM( TMATM , CHECKING_SIDE , Chkpt , -1 , &k , &j , (TMATM+1) , BUFFER_FM_STATION , MoveOnly , i , -1 , FALSE , FALSE , NULL ) ) {
											return 11;
										}
									}
									//
								}
							}
							//
						}
					}
				}
				if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) ) {
					if ( ( TMATM != 0 ) || ( SwitchMode != 2 ) ) {
						//
						if ( ( TMATM == 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) ) { // 2018.05.08
							if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
								return 908;
							}
						}
						//
						return -9;
					}
				}
			}
			else {
				return 10;
			}
		}
	}
	else {
		if ( Chkjp == 4 ) *JumpMode	 = 0;
		return 21;
	}
	return -1;
}

/*
// 2013.03.25
BOOL SCHEDULER_FULL_PATH_FULL_in_FREE_WAFER( int tms , int side , int pt , int ch ) { // 2013.03.17
	int i , k , m , d , fc , s , p , x;
	int fw[MAX_CHAMBER];
	int fm[MAX_CHAMBER];
	int fg[MAX_CHAMBER];
	//
	if ( tms != 0 ) return FALSE;
	//
	if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( tms ) == 2 ) {
		d = _SCH_CLUSTER_Get_PathDo( side , pt );
	}
	else {
		if ( SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( tms , -1 , ch , -1 , 1 ) >= 0 ) { // 2013.03.22
			d = _SCH_CLUSTER_Get_PathDo( side , pt );
		}
		else {
			d = _SCH_CLUSTER_Get_PathDo( side , pt ) + 1;
		}
	}
	//
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) > 1 ) return FALSE;
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) >= _SCH_CLUSTER_Get_PathRange( side , pt ) ) return FALSE;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		fw[i] = 0;
		fm[i] = 0;
		fg[i] = 0;
	}
	//
	for ( i = _SCH_CLUSTER_Get_PathDo( side , pt ) ; i < _SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , i , k );
			//
			if ( m > 0 ) {
				//
				fg[m] = 1;
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) { // 2013.03.11
					//
					fm[m] = 1;
					//
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( m ) != -1 ) fw[m] = 1; // 2013.03.22
					//
				}
				else if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) { // 2013.03.11
					fm[m] = 1;
				}
				else { // 2013.03.22
					if ( _SCH_PRM_GET_MODULE_GROUP( m ) != tms ) fg[ _SCH_PRM_GET_MODULE_GROUP( m ) + TM ] = 1;
				}
				//
				if ( i < d ) {
					fw[m] = 1;
				}
				else {
					if ( fw[m] <= 0 ) {
						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) > 0 ) fw[m] = 1;
					}
				}
			}
		}
	}
	//
	d = 0;
	fc = 0;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( fg[i] ) {
			if ( !fw[i] ) {
				fc++;
			}
		}
		//
		if ( fm[i] ) d++;
	}
	//
	if ( d < 2 ) return FALSE;
	//
	for ( i = TM ; i < ( TM + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !fg[i] ) continue;
		//
		for ( k = 0 ; k < 2 ; k++ ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( i - TM , k ) <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_TM_SIDE( i - TM , k );
			p = _SCH_MODULE_Get_TM_POINTER( i - TM , k );
			//
			x = _SCH_CLUSTER_Get_PathDo( s , p );
			//
			if ( ( x != PATHDO_WAFER_RETURN ) && ( ( x - 2 ) >= 0 ) ) {
				//
				for ( d = 0 ; d < MAX_PM_CHAMBER_DEPTH ; d++ ) {
					//
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , x - 2 , d );
					//
					if ( m > 0 ) {
						if ( fg[m] ) {
							if ( !fw[m] ) {
								fw[m] = 1;
								fc--;
							}
						}
					}
					//
				}
			}
		}
	}
	//
	if ( fc <= 0 ) return TRUE;
	//
	return FALSE;
}
*/

// 2013.03.25
BOOL SCHEDULER_FULL_PATH_FULL_in_FREE_WAFER( int tms , int side , int pt , int ch ) { // 2013.03.17
	int i , k , m , d , fc , pd;
//	int s , p , x;
	int fw[MAX_CHAMBER];
	int fm[MAX_CHAMBER];
	int fg[MAX_CHAMBER];
	int swap;
	//
	if ( tms != 0 ) return FALSE;
	//
	swap = FALSE;
	//
	pd = _SCH_CLUSTER_Get_PathDo( side , pt );
	//
	if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( tms ) == 2 ) {
		d = pd;
	}
	else {
		if ( SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( tms , -1 , ch , -1 , 1 ) >= 0 ) { // 2013.03.22
			d = pd;
			swap = TRUE;
		}
		else {
			d = pd + 1;
		}
	}
	//
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) > 1 ) return FALSE;
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) >= _SCH_CLUSTER_Get_PathRange( side , pt ) ) return FALSE;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		fw[i] = 0;
		fm[i] = 0;
		fg[i] = 0;
	}
	//
	for ( i = pd ; i < _SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , i , k );
			//
			if ( m > 0 ) {
				//
				if ( swap ) {
					if ( i == pd ) { // 2013.03.25
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) {
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) {
								if ( SCHEDULER_Get_MULTI_GROUP_PLACE( m ) != -1 ) {
									swap = FALSE;
								}
							}
						}
					}
				}
				//
				fg[m] = 1;
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) { // 2013.03.11
					//
					fm[m] = 1;
					//
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( m ) != -1 ) fw[m] = 1; // 2013.03.22
					//
				}
				else if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) { // 2013.03.11
					fm[m] = 1;
				}
				else { // 2013.03.22
					if ( _SCH_PRM_GET_MODULE_GROUP( m ) != tms ) {
						//
						if ( _SCH_PRM_GET_MODULE_GROUP( m ) >= 0 ) { // 2014.09.24
							//
							if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( tms ) <= 1 ) { // 2013.03.25
								fw[m] = 1;
							}
	//						fg[ _SCH_PRM_GET_MODULE_GROUP( m ) + TM ] = 1; // 2013.03.25
							//
						}
						//
					}
				}
				//
				if ( i < d ) {
					fw[m] = 1;
				}
				else {
					if ( fw[m] <= 0 ) {
//						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) > 0 ) fw[m] = 1; // 2014.01.10
						if ( !SCH_PM_IS_ABSENT( m , side , pt , i ) ) fw[m] = 1; // 2014.01.10
					}
				}
			}
		}
	}
	//
	d = 0;
	fc = 0;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( fg[i] ) {
			if ( !fw[i] ) {
				fc++;
			}
		}
		//
		if ( fm[i] ) d++;
	}
	//
	if ( d < 2 ) return FALSE;
	//
	if ( swap ) return FALSE; // 2013.03.25
	//
	/*
	// 2013.03.25
	for ( i = TM ; i < ( TM + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !fg[i] ) continue;
		//
		for ( k = 0 ; k < 2 ; k++ ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( i - TM , k ) <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_TM_SIDE( i - TM , k );
			p = _SCH_MODULE_Get_TM_POINTER( i - TM , k );
			//
			x = _SCH_CLUSTER_Get_PathDo( s , p );
			//
			if ( ( x != PATHDO_WAFER_RETURN ) && ( ( x - 2 ) >= 0 ) ) {
				//
				for ( d = 0 ; d < MAX_PM_CHAMBER_DEPTH ; d++ ) {
					//
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , x - 2 , d );
					//
					if ( m > 0 ) {
						if ( fg[m] ) {
							if ( !fw[m] ) {
								fw[m] = 1;
								fc--;
							}
						}
					}
					//
				}
			}
		}
	}
	*/
	//
	if ( fc <= 0 ) return TRUE;
	//
	return FALSE;
}




BOOL Scheduler_Deadlock_Rounding_Possible_Another_PM_will_Come_No_Next_for_SingleArm( int nextplacech ) {
	//
	int i , k , m , s , p , pd , pr;
	BOOL hasnext;
	//
	hasnext = FALSE;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( i == nextplacech ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
		//
		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_PM_SIDE( i , 0 );
		p = _SCH_MODULE_Get_PM_POINTER( i , 0 );
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		pr = _SCH_CLUSTER_Get_PathRange( s , p );
		//
		if ( pd < 0 ) continue;
		if ( pd == PATHDO_WAFER_RETURN ) return TRUE;
		if ( pd >= pr ) return TRUE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k );
			//
			if ( m > 0 ) {
				//
				if ( m == nextplacech ) {
					hasnext = TRUE;
				}
				else {
					if ( SCH_PM_IS_ABSENT( m , s , p , pd ) ) {
						return TRUE;
					}
				}
				//
			}
			//
		}
	}
	//
	if ( hasnext ) return FALSE;
	//
	return TRUE;
	//
}

BOOL Scheduler_Deadlock_Rounding_Possible_Before_CM_Pick_for_SingleArm( int s , int p ) { // 2018.07.12
	int m , k;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TB_STATION ) ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		//
		if ( m > 0 ) {
			//
			if ( SCH_PM_IS_ABSENT( m , s , p , 0 ) ) {
				//
				if ( Scheduler_Deadlock_Rounding_Possible_Another_PM_will_Come_No_Next_for_SingleArm( m ) ) {
					return TRUE;
				}
				//
			}
			//
		}
	}
	//
	return FALSE;
	//
}



BOOL CHECK_LOOP_CHAMBER[MAX_CHAMBER];

BOOL Scheduler_Deadlock_Next_Looping( int TMATM , int mode , int s , int p , int d , int Chamber , BOOL *goOtherTMS , BOOL *goOtherPick , BOOL *goOtherPlace ) { // 2013.03.22
	int k , m , fr , pd , ps , pp , pw;
	int pmslot;
	//
	if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	fr = FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
		//
		if ( m > 0 ) {
			//
			if ( CHECK_LOOP_CHAMBER[m] ) {
				fr = TRUE;
				continue;
			}
			//
			if ( SCH_PM_IS_ABSENT( m , s , p , d ) || ( !SCH_PM_HAS_PICKING( m , &pmslot ) ) ) return FALSE; // 2014.01.10
			//
//			pw = _SCH_MODULE_Get_PM_WAFER( m , 0 ); // 2014.01.10
			pw = _SCH_MODULE_Get_PM_WAFER( m , pmslot ); // 2014.01.10
			//
//			ps = _SCH_MODULE_Get_PM_SIDE( m , 0 ); // 2014.01.10
//			pp = _SCH_MODULE_Get_PM_POINTER( m , 0 ); // 2014.01.10
			ps = _SCH_MODULE_Get_PM_SIDE( m , pmslot ); // 2014.01.10
			pp = _SCH_MODULE_Get_PM_POINTER( m , pmslot ); // 2014.01.10
			//
			if ( pw <= 0 ) return FALSE;
			//
			if ( mode == 1 ) { // 2013.03.25
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) return FALSE;
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) return FALSE;
			}
			//
			if ( m == Chamber ) {
				if ( *goOtherTMS && *goOtherPick && *goOtherPlace ) {
					fr = TRUE;
					continue;
				}
				return FALSE;
			}
			//
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) *goOtherPick = TRUE;
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) *goOtherPlace = TRUE;
			//
			if ( ( ps < 0 ) || ( ps >= MAX_CASSETTE_SIDE ) ) return FALSE;
			if ( ( pp < 0 ) || ( pp >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			pd = _SCH_CLUSTER_Get_PathDo( ps , pp );
			//
//			if ( _SCH_PRM_GET_MODULE_GROUP( m ) != TMATM ) { // 2014.09.24
			if ( ( _SCH_PRM_GET_MODULE_GROUP( m ) != TMATM ) && ( _SCH_PRM_GET_MODULE_GROUP( m ) >= 0 ) ) { // 2014.09.24
				//
				*goOtherTMS = TRUE;
				//
				while ( TRUE ) {
					//
					/*
					// 2014.01.10
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , 0 ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , 0 ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , 0 ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , 0 ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , 0 ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , 0 ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , 0 ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , 0 ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , 0 ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					*/
					//
					// 2014.01.10
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , pmslot ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , pmslot ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , pmslot ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , pmslot ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , pmslot ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , pmslot ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					//
					if ( pw != _SCH_MODULE_Get_PM_WAFER( m , pmslot ) ) return FALSE;
					if ( ps != _SCH_MODULE_Get_PM_SIDE( m , pmslot ) ) return FALSE;
					if ( pp != _SCH_MODULE_Get_PM_POINTER( m , pmslot ) ) return FALSE;
					if ( pd != _SCH_CLUSTER_Get_PathDo( ps , pp ) ) return FALSE;
					//
					break;
				}
				//
			}
			//
			CHECK_LOOP_CHAMBER[m] = TRUE;
			//
			if ( Scheduler_Deadlock_Next_Looping( TMATM , 0 , ps , pp , pd , Chamber , goOtherTMS , goOtherPick , goOtherPlace ) ) {
				fr = TRUE;
			}
			else {
				return FALSE;
			}
			//
		}
		//
	}
	return fr;
}

BOOL Scheduler_Deadlock_Rounding_Possible( int TMATM , int Chamber ) { // 2013.03.22
	int s , p , k , pd;
	BOOL goOtherTMS , goOtherPick , goOtherPlace;
	//
	if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) return FALSE;
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) return FALSE;
	//
	for ( k = 0 ; k < 2 ; k++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , k ) <= 0 ) continue;
		//
		for ( p = 0 ; p < MAX_CHAMBER ; p++ ) CHECK_LOOP_CHAMBER[p] = FALSE;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , k );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , k );
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
		//
		goOtherTMS = FALSE;
		goOtherPick = FALSE;
		goOtherPlace = FALSE;
		//
		if ( Scheduler_Deadlock_Next_Looping( TMATM , 1 , s , p , pd , Chamber , &goOtherTMS , &goOtherPick , &goOtherPlace ) ) {
			//
//			_IO_CIM_PRINTF( "[ROUNDING_SELECT] %d %s\n" , s , _SCH_SYSTEM_GET_MODULE_NAME( Chamber ) );
//			_SCH_LOG_LOT_PRINTF( s , "[ROUNDING_SELECT] %s\n" , _SCH_SYSTEM_GET_MODULE_NAME( Chamber ) );
			//
			return TRUE;
		}
		//
	}
	//
	return FALSE;
}

/*

PICK할려는_PM 이 [여러 TM으로 PICK/PLACE 가능] 일때													<1>

PICK할려는_PM 에 있는 Wafer가																		<2>

	이전 Path가 있고																				<3>
	이전 Path가 TMSIDE 와 다른 쪽 PM일때															<4>

	다음 Path가 TMSIDE 쪽 PM이고 그 PM에 Wafer가 있을때														<5-1>

		또는 다음 Path가 없고 TMSIDE가 0일때																<6-2>

PICK하려는 ARM의 반대 ARM에 Wafer가 있고															<6>

	반대 ARM의 Wafer의 다음 Path의 PM이 [여러 TM으로 PICK/PLACE 가능] 이고									<8>

	반대 ARM의 Wafer의 다음 Path의 PM에 Wafer가 다른 TMSIDE의 PM으로 가고 그 PM에 Wafer가 있을때			<9>
*/

int Scheduler_Deadlock_Rounding_Check_SameDiffTM( int TMATM , int s , int p , int d , int sametm , BOOL switchpm , BOOL haswfrcheck ) { // 2013.10.24
	int k , m , c;
	int s2 , p2 , d2 , k2 , m2;
	int pmslot;
	//
	c = 0;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
		//
		if ( m > 0 ) {
			//
			c++;
			//
			if      ( sametm == 1 ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) != TMATM ) return 1;
			}
			else if ( sametm == 0 ){
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) == TMATM ) return 2;
			}
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) < 0 ) return 12; // 2014.09.24
			//
			if ( switchpm ) {
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) {
					if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) {
						return 3;
					}
				}
				//
				//if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return 4; // 2014.01.10
				//
				if ( SCH_PM_IS_ABSENT( m , s , p , d ) || ( !SCH_PM_HAS_PICKING( m , &pmslot ) ) ) return 4; // 2014.01.10
				//
//				s2 = _SCH_MODULE_Get_PM_SIDE( m , 0 ); // 2014.01.10
//				p2 = _SCH_MODULE_Get_PM_POINTER( m , 0 ); // 2014.01.10
				s2 = _SCH_MODULE_Get_PM_SIDE( m , pmslot ); // 2014.01.10
				p2 = _SCH_MODULE_Get_PM_POINTER( m , pmslot ); // 2014.01.10
				//
				d2 = _SCH_CLUSTER_Get_PathDo( s2 , p2 );
				//
				if ( d2 < _SCH_CLUSTER_Get_PathRange( s2 , p2 ) ) {
					//
					for ( k2 = 0 ; k2 < MAX_PM_CHAMBER_DEPTH ; k2++ ) {
						//
//						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return 4; // 2014.01.10
						if ( _SCH_MODULE_Get_PM_WAFER( m , pmslot ) <= 0 ) return 4; // 2014.01.10
						//
						m2 = SCHEDULER_CONTROL_Get_PathProcessChamber( s2 , p2 , d2 , k2 );
						//
						if ( m2 > 0 ) {
//							if ( _SCH_MODULE_Get_PM_WAFER( m2 , 0 ) <= 0 ) return 6; // 2014.01.10
							if ( SCH_PM_IS_ABSENT( m2 , s2 , p2 , d2 ) ) return 6; // 2014.01.10
							if ( _SCH_PRM_GET_MODULE_GROUP( m2 ) == TMATM ) return 7;
							//
							if ( _SCH_PRM_GET_MODULE_GROUP( m2 ) < 0 ) return 8; // 2014.09.24
							//
						}
					}
					//
				}
				//
//				if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return 4; // 2014.01.10
				if ( SCH_PM_IS_ABSENT( m , s , p , d ) ) return 4; // 2014.01.10
				//
			}
			else {
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) return 8;
				//
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) return 9;
				//
				if ( haswfrcheck ) {
//					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return 10; // 2014.01.10
					if ( SCH_PM_IS_ABSENT( m , s , p , d ) ) return 10; // 2014.01.10
				}
				//
			}
			//
		}
	}
	//
	if ( c <= 0 ) return 11;
	//
	return 0;
}
//

BOOL Scheduler_Deadlock_Rounding_Check_TMONLY_NEXT_HAS_SPACE( int TMATM , int s , int p , BOOL tm ) { // 2017.04.27
	int d , k , m;
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( d == PATHDO_WAFER_RETURN ) return FALSE;
	//
	if ( tm ) d--;
	//
	if ( d < 0 ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
		//
		if ( m > 0 ) {
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) != TMATM ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) < 0 ) continue;
			//
			if ( SCH_PM_IS_ABSENT( m , s , p , d ) ) return TRUE;
			//
		}
	}
	//
	return FALSE;
	//
}

int Scheduler_Deadlock_Rounding_Possible2_for_Pick_PM( int TMATM , int s , int p , int Pick_Chamber , int Pick_Slot , int ts , int tp ) { // 2013.10.24
	int Res , d , td;
	//
	//---------------------------------------------------------------
	//
//	if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) return 0; // 2017.04.18 // 2017.04.27
	//
	if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2017.04.27
		//
		if ( Scheduler_Deadlock_Rounding_Check_TMONLY_NEXT_HAS_SPACE( TMATM , s , p , FALSE ) ) return 0;
		if ( Scheduler_Deadlock_Rounding_Check_TMONLY_NEXT_HAS_SPACE( TMATM , ts , tp , TRUE ) ) return 0;
		//
		return 2000;
		//
	}
	//
	//---------------------------------------------------------------
	//
	if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Pick_Chamber ) < 2 ) return 100;					//<1>
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Pick_Chamber ) < 2 ) return 200;					//<1>
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( d <= 1 ) return 300;																							//<3>
	if ( d == PATHDO_WAFER_RETURN ) return 400;																			//<*>
	//
	td = _SCH_CLUSTER_Get_PathDo( ts , tp );
	//
	if ( td == PATHDO_WAFER_RETURN ) return 500;																		//<*>
	//
	Res = Scheduler_Deadlock_Rounding_Check_SameDiffTM( TMATM , s , p , d - 2 , 0 , FALSE , FALSE );				//<4>
	if ( Res != 0 ) return 600 + Res;
	//
	if ( d < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
		Res = Scheduler_Deadlock_Rounding_Check_SameDiffTM( TMATM , s , p , d , 1 , FALSE , TRUE );					//<5-1>
		if ( Res != 0 ) return 700 + Res;
	}
	else {
		if ( TMATM != 0 ) return 800;																					//<5-2>
	}
	//
	if ( td > _SCH_CLUSTER_Get_PathRange( ts , tp ) ) return 900;
	//
	Res = Scheduler_Deadlock_Rounding_Check_SameDiffTM( TMATM , ts , tp , td - 1 , -1 , TRUE , TRUE );				//<7><8>
	if ( Res != 0 ) return 1000 + Res;
	//
	return 0;
}

int SCHEDULING_CHECK_for_Enable_PLACE_TO_BM_FOR_OUT( int TMATM , int side , int pickch , int placech , int swbm , int Count , BOOL MoveOnly ) { // 2014.10.07
	int j , k;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( placech , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( placech ) != TMATM ) ) return 0;
	//
	if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , placech , G_PLACE , G_MCHECK_NO ) ) return 0; // 2018.06.01
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_MIDDLESWAP ) {
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( placech ) || ( _SCH_MODULE_Get_BM_FULL_MODE( placech ) == BUFFER_TM_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( placech , &j , &k , 0 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) > 0 ) {
				if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , pickch , placech , MoveOnly ) ) return 1;
			}
		}
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_BATCH_ALL ) {
		if ( placech != swbm ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( placech , -1 ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_SIDE( placech , 1 ) != side ) return -1;
			}
		}
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( placech , &j , &k , 0 ) > 0 ) {
			if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , pickch , placech , MoveOnly ) ) return 1;
		}
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_FULLSWAP ) {
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( placech ) || ( _SCH_MODULE_Get_BM_FULL_MODE( placech ) == BUFFER_TM_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( placech , &j , &k , 0 ) > 0 ) {
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( TMATM , placech , FALSE ) ) {
					if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , pickch , placech , MoveOnly ) ) return 1;
				}
			}
		}
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_SINGLESWAP ) {
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( placech ) || ( _SCH_MODULE_Get_BM_FULL_MODE( placech ) == BUFFER_TM_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( placech , &j , &k , 0 ) > 0 ) {
				if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , pickch , placech , MoveOnly ) ) return 1;
			}
		}
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( placech , &j , &k , 0 ) > 0 ) {
			if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , pickch , placech , MoveOnly ) ) return 1;
		}
	}
	//
	return 0;
}
//=======================================================================================
int SCHEDULER_INTERLOCK_ALL_BM_PICK_DENY_FOR_MDL( int TMATM , int CHECKING_SIDE ) { // 2015.08.20
	int i , cf;
	//
	cf = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
		//
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM , i ) ) {
		case 1 :
			if      ( cf == 0 ) {
				cf = 1;
			}
			else if ( cf != 1 ) {
				return 0;
			}
			break;

		case 2 :
			if      ( cf == 0 ) {
				cf = 2;
			}
			else if ( cf != 2 ) {
				return 0;
			}
			break;

		default :
			return 0;
			break;
		}
	}
	//
	return cf;
	//
}





int SCHEDULER_CHECK_SUB_ALG6_FORCE_B_ARM( int tms , int ch , int pmslot ) { // 2018.07.12
	//
	// -1	:	Fail
	// 1	:	Change to B Arm
	// 0	:	NoChange
	//
	int fch , fpslot;
	//
	//============================================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO6 ) return 0;
	//============================================================================================================================
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 7 ) == 0 ) { // PM2 : 1 Slot
		//
		fch = PM1 + 1;
		fpslot = 0;
		//
	}
	else { // 2018.06.29
		//
		fch = ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 7 ) / 100 ) - 1 + PM1;
		fpslot = ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 7 ) % 100 ) - 1;
		//
	}
	//
	if ( ( ch == fch ) && ( pmslot == fpslot ) ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) {
			return -1;
		}
		else {
			return 1;
		}
		//
	}
	//
	return 0;
	//
}


int SCHEDULING_CHECK_for_Enable_PICK_from_PM( int TMATM , int CHECKING_SIDE , int Chamber , int pmslot , int *MovingType , int *MovingOrder , int *Next_Chamber , BOOL FMMode , BOOL MoveOnly , int *WaitMode , int prcsgap , int remaintime , int swbm , BOOL *EndMode , BOOL *wy , int *finger , int *halfpic , int srcheck ) {
	int Count , i , j , k , ChkTrg , Side , Pointer , z , z2 , z3 , mmm , m , wfrsts , nx;
	int ChkTrg2 , Side2 , Pointer2;
	int Next_Free_Check; // 2002.04.16
	int nextstockonly , outnospace;
	int Count_Intlks; // 2015.08.10

	*halfpic = FALSE; // 2007.11.09
	*finger = -1; // 2003.05.21
	*Next_Chamber = -1; // 2006.03.27

	if ( !SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_Pick_PM( CHECKING_SIDE , Chamber , halfpic ) ) return -701; // 2007.10.29

//	if ( _SCH_PRM_GET_MODULE_xGROUP( Chamber ) != TMATM )				return -1;

//	if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , Chamber , G_PICK ) ) return -51; // 2015.11.27
	//
	// 2015.11.27
	//
	if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , Chamber , G_PICK ) ) {
		return -51;
	}
	else {
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() == 3 ) {
			if ( ( ( Chamber - PM1 ) % 2 ) == 0 ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( Chamber + 1 , TRUE , CHECKING_SIDE ) ) return -52;
			}
			else {
				if ( !_SCH_MODULE_GET_USE_ENABLE( Chamber - 1 , TRUE , CHECKING_SIDE ) ) return -52;
			}
		}
	}
	//
	if ( pmslot < 0 ) {
		if ( !SCH_PM_HAS_PICKING( Chamber , &pmslot ) ) return -702; // 2014.01.10
	}

	Side    = _SCH_MODULE_Get_PM_SIDE( Chamber , pmslot ); // 2014.01.10
	if ( Side != CHECKING_SIDE )								return -2;

	if ( _SCH_MODULE_Get_PM_WAFER( Chamber , pmslot ) <= 0 )	return -3; // 2014.01.10

	Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ); // 2007.09.05
	if ( Count == 0 )						return -5; // 2007.09.05
	
	Pointer = _SCH_MODULE_Get_PM_POINTER( Chamber , pmslot ); // 2014.01.10
	//
	//===============================================================================================================
	// 2006.02.01
	//===============================================================================================================
	if ( SCHEDULING_CHECK_for_PICKPLACE_from_PM_NEXT_FULLY_REMAP( TRUE , TMATM , CHECKING_SIDE , Pointer , Chamber ) == 0 ) return -57;
	//===============================================================================================================
	if ( SCHEDULER_FULL_PATH_FULL_in_FREE_WAFER( TMATM , CHECKING_SIDE , Pointer , Chamber ) ) return -56;
	//================================================================================================================
	// 2007.09.05
	//================================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
		switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
		case 6 :
		case 7 :
			if ( Count == 1 ) {
				if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
					//
					switch ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , Chamber , 3 , FALSE ) ) {
					case 1 :
						//============================================================================================
						if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) z = TA_STATION;
						else                                                            z = TB_STATION;
						//============================================================================================
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( ">> PATHDO_WAFER_RETURN A-1 >> CHECKING_SIDE = %d , Chamber = PM%d , s=%d,p=%d\n" , CHECKING_SIDE , Chamber - PM1 + 1 , _SCH_MODULE_Get_TM_SIDE( TMATM , z ) , _SCH_MODULE_Get_TM_POINTER( TMATM , z ) );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );

						_SCH_CLUSTER_Set_SupplyID( _SCH_MODULE_Get_TM_SIDE( TMATM , z ) , _SCH_MODULE_Get_TM_POINTER( TMATM , z ) , -9999 );
						_SCH_CLUSTER_Set_PathDo( _SCH_MODULE_Get_TM_SIDE( TMATM , z ) , _SCH_MODULE_Get_TM_POINTER( TMATM , z ) , 2 );
						_SCH_MODULE_Set_TM_TYPE( TMATM , z , SCHEDULER_MOVING_OUT );
						//============================================================================================
						return -58;
						break;
					case 2 :
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( ">> PATHDO_WAFER_RETURN A-2 >> CHECKING_SIDE = %d , Chamber = PM%d , s=%d,p=%d\n" , CHECKING_SIDE , Chamber - PM1 + 1 , _SCH_MODULE_Get_TM_SIDE( TMATM , z ) , _SCH_MODULE_Get_TM_POINTER( TMATM , z ) );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
						return -59;
						break;
					}
				}
			}
			//================================================================================================================
			break;
		}
	}

	*MovingOrder = _SCH_MODULE_Get_PM_PATHORDER( Chamber , pmslot ); // 2014.01.10

	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( Side , Pointer , 2 ) == 0 ) return -21;

	*wy = TRUE;
	*EndMode = FALSE;

	//=================================================================================
	if ( !MoveOnly && ( _SCH_PRM_GET_Getting_Priority( Chamber ) >= 0 ) ) { // 2007.01.02
		if ( SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PROCESS( CHECKING_SIDE , Chamber , &m ) ) { // 2007.10.29
			*WaitMode = TRUE;
			if ( prcsgap <= 0 ) return -4;
			if ( prcsgap < remaintime ) return -4;
		}
		else {
			*WaitMode = FALSE;
		}
	}
	else { // 2007.01.02
		*WaitMode = FALSE; // 2007.01.02
	} // 2007.01.02


//	Count = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ); // 2007.09.05
//	if ( Count == 0 )						return -5; // 2007.09.05

	//===========================================================================================================
	// 2006.03.23
	//===========================================================================================================
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
		if ( FMMode ) { // 2006.05.25
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) >= 2 ) {
				if ( ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) ) { // 2006.12.21
					j = 0;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) { // 2009.08.21
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
							if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) {
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) {
									//
									if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
										if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
									}
									//
									j = 1;
									break;
								}
							}
						}
					}
					if ( j == 0 ) return -58;
				}
			}
		}
		else {
			if ( TMATM != 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , Chamber , G_PICK ) ) {
					return -57;
				}
			}
		}
		//
		// 2015.08.20
		//
		if ( Count >= 2 ) {
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,Chamber ) ) {
			case 0 :
				//
				switch( SCHEDULER_INTERLOCK_ALL_BM_PICK_DENY_FOR_MDL( TMATM , Side ) ) {
				case 0 :
					break;
				case 1 :
					if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) > 0 ) return -71;
					break;
				case 2 :
					if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) > 0 ) return -72;
					break;
				}
				//
				break;
			}
		}
		//
	}
	//===========================================================================================================
	Count_Intlks = Count; // 2015.08.10
	//
	switch( SCHEDULING_CHECK_for_BPM_ARM_INTLKS( TMATM , Chamber , Side , Pointer , FALSE ) ) { // 2013.11.27
	case 1 :
		Count = 1;
		break;
	case -1 :
		return -19;
		break;
	}
	//
	switch( SCHEDULER_CHECK_SUB_ALG6_FORCE_B_ARM( TMATM , Chamber , pmslot ) ) { // 2018.07.12
	case -1 :
		return -199;
		break;
	}
	//
	//===========================================================================================================
	if ( Count == 1 ) { // 2002.04.27
		//
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,Chamber ) ) { // 2003.02.05
		case 1 :
			if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TA_STATION) > 0 ) return -20;
			break;
		case 2 :
			if ( _SCH_MODULE_Get_TM_WAFER(TMATM , TB_STATION) > 0 ) return -20;
			break;
		} // 2003.02.05
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) {
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) {
				if ( !Get_RunPrm_TM_INTERLOCK_USE_CHECK( TMATM , Chamber , _SCH_MODULE_Get_TM_OUTCH( TMATM , TA_STATION ) ) ) {
					return -21;
				}
			}
			else {
				if ( !Get_RunPrm_TM_INTERLOCK_USE_CHECK( TMATM , Chamber , _SCH_MODULE_Get_TM_OUTCH( TMATM , TB_STATION ) ) ) {
					return -21;
				}
			}
		}
		//=========================================================
		// 2003.05.21
		//=========================================================
		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == PATHDO_WAFER_RETURN ) {
				ChkTrg = _SCH_CLUSTER_Get_PathIn( Side , Pointer );
			}
			else {
				ChkTrg = _SCH_CLUSTER_Get_PathOut( Side , Pointer );
			}
			//
			if ( ChkTrg >= 0 ) { // 2017.01.23
				//
				k = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg );
				//
				if ( k > 0 ) {
					if ( Chamber != ( k - 1 ) + PM1 ) {
						//
	//					if ( _SCH_MODULE_Get_PM_WAFER( ( k - 1 ) + PM1 , 0 ) > 0 ) { // 2014.01.10
						if ( !SCH_PM_IS_FREE( ( k - 1 ) + PM1 ) ) { // 2014.01.10
							//
							if ( SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , -1 , Chamber , -1 , 1 ) < 0 ) return -32; // 2007.09.17
							//
						}
					}
				}
				switch( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ChkTrg ) ) { // 2004.01.30
				case 2 :
				case 3 :
				case 5 :
				case 6 :
					if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) {
						if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) {
							return -44;
						}
						else {
							Side2 = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
							Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
						}
					}
					else if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) {
						if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) {
							return -45;
						}
						else {
							Side2 = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
							Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
						}
					}
					if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) == PATHDO_WAFER_RETURN ) return -46;
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , k );
						//-------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
						//-------------------------------------------------------------------------------------
						if ( ChkTrg > 0 ) {
							if ( Chamber == ChkTrg ) {
								break;
							}
	//						if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
							if ( SCH_PM_IS_ABSENT( ChkTrg , Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 ) ) { // 2014.01.10
								break;
							}
						}
					}
					if ( k == MAX_PM_CHAMBER_DEPTH ) return -47;
					break;
				}
			}
			//
		}
		else { // 2004.01.30
			z = -1;
			if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
				Side2 = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) == PATHDO_WAFER_RETURN ) {
					z = _SCH_CLUSTER_Get_PathIn( Side2 , Pointer2 );
				}
				else {
					z = _SCH_CLUSTER_Get_PathOut( Side2 , Pointer2 );
				}
			}
			else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
				Side2 = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) == PATHDO_WAFER_RETURN ) {
					z = _SCH_CLUSTER_Get_PathIn( Side2 , Pointer2 );
				}
				else {
					z = _SCH_CLUSTER_Get_PathOut( Side2 , Pointer2 );
				}
			}
//			if ( z != -1 ) { // 2017.01.23
			if ( z >= 0 ) { // 2017.01.23
				switch( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( z ) ) {
				case 2 :
				case 3 :
				case 5 :
				case 6 :
					if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == PATHDO_WAFER_RETURN ) return -48;
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
						//-------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
						//-------------------------------------------------------------------------------------
						if ( ChkTrg > 0 ) {
							if ( Chamber == ChkTrg ) {
								break;
							}
//							if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
							if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) ) { // 2014.01.10
								break;
							}
						}
					}
					if ( k == MAX_PM_CHAMBER_DEPTH ) return -49;
					break;
				}
			}
		}
		//=========================================================
		// 2003.03.27
		//=========================================================
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) {
				//----------------------------------------------------------------------------------
//				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH ) return -41; // 2003.11.07 // 2004.03.24
				//----------------------------------------------------------------------------------
				i = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
				j = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( i , j ) == PATHDO_WAFER_RETURN ) {
					ChkTrg = _SCH_CLUSTER_Get_PathIn( i , j );
				}
				else {
					//----------------------------------------------------------------------------------
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) {
						if ( !SCHEDULING_CHECK_TM_GetOut_BM_Conflict_Check_for_Switch( i ) ) return -41; // 2006.05.24
						// return -41; // 2004.03.24 // 2006.05.24
					}
					//----------------------------------------------------------------------------------
					ChkTrg = _SCH_CLUSTER_Get_PathOut( i , j );
				}
				//
				if ( ChkTrg >= 0 ) { // 2017.01.23
					//
					k = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg );
					if ( k > 0 ) {
						if ( Chamber != ( k - 1 ) + PM1 ) {
	//						if ( _SCH_MODULE_Get_PM_WAFER( ( k - 1 ) + PM1 , 0 ) > 0 ) { // 2014.01.10
							if ( !SCH_PM_IS_FREE( ( k - 1 ) + PM1 ) ) { // 2014.01.10
								return -31;
							}
						}
					}
					//
				}
				//
			}
		}
		else if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) {
				//----------------------------------------------------------------------------------
//				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH ) return -41; // 2003.11.07 // 2004.03.24
				//----------------------------------------------------------------------------------
				i = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
				j = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( i , j ) == PATHDO_WAFER_RETURN ) {
					ChkTrg = _SCH_CLUSTER_Get_PathIn( i , j );
				}
				else {
					//----------------------------------------------------------------------------------
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) {
						if ( !SCHEDULING_CHECK_TM_GetOut_BM_Conflict_Check_for_Switch( i ) ) return -41; // 2006.05.24
						// return -41; // 2004.03.24 // 2006.05.24
					}
					//----------------------------------------------------------------------------------
					ChkTrg = _SCH_CLUSTER_Get_PathOut( i , j );
				}
				//
				if ( ChkTrg >= 0 ) { // 2017.01.23
					//
					k = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg );
					if ( k > 0 ) {
						if ( Chamber != ( k - 1 ) + PM1 ) {
	//						if ( _SCH_MODULE_Get_PM_WAFER( ( k - 1 ) + PM1 , 0 ) > 0 ) { // 2014.01.10
							if ( !SCH_PM_IS_FREE( ( k - 1 ) + PM1 ) ) { // 2014.01.10
								return -31;
							}
						}
					}
					//
				}
			}
		}
		//=========================================================
	}

	*MovingType  = SCHEDULER_MOVING_NEXT;
	//
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) { // last
		*EndMode = TRUE;
		*MovingType  = SCHEDULER_MOVING_OUT;
		if ( FMMode ) {
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
				if ( Count == 2 ) return 1;
				if ( _SCH_MODULE_Get_FM_MidCount() != 11 ) {
					i = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
					if ( _SCH_MODULE_Get_BM_WAFER( i , 1 ) <= 0 ) return 14;
					else {
						if ( _SCH_MODULE_Get_BM_STATUS( i , 1 ) == BUFFER_OUTWAIT_STATUS ) return 15;
						//
						Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , -1 , Chamber , -1 , -1 ); // 2007.09.17
						if ( Side2 == 0 ) return 116;
						if ( Side2 == 1 ) return 117;
					}
					return -11;
				}
				else {
					return -12;
				}
			}
			else {
				//=========================================================================================================================
				if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) { // 2006.12.21
					//=======================================================================================================================================
					// 2007.02.05
					//=======================================================================================================================================
					nx = -1;
//					if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count , TRUE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.11
//					if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count_Intlks , TRUE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.11 // 2018.06.26
					i = SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count_Intlks , TRUE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ); // 2015.08.11 // 2018.06.26
					if ( i < 0 ) { // 2015.08.11 // 2018.06.26
//						return -501; // 2018.06.26
						return -501 + ( i * 1000 ); // 2018.06.26
					}
					//=======================================================================================================================================
					i = 0;
					//
					if ( Count == 1 ) { // 2013.03.18
						if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , Pointer ) >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pointer ) ) {
							if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
								i = 1;
							}
						}
					}
					//
					if ( i == 0 ) { // 2013.03.18
						//
						if ( swbm != 0 ) { // 2004.03.24
							if ( TMATM != 0 ) { // 2013.05.03
								i = swbm;
							}
							else {
								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) {
									i = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE );
								}
								else {
									i = swbm;
								}
							}
							//
							*Next_Chamber = i;
							//
							/*
							// 2014.10.07
							//
							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
									if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.11.05
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) > 0 ) {
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
										}
									}
								}
								else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2006.03.21
									if ( i != swbm ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
											if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != CHECKING_SIDE ) return -132;
										}
									}
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
									}
								}
								else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.09.17
									if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.09.17
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) { // 2007.09.17
											if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( TMATM , i , FALSE ) ) {
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.09.17
											}
										}
									}
								}
								else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2007.09.17
									if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.09.17
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) { // 2007.09.17
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.09.17
										}
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
									}
								}
							}
							*/
							//
							// 2014.10.07
							//
							switch( SCHEDULING_CHECK_for_Enable_PLACE_TO_BM_FOR_OUT( TMATM , CHECKING_SIDE , Chamber , i , swbm , Count , MoveOnly ) ) {
							case 1 :
								return 2;
								break;
							case -1 :
								return -132;
								break;
							}
							//
						}
						else {
							//=======================================================================================================================================
							// 2007.02.05
							//=======================================================================================================================================
							nx = -1;
//							if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count , TRUE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.10
							if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count_Intlks , TRUE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.10
								return -502;
							}
							//=======================================================================================================================================
							//==================================================================================
							// 2007.01.04
							//==================================================================================
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								*Next_Chamber = i;
								//
	//							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2009.08.21
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
									//
									if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
										if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
									}
									//
									/*
									// 2014.10.07
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.11.05
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) > 0 ) {
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
											}
										}
									}
									else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2006.03.21
										if ( i != swbm ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
												if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != CHECKING_SIDE ) return -132;
											}
										}
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
										}
									}
									else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.05.21
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.05.21
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
												if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( TMATM , i , FALSE ) ) {
													if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
												}
											}
										}
									}
									else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2007.09.17
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2007.09.17
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) { // 2007.09.17
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.09.17
											}
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
										}
									}
									*/
									//
									// 2014.10.07
									//
									switch( SCHEDULING_CHECK_for_Enable_PLACE_TO_BM_FOR_OUT( TMATM , CHECKING_SIDE , Chamber , i , swbm , Count , MoveOnly ) ) {
									case 1 :
										return 2;
										break;
									case -1 :
										return -132;
										break;
									}
									//
								}
							}
							//==================================================================================
						}
					}
				}
				else {
					//=======================================================================================================================================
					// 2007.02.05
					//=======================================================================================================================================
					nx = -1;
//					if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count , FALSE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.10
					if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , Count_Intlks , FALSE , swbm , 1 , Chamber , &nx , 0 , TRUE , MoveOnly ) < 0 ) { // 2015.08.10
						return -503;
					}
					//=======================================================================================================================================
					if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							//
							*Next_Chamber = i;
							//
//							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2009.08.21
							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
								//
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
								}
								//
								if (
									( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) ||
									( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) // 2002.10.02
									) {
//									if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) > 0 ) {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_DBL( i ) > 0 ) { // 2015.08.28
										if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 0 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
									}
								}
							}
						}
					}
					else {
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // first ordering // 2002.11.05
							return 2;
						}
						else {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
	//									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2006.05.25
								//
								*Next_Chamber = i;
								//
//								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) { // 2006.05.25 // 2009.08.21
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
									//
									if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
										if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
									}
									//
									if (
	//											( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) || // 2006.05.25
										( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) || // 2006.05.25
										( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) // 2002.10.02
										){
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
											//return 2; // 2004.12.28
											if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , Chamber , i , MoveOnly ) ) return 2; // 2007.01.04
										}
									}
								}
							}
						}
					}
					//================================
					// Check Append 2001.11.29
					//================================
					if ( Count == 1 ) {
						j = 1;
						if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
							Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
							Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
							if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
								j = 0;
							}
						}
						else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
							Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
							Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
							if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
								j = 0;
							}
						}
						if ( j == 1 ) {
							if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
	//										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2006.05.25
//									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) { // 2006.05.25 // 2009.08.21
//									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21 // 2015.07.03
									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21 // 2015.07.03
	//											if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_MODE ) { // 2006.05.25
										//
										if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
											if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
										}
										//
//										if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) { // 2006.05.25 // 2015.07.03
										if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) { // 2006.05.25 // 2015.07.03
//											if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) > 0 ) { // 2015.07.02
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_DBL( i ) > 0 ) { // 2015.07.02
												//==================================================================================
												if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , Chamber , i , MoveOnly ) ) break; // 2007.01.04
												//==================================================================================
											}
										}
									}
								}
								if ( i >= ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) return -6;
							}
							else {
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2002.11.05
									i = 0;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
	//											if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2006.05.25
//										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) { // 2006.05.25 // 2009.08.21
										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											if (
	//													( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) || // 2006.05.25
												( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) || // 2006.05.25
												( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) // 2002.10.02
												) {
												if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
													//break; // 2004.12.28
													//==================================================================================
													if ( Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 1 , Count , Chamber , i , MoveOnly ) ) break; // 2007.01.04
													//==================================================================================
												}
											}
										}
									}
								}
								if ( i >= ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) return -6;
							}
						}
//						else { // 2008.04.23
							//-------------------------------------------------------------------------------------
							// 2008.04.10 // 2008.04.23
							//-------------------------------------------------------------------------------------
//							return -6; // 2008.04.23
							//-------------------------------------------------------------------------------------
//						} // 2008.04.23
					}
				}
				//
				// 2001.05.04 PSK Test : Comment Because Wait COOL Free But ( Arm A Will -> PM1 , Arm B Free , PM1 Will Out )
				//return -6;
			}
			//==============================================================================================================
			// 2006.03.22 // 2006.03.28
			//==============================================================================================================
			if ( !Sch_NextPrev_Free_Pick_Check_for_Place( TMATM , 2 , Count , Chamber , -1 , MoveOnly ) ) return -201; // 2007.01.04
			if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return -202; // 2007.01.04
//				if ( ( bmplccheck == 2 ) || ( bmplccheck == 3 ) ) return -201;
			//==============================================================================================================
		}
		else {
			return 3;
		}
	}
	else { // 2017.09.07
		//
		SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM_SUB( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , Chamber );
		//
	}

	Next_Free_Check = _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( Chamber ); // 2002.04.16
//	if ( Next_Free_Check == 0 ) { // 2002.04.16 // 2003.05.21
//		if ( ( Count == 2 ) && ( TMATM == 0 ) ) return 4; // 2003.05.21
//	} // 2003.05.21
//	if ( Count == 2 ) {
////		if ( !SCHEDULING_CHECK_for_NOT_CROSS_OVER( TMATM , Chamber , CHECKING_SIDE , -1 ) ) return -7;
//		return 4;
//	}

	nextstockonly = 0;
	*EndMode = FALSE;
	z3 = 0; // 2003.03.31
	z = 0;
	z2 = -1;
	mmm = 0; // 2014.10.07
	//
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
		//============================================================================================================
		// 2004.12.14 // 2004.12.28
		//============================================================================================================
		if ( Next_Free_Check == 7 ) { // 2004.12.14
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_7( 1 , TMATM , 0 , Count , Side , Pointer ) ) { // 2006.07.13
				return -201;
			}
		}
		else if ( Next_Free_Check == 17 ) { // 2006.07.13
			if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE_17( 1 , TMATM , 0 , Count , Side , Pointer ) ) { // 2006.07.13
				return -202;
			}
		}
		//============================================================================================================
		// 2006.04.15
		//============================================================================================================
		else if ( Next_Free_Check == 101 ) {
//printf( "[101]====>1 [%d][PM%d]\n" , TMATM , Chamber - PM1 + 1 );
			if ( !SCHEDULER_CONTROL_Special_Case_Checking( 0 , TMATM , Chamber , Side , Pointer , 0 , &nextstockonly , FALSE ) ) { // 2006.04.15
//printf( "[101]====>2 [%d][PM%d]\n" , TMATM , Chamber - PM1 + 1 );
				return -1001;
			}
//printf( "[101]====>3 [%d][PM%d][%d]\n" , TMATM , Chamber - PM1 + 1 , nextstockonly );

//				if ( nextstockonly == 1 ) *Next_stockfix = TRUE; // 2006.05.15
		}
		else if ( Next_Free_Check == 102 ) {
//printf( "[102]====>1 [%d][PM%d]\n" , TMATM , Chamber - PM1 + 1 );
			if ( !SCHEDULER_CONTROL_Special_Case_Checking( 1 , TMATM , Chamber , Side , Pointer , 0 , &nextstockonly , FALSE ) ) { // 2006.04.15
//printf( "[102]====>2 [%d][PM%d]\n" , TMATM , Chamber - PM1 + 1 );
				return -1002;
			}
//printf( "[102]====>3 [%d][PM%d][%d]\n" , TMATM , Chamber - PM1 + 1 , nextstockonly );
		}
		//============================================================================================================
		//============================================================================================================
		z3 = 1; // 2003.03.31
//		mmm = 0; // 2014.10.07
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			*finger = -1; // 2003.05.21
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			//=======================================================================================
			// 2006.04.15
			//=======================================================================================
			if ( ChkTrg > 0 ) {
				if ( nextstockonly == 1 ) {
					if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
						ChkTrg = -1;
					}
				}
				else if ( nextstockonly >= 2 ) {
					if ( _SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
						ChkTrg = -1;
					}
				}
			}
			//=======================================================================================
			// 2016.05.30
			//=======================================================================================
			if ( ChkTrg > 0 ) {
				if ( SCH_PM_Check_Impossible_Place_MultiPM( Side , Pointer , ChkTrg ) ) ChkTrg = -1;
			}
			//=======================================================================================
			if ( ChkTrg > 0 ) {
				//================================================================================================
				// 2006.03.27
				//================================================================================================
//				wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ); // 2014.01.10
				if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) || ( !SCH_PM_HAS_PICKING( ChkTrg , &pmslot ) ) ) { // 2014.01.10
					wfrsts = 0;
				}
				else {
					wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , pmslot );
				}
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
					if ( ( wfrsts <= 0 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) ) wfrsts = 99; // 2006.07.13 // 2007.01.02
				}
				//
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
					if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , ChkTrg ) ) { // 2007.03.20
						z++;
						//=============================================================================================================
						if ( !SCHEDULING_CHECK_for_NEXT_FREE_PICK_POSSIBLE( TMATM , Count , Chamber , 1 , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , ChkTrg , FALSE ) ) { // 2006.07.21
							continue;
						}
						//=============================================================================================================
						//===============================================================================
						// 2003.05.21
						//===============================================================================
						if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) { // 2015.08.27
							if ( ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) { // 2003.05.22
								m = SCHEDULING_CHECK_INTERLOCK_PM_PLACE_DENY_FOR_MDL( TMATM , ChkTrg , Side , Pointer , ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 ) ); // 2003.05.22
							} // 2003.05.22
							else {
								m = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg ); // 2003.02.05
							}
						}
						else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) { // 2015.08.27
							m = 1;
						}
						else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) { // 2015.08.27
							m = 2;
						}
						else { // 2015.08.27
							m = 0;
						}
						//
						// 2012.07.10
						//
						switch( m ) {
						case 1 :
							if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) continue;
							//
							if ( Count == 1 ) {
								if ( wfrsts > 0 ) {
									if ( TB_STATION != SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TB_STATION , Chamber , -1 , -1 ) ) {
										continue;
									}
								}
							}
							//
							*finger = TA_STATION;
							break;
						case 2 :
							if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) continue;
							//
							if ( Count == 1 ) {
								if ( wfrsts > 0 ) {
									if ( TA_STATION != SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TA_STATION , Chamber , -1 , -1 ) ) {
										continue;
									}
								}
							}
							//
							*finger = TB_STATION;
							break;
						}
						//===============================================================================
						if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) { // 2013.03.29
							*Next_Chamber = ChkTrg; // 2013.03.11
						}
						//===============================================================================
						if ( ( m == 0 ) && ( Count == 2 ) ) return 4;
						//===============================================================================
						if ( wfrsts > 0 ) {
							//=========================================================================
							// 2007.01.16
							//=========================================================================
							if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) { // 2013.03.29
								if ( Count == 1 ) *Next_Chamber = ChkTrg; // 2013.03.11
							}
							//=========================================================================
							if ( Count == 1 ) { // 2002.08.09
								if ( CROSS_OVER_SIMPLE_CHECK ) { // 2012.10.31
									//
								}
								else {
									//
									if ( ChkTrg != Chamber ) { // 2014.06.20
									//
		//									if ( _SCH_CLUSTER_Check_HasProcessData_Post( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , *MovingOrder ) ) { // 2006.01.06
										if ( _SCH_CLUSTER_Check_HasProcessData_Post( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , *MovingOrder ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , Chamber ) ) { // 2006.01.06
		//										return -111; // 2002.11.07
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PICK ) ) { // 2006.01.29
												if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) { // 2007.01.15
													if ( SCHEDULER_CONTROL_Check_BM_Has_Space_forSW( TMATM ) ) { // 2009.04.15
														if ( mmm == 0 ) mmm = 1;
														//
														if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) { // 2013.03.29
															*Next_Chamber = -1; // 2013.03.11
														}
														//
														continue; // 2002.11.07
													}
												}
											}
										}
									}
								}
							}
							mmm = 2;
							if ( Chamber == ChkTrg ) return 5;
							if ( ( ( m == 0 ) || ( m == 1 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
								if ( SCHEDULER_CONTROL_Check_Arm_Place_BM_Has_Space( FMMode , TMATM , TA_STATION , swbm , FALSE ) ) { // 2007.10.10
									return 6;
								}
								//
								Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TA_STATION , Chamber , -1 , -1 ); // 2007.09.17
								if ( Side2 == TA_STATION ) return 116;
							}
							else if ( ( ( m == 0 ) || ( m == 2 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
								if ( SCHEDULER_CONTROL_Check_Arm_Place_BM_Has_Space( FMMode , TMATM , TB_STATION , swbm , FALSE ) ) { // 2007.10.10
									return 7;
								}
								//
								Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TB_STATION , Chamber , -1 , -1 ); // 2007.09.17
								if ( Side2 == TB_STATION ) return 117;
							}
							if ( ( m != 0 ) && ( Count == 2 ) ) {
								if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,ChkTrg ) == 0 ) { // 2003.06.02
									if ( SCHEDULING_CHECK_INTERLOCK_DENY_PICK_POSSIBLE( TMATM , ChkTrg , Side , Pointer , ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 ) ) ) { // 2003.06.03
										return 77;
									}
								}
								else if ( m == 1 ) { // 2003.07.31
									if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ChkTrg ) != 1 ) {
										return 78;
									}
								}
								else if ( m == 2 ) { // 2003.07.31
									if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,ChkTrg ) != 2 ) {
										return 79;
									}
								}
							}
							//================================================================================================================================
							// 2006.05.25
							//================================================================================================================================
							if ( Count == 1 ) {
								//
								// 2012.07.10
								//
								if ( ( ( m == 0 ) || ( m == 1 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) ) {
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
										//
										if ( TB_STATION == SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TB_STATION , Chamber , -1 , -1 ) ) {
											return 126;
										}
										//
									}
								}
								else if ( ( ( m == 0 ) || ( m == 2 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) ) {
									if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
										//
										if ( TA_STATION == SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TA_STATION , Chamber , -1 , -1 ) ) {
											return 127;
										}
										//
									}
								}
								//
								if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) >= 2 ) {
									if ( SCHEDULING_CHECK_OTHER_GROUP_HAS_CLEAR_POSSIBLE( TMATM , Chamber , Side , Pointer , nextstockonly ) ) { // 2006.05.25
//										return 121; // 2016.12.12
										return 124; // 2016.12.12
									}
								}
							}
							//================================================================================================================================
							//=========================================================================
							if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) { // 2013.03.29
								*Next_Chamber = -1; // 2007.01.15 // 2013.03.11
							}
							//=========================================================================
						}
						else {
							//=========================================================================
							// 2006.03.27
							//=========================================================================
							if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) { // 2013.03.29
								if ( Count == 1 ) *Next_Chamber = ChkTrg; // 2006.03.27 // 2013.03.11
							}
							//=========================================================================
							mmm = 2;
							if ( ( ( m == 0 ) || ( m == 1 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
								//
								if ( !FMMode || ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) ) {
									if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) return 10;
								}
								//
								if ( ( TMATM != 0 ) && ( ( Next_Free_Check == 14 ) || ( Next_Free_Check == 15 ) || ( Next_Free_Check == 16 ) ) ) return 61; // 2006.02.13
								//
								Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TA_STATION , Chamber , ChkTrg , -1 ); // 2007.09.17
								if ( Side2 == TA_STATION ) return 11;
							}
							else if ( ( ( m == 0 ) || ( m == 2 ) ) && ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
								//
								if ( !FMMode || ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) ) {
									if ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) return 12;
								}
								//
								if ( ( TMATM != 0 ) && ( ( Next_Free_Check == 14 ) || ( Next_Free_Check == 15 ) || ( Next_Free_Check == 16 ) ) ) return 62; // 2006.02.13
								//
								Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TB_STATION , Chamber , ChkTrg , -1 ); // 2007.09.17
								if ( Side2 == TB_STATION ) return 13;
							}
							if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
								if ( !FMMode || ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) ) return 14;
							}
							else {
								if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , Chamber , ChkTrg , Chamber ) ) return 15;
							}
							//=========================================================================
							if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) != 0 ) { // 2013.03.29
								*Next_Chamber = -1; // 2006.03.27 // 2013.03.11
							}
							//=========================================================================
						}
						//=========================================================================
						if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) { // 2013.03.29
							*Next_Chamber = -1; // 2013.03.11
						}
						//=========================================================================
					}
				}
//					else if ( _SCH_PRM_GET_MODULE_xGROUP( ChkTrg ) != TMATM ) {
				else {
					mmm = 2;
					z2 = _SCH_PRM_GET_MODULE_GROUP( ChkTrg );
				}
			}
		}
		if ( mmm == 1 ) return -111; // 2002.11.07
	}
	else { // 2006.05.25
		//============================================================================================================
		// 2006.05.25
		//============================================================================================================
		if ( ( Count == 1 ) && FMMode && ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) && ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM != 0 ) ) ) ) { // 2006.12.21
//			if ( !_SCH_CLUSTER_Check_HasProcessData_Post( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , *MovingOrder ) && !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , Chamber ) ) { // 2012.10.31
			if ( CROSS_OVER_SIMPLE_CHECK || ( !_SCH_CLUSTER_Check_HasProcessData_Post( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , *MovingOrder ) && !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , Chamber ) ) ) { // 2012.10.31
				if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
					*finger = TB_STATION;
					//
					Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TA_STATION , Chamber , -1 , -1 ); // 2007.09.17
					if ( Side2 == TA_STATION ) {
						*Next_Chamber = Chamber;
						return 607;
					}
				}
				else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
					*finger = TA_STATION;
					//
					Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , TB_STATION , Chamber , -1 , -1 ); // 2007.09.17
					if ( Side2 == TB_STATION ) {
						*Next_Chamber = Chamber;
						return 609;
					}
				}
			}
		}
		//============================================================================================================
	}
	//
	*finger = -1; // 2003.05.21
	//=========================================================================
	*Next_Chamber = -1; // 2007.06.28
	//=========================================================================
	if ( z == 0 ) {
		//============================================================================================
		// 2009.06.28
		//============================================================================================
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) >= 2 ) {
			if ( ( mmm == 2 ) && ( z2 != -1 ) ) {
				return -1024;
			}
		}
		//============================================================================================
		*EndMode = TRUE;
		if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2002.11.05
			return 88;
		}
		//============================================================================================
		if ( ( Count == 1 ) && ( z2 == -1 ) && ( TMATM != 0 ) && ( z3 == 1 ) && ( mmm == 0 ) ) { // 2010.08.05
			//
			if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
				//
				Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
				//
				i = 0;
				j = 0;
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , k );
						if ( ChkTrg2 > 0 ) {
							if      ( ChkTrg2 == Chamber ) {
								i = 1;
							}
							else {
								j = 1;
								break;
							}
						}
					}
				}
				//
				if ( ( i == 1 ) && ( j == 0 ) ) return 2011;
				//
			}
			else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
				Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
				//
				i = 0;
				j = 0;
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , k );
						if ( ChkTrg2 > 0 ) {
							if      ( ChkTrg2 == Chamber ) {
								i = 1;
							}
							else {
								j = 1;
								break;
							}
						}
					}
				}
				//
				if ( ( i == 1 ) && ( j == 0 ) ) return 2012;
				//
			}
		}
		//============================================================================================
//		if ( ( z2 == -1 ) && ( TMATM == 0 ) ) { // 2013.03.19
		if ( z2 == -1 ) { // 2013.03.19
			//
			if ( z3 != 0 ) { // 2010.12.09
				if ( !MoveOnly ) {
					if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
						switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
						case 6 :
						case 7 :
							if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
								_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN );
								z3 = 0;
							}
							break;
						default : // 2016.07.22
//							if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) { // 2018.04.10
							if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) { // 2018.04.10
								if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
									_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN );
									z3 = 0;
								}
							}
							break;
						}
					}
				}
			}
			//
			if ( z3 == 0 ) { // 2003.03.31
//				if ( FMMode && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) ) { // 2003.11.10 // 2006.12.21 // 2013.03.19
//				if ( FMMode && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) ) { // 2003.11.10 // 2006.12.21 // 2013.03.19 // 2016.12.12
				if ( FMMode && ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) ) { // 2003.11.10 // 2006.12.21 // 2013.03.19 // 2016.12.12
					if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
						//
//						if ( TMATM == 0 ) { // 2013.03.22 // 2016.12.13
							//
							Side2 = SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , -1 , Chamber , -1 , 3 ); // 2007.09.17
							//
							*Next_Chamber = Chamber; // 2016.12.13
							//
							if      ( Side2 == 2 ) return 141;
							else if ( Side2 == 0 ) return 116;
							else if ( Side2 == 1 ) return 118;

	//						if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2007.11.15 // 2008.06.10
							if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.11.15 // 2008.06.10
								return -1022;
							}
							//
//						}

						//========================================================================================================================
						//========================================================================================================================
						// 2004.03.24
						//========================================================================================================================
						//========================================================================================================================
								//
								outnospace = 0; // 2013.03.22
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2013.03.09
									//
									if ( swbm == 0 ) {
										for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//											if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2009.08.21
											if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
												//
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
													//
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
														//
														if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
															if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
														}
														//
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return 116;
														//
														if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) {
															if ( outnospace == 0 ) outnospace = 1; // 2013.03.22
														}
														//
													}
													else {
														if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) outnospace = 2; // 2013.03.22
													}
												}
												else {
													if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) outnospace = 2; // 2013.03.22
												}
											}
										}
									}
									else {
										//
										i = swbm; // 2014.10.07
										//
//										if ( _SCH_MODULE_GET_USE_ENABLE( swbm , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( swbm ) == TMATM ) ) { // 2014.10.07
										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2014.10.07
//											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( swbm ) || ( _SCH_MODULE_Get_BM_FULL_MODE( swbm ) == BUFFER_TM_STATION ) ) { // 2014.10.07
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2014.10.07
//												if ( _SCH_MACRO_CHECK_PROCESSING_INFO( swbm ) <= 0 ) { // 2014.10.07
												if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) { // 2014.10.07
//													if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( swbm , &j , &k , 0 ) > 0 ) return 116; // 2014.10.07
													if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return 116; // 2014.10.07
													//
													if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) {
														if ( outnospace == 0 ) outnospace = 1; // 2013.03.22
													}
													//
												}
												else {
													if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) outnospace = 2; // 2013.03.22
												}
											}
											else {
												if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) outnospace = 2; // 2013.03.22
											}
											//
										}
										//
									}
								}
								//================================================================================================================
								// 2006.07.10
								//================================================================================================================
								else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.07.10 // 2013.03.09
									if ( swbm == 0 ) {
										for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//											if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2009.08.21
											if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
														//
														if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
															if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
														}
														//
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( i , &j , &k , 0 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) > 0 ) return 116;
														//
													}
												}
											}
										}
									}
									else {
										if ( _SCH_MODULE_GET_USE_ENABLE( swbm , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( swbm ) == TMATM ) ) {
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( swbm ) || ( _SCH_MODULE_Get_BM_FULL_MODE( swbm ) == BUFFER_TM_STATION ) ) {
												if ( _SCH_MACRO_CHECK_PROCESSING_INFO( swbm ) <= 0 ) {
													if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( swbm , &j , &k , 0 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) > 0 ) return 116;
												}
											}
										}
									}

								}
								//================================================================================================================
//								} // 2004.11.29
//							} // 2004.11.29
						//========================================================================================================================
						if ( TMATM == 0 ) { // 2013.03.22
							if ( outnospace == 1 ) {
								if ( Scheduler_Deadlock_Rounding_Possible( TMATM , Chamber ) ) {
									return 119;
								}
							}
							//========================================================================================================================
							// 2013.10.25
							if ( Count == 1 ) {
								if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
									//
									j = Scheduler_Deadlock_Rounding_Possible2_for_Pick_PM( TMATM , Side , Pointer , Chamber , pmslot , _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION ) );
									//
									if ( j == 0 ) return 2021;
									//
								}
								else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
									//
									j = Scheduler_Deadlock_Rounding_Possible2_for_Pick_PM( TMATM , Side , Pointer , Chamber , pmslot , _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION ) );
									//
									if ( j == 0 ) return 2022;
									//
								}
							}
							//============================================================================================
							return -116;
							//============================================================================================
						}
						//
					}
					else {
						return 120;
					}
				}
				else {
					return 121;
				}
			}
			else { // 2003.03.31
				return -117; // 2003.03.31
			} // 2003.03.31
		}
//		if ( z2 == -1 ) {
//			return 16;
//		}
//		else {
			if ( z2 < TMATM ) {
				//============================================
				z = 0;
				if ( TMATM > 0 ) {
					if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM-1 ) < 1 ) {
						if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
							z = 0;
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								z2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &z2 ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( z2 > 0 ) {
									//
//									if ( _SCH_PRM_GET_MODULE_GROUP( z2 ) == ( TMATM - 1 ) ) { // 2014.09.24
									//
									if ( ( _SCH_PRM_GET_MODULE_GROUP( z2 ) == ( TMATM - 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( z2 ) >= 0 ) ){ // 2014.09.24
										//
										if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , z2 ) ) { // 2007.03.20
											z++;
//											if ( ( _SCH_MODULE_Get_PM_WAFER( z2 , 0 ) <= 0 ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( z2 ) <= 0 ) ) { // 2014.01.10
											if ( ( SCH_PM_IS_ABSENT( z2 , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( z2 ) <= 0 ) ) { // 2014.01.10
												for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
													ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 , i );
													//-------------------------------------------------------------------------------------
													SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
													//-------------------------------------------------------------------------------------
													if ( ChkTrg2 > 0 ) {
														//
//														if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) == ( TMATM - 1 ) ) { // 2014.09.24
														//
														if ( ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) == ( TMATM - 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) >= 0 ) ) { // 2014.09.24
															//
															if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM - 1 , z2 , ChkTrg2 , -1 ) ) z = 0;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( z == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) { // 2006.05.25
//						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) { // 2006.05.25 // 2009.08.21
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
							//
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
								if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
							}
							//
							if (
//									( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) || // 2006.05.25
								( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) || // 2006.05.25
								( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) // 2002.10.02
								) {
								if ( FMMode && ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) && ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM != 0 ) ) ) && ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) ) { // 2004.12.28 // 2006.12.21
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) { // 2004.12.28
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2004.12.28
											return 17; // 2004.12.28
										} // 2004.12.28
									} // 2004.12.28
								} // 2004.12.28
								else { // 2004.12.28
									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_SWITCH_MODE ) { // 2013.03.19
										//
										j = 0;
										//
										if ( Count == 1 ) { // 2013.03.18
											if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
												if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) {
													j = 1;
												}
											}
										}
										//
										if ( j == 0 ) {
											//
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
													return 17;
												}
											}
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
											return 17;
										}
									}
								} // 2004.12.28
							}
						}
					}
				}
			}
			else if ( z2 > TMATM ) {
				//============================================
				z = 0;
				if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM+1 ) < 1 ) {
					if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
						z = 0;
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							z2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
							//-------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &z2 ); // 2007.10.29
							//-------------------------------------------------------------------------------------
							if ( z2 > 0 ) {
								//
//								if ( _SCH_PRM_GET_MODULE_GROUP( z2 ) == ( TMATM + 1 ) ) { // 2014.09.24
								//
								if ( ( _SCH_PRM_GET_MODULE_GROUP( z2 ) == ( TMATM + 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( z2 ) >= 0 ) ) { // 2014.09.24
									//
									if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , z2 ) ) { // 2007.03.20
										z++;
//										if ( ( _SCH_MODULE_Get_PM_WAFER( z2 , 0 ) <= 0 ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( z2 ) <= 0 ) ) { // 2014.01.10
										if ( ( SCH_PM_IS_ABSENT( z2 , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) ) ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( z2 ) <= 0 ) ) { // 2014.01.10
											for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
												ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) + 1 , i );
												//-------------------------------------------------------------------------------------
												SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg2 ); // 2007.10.29
												//-------------------------------------------------------------------------------------
												if ( ChkTrg2 > 0 ) {
													//
//													if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) == TMATM + 1 ) { // 2014.09.24
													//
													if ( ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) == ( TMATM + 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg2 ) >= 0 ) ) { // 2014.09.24
														//
														if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM + 1 , z2 , ChkTrg2 , -1 ) ) z = 0;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( z == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) { // 2006.05.25 // 2009.08.21
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_00_NOTUSE ) ) { // 2009.08.21
							if (
								( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) ||
								( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) // 2002.10.02
								){
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) return 18;
							}
						}
					}
				}
			}
//		}
	}
	else { // 2010.08.05
		if ( Count == 1 ) { // 2010.08.05
			if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
				//
				Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
				//
				i = 0;
				j = 0;
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , k );
						if      ( ChkTrg2 == -Chamber ) {
							i = 1;
						}
						else {
							if ( ChkTrg2 > 0 ) {
								j = 1;
								break;
							}
						}
					}
				}
				//
				if ( ( i == 1 ) && ( j == 0 ) ) return 2013;
				//
				i = Scheduler_Deadlock_Rounding_Possible2_for_Pick_PM( TMATM , Side , Pointer , Chamber , pmslot , Side2 , Pointer2 ); // 2013.10.24
				//
				if ( i == 0 ) return 2015; // 2013.10.24
				//
				return - ( i + 10000 ); // 2013.10.24
				//
			}
			else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
				Side2    = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
				Pointer2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
				//
				i = 0;
				j = 0;
				if ( _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) <= _SCH_CLUSTER_Get_PathRange( Side2 , Pointer2 ) ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg2 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side2 , Pointer2 , _SCH_CLUSTER_Get_PathDo( Side2 , Pointer2 ) - 1 , k );
						if      ( ChkTrg2 == -Chamber ) {
							i = 1;
						}
						else {
							if ( ChkTrg2 > 0 ) {
								j = 1;
								break;
							}
						}
					}
				}
				//
				if ( ( i == 1 ) && ( j == 0 ) ) return 2014;
				//
				i = Scheduler_Deadlock_Rounding_Possible2_for_Pick_PM( TMATM , Side , Pointer , Chamber , pmslot , Side2 , Pointer2 ); // 2013.10.24
				//
				if ( i == 0 ) return 2016; // 2013.10.24
				//
				return - ( i + 10000 ); // 2013.10.24
				//
			}
		}
		//============================================================================================
	}
//	if ( ( Count == 2 ) && ( TMATM == 0 ) ) return 14; // 2002.04.16
	return -1;
}
//=======================================================================================
int SCHEDULING_CHECK_Enable_PICK_from_PM_for_GETPR( int TMATM , BOOL fmmode , BOOL *Running , BOOL *bmpickx , int *bmoutf ) { // 2002.05.31
	int i , j , m , order = 99999999 , pmc = -1 , Side , Pointer , minorder , bmo , dt , plc;
	int chkpm[MAX_PM_CHAMBER_DEPTH]; // 2003.01.09
	int pmslot , pmcs;

	*Running = FALSE;
	*bmpickx = FALSE;
	*bmoutf = 0;
	bmo = FALSE;
	plc = FALSE;
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return -2;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return -2;

	minorder = -1;

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.01.09
		chkpm[i] = FALSE;
	}
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
		Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,TA_STATION );
		Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,TA_STATION );
		if ( Pointer >= 100 ) {
			return -2;
//			minorder = SCHEDULER_CONTROL_Get_SDM_PathDo( Pointer % 100 ); // 2002.07.12
		}
		else {
			minorder = _SCH_CLUSTER_Get_PathDo( Side , Pointer );
		}
		if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) > ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) { // 2003.01.09
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				dt = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , i );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &dt ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( dt > 0 ) {
//					if ( _SCH_PRM_GET_MODULE_xGROUP( dt ) == TMATM ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , dt , G_PLACE ) ) {
						plc = TRUE;
						chkpm[dt-PM1] = TRUE;
					}
				}
			}
		}
	}
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
		Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,TB_STATION );
		Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,TB_STATION );
		if ( Pointer >= 100 ) {
			return -2;
//			minorder = SCHEDULER_CONTROL_Get_SDM_PathDo( Pointer % 100 ); // 2002.07.12
		}
		else {
			minorder = _SCH_CLUSTER_Get_PathDo( Side , Pointer );
		}
		if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) > ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) { // 2003.01.09
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				dt = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , i );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &dt ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( dt > 0 ) {
//					if ( _SCH_PRM_GET_MODULE_xGROUP( dt ) == TMATM ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , dt , G_PLACE ) ) {
						plc = TRUE;
						chkpm[dt-PM1] = TRUE;
					}
				}
			}
		}
	}
	if ( minorder == -1 ) {
		if ( fmmode ) { // 2004.11.23
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
				if ( _SCH_PRM_GET_MODULE_GROUP( i ) != ( TMATM+1 ) ) continue;
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_IN_MODE ) continue;
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) continue;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
				bmo = TRUE;
				break;
			}
		}
		else { // 2004.11.23
			if ( !SCHEDULING_CHECK_TM_Another_No_More_Supply( -1 ) ) { // 2004.11.23
				bmo = TRUE;
			}
		}
		minorder = SCHEDULER_Get_PM_MidCount( TMATM ) + 1; // 2002.11.14
	}
	for ( m = 0 ; m < 2 ; m++ ) {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			//
//			if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) continue; // 2014.01.10
			//
//			if ( _SCH_PRM_GET_MODULE_xGROUP( i ) != TMATM ) continue;
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
			//
//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 )	continue; // 2014.01.10
			//
			if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue; // 2014.01.10
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) continue; // 2014.01.10
			//
//			if ( !chkpm[ i - PM1 ] ) continue; // 2003.01.09 // 2003.08.19
			if ( plc && !chkpm[ i - PM1 ] ) continue; // 2003.08.19
			//
//			Side    = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//			Pointer = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
			//
			Side    = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2014.01.10
			Pointer = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2014.01.10
			//
			j = _SCH_CLUSTER_Get_PathDo( Side , Pointer );
			//
			if ( j >= minorder ) {
				if ( j < order ) {
					pmc = i;
					pmcs = pmslot; // 2016.06.02
					order = j;
				}
				else if ( j == order ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) > 0 ) {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
							pmc = i;
							pmcs = pmslot; // 2014.01.10
						}
						else {
							if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , pmc ) > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) ) {
								pmc = i;
								pmcs = pmslot; // 2014.01.10
							}
						}
					}
					else {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
							/*
							// 2014.01.10
							if ( ( _SCH_MODULE_Get_PM_POINTER( pmc , 0 ) >= 100 ) || ( _SCH_MODULE_Get_PM_POINTER( i , 0 ) >= 100 ) ) {
								if ( _SCH_PRM_GET_Getting_Priority( pmc ) > _SCH_PRM_GET_Getting_Priority( i ) ) {
									pmc = i;
								}
							}
							else {
								if ( _SCH_MODULE_Get_PM_SIDE( pmc , 0 ) == _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) {
									if ( _SCH_MODULE_Get_PM_POINTER( pmc , 0 ) > _SCH_MODULE_Get_PM_POINTER( i , 0 ) ) {
										pmc = i;
									}
								}
								else {
									if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , pmc ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) {
										pmc = i;
									}
								}
							}
							*/
							//
							// 2014.01.10
							if ( ( _SCH_MODULE_Get_PM_POINTER( pmc , pmcs ) >= 100 ) || ( _SCH_MODULE_Get_PM_POINTER( i , pmslot ) >= 100 ) ) {
								if ( _SCH_PRM_GET_Getting_Priority( pmc ) > _SCH_PRM_GET_Getting_Priority( i ) ) {
									pmc = i;
									pmcs = pmslot; // 2014.01.10
								}
							}
							else {
								if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_PM_SIDE( pmc , pmcs ) , _SCH_MODULE_Get_PM_POINTER( pmc , pmcs ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_PM_SIDE( i , pmslot ) , _SCH_MODULE_Get_PM_POINTER( i , pmslot ) ) ) {
									pmc = i;
									pmcs = pmslot; // 2014.01.10
								}
								else {
									if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , pmc ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) {
										pmc = i;
										pmcs = pmslot; // 2014.01.10
									}
								}
							}
							//
						}
					}
				}
			}
		}
		if ( ( minorder < 0 ) || ( pmc >= 0 ) ) break;
		minorder = -1;
	}
	if ( bmo ) {
		*Running = FALSE;
		*bmpickx = TRUE;
		*bmoutf = pmc;
		return -3;
	}
	else {
		*bmoutf = 0;
		if ( pmc >= 0 ) {
			*Running = ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmc ) > 0 );
		}
		if ( minorder < 0 ) {
			if ( plc ) {
				*bmpickx = TRUE;
			}
			else {
				*bmpickx = FALSE;
			}
		}
		else {
			*bmpickx = TRUE;
		}
	}
	return pmc;
}
//=======================================================================================
int SCHEDULING_CHECK_Disable_PICK_from_CMBM_OUT_for_GETPR( int TMATM , int side , int pointer , int rbcount ) { // 2002.11.04
//	int i , j , m , ts , tp , maxpr = -1 , dt; // 2007.09.17
	int i , m , ts , tp , maxpr = -1;
	int pmslot;
	//
	if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) return FALSE;
	if ( pointer >= 100 ) return FALSE;
	m = FALSE;
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
//		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) continue; // 2014.01.10
		//
//		if ( _SCH_PRM_GET_MODULE_xGROUP( i ) != TMATM ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 )	continue; // 2014.01.10
		//
		if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue; // 2014.01.10
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) continue; // 2014.01.10
		//
//		ts = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//		tp = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
		//
		ts = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2014.01.10
		tp = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2014.01.10
		//
		if ( _SCH_CLUSTER_Get_PathDo( ts , tp ) < _SCH_CLUSTER_Get_PathDo( side , pointer ) ) {
			m = TRUE;
			break;
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( ts , tp ) >= maxpr ) {
				maxpr = _SCH_CLUSTER_Get_PathDo( ts , tp );
			}
		}
	}
	if ( m ) {
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= SCHEDULER_Get_PM_MidCount( TMATM ) ) return TRUE;
	}
	else {
		if ( maxpr == -1 ) {
			if ( rbcount == 1 ) {
				switch( SCHEDULING_CHECK_for_TM_ARM_Has_Switch_Pick_and_Place( TMATM , -1 , -1 , -1 , -1 ) ) { // 2007.09.17
				case 0 :
				case 1 :
					return TRUE;
					break;
				default :
					return FALSE;
					break;
				}
				/*
				// 2007.09.17
				//================================================================================================
				// 2005.12.15
				//================================================================================================
				m = FALSE;
				for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
					if ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) <= 0 )	continue;
					ts = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
					tp = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
					//
					if ( tp < 100 ) {
						if ( _SCH_CLUSTER_Get_PathRange( ts , tp ) > ( _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 ) ) {
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
								dt = SCHEDULER_CONTROL_Get_PathProcessChamber( ts , tp , _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 , j );
								if ( dt > 0 ) {
//									if ( _SCH_PRM_GET_MODULE_xGROUP( dt ) == TMATM ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , dt , G_PLACE ) ) {
										m = TRUE;
										break;
									}
								}
							}
						}
					}
					else {
						m = TRUE;
						break;
					}
				}
				//================================================================================================
				if ( !m ) return FALSE; // 2005.12.15
				//================================================================================================
				return TRUE;
				*/
			}
			else if ( rbcount == 2 ) {
				return FALSE;
			}
		}
		else {
			if ( maxpr > SCHEDULER_Get_PM_MidCount( TMATM ) ) return TRUE;
		}
	}
	return FALSE;
}
//=======================================================================================
int SCHEDULING_CHECK_Enable_MOVE_BM_for_GETPR( int TMATM ) { // 2002.06.21
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return FALSE;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return FALSE;
	return TRUE;
}


//int SCHEDULING_CHECK_Enable_Place_To_BM( int TMATM , int Side , int Pointer , int Finger , int *RChamber , int *RSlot , int plcgroup , int bmmode , BOOL MoveOnly , int swbm , int putprfirstch , int retmode ) { // 2014.04.07
int SCHEDULING_CHECK_Enable_Place_To_BM( int TMATM , int Side , int Pointer , int Finger , int *RChamber , int *RSlot , int plcgroup , int bmmode , BOOL MoveOnly , int swbm , int putprfirstch , int retmode , BOOL dtmothersupply , int *incnt ) { // 2014.04.07
	int i , j , k , ChkSrc , ChkSlot = 1 , priority , chk98tag;
	BOOL chk99tag; // 2004.03.24
	int inpointer , insider , inpttag , inptside; // 2007.01.12
	int nx;

	if ( incnt != NULL ) *incnt = 0;

	priority = 9999999;

	ChkSrc  = -1;
	ChkSlot = 1;

	//
	inpttag = -1; // 2007.01.12
	//
	if ( swbm == 0 ) { // 2002.10.29
		//
		chk99tag = FALSE; // 2004.03.24
		chk98tag = -1; // 2005.01.19
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-171 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) continue; // 2008.05.26
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
			//
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
				if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
			}
			//
			if ( SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( i , FALSE ) < 0 ) continue; // 2008.08.27
			//
			if ( Finger >= 0 ) { // 2013.11.13
				if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 0 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != ( Finger + 1 ) ) ) continue; // 2008.05.14
			}
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == plcgroup ) ) ) { // 2006.12.21
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == bmmode ) ) { // 2006.05.26
						if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
							if ( Scheduler_Switch_Side_Check_for_TM_Place( Side , i ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) ) { // 2003.11.07 // 2008.07.01
								if ( ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) == Side ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) ) { // 2003.11.07
									nx = i;
//									if ( retmode || SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , Side , Pointer , -1 , 1 , swbm , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2007.10.16 // 2013.11.13
									if ( retmode || ( Finger < 0 ) || SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , Side , Pointer , -1 , 1 , swbm , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2007.10.16 // 2013.11.13
										if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( plcgroup ) == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.07 // 2013.03.09
											k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_SINGLESWAP( i , Side , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ); // 2003.11.07
											if ( k > 0 ) { // 2003.11.07
												if ( putprfirstch == i ) { // 2004.03.24
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
													inpttag = inpointer;
													inptside = insider;
													break;
												}
												else {
													if ( k >= 99 ) {
														if ( ChkSrc != -1 ) { // 2009.11.09
															if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) && SCHEDULER_CONTROL_Chk_BM_FREE_ALL( ChkSrc ) ) {
																if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) { // 2009.11.09
																	ChkSrc  = i;
																	ChkSlot = j;
																	priority = _SCH_PRM_GET_Putting_Priority( i );
																	chk99tag = TRUE; // 2004.03.24
																	inpttag = inpointer;
																	inptside = insider;
																}
																else {
																	if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) != ChkSrc ) { // 2009.11.09
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																		chk99tag = TRUE; // 2004.03.24
																		inpttag = inpointer;
																		inptside = insider;
																	}
																}
															}
															else { // 2009.11.09
																ChkSrc  = i;
																ChkSlot = j;
																priority = _SCH_PRM_GET_Putting_Priority( i );
																chk99tag = TRUE; // 2004.03.24
																inpttag = inpointer;
																inptside = insider;
															}
														}
														else {
															ChkSrc  = i;
															ChkSlot = j;
															priority = _SCH_PRM_GET_Putting_Priority( i );
															chk99tag = TRUE; // 2004.03.24
															inpttag = inpointer;
															inptside = insider;
														}
														//break; // 2004.03.24
													}
													else {
														if ( !chk99tag ) { // 2004.03.24
															if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) { // 2007.01.12
																//=========================================================================
																// 2007.01.12
																//=========================================================================
																if ( inpttag == -1 ) {
																	if ( inpointer == -1 ) {
																		if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			inpttag = inpointer;
																			inptside = insider;
																		}
																		else {
																			if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																				ChkSrc  = i;
																				ChkSlot = j;
																				priority = _SCH_PRM_GET_Putting_Priority( i );
																				inpttag = inpointer;
																				inptside = insider;
																			}
																		}
																	}
																	else {
																		if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			inpttag = inpointer;
																			inptside = insider;
																		}
																	}
																}
																else {
																	if ( inpointer == -1 ) {
																		if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			inpttag = inpointer;
																			inptside = insider;
																		}
																	}
																	else {
																		if ( inptside != insider ) {
																			if ( _SCH_CLUSTER_Get_SupplyID( insider , inpointer ) < _SCH_CLUSTER_Get_SupplyID( inptside , inpttag ) ) { // 2007.10.02
																				// 2008.04.18
																				ChkSrc  = i;
																				ChkSlot = j;
																				priority = _SCH_PRM_GET_Putting_Priority( i );
																				inpttag = inpointer;
																				inptside = insider;
																				/*
																				// 2009.01.05
																				if ( CHECKING_SIDE == insider ) { // 2007.02.22
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																				else if ( CHECKING_SIDE != inptside ) { // 2007.02.22
																					if ( _SCH_CLUSTER_Get_Buffer( CHECKING_SIDE , Pointer ) == i ) {
																						ChkSrc  = i;
																						ChkSlot = j;
																						priority = _SCH_PRM_GET_Putting_Priority( i );
																						inpttag = inpointer;
																						inptside = insider;
																					}
																					else {
																						if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																							ChkSrc  = i;
																							ChkSlot = j;
																							priority = _SCH_PRM_GET_Putting_Priority( i );
																							inpttag = inpointer;
																							inptside = insider;
																						}
																					}
																				}
																				*/
																			}
																		}
																		else {
																			if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																				ChkSrc  = i;
																				ChkSlot = j;
																				priority = _SCH_PRM_GET_Putting_Priority( i );
																				inpttag = inpointer;
																				inptside = insider;
																			}
																			else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) {
																				if ( inpttag > inpointer ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																			}
																		}
																	}
																}
																//=========================================================================
															}
															else {
																// 2009.01.05
																if ( ( inpttag != -1 ) && ( inptside != -1 ) ) {
																	if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																		ChkSrc  = i;
																		ChkSlot = j;
																		chk98tag = k;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																		inpttag = inpointer;
																		inptside = insider;
																	}
																	else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) {
																		if ( _SCH_CLUSTER_Get_SupplyID( insider , inpointer ) < _SCH_CLUSTER_Get_SupplyID( inptside , inpttag ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			chk98tag = k;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			inpttag = inpointer;
																			inptside = insider;
																		}
																	}
																}
																else {
																	if ( chk98tag == -1 ) {
																		if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			chk98tag = k;
																			inpttag = inpointer;
																			inptside = insider;
																		}
																		else {
																			if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																				ChkSrc  = i;
																				ChkSlot = j;
																				chk98tag = k;
																				priority = _SCH_PRM_GET_Putting_Priority( i );
																				inpttag = inpointer;
																				inptside = insider;
																			}
																		}
																	}
																	else {
																		if ( chk98tag > k ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			chk98tag = k;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																			inpttag = inpointer;
																			inptside = insider;
																		}
																	}
																}
																/* 2009.01.05
																if ( _SCH_CLUSTER_Get_Buffer( CHECKING_SIDE , Pointer ) == i ) {
																	ChkSrc  = i;
																	ChkSlot = j;
																	priority = _SCH_PRM_GET_Putting_Priority( i );
																}
																else {
																	if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																	}
																}
																*/
															}
														}
													}
												}
											}
										}
										else {
											if ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( plcgroup ) >= BUFFER_SWITCH_FULLSWAP ) || ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( Side ) % 2 ) == 0 ) ) { // 2013.03.09
//												k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( plcgroup ) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2003.11.07 // 2013.03.09 // 2013.11.13
												k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( plcgroup ) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , ( Finger < 0 ) ? 0 : _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2003.11.07 // 2013.03.09 // 2013.11.13
												if ( k > 0 ) { // 2003.11.07
													if ( putprfirstch == i ) { // 2004.03.24
														ChkSrc  = i;
														ChkSlot = j;
														priority = _SCH_PRM_GET_Putting_Priority( i );
														inpttag = inpointer;
														inptside = insider;
														break;
													}
													else {
														if ( k >= 99 ) {
															ChkSrc  = i;
															ChkSlot = j;
															priority = _SCH_PRM_GET_Putting_Priority( i );
															chk99tag = TRUE; // 2004.03.24
															inpttag = inpointer;
															inptside = insider;
															//break; // 2004.03.24
														}
														else {
															if ( !chk99tag ) { // 2004.03.24
																//
																if ( SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( dtmothersupply , i , ChkSrc ) ) { // 2014.04.07
																	ChkSrc  = i;
																	ChkSlot = j;
																	priority = _SCH_PRM_GET_Putting_Priority( i );
																	inpttag = inpointer;
																	inptside = insider;
																}
																else {
																	//=========================================================================
																	// 2007.01.26
																	//=========================================================================
																	if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) {
																		if ( inpttag == -1 ) {
																			if ( inpointer == -1 ) {
																				if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																				else {
																					if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																						ChkSrc  = i;
																						ChkSlot = j;
																						priority = _SCH_PRM_GET_Putting_Priority( i );
																						inpttag = inpointer;
																						inptside = insider;
																					}
																				}
																			}
																			else {
																				if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																			}
																		}
																		else {
																			if ( inpointer == -1 ) {
																				if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																			}
																			else {
																				if ( inptside != insider ) {
																					if ( _SCH_CLUSTER_Get_SupplyID( insider , inpointer ) < _SCH_CLUSTER_Get_SupplyID( inptside , inpttag ) ) { // 2007.10.02
																						// 2008.04.18
																						ChkSrc  = i;
																						ChkSlot = j;
																						priority = _SCH_PRM_GET_Putting_Priority( i );
																						inpttag = inpointer;
																						inptside = insider;
																						/*
																						// 2008.04.18
																						if ( CHECKING_SIDE == insider ) { // 2007.02.22
																							ChkSrc  = i;
																							ChkSlot = j;
																							priority = _SCH_PRM_GET_Putting_Priority( i );
																							inpttag = inpointer;
																							inptside = insider;
																						}
																						else if ( CHECKING_SIDE != inptside ) { // 2007.02.22
																							if ( _SCH_CLUSTER_Get_Buffer( CHECKING_SIDE , Pointer ) == i ) {
																								ChkSrc  = i;
																								ChkSlot = j;
																								priority = _SCH_PRM_GET_Putting_Priority( i );
																								inpttag = inpointer;
																								inptside = insider;
																							}
																							else {
																								if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																									ChkSrc  = i;
																									ChkSlot = j;
																									priority = _SCH_PRM_GET_Putting_Priority( i );
																									inpttag = inpointer;
																									inptside = insider;
																								}
																							}
																						}
																						*/
																					}
																				}
																				else {
																					if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																						ChkSrc  = i;
																						ChkSlot = j;
																						priority = _SCH_PRM_GET_Putting_Priority( i );
																						inpttag = inpointer;
																						inptside = insider;
																					}
																					else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) {
																						if ( inpttag > inpointer ) {
																							ChkSrc  = i;
																							ChkSlot = j;
																							priority = _SCH_PRM_GET_Putting_Priority( i );
																							inpttag = inpointer;
																							inptside = insider;
																						}
																					}
																				}
																			}
																		}
																		//=========================================================================
																	}
																	else {
																		if ( ( inpttag != -1 ) && ( inptside != -1 ) ) { // 2007.10.02
																			if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																				ChkSrc  = i;
																				ChkSlot = j;
																				if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) >= BUFFER_SWITCH_FULLSWAP ) chk98tag = k; // 2005.01.19 // 2013.03.09
																				priority = _SCH_PRM_GET_Putting_Priority( i );
																				inpttag = inpointer;
																				inptside = insider;
																			}
																			else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) {
																				if ( _SCH_CLUSTER_Get_SupplyID( insider , inpointer ) < _SCH_CLUSTER_Get_SupplyID( inptside , inpttag ) ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) >= BUFFER_SWITCH_FULLSWAP ) chk98tag = k; // 2005.01.19 // 2013.03.09
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					inpttag = inpointer;
																					inptside = insider;
																				}
																			}
																		}
																		else {
																			if ( chk98tag == -1 ) { // 2005.01.19
																				if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
																					ChkSrc  = i;
																					ChkSlot = j;
																					priority = _SCH_PRM_GET_Putting_Priority( i );
																					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) < BUFFER_SWITCH_FULLSWAP ) break; // 2003.12.04 // 2013.03.09
																					else chk98tag = k; // 2005.01.19
																					inpttag = inpointer; // 2007.10.02
																					inptside = insider; // 2007.10.02
																					//break; // 2003.12.04
																				}
																				else {
																					if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																						ChkSrc  = i;
																						ChkSlot = j;
																						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) >= BUFFER_SWITCH_FULLSWAP ) chk98tag = k; // 2005.01.19 // 2013.03.09
																						priority = _SCH_PRM_GET_Putting_Priority( i );
																						inpttag = inpointer; // 2007.10.02
																						inptside = insider; // 2007.10.02
																					}
																				}
																			}
																			else { // 2005.01.19
																				if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) >= BUFFER_SWITCH_FULLSWAP ) { // 2005.01.19 // 2013.03.09
																					if ( chk98tag > k ) { // 2005.01.19
																						ChkSrc  = i; // 2005.01.19
																						ChkSlot = j; // 2005.01.19
																						chk98tag = k; // 2005.01.19
																						priority = _SCH_PRM_GET_Putting_Priority( i ); // 2005.01.19
																						inpttag = inpointer; // 2007.10.02
																						inptside = insider; // 2007.10.02
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
											else {
												j = _SCH_CLUSTER_Get_SlotIn( Side , Pointer );
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK( i , j , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ) > 0 ) {
													if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
														ChkSrc  = i;
														ChkSlot = j;
														priority = _SCH_PRM_GET_Putting_Priority( i );
														break;
													}
													else {
														if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
															ChkSrc  = i;
															ChkSlot = j;
															priority = _SCH_PRM_GET_Putting_Priority( i );
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		if ( ( TMATM != 0 ) || ( TMATM != plcgroup ) ) { // 2013.05.03
			i = swbm;
		}
		else {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) == 0 ) { // 2006.03.20
				i = swbm; // 2006.03.20
			} // 2006.03.20
			else { // 2006.03.20
				i = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ); // 2006.03.20
			} // 2006.03.20
		}
		//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-171 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
//		if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == 0 ) || ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == ( Finger + 1 ) ) ) { // 2008.05.14 // 2013.11.13
		if ( ( Finger < 0 ) || ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == 0 ) || ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == ( Finger + 1 ) ) ) { // 2008.05.14 // 2013.11.13
			//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-171 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
			if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , ( TMATM == plcgroup ) ? G_MCHECK_NO : G_MCHECK_ALL ) ) { // 2008.05.26 // 2013.03.18
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-172 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == plcgroup ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-173 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == bmmode ) ) { // 2006.05.26
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-174 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
						if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-175 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------------------
							// 2004.03.10
							//-----------------------------------------------------------------------------------
							nx = i;
//							if ( retmode || SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , Side , Pointer , -1 , 1 , swbm , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2007.10.16 // 2013.11.13
							if ( retmode || ( Finger < 0 ) || SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , Side , Pointer , -1 , 1 , swbm , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2007.10.16 // 2013.11.13
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-176 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
								if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) == BUFFER_SWITCH_SINGLESWAP ) { // 2013.03.09
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-177 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , i );
//------------------------------------------------------------------------------------------------------------------
									k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_SINGLESWAP( i , Side , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ); // 2003.11.07
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-178 \t[retmode=%d][Finger=%d][TMATM=%d][Side=%d][Pointer=%d][%d]\n" , retmode , Finger , TMATM , Side , Pointer , k );
//------------------------------------------------------------------------------------------------------------------
									if ( k > 0 ) {
										if ( k >= 99 ) {
											ChkSrc  = i;
											ChkSlot = j;
											priority = _SCH_PRM_GET_Putting_Priority( i );
										}
										else {
											if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == i ) {
												ChkSrc  = i;
												ChkSlot = j;
												priority = _SCH_PRM_GET_Putting_Priority( i );
											}
											else {
												if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
												}
											}
										}
									}
								}
								else {
									if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) == BUFFER_SWITCH_BATCH_ALL ) { // 2006.03.21 // 2013.03.09
										if ( i != swbm ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) {
												if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != Side ) i = -1;
											}
										}
									}
									if ( i >= 0 ) {
										if ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() > _SCH_PRM_GET_MODULE_SIZE( i ) ) { // 2004.03.15
//											k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2013.03.09 // 2013.11.13
											k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , ( Finger < 0 ) ? 0 : _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2013.03.09 // 2013.11.13
											if ( k > 0 ) {
												if ( k >= 99 ) {
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
												}
												else {
													if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
														ChkSrc  = i;
														ChkSlot = j;
														priority = _SCH_PRM_GET_Putting_Priority( i );
													}
												}
											}
										}
										else {
//													if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) || ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) % 2 ) == 0 ) ) { // 2004.03.15
											if ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( Side ) % 2 ) == 0 ) { // 2004.03.15
//												k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2013.03.09 // 2013.11.13
												k = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( i , Side , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(plcgroup) , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) , &insider , &inpointer , ( Finger < 0 ) ? 0 : _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) ); // 2013.03.09 // 2013.11.13
												if ( k > 0 ) {
													if ( k >= 99 ) {
														ChkSrc  = i;
														ChkSlot = j;
														priority = _SCH_PRM_GET_Putting_Priority( i );
													}
													else {
														if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
															ChkSrc  = i;
															ChkSlot = j;
															priority = _SCH_PRM_GET_Putting_Priority( i );
														}
													}
												}
											}
											else {
												//
												if ( _SCH_CLUSTER_Get_PathIn( Side , Pointer ) >= BM1 ) { // 2016.08.31
													//
													if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
														//
														for ( k = ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 ) ; k <= _SCH_PRM_GET_MODULE_SIZE( i ) ; k++ ) {
															//
															if ( _SCH_MODULE_Get_BM_WAFER( i , k ) <= 0 ) {
																//
																if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK( i , k , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ) > 0 ) {
																	//
																	ChkSrc  = i;
																	ChkSlot = k;
																	priority = _SCH_PRM_GET_Putting_Priority( i );
																	//
																	break;
																}
																//
															}
															//
														}
														//
													}
													//
												}
												else {
													//
													j = _SCH_CLUSTER_Get_SlotIn( Side , Pointer );
													//
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK( i , j , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ) > 0 ) {
														//
														if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
															ChkSrc  = i;
															ChkSlot = j;
															priority = _SCH_PRM_GET_Putting_Priority( i );
														}
													}
													//
												}
												//
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//
	if ( ChkSrc >= 0 ) {
		*RChamber = ChkSrc;
		*RSlot    = ChkSlot;
		//
		if ( incnt != NULL ) { // 2018.10.20
			*incnt = SCHEDULER_CONTROL_Chk_Get_BM_Count( ChkSrc , BUFFER_INWAIT_STATUS );
		}
		//
		return 1;
	}
	return 0;
}

//=======================================================================================
BOOL SCHEDULER_CONTROL_Check_Other_Arm_BeforeLot_And_Wait_Place_This_PM( int Side , int Pointer , int pmc , int tms , int Finger ) { // 2013.09.03
	int arm , w , s , p , k , PahtDo ;
	//
	if      ( Finger == TA_STATION ) {
		arm = TB_STATION;
	}
	else if ( Finger == TB_STATION ) {
		arm = TA_STATION;
	}
	else {
		return FALSE;
	}
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,arm ) )	return FALSE;
	//
	w = _SCH_MODULE_Get_TM_WAFER( tms,arm );
	if ( w <= 0 )		return FALSE;
	//
	/*
	// 2014.07.17
	//
	if ( ( w % 100 ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( tms,arm );
		p = _SCH_MODULE_Get_TM_POINTER( tms,arm );
	}
	else if ( ( w / 100 ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE2( tms,arm );
		p = _SCH_MODULE_Get_TM_POINTER2( tms,arm );
	}
	else {
		return FALSE;
	}
	*/
	//
	// 2014.07.17
	//
	s = _SCH_MODULE_Get_TM_SIDE( tms,arm );
	p = _SCH_MODULE_Get_TM_POINTER( tms,arm );
	//
	if ( s != Side )									return FALSE;
	//
	if ( _SCH_CLUSTER_Get_SupplyID( Side , Pointer ) < _SCH_CLUSTER_Get_SupplyID( s , p ) ) return FALSE;
	//
	PahtDo = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) != PahtDo ) return FALSE;
	//
	if ( PahtDo > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	if ( PahtDo <= 0 ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , PahtDo - 1 , k ) == pmc ) return TRUE;
		//
	}
	//
	return FALSE;
}
//=======================================================================================

int SCH_MULTI_GET_PM_CLUSTER_INDEX( int pm ) { // 2014.01.10
	int i , s , p;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( pm ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_PM_WAFER( pm,i ) <= 0 ) continue;
		if ( _SCH_MODULE_Get_PM_PICKLOCK( pm,i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_PM_SIDE( pm,i );
		p = _SCH_MODULE_Get_PM_POINTER( pm,i );
		return _SCH_CLUSTER_Get_ClusterIndex( s , p );
	}
	return -9999;
}

int SCH_MULTI_PUT_PRIORITY_ORDERING_CHECK( int ps , int pp , int priority , int setch , int chkch ) { // 2014.01.10
	int si,ci;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( chkch ) <= 1 ) {
		//
		if ( priority > _SCH_PRM_GET_Putting_Priority( chkch ) ) {
			return 1;
		}
		else if ( priority == _SCH_PRM_GET_Putting_Priority( chkch ) ) {
			//
			if ( setch < PM1 ) return 0;
			if ( setch > AL ) return 0;
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( setch ) <= 1 ) return 0;
			//
			si = SCH_MULTI_GET_PM_CLUSTER_INDEX( setch );
			//
			if ( si == -9999 ) return 0;
			//
			if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) == si ) return -1;
			//
			return 1;
		}
	}
	else {
		if ( priority > _SCH_PRM_GET_Putting_Priority( chkch ) ) {
			//
			ci = SCH_MULTI_GET_PM_CLUSTER_INDEX( chkch );
			//
			if ( ci == -9999 ) return 1;
			//
			if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) != ci ) return -1;
			//
			return 1;
		}
		else if ( priority == _SCH_PRM_GET_Putting_Priority( chkch ) ) {
			//
			if ( setch < PM1 ) return 0;
			if ( setch > AL ) return 0;
			//
			si = SCH_MULTI_GET_PM_CLUSTER_INDEX( setch );
			ci = SCH_MULTI_GET_PM_CLUSTER_INDEX( chkch );
			//
			if ( ( si == -9999 ) && ( ci == -9999 ) ) {
				return 0;
			}
			else if ( si == -9999 ) {
				if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) == ci ) return 1;
				return -1;
			}
			else if ( ci == -9999 ) {
				if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) == si ) return -1;
				return 1;
			}
			else {
				if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) == si ) return -1;
				if ( _SCH_CLUSTER_Get_ClusterIndex( ps , pp ) == ci ) return 1;
				return -1;
			}
			//
		}
	}
	return -1;
	//
}










int SCHEDULING_CHECK_for_Enable_PLACE_From_TM( int TMATM , int CHECKING_SIDE , int Finger , int *NextChamber , int *NextSlot , int *MovingType , int *MovingOrder , BOOL FMMode , BOOL MoveOnly , int swbm , int *JumpMode , int putprfirstch , int *retmode , int putprfixch ) {
	// Append MoveOnly // 2002.04.08
	int i , j , k , ChkSrc , ChkSlot = 1 , ChkhasIn , hasin , Side , Pointer , z , zd , jbmchk1 , jbmchk2 , priority , nxch , nextstockonly;
	BOOL okpm , pmcond; // 2002.07.23
	int bmd;
	int nx , xx , pathDo;
	int BeforeRunOrder; // 2008.03.11
	int SameOrderPlaceCount , OtherOrderPlaceCount; // 2008.06.22
	int OtherOrderPlaceGrp , OtherOrderPlaceCountOneGrp; // 2016.12.13
	int chkretmode; // 2013.05.02
	int prchkres;
	BOOL OtherArmHasSupplyWafer; // 2014.04.07
	int sbm , ebm , bma , bmb; // 2016.12.13
	int PlusOneGroupPlaceCount; // 2016.12.13
	BOOL PMSameOK; // 2016.12.13

	BOOL WaitingMultiwafer , BM_Waitiing; // 2018.05.08

	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,Finger ) )				return -2;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) <= 0 )				return -4;
	Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,Finger );
	if ( Side != CHECKING_SIDE )										return -3;

	Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,Finger );
	//
	if ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) ) swbm = 0; // 2013.05.03
	//
	//===============================================================================================================
	// 2006.02.01
	//===============================================================================================================
	if ( SCHEDULING_CHECK_for_PICKPLACE_from_PM_NEXT_FULLY_REMAP( FALSE , TMATM , CHECKING_SIDE , Pointer , -1 ) == 0 ) return -57;
	//===============================================================================================================
	//================================================================================================================
	// 2007.09.05
	//================================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
		switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
		case 6 :
		case 7 :
			if (
				( ( Finger == TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) ||
				( ( Finger == TB_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) )
				) {
				switch ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , -1 , 4 , FALSE ) ) {
				case 1 :
					//============================================================================================
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( ">> PATHDO_WAFER_RETURN B-1 >> CHECKING_SIDE = %d , s=%d,p=%d\n" , CHECKING_SIDE , Side , Pointer );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );

					_SCH_CLUSTER_Set_SupplyID( Side , Pointer , -9999 );
					_SCH_CLUSTER_Set_PathDo( Side , Pointer , 2 );
					_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
					//============================================================================================
					return -58;
					break;
				case 2 :
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( ">> PATHDO_WAFER_RETURN B-2 >> CHECKING_SIDE = %d , s=%d,p=%d\n" , CHECKING_SIDE , Side , Pointer );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
//printf( "========================================================================================\n" );
					return -59;
					break;
				}
			}
			break;
		}
	}
	else {
/*
		//=============================================================================================
		// 2007.09.07
		//=============================================================================================
		if (
			( ( Finger == TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) ||
			( ( Finger == TB_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) )
			) {
			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , 5 ) == 1 ) {
				_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN );
				_SCH_CLUSTER_Set_PathStatus( Side , Pointer , SCHEDULER_RETURN_REQUEST );
				_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
				return -59;
			}
		}
		else {
			if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , 6 ) == 1 ) {
					_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN );
//						_SCH_CLUSTER_Set_PathStatus( Side , Pointer , SCHEDULER_RETURN_REQUEST );
					_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
					return -59;
				}
			}
		}
*/
	}
	//================================================================================================================

//	if ( TMATM == 1 ) { // 2014.01.24
//		Sleep(1); // 2014.01.24
//	} // 2014.01.24

	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( Side , Pointer , 2 ) == 0 ) return -21;

	PMSameOK = FALSE; // 2016.12.13
	//
	chkretmode = FALSE; // 2013.05.02
	*retmode = 0;
	*JumpMode = 0;
	*MovingOrder = _SCH_MODULE_Get_TM_PATHORDER( TMATM , Finger );

	if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) {
		_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
	}
	else if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) > _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
		_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
	}
	else { // 2003.10.21
		if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO2 ) { // 2007.09.29
			if ( _SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( Side ) <= 0 ) {
				if ( _SCH_CASSETTE_LAST_RESULT2_GET( Side,Pointer ) == -2 ) {
					switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
					case 6 :
					case 7 :
						*retmode = 1;
						chkretmode = TRUE; // 2013.05.02
						break;
					default : // 2016.07.22
//						if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) { // 2018.04.10
						if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) { // 2018.04.10
							*retmode = 2;
							chkretmode = TRUE;
						}
						break;
					}
				}
			}
		}
		//=============================================================================================
		// 2007.09.18
		//=============================================================================================
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM , Finger ) == SCHEDULER_MOVING_IN ) {
			if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , -1 , 6 , FALSE ) == 1 ) {
//					_SCH_MODULE_Set_TM_TYPE( TMATM , Finger , SCHEDULER_MOVING_OUT );
					//
					if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 3;	else *retmode = 3;	
					//
					chkretmode = TRUE; // 2013.05.02
				}
			}
			else { // 2017.02.13
				if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 0 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) ) { // 2018.04.10 SMART 강제 return
					//
					if ( SCHEDULING_CHECK_for_TM_ARM_Place_ArmIntlks_Impossible( TMATM , Finger ) ) {
						if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 4;	else *retmode = 4;
						chkretmode = TRUE;
					}
					//
				}
			}
			//
			//
			if ( SCHEDULING_CHECK_for_TM_ARM_Place_MultiGroup_Impossible( TMATM , Finger ) ) { // 2018.11.07
				if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 5;	else *retmode = 5;
				chkretmode = TRUE;
			}
			//
			//
//			if ( !( *retmode ) ) { // 2013.05.02
			if ( *retmode <= 0 ) { // 2019.01.30
				if ( TMATM > 0 ) { // 2013.04.25
					if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , Finger , -1 , 5 , FALSE ) == 1 ) {
//						*retmode = TRUE; // 2013.05.02
						//
						if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 6;	else *retmode = -6;
						//
						chkretmode = TRUE; // 2013.05.02
						//
						if ( MULTI_ALL_PM_FULLSWAP ) { // 2018.10.20
							if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 7;	else *retmode = 7;
						}
						//
					}
				}
				//
				// 2014.01.24
				//
				//-----------------------------------------------------------------------
				switch( _SCH_SYSTEM_MODE_END_GET( Side ) ) {
				case 3 :
				case 4 :
				case 5 :
				case 6 :
				case 7 :
					if ( ( _SCH_MODULE_Get_TM_OUTCH( TMATM , Finger ) >= PM1 ) && ( _SCH_MODULE_Get_TM_OUTCH( TMATM , Finger ) < AL ) ) { 
						if ( ( _SCH_PRM_GET_STOP_PROCESS_SCENARIO( _SCH_MODULE_Get_TM_OUTCH( TMATM , Finger ) ) % 2 ) == 1 ) {
							if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 8;	else *retmode = 8;
							chkretmode = TRUE;
						}
					}
					else {
						if ( *retmode > 0 ) *retmode = ( *retmode * 10 ) + 9;	else *retmode = 9;
						chkretmode = TRUE;
					}
					break;
				}
				//-----------------------------------------------------------------------
				//
			}
		}
		//=============================================================================================
	}

//	if ( *retmode ) { // 2013.05.02
	if ( chkretmode ) { // 2013.05.02
		*MovingType  = SCHEDULER_MOVING_OUT;
	}
	else {
		*MovingType  = _SCH_MODULE_Get_TM_TYPE( TMATM , Finger );
	}
	//
	OtherArmHasSupplyWafer = FALSE; // 2014.04.07
	//
	priority = 9999999;

	if ( FMMode ) {
		ChkSrc  = -1;
		ChkSlot = 1;
		if ( *MovingType == SCHEDULER_MOVING_OUT ) {
			//
			if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , -2 ) ) { // 2007.03.20
				if ( _SCH_PRM_GET_MODULE_GROUP( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) ) == TMATM ) {
					if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) ) < 0 ) ) { // 2007.01.02
						*NextChamber = _SCH_CLUSTER_Get_PathOut( Side , Pointer );
						*NextSlot    = 1;
						*MovingType  = SCHEDULER_MOVING_OUT;
						*JumpMode	 = 0;
						return 11;
					}
				}
			}
			//
			// 2014.04.07
			//
			OtherArmHasSupplyWafer = FALSE;
			//
			if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) != 0 ) {
				if      ( ( Finger == TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
					OtherArmHasSupplyWafer = SCHEDULING_CHECK_for_TM_ARM_Has_Supply_Wafer( TMATM , TB_STATION );
				}
				else if ( ( Finger == TB_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) {
					OtherArmHasSupplyWafer = SCHEDULING_CHECK_for_TM_ARM_Has_Supply_Wafer( TMATM , TA_STATION );
				}
			}
			//
			if ( !_SCH_SUB_Run_Impossible_Condition( Side , Pointer , -2 ) || ( _SCH_PRM_GET_MODULE_GROUP( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) ) < TMATM ) ) { // 2007.03.20
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
					i = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
					if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
							ChkSrc  = i;
							ChkSlot = j;
						}
					}
				}
				else {
					if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) { // 2006.12.21
						//
						if ( SCHEDULING_CHECK_Enable_Place_To_BM( TMATM , Side , Pointer , Finger , &i , &j , TMATM , BUFFER_TM_STATION , MoveOnly , swbm , putprfirstch , chkretmode , OtherArmHasSupplyWafer , NULL ) ) {
							ChkSrc  = i;
							ChkSlot = j;
						}
					}
					if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) { // 2006.12.21
						if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) != GROUP_HAS_SWITCH_NONE ) {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 0 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != ( Finger + 1 ) ) ) continue; // 2008.05.14
								//
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
								//
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
								}
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) { // 2006.05.25
									if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM != 0 ) ) ) { // 2006.12.21
										bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
										if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
											nx = i;
//											if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2008.04.16
												if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_OUTS ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
															if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																priority = _SCH_PRM_GET_Putting_Priority( i );
															}
															else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) { // 2012.09.21
																if ( ChkSrc != -1 ) {
																	if ( SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( OtherArmHasSupplyWafer , i , ChkSrc ) ) { // 2014.04.07
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																	}
																	else {
																		if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 0 , i , ChkSrc ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																		}
																	}
																}
															}
														}
													}
												}
												else if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_INS ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_S_MODE ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
															if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																priority = _SCH_PRM_GET_Putting_Priority( i );
															}
															else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) { // 2012.09.21
																if ( ChkSrc != -1 ) {
																	if ( SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( OtherArmHasSupplyWafer , i , ChkSrc ) ) { // 2014.04.07
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																	}
																	else {
																		if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 0 , i , ChkSrc ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																		}
																	}
																}
															}
														}
													}
												}
	//											else if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_OUT ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) { // 2006.05.25
												else if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_OUT ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) { // 2006.05.25
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
															if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																priority = _SCH_PRM_GET_Putting_Priority( i );
															}
															else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) { // 2012.09.21
																if ( ChkSrc != -1 ) {
																	if ( SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( OtherArmHasSupplyWafer , i , ChkSrc ) ) { // 2014.04.07
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																	}
																	else {
																		if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 0 , i , ChkSrc ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																		}
																	}
																}
															}
														}
													}
												}
//											} // 2008.04.16
										}
									}
								}
							}
						}
						else {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 0 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != ( Finger + 1 ) ) ) continue; // 2008.05.14
								//
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
								//
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
								}
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) { // 2006.05.25
									if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM != 0 ) ) ) { // 2006.12.21
										bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
										if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
											if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) { // 2006.05.25
												nx = i;
//												if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22 // 2008.04.16
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
															if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
																ChkSrc  = i;
																ChkSlot = j;
																priority = _SCH_PRM_GET_Putting_Priority( i );
															}
															else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) { // 2012.09.21
																if ( ChkSrc != -1 ) {
																	if ( SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( OtherArmHasSupplyWafer , i , ChkSrc ) ) { // 2014.04.07
																		ChkSrc  = i;
																		ChkSlot = j;
																		priority = _SCH_PRM_GET_Putting_Priority( i );
																	}
																	else {
																		if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 0 , i , ChkSrc ) ) {
																			ChkSrc  = i;
																			ChkSlot = j;
																			priority = _SCH_PRM_GET_Putting_Priority( i );
																		}
																	}
																}
															}
														}
													}
//												} // 2008.04.16
											}
										}
									}
								}
							}
						}
					}
				//
				}
				//
				if ( ChkSrc >= 0 ) {
					*NextChamber = ChkSrc;
					*NextSlot    = ChkSlot;
					*MovingType  = SCHEDULER_MOVING_OUT;
					*JumpMode	 = 0;
					return 1;
				}
			}
		}
		else {
			if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) != GROUP_HAS_SWITCH_NONE ) {
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) continue; // 2008.05.26
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
					//
					if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
						if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
					}
					//
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 0 ) && ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != ( Finger + 1 ) ) ) continue; // 2008.05.14
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
						bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
						if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
							nx = i;
							if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22
								if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_OUTS ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
											if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
												ChkSrc  = i;
												ChkSlot = j;
												priority = _SCH_PRM_GET_Putting_Priority( i );
											}
										}
									}
								}
								else if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_INS ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_S_MODE ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
											if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
												ChkSrc  = i;
												ChkSlot = j;
												priority = _SCH_PRM_GET_Putting_Priority( i );
											}
										}
									}
								}
								else if ( ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) == SWITCH_WILL_GO_OUT ) && _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
											if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
												ChkSrc  = i;
												ChkSlot = j;
												priority = _SCH_PRM_GET_Putting_Priority( i );
											}
										}
									}
								}
							}
						}
					}
				}
				if ( ChkSrc >= 0 ) {
					*NextChamber = ChkSrc;
					*NextSlot    = ChkSlot;
					*MovingType  = SCHEDULER_MOVING_OUT;
					*JumpMode	 = 0;
					return 31;
				}
			}
		}
	}
	else {
		if ( *MovingType == SCHEDULER_MOVING_OUT ) {
			//-------------------------------------------------------------------------------------------------------
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2014.03.19
				if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == PATHDO_WAFER_RETURN ) {
					_SCH_CLUSTER_Set_PathDo( Side , Pointer , _SCH_CLUSTER_Get_PathRange( Side , Pointer ) + 1 );
				}
				if ( _SCH_CLUSTER_Get_PathStatus( Side , Pointer ) == SCHEDULER_RETURN_REQUEST ) {
					_SCH_CLUSTER_Set_PathStatus( Side , Pointer , SCHEDULER_WAITING );
				}
			}
			//-------------------------------------------------------------------------------------------------------
			if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == PATHDO_WAFER_RETURN ) {
				k = _SCH_CLUSTER_Get_PathIn( Side , Pointer );
			}
			else {
				k = _SCH_CLUSTER_Get_PathOut( Side , Pointer );
			}
			//
			if ( k >= 0 ) { // 2017.01.23
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) return -493; // 2019.01.08
				if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , k , G_PLACE ) ) return -494; // 2019.01.08
				//
				switch( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( k ) ) { // 2004.01.30
				case 2 :
				case 3 :
				case 5 :
				case 6 :
					if ( Finger == 0 ) {
						if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) {
							return -44;
						}
					}
					else if ( Finger == 1 ) {
						if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) {
							return -44;
						}
					}
					break;
				}
				//
			}
			//-------------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------------
			if ( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) >= 0 ) { // 2017.01.23
				//-- 2002.03.06
				k = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,_SCH_CLUSTER_Get_PathOut( Side , Pointer ) );
				if ( k > 0 ) {
	//				if ( _SCH_MODULE_Get_PM_WAFER( ( k - 1 ) + PM1 , 0 ) > 0 ) { // 2014.01.10
					if ( !SCH_PM_IS_FREE( ( k - 1 ) + PM1 ) ) { // 2014.01.10
						return -31;
					}
				}
				//
			}
			else { // 2018.12.20
				//
				k = _SCH_SUB_GET_OUT_CM_AND_SLOT2( Side , Pointer , _SCH_CLUSTER_Get_PathIn( Side , Pointer ) , NextChamber , NextSlot , &i );
				//
				if ( i ) {
					return -33;
				}
				//
				if ( k == -1 ) {
					return -34;
				}
				//
				if ( NextChamber <= 0 ) {
					return -35;
				}
				//
				return -36;
			}
			//
			*NextChamber = _SCH_CLUSTER_Get_PathOut( Side , Pointer );
			*NextSlot    = ChkSlot;
			*MovingType  = SCHEDULER_MOVING_OUT;
			*JumpMode	 = 0;
			return 2;
		}
	}
	//============================================================================================================
	// 2006.05.15
	//============================================================================================================
	nextstockonly = 0;
	//
	if ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) ) == 101 ) {
		if ( !SCHEDULER_CONTROL_Special_Case_Checking( 0 , TMATM , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , Side , Pointer , 1 , &nextstockonly , TRUE ) ) {
			return -1001;
		}
	}
	//============================================================================================================
	//========================================================================
	// 2008.03.11
	//========================================================================
	/*
	//
	// 2016.07.05
	//
	if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) {
		jbmchk2 = _SCH_CLUSTER_Get_PathDo( Side , Pointer );
		if ( ( jbmchk2 > 1 ) && ( jbmchk2 != PATHDO_WAFER_RETURN ) ) {
			jbmchk1        = _SCH_CLUSTER_Get_PathRun( Side , Pointer , jbmchk2 - 2 , 0 );
			BeforeRunOrder = _SCH_CLUSTER_Get_PathOrder( Side , Pointer , jbmchk2 - 2 , jbmchk1 );
		}
		else {
			BeforeRunOrder = -1;
		}
	}
	else {
		BeforeRunOrder = -1;
	}
	*/
	//
	//
	//
	// 2016.07.05
	//
	BeforeRunOrder = -1;
	//
	//========================================================================
	*JumpMode = 0;
	jbmchk1 = 0;
	jbmchk2 = -1;
	priority = 9999999;
	//
	SameOrderPlaceCount = 0; // 2008.06.22
	OtherOrderPlaceCount = 0; // 2008.06.22
	OtherOrderPlaceCountOneGrp = 0; // 2016.12.13
	//
	PlusOneGroupPlaceCount = 0; // 2016.12.13
	//
//		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) { // 2006.04.25
	if ( ( *MovingType != SCHEDULER_MOVING_OUT ) && ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) ) { // 2006.04.25
		z = 0;
		zd = 0; // 2003.10.10
		okpm = FALSE; // 2002.07.23
		//
		BM_Waitiing = GatewayBM_waiting( TMATM , Side , Pointer , -1 ); // 2018.10.20
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//				*MovingOrder = k; // 2002.07.23
//				*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ); // 2002.07.23
//				*NextSlot    = 1; // 2002.07.23
			nxch = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ); // 2002.07.23
			//-------------------------------------------------------------------------------------
//				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &nxch ); // 2007.10.29
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( TRUE , CHECKING_SIDE , TMATM , Finger , &nxch , &xx ); // 2007.12.27
			//-------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------
			//=======================================================================================
			// 2006.05.15
			//=======================================================================================
			if ( nxch > 0 ) {
				//===================================================================================================================
				// 2016.01.06
				//===================================================================================================================
				if ( !SCHEDULING_CHECK_for_PARALLEL_Possible_PM( TMATM , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k , nxch ) ) {
					nxch = 0;
				}
				else {
					//===================================================================================================================
					//
					if ( nextstockonly == 1 ) {
						if ( !_SCH_PRM_GET_MODULE_STOCK( nxch ) ) {
							nxch = -1;
						}
					}
					else if ( nextstockonly >= 2 ) {
						if ( _SCH_PRM_GET_MODULE_STOCK( nxch ) ) {
							nxch = -1;
						}
					}
					//
				}
			}
			//=======================================================================================
			if ( nxch > 0 ) { // 2003.10.08
				if ( !_SCH_MODULE_GET_USE_ENABLE( nxch , FALSE , Side ) ) {
					nxch = -1;
				}
				else {
					if ( SCHEDULER_CONTROL_Check_Other_Arm_BeforeLot_And_Wait_Place_This_PM( Side , Pointer , nxch , TMATM , Finger ) ) { // 2013.09.03
						nxch = -1;
					}
					else { // 2015.11.27
						if ( _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() == 3 ) {
							if ( ( ( nxch - PM1 ) % 2 ) == 0 ) {
								if ( !_SCH_MODULE_GET_USE_ENABLE( nxch + 1 , FALSE , Side ) ) nxch = -1;
							}
							else {
								if ( !_SCH_MODULE_GET_USE_ENABLE( nxch - 1 , FALSE , Side ) ) nxch = -1;
							}
						}
					}
				}
			}
			else if ( nxch < 0 ) { // 2003.10.10
				zd++;
			}
			//=======================================================================================
			if ( nxch > 0 ) {
				//
				pmcond = FALSE; // 2002.07.23
				//
				if ( !BM_Waitiing && _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nxch , G_PLACE ) ) {
					//
					if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , nxch ) ) { // 2007.03.20
						switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,nxch ) ) { // 2003.02.05
						case 1 :
							if ( Finger != TA_STATION ) nxch = -1;
							break;
						case 2 :
							if ( Finger != TB_STATION ) nxch = -1;
							break;
						}
						if ( nxch > 0 ) {
							z++;
							jbmchk1++;
//							if ( ( _SCH_MODULE_Get_PM_WAFER( nxch , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
							if ( ( SCH_PM_IS_ABSENT2( nxch , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , &WaitingMultiwafer ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
								//
								if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ) >= 1 ) {
									pmcond = TRUE;
									//return 3; // 2002.07.23
								}
								if ( !pmcond ) { // 2002.07.23
									if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
										pmcond = TRUE;
										//return 4; // 2002.07.23
									}
								}
								if ( !pmcond ) { // 2002.07.23
									if ( ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) {
										pmcond = TRUE;
										//return 5; // 2002.07.23
									}
								}
								if ( !pmcond ) { // 2002.07.23
									if ( ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
										pmcond = TRUE;
										//return 6; // 2002.07.23
									}
								}
								if ( !pmcond ) { // 2002.07.23
									for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
										ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , i );
										//-------------------------------------------------------------------------------------
										SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc ); // 2007.10.29
										//-------------------------------------------------------------------------------------
										if ( ChkSrc > 0 ) {
//												if ( _SCH_PRM_GET_MODULE_xGROUP( ChkSrc ) == TMATM ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkSrc , G_PLACE ) ) {
												if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM , nxch , ChkSrc , -1 ) ) {
													pmcond = TRUE;
													//return 7; // 2002.07.23
												}
											}
										}
									}
								}
								//
								if ( pmcond ) { // 2018.10.20
									if ( putprfixch >= PM1 ) {
										if ( putprfixch == nxch ) {
											*MovingOrder = k;
											*NextChamber = nxch;
											*NextSlot    = 1;
											return 302;
										}
										else {
											pmcond = FALSE;
										}
									}
								}
								//
								if ( pmcond ) { // 2002.07.23
									//
									SameOrderPlaceCount++; // 2008.06.22
									//
									if ( WaitingMultiwafer ) { // 2018.05.08
										*MovingOrder = k;
										*NextChamber = nxch;
										*NextSlot    = 1;
										return 301;
									}
									//
									if ( putprfirstch == nxch ) { // 2002.07.23
										*MovingOrder = k;
										*NextChamber = nxch;
										*NextSlot    = 1;
										return 3;
									}
									//
									//
									/*
									// 2014.01.10
									if ( priority > _SCH_PRM_GET_Putting_Priority( nxch ) ) { // 2002.07.23
										*MovingOrder = k;
										*NextChamber = nxch;
										*NextSlot    = 1;
										okpm = TRUE;
										priority = _SCH_PRM_GET_Putting_Priority( nxch );
									}
									else if ( priority == _SCH_PRM_GET_Putting_Priority( nxch ) ) { // 2008.03.11
										if ( BeforeRunOrder == -1 ) {
											if ( _SCH_TIMER_GET_PROCESS_TIME_END( 0 , *NextChamber ) > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , nxch ) ) {
												*MovingOrder = k;
												*NextChamber = nxch;
												*NextSlot    = 1;
												okpm = TRUE;
												priority = _SCH_PRM_GET_Putting_Priority( nxch );
											}
										}
										else {
											if ( k == BeforeRunOrder ) {
												*MovingOrder = k;
												*NextChamber = nxch;
												*NextSlot    = 1;
												okpm = TRUE;
												priority = _SCH_PRM_GET_Putting_Priority( nxch );
											}
										}
									}
									*/
									//
									// 2014.01.10
									//
									prchkres = SCH_MULTI_PUT_PRIORITY_ORDERING_CHECK( Side , Pointer , priority , *NextChamber , nxch );
									//
									if ( prchkres == 1 ) {
										*MovingOrder = k;
										*NextChamber = nxch;
										*NextSlot    = 1;
										okpm = TRUE;
										priority = _SCH_PRM_GET_Putting_Priority( nxch );
									}
									else if ( prchkres == 0 ) {
										if ( BeforeRunOrder == -1 ) {
											if ( _SCH_TIMER_GET_PROCESS_TIME_END( 0 , *NextChamber ) > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , nxch ) ) {
												*MovingOrder = k;
												*NextChamber = nxch;
												*NextSlot    = 1;
												okpm = TRUE;
												priority = _SCH_PRM_GET_Putting_Priority( nxch );
											}
										}
										else {
											if ( k == BeforeRunOrder ) {
												*MovingOrder = k;
												*NextChamber = nxch;
												*NextSlot    = 1;
												okpm = TRUE;
												priority = _SCH_PRM_GET_Putting_Priority( nxch );
											}
										}
									}
									//
								}
							}
							else { // 2018.10.20
								if ( WaitingMultiwafer ) {
									if ( Finger == TA_STATION ) {
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
											if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) {
												return -1236;
											}
										}
									}
									else if ( Finger == TB_STATION ) {
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) {
											if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) {
												return -1236;
											}
										}
									}
								}
							}
						}
					}
				}
				else {
//					if ( ( _SCH_CLUSTER_Get_PathIn( Side , Pointer ) >= PM1 ) || ( !_SCH_MODULE_Get_Use_Interlock_Run( CHECKING_SIDE , nxch ) ) ) {
//					if ( ( _SCH_MODULE_Get_PM_WAFER( nxch , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2008.06.22 // 2014.01.10
					if ( ( SCH_PM_IS_ABSENT( nxch , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2008.06.22 // 2014.01.10
						//
						if ( _SCH_PRM_GET_Getting_Priority( _SCH_PRM_GET_MODULE_GROUP( nxch ) + TM ) < _SCH_PRM_GET_Getting_Priority( TMATM + TM ) ) {
							//
							if ( OtherOrderPlaceCountOneGrp == 0 ) { // 2016.12.13
								OtherOrderPlaceGrp = _SCH_PRM_GET_MODULE_GROUP( nxch );
							}
							else {
								if ( OtherOrderPlaceGrp != _SCH_PRM_GET_MODULE_GROUP( nxch ) ) {
									if ( _SCH_PRM_GET_Getting_Priority( _SCH_PRM_GET_MODULE_GROUP( nxch ) + TM ) < _SCH_PRM_GET_Getting_Priority( OtherOrderPlaceGrp + TM ) ) {
										OtherOrderPlaceCountOneGrp = 0;
										OtherOrderPlaceGrp = _SCH_PRM_GET_MODULE_GROUP( nxch );
									}
								}
							}
							//
							OtherOrderPlaceCountOneGrp++;
							//
						}
						//
						OtherOrderPlaceCount++; // 2008.06.22
					}
					//
					z++;
					jbmchk2 = _SCH_PRM_GET_MODULE_GROUP( nxch );
//					}
					//
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM + 1 , nxch , G_PLACE ) ) { //  // 2016.12.13
						PlusOneGroupPlaceCount++;
					}
					//
				}
			}
		}
		//
		if ( okpm ) { // 2002.07.23
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2008.06.22
				//
				if ( OtherOrderPlaceCountOneGrp > 0 ) { // 2016.12.13
					//
					PMSameOK = TRUE; // 2016.12.13
					//
				}
				else {
					return 4; // 2002.07.23
				}
			}
			else { // 2008.06.22
				if      ( OtherOrderPlaceCount > SameOrderPlaceCount ) {
					jbmchk1 = 0;
				}
				else {
					return 4;
				}
			}
		}
		//--------------------------------------------------------------------------------
		if ( z == 0 ) {
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2007.09.07
				if ( zd != 0 ) return -98; // 2003.10.10
				return -99; // No More Run
			}
		}
		//--------------------------------------------------------------------------------
	}
//		else if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) { // 2006.04.25
	else if ( ( *MovingType != SCHEDULER_MOVING_OUT ) && ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) == _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) ) { // 2006.04.25
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-01 \t[Side=%d][Pointer=%d]\n" , Side , Pointer );
//------------------------------------------------------------------------------------------------------------------
		z = 0;
		zd = 0; // 2003.10.10
		okpm = FALSE; // 2002.07.23
		//
		BM_Waitiing = GatewayBM_waiting( TMATM , Side , Pointer , -1 ); // 2018.10.20
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//				*MovingOrder = k; // 2002.07.23
//				*NextChamber = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ); // 2002.07.23
//				*NextSlot    = 1; // 2002.07.23
			nxch = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ); // 2002.07.23
			//-------------------------------------------------------------------------------------
//				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &nxch ); // 2007.10.29 // 2007.12.27
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( TRUE , CHECKING_SIDE , TMATM , Finger , &nxch , &xx ); // 2007.12.27
			//-------------------------------------------------------------------------------------
			//=======================================================================================
			// 2006.05.24
			//=======================================================================================
			if ( nxch > 0 ) {
				//
				if ( nextstockonly == 1 ) {
					if ( !_SCH_PRM_GET_MODULE_STOCK( nxch ) ) {
						nxch = -1;
					}
				}
				else if ( nextstockonly >= 2 ) {
					if ( _SCH_PRM_GET_MODULE_STOCK( nxch ) ) {
						nxch = -1;
					}
				}
			}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-02 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
			//==============================================================================================================================================================================
			if ( nxch > 0 ) { // 2003.10.08
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-03 \tSLEEP 5 sec\n" );
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-03 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
				if ( !_SCH_MODULE_GET_USE_ENABLE( nxch , FALSE , Side ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-04 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
//					nxch = -1; // 2011.02.23
					//
					pathDo = _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1; // 2011.03.20
					//
//					nxch = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ); // 2011.02.23 // 2011.03.20
					nxch = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , pathDo , k ); // 2011.02.23 // 2011.03.20
					//
					if      ( nxch < 0 ) { // 2011.02.23
						zd++;
					}
					else if ( nxch > 0 ) { // 2011.02.23
						//
						nxch = -1;
						//
						if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2011.03.20
							//
//							zd++;
							//
//							_SCH_CLUSTER_Set_PathProcessChamber_Disable( Side , Pointer , pathDo , k );
							//
						}
						//
					}
				}
			}
			else if ( nxch < 0 ) { // 2003.10.10
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-05 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
				zd++;
			}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-06 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
			//==============================================================================================================================================================================
			if ( nxch > 0 ) {
				if ( !BM_Waitiing && _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nxch , G_PLACE ) ) {
					if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , nxch ) ) { // 2007.03.20
						switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,nxch ) ) { // 2003.02.05
						case 1 :
							if ( Finger != TA_STATION ) nxch = -1;
							break;
						case 2 :
							if ( Finger != TB_STATION ) nxch = -1;
							break;
						}
						if ( nxch > 0 ) {
							z++;
							jbmchk1++;
							//
//							if ( ( _SCH_MODULE_Get_PM_WAFER( nxch , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 )  ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
							if ( ( SCH_PM_IS_ABSENT2( nxch , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , &WaitingMultiwafer ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 )  ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
								//
								SameOrderPlaceCount++; // 2008.06.22
								//
								if ( WaitingMultiwafer ) {
									*MovingOrder = k;
									*NextChamber = nxch;
									*NextSlot    = 1;
									return 701;
								}
								//
								if ( putprfirstch == nxch ) { // 2002.07.23
									*MovingOrder = k;
									*NextChamber = nxch;
									*NextSlot    = 1;
									return 7;
								}
								/*
								// 2014.01.10
								if ( priority > _SCH_PRM_GET_Putting_Priority( nxch ) ) { // 2002.07.23
									*MovingOrder = k;
									*NextChamber = nxch;
									*NextSlot    = 1;
									okpm = TRUE;
									priority = _SCH_PRM_GET_Putting_Priority( nxch );
								}
								else if ( priority == _SCH_PRM_GET_Putting_Priority( nxch ) ) { // 2008.03.11
									if ( BeforeRunOrder == -1 ) {
										if ( _SCH_TIMER_GET_PROCESS_TIME_END( 0 , *NextChamber ) > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , nxch ) ) {
											*MovingOrder = k;
											*NextChamber = nxch;
											*NextSlot    = 1;
											okpm = TRUE;
											priority = _SCH_PRM_GET_Putting_Priority( nxch );
										}
									}
									else {
										if ( k == BeforeRunOrder ) {
											*MovingOrder = k;
											*NextChamber = nxch;
											*NextSlot    = 1;
											okpm = TRUE;
											priority = _SCH_PRM_GET_Putting_Priority( nxch );
										}
									}
								}
								*/
								//
								// 2014.01.10
								//
								prchkres = SCH_MULTI_PUT_PRIORITY_ORDERING_CHECK( Side , Pointer , priority , *NextChamber , nxch );
								//
								if ( prchkres == 1 ) {
									*MovingOrder = k;
									*NextChamber = nxch;
									*NextSlot    = 1;
									okpm = TRUE;
									priority = _SCH_PRM_GET_Putting_Priority( nxch );
								}
								else if ( prchkres == 0 ) { // 2008.03.11
									if ( BeforeRunOrder == -1 ) {
										if ( _SCH_TIMER_GET_PROCESS_TIME_END( 0 , *NextChamber ) > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , nxch ) ) {
											*MovingOrder = k;
											*NextChamber = nxch;
											*NextSlot    = 1;
											okpm = TRUE;
											priority = _SCH_PRM_GET_Putting_Priority( nxch );
										}
									}
									else {
										if ( k == BeforeRunOrder ) {
											*MovingOrder = k;
											*NextChamber = nxch;
											*NextSlot    = 1;
											okpm = TRUE;
											priority = _SCH_PRM_GET_Putting_Priority( nxch );
										}
									}
								}
								//
								//return 8; // 2002.07.23
							}
							else { // 2018.10.20
								if ( WaitingMultiwafer ) {
									if ( Finger == TA_STATION ) {
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
											if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) {
												return -1237;
											}
										}
									}
									else if ( Finger == TB_STATION ) {
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) {
											if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) {
												return -1237;
											}
										}
									}
								}
							}
						}
					}
				}
				else {
//					if ( ( _SCH_CLUSTER_Get_PathIn( Side , Pointer ) >= PM1 ) || ( !_SCH_MODULE_Get_Use_Interlock_Run( CHECKING_SIDE , nxch ) ) ) {
						//
//					if ( ( _SCH_MODULE_Get_PM_WAFER( nxch , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2008.06.22 // 2014.01.10
					if ( ( SCH_PM_IS_ABSENT( nxch , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( nxch ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( nxch ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( nxch ) < 0 ) ) ) { // 2008.06.22 // 2014.01.10
						//
						if ( _SCH_PRM_GET_Getting_Priority( _SCH_PRM_GET_MODULE_GROUP( nxch ) + TM ) < _SCH_PRM_GET_Getting_Priority( TMATM + TM ) ) {
							//
							if ( OtherOrderPlaceCountOneGrp == 0 ) { // 2016.12.13
								OtherOrderPlaceGrp = _SCH_PRM_GET_MODULE_GROUP( nxch );
							}
							else {
								if ( OtherOrderPlaceGrp != _SCH_PRM_GET_MODULE_GROUP( nxch ) ) {
									if ( _SCH_PRM_GET_Getting_Priority( _SCH_PRM_GET_MODULE_GROUP( nxch ) + TM ) < _SCH_PRM_GET_Getting_Priority( OtherOrderPlaceGrp + TM ) ) {
										OtherOrderPlaceCountOneGrp = 0;
										OtherOrderPlaceGrp = _SCH_PRM_GET_MODULE_GROUP( nxch );
									}
								}
							}
							//
							OtherOrderPlaceCountOneGrp++;
							//
						}
						//
						OtherOrderPlaceCount++; // 2008.06.22
					}
					//
					//
					if ( jbmchk2 < _SCH_PRM_GET_MODULE_GROUP( nxch ) ) { // 2016.10.30
						z++;
						jbmchk2 = _SCH_PRM_GET_MODULE_GROUP( nxch );
					}
					//
//					}
					//
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM + 1 , nxch , G_PLACE ) ) { //  // 2016.12.13
						PlusOneGroupPlaceCount++;
					}
					//
				}
			}
		}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-11 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 );
//------------------------------------------------------------------------------------------------------------------
		if ( okpm ) {
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2008.06.22
				//
				if ( OtherOrderPlaceCountOneGrp > 0 ) { // 2016.12.13
					//
					PMSameOK = TRUE; // 2016.12.13
					//
				}
				else {
					return 8; // 2002.07.23
				}
			}
			else { // 2008.06.22
				if      ( OtherOrderPlaceCount > SameOrderPlaceCount ) {
					jbmchk1 = 0;
				}
				else {
					return 8;
				}
			}
		}
		//--------------------------------------------------------------------------------
		if ( z == 0 ) {
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2007.09.07
				if ( zd != 0 ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-101 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
					return -101; // 2003.10.10
				}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-99 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d]\n" , nxch , k , z , zd , okpm );
//------------------------------------------------------------------------------------------------------------------
				return -99; // No More Run
			}
		}
		//--------------------------------------------------------------------------------
	}
	else {
		if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , -2 ) ) { // 2007.03.20
			if ( _SCH_PRM_GET_MODULE_GROUP( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) ) > TMATM ) {
				jbmchk1 = 0;
				jbmchk2 = TMATM + 1;
			}
		}
	}
	//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-12 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 );
//------------------------------------------------------------------------------------------------------------------
	//==============================================================================================================================================================================
	// 2008.06.12
	//==============================================================================================================================================================================
	if ( ( jbmchk1 > 0 ) && ( jbmchk2 >= 0 ) ) {
		if ( SameOrderPlaceCount == 0 ) jbmchk1 = 0;
	}
	//==============================================================================================================================================================================
	//
//	if ( ( jbmchk1 <= 0 ) && ( jbmchk2 >= 0 ) ) { // 2016.12.13
	if ( ( ( PMSameOK ) || ( jbmchk1 <= 0 ) ) && ( jbmchk2 >= 0 ) ) { // 2016.12.13
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-13 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 );
//------------------------------------------------------------------------------------------------------------------
		ChkSrc = -1;
		priority = 9999999;
		//
		if ( jbmchk2 > TMATM ) {
			//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-14 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 );
//------------------------------------------------------------------------------------------------------------------
			// 2016.12.13
			sbm = BM1;
			ebm = ( BM1 + MAX_BM_CHAMBER_DEPTH );
			//
//			if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 4 ) == 0 ) { // 0:SeparateSupply , 1:AllSupply // 2018.09.17
			if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 4 ) == 1 ) { // 0:AllSupply , 1:SeparateSupply // 2018.09.17
				//
				//
				if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17
					//
					if ( ( TMATM + 2 ) < MAX_TM_CHAMBER_DEPTH ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-15 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 );
//------------------------------------------------------------------------------------------------------------------
						//
						if ( _SCH_PRM_GET_MODULE_MODE( TM + TMATM + 2 ) ) {
							//
							bma = 0;
							bmb = 0;
							//
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) {
									//
									if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) {
										//
										if      ( bma == 0 ) bma = i;
										else if ( bmb == 0 ) bmb = i;
										else                 bma = -1;
										//
									}
									//
								}
							}
							//
							if ( ( bma > 0 ) && ( bmb > 0 ) ) {
								if ( PlusOneGroupPlaceCount <= 0 ) {
									sbm = bmb;
									ebm = bmb+1;
								}
								else {
									sbm = bma;
									ebm = bma+1;
								}
							}
						}
					}
				}
			}
			//
//			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2016.12.13
			for ( i = sbm ; i < ebm ; i++ ) { // 2016.12.13
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
				//
				if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
					if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
				}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-16 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
				//
//				if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) { // 2013.03.18 // 2016.12.12
				if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( TMATM == 0 ) ) ) { // 2016.12.21
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) { // 2006.05.25
						//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-17 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( (TMATM+1) , i ) == BUFFER_SWITCH_MODE ) {
							//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-17(2) \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
							if ( SCHEDULING_CHECK_Enable_Place_To_BM( TMATM , Side , Pointer , Finger , &k , &j , (TMATM+1) , BUFFER_FM_STATION , MoveOnly , i , -1 , FALSE , FALSE , &hasin ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-18 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
								if ( priority > _SCH_PRM_GET_Putting_Priority( k ) ) {
									ChkSrc  = k;
									ChkSlot = j;
									priority = _SCH_PRM_GET_Putting_Priority( k );
									//
									ChkhasIn = hasin;
									//
								}
								else if ( priority == _SCH_PRM_GET_Putting_Priority( k ) ) { // 2018.10.20
									if ( ChkhasIn < hasin ) {
										//
										ChkSrc  = k;
										ChkSlot = j;
										priority = _SCH_PRM_GET_Putting_Priority( k );
										//
										ChkhasIn = hasin;
										//
									}
								}
							}
						}
						//
						//
						// 2016.12.12
						//
						else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( (TMATM+1) , i ) == BUFFER_IN_MODE ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-19 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
							bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
							if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
								nx = i;
								if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 0 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-19 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
									if ( ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) == GROUP_HAS_SWITCH_NONE ) || ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) <= SWITCH_WILL_GO_RUN ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-20 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
												//
												hasin = SCHEDULER_CONTROL_Chk_Get_BM_Count( i , -1 ); // 2018.10.20
												//
												if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-21 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
													//
													ChkhasIn = hasin;
													//
												}
												else if ( priority == _SCH_PRM_GET_Putting_Priority( i ) ) { // 2018.10.20
													if ( ChkhasIn < hasin ) {
														//
														ChkSrc  = k;
														ChkSlot = j;
														priority = _SCH_PRM_GET_Putting_Priority( i );
														//
														ChkhasIn = hasin;
														//
													}
												}
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
				else {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-22 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == (TMATM+1) && !Get_Prm_MODULE_MOVE_GROUP(i) ) ) { // 2006.05.25
						//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-23 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( (TMATM+1) , i ) == BUFFER_IN_MODE ) {
							bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
							if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
								nx = i;
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-24 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
								if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 0 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-25 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
									if ( ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) == GROUP_HAS_SWITCH_NONE ) || ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) <= SWITCH_WILL_GO_RUN ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-26 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_FM_STATION ) ) { // 2006.05.26
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-27 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
												if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-28 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
												}
											}
										}
									}
								}
							}
						}
						//
					}
				}
				//
			}
			if ( ChkSrc >= 0 ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-31 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
				*NextChamber = ChkSrc;
				*NextSlot    = ChkSlot;
				*MovingType  = SCHEDULER_MOVING_OUT;
				*JumpMode	 = 1;
				//
				z = 0;
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2002.11.05
					jbmchk1 = 0;
					if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM+1 ) < 1 ) {
						if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
							z = 0;
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								jbmchk1 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &jbmchk1 ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( jbmchk1 > 0 ) {
									//
//									if ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) == ( TMATM + 1 ) ) { // 2014.09.24
									//
									if ( ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) == ( TMATM + 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) >= 0 ) ) { // 2014.09.24
										//
										if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , jbmchk1 ) ) { // 2007.03.20
											z++;
//											if ( ( _SCH_MODULE_Get_PM_WAFER( jbmchk1 , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( jbmchk1 ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( jbmchk1 ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
											if ( ( SCH_PM_IS_ABSENT( jbmchk1 , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( jbmchk1 ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( jbmchk1 ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
												for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
													ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , i );
													//-------------------------------------------------------------------------------------
													SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc ); // 2007.10.29
													//-------------------------------------------------------------------------------------
													if ( ChkSrc > 0 ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-25 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
														//
//														if ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) == TMATM + 1 ) { // 2014.09.24
														//
														if ( ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) == ( TMATM + 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) >= 0 ) ) { // 2014.09.24
															//
															if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM + 1 , jbmchk1 , ChkSrc , -1 ) ) return 61;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( z == 0 ) return 62;
			}
		}
		else if ( jbmchk2 < TMATM ) {
			//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-41 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
//			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , Finger , -1 , 5 , FALSE ) != 0 ) { // 2013.05.07 // 2014.07.18
			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , Finger , -1 , 5 , FALSE ) == 1 ) { // 2013.05.07 // 2014.07.18
				//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-42 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_00_NOTUSE ) continue; // 2009.08.21
					//
					if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
						if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( TMATM , i , Side , Pointer , BUFFER_OUTWAIT_STATUS ) ) continue; // 2009.08.26
					}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-43 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
							bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
							if ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( i ) < 0 ) ) { // 2007.01.02
								nx = i;
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-44 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
								if ( SCHEDULING_CHECK_for_Next_Place_Possible_Check_when_SingleArm( TMATM , CHECKING_SIDE , Pointer , -1 , 0 , 0 , 1 , _SCH_MODULE_Get_TM_OUTCH( TMATM,Finger ) , &nx , 1 , TRUE , MoveOnly ) > 0 ) { // 2007.02.22
									if ( ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) == GROUP_HAS_SWITCH_NONE ) || ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , Pointer ) >= SWITCH_WILL_GO_OUTS ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-45 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
												if ( priority > _SCH_PRM_GET_Putting_Priority( i ) ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-46 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
													ChkSrc  = i;
													ChkSlot = j;
													priority = _SCH_PRM_GET_Putting_Priority( i );
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( ChkSrc >= 0 ) {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-47 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
					*NextChamber = ChkSrc;
					*NextSlot    = ChkSlot;
					*MovingType  = SCHEDULER_MOVING_OUT;
					*JumpMode	 = 0;
					//
					z = 0;
					if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2002.11.05
						jbmchk1 = 0;
						if ( TMATM > 0 ) {
							if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM-1 ) < 1 ) {
								if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
									z = 0;
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
										jbmchk1 = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
										//-------------------------------------------------------------------------------------
										SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &jbmchk1 ); // 2007.10.29
										//-------------------------------------------------------------------------------------
										if ( jbmchk1 > 0 ) {
											//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-48 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
											if ( _SCH_MODULE_GET_USE_ENABLE( jbmchk1 , FALSE , Side ) ) { // 2003.10.08
												//
//												if ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) == ( TMATM - 1 ) ) { // 2014.09.24
												//
												if ( ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) == ( TMATM - 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( jbmchk1 ) >= 0 ) ) { // 2014.09.24
													//
													if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , jbmchk1 ) ) { // 2007.03.20
														z++;
//														if ( ( _SCH_MODULE_Get_PM_WAFER( jbmchk1 , 0 ) <= 0 ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( jbmchk1 ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( jbmchk1 ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
														if ( ( SCH_PM_IS_ABSENT( jbmchk1 , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) && ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( jbmchk1 ) == TMATM ) ) && ( MoveOnly || ( _SCH_MACRO_CHECK_PROCESSING_INFO( jbmchk1 ) <= 0 ) || ( _SCH_PRM_GET_Putting_Priority( jbmchk1 ) < 0 ) ) ) { // 2006.02.08 // 2006.06.14 // 2007.01.02 // 2014.01.10
															for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
																ChkSrc = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , i );
																//-------------------------------------------------------------------------------------
																SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkSrc ); // 2007.10.29
																//-------------------------------------------------------------------------------------
																if ( ChkSrc > 0 ) {
																	//
//																	if ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) == TMATM - 1 ) { // 2014.09.24
																	//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-49 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
																	if ( ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) == ( TMATM - 1 ) ) && ( _SCH_PRM_GET_MODULE_GROUP( ChkSrc ) >= 0 ) ) { // 2014.09.24
																		//
																		if ( SCHEDULING_CHECK_for_MOVE_ENABLE( TMATM - 1 , jbmchk1 , ChkSrc , -1 ) ) return 71;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-50 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
					if ( z == 0 ) return 72;
				}
			}
		}
	}
	//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_PLACE-51 \t[nxch=%d][k=%d][z=%d][zd=%d][okpm=%d][%d,%d,%d][%d,%d][%d]\n" , nxch , k , z , zd , okpm , OtherOrderPlaceCountOneGrp , SameOrderPlaceCount , PlusOneGroupPlaceCount , jbmchk1 , jbmchk2 , i );
//------------------------------------------------------------------------------------------------------------------
	if ( PMSameOK ) return 841; // 2016.12.13
	//
	return -1;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( int TMATM , int CHECKING_SIDE , int mode , int pt ) {
	int i , j = 0 , bmd;
	BOOL ckb = FALSE;

	if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( TMATM ) == GROUP_HAS_SWITCH_NONE ) return TRUE;
	if ( mode == 0 ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				bmd = _SCH_MODULE_Get_BM_FULL_MODE( i ); // 2006.05.26
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_MODE ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( i , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( bmd == BUFFER_TM_STATION ) ) { // 2006.05.26
								ckb = TRUE;
							}
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_OUT_S_MODE ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
							return FALSE;
						}
					}
				}
			}
		}
	}
	else if ( mode == 1 ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_S_MODE ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
							return FALSE;
						}
					}
				}
			}
		}
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) { // 2002.08.27
			if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ) <= 0 ) {
				return FALSE;
			}
		}
		if ( pt >= 0 ) {
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				j = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , i );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &j ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( j > 0 ) {
					if ( _SCH_PRM_GET_MODULE_GROUP( j ) == TMATM ) {
//						if ( ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) <= 0 ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) <= 0 ) ) { // 2014.01.10
						if ( ( SCH_PM_IS_ABSENT( j , CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 ) ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) <= 0 ) ) { // 2014.01.10
							ckb = TRUE;
						}
					}
				}
			}
		}
	}
	else if ( mode == 2 ) {
		if ( pt >= 0 ) {
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				j = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , pt , 0 , i );
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &j ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				if ( j > 0 ) {
					if ( _SCH_PRM_GET_MODULE_GROUP( j ) == TMATM ) {
						ckb = TRUE;
					}
				}
			}
		}
	}
	return ckb;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_TM_Another_No_More_Supply( int side ) { // 2004.11.23
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) > 0 ) { // 2005.03.04
//			if ( ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) || ( ( _SCH_SYSTEM_USING_GET( i ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( i ) < 2 ) ) ) { // 2005.03.04 // 2005.07.29
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( ( _SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && ( !_SCH_MODULE_Chk_TM_Finish_Status( i ) ) ) {
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
//=======================================================================================
//=======================================================================================
BOOL SCHEDULING_CHECK_TM_Another_Free_Status( int side , BOOL fmcheck , BOOL Swapping , int bmc ) {
	int i , ck;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( side != i ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) >= 11 ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) { // 2003.06.13 // 2005.07.29
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , bmc ) != MUF_01_USE ) continue; // 2013.04.10
			//
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( Swapping ) { // 2004.03.10
					ck = FALSE;
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == 0 ) {
						ck = TRUE;
					}
					else {
						if ( bmc == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) ) {
							ck = TRUE;
						}
					}
				}
				else {
					ck = TRUE;
				}
				if ( ck ) {
					if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return FALSE;
					if ( fmcheck ) {
						if ( !_SCH_MODULE_Chk_FM_Free_Status( i ) ) return FALSE; // 2003.06.13
					}
				}
			}
		}
	}
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_TM_All_Free_Status( int mode ) { // 2007.08.16 // 2012.08.30
	int i , ck = FALSE;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
			if ( !_SCH_MODULE_Chk_TM_Free_Status2( i , mode ) ) return FALSE;
			ck = TRUE;
		}
	}
	return ck;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_TM_Has_More_Space( int oside , int TMATM , int finger , int bmc ) { // 2003.11.07
	int side , pointer;
//	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return FALSE; // 2007.01.04
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE; // 2007.01.04
	if ( finger == TA_STATION ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) return TRUE;
		side = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
		pointer = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
		//
		if ( bmc >= BM1 ) { // 2014.05.30
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , bmc ) == MUF_00_NOTUSE ) return TRUE;
		}
		//
//		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= 1 ) return TRUE; // 2010.07.09
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= _SCH_CLUSTER_Get_PathRange( side , pointer ) ) return TRUE; // 2010.07.09
		if ( side != oside ) { // 2004.03.10
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != 0 ) {
				if ( bmc != SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) ) return TRUE;
			}
		}
	}
	else if ( finger == TB_STATION ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) return TRUE;
		side = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
		pointer = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
		//
		if ( bmc >= BM1 ) { // 2014.05.30
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , bmc ) == MUF_00_NOTUSE ) return TRUE;
		}
		//
//		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= 1 ) return TRUE; // 2010.07.09
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) <= _SCH_CLUSTER_Get_PathRange( side , pointer ) ) return TRUE; // 2010.07.09
		if ( side != oside ) { // 2004.03.10
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != 0 ) {
				if ( bmc != SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) ) return TRUE;
			}
		}
	}
	return FALSE;
}
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
BOOL SCHEDULING_CHECK_CM_Another_No_More_Supply( int side , int pt ) { // 2005.01.03
	int i , ch , ChkTrg , s , p;
	int pmslot;

	//============================================================================================================
	if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) != 2 ) return FALSE;
	//============================================================================================================
	ch = -1;
	for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
		//
		if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue; // 2014.01.10
			//
//			if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) { // 2014.01.10
			if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) { // 2014.01.10
				//
//				s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2005.01.11 // 2014.01.10
//				p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2005.01.11 // 2014.01.10
				//
				s = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2005.01.11 // 2014.01.10
				p = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2005.01.11 // 2014.01.10
				//
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) { // 2005.01.11
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
						if ( ch == -1 ) {
							ch = i;
						}
						else {
							if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) < _SCH_TIMER_GET_PROCESS_TIME_START( 0 , ch ) ) {
								ch = i;
							}
						}
					}
				}
			}
//		} // 2014.01.10
	}
	//============================================================================================================
	if ( ch == -1 ) {
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
			//
//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
			//
			if ( !SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) continue; // 2014.01.10
				//
//				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) { // 2014.01.10
				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) { // 2014.01.10
					//
//					s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2005.01.11 // 2014.01.10
//					p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2005.01.11 // 2014.01.10
					//
					s = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2005.01.11 // 2014.01.10
					p = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2005.01.11 // 2014.01.10
					//
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) { // 2005.01.11
						if ( ch == -1 ) {
							ch = i;
						}
						else {
							if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) < _SCH_TIMER_GET_PROCESS_TIME_START( 0 , ch ) ) {
								ch = i;
							}
						}
					}
				}
//			} // 2014.01.10
		}
	}
	//============================================================================================================
	if ( ch == -1 ) return FALSE;
	//============================================================================================================
	if ( 0 < _SCH_CLUSTER_Get_PathRange( side , pt ) ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , 0 , i );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , side ) ) {
//					if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) return FALSE; // 2005.01.06 // 2014.01.10
					if ( SCH_PM_IS_ABSENT( ChkTrg , side , pt , 0 ) ) return FALSE; // 2005.01.06 // 2014.01.10
					if ( ChkTrg == ch ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}


BOOL SCHEDULING_CHECK_REMAKE_GROUPPING_POSSIBLE( int side , int pt , int sind ) {
	int i , j , xc = -1 , ch;
	for ( i = sind ; i < _SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			ch = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , i , j );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ch > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) {
					if ( i == sind ) {
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
							if ( xc == -1 ) xc = ch;
							else            return FALSE;
						}
					}
					else {
						if ( xc == -1 ) return FALSE;
						if ( xc == ch ) return FALSE;
					}
				}
			}
		}
	}
	if ( xc == -1 ) return FALSE;
	return TRUE;
}


#define TIMECHECK_TARGET_SEC	1
int timecheck[MAX_TM_CHAMBER_DEPTH];
time_t timedata[MAX_TM_CHAMBER_DEPTH];


BOOL SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( int TMATM , int mode ) {
	time_t finish;
	if ( mode == 1 ) {
		if ( timecheck[TMATM] == 0 ) {
			timecheck[TMATM] = 1;
			time( &timedata[TMATM] );
			return FALSE;
		}
		else if ( timecheck[TMATM] == 1 ) {
			time( &finish );
			if ( difftime( finish , timedata[TMATM] ) >= TIMECHECK_TARGET_SEC ) {
				timecheck[TMATM] = 2;
				return TRUE;
			}
		}
		else if ( timecheck[TMATM] == 2 ) {
			return TRUE;
		}
	}
	else {
		timecheck[TMATM] = 0;
	}
	return FALSE;
}


int SCHEDULING_CHECK_for_Enable_PICK_from_PM_MG( int TMATM , int CHECKING_SIDE , int Chamber , int pmslot , int *MovingType , int *MovingOrder , int *Next_Chamber , BOOL FMMode , BOOL MoveOnly , int *WaitMode , int prcsgap , int remaintime , int swbm , BOOL *EndMode , BOOL *wy , int *finger , int *halfpick , int srcheck , int *prret ) {
	int Result , i , s , p , d , nch , findex , rfindex;
	int orch[MAX_PM_CHAMBER_DEPTH];

	if ( pmslot < 0 ) { // 2014.01.10
		if ( !SCH_PM_HAS_PICKING( Chamber , &pmslot ) ) return -510; // 2014.01.10
	}
	//
//	if ( _SCH_MODULE_Get_PM_WAFER( Chamber , 0 ) <= 0 ) { // 2014.01.10
	if ( _SCH_MODULE_Get_PM_WAFER( Chamber , pmslot ) <= 0 ) { // 2014.01.10
		return -511;
	}

//	s = _SCH_MODULE_Get_PM_SIDE( Chamber , 0 ); // 2014.01.10
//	p = _SCH_MODULE_Get_PM_POINTER( Chamber , 0 ); // 2014.01.10
	//
	s = _SCH_MODULE_Get_PM_SIDE( Chamber , pmslot ); // 2014.01.10
	p = _SCH_MODULE_Get_PM_POINTER( Chamber , pmslot ); // 2014.01.10
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );

	if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) {
		return -512;
	}

	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) {
		*prret = TRUE; // 2006.03.23
		return -513;
	}

	Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( TMATM , CHECKING_SIDE , Chamber , pmslot , MovingType , MovingOrder , Next_Chamber , FMMode , TRUE , WaitMode , prcsgap , remaintime , swbm , EndMode , wy , finger , halfpick , srcheck );

	if ( Result >= 0 ) {
		return -514;
	}

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( i != TMATM ) {
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( i , Chamber ) == 1 ) {
				if ( timecheck[i] != 2 ) return -515;
			}
		}
	}

	findex = FALSE;
	rfindex = FALSE;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		orch[i] = 0;
		nch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , i );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &nch ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if      ( nch > 0 ) {
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , nch , G_PLACE ) ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , d , i );
				orch[i] = 1;
				findex = TRUE;
			}
			else {
				rfindex = TRUE;
			}
		}
		else if ( nch < 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , -nch , G_PLACE ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( -nch , FALSE , s ) ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , d , i );
					orch[i] = -1;
					findex = TRUE;
					rfindex = TRUE;
				}
			}
		}
	}

	if ( !findex ) {
		return -516;
	}

	if ( rfindex ) {
		Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( TMATM , CHECKING_SIDE , Chamber , pmslot , MovingType , MovingOrder , Next_Chamber , FMMode , FALSE , WaitMode , prcsgap , remaintime , swbm , EndMode , wy , finger , halfpick , srcheck );
	}
	else {
		Result = -1;
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( orch[i] == 1 ) {
			_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , d , i );
		}
		else if ( orch[i] == -1 ) {
			_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , d , i );
		}
	}

	if ( Result < 0 ) return Result - 1000;
	return Result;
}

int SCHEDULING_CHECK_FIXED_GROUPPING_FLOWDATA( int side , int pt , int pd , int tmatm ) {
	int i , nch;
//	char Buffer[256];
	//=============================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) return -1; // 2007.01.04
	//=============================================================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		nch = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , pd , i );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &nch ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if      ( nch > 0 ) {
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , nch , G_PLACE ) ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , pd , i );
			}
		}
		else if ( nch < 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , -nch , G_PLACE ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( -nch , FALSE , side ) ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Enable( side , pt , pd , i );
				}
			}
		}
		//-------------------------------------------------------------------------------------
	}
	return 0;
}

int SCHEDULING_CHECK_REMAKE_GROUPPING_FLOWDATA( int TMATM , int arm , int side , int pt , int curch ) {
	int i , j , k , sn = 0;
	int PathDo , ch , gres , Result , wafer , tdata , ttm , tx , st , pm , s2 , p2 , ttm2;
	int grp_mv[MAX_TM_CHAMBER_DEPTH];
	int grp_tm[MAX_TM_CHAMBER_DEPTH];
	int grp_tx[MAX_TM_CHAMBER_DEPTH];
	int grp_pm[MAX_TM_CHAMBER_DEPTH];
	int grp_st[MAX_TM_CHAMBER_DEPTH];
	int grp_pmo[MAX_TM_CHAMBER_DEPTH];
	int grp_sto[MAX_TM_CHAMBER_DEPTH];
	int grp_go[MAX_TM_CHAMBER_DEPTH];
	char Buffer[256];
	char Buffer2[64];
	int pmslot;

	if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == -1 ) return -1;

	//=============================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) return -1; // 2007.01.04
	//=============================================================================================================
	//=============================================================================================================
	PathDo = _SCH_CLUSTER_Get_PathDo( side , pt );
	if ( PathDo >= _SCH_CLUSTER_Get_PathRange( side , pt ) ) return -2;
	//=============================================================================================================
	gres = -1;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		grp_tm[i] = -1;
		grp_tx[i] = 0;
		grp_mv[i] = 0;
		grp_pm[i] = 0;
		grp_st[i] = 0;
		grp_pmo[i] = 0;
		grp_sto[i] = 0;
		grp_go[i] = FALSE;
	}
	//=============================================================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		ch = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , PathDo , i );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ch > 0 ) {
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( j , curch ) == 1 ) {
					//
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( j , ch , G_PLACE ) ) {
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) {
							//
							grp_go[j] = TRUE;
							//
//							if ( ( curch == ch ) || ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) ) { // 2014.01.10
							if ( ( curch == ch ) || ( SCH_PM_IS_ABSENT( ch , side , pt , PathDo ) ) ) { // 2014.01.10
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( ch , FALSE , &tdata , curch ) ) {
									if ( grp_tm[j] == -1 ) {
										grp_tm[j] = tdata;
									}
									else {
										if ( grp_tm[j] > tdata ) {
											grp_tm[j] = tdata;
										}
									}
								}
								else {
									if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
										grp_st[j]++;
									}
									else {
										grp_pm[j]++;
									}
								}
							}
							else {
								//
								if ( !SCH_PM_HAS_PICKING( ch , &pmslot ) ) { // 2014.01.10
									wafer = FALSE;
								}
								else {
									//
//									if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( ch , 0 ) , _SCH_MODULE_Get_PM_POINTER( ch , 0 ) ) == SCHEDULER_RUNNING2 ) { // 2014.01.10
									if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( ch , pmslot ) , _SCH_MODULE_Get_PM_POINTER( ch , pmslot ) ) == SCHEDULER_RUNNING2 ) { // 2014.01.10
										wafer = FALSE;
									}
									else {
										wafer = TRUE;
									}
									//
								}
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( ch , wafer , &tdata , curch ) ) {
									if ( !_SCH_PRM_GET_MODULE_STOCK( ch ) ) {
										if ( grp_tm[j] == -1 ) {
											grp_tm[j] = tdata;
										}
										else {
											if ( grp_tm[j] > tdata ) {
												grp_tm[j] = tdata;
											}
										}
									}
								}
								else {
									if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
										grp_sto[j]++;
									}
									else {
										grp_pmo[j]++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//--------------------------------------------
	Result = 0;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( !grp_go[i] ) continue;
		Result++;
	}
	if ( Result <= 1 ) return -3;
	//--------------------------------------------
	Result = 0;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( !grp_go[i] ) continue;
		//
		ttm = 0;
		tx = 0;
		st = 0;
		pm = 0;
		//
		for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
			ttm2 = FALSE;
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( i , j ) ) {
//				if ( _SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) { // 2006.03.27
				if ( ( ( i == TMATM ) && ( j == arm ) ) || ( _SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) ) { // 2006.03.27
					tx++;
				}
				else {
					s2 = _SCH_MODULE_Get_TM_SIDE( i , j );
					p2 = _SCH_MODULE_Get_TM_POINTER( i , j );
					if      ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) == ( PathDo + 1 ) ) {
						// go same
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							ch = SCHEDULER_CONTROL_Get_PathProcessChamber( s2 , p2 , PathDo , k );
							//-------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
							//-------------------------------------------------------------------------------------
							if ( ch > 0 ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , ch , G_PLACE ) ) {
									if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , s2 ) ) {
										//
//										if ( ( curch == ch ) || ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) ) { // 2014.01.10
										if ( ( curch == ch ) || ( SCH_PM_IS_ABSENT( ch , s2 , p2 , PathDo ) ) ) { // 2014.01.10
											//
											if ( !_SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( ch , FALSE , &tdata , curch ) ) {
												if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
													st++;
													ttm2 = TRUE;
												}
												else {
//													pm++;
													tx++;
												}
											}
										}
									}
								}
							}
						}
					}
					else if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) <  ( PathDo + 1 ) ) {
						// go curr
					}
					else if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) >  ( PathDo + 1 ) ) {
						// go next
						if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) > _SCH_CLUSTER_Get_PathRange( s2 , p2 ) ) {
							ttm++;
						}
						else {
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								ch = SCHEDULER_CONTROL_Get_PathProcessChamber( s2 , p2 , _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 , k );
								//-------------------------------------------------------------------------------------
								SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
								//-------------------------------------------------------------------------------------
								if ( ch > 0 ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , ch , G_PLACE ) ) {
										if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , s2 ) ) {
											//
//											if ( ( curch == ch ) || ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) ) { // 2014.01.10
											if ( ( curch == ch ) || ( SCH_PM_IS_ABSENT( ch , s2 , p2 , _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 ) ) ) { // 2014.01.10
												//
												if ( !_SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( ch , FALSE , &tdata , curch ) ) {
													if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
														st++;
													}
													else {
														pm++;
													}
													ttm2 = TRUE;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			if ( ttm2 ) ttm++;
		}
		//====================================================================
		grp_mv[i] = ttm;
		grp_tx[i] = tx + grp_mv[i];
		//
		if      ( pm > 0 ) {
			grp_pm[i] = grp_pm[i] - 1;
		}
		else if ( st > 0 ) {
			grp_st[i] = grp_st[i] - 1;
		}
		//
		if ( ( grp_tx[i] >= 2 ) && ( grp_mv[i] == 0 ) ) Result++;
		//====================================================================
	}
	if ( Result >= 2 ) {
		gres = -1;
		Result = 0;
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( ( grp_mv[i] != 0 ) || ( grp_pm[i] <= 0 ) || ( grp_tx[i] < 2 ) ) continue;
			//
			if ( gres == -1 ) {
				gres = i;
			}
			else {
				//=================================================================================================
				// 2006.06.14
				//=================================================================================================
//				if      ( _SCH_PRM_GET_Getting_Priority( i + TM ) < _SCH_PRM_GET_Getting_Priority( gres + TM ) ) {
//					Result = 1;
//					gres = i;
//				}
//				else if ( _SCH_PRM_GET_Getting_Priority( i + TM ) > _SCH_PRM_GET_Getting_Priority( gres + TM ) ) {
//					Result = 1;
//				}
				//=================================================================================================
				if ( grp_tm[ gres ] == grp_tm[ i ] ) {
					if      ( _SCH_PRM_GET_Getting_Priority( i + TM ) < _SCH_PRM_GET_Getting_Priority( gres + TM ) ) {
						Result = 1;
						gres = i;
					}
					else if ( _SCH_PRM_GET_Getting_Priority( i + TM ) > _SCH_PRM_GET_Getting_Priority( gres + TM ) ) {
						Result = 1;
					}
				}
				else {
					Result = 1;
					if ( grp_tm[ gres ] > grp_tm[ i ] ) {
						gres = i;
					}
				}
				//=================================================================================================
			}
		}
		if ( Result == 0 ) gres = -1;
	}
	sn = 1;
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//----------------------------------------------------------------
			if ( !grp_go[i] ) continue;
			//----------------------------------------------------------------
			if ( i == 0 ) grp_tx[i]--;
			//
			if ( grp_pmo[i] > 0 ) grp_tx[i] = grp_tx[i] - grp_pmo[i];
			//
			if ( grp_tx[i] <= 0 ) continue;
			//----------------------------------------------------------------
			if ( grp_pm[i] > 0 ) {
				if ( gres < 0 ) {
					gres = i;
				}
				else {
					if      ( grp_pm[i] > grp_pm[gres] ) {
						gres = i;
					}
					else if ( grp_pm[i] == grp_pm[gres] ) {
						if      ( grp_tm[i] < grp_tm[gres] ) {
							gres = i;
						}
						else if ( grp_tm[i] == grp_tm[gres] ) {
							if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
								gres = i;
							}
						}
					}
				}
			}
		}
		sn = 2;
	}
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//----------------------------------------------------------------
			if ( !grp_go[i] ) continue;
			//----------------------------------------------------------------
			if ( i == 0 ) grp_tx[i]++;
			//----------------------------------------------------------------
			if ( grp_tx[i] <= 0 ) continue;
			//----------------------------------------------------------------
			if ( ( grp_pm[i] + grp_st[i] ) > 0 ) {
				if ( gres < 0 ) {
					gres = i;
				}
				else {
					if      ( ( grp_pm[i] + grp_st[i] ) > ( grp_pm[gres] + grp_st[gres] ) ) {
						gres = i;
					}
					else if ( ( grp_pm[i] + grp_st[i] ) == ( grp_pm[gres] + grp_st[gres] ) ) {
						if      ( grp_tm[i] < grp_tm[gres] ) {
							gres = i;
						}
						else if ( grp_tm[i] == grp_tm[gres] ) {
							if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
								gres = i;
							}
						}
					}
				}
			}
		}
		sn = 3;
	}
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//----------------------------------------------------------------
			if ( !grp_go[i] ) continue;
			//----------------------------------------------------------------
			if ( grp_tx[i] >= 2 ) {
				if ( gres < 0 ) {
					gres = i;
				}
				else {
					//====================================================================================================
					// 2006.06.02
					//====================================================================================================
//					if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
//						gres = i;
//					}
					//====================================================================================================
					if ( grp_tm[ gres ] == grp_tm[ i ] ) {
						if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
							gres = i;
						}
					}
					else {
						if ( grp_tm[ gres ] > grp_tm[ i ] ) {
							gres = i;
						}
					}
					//====================================================================================================
				}
			}
		}
		sn = 4;
	}
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//----------------------------------------------------------------
			if ( !grp_go[i] ) continue;
			//----------------------------------------------------------------
			if ( ( grp_pm[i] + grp_st[i] ) > 0 ) {
				if ( gres < 0 ) {
					gres = i;
				}
				else {
					if      ( ( grp_pm[i] + grp_st[i] ) > ( grp_pm[gres] + grp_st[gres] ) ) {
						gres = i;
					}
					else if ( ( grp_pm[i] + grp_st[i] ) == ( grp_pm[gres] + grp_st[gres] ) ) {
						if      ( grp_tm[i] < grp_tm[gres] ) {
							gres = i;
						}
						else if ( grp_tm[i] == grp_tm[gres] ) {
							if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
								gres = i;
							}
						}
					}
				}
			}
		}
		sn = 5;
	}
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//----------------------------------------------------------------
			if ( !grp_go[i] ) continue;
			//----------------------------------------------------------------
			if ( grp_tm[i] >= 0 ) {
				if ( gres < 0 ) {
					gres = i;
				}
				else {
					if      ( grp_tm[i] < grp_tm[gres] ) {
						gres = i;
					}
					else if ( grp_tm[i] == grp_tm[gres] ) {
						if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
							gres = i;
						}
					}
				}
			}
		}
		sn = 6;
	}
	//--------------------------------------------
	if ( gres < 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( !grp_go[i] ) continue;
			if ( gres < 0 ) {
				gres = i;
			}
			else {
				if ( _SCH_PRM_GET_Getting_Priority( gres + TM ) > _SCH_PRM_GET_Getting_Priority( i + TM ) ) {
					gres = i;
				}
			}
		}
		sn = 7;
	}
	//=============================================================================================================
	if ( gres >= 0 ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			ch = SCHEDULER_CONTROL_Get_PathProcessChamber( side , pt , PathDo , i );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ch > 0 ) {
				if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( gres , ch , G_PICK ) ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , PathDo , i );
				}
			}
		}
		//
		strcpy( Buffer , "" );
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( grp_go[i] ) {
				sprintf( Buffer2 , "[%d;%d,%d,%d,%d]" , i , grp_tx[i] , grp_pm[i] , grp_st[i] , grp_tm[i] );
				strcat( Buffer , Buffer2 );
			}
		}
		//
		if ( TMATM == 0 ) {
//			_SCH_LOG_LOT_PRINTF( side , "TM Handling Remake Multi Group %s(%d) to [%d] with %d:%s%cWHTMMG PM%d:%d:%d:%d:%s%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , gres , sn , Buffer , 9 , curch - PM1 + 1 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , gres , sn , Buffer , 9 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) ); // 2006.05.28
			_SCH_LOG_LOT_PRINTF( side , "TM Handling Remake Multi Group %s(%d) to [%d] with %d:%s%cWHTMMG PM%d:%d:%d:%d:%s%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , gres , sn , Buffer , 9 , curch - PM1 + 1 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , gres , sn , Buffer , 9 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) ); // 2006.05.28
		}
		else {
//			_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Remake Multi Group %s(%d) to [%d] with %d:%s%cWHTM%dMG PM%d:%d:%d:%d:%s%c%d\n" , TMATM + 1 , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , gres , sn , Buffer , 9 , TMATM + 1 , curch - PM1 + 1 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , gres , sn , Buffer , 9 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) ); // 2006.05.28
			_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Remake Multi Group %s(%d) to [%d] with %d:%s%cWHTM%dMG PM%d:%d:%d:%d:%s%c%d\n" , TMATM + 1 , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , gres , sn , Buffer , 9 , TMATM + 1 , curch - PM1 + 1 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , gres , sn , Buffer , 9 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) ); // 2006.05.28
		}
		//
		//======================================================================================
	}
	else {
		if ( TMATM == 0 ) {
//			_SCH_LOG_LOT_PRINTF( side , "TM Handling Remake Multi Group %s(%d) Rejected%cWHTMMGR PM%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , 9 , curch - PM1 + 1 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , 9 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) ); // 2006.05.28
			_SCH_LOG_LOT_PRINTF( side , "TM Handling Remake Multi Group %s(%d) Rejected%cWHTMMGR PM%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , 9 , curch - PM1 + 1 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , 9 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) ); // 2006.05.28
		}
		else {
//			_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Remake Multi Group %s(%d) Rejected%cWHTM%dMGR PM%d:%d%c%d\n" , TMATM + 1 , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , 9 , TMATM + 1 , curch - PM1 + 1 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) , 9 , _SCH_MODULE_Get_PM_WAFER( curch , 0 ) ); // 2006.05.28
			_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Remake Multi Group %s(%d) Rejected%cWHTM%dMGR PM%d:%d%c%d\n" , TMATM + 1 , _SCH_SYSTEM_GET_MODULE_NAME( curch ) , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , 9 , TMATM + 1 , curch - PM1 + 1 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) , 9 , _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) ); // 2006.05.28
		}
		return -4;
	}
	//=============================================================================================================
	return 0;
}




void SCHEDULING_CHECK_Chk_BM_HAS_IN_ANOTHER_WAFER_for_DOUBLE( int ch , int side , int s0 , int *side2 , int *pointer , int *s1 , BOOL swmode , BOOL doubleslotfixmode , BOOL oddpick , BOOL xcr ) {
	int i , no = -1 , s , offset;
	*s1 = 0;
	s = s0;
	*side2 = side; // 2007.04.18
	if ( swmode ) { // 2006.02.17
		//
		while( TRUE ) { // 2007.02.22
			if ( oddpick ) offset = 2; // 2007.11.04
			else           offset = 1; // 2007.11.04
			if ( ( s + offset ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , s + offset ) > 0 ) { // 2007.02.22
//					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2007.04.18
//						if ( _SCH_MODULE_Get_BM_SIDE( ch , s + offset ) != side ) return; // 2007.04.18
//					} // 2007.04.18
					if ( xcr ) { // 2013.02.07
						if ( _SCH_MODULE_Get_BM_STATUS( ch , s + offset ) == BUFFER_INWAIT_STATUS ) return;
					}
					else {
						if ( _SCH_MODULE_Get_BM_STATUS( ch , s + offset ) == BUFFER_OUTWAIT_STATUS ) return;
					}
//					if ( _SCH_MODULE_Get_BM_WAFER( ch , s + offset ) <= 0 ) return; // 2007.02.22
					//
					//==================================================================================================
					// 2006.08.04
					//==================================================================================================
//					if ( _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , s ) ) <= 0 ) { // 2007.03.27
					if ( _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , s0 ) ) <= 0 ) { // 2007.03.27
//						if ( _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , s + offset ) ) > 0 ) return; // 2008.07.17
						if ( _SCH_CLUSTER_Get_PathRange( _SCH_MODULE_Get_BM_SIDE( ch , s + offset ) , _SCH_MODULE_Get_BM_POINTER( ch , s + offset ) ) > 0 ) return; // 2008.07.17
					}
					else {
//						if ( _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , s + offset ) ) <= 0 ) return; // 2008.07.17
						if ( _SCH_CLUSTER_Get_PathRange( _SCH_MODULE_Get_BM_SIDE( ch , s + offset ) , _SCH_MODULE_Get_BM_POINTER( ch , s + offset ) ) <= 0 ) return; // 2008.07.17
					}
					//==================================================================================================
					*s1 = s + offset;
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2007.05.17
						*side2 = _SCH_MODULE_Get_BM_SIDE( ch , 1 );
					}
					else {
						*side2 = _SCH_MODULE_Get_BM_SIDE( ch , s + offset ); // 2007.04.18
					}
					*pointer = _SCH_MODULE_Get_BM_POINTER( ch , s + offset );
					break;
				}
				else { // 2007.02.22
					if ( doubleslotfixmode ) return;
					if ( oddpick ) return; // 2007.11.04
					s++;
				}
			}
			else { // 2007.02.22
				break;
			}
		}
		//
		//---------------------------------------------------------------------------------------
		//
		// 2017.07.13
		//
		//---------------------------------------------------------------------------------------
		//
		if ( *s1 == 0 ) {
			//
			s = s0;
			//
			while( TRUE ) { // 2007.02.22
				if ( oddpick ) offset = 2; // 2007.11.04
				else           offset = 1; // 2007.11.04
				if ( ( s - offset ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , s - offset ) > 0 ) { // 2007.02.22
						if ( xcr ) { // 2013.02.07
							if ( _SCH_MODULE_Get_BM_STATUS( ch , s - offset ) == BUFFER_INWAIT_STATUS ) return;
						}
						else {
							if ( _SCH_MODULE_Get_BM_STATUS( ch , s - offset ) == BUFFER_OUTWAIT_STATUS ) return;
						}
						//
						//==================================================================================================
						if ( _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , s0 ) ) <= 0 ) { // 2007.03.27
							if ( _SCH_CLUSTER_Get_PathRange( _SCH_MODULE_Get_BM_SIDE( ch , s - offset ) , _SCH_MODULE_Get_BM_POINTER( ch , s - offset ) ) > 0 ) return; // 2008.07.17
						}
						else {
							if ( _SCH_CLUSTER_Get_PathRange( _SCH_MODULE_Get_BM_SIDE( ch , s - offset ) , _SCH_MODULE_Get_BM_POINTER( ch , s - offset ) ) <= 0 ) return; // 2008.07.17
						}
						//==================================================================================================
						*s1 = s - offset;
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2007.05.17
							*side2 = _SCH_MODULE_Get_BM_SIDE( ch , 1 );
						}
						else {
							*side2 = _SCH_MODULE_Get_BM_SIDE( ch , s - offset ); // 2007.04.18
						}
						*pointer = _SCH_MODULE_Get_BM_POINTER( ch , s - offset );
						break;
					}
					else { // 2007.02.22
						if ( doubleslotfixmode ) return;
						if ( oddpick ) return; // 2007.11.04
						s--;
					}
				}
				else { // 2007.02.22
					break;
				}
			}
		}
		//
		//
		//
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
//			if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) { // 2007.06.28
//				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) { // 2013.02.07
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				//
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
					if ( xcr ) continue; // 2013.02.07
				}
				else if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) { // 2013.02.07
					if ( !xcr ) continue; // 2013.02.07
				}
				else { // 2013.02.07
					continue;
				}
				//
				if ( s != i ) {
					if ( no == -1 ) {
						no = _SCH_MODULE_Get_BM_WAFER( ch , i );
						*side2 = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2007.06.28
						*s1 = i;
						*pointer = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
					else {
//							if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) < no ) { // 2008.12.09
						if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < _SCH_CLUSTER_Get_SupplyID( *side2 , *pointer ) ) { // 2008.12.09
							no = _SCH_MODULE_Get_BM_WAFER( ch , i );
							*side2 = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2007.06.28
							*s1 = i;
							*pointer = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
					}
				}
			}
//			} // 2007.06.28
		}
	}
}


int SCHEDULING_CHECK_Get_BM_FREE_IN_CHAMBER_FOR_PICK_BM( int side , int TMATM ) {
	int i , j , k , c , m;
	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
		if ( _SCH_MODULE_Get_BM_WAFER( Get_Prm_MODULE_BUFFER_SINGLE_MODE() , 1 ) <= 0 ) {
			return Get_Prm_MODULE_BUFFER_SINGLE_MODE();
		}
	}
	else {
		c = 0; // 2007.03.07
		m = -1;
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) == BUFFER_IN_MODE ) {
					if ( c == 1 ) return -1;
					c++;
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( i , &j , &k , 0 ) > 0 ) {
	//						return i; // 2007.03.07
						if ( m == -1 ) m = i; // 2007.03.07
					}
				}
			}
		}
		return m; // 2007.03.07
	}
	return -1;
}
//=======================================================================================
int SCHEDULING_CHECK_for_Check_PLACE_Deadlock( int TMATM , int CKSIDE ) {
	int i , m , ChkTrg , Side , Pointer;
	int p , ok , allwafer; // 2010.12.03

	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return -1; // 2010.12.06

	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) allwafer = 0; // 2010.12.03
	else allwafer = 1; // 2010.12.03

	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
//		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 )		return -1; // 2010.12.03
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 )		continue; // 2010.12.03
		//
		/*
		// 2014.07.17
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2007.01.15
			Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
			if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) % 100 ) > 0 ) {
				Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
			}
			else {
				Pointer = _SCH_MODULE_Get_TM_POINTER2( TMATM,i );
			}
		}
		else {
			Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
			Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
		}
		//
		if ( Pointer >= 100 ) return -2;
		*/
		//
		// 2014.07.17
		Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
		Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
		//
		if ( ( Side < 0 ) || ( Side >= MAX_CASSETTE_SIDE ) ) return -2;
		if ( ( Pointer < 0 ) || ( Pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return -2;
		//
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM , i ) == SCHEDULER_MOVING_OUT ) return -3;
		else if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) > _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return -4;

		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , m );
			//-------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
			//-------------------------------------------------------------------------------------
			if ( ChkTrg > 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
//						if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) return -5; // 2014.01.10
						if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ) ) return -5; // 2014.01.10
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) return -7;
					}
				}
				else { // 2014.06.26
					return -21;
				}
			}
		}
		//
		if ( allwafer == 0 ) { // 2010.12.03 next can not go
			//
			ok = TRUE;
			//
			for ( p = _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 ; p < _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ; p++ ) {
				//
				for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
					//
					ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , p , m );
					//-------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
					//-------------------------------------------------------------------------------------
					if ( ChkTrg > 0 ) {
//						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) { // 2010.12.07
							if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
								//
								break;
								//
							}
						}
//					}
				}
				//
				if ( m == MAX_PM_CHAMBER_DEPTH ) {
					ok = FALSE;
					break;
				}
				//
			}
			//
			if ( ok ) return -8;
			//
		}
		//
	}

	ChkTrg = FALSE;

	/*
	// 2014.07.17
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2007.01.15
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) % 100 ) > 0 ) {
				Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
				Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
				//
				if ( allwafer == 0 ) { // 2010.12.03 next can not go
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
					_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
					ChkTrg = TRUE;
				}
				else {
					if ( _SCH_CASSETTE_LAST_RESULT_GET( _SCH_CLUSTER_Get_PathIn(Side,Pointer) , _SCH_CLUSTER_Get_SlotIn(Side,Pointer) ) == -2 ) {
	//					_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN ); // 2008.09.19
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
						_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
						ChkTrg = TRUE;
					}
				}
			}
			if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) / 100 ) > 0 ) {
				Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
				Pointer = _SCH_MODULE_Get_TM_POINTER2( TMATM,i );
				//
				if ( allwafer == 0 ) { // 2010.12.03 next can not go
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
					_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
					ChkTrg = TRUE;
				}
				else {
					if ( _SCH_CASSETTE_LAST_RESULT_GET( _SCH_CLUSTER_Get_PathIn(Side,Pointer) , _SCH_CLUSTER_Get_SlotIn(Side,Pointer) ) == -2 ) {
	//					_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN ); // 2008.09.19
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
						_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
						ChkTrg = TRUE;
					}
				}
				//
			}
		}
	}
	else {
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 ) continue; // 2010.12.06
			//
			Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
			Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
			//
			if ( allwafer == 0 ) { // 2010.12.03 next can not go
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
				_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
				ChkTrg = TRUE;
			}
			else {
				if ( _SCH_CASSETTE_LAST_RESULT_GET( _SCH_CLUSTER_Get_PathIn(Side,Pointer) , _SCH_CLUSTER_Get_SlotIn(Side,Pointer) ) == -2 ) {
	//				_SCH_CLUSTER_Set_PathDo( Side , Pointer , PATHDO_WAFER_RETURN ); // 2008.09.19
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 ); // 2008.09.19
					_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
					ChkTrg = TRUE;
				}
			}
		}
	}
	*/
	//
	// 2014.07.17
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 ) continue;
		//
		Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
		Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
		//
		if ( allwafer == 0 ) { // next can not go
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 );
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( Side , Pointer , FALSE , TRUE , 32 ); // 2016.02.19
			//
			_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
			ChkTrg = TRUE;
		}
		else {
			if ( _SCH_CASSETTE_LAST_RESULT2_GET( Side,Pointer ) == -2 ) {
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 );
				//
				SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( Side , Pointer , FALSE , TRUE , 33 ); // 2016.02.19
				//
				_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
				ChkTrg = TRUE;
			}
		}
		//
		if ( ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) % 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) / 100 ) > 0 ) ) {
			//
			Side    = _SCH_MODULE_Get_TM_SIDE2( TMATM,i );
			Pointer = _SCH_MODULE_Get_TM_POINTER2( TMATM,i );
			//
			if ( allwafer == 0 ) { // next can not go
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 );
				//
				SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( Side , Pointer , FALSE , TRUE , 34 ); // 2016.02.19
				//
				_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
				ChkTrg = TRUE;
			}
			else {
				if ( _SCH_CASSETTE_LAST_RESULT2_GET( Side,Pointer ) == -2 ) {
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( Side , Pointer , -1 );
					//
					SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( Side , Pointer , FALSE , TRUE , 35 ); // 2016.02.19
					//
					_SCH_MODULE_Set_TM_TYPE( TMATM , i , SCHEDULER_MOVING_OUT );
					ChkTrg = TRUE;
				}
			}
			//
		}
	}
	//
	//
	if ( ChkTrg ) return 0;
	return -11;
}


BOOL SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( int TMATM , int side , int arm , int Next_Chamber ) {
	int s , p , i , k , ch;
	if ( TMATM == 0 ) return TRUE;
	if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM - 1 , Next_Chamber , G_PLACE ) ) return TRUE;

	//=================================================================================================================================================
	// 2006.07.13
	//=================================================================================================================================================
	if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) return TRUE;
	//=================================================================================================================================================
	//
//	if ( SCHEDULER_Get_MULTI_GROUP_PLACE( Next_Chamber ) != -1 ) return FALSE; // 2006.06.14
	if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( Next_Chamber ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( Next_Chamber ) != TMATM ) ) return FALSE; // 2006.06.14
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM - 1 , i ) ) {
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM - 1 , i ) > 0 ) {
				s = _SCH_MODULE_Get_TM_SIDE( TMATM - 1 , i );
				p = _SCH_MODULE_Get_TM_POINTER( TMATM - 1 , i );
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= _SCH_CLUSTER_Get_PathRange( s , p ) ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
						//-------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ch ); // 2007.10.29
						//-------------------------------------------------------------------------------------
						if ( ch > 0 ) {
							if ( ch != Next_Chamber ) {
								if ( !_SCH_PRM_GET_MODULE_PLACE_GROUP( TMATM - 1 , ch ) ) {
									return TRUE;
								}
//								if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) { // 2014.01.10
								if ( SCH_PM_IS_ABSENT( ch , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) { // 2014.01.10
									if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
//										if ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) == -1 ) { // 2006.06.14
										if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) == -1 ) || ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) == TMATM ) ) { // 2006.06.14
											return TRUE;
										}
									}
									else {
										return TRUE;
									}
								}
							}
						}
					}
				}
				else {
					if ( ( TMATM - 1 ) == 0 ) return TRUE;
				}
			}
			else {
				return TRUE;
			}
		}
	}
	return FALSE;
}



BOOL SCHEDULER_CONTROL_Next_All_Stock_and_Nothing_to_Go( int TMATM , int Side , int Pointer , int Chamber ) { // 2006.04.11
	int k , ChkTrg , c;
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) >= 2 ) return FALSE;
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return FALSE;
	for ( k = 0 , c = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ChkTrg > 0 ) {
			c++; // 2006.07.10
			if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) return FALSE;
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , ChkTrg , G_PLACE ) ) return FALSE;
		}
	}
	if ( c == 0 ) return FALSE; // 2006.07.10
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( int tmatm , int Side , int Pointer , int mode ) { // 2006.04.11
	int k , ChkTrg , r = TRUE;
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return TRUE;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ChkTrg > 0 ) {
			if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , ChkTrg , G_PLACE ) ) {
						r = FALSE;
					}
				}
			}
			else {
				if ( mode == 1 ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Disable( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
				}
			}
		}
		else if ( ChkTrg < 0 ) { // 2006.04.14
			if ( !_SCH_PRM_GET_MODULE_STOCK( -ChkTrg ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( -ChkTrg , FALSE , Side ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmatm , -ChkTrg , G_PLACE ) ) {
						r = FALSE;
						if ( mode == 1 ) {
							_SCH_CLUSTER_Set_PathProcessChamber_Enable( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
						}
					}
				}
			}
		}
	}
	return r;
}

//===================================================================================================================================
// 2006.04.14
//===================================================================================================================================

/*
BOOL SCHEDULER_CONTROL_Stock_Other_Next_Disable_Before_Pick_PM( int tmatm , int Side , int Pointer ) { // 2006.04.15
	int k , ChkTrg;
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) >= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return TRUE;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
		if ( ChkTrg > 0 ) {
			if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Disable( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
			}
		}
	}
	return TRUE;
}
*/

int SCHEDULER_CONTROL_Special_Case_Checking( int mode , int tmside , int ch , int Side , int Pointer , int offset , int *stock , BOOL placecheck ) {
	int k , i , x , ocd , ChkTrg , ts , tp , xm[MAX_TM_CHAMBER_DEPTH] , lwafer , wfrsts;
	int styes = FALSE;
	int pmslot;

	for ( x = 0 ; x < MAX_TM_CHAMBER_DEPTH ; x++ ) xm[x] = FALSE;
	//-------------------------------------
	// Next Check
	//-------------------------------------
	lwafer = FALSE;
	*stock = 0;
	ocd = 0;
	i = 0;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - offset , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ChkTrg > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmside , ChkTrg , G_PLACE ) ) {
					if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , ChkTrg ) ) { // 2007.03.20
						if ( _SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
							styes = TRUE;
						}
						else {
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
								i++;
								for ( x = 0 ; x < MAX_TM_CHAMBER_DEPTH ; x++ ) {
									if ( x != tmside ) {
										if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( x , ChkTrg ) ) xm[x] = TRUE;
									}
								}
								//
//								wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ); // 2014.01.10
								if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - offset ) || ( !SCH_PM_HAS_PICKING( ChkTrg , &pmslot ) ) ) { // 2014.01.10
									wfrsts = 0;
								}
								else {
									wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , pmslot );
								}
								//
								if ( placecheck ) { // 2006.06.14
									if ( ( wfrsts <= 0 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != tmside ) ) wfrsts = 99;
								}
								else {
									if ( ( wfrsts <= 0 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) ) wfrsts = 99;
								}
								//
								if ( wfrsts > 0 ) {
									if ( mode == 0 ) ocd++;
								}
								else {
									if ( mode != 0 ) ocd++;
								}
							}
						}
					}
				}
			}
		}
	}
	//-------------------------------------
	// Curr Check
	//-------------------------------------
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 - offset , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		//=============================================================
		// 2006.05.24
		//=============================================================
		if ( ChkTrg < 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( -ChkTrg , FALSE , Side ) ) {
				ChkTrg = -ChkTrg;
			}
		}
		//=============================================================
		if ( ChkTrg > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , Side ) ) {
				if ( _SCH_SUB_Run_Impossible_Condition( Side , Pointer , ChkTrg ) ) { // 2007.03.20
					if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
						if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tmside , ChkTrg , G_PLACE ) ) {
							for ( x = 0 ; x < MAX_TM_CHAMBER_DEPTH ; x++ ) {
								if ( xm[x] ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( x , ChkTrg , G_PLACE ) ) {
										//
//										wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ); // 2014.01.10
										if ( SCH_PM_IS_ABSENT( ChkTrg , Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 - offset ) || ( !SCH_PM_HAS_PICKING( ChkTrg , &pmslot ) ) ) { // 2014.01.10
											wfrsts = 0;
										}
										else {
											wfrsts = _SCH_MODULE_Get_PM_WAFER( ChkTrg , pmslot );
										}
										//
										if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
											if ( placecheck ) { // 2006.06.14
												if ( ( wfrsts <= 0 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != x ) ) wfrsts = 99;
											}
											else {
												if ( ( wfrsts <= 0 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ChkTrg ) != -1 ) ) wfrsts = 99;
											}
										}
										//
										if ( wfrsts > 0 ) {
											if ( mode == 0 ) {
												ocd++;
											}
											else {
//												if ( _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( ChkTrg ) >= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , ChkTrg ) ) { // 2006.05.04
												if ( wfrsts != 99 ) { // 2006.05.30
													if ( _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( ChkTrg ) >= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , ChkTrg ) ) { // 2006.05.04
														lwafer = TRUE;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//-------------------------------------------------------------------
	if ( mode == 1 ) {
		if ( lwafer ) return FALSE;
		if ( ocd >= 2 ) return TRUE;
		if ( ocd == 0 ) return FALSE;
		lwafer = FALSE;
	}
	//-------------------------------------------------------------------
	for ( x = 0 ; x < MAX_TM_CHAMBER_DEPTH ; x++ ) {
		if ( xm[x] ) {
			for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
				if ( _SCH_MODULE_Get_TM_WAFER( x , i ) <= 0 )	continue;
				ts = _SCH_MODULE_Get_TM_SIDE( x , i );
				tp = _SCH_MODULE_Get_TM_POINTER( x , i );
				//
				ocd++;
				//
				if ( tp < 100 ) {
					if ( _SCH_CLUSTER_Get_PathRange( ts , tp ) > ( _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 ) ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( ts , tp , _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 , k );
							//-------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
							//-------------------------------------------------------------------------------------
							if ( ChkTrg > 0 ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmside , ChkTrg , G_PLACE ) ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( x , ChkTrg , G_PLACE ) ) {
										lwafer = TRUE;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if ( mode == 0 ) {
		if ( ocd == 0 ) {
			*stock = 2;
			return TRUE;
		}
		if ( ( ocd == 1 ) && ( lwafer ) ) {
			*stock = 3;
			return TRUE;
		}
		if ( styes ) {
			*stock = 1;
			return TRUE;
		}
	}
	else {
		if ( !lwafer ) return TRUE;
	}
	return FALSE;
}


//===================================================================================================================================

BOOL SCHEDULING_CHECK_OTHER_GROUP_HAS_CLEAR_POSSIBLE( int TMATM , int Chamber , int Side , int Pointer , int nex ) { // 2006.05.25
	int t , i , k , ok , w , ChkTrg , ts , tp;

	ok = FALSE;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) , k );
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
		//-------------------------------------------------------------------------------------
		if ( ChkTrg > 0 ) {
			if ( nex == 1 ) {
				if ( !_SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
					ChkTrg = -1;
				}
			}
			else if ( nex >= 2 ) {
				if ( _SCH_PRM_GET_MODULE_STOCK( ChkTrg ) ) {
					ChkTrg = -1;
				}
			}
		}
		if ( ChkTrg > 0 ) {
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) {
				ok = TRUE;
				break;
			}
		}
	}

	if ( !ok ) return FALSE;

	for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
		if ( t != TMATM ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( t , Chamber , G_PLACE ) ) {
				if ( ( _SCH_MODULE_Get_TM_WAFER( t , TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( t , TB_STATION ) > 0 ) ) {
					ok = 0;
					w = 0;
					for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
						if ( _SCH_MODULE_Get_TM_WAFER( t , i ) <= 0 )	continue;
						ts = _SCH_MODULE_Get_TM_SIDE( t , i );
						tp = _SCH_MODULE_Get_TM_POINTER( t , i );
						//
						if ( tp < 100 ) {
							if ( _SCH_CLUSTER_Get_PathRange( ts , tp ) > ( _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 ) ) {
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( ts , tp , _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 , k );
									//-------------------------------------------------------------------------------------
									SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &ChkTrg ); // 2007.10.29
									//-------------------------------------------------------------------------------------
									if ( ChkTrg > 0 ) {
										w++;
										if ( ChkTrg == Chamber ) ok++;
									}
								}
							}
						}
					}
					if ( ( ok > 0 ) && ( w == ok ) ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
void SCHEDULING_CHECK_MULTI_PLACE_GROUP_RESET( int tmside , int ch ) { // 2006.06.14
	int i;
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( i == ch ) continue;
		if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
			if ( SCHEDULER_Get_MULTI_GROUP_PLACE( i ) == tmside ) {
				SCHEDULER_Set_MULTI_GROUP_PLACE( i , -1 );
			}
		}
	}
}
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================

BOOL SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( int TMATM , int Finger ) {
	int k , Side , Pointer;

	Side = _SCH_MODULE_Get_TM_SIDE( TMATM,Finger );
	Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,Finger );
	if ( Pointer >= 100 ) return FALSE;

	if ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) return FALSE;
	if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) > _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) return FALSE;

	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		if ( SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ) > 0 ) {
			if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ) ) return TRUE;
		}
	}
	return FALSE;
}
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
/*
//
// 2018.11.29
//
void SCHEDULING_CHECK_PRE_END_RUN( int TMATM , int CHECKING_SIDE ) { // 2006.07.20
	int i , j , k , pmc;
	int Sch_Ch_End_Chk[MAX_CASSETTE_SIDE][MAX_PM_CHAMBER_DEPTH];

	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if      ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
			Sch_Ch_End_Chk[CHECKING_SIDE][i-PM1] = 3;
		}
		else if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
			Sch_Ch_End_Chk[CHECKING_SIDE][i-PM1] = 3;
		}
		else {
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( TMATM , i ) ) {
//				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2009.07.12
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) { // 2009.07.12
					Sch_Ch_End_Chk[CHECKING_SIDE][i-PM1] = 0;
				}
				else {
					Sch_Ch_End_Chk[CHECKING_SIDE][i-PM1] = 2;
				}
			}
			else {
				Sch_Ch_End_Chk[CHECKING_SIDE][i-PM1] = 3;
			}
		}
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
		//
		for ( j = 0 ; j < MAX_CLUSTER_DEPTH ; j++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , i , j , 0 ) != 0 ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				pmc = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
				//
				if ( pmc > 0 ) Sch_Ch_End_Chk[CHECKING_SIDE][pmc-PM1] = 4;
			}
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( Sch_Ch_End_Chk[CHECKING_SIDE][i] == 0 ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( i + PM1 , 0 ) <= 0 ) { // 2014.01.10
			if ( SCH_PM_IS_EMPTY( i + PM1 ) ) { // 2014.01.10
				if ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) { // 2007.06.08
					_SCH_PREPOST_Pre_End_Part( CHECKING_SIDE , i + PM1 ); // 2007.06.08
				}
				else {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i + PM1 ) <= 0 ) {
						_SCH_PREPOST_Pre_End_Part( CHECKING_SIDE , i + PM1 );
					}
				}
			}
			else { // 2018.11.27
				_SCH_PREPOST_Pre_End_Part( CHECKING_SIDE , ( i + PM1 ) + 1000 );
			}
		}
		//
	}
	//
}
*/


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// SCHEDULING_CHECK_PICK_FROM_CM_POSSIBLE_FOR_SWITCHMESSAGE
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULING_CHECK_PICK_FROM_CM_POSSIBLE_FOR_SWITCHMESSAGE( int side ) { // 2004.10.12
	int R_Curr_Chamber , R_Next_Chamber , R_Next_Move , R_Path_Order , R_NextFinger , R_ArmIntlks;
	//
	if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_CM( side , &R_Curr_Chamber , &R_Path_Order ) < 0 ) return FALSE;
	//
	if ( !KPLT0_CHECK_CASSETTE_TIME_SUPPLY( _SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() ) ) return FALSE;
	if ( _SCH_MODULE_Need_Do_Multi_TAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) return FALSE;
	if ( SCHEDULING_CHECK_for_Enable_PICK_from_CM( 0 , side , &R_Curr_Chamber , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , &R_NextFinger , &R_ArmIntlks , FALSE , TRUE , FALSE ) <= 0 ) return FALSE;
//
	if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2005.01.14
		if ( SCHEDULING_CHECK_CM_Another_No_More_Supply( side , _SCH_MODULE_Get_TM_DoPointer( side ) ) ) {
			return FALSE;
		}
	}
	return TRUE;
}
//
int SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() { // 2006.12.05
	int k , ck , l;
	l = 0;
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
			ck = FALSE;
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
				ck = TRUE;
			}
			if ( ck ) {
				l = SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k ) + l;
			}
		}
	}
	return l;
}

BOOL SCHEDULING_CHECK_Chk_OTHERGROUPBM_WAITING_HASRETURN( int side , int tmside , BOOL only , int ch , int swmd ) { // 2018.05.08
	int i , k;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( only ) {
			if ( i != ch ) continue;
		}
		else {
			if ( i == ch ) continue;
		}
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != (tmside+1) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tmside , i , G_PICK , G_MCHECK_ALL ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( i ) ; k++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i,k ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i,k ) == BUFFER_OUTWAIT_STATUS ) return TRUE;
			//
		}
		//
	}
	//
	return FALSE;
	//
}


BOOL SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( int side , int tmside , int ch , int swmd , BOOL jmpmode , int cntarmminus , BOOL bmonly ) { // 2006.12.06
	int i , k;
	int GetChamber;
	int GetSlot;
	int NextChamber;
	int MovingType;
	int MovingData;
	int MovingOrder;
	int schpt;
	int JumpMode;
	int halfpick;
	int Result;
	//
	int b_outch , b_ptorder , b_type , b_side , b_pointer ,b_side2 , b_pointer2 , b_wafer;
	//
	if ( !bmonly ) { // 2007.05.29
		if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( ch ) ) return TRUE;
	}
	else { // 2007.05.30
		if ( swmd == 0 ) return TRUE;
		if ( ( swmd == 2 ) && ( tmside != 0 ) ) return TRUE;
	}
	//
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		if ( _SCH_SYSTEM_USING_GET(k) < 10 ) continue;
		if ( !bmonly ) {
			if ( jmpmode ) {
				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					b_outch    = _SCH_MODULE_Get_TM_OUTCH( tmside , cntarmminus );
					b_ptorder  = _SCH_MODULE_Get_TM_PATHORDER( tmside , cntarmminus );
					b_type     = _SCH_MODULE_Get_TM_TYPE( tmside , cntarmminus );
					b_side     = _SCH_MODULE_Get_TM_SIDE( tmside , cntarmminus );
					b_side2    = _SCH_MODULE_Get_TM_SIDE2( tmside , cntarmminus );
					b_pointer  = _SCH_MODULE_Get_TM_POINTER( tmside , cntarmminus );
					b_pointer2 = _SCH_MODULE_Get_TM_POINTER2( tmside , cntarmminus );
					b_wafer    = _SCH_MODULE_Get_TM_WAFER( tmside , cntarmminus );
					//
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , 0 );
				}
				//==============================================================
				Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( tmside , k , ch , &GetChamber , &GetSlot , &NextChamber , &MovingType , &MovingOrder , 0 , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &schpt , &JumpMode , TRUE , FALSE , TRUE , -1 , -1 , -1 , -1 , -1 , -1 , -1 ); /* 2013.11.12 */

//				_SCH_LOG_LOT_PRINTF( side , "TM%d SysLog with A.[S=%d][swmd(what)=%d,%d][GetCh=%d][NextCh=%d][MovType=%d][schpt=%d][Jmp=%d][Result=%d]%c\n" , tmside + 1 , k , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , GetChamber , NextChamber , MovingType , schpt , JumpMode , Result , 9 );

				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					_SCH_MODULE_Set_TM_OUTCH( tmside , cntarmminus , b_outch );
					_SCH_MODULE_Set_TM_PATHORDER( tmside , cntarmminus , b_ptorder );
					_SCH_MODULE_Set_TM_TYPE( tmside , cntarmminus , b_type );
//					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side , 0 ); // 2008.07.18
					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side2 , b_pointer2 ); // 2008.07.18
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , b_wafer );
				}
				//==============================================================
				if ( Result > 0 ) {
					if ( GetChamber != ch ) {
						if ( NextChamber != 0 ) {
							if ( NextChamber == ch ) return FALSE;
							if ( _SCH_PRM_GET_MODULE_GROUP( NextChamber ) > tmside ) {
								return FALSE;
							}
						}
					}
					if ( SCHEDULER_MOVING_OUT == MovingType ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( tmside , ch ) == BUFFER_OUT_MODE ) {
							return FALSE;
						}
					}
				}
			}
			else {
				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					b_outch   = _SCH_MODULE_Get_TM_OUTCH( tmside , cntarmminus );
					b_ptorder = _SCH_MODULE_Get_TM_PATHORDER( tmside , cntarmminus );
					b_type    = _SCH_MODULE_Get_TM_TYPE( tmside , cntarmminus );
					b_side    = _SCH_MODULE_Get_TM_SIDE( tmside , cntarmminus );
					b_side2    = _SCH_MODULE_Get_TM_SIDE2( tmside , cntarmminus );
					b_pointer = _SCH_MODULE_Get_TM_POINTER( tmside , cntarmminus );
					b_pointer2 = _SCH_MODULE_Get_TM_POINTER2( tmside , cntarmminus );
					b_wafer   = _SCH_MODULE_Get_TM_WAFER( tmside , cntarmminus );
					//
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , 0 );
				}
				//==============================================================
				Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( tmside , k , ch , &GetChamber , &GetSlot , &NextChamber , &MovingType , &MovingOrder , 0 , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &schpt , &JumpMode , FALSE , TRUE , TRUE , -1 , -1 , -1 , -1 , -1 , -1 , -1 ); /* 2013.11.12 */

//				_SCH_LOG_LOT_PRINTF( side , "TM%d SysLog with B.[S=%d][swmd(what)=%d,%d][GetCh=%d][NextCh=%d][MovType=%d][schpt=%d][Jmp=%d][Result=%d]%c\n" , tmside + 1 , k , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , GetChamber , NextChamber , MovingType , schpt , JumpMode , Result , 9 );

				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					_SCH_MODULE_Set_TM_OUTCH( tmside , cntarmminus , b_outch );
					_SCH_MODULE_Set_TM_PATHORDER( tmside , cntarmminus , b_ptorder );
					_SCH_MODULE_Set_TM_TYPE( tmside , cntarmminus , b_type );
//					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side , 0 ); // 2008.07.18
					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side2 , b_pointer2 ); // 2008.07.18
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , b_wafer );
				}
				//==============================================================
				if ( Result > 0 ) {
					if ( GetChamber != ch ) {
						if ( NextChamber != 0 ) {
							if ( NextChamber == ch ) return FALSE;
							if ( _SCH_PRM_GET_MODULE_GROUP( NextChamber ) > tmside ) {
								return FALSE;
							}
						}
					}
					if ( SCHEDULER_MOVING_OUT == MovingType ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( tmside , ch ) == BUFFER_OUT_MODE ) {
							return FALSE;
						}
					}
				}
			}
			if ( cntarmminus == -1 ) {
				for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
					Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( tmside , k , i , &NextChamber , &GetSlot , &MovingType , &MovingOrder , _SCH_PRM_GET_MODULE_MODE( FM ) , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( tmside ) , &schpt , -1 );
					if ( Result > 0 ) {
						if ( NextChamber == ch ) return FALSE;
						if ( SCHEDULER_MOVING_OUT == MovingType ) {
							if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( tmside , ch ) == BUFFER_OUT_MODE ) {
								return FALSE;
							}
						}
					}
				}
			}
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tmside , i , G_PICK ) ) continue;
				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					b_outch   = _SCH_MODULE_Get_TM_OUTCH( tmside , cntarmminus );
					b_ptorder = _SCH_MODULE_Get_TM_PATHORDER( tmside , cntarmminus );
					b_type    = _SCH_MODULE_Get_TM_TYPE( tmside , cntarmminus );
					b_side    = _SCH_MODULE_Get_TM_SIDE( tmside , cntarmminus );
					b_side2    = _SCH_MODULE_Get_TM_SIDE2( tmside , cntarmminus );
					b_pointer = _SCH_MODULE_Get_TM_POINTER( tmside , cntarmminus );
					b_pointer2 = _SCH_MODULE_Get_TM_POINTER2( tmside , cntarmminus );
					b_wafer   = _SCH_MODULE_Get_TM_WAFER( tmside , cntarmminus );
					//
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , 0 );
				}
				//==============================================================
				Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( tmside , k , i , -1 , &MovingType , &MovingOrder , &NextChamber , _SCH_PRM_GET_MODULE_MODE( FM ) , TRUE , &GetSlot , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &schpt , &MovingData , &JumpMode , &halfpick , 0 );
				//==============================================================
				// 2007.02.05
				//==============================================================
				if ( cntarmminus != -1 ) {
					_SCH_MODULE_Set_TM_OUTCH( tmside , cntarmminus , b_outch );
					_SCH_MODULE_Set_TM_PATHORDER( tmside , cntarmminus , b_ptorder );
					_SCH_MODULE_Set_TM_TYPE( tmside , cntarmminus , b_type );
//					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side , 0 ); // 2008.07.18
					_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side2 , b_pointer2 ); // 2008.07.18
					_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , b_wafer );
				}
				//==============================================================
				if ( Result > 0 ) {
					if ( NextChamber > 0 ) {
						if ( NextChamber == ch ) return FALSE;
						if ( _SCH_PRM_GET_MODULE_GROUP( NextChamber ) > tmside ) {
							return FALSE;
						}
					}
					else {
						if ( SCHEDULER_MOVING_OUT == MovingType ) {
							if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( tmside , ch ) == BUFFER_OUT_MODE ) {
								return FALSE;
							}
						}
					}
				}
			}
		}
		else {
			//===========================================================================================
			// 2007.05.29
			//===========================================================================================
			if ( cntarmminus != -1 ) {
				b_outch   = _SCH_MODULE_Get_TM_OUTCH( tmside , cntarmminus );
				b_ptorder = _SCH_MODULE_Get_TM_PATHORDER( tmside , cntarmminus );
				b_type    = _SCH_MODULE_Get_TM_TYPE( tmside , cntarmminus );
				b_side    = _SCH_MODULE_Get_TM_SIDE( tmside , cntarmminus );
				b_side2   = _SCH_MODULE_Get_TM_SIDE2( tmside , cntarmminus );
				b_pointer = _SCH_MODULE_Get_TM_POINTER( tmside , cntarmminus );
				b_pointer2= _SCH_MODULE_Get_TM_POINTER2( tmside , cntarmminus );
				b_wafer   = _SCH_MODULE_Get_TM_WAFER( tmside , cntarmminus );
				//
				_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , 0 );
			}
			//==============================================================
//			Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( tmside , k , &GetChamber , &GetSlot , &NextChamber , &MovingType , &MovingOrder , 0 , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &schpt , &JumpMode , FALSE , FALSE , TRUE );
			Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( tmside , k , ch , &GetChamber , &GetSlot , &NextChamber , &MovingType , &MovingOrder , 2 , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &schpt , &JumpMode , FALSE , FALSE , TRUE , -1 , -1 , -1 , -1 , -1 , -1 , -1 ); /* 2013.11.12 */

//			_SCH_LOG_LOT_PRINTF( side , "TM%d SysLog with C.[S=%d][swmd(what)=%d,%d][Ch:GetCh=%d:%d][NextCh=%d][MovType=%d][schpt=%d][Jmp=%d][Result=%d]%c\n" , tmside + 1 , k , swmd , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , ch , GetChamber , NextChamber , MovingType , schpt , JumpMode , Result , 9 );

			//==============================================================
			// 2007.02.05
			//==============================================================
			if ( cntarmminus != -1 ) {
				_SCH_MODULE_Set_TM_OUTCH( tmside , cntarmminus , b_outch );
				_SCH_MODULE_Set_TM_PATHORDER( tmside , cntarmminus , b_ptorder );
				_SCH_MODULE_Set_TM_TYPE( tmside , cntarmminus , b_type );
//				_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side , 0 ); // 2008.07.18
				_SCH_MODULE_Set_TM_SIDE_POINTER( tmside , cntarmminus , b_side , b_pointer , b_side2 , b_pointer2 ); // 2008.07.18
				_SCH_MODULE_Set_TM_WAFER( tmside , cntarmminus , b_wafer );
			}
			//==============================================================
			if ( Result > 0 ) {
				if ( GetChamber == ch ) return FALSE;
			}
			//===========================================================================================
		}
	}
	return TRUE;
}



//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_TM_WAFER_FINISH( int tmside , int arm ) { // 2008.09.26
	int s , p;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tmside , arm ) <= 0 ) return FALSE;
	s = _SCH_MODULE_Get_TM_SIDE( tmside , arm );
	p = _SCH_MODULE_Get_TM_POINTER( tmside , arm );
	//-------------------------------------------------------------------------------------------
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE;
	//-------------------------------------------------------------------------------------------
	return FALSE;
}
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
int SCHEDULER_CONTROL_Chk_BM_TARGET_PM_HAS_SPACE( int tmside , int ch , int slot ) { // 2012.07.25
	int i , m , s , p;
	//
	if ( ( ch < BM1 ) || ( ch >= TM ) ) return -1;
	//
	if ( _SCH_MODULE_Get_BM_WAFER( ch , slot ) <= 0 ) return -2;
	//
	s = _SCH_MODULE_Get_BM_SIDE( ch , slot );
	p = _SCH_MODULE_Get_BM_POINTER( ch , slot );
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) > 0 ) return -3;
	//
	if ( _SCH_CLUSTER_Get_PathRange( s , p ) <= 0 ) return -4;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , i );
		//
		if ( m <= 0 ) continue;
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
		if ( SCH_PM_IS_ABSENT( m , s , p , 0 ) ) { // 2014.01.10
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) return 1;
		}
	}
	//
	return -9;
}
//
int SCHEDULER_CONTROL_Chk_BM_TARGET_PM_HAS_WFR_POST( int tmside , int ch , int slot ) { // 2012.07.11
	int i , m , k , s , p , ps , pp , curdo , swposs , npost , freespace;
	int pmslot;
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) {
//
// BM에서 Pick 하려고 할때
//
//
// 0) 1S에서 TM에 Return Wafer가 있을때 <PICK 수행>
//
	//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2014.01.28
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
				//
				if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( tmside , FALSE ) ) return -21;
				//
			}
		}
		//
		if ( _SCH_MODULE_Get_BM_WAFER( ch , slot ) <= 0 ) return -1;
//
// 1) BM에 Pick할 Wafer가 없을때 <PICK 수행> *
//
		s = _SCH_MODULE_Get_BM_SIDE( ch , slot );
		p = _SCH_MODULE_Get_BM_POINTER( ch , slot );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) > 0 ) return -2;
		if ( _SCH_CLUSTER_Get_PathRange( s , p ) <= 0 ) return -3;
		//
//
// 2) BM에서 Pick할 Wafer의 PathDo,PathRange 값이 올바르지 않을 때(PM Path를 아무것도 진행하지 않은것이 아닐때) <PICK 수행> *
//
		swposs = FALSE;
		npost = FALSE; // 2012.07.26
		freespace = FALSE; // 2012.07.27
		//
//
//			swposs		: [처음 Path의 PM이 Wafer를 같고 있을때 그 PM의 Wafer가  Post Process를 가지고 있을때 TRUE]
//			npost		: [BM에서 Pick할 Wafer가 Post Process를 가지고 있지 않을때 TRUE (X)]
//			freespace	: [처음 Path의 PM이 Wafer를 같고 있지 않고 처음 Path의 PM이 Processing중이 아닐때 TRUE]
//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , i );
			//
			if ( m <= 0 ) continue;
			//
//			if ( !_SCH_CLUSTER_Check_HasProcessData_Post( s , p , 0 , i ) ) return -11; // 2012.07.26
			if ( !_SCH_CLUSTER_Check_HasProcessData_Post( s , p , 0 , i ) ) npost = TRUE; // 2012.07.26
			//
//			if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
			//
			if ( SCH_PM_IS_ABSENT( m , s , p , 0 ) || ( !SCH_PM_HAS_PICKING( m , &pmslot ) ) ) { // 2014.01.10
//				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) return -12; // 2012.07.27
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) freespace = TRUE; // 2012.07.27
			}
			else {
				//
//				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) { // 2012.07.27
					//
//					ps = _SCH_MODULE_Get_PM_SIDE( m , 0 ); // 2014.01.10
//					pp = _SCH_MODULE_Get_PM_POINTER( m , 0 ); // 2014.01.10
					//
					ps = _SCH_MODULE_Get_PM_SIDE( m , pmslot ); // 2014.01.10
					pp = _SCH_MODULE_Get_PM_POINTER( m , pmslot ); // 2014.01.10
					//
					if ( ( ps >= 0 ) && ( ps < MAX_CASSETTE_SIDE ) ) {
						if ( ( pp >= 0 ) && ( pp < MAX_CASSETTE_SLOT_SIZE ) ) {
							//
							curdo = _SCH_CLUSTER_Get_PathDo( ps , pp ) - 1;
							//
							if ( ( curdo >= 0 ) && ( curdo < _SCH_CLUSTER_Get_PathRange( ps , pp ) ) ) {
								//
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									if ( SCHEDULER_CONTROL_Get_PathProcessChamber( ps , pp , curdo , k ) == m ) {
										if ( _SCH_CLUSTER_Check_HasProcessData_Post( ps , pp , curdo , k ) ) break;
									}
								}
								//
//								if ( k == MAX_PM_CHAMBER_DEPTH ) swposs = TRUE; // 2012.07.24
								if ( k != MAX_PM_CHAMBER_DEPTH ) swposs = TRUE; // 2012.07.24
								//
							}
						}
					}
					//
//				}
			}
		}
		//
		if ( ( _SCH_MODULE_Get_TM_WAFER( tmside,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tmside,TB_STATION ) <= 0 ) ) {
//			if ( swposs ) return -13; // 2012.07.24
			if ( !swposs ) return -13; // 2012.07.24
			if ( freespace ) return -14; // 2012.07.27
//
// 3) TM에 Wafer를 가지고 있지 않을때 처음 Path의 PM이 Wafer를 같고 있지 않고 처음 Path의 PM이 Processing중이 아닐때 <PICK 수행> 
//
		}
		//
//		if ( !swposs && npost ) return -14; // 2012.07.26 // 2012.07.31
		if ( !swposs ) return -14; // 2012.07.26 // 2012.07.31
		//
//
// 4) 처음 Path의 PM이 Wafer를 같고 있지 않거나 그 PM의 Wafer가 Post Process를 가지고 있지 않을때 <PICK 수행> 
//
//
// 5) 그 외는 BM에서 PICK 하지 못하도록 함
//
		return 1;
	}
	else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
//
// PM에서 Pick 하려고 할때
//
		if ( ( _SCH_MODULE_Get_TM_WAFER( tmside,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tmside,TB_STATION ) <= 0 ) ) return -41;
//
// 1) TM에 Wafer를 가지고 있지 않을때 <PICK 수행>
//
//		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) return -42; // 2014.01.10
		if ( _SCH_MODULE_Get_PM_WAFER( ch , slot ) <= 0 ) return -42; // 2014.01.10
//
// 2) PM에 Pick할 Wafer가 없을때 <PICK 수행> *
//
//		s = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); // 2014.01.10
//		p = _SCH_MODULE_Get_PM_POINTER( ch , 0 ); // 2014.01.10
		//
		s = _SCH_MODULE_Get_PM_SIDE( ch , slot ); // 2014.01.10
		p = _SCH_MODULE_Get_PM_POINTER( ch , slot ); // 2014.01.10
		//
		if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return -51;
		if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return -52;
//
// 3) PM에서 Pick 하려는 Wafer의 Side/Pointer 값이 올바르지 않을 때 <PICK 수행> *
//
		i = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
		//
		if ( ( i < 0 ) || ( i >= _SCH_CLUSTER_Get_PathRange( s , p ) ) ) return -53;
//
// 4) PM에서 Pick 하려는 Wafer의 다음 Path가 없을때(현재 PM이 마지막 Path의 PM일때) <PICK 수행>
//
		swposs = 0; // 2014.01.14
		npost = FALSE; // 2014.01.14
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k ) == ch ) {
//				if ( _SCH_CLUSTER_Check_HasProcessData_Post( s , p , i , k ) ) break; // 2014.01.14
				if ( _SCH_CLUSTER_Check_HasProcessData_Post( s , p , i , k ) ) npost = TRUE; // 2014.01.14
				swposs++; // 2014.01.14
			}
		}
		//
//		if ( k == MAX_PM_CHAMBER_DEPTH ) return -54; // 2014.01.14
		if ( !npost ) return -54; // 2014.01.14
//
// 5) PM에서 Pick 하려는 Wafer가 Post Process를 가지고 있지 않을때 <PICK 수행>
//
		if ( swposs <= 1 ) return -55; // 2014.01.14
		//
//
// 5-1) PM에서 Pick 하려는 Wafer의 다른 Parllel 이 존재하지 않을 때 <PICK 수행>
//
		//
		i++;
		//
		if ( ( i < 0 ) || ( i >= _SCH_CLUSTER_Get_PathRange( s , p ) ) ) return -61;
//
// 6) PM에서 Pick 하려는 Wafer의 다음 Path가 있을때 다음 Path가 마지막 Path의 PM일때 <PICK 수행>
//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k );
			//
			if ( m > 0 ) {
				//
				if ( m == ch ) return -62;
				//
//
// 7) 다음 Path의 PM이 현재 PM과 같을때 <PICK 수행>
//
//				if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.11
				if ( SCH_PM_IS_ABSENT( m , s , p , i ) ) { // 2014.01.10
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) return -63;
//
// 8) 다음 Path의 PM이 현재 PM과 같지 않고 다음 Path의 PM이 Wafer를 같고 있지 않고 다음 Path의 PM이 Processing중이 아닐때 <PICK 수행>
//
				}
				//
			}
		}
		//
//
// 9) 그 외는 PM에서 PICK 하지 못하도록 함
//
		return 2;
		//
	}
	return -99;
}

//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
int SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( int tmside , BOOL retcheck , BOOL tmchk , int ch ) { // 2007.08.29
	int m , j , s , p , a;
	//
	if ( retcheck ) { // 2008.02.04
		//-------------------------------------------------------------------------------------------
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return 1;
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) return 2;
		//-------------------------------------------------------------------------------------------
		if ( ( ch < BM1 ) || ( ch >= TM ) ) return 3;
		//-------------------------------------------------------------------------------------------
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_OUTWAIT_STATUS ) > 0 ) return 4;
		//-------------------------------------------------------------------------------------------
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_INWAIT_STATUS ) <= 0 ) return 5;
		//-------------------------------------------------------------------------------------------
		if      ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) {
			s = _SCH_MODULE_Get_TM_SIDE( tmside , TA_STATION );
			p = _SCH_MODULE_Get_TM_POINTER( tmside , TA_STATION );
			a = TA_STATION;
		}
		else if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) {
			s = _SCH_MODULE_Get_TM_SIDE( tmside , TB_STATION );
			p = _SCH_MODULE_Get_TM_POINTER( tmside , TB_STATION );
			a = TB_STATION;
		}
		else {
			return 6;
		}
		//-------------------------------------------------------------------------------------------
		if ( tmchk ) {
			//-------------------------------------------------------------------------------------------
			if (
				( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) &&
				( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 )
				) {
				return 7;
			}
			//-------------------------------------------------------------------------------------------
			if ( ( p < 0 ) || ( p >= 100 ) ) return 8;
			//-------------------------------------------------------------------------------------------
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= _SCH_CLUSTER_Get_PathRange( s , p ) ) return 9;
			//-------------------------------------------------------------------------------------------
		}
		//-------------------------------------------------------------------------------------------
		for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
			//
			if ( m == ch ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( m ) != tmside ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( m ) && ( _SCH_MODULE_Get_BM_FULL_MODE( m ) != BUFFER_TM_STATION ) ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) > 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( m , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( m , BUFFER_INWAIT_STATUS ) > 0 ) continue;
			//
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
				//
				if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
				//
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_SINGLESWAP( m , s , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( m ) ) , &p , &p , _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tmside ) > 0 ) > 0 ) {
						return 0;
					}
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( m , s , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , &j , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( m ) ) , &p , &p , _SCH_MODULE_Get_TM_WAFER( tmside,a ) ) > 0 ) {
						return 0;
					}
				}
			}
			//
		}
	}
	else {
		if ( tmchk ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) return 0; // 2007.10.03
			if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) return 0; // 2007.10.03
		}
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2007.11.28
			for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) { // 2009.07.08
					if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( tmside , m , G_PICK , G_MCHECK_SAME ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( m , -1 ) > 0 ) return 0;
					}
				}
			}
		}
		else {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2007.11.28
				for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( m ) == tmside ) ) {
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( m ) || ( _SCH_MODULE_Get_BM_FULL_MODE( m ) == BUFFER_TM_STATION ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( m , BUFFER_INWAIT_STATUS ) > 0 ) return 0;
						}
					}
				}
			}
		}
		//
	}
	return 99;
}

BOOL SCHEDULING_CHECK_Diff_Path( int s , int p , int src_s , int src_p , BOOL );

BOOL SCHEDULER_CONTROL_Chk_ANOTHER_BM_HAS_NO_PASSING_WAFER( int higher , int src_s , int src_p , int curch , int tmside ) { // 2008.02.25
	int m , i , s , p;
	//-------------------------------------------------------------------------------------------
//	if ( higher != 2 ) { // 2011.01.14 // 2011.02.14
//	if ( higher != 3 ) { // 2011.01.14 // 2011.02.14 // 2015.07.23
	if ( ( higher != 3 ) && ( higher != 6 ) ) { // 2011.01.14 // 2011.02.14 // 2015.07.23
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) return FALSE;
	}
	//-------------------------------------------------------------------------------------------
	if ( ( curch < BM1 ) || ( curch >= TM ) ) return FALSE;
	//-------------------------------------------------------------------------------------------
	for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
		//
		if ( m == curch ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( m , 0 ) ) continue; // 2010.09.28
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( m ) != tmside ) ) continue;
		//
		for ( i = _SCH_PRM_GET_MODULE_SIZE( m ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( m , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( m , i ) == BUFFER_INWAIT_STATUS ) {
					//
					s = _SCH_MODULE_Get_BM_SIDE( m , i );
					p = _SCH_MODULE_Get_BM_POINTER( m , i );
					//
//					if ( !SCHEDULING_CHECK_Diff_Path( s , p , src_s , src_p ) ) { // 2013.12.09 // 2016.11.19
					if ( !SCHEDULING_CHECK_Diff_Path( s , p , src_s , src_p , FALSE ) ) { // 2013.12.09 // 2016.11.19
						//
						if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
							if ( _SCH_CLUSTER_Get_SupplyID( s , p ) > _SCH_CLUSTER_Get_SupplyID( src_s , src_p ) ) continue;
							return TRUE;
						}
						//
					}
					//
				}
			}
		}
		//
	}
	return FALSE;
}

BOOL SCHEDULING_CHECK_Same_Path( int s , int p , int src_s , int src_p ) { // 2008.07.17
	int i , j , k , m , p1[MAX_PM_CHAMBER_DEPTH] , p2[MAX_PM_CHAMBER_DEPTH];
	//
	if ( _SCH_CLUSTER_Get_PathRange( s , p ) != _SCH_CLUSTER_Get_PathRange( src_s , src_p ) ) return FALSE;
	//
	k = _SCH_CLUSTER_Get_PathRange( s , p );
	//
	for ( i = 0 ; i < k ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			p1[j] = 0;
			p2[j] = 0;
		}
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , j );
			//
			if      ( m > 0 ) {
				p1[m-PM1] = 1;
			}
			else if ( m < 0 ) {
				p1[-m-PM1] = 1;
			}
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( src_s , src_p , i , j );
			//
			if      ( m > 0 ) {
				p2[m-PM1] = 1;
			}
			else if ( m < 0 ) {
				p2[-m-PM1] = 1;
			}
			//
		}
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( p1[j] != p2[j] ) return FALSE;
		}
	}
	return TRUE;
}


BOOL SCHEDULING_CHECK_Parallel_Use( int side , int pt , int j , int pm ) { // 2017.03.15
	char selid;
	//
	if ( _SCH_CLUSTER_Get_PathProcessParallel( side , pt , j ) == NULL ) return FALSE;
	//
	selid = _SCH_CLUSTER_Get_PathProcessParallel2( side , pt , j , ( pm - PM1 ) );
	//
	if ( selid <= 1 ) return FALSE;
	//
	return TRUE;
}

//BOOL SCHEDULING_CHECK_Diff_Path( int s , int p , int src_s , int src_p ) { // 2008.09.19 // 2016.11.19
BOOL SCHEDULING_CHECK_Diff_Path( int s , int p , int src_s , int src_p , BOOL LikeRandCheck ) { // 2008.09.19 // 2016.11.19
	int i , j , k , m , pm[MAX_PM_CHAMBER_DEPTH] , pm2[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		pm[i] = FALSE;
		pm2[i] = FALSE; // 2016.11.19
	}
	//
	k = _SCH_CLUSTER_Get_PathRange( s , p );
	//
	if ( LikeRandCheck ) { // 2016.11.19
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) > k ) {
			return TRUE;
		}
	}
	//
	for ( i = 0 ; i < k ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , j );
			//
			if      ( m > 0 ) {
				//
				if ( !SCHEDULING_CHECK_Parallel_Use( s , p , i , m ) ) { // 2017.03.15
					pm[m-PM1] = TRUE;
				}
				//
			}
			else if ( m < 0 ) {
				switch( _SCH_MODULE_Get_Mdl_Use_Flag( s , -m ) ) { // 2014.01.02
				case MUF_97_USE_to_SEQDIS_RAND :
				case MUF_98_USE_to_DISABLE_RAND :
					break;
				default :
					//
					if ( !SCHEDULING_CHECK_Parallel_Use( s , p , i , -m ) ) { // 2017.03.15
						pm[-m-PM1] = TRUE;
					}
					//
					break;
				}
			}
			//
		}
	}
	//
	if ( LikeRandCheck ) { // 2016.11.19
		//
		k = _SCH_CLUSTER_Get_PathRange( src_s , src_p );
		//
//		if ( _SCH_CLUSTER_Get_PathDo( s , p ) > k ) { // 2017.01.11
		if ( _SCH_CLUSTER_Get_PathDo( src_s , src_p ) > k ) { // 2017.01.11
			return TRUE;
		}
		//
		for ( i = 0 ; i < k ; i++ ) {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( src_s , src_p , i , j );
				//
				if      ( m > 0 ) {
					pm2[m-PM1] = TRUE;
				}
				else if ( m < 0 ) {
					switch( _SCH_MODULE_Get_Mdl_Use_Flag( src_s , -m ) ) {
					case MUF_97_USE_to_SEQDIS_RAND :
					case MUF_98_USE_to_DISABLE_RAND :
						break;
					default :
						pm2[-m-PM1] = TRUE;
						break;
					}
					//
				}
				//
			}
		}
		//
		j = 0;
		k = 0;
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i+PM1 ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i+PM1 ) >= 2 ) ) continue;
			//
//			if      ( pm[i+PM1] && !pm2[i+PM1] ) { // 2017.01.11
			if      ( pm[i] && !pm2[i] ) { // 2017.01.11
				//
				if ( SCH_PM_IS_ABSENT( i+PM1 , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i+PM1 ) == 0 ) {
						j++;
					}
				}
				//
			}
//			else if ( pm[i+PM1] && pm2[i+PM1] ) { // 2017.01.11
			else if ( pm[i] && pm2[i] ) { // 2017.01.11
				k++;
			}
			//
		}
		//
		if ( ( k >= 0 ) && ( j == 0 ) ) return FALSE;
		//
	}
	else {
		//
		k = _SCH_CLUSTER_Get_PathRange( src_s , src_p );
		//
		for ( i = 0 ; i < k ; i++ ) {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( src_s , src_p , i , j );
				//
				if      ( m > 0 ) {
					//
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) ) continue; // 2009.04.08
					//
					if ( pm[m-PM1] ) return FALSE;
				}
				else if ( m < 0 ) {
					//
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) ) continue; // 2009.04.08
					//
					switch( _SCH_MODULE_Get_Mdl_Use_Flag( src_s , -m ) ) { // 2014.01.02
					case MUF_97_USE_to_SEQDIS_RAND :
					case MUF_98_USE_to_DISABLE_RAND :
						break;
					default :
						if ( pm[-m-PM1] ) return FALSE;
						break;
					}
					//
				}
				//
			}
		}
	}
	//
	return TRUE;
}

BOOL SCHEDULING_CHECK_Have_Again_Path( int s , int p , int pd0 ) { // 2009.11.05
	int i , j , m , pm[MAX_PM_CHAMBER_DEPTH] , pd;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		pm[i] = 0;
	}
	//
	if ( pd0 == -1 ) {
		pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
	}
	else {
		pd = pd0;
	}
	//
	for ( i = pd; i < _SCH_CLUSTER_Get_PathRange( s , p ) ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , j );
			//
			if      ( m > 0 ) {
				if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) < 2 ) ) {
					if ( pm[m-PM1] > 0 ) return TRUE;
					pm[m-PM1]++;
				}
			}
			else if ( m < 0 ) {
				if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) < 2 ) ) {
					//
					switch( _SCH_MODULE_Get_Mdl_Use_Flag( s , -m ) ) { // 2014.01.02
					case MUF_97_USE_to_SEQDIS_RAND :
					case MUF_98_USE_to_DISABLE_RAND :
						break;
					default :
						if ( pm[-m-PM1] > 0 ) return TRUE;
						pm[-m-PM1]++;
						break;
					}
					//
				}
			}
			//
		}
	}
	//
	return FALSE;
}

BOOL SCHEDULING_CHECK_Have_Next_Path( int pm_s , int pm_p , int bm_s , int bm_p ) { // 2011.01.05
	int i , j , m;
	int i2 , j2 , m2 , x;
	//
	int pm_do , pm_rng;
	int bm_rng;
	//
	pm_do  = _SCH_CLUSTER_Get_PathDo( pm_s , pm_p );
	pm_rng = _SCH_CLUSTER_Get_PathRange( pm_s , pm_p );
	//
	if ( pm_do >= pm_rng ) return FALSE;
	//
	bm_rng = _SCH_CLUSTER_Get_PathRange( bm_s , bm_p );
	//
	for ( i = 0 ; i < bm_rng ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( bm_s , bm_p , i , j );
			//
			if      ( m > 0 ) {
				if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) ) continue;
			}
			else if ( m < 0 ) {
				if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) ) continue;
				//
				m = -m;
				//
			}
			else {
				continue;
			}
			//
			x = pm_do + i - 1;	if ( x < 0 ) x = 0;
			//
			for ( i2 = x ; i2 < pm_rng ; i2++ ) {
				//
				for ( j2 = 0 ; j2 < MAX_PM_CHAMBER_DEPTH ; j2++ ) {
					//
					m2 = SCHEDULER_CONTROL_Get_PathProcessChamber( pm_s , pm_p , i2 , j2 );
					//
					if      ( m2 > 0 ) {
						if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m2 ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m2 ) >= 2 ) ) continue;
					}
					else if ( m2 < 0 ) {
						if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m2 ) >= 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m2 ) >= 2 ) ) continue;
						//
						m2 = -m2;
						//
					}
					else {
						continue;
					}
					//
					if ( m == m2 ) return TRUE;
					//
				}
			}
			//
		}
	}
	//
	return FALSE;
}


BOOL SCHEDULING_CHECK_INCLUDING_for_NOPASS( int src_s , int src_p , int s , int p , int pmmode , int doff ) { // 2009.01.19
	int i , k , pr , pch[2][MAX_PM_CHAMBER_DEPTH] , trg;

//	pr = _SCH_CLUSTER_Get_PathRange( s , p );  // 2010.09.28
	//
	if ( _SCH_CLUSTER_Get_PathRange( src_s , src_p ) > _SCH_CLUSTER_Get_PathRange( s , p ) )
		pr = _SCH_CLUSTER_Get_PathRange( s , p );
	else 
		pr = _SCH_CLUSTER_Get_PathRange( src_s , src_p );
	//
	for ( i = 0 ; i < pr ; i++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) pch[0][k] = pch[1][k] = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			trg = SCHEDULER_CONTROL_Get_PathProcessChamber( src_s , src_p , i , k );
			if ( trg > 0 ) pch[0][trg-PM1] = TRUE;
			trg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k );
			if ( trg > 0 ) pch[1][trg-PM1] = TRUE;
		}
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			if ( pch[0][k] != pch[1][k] ) return FALSE;
		}
	}
	//=====================================================================
	// 2010.02.26
	//=====================================================================
	if ( pmmode != 2 ) { // 2010.09.28
		if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) != _SCH_CLUSTER_Get_ClusterIndex( src_s , src_p ) ) {
			if ( _SCH_CLUSTER_Get_PathRun( s , p , 16 , 2 ) < 20 ) {
				if ( _SCH_CLUSTER_Get_PathRun( src_s , src_p , 16 , 2 ) < 20 ) {
					return TRUE;
				}
			}
		}
	}

	// dummy
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) pch[0][k] = pch[1][k] = FALSE;

	for ( i = _SCH_CLUSTER_Get_PathDo( src_s , src_p ) ; i < _SCH_CLUSTER_Get_PathRange( src_s , src_p ) ; i++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			trg = SCHEDULER_CONTROL_Get_PathProcessChamber( src_s , src_p , i , k );
			if ( trg > 0 ) pch[0][trg-PM1] = TRUE;
		}
	}
	for ( i = _SCH_CLUSTER_Get_PathDo( s , p ) + doff ; i < _SCH_CLUSTER_Get_PathRange( s , p ) ; i++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			trg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k );
			if ( trg > 0 ) pch[1][trg-PM1] = TRUE;
		}
	}
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		if ( pch[0][k] == pch[1][k] ) return FALSE;
	}
	//
	return TRUE;
}

//int SCHEDULING_CHECK_Before_Invalid_Order( int higher , int Pickch , int s , int p , int src_s , int src_p , int ch , int swp , int pmmode , int dooff ) { // 2010.09.28 // 2013.12.18
int SCHEDULING_CHECK_Before_Invalid_Order( int higher , int Pickch , int s , int p , int src_s , int src_p , int ch , int swp , int pmmode , int dooff , BOOL tagcheckskip ) { // 2010.09.28 // 2013.12.18
	int i , k , v , trg , subchk;
	int noxchk; // 2017.07.07
	//
//printf( "[XX] [1] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
//	if ( SCHEDULING_CHECK_Diff_Path( s , p , src_s , src_p ) ) return 0; // 2016.11.19
	if ( SCHEDULING_CHECK_Diff_Path( s , p , src_s , src_p , FALSE ) ) return 0; // 2016.11.19
	//
//printf( "[XX] [2] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
//	if ( higher == 1 ) { // 2009.11.05 // 2011.02.14
	if ( ( higher == 1 ) || ( higher == 2 ) ) { // 2009.11.05 // 2011.02.14
//printf( "[XX] [3] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		if ( swp == 0 ) { // 2009.12.26
//printf( "[XX] [4] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
			if ( SCHEDULING_CHECK_Have_Again_Path( s , p , -1 ) ) return 1;
			//
		}
		//
		subchk = TRUE; // 2011.02.14
		//
		if ( higher == 1 ) { // 2011.02.14
			if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) == _SCH_CLUSTER_Get_ClusterIndex( src_s , src_p ) ) {
				subchk = FALSE;
			}
		}
		//
		if ( subchk ) { // 2011.02.14
			if      ( swp == 0 ) { // 2009.12.26
				if ( SCHEDULING_CHECK_Have_Next_Path( s , p , src_s , src_p ) ) return 2; // 2010.12.30
			}
			else if ( swp == 1 ) { // 2011.01.06
			}
			else if ( swp == 2 ) { // 2011.01.06
				trg = SCHEDULING_CHECK_Have_Again_Path( s , p , 0 );
				if ( !trg ) trg = SCHEDULING_CHECK_Have_Again_Path( src_s , src_p , 0 );
				//
				if ( trg ) {
					if ( SCHEDULING_CHECK_Have_Next_Path( s , p , src_s , src_p ) ) return 3; // 2010.12.30
				}
				//
			}
		}
		//
	}
	//
	if ( !tagcheckskip ) { // 2013.12.18
	//printf( "[XX] [5] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) == _SCH_CLUSTER_Get_ClusterIndex( src_s , src_p ) ) {
			//
			if ( pmmode != 0 ) v = 20;
			else               v = 22;
			//
	//printf( "[XX] [6] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
			if ( _SCH_CLUSTER_Get_PathRun( s , p , 16 , 2 ) < v ) { // 2010.02.16
				if ( _SCH_CLUSTER_Get_PathRun( src_s , src_p , 16 , 2 ) < v ) { // 2010.02.16
					return 0;
				}
			}
			//
		}
		//
	//printf( "[XX] [7] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		if ( SCHEDULING_CHECK_Same_Path( s , p , src_s , src_p ) ) {
	//printf( "[XX] [8] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
			if ( pmmode != 0 ) {
				if ( _SCH_CLUSTER_Get_PathRun( s , p , 16 , 2 ) < 20 ) { // 2010.02.16
					if ( _SCH_CLUSTER_Get_PathRun( src_s , src_p , 16 , 2 ) < 20 ) { // 2010.02.16
						return 0;
					}
				}
			}
			else {
				return 0;
			}
		}
	}
	//
//printf( "[XX] [9] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
	if ( Pickch != -1 ) {
//printf( "[XX] [A] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		if ( _SCH_CLUSTER_Get_SupplyID( s , p ) > _SCH_CLUSTER_Get_SupplyID( src_s , src_p ) ) return 0;
	}
	//
//	if ( higher == 1 ) { // 2011.02.14
	if ( ( higher == 1 ) || ( higher == 2 ) ) { // 2011.02.14
//printf( "[XX] [B] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		//
		if ( SCHEDULING_CHECK_INCLUDING_for_NOPASS( src_s , src_p , s , p , pmmode , dooff ) ) return 0;
		//
//printf( "[XX] [C] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		//
		//
		noxchk = 0; // 2017.07.07
		//
		for ( i = _SCH_CLUSTER_Get_PathDo( s , p ) + dooff ; i < _SCH_CLUSTER_Get_PathRange( s , p ) ; i++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				trg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k );
				if ( trg > 0 ) {
					//
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trg ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , trg ) < 2 ) ) {
						if ( !tagcheckskip ) { // 2017.07.07
							return 4;
						}
						else { // 2017.07.07
							if ( ch != trg ) {
								noxchk = 2;
							}
							else {
								if ( noxchk == 0 ) noxchk = 1;
							}
						}
					}
					//
				}
			}
		}
		//
		if ( tagcheckskip ) { // 2017.07.07
			if ( noxchk == 1 ) return 6;
		}
		//
//printf( "[XX] [D] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
	}
	else {
//printf( "[XX] [E] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
		for ( i = ( _SCH_CLUSTER_Get_PathDo( s , p ) + dooff ) ; i < _SCH_CLUSTER_Get_PathRange( s , p ) ; i++ ) { // 2008.01.28
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				if ( ch == SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k ) ) return 5;
			}
		}
	}
//printf( "[XX] [F] [higher=%d][Pickch=%d][s=%d][p=%d][src_s=%d][src_p=%d][ch=%d][swp=%d][pmmode=%d][dooff=%d]\n" , higher , Pickch , s , p , src_s , src_p , ch , swp , pmmode , dooff );
	return 0;
}


//int SCHEDULING_CHECK_Before_Order_USE_for_This( int higher , int src_s , int src_p , int Pickch , int ch ) { // 2013.12.18
int SCHEDULING_CHECK_Before_Order_USE_for_This( int higher , int src_s , int src_p , int Pickch , int ch , BOOL tagcheckskip ) { // 2013.12.18
	int i , m , t , s , p;
	int swp = 0 , Res;
	int pmslot;
	//
	if ( higher >= 4 ) { // 2015.07.23
		//
		higher = higher - 3; // 2015.07.23
		//
		swp = 1; // 2015.06.25
	}
	//
	if ( Pickch == -1 ) { // 2010.09.28
		swp = 2;
	}
	else {
		if ( ( Pickch >= BM1 ) && ( Pickch < TM ) ) { // 2009.12.26
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
//					swp = 1; // 2011.02.22
					swp = 2; // 2011.02.22
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
					swp = 1;
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
					swp = 1;
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
					swp = 1;
				}
				else {
				}
			}
		}
	}
	//
	if ( Pickch == -1 ) { // 2010.09.26
		//
		for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( m , 0 ) ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( m ) != 0 ) ) continue;
			//
			for ( i = _SCH_PRM_GET_MODULE_SIZE( m ) ; i >= 1 ; i-- ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( m , i ) > 0 ) {
					//
					if ( _SCH_MODULE_Get_BM_STATUS( m , i ) == BUFFER_INWAIT_STATUS ) {
						//
						s = _SCH_MODULE_Get_BM_SIDE( m , i );
						p = _SCH_MODULE_Get_BM_POINTER( m , i );
						//
						if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {

//							Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 2 , 0 ); // 2013.12.18
							Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 2 , 0 , tagcheckskip ); // 2013.12.18

							if ( Res > 0 ) return ( (Res*10) + 1 );

						}
						//
					}
				}
			}
			//
		}
	}
	//
	for ( m = PM1 ; m < ( MAX_PM_CHAMBER_DEPTH + PM1 ); m++ ) {
		//
		if ( Pickch == m ) continue;
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) continue; // 2014.01.10
		//
		if ( !SCH_PM_HAS_ONLY_PICKING( m , &pmslot ) ) continue; // 2014.01.10
		//
//		s = _SCH_MODULE_Get_PM_SIDE( m , 0 ); // 2014.01.10
//		p = _SCH_MODULE_Get_PM_POINTER( m , 0 ); // 2014.01.10
		//
		s = _SCH_MODULE_Get_PM_SIDE( m , pmslot ); // 2014.01.10
		p = _SCH_MODULE_Get_PM_POINTER( m , pmslot ); // 2014.01.10
		//
		if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
		//
//		Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 1 , 0 ); // 2010.09.28 // 2013.12.18
		Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 1 , 0 , tagcheckskip ); // 2010.09.28 // 2013.12.18
		//
		if ( Res > 0 ) return ( (Res*10) + 2 );
		//
	}
	//
	for ( m = 0 ; m < MAX_TM_CHAMBER_DEPTH ; m++ ) {
		for ( t = 0 ; t < 2 ; t++ ) {
			if ( _SCH_MODULE_Get_TM_WAFER( m , t ) <= 0 ) continue;
			s = _SCH_MODULE_Get_TM_SIDE( m , t );
			p = _SCH_MODULE_Get_TM_POINTER( m , t );
			//
//			if ( ( p < 0 ) || ( p >= 100 ) ) continue; // 2015.05.27
			if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) continue; // 2015.05.27
			//
//			Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 0 , -1 ); // 2010.09.28 // 2013.12.18
			Res = SCHEDULING_CHECK_Before_Invalid_Order( higher , Pickch , s , p , src_s , src_p , ch , swp , 0 , -1 , tagcheckskip ); // 2010.09.28 // 2013.12.18

			if ( Res > 0 ) return ( (Res*10) + 3 );
			//
		}
	}
	//
	return 0;
}


//int SCHEDULING_CHECK_NOPASING_PICK_REJECT( int higher , int TMATM , int CHECKING_SIDE , int mode , int PickChamber_Side , int PickSlot_Pointer ) { // 2007.01.15 // 2013.12.18
int SCHEDULING_CHECK_NOPASING_PICK_REJECT( int higher , int TMATM , int CHECKING_SIDE , int mode , int PickChamber_Side , int PickSlot_Pointer , BOOL tagcheckskip ) { // 2007.01.15 // 2013.12.18
	int s , p , i , k , ChkTrg , dt , PickChamber , pd;
	//
	if ( higher == 0 ) return 0;
	//
	if ( mode == 1 ) { // 2011.01.14
//		if ( higher == 2 ) return 0; // 2011.01.14 // 2011.02.14
//		if ( higher == 3 ) return 0; // 2011.01.14 // 2011.02.14 // 2015.07.23
		if ( ( higher == 3 ) || ( higher == 6 ) ) return 0; // 2011.01.14 // 2011.02.14 // 2015.07.23
	} // 2011.01.14
	//
	if ( mode == 0 ) {
		//
		PickChamber = PickChamber_Side;
		//
		if      ( ( PickChamber >= PM1 ) && ( PickChamber < AL ) ) {
			//
			return 0;
			//
//			s = _SCH_MODULE_Get_PM_SIDE( PickChamber , 0 ); // 2014.01.10
//			p = _SCH_MODULE_Get_PM_POINTER( PickChamber , 0 ); // 2014.01.10
			//
			s = _SCH_MODULE_Get_PM_SIDE( PickChamber , PickSlot_Pointer ); // 2014.01.10
			p = _SCH_MODULE_Get_PM_POINTER( PickChamber , PickSlot_Pointer ); // 2014.01.10
			//
			pd = _SCH_CLUSTER_Get_PathDo( s , p );
			//
		}
		else if ( ( PickChamber >= BM1 ) && ( PickChamber < TM ) ) {
			s = _SCH_MODULE_Get_BM_SIDE( PickChamber , PickSlot_Pointer );
			p = _SCH_MODULE_Get_BM_POINTER( PickChamber , PickSlot_Pointer );
			//
			pd = _SCH_CLUSTER_Get_PathDo( s , p );
			//
			if ( ( TMATM != 0 ) && ( pd == 1 ) ) pd = 0; // 2013.11.15
			//
		}
		else {
			return 0;
		}
	}
	else { // 2010.09.28
		//
		PickChamber = -1;
		//
		s = PickChamber_Side;
		p = PickSlot_Pointer;
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		//
	}
	//
	if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
		if ( pd < _SCH_CLUSTER_Get_PathRange( s , p ) ) { // 2008.01.15
			//===================================================================================
			if ( SCHEDULER_CONTROL_Chk_ANOTHER_BM_HAS_NO_PASSING_WAFER( higher , s , p , PickChamber , TMATM ) ) return 1; // 2008.02.25
			//===================================================================================
			//
//			if ( higher != 2 ) { // 2011.01.14 // 2011.02.14
//			if ( higher != 3 ) { // 2011.01.14 // 2011.02.14 // 2015.07.23
			if ( ( higher != 3 ) && ( higher != 6 ) ) { // 2011.01.14 // 2011.02.14 // 2015.07.23
				//
				for ( i = pd ; i < _SCH_CLUSTER_Get_PathRange( s , p ) ; i++ ) {
	//				find = FALSE; // 2008.01.28
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k );
						if ( ChkTrg > 0 ) {
							if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) < 2 ) ) {
	//							find = TRUE; // 2008.01.28
//								dt = SCHEDULING_CHECK_Before_Order_USE_for_This( higher , s , p , PickChamber , ChkTrg ); // 2013.12.18
								dt = SCHEDULING_CHECK_Before_Order_USE_for_This( higher , s , p , PickChamber , ChkTrg , tagcheckskip ); // 2013.12.18
								if ( dt > 0 ) return ( (dt*10) + 2 );
							}
						}
					}
	//				if ( find ) break; // 2008.01.28
				}
			}
			//
		}
	}
	//
	return 0;
}

//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================
// 2002.03.25 // 0->NOT Switch 1->Switch 2->Switch but has Pre Recipe
int SCHEDULING_CHECK_Switch_PLACE_From_TM( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber ) {
	int k , chkch , Pointer , Side , Finger , rets , chk;
	//
	if ( Fingerx == TA_STATION ) Finger = TB_STATION;
	else                         Finger = TA_STATION;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,Finger ) )									return 0;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) <= 0 )				return 0;
	//
	Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,Finger );
	Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,Finger );
	//
	if ( ( PickChamber >= CM1 ) && ( PickChamber < ( CM1 + MAX_CASSETTE_SIDE ) ) ) {
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) {
			if ( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) == PickChamber ) return 1;
			//=============================================================================================================
			// 2005.11.28
			//=============================================================================================================
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
				if ( _SCH_CLUSTER_Get_PathHSide( Side , Pointer ) ) {
					_SCH_CLUSTER_Set_PathOut( Side , Pointer , PickChamber );
					return 1;
				}
			}
			//=============================================================================================================
			return 0;
		}
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
			if ( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) == PickChamber ) return 1;
			//=============================================================================================================
			// 2005.11.28
			//=============================================================================================================
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
				if ( _SCH_CLUSTER_Get_PathHSide( Side , Pointer ) ) {
					_SCH_CLUSTER_Set_PathOut( Side , Pointer , PickChamber );
					return 1;
				}
			}
			//=============================================================================================================
			return 0;
		}
	}
	else if ( ( PickChamber >= PM1 ) && ( PickChamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) return 0;
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) return 0;
		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) <= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			rets = -1; // 2005.10.18
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				chkch = SCHEDULER_CONTROL_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
				//
				//-------------------------------------------------------------------------------------
				SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM( &chkch ); // 2007.10.29
				//-------------------------------------------------------------------------------------
				//
				if ( chkch > 0 ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , chkch , G_PLACE ) ) {
						//================================================================
						// 2007.11.15
						//================================================================
						chk = FALSE;
						if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
							if ( ( ( chkch - PM1 ) / 2 ) == ( ( PickChamber - PM1 ) / 2 ) ) {
								chk = TRUE;
							}
						}
						else {
							if ( chkch == PickChamber ) {
								chk = TRUE;
							}
						}
						//================================================================
						if ( chk ) {
							//----------------------------------------------------------
							// 2002.03.25 // Check more for Pre Recipe
							//----------------------------------------------------------
							//return TRUE;
							if ( -1 != _SCH_SYSTEM_HAS_USER_PROCESS_GET( chkch ) ) { // 2017.07.17
								rets = 2;
							}
							else {
								if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( Side , chkch ) ) { // 2003.11.11
									rets = 2; // 2005.10.18
	//								return 2; // 2005.10.18
								}
								else {
									if ( _SCH_CLUSTER_Check_Possible_UsedPre( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k , chkch , FALSE ) ) {
										if ( _SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() >= 5 ) { // 2007.10.12
	//										rets = 3; // 2007.10.12
											rets = 2; // 2007.10.12 // 2008.02.15
										}
										else {
											rets = 2; // 2005.10.18
	//										return 2; // 2005.10.18
										}
									}
									else {
										rets = 1; // 2005.10.18
	//									return 1; // 2005.10.18
									}
								}
							}
						}
						//=====================================================================================================================================
						else { // 2005.10.18
							if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ) ) { // 2005.05.11
								return 0;
							}
						}
						//=====================================================================================================================================
					}
				}
			}
			if ( rets != -1 ) {
				//==================================================================================================================
				// 2007.04.03
				//==================================================================================================================
				if ( rets == 1 ) {
					if ( _SCH_SUB_Check_Last_And_Current_CPName_is_Different( Side , Pointer , PickChamber ) ) rets = 2;
				}
				//==================================================================================================================
				return rets; // 2005.10.18
			}
		}
	}
	else if ( ( PickChamber >= BM1 ) && ( PickChamber < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2004.03.24
				//==================================================================================
				if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
					if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) != 0 ) { // 2006.03.21
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) == PickChamber ) return 1;
					}
					else {
						if ( _SCH_CLUSTER_Get_Buffer( Side , Pointer ) == PickChamber ) return 1;
					}
				}
				//==================================================================================
			}
//			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2004.03.24
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2004.03.24 // 2008.02.15
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , PickChamber ) == MUF_00_NOTUSE ) return 0; // 2018.03.21
				//
				if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
					//
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 1 ) && ( Finger == TB_STATION ) ) return 0; // 2009.07.17
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 2 ) && ( Finger == TA_STATION ) ) return 0; // 2009.07.17
					//
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == 0 ) { // 2006.02.22
						return 1;
					}
					else {
//						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) != 0 ) { // 2006.03.21 // 2013.05.03
						if ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) != 0 ) ) { // 2006.03.21 // 2013.05.03
							if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(Side) == PickChamber ) { // 2006.02.22
								return 1;
							}
						}
						else {
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == PickChamber ) { // 2006.02.22
								return 1;
							}
						}
					}
				}
				else {
					return 0;
				}
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2008.02.15
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , PickChamber ) == MUF_00_NOTUSE ) return 0; // 2018.03.21
				//
				if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
					//
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 1 ) && ( Finger == TB_STATION ) ) return 0; // 2009.07.17
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 2 ) && ( Finger == TA_STATION ) ) return 0; // 2009.07.17
					//
					if ( SCHEDULER_CONTROL_Chk_BM_FULL_OUT_MIDDLESWAP( TMATM , PickChamber ) ) { // 2008.02.15
						return 0;
					}
					else {
						if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == 0 ) { // 2006.02.22
							return 1;
						}
						else {
							if ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) != 0 ) ) { // 2006.03.21
								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(Side) == PickChamber ) { // 2006.02.22
									return 1;
								}
							}
							else {
								if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == PickChamber ) { // 2006.02.22
									return 1;
								}
							}
						}
					}
				}
				else {
					return 0;
				}
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2004.03.24
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( Side , PickChamber ) == MUF_00_NOTUSE ) return 0; // 2018.03.21
				//
				if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
					//
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 1 ) && ( Finger == TB_STATION ) ) return 0; // 2009.07.17
					if ( ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,PickChamber ) == 2 ) && ( Finger == TA_STATION ) ) return 0; // 2009.07.17
					//
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( PickChamber , BUFFER_OUTWAIT_STATUS ) > 0 ) {
						return 0;
					}
					else {
						if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == 0 ) { // 2006.03.21
							return 1;
						}
						else {
							if ( ( TMATM == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( Side ) != 0 ) ) { // 2006.03.21
								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(Side) == PickChamber ) { // 2006.03.21
									return 1;
								}
							}
							else {
								if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(Side) == PickChamber ) { // 2006.03.21
									return 1;
								}
							}
						}
					}
				}
				else {
					return 0;
				}
			}
		}
		else {
			if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) return 0;
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) return 0;
		}
	}
	return 0;
}


BOOL SCHEDULING_CHECK_All_BM_Out_Nothing( int TMATM , int pickch ) { // 2009.07.01
	int i;
	//
	if ( ( pickch < BM1 ) || ( pickch >= TM ) ) return FALSE;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) <= BUFFER_SWITCH_BATCH_PART ) { // 2013.03.09
			return FALSE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_FULLSWAP ) { // 2013.03.09
			if ( TMATM != 0 ) return TRUE; // 2013.03.20
			return FALSE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.03.09
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i + BM1 ) == TMATM ) ) {
					if      ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i + BM1 ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) == BUFFER_TM_STATION ) ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FULL_OUT_MIDDLESWAP( TMATM , i + BM1 ) ) return FALSE;
					}
					else {
						return FALSE;
					}
				}
			}
			return TRUE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(TMATM) == BUFFER_SWITCH_SINGLESWAP ) { // 2013.03.09
			return FALSE;
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	return FALSE;
}
//=======================================================================================================================================
//=======================================================================================================================================
BOOL SCHEDULING_CHECK_Switch_Disable( int TMATM , int PickChamber ) { // 2010.08.01
	int s , p , k , nc , arm , find;
	//
	if ( ( PickChamber < PM1 ) || ( PickChamber >= AL ) ) return FALSE; // 2012.07.25
	//
//	if ( _SCH_MODULE_Get_PM_WAFER( PickChamber , 0 ) <= 0 ) return FALSE; // 2014.01.10
	if ( !SCH_PM_IS_PICKING( PickChamber ) ) return FALSE; // 2014.01.10
	//
	s = 0;
	//
	for ( p = TA_STATION ; p <= TB_STATION ; p++ ) {
		if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , p ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , p ) > 0 ) ) {
			s++;
			arm = p;
		}
	}
	//
	if ( s != 1 ) return FALSE;
	//
	if ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) == SCHEDULER_MOVING_OUT ) return FALSE;
	//
	//============================================================================================================
	find = FALSE;
	//============================================================================================================
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//============================================================================================================
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//-------------------------------------------------------------------------------------
		nc = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
		//-------------------------------------------------------------------------------------
		if ( nc > 0 ) return FALSE;
		if ( nc < 0 ) find = TRUE;
	}
	//============================================================================================================
	return find;
}
//=======================================================================================================================================
//=======================================================================================================================================
BOOL SCHEDULING_CHECK_1S_TM_PATHTHRU_REJECT_FOR_GLOBAL_DEADLOCK( int TMATM , int CHECKING_SIDE , int PickChamber , int PickSlot , int NextChamber ) { // 2013.01.09
	int wc_y , wc_f , wc_lf , wc_ly , wc_lnp , f;
	int j , s , p , k , ChkTrg;
	int mc[MAX_CHAMBER]; // 2013.03.11
	//
	if ( ( PickChamber < BM1 ) || ( PickChamber >= TM ) ) return FALSE;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return FALSE; // 2013.03.11
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( PickChamber ) > 1 ) return FALSE;
	//
	if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) return FALSE;
	//
	s = _SCH_MODULE_Get_BM_SIDE( PickChamber , PickSlot );
	p = _SCH_MODULE_Get_BM_POINTER( PickChamber , PickSlot );
	//
	f = 0;
	//
	wc_y = 0;
	wc_f = 0;
	//
	wc_lf = 0;
	wc_ly = 0;
	wc_lnp = 0;
	//
	for ( j = 0 ; j < MAX_CHAMBER ; j++ ) mc[j] = 0; // 2013.03.11
	//
	for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( s , p ) ; j++ ) {
		//
		wc_lf = 0;
		wc_ly = 0;
		wc_lnp = 0;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			ChkTrg = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , j , k );
			//
			if ( ChkTrg > 0 ) {
				//
//				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) f++; // 2013.03.11
//				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) f++; // 2013.03.11
				//
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) { // 2013.03.11
					f++;
					mc[ChkTrg]++; // 2013.03.11
				}
				else if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) { // 2013.03.11
					f++;
					mc[ChkTrg]++; // 2013.03.11
				}
				//
//				if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) { // 2014.01.10
				if ( SCH_PM_IS_ABSENT( ChkTrg , s , p , j ) ) { // 2014.01.10
					wc_f++;
					wc_lf++;
				}
				else {
					wc_y++;
					wc_ly++;
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ChkTrg ) <= 0 ) wc_lnp++;
					//
				}
				//
			}
		}
		//
	}
	//
	if ( f <= 0 ) return FALSE;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2013.03.11
		for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
			if ( mc[j] > 1 ) break;
		}
		if ( j == MAX_CHAMBER ) return FALSE;
	}
	//
	if ( wc_ly <= 0 ) {
//		if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
//			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_BM 1SSEL[2] [CHECKING_SIDE=%d]=> BM%d,Slot=%d,(wc_y=%d,wc_f=%d,wc_lf=%d,wc_ly=%d)\t\t\n" , CHECKING_SIDE , PickChamber - BM1 + 1 , PickSlot , wc_y , wc_f , wc_lf , wc_ly );
//		}
		return FALSE;
	}
	//
	if ( wc_lf > 1 ) {
//		if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
//			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_BM 1SSEL[3] [CHECKING_SIDE=%d]=> BM%d,Slot=%d,(wc_y=%d,wc_f=%d,wc_lf=%d,wc_ly=%d)\t\t\n" , CHECKING_SIDE , PickChamber - BM1 + 1 , PickSlot , wc_y , wc_f , wc_lf , wc_ly );
//		}
		return FALSE;
	}
	else if ( wc_lf == 1 ) {
		if ( wc_lnp <= 0 ) return FALSE;
	}
	//
	return TRUE;
	//
}


//

BOOL SCHEDULING_CHECK_OutOnly_BM( int TMATM , int pickch , int nextch ) { // 2014.10.07
	//
	if ( ( nextch < BM1 ) || ( nextch >= TM ) ) return FALSE;
	//
	if ( ( pickch >= BM1 ) && ( pickch < TM ) ) {
		if ( _SCH_PRM_GET_MODULE_GROUP( pickch ) == _SCH_PRM_GET_MODULE_GROUP( nextch ) ) return FALSE;
	}
	else if ( ( pickch >= PM1 ) && ( pickch < AL ) ) {
		if ( _SCH_PRM_GET_MODULE_GROUP( pickch ) != _SCH_PRM_GET_MODULE_GROUP( nextch ) ) return FALSE;
	}
	else {
		return FALSE;
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , nextch ) == BUFFER_SWITCH_MODE ) {
		if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , nextch , G_PLACE , G_MCHECK_SAME ) ) {
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , nextch , G_PICK , G_MCHECK_SAME ) ) {
				return TRUE;
			}
		}
	}
	else {
		if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , nextch ) == BUFFER_OUT_MODE ) return TRUE;
	}
	//
	return FALSE;
	//
}


BOOL SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked_Cannot_sameTM( int TMATM , int arm , int pickch ) { // 2016.12.01
	int s , p , d , k;
	//
	//============================================================================================================
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) <= 0 ) return FALSE;
	//============================================================================================================
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//============================================================================================================
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) <= 0 ) return FALSE;
	//============================================================================================================
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( d > _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//-------------------------------------------------------------------------------------
		if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d - 1 , k ) == pickch ) return TRUE;
		//-------------------------------------------------------------------------------------
	}
	//============================================================================================================
	return FALSE;
	//============================================================================================================
}

BOOL SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked_Next_Having( int pm ) {
	int s , p , d , k , nc;
	//
	//============================================================================================================
	if ( _SCH_MODULE_Get_PM_WAFER( pm,0 ) <= 0 ) return FALSE;
	//============================================================================================================
	s = _SCH_MODULE_Get_PM_SIDE( pm , 0 );
	p = _SCH_MODULE_Get_PM_POINTER( pm , 0 );
	//============================================================================================================
	if ( _SCH_MODULE_Get_PM_WAFER( pm,0 ) <= 0 ) return FALSE;
	//============================================================================================================
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//-------------------------------------------------------------------------------------
		nc = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
		//
		if ( nc > 0 ) {
			if ( _SCH_MODULE_Get_PM_WAFER( nc,0 ) > 0 ) return TRUE;
		}
		//-------------------------------------------------------------------------------------
	}
	//============================================================================================================
	return FALSE;
	//============================================================================================================
}

BOOL SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked( int TMATM , int pickch , int nextch ) { // 2016.12.01
	int i , k , m , s , p , d , fpm1 , fpm2 , fpm3;
	//
	if ( TMATM != 0 ) return FALSE;
	//
	if ( ( pickch < PM1 ) || ( pickch >= AL ) ) return FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , pickch ) < 2 ) return FALSE;
	//
	//=======================================================================================================
	//=======================================================================================================
	//
	fpm1 = 0;
	fpm2 = 0;
	fpm3 = 0;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		//
		d = _SCH_CLUSTER_Get_PathDo( s , p );
		//
		if ( d > _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//-------------------------------------------------------------------------------------
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d - 1 , k );
			//
			if ( m > 0 ) {
				//
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) {
					//
					if ( SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked_Next_Having( m ) ) {
						if      ( fpm1 == 0 ) fpm1 = m;
						else if ( fpm2 == 0 ) fpm2 = m;
						else if ( fpm3 == 0 ) fpm3 = m;
					}
					//
				}
				//
			}
			//-------------------------------------------------------------------------------------
		}
	}
	//
	if ( fpm1 == 0 ) return FALSE;
	//
	//=======================================================================================================
	//=======================================================================================================
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( i == TMATM ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( TM + i ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( i , pickch , G_PLACE ) ) continue;
		//
		m = 0;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( i,TA_STATION ) > 0 ) {
			if ( SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked_Cannot_sameTM( i , TA_STATION , pickch ) ) m = 1;
		}
		//
		if ( m == 0 ) {
			if ( _SCH_MODULE_Get_TM_WAFER( i,TB_STATION ) > 0 ) {
				if ( SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked_Cannot_sameTM( i , TB_STATION , pickch ) ) m = 1;
			}
		}
		//
		if ( m == 0 ) continue;
		//
		if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , fpm1 , G_PICK ) ) return TRUE;
		//
		if ( fpm2 != 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , fpm2 , G_PICK ) ) return TRUE;
		}
		//
		if ( fpm3 != 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , fpm3 , G_PICK ) ) return TRUE;
		}
		//
	}
	//
	return FALSE;
	//
}


//=======================================================================================
BOOL SCHEDULING_CHECK_for_TM_ARM_All_Disable( int TMATM , int Finger ) { // 2017.04.06
	int s , p , pd , k , m;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,Finger ) )				return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) <= 0 )				return FALSE;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , Finger );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , Finger );
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( pd > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd - 1 , k );
		//
		if ( m > 0 ) return FALSE;
		//
	}
	//
	return TRUE;
}

//=======================================================================================================================================
int SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( int TMATM , int CHECKING_SIDE , int PickChamber , int PickSlot , int NextChamber , BOOL *switchable ) { // 2007.01.15
	//
	// lower
	//
	//	<기본조건>
	//		Single Arm일때 검사 안함(Pick 승인)
	//
	//	<1차 PICK 거절,승인 조건>
	//
	//													적용일자	1	2	3	4	5	6
	//
	//		A.BM에서 Pick할 때							2012.07.11					X
	//			BM에서 Pick할 Wafer가
	//			갈 PM에 POST RECIPE를 가지고 있을때
	//
	//		B.PM에서 Pick할 때							2007.11.28				X
	//			TM에 Wafer가 하나도 없고
	//			한 BM에서 공급할 Wafer를
	//			가지고 있을때
	//
	//		C.BM에서 Pick할 때							2007.11.28				X
	//			{Switch 상황일때}
	//			TM에 Wafer가 Place 할
	//			BM이 대기 중일때
	//
	//		D.{Switch 상황이 아닐때}					2008.09.26				O
	//			TM이 회수할 Wafer를
	//			가지고 있을때
	//
	//	<1차 PICK 거절,승인 조건 통과시 Switch 상황일때는 Pick 승인>
	//
	//													적용일자	1	2	3	4	5	6
	//
	//													2007.01.15	O	O	O	O	O	O
	//
	//	<1차 PICK 거절,승인 조건 통과후 Switch 상황이 아닐때,Pick할 Chamber의 Get Priority가 0보다 작을때>
	//
	//													적용일자	1	2	3	4	5	6
	//
	//			갈 PM이 Wafer가 있으면 {Pick 거절}		2014.01.10	X	X	X	X	X	X
	//
	//			갈 PM이 Wafer가 없으면 {Pick 승인}		2007.01.15	O	O	O	O	O	O
	//
	//	<1차 PICK 거절,승인 조건 통과후 Switch 상황이 아닐때,Pick할 Chamber의 Get Priority가 0보다 크거나 같을때, 2차 Pick 거절,승인 조건>
	//
	//													적용일자	1	2	3	4	5	6
	//
	//		A.BM에서 Pick할 때							2017.02.08						O
	//			Pick 할 Wafer가 회수 Wafer일때
	//
	//		B.BM에서 Pick할 때							2014.09.01	O	O	O	O	O	{O}
	//			Pick 할 Wafer가 회수 Wafer이고
	//			회수할 BM이 하나이고
	//			회수할 BM과 Pick할 BM이 같을때
	//
	//		C.Pick한 후 Place할 곳이 BM으로				2014.10.08	O	O	O	O	O	O
	//			회수 동작중일때
	//
	//		D.TM이 바로 Place할 수 있는 Wafer를			2007.03.06		O	O		O	O
	//			가지고 있지 않고 , 갈 PM에 Wafer가 없을 때
	//
	//		E.TM이 바로 Place할 수 있는 Wafer를			2010.08.01		O	O		O	O
	//			가지고 있고 , 갈 PM이 Disable일 때
	//
	//		F.BM에서 Pick할 때
	//			Target PM에 Place할 공간이 있을때		2012.07.25					O
	//
	//		G.TM이 바로 Place할 수 있는 Wafer를			2007.03.06			O		O	O
	//			가지고 있지 않고 , PM에서 Pick할 Wafer가
	//			더이상 갈 PM이 없을때
	//
	//		H.TM이 바로 Place할 수 있는 Wafer를			2009.07.01			O		O	O
	//			가지고 있고 , BM에서 Pick할 때
	//			Middle Swap의 경우 회수할 수 있는 상태의
	//			BM이 없을때
	//
	//
	//	<외 거절>
	//
	int Finger , higher , lower;
	int s , p;
	BOOL tagcheckskip; // 2013.12.18
	//
	*switchable = FALSE;
	tagcheckskip = FALSE; // 2013.12.18
	//
	lower  = _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10; // 2013.11.27
	higher = _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) / 10; // 2013.11.27
	//
	if ( higher == 0 ) { // 2013.11.27
		//
		if ( ( PickChamber >= BM1 ) && ( PickChamber < TM ) ) {
			//
			if ( TMATM == 0 ) { // 2014.01.02
				//
				if ( SCHEDULING_CHECK_for_BPM_ARM_INTLKS( TMATM , PickChamber , _SCH_MODULE_Get_BM_SIDE( PickChamber , PickSlot ) , _SCH_MODULE_Get_BM_POINTER( PickChamber , PickSlot ) , TRUE ) == -1 ) {
					tagcheckskip = TRUE; // 2013.12.18
					higher = 1;
				}
				//
			}
			//
		}
		//
	}
	//
	if ( ( PickSlot < 0 ) && ( PickChamber >= PM1 ) && ( PickChamber < AL ) ) { // 2014.01.11
		if ( !SCH_PM_HAS_PICKING( PickChamber , &PickSlot ) ) return 7000; // 2014.01.10
	}
	//
//	if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10 ) == 0 ) { // 2013.11.27
	if ( lower == 0 ) { // 2013.11.27
		if ( SCHEDULING_CHECK_1S_TM_PATHTHRU_REJECT_FOR_GLOBAL_DEADLOCK( TMATM , CHECKING_SIDE , PickChamber , PickSlot , NextChamber ) ) return 6000; // 2013.01.09
	}
	//
//	if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) == 0 ) return 0; // 2013.11.27
	if ( ( higher == 0 ) && ( lower == 0 ) ) return 0; // 2013.11.27
	//
//	lower  = _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10; // 2013.11.27
//	higher = _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) / 10; // 2013.11.27
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return 0;
	//
//	s = SCHEDULING_CHECK_NOPASING_PICK_REJECT( higher , TMATM , CHECKING_SIDE , 0 , PickChamber , PickSlot ); // 2013.12.18
	s = SCHEDULING_CHECK_NOPASING_PICK_REJECT( higher , TMATM , CHECKING_SIDE , 0 , PickChamber , PickSlot , tagcheckskip ); // 2013.12.18
	if ( s > 0 ) return ( (s * 10000) + 1000 );
	//
	if ( lower == 0 ) return 0; // 2013.10.31
	//
	if ( lower == 5 ) { // 2012.07.11
		//
		s = SCHEDULER_CONTROL_Chk_BM_TARGET_PM_HAS_WFR_POST( TMATM , PickChamber , PickSlot );
		//

//_IO_CONSOLE_PRINTF( "[%s][%d] [%d]\n" ,  _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( PickChamber ) , PickSlot , s );

		if ( s > 0 ) {
			//
			return ( (s * 10000) + 2000 );
			//
		}
		//
	}
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) {
		//===================================================================================================
		// 2007.11.28
		//===================================================================================================
		if ( lower == 4 ) {
			if ( ( PickChamber >= PM1 ) && ( PickChamber < AL ) ) {
				if ( SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( TMATM , FALSE , FALSE , -1 ) == 0 ) {
					return 3000;
				}
			}
		}
		//===================================================================================================
		return 0;
	}
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return 0;
	//
	if      ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) Finger = TA_STATION;
	else if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) Finger = TB_STATION;
	//
	if ( SCHEDULING_CHECK_Switch_PLACE_From_TM( TMATM , CHECKING_SIDE , Finger , PickChamber ) != 0 ) {
		//===================================================================================================
		// 2007.11.28
		//===================================================================================================
		if ( lower == 4 ) {
			if ( ( PickChamber >= BM1 ) && ( PickChamber < TM ) ) {
				if ( SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( TMATM , TRUE , TRUE , PickChamber ) == 0 ) {
					return 4000;
				}
			}
		}
		//===================================================================================================
		*switchable = TRUE;
		return 0;
	}
	else { // 2008.09.26
		if ( lower == 4 ) {
			if ( SCHEDULER_CONTROL_Chk_TM_WAFER_FINISH( TMATM , ( Finger == TA_STATION ) ? TB_STATION : TA_STATION ) ) { // 2008.09.26
				return 0;
			}
		}
	}
	//
	if ( _SCH_PRM_GET_Getting_Priority( PickChamber ) < 0 ) {
		if ( ( NextChamber >= PM1 ) && ( NextChamber < AL ) ) {
			//
//			if ( _SCH_MODULE_Get_PM_WAFER( NextChamber , 0 ) > 0 ) return 5000; // 2014.01.10
			if ( !SCH_PM_IS_FREE( NextChamber ) ) return 5000; // 2014.01.10
			//
		}
		return 0;
	}
	else { // 2007.03.06
		//
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return 0; // 2008.11.22
		//
		// 2014.09.01
		//
		if ( ( PickChamber >= BM1 ) && ( PickChamber < TM ) ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( PickChamber , PickSlot ) > 0 ) {
				//
				s = _SCH_MODULE_Get_BM_SIDE( PickChamber , PickSlot );
				p = _SCH_MODULE_Get_BM_POINTER( PickChamber , PickSlot );
				//
				if ( ( p < MAX_CASSETTE_SLOT_SIZE ) && ( p >= 0 ) ) {
					//
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
						//
						if ( lower == 6 ) return 0; // 2017.02.08
						//
						if ( SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , s , &p ) ) {
							//
							if ( PickChamber == p ) { // 2014.09.08 // 2014.09.12
								return 0;
							}
						}
					}
					//
				}
				//
			}
		}
		//
		// 2014.10.08
		//
		if ( SCHEDULING_CHECK_OutOnly_BM( TMATM , PickChamber , NextChamber ) ) return 0;
		//
		//
		// 2016.12.01
		//
		if ( SCHEDULING_CHECK_Gateway_Pick_PM_and_OtherTM_Loop_Locked( TMATM , PickChamber , NextChamber ) ) return 0;
		//
		switch( lower ) {
		case 2 :
			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , -1 , 2 , FALSE ) != 0 ) {
				//
				// (a-2/b-1/c-1) , (a-2/b-1/c-2) , (a-2/b-1/c-3)
				//
				if ( ( NextChamber >= PM1 ) && ( NextChamber < AL ) ) {
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( NextChamber , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_FREE( NextChamber ) ) { // 2014.01.10
						//
						// (a-2/b-1/c-1)
						//
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , NextChamber ) < 2 ) return 0;
					}
				}
				//
			}
			else {
				//
				// (a-1) , (a-2/b-2) , (a-2/b-3)
				//
				if ( SCHEDULING_CHECK_Switch_Disable( TMATM , PickChamber ) ) return 0; // 2010.08.01
				//
			}
			break;
		case 3 : // 2007.03.13
		case 5 : // 2012.07.11
		case 6 : // 2017.02.08
			//
			if ( lower == 5 ) { // 2012.07.25
				if ( SCHEDULER_CONTROL_Chk_BM_TARGET_PM_HAS_SPACE( TMATM , PickChamber , PickSlot ) > 0 ) return 0;
			}
			//
			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , -1 , 2 , FALSE ) != 0 ) {
				//
				// (a-2/b-1/c-1) , (a-2/b-1/c-2) , (a-2/b-1/c-3)
				//
				if ( ( NextChamber >= PM1 ) && ( NextChamber < AL ) ) {
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( NextChamber , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_FREE( NextChamber ) ) { // 2014.01.10
						//
						// (a-2/b-1/c-1)
						//
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , NextChamber ) < 2 ) return 0;
						//
					}
				}
				//
				if ( ( PickChamber >= PM1 ) && ( PickChamber < AL ) ) {
					//
					//
//					if ( _SCH_MODULE_Get_PM_WAFER( PickChamber , 0 ) > 0 ) { // 2014.01.10
					if ( _SCH_MODULE_Get_PM_WAFER( PickChamber , PickSlot ) > 0 ) { // 2014.01.10
						//
//						s = _SCH_MODULE_Get_PM_SIDE( PickChamber , 0 ); // 2014.01.10
//						p = _SCH_MODULE_Get_PM_POINTER( PickChamber , 0 ); // 2014.01.10
						//
						s = _SCH_MODULE_Get_PM_SIDE( PickChamber , PickSlot ); // 2014.01.10
						p = _SCH_MODULE_Get_PM_POINTER( PickChamber , PickSlot ); // 2014.01.10
						//
//						if ( ( p < 100 ) && ( p >= 0 ) ) { // 2014.09.01
						if ( ( p < MAX_CASSETTE_SLOT_SIZE ) && ( p >= 0 ) ) { // 2014.09.01
							//
							// (a-2/b-1/c-3)
							//
							if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return 0;
						}
					}
				}
			}
			else {
				//
				// (a-1) , (a-2/b-2) , (a-2/b-3) SCHEDULING_CHECK_All_BM_Out_Nothing(?)
				//
				if ( SCHEDULING_CHECK_All_BM_Out_Nothing( TMATM , PickChamber ) ) return 0; // 2009.07.01
				//
				if ( SCHEDULING_CHECK_Switch_Disable( TMATM , PickChamber ) ) return 0; // 2010.08.01
				//
			}
			break;
		}
	}
	//
	if ( ( PickChamber >= BM1 ) && ( PickChamber < TM ) ) { // 2017.04.06
		if ( SCHEDULING_CHECK_for_TM_ARM_All_Disable( TMATM , ( Finger == TA_STATION ) ? TB_STATION : TA_STATION ) ) return 0;
	}
	//
	return 9000;
}






BOOL SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_0( int side , int *ch , int *slot , int *uc , int skipslot ) { // 2007.03.19
	int i , mx = -1 , s = -1 , c;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		if ( ( i + 1 ) == skipslot ) continue;
		if ( *ch == -1 ) {
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) == 0 ) continue; // 2007.05.04
			if ( _SCH_PRM_GET_MODULE_MODE( _SCH_SDM_Get_RUN_CHAMBER(i) ) ) continue; // 2007.05.04
		}
		else {
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) != *ch ) continue; // 2007.05.04
		}
		if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) <= 0 ) continue;
			if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
				mx = _SCH_SDM_Get_RUN_USECOUNT(i);
				s = i;
				c = _SCH_SDM_Get_RUN_CHAMBER(i);
			}
//		}
	}
	if ( s == -1 ) return FALSE;
	*slot = s + 1;
	*ch = c;
	*uc = mx;
	return TRUE;
}
//










BOOL SCHEDULER_CONTROL_DUALTM_for_PICK_AND_PLACE_IMPOSSIBLE( int tms , int pick_Arm , int place_pm ) { // 2013.06.12
	int m2;
	//
	if ( pick_Arm == 0 ) return FALSE;
	//
	m2 = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms,place_pm );
	//
	if ( m2 == 0 ) return FALSE;
	//
	if ( pick_Arm == m2 ) return FALSE;
	//
	return TRUE;
	//
}

BOOL SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS( int tms , int s , int p , int pathdo , int pick_bm , int mode , BOOL prcscheck ) { // 2013.07.13
	int m , k;
	//
	if ( pathdo >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pathdo , k );
		//
		if ( m <= 0 ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( m ) != tms ) continue;
		//
		if ( prcscheck ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) != 0 ) continue;
//			if ( _SCH_MODULE_Get_PM_WAFER( m,0 ) > 0 ) continue; // 2014.01.10
			if ( !SCH_PM_IS_ABSENT( m , s , p , pathdo ) ) continue; // 2014.01.10
		}
		//
		if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 2 ) == mode ) { // 0:NoReturn , 1:ReturnCM , 2:Ignore
			if ( SCHEDULER_CONTROL_DUALTM_for_PICK_AND_PLACE_IMPOSSIBLE( tms , _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,pick_bm ) , m ) ) continue;
		}
		//
		return TRUE;
		//
	}
	//
	return FALSE;
	//
}

BOOL SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS_CURR_TM( int tms , int s , int p , int pathdo , int pick_bm , BOOL prcscheck ) { // 2016.12.13
	int m , k;
	//
	if ( pathdo >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pathdo , k );
		//
		if ( m <= 0 ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( m ) != tms ) continue;
		//
		if ( prcscheck ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) != 0 ) continue;
			if ( !SCH_PM_IS_ABSENT( m , s , p , pathdo ) ) continue;
		}
		//
		return TRUE;
		//
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
		if ( _SCH_MODULE_Get_TM_WAFER( tms,TA_STATION ) <= 0 ) {
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,pick_bm ) != 2 ) return TRUE;
		}
		if ( _SCH_MODULE_Get_TM_WAFER( tms,TB_STATION ) <= 0 ) {
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,pick_bm ) != 1 ) return TRUE;
		}
	}
	//
	return FALSE;
	//
}

int SCHEDULER_CONTROL_DUALTM_for_GET_PROPERLY_ARM_WHEN_PICK_FROM_BM_POSSIBLE( int tms , int s , int p , int pathdo , int pick_bm ) { // 2013.07.13
	int m , k , f , pick_arm;
	//
	pick_arm = _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,pick_bm );
	//
	if ( pick_arm == 0 ) return pick_arm;
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 2 ) != 2 ) return pick_arm; // 0:NoReturn , 1:ReturnCM , 2:Ignore
	//
	if ( pathdo >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return pick_arm;
	//
	f = 0;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pathdo , k );
		//
		if ( m <= 0 ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( m ) != tms ) continue;
		//
		f++;
		//
		if ( SCHEDULER_CONTROL_DUALTM_for_PICK_AND_PLACE_IMPOSSIBLE( tms , pick_arm , m ) ) continue;
		//
		return pick_arm;
		//
	}
	//
	if ( f > 0 ) {
		if ( pick_arm == 1 )
			return 2;
		else
			return 1;
		//
	}
	//
	return pick_arm;
	//
}