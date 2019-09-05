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
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_FMArm_multi.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_sch_commonuser.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_subBM.h"
#include "sch_A0_param.h"
#include "sch_A0_dll.h"
#include "sch_A0_sub.h"
#include "sch_A0_sub_sp4.h"
#include "sch_A0_sub_F_sw.h"
//================================================================================

BOOL Scheduling_TM_Double( BOOL optonly ); // 2018.09.19

int  FM_PLACE_LOCKEDBM; // 2017.02.18 L.A.M 확인중

//BOOL SCHEDULER_FM_has_No_TMSIDE( int side ); // 2017.10.16 AKRA+SMART // 2018.05.16
int SCHEDULER_FM_has_No_TMSIDE( int side ); // 2017.10.16 AKRA+SMART // 2018.05.16

int SCHEDULER_CONTROL_Chk_BM_HAS_OTHERSIDE_OUT_GET_SLOT_SWITCH( int mode , int side , int ch ); // 2016.04.08

#define DUAL_AL_FM_PICK_NOWAIT	FALSE

extern BOOL FM_Pick_Running_Blocking_style_0;
extern int  FM_Place_Waiting_for_BM_style_0; // 2014.01.03

int wfrreturn_mode[MAX_CASSETTE_SIDE];

BOOL OTHER_SIDE_PLACED_WAFER; // 2011.06.28

extern int Dual_AL_Dual_FM_Second_Check; // 2014.03.05

int SCHEDULER_CONTROL_Chk_FM_ARM_Has_Been_Waiting_For_1SlotBM_Supply( int side , int mode ); // 2011.10.17
BOOL SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( int tmside , int side , int *bmc ); // 2014.08.14

BOOL SCHEDULER_COOLING_SKIP_AL0( int side , int pt );
BOOL SCHEDULER_ALIGN_SKIP_AL0( int side , int pt );

/*
void xxx_check_printf( int side , BOOL check , char *message , int im ) {
	//
	return;
	//
	if ( check ) {
		if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) return;
		//
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL ) return;
		}
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL ) return;
		}
	}
	//
	//----------
	_SCH_LOG_LOT_PRINTF( side , "XXX Scheduling CHECKED[%s][%d]%cWHFMSKIP\n" , message , im , 9 );
	printf( "========================================\n" );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "================ [%s][%d] ==\n" , message , im );
	printf( "========================================\n" );
	//----------
}
*/

//======================================================================================================================================================================
//======================================================================================================================================================================

BOOL SCH_TM_EVEN_SWAP_CHECK( int s , int p ) { // 2017.07.13
	//
	int c , j , tmside , pm;
	//
	if ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() != 2 ) return FALSE;
	//
	c = 0;
	tmside = 0;
	//
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		pm = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , j );
		if ( pm > 0 ) {
			//
			if ( c != 0 ) return FALSE;
			//
			c++;
			//
			tmside = _SCH_PRM_GET_MODULE_GROUP( pm );
			//
		}
	}
	//
	if ( ( tmside % 2 ) == 1 ) return TRUE;
	return FALSE;
}


void SCHEDULING_BM_LOCK_CHECK_TM_SIDE( int side , int bmc ) { // 2017.02.08
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) >= BUFFER_SWITCH_SINGLESWAP ) {
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
				//-------------------------------------------------------------------------------
				SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Pump_for_Switch_SingleSwap1( side , bmc );
				//-------------------------------------------------------------------------------
			}
		}
	}
	//
}

//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
BOOL SCHEDULER_FM_PM_GET_PLACE_PM( int *pm ) { // 2014.11.06
	int k;
	int rp;
	BOOL last; // 2015.12.21
	//
	*pm = 0;
	//
	rp = 0;
	//
	last = FALSE; // 2015.12.21
	//
	for ( k = ( Get_RunPrm_FM_PM_START_MODULE() + 1 ) ; k < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; k++ ) { // from pm11 // 2015.12.21
		if ( _SCH_PRM_GET_Putting_Priority( Get_RunPrm_FM_PM_START_MODULE() ) > _SCH_PRM_GET_Putting_Priority( k ) ) {
			last = TRUE;
			break;
		}
	}
	//
	if ( last ) { // 2015.12.21
		//
		for ( k = ( PM1 + MAX_PM_CHAMBER_DEPTH - 1 ) ; k >= Get_RunPrm_FM_PM_START_MODULE() ; k-- ) { // to pm11
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( k ) != -1 ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( k ) ) continue; // 2015.11.20
			//
			if ( _SCH_MODULE_Get_PM_WAFER( k , 0 ) > 0 ) continue;
			if ( _SCH_MODULE_Get_PM_WAFER( k , 1 ) > 0 ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) != 0 ) continue;
			//
			if ( rp == 0 ) {
				rp = k;
			}
			else {
				//
				if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , k )	< _SCH_TIMER_GET_PROCESS_TIME_START( 0 , rp ) ) {
					rp = k;
				}
				//
			}
			//
		}
	}
	else {
		//
		for ( k = Get_RunPrm_FM_PM_START_MODULE() ; k < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; k++ ) { // from pm11
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( k ) != -1 ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( k ) ) continue; // 2015.11.20
			//
			if ( _SCH_MODULE_Get_PM_WAFER( k , 0 ) > 0 ) continue;
			if ( _SCH_MODULE_Get_PM_WAFER( k , 1 ) > 0 ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) != 0 ) continue;
			//
			if ( rp == 0 ) {
				rp = k;
			}
			else {
				//
				if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 , k )	< _SCH_TIMER_GET_PROCESS_TIME_START( 0 , rp ) ) {
					rp = k;
				}
				//
			}
			//
		}
	}
	//
	*pm = rp;
	//
	if ( rp == 0 ) return FALSE;
	//
	return TRUE;
	//
}

BOOL SCHEDULER_FM_CPM_PICK_POSSIBLE( int bmc ) { // 2015.05.29
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return TRUE;
	//
	if ( bmc >= BM1 ) {
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
	}
	//
	return TRUE;
}

int SCHEDULER_FM_PM_GET_PICK_PM( int mode , int side , int cldx , int *pm , int *l1 , int *w1 , int *s1 , int *p1 , int *l2 , int *w2 , int *s2 , int *p2 ) { // 2014.11.06
	int k , s , p , xs1 , xp1 , xw1 , xs2 , xp2 , xw2 , csp , clx;
	int rpm , rsp , rl1 , rw1 , rs1 , rp1 , rl2 , rw2 , rs2 , rp2;
	int Pres , Pres2;
	//
	*pm = 0;
	*w1 = 0;
	*w2 = 0;
	*l1 = 0;
	*l2 = 0;
	//
	rpm = 0;
	//
	for ( k = Get_RunPrm_FM_PM_START_MODULE() ; k < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; k++ ) { // from pm11
		//
//		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue; // 2016.03.07
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , ( mode == -1 ) , -1 ) ) continue; // 2016.03.07
		//
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Skip( k ) ) continue; // 2015.11.20
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( k ) != -1 ) continue;
		//
		if ( ( _SCH_MODULE_Get_PM_WAFER( k , 0 ) <= 0 ) && ( _SCH_MODULE_Get_PM_WAFER( k , 1 ) <= 0 ) ) continue;
		//
		xs1 = _SCH_MODULE_Get_PM_SIDE( k , 0 );
		xp1 = _SCH_MODULE_Get_PM_POINTER( k , 0 );
		xw1 = _SCH_MODULE_Get_PM_WAFER( k , 0 );
		xs2 = _SCH_MODULE_Get_PM_SIDE( k , 1 );
		xp2 = _SCH_MODULE_Get_PM_POINTER( k , 1 );
		xw2 = _SCH_MODULE_Get_PM_WAFER( k , 1 );
		//
		if ( xw1 > 0 ) {
			s = xs1;
			p = xp1;
		}
		else {
			s = xs2;
			p = xp2;
		}
		//
		Pres = _SCH_MACRO_CHECK_PROCESSING_INFO( k );
		//
		if ( mode == -1 ) { // 2016.03.07 // return
			//
			if ( Pres > 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 3 ) {
				//
				Pres2 = _SCH_MACRO_CHECK_PROCESSING_INFO( k + 1 );
				//
				if ( Pres2 > 0 ) continue;
				//
				if ( ( Pres >= 0 ) && ( Pres2 >= 0 ) ) continue;
				//
			}
			else {
				//
				if ( Pres >= 0 ) continue;
				//
			}
			//
		}
		else {
			//
			if ( Pres < 0 ) continue; // 2015.12.01
			//
			if ( side != -1 ) { // 2016.02.16
				//
				if ( side != s ) {
					if ( !_SCH_SUB_Check_Complete_Return( s ) ) continue;
				}
				//
			}
			//
		}
		//
		clx = _SCH_CLUSTER_Get_ClusterIndex( s , p );
		//
		if ( cldx >= 0 ) {
			if ( clx != cldx ) continue;
		}
		//
		csp = _SCH_CLUSTER_Get_SupplyID( s , p );
		//
		if ( rpm == 0 ) {
			//
			rpm = k;
			//
			rl1 = 1;
			rw1 = xw1;
			rs1 = xs1;
			rp1 = xp1;
			rl2 = 2;
			rw2 = xw2;
			rs2 = xs2;
			rp2 = xp2;
			//
			rsp = csp;
			//
		}
		else {
			if ( csp < rsp ) {
				//
				rpm = k;
				//
				rl1 = 1;
				rw1 = xw1;
				rs1 = xs1;
				rp1 = xp1;
				rl2 = 2;
				rw2 = xw2;
				rs2 = xs2;
				rp2 = xp2;
				//
				rsp = csp;
				//
			}
		}
		//
	}
	//
	if ( rpm == 0 ) return -1;
	//
	if ( side != -1 ) {
		if ( rw1 > 0 ) {
			if ( side != rs1 ) return -2;
		}
		else {
			if ( side != rs2 ) return -2;
		}
	}
	//
	if ( mode != -1 ) { // 2016.03.07 // return
		//
		if ( mode != 9 ) {
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( rpm ) != 0 ) return -3;
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 3 ) { // 2015.11.20
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( rpm + 1 ) != 0 ) return -4;
			}
			//
		}
	}
	//
	switch( mode ) {
	case -1 : // 2016.03.07 // return
	case 0 :
	case 9 :
		*pm = rpm;
		//
		*l1 = rl1;
		*w1 = rw1;
		*s1 = rs1;
		*p1 = rp1;
		//
		*l2 = rl2;
		*w2 = rw2;
		*s2 = rs2;
		*p2 = rp2;
		//
		break;
	case 1 :
	case 3 :
		*pm = rpm;
		//
		*l1 = rl1;
		*w1 = rw1;
		*s1 = rs1;
		*p1 = rp1;
		break;
	case 2 :
		*pm = rpm;
		//
		*l2 = rl1;
		*w2 = rw1;
		*s2 = rs1;
		*p2 = rp1;
		break;
	default :
		return -5;
		break;
	}
	//
	return 1;
	//
}

BOOL SCHEDULER_FM_PM_DIRECT_MODE( int side , int pt , BOOL *NoMore ) { // 2014.11.25
	int i , ch , selch , m , Act;
	int pmc[MAX_PM_CHAMBER_DEPTH];
	char *data;
	//
	*NoMore = FALSE; // 2015.06.22
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return FALSE;
	//
	switch( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) { // 2015.11.18
	case 1 :
	case 3 :
		Act = 1;
		break;
	default :
		Act = 0;
		break;
	}
	//
	selch = -1;
	//
	if ( Act == 1 ) { // 2015.11.18
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = 0;
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			ch = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , i );
			//
			if ( ch > 0 ) {
				//
				if ( ch >= Get_RunPrm_FM_PM_START_MODULE() ) { // 2016.05.16
					_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pt , 0 , i );
					ch = -1;
				}
				//
				if ( ch > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							pmc[ch-PM1] = 1;
						}
					}
					//
				}
				//
			}
		}
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( pmc[i] || pmc[i+1] ) {
				//
				if ( selch == -1 ) {
					//
					selch = i + PM1;
					//
				}
				else {
					if ( Get_RunPrm_FM_PM_SEL_PM( selch , i + PM1 ) ) {
						//
						selch = i + PM1;
						//
					}
				}
			}
			//
			i++;
		}
		//
	}
	else {
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			ch = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , i );
			//
			if ( ch > 0 ) {
				//
				if ( ch >= Get_RunPrm_FM_PM_START_MODULE() ) { // 2016.05.16
					_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pt , 0 , i );
					ch = -1;
				}
				//
				if ( ch > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							//
							if ( selch == -1 ) {
								selch = ch;
							}
							else {
								if ( Get_RunPrm_FM_PM_SEL_PM( selch , ch ) ) {
									selch = ch;
								}
							}
							//
						}
					}
				}
				//
			}
		}
		//
	}
	//
	if ( selch != -1 ) {
		//
		m = 0;
		data = _SCH_CLUSTER_Get_UserArea( side , pt );
		//
		if ( data != NULL ) {
			//
			i = atoi( data );
			//
			switch( selch ) {
			case PM1 + 0 :
				m = i % 10;
				break;
			case PM1 + 1 :
				m = ( i / 10 ) % 10;
				break;
			case PM1 + 2 :
				m = ( i / 100 ) % 10;
				break;
			case PM1 + 3 :
				m = ( i / 1000 ) % 10;
				break;
			case PM1 + 4 :
				m = ( i / 10000 ) % 10;
				break;
			case PM1 + 5 :
				m = ( i / 100000 ) % 10;
				break;
			}
			//
		}
		//
		if ( m == 0 ) {
			return FALSE;
		}
		else {
			return TRUE;
		}
		//
	}
	else {
		//
		*NoMore = TRUE; // 2015.06.22
		//
	}
	//
	return FALSE;
}




//void SCHEDULER_FM_PM_DIRECT_RUN( int mode , int wfr1 , int side1 , int pt1 , int wfr2 , int side2 , int pt2 ) { // 2014.11.25 // 2015.05.27
//void SCHEDULER_FM_PM_DIRECT_RUN( int mode , int wfr1 , int side1 , int pt1 , int wfr2 , int side2 , int pt2 , int *rmode ) { // 2014.11.25 // 2015.05.27 // 2015.11.10
BOOL SCHEDULER_FM_PM_DIRECT_RUN( int mode , int wfr1 , int side1 , int pt1 , int wfr2 , int side2 , int pt2 , int *rmode ) { // 2014.11.25 // 2015.05.27 // 2015.11.10
	int i , ch , selch , sellr , m , Act;
	int pmc[MAX_PM_CHAMBER_DEPTH];
	char *data;
	int selrmode , selfailch1 , selfailch2 , selfailch3; // 2016.08.12
	//
	*rmode = 0;
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return TRUE;
	//
	switch( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) { // 2015.11.18
	case 1 :
	case 3 :
		Act = 1;
		break;
	default :
		Act = 0;
		break;
	}
	//
	selch = -1;
	sellr = 0;
	//
	if ( Act == 1 ) { // 2015.11.10
		//
		selfailch1 = -1; // 2016.08.12
		selfailch2 = -1; // 2016.08.12
		selfailch3 = -1; // 2016.08.12
		//
		while ( TRUE ) { // 2016.08.12
			//
			*rmode = 0; // 2016.08.12
			//
			selch = -1; // 2016.08.12
			sellr = 0; // 2016.08.12
			//
			if ( wfr1 > 0 ) {
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = 0;
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					ch = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , i );
					//
					if ( ch > 0 ) {
						//
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
								pmc[ch-PM1] = 1;
							}
						}
						//
					}
					//
				}
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					if ( selfailch1 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					if ( selfailch2 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					if ( selfailch3 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					//
					if ( pmc[i] || pmc[i+1] ) {
						//
						if ( selch == -1 ) {
							//
							if      ( pmc[i] && !pmc[i+1] ) {
								selch = i + PM1;
								sellr = 1;
							}
							else if ( !pmc[i] && pmc[i+1] ) {
								selch = i + PM1;
								sellr = 2;
							}
							else if ( pmc[i] && pmc[i+1] ) {
								selch = i + PM1;
								sellr = 0;
							}
							//
						}
						else {
							if ( Get_RunPrm_FM_PM_SEL_PM( selch , i + PM1 ) ) {
								//
								if      ( pmc[i] && !pmc[i+1] ) {
									selch = i + PM1;
									sellr = 1;
								}
								else if ( !pmc[i] && pmc[i+1] ) {
									selch = i + PM1;
									sellr = 2;
								}
								else if ( pmc[i] && pmc[i+1] ) {
									selch = i + PM1;
									sellr = 0;
								}
								//
							}
						}
					}
					//
					i++;
				}
				//
				data = _SCH_CLUSTER_Get_UserArea( side1 , pt1 );
				//
			}
			else if ( wfr2 > 0 ) {
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = 0;
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					ch = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , i );
					//
					if ( ch > 0 ) {
						//
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
								pmc[ch-PM1] = 1;
							}
						}
						//
					}
					//
				}
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					if ( selfailch1 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					if ( selfailch2 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					if ( selfailch3 == ( i + PM1 ) ) {
						i++; // 2016.08.17
						continue; // 2016.08.12
					}
					//
					if ( pmc[i] || pmc[i+1] ) {
						//
						if ( selch == -1 ) {
							//
							if      ( pmc[i] && !pmc[i+1] ) {
								selch = i + PM1;
								sellr = 1;
							}
							else if ( !pmc[i] && pmc[i+1] ) {
								selch = i + PM1;
								sellr = 2;
							}
							else if ( pmc[i] && pmc[i+1] ) {
								selch = i + PM1;
								sellr = 0;
							}
							//
						}
						else {
							if ( Get_RunPrm_FM_PM_SEL_PM( selch , i + PM1 ) ) {
								//
								if      ( pmc[i] && !pmc[i+1] ) {
									selch = i + PM1;
									sellr = 1;
								}
								else if ( !pmc[i] && pmc[i+1] ) {
									selch = i + PM1;
									sellr = 2;
								}
								else if ( pmc[i] && pmc[i+1] ) {
									selch = i + PM1;
									sellr = 0;
								}
								//
							}
						}
					}
					//
					i++;
				}
				//
				data = _SCH_CLUSTER_Get_UserArea( side2 , pt2 );
				//
			}
			else {
				return TRUE;
			}
			//
			if ( selch != -1 ) {
				//
				m = 0;
				//
				if ( data != NULL ) {
					//
					i = atoi( data );
					//
					switch( selch ) {
					case PM1 + 0 :
						m = i % 10;
						break;
					case PM1 + 1 :
						m = ( i / 10 ) % 10;
						break;
					case PM1 + 2 :
						m = ( i / 100 ) % 10;
						break;
					case PM1 + 3 :
						m = ( i / 1000 ) % 10;
						break;
					case PM1 + 4 :
						m = ( i / 10000 ) % 10;
						break;
					case PM1 + 5 :
						m = ( i / 100000 ) % 10;
						break;
					}
					//
				}
				//
				selrmode = _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( selch ); // 2015.05.27
				//
				if      ( selrmode == 0 ) {
					*rmode = sellr;
				}
				else if ( selrmode == 1 ) {
					//
//					if ( ( sellr != 0 ) && ( sellr != 1 ) ) return FALSE; // 2016.08.12
					//
					if ( ( sellr != 0 ) && ( sellr != 1 ) ) { // 2016.08.12
						//
						if ( selfailch1 == -1 ) {
							selfailch1 = selch;
							continue;
						}
						if ( selfailch2 == -1 ) {
							selfailch2 = selch;
							continue;
						}
						if ( selfailch3 == -1 ) {
							selfailch3 = selch;
							continue;
						}
						//
						return FALSE;
						//
					}
					//
					*rmode = selrmode; // 2016.08.17
					//
				}
				else if ( selrmode == 2 ) {
					//
//					if ( ( sellr != 0 ) && ( sellr != 2 ) ) return FALSE; // 2016.08.12
					//
					if ( ( sellr != 0 ) && ( sellr != 2 ) ) { // 2016.08.12
						//
						if ( selfailch1 == -1 ) {
							selfailch1 = selch;
							continue;
						}
						if ( selfailch2 == -1 ) {
							selfailch2 = selch;
							continue;
						}
						if ( selfailch3 == -1 ) {
							selfailch3 = selch;
							continue;
						}
						//
						return FALSE;
						//
					}
					//
					*rmode = selrmode; // 2016.08.17
					//
				}
				//
				if ( wfr1 > 0 ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						//
						ch = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , i );
						//
						if ( ch > 0 ) {
							//
	//						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12 // 2016.07.19
	//							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12 // 2016.07.19
									if ( ( ch != selch ) && ( ch != (selch+1) ) ) {
										_SCH_CLUSTER_Set_PathProcessChamber_Disable( side1 , pt1 , 0 , i );
									}
	//							}
	//						}
						}
						//
					}
				}
				//
				if ( ( wfr1 > 0 ) && ( wfr2 > 0 ) && ( *rmode != 0 ) ) { // 2015.05.27
				}
				else {
					if ( wfr2 > 0 ) {
						for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
							//
							ch = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , i );
							//
							if ( ch > 0 ) {
								//
	//							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12 // 2016.07.19
	//								if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12 // 2016.07.19
										if ( ( ch != selch ) && ( ch != (selch+1) ) ) {
											_SCH_CLUSTER_Set_PathProcessChamber_Disable( side2 , pt2 , 0 , i );
										}
	//								}
	//							}
								//
							}
						}
					}
				}
				//
				Set_RunPrm_FM_PM_SEL_PM( selch );
				//
			}
			else { // 2016.08.12
				return FALSE;
			}
			//
			break; // 2016.08.12
			//
		}
		//
	}
	else {
		if ( wfr1 > 0 ) {
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				//
				ch = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , i );
				//
				if ( ch > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							if ( selch == -1 ) {
								selch = ch;
							}
							else {
								if ( Get_RunPrm_FM_PM_SEL_PM( selch , ch ) ) {
									selch = ch;
								}
							}
						}
					}
				}
				//
			}
			//
			data = _SCH_CLUSTER_Get_UserArea( side1 , pt1 );
			//
		}
		else if ( wfr2 > 0 ) {
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				//
				ch = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , i );
				//
				if ( ch > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							//
							if ( selch == -1 ) {
								selch = ch;
							}
							else {
								if ( Get_RunPrm_FM_PM_SEL_PM( selch , ch ) ) {
									selch = ch;
								}
							}
						}
					}
				}
				//
			}
			//
			data = _SCH_CLUSTER_Get_UserArea( side2 , pt2 );
			//
		}
		else {
			return TRUE;
		}
		//
		if ( selch != -1 ) {
			//
			m = 0;
			//
			if ( data != NULL ) {
				//
				i = atoi( data );
				//
				switch( selch ) {
				case PM1 + 0 :
					m = i % 10;
					break;
				case PM1 + 1 :
					m = ( i / 10 ) % 10;
					break;
				case PM1 + 2 :
					m = ( i / 100 ) % 10;
					break;
				case PM1 + 3 :
					m = ( i / 1000 ) % 10;
					break;
				case PM1 + 4 :
					m = ( i / 10000 ) % 10;
					break;
				case PM1 + 5 :
					m = ( i / 100000 ) % 10;
					break;
				}
				//
			}
			//
//			*rmode = _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( selch ); // 2015.05.27 // 2016.09.01
			//
			switch( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) { // 2016.09.01
			case 0 :
				*rmode = 0;
				break;
			default :
				*rmode = _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( selch );
				break;
			}
			//
			if ( wfr1 > 0 ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					ch = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , i );
					//
					if ( ch > 0 ) {
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side1 , pt1 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
								//
								if ( ch != selch ) {
									_SCH_CLUSTER_Set_PathProcessChamber_Disable( side1 , pt1 , 0 , i );
								}
							}
						}
					}
					//
				}
			}
			//
			if ( ( wfr1 > 0 ) && ( wfr2 > 0 ) && ( *rmode != 0 ) ) { // 2015.05.27
			}
			else {
				if ( wfr2 > 0 ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						//
						ch = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , i );
						//
						if ( ch > 0 ) {
							if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
								if ( _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pt2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
									//
									if ( ch != selch ) {
										_SCH_CLUSTER_Set_PathProcessChamber_Disable( side2 , pt2 , 0 , i );
									}
								}
							}
						}
						//
					}
				}
			}
			//
			Set_RunPrm_FM_PM_SEL_PM( selch );
			//
		}
		else { // 2016.08.12
			//
			return FALSE;
			//
		}
		//
	}
	//
	return TRUE;
}

BOOL SCHEDULER_FM_HAS_PICK_PM() { // 2014.11.25
	int dum_pt , dum_rcm;
	int pm , s1 , p1 , w1 , s2 , p2 , w2;
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return FALSE;
	//
	if ( SCHEDULER_FM_PM_GET_PICK_PM( 9 , -1 , -1 , &pm , &dum_pt , &w1 , &s1 , &p1 , &dum_rcm , &w2 , &s2 , &p2 ) > 0 ) return TRUE;
	//
	return FALSE;
	//
}
int SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( int side , int *rcm , int *rpt , int mode ); // 2010.09.28

int SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( int s , int p , BOOL whenalldis , BOOL dataonly , int log ); // 2015.06.10

BOOL SCHEDULER_FM_PM_PICK_CM_TO_PM( int side ) { // 2014.11.06
	int ret_rcm , ret_pt;
	int pm , FM_CM;
	int FM_dbSide , FM_dbSide2 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , retpointer , FM_HSide , FM_HSide2 , cpreject;
	int i , x , Result , Result2;
	int check_side , check_pt , check_side2 , check_pt2;
	int FM_SlotN , FM_Slotx;
	int FM_PointerN , FM_Pointerx;
	int FM_HSideN , FM_HSidex;
	int cprejectN , retpointerN;
	int FM_SideN;
	int rmode , pmlot1 , pmlot2;
	BOOL NoMore; // 2015.06.22
	int Act , pc1 , pc2; // 2015.11.20
	//
	int FM_SlotN_2 , FM_Slotx_2; // 2016.05.12
	int FM_PointerN_2 , FM_Pointerx_2; // 2016.05.12
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return TRUE;
	//
	if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( side , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) return TRUE;
	//
	if ( !SCHEDULER_FM_PM_GET_PLACE_PM( &pm ) ) return TRUE;
	//
	if ( _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , side , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_HSide , &FM_HSide2 , &cpreject ) < 0 ) return TRUE;
	//
	FM_dbSide = side;
	FM_dbSide2 = side;
	//
	if ( FM_Slot > 0 ) { // 2014.11.25
		if ( SCHEDULER_FM_PM_DIRECT_MODE( FM_dbSide , FM_Pointer , &NoMore ) ) {
			//
			if ( NoMore ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( FM_dbSide , FM_Pointer , FALSE , TRUE , 20 ); // 2015.06.22
			//
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , cpreject );
			//
			return TRUE;
		}
		//
		if ( NoMore ) { // 2015.06.22
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( FM_dbSide , FM_Pointer , FALSE , TRUE , 21 );
			//
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , cpreject );
			//
			return TRUE;
		}
		//
	}
	else if ( FM_Slot2 > 0 ) { // 2014.11.25
		if ( SCHEDULER_FM_PM_DIRECT_MODE( FM_dbSide2 , FM_Pointer2 , &NoMore ) ) {
			//
			if ( NoMore ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , 22 ); // 2015.06.10
			//
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , cpreject );
			//
			return TRUE;
		}
		//
		if ( NoMore ) { // 2015.06.22
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , 23 );
			//
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , cpreject );
			//
			return TRUE;
		}
		//
	}
	//
//	SCHEDULER_FM_PM_DIRECT_RUN( 0 , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , &rmode ); // 2015.05.27 // 2015.11.10
	if ( !SCHEDULER_FM_PM_DIRECT_RUN( 0 , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , &rmode ) ) return TRUE; // 2015.05.27 // 2015.11.10
	//
	//
	pmlot1 = 1;
	pmlot2 = 2;
	//
	if      ( rmode == 1 ) {
		if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
			FM_Slot2 = 0;
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer );
		}
		else if ( FM_Slot > 0 ) {
		}
		else if ( FM_Slot2 > 0 ) {
			pmlot2 = 1;
		}
	}
	else if ( rmode == 2 ) {
		if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
			//
			_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer );
			//
			FM_Slot2 = FM_Slot;			FM_Slot = 0;
			FM_dbSide2 = FM_dbSide;		FM_dbSide = 0;
			FM_Pointer2 = FM_Pointer;	FM_Pointer = 0;
			//
		}
		else if ( FM_Slot > 0 ) {
			pmlot1 = 2;
		}
		else if ( FM_Slot2 > 0 ) {
		}
	}
	//
	//----------------------------------------------------------------------------------------------------------------------------------------------
	// User Interface
	// 2016.05.10
	//----------------------------------------------------------------------------------------------------------------------------------------------
	switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 1 , FM_dbSide , FM_Pointer , FM_Slot , FM_dbSide2 , FM_Pointer2 , FM_Slot2 , 0 ) ) {
	case 0 :
		return FALSE;
		break;
	case 1 :
		return TRUE;
		break;
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------
	//
	//----------------------------------------------
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
			if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
				//==========================================================================================
				if ( _SCH_MODULE_Chk_FM_Finish_Status( FM_dbSide ) ) _SCH_SYSTEM_LASTING_SET( FM_dbSide , TRUE );
				//==========================================================================================
			}
		}
	}
	//----------------------------------------------------------------------
	if ( FM_Slot  > 0 ) {
		//---------------------------------------------------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
			//=============================================================================================
			SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide , FM_Pointer ) );
			//=============================================================================================
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 0 , 0 , 0 );
		}
		else {
			if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer ,  0 ,  0 , 0 );
			}
			else {
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 20 , 20 , 2 );
			}
		}
		//---------------------------------------------------------------------------------------------------------------
	}
	if ( FM_Slot2 > 0 ) {
		//---------------------------------------------------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
			//=============================================================================================
			SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide2 , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) );
			//=============================================================================================
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 );
		}
		else {
			if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  0 ,  0 , 0 );
			}
			else {
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 20 , 20 , 2 );
			}

		}
		//---------------------------------------------------------------------------------------------------------------
	}
	//==========================================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
		//
		if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
			if ( FM_dbSide == FM_dbSide2 ) {
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 2  + 21000 );
			}
			else {
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 22000 );
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 23000 );
			}
		}
		else if ( FM_Slot > 0 ) {
			_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 24000 );
		}
		else if ( FM_Slot2 > 0 ) {
			_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 25000 );
		}
		//
	}
	//-----------------------------------------------
	_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
	//----------------------------------------------
	//
	if ( FM_Slot  > 0 ) {
		_SCH_MODULE_Inc_FM_OutCount( FM_dbSide );
		//--------------------------
		_SCH_MODULE_Set_FM_LastOutPointer( FM_dbSide , FM_Pointer );
		//
	}
	if ( FM_Slot2 > 0 ) {
		_SCH_MODULE_Inc_FM_OutCount( FM_dbSide2 );
		//--------------------------
		_SCH_MODULE_Set_FM_LastOutPointer( FM_dbSide2 , FM_Pointer2 );
		//
	}
	//
	//----------------------------------------------------------------------
	// Code for Pick from CM
	// FM_Slot
	//----------------------------------------------------------------------
	switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_dbSide , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) {
	case -1 :
		return FALSE;
		break;
	}
	//
	//----------------------------------------------------------------------------------------------------------------------------
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_dbSide , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL );
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
//	if ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) { // 2015.05.29
	if ( ( rmode == 0 ) && ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) ) { // 2015.05.29
		//
		if ( FM_Slot > 0 ) {
			check_side = FM_dbSide;
			check_pt = FM_Pointer;
		}
		else {
			check_side = FM_dbSide2;
			check_pt = FM_Pointer2;
		}
		//
		FM_SlotN = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//==================================================================================================
			if ( check_side == i ) continue;
			//==========================================================================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( i , 3 );
			//==========================================================================================
			if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) {
				//
				FM_SideN = i;
				//======================================================================================
				Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , i , &FM_SlotN , &FM_Slotx , &FM_PointerN , &FM_Pointerx , &retpointerN , &FM_HSideN , &FM_HSidex , &cprejectN );
				//======================================================================================
				if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( i , FM_PointerN ) ) == 3 ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointerN );
						FM_SlotN = 0;
					}
				}
				else {
					if ( Result < 0 ) {
						FM_SlotN = 0;
					}
				}
				//
				if ( FM_SlotN > 0 ) {
					//
					for ( x = 0 ; x < 6 ; x++ ) {
						//
						if ( x != 0 ) Sleep(500);
						//
						if ( _SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( i , FM_PointerN ) ) break;
						//
					}
					//
					if ( x == 6 ) {
						//
						_SCH_MODULE_Set_FM_DoPointer( i , retpointerN );
						FM_SlotN = 0;
						//
					}
					//
				}
				//======================================================================================
				if ( FM_SlotN > 0 ) {
					if ( check_side >= 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( i , FM_PointerN , check_side , check_pt ) ) {
							_SCH_MODULE_Set_FM_DoPointer( i , retpointerN );
							FM_SlotN = 0;
						}
						else {
							if ( i != check_side ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) {
									_SCH_MODULE_Set_FM_DoPointer( i , retpointerN );
									FM_SlotN = 0;
								}
							}
						}
					}
				}
				//======================================================================================
				if ( FM_SlotN > 0 ) break;
				//======================================================================================
			}
		}
		//
		if ( FM_SlotN > 0 ) {
			//---------------------------------------------------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
				//=============================================================================================
				SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_SideN , _SCH_CLUSTER_Get_ClusterIndex( FM_SideN , FM_PointerN ) );
				//=============================================================================================
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_SideN , FM_PointerN , 0 , 0 , 0 );
			}
			else {
				if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_SideN , FM_PointerN ,  0 ,  0 , 0 );
				}
				else {
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_SideN , FM_PointerN , 20 , 20 , 2 );
				}

			}
			//---------------------------------------------------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
				//
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_SideN , 1  + 25000 );
				//
			}
			//---------------------------------------------------------------------------------------------------------------
			_SCH_MODULE_Inc_FM_OutCount( FM_SideN );
			//--------------------------
			_SCH_MODULE_Set_FM_LastOutPointer( FM_SideN , FM_PointerN );
			//---------------------------------------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------------------------------------
			if ( FM_Slot <= 0 ) {
				//
				FM_Slot = FM_SlotN;
				FM_dbSide = FM_SideN;
				FM_Pointer = FM_PointerN;
				//
				switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_dbSide , FM_Pointer , -1 , 0 , 0 , 0 , 0 , -1 ) ) {
				case -1 :
					return FALSE;
					break;
				}
				//
				//----------------------------------------------------------------------------------------------------------------------------
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_dbSide , FM_Pointer , -1 , 0 , 0 , 0 , 0 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL );
				//----------------------------------------------------------------------
				//
			}
			else {
				//
				FM_Slot2 = FM_SlotN;
				FM_dbSide2 = FM_SideN;
				FM_Pointer2 = FM_PointerN;
				//
				switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) {
				case -1 :
					return FALSE;
					break;
				}
				//
				//----------------------------------------------------------------------------------------------------------------------------
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL );
				//----------------------------------------------------------------------
				//
			}
			//
		}
		//
	}
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
		//
		//=========================================================================================================
		_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );
		_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );
		//=========================================================================================================
		//
		if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide , FM_Pointer ) ) {
			//
			if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) {
				//
			}
			else {
				//
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , pm , TRUE , 0 , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , 0 , FM_Slot2 , 9 , 0 , FM_Slot2 , 9 , FM_Slot2 * 100 + 0 );
					return FALSE;
				}
				//
			}
			//
		}
		else {
			//
			if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) {
				//
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide , AL , FM_Slot , 0 , FM_CM , pm , TRUE , FM_Slot , 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , 0 , 9 , FM_Slot , 0 , 9 , 0 * 100 + FM_Slot );
					return FALSE;
				}
				//
			}
			else {
				//
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide , AL , FM_Slot , FM_Slot2 , FM_CM , pm , TRUE , FM_Slot , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
					return FALSE;
				}
				//
			}
			//
		}
	}
	else if ( FM_Slot > 0 ) {
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
		//
		//=========================================================================================================
		_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );
		//=========================================================================================================
		//
		if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide , FM_Pointer ) ) {
			//
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide , AL , FM_Slot , 0 , FM_CM , pm , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , 0 , 9 , FM_Slot , 0 , 9 , 0 * 100 + FM_Slot );
				return FALSE;
			}
			//
		}
		//
	}
	else if ( FM_Slot2 > 0 ) {
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 );
		//
		//=========================================================================================================
		_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );
		//=========================================================================================================
		//
		if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) {
			//
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , pm , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , 0 , FM_Slot2 , 9 , 0 , FM_Slot2 , 9 , FM_Slot2 * 100 + 0 );
				return FALSE;
			}
			//
		}
		//
	}
	//
//	SCHEDULER_FM_PM_DIRECT_RUN( 0 , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 ); // 2015.05.27
	//
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	if ( _SCH_MACRO_FM_OPERATION( 0 , 460 + MACRO_PLACE , pm , FM_Slot , pmlot1 , FM_dbSide , FM_Pointer , pm , FM_Slot2 , pmlot2 , FM_dbSide2 , FM_Pointer2 , FALSE ) == -1 ) {
		return FALSE;
	}
	//
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , pm , FM_Slot , pmlot1 , FM_dbSide , FM_Pointer , pm , FM_Slot2 , pmlot2 , FM_dbSide2 , FM_Pointer2 , 0 , 0 );
	//
	if ( FM_Slot > 0 ) {
		check_side = FM_dbSide;
		check_pt = FM_Pointer;
		//
		if ( FM_Slot2 > 0 ) {
			check_side2 = FM_dbSide2;
			check_pt2 = FM_Pointer2;
		}
		else {
			check_side2 = -1;
			check_pt2 = -1;
		}
	}
	else {
		check_side = FM_dbSide2;
		check_pt = FM_Pointer2;
		//
		check_side2 = -1;
		check_pt2 = -1;
	}
	//
	FM_SlotN = -1;
	FM_PointerN = -1;
	//
	FM_Slotx = -1; // 2015.11.20
	FM_Pointerx = -1; // 2015.11.20
	//
	FM_SlotN_2 = -1; // 2016.05.12
	FM_Slotx_2 = -1; // 2016.05.12
	//
	Result = -1;
	Result2 = -1; // 2015.11.20
	//
	switch( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() ) { // 2015.11.20
	case 3 :
		Act = 1;
		break;
	default :
		Act = 0;
		break;
	}
	//
	if ( Act == 1 ) {
		//
		if ( FM_Slot > 0 ) {
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
				//
				x =  _SCH_CLUSTER_Get_PathProcessChamber( FM_dbSide , FM_Pointer , 0 , i );
				//
				if ( x > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide , FM_Pointer , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide , FM_Pointer , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							if ( ( ( x - PM1 ) % 2 ) == 0 ) { // pm1
								if ( FM_SlotN == -1 ) {
									FM_SlotN = x;
									FM_PointerN = i;
								}
							}
							else { // 2016.05.12
								if ( FM_SlotN_2 == -1 ) {
									FM_SlotN_2 = x;
									FM_PointerN_2 = i;
								}
							}
						}
					}
					//
				}
				else if ( x == 0 ) {
					if ( Result == -1 ) {
						Result = i;
					}
				}
				//
			}
		}
		//
		if ( FM_Slot2 > 0 ) {
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
				//
				x =  _SCH_CLUSTER_Get_PathProcessChamber( FM_dbSide2 , FM_Pointer2 , 0 , i );
				//
				if ( x > 0 ) {
					//
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide2 , FM_Pointer2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
						if ( _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide2 , FM_Pointer2 , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
							if ( ( ( x - PM1 ) % 2 ) != 0 ) { // pm2
								if ( FM_Slotx == -1 ) {
									FM_Slotx = x;
									FM_Pointerx = i;
								}
							}
							else { // 2016.05.12
								if ( FM_Slotx_2 == -1 ) {
									FM_Slotx_2 = x;
									FM_Pointerx_2 = i;
								}
							}
						}
					}
					//
				}
				else if ( x == 0 ) {
					if ( Result2 == -1 ) {
						Result2 = i;
					}
				}
				//
			}
		}
		//
		if ( ( FM_Slot > 0 ) && ( FM_SlotN < 0 ) && ( FM_SlotN_2 >= 0 ) ) { // 2016.05.12
			FM_SlotN = FM_SlotN_2;
			FM_PointerN = FM_PointerN_2;
		}
		//
		if ( ( FM_Slot2 > 0 ) && ( FM_Slotx < 0 ) && ( FM_Slotx_2 >= 0 ) ) { // 2016.05.12
			FM_Slotx = FM_Slotx_2;
			FM_Pointerx = FM_Pointerx_2;
		}
		//
		if ( ( FM_SlotN >= 0 ) && ( Result >= 0 ) ) {
			//
			pc1 = FM_SlotN;
			//
			_SCH_CLUSTER_Set_PathProcessData( FM_dbSide , FM_Pointer , 0 , Result , pm , _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide , FM_Pointer , 0 , FM_PointerN , 0 ) , "" , "" );
			//
		}
		else {
			pc1 = 0;
		}
		//
		if ( ( FM_Slotx >= 0 ) && ( Result2 >= 0 ) ) {
			//
			pc2 = FM_Slotx;
			//
			_SCH_CLUSTER_Set_PathProcessData( FM_dbSide2 , FM_Pointer2 , 0 , Result2 , pm + 1 , _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide2 , FM_Pointer2 , 0 , FM_Pointerx , 0 ) , "" , "" );
			//
		}
		else {
			pc2 = 0;
		}
		//
		if ( FM_Slot > 0 ) {
			_SCH_MACRO_MAIN_PROCESS( FM_dbSide , FM_Pointer ,
				  pm + ( pc1 * 1000 ) , -1 ,
				  FM_Slot , -1 , 0 , -1 ,
				  ( pc1 == 0 ) ? "?" : _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide , FM_Pointer , 0 , Result , 0 ) ,
				  0 , "" , 0 ,
				  0 , Result , 0 ,
				  -1 , -1 , -1 , 0 ,
				  0 , 101 );
		}
		//
		if ( FM_Slot2 > 0 ) {
			_SCH_MACRO_MAIN_PROCESS( FM_dbSide2 , FM_Pointer2 ,
				  ( pm + 1 ) + ( pc2 * 1000 ) , -1 ,
				  FM_Slot2 , -1 , 0 , -1 ,
				  ( pc2 == 0 ) ? "?" : _SCH_CLUSTER_Get_PathProcessRecipe( FM_dbSide2 , FM_Pointer2 , 0 , Result2 , 0 ) ,
				  0 , "" , 0 ,
				  0 , Result , 0 ,
				  -1 , -1 , -1 , 0 ,
				  0 , 101 );
		}
		//
	}
	else {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
			//
			x =  _SCH_CLUSTER_Get_PathProcessChamber( check_side , check_pt , 0 , i );
			//
			if ( x > 0 ) {
				//
				if ( _SCH_CLUSTER_Get_PathProcessRecipe( check_side , check_pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN ) != NULL ) { // 2016.05.12
					if ( _SCH_CLUSTER_Get_PathProcessRecipe( check_side , check_pt , 0 , i , SCHEDULER_PROCESS_RECIPE_IN )[0] != 0 ) { // 2016.05.12
						if ( FM_SlotN == -1 ) {
							FM_SlotN = x;
							FM_PointerN = i;
						}
					}
				}
			}
			else if ( x == 0 ) {
				if ( Result == -1 ) {
					Result = i;
				}
			}
			//
		}
		//
		if ( ( FM_SlotN >= 0 ) && ( Result >= 0 ) ) {
			//
			i = FM_SlotN;
			//
			_SCH_CLUSTER_Set_PathProcessData( check_side , check_pt , 0 , Result , pm , _SCH_CLUSTER_Get_PathProcessRecipe( check_side , check_pt , 0 , FM_PointerN , 0 ) , "" , "" );
			//
		}
		else {
			i = 0;
		}
		//
		_SCH_MACRO_MAIN_PROCESS( check_side , check_pt ,
			  pm + ( i * 1000 ) , -1 ,
			  ( FM_Slot2 * 100 ) + FM_Slot , -1 , 0 , -1 ,
			  ( i == 0 ) ? "?" : _SCH_CLUSTER_Get_PathProcessRecipe( check_side , check_pt , 0 , Result , 0 ) ,
			  0 , "" , 0 ,
			  0 , Result , 0 ,
			  check_side2 , check_pt2 , -1 , 0 ,
			  0 , 101 );
		//
	}
	//----------------------------------------------------------------------
	return TRUE;
	//
}

//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================


BOOL SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( int side , int ch ) { // 2009.03.11
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
		return FALSE;
	}
	else {
		if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != 0 ) {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != ch ) return TRUE;
		}
	}
	return FALSE;
}
//

int Sch_NoWay_Option() { // 2010.12.08
	switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
	case 6 :
	case 7 :
		return -2;
		break;
	default :
		//
		if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) return -2; // 2016.07.22
		if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) return -2; // 2018.04.10
		//
		return -1;
		break;
	}
}

BOOL Scheduler_Other_BM_Make_TMSide( int side , int waitbmforfmplace , int *actwaitcheck ) { // 2011.05.18
	//
	int i , selbm;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) { *actwaitcheck = 0; return TRUE; } // 2018.11.07
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) { // 2018.11.07
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) { // 2018.11.07
			*actwaitcheck = 0; return TRUE;
		}
	}
	//
	if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( waitbmforfmplace ) ) { *actwaitcheck = 0; return TRUE; }
	if ( _SCH_MODULE_Get_BM_FULL_MODE( waitbmforfmplace ) != BUFFER_TM_STATION ) { *actwaitcheck = 0; return TRUE; }
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( waitbmforfmplace ) > 0 ) { *actwaitcheck = 0; return TRUE; }
	//
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( waitbmforfmplace , &i , &i , 0 , FALSE ) > 0 ) { *actwaitcheck = 0; return TRUE; }
	//
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( waitbmforfmplace , FALSE ) == SYS_RUNNING ) { *actwaitcheck = 0; return TRUE; }
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( TM , FALSE ) == SYS_RUNNING ) { *actwaitcheck = 0; return TRUE; }
	//
	selbm = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//----------------------------------------------------------------------
		if ( waitbmforfmplace == i ) continue;
		//----------------------------------------------------------------------
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//----------------------------------------------------------------------
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) { *actwaitcheck = 0; return TRUE; }
		if ( !SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( i ) ) { *actwaitcheck = 0; return TRUE; }
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) { *actwaitcheck = 0; return TRUE; }
		//----------------------------------------------------------------------
		selbm = i;
		//----------------------------------------------------------------------
	}
	//
	if ( selbm != -1 ) {
		//=============================================================================================================================
		(*actwaitcheck)++;
		//----------------------------------------------------------------------
		if ( (*actwaitcheck) <= 30 ) return TRUE;
		//=============================================================================================================================
		//
		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , selbm ); // 2017.02.08
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm , -1 , TRUE , 0 , 1800 );
		//=============================================================================================================================
		*actwaitcheck = 0;
		//=============================================================================================================================
	}
	//
	return TRUE;
	//
}

// 2017.01.20
extern int Half_Placed_CM_SIDE;
extern int Half_Placed_CM_IC_SLOT1;
extern int Half_Placed_CM_IC_SLOT2;
extern int Half_Placed_CM_FM_CM;
extern int Half_Placed_CM_Slot1;
extern int Half_Placed_CM_Slot2;
extern int Half_Placed_CM_Side1;
extern int Half_Placed_CM_Side2;
extern int Half_Placed_CM_Pt1;
extern int Half_Placed_CM_Pt2;

//BOOL Scheduler_Other_Arm_Cool_Return( int side , BOOL *act ) { // 2010.08.27 // 2012.08.10
//BOOL Scheduler_Other_Arm_Cool_Return( int side , BOOL coolyes , BOOL *act ) { // 2010.08.27 // 2012.08.10 // 2015.03.25
//BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02
//BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act , int waitplacebm , int waitarm ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act , int waitplacebm , int waitarm , BOOL noplacecm , BOOL *noplacedcm  ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
	int Arm , s , w , pt , Slot , i , k , m , ICsts , ICsts2 , FM_CM , Result;
	int selbm , selslot , sels , selp , seld , selw , gocm;
	BOOL blockit; // 2016.05.02
	int outbm; // 2016.05.17
	int outbm1bmc = 0; // 2017.02.18 L.A.M 확인중
	int lockbm , lockbmhastm; // 2016.06.03
	//
	*act = FALSE;
	//
	blockit = FALSE; // 2016.05.02
	outbm = 0; // 2016.05.17
	lockbm = 0; // 2016.06.03
	lockbmhastm = FALSE; // 2016.06.03
	//
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 1 [%d][%d][%d]\n" , skipbm , coolyes , waitplacebm );

	if ( coolyes ) { // 2012.08.10
//		if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) return TRUE; // 2016.05.02
		//
		if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) { // 2016.05.02
			//
			if ( waitplacebm <= 0 ) return TRUE;
			//
			// 2016.05.17
			//
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2016.05.17
				if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//----------------------------------------------------------------------
						if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
						if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
						if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
						if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
						//----------------------------------------------------------------------
						if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
						//----------------------------------------------------------------------
						//
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( i ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) {
									if ( waitplacebm == i ) {
										outbm = 1;
										break;
									}
									else {
										if ( outbm == 0 ) outbm = 2;
									}
								}
								else { // 2017.02.18 L.A.M 확인중
									if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) {
										if ( waitplacebm != i ) {
											if ( outbm == 0 ) {
												outbm = 2;
												outbm1bmc = i;
											}
										}
									}
								}
							}
						}
						else { // 2016.06.03
							if ( waitplacebm > 0 ) {
								if ( waitplacebm != i ) {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) {
											if ( lockbm == 0 ) lockbm = i;
										}
									}
								}
							}
						}
						//
					}
					//
				}
			}
			//
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 11 [%d][%d]\n" , lockbm , outbm );
			//
			if ( outbm == 0 ) {
				//
				if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) {
					if ( ( _SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION ) > 0 ) ) {
						//
						if ( ( _SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION ) == _SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION ) ) ) return TRUE;
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION ) , waitplacebm ) == MUF_00_NOTUSE ) {
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION ) , waitplacebm ) == MUF_00_NOTUSE ) {
								return TRUE;
							}
						}
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION ) , waitplacebm ) == MUF_01_USE ) {
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION ) , waitplacebm ) == MUF_01_USE ) {
								return TRUE;
							}
						}
						//
						if ( lockbm > 0 ) { // 2016.06.03
							//
							sels = _SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION );
							selp = _SCH_MODULE_Get_TM_POINTER( 0 , TA_STATION );
							seld = _SCH_CLUSTER_Get_PathDo( sels , selp );
							//
							if ( seld > _SCH_CLUSTER_Get_PathRange( sels , selp ) ) {
								//
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
										lockbmhastm = TRUE;
									}
								}
								//
							}
							//
							sels = _SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION );
							selp = _SCH_MODULE_Get_TM_POINTER( 0 , TB_STATION );
							seld = _SCH_CLUSTER_Get_PathDo( sels , selp );
							//
							if ( seld > _SCH_CLUSTER_Get_PathRange( sels , selp ) ) {
								//
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
										lockbmhastm = TRUE;
									}
								}
								//
							}
							//
						}
						//
					}
					else if ( _SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION ) > 0 ) { // 2016.06.07
						//
						if ( lockbm > 0 ) {
							//
							sels = _SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION );
							selp = _SCH_MODULE_Get_TM_POINTER( 0 , TA_STATION );
							seld = _SCH_CLUSTER_Get_PathDo( sels , selp );
							//
							if ( seld <= _SCH_CLUSTER_Get_PathRange( sels , selp ) ) {
								//
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									//
									m = _SCH_CLUSTER_Get_PathProcessChamber( sels , selp , seld - 1 , k );
									//
									if ( m > 0 ) {
										//
										if ( SCH_PM_IS_ABSENT( m , sels , selp , seld - 1 ) ) break;
										//
									}
								}
								//
								if ( k == MAX_PM_CHAMBER_DEPTH ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
											lockbmhastm = TRUE;
										}
									}
								}
								//
							}
							else { // 2016.06.23
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
										lockbmhastm = TRUE;
									}
								}
							}
							//
						}
						//
					}
					else if ( _SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION ) > 0 ) { // 2016.06.07
						//
						if ( lockbm > 0 ) {
							//
							sels = _SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION );
							selp = _SCH_MODULE_Get_TM_POINTER( 0 , TB_STATION );
							seld = _SCH_CLUSTER_Get_PathDo( sels , selp );
							//
							if ( seld <= _SCH_CLUSTER_Get_PathRange( sels , selp ) ) {
								//
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									//
									m = _SCH_CLUSTER_Get_PathProcessChamber( sels , selp , seld - 1 , k );
									//
									if ( m > 0 ) {
										//
										if ( SCH_PM_IS_ABSENT( m , sels , selp , seld - 1 ) ) break;
										//
									}
								}
								//
								if ( k == MAX_PM_CHAMBER_DEPTH ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
											lockbmhastm = TRUE;
										}
									}
								}
								//
							}
							else { // 2016.06.23
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , lockbm ) == MUF_01_USE ) {
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( sels , waitplacebm ) == MUF_00_NOTUSE ) {
										lockbmhastm = TRUE;
									}
								}
							}
							//
						}
						//
					}
				}
				else {
					return TRUE;
				}
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 12 [%d][%d][%d]\n" , lockbm , lockbmhastm , outbm );
				//
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//----------------------------------------------------------------------
					if ( waitplacebm == i ) continue;
					//----------------------------------------------------------------------
					if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
					if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
					//----------------------------------------------------------------------
//					if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) continue; // 2016.06.03
					if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue; // 2016.06.03
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
					//----------------------------------------------------------------------
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_FM_SIDE( waitarm ) , i ) != MUF_00_NOTUSE ) return TRUE;
					//
					break;
					//
				}
				//
				if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) return TRUE;
				//
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 13 [%d][%d][%d]\n" , lockbm , lockbmhastm , outbm );
				if ( ( lockbm > 0 ) && lockbmhastm ) { // 2016.06.03
					//=============================================================================================================================
					//
					SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , lockbm ); // 2017.02.08
					//
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , lockbm , -1 , TRUE , 0 , 1818 );
					//=============================================================================================================================
					return TRUE;
					//
				}
				//
			}
			//
			blockit = TRUE;
			//
		}
		//
	}
	//
	if ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) return TRUE;
	//
	//======================================================================================================
	if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
		Arm = TA_STATION;
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
		Arm = TB_STATION;
	}
	else {
		return TRUE;
	}
	//
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 1 = Arm=%d,blockit=%d,outbm=%d\n" , Arm , blockit , outbm );
	//==============================================================================================
	//
	selbm = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( outbm == 1 ) { // 2016.05.17
			//----------------------------------------------------------------------
			if ( waitplacebm != i ) continue;
			//----------------------------------------------------------------------
		}
		else {
			//----------------------------------------------------------------------
			if ( skipbm == i ) continue; // 2015.03.25
			//----------------------------------------------------------------------
		}
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//----------------------------------------------------------------------
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2012.08.10
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2011.10.12
				//
				// 2017.02.18 L.A.M 확인중
				//
//				if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) return TRUE;
				//
				if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) {
					//
					if ( outbm != 2 ) return TRUE;
					if ( waitplacebm == i ) return TRUE;
					if ( outbm1bmc != i ) return TRUE;
					//
				}
				//
			}
		}
		//----------------------------------------------------------------------
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//----------------------------------------------------------------------
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2012.08.10
			if      ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
				Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , -1 , &Slot , &pt , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) );
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
				Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , -1 , &Slot , &pt , 3 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) );
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
				Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , -1 , &Slot , &pt , 4 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) );
			}
			else {
				Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , -1 , &Slot , &pt , 1 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) );
			}
		}
		else { // 2012.08.10
			Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE2( i , -1 , &Slot , &pt , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) );
		}
		//
		if ( Result <= 0 ) continue;
		//----------------------------------------------------------------------
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) < 0 ) return FALSE; // 2013.06.28
		//----------------------------------------------------------------------
		s = _SCH_MODULE_Get_BM_SIDE( i , Slot );
		w = _SCH_MODULE_Get_BM_WAFER( i , Slot );
		//
		if ( !_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( s , pt ) ) continue; // 2013.05.27
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( s , i ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , i ) == MUF_00_NOTUSE ) continue;
		//----------------------------------------------------------------------
		if ( selbm == -1 ) {
			selbm = i;
			selslot = Slot;
			sels = s;
			selp = pt;
			selw = w;
		}
		else {
			if ( _SCH_CLUSTER_Get_SupplyID( s , pt ) < _SCH_CLUSTER_Get_SupplyID( sels , selp ) ) {
				selbm = i;
				selslot = Slot;
				sels = s;
				selp = pt;
				selw = w;
			}
		}
	}
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 2 = Arm=%d,selbm=%d,selslot=%d,sels=%d,selp=%d,selw=%d\n" , Arm , selbm , selslot , sels , selp , selw );
	//==============================================================================================
	//=======================================================================================
	// PICK BM to IC
	//=======================================================================================
	gocm = 0;
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) gocm = 2; // 2012.08.10
	//
	if ( selbm != -1 ) {
		//
		if ( gocm == 0 ) { // 2012.08.10
			//
			if ( _SCH_CLUSTER_Get_PathDo( sels , selp ) == PATHDO_WAFER_RETURN ) gocm = 1;
			if ( SCHEDULER_COOLING_SKIP_AL0( sels , selp ) ) gocm = 1;
			//
			pt = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( sels , selp , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
		}
		else { // 2012.08.10
			gocm = 1;
		}
		//
		if ( ( pt > 0 ) || ( gocm == 1 ) ) {
			//
			*act = TRUE;
			//=======================================================================================
			// PICK BM
			//=======================================================================================
			if ( Arm == TA_STATION ) {
				//=============================================================================================
				if ( _SCH_MACRO_FM_OPERATION( 0 , 110 + MACRO_PICK , selbm , selw , selslot , sels , selp , 0 , 0 , 0 , 0 , 0 , 0 ) == -1 ) {
					return FALSE;
				}
				//=============================================================================================
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , selbm , selw , selslot , sels , selp , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
				//=============================================================================================
			}
			else {
				//=============================================================================================
				if ( _SCH_MACRO_FM_OPERATION( 0 , 120 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , selbm , selw , selslot , sels , selp , 0 ) == -1 ) {
					return FALSE;
				}
				//=============================================================================================
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , selbm , selw , selslot , sels , selp , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
				//=============================================================================================
			}
			//
			//
//			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2012.08.10 // 2016.05.02
//			if ( blockit || ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) ) { // 2012.08.10 // 2016.05.02
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2012.08.10 // 2016.05.02 // 2016.05.17
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( selbm ) ) {
					//=============================================================================================================================
					//
					SCHEDULING_BM_LOCK_CHECK_TM_SIDE( sels , selbm ); // 2017.02.08
					//
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , sels , selbm , -1 , TRUE , 0 , 1815 );
					//=============================================================================================================================
				}
			}
			else { // 2016.05.17
				if ( blockit ) {
					if ( outbm != 0 ) {
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( selbm , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( selbm , BUFFER_INWAIT_STATUS ) > 0 ) {
									//=============================================================================================================================
									//
									SCHEDULING_BM_LOCK_CHECK_TM_SIDE( sels , selbm ); // 2017.02.08
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , sels , selbm , -1 , TRUE , 0 , 1816 );
									//=============================================================================================================================
								}
							}
						}
					}
				}
			}
			//
			if ( gocm == 0 ) {
				//=======================================================================================
				// PLACE IC
				//=======================================================================================
				FM_CM    = _SCH_CLUSTER_Get_PathOut( sels , selp );
				//
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , FM_CM , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
					return FALSE;
				}
				//========================================================================================
				if ( Arm == TA_STATION ) {
					//----------------------------------------------------------------------
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , selw , ICsts , sels , selp , 0 , 0 , 0 , 0 , 0 , -1 , -1 );
					//----------------------------------------------------------------------
				}
				else {
					//----------------------------------------------------------------------
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , IC , selw , ICsts , sels , selp , -1 , -1 );
					//----------------------------------------------------------------------
				}
				//============================================================================
//				_SCH_MACRO_SPAWN_FM_MCOOLING( sels , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , selw , 0 , FM_CM ); // 2011.06.28
				_SCH_MACRO_SPAWN_FM_MCOOLING( sels , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , FM_CM ); // 2011.06.28
				//============================================================================
			}
		}
		//
	}
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 3 = Arm=%d,selbm=%d,selslot=%d,sels=%d,selp=%d,selw=%d,gocm=%d\n" , Arm , selbm , selslot , sels , selp , selw , gocm );
	//==============================================================================================
	//=======================================================================================
	// PICK IC
	//=======================================================================================
	if ( gocm == 0 ) {
		//
		pt = _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &s , &ICsts , &ICsts2 );
		//
		//==============================================================================================
		_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 4 = Arm=%d,selbm=%d,selslot=%d,sels=%d,selp=%d,selw=%d,gocm=%d,pt=%d,ICsts=%d,ICsts2=%d\n" , Arm , selbm , selslot , sels , selp , selw , gocm , pt , ICsts , ICsts2 );
		//==============================================================================================
		if ( pt == SYS_SUCCESS ) {
			//
			*act = TRUE;
			//
			gocm = 1;
			//
			sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
			selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
			selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
			//
			FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			//
			//==================================================================================================================
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
				return FALSE;
			}
//xxx_check_printf( side , TRUE , "PICK From IC in (Scheduler_Other_Arm_Cool_Return)" , 1 );
			//========================================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
		}
	}
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 99-C CHECK_COOL 5 = Arm=%d,selbm=%d,selslot=%d,sels=%d,selp=%d,selw=%d,gocm=%d,pt=%d,ICsts=%d,ICsts2=%d\n" , Arm , selbm , selslot , sels , selp , selw , gocm , pt , ICsts , ICsts2 );
	//==============================================================================================
	//=======================================================================================
	// PLACE to CM
	//=======================================================================================
	if ( gocm == 1 ) {
		//
		if ( noplacecm ) { // 2017.01.20
			//
			*noplacedcm = TRUE;
			//
			Half_Placed_CM_SIDE = sels;
			Half_Placed_CM_IC_SLOT1 = ( Arm == TA_STATION ) ? 1 : 0;
			Half_Placed_CM_IC_SLOT2 = ( Arm == TA_STATION ) ? 0 : 1;
			Half_Placed_CM_FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			Half_Placed_CM_Slot1 = ( Arm == TA_STATION ) ? selw : 0;
			Half_Placed_CM_Slot2 = ( Arm == TA_STATION ) ? 0 : selw;
			Half_Placed_CM_Side1 = sels;
			Half_Placed_CM_Side2 = sels;
			Half_Placed_CM_Pt1 = selp;
			Half_Placed_CM_Pt2 = selp;
			//
			return TRUE;
			//
		}
		//
		*act = TRUE;
		//
		//=============================================================================================
		// 2013.11.20
		//=============================================================================================
		if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) {
			//
			FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			//
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , sels , IC , ( Arm == TA_STATION ) ? 1 : 0 , ( Arm == TA_STATION ) ? 0 : 1 , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , sels , sels , selp , selp ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
				return FALSE;
			}
		}
		//==========================================================================
		//
		if ( _SCH_MACRO_FM_PLACE_TO_CM( sels , ( Arm == TA_STATION ) ? selw : 0 , sels , selp , ( Arm == TA_STATION ) ? 0 : selw , sels , selp , FALSE , TRUE , 0 ) == -1 ) {
			return FALSE;
		}
		//
	}
	return TRUE;
}

/*
//
// 2018.09.18
//
//BOOL Scheduler_All_FM_Arm_Return( int side ) { // 2015.06.16 // 2015.09.02
int Scheduler_All_FM_Arm_Return( int side , BOOL Check ) { // 2015.06.16 // 2015.09.02
	int i , s , p , w;
	BOOL run = FALSE; // 2015.09.02
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		w = _SCH_MODULE_Get_FM_WAFER( i );
		//
		if ( w <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_FM_SIDE( i );
		p = _SCH_MODULE_Get_FM_POINTER( i );
		//
		if ( Check ) { // 2015.09.02
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
				//
//				continue; // 2016.07.22
				//
				// 2016.07.22
				//
				if ( _SCH_SUB_FM_Current_No_Way_Supply( s , p , -1 ) ) {
					//
//					if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) { // 2018.04.10
					if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) { // 2018.04.10
						//
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s , p , FALSE , TRUE , 41 );
						//
					}
					else {
						continue;
					}
				}
				else {
					continue;
				}
			}
		}
		else {
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s , p , FALSE , TRUE , 24 ); // 2016.02.19
			//
		}
		//
		//
		if ( _SCH_MACRO_FM_PLACE_TO_CM( s , ( i == TA_STATION ) ? w : 0 , s , p , ( i == TA_STATION ) ? 0 : w , s , p , FALSE , TRUE , 0 ) == -1 ) {
//			return FALSE; // 2015.09.02
			return 0; // 2015.09.02
		}
		//
		run = TRUE; // 2015.09.02
		//
	}
	//
//	return TRUE; // 2015.09.02
	//
	if ( run ) return 1;
	return -1;
	//
}
//
*/
//
// 2018.09.18
//
//BOOL Scheduler_All_FM_Arm_Return( int side ) { // 2015.06.16 // 2015.09.02
int Scheduler_All_FM_Arm_Return( int side , int Check ) { // 2015.06.16 // 2015.09.02
	int i , s , p , w;
	BOOL run = FALSE;
	//
	//
	// 2019.03.15
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 11 ) == 0 ) {
		if      ( Check == 2 ) return -1;
		else if ( Check == 3 ) return FALSE;
	}
	else { // 2019.03.21
		if ( Check == 3 ) {
			_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , -1 );
		}
	}
	//
//	if ( Check ) { // 2019.03.15
	if ( ( Check == 1 ) || ( Check == 2 ) || ( Check == 3 ) ) { // 2019.03.15
		//
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			//
			w = _SCH_MODULE_Get_FM_WAFER( i );
			//
			if ( w <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_FM_SIDE( i );
			p = _SCH_MODULE_Get_FM_POINTER( i );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
				//
				if ( ( Check == 2 ) || ( Check == 3 ) ) continue; // 2019.03.15
				//
				if ( _SCH_SUB_FM_Current_No_Way_Supply( s , p , -1 ) ) {
					//
					if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) {
					}
					else {
						continue;
					}
				}
				else {
					continue;
				}
			}
			//
			run = TRUE;
			//
			break;
			//
		}
		//
	}
	else {
		//
		run = TRUE;
		//
	}
	//
	if ( Check == 3 ) return run; // 2019.03.15
	//
	if ( !run ) return -1;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		w = _SCH_MODULE_Get_FM_WAFER( i );
		//
		if ( w <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_FM_SIDE( i );
		p = _SCH_MODULE_Get_FM_POINTER( i );
		//
		_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
		//
		SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s , p , FALSE , TRUE , 24 );
		//
		//
		if ( _SCH_MACRO_FM_PLACE_TO_CM( s , ( i == TA_STATION ) ? w : 0 , s , p , ( i == TA_STATION ) ? 0 : w , s , p , FALSE , TRUE , 0 ) == -1 ) {
			return 0;
		}
		//
	}
	//
	return 1;
	//
}

BOOL Scheduler_FEM_GetIn_Single_BM_Slot_Properly_Check_for_Switch( int CHECKING_SIDE , int bmc ) {//2008.08.22
	int FM_Buffer;

	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
			for ( FM_Buffer = BM1 ; FM_Buffer < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; FM_Buffer++ ) {
				//
				if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , FM_Buffer ) ) continue; // 2009.03.11
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( FM_Buffer , G_PLACE , 0 ) ) continue; // 2018.06.01
				//
//				if ( _SCH_MODULE_GET_USE_ENABLE( FM_Buffer , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) == 0 ) ) { // 2009.09.28
				if ( _SCH_MODULE_GET_USE_ENABLE( FM_Buffer , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , FM_Buffer ) != MUF_00_NOTUSE ) ) { // 2009.09.28
					//
					if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) {
						if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
	else { // 2011.07.04
		if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) == 1 ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmc , 1 ) > 0 ) {
				return FALSE;
			}
			if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) == BUFFER_TM_STATION ) {
				return FALSE;
			}
			if ( _SCH_MODULE_Get_BM_WAFER( bmc , 1 ) > 0 ) {
				return FALSE;
			}
		}
	}
	return TRUE;
}
//
BOOL scheduler_Check_NotUse_BM_For_Switch( int side , int bm ) { // 2008.11.24
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == bm ) return FALSE;
	}
	return TRUE;
}

BOOL scheduler_malic_locking( int side ) {
	if ( _SCH_MODULE_Chk_MFIC_LOCKING( side ) ) return TRUE;
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
		if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_MODULE_Get_MFAL_SIDE() == side ) ) return TRUE;
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
		if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
				if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULER_FM_Other_Supply_Impossible( int Side ) { // 2016.01.05
	//
	if ( Get_RunPrm_FM_PM_CONTROL_USE() ) {
		//
		if ( !_SCH_SUB_Check_Complete_Return( Side ) ) return TRUE;
		//
	}
	//
	return FALSE;
}

BOOL SCHEDULER_FM_Current_No_More_Supply_Sub( int Side , int option , int option2 , int option3 ) { // 2014.11.07
	int dum_pt , dum_rcm;
	int pm , s1 , p1 , w1 , s2 , p2 , w2;
	//
	if ( !_SCH_SUB_FM_Current_No_More_Supply( Side , &dum_pt , option , &dum_rcm , option2 , option3 ) ) return FALSE;
//
//_SCH_LOG_DEBUG_PRINTF( Side , 0 , "FEM FM_Current_No_More_Supply = (dum_pt=%d)(option=%d)(dum_rcm=%d)(option2=%d)(option3=%d)\n" , dum_pt , option , dum_rcm , option2 , option3 );
//
	//
	if ( Get_RunPrm_FM_PM_CONTROL_USE() ) {
		//
		if ( !_SCH_SUB_Check_Complete_Return( Side ) ) return TRUE; // 2016.01.04
		//
		if ( SCHEDULER_FM_PM_GET_PICK_PM( 9 , Side , -1 , &pm , &dum_pt , &w1 , &s1 , &p1 , &dum_rcm , &w2 , &s2 , &p2 ) < 0 ) return TRUE;
		//
		if ( w1 > 0 ) {
			if ( _SCH_SUB_FM_Current_No_Way_Supply( s1 , p1 , -1 ) ) return TRUE;
		}
		//
		if ( w2 > 0 ) {
			if ( _SCH_SUB_FM_Current_No_Way_Supply( s2 , p2 , -1 ) ) return TRUE;
		}
		//
		return FALSE;
		//
	}
	//
	return TRUE;
}

BOOL SCHEDULER_FM_Current_No_More_Supply( int Side ) { // 2014.11.07
	return SCHEDULER_FM_Current_No_More_Supply_Sub( Side , Sch_NoWay_Option() , -1 , -1 );
}

BOOL SCHEDULER_FM_Current_No_More_Supply2( int Side ) { // 2014.11.07
	return SCHEDULER_FM_Current_No_More_Supply_Sub( Side , -1 , -1 , -1 );
}

BOOL SCHEDULER_FM_Another_No_More_Supply( int Side , int option ) { // 2014.11.07
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != Side ) {
			if ( !SCHEDULER_FM_Current_No_More_Supply_Sub( i , option , -1 , -1 ) ) return FALSE;
		}
	}
	return TRUE;
}

int SCHEDULER_FM_Another_No_More_Supply_Ex( int Side , int option ) { // 2018.05.16
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != Side ) {
			if ( !SCHEDULER_FM_Current_No_More_Supply_Sub( i , option , -1 , -1 ) ) return i;
		}
	}
	return -1;
}

BOOL SCHEDULER_FM_Another_No_More_Supply2( int Side , int option , int option2 ) { // 2014.11.07
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != Side ) {
			if ( !SCHEDULER_FM_Current_No_More_Supply_Sub( i , option , -1 , option2 ) ) return FALSE;
		}
	}
	return TRUE;
}

int SCHEDULER_FM_All_No_More_Supply() { // 2017.02.10
	int i;
	int nc;
	int dc;
	//
	nc = 0;
	dc = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( SCHEDULER_FM_Current_No_More_Supply_Sub( i , -1 , -1 , -1 ) ) {
			nc++;
			continue;
		}
		//
		if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( i , -1 , TRUE ) ) {
			dc++;
			continue;
		}
		//
		return -1;
		//
	}
	//
	return ( ( dc * 100 ) + nc );
}


BOOL SCHEDULER_FM_FM_Finish_Status( int Side ) { // 2014.11.07
	int dum_pt , dum_rcm;
	int pm , s1 , p1 , w1 , s2 , p2 , w2;
	//
	if ( ( _SCH_SYSTEM_MODE_END_GET( Side ) == 0 ) && !_SCH_MODULE_Chk_FM_Finish_Status( Side ) ) return FALSE;
	//
	if ( Get_RunPrm_FM_PM_CONTROL_USE() ) {
		//
		if ( SCHEDULER_FM_PM_GET_PICK_PM( 9 , Side , -1 , &pm , &dum_pt , &w1 , &s1 , &p1 , &dum_rcm , &w2 , &s2 , &p2 ) > 0 ) return FALSE;
		//
	}
	//
	return TRUE;
	//
}


//
BOOL Scheduler_No_More_Supply_Check( int side ) { // 2008.07.28
	int i;

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( ( side != -1 ) && ( side != i ) ) continue; // 2015.06.18
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( !SCHEDULER_FM_Current_No_More_Supply( i ) ) {
//			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2015.06.18
				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( i , -1 , TRUE ) ) {
					return FALSE;
				}
//			} // 2015.06.18
//			else { // 2015.06.18
//				return FALSE; // 2015.06.18
//			} // 2015.06.18
		}
	}
	return TRUE;
}

int Scheduler_Deadlock_for_impossible_Supply_and_Action_MiddleSwap( int CHECKING_SIDE , int mode , int bmc ) { // 2008.04.10
	int i , j , k , s , p , x , rbmc , fms , yms , bmx;
	int w , w_imp , w_ret , coolplace , ICsts , ICsts2 , ICsts_Real , ALsts;
	//=============================================================================================================================
//	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) return 1; // 2013.10.01
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) { // 2013.10.01
		//
		if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return 1; // 2013.10.01
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
			return 1;
		}
	}
	//=============================================================================================================================
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) == BUFFER_TM_STATION ) { // 2008.04.17
		//
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( bmc ) > 0 ) return 2;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_OUTWAIT_STATUS ) <= 0 ) return 3;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_INWAIT_STATUS ) <= 0 ) return 4;
		//
	}
	else { // 2013.10.01
		//
		if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
		//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
				//
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( bmc ) <= 0 ) return 99;
				//
			}
		}
		//
	}
	//=============================================================================================================================
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 5;
	//
	if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
		fms = TA_STATION;
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
		fms = TB_STATION;
	}
	else {
		return 6;
	}
	//
	yms = -1;
	//
	if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		yms = TA_STATION;
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		yms = TB_STATION;
	}
	//=============================================================================================================================
	rbmc = -1;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
		//
		if ( k != bmc ) {
//			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2009.09.28
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2009.09.28
				//
				if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) return 7;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) return 8;
				//
				if ( rbmc == -1 ) {
					rbmc = k;
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( rbmc , -1 ) > 0 ) rbmc = k;
					}
				}
			}
		}
	}
	//=============================================================================================================================
	if ( mode == 0 ) {
		for ( i = 0 ; i < 2 ; i++ ) {
			w_imp = 0;
			w_ret = 0;
			//
			for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
				if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) return 9;
			}
			//
			for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
				//
				s = _SCH_MODULE_Get_TM_SIDE( 0 , j );
				p = _SCH_MODULE_Get_TM_POINTER( 0 , j );
				//
				if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) return 10;
				//
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) {
					w_ret++;
				}
				else {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						x = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
						if ( x > 0 ) {
//							if ( _SCH_MODULE_Get_PM_WAFER( x , 0 ) <= 0 ) break; // 2014.01.10
							if ( SCH_PM_IS_ABSENT( x , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) ) break;
						}
					}
					if ( k == MAX_PM_CHAMBER_DEPTH ) w_imp++;
				}
				//
			}
			//
			if ( w_imp <= 0 ) {
				if ( w_ret <= 1 ) return 11;
			}
			if ( w_ret <= 0 ) {
				if ( w_imp <= 1 ) return 12;
			}
			//
		}
	}
	//=============================================================================================================================
	if ( yms != -1 ) { // 2008.04.17
		s = _SCH_MODULE_Get_FM_SIDE( yms );
		p = _SCH_MODULE_Get_FM_POINTER( yms );
		w = _SCH_MODULE_Get_FM_WAFER( yms );
		//
		x = 0;
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 0 ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				if ( _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , k ) > 0 ) break;
			}
			if ( k == MAX_PM_CHAMBER_DEPTH ) x = 1;
		}
		else if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) { // 2008.04.17
			x = 1;
		}
		//
		if ( x == 1 ) {
			//
			yms = -1; // 2008.09.03
			//
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s , p , FALSE , TRUE , 25 ); // 2016.02.19
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( s , ( yms == TA_STATION ) ? w : 0 , s , p , ( yms == TB_STATION ) ? w : 0 , s , p , FALSE , FALSE , 0 ) ) {
			case -1 :
				return -1;
				break;
			}
		}
//		else { // 2008.09.03
//			yms = -1; // 2008.09.03
//		} // 2008.09.03
	}
	//=============================================================================================================================
	for ( x = 0 ; x < 2 ; x++ ) {
		//------------------------------------
		if ( x == 0 ) {
			bmx = bmc;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( bmx ) != BUFFER_FM_STATION ) continue;
			//
			if ( yms != -1 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmx , BUFFER_INWAIT_STATUS ) <= 0 ) return 14;
			}
			//
		}
		else {
			bmx = rbmc;
		}
		//------------------------------------
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmx ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmx , i ) > 0 ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmx ) < 0 ) return -1; // 2013.06.28
				//
				s = _SCH_MODULE_Get_BM_SIDE( bmx , i );
				p = _SCH_MODULE_Get_BM_POINTER( bmx , i );
				w = _SCH_MODULE_Get_BM_WAFER( bmx , i );
				//
				w_ret = 0; // 2008.09.01
				//
				if ( _SCH_MODULE_Get_BM_STATUS( bmx , i ) != BUFFER_OUTWAIT_STATUS ) {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) { // 2008.04.17
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							if ( _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , k ) > 0 ) break;
						}
						if ( k == MAX_PM_CHAMBER_DEPTH ) {
							//
							w_ret = 1; // 2008.09.01
							//
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s , p , FALSE , TRUE , 26 ); // 2016.02.19
							//
						}
						else {
							w = 0;
						}
					}
					else {
						//
						w_ret = 1; // 2008.09.01
						//
					}
				}
				//
				if ( !_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( s , p ) ) w = 0; // 2013.05.27
				//
				if ( w != 0 ) {
					//-------------------------------------------------------
					coolplace = 0; // 2008.09.01
					//-------------------------------------------------------
					if ( w_ret == 0 ) { // 2008.09.01
						//----------------------------------------------------------------------
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
							if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) coolplace = 2;
							if ( SCHEDULER_COOLING_SKIP_AL0( s , p ) ) coolplace = 2;
						}
						else {
							coolplace = 1;
						}
						//----------------------------------------------------------------------
						if ( coolplace == 0 ) {
							//==================================================================================================================
							ALsts = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( s , p , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
							//==================================================================================================================
							if ( ALsts <= 0 ) return 13;
							//==================================================================================================================
							if ( !_SCH_MODULE_Chk_MFIC_MULTI_FREE( ICsts , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) {
								return 14;
							}
							//==================================================================================================================
						}
						//----------------------------------------------------------------------
					}
					//-------------------------------------------------------
					// Pick BM
					//-------------------------------------------------------
					//==========================================================================
					if ( _SCH_MACRO_CHECK_PROCESSING( bmx ) < 0 ) return -1; // 2009.04.14
					//==========================================================================
					switch( _SCH_MACRO_FM_OPERATION( 0 , 210 + MACRO_PICK , bmx , ( fms == TA_STATION ) ? w : 0 , ( fms == TA_STATION ) ? i : 0 , s , p , bmx , ( fms == TB_STATION ) ? w : 0 , ( fms == TB_STATION ) ? i : 0 , s , p , FALSE ) ) {
					case -1 :
						return -1;
						break;
					}
					//----------------------------------------------------------------------
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , bmx , ( fms == TA_STATION ) ? w : 0 , ( fms == TA_STATION ) ? i : 0 , s , p , bmx , ( fms == TB_STATION ) ? w : 0 , ( fms == TB_STATION ) ? i : 0 , s , p , 0 , 0 );
					//----------------------------------------------------------------------
					//-------------------------------------------------------
					if ( w_ret == 0 ) { // 2008.09.01
						if      ( coolplace == 1 ) {
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , s , IC , ( fms == TA_STATION ) ? w : 0 , ( fms == TA_STATION ) ? 0 : w , _SCH_CLUSTER_Get_PathIn( s , p ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( s ) , TRUE , w , w , s , s , p , p ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( s , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , w , 9 , w , 9 , w );
								return -1;
							}
						}
						else if ( coolplace == 0 ) {
							//==================================================================================================================
							if ( fms == TA_STATION ) {
								_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , ICsts , &ICsts_Real , &k );
							}
							else {
								ICsts_Real = ICsts;
							}
							//======================================================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , s , IC , ( fms == TA_STATION ) ? ICsts_Real : 0 , ( fms == TA_STATION ) ? 0 : ICsts_Real , _SCH_CLUSTER_Get_PathIn( s , p ) , _SCH_CLUSTER_Get_PathIn( s , p ) , TRUE , w , w , s , s , p , p ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( s , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , w , 9 , w , 9 , w );
								return -1;
							}
							//========================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , ( fms == TA_STATION ) ? w : 0 , ICsts , s , p , IC , ( fms == TA_STATION ) ? 0 : w , ICsts , s , p , -1 , -1 );
							//============================================================================
							_SCH_MACRO_SPAWN_FM_MCOOLING( s , ( fms == TA_STATION ) ? ICsts : 0 , ( fms == TA_STATION ) ? 0 : ICsts , ( fms == TA_STATION ) ? w : 0 , ( fms == TA_STATION ) ? 0 : w , _SCH_CLUSTER_Get_PathIn( s , p ) );
							//============================================================================
							if ( fms == TA_STATION ) {
								_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts );
								_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts );
							}
							//============================================================================
							return 0;
						}
					}
					//-------------------------------------------------------
					// Place cm
					//-------------------------------------------------------
					switch( _SCH_MACRO_FM_PLACE_TO_CM( s , ( fms == TA_STATION ) ? w : 0 , s , p , ( fms == TB_STATION ) ? w : 0 , s , p , FALSE , FALSE , 0 ) ) {
					case -1 :
						return -1;
						break;
					}
				}
			}
		}
		//------------------------------------
		if ( x == 0 ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmx , BUFFER_INWAIT_STATUS ) > 0 ) {
				//=============================================================================================================================
				//
				SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , bmx ); // 2017.02.08
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmx , -1 , TRUE , 0 , 1615 );
				//=============================================================================================================================
			}
			else { // 2008.04.17
				if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
					//=============================================================================================================================
					//
					SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , bmx ); // 2017.02.08
					//
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmx , -1 , TRUE , 0 , 1616 );
					//=============================================================================================================================
				}
			}
		}
		else {
			//=============================================================================================================================
			//
			SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , bmx ); // 2017.02.08
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmx , -1 , TRUE , 0 , 1616 );
			//=============================================================================================================================
		}
	}
	//
//	if ( yms == -1 ) return 15; // 2008.09.03
	if ( yms != -1 ) return 15; // 2008.09.03
	//
	return 0;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ALL_ONE_SLOT_MODE( int CHECKING_SIDE ) { // 2008.05.20
	int k;
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
		//
//		if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2009.09.28
		if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2009.09.28
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return FALSE;
		}
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_PREPARED_For_PLACE( int CHECKING_SIDE , int bmc ) { // 2009.03.11
	int k , s1 , s2;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( bmc >= BM1 ) {
			if ( bmc != k ) continue;
		}
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) == MUF_00_NOTUSE ) continue; // 2009.09.28
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
		//
		if      ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k ) > 0 ) return TRUE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k ) > 0 ) return TRUE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( k , &s1 , &s2 , 0 ) > 0 ) return TRUE;
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) <= 0 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k ) > 0 ) return TRUE;
			}
		}
	}
	return FALSE;
}

//int SCHEDULING_CHECK_NOPASING_PICK_REJECT( int higher , int TMATM , int CHECKING_SIDE , int mode , int PickChamber_Side , int PickSlot_Pointer ); // 2013.12.18
int SCHEDULING_CHECK_NOPASING_PICK_REJECT( int higher , int TMATM , int CHECKING_SIDE , int mode , int PickChamber_Side , int PickSlot_Pointer , BOOL ); // 2013.12.18

//int SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( int side , int pt ) { // 2011.01.26 // 2015.06.25
int SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( int side , int pt , BOOL force ) { // 2011.01.26 // 2015.06.25
	int k , ones;

	if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 > 0 ) { // 2011.01.27
		//
		if ( force ) { // 2015.06.25
			//
			if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 < 4 ) return FALSE; // 2015.07.23
			//
			ones = TRUE;
			//
		}
		else {
			//
			ones = FALSE;
			//
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
				//
				if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( side , k ) ) continue;
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , k ) != MUF_00_NOTUSE ) ) {
					//
					if ( _SCH_PRM_GET_MODULE_SIZE( k ) <= 1 ) {
						ones = TRUE;
						break;
					}
					//
				}
			}
		}
		//
		if ( ones ) {
			//
			for ( k = 0 ; k < 2 ; k++ ) {
				//
//				ones = SCHEDULING_CHECK_NOPASING_PICK_REJECT( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 , 0 , side , 1 , side , pt ); // 2013.12.18
				ones = SCHEDULING_CHECK_NOPASING_PICK_REJECT( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 , 0 , side , 1 , side , pt , FALSE ); // 2013.12.18
				//
				if ( ones > 0 ) {


//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "= Checked No Pass [Side=%d][Pt=%d][Res=%d]=\n" , side , *rpt , ones );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
					return TRUE;
				}
				else {
//printf( "******************************************************************\n" );
//printf( "******************************************************************\n" );
//printf( "= Checked No Pass [Side=%d][Pt=%d]OK=\n" , side , *rpt );
//printf( "******************************************************************\n" );
//printf( "******************************************************************\n" );
				}
			}
		}
		//
	}
	//
	return FALSE;
}
	//======================================================================================================================================

int SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( int side , int *rcm , int *rpt , int mode ) { // 2010.09.28
//	int k , ones , Res;
	int Res;

	if ( mode == 99 ) { // 2015.06.25
		Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , rcm , rpt , 0 );
	}
	else {
		Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , rcm , rpt , ( mode == 0 ) ? 10 : mode );
	}
	//
	if ( !Res ) return 1;
	//
	if ( mode != 99 ) { // 2015.06.25
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2011.10.17
			if ( SCHEDULER_CONTROL_Chk_FM_ARM_Has_Been_Waiting_For_1SlotBM_Supply( side , 1 ) != 0 ) return 3;
		}
	}
	//
	if ( mode == 1 ) return 0; // 2010.10.05
	//
//	if ( SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( side , *rpt ) ) return 2; // 2011.01.26 // 2015.06.25
	if ( SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( side , *rpt , ( mode == 99 ) ) ) return 2; // 2011.01.26 // 2015.06.25
	//
	return 0;
/*
// 2011.01.27
	if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 > 0 ) { // 2010.09.28
		//
		ones = FALSE;
		//
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( side , k ) ) continue;
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , k ) != MUF_00_NOTUSE ) ) {
				//
				if ( _SCH_PRM_GET_MODULE_SIZE( k ) <= 1 ) {
					ones = TRUE;
					break;
				}
				//
			}
		}
		//
		if ( ones ) {
			//
			for ( k = 0 ; k < 2 ; k++ ) {
				//
				ones = SCHEDULING_CHECK_NOPASING_PICK_REJECT( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 , 0 , side , 1 , side , *rpt );
				//
				if ( ones > 0 ) {


//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "= Checked No Pass [Side=%d][Pt=%d][Res=%d]=\n" , side , *rpt , ones );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
//printf( "==================================================================\n" );
					return 2;
				}
				else {
//printf( "******************************************************************\n" );
//printf( "******************************************************************\n" );
//printf( "= Checked No Pass [Side=%d][Pt=%d]OK=\n" , side , *rpt );
//printf( "******************************************************************\n" );
//printf( "******************************************************************\n" );
				}
			}
		}
		//
	}
	//
	return 0;
*/
}



int SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( int side , int *rcm , int *rpt , int mode ) { // 2010.09.28
	int Res;

	Res = SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( side , rcm , rpt , mode );
	//
	if ( Res == 0 ) return TRUE;
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( int side , int bmc ) { // 2008.01.04
	int rcm , pt , ssd , ssp , logres;
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() < BUFFER_SWITCH_FULLSWAP ) return TRUE;
	//
//	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) return TRUE; // 2016.08.31
	//
//	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 0 ) return TRUE; // 2016.08.31 // 2016.12.07
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return TRUE; // 2016.12.07
	//
//	if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( side , &rcm , &pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) return TRUE; // 2018.12.20
	if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( side , &rcm , &pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1010 ) ) return TRUE; // 2018.12.20
	//
	if ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( side , bmc , side , pt , 1 , bmc , 0 , 0 , 0 , &logres ) != 0 ) { // 2016.02.18
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) == 0 ) { // 2017.01.04
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
					//=====================================================================
					//
					SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , bmc ); // 2017.02.08
					//
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bmc , -1 , TRUE , 0 , 1187 );
					//=====================================================================
				}
			}
		}
		return FALSE;
	}
	//
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) return TRUE; // 2016.08.31
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_FM_STATION ) return TRUE; // 2016.10.06
	//
	if ( SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( -1 , bmc , side , pt , 1 , 0 , 0 , 0 , &ssd , &ssp , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) ) == 1 ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) == 0 ) { // 2017.01.04
//		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) { // 2016.10.06
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
				//=====================================================================
				//
				SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , bmc ); // 2017.02.08
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bmc , -1 , TRUE , 0 , 1188 );
				//=====================================================================
			}
		}
//		} // 2016.10.06
		return FALSE;
	}
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_FM_Double_Has2AL_HalfPick( int *log ) { // 2016.10.13
	//
	int side , pt , bmc , arm , slot1 , slot2;
	int rcm , logres;
	//
	*log = 0;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() < BUFFER_SWITCH_FULLSWAP ) return -1;
	//
	*log = 1;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return -1;
	//
	*log = 2;
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return -1;
	//
	*log = 3;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) return -1;
	//
	*log = 4;
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return -1;
	//
	*log = 5;
	//
	if      ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
		//
		side = _SCH_MODULE_Get_MFAL_SIDE();
		bmc  = _SCH_MODULE_Get_MFAL_BM();
		//
	}
	else if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
		//
		side = _SCH_MODULE_Get_MFALS_SIDE(2);
		bmc  = _SCH_MODULE_Get_MFALS_BM(2);
		//
	}
	else {
		return -1;
	}
	//
	*log = 6;
	//
	if ( ( bmc < BM1 ) || ( bmc >= TM ) ) return -1;
	//
	*log = 7;
	//
//	if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( side , &rcm , &pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) return -1; // 2018.12.20
	if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( side , &rcm , &pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1010 ) ) return -1; // 2018.12.20
	//
	*log = 8;
	//
	if ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( side , bmc , side , pt , 1 , bmc , 0 , 0 , 0 , &logres ) != 0 ) return -1;
	//
	*log = 9;
	//
	switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) {
	case 1 :
		switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
		case 2 :
			return -1;
			break;
		default :
			arm = TA_STATION;
			break;
		}
		break;
	case 2 :
		switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
		case 1 :
			return -1;
			break;
		default :
			arm = TB_STATION;
			break;
		}
		break;
	default :
		switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
		case 1 :
			arm = TA_STATION;
			break;
		case 2 :
			arm = TB_STATION;
			break;
		default :
			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
				arm = TA_STATION;
			}
			else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
				arm = TA_STATION;
			}
			else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
				arm = TB_STATION;
			}
			else {
				return -1;
			}
			break;
		}
		break;
	}
	//
	*log = 10;
	//
	//
	_SCH_MODULE_Inc_FM_OutCount( side );
	_SCH_MODULE_Set_FM_LastOutPointer( side , pt );
	//---------------------------------------------------------------------------------------------------------------
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
		//=============================================================================================
		SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( side , _SCH_CLUSTER_Get_ClusterIndex( side , pt ) );
		//=============================================================================================
		_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , pt , 0 , 0 , 0 );
	}
	else {
		if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , pt ,  0 ,  0 , 0 );
		}
		else {
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , pt , 20 , 20 , 2 );
		}
	}
	//---------------------------------------------------------------------------------------------------------------
	_SCH_CLUSTER_Set_Buffer( side , pt , bmc );
	//---------------------------------------------------------------------------------------------------------------
	if ( arm == TA_STATION ) {
		slot1 = _SCH_CLUSTER_Get_SlotIn( side , pt );
		slot2 = 0;
	}
	else {
		slot1 = 0;
		slot2 = _SCH_CLUSTER_Get_SlotIn( side , pt );
	}
	switch( _SCH_MACRO_FM_OPERATION( 0 , 910 + MACRO_PICK , -1 , slot1 , slot1 , side , pt , -1 , slot2 , slot2 , side , pt , -1 ) ) {
	case -1 :
		return 0;
		break;
	}
	//----------------------------------------------------------------------------------------------------------------------------
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , slot1 , slot1 , side , pt , -1 , slot2 , slot2 , side , pt , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL );
	//----------------------------------------------------------------------
	return 1;
	//
}

BOOL Check_TM_Has_Finish_Wafer( int tmside ) { // jmlee 2010.10.25 for Cham&C 1slot
	int ps , pp , i ;
	//
	for ( i = TA_STATION ; i <= TD_STATION ; i++ ) {
		//
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , i ) == FALSE ) continue;
		if ( _SCH_MODULE_Get_TM_WAFER( tmside , i ) <= 0 ) continue;
		//
		ps = _SCH_MODULE_Get_TM_SIDE( tmside , i );
		pp = _SCH_MODULE_Get_TM_POINTER( tmside , i );
		//
		if ( _SCH_CLUSTER_Get_PathRange( ps , pp ) <=  _SCH_CLUSTER_Get_PathDo( ps , pp )) return TRUE;
		//
	}
	//
	return FALSE;
}

void SCHEDULER_CONTROL_Chk_SWITCH_PRE_CONDITION_RUN( int CHECKING_SIDE ) {
	int k;
	int Result;
	int RunBuffer;
	//
	int x;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2003.11.07
		//==================================================================================
		// 2013.05.28
		//==================================================================================
		//
		// 2017.03.24
		//
		if ( _SCH_SYSTEM_MFIDLL_USE() ) {
			x = 0;
		}
		else {
			x = 2;
		}
		//
		//
//		for ( x = 0 ; x < 3 ; x++ ) { // 2017.02.27 // 2017.03.24
//
		for ( ; x < 3 ; x++ ) { // 2017.02.27 // 2017.03.24
			//
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) { // 2013.05.03
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) {
					//
					if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) {
						//
						if ( ( _SCH_PRM_GET_MODULE_GROUP( k ) > 0 ) || ( x == 2 ) ) { // 2017.02.27
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( k ) , k ) == BUFFER_SWITCH_MODE ) {
								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == k ) {
									//
		//							_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 ); // 2013.05.28
									if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
											if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) {
												if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1001 );
													}
												}
											}
										}
									}
		//							_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 ); // 2013.05.28
		//							Sleep(100); // 2013.05.28
									//
								}
								else {
									//
									if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( k ) , k , G_PICK , G_MCHECK_SAME ) ) {
		//								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 9999 ); // 2017.02.27
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1601 ); // 2017.02.27
									}
									else {
		//								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 9999 ); // 2017.02.27
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1602 ); // 2017.02.27
									}
									//
								}
							}
						}
						//
					}
					else if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2017.02.27
						//
						if ( x == 1 ) {
							if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( k ) , k , G_PICK , G_MCHECK_SAME ) ) {
								x = 99;
								break;
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
		//==================================================================================
		// 2006.12.19
		//==================================================================================
		if ( _SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) {
		//==================================================================================
			/*
			// 2013.05.03
			if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2003.11.28
				k = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE );
				_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 );
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1001 );
								}
							}
						}
					}
				}
				_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
				Sleep(100);
			}
			*/
			while ( TRUE ) {
				//-------------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return; // 2013.10.21
				//-------------------------------------------------------------------------
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 1\n" ); // 2015.09.17
//============================================================================================================================================
				//--------------------------------------------------------------------------------
				// 2004.08.14
				//--------------------------------------------------------------------------------
				Result = 0;
				RunBuffer = 0;
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
						//
						if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) Result = k;
						RunBuffer++;
					}
				}
				//--------------------------------------------------------------------------------
				if ( ( Result != 0 ) && ( RunBuffer == 1 ) ) { // 2004.08.14
					_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 );
					if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != Result ) { // 2003.11.28
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( Result ) ) {
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Result ) && ( _SCH_PRM_GET_MODULE_GROUP( Result ) == 0 ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Result ) <= 0 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , Result , -1 , TRUE , 0 , 4 );
								}
							}
						}
					}
					_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
					Sleep(100);
					break;
				}
				else {
					//==================================================================================
					// 2007.05.23
					//==================================================================================
					if ( !_SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) break;
					//==================================================================================
					Result = FALSE;
					RunBuffer = 0;
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
						//
						if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
						//
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != k ) { // 2003.11.28
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
								if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
									Result = TRUE;
								}
								else {
									RunBuffer++;
								}
							}
						}
					}
					if ( Result ) break;
					if ( RunBuffer == 1 ) break;
					_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 );
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
						//
						if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
						//
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != k ) { // 2003.11.28
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
//									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) { // 2006.12.21
										if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 2 );
												if ( _SCH_SYSTEM_USING_ANOTHER_RUNNING( CHECKING_SIDE ) ) { // 2003.11.10
													k = 999999;
												}
											}
										}
									}
								}
							}
						}
					}
					_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
					Sleep(100);
				}
			}
		}
		//==================================================================================
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.07
		//
		while ( TRUE ) {
			//-------------------------------------------------------------------------
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return; // 2013.10.21
			//-------------------------------------------------------------------------
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 2\n" ); // 2015.09.17
//============================================================================================================================================
			//--------------------------------------------------------------------------------
			// 2004.08.14
			//--------------------------------------------------------------------------------
			Result = 0;
			RunBuffer = 0;
			//
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				//
				if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
				//
				if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
				//
//				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
					//
					if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) Result = k;
					RunBuffer++;
				}
			}
			//--------------------------------------------------------------------------------
			if ( ( Result != 0 ) && ( RunBuffer == 1 ) ) { // 2004.08.14
				//
//				_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 ); // 2011.02.22
				//
				if ( !_SCH_SYSTEM_TRYENTER_TM_CRITICAL( 0 ) ) { // 2011.02.22
					Sleep(1);
					continue;
				}
				//
				if ( Scheduler_FEM_GetIn_Properly_Check_for_Switch( CHECKING_SIDE , Result ) ) { // 2004.03.09
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( Result ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Result ) && ( _SCH_PRM_GET_MODULE_GROUP( Result ) == 0 ) ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Result ) <= 0 ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , Result , -1 , TRUE , 0 , 5 );
							}
						}
					}
				}
				//
				_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
				Sleep(100);
				break; // 2004.08.16
			}
			else {
				//==================================================================================
				if ( !_SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) break; // 2015.02.10
				//==================================================================================
				Result = FALSE;
				RunBuffer = 0;
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
						//
						if ( Scheduler_FEM_GetIn_Properly_Check_for_Switch( CHECKING_SIDE , k ) ) { // 2004.03.09
		//					if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) { // 2004.03.09
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2004.03.09
								Result = TRUE;
							}
							else {
								RunBuffer++;
							}
						}
						else { // 2004.03.09
							RunBuffer++; // 2004.03.09
						} // 2004.03.09
					}
				}
				if ( Result ) break;
				if ( RunBuffer == 1 ) break;
				//
				//
//				_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 ); // 2011.02.22
				//
				if ( !_SCH_SYSTEM_TRYENTER_TM_CRITICAL( 0 ) ) { // 2011.02.22
					Sleep(1);
					continue;
				}
				//
				//-------------------------------------------------------------------------------------------------------
				// 2008.04.10
				//-------------------------------------------------------------------------------------------------------
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2009.09.28
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2009.09.28
						//
						if ( Scheduler_FEM_GetIn_Properly_Check_for_Switch( CHECKING_SIDE , k ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1133 );
										}
									}
								}
							}
						}
					}
				}
				//
				//-------------------------------------------------------------------------------------------------------
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
					//
					if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) { // 2010.05.06
						if ( _SCH_PRM_GET_MODULE_SIZE( k ) <= 1 ) continue; // 2010.05.06
					} // 2010.05.06
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
						//
						if ( Scheduler_FEM_GetIn_Properly_Check_for_Switch( CHECKING_SIDE , k ) ) { // 2004.03.09
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) { // 2006.12.21
									//jmlee 2010.10.25 for Cham&C 1slot
									if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) { // 2010.05.06
										if ( _SCH_PRM_GET_MODULE_SIZE( k ) <= 1 ) {
											if ( Check_TM_Has_Finish_Wafer(0)) continue;
										}
									}
									//
									if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) { // 2004.03.09
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 3 );
											if ( _SCH_SYSTEM_USING_ANOTHER_RUNNING( CHECKING_SIDE ) ) { // 2003.11.10
												k = 999999;
											}
										}
									}
								}
							}
						}
					}
				}
				//-------------------------------------------------------------------------------------------------------
				//
				_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
				//
				Sleep(100);
			}
		}
	}
	else { // 2006.03.21
		if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) {
			k = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE );
			_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 );
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) {
						if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) {
							//======================================================================================================
							// 2007.01.05
							//======================================================================================================
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 18 );
								}
							}
							//======================================================================================================
						}
						else {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 19 );
								}
							}
						}
					}
				}
			}
			_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 );
			Sleep(100);
		}
	}
}

//------------------------------------------------------------------------------------------

/*
// 2013.05.06
int SCHEDULER_CONTROL_Chk_BM_LOW_SUPPLYID( int ch , int mode ) { // 2009.09.02
	int i , j = -1;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( mode == -1 ) {
				if ( j == -1 ) {
					j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
				}
				else {
					if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < j ) {
						j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
					}
				}
			}
			else {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == mode ) {
					if ( j == -1 ) {
						j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
					}
					else {
						if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < j ) {
							j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
						}
					}
				}
			}
		}
	}
	return j;
}
*/

// 2013.05.06
int SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( int ch , int mode , int *prcsid ) { // 2009.09.02
	int i , j = -1;
	*prcsid = 0;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( ( mode == -1 ) || ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == mode ) ) {
				if ( j == -1 ) {
					j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
					*prcsid = _SCH_CLUSTER_Get_Ex_PrcsID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
				}
				else {
					if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < j ) {
						j = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
						*prcsid = _SCH_CLUSTER_Get_Ex_PrcsID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
					}
				}
			}
		}
	}
	return j;
}

int SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( int ch , int ch2 ) { // 2013.05.06
	int sid , sid2 , prcsid , prcsid2;
	//
	sid  = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( ch , BUFFER_OUTWAIT_STATUS , &prcsid );
	sid2 = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( ch2 , BUFFER_OUTWAIT_STATUS , &prcsid2 );
	//
	if (
		( ( prcsid % 10 ) == 0 ) ||
		( ( prcsid2 % 10 ) == 0 ) ||
		( ( prcsid % 10 ) == ( prcsid2 % 10 ) )
		) {
		if ( sid < sid2 ) return TRUE;
	}
	else {
		if ( ( prcsid / 10 ) < ( prcsid2 / 10 ) ) return TRUE;
	}
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_LATE_SUPPLY_PLACE( BOOL check , int newch , int oldch ) { // 2014.04.07
	int sid , sid2 , prcsid , prcsid2;
	//
	if ( !check ) return FALSE;
	if ( oldch < 0 ) return FALSE;
	//
	sid  = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( newch , BUFFER_INWAIT_STATUS , &prcsid );
	sid2 = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( oldch , BUFFER_INWAIT_STATUS , &prcsid2 );
	//
	if ( sid < sid2 ) return FALSE;
	//
	return TRUE;
}



/*
// 2012.09.21
BOOL SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( int newch , int oldch ) { // 2009.09.28
	if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( newch ) ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( oldch ) ) return FALSE;
	//
	if ( _SCH_TIMER_GET_PROCESS_TIME_START( 1 / BUFFER_TM_STATION / , newch ) < _SCH_TIMER_GET_PROCESS_TIME_START( 1 / BUFFER_TM_STATION / , newch ) ) {
		return TRUE;
	}
	//
	return FALSE;
}
*/

/*
// 2013.05.06
BOOL SCHEDULING_CHECK_FM_Current_BM_is_Old( int curbm , int oldbm ) { // 2007.12.28
	int i , s , p , idos = -1;
	//
	if ( oldbm == -1 ) return FALSE;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( oldbm ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( oldbm , i ) > 0 ) {
			s = _SCH_MODULE_Get_BM_SIDE( oldbm , i );
			p = _SCH_MODULE_Get_BM_POINTER( oldbm , i );
			//
			if ( ( s < 0 ) || ( p >= 100 ) ) continue;
			//
			if ( idos == -1 ) {
				idos = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
			else {
				if ( _SCH_CLUSTER_Get_SupplyID( s , p ) < idos ) {
					idos = _SCH_CLUSTER_Get_SupplyID( s , p );
				}
			}
			//
		}
	}
	//
	if ( idos == -1 ) return FALSE;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( curbm ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( curbm , i ) > 0 ) {
			s = _SCH_MODULE_Get_BM_SIDE( curbm , i );
			p = _SCH_MODULE_Get_BM_POINTER( curbm , i );
			//
			if ( ( s < 0 ) || ( p >= 100 ) ) continue;
			//
			if ( _SCH_CLUSTER_Get_SupplyID( s , p ) < idos ) return TRUE;
			//
		}
	}
	return FALSE;
}
*/

BOOL SCHEDULING_CHECK_FM_Current_BM_is_Old( int ch , int ch2 ) { // 2013.05.06
	int sid , sid2 , prcsid , prcsid2;
	//
	if ( ch2 == -1 ) return FALSE;
	//
	sid  = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( ch , -1 , &prcsid );
	sid2 = SCHEDULER_CONTROL_Chk_BM_RET_SUPPLY_PRCS_ID( ch2 , -1 , &prcsid2 );
	//
	if ( sid2 == -1 ) return FALSE;
	if ( sid  == -1 ) return FALSE;
	//
	if (
		( ( prcsid % 10 ) == 0 ) ||
		( ( prcsid2 % 10 ) == 0 ) ||
		( ( prcsid % 10 ) == ( prcsid2 % 10 ) )
		) {
		if ( sid < sid2 ) return TRUE;
	}
	else {
		if ( ( prcsid / 10 ) < ( prcsid2 / 10 ) ) return TRUE;
	}
	return FALSE;
}


//------------------------------------------------------------------------------------------

int SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( int side , int bmc , BOOL placecmcheck ) {
	int i , j , ssd , ssp , ssw , RemainCount , bmslot , ch , dummyslot , dmpt , uc , Result , arm;
	int FM_CM , FM_CO1 , FM_CO2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_OSlot , FM_OSlot2 , FM_TSlot , FM_TSlot2 , finc;
	int dumsel;
//	int FM_Run , FM_Run2 , FM_Run3; // 2007.05.04
//	int FM_Time , FM_Time2 , FM_Time3; // 2007.05.04
	//
	Result = 1;
	//
	//============================================================================================================================
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 6 ) == 1 ) return Result; // 2018.02.11
	//============================================================================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) return Result;
	//============================================================================================================================
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return Result;
	//============================================================================================================================
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return Result;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , -1 ) ) return Result;
	//============================================================================================================================
	RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmc , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) ) ); // 2007.05.03
	if ( RemainCount <= 0 ) return Result;
	if ( ( RemainCount % 2 ) == 0 ) return Result;
	if ( ssw <= 0 ) return Result;
	if ( bmslot <= 0 ) return Result;
	//============================================================================================================================
	// 2007.05.02
	//============================================================================================================================
	arm = TA_STATION;
	//
	if ( !placecmcheck ) {
		if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return -1;
		if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) arm = TA_STATION;
		else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) arm = TB_STATION;
	}
	else {
		FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
		FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
		//
		FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		//
		FM_TSlot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
		FM_TSlot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
		//-------------------------------------------------------------------------
		if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) {
			//-------------------------------------------------------------------------
			Result = 0;
			//-------------------------------------------------------------------------
			if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
			}
			else if ( FM_TSlot > 0 ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
			}
			else if ( FM_TSlot2 > 0 ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
			}
			//-------------------------------------------------------------------------
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , side , IC , FM_TSlot , FM_TSlot2 , FM_CM , -1 , TRUE , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , FM_TSlot , FM_TSlot2 , 9 , FM_TSlot , FM_TSlot2 , FM_TSlot2 * 100 + FM_TSlot );
				return -1;
			}
			if ( FM_TSlot > 0 ) {
				//-------------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( FM_Side ) ) || ( _SCH_SYSTEM_USING_GET( FM_Side ) <= 0 ) ) return -1;
				//-------------------------------------------------------------------------
				//-------------------------------------------------------------------------
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side , FM_Pointer , -1 , &FM_CO1 , &FM_OSlot ); // 2007.07.11
				//-------------------------------------------------------------------------
			}
			else {
				FM_OSlot = 0;
				FM_CO1 = 0;
			}
			if ( FM_TSlot2 > 0 ) {
				//-------------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( FM_Side2 ) ) || ( _SCH_SYSTEM_USING_GET( FM_Side2 ) <= 0 ) ) return -1;
				//-------------------------------------------------------------------------
				//-------------------------------------------------------------------------
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side2 , FM_Pointer2 , -1 , &FM_CO2 , &FM_OSlot2 ); // 2007.07.11
				//-------------------------------------------------------------------------
			}
			else {
				FM_OSlot2 = 0;
				FM_CO2 = 0;
			}
			switch( _SCH_MACRO_FM_PLACE_TO_CM( side , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FALSE , FALSE , 0 ) ) {
			case -1 :
				return -1;
				break;
			}
			//
		}
		//----------------------------------------------------------------------

	}
	//===============================================================================================================================
	//===============================================================================================================================
	while ( TRUE ) {
		RemainCount = 0;
		finc = -1;
		//
		if ( _SCH_CLUSTER_Get_PathDo( ssd , ssp ) < _SCH_CLUSTER_Get_PathRange( ssd , ssp ) ) {
			if ( RemainCount == 0 ) {
				ch = -1;
				if ( SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_0( side , &ch , &dmpt , &uc , -1 ) ) {
					RemainCount = 1;
					finc = uc;
					dummyslot = dmpt;
					ssw = -1;
				}
				if ( RemainCount == 0 ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						ch = _SCH_CLUSTER_Get_PathProcessChamber( ssd , ssp , _SCH_CLUSTER_Get_PathDo( ssd , ssp ) , i );
						if ( ch > 0 ) {
							if ( SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_0( side , &ch , &dmpt , &uc , -1 ) ) {
								if ( RemainCount == 0 ) {
									RemainCount = 1;
									finc = uc;
									dummyslot = dmpt;
									ssw = ch;
								}
								else {
									if ( finc > uc ) {
										finc = uc;
										dummyslot = dmpt;
										ssw = ch;
									}
								}
							}
						}
					}
				}
			}
		}
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP WHILE 3-1\n" ); // 2018.02.01
//============================================================================================================================================
		//=====================================================
		if ( RemainCount == 0 ) {
			//=====================================================================================
			// Dll Interface
			//=====================================================================================
			switch( USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE( bmc - BM1 + 1 , bmslot ) ) {
			case 0 :
				return Result;
				break;
			case 1 :
				continue;
				break;
			default :
				return -1;
				break;
			}
			//=====================================================================================
		}
		else {
			break;
		}
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP WHILE 3\n" ); // 2015.09.17
//============================================================================================================================================
	}
	//===============
	// Pick From Dummy dummyslot & Place to bmc bmslot arm
	//===============
	RemainCount = 0;
	//
	for ( j = 0 ; j < 2 ; j++ ) {
		for ( i = ssp + 1 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//------------------------------------------------------------
			// 2007.05.15
			//------------------------------------------------------------
			dumsel = FALSE;
			if ( _SCH_CLUSTER_Get_PathRange( ssd , i ) < 0 ) {
				dumsel = TRUE;
			}
			else {
				if ( j != 0 ) {
					if ( _SCH_CLUSTER_Get_PathIn( ssd , i ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( _SCH_CLUSTER_Get_PathStatus( ssd , i ) == SCHEDULER_CM_END ) {
							if ( _SCH_CLUSTER_Get_PathDo( ssd , i ) != PATHDO_WAFER_RETURN ) {
								if ( _SCH_CLUSTER_Get_PathDo( ssd , i ) > _SCH_CLUSTER_Get_PathRange( ssd , i ) ) {
									dumsel = TRUE;
								}
							}
						}
					}
				}
			}
			//------------------------------------------------------------
			if ( dumsel ) {
				//==========================================================================================================
				// (dummy,dummyslot)
				//==========================================================================================================
				if ( ssw != -1 ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( ssd , ssp , _SCH_CLUSTER_Get_PathDo( ssd , ssp ) , ssw );
				}
				//
				_SCH_CLUSTER_Copy_Cluster_Data( ssd , i , ssd , ssp , 0 );
				//
				_SCH_CLUSTER_Set_PathIn( ssd , i , _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() );
				_SCH_CLUSTER_Set_PathOut( ssd , i , _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() );
				_SCH_CLUSTER_Set_SlotIn( ssd , i , dummyslot );
				_SCH_CLUSTER_Set_SlotOut( ssd , i , dummyslot );
				_SCH_CLUSTER_Set_PathStatus( ssd , i , SCHEDULER_READY );
				//==========================================================================================================
				_SCH_CLUSTER_Set_PathHSide( ssd , ssp , HANDLING_ALL_SIDE ); // 2009.08.05
				//==========================================================================================================
				dmpt = i;
				RemainCount = 99;
				break;
				//==========================================================================================================
			}
		}
		//===============
		if ( RemainCount == 99 ) break;
		//===============
	}
	//===============
	if ( RemainCount != 99 ) return -1;
	//===============
	// Pick From Dummy dummyslot,dummyslot2 & Place to bmc bmslot,bmslot2	ssd,dmpt  ssd,dmpt2
	//=============================================================================================
	_SCH_SDM_Set_CHAMBER_INC_COUNT( dummyslot - 1 );
	//=============================================================================================
	_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , MAX_CASSETTE_SIDE + 1 );
	_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , 0 );
	_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , dummyslot );
	//=============================================================================================
	_SCH_MODULE_Inc_FM_OutCount( ssd );
	//=============================================================================================
	_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( ssd , dmpt , 0 , -1 , -1 );
	//=============================================================================================
	if ( arm == TA_STATION ) {
		switch( _SCH_MACRO_FM_OPERATION( 0 , 310 + MACRO_PICK , -1 , dummyslot , dummyslot , ssd , dmpt , -1 , 0 , 0 , 0 , 0 , -1 ) ) {
		case -1 :
			return -1;
			break;
		}
	}
	else {
		switch( _SCH_MACRO_FM_OPERATION( 0 , 320 + MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , dummyslot , dummyslot , ssd , dmpt , -1 ) ) {
		case -1 :
			return -1;
			break;
		}
	}
	//=============================================================================================
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( ssd ) ) || ( _SCH_SYSTEM_USING_GET( ssd ) <= 0 ) ) return -1;
	//=============================================================================================
	if ( !SCHEDULER_ALIGN_SKIP_AL0( ssd , dmpt ) ) { // 2014.08.26
		//
		if ( arm == TA_STATION ) {
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , side , AL , dummyslot , 0 , _SCH_CLUSTER_Get_PathIn( ssd , dmpt ) , bmc , TRUE , dummyslot , 0 , ssd , 0 , dmpt , 0 ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "FM Handling Fail at AL(A:%d)%cWHFMALFAIL A:%d%c%d\n" , dummyslot , 9 , dummyslot , 9 , dummyslot );
				return -1;
			}
		}
		else {
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , side , AL , 0 , dummyslot , _SCH_CLUSTER_Get_PathIn( ssd , dmpt ) , bmc , TRUE , 0 , dummyslot , 0 , ssd , 0 , dmpt ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "FM Handling Fail at AL(B:%d)%cWHFMALFAIL B:%d%c%d\n" , dummyslot , 9 , dummyslot , 9 , dummyslot );
				return -1;
			}
		}
		//
	}
	//=============================================================================================
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( ssd ) ) || ( _SCH_SYSTEM_USING_GET( ssd ) <= 0 ) ) return -1;
	//=============================================================================================
	if ( arm == TA_STATION ) {
		switch( _SCH_MACRO_FM_OPERATION( 0 , 330 + MACRO_PLACE , bmc , dummyslot , bmslot , ssd , dmpt , 0 , 0 , 0 , 0 , 0 , -1 ) ) {
		case -1 :
			return -1;
			break;
		}
	}
	else {
		switch( _SCH_MACRO_FM_OPERATION( 0 , 340 + MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , bmc , dummyslot , bmslot , ssd , dmpt , -1 ) ) {
		case -1 :
			return -1;
			break;
		}
	}
	//=============================================================================================
	_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , bmslot , ssd , dmpt );
	_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , bmslot , dummyslot , BUFFER_INWAIT_STATUS );
	//=============================================================================================
	return Result;
}



void Pre_Make_FM_Side( int CHECKING_SIDE ) { // 2011.03.08
	int k;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_WAIT_STATION ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_01_USE ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_WAIT_STATION ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_01_USE ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) > 0 ) { // 2014.10.30 restart
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1311 );
				//
				continue;
				//
			}
		}
		//
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) { // 2018.06.01
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1312 );
			//
		}
		else {
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1311 );
			//
		}
		//
		if ( _SCH_SYSTEM_MFIDLL_USE() ) return; // 2017.03.24
		//
//		return; // 2017.02.24; 확인중 // 2017.03.24
	}
}

int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_SWITCH_STYLE_0( int CHECKING_SIDE , int mode ) {
//	int k , FM_Pointer; // 2009.03.06
	int k; // 2009.03.06

//	if ( mode != 0 ) return 0; // 2009.02.12
	if ( mode != 0 ) return -1; // 2009.02.12
	//
	wfrreturn_mode[CHECKING_SIDE] = 0;
	//
	SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 0 );
	//-----------------------------------------------
	_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 2 );
	//-----------------------------------------------

	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2007.05.23
		//========================================================================
		// 2007.05.23
		// This Part Have to check other mode(More 3 Cassette & Third Cassette is possible but Contnue Mode => Hang
		//========================================================================
		Pre_Make_FM_Side( CHECKING_SIDE ); // 2013.10.01
		//
		_SCH_SYSTEM_ENTER_FEM_CRITICAL();
		//========================================================================
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2007.06.27
		//========================================================================
		// 2007.06.27
		// This Part Have to check other mode(More 3 Cassette & Third Cassette is possible but Contnue Mode => Hang
		//========================================================================
		Pre_Make_FM_Side( CHECKING_SIDE ); // 2011.03.08
		//
		_SCH_SYSTEM_ENTER_FEM_CRITICAL();
		//========================================================================
	}
	else {
		//========================================================================
		// 2007.05.23
		// This Part Have to check other mode(More 3 Cassette & Third Cassette is possible but Contnue Mode => Hang
		//========================================================================
/* // 2009.03.07
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 1 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
		while ( TRUE ) {
			_SCH_SYSTEM_ENTER_FEM_CRITICAL();

			FM_Pointer = 0;
			if ( _SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) != 0 ) { // 2005.07.29
				for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
					if ( k != CHECKING_SIDE ) {
						if ( _SCH_SYSTEM_MODE_GET( k ) != 0 ) { // 2005.07.29
//							if ( ( _SCH_SYSTEM_USING_GET( k ) > 0 ) && ( _SCH_SYSTEM_USING2_GET( k ) <= 1 ) ) { // 2009.03.05
							if ( _SCH_SYSTEM_USING_RUNNING( k ) ) { // 2009.03.05
								if ( _SCH_SYSTEM_RUNORDER_GET( k ) < _SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
									FM_Pointer = 1;
									break;
								}
							}
						}
					}
				}
			}
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 2 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
			if ( FM_Pointer == 1 ) {
				_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
				Sleep(100);
			}
			else {
				break;
			}
		}
*/
		_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2009.03.07

		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 3 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
	}
	_SCH_MODULE_Set_FM_MidCount( 1 );

	_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 2 );

	_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Start for FEM Module%cWHFMSTART\n" , 9 );

	//============================================================================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 4 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
	//============================================================================================================================================
	//-----------------------------------------------
	_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 32 );
	//-----------------------------------------------
	while ( TRUE ) {

 		//============================================================================================================
 		// 2007.05.15
 		//============================================================================================================
/*
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
//			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1 from FEM%cWHFMFAIL 1\n" , 9 ); // 2005.09.27
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 1 for FEM Module%cWHFMSUCCESS 1\n" , 9 ); // 2005.09.27
			_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			return;
		}
*/
		//============================================================================================================
		switch( _SCH_SYSTEM_ABORT_PAUSE_CHECK( CHECKING_SIDE ) ) {
		case -1 :
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 1 for FEM Module%cWHFMSUCCESS 1\n" , 9 ); // 2005.09.27
			_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
			return SYS_ABORTED;
			break;
		case 1 :
			_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
			Sleep(1);
			_SCH_SYSTEM_ENTER_FEM_CRITICAL();
			continue;
			break;
		}
		//============================================================================================================
		//====================================================================================================
		// 2006.11.13
		//====================================================================================================
		if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
			// Will be Update for Future
		}

		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
		//====================================================================================================
		// 2007.01.24
		//====================================================================================================
		if (
			( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) &&
			( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
			( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() != CHECKING_SIDE )
			) {
			k = 0;
		}
		//====================================================================================================
		else {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) {
				k = MAX_CASSETTE_SIDE;
			}
			else {
				for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
					if ( k != CHECKING_SIDE ) {
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-1 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
//						if ( ( _SCH_SYSTEM_USING_GET( k ) > 0 ) && ( _SCH_SYSTEM_USING2_GET( k ) > 2 ) ) { // 2009.03.05
						if ( _SCH_SYSTEM_USING_RUNNING( k ) ) { // 2009.03.05
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-2 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) ) {
								if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( k ) == 0 ) { // 2006.03.21
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-3 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2009.04.02
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-31 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
										break;
									}
								}
								else { // 2006.03.21
//									if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( k ) != _SCH_MODULE_Get_BM_Switch_7_Cross( CHECKING_SIDE ) ) { // 2006.12.19 (Bug) // 2007.04.05
//										if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) ) ) { // 2009.03.04
//											break; // 2009.03.04
//										} // 2009.03.04
//									} // 2007.04.05
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-4 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
									//--------------------------------------------------------------
									// 2009.03.04
									//--------------------------------------------------------------
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-5 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) ) ) {
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-6 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
											if ( _SCH_MODULE_Get_BM_SIDE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , 1 ) != CHECKING_SIDE ) break;
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-7 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
										}
									}
									else {
										if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) ) ) {
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-8 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
											break;
										}
									}
									//--------------------------------------------------------------
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-9 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
								}
							}
							//==============================================================================================
							// 2006.11.13
							//==============================================================================================
							else {
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-10 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
								if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != 0 ) {
									if      ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) ) {
										break;
									}
									else if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) == SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( k ) ) {
										break;
									}
									if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) {
										if      ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) ) {
											break;
										}
										else if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( k ) ) {
											break;
										}
									}
								}
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 5-11 [k=%d][%d,%d]\n" , k , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
		//============================================================================================================================================
							}
							//==============================================================================================
						}
					}
				}
			}
		}
		//
		if ( k == MAX_CASSETTE_SIDE ) break;
		//
		_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
		Sleep(1);
		_SCH_SYSTEM_ENTER_FEM_CRITICAL();
	}
	//============================================================================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP ENTER 6 [%d,%d]\n" , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) );
	//============================================================================================================================================
	//----------------------------------------------------------------------
//	FM_Pick_Running_Blocking_style_0 = 0; // 2008.03.31 // 2008.09.25
	//-----------------------------------------------
	_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 33 );
	//-----------------------------------------------
	//-------------------------------------------------------------------------------------
	// 2003.11.07
	//--------------------------------------------------
	SCHEDULER_CONTROL_Chk_SWITCH_PRE_CONDITION_RUN( CHECKING_SIDE );
	//----------------------------------------------------------------------
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
		_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
	}
	//----------------------------------------------------------------------
	_SCH_SYSTEM_LEAVE_FEM_CRITICAL(); // 2004.05.21
	//----------------------------------------------------------------------
	return 0;
}


BOOL SCHEDULER_BATCHALL_SWITCH_BM_ENDING_CHECK_INOUT_TOGETHER( int bmc ) { // 2010.07.22
	int i , s , p , in , out;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmc ) ) return FALSE;
	//
	in  = 0;
	out = 0;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_INWAIT_STATUS ) in++;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) out++;
			//
		}
		//
	}
	//
	if ( ( in > 0 ) && ( out > 0 ) ) return TRUE;
	//
	return FALSE;
}

void SCHEDULER_BATCHALL_SWITCH_BM_ENDING( int CHECKING_SIDE , int bmc , int mode ) { // 2010.01.19
	int i , s , p;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
			//
			if      ( mode == 1 ) { // 2010.05.12
				if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_INWAIT_STATUS ) {
					//
					_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
					//
					_SCH_MODULE_Inc_FM_InCount( s );
					//
					_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , i , 0 , -1 );
					//
				}
			}
			else if ( mode == 0 ) {
				//
				_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
				//
				_SCH_MODULE_Inc_FM_InCount( s );
				//
				_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , i , 0 , -1 );
				//
			}
			else if ( mode == 2 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) {
					//
					_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
					//
					_SCH_MODULE_Inc_FM_InCount( s );
					//
					_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , i , 0 , -1 );
					//
				}
			}
			else if ( mode == 3 ) { // all set OUT with IN
				if ( _SCH_CLUSTER_Get_PathOut( s , p ) >= BM1 ) {
					//
					_SCH_CLUSTER_Set_PathOut( s , p , _SCH_CLUSTER_Get_PathIn( s , p ) );
					_SCH_CLUSTER_Set_SlotOut( s , p , _SCH_CLUSTER_Get_SlotIn( s , p ) );
					//
				}
				else if ( _SCH_CLUSTER_Get_PathIn( s , p ) >= BM1 ) {
					//
					_SCH_CLUSTER_Set_PathIn( s , p , _SCH_CLUSTER_Get_PathOut( s , p ) );
					_SCH_CLUSTER_Set_SlotIn( s , p , _SCH_CLUSTER_Get_SlotOut( s , p ) );
					//
				}
			}
		}
	}
}


int SCHEDULER_BATCHALL_SWITCH_BM_ONLY_SUPPLY_CHECK( int CHECKING_SIDE ) {
	int ret_rcm , ret_pt , bmcin , bmcout;

	bmcin  = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
	bmcout = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(CHECKING_SIDE);
	if ( bmcout == 0 ) bmcout = bmcin;
	//
	if ( bmcin == 0 ) return 1;
	//
//	if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2018.12.20
	if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1010 ) ) { // 2018.12.20
		//
		if ( ret_rcm != bmcin ) return -1;
		//
		if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , ret_pt ,  0 ,  0 , 0 );
		}
		else {
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , ret_pt , 20 , 20 , 2 );
		}
		//=============================================================================================
		_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
		_SCH_MODULE_Set_FM_LastOutPointer( CHECKING_SIDE , ret_pt );
		//=============================================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return -1;
		//=============================================================================================
		_SCH_MODULE_Set_BM_SIDE_POINTER( ret_rcm , _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,ret_pt) , CHECKING_SIDE , ret_pt );
		_SCH_MODULE_Set_BM_WAFER_STATUS( ret_rcm , _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,ret_pt) , _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,ret_pt) , BUFFER_INWAIT_STATUS );
		//=============================================================================================
		_SCH_MODULE_Set_BM_SIDE( ret_rcm , 1 , CHECKING_SIDE ); // 2010.03.22
		//=============================================================================================
	}
	//===================================================================================================================================
	if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < BM1 ) {
		if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
//			if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) != BUFFER_TM_STATION ) { // 2010.05.27
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmcin ) <= 0 ) {
					//=====================================================================
					_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , TRUE );
					//=====================================================================
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmcin , -1 , TRUE , 0 , 3001 );
					//=====================================================================
					_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , bmcin );
					//=====================================================================
				}
//			}
		}
	}
	//===================================================================================================================================
	if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < BM1 ) return 1;
	return 0;
}

int SCHEDULER_BATCHALL_SWITCH_BM_ONLY_RETURN_CHECK( int CHECKING_SIDE , BOOL lpin , BOOL lpout ) {
	int bmcin , bmcout , r , inc , outc;
	//
	inc  = -1;
	outc = -1;
	for ( r = 0 ; r < MAX_CASSETTE_SLOT_SIZE ; r++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , r ) >= 0 ) {
			inc  = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , r );
			outc = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , r );
			break;
		}
	}
	//
	bmcin  = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
	bmcout = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(CHECKING_SIDE);
	if ( bmcout == 0 ) bmcout = bmcin;
	//
	if ( bmcin == 0 ) return 1;
	//
	//===================================================================================================================================
	if ( !lpout ) {
		if ( bmcout != bmcin ) {
			if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < BM1 ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcout ) != BUFFER_TM_STATION ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmcout ) <= 0 ) {
						//=====================================================================
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmcout , -1 , TRUE , 0 , 3002 );
						//=====================================================================
					}
				}
			}
		}
	}
	//===================================================================================================================================
	if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
		//
		r = 0;
		//
		if ( !lpout ) {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcout ) == BUFFER_FM_STATION ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status2( CHECKING_SIDE , 0 ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcout ) ) {
						r = 1;
						SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcout , 0 /* All End */ );
					}
				}
				else {
					if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
						if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcout ) ) {
								r = 1;
								SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcout , 0 /* All End */ );
							}
						}
					}
					else { // 2010.05.28
						if ( lpin ) {
							if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < BM1 ) {
								if ( !_SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
									if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
										if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcin ) ) {
											r = 1;
											SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 2 /* Out Only */ );
											//
											if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcin ) ) {
												//
												if ( !_SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
													//
													_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 );
													//
													SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE , BATCHALL_SWITCH_LP_2_LP );
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
			}
		}
		//
		if ( r == 0 ) { // 2010.05 06
			if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
					if ( !lpout ) {
						if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) == BUFFER_TM_STATION ) {
							if ( lpin ) { // 2010.05.28
								//=====================================================================================================
								// 2010.07.20
								//=====================================================================================================
								switch( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( CM1 ) ) { // LP->LL
								case 0 : // NP(LP) , P(LL) , [LL-F] : Separate Mode
									//
									SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 2 /* Out Only */ );
									//
									if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcin ) ) {
										//
										if ( !_SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
											_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 );
											//=====================================================================
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , bmcin , -1 , TRUE , 0 , 3011 );
											//=====================================================================
											SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE , BATCHALL_SWITCH_LP_2_LP );
										}
									}
									break;
								case 1 : // NP(LL) , P(LL) , [LL-T] : LL Mode
									//
									SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 0 /* All End */ );
									//
									break;
								case 2 : // NP(LP) , P(LP) , [LL-F] : LP Mode
									//
									if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( bmcin ) ) {
										//
										SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 3 /* All Set Out With IN */ );
										//
										if ( !_SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
											_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 );
											//=====================================================================
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , bmcin , -1 , TRUE , 0 , 3011 );
											//=====================================================================
											SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE , BATCHALL_SWITCH_LP_2_LP );
										}
									}
									break;
								}
								//=====================================================================================================
								//=====================================================================================================
							}
							else {
								//=====================================================================================================
								// 2010.07.20
								//=====================================================================================================
								SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 0 /* All End */ );
								if ( bmcout != bmcin ) {
									SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcout , 0 /* All End */ );
								}
								//=====================================================================================================
								//=====================================================================================================
							}
						}
					}
					else {
						if ( lpin ) { // 2010.07.20
							if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) == BUFFER_FM_STATION ) {
								SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 1 /* IN Only */ );
							}
						}
						else {
							//=====================================================================================================
							// 2010.07.20
							//=====================================================================================================
							switch( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( CM1 + 1 ) ) { // LL->LP
							case 0 : // NP(LL) , P(LP) , [LL-F] : Separate Mode
								//
								if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) == BUFFER_FM_STATION ) {
									//
									SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 1 /* IN Only */ );
									//
								}
								//
								break;
							case 1 : // NP(LL) , P(LL) , [LL-T] : LL Mode
								//
								if ( SCHEDULER_BATCHALL_SWITCH_BM_ENDING_CHECK_INOUT_TOGETHER( bmcin ) ) { // 2010.07.22
									//
									SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 0 /* All End */ );
									//
									SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE , BATCHALL_SWITCH_LL_2_LL );
									//
								}
								else {
									//
									if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) == BUFFER_FM_STATION ) {
										//
										SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 1 /* IN Only */ );
										//
									}
									//
								}
								//
								break;
							case 2 : // NP(LP) , P(LP) , [LL-F] : LP Mode
								//
								if ( SCHEDULER_BATCHALL_SWITCH_BM_ENDING_CHECK_INOUT_TOGETHER( bmcin ) ) { // 2010.07.22
									//
									if ( !_SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
										//
										SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 3 /* All Set Out With IN */ );
										//
										_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 );
										//
										if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) != BUFFER_FM_STATION ) {
											//=====================================================================
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , bmcin , -1 , TRUE , 0 , 3012 );
											//=====================================================================
										}
										//
									}
									//
								}
								else {
									//
									if ( _SCH_MODULE_Get_BM_FULL_MODE( bmcin ) == BUFFER_FM_STATION ) {
										//
										SCHEDULER_BATCHALL_SWITCH_BM_ENDING( CHECKING_SIDE , bmcin , 1 /* IN Only */ );
										//
									}
									//
								}
								break;
							}
							//=====================================================================================================
							//=====================================================================================================
						}
					}
				}
			}
			//
		}
	}
	//===================================================================================================================================
	return 0;
}

int SCHEDULER_CONTROL_Chk_FM_ARM_Has_Been_Waiting_For_1SlotBM_Supply( int side , int mode ) { // 2011.10.10
	//
	int k , s , p , f = 0 , ICsts , ICsts2 , fs , fp;
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 0;
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
		//
		if ( mode == 0 ) { // 2011.10.17
			if ( Scheduler_No_More_Supply_Check( -1 ) ) return 0;
		}
		//
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , k ) == MUF_00_NOTUSE ) ) continue;
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return 0;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
			//
			if ( mode == 0 ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) continue;
				//
			}
			else { // 2011.10.17
				//
				f++;
				//
			}
			//
			if ( _SCH_MODULE_Get_BM_WAFER( k , 1 ) <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_BM_SIDE( k , 1 );
			p = _SCH_MODULE_Get_BM_POINTER( k , 1 );
			//
			if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( s , p , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) <= 0 ) {
				//
				if ( mode != 0 ) return 1;
				//
				continue;
				//
			}
			//
			f++;
		}
		//
		if ( f == 0 ) {
			if ( mode == 0 ) return 0;
			else             return 1;
		}
		else {
			if ( mode == 0 ) return 1;
			else             return 0;
		}
		//
	}
	else {
		//
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL ) return 0;
			fs = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
			fp = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		}
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL ) return 0;
			fs = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
			fp = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		}
		//
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( fs , k ) ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( fs , k ) == MUF_00_NOTUSE ) ) continue;
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return 0;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
			//
			if ( mode == 0 ) {
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) continue;
				//
			}
			else { // 2011.10.17
				//
				f++;
			}
			//
			if ( _SCH_MODULE_Get_BM_WAFER( k , 1 ) <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_BM_SIDE( k , 1 );
			p = _SCH_MODULE_Get_BM_POINTER( k , 1 );
			//
			if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( s , p , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) <= 0 ) {
				//
				if ( mode != 0 ) return 3;
				//
				continue;
			}
			//
			f++;
		}
		//
//		if ( f == 0 ) return 0; // 2011.10.13
		//
		//
		if ( f == 0 ) {
			if ( mode != 0 ) return 4;
		}
		else {
			if ( mode != 0 ) return 0;
		}
		//
		return 2;
		//
	}
}

BOOL SCHEDULER_NEXT_GO_PM_FREE( int s , int p , int *onepm , BOOL ); // 2013.05.07
BOOL SCHEDULER_HAS_GO_OTHER_PM( int s , int pm );

BOOL SCHEDULER_ONEFMTM_INVALID_SUPPLY( int s , int p , int *fm_mode ) { // 2013.05.08
	int c , k , tmfreecount , fmfreecount;
	int sels , selp , selr , regpm , pp , ps , op;
	int cs , cp , cr;
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return FALSE; // 2013.05.28
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) return FALSE;
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return FALSE;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() != 0 ) return FALSE;
	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) return FALSE;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) {
			return FALSE;
		}
	}
	//
	fmfreecount = 0;
	tmfreecount = 0;
	regpm = 0;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.01
		//
		for ( c = 0 ; c < MAX_CASSETTE_SIDE ; c++ ) {
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( c ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( c , k ) == MUF_00_NOTUSE ) return FALSE;
			//
		}
		//
		if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return FALSE;
		//
		if ( _SCH_MODULE_Get_BM_WAFER( k,1 ) <= 0 ) {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
				tmfreecount++;
			}
			else {
				fmfreecount++;
			}
		}
		else {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
				//
				if ( _SCH_MODULE_Get_BM_STATUS( k,1 ) == BUFFER_INWAIT_STATUS ) {
					//
					ps = _SCH_MODULE_Get_BM_SIDE( k,1 );
					pp = _SCH_MODULE_Get_BM_POINTER( k,1 );
					//
					if ( SCHEDULER_NEXT_GO_PM_FREE( ps , pp , &op , TRUE ) ) regpm = op;
					//
				}
			}
		}
		//
	}
	//
	if ( fmfreecount < 1 ) return TRUE;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( 0,TA_STATION ) > 0 ) { // 2013.05.15
		//
		ps = _SCH_MODULE_Get_TM_SIDE( 0,TA_STATION );
		pp = _SCH_MODULE_Get_TM_POINTER( 0,TA_STATION );
		//
		if ( SCHEDULER_NEXT_GO_PM_FREE( ps , pp , &op , FALSE ) ) regpm = op;
		//
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( 0,TB_STATION ) > 0 ) { // 2013.05.15
		//
		ps = _SCH_MODULE_Get_TM_SIDE( 0,TB_STATION );
		pp = _SCH_MODULE_Get_TM_POINTER( 0,TB_STATION );
		//
		if ( SCHEDULER_NEXT_GO_PM_FREE( ps , pp , &op , FALSE ) ) regpm = op;
		//
	}
	//
	if ( SCHEDULER_NEXT_GO_PM_FREE( s , p , &op , TRUE ) ) {
		if ( regpm <= 0 ) return FALSE;
		if ( regpm != op ) return FALSE;
	}
	//
	if ( regpm <= 0 ) {
		if ( tmfreecount < 1 ) {
			*fm_mode = 0;
			return TRUE;
		}
	}
	//
	sels = -1;
	selp = -1;
	selr = -1;
	//
	for ( c = 0 ; c < MAX_CASSETTE_SIDE ; c++ ) {
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( c ) ) continue;
		//
		for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathRange( c , k ) <= 0 ) continue;
			//
			if ( ( _SCH_CLUSTER_Get_PathStatus( c , k ) != SCHEDULER_READY ) && ( _SCH_CLUSTER_Get_PathStatus( c , k ) != SCHEDULER_CM_END ) ) {
				//
				cs = c;
				cp = _SCH_CLUSTER_Get_SupplyID( c , k );
				cr = _SCH_CLUSTER_Get_Ex_PrcsID( c , k );
				//
				if ( sels == -1 ) {
					sels = cs;
					selp = cp;
					selr = cr;
				}
				else {
					if      ( ( cr == 0 ) && ( selr == 0 ) ) {
						if ( cp < selp ) {
							sels = cs;
							selp = cp;
							selr = cr;
						}
					}
					else if ( ( cr == 0 ) && ( selr != 0 ) ) {
					}
					else if ( ( cr != 0 ) && ( selr == 0 ) ) {
						sels = cs;
						selp = cp;
						selr = cr;
					}
					else if ( ( cr != 0 ) && ( selr != 0 ) ) {
						//
						ps = ( cr   % 1000 ) / 10;
						pp = ( selr % 1000 ) / 10;
						//
						if ( ( ps >= PM1 ) && ( ps < AL ) && ( pp >= PM1 ) && ( pp < AL ) ) {
							if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , ps ) < _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , pp ) ) {
								sels = cs;
								selp = cp;
								selr = cr;
							}
						}
						else {
							if ( cp < selp ) {
								sels = cs;
								selp = cp;
								selr = cr;
							}
						}
					}
				}
			}
		}
	}
	//
	if ( ( sels == -1 ) || ( sels == s ) ) return FALSE;
	//
	if ( !SCHEDULER_HAS_GO_OTHER_PM( sels , -2 ) ) return FALSE;
	//

//printf( "[S]===================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "======================================================\n" );
//printf( "[E]===================================================\n" );

	return TRUE;
	//
}

void SCHEDULER_ONEFMTM_FM_LOCK_FREE( int side ) { // 2013.05.08
	int k , c , tmlock , fmlock , s , p , op , caseonecheck , hastmside;
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return; // 2013.05.28
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) return;
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() != 0 ) return;
	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) return;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_MIDDLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) {
			return;
		}
	}
	//
	//
	for ( c = 0 ; c < MAX_CASSETTE_SIDE ; c++ ) { // 2013.05.15
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( c ) ) continue;
		//
		if ( SCHEDULER_HAS_GO_OTHER_PM( c , -1 ) ) break;
		//
	}
	//
	if ( c != MAX_CASSETTE_SIDE ) return;
	//
	hastmside = FALSE;
	caseonecheck = TRUE;
	//
	tmlock = 0;
	fmlock = 0;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) ) continue;
		//
		for ( c = 0 ; c < MAX_CASSETTE_SIDE ; c++ ) {
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( c ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( c , k ) == MUF_00_NOTUSE ) return;
			//
		}
		//
		if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( k,1 ) <= 0 ) {
				//
				hastmside = TRUE;
				//
				caseonecheck = FALSE;
//				return;
				continue;
			}
			//
			if ( _SCH_MODULE_Get_BM_STATUS( k,1 ) != BUFFER_INWAIT_STATUS ) {
				//
				hastmside = TRUE;
				//
				caseonecheck = FALSE;
//				return;
				continue;
			}
			//
			s = _SCH_MODULE_Get_BM_SIDE( k,1 );
			p = _SCH_MODULE_Get_BM_POINTER( k,1 );
			//
			if ( SCHEDULER_NEXT_GO_PM_FREE( s , p , &op , TRUE ) ) {
				//
				hastmside = TRUE;
				//
				caseonecheck = FALSE;
//				return;
				continue;
			}
			//
			tmlock++;
			//
		}
		else {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) return;
			if ( _SCH_MODULE_Get_BM_WAFER( k,1 ) > 0 ) return;
			if ( fmlock == 0 ) fmlock = k;
		}
		//
	}
	//
	if ( caseonecheck && ( fmlock > 0 ) && ( tmlock > 0 ) ) {
		//=====================================================================
		//
		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , fmlock ); // 2017.02.08
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , fmlock , -1 , TRUE , 0 , 1986 );
		//=====================================================================
	}
	else {
		if ( !hastmside && ( fmlock > 0 ) ) {
			//
			for ( k = PM1 ; k < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; k++ ) {
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( k , TRUE , -1 ) ) continue;
				//
//				if ( _SCH_MODULE_Get_PM_WAFER( k,0 ) <= 0 ) continue; // 2014.01.10
				if ( !SCH_PM_IS_PICKING( k ) ) continue; // 2014.01.10
				//
				break;
			}
			//
			if ( k == ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) return;
			//
			//=====================================================================
			//
			SCHEDULING_BM_LOCK_CHECK_TM_SIDE( side , fmlock ); // 2017.02.08
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , fmlock , -1 , TRUE , 0 , 1987 );
			//=====================================================================
		}
	}
}


//
BOOL SCHEDULER_CONTROL_Chk_BM_HAS_IN_OUT_BMOUTFIRST() { // 2015.02.05
	int k;
	//
	if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) return FALSE; // 2015.02.11
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) return FALSE; // 2015.02.11
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return FALSE;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k ) > 0 ) continue; // 2015.02.12
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) <= 0 ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue;
		//
		return TRUE;
		//
	}
	//
	return FALSE;
	//
}
//

//
BOOL SCHEDULER_CONTROL_Chk_BM_ONESLOT_FULL_AND_TM_ALL_RETURN( int bmc ) { // 2015.06.18
	int i , j , s , p , yes;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) != 1 ) return FALSE;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return FALSE;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) > 0 ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , -1 ) <= 0 ) return FALSE;
	//
	yes = 0;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
			if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) return FALSE;
		}
		//
		for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
			//
			s = _SCH_MODULE_Get_TM_SIDE( 0 , j );
			p = _SCH_MODULE_Get_TM_POINTER( 0 , j );
			//
			if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) return FALSE;
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) yes++;
			//
		}
		//
		Sleep(1);
		//
	}
	//
	if ( yes > 0 ) return TRUE;
	//
	return FALSE;
}
//
BOOL SCHEDULER_FM_PM_FAIL_RETURN( int Side ) { // 2016.03.07
	int FM_Pm , Res;
	int FM_TSlot , FM_Slot , FM_dbSide , FM_Pointer;
	int FM_TSlot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2;
	//
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( Side , 0 , "FEM STEP SCHEDULER_FM_PM_FAIL_RETURN = 1\n" );
	//==============================================================================================
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return TRUE;
	//
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( Side , 0 , "FEM STEP SCHEDULER_FM_PM_FAIL_RETURN = 2\n" );
	//==============================================================================================
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return TRUE;
	//
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( Side , 0 , "FEM STEP SCHEDULER_FM_PM_FAIL_RETURN = 3\n" );
	//==============================================================================================
	//
	Res = SCHEDULER_FM_PM_GET_PICK_PM( -1 , Side , -1 , &FM_Pm , &FM_TSlot , &FM_Slot , &FM_dbSide , &FM_Pointer , &FM_TSlot2 , &FM_Slot2 , &FM_dbSide2 , &FM_Pointer2 );
	//
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( Side , 0 , "FEM STEP SCHEDULER_FM_PM_FAIL_RETURN = 4 [Res=%d],FM_Pm=%d,S=%d,%d,P=%d,%d,W=%d,%d,T=%d,%d\n" , Res , FM_Pm , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 );
	//==============================================================================================
	//
	if ( Res <= 0 ) return TRUE;
	//
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		//
		switch( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) {
		case -1 :
			return FALSE;
			break;
		}
		//
		//----------------------------------------------------------------------------------------------------------------------------
		_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
		//----------------------------------------------------------------------
		//
		if ( FM_Slot > 0 ) _SCH_CLUSTER_Set_PathDo( FM_dbSide , FM_Pointer , PATHDO_WAFER_RETURN );
		if ( FM_Slot2 > 0 ) _SCH_CLUSTER_Set_PathDo( FM_dbSide2 , FM_Pointer2 , PATHDO_WAFER_RETURN );
		//
		switch( _SCH_MACRO_FM_PLACE_TO_CM( Side , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , 0 ) ) {
		case -1 :
			return FALSE;
			break;
		}
		//
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		//
		if ( FM_Slot > 0 ) {
			//
			switch( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
			//----------------------------------------------------------------------------------------------------------------------------
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//----------------------------------------------------------------------
			//
			_SCH_CLUSTER_Set_PathDo( FM_dbSide , FM_Pointer , PATHDO_WAFER_RETURN );
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( Side , FM_Slot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , FALSE , FALSE , 0 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
		}
		//
		if ( FM_Slot2 > 0 ) {
			//
			switch( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 , 0 , 0 , -1 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
			//----------------------------------------------------------------------------------------------------------------------------
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//----------------------------------------------------------------------
			//
			_SCH_CLUSTER_Set_PathDo( FM_dbSide2 , FM_Pointer2 , PATHDO_WAFER_RETURN );
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( Side , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 , FALSE , FALSE , 0 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
		}
		//
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		//
		if ( FM_Slot > 0 ) {
			//
			switch( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , -1 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
			//----------------------------------------------------------------------------------------------------------------------------
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//----------------------------------------------------------------------
			//
			_SCH_CLUSTER_Set_PathDo( FM_dbSide , FM_Pointer , PATHDO_WAFER_RETURN );
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( Side , 0 , 0 , 0 , FM_Slot , FM_dbSide , FM_Pointer , FALSE , FALSE , 0 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
		}
		//
		if ( FM_Slot2 > 0 ) {
			//
			switch( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
			//----------------------------------------------------------------------------------------------------------------------------
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//----------------------------------------------------------------------
			//
			_SCH_CLUSTER_Set_PathDo( FM_dbSide2 , FM_Pointer2 , PATHDO_WAFER_RETURN );
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( Side , 0 , 0 , 0 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , 0 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
		}
		//
	}
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_AL_BM_PLACE_DIFF( int bmc ) { // 2016.06.26
	//
	int i , bs , bp , s , p , f , iw;

	if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) return FALSE;
	//
	f = 0;
	iw = 0;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) return FALSE;
		//
		iw++;
		//
		bs = _SCH_MODULE_Get_BM_SIDE( bmc , i );
		bp = _SCH_MODULE_Get_BM_POINTER( bmc , i );
		//
		if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
			//
			s = _SCH_MODULE_Get_MFAL_SIDE();
			p = _SCH_MODULE_Get_MFAL_POINTER();
			//
			if ( _SCH_CLUSTER_Get_ClusterIndex( bs , bp ) != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) f++;
			//
		}
		//
		if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
			//
			s = _SCH_MODULE_Get_MFALS_SIDE(2);
			p = _SCH_MODULE_Get_MFALS_POINTER(2);
			//
			if ( _SCH_CLUSTER_Get_ClusterIndex( bs , bp ) != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) f++;
			//
		}
		//
	}
	//
	if ( iw <= 0 ) return FALSE;
	//
	if ( f > 0 ) return TRUE;
	//
	if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) return TRUE;
	//
	return FALSE;
	//
}



BOOL Scheduler_SingleSwap_Has_Full_IN( int bmc ) { // 2016.08.25
	int s , ic;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) return FALSE;
	//
	s = _SCH_PRM_GET_MODULE_SIZE( bmc );
	//
	if ( s == 1 ) return FALSE;
	//
	ic = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_INWAIT_STATUS );
	//
	if ( ( s / 2 ) == ic ) return TRUE;
	//
	return FALSE;
	//
}

//
BOOL Scheduler_No_More_Supply_and_all_Fmside() { // 2017.10.25
	int i;
	//
	if ( !Scheduler_No_More_Supply_Check( -1 ) ) return FALSE;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( ( _SCH_SYSTEM_MODE_END_GET( i ) == 0 ) && !_SCH_MODULE_Chk_FM_Finish_Status( i ) ) break;
		//
	}
	//
	if ( i == MAX_CASSETTE_SIDE ) return TRUE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2018.06.01
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) return FALSE;
		}
		//
	}
	//
	return TRUE;
	//
}


/*
//
// 2018.09.17
//
BOOL MULTI_COOLER_PLACE_PICK_RUN( int *wafer1 , int *side1 , int *pt1 , int *wafer2 , int *side2 , int *pt2 ) { // 2018.02.20
	int wfr1action , wfr2action; // 0: PlaceCM , 1:PlaceIC
	int ret , ICsts , ICsts2 , s , FM_CM;
	int Arm , sels , selw , selp;
	//
	//====================================================================================================================================================
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		//
		wfr1action = 0;
		//
	}
	else {
		//
		if ( *wafer1 <= 0 ) {
			//
			wfr1action = 1;
			//
		}
		else {
			//
			wfr1action = 0;
			//
			if ( _SCH_CLUSTER_Get_PathDo( *side1 , *pt1 ) != PATHDO_WAFER_RETURN ) {
				//
				if ( !SCHEDULER_COOLING_SKIP_AL0( *side1 , *pt1 ) ) {
					//
					ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( *side1 , *pt1 , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
					//
					if ( ret <= 0 ) return FALSE;
					//
					//=======================================================================================
					// PLACE IC
					//=======================================================================================
					//
					wfr1action = 1;
					//
					FM_CM    = _SCH_CLUSTER_Get_PathOut( *side1 , *pt1 );
					//
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , *side1 , IC , ICsts , 0 , FM_CM , FM_CM , TRUE , *wafer1 , 0 , *side1 , 0 , *pt1 , 0 ) == SYS_ABORTED ) {
						//
						_SCH_LOG_LOT_PRINTF( *side1 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer1 , 9 , *wafer1 , 9 , *wafer1 );
						//
						return FALSE;
						//
					}
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , *wafer1 , ICsts , *side1 , *pt1 , 0 , 0 , 0 , 0 , 0 , -1 , -1 );
					//============================================================================
					_SCH_MACRO_SPAWN_FM_MCOOLING( *side1 , ICsts , 0 , *wafer1 , 0 , FM_CM );
					//============================================================================
					//
					*wafer1 = 0;
					//
				}
			}
			//
		}
		//
	}
	//====================================================================================================================================================
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		//
		wfr2action = 0;
		//
	}
	else {
		//
		if ( *wafer2 <= 0 ) {
			//
			wfr2action = 1;
			//
		}
		else {
			//
			wfr2action = 0;
			//
			if ( _SCH_CLUSTER_Get_PathDo( *side2 , *pt2 ) != PATHDO_WAFER_RETURN ) {
				//
				if ( !SCHEDULER_COOLING_SKIP_AL0( *side2 , *pt2 ) ) {
					//
					ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( *side2 , *pt2 , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
					//
					if ( ret <= 0 ) return FALSE;
					//
					//=======================================================================================
					// PLACE IC
					//=======================================================================================
					//
					wfr2action = 1;
					//
					FM_CM    = _SCH_CLUSTER_Get_PathOut( *side2 , *pt2 );
					//
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , *side2 , IC , 0 , ICsts , FM_CM , FM_CM , TRUE , 0 , *wafer2 , 0 , *side2 , 0 , *pt2 ) == SYS_ABORTED ) {
						//
						_SCH_LOG_LOT_PRINTF( *side2 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer2 , 9 , *wafer2 , 9 , *wafer2 );
						//
						return FALSE;
						//
					}
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , IC , *wafer2 , ICsts , *side2 , *pt2 , -1 , -1 );
					//============================================================================
					_SCH_MACRO_SPAWN_FM_MCOOLING( *side2 , 0 , ICsts , 0 , *wafer2 , FM_CM );
					//============================================================================
					//
					*wafer2 = 0;
					//
				}
			}
			//
		}
		//
	}
	//====================================================================================================================================================
	//
	if ( ( wfr1action == 1 ) || ( wfr2action == 1 ) ) {
		//
		ret = _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &s , &ICsts , &ICsts2 );
		//
		if ( ret != SYS_SUCCESS ) return TRUE;
		//
		if ( ( wfr1action == 1 ) && ( wfr2action == 1 ) ) {
			//
			Arm = TA_STATION;
			//
			sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
			selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
			selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
			//
			FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			//
			//==================================================================================================================
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
				return FALSE;
			}
			//========================================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//
			*wafer1 = selw;
			*side1 = sels;
			*pt1 = selp;
			//			
			if ( ICsts2 > 0 ) {
				//
				ICsts = ICsts2;
				//
				Arm = TB_STATION;
				//
				sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
				selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
				selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
				//
				FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
				//
				//==================================================================================================================
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
					return FALSE;
				}
				//========================================================================================
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
				//
				*wafer2 = selw;
				*side2 = sels;
				*pt2 = selp;
				//
			}
			//
		}
		else {
			//
			if ( wfr1action == 1 )
				Arm = TA_STATION;
			else
				Arm = TB_STATION;
			//
			sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
			selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
			selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
			//
			FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			//
			//==================================================================================================================
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
				return FALSE;
			}
			//========================================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//
			//
			if ( wfr1action == 1 ) {
				//
				*wafer1 = selw;
				*side1 = sels;
				*pt1 = selp;
				//			
			}
			else {
				//
				*wafer2 = selw;
				*side2 = sels;
				*pt2 = selp;
				//			
			}
		}
	}
	return TRUE;
}
*/
//
// 2018.09.17
//
BOOL MULTI_COOLER_PLACE_PICK_RUN( int *wafer1 , int *side1 , int *pt1 , int *wafer2 , int *side2 , int *pt2 ) { // 2018.02.20
	int wfr1action , wfr2action; // 0: PlaceCM , 1:PlaceIC
	int ret , ICsts , ICsts2 , s , FM_CM , FM_CM2;
	int Arm , sels , selw , selp;
	//
	BOOL dualrun;
	//
	//====================================================================================================================================================
	//
	dualrun = FALSE;
	//
	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 10 ) == 1 ) { // 2018.09.28
		//
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( ( *wafer1 > 0 ) && ( *wafer2 > 0 ) ) {
				if ( _SCH_CLUSTER_Get_PathDo( *side1 , *pt1 ) != PATHDO_WAFER_RETURN ) {
					if ( !SCHEDULER_COOLING_SKIP_AL0( *side1 , *pt1 ) ) {
						if ( _SCH_CLUSTER_Get_PathDo( *side2 , *pt2 ) != PATHDO_WAFER_RETURN ) {
							if ( !SCHEDULER_COOLING_SKIP_AL0( *side2 , *pt2 ) ) {
								dualrun = TRUE;
							}
						}
					}
				}
			}
			//
		}
	}
	//
	if ( dualrun ) {
		//
		ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( *side1 , *pt1 , *side2 , *pt2 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
		//
		if ( ret <= 0 ) return FALSE;
		//
		if ( ICsts <= 0 ) return FALSE;
		if ( ICsts2 <= 0 ) return FALSE;
		//
		FM_CM    = _SCH_CLUSTER_Get_PathOut( *side1 , *pt1 );
		FM_CM2   = _SCH_CLUSTER_Get_PathOut( *side2 , *pt2 );
		//
		if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , *side1 , IC , ICsts , ICsts2 , FM_CM , FM_CM2 , TRUE , *wafer1 , *wafer2 , *side1 , *side2 , *pt1 , *pt2 ) == SYS_ABORTED ) {
			//
			_SCH_LOG_LOT_PRINTF( *side1 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer1 , 9 , *wafer1 , 9 , *wafer1 );
			//
			return FALSE;
			//
		}
		//========================================================================================
		_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , *wafer1 , ICsts , *side1 , *pt1 , IC , *wafer2 , ICsts2 , *side2 , *pt2 , -1 , -1 );
		//============================================================================
		_SCH_MACRO_SPAWN_FM_MCOOLING( *side1 , ICsts , ICsts2 , *wafer1 , *wafer2 , FM_CM );
		//============================================================================
		//
		*wafer1 = 0;
		*wafer2 = 0;
		//
		wfr1action = 1;
		wfr2action = 1;
		//
	}
	//
	else {
		//
		if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			//
			wfr1action = 0;
			//
		}
		else {
			//
			if ( *wafer1 <= 0 ) {
				//
				wfr1action = 1;
				//
			}
			else {
				//
				wfr1action = 0;
				//
				if ( _SCH_CLUSTER_Get_PathDo( *side1 , *pt1 ) != PATHDO_WAFER_RETURN ) {
					//
					if ( !SCHEDULER_COOLING_SKIP_AL0( *side1 , *pt1 ) ) {
						//
						ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( *side1 , *pt1 , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
						//
						if ( ret <= 0 ) return FALSE;
						//
						//=======================================================================================
						// PLACE IC
						//=======================================================================================
						//
						wfr1action = 1;
						//
						FM_CM    = _SCH_CLUSTER_Get_PathOut( *side1 , *pt1 );
						//
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , *side1 , IC , ICsts , 0 , FM_CM , FM_CM , TRUE , *wafer1 , 0 , *side1 , 0 , *pt1 , 0 ) == SYS_ABORTED ) {
							//
							_SCH_LOG_LOT_PRINTF( *side1 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer1 , 9 , *wafer1 , 9 , *wafer1 );
							//
							return FALSE;
							//
						}
						//========================================================================================
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , *wafer1 , ICsts , *side1 , *pt1 , 0 , 0 , 0 , 0 , 0 , -1 , -1 );
						//============================================================================
						_SCH_MACRO_SPAWN_FM_MCOOLING( *side1 , ICsts , 0 , *wafer1 , 0 , FM_CM );
						//============================================================================
						//
						*wafer1 = 0;
						//
					}
				}
				//
			}
			//
		}
		//
		//====================================================================================================================================================
		//
		if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			//
			wfr2action = 0;
			//
		}
		else {
			//
			if ( *wafer2 <= 0 ) {
				//
				wfr2action = 1;
				//
			}
			else {
				//
				wfr2action = 0;
				//
				if ( _SCH_CLUSTER_Get_PathDo( *side2 , *pt2 ) != PATHDO_WAFER_RETURN ) {
					//
					if ( !SCHEDULER_COOLING_SKIP_AL0( *side2 , *pt2 ) ) {
						//
						ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( *side2 , *pt2 , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
						//
						if ( ret <= 0 ) return FALSE;
						//
						//=======================================================================================
						// PLACE IC
						//=======================================================================================
						//
						wfr2action = 1;
						//
						FM_CM    = _SCH_CLUSTER_Get_PathOut( *side2 , *pt2 );
						//
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , *side2 , IC , 0 , ICsts , FM_CM , FM_CM , TRUE , 0 , *wafer2 , 0 , *side2 , 0 , *pt2 ) == SYS_ABORTED ) {
							//
							_SCH_LOG_LOT_PRINTF( *side2 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer2 , 9 , *wafer2 , 9 , *wafer2 );
							//
							return FALSE;
							//
						}
						//========================================================================================
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , IC , *wafer2 , ICsts , *side2 , *pt2 , -1 , -1 );
						//============================================================================
						_SCH_MACRO_SPAWN_FM_MCOOLING( *side2 , 0 , ICsts , 0 , *wafer2 , FM_CM );
						//============================================================================
						//
						*wafer2 = 0;
						//
					}
				}
				//
			}
			//
		}
	}
	//====================================================================================================================================================
	//====================================================================================================================================================
	//====================================================================================================================================================
	//====================================================================================================================================================
	//====================================================================================================================================================
	//====================================================================================================================================================
	//====================================================================================================================================================
	//
	if ( ( wfr1action == 1 ) || ( wfr2action == 1 ) ) {
		//
		ret = _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &s , &ICsts , &ICsts2 );
		//
		if ( ret != SYS_SUCCESS ) return TRUE;
		//
		if ( ( wfr1action == 1 ) && ( wfr2action == 1 ) ) {
			//
			if ( ( ICsts > 0 ) && ( ICsts2 > 0 ) ) {
				//
				*side1 = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
				*wafer1 = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
				*pt1 = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
				//
				FM_CM = _SCH_CLUSTER_Get_PathOut( *side1 , *pt1 );
				//
				//
				*side2 = _SCH_MODULE_Get_MFIC_SIDE( ICsts2 );
				*wafer2 = _SCH_MODULE_Get_MFIC_WAFER( ICsts2 );
				*pt2 = _SCH_MODULE_Get_MFIC_POINTER( ICsts2 );
				//
				FM_CM2 = _SCH_CLUSTER_Get_PathOut( *side2 , *pt2 );
				//
				//==================================================================================================================
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , *side1 , IC , ICsts , ICsts2 , FM_CM , -1 , TRUE , *wafer1 , *wafer2 , *side1 , *side2 , *pt1 , *pt2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( *side1 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , *wafer1 , 9 , *wafer1 , 9 , *wafer1 );
					return FALSE;
				}
				//========================================================================================
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , *wafer1 , ICsts , *side1 , *pt1 , IC , *wafer2 , ICsts2 , *side2 , *pt2 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
				//========================================================================================
			}
			else {
				Arm = TA_STATION;
				//
				sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
				selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
				selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
				//
				FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
				//
				//==================================================================================================================
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
					return FALSE;
				}
				//========================================================================================
				_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
				//
				*wafer1 = selw;
				*side1 = sels;
				*pt1 = selp;
				//			
				if ( ICsts2 > 0 ) {
					//
					ICsts = ICsts2;
					//
					Arm = TB_STATION;
					//
					sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
					selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
					selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
					//
					FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
					//
					//==================================================================================================================
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
						return FALSE;
					}
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
					//
					*wafer2 = selw;
					*side2 = sels;
					*pt2 = selp;
					//
				}
				//
			}
		}
		else {
			//
			if ( wfr1action == 1 )
				Arm = TA_STATION;
			else
				Arm = TB_STATION;
			//
			sels = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
			selw = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
			selp = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
			//
			FM_CM = _SCH_CLUSTER_Get_PathOut( sels , selp );
			//
			//==================================================================================================================
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , sels , IC , ( Arm == TA_STATION ) ? ICsts : 0 , ( Arm == TA_STATION ) ? 0 : ICsts , FM_CM , -1 , TRUE , ( Arm == TA_STATION ) ? selw : 0 , ( Arm == TA_STATION ) ? 0 : selw , ( Arm == TA_STATION ) ? sels : 0 , ( Arm == TA_STATION ) ? 0 : sels , ( Arm == TA_STATION ) ? selp : 0 , ( Arm == TA_STATION ) ? 0 : selp ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( sels , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , selw , 9 , selw , 9 , selw );
				return FALSE;
			}
			//========================================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( Arm == TA_STATION ) ? selw : 0 , ICsts , sels , selp , IC , ( Arm == TA_STATION ) ? 0 : selw , ICsts , sels , selp , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC );
			//
			//
			if ( wfr1action == 1 ) {
				//
				*wafer1 = selw;
				*side1 = sels;
				*pt1 = selp;
				//			
			}
			else {
				//
				*wafer2 = selw;
				*side2 = sels;
				*pt2 = selp;
				//			
			}
		}
	}
	return TRUE;
}


BOOL MULTI_COOLER_PLACE_POSSIBLE( int side , int pt ) { // 2018.02.20
	int ret , ICsts , ICsts2;
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) return TRUE;
	//
	if ( _SCH_CLUSTER_Get_PathDo( side , pt ) == PATHDO_WAFER_RETURN ) return TRUE;
	//
	if ( SCHEDULER_COOLING_SKIP_AL0( side , pt ) ) return TRUE;
	//
	ret = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( side , pt , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
	//
	if ( ret > 0 ) return TRUE;
	//
	return FALSE;
}

								//

int USER_DLL_SCH_INF_RUN_CONTROL_FEM_SWITCH_STYLE_0( int CHECKING_SIDE ) {
	int	FM_Slot;
	int	FM_Slot2;
	int	FM_TSlot , FM_CO1 , FM_CO2;
	int	FM_TSlot2;
	int FM_Slot_Real; // 2006.07.24
	int FM_TSlot_Real; // 2006.07.24
	int	FM_Pointer;
	int	FM_Pointer2;
	int	FM_Side;
	int	FM_Side2;
//	int	FM_TSX; // 2007.09.11
	int k;
	int FM_Buffer;
	int FM_Buffer2; // 2006.09.26
	int FM_Buffer_Toggle; // 2007.04.11
	int	RunBuffer;
	int Result;
	int FM_CM;
	int FM_Mode;
	int FM_Mode4_Action; // 2013.05.29
	int FM_dbSidex , FM_dbSide , FM_dbSide2; // 2007.09.11
//	int FM_Side_Check; // 2007.05.17
	//
	int FM_Ret_Pointer; // 2003.10.07
	//
	int pgin , pgout , lx , lxb;

	int bck[MAX_BM_CHAMBER_DEPTH]; // 2006.12.22

	int retpointer;
	int cpreject;
	int cside;
	int middleswap_tag; // 2006.02.22
	int bmd;
	int FM_Bufferx , FM_gpmc; // 2006.12.22
	//-----------------------------------------------
	int ICsts , ICsts2 , ICfinger , ALsts , ICsts_Real; // 2007.02.28
	int Premove; //A LJM 2009.02.17
	//-----------------------------------------------
	int mxcmb , mxcnt , mxside , mxpt , mxrside , mxrpt; // 2007.04.11
	//-----------------------------------------------
	int place_bm_Separate_supply; // 2007.05.15
	//-----------------------------------------------
	int dmi; // 2007.05.21
	//-----------------------------------------------
	int ret_rcm , ret_pt , al_arm , al_arm2 , dum_pt , dum_rcm;

	int FM_Other_Pick;
	int coolplace;
	int mfic_locked;
//	int for_only;//A LJM 2009.02.25
	//-----------------------------------------------
	int MCR_PICK_FROM_FM; // 2010.08.11
	int act , actwaitcheck; // 2011.05.18
	int plcslot; // 2013.10.01
	int MCR_PICK_NOTFINISH_STS; // 2014.11.07

	int FM_dbSideM , FM_PointerM , FM_SlotM , FM_SlotM_Real; // 2013.10.01
	int FM_dbSideM2 , FM_SlotM2 , FM_SlotM2_Real; // 2014.03.11
	int FM_Pm; // 2014.11.07

	int Dual_AL_Dual_FM_Check; // 2014.03.06
	BOOL MULTI_PM_DIRECT_MODE; // 2014.11.25
	//----------------------------------------------------------------------
	int rmode , pmslotmode; // 2015.05.27
	BOOL NoMore; // 2015.06.22 
	//----------------------------------------------------------------------
	BOOL MULTI_PM_PICK_FROM_PM; // 2015.11.21
	//----------------------------------------------------------------------
	BOOL FixBMPlace; // 2016.06.21
	int FM_Buffer_Temp; // 2017.01.12

	//----------------------------------------------------------------------
	FM_Pick_Running_Blocking_style_0 = 0; // 2008.03.31 // 2008.09.25
	FM_Place_Waiting_for_BM_style_0 = 0; // 2014.01.03
	//-------------------------------------------------------------------------------------
	// PICK FROM CM & PLACE TO BM PART
	//-------------------------------------------------------------------------------------

// 0 : LP->LP
// 1 : LL->LL
// 2 : LP->LL
// 3 : LL->LP
	//============================================================================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 00 SELECT [%d]\n" , Dual_AL_Dual_FM_Second_Check );
	//============================================================================================================================================
	switch ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) ) { // 2010.01.19
	case BATCHALL_SWITCH_LL_2_LL :
		Result = SCHEDULER_BATCHALL_SWITCH_BM_ONLY_SUPPLY_CHECK( CHECKING_SIDE );
		if ( Result == -1 ) return 0;
		if ( Result ==  1 ) return 1;
		Result = SCHEDULER_BATCHALL_SWITCH_BM_ONLY_RETURN_CHECK( CHECKING_SIDE , FALSE , FALSE );
		if ( Result == -1 ) return 0;
		if ( Result ==  1 ) return 1;
		return 1;
		break;
	case BATCHALL_SWITCH_LP_2_LL :
		Result = SCHEDULER_BATCHALL_SWITCH_BM_ONLY_RETURN_CHECK( CHECKING_SIDE , TRUE , FALSE );
		if ( Result == -1 ) return 0;
		if ( Result ==  1 ) return 1;
		break;
	case BATCHALL_SWITCH_LL_2_LP :
		Result = SCHEDULER_BATCHALL_SWITCH_BM_ONLY_SUPPLY_CHECK( CHECKING_SIDE );
		if ( Result == -1 ) return 0;
		if ( Result ==  1 ) return 1;
		Result = SCHEDULER_BATCHALL_SWITCH_BM_ONLY_RETURN_CHECK( CHECKING_SIDE , FALSE , TRUE ); // 2010.05.12
		if ( Result == -1 ) return 0;
		if ( Result ==  1 ) return 1;
		break;
	}
	//============================================================================================================================================
	//============================================================================================================================================
	//============================================================================================================================================
	if ( 0 == Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 2 ) ) return 0; // 2019.03.15
	//============================================================================================================================================
	//============================================================================================================================================
	//============================================================================================================================================
	MULTI_PM_DIRECT_MODE = FALSE; // 2014.11.25
	//
	MULTI_PM_PICK_FROM_PM = FALSE; // 2015.11.21
	//
	FM_Mode = 0;
	FM_Buffer = 0; // 2004.11.08
	FM_Other_Pick = FALSE; // 2007.09.28
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
		if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) { // 2002.10.29
			//==========================================================================================
			switch ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) / 2 ) { // 2003.11.28
			case 0 :
				k = CHECKING_SIDE + BM1;
				break;
			case 1 :
				k = 0 + BM1;
				break;
			case 2 :
				k = 1 + BM1;
				break;
			}
			//==========================================================================================
//				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2007.05.28
//			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2007.05.28 // 2008.10.24
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.05.28 // 2008.10.24
//				if ( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( k ) == -1 ) { // 2008.11.24
				if ( scheduler_Check_NotUse_BM_For_Switch( CHECKING_SIDE , k ) ) { // 2008.11.24
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) { // 2008.10.15
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) { // 2008.05.26
//							if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
							if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15
								//=================================================================================
								// 2006.09.07
								//=================================================================================
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
									FM_Mode = 1;
									FM_Buffer = k;
								}
								//=================================================================================
								else {
									if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02
										FM_Mode = 1;
										FM_Buffer = k;
									}
								}
								//=================================================================================
							}
						}
					}
				}
/*
// 2008.11.24
				else if (
					( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( k ) == CHECKING_SIDE ) && // 2003.11.07
					( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 )
					) {
					if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) { // 2008.05.26
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , TRUE , k ) ) {
							if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
								FM_Mode = 2;
								FM_Buffer = k;
							}
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , FALSE , k ) ) {
								if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
									if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
										FM_Mode = 2;
										FM_Buffer = k;
									}
								}
							}
						}
					}
				}
*/
			}
			if ( FM_Mode == 0 ) {
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
//						if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2008.10.24
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.05.28 // 2008.10.24
//						if ( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( k ) == -1 ) { // 2008.11.24
						if ( scheduler_Check_NotUse_BM_For_Switch( CHECKING_SIDE , k ) ) { // 2008.11.24
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) { // 2008.10.15
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) { // 2008.05.26
//									if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
									if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15
										//=================================================================================
										// 2006.09.07
										//=================================================================================
										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
											FM_Mode = 1;
											FM_Buffer = k;
											break;
										}
										//=================================================================================
										else {
											if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02
												FM_Mode = 1;
												FM_Buffer = k;
												break;
											}
										}
										//=================================================================================
									}
								}
							}
						}
/*
// 2008.11.24
						else if (
							( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( k ) == CHECKING_SIDE ) && // 2003.11.07
							( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 )
							) {
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) { // 2008.05.26
								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , TRUE , k ) ) {
									if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
										FM_Mode = 2;
										FM_Buffer = k;
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , FALSE , k ) ) {
										if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
											if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
												FM_Mode = 2;
												FM_Buffer = k;
											}
										}
									}
								}
							}
						}
*/
					}
				}
			}
			//==========================================================
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				if ( FM_Mode != 0 ) {
					SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , FM_Buffer );
					//=================================================================
					// 2008.12.12
					//=================================================================
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
						//
						if ( k == FM_Buffer ) continue;
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2008.12.12
							_SCH_PREPOST_Pre_End_Part( CHECKING_SIDE , k );
						}
					}
					//=================================================================
				}
			}
			//==========================================================
		}
		else {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2006.03.21
				if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2008.11.13
//					if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
					if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PLACE , 0 ) ) { // 2008.05.26
							FM_Mode = 1;
							FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(CHECKING_SIDE) , CHECKING_SIDE ) ) { // 2007.11.06
							if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PICK , 0 ) ) { // 2008.05.26
									FM_Mode = 2;
									FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(CHECKING_SIDE);
								}
							}
						}
					}
				}
			}
			else {
				//===================================================================================================================================
				// 2007.06.11
				//===================================================================================================================================
				if ( _SCH_MODULE_Get_BM_FULL_MODE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) ) != BUFFER_TM_STATION ) { // 2007.06.11
				//===================================================================================================================================
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() < BUFFER_SWITCH_BATCH_PART ) { // 2008.04.24
						if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2008.11.13
//							if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2008.04.24 // 2009.10.15
							if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PLACE , 0 ) ) { // 2008.05.26
									//======================================================================================================
									FM_Mode = 1;
									FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
									//======================================================================================================
								}
							}
							else { // 2008.04.24
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , CHECKING_SIDE ) ) { // 2007.11.06
									if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
	//									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2008.07.12 // 2008.08.29
										if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER( 2 ) <= 0 ) ) { // 2008.07.12 // 2008.08.29
											if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PICK , 0 ) ) { // 2008.05.26
												FM_Mode = 2;
												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
											}
										}
									}
								}
							}
						}
						else { // 2008.12.29
							if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) % 2 ) == 0 ) {
//								if ( !_SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
								if ( !SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , CHECKING_SIDE ) ) {
										if ( !scheduler_malic_locking( CHECKING_SIDE ) ) {
											if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PICK , 1 ) ) {
												FM_Mode = 2;
												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
											}
										}
									}
								}
							}
						}

					}
					else { // 2008.04.24
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) , G_PLACE , 0 ) ) { // 2008.05.26
							//======================================================================================================
							FM_Mode = 1;
							FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
							//======================================================================================================
						}
					}
				}
				//---------------------------------------------------------------------------
				// 2008.07.12
				//---------------------------------------------------------------------------
				if ( FM_Mode == 0 ) {
					if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 0 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() < BUFFER_SWITCH_BATCH_PART ) {
							if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 2 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) {
								if ( _SCH_MODULE_Get_BM_FULL_MODE( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() ) != BUFFER_TM_STATION ) {
									if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_SIDE_WAFER( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() , CHECKING_SIDE ) ) {
											if ( !scheduler_malic_locking( CHECKING_SIDE ) ) {
//												if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2008.08.29
												if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER( 2 ) <= 0 ) ) { // 2008.07.12 // 2008.08.29
													if ( _SCH_MODULE_GROUP_FM_POSSIBLE( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() , G_PICK , 0 ) ) {
														FM_Mode = 2;
														FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch();
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
				//---------------------------------------------------------------------------
			}
		}
	}
	else {
		//=====================================================================
		lxb = _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) / 2;
		if ( lxb == 0 ) lx = 2;
		else            lx = 1;
		for ( k = 0 ; k < MAX_BM_CHAMBER_DEPTH ; k++ ) bck[k] = FALSE; // 2007.04.27 // 2007.08.10
		//=====================================================================
		// 2006.12.22
		//=====================================================================
//			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2007.11.30
//		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2007.11.30 // 2018.05.23
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_SINGLESWAP ) { // 2007.11.30 // 2018.05.23
			mxcmb = 0;
			mxcnt  = 0;
			mxside = -1;
			mxpt   = -1;
//				for ( k = 0 ; k < MAX_BM_CHAMBER_DEPTH ; k++ ) bck[k] = FALSE; // 2007.04.27 // 2007.08.10
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				//
				if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
				//
				if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
				//
//				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2009.09.28
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2009.09.28
//					if ( SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( k , 0 , &mxrside , &mxrpt , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2018.05.23
					if ( SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( k , 0 , &mxrside , &mxrpt , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2018.05.23
						if ( mxside == -1 ) {
							mxside = mxrside;
							mxpt   = mxrpt;
							bck[k-BM1] = TRUE;
							mxcmb = k-BM1;
						}
						else {
							if ( mxside == mxrside ) {
								if ( mxpt > mxrpt ) {
									mxside = mxrside;
									mxpt   = mxrpt;
									bck[k-BM1] = TRUE;
									bck[mxcmb] = FALSE;
									mxcmb = k-BM1;
								}
							}
							else {
								if ( mxside != CHECKING_SIDE ) {
									mxside = mxrside;
									mxpt   = mxrpt;
									bck[k-BM1] = TRUE;
									bck[mxcmb] = FALSE;
									mxcmb = k-BM1;
								}
							}
						}
						mxcnt++;
					}
					else {
						bck[k-BM1] = FALSE;
					}
				}
				else {
					bck[k-BM1] = FALSE;
				}
			}
//				if ( mxcnt > 1 ) lx = 0; // 2008.04.03
			if ( mxcnt > 0 ) lx = 0; // 2008.04.03
			//============================================================================================================================================
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01a SELECT lx=%d,mxcnt=%d,mxside=%d,bck=(%d,%d,%d,%d)\n" , lx , mxcnt , mxside , bck[0] , bck[1] , bck[2] , bck[3] );
			//============================================================================================================================================
			//====================================================================================================
			// 2007.04.11
			//====================================================================================================
			if ( lx != 0 ) {
				mxcmb = 0;
				mxcnt  = 0;
				mxside = -1;
				mxpt   = -1;
				for ( k = 0 ; k < MAX_BM_CHAMBER_DEPTH ; k++ ) bck[k] = FALSE; // 2007.04.27
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					//
					if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2009.09.28
					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2009.09.28
						//
//						if ( SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( k , 1 , &mxrside , &mxrpt , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2018.05.23
						if ( SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( k , 1 , &mxrside , &mxrpt , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2018.05.23
							if ( mxside == -1 ) {
								mxside = mxrside;
								mxpt   = mxrpt;
								bck[k-BM1] = TRUE;
								mxcmb = k-BM1;
							}
							else {
								if ( mxside == mxrside ) {
									if ( mxpt > mxrpt ) {
										mxside = mxrside;
										mxpt   = mxrpt;
										bck[k-BM1] = TRUE;
										bck[mxcmb] = FALSE;
										mxcmb = k-BM1;
									}
								}
								else {
									if ( mxside != CHECKING_SIDE ) {
										mxside = mxrside;
										mxpt   = mxrpt;
										bck[k-BM1] = TRUE;
										bck[mxcmb] = FALSE;
										mxcmb = k-BM1;
									}
								}
							}
							mxcnt++;
						}
						else {
							bck[k-BM1] = FALSE;
						}
					}
					else {
						bck[k-BM1] = FALSE;
					}
				}
//					if ( mxcnt > 1 ) lx = 0; // 2008.04.03
				if ( mxcnt > 0 ) lx = 0; // 2008.04.03
			}
			//====================================================================================================
		}
		//============================================================================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 SELECT lx=%d,mxcnt=%d,mxside=%d,bck=(%d,%d,%d,%d)\n" , lx , mxcnt , mxside , bck[0] , bck[1] , bck[2] , bck[3] );
		//============================================================================================================================================
		//
		if ( Get_RunPrm_FM_PM_CONTROL_USE() ) { // 2014.11.06
			//
			ret_pt = SCHEDULER_FM_PM_GET_PICK_PM( 0 , CHECKING_SIDE , -1 , &FM_Pm , &FM_TSlot , &FM_Slot , &FM_dbSide , &FM_Pointer , &FM_TSlot2 , &FM_Slot2 , &FM_dbSide2 , &FM_Pointer2 );
			//
			ret_rcm = FM_Pm;
			//
			MCR_PICK_FROM_FM = ( ret_pt > 0 );
			//
			MCR_PICK_NOTFINISH_STS = MCR_PICK_FROM_FM;
			//
			if ( !MCR_PICK_FROM_FM ) { // 2014.11.25
				//
				if ( !SCHEDULER_FM_HAS_PICK_PM() ) { // 2014.11.25 /* 추월허용시 Comment */
					//
					MCR_PICK_FROM_FM = SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ); // 2010.08.10
					//
					MCR_PICK_NOTFINISH_STS = !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE );
					//
					if ( MCR_PICK_FROM_FM ) {
						//
						if ( SCHEDULER_FM_PM_DIRECT_MODE( CHECKING_SIDE , ret_pt , &NoMore ) ) {
							MULTI_PM_DIRECT_MODE = TRUE;
						}
						else {
							MCR_PICK_FROM_FM = FALSE;
						}
						//
						if ( NoMore ) { // 2015.06.22
							MCR_PICK_FROM_FM = FALSE;
						}
					}
					//
				}
				//
			}
			//
			if ( MCR_PICK_FROM_FM ) MULTI_PM_PICK_FROM_PM = TRUE; // 2015.11.21
			//
		}
		else {
			//
			MCR_PICK_FROM_FM = SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ); // 2010.08.10
			//
			MCR_PICK_NOTFINISH_STS = !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE );
			//
		}
		//
		for ( ; lx < 3 ; lx++ ) { // 2003.11.28
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				//
				if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
				//
//				if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11 // 2013.05.03
				//
				//==========================================================================================
				// 2006.12.22
				//==========================================================================================
				if      ( lx == 0 ) {
					if ( !bck[k-BM1] ) continue;
				}
				//==========================================================================================
				//==========================================================================================
				else if ( lx == 1 ) { // 2003.11.28
					if ( lxb == 0 ) { // 2003.11.28
						if ( k != CHECKING_SIDE + BM1 ) continue; // 2003.11.28
					} // 2003.11.28
					else if ( lxb == 1 ) { // 2003.11.28
						if ( k != 0 + BM1 ) continue; // 2003.11.28
					} // 2003.11.28
					else if ( lxb == 2 ) { // 2003.11.28
						if ( k != 1 + BM1 ) continue; // 2003.11.28
					} // 2003.11.28
				} // 2003.11.28
				//==========================================================================================
//					if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2006.12.21
//				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21 // 2009.09.28
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2006.12.21 // 2009.09.28
					//
					pgout = Scheduler_FEM_GetOut_Properly_Check_for_Switch( CHECKING_SIDE , k ); // 2003.11.28
					pgin = Scheduler_FEM_GetIn_Properly_Check_for_Switch( CHECKING_SIDE , k ); // 2003.11.28
					//
					if ( pgin && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) ) { // 2013.10.01
						if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
							if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
										pgin = FALSE;
									}
								}
							}
						}
					}
					//
					if ( pgin ) { // 2014.03.06
						if ( Dual_AL_Dual_FM_Second_Check > 1 ) {
							pgin = FALSE;
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_OUT_BMOUTFIRST() ) { // 2015.02.11
								pgin = FALSE;
							}
						}
					}
					//
					if ( pgin ) { // 2014.11.07
						if ( !MULTI_PM_DIRECT_MODE ) {// 2014.11.25
							if ( Get_RunPrm_FM_PM_CONTROL_USE() && !MCR_PICK_FROM_FM ) {
								pgin = FALSE;
							}
						}
					}
					//
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z SELECT = (lx=%d) (k=%d) pgin=%d , pgout=%d [%d,%d,%d]\n" , lx , k , pgin , pgout , MCR_PICK_FROM_FM , MCR_PICK_NOTFINISH_STS , MULTI_PM_DIRECT_MODE );
					//
//					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_SINGLESWAP ) { // 2018.05.17
						//
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z1 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)\n" , k , FM_Mode , FM_Buffer );
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
//							if ( pgout && SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( CHECKING_SIDE , TRUE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2003.11.28 // 2004.03.22 // 2018.05.17
							if ( pgout && SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( CHECKING_SIDE , TRUE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2003.11.28 // 2004.03.22 // 2018.05.17
								if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
									//==========================================================================
									// 2007.05.21
									//==========================================================================
									if ( FM_Mode == 2 ) {
										if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
												if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) { // 2008.12.12
													FM_Mode = 2;
													FM_Buffer = k;
												}
											}
										}
										else { // 2009.09.02
											if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) {
												if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) {
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
															if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																FM_Mode = 2;
																FM_Buffer = k;
															}
														}
													}
												}
											}
										}
									}
									//==========================================================================
//										else { // 2007.11.30
									else if ( FM_Mode != 1 ) { // 2007.11.30
										FM_Mode = 2;
										FM_Buffer = k;
									}
								}
							}
							else {
//								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT( CHECKING_SIDE , 1 , FALSE , k ) ) { // 2003.11.28
//								if ( pgout && SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( CHECKING_SIDE , FALSE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2003.11.28 // 2004.03.22 // 2018.05.17
								if ( pgout && SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( CHECKING_SIDE , FALSE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2003.11.28 // 2004.03.22 // 2018.05.17
									if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
										if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
											//==========================================================================
											// 2007.05.21
											//==========================================================================
											if ( FM_Mode == 2 ) {
												if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) {
													if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) { // 2008.12.12
															FM_Mode = 2;
															FM_Buffer = k;
														}
													}
												}
												else { // 2009.09.02
													if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) {
														if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) {
															if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
																if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																	if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																		FM_Mode = 2;
																		FM_Buffer = k;
																	}
																}
															}
														}
													}
												}
											}
											//==========================================================================
//												else { // 2007.11.30
											else if ( FM_Mode != 1 ) { // 2007.11.30
												FM_Mode = 2;
												FM_Buffer = k;
											}
										}
									}
									else { // 2004.03.22
										if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2004.03.22
											if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
												//==========================================================================
												// 2007.05.21
												//==========================================================================
												if ( FM_Mode == 2 ) {
													if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) {
														if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
															if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) { // 2008.12.12
																FM_Mode = 2;
																FM_Buffer = k;
															}
														}
													}
													else { // 2009.09.02
														if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) {
															if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) {
																if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
																	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																		if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																			FM_Mode = 2;
																			FM_Buffer = k;
																		}
																	}
																}
															}
														}
													}
												}
												//==========================================================================
//													else { // 2007.11.30
												else if ( FM_Mode != 1 ) { // 2007.11.30
													FM_Mode = 2;
													FM_Buffer = k;
												}
											}
										}
									}
								}
							}
						}
						//
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z2 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)\n" , k , FM_Mode , FM_Buffer );
						//
//							if ( ( FM_Mode == 0 ) || ( FM_Mode == 2 ) ) { // 2007.03.20
//							if ( ( FM_Mode == 0 ) || ( FM_Mode == 2 ) || ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( FM_Mode == 1 ) ) ) { // 2007.03.20 // 2007.09.11
						if ( ( FM_Mode == 0 ) || ( FM_Mode == 2 ) || ( FM_Mode == 11 ) || ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( FM_Mode == 1 ) ) ) { // 2007.03.20 // 2007.09.11
//								if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09  // 2007.03.27
							//
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z3 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)\n" , k , FM_Mode , FM_Buffer );
							//
							if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
								//
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z4 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)\n" , k , FM_Mode , FM_Buffer );
								//
								bmd = _SCH_MODULE_Get_BM_FULL_MODE( k ); // 2006.05.26
								//=======================================================================================================================
								// 2007.09.11
								//=======================================================================================================================
								if ( SCHEDULER_CONTROL_Chk_SWAP_PRE_PICK_POSSIBLE_in_MIDDLESWAP( bmd ) ) bmd = -1;
								//=======================================================================================================================
								//
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z5 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
								//
								if ( ( bmd == -1 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) || ( ( FM_Mode != 0 ) || ( lx != 1 ) ) ) { // 2006.02.22 // 2006.12.19 // 2007.08.10 // 2007.09.11
									//
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z6 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
									//
									if ( pgin ) { // 2003.11.28
										//
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z7 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
										//
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
											//
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z8 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
											//
											//=======================================================================================
											// 2006.12.22
											//=======================================================================================
//												if ( ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) ) { // 2007.09.11
											if ( ( bmd == -1 ) || ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) ) { // 2007.09.11
												if ( !SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) {
													//
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z81 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
													//
													if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( CHECKING_SIDE , k ) ) { // 2008.01.04
														if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2007.05.21
//															if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.05.17
															if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
																if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( k , bmd ) ) {
																	if ( bmd == -1 ) { // 2007.09.11
																		if ( FM_Mode == 0 ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																		//=====================================================================
																		else if ( FM_Mode == 11 ) { // 2007.12.28
																			if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																		}
																		//=====================================================================
																	}
																	else {
																		if ( FM_Mode == 1 ) { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																		else {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																	}
																}
																else {
																	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-A SELECT = Res(%d)\n" , k );
																}
															}
															else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.03.20
																if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( k , FM_Buffer , FALSE ) ) {
																	if ( bmd == -1 ) { // 2007.09.11
																		if ( FM_Mode == 0 ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																		//=====================================================================
																		else if ( FM_Mode == 11 ) { // 2007.12.28
																			if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																		}
																		//=====================================================================
																	}
																	else {
																		if ( FM_Mode == 1 ) { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																		else {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																	}
																}
																else {
																	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-A SELECT = Res(%d)\n" , k );
																}
															}
															else {
																if ( bmd == -1 ) { // 2007.09.11
																	if ( FM_Mode == 0 ) {
																		FM_Mode = 11;
																		FM_Buffer = k;
																	}
																	//=====================================================================
																	else if ( FM_Mode == 11 ) { // 2007.12.28
																		if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																	}
																	//=====================================================================
																}
																else {
																	if ( FM_Mode == 1 ) { // 2009.09.02
																		if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																			FM_Buffer = k;
																		}
//																		else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																		else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			FM_Buffer = k;
																		}
																	}
																	else {
																		FM_Mode = 1;
																		FM_Buffer = k;
																	}
																}
															}
														}
														else { // 2008.12.24
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-B SELECT = Res(%d)\n" , k );
															//===============================================================================
															for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																if ( dmi == CHECKING_SIDE ) continue;
																if ( !_SCH_SYSTEM_USING_RUNNING( dmi ) ) continue;
																//
																if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																//
																if ( SCHEDULER_FM_Current_No_More_Supply( dmi ) ) continue;
																if ( !SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) continue;
																if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) continue;
																break;
															}
															//===============================================================================
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-B1 SELECT = Res(%d) %d\n" , k , dmi );
															//===============================================================================
															if ( dmi == MAX_CASSETTE_SIDE ) {
																if ( bmd == BUFFER_FM_STATION ) {
																	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																		//=====================================================================
																		//
																		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k ); // 2017.02.08
																		//
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1231 );
																		//=====================================================================
																	}
																}
															}
															//===============================================================================
															_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 + 1000  ); // 2008.12.09
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-B2 SELECT = Res(%d)\n" , k );
															//===============================================================================
														}
													}
													else {
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-C SELECT = Res(%d)\n" , k );
													}
												}
												else {
													//
													_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 2000 );
													//
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z82 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
													//
													if ( !SCHEDULER_FM_Other_Supply_Impossible( CHECKING_SIDE ) ) { // 2016.01.05
														//
//														if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.05.17
														if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
															if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( k , bmd ) ) {
																//===============================================================================
																for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																	if ( dmi != CHECKING_SIDE ) {
																		//
																		if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																		//
																		if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																			if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																					//
																					FM_Mode = 101;
																					//
																					break;
																				}
																			}
																		}
																	}
																}
																//===============================================================================
															}
														}
														else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.03.20
															if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( k , FM_Buffer , TRUE ) ) {
																//===============================================================================
																// 2007.05.21
																//===============================================================================
	//																FM_Mode = 10?;
																//===============================================================================
																for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																	if ( dmi != CHECKING_SIDE ) {
																		//
																		if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																		//
																		if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																			if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																					FM_Mode = 102;
																					//
																					break;
																				}
																			}
																		}
																	}
																}
																//===============================================================================
															}
														}
														else {
															//===============================================================================
															for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																if ( dmi != CHECKING_SIDE ) {
																	//
																	if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																	//
																	if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																		if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																			if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																				//
																				FM_Mode = 103;
																				//
																				break;
																			}
																		}
																	}
																}
															}
															//===============================================================================
														}
														//
													}
												}
											}
											//
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z9 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
											//
											//=======================================================================================
										}
										else { // 2004.03.22
											//
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z72 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
											//
//												if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , FALSE ) ) { // 2007.09.11
//												if ( ( bmd == -1 ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , FALSE ) ) { // 2007.09.11 // 2007.11.23
//											if ( ( bmd == -1 ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) { // 2007.09.11 // 2007.11.23 // 2013.10.01
//											if ( ( bmd == -1 ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2007.09.11 // 2007.11.23 // 2013.10.01 // 2018.05.17
											if ( ( bmd == -1 ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP , TRUE ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2007.09.11 // 2007.11.23 // 2013.10.01 // 2018.05.17
												//
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z73 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
												//
												//=======================================================================================
												// 2006.12.22
												//=======================================================================================
//													if ( ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) ) { // 2007.09.11
//												if ( ( bmd == -1 ) || ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) ) { // 2007.09.11 // 2013.10.01
//												if ( ( bmd == -1 ) || ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2007.09.11 // 2013.10.01 // 2018.05.17
												if ( ( bmd == -1 ) || ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( CHECKING_SIDE , k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) ) { // 2007.09.11 // 2013.10.01 // 2018.05.17
													//
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z74 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
													//
													if ( !SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) {
														//
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z75 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
														//
														if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( CHECKING_SIDE , k ) ) { // 2008.01.04
															//
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z76 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
															//
															if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2007.05.21
																//
																_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z77 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
																//
//																if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.05.17
																if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
																	if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( k , bmd ) ) {
																		if ( bmd == -1 ) { // 2007.09.11
																			if ( FM_Mode == 0 ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																			//=====================================================================
																			else if ( FM_Mode == 11 ) { // 2007.12.28
																				if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																					FM_Mode = 11;
																					FM_Buffer = k;
																				}
																			}
																			//=====================================================================
																		}
																		else {
																			if ( FM_Mode == 1 ) { // 2009.09.02
																				if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																					FM_Buffer = k;
																				}
//																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																					FM_Buffer = k;
																				}
																			}
																			else {
																				FM_Mode = 1;
																				FM_Buffer = k;
																			}
																		}
																	}
																}
																else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.03.20
																	if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( k , FM_Buffer , FALSE ) ) {
																		if ( bmd == -1 ) { // 2007.09.11
																			if ( FM_Mode == 0 ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																			//=====================================================================
																			else if ( FM_Mode == 11 ) { // 2007.12.28
																				if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																					FM_Mode = 11;
																					FM_Buffer = k;
																				}
																			}
																			//=====================================================================
																		}
																		else {
																			if ( FM_Mode == 1 ) { // 2009.09.02
																				if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																					FM_Buffer = k;
																				}
//																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28  // 2012.09.21
																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28  // 2012.09.21
																					FM_Buffer = k;
																				}
																			}
																			else {
																				FM_Mode = 1;
																				FM_Buffer = k;
																			}
																		}
																	}
																}
																else {
																	if ( bmd == -1 ) { // 2007.09.11
																		if ( FM_Mode == 0 ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																		//=====================================================================
																		else if ( FM_Mode == 11 ) { // 2007.12.28
																			if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																		}
																		//=====================================================================
																	}
																	else {
																		if ( FM_Mode == 1 ) { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																		else {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																	}
																}
															}
															else {
																_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 3000 ); // 2008.12.24
																_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z77A SELECT = Res(%d)\n" , k );
															}
														}
													}
													else {
														//
														_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 4000 ); // 2010.12.08
														//
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z78 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
														//
														if ( !SCHEDULER_FM_Other_Supply_Impossible( CHECKING_SIDE ) ) { // 2016.01.05
														//
//															if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.05.17
															if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
																if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( k , bmd ) ) {
																	//===============================================================================
																	for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																		if ( dmi != CHECKING_SIDE ) {
																			//
																			if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																			//
																			if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																				if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																					if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																						FM_Mode = 104;
																						//
																						break;
																					}
																				}
																			}
																		}
																	}
																	//===============================================================================
																}
															}
															else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.03.20
																if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( k , FM_Buffer , TRUE ) ) {
																	//===============================================================================
																	// 2007.05.21
																	//===============================================================================
	//																FM_Mode = 10?;
																	//===============================================================================
																	for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																		if ( dmi != CHECKING_SIDE ) {
																			//
																			if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																			//
																			if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																				if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																					if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																						FM_Mode = 105;
																						//
																						break;
																					}
																				}
																			}
																		}
																	}
																	//===============================================================================
																}
															}
															else {
																//===============================================================================
																for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																	if ( dmi != CHECKING_SIDE ) {
																		//
																		if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																		//
																		if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																			if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																					//
																					FM_Mode = 106;
																					//
																					break;
																				}
																			}
																		}
																	}
																}
																//===============================================================================
															}
														}
													}
												}
												//
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-Z79 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
												//
												//=======================================================================================
/*
// 2006.12.22
												if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) {
													//=================================================================================
													if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02
//															if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2006.05.26
														if ( bmd == BUFFER_FM_STATION ) { // 2006.05.26
															FM_Mode = 1;
															FM_Buffer = k;
														}
//															else if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) { // 2006.05.26
														else if ( bmd == BUFFER_WAITx_STATION ) { // 2006.05.26
															FM_Mode = 1;
															FM_Buffer = k;
														}
													}
													//=================================================================================
												}
*/
											}
										}
									}
								}
							}
							else {
								//
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-ZA SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
								//
								//====================================================================================================
								// 2008.01.04
								//====================================================================================================
								bmd = _SCH_MODULE_Get_BM_FULL_MODE( k ); // 2006.05.26
								//=======================================================================================================================
								if ( SCHEDULER_CONTROL_Chk_SWAP_PRE_PICK_POSSIBLE_in_MIDDLESWAP( bmd ) ) bmd = -1;
								//=======================================================================================================================
								if ( ( bmd == -1 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) || ( ( FM_Mode != 0 ) || ( lx != 1 ) ) ) {
									//
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-ZB SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
									//
//									if ( pgin ) { // 2016.05.21
										//
										// 2013.01.23
										dmi = 0;
										//
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) { // 2018.11.23
											//
	//										if ( !pgin && ( FM_Mode == 2 ) && ( bmd == BUFFER_FM_STATION ) ) { // 2016.05.21 // 2017.02.16
											if ( ( !pgin || !MCR_PICK_NOTFINISH_STS ) && ( FM_Mode == 2 ) && ( bmd == BUFFER_FM_STATION ) ) { // 2016.05.21 // 2017.02.16
												dmi = 1;
											}
											else {
												if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
													dmi = 1;
												}
												else {
													//
													if ( ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
														//
														if ( SCHEDULER_CONTROL_Chk_BM_IN_HAS_SPACE_FULL_SWITCH( k , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) {
															dmi = 1;
														}
														//
													}
													else { // 2014.06.20
		//
														if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) {
															if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) {
																dmi = 1;
															}
														}

													}
													//
												}
											}
											//
										}
										//
//										if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) { // 2014.01.23
										if ( dmi == 1 ) { // 2014.01.23
											if ( ( bmd == -1 ) || ( bmd == BUFFER_WAIT_STATION ) || ( bmd == BUFFER_FM_STATION ) ) { // 2007.09.11
												//
												if ( !SCHEDULER_FM_Other_Supply_Impossible( CHECKING_SIDE ) ) { // 2016.01.05
													//
//													if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2018.05.17
													if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) { // 2018.05.17
														if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( k , bmd ) ) {
															//===============================================================================
															if ( SCHEDULER_FM_CPM_PICK_POSSIBLE( k ) ) { // 2016.07.28
																for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																	if ( dmi != CHECKING_SIDE ) {
																		//
																		if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																		//
																		if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																			if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																					FM_Mode = 107;
																					break;
																				}
																			}
																		}
																	}
																}
															}
															//===============================================================================
														}
													}
													else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.03.20
														if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( k , FM_Buffer , TRUE ) ) {
															//===============================================================================
															if ( SCHEDULER_FM_CPM_PICK_POSSIBLE( k ) ) { // 2016.07.28
																for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																	if ( dmi != CHECKING_SIDE ) {
																		//
																		if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																		//
																		if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																			if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																				if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																					FM_Mode = 108;
																					break;
																				}
																			}
																		}
																	}
																}
															}
															//===============================================================================
														}
													}
													else {
														//===============================================================================
														if ( SCHEDULER_FM_CPM_PICK_POSSIBLE( k ) ) { // 2016.07.28
															for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
																if ( dmi != CHECKING_SIDE ) {
																	//
																	if ( _SCH_MODULE_Get_Mdl_Use_Flag( dmi , k ) == MUF_00_NOTUSE ) continue; // 2014.05.08
																	//
																	if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
																		if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
																			if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
																				FM_Mode = 109;
																				break;
																			}
																		}
																	}
																}
															}
														}
														//===============================================================================
													}
												}
											}
										}
//									} // 2016.05.21
								}
							}
							//=======================================================================================
						}
						//
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-ZC SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
						//
						if ( FM_Mode >= 100 ) { // 2006.12.22
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-1 SELECT = FM_Mode(%d,%d)\n" , FM_Mode , FM_Buffer );
							//----------------------------------------------
							FM_Other_Pick = TRUE; // 2007.09.28
							//----------------------------------------------
							FM_Mode = 0;
						}
					}
					//
					//
					//
//					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2018.05.17
//					if ( ( FM_Mode == 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) { // 2018.05.17 // 2018.05.30
					if ( ( FM_Mode == 0 ) && ( !FM_Other_Pick ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) { // 2018.05.17 // 2018.05.30
						//
						//=======================================================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X1 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
						//=======================================================================================================================
						// 2007.10.04
						//=======================================================================================================================
						bmd = _SCH_MODULE_Get_BM_FULL_MODE( k );
						if ( SCHEDULER_CONTROL_Chk_SWAP_PRE_PICK_POSSIBLE_in_MIDDLESWAP( bmd ) ) bmd = -1;
						//=======================================================================================================================
						if ( ( bmd == -1 ) || ( bmd == BUFFER_FM_STATION ) ) {
							//=================================================================================================================
							// 2004.11.29
							//=================================================================================================================
//							Result = 0; // 2014.01.10
//							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) Result++; // 2014.01.10
//							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) Result++; // 2014.01.10
							//
//							if ( Result == 1 ) { // 2014.01.10
								if ( bmd == BUFFER_FM_STATION ) { // 2009.03.11
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
										Result = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP1( CHECKING_SIDE , k );
										if ( Result == 1 ) { // here
											if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
												FM_Mode = 2;
												FM_Buffer = k;
												break;
											}
										}
										else if ( Result == 2 ) { // other
											FM_Mode = 0;
											FM_Buffer = k;
											break;
										}
									}
								}
//							} // 2014.01.10
							//=======================================================================================================================
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X2 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
							//=======================================================================================================================
							if ( pgin ) { // 2004.03.09
//								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_RUN_SEND_FM_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) ) { // 2004.11.29
								//
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2010.06.15
									//
									if ( Scheduler_FEM_GetIn_Single_BM_Slot_Properly_Check_for_Switch( CHECKING_SIDE , k ) ) { // 2008.08.22
										//=======================================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X2-0 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)(Result=%d)\n" , k , FM_Mode , FM_Buffer , bmd , Result );
										//=======================================================================================================================
										if ( ( bmd == -1 ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_SINGLSWAP( CHECKING_SIDE , k , &Result ) ) {
											//
											if ( bmd == -1 ) SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_SINGLSWAP( CHECKING_SIDE , k , &Result ); // 2008.07.15
											//
											//=======================================================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X2-1 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)(Result=%d)\n" , k , FM_Mode , FM_Buffer , bmd , Result );
											//=======================================================================================================================
											if ( Result == -2 ) {
		//											if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
												if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//												if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//													if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
													if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
											//=======================================================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X2-2 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(ret_rcm=%d)(ret_pt=%d)\n" , k , FM_Mode , FM_Buffer , ret_rcm , ret_pt );
											//=======================================================================================================================
//														if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
														if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
															if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2004.11.09
																if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																	if ( FM_Mode == 1 ) {
																		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) > 0 ) {
																			FM_Buffer = k;
																		}
																		else { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																	}
																	else {
																		if ( bmd == -1 ) { // 2007.10.04
																			if ( FM_Mode == 0 ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																			//=====================================================================
																			else if ( FM_Mode == 11 ) { // 2007.12.28
																				if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																					FM_Mode = 11;
																					FM_Buffer = k;
																				}
																			}
																			//=====================================================================
																		}
																		else {
																			if ( FM_Mode == 1 ) { // 2009.09.02
																				if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																					FM_Buffer = k;
																				}
//																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																					FM_Buffer = k;
																				}
																			}
																			else {
																				FM_Mode = 1;
																				FM_Buffer = k;
																			}
																		}
																	}
																}
																else {
																	if ( bmd == -1 ) { // 2007.10.04
																		if ( FM_Mode == 0 ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																		//=====================================================================
																		else if ( FM_Mode == 11 ) { // 2007.12.28
																			if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																		}
																		//=====================================================================
																	}
																	else {
																		if ( FM_Mode == 1 ) { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																		else {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																	}
																}
															}
															else { // 2004.12.06
																_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 5000 );
															}
														}
														else {
															if ( ( bmd != -1 ) || ( _SCH_PRM_GET_MODULE_SIZE( k ) != 1 ) ) { // 2009.09.11
																if ( Scheduler_FEM_Other_GetOut_Possible_SingleSwap( CHECKING_SIDE , k , &cside ) ) {
																	if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( cside , -1 , TRUE ) ) { // 2012.06.20
																		FM_Mode = 111;
																	}
																}
															}
														}
														//=================================================================================
													}
													else {
														if ( ( bmd != -1 ) || ( _SCH_PRM_GET_MODULE_SIZE( k ) != 1 ) ) { // 2009.09.11
															if ( Scheduler_FEM_Other_GetOut_Possible_SingleSwap( CHECKING_SIDE , k , &cside ) ) {
																if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( cside , -1 , TRUE ) ) { // 2004.11.08
																	FM_Mode = 112;
																}
															}
														}
													}
												}
												else {
													if ( ( bmd != -1 ) || ( _SCH_PRM_GET_MODULE_SIZE( k ) != 1 ) ) { // 2009.09.11
														if ( Scheduler_FEM_Other_GetOut_Possible_SingleSwap( CHECKING_SIDE , k , &cside ) ) {
															if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( cside , -1 , TRUE ) ) { // 2004.11.08
																FM_Mode = 113;
															}
														}
													}
												}
											}
											else if ( Result == -1 ) {
		//											if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
												if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//												if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//													if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
													if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
														//=================================================================================
//														if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
														if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
															if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2004.11.09
																if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																	if ( FM_Mode == 1 ) {
																		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) > 0 ) {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																		else { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																	}
																	else {
																		if ( bmd == -1 ) { // 2007.10.04
																			if ( FM_Mode == 0 ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																			//=====================================================================
																			else if ( FM_Mode == 11 ) { // 2007.12.28
																				if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																					FM_Mode = 11;
																					FM_Buffer = k;
																				}
																			}
																			//=====================================================================
																		}
																		else {
																			if ( FM_Mode == 1 ) { // 2009.09.02
																				if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																					FM_Buffer = k;
																				}
//																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																					FM_Buffer = k;
																				}
																			}
																			else {
																				FM_Mode = 1;
																				FM_Buffer = k;
																			}
																		}
																	}
																}
																else {
																	if ( bmd == -1 ) { // 2007.10.04
																		if ( FM_Mode == 0 ) {
																			FM_Mode = 11;
																			FM_Buffer = k;
																		}
																		//=====================================================================
																		else if ( FM_Mode == 11 ) { // 2007.12.28
																			if ( SCHEDULING_CHECK_FM_Current_BM_is_Old( k , FM_Buffer ) ) {
																				FM_Mode = 11;
																				FM_Buffer = k;
																			}
																		}
																		//=====================================================================
																	}
																	else {
																		if ( FM_Mode == 1 ) { // 2009.09.02
																			if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																				FM_Buffer = k;
																			}
//																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																			else if ( SCHEDULER_CONTROL_Chk_BM_LOW_BLANKINTIME( 1 , k , FM_Buffer ) ) { // 2009.09.28 // 2012.09.21
																				FM_Buffer = k;
																			}
																		}
																		else {
																			FM_Mode = 1;
																			FM_Buffer = k;
																		}
																	}
																}
															}
															else { // 2004.12.06
																_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 6000 );
															}
														}
														//=================================================================================
													}
												}
											}
											else {
		//											if ( _SCH_SYSTEM_USING_GET( Result ) >= 9 ) { // 2005.07.29
												if ( _SCH_SYSTEM_USING_RUNNING( Result ) ) { // 2005.07.29
		//												if ( _SCH_SYSTEM_MODE_END_GET( Result ) == 0 ) { // 2003.11.09 // 2007.03.27
													if ( !SCHEDULER_FM_FM_Finish_Status( Result ) ) { // 2007.03.27
	//													if ( _SCH_SUB_Remain_for_FM( Result ) ) { // 2009.10.15
//														if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( Result , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2018.12.20
														if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( Result , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1010 ) ) { // 2009.10.15 // 2018.12.20
															if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( Result , TRUE , 1 ) ) { // 2004.05.25
																if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( Result , -1 , TRUE ) ) { // 2004.11.08
																	FM_Mode = 114;
																}
															}
															else { // 2004.05.25
		//															if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
																if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//																if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//																	if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
																	if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
																		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																			if ( FM_Mode != 1 ) {
																				FM_Mode = 3;
																				FM_Buffer = k;
																			}
																		}
																		else {
																			if ( FM_Mode != 1 ) {
																				if ( FM_Mode != 3 ) {
																					FM_Mode = 3;
																					FM_Buffer = k;
																				}
																			}
																		}
																	}
																}
															}
														}
														else {
		//														if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
															if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//															if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//																if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
																if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
																	//=================================================================================
//																	if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
																	if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
																		if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2004.11.09
																			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																				if ( FM_Mode != 1 ) {
																					FM_Mode = 3;
																					FM_Buffer = k;
																				}
																			}
																			else {
																				if ( FM_Mode != 1 ) {
																					if ( FM_Mode != 3 ) {
																						FM_Mode = 3;
																						FM_Buffer = k;
																					}
																				}
																			}
																		}
																		else { // 2004.12.06
																			_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 7000 );
																		}
																	}
																	//=================================================================================
																}
															}
														}
													}
													else {
		//													if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
														if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//														if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//															if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
															if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
																//=================================================================================
//																if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
																if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
																	if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2004.11.09
																		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																			if ( FM_Mode != 1 ) {
																				FM_Mode = 3;
																				FM_Buffer = k;
																			}
																		}
																		else {
																			if ( FM_Mode != 1 ) {
																				if ( FM_Mode != 3 ) {
																					FM_Mode = 3;
																					FM_Buffer = k;
																				}
																			}
																		}
																	}
																	else { // 2004.12.06
																		_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 8000 );
																	}
																}
																//=================================================================================
															}
														}
													}
												}
												else {
		//												if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2003.11.09 // 2007.03.27
													if ( MCR_PICK_NOTFINISH_STS ) { // 2007.03.27
	//													if ( _SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) { // 2009.10.15
//														if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2009.10.15 // 2010.08.10
														if ( MCR_PICK_FROM_FM ) { // 2009.10.15 // 2010.08.10
															//=================================================================================
//															if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
															if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.09.02 // 2015.11.21
																if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , TRUE ) ) { // 2004.11.09
																	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
																		if ( FM_Mode != 1 ) {
																			FM_Mode = 3;
																			FM_Buffer = k;
																		}
																	}
																	else {
																		if ( FM_Mode != 1 ) {
																			if ( FM_Mode != 3 ) {
																				FM_Mode = 3;
																				FM_Buffer = k;
																			}
																		}
																	}
																}
																else { // 2004.12.06
																	_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 9000 );
																}
															}
															//=================================================================================
														}
													}
												}
											}
										}
									}
								}
								//=======================================================================================================================
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X3 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
								//=======================================================================================================================
								if ( ( FM_Mode != 1 ) && ( FM_Mode != 3 ) && ( FM_Mode < 100 ) ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
										if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( CHECKING_SIDE , k , &Result ) ) {
											if ( !FM_Other_Pick ) { // 2009.09.11
												if ( FM_Mode == 2 ) {
	//												if ( Result ) { // 2009.09.10
													if ( Result || ( _SCH_PRM_GET_MODULE_SIZE( k ) == 1 ) ) { // 2009.09.10
														if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
															//
															if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2009.09.02
																if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) { // 2009.09.02
																	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
																		if ( SCHEDULER_CONTROL_Chk_BM_RETURN_LOW_SUPPLYID( k , FM_Buffer ) ) {
																			FM_Mode = 2;
																			FM_Buffer = k;
																		}
																	}
																}
																else {
																	FM_Mode = 2;
																	FM_Buffer = k;
																}
															}
														}
													}
												}
												else {
													if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2016.06.07
														if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02
															FM_Mode = 2;
															FM_Buffer = k;
														}
													}
												}
											}
										}
									}
								}
								//=======================================================================================================================
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-X4 SELECT = (K=%d)(FM_Mode=%d)(FM_Buffer=%d)(bmd=%d)\n" , k , FM_Mode , FM_Buffer , bmd );
								//=======================================================================================================================
								if ( FM_Mode >= 100 ) {
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 SELECT = FM_Mode(%d,%d)\n" , FM_Mode , FM_Buffer );
									//----------------------------------------------
									FM_Other_Pick = TRUE; // 2007.09.28
									//----------------------------------------------
									FM_Mode = 0;
								}
							}
						}
					}
				}
			}
//				if ( FM_Mode != 0 ) break; // 2003.11.28 // 2007.09.11
			if ( ( FM_Mode != 0 ) && ( FM_Mode != 11 ) ) break; // 2003.11.28 // 2007.09.11
			//====================================================================
			// 2006.12.22
			//====================================================================
			if ( lx == 0 ) break;
			//====================================================================
		}
	}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 SELECT = FM_Mode(%d,%d)\n" , FM_Mode , FM_Buffer );
	//----------------------------------------------
	//
	if ( FM_Mode != 1 ) {
		if ( !SCHEDULER_FM_PM_PICK_CM_TO_PM( CHECKING_SIDE ) ) { // 2014.11.06
			return 0;
		}
	}
	//
	if ( FM_Mode == 0 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2004.03.24
			if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) { // stop
				k = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE );
				if ( k != 0 ) {
					//======================================================================================
					if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == 0 ) { // 2006.12.19
					//======================================================================================
						if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) { // 2006.05.26
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) { // 2006.05.26
									//=====================================================================
									// 2007.05.03
									//=====================================================================
									if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , k , FALSE ) == -1 ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 301 from FEM%cWHFMFAIL 301\n" , 9 );
										return 0;
									}
									//=====================================================================
									//
									SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k ); // 2017.02.08
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 15 );
									//=====================================================================
								}
							}
						}
					}
				}
			}
		}
		else {
			//==========================================================================================
			// 2007.09.15
			//==========================================================================================
			if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 0 ) {
				for ( lx = 0 ; lx < 3 ; lx++ ) {
					k = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE );
					if ( k != 0 ) {
						//======================================================================================
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) break;
						//======================================================================================
						if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) break;
						//======================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) break;
						//======================================================================================
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) break;
						//======================================================================================
						if ( !_SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) break;
						//======================================================================================
						if ( !_SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) break;
						//======================================================================================
						if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) break;
						//======================================================================================
						if ( lx != 2 ) {
							Sleep( 1000 );
						}
						else {
							//=====================================================================
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 1502 );
							//=====================================================================
						}
						//======================================================================================
					}
				}
			}
			//==========================================================================================
		}
		Sleep(1);
	}
	//----------------------------------------------
	// 2007.09.11
	//----------------------------------------------
	if ( ( FM_Mode == 0 ) && ( !FM_Other_Pick ) ) {
		if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
			//
//			if ( !SCHEDULER_CONTROL_Chk_BM_HAS_OUT_WAFER( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) { // 2014.01.27
			//
			lx = 1; // 2015.02.19
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_WAFER( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) { // 2015.02.19
				for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
					if ( !_SCH_SYSTEM_USING_RUNNING( dmi ) ) continue;
					if ( SCHEDULER_FM_Current_No_More_Supply( dmi ) ) continue;
					if ( !SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) continue;
					if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) continue;
					//
					lx = 0;
					//
					break;
				}
			}
			//
			if ( lx ) { // 2015.02.19
				if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
					//
					if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
						ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
						if ( ALsts == SYS_ABORTED ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 901 from FEM%cWHFMFAIL 901\n" , 9 );
							return 0;
						}
						if ( ALsts == SYS_SUCCESS ) {
							if ( SCHEDULER_CONTROL_Chk_BM_ALL_ONE_SLOT_MODE( CHECKING_SIDE ) ) { // 2008.05.20
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2009.03.12
									if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2008.05.20
										FM_Mode = 12;
									}
								}
							}
							else {
	//								FM_Mode = 12; // 2013.10.01

								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2009.03.12
									//
									// 2013.10.01
									//
									if ( (_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
										FM_Mode = 12;
									}
									else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
										if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
											FM_Mode = 12;
										}
									}
									else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
										if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
											FM_Mode = 12;
										}
									}
								}
								else { // 2009.03.12
									if ( SCHEDULER_CONTROL_Chk_BM_PREPARED_For_PLACE( CHECKING_SIDE , _SCH_MODULE_Get_MFALS_BM( 1 ) ) ) {
										FM_Mode = 12;
									}
								}
							}
						}
					}
				}
				//
				if ( FM_Mode == 0 ) { // 2008.08.29
					if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
						if ( _SCH_MODULE_Get_MFALS_SIDE(2) == CHECKING_SIDE ) {
							ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
							if ( ALsts == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 901 from FEM%cWHFMFAIL 901\n" , 9 );
								return 0;
							}
							if ( ALsts == SYS_SUCCESS ) {
								if ( SCHEDULER_CONTROL_Chk_BM_ALL_ONE_SLOT_MODE( CHECKING_SIDE ) ) { // 2008.05.20
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2009.03.12
										if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2008.05.20
											FM_Mode = 12;
										}
									}
								}
								else {
	// 								FM_Mode = 12; // 2013.10.01

									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2009.03.12
										//
										// 2013.10.01
										//
										if ( (_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
											FM_Mode = 12;
										}
										else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
											if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
												FM_Mode = 12;
											}
										}
										else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
											if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
												FM_Mode = 12;
											}
										}
									}
									else { // 2009.03.12
										if ( SCHEDULER_CONTROL_Chk_BM_PREPARED_For_PLACE( CHECKING_SIDE , _SCH_MODULE_Get_MFALS_BM(2) ) ) {
											FM_Mode = 12;
										}
									}
								}
							}
						}
					}
				}
			}
			//
			if ( FM_Mode == 12 ) { // 2016.06.17 
				//
				FM_Buffer = 0; // 2016.07.14
				FM_Buffer2 = 0; // 2016.07.14
				//
				if ( Dual_AL_Dual_FM_Second_Check > 1 ) Dual_AL_Dual_FM_Second_Check = 0;
			}
			//
		}
	}
	//----------------------------------------------
	middleswap_tag = 0; // 2006.02.22
	//----------------------------------------------
	FM_Buffer_Toggle = -1; // 2007.04.11
	//----------------------------------------------
//		if ( ( FM_Mode == 1 ) || ( FM_Mode == 3 ) ) { // 2007.09.11
	if ( ( FM_Mode == 1 ) || ( FM_Mode == 3 ) || ( FM_Mode == 11 ) || ( FM_Mode == 12 ) ) { // 2007.09.11
		if ( FM_Mode != 12 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.08
				// Append 2002.01.02
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_PART ) { // 2005.06.28
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) {
						_SCH_MODULE_Set_FM_OutCount( CHECKING_SIDE , 0 );
						_SCH_MODULE_Set_FM_InCount( CHECKING_SIDE , 0 );
					}
				}
				//
				if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 2 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) { // 2007.01.05
					_SCH_MODULE_Set_BM_SIDE( FM_Buffer , 1 , CHECKING_SIDE ); // 2007.03.14
				}
				else {
					_SCH_MODULE_Set_BM_SIDE( FM_Buffer , 1 , CHECKING_SIDE );
//					SCHEDULER_CONTROL_Set_BM_Switch_UseSide( FM_Buffer , CHECKING_SIDE ); // 2008.11.24
				}
			}
			//----------------------------------------------------------------------
			if ( FM_Mode != 11 ) { // 2007.09.11
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.08
					if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) {
	//					FM_Side_Check = 0; // 2007.05.17
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 102 );
					}
					else {
	//					FM_Side_Check = 1; // 2007.05.17
					}
				}
	//			else { // 2007.05.17
	//				FM_Side_Check = 0; // 2007.05.17
	//			}
			}
		}
		//-----------------------------------------------
		_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 34 );
		//-----------------------------------------------

middlererun :	// 2006.02.22

		while( TRUE ) {
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02-1a SELECT = FM_Mode(%d) FM_Buffer=%d,%d,%d middleswap_tag=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , middleswap_tag );
			//----------------------------------------------
			if ( FM_Mode != 12 ) {
				if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.09
						//
						if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 2 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) ) { // 2008.11.13
							for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
								if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , k ) >= 0 ) {
									if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , k ) != SCHEDULER_READY ) {
										_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , k , -1 );
									}
								}
							}
							//
							if      ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 2 ) wfrreturn_mode[CHECKING_SIDE] = 2; // 2008.11.13
							else if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) wfrreturn_mode[CHECKING_SIDE] = 4; // 2008.11.13
							else if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) wfrreturn_mode[CHECKING_SIDE] = 6; // 2008.11.13
							//
						}
						else {
							for ( k = _SCH_MODULE_Get_FM_DoPointer( CHECKING_SIDE ) ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
								_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , k , -1 );
							}
						}
						//
						_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
						_SCH_MODULE_Set_FM_End_Status( CHECKING_SIDE );
//						_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 ); // 2009.01.09
						//
						//==========================================================================
						// 2007.03.27
						//==========================================================================
						if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) {
							_SCH_MODULE_Set_TM_End_Status( CHECKING_SIDE );
							FM_Mode = 0;
							break;
						}
						//==========================================================================
					}
					else {
						if ( FM_Mode != 1 ) { // 2014.11.25
							FM_Mode = 0;
							break;
						}
					}
				}
			}
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 2 from FEM%cWHFMFAIL 2\n" , 9 );
				return 0;
			}
			//----------------------------------------------------------------------
			_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ); // 2009.05.21
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02-1 SELECT = FM_Mode(%d) FM_Buffer=%d,%d,%d middleswap_tag=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , middleswap_tag );
			//----------------------------------------------
			//-- Modify 2002.01.02
			if ( FM_Mode != 12 ) {
//				if ( _SCH_MODULE_Get_FM_OutCount( CHECKING_SIDE ) >= _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) ) {
//				if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.07
					if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) {
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_TM_STATION ) { // 2007.09.11
							_SCH_SUB_Remain_for_FM( CHECKING_SIDE );
							//=====================================================================
							// 2007.05.03
							//=====================================================================
							if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 302 from FEM%cWHFMFAIL 302\n" , 9 );
								return 0;
							}
							//=====================================================================
							//
							SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
							//
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 1 );
							//=====================================================================
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//=====================================================================
							break;
						}
						else {
//								break; // 2007.10.02
							//=====================================================================
							if ( FM_Mode != 11 ) break; // 2007.10.02
							//=====================================================================
						}
					}
					else {
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
							middleswap_tag = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( FM_Buffer , 0 );
							if ( middleswap_tag != 0 ) {
//									break; // 2007.10.02
								if ( FM_Mode != 11 ) break; // 2007.10.02
							}
						}
					}
				}
				//==========================================================================================
				// 2004.05.19
				//==========================================================================================
				FM_Buffer2 = 0; // 2006.09.26
				//
				cpreject = FALSE; // 2019.02.01
				//
				//==========================================================================================
//				FM_Pick_Running_Blocking_style_0 = CM1; // 2008.03.31 // 2008.09.25
				//==========================================================================================
				if ( Get_RunPrm_FM_PM_CONTROL_USE() && !MULTI_PM_DIRECT_MODE ) { // 2014.11.06 // 2014.12.05
					//
					if ( SCHEDULER_FM_PM_GET_PICK_PM( 0 , CHECKING_SIDE , -1 , &FM_Pm , &FM_TSlot , &FM_Slot , &FM_dbSide , &FM_Pointer , &FM_TSlot2 , &FM_Slot2 , &FM_dbSide2 , &FM_Pointer2 ) > 0 ) {
						//
						retpointer = 0;
//						cpreject = FALSE; // 2019.02.01
						//
						ret_rcm = FM_Pm;
						ret_pt = FM_Pointer;
						//
						Result = 9999;
					}
					else {
						Result = -9999;
					}
				}
				else {
					//
					FM_dbSide = CHECKING_SIDE; // 2015.05.29
					FM_dbSide2 = CHECKING_SIDE; // 2015.05.29
					//
					if ( Dual_AL_Dual_FM_Second_Check > 1 ) { // 2014.03.11
						Result = -9998;
					}
					else {
						if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2007.08.29
							Result = 9999;
						}
						else {
							Result = -9999;
						}
					}
				}
				//
//				if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2007.08.29 // 2014.03.06
				if ( Result > 0 ) { // 2007.08.29 // 2014.03.06
					//==========================================================================================
					FM_Pick_Running_Blocking_style_0 = ret_rcm; // 2008.03.31 // 2008.09.25
					//==========================================================================================
					if ( SCHEDULER_ONEFMTM_INVALID_SUPPLY( CHECKING_SIDE , ret_pt , &FM_Mode ) ) { // 2013.05.08
						_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 10000 );
						Result = -903;
					}
					else {
//						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) && _SCH_SUB_FM_Current_No_Way_Supply( CHECKING_SIDE , ret_pt , Sch_NoWay_Option() ) ) { // 2008.07.19 // 2014.11.07
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) && _SCH_SUB_FM_Current_No_Way_Supply( CHECKING_SIDE , ret_pt , Sch_NoWay_Option() ) && ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) ) { // 2008.07.19 // 2014.11.07
							_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1  + 11000 ); // 2008.07.19
							Result = -902; // 2008.07.19
						} // 2008.07.19
						else {
							//
							//==================================================================================================
							if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ) { // 2006.09.07
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 101 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
								switch ( SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( CHECKING_SIDE , &FM_Buffer , &FM_Buffer2 , 0 , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , TRUE ) ) { // 2006.02.22 // 2007.05.17
								case 1 :
									//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 102 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
										//
										if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
											//
											if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) || ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) ) { // 2019.01.31
												Result = -2001;
												if ( FM_Mode == 1 ) FM_Mode = 12;
											}
											else {
												//==========================================================================
												// 2007.08.28
												//==========================================================================
												switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
												case 1 :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 2 :
														Result = -1000;
														break;
													default :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													}
													break;
												case 2 :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 1 :
														Result = -1000;
														break;
													default :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													}
													break;
												default :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 1 :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													case 2 :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													default :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 3 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													}
													break;
												}
												//==========================================================================
											}
										}
										else {
											//==========================================================================
											// 2007.08.28
											//==========================================================================
											switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
											case 1 :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											case 2 :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											default :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 3 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											}
											//==========================================================================
										}
									}
									break;
								case 0 :
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 103 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									//
									if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
										//
										if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
											//
											if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) || ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) ) { // 2019.01.31
												Result = -2002;
												if ( FM_Mode == 1 ) FM_Mode = 12;
											}
											else {
												//==========================================================================
												// 2007.08.28
												//==========================================================================
												switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
												case 1 :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 2 :
														Result = -1000;
														break;
													default :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													}
													break;
												case 2 :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 1 :
														Result = -1000;
														break;
													default :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													}
													break;
												default :
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
													case 1 :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													case 2 :
			//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
														Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
														break;
													default :
														if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) ) { // 2014.03.04
															if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) {
																Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															}
															else if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) {
																Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															}
															else {
																Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															}
														}
														else {
															if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) { // 2007.08.29 // 2008.08.29
																Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															}
															else {
																if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) == 3 ) { // 2008.07.08
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
																else {
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
															}
														}
														break;
													}
													break;
												}
												//==========================================================================
											}
										}
										else {
											//==========================================================================
											// 2007.08.28
											//==========================================================================
											switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
											case 1 :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											case 2 :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											default :
		//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 0 , &cpreject , &retpointer ); // 2008.11.01
												Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
												break;
											}
											//==========================================================================
										}
									}
									break;
								case -1 : // 2008.02.15
									Result = -1001;
									break;
								}
							}
							else {
//								if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2003.04.23 // 2015.11.21
								if ( MULTI_PM_PICK_FROM_PM || _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2003.04.23 // 2015.11.21
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 201 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									switch ( SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( CHECKING_SIDE , &FM_Buffer , &FM_Buffer2 , 0 , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , TRUE ) ) { // 2006.02.22 // 2007.05.17
									case 1 :
										//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 202 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
											//
											if ( _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2007.08.29
												//
												if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) || ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) ) { // 2019.01.31
													Result = -2003;
													if ( FM_Mode == 1 ) FM_Mode = 12;
												}
												else {
													//==========================================================================
													// 2007.08.28
													//==========================================================================
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
													case 1 :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 2 :
															Result = -1000;
															break;
														default :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														}
														break;
													case 2 :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 1 :
															Result = -1000;
															break;
														default :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														}
														break;
													default :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 1 :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														case 2 :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														default :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 3 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														}
														break;
													}
													//==========================================================================
												}
											}
											else {
												//==========================================================================
												// 2007.08.28
												//==========================================================================
												switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
												case 1 :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												case 2 :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												default :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 3 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												}
												//==========================================================================
											}
										}
										break;
									case 0 :
										//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 203 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
											//
											if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
												//
												if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) || ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) ) { // 2019.01.31
													Result = -2004;
													if ( FM_Mode == 1 ) FM_Mode = 12;
												}
												else {
													//==========================================================================
													// 2007.08.28
													//==========================================================================
													switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
													case 1 :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 2 :
															Result = -1000;
															break;
														default :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														}
														break;
													case 2 :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 1 :
															Result = -1000;
															break;
														default :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														}
														break;
													default :
														switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) {
														case 1 :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														case 2 :
			//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
															Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
															break;
														default :
															if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) ) { // 2014.03.04
																if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) {
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
																else if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) ) {
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
																else {
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
															}
															else {
																if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) { // 2007.08.29 // 2008.08.29
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
																else {
																	Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
																}
															}
															break;
														}
														break;
													}
													//==========================================================================
												}
											}
											else {
												//==========================================================================
												// 2007.08.28
												//==========================================================================
												switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ret_rcm ) ) { // 2007.08.28
												case 1 :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 1 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 1 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												case 2 :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 2 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 2 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												default :
		//											Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , FALSE , 0 , &cpreject , &retpointer ); // 2008.11.01
													Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_Side , &FM_Side2 , &cpreject ); // 2008.11.01
													break;
												}
												//==========================================================================
											}
										}
										break;
									case -1 : // 2008.02.15
										Result = -1001;
										break;
									}
								}
								else {
									Result = -101;
									cpreject = TRUE;
								}
							}
						}
					}
				}
				else { // 2007.09.11
					Result = -901;
				}
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d , %d,%d [%d] [rejectpt=%d]\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 , middleswap_tag , retpointer );
				//==============================================================================================
				place_bm_Separate_supply = FALSE; // 2007.05.15
				//==============================================================================================
				// 2007.05.03
				//==============================================================================================
				//
				pmslotmode = 0; // 2015.05.27
				//
				if ( Result >= 0 ) {
					//
					// 2018.09.19
					//
					if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( Scheduling_TM_Double( FALSE ) ) {
							if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_A_SIDE_ONLY ) {
								FM_Slot2 = 0;
								if ( FM_dbSide == FM_dbSide2 ) _SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer2 );
							}
							else if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_B_SIDE_ONLY ) {
								FM_Slot2 = 0;
								if ( FM_dbSide == FM_dbSide2 ) _SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer2 );
							}
						}
					}
					//
					//
					//
					if ( !SCHEDULER_FM_CPM_PICK_POSSIBLE( FM_Buffer ) ) { // 2015.05.29
						//
						if ( FM_Slot > 0 ) {
							_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
						}
						else if ( FM_Slot2 > 0 ) {
							_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , retpointer );
						}
						//
						Result = -72;
					}
					else {
						if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
							//
//							FM_dbSide = CHECKING_SIDE; // 2015.05.29
//							FM_dbSide2 = CHECKING_SIDE; // 2015.05.29
							//
							FM_TSlot = FM_Slot;
							FM_TSlot2 = FM_Slot2;
							//
							if ( Get_RunPrm_FM_PM_CONTROL_USE() ) { // 2015.05.27
								//
								if ( !SCHEDULER_FM_PM_DIRECT_RUN( 1 , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , &rmode ) ) {
									//
									// 2016.08.12
									//
									if ( FM_Slot > 0 ) {
										_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
									}
									else if ( FM_Slot2 > 0 ) {
										_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , retpointer );
									}
									//
									//
									Result = -73;
								}
								else {
									//
									if      ( rmode == 1 ) {
										//
										pmslotmode = 10;
										//
										if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
											FM_Slot2 = 0;
											_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer );
										}
										else if ( FM_Slot > 0 ) {
										}
										else if ( FM_Slot2 > 0 ) {
											pmslotmode = 11;
										}
									}
									else if ( rmode == 2 ) {
										//
										pmslotmode = 20;
										//
										if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
											_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , FM_Pointer );
											//
											FM_Slot2 = FM_Slot;			FM_Slot = 0;
											FM_dbSide2 = FM_dbSide;		FM_dbSide = 0;
											FM_Pointer2 = FM_Pointer;	FM_Pointer = 0;
											//
										}
										else if ( FM_Slot > 0 ) {
											pmslotmode = 22;
										}
										else if ( FM_Slot2 > 0 ) {
										}
									}
									//
								}
							}
							//
						}
						//
						if ( Result >= 0 ) {
							//
							switch ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( CHECKING_SIDE , FM_Buffer , FM_dbSide , FM_Pointer , FM_Slot , FM_Buffer2 , FM_dbSide2 , FM_Pointer2 , FM_Slot2 , &k ) ) { // 2007.05.03
							case 0 :
								break;
							case 1 :
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
								Result = -71;
								//
								// 2014.05.14
								//
								if ( ( Dual_AL_Dual_FM_Second_Check <= 1 ) && _SCH_MODULE_Need_Do_Multi_FAL() ) {
									if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) {
										if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
											//
											if ( FM_Mode == 11 ) FM_Mode = 12;
											//
										}
										else { // 2016.06.26
											if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
												//
												if ( FM_Mode == 1 ) FM_Mode = 12;
												//
											}
										}
									}
								}
								//
								break;
							case 2 :
								place_bm_Separate_supply = TRUE; // 2007.05.15
								if ( FM_Slot2 > 0 ) {
									_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , FM_Pointer2 ); // 2007.05.15
									FM_Slot2 = 0;
									FM_Pointer2 = 0;
								}
								break;
							}
						}
					}
				}
				//
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-A CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d %d,%d K=%d,rmode=%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 , k , rmode );
				//==============================================================================================
				//==============================================================================================
				// 2004.05.10
				//==============================================================================================
				if ( Result >= 0 ) {
					if ( ( Result >= 0 ) && ( FM_Slot > 0 ) ) {
						if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( FM_dbSide , FM_Pointer ) ) {
							_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
							Result = -101;
							break;
						}
					}
					if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( FM_dbSide2 , FM_Pointer2 ) ) {
							_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
							Result = -102;
							break;
						}
					}
					//==============================================================================================
					//==============================================================================================
					// 2004.11.05
					//==============================================================================================
					//==============================================================================================
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
						if ( ( Result >= 0 ) && ( FM_Slot > 0 ) ) {
							if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( FM_dbSide , FM_Pointer , TRUE ) ) {
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 12000 ); // 2004.12.06
								Result = -101;
								break;
							}
						}
						if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( FM_dbSide2 , FM_Pointer2 , TRUE ) ) {
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 13000 ); // 2004.12.06
								Result = -102;
								break;
							}
						}
					}
					//==============================================================================================
					// 2007.05.21
					//==============================================================================================
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) {
						if ( ( Result >= 0 ) && ( FM_Slot > 0 ) ) {
							if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( FM_dbSide , FM_Pointer , TRUE ) ) {
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 14000 );
								Result = -101;
								break;
							}
						}
						if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( FM_dbSide2 , FM_Pointer2 , TRUE ) ) {
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 15000 );
								Result = -102;
								break;
							}
						}
					}
					//==============================================================================================
					//==============================================================================================
					if ( Result >= 0 ) {
						//
						//----------------------------------------------------------------------------------------------------------------------------------------------
						// User Interface
						// 2016.05.10
						//----------------------------------------------------------------------------------------------------------------------------------------------
						switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 1 , FM_dbSide , FM_Pointer , FM_Slot , FM_dbSide2 , FM_Pointer2 , FM_Slot2 , 0 ) ) {
						case 0 :
							return 0;
							break;
						case 1 :
							_SCH_MODULE_Set_FM_DoPointer( FM_dbSide , retpointer );
							Result = -111;
							break;
							//
						case -2 : // 2018.10.31
							//
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								//
								place_bm_Separate_supply = TRUE;
								//
								_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , FM_Pointer2 );
								//
								FM_Slot2 = 0;
								FM_Pointer2 = 0;
								//
							}
							//
							break;
							//
						}
						//----------------------------------------------------------------------------------------------------------------------------------------------
						//
					}
					//==============================================================================================
					//==============================================================================================
				}
				else {
					//-----------------------------------------------------------------
					FM_Pick_Running_Blocking_style_0 = 0; // 2008.03.31
					//-----------------------------------------------------------------
					if ( cpreject ) break;
				}
				if ( Result < 0 ) { // 2008.03.31
					//-----------------------------------------------------------------
					FM_Pick_Running_Blocking_style_0 = 0; // 2008.03.31
					//-----------------------------------------------------------------
				}
				//
			}
			else {
				//----------------------------------------------
				place_bm_Separate_supply = TRUE; // 2007.09.11
				//----------------------------------------------
			}
			//
			//==============================================================================================
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
			//==============================================================================================
			//==============================================================================================
			//
			if ( ( Result >= 0 ) || ( FM_Mode == 12 ) ) {
				//
				if ( FM_Mode != 12 ) {
					//----------------------------------------------------------------------
//					FM_dbSide  = CHECKING_SIDE; // 2007.09.11 // 2014.11.07
//					FM_dbSide2 = CHECKING_SIDE; // 2007.09.11 // 2014.11.07
					//==========================================================================================
					// 2006.09.26
					//==========================================================================================
					if ( FM_Buffer2 > 0 ) {
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer2 ) == BUFFER_WAIT_STATION ) {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer2 , -1 , TRUE , 0 , 201 );
						}
					}
					/*
					// 2013.03.18
					//==============================================================================================
					// 2007.11.05
					//==============================================================================================
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
						if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
								//==========================================================================================
								if ( _SCH_MODULE_Chk_FM_Finish_Status( FM_dbSide ) ) _SCH_SYSTEM_LASTING_SET( FM_dbSide , TRUE );
								//==========================================================================================
								if ( Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch( -1 , &FM_dbSide2 , &FM_Slot2 , &FM_Pointer2 , FM_dbSide , FM_Pointer ) != 1 ) { // 2007.11.16
									FM_Slot2 = 0;
								}
								else {
									//===========================================================================================================
									// 2007.11.16
									//===========================================================================================================
									switch ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( CHECKING_SIDE , FM_Buffer , FM_dbSide , FM_Pointer , FM_Slot , FM_Buffer2 , FM_dbSide2 , FM_Pointer2 , FM_Slot2 ) ) {
									case 0 :
										break;
									case 1 :
									case 2 :
										_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , 0 );
										FM_Slot2 = 0;
										break;
									}
									//===========================================================================================================
								}
							}
						}
					}
					*/
					// 2013.10.01
					if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.07
						//----------------------------------------------------------------------
						// 2013.03.18
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
							if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
									//==========================================================================================
									if ( _SCH_MODULE_Chk_FM_Finish_Status( FM_dbSide ) ) _SCH_SYSTEM_LASTING_SET( FM_dbSide , TRUE );
									//==========================================================================================
								}
							}
						}
						//----------------------------------------------------------------------
						if ( FM_Slot  > 0 ) {
							//---------------------------------------------------------------------------------------------------------------
							// 2004.05.06
							//---------------------------------------------------------------------------------------------------------------
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2004.05.06
								//=============================================================================================
								SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide , FM_Pointer ) ); // 2007.10.04
								//=============================================================================================
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 0 , 0 , 0 ); // 2006.11.27 // 2006.12.19
							}
							else {
								if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) { // 2008.01.15
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer ,  0 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								}
								else {
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 20 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								}
							}
							//---------------------------------------------------------------------------------------------------------------
						}
						if ( FM_Slot2 > 0 ) {
							//---------------------------------------------------------------------------------------------------------------
							// 2004.05.06
							//---------------------------------------------------------------------------------------------------------------
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
								//=============================================================================================
								SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide2 , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) ); // 2007.10.04
								//=============================================================================================
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27 // 2006.12.19
							}
							else {
								if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) { // 2008.01.15
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  0 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								}
								else {
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 20 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								}

							}
							//---------------------------------------------------------------------------------------------------------------
						}
						//==========================================================================================
						// 2006.09.07
						//==========================================================================================
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2006.09.07
							//==========================================================================================
							// 2004.05.19
							//==========================================================================================
							/*
							// 2014.01.14
							//
	//						_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1 ); // 2003.10.10 // 2004.05.19
							//
							// 2014.01.09
							//
							if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2014.01.09
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									if ( FM_dbSide == FM_dbSide2 ) {
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 16000 );
									}
									else {
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 17000 );
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 18000 );
									}
								}
								else if ( FM_Slot > 0 ) {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 19000 );
								}
								else if ( FM_Slot2 > 0 ) {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 20000 );
								}
							}
							else {
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									if ( FM_dbSide == FM_dbSide2 ) {
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 2  + 21000 );
									}
									else {
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 22000 );
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 23000 );
									}
								}
								else if ( FM_Slot > 0 ) {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 24000 );
								}
								else if ( FM_Slot2 > 0 ) {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 25000 );
								}
							}
							*/
							//
							// 2014.01.14
							//
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								if ( FM_dbSide == FM_dbSide2 ) {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 2  + 21000 );
								}
								else {
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 22000 );
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 23000 );
								}
							}
							else if ( FM_Slot > 0 ) {
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide , 1  + 24000 );
							}
							else if ( FM_Slot2 > 0 ) {
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( FM_dbSide2 , 1  + 25000 );
							}
							//
							//==========================================================================================
						}
						//==============================================================================================
						//----------------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06 START_PICK_from_CM = Slot(%d,%d) Pointer(%d,%d) Side(%d,%d)\n" , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
						//-----------------------------------------------
						_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
						//----------------------------------------------
						//
						if ( FM_Slot  > 0 ) {
							_SCH_MODULE_Inc_FM_OutCount( FM_dbSide );
							//--------------------------
							// Insert 2001.12.27
							_SCH_MODULE_Set_FM_LastOutPointer( FM_dbSide , FM_Pointer );
						}
						if ( FM_Slot2 > 0 ) {
							_SCH_MODULE_Inc_FM_OutCount( FM_dbSide2 );
							//--------------------------
							// Insert 2001.12.27
							_SCH_MODULE_Set_FM_LastOutPointer( FM_dbSide2 , FM_Pointer2 );
						}
					}
					else {
						//
						if ( FM_Slot  > 0 ) {
							_SCH_CLUSTER_Set_PathStatus( FM_dbSide , FM_Pointer , SCHEDULER_SUPPLY );
						}
						if ( FM_Slot2 > 0 ) {
							_SCH_CLUSTER_Set_PathStatus( FM_dbSide2 , FM_Pointer2 , SCHEDULER_SUPPLY );
						}
						//
					}
					//----------------------------------------------------------------------
					// Code for Pick from CM
					// FM_Slot
					//----------------------------------------------------------------------
/*
// 2013.10.01
					//----------------------------------------------------------------------
					if ( FM_Slot  > 0 ) {
						//---------------------------------------------------------------------------------------------------------------
						// 2004.05.06
						//---------------------------------------------------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2004.05.06
							//=============================================================================================
							SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide , FM_Pointer ) ); // 2007.10.04
							//=============================================================================================
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 0 , 0 , 0 ); // 2006.11.27 // 2006.12.19
						}
						else {
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) { // 2008.01.15
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer ,  3 ,  2 , 0 ); // 2006.11.27 // 2008.06.26
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer ,  3 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer ,  0 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
							}
							else {
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 23 , -1 , 2 ); // 2006.11.27 // 2008.06.26
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 23 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide , FM_Pointer , 20 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
							}
						}
						//---------------------------------------------------------------------------------------------------------------
					}
					if ( FM_Slot2 > 0 ) {
						//---------------------------------------------------------------------------------------------------------------
						// 2004.05.06
						//---------------------------------------------------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
							//=============================================================================================
							SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide2 , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) ); // 2007.10.04
							//=============================================================================================
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27 // 2006.12.19
						}
						else {
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) { // 2008.01.15
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  3 ,  2 , 0 ); // 2006.11.27 // 2008.06.26
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  3 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  0 ,  0 , 0 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
							}
							else {
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 23 , -1 , 2 ); // 2006.11.27 // 2008.06.26
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 23 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 20 , 20 , 2 ); // 2006.11.27 // 2008.06.26 // 2008.11.13
							}

						}
						//---------------------------------------------------------------------------------------------------------------
					}
					*/
					if ( ( FM_Slot  > 0 ) && ( FM_Slot2 > 0 ) && ( FM_dbSide == FM_dbSide2 ) ) {
						_SCH_CLUSTER_Set_Buffer( FM_dbSide , FM_Pointer , FM_Buffer );
						_SCH_CLUSTER_Set_Buffer( FM_dbSide2 , FM_Pointer2 , FM_Buffer );
					}
					else {
						if ( FM_Slot  > 0 ) {
							_SCH_CLUSTER_Set_Buffer( FM_dbSide , FM_Pointer , FM_Buffer );
						}
						if ( FM_Slot2 > 0 ) {
							_SCH_CLUSTER_Set_Buffer( FM_dbSide2 , FM_Pointer2 , FM_Buffer );
						}
					}
					//=================================================================================================
					// 2007.03.20
					//=================================================================================================
					if ( FM_Slot > 0 ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) , 0 ) ) { // 2009.01.21
							_SCH_IO_SET_MODULE_SOURCE( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) , FM_Slot , MAX_CASSETTE_SIDE + 1 );
							_SCH_IO_SET_MODULE_RESULT( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) , FM_Slot , 0 );
							_SCH_IO_SET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) , FM_Slot , FM_Slot );
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , 0 ) ) { // 2009.01.21
							_SCH_IO_SET_MODULE_SOURCE( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , MAX_CASSETTE_SIDE + 1 );
							_SCH_IO_SET_MODULE_RESULT( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , 0 );
							_SCH_IO_SET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , FM_Slot2 );
						}
					}
					//=================================================================================================
					//----------------------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------------------
					// 2003.10.07
					//----------------------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------------------
					_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
					//----------------------------------------------------------------------------------------------------------------------------
					if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2006.02.07
							//======================================================================================================
							// 2006.07.24
							//======================================================================================================
							_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , FM_Slot , &FM_Slot_Real , &k );
							//======================================================================================================
							_SCH_FMARMMULTI_DATA_SET_FROM_CM( FM_dbSide , 0 , FM_Slot_Real , k ); // 2007.08.01
							//===============================================================================================================
//							_SCH_FMARMMULTI_PICK_FROM_CM_POST( 1 , FM_Buffer ); // 2007.07.11 // 2009.03.04
							_SCH_FMARMMULTI_PICK_FROM_CM_POST(  ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) != 1 ) ) ? 1 : 0 , FM_Buffer ); // 2007.07.11 // 2009.03.04
							//===============================================================================================================
						}
						else { // 2006.07.24
							FM_Slot_Real = FM_Slot;
						}
					}
					else { // 2006.07.24
						FM_Slot_Real = FM_Slot;
					}
					//----------------------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------------------
					// 2009.04.24
					//----------------------------------------------------------------------------------------------------------------------------
					if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					}
					//----------------------------------------------------------------------------------------------------------------------------
					if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
						//----------------------------------------------------------------------------------------------------------------------------
						// 2007.11.05
						//----------------------------------------------------------------------------------------------------------------------------
						switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , -1 , FM_Slot , ( FM_Slot_Real * 10000 ) + FM_Slot , FM_dbSide , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
						//
						//----------------------------------------------------------------------------------------------------------------------------
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_dbSide , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL ); // 2007.11.26
						//----------------------------------------------------------------------
						//
//						SCHEDULER_FM_PM_DIRECT_RUN( 1 , FM_Slot , FM_dbSide , FM_Pointer , FM_Slot2 , FM_dbSide2 , FM_Pointer2 ); // 2015.05.27
						//
					}
					else {
						//----------------------------------------------------------------------------------------------------------------------------
						// 2007.11.05
						//----------------------------------------------------------------------------------------------------------------------------
						switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
						//
						//----------------------------------------------------------------------------------------------------------------------------
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Pm , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Pm , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL ); // 2007.11.26
						//----------------------------------------------------------------------
					}
					//==============================================================================================
					// 2013.03.18
					//==============================================================================================
					if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.07
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
							if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
								//
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B2 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_dbSide=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 );
				//==============================================================================================
								if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2014.01.23
									//
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B3 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_dbSide=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 );
				//==============================================================================================
									if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
										//
										if ( !Get_RunPrm_FM_PM_CONTROL_USE() || ( ( pmslotmode / 10 ) == 0 ) ) { // 2015.05.29
											//
											//==========================================================================================
											if ( Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch( -1 , &FM_dbSide2 , &FM_Slot2 , &FM_Pointer2 , FM_dbSide , FM_Pointer ) != 1 ) { // 2007.11.16
												FM_Slot2 = 0;
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B4 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_dbSide=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 );
					//==============================================================================================
											}
											else {
												//===========================================================================================================
												// 2007.11.16
												//===========================================================================================================
												switch ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( CHECKING_SIDE , FM_Buffer , FM_dbSide , FM_Pointer , FM_Slot , FM_Buffer2 , FM_dbSide2 , FM_Pointer2 , FM_Slot2 , &k ) ) {
												case 0 :
													break;
												case 1 :
												case 2 :
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B5 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_dbSide=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 );
					//==============================================================================================
													_SCH_MODULE_Set_FM_DoPointer( FM_dbSide2 , 0 );
													FM_Slot2 = 0;
													break;
												}
												//===========================================================================================================
											}
										}
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-B6 CHECK_PICK_from_FM = Result(%d) FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_dbSide=%d,%d\n" , Result , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_dbSide , FM_dbSide2 );
				//==============================================================================================
										//
										if ( FM_Slot2 > 0 ) {
											//
											_SCH_MODULE_Inc_FM_OutCount( FM_dbSide2 );
											_SCH_MODULE_Set_FM_LastOutPointer( FM_dbSide2 , FM_Pointer2 );
											//---------------------------------------------------------------------------------------------------------------
											if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) {
												//=============================================================================================
												SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_dbSide2 , _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) );
												//=============================================================================================
												_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 0 , 0 , 0 );
											}
											else {
												if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
													_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 ,  0 ,  0 , 0 );
												}
												else {
													_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_dbSide2 , FM_Pointer2 , 20 , 20 , 2 );
												}
											}
											//---------------------------------------------------------------------------------------------------------------
											_SCH_CLUSTER_Set_Buffer( FM_dbSide2 , FM_Pointer2 , FM_Buffer );
											//---------------------------------------------------------------------------------------------------------------
											if ( SCHEDULING_ThisIs_BM_OtherChamber( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , 0 ) ) { // 2009.01.21
												_SCH_IO_SET_MODULE_SOURCE( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , MAX_CASSETTE_SIDE + 1 );
												_SCH_IO_SET_MODULE_RESULT( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , 0 );
												_SCH_IO_SET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) , FM_Slot2 , FM_Slot2 );
											}
											//---------------------------------------------------------------------------------------------------------------
											switch( _SCH_MACRO_FM_OPERATION( 0 , 410 + MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) {
											case -1 :
												return 0;
												break;
											}
											//----------------------------------------------------------------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_NEED_AL , FMWFR_PICK_CM_NEED_AL );
											//----------------------------------------------------------------------
										}
										//
									}
								}
							}
						}
					}
					//
					//----------------------------------------------------------------------------------------------------------------------------
					FM_Pick_Running_Blocking_style_0 = 0; // 2008.03.31
					//----------------------------------------------------------------------------------------------------------------------------
					// 2003.10.07
					//----------------------------------------------------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------------------
					_SCH_FMARMMULTI_FA_SUBSTRATE( -1 , 0 , FA_SUBST_SUCCESS ); // 2007.07.11
					//----------------------------------------------------------------------
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
						return 0;
					}
					//----------------------------------------------------------------------------------------------------------------------------
					// 2009.04.24
					//----------------------------------------------------------------------------------------------------------------------------
					if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
						if ( FM_Slot > 0 ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
								FM_Slot = 0;
							}
						}
						if ( FM_Slot2 > 0 ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_Slot2 = 0;
							}
						}
					}
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06-1 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
					//==============================================================================================
				}
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================
				if ( Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 3 ) ) return 1; // 2019.03.15
				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				//
				// Code Aligning Before Place to BI
				//
				//----------------------------------------------------------------------
				// 2007.08.29
				//----------------------------------------------------------------------
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06-1B CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) );
//==============================================================================================
				//
				if ( ( Dual_AL_Dual_FM_Second_Check <= 1 ) && _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2014.03.11
					//
Dual_AL_Dual_FM_Second_Check_Position : // 2014.03.05
					//
					Dual_AL_Dual_FM_Second_Check = 0;
					//
					if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) { // 2007.08.29 // 2008.08.29
						if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2008.08.29
							//
							if ( !SCHEDULER_CONTROL_Chk_BM_HAS_IN_OUT_BMOUTFIRST() ) { // 2015.02.12
								//
								if ( SCHEDULER_CONTROL_Chk_FM_Double_Has2AL_HalfPick( &al_arm ) == 0 ) { // 2016.10.13
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(PICK CM)%cWHFMALFAIL %c\n" , 9 , 9 );
									return 0;
								}
								//
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06-91 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DualAL=%d [%d,%d]\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM(1) , _SCH_MODULE_Get_MFALS_BM(2) );
				//==============================================================================================
								//
								if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									al_arm      = TA_STATION;
									FM_dbSide   = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
									al_arm      = TB_STATION;
									FM_dbSide   = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								}
								else {
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
										al_arm  = TA_STATION;
									}
									else {
										al_arm  = TB_STATION;
									}
									//
									if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
										FM_dbSide  = _SCH_MODULE_Get_MFALS_SIDE(2);
									}
									else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
										FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
									}
								}
								//
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) ) { // 2008.08.29
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 1-2 from FEM%cWHFMFAIL 1001\n" , 9 );
										return 0;
									}
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
										FM_Slot_Real = 1;
									}
									else if ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) {
										FM_Slot_Real = 2;
									}
									else {
										FM_Slot_Real = 0;
									}
								}
								else {
									FM_Slot_Real = 0;
								}
								//====================================================================================================================
								if ( _SCH_MACRO_CHECK_FM_MALIGNING( FM_dbSide , TRUE ) != SYS_SUCCESS ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//====================================================================================================================
								if ( FM_Slot_Real != 0 ) { // place
									//====================================================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
										//
										if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
											_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_Bufferx );
										}
										else {
											//
											FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.03.12
											//
											if ( FM_Bufferx == 0 ) { // 2009.03.12
												//
												if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) {
													FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Pointer , FM_Buffer );
												}
												else {
													//
													//
													// 2015.02.12
													//
	//												if ( FM_Buffer2 < BM1 ) {
	//													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
	//												}
	//												else {
	//													FM_Bufferx = FM_Buffer2;
	//												}
													//
													if ( FM_Buffer < BM1 ) {
														if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
													}
													else {
														FM_Bufferx = FM_Buffer;
													}
												}
												//
											}
										}
									}
									else { // 2009.02.20
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
									}
									//====================================================================================================================
									FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
									//====================================================================================================================
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide , AL , ( al_arm == TA_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot , FM_dbSide , FM_dbSide , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
										return 0;
									}
									//====================================================================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , FM_Slot_Real , FM_dbSide , FM_Pointer , AL , ( al_arm == TA_STATION ) ? 0 : FM_Slot , FM_Slot_Real , FM_dbSide , FM_Pointer , FM_Bufferx , FM_Bufferx );
									//====================================================================================================================
									FM_dbSide2  = FM_dbSide;
									FM_Slot2    = FM_Slot;
									FM_Pointer2 = FM_Slot_Real;
								}
								//====================================================================================================================
								// pick
								//====================================================================================================================
								al_arm2 = FM_Slot_Real;
								FM_Slot_Real = 0;
								FM_Slot = 0;
								//
								if ( al_arm2 != 2 ) { // 0 , 1
									if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
										FM_dbSide    = _SCH_MODULE_Get_MFALS_SIDE(2);
										FM_Pointer   = _SCH_MODULE_Get_MFALS_POINTER(2);
										FM_Slot      = _SCH_MODULE_Get_MFALS_WAFER(2);
										FM_Slot_Real = 2;
									}
								}
								if ( al_arm2 != 1 ) { // 0 , 2
									if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
										if ( FM_Slot == 0 ) {
											FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
											FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
											FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
											FM_Slot_Real = 1;
										}
										else {
											if ( _SCH_CLUSTER_Get_SupplyID( FM_dbSide , FM_Pointer ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) ) {
												FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
												FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
												FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
												FM_Slot_Real = 1;
											}
										}
									}
								}
								//
								FixBMPlace = FALSE; // 2016.06.21
								//
								if ( FM_Slot_Real != 0 ) {
//									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide , AL , ( al_arm == TA_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , -1 , -1 , TRUE , FM_Slot , FM_Slot , FM_dbSide , FM_dbSide , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) { // 2016.06.26
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide , AL , ( al_arm == TA_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , -1 , -1 , TRUE , FM_Slot , FM_Slot , FM_dbSide , FM_dbSide , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) { // 2016.06.26
										_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
										return 0;
									}
									//===============================================================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , FM_Slot_Real , FM_dbSide , FM_Pointer , AL , ( al_arm == TA_STATION ) ? 0 : FM_Slot , FM_Slot_Real , FM_dbSide , FM_Pointer , FMWFR_PICK_CM_DONE_AL , FMWFR_PICK_CM_DONE_AL );
									//===============================================================================================================
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
										FM_Buffer = _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real ); // 2007.09.28
										FixBMPlace = TRUE; // 2016.06.21
									}
									else { // 2009.03.12
										if ( _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real ) >= BM1 ) {
											FM_Buffer = _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real );
											FixBMPlace = TRUE; // 2016.06.21
										}
									}
									//===============================================================================================================
									FM_Buffer2 = 0;
									//===============================================================================================================
									if ( al_arm2 != 0 ) {
										// Aligning
										//===============================================================================================================
										_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide2 , ( al_arm == TA_STATION ) ? FM_Slot2 + ( FM_Pointer2 - 1 ) * 10000 : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 + ( FM_Pointer2 - 1 ) * 10000 : 0 , FM_CM , FM_Bufferx );
										//===============================================================================================================
									}

								}
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01
									while ( TRUE ) { // 2007.09.11
										// 2009.03.12
										//
//										if ( al_arm == TA_STATION ) { // 2016.06.26
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ) != 0 ) {
												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
												break;
											}
//										} // 2016.06.26
//										else { // 2016.06.26
//											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ) != 0 ) { // 2016.06.26
//												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2016.06.26
//												break; // 2016.06.26
//											} // 2016.06.26
//										} // 2016.06.26
										//
//										if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) { // 2016.06.21
//										if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , FixBMPlace , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) { // 2016.06.21 // 2016.06.26
										if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( FM_dbSide , FixBMPlace , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) { // 2016.06.21 // 2016.06.26
											FM_Buffer2 = FM_Buffer;
											break;
										}
										if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
											return 0;
										}
										//----------------------------------------------------------------------------------------------------------------------------
										// 2009.05.21
										//----------------------------------------------------------------------------------------------------------------------------
										if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
											if ( FM_Slot > 0 ) {
												if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
													FM_Slot = 0;
												}
											}
											if ( FM_Slot2 > 0 ) {
												if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
													FM_Slot2 = 0;
												}
											}
										}
										//----------------------------------------------------------------------------------------------------------------------------
										Sleep(50);
										//----------------------------------------------------------------------------------------------------------------------------
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 4 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
//============================================================================================================================================
									}
								}
								else {
									if ( al_arm == TA_STATION ) {
										FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2008.07.01
									}
									else {
										FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2008.07.01
									}
									//
									FM_Buffer2 = 0; // 2008.07.01
								}
								//============================================================================
								if ( FM_Mode == 12 ) {
									if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
										FM_Slot2 = 0;
										//
										FM_Slot_Real = FM_Slot;
									}
									else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										FM_dbSide2  = FM_dbSide;
										FM_Pointer2 = FM_Pointer;
										FM_Slot2    = FM_Slot;
										//
										FM_Slot = 0;
										FM_Slot_Real = 0;
									}
									//
									Result = 902;
								}
								else {
									if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
										FM_Slot2 = 0;
										//
										FM_Slot_Real = FM_Slot;
									}
									else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										FM_dbSide2  = FM_dbSide;
										FM_Pointer2 = FM_Pointer;
										FM_Slot2    = FM_Slot;
										//
										FM_Slot = 0;
										FM_Slot_Real = 0;
									}
									//============================================================================
								}
								//============================================================================
							}
						}
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						else if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2014.03.05
							//
							if ( !SCHEDULER_CONTROL_Chk_BM_HAS_IN_OUT_BMOUTFIRST() ) { // 2015.02.05
								//
								if ( SCHEDULER_CONTROL_Chk_FM_Double_Has2AL_HalfPick( &FM_Buffer ) == 0 ) { // 2016.10.13
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(PICK CM)%cWHFMALFAIL %c\n" , 9 , 9 );
									return 0;
								}
								//
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-91 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DualAL=%d [%d,%d]\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM(1) , _SCH_MODULE_Get_MFALS_BM(2) );
				//==============================================================================================
								//
								FM_Buffer = 0;
								//
								SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( CHECKING_SIDE , &FM_Buffer , &FM_Buffer2 , 0 , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , TRUE );
								//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 301 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
								//
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									if ( ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 3-1 from FEM%cWHFMFAIL 1001\n" , 9 );
										return 0;
									}
									//
									Dual_AL_Dual_FM_Second_Check = 2;
									//
								}
								else {
									//
									Dual_AL_Dual_FM_Second_Check = 1;
									//
								}
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05-92 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DualAL=%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check );
				//==============================================================================================

								//
								if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									al_arm      = TA_STATION;
									FM_dbSide   = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
									al_arm      = TB_STATION;
									FM_dbSide   = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								}
								else {
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
										al_arm  = TA_STATION;
									}
									else {
										al_arm  = TB_STATION;
									}
									//
									if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
										FM_dbSide  = _SCH_MODULE_Get_MFALS_SIDE(2);
									}
									else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
										FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
									}
								}
								//
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
										FM_Slot_Real = 1;
									}
									else if ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) {
										FM_Slot_Real = 2;
									}
									else {
										FM_Slot_Real = 0;
									}
								}
								else {
									FM_Slot_Real = 0;
								}
								//====================================================================================================================
								if ( _SCH_MACRO_CHECK_FM_MALIGNING( FM_dbSide , TRUE ) != SYS_SUCCESS ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//====================================================================================================================
								if ( FM_Slot_Real != 0 ) { // place
									//====================================================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
										//
										if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
											_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_Bufferx );
										}
										else {
											//
											FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.03.12
											//
											if ( FM_Bufferx == 0 ) { // 2009.03.12
												//
												if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) {
													FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Pointer , FM_Buffer );
												}
												else {
													if ( FM_Buffer < BM1 ) {
														if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
													}
													else {
														FM_Bufferx = FM_Buffer;
													}
												}
												//
											}
											//
										}
									}
									else { // 2009.02.20
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
									}
									//====================================================================================================================
									FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
									//====================================================================================================================
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide , AL , ( al_arm == TA_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot , FM_dbSide , FM_dbSide , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
										return 0;
									}
									//====================================================================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , FM_Slot_Real , FM_dbSide , FM_Pointer , AL , ( al_arm == TA_STATION ) ? 0 : FM_Slot , FM_Slot_Real , FM_dbSide , FM_Pointer , FM_Bufferx , FM_Bufferx );
									//====================================================================================================================
									FM_dbSide2  = FM_dbSide;
									FM_Slot2    = FM_Slot;
									FM_Pointer2 = FM_Slot_Real;
								}
								//====================================================================================================================
								// pick
								//====================================================================================================================
								al_arm2 = FM_Slot_Real;
								FM_Slot_Real = 0;
								FM_Slot = 0;
								//
								if ( al_arm2 != 2 ) { // 0 , 1
									if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
										FM_dbSide    = _SCH_MODULE_Get_MFALS_SIDE(2);
										FM_Pointer   = _SCH_MODULE_Get_MFALS_POINTER(2);
										FM_Slot      = _SCH_MODULE_Get_MFALS_WAFER(2);
										FM_Slot_Real = 2;
									}
								}
								if ( al_arm2 != 1 ) { // 0 , 2
									if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
										if ( FM_Slot == 0 ) {
											FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
											FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
											FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
											FM_Slot_Real = 1;
										}
										else {
											if ( _SCH_CLUSTER_Get_SupplyID( FM_dbSide , FM_Pointer ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) ) {
												FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
												FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
												FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
												FM_Slot_Real = 1;
											}
										}
									}
								}
								if ( FM_Slot_Real != 0 ) {
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide , AL , ( al_arm == TA_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_Slot + ( ( FM_Slot_Real - 1 ) * 10000 ) : 0 , -1 , -1 , TRUE , FM_Slot , FM_Slot , FM_dbSide , FM_dbSide , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
										return 0;
									}
									//===============================================================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , FM_Slot_Real , FM_dbSide , FM_Pointer , AL , ( al_arm == TA_STATION ) ? 0 : FM_Slot , FM_Slot_Real , FM_dbSide , FM_Pointer , FMWFR_PICK_CM_DONE_AL , FMWFR_PICK_CM_DONE_AL );
									//===============================================================================================================
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
										FM_Buffer = _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real ); // 2007.09.28
									}
									else { // 2009.03.12
										if ( _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real ) >= BM1 ) {
											FM_Buffer = _SCH_MODULE_Get_MFALS_BM( FM_Slot_Real );
										}
									}
									//===============================================================================================================
									FM_Buffer2 = 0;
									//===============================================================================================================
									if ( al_arm2 != 0 ) {
										// Aligning
										//===============================================================================================================
										_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide2 , ( al_arm == TA_STATION ) ? FM_Slot2 + ( FM_Pointer2 - 1 ) * 10000 : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 + ( FM_Pointer2 - 1 ) * 10000 : 0 , FM_CM , FM_Bufferx );
										//===============================================================================================================
									}
									//
									if ( al_arm == TB_STATION ) {
										//
										FM_dbSide2  = FM_dbSide;
										FM_Pointer2 = FM_Pointer;
										FM_Slot2    = FM_Slot;
										//
										FM_Slot = 0;
										FM_Slot_Real = 0;
									}
									else {
										//
										FM_Slot2 = 0;
										//
										FM_Slot_Real = FM_Slot;
									}
									//
								}
								else {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 3-2 from FEM%cWHFMFAIL 1001\n" , 9 );
									return 0;
								}
								//====================================================================================================================================================================================================================================================
								if ( DUAL_AL_FM_PICK_NOWAIT ) {
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01
										if ( al_arm == TA_STATION ) {
											FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2008.07.01
										}
										else {
											FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2008.07.01
										}
										//
										FM_Buffer2 = 0; // 2008.07.01
									}
								}
								else { // 2014.03.11
									//
									Dual_AL_Dual_FM_Second_Check = 2;
									//
									if      ( ( al_arm == TA_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
										al_arm      = TB_STATION;
										FM_dbSideM  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_PointerM = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										FM_SlotM    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_SlotM_Real = 1;
									}
									else if ( ( al_arm == TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
										al_arm      = TA_STATION;
										FM_dbSideM  = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_PointerM = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_SlotM    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_SlotM_Real = 1;
									}
									else {
										//
										FM_SlotM_Real = -1;
										//
										if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
											//
											al_arm  = TA_STATION;
											//
											if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
												FM_dbSideM = _SCH_MODULE_Get_MFALS_SIDE(2);
												FM_SlotM_Real = 0;
											}
											else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
												FM_dbSideM = _SCH_MODULE_Get_MFAL_SIDE();
												FM_SlotM_Real = 0;
											}
											//
										}
										else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
											al_arm  = TB_STATION;
											//
											if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
												FM_dbSideM = _SCH_MODULE_Get_MFALS_SIDE(2);
												FM_SlotM_Real = 0;
											}
											else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
												FM_dbSideM = _SCH_MODULE_Get_MFAL_SIDE();
												FM_SlotM_Real = 0;
											}
											//
										}
										else {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 4-1 from FEM%cWHFMFAIL 1001\n" , 9 );
											return 0;
										}
										//
									}
									//
									if ( FM_SlotM_Real != -1 ) {
										//
										if ( FM_SlotM_Real == 1 ) {
											//
											if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
												FM_SlotM_Real = 1;
											}
											else if ( _SCH_MODULE_Get_MFALS_WAFER(2) <= 0 ) {
												FM_SlotM_Real = 2;
											}
											else {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 4-1 from FEM%cWHFMFAIL 1001\n" , 9 );
												return 0;
											}
										}
										//
										//====================================================================================================================
										if ( _SCH_MACRO_CHECK_FM_MALIGNING( FM_dbSideM , TRUE ) != SYS_SUCCESS ) {
											_SCH_LOG_LOT_PRINTF( FM_dbSideM , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_SlotM , 9 , FM_SlotM , 9 , FM_SlotM );
											return 0;
										}
										//====================================================================================================================
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 401 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										if ( FM_SlotM_Real != 0 ) { // place
											//
											//
											FM_Buffer2 = 0;
											//
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( CHECKING_SIDE , &FM_Buffer2 , &FM_Bufferx , FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , TRUE );
											//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 402 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
											//
											//====================================================================================================================
											if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
												//
												if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
													_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSideM , FM_PointerM , FALSE , FALSE , &FM_Bufferx );
												}
												else {
													//
													FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSideM );
													//
													if ( FM_Bufferx == 0 ) { // 2009.03.12
														//
														if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) {
															FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSideM , FM_PointerM , FM_Buffer2 );
														}
														else {
															if ( FM_Buffer2 < BM1 ) {
																if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSideM , FM_PointerM , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
															}
															else {
																FM_Bufferx = FM_Buffer2;
															}
														}
														//
													}
													//
												}
												//
											}
											else { // 2009.02.20
												FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
											}
											//====================================================================================================================
											FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSideM , FM_PointerM );
											//====================================================================================================================
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSideM , AL , ( al_arm == TA_STATION ) ? FM_SlotM + ( ( FM_SlotM_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_SlotM + ( ( FM_SlotM_Real - 1 ) * 10000 ) : 0 , FM_CM , FM_Bufferx , TRUE , FM_SlotM , FM_SlotM , FM_dbSideM , FM_dbSideM , FM_PointerM , FM_PointerM ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
												return 0;
											}
											//====================================================================================================================
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm == TA_STATION ) ? FM_SlotM : 0 , FM_SlotM_Real , FM_dbSideM , FM_PointerM , AL , ( al_arm == TA_STATION ) ? 0 : FM_SlotM , FM_SlotM_Real , FM_dbSideM , FM_PointerM , FM_Bufferx , FM_Bufferx );
											//====================================================================================================================
											FM_dbSideM2  = FM_dbSideM;
											FM_SlotM2    = FM_SlotM;
											FM_SlotM2_Real = FM_SlotM_Real;
										}
										//====================================================================================================================
										// pick
										//====================================================================================================================
										al_arm2 = FM_SlotM_Real;
										FM_SlotM_Real = 0;
										FM_SlotM = 0;
										//
										if ( al_arm2 != 2 ) { // 0 , 1
											if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
												FM_dbSideM   = _SCH_MODULE_Get_MFALS_SIDE(2);
												FM_PointerM  = _SCH_MODULE_Get_MFALS_POINTER(2);
												FM_SlotM     = _SCH_MODULE_Get_MFALS_WAFER(2);
												FM_SlotM_Real = 2;
											}
										}
										if ( al_arm2 != 1 ) { // 0 , 2
											if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
												if ( FM_SlotM == 0 ) {
													FM_dbSideM = _SCH_MODULE_Get_MFAL_SIDE();
													FM_PointerM = _SCH_MODULE_Get_MFAL_POINTER();
													FM_SlotM    = _SCH_MODULE_Get_MFAL_WAFER();
													FM_SlotM_Real = 1;
												}
												else {
													if ( _SCH_CLUSTER_Get_SupplyID( FM_dbSideM , FM_PointerM ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) ) {
														FM_dbSideM  = _SCH_MODULE_Get_MFAL_SIDE();
														FM_PointerM = _SCH_MODULE_Get_MFAL_POINTER();
														FM_SlotM    = _SCH_MODULE_Get_MFAL_WAFER();
														FM_SlotM_Real = 1;
													}
												}
											}
										}
										if ( FM_SlotM_Real != 0 ) {
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSideM , AL , ( al_arm == TA_STATION ) ? FM_SlotM + ( ( FM_SlotM_Real - 1 ) * 10000 ) : 0 , ( al_arm == TB_STATION ) ? FM_SlotM + ( ( FM_SlotM_Real - 1 ) * 10000 ) : 0 , -1 , -1 , TRUE , FM_SlotM , FM_SlotM , FM_dbSideM , FM_dbSideM , FM_PointerM , FM_PointerM ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
												return 0;
											}
											//===============================================================================================================
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , AL , ( al_arm == TA_STATION ) ? FM_SlotM : 0 , FM_SlotM_Real , FM_dbSideM , FM_PointerM , AL , ( al_arm == TA_STATION ) ? 0 : FM_SlotM , FM_SlotM_Real , FM_dbSideM , FM_PointerM , FMWFR_PICK_CM_DONE_AL , FMWFR_PICK_CM_DONE_AL );
											//===============================================================================================================
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Buffer2 = _SCH_MODULE_Get_MFALS_BM( FM_SlotM_Real ); // 2007.09.28
											}
											else { // 2009.03.12
												if ( _SCH_MODULE_Get_MFALS_BM( FM_SlotM_Real ) >= BM1 ) {
													FM_Buffer2 = _SCH_MODULE_Get_MFALS_BM( FM_SlotM_Real );
												}
											}
											//
											if ( FM_Buffer == FM_Buffer2 ) FM_Buffer2 = 0;
											//===============================================================================================================
											if ( al_arm2 != 0 ) {
												// Aligning
												//===============================================================================================================
												_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSideM2 , ( al_arm == TA_STATION ) ? FM_SlotM2 + ( FM_SlotM2_Real - 1 ) * 10000 : 0 , ( al_arm == TB_STATION ) ? FM_SlotM2 + ( FM_SlotM2_Real - 1 ) * 10000 : 0 , FM_CM , FM_Bufferx );
												//===============================================================================================================
											}
											//
											if ( al_arm == TB_STATION ) {
												//
												FM_dbSide2  = FM_dbSideM;
												FM_Pointer2 = FM_PointerM;
												FM_Slot2    = FM_SlotM;
												//
											}
											else {
												//
												FM_dbSide  = FM_dbSideM;
												FM_Pointer = FM_PointerM;
												FM_Slot    = FM_SlotM;
												//
												FM_Slot_Real = FM_Slot;
											}
											//
										}
										else {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 3-2 from FEM%cWHFMFAIL 1001\n" , 9 );
											return 0;
										}
									}
								}
								//============================================================================
							}
						}
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						//=======================================================================================================================
						else {
							switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) {
							case 1 :
								if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 2 from FEM%cWHFMFAIL 100\n" , 9 );
									return 0;
								}
								al_arm  = TA_STATION;
								//
								FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
								FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
								FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
								FM_Slot_Real = FM_Slot;
								//
								al_arm2 = TB_STATION;
								break;
							case 2 :
								if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 3 from FEM%cWHFMFAIL 100\n" , 9 );
									return 0;
								}
								al_arm  = TB_STATION;
								//
								FM_dbSide2  = _SCH_MODULE_Get_MFAL_SIDE();
								FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER();
								FM_Slot2    = _SCH_MODULE_Get_MFAL_WAFER();
								//
								al_arm2 = TA_STATION;
								break;
//							case 0 : // 2009.10.15
							default : // 2009.10.15
								if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
									al_arm  = TA_STATION;
									//
									FM_dbSide  = _SCH_MODULE_Get_MFAL_SIDE();
									FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
									FM_Slot    = _SCH_MODULE_Get_MFAL_WAFER();
									FM_Slot_Real = FM_Slot;
									//
									al_arm2 = TB_STATION;
								}
								else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
									al_arm  = TB_STATION;
									//
									FM_dbSide2  = _SCH_MODULE_Get_MFAL_SIDE();
									FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER();
									FM_Slot2    = _SCH_MODULE_Get_MFAL_WAFER();
									//
									al_arm2 = TA_STATION;
								}
								else {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Align Try Fail 1 from FEM%cWHFMFAIL 100\n" , 9 );
									return 0;
								}
							}
							//============================================================================
							if ( _SCH_MACRO_CHECK_FM_MALIGNING( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , TRUE ) != SYS_SUCCESS ) {
								_SCH_LOG_LOT_PRINTF( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 );
								return 0;
							}
							//============================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 );
								return 0;
							}
							//===============================================================================================================
							//----------------------------------------------------------------------
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , 1 , FM_dbSide , FM_Pointer , AL , ( al_arm == TA_STATION ) ? 0 : FM_Slot2 , 1 , FM_dbSide2 , FM_Pointer2 , FMWFR_PICK_CM_DONE_AL , FMWFR_PICK_CM_DONE_AL ); // 2007.11.26
							//----------------------------------------------------------------------
							//===============================================================================================================
							/*
							// 2013.01.01
							//
							if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
								FM_Buffer = _SCH_MODULE_Get_MFAL_BM(); // 2007.09.28
							}
							else { // 2009.03.12
								if ( _SCH_MODULE_Get_MFAL_BM() >= BM1 ) {
									FM_Buffer = _SCH_MODULE_Get_MFAL_BM();
								}
							}
							*/
							//===============================================================================================================
							// 2013.10.01
							//===============================================================================================================
							FM_Buffer = _SCH_MODULE_Get_MFAL_BM();
							//===============================================================================================================
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 1 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2017.01.11
//============================================================================================================================================
							if ( FM_Mode != 12 ) { // 2007.10.03
								//
								FM_dbSideM  = _SCH_MODULE_Get_FM_SIDE( al_arm2 );
								FM_PointerM = _SCH_MODULE_Get_FM_POINTER( al_arm2 );
								FM_SlotM    = _SCH_MODULE_Get_FM_WAFER( al_arm2 );
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2012.04.13
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSideM , &FM_Buffer_Temp , 1 , FM_Buffer , 101 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 2 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									}
								}
//								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2012.11.01 // 2019.02.08
								else { // 2012.11.01 // 2019.02.08
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSideM , &FM_Buffer_Temp , 1 , FM_Buffer , 101 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 3 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									}
								}
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
									//
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
										_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSideM , FM_PointerM , FALSE , FALSE , &FM_Bufferx );
									}
									else {
										//
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSideM ); // 2009.03.12
										//
										if ( FM_Bufferx == 0 ) { // 2009.03.12
											//
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSideM , FM_PointerM , FM_Buffer_Temp );
											}
											else {
												if ( FM_Buffer_Temp < BM1 ) {
													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSideM , FM_PointerM , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
												}
												else {
													FM_Bufferx = FM_Buffer_Temp;
												}
											}
										}
									}
								}
								else {
									FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSideM ); // 2009.02.20
								}
								//===============================================================================================================
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSideM , FM_PointerM ); // 2008.08.09
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSideM , AL , ( al_arm2 == TA_STATION ) ? FM_SlotM : 0 , ( al_arm2 == TB_STATION ) ? FM_SlotM : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm2 == TA_STATION ) ? FM_SlotM : 0 , ( al_arm2 == TB_STATION ) ? FM_SlotM : 0 , FM_dbSideM , FM_dbSideM , FM_PointerM , FM_PointerM ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSideM , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_SlotM , 9 , FM_SlotM , 9 , FM_SlotM );
									return 0;
								}
								//====================================================================================================================
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm2 == TA_STATION ) ? FM_SlotM : 0 , 1 , FM_dbSideM , FM_PointerM , AL , ( al_arm2 == TA_STATION ) ? 0 : FM_SlotM , 1 , FM_dbSideM , FM_PointerM , FM_Bufferx , FM_Bufferx ); // 2007.11.26
								//----------------------------------------------------------------------
								//============================================================================
								// Aligning
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSideM , ( al_arm2 == TA_STATION ) ? FM_SlotM : 0 , ( al_arm2 == TB_STATION ) ? FM_SlotM : 0 , FM_CM , FM_Bufferx );
								//============================================================================
								//============================================================================
								//
								//
								// 2019.02.12
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01
									//
									while ( TRUE ) { // 2007.09.11
										// 2009.03.12
										if ( al_arm == TA_STATION ) {
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ) != 0 ) {
												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
												break;
											}
										}
										else {
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ) != 0 ) {
												FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 );
												break;
											}
										}
										//
										if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) {
											FM_Buffer2 = FM_Buffer;
											break;
										}
										if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
											return 0;
										}
										Sleep(50);
	//============================================================================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 5 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
	//============================================================================================================================================
									}
								}
								else {
									if ( al_arm == TA_STATION ) {
										FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2008.07.01
									}
									else {
										FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2008.07.01
									}
									//
									FM_Buffer2 = 0; // 2008.07.01
								}
								//
								//
							}
							//
							/*
							//
							// 2019.02.12
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01
								//
								while ( TRUE ) { // 2007.09.11
									// 2009.03.12
									if ( al_arm == TA_STATION ) {
										if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ) != 0 ) {
											FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide );
											break;
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ) != 0 ) {
											FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 );
											break;
										}
									}
									//
									if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) {
										FM_Buffer2 = FM_Buffer;
										break;
									}
									if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
										return 0;
									}
									Sleep(50);
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 5 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
//============================================================================================================================================
								}
							}
							else {
								if ( al_arm == TA_STATION ) {
									FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2008.07.01
								}
								else {
									FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2008.07.01
								}
								//
								FM_Buffer2 = 0; // 2008.07.01
							}
							//
							*/
							//
							//============================================================================
							//============================================================================
							if ( FM_Mode == 12 ) {
								if ( al_arm == TA_STATION ) {
									FM_Slot2 = 0;
								}
								else {
									FM_Slot  = 0;
								}
								Result = 902;
							}
							else {
								//============================================================================
								if ( al_arm2 == TA_STATION ) {
									FM_Slot = 0;
								}
								else {
									FM_Slot2 = 0;
								}
							}
						}
					}
					else {
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							//====================================================================================================================
							switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) {
							case 2 :
								al_arm  = TB_STATION;
								al_arm2 = TA_STATION;
								break;
							default :
								al_arm  = TA_STATION;
								al_arm2 = TB_STATION;
								break;
							}
							//====================================================================================================================
							// 2007.09.11
							//====================================================================================================================
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 10 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
							if ( al_arm == TA_STATION ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , 0 , 0 , 102 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 11 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
//									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2019.02.08
									else { // 2013.10.01 // 2019.02.08
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , 1 , _SCH_MODULE_Get_MFAL_BM() , 102 ); // 2017.03.14
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 102 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 12 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
									//
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
										_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_Bufferx );
									}
									else {
										//
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.03.12
										//
										if ( FM_Bufferx == 0 ) { // 2009.03.12
											//
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Pointer , -1 );
											}
											else {
												if ( FM_Buffer < BM1 ) {
													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
												}
												else {
													FM_Bufferx = FM_Buffer;
												}
											}
										}
									}
								}
								else {
									FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.02.20
								}
								//
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ); // 2008.08.09
							}
							else {
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 20 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , 0 , 0 , 103 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 21 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
//									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2019.02.08
									else { // 2013.10.01 // 2019.02.08
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , 1 , _SCH_MODULE_Get_MFAL_BM() , 103 ); // 2017.03.14
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 103 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 22 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
									//
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
										_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_Bufferx );
									}
									else {
										//
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.03.12
										//
										if ( FM_Bufferx == 0 ) { // 2009.03.12
											//
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide2 , FM_Pointer2 , -1 );
											}
											else {
												if ( FM_Buffer2 < BM1 ) {
													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
												}
												else {
													FM_Bufferx = FM_Buffer2;
												}
											}
										}
									}
								}
								else {
									FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.02.20
								}
								//
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ); // 2008.08.09
							}
							//====================================================================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 );
								return 0;
							}
							//========================================================================================
							_SCH_MACRO_SPAWN_FM_MALIGNING( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx );
							//============================================================================
							if ( _SCH_MACRO_CHECK_FM_MALIGNING( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , TRUE ) != SYS_SUCCESS ) {
								_SCH_LOG_LOT_PRINTF( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 );
								return 0;
							}
							//============================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , AL , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm == TA_STATION ) ? FM_Slot : 0 , ( al_arm == TB_STATION ) ? FM_Slot2 : 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 , 9 , ( al_arm == TA_STATION ) ? FM_Slot : FM_Slot2 );
								return 0;
							}
							//===============================================================================================================
							_SCH_MODULE_Set_FM_MODE( al_arm , FMWFR_PICK_CM_DONE_AL );
							//===============================================================================================================
//							if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28 // 2009.03.12
//								FM_Buffer = FM_Bufferx; // 2007.09.28 // 2009.03.12
//							} // 2009.03.12
							//===============================================================================================================
/*
// 2013.10.01
							if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2009.03.12
								FM_Buffer = _SCH_MODULE_Get_MFAL_BM();
							}
							else { // 2009.03.12
								if ( _SCH_MODULE_Get_MFAL_BM() >= BM1 ) {
									FM_Buffer = _SCH_MODULE_Get_MFAL_BM();
								}
							}
*/
							//===============================================================================================================
							if ( al_arm2 == TA_STATION ) {
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 30 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , 0 , 0 , 104 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 31 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
//									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2015.09.17
									else { // 2013.10.01 // 2015.09.17
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , 1 , _SCH_MODULE_Get_MFAL_BM() , 104 ); // 2017.03.14
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 104 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 32 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
									//
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
										_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_Bufferx );
									}
									else {
										//
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.03.12
										//
										if ( FM_Bufferx == 0 ) { // 2009.03.12
											//
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Pointer , FM_Buffer );
											}
											else {
												if ( FM_Buffer < BM1 ) {
													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
												}
												else {
													FM_Bufferx = FM_Buffer;
												}
											}
										}
									}
								}
								else {
									FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.02.20
								}
								//
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ); // 2008.08.09
							}
							else {
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 40 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , 0 , 0 , 105 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 41 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
//									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2015.09.17
									else { // 2013.10.01 // 2015.09.17
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , 1 , _SCH_MODULE_Get_MFAL_BM() , 105 ); // 2017.03.14
											SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer2 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 105 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 42 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
										}
									}
									//
									if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
										_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_Bufferx );
									}
									else {
										//
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.03.12
										//
										if ( FM_Bufferx == 0 ) { // 2009.03.12
											//
											if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
												FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide2 , FM_Pointer2 , FM_Buffer );
											}
											else {
												if ( FM_Buffer2 < BM1 ) {
													if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
												}
												else {
													FM_Bufferx = FM_Buffer2;
												}
											}
										}
										//
									}
								}
								else {
									FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.02.20
								}
								//
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ); // 2008.08.09
							}
							//====================================================================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , AL , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
								return 0;
							}
							//===============================================================================================================
							// 2013.10.01
							//===============================================================================================================
							FM_Buffer = _SCH_MODULE_Get_MFAL_BM();
							//===============================================================================================================
							//----------------------------------------------------------------------
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , 1 , FM_dbSide , FM_Pointer , AL , ( al_arm2 == TA_STATION ) ? 0 : FM_Slot2 , 1 , FM_dbSide2 , FM_Pointer2 , FM_Bufferx , FM_Bufferx ); // 2007.11.26
							//----------------------------------------------------------------------
							//============================================================================
							// Aligning
							//============================================================================
							_SCH_MACRO_SPAWN_FM_MALIGNING( ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx );
							//============================================================================
							//============================================================================
// 2007.10.03
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01
								while ( TRUE ) { // 2007.09.11
									if ( SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( ( al_arm == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) , &FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) {
										FM_Buffer2 = FM_Buffer;
										break;
									}
									if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
										return 0;
									}
									Sleep(50);
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 6 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
//============================================================================================================================================
								}
							}
							//============================================================================
							//============================================================================
							//============================================================================
							//============================================================================
							//============================================================================
							if ( al_arm2 == TA_STATION ) {
								FM_Slot = 0;
							}
							else {
								FM_Slot2 = 0;
							}
							//============================================================================
						}
						else {
							if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) {
								//
								FM_Buffer = _SCH_MODULE_Get_MFAL_BM(); // 2017.01.11
								//
								if      ( FM_Slot > 0 ) {
									al_arm2 = TA_STATION;
									//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 50 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
										//
										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
											if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer_Temp , 0 , 0 , 106 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 51 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
											}
										}
//										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2019.02.08
										else { // 2013.10.01 // 2019.02.08
											if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer_Temp , 1 , _SCH_MODULE_Get_MFAL_BM() , 106 ); // 2017.03.14
												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide , &FM_Buffer_Temp , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 106 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 52 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
											}
										}
										//
										if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
											_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_Bufferx );
										}
										else {
											//
											FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.03.12
											//
											if ( FM_Bufferx == 0 ) { // 2009.03.12
												//
												if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
													FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Pointer , -1 );
												}
												else { // 2008.09.16
													if ( FM_Buffer_Temp < BM1 ) {
														if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
													}
													else {
														FM_Bufferx = FM_Buffer_Temp;
													}
												}
											}
										}
									}
									else {
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide ); // 2009.02.20
									}
									//
									FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ); // 2008.08.09
								}
								else if ( FM_Slot2 > 0 ) {
									al_arm2 = TB_STATION;
									//
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 60 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2009.02.20
										//
										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
											if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer_Temp , 0 , 0 , 107 );
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 61 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
											}
										}
//										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2013.10.01 // 2019.02.08
										else { // 2013.10.01 // 2019.02.08
											if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer_Temp , 1 , _SCH_MODULE_Get_MFAL_BM() , 107 ); // 2017.03.14
												SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( FM_dbSide2 , &FM_Buffer_Temp , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? 1 : 0 , ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ? _SCH_MODULE_Get_MFAL_BM() : 0 , 107 ); // 2017.03.14
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP MFAL 62 = [FM_Mode=%d][al_arm=%d,%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d) (%d,%d,%d)\n" , FM_Mode , al_arm , al_arm2 , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Temp , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) , FM_dbSideM , FM_PointerM , FM_SlotM ); // 2017.01.11
//============================================================================================================================================
											}
										}
										//
										if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 3 ) { // 2015.12.17
											_SCH_ONESELECT_Get_First_One_What( FALSE , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_Bufferx );
										}
										else {
											//
											FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.03.12
											//
											if ( FM_Bufferx == 0 ) { // 2009.03.12
												//
												if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 2 ) { // 2007.09.28
													FM_Bufferx = SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide2 , FM_Pointer2 , -1 );
												}
												else { // 2008.09.16
													if ( FM_Buffer_Temp < BM1 ) {
														if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
													}
													else {
														FM_Bufferx = FM_Buffer_Temp;
													}
												}
											}
											//
										}
									}
									else {
										FM_Bufferx = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( FM_dbSide2 ); // 2009.02.20
									}
									//
									FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ); // 2008.08.09
								}
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , AL , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx , TRUE , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
									return 0;
								}
								//====================================================================================================================
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , 1 , FM_dbSide , FM_Pointer , AL , ( al_arm2 == TA_STATION ) ? 0 : FM_Slot2 , 1 , FM_dbSide2 , FM_Pointer2 , FM_Bufferx , FM_Bufferx ); // 2007.11.26
								//----------------------------------------------------------------------
								//============================================================================
								// Aligning
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( ( al_arm2 == TA_STATION ) ? FM_dbSide : FM_dbSide2 , ( al_arm2 == TA_STATION ) ? FM_Slot : 0 , ( al_arm2 == TB_STATION ) ? FM_Slot2 : 0 , FM_CM , FM_Bufferx );
								//============================================================================
								//============================================================================
								//============================================================================
								//============================================================================
								//============================================================================
								//============================================================================
								//============================================================================
								if ( al_arm2 == TA_STATION ) {
									FM_Slot = 0;
								}
								else {
									FM_Slot2 = 0;
								}
								//============================================================================
								// 2007.10.02
								//============================================================================
								break;
								//============================================================================
							}
						}
					}
				}
				else {
					//
					if ( !Get_RunPrm_FM_PM_CONTROL_USE() || MULTI_PM_DIRECT_MODE ) { // 2014.11.06
						//
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) && ( FM_Buffer2 > 0 ) ) { // 2006.09.26
							//=========================================================================================================
							// 2006.09.26
							//=========================================================================================================
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
							//=========================================================================================================
							// 2006.12.22
							//=========================================================================================================
	//						if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide , FM_Pointer , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								FM_Bufferx = ( FM_gpmc * 100 ) + FM_Buffer;
	//							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
									FM_Bufferx = FM_Bufferx + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
								} 
								else {
									FM_Bufferx = FM_Bufferx + ( FM_Buffer * 10000 );
								}
							}
							else {
								FM_Bufferx = FM_Buffer;
	//							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
									FM_Bufferx = FM_Bufferx + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
								}
								else {
									FM_Bufferx = FM_Bufferx + ( FM_Buffer * 10000 );
								}
							}
							//=========================================================================================================
							if ( !SCHEDULER_ALIGN_SKIP_AL0( CHECKING_SIDE , FM_Pointer ) ) { // 2014.08.26
								//
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , CHECKING_SIDE , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2006.12.22
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(A:%d)%cWHFMALFAIL A:%d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//
							}
							//=========================================================================================================
							_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
							_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
							//=========================================================================================================
						}
						else {
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
								//=========================================================================================================
								// 2006.12.22
								//=========================================================================================================
	//							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide , FM_Pointer , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
									FM_Bufferx = ( FM_gpmc * 100 ) + FM_Buffer;
									//
									if ( _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide , FM_Pointer ) == _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) ) { // 2016.10.17
										//
										FM_Bufferx = FM_Bufferx + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
										//
									}
									else {
										//
		//								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
											FM_Bufferx = FM_Bufferx + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
										}
										else {
											FM_Bufferx = FM_Bufferx + ( FM_Buffer * 10000 );
										}
										//
									}
								}
								else {
									//
									FM_Bufferx = FM_Buffer;
									//
									if ( _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide , FM_Pointer ) == _SCH_CLUSTER_Get_ClusterIndex( FM_dbSide2 , FM_Pointer2 ) ) { // 2016.10.17
										//
										FM_Bufferx = FM_Bufferx + ( FM_Buffer * 10000 );
										//
									}
									else {
		//								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
											FM_Bufferx = FM_Bufferx + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
										}
										else {
		//									FM_Bufferx = FM_Bufferx + ( FM_Buffer * 1000 ); // 2014.01.03
											FM_Bufferx = FM_Bufferx + ( FM_Buffer * 10000 ); // 2014.01.03
										}
									}
								}
								//=========================================================================================================
								_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
								_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
								//=========================================================================================================
								//
								// 2014.08.26
								//
								if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide , FM_Pointer ) ) { // 2014.08.26
									//
									if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) { // 2014.08.26
										//
									}
									else {
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , 0 , FM_Slot2 , 9 , 0 , FM_Slot2 , 9 , FM_Slot2 * 100 + 0 );
											return 0;
										}
										//
									}
									//
								}
								else {
									//
									if ( SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) { // 2014.08.26
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , 0 , 9 , FM_Slot , 0 , 9 , 0 * 100 + FM_Slot );
											return 0;
										}
										//
									}
									else {
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
											return 0;
										}
										//
									}
									//
								}
								//
							}
							else if ( FM_Slot > 0 ) {
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
								//=========================================================================================================
								// 2006.12.22
								//=========================================================================================================
	//							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide , FM_Pointer , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide , FM_Pointer , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
									FM_Bufferx = ( FM_gpmc * 100 ) + FM_Buffer;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//=========================================================================================================
								_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
								//=========================================================================================================
								//
								// 2014.08.26
								//
								if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide , FM_Pointer ) ) { // 2014.08.26
									//
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , 0 , 9 , FM_Slot , 0 , 9 , 0 * 100 + FM_Slot );
										return 0;
									}
									//
								}
								//
							}
							else if ( FM_Slot2 > 0 ) {
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 );
								//=========================================================================================================
								// 2006.12.22
								//=========================================================================================================
	//							if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
									FM_Bufferx = ( FM_gpmc * 100 ) + FM_Buffer;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//=========================================================================================================
								_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );	// 2007.09.11
								//=========================================================================================================
								//
								// 2014.08.26
								//
								if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) { // 2014.08.26
									//
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , 0 , FM_Slot2 , 9 , 0 , FM_Slot2 , 9 , FM_Slot2 * 100 + 0 );
										return 0;
									}
									//
								}
								//
							}
							//
							/*
							// 2014.08.26
							//
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2004.02.10 // 2006.12.22
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//
							*/
						}
					}
				}
				//----------------------------------------------------------------------
placeFMwaferCheck :	// 2007.12.28
				//----------------------------------------------------------------------
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06-10 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
				//==============================================================================================
				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================
				if ( Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 3 ) ) return 1; // 2019.03.15
				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================

				//----------------------------------------------------------------------
				// 2008.05.26
				//----------------------------------------------------------------------
				if ( FM_Buffer == 0 ) FM_Buffer = FM_Buffer2; // 2008.08.22
				//
				if ( FM_Buffer == 0 ) break; // 2016.07.14
				//
				while( TRUE ) {
					if ( _SCH_MODULE_GROUP_FM_POSSIBLE( FM_Buffer , G_PLACE , 0 ) ) { // 2008.05.26
						if ( FM_Buffer2 > 0 ) {
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( FM_Buffer2 , G_PLACE , 0 ) ) break;
						}
						else {
							break;
						}
					}
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
						return 0;
					}
					Sleep(50);
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 7 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
//============================================================================================================================================
				}
				//----------------------------------------------------------------------
				// 2007.12.28
				//----------------------------------------------------------------------
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.08
					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
					//=========================================================================================
					// 2006.09.26
					//=========================================================================================
					if ( FM_Buffer2 > 0 ) {
						RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
					}
					//=========================================================================================
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) {
						FM_Buffer2 = -1;
						RunBuffer = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 0 , FM_Buffer ,
							FM_dbSide ,
							FM_Pointer ,
							FM_Slot ,
							FM_dbSide2 ,
							FM_Pointer2 ,
							FM_Slot2 ,
							&FM_TSlot ,
							&FM_TSlot2 , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) );
					}
					//===============================================================================================
					else {
						if      ( FM_Slot > 0 ) { // 2017.07.11
							RunBuffer = SCH_TM_EVEN_SWAP_CHECK( FM_dbSide , FM_Pointer );
						}
						else if ( FM_Slot2 > 0 ) {
							RunBuffer = SCH_TM_EVEN_SWAP_CHECK( FM_dbSide2 , FM_Pointer2 );
						}
						else {
							RunBuffer = FALSE;
						}
						//
						if ( !RunBuffer ) { // 2017.07.11
							//
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
							//=========================================================================================
							// 2006.09.26
							//=========================================================================================
							if ( FM_Buffer2 > 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
							}
							//=========================================================================================
						}
						else { // 2017.07.11
							//
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) , TRUE );
							if ( FM_Buffer2 > 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) , TRUE );
							}
							//=========================================================================================
						}
						//
					}
				}
				else {
					//===============================================================================================
					// 2007.11.09
					//===============================================================================================
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) {
						FM_Buffer2 = -1;
						RunBuffer = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 0 , FM_Buffer ,
							FM_dbSide ,
							FM_Pointer ,
							FM_Slot ,
							FM_dbSide2 ,
							FM_Pointer2 ,
							FM_Slot2 ,
							&FM_TSlot ,
							&FM_TSlot2 , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) );
					}
					//===============================================================================================
					else {
						//
						RunBuffer = 0; // 2013.10.01
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
							if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) != 1 ) {
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
										//
										if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
											//
											RunBuffer = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , -1 , &FM_TSlot , &FM_TSlot2 , 4 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
											//
										}
										//
									}
									else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
										//
										if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
											//
											RunBuffer = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , -1 , &FM_TSlot2 , &FM_TSlot , 4 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
											//
										}
										//
									}
								}
							}
						}
						//
						if ( RunBuffer == 0 ) { // 2013.10.01
							//
							if ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) % 2 ) == 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.02.25
									FM_TSlot2 = FM_TSlot;
									FM_TSlot  = 0;
								}
							}
							else {
								//
								RunBuffer = 2;
								//
								FM_TSlot  = FM_Slot;
								FM_TSlot2 = FM_Slot2;
								//
								// 2016.08.31
								//
								ret_rcm = 0;
								//
								if ( FM_Slot > 0 ) {
									//
									if ( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) >= BM1 ) {
										//
										for ( al_arm = ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 ) ; al_arm <= _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) ; al_arm++ ) {
											//
											if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , al_arm ) <= 0 ) {
												//
												FM_TSlot  = al_arm;
												//
												ret_rcm = FM_TSlot;
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
								if ( FM_Slot2 > 0 ) {
									//
									if ( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) >= BM1 ) {
										//
										for ( al_arm = ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 ) ; al_arm <= _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) ; al_arm++ ) {
											//
											if ( ret_rcm == al_arm ) continue;
											//
											if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , al_arm ) <= 0 ) {
												//
												if ( FM_Slot <= 0 ) FM_TSlot  = 0;
												//
												FM_TSlot2  = al_arm;
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
								//
								//
							}
							//=========================================================================================
							// 2006.09.26
							//=========================================================================================
							if ( FM_Buffer2 > 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
							}
							//=========================================================================================
						}
						//
						//=========================================================================================
					}
				}
				//======================================================================================================
				// 2006.07.24
				//======================================================================================================
				FM_TSlot_Real = FM_TSlot - ( FM_Slot - FM_Slot_Real );
				//======================================================================================================
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
				//==============================================================================================
				//----------------------------------------------------------------------
				// 2007.11.20
				//----------------------------------------------------------------------
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) {
							if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) { // 2007.11.20
									break;
								}
							}
						}
					}
					//
				}
				else { // 2008.12.09
					if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( FM_TSlot > 0 ) {
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
					}
				}
				//
				//
				//
				// 2018.09.19
				//
				if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if      ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
						if      ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_A_SIDE_ONLY ) {
							if ( ( FM_TSlot % 2 ) == 0 ) { // b
								//
								FM_TSlot--;
								FM_TSlot_Real--;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot ) > 0 ) return 0;
								//
							}
						}
						else if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_B_SIDE_ONLY ) {
							if ( ( FM_TSlot % 2 ) == 1 ) { // a
								//
								FM_TSlot++;
								FM_TSlot_Real++;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot ) > 0 ) return 0;
								//
							}
						}
					}
					else if ( ( FM_Slot2 > 0 ) && ( FM_Slot <= 0 ) ) {
						if      ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide2 , FM_Pointer2 ) == HANDLING_A_SIDE_ONLY ) {
							if ( ( FM_TSlot2 % 2 ) == 0 ) { // b
								//
								FM_TSlot2--;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) > 0 ) return 0;
								//
							}
						}
						else if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide2 , FM_Pointer2 ) == HANDLING_B_SIDE_ONLY ) {
							if ( ( FM_TSlot2 % 2 ) == 1 ) { // a
								//
								FM_TSlot2++;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) > 0 ) return 0;
								//
							}
						}
					}
				}
				//
				//
				//
				//====================================================================================
				// 2007.10.03
				//====================================================================================
				if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
					Premove = 0;
				}
				else {
					Premove = 1;
				}
				//====================================================================================
				while( TRUE ) {
					//==============================================================================================
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) { // 2012.09.11
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 31 from FEM%cWHFMFAIL 31\n" , 9 );
						return 0;
					}
					//==============================================================================================
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2012.09.11
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling BM%d Abort from FEM%cWHFMFAIL 31\n" , FM_Buffer - BM1 + 1 , 9 );
						return 0;
					}
					//==============================================================================================
//					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) break; // 2012.09.11
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) == 0 ) break; // 2012.09.11
					//-----------------------------------------------------
					// 2007.10.03
					//-----------------------------------------------------
					if ( Premove == 0 ) {
						Premove = 1;
						if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , ( FM_Slot > 0 ) ? FM_TSlot : 0 , ( FM_Slot2 > 0 ) ? FM_TSlot2 : 0 ) == SYS_ABORTED ) {
							//-----------------------------------------------
							return 0;
						}
					}
					//==============================================================================================
					//
					// 2015.09.02
					//
					FM_Bufferx = Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 1 );
					//
					if ( FM_Bufferx == 0 ) return 0;
					//
					if ( FM_Bufferx == 1 ) {
						Premove = -1;
						break;
					}
					//
					//==============================================================================================
					//-----------------------------------------------------
					Sleep(1);
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-1(W) CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
					//==============================================================================================
				}
				//
				if ( Premove == -1 ) { // 2015.09.02
					break;
				}
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) {
					return 0;
				}
				if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) {
					//
					if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) { // 2007.12.14
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 103 ); // 2007.12.14
					} // 2007.12.14
					//
					//============================================================================================
					// 2007.11.20
					//============================================================================================
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling %s Wait FMSIDE%cWHFMWAITFMSIDE BM%d:1-3\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
					//
					if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2007.12.28
						if ( FM_Slot > 0 ) {
							FM_TSlot = 1;
							FM_TSlot_Real = 1;
						}
						if ( FM_Slot2 > 0 ) FM_TSlot2 = 1;
					}
					//
					Premove = 0; // 2008.04.10
					actwaitcheck = 0; // 2011.05.18
					//
					OTHER_SIDE_PLACED_WAFER = FALSE; // 2011.06.28
					//
					while( TRUE ) {
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							//-----------------------------------------------
							return 0;
						}
						//
						if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2007.12.14
							//-----------------------------------------------------------
							// 2009.08.26
							//-----------------------------------------------------------
							if ( FM_Slot > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_Slot = 0;
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot2 = 0;
								}
							}
							//
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
								Premove = 1;
								break;
							}
							//-----------------------------------------------------------
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2011.06.28
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2011.06.28
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
											//
											if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) { // 2013.10.01
												//
//												Premove = 1; // 2011.06.28 // 2013.10.01
//												break; // 2013.10.01
												//
											}
										}
									}
								}
							}
							//-----------------------------------------------------------
						}
						//-----------------------------------------------------
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) {
							//
							if ( !Scheduler_SingleSwap_Has_Full_IN( FM_Buffer ) ) { // 2016.08.25
								//
								break;
								//
							}
						}
						//-----------------------------------------------------
						// 2008.04.10
						//-----------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) != 1 ) {
							FM_Bufferx = Scheduler_Deadlock_for_impossible_Supply_and_Action_MiddleSwap( CHECKING_SIDE , 0 , FM_Buffer );
							if   ( FM_Bufferx == -1 ) {
								return 0;
							}
							else if ( FM_Bufferx == 0 ) {
								Premove = 1;
								break;
							}
							else if ( FM_Bufferx == 99 ) { // 2013.10.01
								Premove = 1;
								break;
							}
							Sleep(1);
						}
						else { // 2009.08.25
							//
							if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) { // 2014.01.03
								FM_Place_Waiting_for_BM_style_0 = FM_Buffer;
							}
							//
							if ( _SCH_PRM_GET_MTLOUT_INTERFACE() == 1 ) { // 2017.09.05
								Sleep(1);
							}
							else {
								_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
								Sleep(1);
								_SCH_SYSTEM_ENTER_FEM_CRITICAL();
							}
							//
							if ( OTHER_SIDE_PLACED_WAFER ) { // 2011.06.28
								Premove = 1;
								break;
							}
							/*
							// 2014.01.03
							//
							// 2010.01.17
							if ( SCHEDULING_CHECK_TM_All_Free_Status( 0 ) ) { // 2012.08.30

								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
											//
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_INWAIT_STATUS ) <= 0 ) {
												//
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
													//
													if ( ( _SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION ) <= 0 ) ) { // 2010.11.08

														if ( SCHEDULING_CHECK_TM_All_Free_Status( 0 ) ) { // 2010.11.08 // 2012.08.30
															//=====================================================================
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 1301 );
															//=====================================================================
														}
													}
												}
											}
										}
									}
								}
							}
							//
							*/
						}
						//-----------------------------------------------------
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-2 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d,FM_Bufferx=%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_Bufferx );
						//==============================================================================================
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , &act ) ) { // 2010.08.27 // 2012.08.10
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , TRUE , &act ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , TRUE , &act ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , TRUE , &act , FM_Buffer , ( FM_Slot > 0 ) ? TA_STATION : TB_STATION ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , TRUE , &act , FM_Buffer , ( FM_Slot > 0 ) ? TA_STATION : TB_STATION , FALSE , NULL ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
							return 0;
						}
						//==============================================================================================
						if ( !act ) { // 2011.05.18
							if ( !Scheduler_Other_BM_Make_TMSide( CHECKING_SIDE , FM_Buffer , &actwaitcheck ) ) { // 2010.08.27
								return 0;
							}
						}
						//==============================================================================================
						//
						// 2016.07.22
						//
						FM_Bufferx = Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 1 );
						//
						if ( FM_Bufferx == 0 ) return 0;
						//
						if ( FM_Bufferx == 1 ) {
							Premove = -1;
							break;
						}
						//
						//==============================================================================================
					}
					//============================================================================================
					FM_Place_Waiting_for_BM_style_0 = 0; // 2014.01.03
					//============================================================================================
					if ( Premove != 0 ) break; // 2008.04.10
					//============================================================================================
/*
// 2011.06.28
					if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2007.12.14
						if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
								break;
							}
						}
					}
*/
					//============================================================================================
					actwaitcheck = 0; // 2011.05.18
					//
					while( TRUE ) {
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							//-----------------------------------------------
							return 0;
						}
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
							//
							if ( !Scheduler_SingleSwap_Has_Full_IN( FM_Buffer ) ) { // 2016.08.25
								//
								break;
								//
							}
							//
						}
						//-----------------------------------------------------
						// 2007.10.03
						//-----------------------------------------------------
						if ( Premove == 0 ) {
							Premove = 1;
							if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , FM_TSlot , FM_TSlot2 ) == SYS_ABORTED ) {
								//-----------------------------------------------
								return 0;
							}
						}
						//-----------------------------------------------------
						Sleep(1);
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-3 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
						//==============================================================================================
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , &act ) ) { // 2010.08.27 // 2012.08.10
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , TRUE , &act ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , FM_Buffer , TRUE , &act ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02
//						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , FM_Buffer , TRUE , &act , FM_Buffer , ( FM_Slot > 0 ) ? TA_STATION : TB_STATION ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , FM_Buffer , TRUE , &act , FM_Buffer , ( FM_Slot > 0 ) ? TA_STATION : TB_STATION , FALSE , NULL ) ) { // 2010.08.27 // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
							return 0;
						}
						//==============================================================================================
						if ( !act ) { // 2011.05.18
							if ( !Scheduler_Other_BM_Make_TMSide( CHECKING_SIDE , FM_Buffer , &actwaitcheck ) ) { // 2010.08.27
								return 0;
							}
						}
						//==============================================================================================
						//
						// 2015.09.02
						//
						FM_Bufferx = Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 1 );
						//
						if ( FM_Bufferx == 0 ) return 0;
						//
						if ( FM_Bufferx == 1 ) {
							Premove = -1;
							break;
						}
						//
						//==============================================================================================
					}
					//
					if ( Premove == -1 ) { // 2015.09.02
						break;
					}
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) {
						return 0;
					}
				}
				//============================================================================================
				// 2007.11.20
				//============================================================================================
				if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2007.11.20
					//
					Premove = 0; // 2011.06.28
					//
					while( TRUE ) {
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							//-----------------------------------------------
							return 0;
						}
						//
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2011.06.28
							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2011.06.28
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
										Premove = 1; // 2011.06.28
										break;
									}
								}
							}
						}
						//-----------------------------------------------------
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) break;
						//-----------------------------------------------------
						Sleep(1);
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-4 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
						//==============================================================================================
					}
					//============================================================================================
					if ( Premove == 1 ) { // 2011.06.28
						break;
					}
					//
//					if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) { // 2011.06.28
//						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
//							break;
//						}
//					}
					//============================================================================================
				}


				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================
				if ( Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 3 ) ) return 1; // 2019.03.15
				//============================================================================================================================================
				//============================================================================================================================================
				//============================================================================================================================================

				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				// 2013.10.01
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------

				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
					//
					if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
						//
						ret_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
						al_arm  = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
						ret_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
						//
						if ( al_arm > 0 ) {
							//
							if ( MULTI_COOLER_PLACE_POSSIBLE( ret_rcm , ret_pt ) ) { // 2018.02.20
								//
								//=============================================================================================
								if ( _SCH_MACRO_FM_OPERATION( 0 , 1430 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm , FM_TSlot , ret_rcm , ret_pt , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
									return 0;
								}
								//=============================================================================================
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm , FM_TSlot , ret_rcm , ret_pt , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
								//=============================================================================================
								//=============================================================================================
								FM_Mode = 21;
								//=============================================================================================
								//
							}
							//
						}
					}
					if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
						//
						ret_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot2 );
						al_arm  = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 );
						ret_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot2 );
						//
						if ( al_arm > 0 ) {
							//
							if ( MULTI_COOLER_PLACE_POSSIBLE( ret_rcm , ret_pt ) ) { // 2018.02.20
								//
								//=============================================================================================
								if ( _SCH_MACRO_FM_OPERATION( 0 , 1430 + MACRO_PICK , FM_Buffer , al_arm , FM_TSlot2 , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
									return 0;
								}
								//=============================================================================================
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , al_arm , FM_TSlot2 , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
								//=============================================================================================
								//=============================================================================================
								FM_Mode = 21;
								//=============================================================================================
								//
							}
							//
						}
					}
				}
				//
				if ( Dual_AL_Dual_FM_Second_Check == 2 ) { // 2014.03.05
					//
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) {
						Dual_AL_Dual_FM_Second_Check = FM_Buffer;
					}
					//
				}
				//

// 2009.03.16
				//----------------------------------------------------------------------
				// 2007.12.28
				//----------------------------------------------------------------------
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.08
					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
					//=========================================================================================
					// 2006.09.26
					//=========================================================================================
					if ( FM_Buffer2 > 0 ) {
						RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
					}
					//=========================================================================================
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) {
						FM_Buffer2 = -1;
						RunBuffer = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 0 , FM_Buffer ,
							FM_dbSide ,
							FM_Pointer ,
							FM_Slot ,
							FM_dbSide2 ,
							FM_Pointer2 ,
							FM_Slot2 ,
							&FM_TSlot ,
							&FM_TSlot2 , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) );
					}
					//===============================================================================================
					else {
						//
/*
// 2018.11.22
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2017.10.16
							if (
								( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_ALL_SIDE ) &&
								( _SCH_CLUSTER_Get_PathHSide( FM_dbSide2 , FM_Pointer2 ) == HANDLING_END_SIDE  ) ) {
								//
								_SCH_CLUSTER_Set_PathHSide( FM_dbSide2 , FM_Pointer2 , HANDLING_ALL_SIDE );
								//
							}
						}
*/
						//
						if      ( FM_Slot > 0 ) { // 2017.07.11
							RunBuffer = SCH_TM_EVEN_SWAP_CHECK( FM_dbSide , FM_Pointer );
						}
						else if ( FM_Slot2 > 0 ) {
							RunBuffer = SCH_TM_EVEN_SWAP_CHECK( FM_dbSide2 , FM_Pointer2 );
						}
						else {
							RunBuffer = FALSE;
						}
						//
						if ( !RunBuffer ) { // 2017.07.11
							//
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
							//=========================================================================================
							// 2006.09.26
							//=========================================================================================
							if ( FM_Buffer2 > 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
							}
							//=========================================================================================
						}
						else { // 2017.07.11
							//
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) , TRUE );
							if ( FM_Buffer2 > 0 ) {
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) , TRUE );
							}
							//=========================================================================================
						}
						//
					}
				}
				else {
					//===============================================================================================
					// 2007.11.09
					//===============================================================================================
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) {
						FM_Buffer2 = -1;
						RunBuffer = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 0 , FM_Buffer ,
							FM_dbSide ,
							FM_Pointer ,
							FM_Slot ,
							FM_dbSide2 ,
							FM_Pointer2 ,
							FM_Slot2 ,
							&FM_TSlot ,
							&FM_TSlot2 , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) );
					}
					//===============================================================================================
					else {
						if ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) % 2 ) == 0 ) {
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) );
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.02.25
								FM_TSlot2 = FM_TSlot;
								FM_TSlot  = 0;
							}
						}
						else {
							//
							RunBuffer = 2;
							//
							FM_TSlot  = FM_Slot;
							FM_TSlot2 = FM_Slot2;
							//
							// 2016.08.31
							//
							ret_rcm = 0;
							//
							if ( FM_Slot > 0 ) {
								//
								if ( _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ) >= BM1 ) {
									//
									for ( al_arm = ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 ) ; al_arm <= _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) ; al_arm++ ) {
										//
										if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , al_arm ) <= 0 ) {
											//
											FM_TSlot  = al_arm;
											//
											ret_rcm = FM_TSlot;
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
							if ( FM_Slot2 > 0 ) {
								//
								if ( _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 ) >= BM1 ) {
									//
									for ( al_arm = ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 ) ; al_arm <= _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) ; al_arm++ ) {
										//
										if ( ret_rcm == al_arm ) continue;
										//
										if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , al_arm ) <= 0 ) {
											//
											if ( FM_Slot <= 0 ) FM_TSlot  = 0;
											//
											FM_TSlot2  = al_arm;
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
							//
							//
						}
						//=========================================================================================
						// 2006.09.26
						//=========================================================================================
						if ( FM_Buffer2 > 0 ) {
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( FM_Buffer2 , &FM_TSlot2 , &k , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer2 ) ) );
						}
						//=========================================================================================
					}
				}
				//======================================================================================================
				// 2006.07.24
				//======================================================================================================
				FM_TSlot_Real = FM_TSlot - ( FM_Slot - FM_Slot_Real );
				//======================================================================================================
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d,pmslotmode=%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , pmslotmode );
				//==============================================================================================
				//----------------------------------------------------------------------
				// 2007.11.20
				//----------------------------------------------------------------------
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) {
							if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) > 0 ) { // 2007.11.20
									//
									if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) { // 2013.10.01
										//
//										break; // 2013.10.01
										//
									}
									//
								}
							}
						}
					}
				}

//				else { // 2008.12.09 // 2014.03.06
				if ( Get_RunPrm_FM_PM_CONTROL_USE() ) { // 2015.05.27
					//
					if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 0 ) { // 2016.09.01
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( FM_TSlot > 0 ) {
								FM_TSlot2 = FM_TSlot;
								FM_TSlot = 0;
							}
						}
					}
					else {
						//
						if ( ( pmslotmode % 10 ) == 1 ) {
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
								if ( FM_TSlot > 0 ) {
									FM_TSlot2 = FM_TSlot;
									FM_TSlot = 0;
								}
							}
						}
						else if ( ( pmslotmode % 10 ) == 2 ) {
							if ( ( FM_Slot2 <= 0 ) && ( FM_Slot > 0 ) ) {
								if ( FM_TSlot2 > 0 ) {
									//
									FM_TSlot = FM_TSlot2;
									FM_TSlot2 = 0;
									//
									FM_TSlot_Real = FM_TSlot - ( FM_Slot - FM_Slot_Real );
									//
								}
							}
						}
						//
					}
				}
				else {
					if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( FM_TSlot > 0 ) {
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
					}
				}
//				} // 2014.03.06
// 2009.03.16


				//----------------------------------------------------------------------------------------------------------------------------
				// 2009.04.24
				//----------------------------------------------------------------------------------------------------------------------------
				if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( FM_Slot > 0 ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_Slot = 0;
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							FM_Slot2 = 0;
						}
					}
					if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) break;
				}
				//
				if ( Dual_AL_Dual_FM_Second_Check == 0 ) { // 2014.03.11
					//
					//========================================================================================================
					// 2007.12.28
					//========================================================================================================
					if ( ( FM_Slot  > 0 ) && ( FM_TSlot  <= 0 ) ) {
						FM_Bufferx = Scheduler_Deadlock_for_impossible_Supply_and_Action_MiddleSwap( CHECKING_SIDE , 1 , FM_Buffer ); // 2008.04.14
						if   ( FM_Bufferx == -1 ) {
							//-----------------------------------------------
							return 0;
						}
						else if ( FM_Bufferx == 0 ) { // 2008.04.17
							break;
						}
						else if ( FM_Bufferx == 99 ) { // 2013.10.01
							FM_Mode = 0;
							break;
						}
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-5 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d,FM_Bufferx=%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_Bufferx );
						//==============================================================================================
						//
						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , TRUE , &act , FM_Buffer , TA_STATION , FALSE , NULL ) ) { // 2016.06.01
							return 0;
						}
						//
						//-----------------------------------------------------
						Sleep(1); // 2008.04.14
						//-----------------------------------------------------
						goto placeFMwaferCheck; // 2007.12.28
					}
					if ( ( FM_Slot2 > 0 ) && ( FM_TSlot2 <= 0 ) ) {
						FM_Bufferx = Scheduler_Deadlock_for_impossible_Supply_and_Action_MiddleSwap( CHECKING_SIDE , 1 , FM_Buffer ); // 2008.04.14
						if   ( FM_Bufferx == -1 ) {
							//-----------------------------------------------
							return 0;
						}
						else if ( FM_Bufferx == 0 ) { // 2008.04.17
							break;
						}
						else if ( FM_Bufferx == 99 ) { // 2013.10.01
							FM_Mode = 0;
							break;
						}
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-6 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d,FM_Bufferx=%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_Bufferx );
						//==============================================================================================
						//
						if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , TRUE , &act , FM_Buffer , TB_STATION , FALSE , NULL ) ) { // 2016.06.01
							return 0;
						}
						//
						//-----------------------------------------------------
						Sleep(1); // 2008.04.14
						//-----------------------------------------------------
						goto placeFMwaferCheck; // 2007.12.28
					}
					//========================================================================================================
				}
				//======================================================================================================
				// 2015.06.19
				//======================================================================================================
				if (
					( ( FM_Slot > 0 ) && ( FM_TSlot <= 0 ) ) ||
					( ( FM_Slot2 > 0 ) && ( FM_TSlot2 <= 0 ) )
				) {
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 CHECK_PICK_from_FM(X) = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
					//==============================================================================================
//					if ( !Scheduler_All_FM_Arm_Return( CHECKING_SIDE ) ) { // 2015.09.02
					if ( Scheduler_All_FM_Arm_Return( CHECKING_SIDE , 0 ) == 0 ) { // 2015.09.02
						return 0;
					}
					//
					break;
					//
				}
				//
				//
				//
				// 2018.09.19
				//
				if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if      ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
						if      ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_A_SIDE_ONLY ) {
							if ( ( FM_TSlot % 2 ) == 0 ) { // b
								//
								FM_TSlot--;
								FM_TSlot_Real--;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot ) > 0 ) return 0;
								//
							}
						}
						else if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide , FM_Pointer ) == HANDLING_B_SIDE_ONLY ) {
							if ( ( FM_TSlot % 2 ) == 1 ) { // a
								//
								FM_TSlot++;
								FM_TSlot_Real++;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot ) > 0 ) return 0;
								//
							}
						}
					}
					else if ( ( FM_Slot2 > 0 ) && ( FM_Slot <= 0 ) ) {
						if      ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide2 , FM_Pointer2 ) == HANDLING_A_SIDE_ONLY ) {
							if ( ( FM_TSlot2 % 2 ) == 0 ) { // b
								//
								FM_TSlot2--;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) > 0 ) return 0;
								//
							}
						}
						else if ( _SCH_CLUSTER_Get_PathHSide( FM_dbSide2 , FM_Pointer2 ) == HANDLING_B_SIDE_ONLY ) {
							if ( ( FM_TSlot2 % 2 ) == 1 ) { // a
								//
								FM_TSlot2++;
								//
								if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) > 0 ) return 0;
								//
							}
						}
					}
				}
				//
				//
				//
				FM_PLACE_LOCKEDBM = 0; // 2017.02.18 L.A.M 확인중
				//
				//
				//==========================================================================================
				if ( FM_Mode == 11 ) FM_Mode = 1; // 2007.10.02
				//==========================================================================================
				// Code for Place to BI
				// FM_Slot
				// FM_Slot2
				//==========================================================================================
				if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) && ( FM_Buffer2 <= 0 ) ) { // 2006.09.26
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2009.10.13
						if ( SCHEDULER_CONTROL_Chk_BM_CROSS_OUT_MIDDLESWAP_ONLY_PLACE_PICK_PLACE_NOT_PICK( FM_Buffer , FM_TSlot , FM_TSlot2 , &ICsts , &ICsts2 ) ) {
							//=============================================================================================
							// 2009.10.13
							//=============================================================================================
							FM_dbSidex = FM_dbSide;
							//=============================================================================================
							if ( _SCH_MACRO_FM_OPERATION( 0 , 420 + MACRO_PLACE , FM_Buffer , FM_Slot , ( FM_TSlot_Real * 10000 ) + FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
								return 0;
							}
							//=============================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
							//=============================================================================================
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2007.09.11
							//=============================================================================================
							//
							_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
							//
							//=============================================================================================
							//=============================================================================================
							if ( ICsts > 0 ) {
								//
								ret_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , ICsts );
								al_arm  = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , ICsts );
								ret_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , ICsts );
								//
								if ( al_arm > 0 ) {
									//
									if ( MULTI_COOLER_PLACE_POSSIBLE( ret_rcm , ret_pt ) ) { // 2018.02.20
										//
										//=============================================================================================
										if ( _SCH_MACRO_FM_OPERATION( 0 , 430 + MACRO_PICK , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
											return 0;
										}
										//=============================================================================================
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
										//=============================================================================================
										//
									}
									//
								}
							}
							//=============================================================================================
							//=============================================================================================
							if ( _SCH_MACRO_FM_OPERATION( 0 , 440 + MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
								return 0;
							}
							//=============================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
							//=============================================================================================
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2007.09.11
							//=============================================================================================
							//
							_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide2 , FM_Pointer2 , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
							//
							//=============================================================================================
							//=============================================================================================
							if ( ICsts2 > 0 ) {
								//
								dum_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , ICsts2 );
								al_arm2 = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , ICsts2 );
								dum_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , ICsts2 );
								//
								if ( al_arm2 > 0 ) {
									//
									if ( MULTI_COOLER_PLACE_POSSIBLE( dum_rcm , dum_pt ) ) { // 2018.02.20
										//
										//=============================================================================================
										if ( _SCH_MACRO_FM_OPERATION( 0 , 450 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm2 , ICsts2 , dum_rcm , dum_pt , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
											return 0;
										}
										//=============================================================================================
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm2 , ICsts2 , dum_rcm , dum_pt , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
										//=============================================================================================
										//
									}
								}
							}
							//=============================================================================================
							FM_Mode = 21;
							//=============================================================================================
						}
						else {
							//=============================================================================================
							FM_dbSidex = FM_dbSide; // 2008.07.01
							//=============================================================================================
							//=============================================================================================
							// 2007.11.26
							//=============================================================================================
							if ( _SCH_MACRO_FM_OPERATION( 0 , 460 + MACRO_PLACE , FM_Buffer , FM_Slot , ( FM_TSlot_Real * 10000 ) + FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
								return 0;
							}
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
							//=============================================================================================
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2007.09.11
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2007.09.11
							//
							_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
							//
						}
					}
					else {
						//=============================================================================================
						FM_dbSidex = FM_dbSide; // 2008.07.01
						//=============================================================================================
						//=============================================================================================
						// 2007.11.26
						//=============================================================================================
						if ( _SCH_MACRO_FM_OPERATION( 0 , 470 + MACRO_PLACE , FM_Buffer , FM_Slot , ( FM_TSlot_Real * 10000 ) + FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , ( ( RunBuffer - 2 ) == 0 ) ) == -1 ) {
							return 0;
						}
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
						//=============================================================================================
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2007.09.11
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2007.09.11
						//
						_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
						//
					}
				}
				else if ( FM_Slot > 0 ) {
					//=============================================================================================
					FM_dbSidex = FM_dbSide; // 2008.07.01
					//=============================================================================================
					// 2007.11.26
					//=============================================================================================
					if ( _SCH_MACRO_FM_OPERATION( 0 , 480 + MACRO_PLACE , FM_Buffer , FM_Slot , ( FM_TSlot_Real * 10000 ) + FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , ( ( RunBuffer - 1 ) == 0 ) ) == -1 ) {
						return 0;
					}
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
					//=============================================================================================
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2007.09.11
					//-----------------------------------------------------------------------------------------
					//
					_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
					//
					//-----------------------------------------------------------------------------------------
					// 2003.10.08
					//-----------------------------------------------------------------------------------------
					//-----------------------------------------------------------------------------------------
					_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( FM_Buffer , FM_TSlot , 3 ); // 2007.07.11
					//========================================================================================================
					// 2006.09.26
					//========================================================================================================
					if ( ( FM_Slot2 > 0 ) && ( FM_Buffer2 > 0 ) ) {
						//========================================================================================================
						if ( !SCHEDULER_CONTROL_Chk_BM_HAS_OUT_CHECK_SWITCH( FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() ) ) {
							//=====================================================================
							// 2007.05.03
							//=====================================================================
							if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 303 from FEM%cWHFMFAIL 303\n" , 9 );
								return 0;
							}
							//=====================================================================
							//
							SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
							//
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 202 );
						}
						//========================================================================================================
						FM_Buffer_Toggle = FM_Buffer; // 2007.04.11
						//========================================================================================================
						FM_Slot = 0;
						FM_Buffer = FM_Buffer2;
						//========================================================================================================
						FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 );
						//=========================================================================================================
						// 2006.12.22
						//=========================================================================================================
//						if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_dbSide2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
						if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , FM_dbSide2 , FM_Pointer2 , FALSE , FALSE , &FM_gpmc ) == 1 ) { // 2008.06.23
							FM_Bufferx = ( FM_gpmc * 100 ) + FM_Buffer;
						}
						else {
							FM_Bufferx = FM_Buffer;
						}
						//=========================================================================================================
						if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) { // 2015.10.07
							//
							if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_dbSide2 , FM_Pointer2 ) ) { // 2014.08.26
								//
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.12.22
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(B:%d)%cWHFMALFAIL B:%d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
									return 0;
								}
								//
							}
							//
						}
						//=========================================================================================================
						if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , FM_Buffer , TRUE ) != SYS_SUCCESS ) {
							return 0;
						}
						//========================================================================================================
					}
					//
					//========================================================================================================
					//========================================================================================================
					//========================================================================================================
					//
					else { // 2016.04.08
						//
						if ( FM_Slot2 <= 0 ) {
							//
							if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) {
								//
								if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
										//
										ICsts = SCHEDULER_CONTROL_Chk_BM_HAS_OTHERSIDE_OUT_GET_SLOT_SWITCH( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide , FM_Buffer );
										//
										if ( ICsts > 0 ) {
											//
											ret_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , ICsts );
											al_arm  = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , ICsts );
											ret_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , ICsts );
											//
											if ( al_arm > 0 ) {
												//
												if ( MULTI_COOLER_PLACE_POSSIBLE( ret_rcm , ret_pt ) ) { // 2018.02.20
													//
													//=============================================================================================
													if ( _SCH_MACRO_FM_OPERATION( 0 , 480 + MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , FALSE ) == -1 ) {
														return 0;
													}
													//=============================================================================================
													_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
													//=============================================================================================
													FM_Mode = 21;
													//=============================================================================================
													//
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
						}
						//
					}
					//
					//========================================================================================================
					//========================================================================================================
					//========================================================================================================
					//
				}
				if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.09.26

					//===================================================================================================================================
					// 2012.09.11
					//===================================================================================================================================
//					Premove = 0; // 2014.03.11
					if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) { // 2014.03.11
						Premove = 0;
					}
					else {
						Premove = 1;
					}
					//====================================================================================
					while( TRUE ) {
						//==============================================================================================
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 32 from FEM%cWHFMFAIL 32\n" , 9 );
							return 0;
						}
						//==============================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling BM%d Abort from FEM%cWHFMFAIL 32\n" , FM_Buffer - BM1 + 1 , 9 );
							return 0;
						}
						//==============================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) == 0 ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) == 0 ) {
									break;
								}
							}
						}
						//-----------------------------------------------------
						// 2007.10.03
						//-----------------------------------------------------
						if ( Premove == 0 ) {
							Premove = 1;
							if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , ( FM_Slot > 0 ) ? FM_TSlot : 0 , ( FM_Slot2 > 0 ) ? FM_TSlot2 : 0 ) == SYS_ABORTED ) {
								//-----------------------------------------------
								return 0;
							}
						}
						//-----------------------------------------------------
						Sleep(1);
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-101 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
						//==============================================================================================
					}
					//
					//=============================================================================================
					FM_dbSidex = FM_dbSide2; // 2008.07.01
					//=============================================================================================
					// 2007.11.26
					//=============================================================================================
					if ( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , ( ( RunBuffer - 1 ) == 0 ) ) == -1 ) {
						return 0;
					}
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
					//
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2007.09.11
					//
					_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , FM_dbSide2 , FM_Pointer2 , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
					//
					//====================================================================================
					// 2007.04.11
					//====================================================================================
					//====================================================================================
					//
					//====================================================================================
					//====================================================================================
					// 2016.04.08
					//====================================================================================
					//====================================================================================
					//
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) ) {
						//
						if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
							//
							if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
								//
								ICsts = SCHEDULER_CONTROL_Chk_BM_HAS_OTHERSIDE_OUT_GET_SLOT_SWITCH( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , FM_dbSide2 , FM_Buffer );
								//
								if ( ICsts > 0 ) {
									//
									ret_rcm = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , ICsts );
									al_arm  = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , ICsts );
									ret_pt  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , ICsts );
									//
									if ( al_arm > 0 ) {
										//
										if ( MULTI_COOLER_PLACE_POSSIBLE( ret_rcm , ret_pt ) ) { // 2018.02.20
											//
											//=============================================================================================
											if ( _SCH_MACRO_FM_OPERATION( 0 , 490 + MACRO_PICK , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , FALSE ) == -1 ) {
												return 0;
											}
											//=============================================================================================
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , al_arm , ICsts , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC );
											//=============================================================================================
											FM_Mode = 21;
											//=============================================================================================
											//
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
					//
				}
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-1 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
				//==============================================================================================
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.08
					if ( _SCH_MODULE_Chk_FM_Finish_Status( FM_dbSidex ) ) {
						if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) || ( _SCH_MODULE_Get_MFAL_SIDE() != FM_dbSidex ) ) {  // 2008.07.01
							//
							if ( ( _SCH_SYSTEM_MODE_END_GET( FM_dbSidex ) == 0 ) || ( !_SCH_MODULE_Chk_TM_Free_Status( FM_dbSidex ) ) ) { // 2007.03.13
								if ( !_SCH_MODULE_Chk_TM_Finish_Status( FM_dbSidex ) ) { // 2007.03.27
									//=====================================================================
									//
									SCHEDULING_BM_LOCK_CHECK_TM_SIDE( FM_dbSidex , FM_Buffer ); // 2017.02.08
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , FM_dbSidex , FM_Buffer , -1 , TRUE , 0 , 203 );
									//=====================================================================
								}
								else {
				//==============================================================================================
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-11 CHECK_PICK_from_FM = FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
				//==============================================================================================
								}
							}
							//=====================================================================
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//=====================================================================
							// 2007.01.24
							//=====================================================================
							if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 1 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == FM_dbSidex ) ) {
								//===================================================================================================
//								SCHEDULER_CONTROL_Set_BM_Switch_UseSide( SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch() , -1 ); // 2008.11.24
								//===================================================================================================
								SCHEDULER_CONTROL_Set_BM_Switch_Separate( 0 , 0 , 0 , 0 , 0 );
								//===================================================================================================
							}
							//=====================================================================
						}
						//
//						_SCH_MODULE_Set_FM_SwWait( FM_dbSidex , 3 ); // 2008.07.01 // 2008.08.25
						_SCH_MODULE_Set_FM_SwWait( FM_dbSidex , FM_Buffer ); // 2008.07.01 // 2008.08.25
						//
						break;
					}
					else {
						//======================================================================================================
						// 2007.01.05
						//======================================================================================================
						if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 1 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == FM_dbSidex ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) >= SCHEDULER_CONTROL_Get_BM_Switch_SeparateCount() ) {
								//===================================================================================================
								SCHEDULER_CONTROL_Set_BM_Switch_Separate( 2 , 0 , SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) , 0 , 0 );
								//===================================================================================================
								SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( FM_dbSidex , SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch() );
								//===================================================================================================
								//
								SCHEDULING_BM_LOCK_CHECK_TM_SIDE( FM_dbSidex , FM_Buffer ); // 2017.02.08
								//
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , FM_dbSidex , FM_Buffer , -1 , TRUE , 0 , 204 );
								//=====================================================================
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								//=====================================================================
								FM_Mode = 3;
								//
//								_SCH_MODULE_Set_FM_SwWait( FM_dbSidex , 3 ); // 2008.08.25
								_SCH_MODULE_Set_FM_SwWait( FM_dbSidex , FM_Buffer ); // 2008.08.25
								//=====================================================================
								break;
							}
						}
						//======================================================================================================
					}
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2003.11.08
					//
					if ( SCHEDULER_CONTROL_Chk_AL_BM_PLACE_DIFF( FM_Buffer ) ) { // 2016.06.26
						//=====================================================================
						//
						SCHEDULING_BM_LOCK_CHECK_TM_SIDE( FM_dbSidex , FM_Buffer ); // 2017.02.08
						//
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , FM_dbSidex , FM_Buffer , -1 , TRUE , 0 , 2039 );
						//=====================================================================
						break;
					}
					//
					else {
	//					if ( SCHEDULER_CONTROL_Chk_AL2_FREE() ) { // 2014.05.14 // 2014.11.25
						if ( SCHEDULER_CONTROL_Chk_AL2_FREE() || Get_RunPrm_FM_PM_CONTROL_USE() ) { // 2014.05.14 // 2014.11.25
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
								//=====================================================================
								//
								SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
								//
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 42 );
								//=====================================================================
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							}
							//
							if ( Get_RunPrm_FM_PM_CONTROL_USE() ) break; // 2014.11.25
							//
						}
						else {
							//
							// 2019.02.07
							//
							//
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) {
								//
								if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 304(4000) from FEM%cWHFMFAIL 304\n" , 9 );
									return 0;
								}
								//=====================================================================
								//
								SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
								//
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 4000 );
								//=====================================================================
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								//
								break;
							}
							//
							if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
								//
								/*
								//
								// 2019.02.07
								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) { // 2004.03.22
									//=====================================================================
									// 2007.05.03
									//=====================================================================
									if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 304 from FEM%cWHFMFAIL 304\n" , 9 );
										return 0;
									}
									//=====================================================================
									//
									SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 4 );
									//=====================================================================
									_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								}
								else { // 2004.03.22
								//
								*/
									if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.03.22 // 2007.05.03
										//
										if ( !SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , FALSE ) ) { // 2014.03.06
											//
											if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
												if ( SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( FM_Buffer ) ) {
													//=====================================================================
													// 2007.05.03
													//=====================================================================
													if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 305 from FEM%cWHFMFAIL 305\n" , 9 );
														return 0;
													}
													//=====================================================================
													//
													SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
													//
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 41 );
													//=====================================================================
													_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
												}
											}
											else {
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2007.11.27
													if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2013.10.01
														//=====================================================================
														//
														SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 42 );
														//=====================================================================
														_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
													}
												}
											}
											//
										}
									}
								//
								//
								/*
								//
								// 2019.02.07
								//
								}
								//
								*/
								//
								//
								break;
							}
							else { // 2007.05.15
								//====================================================================================================
								// 2007.05.15
								//====================================================================================================
		//						if ( place_bm_Separate_supply ) { // 2009.10.13
								if ( place_bm_Separate_supply || ( FM_Mode == 21 ) ) { // 2009.10.13
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.02.22
										if ( SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( FM_Buffer ) ) {
											//=====================================================================
											// 2007.05.03
											//=====================================================================
											if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 305-2 from FEM%cWHFMFAIL 305-2\n" , 9 );
												return 0;
											}
											//=====================================================================
											//
											SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
											//
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 4102 );
											//=====================================================================
											_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
										}
									}
									break;
								}
								//====================================================================================================
							}
						}
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 08 END_PLACE_to_BM = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 );
				//----------------------------------------------
				_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
			}
			else {
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2003.11.07
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_Buffer ) ) {
							if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
								if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.03.22 // 2007.05.03
									//
									if ( !SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , FALSE ) ) { // 2014.03.06
										//
										if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
											//=====================================================================
											// 2007.05.03
											//=====================================================================
											if ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , FALSE ) == -1 ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 306 from FEM%cWHFMFAIL 306\n" , 9 );
												return 0;
											}
											//=====================================================================
											//
											SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
											//
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 5 );
											//=====================================================================
											_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
										}
										//
									}
									//
								}
							}
						}
					}
				}
				else {
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2004.11.09
						if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) > 1 ) { // 2007.11.20
							if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( -1 , FM_Buffer , &k ) ) { // 2004.12.03
								//=====================================================================
								if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) == 0 ) { // 2009.03.24
									//=====================================================================
									if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.12.06
										//
										SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
										//
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 6 );
										//=====================================================================
										_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
									}
									//=====================================================================
								}
							}
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2004.11.09
						if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP2( -1 , FM_Buffer , &k ) ) { // 2004.12.03
							//=====================================================================
							if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.12.06
								//
								SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
								//
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 6 );
								//=====================================================================
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							}
						}
					}
				}
				break;
			}
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.08
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( -1 , FM_Buffer , &k ) ) {
						//
						if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2019.05.30
							//
							if ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) % 2 ) == 0 ) { // 2004.03.10
								//=====================================================================
								//
								SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
								//
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 7 );
								//=====================================================================
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							}
							else { // 2004.03.10
								if ( !_SCH_CLUSTER_Check_Possible_UsedPre_GlobalOnly( FM_Buffer ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( FM_Buffer ) <= 0 ) {
										//=====================================================================
										_SCH_CLUSTER_Set_PathProcessData_SkipPre( CHECKING_SIDE , FM_Buffer );
										//=====================================================================
										//
										SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
										//
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 71 );
										//=====================================================================
										_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
									}
								}
								else {
									//=====================================================================
									//
									SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 72 );
									//=====================================================================
									_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								}
							}
							//
						}
						//
					}
				}
				break;
			}
			//
			//==========================================================================
			if ( FM_Mode == 12 ) break; // 2007.09.11
			//==========================================================================
			if ( FM_Mode == 21 ) break; // 2009.10.13
			//==========================================================================
		}
	}
	if ( FM_Mode == 3 ) {
		return TRUE;
	}
	//
	if ( FM_Mode == 21 ) { // 2009.10.13
		//=========================================================================
		// 2009.10.13
		//=========================================================================
		ret_rcm = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
		al_arm  = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
		ret_pt  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		//=============================================================================================
		dum_rcm = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
		al_arm2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
		dum_pt  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		//=============================================================================================
		//
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2018.02.20
			//
			if ( !MULTI_COOLER_PLACE_PICK_RUN( &al_arm , &ret_rcm , &ret_pt , &al_arm2 , &dum_rcm , &dum_pt ) ) {
				if ( al_arm > 0 ) {
					_SCH_LOG_LOT_PRINTF( ret_rcm , "FM Handling Fail-101 at IC(%d)%cWHFMICFAIL %d%c%d\n" , al_arm , 9 , al_arm , 9 , al_arm );
				}
				else {
					_SCH_LOG_LOT_PRINTF( dum_rcm , "FM Handling Fail-102 at IC(%d)%cWHFMICFAIL %d%c%d\n" , al_arm2 , 9 , al_arm2 , 9 , al_arm2 );
				}
				return 0;
			}
			//
		}
		else {
			//
			if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) { // 2017.11.24
				//
				if ( al_arm > 0 ) {
					//
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK ,
						ret_rcm ,
						IC ,
						1 ,
						0 ,
						_SCH_CLUSTER_Get_PathOut( ret_rcm , ret_pt ) ,
						-1 ,
						TRUE ,
						al_arm ,
						0 ,
						ret_rcm ,
						ret_rcm ,
						ret_pt ,
						ret_pt
						) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( ret_rcm , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , al_arm , 9 , al_arm , 9 , al_arm );
						return 0;
					}
				}
				//
				if ( al_arm2 > 0 ) {
					//
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK ,
						dum_rcm ,
						IC ,
						0 ,
						1 ,
						_SCH_CLUSTER_Get_PathOut( dum_rcm , dum_pt ) ,
						-1 ,
						TRUE ,
						0 ,
						al_arm2 ,
						dum_rcm ,
						dum_rcm ,
						dum_pt ,
						dum_pt
						) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( dum_rcm , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , al_arm2 , 9 , al_arm2 , 9 , al_arm2 );
						return 0;
					}
				}
			}
		}
		//
		//
		if ( ( al_arm > 0 ) || ( al_arm2 > 0 ) ) {
			//=============================================================================================
			Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , al_arm , ret_rcm , ret_pt , al_arm2 , dum_rcm , dum_pt , FALSE , TRUE , 0);
			//=============================================================================================
			if      ( Result == -1 ) { // Abort
				return 0;
			}
			else if ( Result != 0 ) { // disappear
			}
		}
		//=========================================================================
		FM_Mode = 0;
		//=========================================================================
	}

	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 08-1 SELECT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
	//==============================================================================================

pickfrombmrun :	// 2007.03.12

	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {  // 2008.07.14
		if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) ) return TRUE;  // 2008.07.01
	}

	//-------------------------------------------------------------------------------------
	mfic_locked = 0; // 2008.04.01
	//-------------------------------------------------------------------------------------
	// PICK FROM BM & PLACE TO CM PART
	//-------------------------------------------------------------------------------------
	if ( ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) == BATCHALL_SWITCH_LL_2_LL ) || ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) == BATCHALL_SWITCH_LP_2_LL ) ) { // 2010.01.19
		
		goto lastcheck;

	}

	if ( ( FM_Mode == 1 ) || ( FM_Mode == 11 ) ) { // 2007.09.11
		//==========================================================================================
		// 2007.04.11
		//==========================================================================================
		if ( FM_Buffer_Toggle != -1 ) FM_Buffer = FM_Buffer_Toggle;
		//==========================================================================================
		//-------------------------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2004.05.24
			_SCH_SYSTEM_LEAVE_FEM_CRITICAL(); // 2004.05.24
		}
		//-----------------------------------------------
		_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 36 );
		//-----------------------------------------------
		//==============================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d FM_PLACE_LOCKEDBM=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_PLACE_LOCKEDBM );
		//==============================================================================================

		//==============================================================================================
		// 2008.04.01
		//==============================================================================================
		if ( scheduler_malic_locking( CHECKING_SIDE ) ) {
			mfic_locked = 1;
			if ( FM_Mode == 1 ) {
				if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
							mfic_locked = 2;
						}
					}
				}
			}
		}
		else {
			mfic_locked = 0;
		}
		//==============================================================================================
//			if ( !scheduler_malic_locking( CHECKING_SIDE ) ) { // 2007.03.02 // 2008.04.01
		if ( mfic_locked != 1 ) { // 2007.03.02 // 2008.04.01
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) { // 2002.10.29
					if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < 3 ) {
						_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
					}
					FM_Mode = 0;
/*
// 2008.11.24
					if (
						( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( FM_Buffer ) == CHECKING_SIDE ) && // 2003.11.07
						( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 )
						) {
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , TRUE , FM_Buffer ) ) {
							FM_Mode = 2;
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , FALSE , FM_Buffer ) ) {
								if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
									FM_Mode = 2;
								}
							}
						}
					}
*/
				}
				else {
					_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
					//
					//===============================================================================================================
					FM_Mode = 2;
					//======================================================================================================
					// 2007.01.24
					//======================================================================================================
					if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 2 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) {
						FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch();
					}
					//===============================================================================================================
					while( TRUE ) {
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-A CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
						//==============================================================================================
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 4 from FEM%cWHFMFAIL 4\n" , 9 );
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2004.05.24
								_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2004.05.24
							}
							return 0;
						}
						//======================================================================================================
						// 2007.01.24
						//======================================================================================================
						if (
							( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
							( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) &&
							( FM_Buffer == SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() )
							) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) ) {
									break;
								}
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) { // 2008.12.12
									//======================================================================================
									// 2007.03.27
									//======================================================================================
									if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) {
										if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
											if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
												if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
													if ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) {
														if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
															if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_TM_STATION ) {
																//=====================================================================
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 212 );
																//=====================================================================
															}
														}
													}
												}
											}
										}
									}
									//======================================================================================
								}
							}
						}
						// Modify for Multiple Switch(2001.12.27)
						else {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
								if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
									if ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) {
										break;
									}
									if ( _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) > _SCH_MODULE_Get_FM_LastOutPointer( CHECKING_SIDE ) ) {
										break;
									}
								}
							}
						}
						//===============================================================================================================
						// 2008.01.15
						//===============================================================================================================
						if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , TRUE , FM_Buffer ) ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 0 , FALSE , FM_Buffer ) ) {
								FM_Mode = 0;
								break;
							}
							if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
								FM_Mode = 0;
								break;
							}
						}
						//===============================================================================================================
						Sleep(1);
					}
				}
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) {
				if ( Scheduler_FEM_GetOut_Properly_Check_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2003.11.28
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 1 , TRUE , FM_Buffer ) ) {
								FM_Mode = 2;
							}
							else {
								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 1 , FALSE , FM_Buffer ) ) {
//										if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) { // 2007.12.27
										FM_Mode = 2;
//										} // 2007.12.27
								}
							}
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 2 , TRUE , FM_Buffer ) ) {
								FM_Mode = 2;
							}
							else {
								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( CHECKING_SIDE , 2 , FALSE , FM_Buffer ) ) {
									if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
										FM_Mode = 2;
									}
								}
							}
						}
					}
				}
			}
			else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( CHECKING_SIDE , FM_Buffer , &Result ) ) {
					FM_Mode = 2;
				}
			}
		}
		else { // 2007.02.28
			FM_Mode = 0;
			//========================================================================================
			// 2007.11.06
			//========================================================================================
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) { // 2002.10.29
					if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < 3 ) {
						_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
					}
				}
				else {
					_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
				}
			}
			//========================================================================================
		}
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2004.05.24
			_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2004.05.24
		}
	}

	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-1 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
	//==============================================================================================
	if ( FM_Mode == 0 ) { // 2009.03.11
		if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
			if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < 3 ) {
				_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 3 );
			}
		}
	}
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-1-1 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
	//==============================================================================================

pickfrombmrun2 :	// 2007.11.06

	//=======================================================================================
	// 2007.02.28 // 2007.08.03
	//=======================================================================================
	//
	FM_Mode4_Action = FALSE; // 2013.05.29
	//
	if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
		//
//		if ( FM_Mode == 0 ) { // 2018.05.30
		if ( ( FM_Mode == 0 ) && ( !FM_Other_Pick ) ) { // 2018.05.30
			//
			FM_Mode = SCHEDULER_CONTROL_Chk_FM_ARM_Has_Been_Waiting_For_1SlotBM_Supply( CHECKING_SIDE , 0 ); // 2011.10.10
			if ( FM_Mode == 0 ) { // 2011.10.10
				//
				FM_Mode = 4;
				//
				FM_Buffer = 0;
				//
				FM_TSlot = 0; // 2007.08.03
				FM_TSlot2 = 0; // 2007.08.03
				//
			}
			else {
//xxx_check_printf( CHECKING_SIDE , FALSE , "IC Pick Check Reject" , FM_Mode );
				FM_Mode = 0;
			}
		}
		else if ( FM_Mode == 2 ) { // 2007.08.03
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) <= 0 ) {
					FM_Mode = 4;
					FM_Buffer = 0;
					//
					FM_TSlot = 0; // 2007.08.03
					FM_TSlot2 = 0; // 2007.08.03
				}
			}
			//
			if ( FM_Mode == 2 ) { // 2010.08.27
				if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) {
					FM_Mode = 4;
					FM_Buffer = 0;
					//
					FM_TSlot = 0;
					FM_TSlot2 = 0;
				}
			}
			//
		}
	}
	//=======================================================================================
	Dual_AL_Dual_FM_Check = 0; // 2014.03.06
	//=======================================================================================
//		if ( FM_Mode == 2 ) { // 2007.02.28
	if ( ( FM_Mode == 2 ) || ( FM_Mode == 4 ) ) { // 2007.02.28
//			_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2003.11.07 // 2004.05.21
		//==============================================================================================
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-2 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
		//==============================================================================================
		if ( ( FM_Mode == 4 ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) { // 2004.02.24 // 2004.05.10(Move Here) // 2007.02.28
			//-----------------------------------------------
			_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 37 );
			//-----------------------------------------------
			if ( ( FM_Mode == 4 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) ) { // 2004.05.10 (Move Here) // 2007.02.28
				if ( FM_Mode != 4 ) { // 2007.02.28
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_PART ) { // 2004.05.10 (Move Here) // 2006.09.07
						if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) { // 2002.10.29 // 2004.05.10 (Move Here)
							_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 2 ); // 2004.05.10 (Move Here)
						} // 2004.05.10 (Move Here)
					} // 2004.05.10 (Move Here)
				}

//				_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2003.11.07 Bug
				//-----------------------------------------------
				_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 38 );
				//-----------------------------------------------
				FM_CO1 = 0;
				FM_CO2 = 0;
				while( TRUE ) {
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-3 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
					//==============================================================================================
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 5 from FEM%cWHFMFAIL 5\n" , 9 );
						return 0;
					}
					//----------------------------------------------------------------------------------------------------------------------------
					// 2009.05.21
					//----------------------------------------------------------------------------------------------------------------------------
					_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 );
					//----------------------------------------------------------------------------------------------------------------------------
					if ( FM_Mode != 4 ) { // 2007.02.28
						//======================================================================================================
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) break; // 2007.03.13
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) > 0 ) break; // 2007.03.13
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) return 0; // 2013.06.28
						//
						if ( DUAL_AL_FM_PICK_NOWAIT ) {
							//
							if ( Dual_AL_Dual_FM_Second_Check == 3 ) break; // 2014.03.05
							//
							if ( Dual_AL_Dual_FM_Second_Check >= BM1 ) { // 2014.03.05
								//
								if ( Dual_AL_Dual_FM_Second_Check != FM_Buffer ) break; // 2014.03.05
								//
							}
							//
							if ( SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , TRUE ) ) { // 2014.03.06
								Dual_AL_Dual_FM_Check = 1;
								break;
							}
							//
						}
						//
						//======================================================================================================
//							if ( scheduler_malic_locking( CHECKING_SIDE ) ) break; // 2007.03.02 // 2008.04.01
						if ( mfic_locked == 1 ) break; // 2007.03.02 // 2008.04.01
						//======================================================================================================
						//----------------------------------------------------------------------
						// 2008.05.26
						//----------------------------------------------------------------------
						while( TRUE ) {
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( FM_Buffer , G_PICK , 0 ) ) { // 2008.05.26
								break;
							}
							if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
								return 0;
							}
							Sleep(50);
//============================================================================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 8 = [al_arm=%d][%d,%d] FM_Buffer=%d,%d,%d FM_Slot=%d,%d FM_Pointer=%d,%d FM_Side=%d,%d DAL=%d(%d,%d)\n" , al_arm , FM_dbSide , FM_dbSide2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Dual_AL_Dual_FM_Second_Check , _SCH_MODULE_Get_MFALS_BM( 1 ) , _SCH_MODULE_Get_MFALS_BM( 2 ) ); // 2015.09.17
//============================================================================================================================================
						}
						//----------------------------------------------------------------------
						if      ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
							Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , CHECKING_SIDE , &FM_TSlot , &FM_TSlot2 , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) ); // 2003.11.08
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
							Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , CHECKING_SIDE , &FM_TSlot , &FM_TSlot2 , 3 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) ); // 2006.02.22
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.11.23
							Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , CHECKING_SIDE , &FM_TSlot , &FM_TSlot2 , 4 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) ); // 2003.11.08
						}
						else {
							Result = SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , CHECKING_SIDE , &FM_TSlot , &FM_TSlot2 , 1 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) ); // 2003.11.08
						}
						//==========================================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 CHECK_GETOUT_PART3(%d,%d)\n" , FM_Buffer , Result );
						//==========================================================================================================
						// 2006.02.08
						//==========================================================================================================
						FM_Ret_Pointer = 0;
						if ( Result > 0 ) {
							if ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) {
								//==================================================================================================================
								// 2007.08.02
								//==================================================================================================================
								if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
									//
									if ( !SCHEDULER_COOLING_SKIP_AL0( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) ) ) { // 2013.03.14
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-a CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) );
										//==========================================================================================================
										ALsts = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
										if ( ALsts <= 0 ) {
											//==========================================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-b CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) );
											//==========================================================================================================
											FM_Mode = 0; // 2007.11.06
											goto pickfrombmrun2; // 2007.11.06
	//											break; // 2007.11.06
										}
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-c CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d,%d,%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , ICsts , ICsts2 );
										//==========================================================================================================
										if ( !_SCH_MODULE_Chk_MFIC_MULTI_FREE( ICsts , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) {
											//==========================================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-d CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d,%d,%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , ICsts , ICsts2 );
											//==========================================================================================================
											FM_Mode = 0; // 2007.11.06
											goto pickfrombmrun2; // 2007.11.06
	//											break; // 2007.11.06
										}
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-e CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d,%d,%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , ICsts , ICsts2 );
										//==========================================================================================================
									}
								}
								//==================================================================================================================
								FM_TSlot2 = 0;
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										Result = SCHEDULING_CHECK_BM_HAS_DualExt_for_Switch( FM_Buffer , CHECKING_SIDE , FM_TSlot , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) , _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) , &FM_Ret_Pointer );
									}
								}
							}
							else { // 2011.05.17
								//==================================================================================================================
								if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
									//
									if ( !SCHEDULER_COOLING_SKIP_AL0( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) ) ) { // 2013.03.14
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-a2 CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) );
										//==========================================================================================================
										ALsts = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
										if ( ALsts <= 0 ) {
											//==========================================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-b2 CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) );
											//==========================================================================================================
											FM_Mode = 0;
											goto pickfrombmrun2;
										}
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-c2 CHECK_GETOUT_PART3(%d,%d,TS=%d,PT=%d,%d,%d)\n" , FM_Buffer , Result , FM_TSlot , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot ) , ICsts , ICsts2 );
										//==========================================================================================================
									}
								}
								//==================================================================================================================
							}
						}
						//==========================================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 CHECK_GETOUT_PART4(%d,%d)\n" , FM_Buffer , Result );
						//==========================================================================================================
						// 2007.11.05
						//==========================================================================================================
						if ( Result > 0 ) {
							if ( FM_Buffer >= 0 ) {
								if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
									//===============================================================================================
//									if ( SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( FM_Buffer , FM_TSlot , &FM_TSlot2 ) == 2 ) { // 2018.06.18
									if ( SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( FM_Buffer , FM_TSlot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) , &FM_TSlot2 ) == 2 ) { // 2018.06.18
										FM_Buffer = -1;
										Result = -111;
										//==========================================================================================================
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09a CHECK_GETOUT_PART4-2(%d,%d)\n" , FM_Buffer , Result );
										//==========================================================================================================
										break;
									}
									//===============================================================================================
								}
								//==========================================================================================================
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09a CHECK_GETOUT_PART4(%d,%d)\n" , FM_Buffer , Result );
								//==========================================================================================================
							}
						}
					}
					//==========================================================================================================
//					if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( FM_Buffer , CHECKING_SIDE , &FM_CO1 , &FM_CO2 , 1 , CHECKORDER_FOR_FM , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) > 0 ) {
//						if ( Result > 0 ) { // 2007.02.28
					if ( ( FM_Mode != 4 ) && ( Result > 0 ) ) { // 2007.02.28
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( FM_Ret_Pointer ) { // 2006.02.08
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot2  = FM_TSlot;
									FM_Slot   = 0;
								}
								else {
									Result = -101;
								}
							}
							else {
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
									FM_Slot   = FM_TSlot;
									FM_Slot2  = FM_TSlot2;
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_Slot   = FM_TSlot;
									FM_Slot2  = 0;
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot2  = FM_TSlot;
									FM_Slot   = 0;
								}
								else { // 2008.09.10
									Result = -102;
								}
							}
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
								FM_Slot   = FM_TSlot;
								FM_Slot2  = 0;
							}
							else { // 2008.09.10
								Result = -103;
							}
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_Slot2  = FM_TSlot;
								FM_Slot   = 0;
							}
							else { // 2008.09.10
								Result = -104;
							}
						}
					}
					//==============================================================================================
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-4 CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result );
					//==============================================================================================
					//==============================================================================================================================
//						if ( Result > 0 ) { // 2007.02.28
					if ( ( FM_Mode == 4 ) || ( Result > 0 ) ) { // 2007.02.28
						if ( FM_Mode != 4 ) { // 2007.02.28
							//=================================================================================
							// 2006.09.07
							//=================================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
								//=================================================================================
								// 2007.01.05
								//=================================================================================
								if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) <= 1 ) {
										//===================================================================================================
//										SCHEDULER_CONTROL_Set_BM_Switch_UseSide( FM_Buffer , -1 ); // 2008.11.24
										//===================================================================================================
										SCHEDULER_CONTROL_Set_BM_Switch_Separate( 3 , 0 , 0 , 0 , 0 );
									}
								}
								else {
									if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) % 2 ) == 1 ) {
										do {
											if ( !Scheduler_Supply_Waiting_for_Batch_All( CHECKING_SIDE , TRUE , FM_Buffer ) ) {
												if ( Scheduler_Supply_Waiting_for_Batch_All( CHECKING_SIDE , FALSE , FM_Buffer ) ) {
													_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
													Sleep(1);
													_SCH_SYSTEM_ENTER_FEM_CRITICAL();
												}
												//
												if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) || ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) ) { // 2008.08.12
													break;
												}
												//
											}
											//
											if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) { // 2009.04.27
												if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) <= 2 ) _SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , FM_Buffer );
											}
											//
											_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
											Sleep(1);
											_SCH_SYSTEM_ENTER_FEM_CRITICAL();
											//==============================================================================================
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09-R CHECK_GETOUT_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result );
											//==============================================================================================
										}
										while( TRUE );
										//-----
									}
								}
								//=================================================================================
							}
							//===============================================================================================
							// 2007.02.28
							//===============================================================================================
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									if (
										( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) ) ) &&
										( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 ) ) )
										) { // 2013.03.06
										if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
											FM_Slot2 = 0;
											FM_Mode = 5;
										}
										else if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() == 1 ) {
											if ( !_SCH_MODULE_Chk_MFIC_Yes_for_Get( CHECKING_SIDE ) ) {
												FM_Slot2 = 0;
											}
										}
									}
									else if ( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) ) ) { // 2013.03.06
										if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
											FM_Slot = FM_Slot2;
											FM_Slot2 = 0;
										}
									}
									else if ( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 ) ) ) { // 2013.03.06
										if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
											FM_Slot2 = 0;
										}
									}
								}
								else if ( FM_Slot > 0 ) {
									if ( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) ) ) { // 2013.03.06
										if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
											FM_Mode = 5;
										}
									}
								}
								else if ( FM_Slot2 > 0 ) {
									if ( !SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 ) ) ) { // 2013.03.06
										if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
											FM_Mode = 6;
										}
									}
								}
							}
							else { // 2008.08.09
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
									if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
										//
										if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) ) == PATHDO_WAFER_RETURN ) {
											FM_CM = _SCH_CLUSTER_Get_PathIn( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) );
										}
										else {
											FM_CM = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) );
										}
										//
										switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) ) {
										case 1 :
											FM_Slot2 = 0;
											break;
										case 2 :
											FM_Slot2 = FM_Slot;
											FM_Slot  = 0;
											break;
										}
									}
								}
							}
							//-----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
							//----------------------------------------------
							//----------------------------------------------------------------------------
							//===============================================================================================
							// 2007.11.30
							//===============================================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.11.30
								if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) {
									if ( ( FM_Slot % 2 ) == 0 ) {
										if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot - 1 ) > 0 ) {
											if ( _SCH_MODULE_Get_BM_STATUS( FM_Buffer , FM_Slot - 1 ) == BUFFER_OUTWAIT_STATUS ) {
												FM_Slot2 = FM_Slot;
												FM_Slot--;
											}
										}
									}
								}
							}
							//===============================================================================================
							k = 0; // 2010.05.12
							if ( FM_Slot  > 0 ) {
								//----------------------------------------------------------------------
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) return 0; // 2013.06.28
								//----------------------------------------------------------------------
								FM_Side    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ); // 2007.11.05
								FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
								FM_TSlot   = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot );
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2010.05.12
									if (
										( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( FM_Side ) == BATCHALL_SWITCH_LL_2_LL ) ||
										( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( FM_Side ) == BATCHALL_SWITCH_LL_2_LP )
										) {
										if ( _SCH_MODULE_Get_BM_STATUS( FM_Buffer , FM_Slot ) == BUFFER_INWAIT_STATUS ) {
											k++;
											FM_TSlot = 0;
										}
									}
								}
								else { // 2013.05.27
									if ( !_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( FM_Side , FM_Pointer ) ) {
										k++;
										FM_TSlot = 0;
									}
								}
								//
							}
							else {
								FM_TSlot = 0;
								FM_Pointer = 0;
							}
							if ( FM_Slot2  > 0 ) {
								//----------------------------------------------------------------------
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) return 0; // 2013.06.28
								//----------------------------------------------------------------------
								FM_Side2    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 ); // 2007.11.05
								FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
								FM_TSlot2   = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot2 );
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2010.05.12
									if (
										( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( FM_Side2 ) == BATCHALL_SWITCH_LL_2_LL ) ||
										( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( FM_Side2 ) == BATCHALL_SWITCH_LL_2_LP )
										) {
										if ( _SCH_MODULE_Get_BM_STATUS( FM_Buffer , FM_Slot2 ) == BUFFER_INWAIT_STATUS ) {
											k++;
											FM_TSlot2 = 0;
										}
									}
								}
								else { // 2013.05.27
									if ( !_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( FM_Side2 , FM_Pointer2 ) ) {
										k++;
										FM_TSlot2 = 0;
									}
								}
								//
								//
							}
							else {
								FM_TSlot2 = 0;
								FM_Pointer2 = 0;
							}
							//==============================================================================================
							if ( k > 0 ) { // 2010.05.12
								if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) return 1;
							}
							//==============================================================================================
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 START_PICK_from_BM = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result );
							//==============================================================================================
							//----
							// Code for Pick from BO
							// FM_Slot
							//----------------------------------------------------------------------
							//=================================================================================================================================
							//
							k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 );
							if ( FM_Slot > 0 ) k = k - 1;
							if ( FM_Slot2 > 0 ) k = k - 1;
							//
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
//									FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2007.11.05
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2007.11.05
							}
							else if ( FM_Slot > 0 ) {
//									FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2007.11.05
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2007.11.05
							}
							else if ( FM_Slot2 > 0 ) {
//									FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ); // 2007.11.05
								FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ); // 2007.11.05
							}
							//----------------------------------------------------------------------------------------------------------------------------
							//----------------------------------------------------------------------------------------------------------------------------
							//----------------------------------------------------------------------------------------------------------------------------
							// 2003.10.07
							//----------------------------------------------------------------------------------------------------------------------------
							//----------------------------------------------------------------------------------------------------------------------------
							_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
							//----------------------------------------------------------------------------------------------------------------------------
							FM_TSlot_Real = 0; // 2006.07.24
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
//								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2006.02.07
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2006.02.07
									//======================================================================================================
									// 2006.07.24
									//======================================================================================================
									_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , FM_Slot , &FM_Slot_Real , &FM_TSlot_Real );
									//======================================================================================================
									_SCH_FMARMMULTI_DATA_SET_FROM_BM( 0 , FM_Buffer , FM_Slot_Real , FM_TSlot_Real , 0 ); // 2007.08.01
									//======================================================================================================
									_SCH_FMARMMULTI_FA_SUBSTRATE( FM_Buffer , FM_Slot_Real , FA_SUBST_RUNNING ); // 2007.07.11
									//======================================================================================================
								}
								else { // 2006.07.24
									FM_Slot_Real = FM_Slot;
								}
							}
							else { // 2006.07.24
								FM_Slot_Real = FM_Slot;
							}
							//----------------------------------------------------------------------------------------------------------------------------
							//----------------------------------------------------------------------------------------------------------------------------
							//==========================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0; // 2009.04.14
							//==========================================================================
							//----------------------------------------------------------------------------------------------------------------------------
							// 2009.04.24
							//----------------------------------------------------------------------------------------------------------------------------
							if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
								if ( FM_Slot > 0 ) {
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot ) <= 0 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								if ( FM_Slot2 > 0 ) {
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) <= 0 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
								}
							}
							//
							if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) { // 2009.04.27
								//----------------------------------------------------------------------------------------------------------------------------
								switch( _SCH_MACRO_FM_OPERATION( 0 , 600 + MACRO_PICK , FM_Buffer , FM_TSlot , ( FM_Slot_Real * 10000 ) + FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , ( k <= 0 ) ) ) { // 2007.03.21
								case -1 :
									return 0;
									break;
								}
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_TSlot , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FMWFR_PICK_BM_NEED_IC , FMWFR_PICK_BM_NEED_IC ); // 2007.11.26
								//----------------------------------------------------------------------
								// 2003.10.07
								//----------------------------------------------------------------------------------------------------------------------------
								_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( FM_Buffer , 0 , 11 ); // 2007.07.11
								//----------------------------------------------------------------------------------------------------------------------------
							}
							//----------------------------------------------------------------------------------------------------------------------------
							// 2009.04.24
							//----------------------------------------------------------------------------------------------------------------------------
							if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
								if ( FM_Slot > 0 ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								if ( FM_Slot2 > 0 ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
								}
							}
							//----------------------------------------------------------------------------------------------------------------------------
							// 2007.10.04
							//----------------------------------------------------------------------------------------------------------------------------
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
//								if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2013.10.01
								//
								if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2013.10.01
									//
									if ( ( FM_PLACE_LOCKEDBM >= BM1 ) && ( FM_PLACE_LOCKEDBM < TM ) && ( FM_Buffer != FM_PLACE_LOCKEDBM ) ) { // 2017.02.18 L.A.M 확인중
										plcslot = -1;
									}
									else {
										if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 , FM_Buffer ) > 0 ) { // 2017.02.18 L.A.M 확인중
											plcslot = -1;
											//
											FM_PLACE_LOCKEDBM = FM_Buffer; // 2017.02.18 L.A.M 확인중
											//
										}
										else {
											plcslot = 1;
										}
									}
									//
								}
								else { // 2013.10.01
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_FULLSWAP ) {
										plcslot = -1;
									}
									else {
										if ( FM_Slot > 0 ) {
											plcslot = FM_Slot;
										}
										else {
											plcslot = FM_Slot2;
										}
									}
								}
								//
								if ( plcslot != -1 ) { // 2013.10.01
									//
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , plcslot ) <= 0 ) {
										//
										FM_PLACE_LOCKEDBM = 0; // 2017.02.18 L.A.M 확인중
										//
										al_arm = -1;
										if      ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
											al_arm = TA_STATION;
										}
										else if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
											al_arm = TB_STATION;
										}
										//
										if ( al_arm != -1 ) {
											//
											ret_rcm = _SCH_MODULE_Get_FM_SIDE( al_arm );
											ret_pt  = _SCH_MODULE_Get_FM_POINTER( al_arm );
											al_arm2 = _SCH_MODULE_Get_FM_WAFER( al_arm );
											//
											//=============================================================================================
											// 2007.11.26
											//=============================================================================================
											if ( al_arm == TA_STATION ) {
												//
												if ( _SCH_MACRO_FM_OPERATION( 0 , 610 + MACRO_PLACE , FM_Buffer , al_arm2 , plcslot , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , 0 ) == -1 ) {
													return 0;
												}
												_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , al_arm2 , plcslot , ret_rcm , ret_pt , 0 , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
											}
											else {
												if ( _SCH_MACRO_FM_OPERATION( 0 , 620 + MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm2 , plcslot , ret_rcm , ret_pt , 0 ) == -1 ) {
													return 0;
												}
												_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , al_arm2 , plcslot , ret_rcm , ret_pt , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
											}
											//=============================================================================================
											_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , ret_rcm , ret_pt );
											//
											_SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 10002 , ret_rcm , ret_pt , FM_Buffer , 0 , 0 , 0 , 0 ); // 2017.02.01
											//
											OTHER_SIDE_PLACED_WAFER = TRUE; // 2011.06.28
										}
									}
								}
							}
							//----------------------------------------------------------------------------------------------------------------------------
							//
							if ( Dual_AL_Dual_FM_Second_Check == FM_Buffer ) { // 2014.03.05
								//
								Dual_AL_Dual_FM_Second_Check = 3;
								//
							}
							//
							//----------------------------------------------------------------------------------------------------------------------------
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2003.11.07
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_Buffer ) ) {
										//
										if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( FM_Buffer , G_PLACE , 0 ) ) { // 2018.06.01
											//=====================================================================
											middleswap_tag = 0;
											//=====================================================================
											//
											SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer );
											//
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 833 );
										}
										else {
											if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) == FM_Buffer ) { // 2013.05.03
												//================================================================================================
												middleswap_tag = 0;
												//=====================================================================
												//
												SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
												//
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 813 );
												//================================================================================================
											}
											else {
												if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) || ( SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) ) { // 2008.01.09
													if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2005.01.19
														if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.03.22 // 2007.05.03
															//
															if ( !SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , FALSE ) ) { // 2014.03.06
																//
																if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																	//=====================================================================
																	middleswap_tag = 0; // 2006.02.22
																	//=====================================================================
																	//
																	SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																	//
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 10 );
																	//=====================================================================
																}
																else {
																	if ( !SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , FALSE , TRUE , FM_Buffer ) ) { // 2004.03.22
																		//=====================================================================
																		middleswap_tag = 0; // 2006.02.22
																		//=====================================================================
																		//
																		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																		//
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 11 );
																		//=====================================================================
																	}
																	else { // 2006.12.22
																		//========================================================================================
																		if ( !SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , TRUE , FALSE , FM_Buffer ) ) {
																			//=====================================================================
																			middleswap_tag = 0;
																			//=====================================================================
																			//
																			SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																			//
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 92 );
																			//=====================================================================
																		}
																		//========================================================================================
																	}
																}
															}
															//
														}
														else {
															//================================================================================================
															// 2007.05.21
															//================================================================================================
	//														if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( CHECKING_SIDE ) > 0 ) { // 2007.05.21 // 2014.06.10
															if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( CHECKING_SIDE , FM_Buffer ) > 0 ) { // 2007.05.21 // 2014.06.10
																//=====================================================================
																middleswap_tag = 0;
																//=====================================================================
																//
																SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																//
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 802 );
															}
															//================================================================================================
														}
													}
												}
												else {
													//================================================================================================
													// 2007.05.21
													//================================================================================================
	//												if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( CHECKING_SIDE ) > 0 ) { // 2007.05.21 // 2014.06.10
													if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( CHECKING_SIDE , FM_Buffer ) > 0 ) { // 2007.05.21 // 2014.06.10
														//=====================================================================
														middleswap_tag = 0;
														//=====================================================================
														//
														SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 803 );
													}
													//================================================================================================
												}
											}
										}
									}
									else { // 2006.02.22
										if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( FM_Buffer ) ) {
												//=====================================================================
												middleswap_tag = 0; // 2006.02.22
												//=====================================================================
												//=====================================================================
												// 2007.05.03
												//=====================================================================
												switch ( SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_MiddleSwap( CHECKING_SIDE , FM_Buffer , TRUE ) ) {
												case -1 :
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 307 from FEM%cWHFMFAIL 307\n" , 9 );
													return 0;
													break;
												case 0 :
													FM_TSlot = 0;
													FM_TSlot2 = 0;
													break;
												}
												//=====================================================================
												//
												SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
												//
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 11 );
												//=====================================================================
											}
										}
										else { // 2007.11.27
											//================================================================================================================
											// 2007.11.27
											//================================================================================================================
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2007.11.27
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_INWAIT_STATUS ) > 0 ) { // 2007.11.27
//													if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( FM_Buffer ) <= 0 ) { // 2013.10.01 // 2014.11.25
													if ( ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( FM_Buffer ) <= 0 ) || Get_RunPrm_FM_PM_CONTROL_USE() ) { // 2013.10.01 // 2014.11.25
														//=====================================================================
														middleswap_tag = 0;
														//=====================================================================
														//=====================================================================
														//
														SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 1103 );
														//=====================================================================
														_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
													}
													else {
														if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) || ( SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) ) { // 2008.01.09
															if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.03.22 // 2007.05.03
																//
																if ( !SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , FALSE ) ) { // 2014.03.06
																	//
																	//=====================================================================
																	middleswap_tag = 0;
																	//=====================================================================
																	//=====================================================================
																	//
																	SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																	//
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 1102 );
																	//=====================================================================
																	_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
																	//
																}
															}
														}
													}
												}
											}
											//================================================================================================================
										}
									}
								}
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.07
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									//==================================================================================
									// 2007.10.04
									//==================================================================================
									if ( _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) == 1 ) { // 2007.10.04
										if ( ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , 1 ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( FM_Buffer , 1 ) == BUFFER_INWAIT_STATUS ) ) {
											//=====================================================================
											middleswap_tag = 0;
											//=====================================================================
											//
											SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
											//
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 9203 );
											//=====================================================================
										}
										else {
											if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
												if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2005.01.19
													if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.03.22 // 2007.05.03
														//
														if ( !SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( FM_Buffer , FALSE ) ) { // 2014.03.06
															//
															if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																//=====================================================================
																middleswap_tag = 0; // 2006.02.22
																//=====================================================================
																//
																SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																//
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 10 );
																//=====================================================================
															}
															else {
																if ( !SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , FALSE , TRUE , FM_Buffer ) ) { // 2004.03.22
																	//=====================================================================
																	middleswap_tag = 0; // 2006.02.22
																	//=====================================================================
																	//
																	SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																	//
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 11 );
																	//=====================================================================
																}
																else { // 2006.12.22
																	//========================================================================================
																	if ( !SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , TRUE , FALSE , FM_Buffer ) ) {
																		//=====================================================================
																		middleswap_tag = 0;
																		//=====================================================================
																		//
																		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																		//
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 92 );
																		//=====================================================================
																	}
																	else {
																		//================================================================================================
																		// 2009.10.06
																		//================================================================================================
//																		if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 ) > 0 ) { // 2014.06.10
																		if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 , FM_Buffer ) > 0 ) { // 2014.06.10
																			//=====================================================================
																			middleswap_tag = 0;
																			//=====================================================================
																			//
																			SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																			//
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 805 );
																		}
	//																	else {
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TMSIDE SKIP 1\n" );
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
	//																		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
	//																	}
																		//================================================================================================
																	}
																	//========================================================================================
																}
															}
														}
													}
													else {
														//================================================================================================
														// 2007.05.21
														//================================================================================================
//														if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( CHECKING_SIDE ) > 0 ) { // 2007.05.21 // 2009.10.06
//														if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 ) > 0 ) { // 2007.05.21 // 2009.10.06 // 2014.06.10
														if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 , FM_Buffer ) > 0 ) { // 2007.05.21 // 2009.10.06 // 2014.06.10
															//=====================================================================
															middleswap_tag = 0;
															//=====================================================================
															//
															SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
															//
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 802 );
														}
//														else {
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TMSIDE SKIP 2\n" );
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//														}
														//================================================================================================
													}
												}
												else {
													//================================================================================================
													// 2009.10.06
													//================================================================================================
//													if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 ) > 0 ) { // 2014.06.10
													if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 , FM_Buffer ) > 0 ) { // 2014.06.10
														//=====================================================================
														middleswap_tag = 0;
														//=====================================================================
														//
														SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 804 );
													}
//													else {
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TMSIDE SKIP 3\n" );
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//													}
													//================================================================================================
												}
											}
											else {
												//================================================================================================
												// 2009.10.06
												//================================================================================================
//												if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 ) > 0 ) { // 2014.06.10
												if ( SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 , FM_Buffer ) > 0 ) { // 2014.06.10
													//=====================================================================
													middleswap_tag = 0;
													//=====================================================================
													//
													SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
													//
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 803 );
												}
//												else {
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TMSIDE SKIP 4 [%d]\n" , SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( -1 ) );
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "=====================================================================\n" );
//												}
												//================================================================================================
											}
										}
									}
									//==================================================================================
									else {
										if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( -1 , FM_Buffer , &Result ) ) { // 2004.01.27
											if ( Result ) { // 2004.01.29
												//=====================================================================
												//
												SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
												//
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 12 );
												//=====================================================================
											}
											else {
												if ( !SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( CHECKING_SIDE , FM_Buffer ) ) { // 2004.11.29
													//
//													if ( Scheduler_No_More_Supply_Check( -1 ) ) { // 2017.10.24 // 2017.10.25
													if ( Scheduler_No_More_Supply_and_all_Fmside() ) { // 2017.10.24 // 2017.10.25
														//
														if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
															//=====================================================================
															//
															SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
															//
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 13 );
															//=====================================================================
														}
														else {
															if ( !SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , FALSE , TRUE , FM_Buffer ) ) {
																//=====================================================================
																//
																SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
																//
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 14 );
																//=====================================================================
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
							else { // 2006.03.21
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_Buffer ) ) {
									for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
										if ( k != CHECKING_SIDE ) {
//											if ( ( _SCH_SYSTEM_USING_GET( k ) > 0 ) && ( _SCH_SYSTEM_USING2_GET( k ) > 2 ) ) { // 2009.03.05
											if ( _SCH_SYSTEM_USING_RUNNING( k ) ) { // 2009.03.05
												if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( k ) == FM_Buffer ) {
													//=====================================================================
													//
													SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , FM_Buffer ); // 2017.02.08
													//
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 14 );
													//=====================================================================
													break;
												}
											}
										}
									}
									if ( k != MAX_CASSETTE_SIDE ) {
									}
								}
							}
						}
						//----------------------------------------------------------------------------------------------------------------------------
						//----------------------------------------------------------------------------------------------------------------------------
						//----------------------------------------------------------------------
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 6 from FEM%cWHFMFAIL 6\n" , 9 );
							return 0;
						}
						//----------------------------------------------------------------------------------------------------------------------------
						// 2009.05.21
						//----------------------------------------------------------------------------------------------------------------------------
						_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 );
						//----------------------------------------------------------------------------------------------------------------------------
						//-----------------------------------------------
						_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
						//----------------------------------------------
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 START_PICK_from_BM = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d FM_PLACE_LOCKEDBM=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result , FM_PLACE_LOCKEDBM );
						//==============================================================================================
						//----------------------------------------------------------------------
//							if ( ( FM_Mode == 4 )( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) { // 2007.05.03
						if ( ( FM_Mode == 4 ) || ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) ) { // 2007.05.03 // 2007.08.03
							//----------------------------------------------------------------------
							//
							// Code Cooling Before Place to CM
							//
							//----------------------------------------------------------------------
							coolplace = 0;
							//
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2008.02.20
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
									if ( SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) coolplace = 2;
								}
								//
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
									if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
									if ( SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) coolplace = 2;
								}
							}
							else {
								if ( FM_Mode != 4 ) coolplace = 1;
							}
							//
							if ( coolplace != 0 ) {
								if ( coolplace == 1 ) { // 2008.02.20
									//----------------------------------------------------------------------------------------------------------------------------
									FM_Mode4_Action = TRUE; // 2013.05.29
									//----------------------------------------------------------------------------------------------------------------------------
//									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , IC , FM_TSlot , FM_TSlot2 , FM_CM , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2015.07.28
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , IC , FM_Slot , FM_Slot2 , FM_CM , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , TRUE , FM_TSlot , FM_TSlot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2015.07.28
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" ,  FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
										return 0;
									}
								}
								//=========================================================================================================
								if ( FM_TSlot  > 0 ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_BM_DONE_IC );	// 2007.09.11
								if ( FM_TSlot2 > 0 ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_BM_DONE_IC );	// 2007.09.11
								//=========================================================================================================
								FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2007.11.05
								FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2007.11.05
							}
							else {
								//========================================================================================================================
								// 2007.02.28
								//========================================================================================================================
								// Place to MFIC
								if ( FM_Mode == 2 ) {
									//==================================================================================================================
									if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
										FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_Pointer2  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									}
									else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
										FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_Side2 = -1;
									}
									else {
										FM_Side = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										FM_Side2 = -1;
									}
									//==================================================================================================================
									ALsts = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( FM_Side , FM_Pointer , FM_Side2 , FM_Pointer2 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
									if ( ALsts <= 0 ) {
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail 2 at IC%cWHFMICFAIL%c\n" , 9 , 9 );
										return 0;
									}
									//==================================================================================================================
									// 2007.08.02
									//==================================================================================================================
									if ( !_SCH_MODULE_Chk_MFIC_MULTI_FREE( ICsts , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) {
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail 2-1 at IC%cWHFMICFAIL%c\n" , 9 , 9 );
										return 0;
									}
									//==================================================================================================================
									if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
										if ( ALsts >= 2 ) {
											ICfinger = 0;
										}
										else {
											ICfinger = 1;
											FM_Mode = 6;
										}
									}
									else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
										ICfinger = 1;
									}
									else {
										ICfinger = 2;
									}
									//==================================================================================================================
									if ( ICfinger == 0 ) {
										FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										//
										FM_Side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									}
									else if ( ICfinger == 1 ) {
										FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_TSlot2   = 0;
									}
									else if ( ICfinger == 2 ) {
										FM_Side     = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										FM_TSlot2   = 0;
									}
									FM_CM = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
									//==================================================================================================================
									//----------------------------------------------------------------------------------------------------------------------------
									FM_Mode4_Action = TRUE; // 2013.05.29
									//----------------------------------------------------------------------------------------------------------------------------
									if ( ICfinger == 0 ) {
										//
										/*
										//
										//
										// 2018.04.04
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , CHECKING_SIDE , IC , ICsts , ICsts2 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
											return 0;
										}
										//========================================================================================
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , FM_TSlot , ICsts , FM_Side , FM_Pointer , IC , FM_TSlot2 , ICsts2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
										//----------------------------------------------------------------------
										//============================================================================
										_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ICsts , 0 , FM_TSlot , 0 , FM_CM );
										//============================================================================
										_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts ); // 2007.07.11
										_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts ); // 2007.08.02
										//============================================================================
										_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side2 , ICsts2 , 0 , FM_TSlot2 , 0 , FM_CM );
										//============================================================================
										//
										*/
										//
										//
										//
										// 2018.04.04
										//
										//if ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() >= 0 ) { // 2018.09.28
										if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 10 ) == 0 ) && ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() >= 0 ) ) { // 2018.09.28
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , ICsts , 0 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , FM_Side , FM_Side , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
												return 0;
											}
											//----------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , FM_TSlot , ICsts , FM_Side , FM_Pointer , IC , 0 , ICsts , FM_Side , FM_Pointer , -1 , -1 );
											//----------------------------------------------------------------------
											//============================================================================
											_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ICsts , 0 , FM_TSlot , 0 , FM_CM );
											//============================================================================
											_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts );
											_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts );
											//============================================================================
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , 0 , ICsts2 , FM_CM , FM_CM , TRUE , FM_TSlot2 , FM_TSlot2 , FM_Side2 , FM_Side2 , FM_Pointer2 , FM_Pointer2 ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot2 , 9 , FM_TSlot2 , 9 , FM_TSlot2 );
												return 0;
											}
											//----------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , 0 , ICsts2 , FM_Side2 , FM_Pointer2 , IC , FM_TSlot2 , ICsts2 , FM_Side2 , FM_Pointer2 , -1 , -1 );
											//----------------------------------------------------------------------
											//============================================================================
											_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side2 , ICsts2 , 0 , FM_TSlot2 , 0 , FM_CM );
											//============================================================================
											//
										}
										else {
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , CHECKING_SIDE , IC , ICsts , ICsts2 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
												return 0;
											}
											//========================================================================================
											//----------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , FM_TSlot , ICsts , FM_Side , FM_Pointer , IC , FM_TSlot2 , ICsts2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
											//----------------------------------------------------------------------
											//============================================================================
											_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ICsts , 0 , FM_TSlot , 0 , FM_CM );
											//============================================================================
											_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts ); // 2007.07.11
											_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts ); // 2007.08.02
											//============================================================================
											_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side2 , ICsts2 , 0 , FM_TSlot2 , 0 , FM_CM );
											//============================================================================
											//
										}
										//
									}
									else {
										//==================================================================================================================
										// 2007.10.15
										//==================================================================================================================
										if ( ICfinger == 1 ) {
											_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , ICsts , &ICsts_Real , &k );
										}
										else {
											ICsts_Real = ICsts;
										}
										//======================================================================================================
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_Side , IC , ( ICfinger == 1 ) ? ICsts_Real : 0 , ( ICfinger == 1 ) ? 0 : ICsts_Real , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , FM_Side , FM_Side , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
											return 0;
										}
										//========================================================================================
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , ( ICfinger == 1 ) ? FM_TSlot : 0 , ICsts , FM_Side , FM_Pointer , IC , ( ICfinger == 1 ) ? 0 : FM_TSlot , ICsts , FM_Side , FM_Pointer , -1 , -1 ); // 2007.11.26
										//----------------------------------------------------------------------
										//============================================================================
										_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ( ICfinger == 1 ) ? ICsts : 0 , ( ICfinger == 1 ) ? 0 : ICsts , ( ICfinger == 1 ) ? FM_TSlot : 0 , ( ICfinger == 1 ) ? 0 : FM_TSlot , FM_CM );
										//============================================================================
										if ( ICfinger == 1 ) {
											_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts ); // 2007.07.11
											_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts ); // 2007.08.02
										}
										//============================================================================
									}
									//==================================================================================================================
								}
								//==============================================================================================================================
								// 2007.02.28
								//==============================================================================================================================
								// Pick from MFIC
								FM_TSlot_Real = 0;
								//
								FM_Side = CHECKING_SIDE;
								ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( FM_Side , &FM_Side2 , &ICsts , &ICsts2 );
								//
//								_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , ICsts , &ICsts_Real , &FM_TSlot_Real ); // 2007.11.06 // 2008.04.01
								//==============================================================================================================================
								// 2007.08.02
								//==============================================================================================================================
								if ( ALsts == SYS_SUCCESS ) {
									if ( !_SCH_MODULE_Chk_MFIC_MULTI_FINISH( ICsts , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) ALsts = SYS_RUNNING;
								}
								//==============================================================================================================================
								// 2008.04.01
								//==============================================================================================================================
								if ( ALsts == SYS_RUNNING ) {
									//
									if ( mfic_locked == 2 ) {
										//
										if ( ( FM_Mode == 5 ) || ( FM_Mode == 6 ) ) {
											//
											k = 0; // 2008.06.04
											//
											while ( TRUE ) {
												//
												if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) { // 2008.06.04
													//-----------------------------------------------
													return 0;
												}
												//
//												ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( FM_Side , &FM_Side2 , &ICsts , &ICsts2 ); // 2008.06.04
												ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( ( k > 50 ) ? -1 : FM_Side , &FM_Side2 , &ICsts , &ICsts2 ); // 2008.06.04
												//
												if ( ALsts != SYS_RUNNING ) break;
												//
												Sleep(100);
												//==============================================================================================
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 99 CHECK_LOOP_PART = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result );
												//==============================================================================================
												k++; // 2008.06.04
											}
										}
									}
								}
								//==============================================================================================================================
								_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , ICsts , &ICsts_Real , &FM_TSlot_Real ); // 2007.11.06 // 2008.04.01
								//==============================================================================================================================
								if ( ALsts == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
									return 0;
								}
								else if ( ALsts == SYS_SUCCESS ) {
									//----------------------------------------------------------------------------------------------------------------------------
									FM_Mode4_Action = TRUE; // 2013.05.29
									//----------------------------------------------------------------------------------------------------------------------------
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( ICsts > 0 ) && ( ICsts2 > 0 ) ) {
										//
										FM_Side  = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
										FM_Side2 = _SCH_MODULE_Get_MFIC_SIDE( ICsts2 ); // 2007.11.06
										//
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Success at IC(%d:%d)%cWHFMICSUCCESS %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										FM_CM = _SCH_CLUSTER_Get_PathOut( FM_Side , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) );
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , FM_Side , IC , ICsts , ICsts2 , FM_CM , -1 , TRUE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts2 ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
											return 0;
										}
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , ICsts , FM_Side , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , 0 , 0 , 0 , 0 , 0 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC ); // 2007.11.26
										//----------------------------------------------------------------------
										//====================================================================================
										_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts , 0 );
										//====================================================================================
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , IC , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , ICsts2 , FM_Side2 , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC ); // 2007.11.26
										//----------------------------------------------------------------------
										//====================================================================================
										FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										//=========================================================================================================
									}
									//===============================================================================================
									else {
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
												ICfinger = TA_STATION;
											}
											else {
												ICfinger = TB_STATION;
											}
										}
										else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
											ICfinger = TA_STATION;
										}
										else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											ICfinger = TB_STATION;
										}
										if ( ICfinger == TA_STATION ) {
											//
											FM_Side     = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
											FM_TSlot	= _SCH_MODULE_Get_MFIC_WAFER( ICsts );
											FM_Pointer	= _SCH_MODULE_Get_MFIC_POINTER( ICsts );
											//
											FM_Pointer2	= 0;
											FM_TSlot2	= 0;
											//
											FM_CM		= _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
											//====================================================================================
											_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts_Real , FM_TSlot_Real ); // 2007.10.24
											//====================================================================================
										}
										else {
											FM_Side2    = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
											FM_TSlot2	= _SCH_MODULE_Get_MFIC_WAFER( ICsts );
											FM_Pointer2	= _SCH_MODULE_Get_MFIC_POINTER( ICsts );
											//
											FM_Pointer	= 0;
											FM_TSlot	= 0;
											FM_CM		= _SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 );
										}
										//==================================================================================================================
										// 2007.10.15 // 2007.10.24
										//==================================================================================================================
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , ( FM_TSlot > 0 ? FM_Side : FM_Side2 ) , IC , ( FM_TSlot > 0 ? ICsts_Real : 0 ) , ( FM_TSlot2 > 0 ? ICsts_Real : 0 ) , FM_CM , -1 , TRUE , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_SIDE( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_SIDE( ICsts ) : 0 ) , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( ( FM_TSlot > 0 ? FM_Side : FM_Side2 ) , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
											return 0;
										}
										//========================================================================================
//xxx_check_printf( CHECKING_SIDE , TRUE , "Pick from IC Normal" , 2 );
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( ICfinger == TA_STATION ) ? FM_TSlot : 0 , ICsts , FM_Side , FM_Pointer , IC , ( ICfinger == TA_STATION ) ? 0 : FM_TSlot2 , ICsts , FM_Side2 , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC , FMWFR_PICK_BM_DONE_IC ); // 2007.11.26
										//----------------------------------------------------------------------
									}
								}
								else {
									if ( ( FM_Mode == 5 ) || ( FM_Mode == 6 ) ) {
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail 3 at IC%cWHFMICFAIL%c\n" , 9 , 9 );
										return 0;
									}
									FM_TSlot = 0;
									FM_TSlot2 = 0;
								}
								//==============================================================================================================================
								// Place to MFIC
								if ( ( FM_Mode == 5 ) || ( FM_Mode == 6 ) ) {
									if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( _SCH_MODULE_Get_FM_SIDE( ( FM_Mode == 5 ) ? TA_STATION : TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( ( FM_Mode == 5 ) ? TA_STATION : TB_STATION ) , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) >= 1 ) {
										//----------------------------------------------------------------------------------------------------------------------------
										FM_Mode4_Action = TRUE; // 2013.05.29
										//----------------------------------------------------------------------------------------------------------------------------
										ICfinger = _SCH_MODULE_Get_FM_SIDE( ( FM_Mode == 5 ) ? TA_STATION : TB_STATION );
										ICsts2   = _SCH_MODULE_Get_FM_WAFER( ( FM_Mode == 5 ) ? TA_STATION : TB_STATION );
										FM_CM    = _SCH_CLUSTER_Get_PathOut( ICfinger , _SCH_MODULE_Get_FM_POINTER( ( FM_Mode == 5 ) ? TA_STATION : TB_STATION ) );
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , ICfinger , IC , ( FM_Mode == 5 ) ? ICsts : 0 , ( FM_Mode == 5 ) ? 0 : ICsts , FM_CM , FM_CM , TRUE , ( FM_Mode == 5 ) ? ICsts2 : 0 , ( FM_Mode == 5 ) ? 0 : ICsts2 , ( FM_Mode == 5 ) ? _SCH_MODULE_Get_FM_SIDE( TA_STATION ) : 0 , ( FM_Mode == 5 ) ? 0 : _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , ( FM_Mode == 5 ) ? _SCH_MODULE_Get_FM_POINTER( TA_STATION ) : 0 , ( FM_Mode == 5 ) ? 0 : _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( ICfinger , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , ICsts2 , 9 , ICsts2 , 9 , ICsts2 );
											return 0;
										}
										//========================================================================================
										if ( FM_Mode == 5 ) {
											//----------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , ICsts2 , ICsts , ICfinger , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , 0 , 0 , 0 , 0 , -1 , -1 ); // 2007.11.26
											//----------------------------------------------------------------------
										}
										else {
											//----------------------------------------------------------------------
											_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , IC , ICsts2 , ICsts , ICfinger , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , -1 , -1 ); // 2007.11.26
											//----------------------------------------------------------------------
										}
										//============================================================================
//										_SCH_MACRO_SPAWN_FM_MCOOLING( ICfinger , ( FM_Mode == 5 ) ? ICsts : 0 , ( FM_Mode == 5 ) ? 0 : ICsts , ICsts2 , 0 , FM_CM ); // 2011.06.28
										_SCH_MACRO_SPAWN_FM_MCOOLING( ICfinger , ( FM_Mode == 5 ) ? ICsts : 0 , ( FM_Mode == 5 ) ? 0 : ICsts , ( FM_Mode == 5 ) ? ICsts2 : 0 , ( FM_Mode == 5 ) ? 0 : ICsts2 , FM_CM ); // 2011.06.28
										//============================================================================
									}
									else {
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail 4 at IC%cWHFMICFAIL%c\n" , 9 , 9 );
										return 0;
									}
								}
							}
						}
						//----------------------------------------------------------------------
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 7 from FEM%cWHFMFAIL 7\n" , 9 );
							return 0;
						}
						//----------------------------------------------------------------------------------------------------------------------------
						// 2009.05.21
						//----------------------------------------------------------------------------------------------------------------------------
						if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( FM_TSlot > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_TSlot = 0;
								}
							}
							if ( FM_TSlot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_TSlot2 = 0;
								}
							}
						}
						//----------------------------------------------------------------------------------------------------------------------------
						if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) { // 2007.02.28
							//
							if ( FM_TSlot > 0 ) { // 2008.11.13
								//
								if ( wfrreturn_mode[FM_Side] != 0 ) {
									//
									_SCH_SYSTEM_MODE_END_SET( FM_Side , wfrreturn_mode[FM_Side] );
									//
									_SCH_MODULE_Set_TM_DoPointer( FM_Side , 0 );
									_SCH_MODULE_Set_FM_DoPointer( FM_Side , 0 );
								}
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2009.01.09
									if ( ( _SCH_SYSTEM_MODE_END_GET( FM_Side ) == 2 ) || ( _SCH_SYSTEM_MODE_END_GET( FM_Side ) == 4 ) || ( _SCH_SYSTEM_MODE_END_GET( FM_Side ) == 6 ) ) {
										if ( _SCH_CASSETTE_LAST_RESULT2_GET( FM_Side,FM_Pointer ) < 0 ) { // 2009.01.19
											_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_Side , FM_Pointer , -2 );
										}
										else {
											_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_Side , FM_Pointer , 0 );
										}
									}
								}
								//
							}
							//
							if ( FM_TSlot2 > 0 ) { // 2008.11.13
								//
								if ( wfrreturn_mode[FM_Side2] != 0 ) {
									//
									_SCH_SYSTEM_MODE_END_SET( FM_Side2 , wfrreturn_mode[FM_Side2] );
									//
									_SCH_MODULE_Set_TM_DoPointer( FM_Side2 , 0 );
									_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , 0 );
								}
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2009.01.09
									if ( ( _SCH_SYSTEM_MODE_END_GET( FM_Side2 ) == 2 ) || ( _SCH_SYSTEM_MODE_END_GET( FM_Side2 ) == 4 ) || ( _SCH_SYSTEM_MODE_END_GET( FM_Side2 ) == 6 ) ) {
										if ( _SCH_CASSETTE_LAST_RESULT2_GET( FM_Side2,FM_Pointer2 ) < 0 ) { // 2009.01.19
											_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_Side2 , FM_Pointer2 , -2 );
										}
										else {
											_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_Side2 , FM_Pointer2 , 0 );
										}
									}
								}
								//
							}
							//
							//----------------------------------------------------------------------
							//
							// Code for Place to CM
							// FM_Slot
							//----
							//----------------------------------------------------------------------------------------------------------------------------
							FM_Mode4_Action = TRUE; // 2013.05.29
							//----------------------------------------------------------------------------------------------------------------------------
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
//							Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FALSE , FALSE , -FM_TSlot_Real ); // 2007.07.11 // 2009.04.24
							Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FALSE , TRUE , -FM_TSlot_Real ); // 2007.07.11 // 2009.04.24
							if      ( Result == -1 ) { // Abort
								return 0;
							}
							else if ( Result != 0 ) { // disappear
							}
							//----------------------------------------------------------------------
							//
							if ( FM_TSlot > 0 ) { // 2008.11.13
								//
								if ( wfrreturn_mode[FM_Side] != 0 ) {
									//
									if ( _SCH_MODULE_Chk_FM_Free_Status( FM_Side ) ) {
										//
										_SCH_SYSTEM_MODE_END_SET( FM_Side , wfrreturn_mode[FM_Side] );
										//
										_SCH_MODULE_Set_TM_DoPointer( FM_Side , 0 );
										_SCH_MODULE_Set_FM_DoPointer( FM_Side , 0 );
										//
										wfrreturn_mode[FM_Side] = 0;
									}
								}
                                // 2009.04.02 jmlee
                                if ( _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
                                        _SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , FM_Side , FM_Pointer , 4 );
                                }
                                //
							}
							//
							if ( FM_TSlot2 > 0 ) { // 2008.11.13
								//
								if ( wfrreturn_mode[FM_Side2] != 0 ) {
									//
									if ( _SCH_MODULE_Chk_FM_Free_Status( FM_Side2 ) ) {
										//
										_SCH_SYSTEM_MODE_END_SET( FM_Side2 , wfrreturn_mode[FM_Side2] );
										//
										_SCH_MODULE_Set_TM_DoPointer( FM_Side2 , 0 );
										_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , 0 );
										//
										wfrreturn_mode[FM_Side2] = 0;
										//
									}
								}
                                // 2009.04.02 jmlee
                                if ( _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
                                        _SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , FM_Side2 , FM_Pointer2 , 4 );
                                }
								//
							}
							//
						}
						//=================================================================================
						//=================================================================================
						//==============================================================================================
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13 START_PLACE_to_CM = FM_Mode=%d OC=%d,%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d Result=%d middleswap_tag=%d\n" , FM_Mode , FM_CO1 , FM_CO2 , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , Result , middleswap_tag );
						//==============================================================================================
						if ( FM_Mode == 4 ) break; // 2007.02.28
						//==========================================================================================================
						// 2007.03.12
						//==========================================================================================================
						if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() == 3 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) { // 2007.03.12
							FM_Mode = 1;
							FM_Buffer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE);
							SCHEDULER_CONTROL_Set_BM_Switch_Separate( 0 , 0 , 0 , 0 , 0 );
							goto pickfrombmrun;
						}
						//=================================================================================
						// 2007.02.02
						//=================================================================================
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
							if ( middleswap_tag == 0 ) {
								if ( SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) {
									if ( !SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , Sch_NoWay_Option() ) ) {
										if ( 0 == SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( FM_Buffer , 0 ) ) {
											if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( CHECKING_SIDE , FM_Buffer ) ) break; // 2008.06.11
//											break; // 2008.06.11
										}
									}
								}
							}
						}
						//=================================================================================
						// 2007.11.23
						//=================================================================================
						else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , FALSE , FALSE ) ) {
								if ( !SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) {
									if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( CHECKING_SIDE , FM_Buffer ) ) break; // 2008.01.04
								}
								//
								for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
									if ( dmi != CHECKING_SIDE ) {
										if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
											if ( SCHEDULER_CONTROL_Chk_FM_Double_Pick_Possible( dmi , k ) ) { // 2008.01.04
												break;
											}
										}
									}
								}
								if ( dmi != MAX_CASSETTE_SIDE ) break;
								//
							}
							else { // 2013.10.01
								if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , FALSE , TRUE ) ) {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
												break;
											}
										}
									}
									//
								}
							}
						}
						//=================================================================================
						// 2006.02.22
						//=================================================================================
						if ( middleswap_tag == 2 ) break; // 2006.02.22
						//=================================================================================
						Sleep(1);
					}
					else {
						//
						if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 2 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) ) { // 2008.11.13
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
								Sleep(1);
								break;
							}
						}
						//
//						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_Buffer ) ) { // 2008.10.30 // 2008.11.24
//							SCHEDULER_CONTROL_Set_BM_Switch_UseSide( FM_Buffer , -1 ); // 2008.11.24
//						} // 2008.11.24
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07
							if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 FINISH_CHECK_2 = TRUE\n" );
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 2 for FEM Module%cWHFMSUCCESS 2\n" , 9 );
								return SYS_SUCCESS;
							}
						}
						else {
							// 2004.03.25
							if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 FINISH_CHECK_2 = TRUE\n" );
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 4 for FEM Module%cWHFMSUCCESS 4\n" , 9 );
								return SYS_SUCCESS;
							}
						}
						Sleep(1);
						break; // Append 2002.01.02
					}
				} // (while)
				//=======================================================================================================
				// 2006.02.22
				//=======================================================================================================
				if ( middleswap_tag == 2 ) {
					if ( !SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) {
						middleswap_tag = 0;
						FM_Mode = 1;
						goto middlererun;
					}
				}
				//=======================================================================================================
			} // 2004.02.24
			else { // 2004.02.24
				Sleep(1); // 2004.02.24
			} // 2004.02.24
		}
	}

	//
	if ( DUAL_AL_FM_PICK_NOWAIT ) { // 2014.03.11
		//
		if ( ( Dual_AL_Dual_FM_Second_Check == 2 ) || ( Dual_AL_Dual_FM_Second_Check == 3 ) ) goto Dual_AL_Dual_FM_Second_Check_Position; // 2014.03.04
		//
		if ( ( FM_Mode == 2 ) && ( Dual_AL_Dual_FM_Check == 1 ) ) { // 204.03.06
			//
			for ( dmi = 0 ; dmi < MAX_CASSETTE_SIDE ; dmi++ ) {
				if ( !SCHEDULER_FM_Current_No_More_Supply( dmi ) ) {
					if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( dmi , -1 , TRUE ) ) {
						break;
					}
				}
			}
			//
			if ( dmi == MAX_CASSETTE_SIDE ) {
				if ( Dual_AL_Dual_FM_Second_Check < 2 ) goto Dual_AL_Dual_FM_Second_Check_Position;
			}
			//
		}
		//
	}
	else {
		//
		if ( ( Dual_AL_Dual_FM_Second_Check == 2 ) || ( Dual_AL_Dual_FM_Second_Check == 3 ) ) Dual_AL_Dual_FM_Second_Check = 0; // 2014.03.04
		//
	}
	//
lastcheck :	// 2010.01.19

	//========================================================================================================
	// 2007.03.27
	//========================================================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2007.11.29
		if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) {
			if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
				if ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) {
					if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) { // 2007.03.28
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							//
							if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
							//
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2007.11.29 // 2009.09.28
							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.11.29 // 2009.09.28
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) > 0 ) {
												if ( _SCH_MODULE_Get_BM_SIDE( k , 1 ) == CHECKING_SIDE ) {
													//=====================================================================
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 211 );
													//=====================================================================
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
	//========================================================================================================


	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10-1 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
	//==============================================================================================
	//========================================================================================================
	// 2009.03.23
	//========================================================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
		if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
			if ( !_SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
					if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) == 999999 ) {
						dmi = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE );
						if ( dmi != 0 ) {
							if ( ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != dmi ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( dmi ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( dmi ) == BUFFER_FM_STATION ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( dmi ) <= 0 ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( dmi , -1 ) > 0 ) {
												if ( _SCH_MODULE_Get_BM_SIDE( dmi , 1 ) == CHECKING_SIDE ) {
													//=====================================================================
													for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
														if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , k ) >= 0 ) {
															if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , k ) != SCHEDULER_READY ) {
																_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , k , PATHDO_WAFER_RETURN );
															}
														}
													}
													//=====================================================================
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , dmi , -1 , TRUE , 0 , 221 );
													//=====================================================================
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
	//========================================================================================================

	//-------------------------------------------------------------------------------------------
	if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 5 for FEM Module%cWHFMSUCCESS 5\n" , 9 );
		return SYS_SUCCESS;
	}
	//----------------------------------------------------------------------------------------------------------------------------
	if ( FM_Mode == 4 ) { // 2013.05.29
		if ( !FM_Mode4_Action ) FM_Mode = 0;
	}
	//----------------------------------------------------------------------------------------------------------------------------
	//==============================================================================================
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10-2 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
	//==============================================================================================
	//----------------------------------------------
	// 2007.11.29
	//----------------------------------------------
	if ( FM_Mode == 0 ) {
		//
		if ( !SCHEDULER_FM_PM_FAIL_RETURN( CHECKING_SIDE ) ) return 0; // 2016.03.07
		//
		SCHEDULER_ONEFMTM_FM_LOCK_FREE( CHECKING_SIDE ); // 2013.05.08
		//
//			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.11.29 // 2008.04.21
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2007.11.29 // 2008.04.21
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
//					if ( Scheduler_No_More_Supply_Check( CHECKING_SIDE ) ) { // 2008.07.29 // 2015.06.18
					if ( Scheduler_No_More_Supply_Check( -1 ) ) { // 2008.07.29 // 2015.06.18
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							//
							if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
							//
//							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2007.11.29 // 2009.09.28
							if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.11.29 // 2009.09.28
								//
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
											//
											if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) { // 2018.06.01
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) {
													//=====================================================================
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 304 );
													//=====================================================================
												}
											}
											else {
												//
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) > 0 ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
														//=====================================================================
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 301 );
														//=====================================================================
													}
												}
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
		//
//		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2008.03.20 // 2013.05.29
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 61 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
//==============================================================================================
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) { // 2008.03.20 // 2013.05.29
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
//					if ( SCHEDULER_FM_Current_No_More_Supply( CHECKING_SIDE ) ) { // 2015.06.18
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 62 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
//==============================================================================================
					if ( Scheduler_No_More_Supply_Check( CHECKING_SIDE ) ) { // 2015.06.18
						//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 63 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
//==============================================================================================
//						if ( SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , Sch_NoWay_Option() ) ) { // 2017.10.16 AKRA+SMART
//						if ( SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , Sch_NoWay_Option() ) && SCHEDULER_FM_has_No_TMSIDE( CHECKING_SIDE ) ) { // 2017.10.16 AKRA+SMART // 2018.05.16
//
//
						ICfinger = SCHEDULER_FM_Another_No_More_Supply_Ex( CHECKING_SIDE , Sch_NoWay_Option() );
						//
						ICsts = SCHEDULER_FM_has_No_TMSIDE( CHECKING_SIDE );
//
						if ( ( ICfinger == -1 ) && ( ICsts == 0 ) ) { // 2017.10.16 AKRA+SMART // 2018.05.16
							//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 64 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d [%d,%d]\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , ICfinger , ICsts );
//==============================================================================================
							ICfinger = 0;
							ICsts = 0;
							//
							for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
								if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
								//
								if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue; // 2009.03.11
								//
								if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) { // 2018.06.07
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.07
								}
								//
//								if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2007.11.29 // 2009.09.28
								if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.11.29 // 2009.09.28
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
										if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) <= 0 ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
														/*
														// 2010.01.17
														if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) <= 0 ) {
															if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) { // 2009.12.19
																//=====================================================================
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 301 );
																//=====================================================================
															}
														}
														else { // 2009.10.27
															if ( _SCH_PRM_GET_MODULE_SIZE( k ) == 1 ) {
																if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																	ICfinger = 1;
																}
															}
														}
														*/
														// 2010.01.17
														if ( _SCH_PRM_GET_MODULE_SIZE( k ) == 1 ) {
															if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																ICfinger = 1;
															}
														}
														else {
															if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) <= 0 ) {
																if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) { // 2009.12.19
																	//=====================================================================
//																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 301 ); // 2014.12.18
																	//=====================================================================
																	//
																	// 2014.12.18
																	//
																	if ( SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( 0 , CHECKING_SIDE , &Result ) ) {
																		//
																		if ( k == Result ) {
																			//
																			if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																				//=====================================================================
																				//
																				SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k ); // 2017.02.08
																				//
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 303 );
																				//=====================================================================
																			}
																			//
																		}
																	}
																	else {
																		//=====================================================================
																		//
																		SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k ); // 2017.02.08
																		//
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 301 );
																		//=====================================================================
																	}
																}
																else { // 2013.09.04
																	if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																		ICfinger = 1;
																	}
																}
															}
															else { // 2013.09.04
																if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) <= 1 ) {
																	if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
																		ICfinger = 1;
																	}
																}
															}
														}
													}
												}
											}
										}
										else { // 2009.10.28
											//
//											ICsts = 1; // 2015.06.15
											//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 65 ELSERT = ICfinger=%d ICsts=%d,k=%d\n" , ICfinger , ICsts , k );
//==============================================================================================
											if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) != BUFFER_SWITCH_SINGLESWAP ) { // 2018.05.10
												//
												if ( !SCHEDULER_CONTROL_Chk_BM_ONESLOT_FULL_AND_TM_ALL_RETURN( k ) ) { // 2015.06.16
													ICsts = 1;
												}
												//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 66 ELSERT = ICfinger=%d ICsts=%d,k=%d\n" , ICfinger , ICsts , k );
//==============================================================================================
											}
											//
										}
									}
								}
							}
							//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 67 ELSERT = ICfinger=%d ICsts=%d,k=%d\n" , ICfinger , ICsts , k );
//==============================================================================================
							if ( ( ICfinger == 1 ) && ( ICsts == 0 ) ) { // 2009.10.28
								for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
									if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
									//
									if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue;
									//
									if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) { // 2018.06.07
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue; // 2018.06.07
									}
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) { // 2007.11.29 // 2009.09.28
										//
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
												if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) <= 0 ) {
														if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
															//=====================================================================
															//
															SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k ); // 2017.02.08
															//
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 302 );
															//=====================================================================
															break;
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
						else { // 2014.06.10
							//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 68 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d [%d,%d]\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , ICfinger , ICsts );
//==============================================================================================
							if ( !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
								//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 69 ELSERT = FM_Mode=%d FM_Buffer=%d,%d,%d FM_Slot=%d,%d TSlot=%d,%d,%d FM_Pointer=%d,%d FM_Side=%d,%d\n" , FM_Mode , FM_Buffer , FM_Buffer2 , FM_Buffer_Toggle , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_TSlot_Real , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 );
//==============================================================================================
								ICsts = 0;
								ICfinger = 0;
								//
								for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
									//
									if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
									//
									if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue;
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) {
										//
										if ( ICsts == 0 ) {
											ICsts = k;
										}
										else {
											ICsts = 0;
											break;
										}
										//
										for ( lx = 0 ; lx < MAX_CASSETTE_SIDE ; lx++ ) {
											//
											if ( lx == CHECKING_SIDE ) continue;
											//
											if ( !_SCH_SYSTEM_USING_RUNNING( lx ) ) continue;
											//
											if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( lx , k ) ) continue;
											//
											if ( _SCH_MODULE_Get_Mdl_Use_Flag( lx , k ) != MUF_00_NOTUSE ) {
												//
												ICfinger = 1;
												//
												break;
												//
											}
											//
										}
										//
									}
									//
									if ( ICfinger == 1 ) break;
									//
								}
								//
//==============================================================================================
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 70 ELSERT = ICfinger=%d ICsts=%d,k=%d\n" , ICfinger , ICsts , k );
//==============================================================================================
								if ( ( ICsts > 0 ) && ( ICfinger == 0 ) ) {
									//
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ICsts ) ) {
										if ( _SCH_MODULE_Get_BM_FULL_MODE( ICsts ) == BUFFER_FM_STATION ) {
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ICsts ) <= 0 ) {
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ICsts , -1 ) <= 0 ) {
													//
													if ( _SCH_SUB_Simple_Supply_Possible( CHECKING_SIDE ) && _SCH_CASSETTE_Check_Side_Monitor( CHECKING_SIDE + 100 ) ) { // 2014.06.27
														_SCH_CASSETTE_Reset_Side_Monitor( CHECKING_SIDE , 0 );
													}
													else {
														//=====================================================================
														//
														SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , ICsts ); // 2017.02.08
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , ICsts , -1 , TRUE , 0 , 391 );
														//=====================================================================
													}
													//
												}
											}
										}
									}
								}
								//
							}
							//
						}
					}
					//
					//
					// 2018.11.23
					//
					//
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
						//
						if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( CHECKING_SIDE , k ) ) continue;
						//
						if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) continue;
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) continue;
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) != MUF_00_NOTUSE ) ) {
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
								if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) <= 0 ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
												//=====================================================================
												//
												SCHEDULING_BM_LOCK_CHECK_TM_SIDE( CHECKING_SIDE , k );
												//
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 3021 );
												//=====================================================================
												break;
											}
										}
									}
								}
							}
						}
					}
					//
					//
				}
			}
		}
	}
	//-------------------------------------------------------------------------------------------
	return 1;
	//-----------------------------------------------------------------------------------
}


BOOL SCHEDULER_CONTROL_Chk_BM_FULL_AND_TM_RETURN_SUPPLY_LOCKED( int bmc ) { // 2018.05.10
	int i , j , m , s , p , sc , lc , hasret , hasfree , chcnt , pd;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return TRUE;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) > 0 ) return FALSE;
	//
	//=================================================================================================
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , FALSE , &sc , &lc ); // out
	//
	for ( i = sc ; i <= lc ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) {
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmc , i ) ) {
				return FALSE;
			}
		}
		//
	}
	//
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		hasfree = -1;
		hasret = 0;
		//
		for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
			//
			if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , j ) ) continue;
			//
			if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) {
				//
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0,bmc ) ) {
				case 0 :
					hasfree = 0;
					break;
				case 1 :
					if ( j == TA_STATION ) hasfree = 1;
					break;
				case 2 :
					if ( j == TB_STATION ) hasfree = 2;
					break;
				}
				//
				continue;
			}
			//
			s = _SCH_MODULE_Get_TM_SIDE( 0 , j );
			p = _SCH_MODULE_Get_TM_POINTER( 0 , j );
			//
			if ( _SCH_MODULE_Get_TM_WAFER( 0 , j ) <= 0 ) return FALSE;
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) hasret++;
			//
		}
		//
		if ( hasfree == -1 ) return FALSE;
		if ( hasret <= 0 ) return FALSE;
		//
		Sleep(1);
		//
	}
	//
	//
	chcnt = 0;
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , TRUE , &sc , &lc ); // in
	//
	for ( i = sc ; i <= lc ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) != BUFFER_INWAIT_STATUS ) continue;
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
		p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		//
		if ( pd >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE;
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , pd , j );
			//
			if ( m > 0 ) {
				//
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0,m ) ) {
				case 0 :
					break;
				case 1 :
					if ( hasfree == 2 ) continue;
					break;
				case 2 :
					if ( hasfree == 1 ) continue;
					break;
				}
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
					if ( _SCH_MODULE_Get_PM_WAFER( m , -1 ) <= 0 ) {
						return FALSE;
					}
				}
			}
			//
		}
		//
		chcnt++;
		//
	}
	//
	if ( chcnt == 0 ) return FALSE;
	//
	return TRUE;
}

//BOOL SCHEDULER_FM_has_No_TMSIDE( int side ) { // 2017.10.16 AKRA+SMART // 2018.05.16
int SCHEDULER_FM_has_No_TMSIDE( int side ) { // 2017.10.16 AKRA+SMART // 2018.05.16
	int k;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) != BUFFER_SWITCH_SINGLESWAP ) return TRUE; // 2018.05.16
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) != BUFFER_SWITCH_SINGLESWAP ) return 0; // 2018.05.16
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( side , k ) ) continue;
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , k ) != MUF_00_NOTUSE ) ) {
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_FULL_AND_TM_RETURN_SUPPLY_LOCKED( k ) ) { // 2018.05.10
//						return FALSE; // 2018.05.16
						return k; // 2018.05.16
					}
				}
			}
		}
	}
//	return TRUE; // 2018.05.16
	return 0; // 2018.05.16
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Wait
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Scheduler_Main_Waiting_for_Switch( int CHECKING_SIDE ) {
	int i , j , k , paral = FALSE;

	if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) <= 0 ) {
		return FALSE;
	}
//	if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 4 ) { // 2008.08.25
	if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 999999 ) { // 2008.08.25
		return FALSE;
	}
//	if ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) == 3 ) { // 2008.08.25
	if ( ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 3 ) && ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) < 999999 ) ) { // 2008.08.25
		//-------------------------------------------------------------------------------------------
		// 2008.08.25
		//-------------------------------------------------------------------------------------------
		i = _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE );
		if ( ( i > 3 ) && ( i >= BM1 ) && ( i < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return TRUE;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return TRUE;
			}
		}
		//-------------------------------------------------------------------------------------------
		//
		switch( _SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) ) { // 2006.09.07
		case 1 :	// Continue // 2006.09.07
			for ( i = 0 , j = -1 , k = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != CHECKING_SIDE ) {
					if ( _SCH_SYSTEM_USING_GET( i ) > 0 ) {
//						if ( !_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , FALSE , FALSE ) ) { // 2003.01.11 // 2008.07.31
						if ( !_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 2 : 0 , FALSE ) ) { // 2003.01.11 // 2008.07.31
							if ( _SCH_SYSTEM_RUNORDER_GET( i ) < _SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
								if ( j == -1 ) {
									j = i;
									k = _SCH_SYSTEM_RUNORDER_GET( i );
								}
								else {
									if ( _SCH_SYSTEM_RUNORDER_GET( i ) > k ) {
										j = i;
										k = _SCH_SYSTEM_RUNORDER_GET( i );
									}
								}
							}
						}
						else { // 2008.07.31
							paral = TRUE;
						}
					}
				}
			}
			if ( j == -1 ) {
//				_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 4 ); // 2008.08.25
				_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 ); // 2008.08.25
				//---------------------------------------------------
				_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , -1 , paral ? 2 : 0 , FALSE ); // 2008.07.31
				//---------------------------------------------------
				return FALSE;
			}
			else {
				// 2001.12.27
				if ( _SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) == 3 ) { // 2002.12.26
					if ( ( _SCH_SYSTEM_USING_GET( j ) <= 0 ) || ( _SCH_MODULE_Get_TM_DoPointer( j ) > _SCH_MODULE_Get_FM_LastOutPointer( j ) ) ) {
						if ( _SCH_MODULE_Chk_TM_Free_Status2( j , _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) ) ) {
//							_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 4 ); // 2008.08.25
							_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 ); // 2008.08.25
							//---------------------------------------------------
							_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , -1 , paral ? 2 : 0 , FALSE ); // 2008.07.31
							//---------------------------------------------------
							return FALSE;
						}
					}
				}
				else {
					if ( ( _SCH_SYSTEM_USING_GET( j ) <= 0 ) || ( _SCH_MODULE_Get_TM_DoPointer( j ) > _SCH_MODULE_Get_FM_LastOutPointer( j ) ) ) {
//						_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 4 ); // 2008.08.25
						_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 ); // 2008.08.25
						//---------------------------------------------------
						_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , -1 , paral ? 2 : 0 , FALSE ); // 2008.07.31
						//---------------------------------------------------
						return FALSE;
					}
				}
			}
			break; // 2006.09.07
		default : // 2006.09.07
//			_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 4 ); // 2006.09.07 // 2008.08.25
			_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 999999 ); // 2006.09.07 // 2008.08.25
			return FALSE; // 2006.09.07
			break; // 2006.09.07
		} // 2006.09.07
	}
	return TRUE;
}





