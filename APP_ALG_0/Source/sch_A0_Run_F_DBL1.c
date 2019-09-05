//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_CimSeqnc.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_commonuser.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_subBM.h"
#include "sch_A0_param.h"
#include "sch_A0_sub.h"
#include "sch_A0_sub_sp2.h" // 2005.10.07
#include "sch_A0_sub_sp3.h" // 2006.03.07
#include "sch_A0_sub_sp4.h" // 2006.03.07
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_sub_F_dbl1.h"
//================================================================================

/*
	BATCH_SUPPLY_INTERRUPT
															A		B		C	
	0	:	N/A						:	Supply Priority	:		:		:		:
	1	:	On						:	Supply Priority	:	*	:		:		:	A
	2	:	N/A(Normal End)			:	Supply Priority	:	*	:	*	:		:	A,B
	3	:	On(Normal End)			:	Supply Priority	:	*	:	*	:	*	:	A,B,C
	4	:	N/A(M-FM)				:	Return Priority	:		:		:		:
	5	:	On(M-FM)				:	Return Priority	:	*	:		:		:	A
	6	:	N/A(Normal End)(M-FM)	:	Return Priority	:	*	:	*	:		:	A,B
	7	:	On(Normal End)(M-FM)	:	Return Priority	:	*	:	*	:	*	:	A,B,C

	0	:	Always Go BM_IN								:	if Direct Going Possible then Skip BM_OUT else Go BM_OUT	:
	1	:	if Direct Going Possible then Skip BM_IN	:	if Direct Going Possible then Skip BM_OUT else Go BM_OUT	:
	2	:	if Direct Going Possible then Skip BM_IN	:	Direct_CM													:
	3	:	if Direct Going Possible then Skip BM_IN	:	Direct_CM													:	BUffering


  CM_SUPPLY_MODE

	0	:	Supply First
	1	:	Return First

*/




extern BOOL FM_Pick_Running_Blocking_style_0;

BOOL FM_Last_Moving_style_0 = FALSE; // 2010.02.20

#ifdef PM_CHAMBER_30

int	BUFFER_IN_ACT_MODE[ MAX_CASSETTE_SLOT_SIZE ];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FEM Scheduling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Scheduler_Check_ALL_TM_Free() {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return FALSE;
	}
	return TRUE;
}

int Scheduler_Check_BM_OTHERSLOT_HAS_WAFER( int ch , int slot ) { // 2007.10.25
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( ( i + 1 ) == slot ) continue;
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			return TRUE;
		}
	}
	return FALSE;
}
//
int SCHEDULER_CONTROL_Chk_BM_FREE_DBL_SLOT( int ch , int *s1 , int *s2 , int orderoption ) { // 2009.03.03
	int i , k = 0;
	*s1 = 0;
	*s2 = 0;
	if ( orderoption == 2 ) {
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
					//
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
						//
						*s2 = i + 1;
						*s1 = i;
						k++;
						k++;
						//
					}
					//
				}
			}
			i++;
		}
		if ( k == 0 ) {
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					//
					*s1 = i;
					k++;
				}
			}
		}
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) <= 0 ) {
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i-1 ) ) { // 2018.04.03
						*s2 = i;
						*s1 = i - 1;
						k++;
						k++;
					}
				}
			}
			i--;
		}
		if ( k == 0 ) {
			for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					//
					*s1 = i;
					k++;
				}
			}
		}
	}
	return k;
}
//=================================================================================================================================
BOOL Scheduler_Check_Pick_CM_FM1_Use( int ch ) {
	if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ch ) < 3 ) return TRUE;
	return FALSE;
}

BOOL Scheduler_Check_Place_CM_FM1_Use( int ch ) {
	if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( ch ) < 3 ) return TRUE;
	return FALSE;
}
//=================================================================================================================================
BOOL Scheduler_Check_Possible_Plae_to_BM_Has_Wafer() {
	int k;
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
			}
		}
		else {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , FALSE ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
			}
		}
	}
	return FALSE;
}
//=================================================================================================================================
BOOL SCHEDULING_CHECK_FM_No_More_DBL( int ch ) { // 2015.07.02
	int i;
	//
	if ( ch == -1 ) {
		if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) return TRUE;
	}
	else {
		if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( -1 , ch , 1 ) ) return TRUE;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( i , -1 , TRUE ) ) return FALSE;
	}
	//
	return TRUE;
}

//=================================================================================================================================
BOOL Scheduler_Check_BM_Out_to_go_TMSIDE( int ch , int bmin ) {
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2008.07.13
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_OUTWAIT_STATUS ) > 0 ) {
			if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) != 0 ) {
				//
				if ( Scheduler_Check_Possible_Plae_to_BM_Has_Wafer() ) return TRUE; // 2008.02.11
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE; // 2008.02.11
				//
				if ( !Scheduler_Check_ALL_TM_Free() ) return TRUE;
				//
			}
			return FALSE;
		}
	}
	else { // 2008.07.13
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
//			if ( !SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( -1 , ch , 1 ) ) return FALSE; // 2015.07.02
			if ( !SCHEDULING_CHECK_FM_No_More_DBL( ch ) ) return FALSE; // 2015.07.02
			//
			if ( !Scheduler_Check_ALL_TM_Free() ) return TRUE;
			//
			return FALSE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
//			if ( !SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( -1 , ch , 1 ) ) return FALSE; // 2015.07.02
			if ( !SCHEDULING_CHECK_FM_No_More_DBL( ch ) ) return FALSE; // 2015.07.02
			//
			if ( !Scheduler_Check_ALL_TM_Free() ) return TRUE;
			//
			return FALSE;
		}
		else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2008.09.30
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_OUTWAIT_STATUS ) > 0 ) return FALSE;
			//
			if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( ch , BUFFER_INWAIT_STATUS ) > 0 ) ) return TRUE; // 2009.03.28
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( ch , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) return TRUE;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
			//
//			if ( !SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( -1 , ch , 1 ) ) return FALSE; // 2015.07.02
			if ( !SCHEDULING_CHECK_FM_No_More_DBL( ch ) ) return FALSE; // 2015.07.02
			//
			if ( !Scheduler_Check_ALL_TM_Free() ) return TRUE;
			//
			return FALSE;
		}
		else {
			// not prepared
		}
		//
	}
	return TRUE;
}
//=================================================================================================================================
int Scheduler_Check_Possible_Plae_to_BM( int *FM_TSlot , int *FM_TSlot2 , BOOL prcscheck ) {
	int k , RunBuffer;
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) continue; // 2015.10.23
		//
		RunBuffer = -1;
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.01.02
			if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue;
			if ( !prcscheck || ( _SCH_MACRO_CHECK_PROCESSING( k ) <= 0 ) ) { // 2008.07.13
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
//					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_DBL_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2008.09.30
//					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
					RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_DBL_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
				}
				else {
					// not prepared
				}
			}
		}
		else {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , prcscheck ) ) {
//				RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
				RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_DBL_SLOT( k , FM_TSlot , FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2009.03.03
			}
		}
		//
		if ( RunBuffer > 0 ) {
			if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) || ( RunBuffer >= 2 ) ) {
//					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
					//
//					return k; // 2013.11.14
					//
					// 2013.11.14
					if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
						if ( !prcscheck || ( _SCH_MACRO_CHECK_PROCESSING( k ) <= 0 ) ) {
							return k;
						}
					}
					else {
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , prcscheck ) ) {
							return k;
						}
					}
					//
				}
			}
		}
	}
	return 0;
}


BOOL Scheduler_Check_BM_FULL_ALL_FOR_IN_DBL_PATHTHRU( int ch ) {
	int i;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
					//
					return FALSE;
					//
				}
				//
			}
		}
		i++;
	}
	return TRUE;
}

BOOL Scheduler_Check_NoMore_BM_OUT_Supply( int bmout ) { // 2011.01.21
	int k , i;
	//
//	if ( !_SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) return FALSE; // 2015.07.02
	if ( !SCHEDULING_CHECK_FM_No_More_DBL( -1 ) ) return FALSE; // 2015.07.02
	//
	if ( !Scheduler_Check_ALL_TM_Free() ) return FALSE;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( k ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( k , i ) > 0 ) return FALSE;
		}
	}
	//
	return TRUE;
}

int Scheduler_Check_Possible_Pick_from_BUFFER_BM( int side , BOOL outmode , int ch , int *s1 , int *s2 ) {
	int i , pt = -1 , sside = -1;
	int j , k;
	int Cooling , cres;
	//=============================================================================
	Cooling = 0; // 2011.01.23
	//
	if ( outmode ) { // 2011.01.21
		//
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
			//
			Cooling = 1; // 2011.01.23
			//
			j = 0;
			k = 0;
			for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
					j++;
				}
				else {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
						j++;
					}
					else {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
							//
							k++;
							//
						}
						//
					}
				}
				i++;
			}
			//
			if      ( j <= 0 ) {
				return 0;
			}
//			else if ( j <= 1 ) { // 2011.04.05 for Future Option
//				if ( k > 0 ) {
//					if ( !Scheduler_Check_NoMore_BM_OUT_Supply( ch ) ) {
//						return 0;
//					}
//				}
//			}
			//
		}
	}
	//=============================================================================
	*s1 = 0;
	*s2 = 0;
	//=============================================================================
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) == 1 ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , 1 ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) != side ) {
				return 99;
			}
			else {
				//
				if ( Cooling == 1 ) { // 2011.01.24
					//
					cres = _SCH_MACRO_CHECK_FM_MCOOLING( side , 1 , FALSE );
					//
					if ( cres == SYS_ABORTED ) return -1;
					if ( cres != SYS_SUCCESS ) return 0;
					//
				}
				//
				*s1 = 1;
				return 1;
			}
		}
		return 0;
	}
	//=============================================================================
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( sside == -1 ) {
				sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
				pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
				*s1 = i;
				if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
						if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) , _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) , _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ) ) ) { // 2008.06.27
							*s2 = i + 1;
						}
						else {
							*s2 = 0;
						}
					}
					else {
						*s2 = 0;
					}
				}
				else {
					*s2 = 0;
				}
			}
			else {
				if ( sside == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
					if ( pt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
						sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
						pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						*s1 = i;
						if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
							if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) , _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) , _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ) ) ) { // 2008.06.27
									*s2 = i + 1;
								}
								else {
									*s2 = 0;
								}
							}
							else {
								*s2 = 0;
							}
						}
						else {
							*s2 = 0;
						}
					}
				}
				else {
//					if ( _SCH_SYSTEM_RUNORDER_GET( sside ) > _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( ch , i ) ) ) { // 2007.09.05
					if ( _SCH_CLUSTER_Get_SupplyID( sside , pt ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) { // 2007.09.05
						sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
						pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						*s1 = i;
						if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
							if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) , _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) , _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ) ) ) { // 2008.06.27
									*s2 = i + 1;
								}
								else {
									*s2 = 0;
								}
							}
							else {
								*s2 = 0;
							}
						}
						else {
							*s2 = 0;
						}
					}
				}
			}
		}
	}
	//=============================================================================
	for ( i = 2 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) <= 0 ) {
			//
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i - 1 ) ) { // 2018.04.03
				//
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
					if ( sside == -1 ) {
						sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
						pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						*s1 = i;
						*s2 = 0;
					}
					else {
						if ( sside == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
							if ( pt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
								sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
								pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								*s1 = i;
								*s2 = 0;
							}
						}
						else {
	//						if ( _SCH_SYSTEM_RUNORDER_GET( sside ) > _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( ch , i ) ) ) { // 2007.09.05
							if ( _SCH_CLUSTER_Get_SupplyID( sside , pt ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) { // 2007.09.05
								sside = _SCH_MODULE_Get_BM_SIDE( ch , i );
								pt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								*s1 = i;
								*s2 = 0;
							}
						}
					}
				}
				//
			}
			//
		}
		i++;
	}
	//=============================================================================
	if ( sside == -1 ) return 0;
	//=============================================================================
	//
	if ( Cooling == 1 ) { // 2011.01.24
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( *s1 > 0 ) {
				//
				cres = _SCH_MACRO_CHECK_FM_MCOOLING( side , *s1 , FALSE );
				//
				if ( cres == SYS_ABORTED ) return -1;
				if ( cres != SYS_SUCCESS ) return 0;
			}
			//
			if ( *s2 > 0 ) {
				//
				cres = _SCH_MACRO_CHECK_FM_MCOOLING( side , *s2 , FALSE );
				//
				if ( cres == SYS_ABORTED ) return -1;
				if ( cres != SYS_SUCCESS ) return 0;
			}
			//
		}
		//
	}
	//
	//=============================================================================
	if ( sside != side ) {
		if ( ( *s1 >  0 ) && ( *s2 >  0 ) ) return 99;
		if ( ( *s1 >  0 ) && ( *s2 <= 0 ) ) return 98;
		if ( ( *s1 <= 0 ) && ( *s2 >  0 ) ) return 98;
		return 97;
	}
	//=============================================================================
	if ( ( *s1 > 0 ) && ( *s2 > 0 ) ) return 2;
	if ( ( *s1 > 0 ) && ( *s2 <= 0 ) ) return 1;
	if ( ( *s1 <= 0 ) && ( *s2 > 0 ) ) return 1;
	//=============================================================================
	return 0;
}

int Scheduler_Check_Possible_Place_to_BUFFER_BM( int ch , BOOL outcheck , int mode , int *s1 , int *s2 , int *s3 ) {
	int i , k = 0;
	*s1 = 0;
	*s2 = 0;
	*s3 = 0;
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) >= 2 ) {
		//==================================================================================
		if ( mode == 0 ) {
			for ( i = 2 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
							//
							*s3 = i - 1;
							*s2 = 0;
							*s1 = i;
							k = 1;
							return k;
						}
						//
					}
				}
				i++;
			}
			return 0;
		}
		//==================================================================================
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					//
					*s2 = 0;
					*s1 = i;
					k++;
					i++;
					if ( i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
								*s2 = i;
								*s3 = 0;
								k++;
							}
						}
						else {
							*s3 = i;
						}
					}
					else {
						*s3 = 0;
					}
					break;
					//
				}
			}
			i++;
		}
		//==================================================================================
		if ( k == 0 ) {
			for ( i = 2 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
							//
							*s3 = i - 1;
							*s2 = 0;
							*s1 = i;
							k = 1;
							break;
							//
						}
						//
					}
				}
				i++;
			}
		}
		//==================================================================================
	}
	else {
		//
		if ( mode == 0 ) return 0;
		//
		if ( _SCH_MODULE_Get_BM_WAFER( ch , 1 ) <= 0 ) {
			*s3 = 0;
			*s2 = 0;
			*s1 = 1;
			k = 1;
		}
	}
	return k;
}

BOOL Scheduler_Check_Possible_Pick_from_BM_OUT_WAFER( int ch , int side , int *slot1 , int *slot2 ) {
	int i , res , sp;
	res = FALSE;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
			if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
				if ( ( !res ) || ( sp > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
					res = TRUE;
					*slot1 = i;
					if ( ( i + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) {
						if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) && _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
							*slot2 =  i + 1;
						}
						else {
							*slot2 = 0;
						}
					}
					else {
						*slot2 = 0;
					}
					sp = _SCH_MODULE_Get_BM_POINTER( ch , i );
				}
			}
		}
		i++;
	}
	return res;
}

int Scheduler_Check_Possible_Pick_from_BM( int side , int *ch , int *s1 , int *s2 , BOOL prcscheck ) {
	int i , k , sside = 0 , sspt , RunBuffer;
	int FM_CO1 , FM_CO2 , FM_Buffer , FM_Slot , FM_Slot2;

	FM_Buffer = -1;

	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return 0;

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			//
			RunBuffer = FALSE;
			//
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.01.02
				if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2013.11.14
						if ( !prcscheck || ( _SCH_MACRO_CHECK_PROCESSING( k ) <= 0 ) ) { // 2008.07.13
							if ( Scheduler_Check_Possible_Pick_from_BM_OUT_WAFER( k , i , &FM_CO1 , &FM_CO2 ) ) {
//								if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2013.11.14
								RunBuffer = TRUE;
//								} // 2013.11.14
							}
						}
					}
				}
			}
			else {
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2013.11.14
					if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , prcscheck ) ) {
						if ( Scheduler_Check_Possible_Pick_from_BM_OUT_WAFER( k , i , &FM_CO1 , &FM_CO2 ) ) {
//							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2013.11.14
							RunBuffer = TRUE;
//							} // 2013.11.14
						}
					}
				}
			}
			//
			if ( RunBuffer ) {
//				if ( ( FM_Buffer == -1 ) || ( _SCH_SYSTEM_RUNORDER_GET( sside ) > _SCH_SYSTEM_RUNORDER_GET( i ) ) ) { // 2007.09.05
				if ( FM_Buffer == -1 ) {
					sside = i;
					sspt  = _SCH_MODULE_Get_BM_POINTER( k , FM_CO1 );
					FM_Buffer = k;
					FM_Slot   = FM_CO1;
					FM_Slot2  = FM_CO2;
				}
				else {
					if ( sside == i ) {
						if ( sspt > _SCH_MODULE_Get_BM_POINTER( k , FM_CO1 ) ) { // 2007.09.05
							sside = i;
							sspt  = _SCH_MODULE_Get_BM_POINTER( k , FM_CO1 );
							FM_Buffer = k;
							FM_Slot   = FM_CO1;
							FM_Slot2  = FM_CO2;
						}
					}
					else {
						if ( _SCH_CLUSTER_Get_SupplyID( sside , sspt ) > _SCH_CLUSTER_Get_SupplyID( i , _SCH_MODULE_Get_BM_POINTER( k , FM_CO1 ) ) ) { // 2007.09.05
							sside = i;
							sspt  = _SCH_MODULE_Get_BM_POINTER( k , FM_CO1 );
							FM_Buffer = k;
							FM_Slot   = FM_CO1;
							FM_Slot2  = FM_CO2;
						}
					}
				}
			}
		}
	}
	if ( FM_Buffer >= 0 ) {
		if ( sside == side ) {
			*ch = FM_Buffer;
			*s1 = FM_Slot;
			*s2 = FM_Slot2;
			return 1;
		}
		else {
			*ch = FM_Buffer;
			*s1 = FM_Slot;
			*s2 = FM_Slot2;
			return 99;
		}
	}
	return 0;
}

int Scheduler_Check_Possible_Pick_from_BM_Out_First() { // 2008.10.23
	int k;
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return FALSE;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , FALSE ) ) {
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
						return TRUE;
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , FALSE ) ) {
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_OUTWAIT_STATUS ) > 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , BUFFER_INWAIT_STATUS ) > 0 ) {
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}

int Scheduler_Check_Possible_Pick_from_CM( BOOL dbl , int side , int *sl , int *pt , int *sl2 , int *pt2 , int *sider , int check_side , int check_pt , int bmin , int bmout ) {
	int i , Result , ret_rcm , ret_pt;
	int FM_Slot , FM_Slot2 , FM_Slotx;
	int FM_Pointer , FM_Pointer2 , FM_Pointerx;
	int FM_HSide , FM_HSide2 , FM_HSidex;
	int cpreject , retpointer1 , retpointer2;
	int otheryes;
	int makedeny; // 2009.07.22

	//==================================================================================================
	Result = 0;
	ret_rcm = 0;
	ret_pt = 0;
	FM_Pointer = 0;
	FM_Pointer2 = 0;
	FM_Slot = 0;
	FM_Slot2 = 0;
	otheryes = 0;
//	dbl = 0; // 2013.06.21
	//==================================================================================================

	otheryes = FALSE;
	makedeny = FALSE; // 2009.07.22

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//==================================================================================================
		_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 201 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d][dbl=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes , dbl );
		//==================================================================================================
		if ( side == -1 ) {
			//==========================================================================================
			// 2007.09.04
			//==========================================================================================
			if ( ( check_side != -1 ) && ( check_side == i ) ) continue;
			//==========================================================================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( i , 3 ); // 2007.09.06
			//==========================================================================================
		}
		//==================================================================================================
		_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 202 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
		//==================================================================================================
		if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , 0 ) ) {
			//==================================================================================================
			if ( _SCH_SUB_FM_Current_No_Way_Supply( i , ret_pt , -1 ) ) {
				//
				makedeny = TRUE; // 2009.07.22
				//
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( i , 1 ); // 2008.06.27
				continue; // 2008.06.22
			}
			//==================================================================================================
			if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( i , ret_pt , TRUE ) ) { // 2009.04.06
				//
				makedeny = TRUE; // 2009.07.22
				//
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( i , 1 ); // 2009.04.06
				continue; // 2009.04.06
			} // 2009.04.06
			//==================================================================================================
			_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 203 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
			//==================================================================================================
			FM_Slot = 0;
			FM_Slot2 = 0;
			//======================================================================================
//			Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , i , &FM_Slot , &FM_Slotx , &FM_Pointer , &FM_Pointerx , &FM_HSide , &FM_HSidex , FALSE , 3 , &cpreject , &retpointer1 ); // 2008.11.01
			Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , i , &FM_Slot , &FM_Slotx , &FM_Pointer , &FM_Pointerx , &retpointer1 , &FM_HSide , &FM_HSidex , &cpreject ); // 2008.11.01
			//======================================================================================
			//==================================================================================================
			_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 204 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
			//==================================================================================================
/*
// 2008.01.11
			if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
				if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( i , FM_Pointer ) ) == 3 ) {
					if ( SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() <= 0 ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
						FM_Slot = 0;
					}
				}
			}
			else {
				if ( Result < 0 ) FM_Slot = 0;
			}
*/
			if ( FM_Slot > 0 ) {
				if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( i , FM_Pointer ) ) {
					_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
					FM_Slot = 0;
				}
			}
			//==================================================================================================
			_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 205 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
			//==================================================================================================
			if ( ( FM_Slot > 0 ) && ( dbl ) ) {
				//======================================================================================
//				Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , i , &FM_Slot2 , &FM_Slotx , &FM_Pointer2 , &FM_Pointerx , &FM_HSide2 , &FM_HSidex , FALSE , 3 , &cpreject , &retpointer2 ); // 2008.11.01
				Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , i , &FM_Slot2 , &FM_Slotx , &FM_Pointer2 , &FM_Pointerx , &retpointer2 , &FM_HSide2 , &FM_HSidex , &cpreject ); // 2008.11.01
				//======================================================================================
				//==================================================================================================
				_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 206 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
				//==================================================================================================
/*
// 2008.01.11
				if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( i , FM_Pointer2 ) ) == 3 ) {
						if ( SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() <= 0 ) {
							_SCH_MODULE_Set_FM_DoPointer( i , retpointer2 );
							FM_Slot2 = 0;
						}
					}
				}
				else {
					if ( Result < 0 ) FM_Slot2 = 0;
				}
*/
				if ( FM_Slot2 > 0 ) {
					if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( i , FM_Pointer2 ) ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer2 );
						FM_Slot2 = 0;
					}
				}
				//==================================================================================================
				_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 207 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
				//==================================================================================================
			}
			//======================================================================================
			if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) {
				//====================================================
				if ( FM_Slot2 > 0 ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( i , FM_Pointer , i , FM_Pointer2 ) ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer2 );
						FM_Slot2 = 0;
					}
				}
				//====================================================
				if ( check_side >= 0 ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( i , FM_Pointer , check_side , check_pt ) ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
						FM_Slot = 0;
					}
					else { // 2008.01.11
						if ( _SCH_MACRO_HAS_PRE_PROCESS_OPERATION( i , FM_Pointer , -1 ) ) {
							_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
							FM_Slot = 0;
						}
						else {
							if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) {
								if ( i != check_side ) {
									_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
									FM_Slot = 0;
								}
							}
						}
					}
				}
			}
			//==================================================================================================
			_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 208 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
			//==================================================================================================
			if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) {
				if ( side == -1 ) {
					*sider = i;
					*sl = FM_Slot;
					*pt = FM_Pointer;
					*sl2 = FM_Slot2;
					*pt2 = FM_Pointer2;
					//==================================================================================================
					_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 209 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
					//==================================================================================================
					return 1;
				}
				else {
					if ( side == i ) {
						*sider = i;
						*sl = FM_Slot;
						*pt = FM_Pointer;
						*sl2 = FM_Slot2;
						*pt2 = FM_Pointer2;
						//==================================================================================================
						_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 210 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
						//==================================================================================================
						return 1;
					}
					else {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
						otheryes = TRUE;
						//==================================================================================================
						_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 211 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
						//==================================================================================================
					}
				}
			}
			//======================================================================================
		}
		//==================================================================================================
		_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 219 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
		//==================================================================================================
	}
	//==================================================================================================
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 299 [%d]= [Result=%d][ret_rcm=%d][ret_pt=%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][otheryes=%d]\n" , i , Result , ret_rcm , ret_pt , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , otheryes );
	//==================================================================================================
	if ( otheryes ) return 2;
	//
	//----------------------------------------------------------------------
	// 2009.07.22
	//----------------------------------------------------------------------
	if ( makedeny ) {
		//
		if ( !SCHEDULING_CHECK_TM_All_Free_Status( 1 ) ) { // 2012.08.30
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , -1 ) <= 0 ) {
				//
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( i == bmin ) continue;
					if ( i == bmout ) continue;
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
					//
					if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , FALSE ) ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) {
										//
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , i , 1 , TRUE , 0 , 3001 );
										//
										break;
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
	}
	//----------------------------------------------------------------------
	//
	return 0;
}


int Scheduler_Check_Possible_BM_BUFFER_MOVEMENT( int side , int bmin , int bmout , BOOL outmodecheck ) { // 2007.09.21
	int FM_Buffer , FM_TSlot , FM_TSlot2 , FM_T2Slot , FM_T2Slot2 , res;
	if ( Scheduler_Check_Possible_Pick_from_BM( side , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE ) != 0 ) {
		res = Scheduler_Check_Possible_Place_to_BUFFER_BM( bmout , TRUE , 1 , &FM_T2Slot , &FM_T2Slot2 , &FM_Buffer );
		if ( res > 0 ) {
			if ( ( FM_TSlot <= 0 ) || ( FM_TSlot2 <= 0 ) || ( res >= 2 ) ) { // 2007.11.23
				return 1;
			}
		}
	}
	//
	if ( outmodecheck ) { // 2008.10.23
		if ( Scheduler_Check_Possible_Pick_from_BM_Out_First() ) return 0;
	}
	//
	if ( Scheduler_Check_Possible_Plae_to_BM( &FM_TSlot , &FM_TSlot2 , FALSE ) > 0 ) {
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmin , -1 ) > 0 ) {
			return 2;
		}
	}
	return 0;
}





int Scheduler_Pick_from_CM_to_BM_IN( int curside , int BM_IN , int BM_OUT ) {
	int k , l , diff , FM_Count;
	int FM_OSlot , FM_OSlot2 , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2;
	int FM_CO1 , FM_CO2;
	int FM_P_Buffer , FM_P_TSlot , FM_P_TSlot2;
	int	runaction = FALSE , res , buffering;
	//
	if ( Scheduler_Check_Possible_Pick_from_BM_Out_First() ) return 0; // 2008.10.23
	//
	//===============================================
	res = 0;
	FM_Count = 0;
	diff = 0;
	FM_Side = 0;
	FM_Side2 = 0;
	FM_Pointer = 0;
	FM_Pointer2 = 0;
	FM_Slot = 0;
	FM_Slot2 = 0;
	FM_TSlot = 0;
	FM_TSlot2 = 0;
	FM_OSlot = 0;
	FM_OSlot2 = 0;
	FM_CO1 = 0;
	FM_CO2 = 0;
	FM_P_Buffer = 0;
	//===============================================

	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		//
//		if ( _SCH_SYSTEM_USING_GET(k) < 10 ) continue;
		if ( !_SCH_SYSTEM_USING_RUNNING( k ) ) continue; // 2010.05.21
		//
		if ( ( curside != -1 ) && ( curside != k ) ) continue;
		//
		_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 101 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
		//
//		res = Scheduler_Check_Possible_BM_BUFFER_MOVEMENT( k , BM_IN , BM_OUT ); // 2008.01.15
		//
//		if ( res == 0 ) { // 2007.09.21 // 2008.01.15
			//
//			_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 102 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer ); // 2008.01.15
			//
		//
		//------------------------------------------------------------------------------
		// 2008.01.18
		//------------------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) != 0 ) {
			l = 1;
		}
		else {
			l = 0;
		}
		//------------------------------------------------------------------------------
		for ( ; l < 2 ; l++ ) { // 2008.01.14
			//
//			res = 0; // 2008.01.15
			//
			if ( l == 1 ) { // 2008.01.15
				res = Scheduler_Check_Possible_BM_BUFFER_MOVEMENT( k , BM_IN , BM_OUT , FALSE ); // 2008.01.15
			}
			//
			_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 102 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer ); // 2008.01.15
			//
			if ( ( l == 0 ) || ( res == 0 ) ) { // 2008.01.15
				//
				FM_Count = Scheduler_Check_Possible_Place_to_BUFFER_BM( BM_IN , FALSE , l , &FM_OSlot , &FM_OSlot2 , &diff );
				//
				_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 103 [%d]= [l=%d][res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , l , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
				//
				if ( FM_Count > 0 ) {
					//
					if ( diff > 0 ) {
						FM_TSlot = _SCH_MODULE_Get_BM_SIDE( BM_IN , diff );
						FM_TSlot2 = _SCH_MODULE_Get_BM_POINTER( BM_IN , diff );
					}
					else {
						FM_TSlot = -1;
						FM_TSlot2 = -1;
					}
					//
					_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 104 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
					//
					res = Scheduler_Check_Possible_Pick_from_CM( ( FM_Count >= 2 ) ? TRUE : FALSE , k , &FM_Slot , &FM_Pointer , &FM_Slot2 , &FM_Pointer2 , &FM_Side , FM_TSlot , FM_TSlot2 , BM_IN , BM_OUT );
					//
					_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 104-2 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
					//
					if ( res == 1 ) break;
				}
			}
		}
		//
		if ( l != 2 ) {
			//
			_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 105 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
			//
			FM_Side2 = FM_Side;
			//
//				_SCH_CASSETTE_Reset_Side_Monitor( k , 0 ); // 2007.11.27
			_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( k , 1 );
			//
			if ( _SCH_MODULE_Chk_FM_Finish_Status( k ) ) _SCH_SYSTEM_LASTING_SET( k , TRUE );
			//
			if ( FM_Slot2 <= 0 ) {
				if ( FM_Count >= 2 ) {
					if ( Scheduler_Check_Possible_Pick_from_CM( FALSE , -1 , &FM_Slot2 , &FM_Pointer2 , &FM_TSlot , &FM_TSlot2 , &FM_Side2 , FM_Side , FM_Pointer , BM_IN , BM_OUT ) != 1 ) {
						FM_Slot2 = 0;
					}
				}
			}
			//==========================
			// run
			//==========================
			FM_Pick_Running_Blocking_style_0 = 100; // 2008.10.23
			//
			FM_Last_Moving_style_0 = FALSE; // 2010.02.20
			//----------------------------------------------------------------------
			_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
			//----------------------------------------------------------------------
			if ( FM_Slot  > 0 ) _SCH_MODULE_Inc_FM_OutCount( FM_Side );
			if ( FM_Slot2 > 0 ) _SCH_MODULE_Inc_FM_OutCount( FM_Side2 );
			//----------------------------------------------------------------------
			if ( FM_Slot  > 0 ) FM_CO1 = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
			if ( FM_Slot2 > 0 ) FM_CO2 = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
			//----------------------------------------------------------------------
			buffering = -1; // 2008.06.26
			//
			if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
				if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO1 ) ) {
					if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO2 ) ) {
						diff = FALSE;
					}
					else {
						diff = TRUE;
					}
				}
				else {
					if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO2 ) ) {
						diff = TRUE;
					}
					else {
						diff = FALSE;
					}
				}
				//
				FM_P_Buffer = 0; // 2007.10.25
				//
				if ( !diff ) { // Sequential
					if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO1 ) ) {
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
							//======================================================================
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
							//======================================================================
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , ( FM_Slot2 * 100 ) + FM_Slot , ( FM_Side2 * 100 ) + FM_Side , ( FM_Pointer2 * 100 ) + FM_Pointer , -1 , ( FM_Slot2 * 100 ) + FM_Slot , 0 , 0 ) ) {
								return -1;
							}
							//
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , 0 , 0 , BM_IN , FM_OSlot ) ) {
								return -1;
							}
							//
						}
						else {
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
								return -1;
							}
							//======================================================================
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
							//======================================================================
						}
						//
						if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 0 ) { // 2007.10.25
							if ( !Scheduler_Check_BM_OTHERSLOT_HAS_WAFER( BM_IN , FM_OSlot ) ) { // 2007.10.25
								FM_P_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_P_TSlot , &FM_P_TSlot2 , TRUE ); // 2007.10.25
							}
						}
						//
						if ( FM_P_Buffer != 0 ) { // 2007.10.25
							//======================================================================
							// 2007.10.25
							//================
							// FM1 : Pick CM    FM_Slot2 => Place FM_P_Buffer FM_P_TSlot2
							// FM2 : Pick BM_IN FM_OSlot => Place FM_P_Buffer FM_P_TSlot
							//======================================================================
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_IN , FM_OSlot , FM_P_Buffer , FM_P_TSlot ) ) {
								return -1;
							}
							//==================================================================================
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 * 100 , FM_Side2 * 100 , FM_Pointer2 * 100 , 0 , 0 , FM_P_Buffer * 100 , FM_P_TSlot2 * 100 ) ) {
									_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
									return -1;
								}
							}
							else {
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , FM_P_Buffer , FM_P_TSlot2 ) ) {
									_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
									return -1;
								}
							}
							//==================================================================================
							if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 ) ) {
								return -1;
							}
							//==================================================================================
						}
						else {
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) { // 2008.06.25
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
									if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 * 100 , FM_Side2 * 100 , FM_Pointer2 * 100 , 0 , 0 , BM_IN * 100 , FM_OSlot2 * 100 ) ) {
										return -1;
									}
								}
								else {
									if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
										return -1;
									}
								}
							}
							else {
								buffering = 1; // 2008.06.26
							}
						}
						//======================================================================
					}
					else {
/*
// Order Change 2007.10.25
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
							return 0;
						}
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
*/
// Order Change 2007.10.25
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05 // 2007.11.26
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , ( FM_Slot2 * 100 ) + FM_Slot , ( FM_Side2 * 100 ) + FM_Side , ( FM_Pointer2 * 100 ) + FM_Pointer , -1 , ( FM_Slot2 * 100 ) + FM_Slot , 0 , 0 ) ) {
								return -1;
							}
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 * 100 , FM_Side2 * 100 , FM_Pointer2 * 100 , 0 , 0 , BM_IN * 100 , FM_OSlot2 * 100 ) ) {
								return -1;
							}
						}
						else {
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
								return -1;
							}
						}
						//======================================================================
//								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05 // 2007.11.26
						//======================================================================
						if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 0 ) { // 2007.10.25
							if ( !Scheduler_Check_BM_OTHERSLOT_HAS_WAFER( BM_IN , FM_OSlot2 ) ) { // 2007.10.25
								FM_P_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_P_TSlot , &FM_P_TSlot2 , TRUE ); // 2007.10.25
							}
						}
						//
						if ( FM_P_Buffer != 0 ) { // 2007.10.25
							//======================================================================
							// 2007.10.25
							//================
							// FM2 : Pick CM    FM_Slot   => Place FM_P_Buffer FM_P_TSlot
							// FM1 : Pick BM_IN FM_OSlot2 => Place FM_P_Buffer FM_P_TSlot2
							//======================================================================
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_IN , FM_OSlot2 , FM_P_Buffer , FM_P_TSlot2 ) ) {
								return -1;
							}
							//==================================================================================
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , 0 , 0 , FM_P_Buffer , FM_P_TSlot ) ) {
									_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
									return -1;
								}
							}
							else {
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , FM_P_Buffer , FM_P_TSlot ) ) {
									_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
									return -1;
								}
							}
							//==================================================================================
							if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 ) ) {
								return -1;
							}
							//==================================================================================
						}
						else {
/*
// Order Change 2007.10.25
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
								return 0;
							}
*/
// Order Change 2007.10.25
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) { // 2008.06.25
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( 100 + TB_STATION ) && ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) && ( FM_Slot + 1 == FM_Slot2 ) ) { // 2008.06.28
									if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , 0 , 0 , BM_IN , FM_OSlot ) ) {
										return -1;
									}
								}
								else {
									if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
										return -1;
									}
								}
							}
							else {
								buffering = 0; // 2008.06.26
							}
						}
						//======================================================================
					}
				}
				else {
					if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO1 ) ) {
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
							return -1;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return -1;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 ) ) {
							return -1;
						}
						//==================================================================================
					}
					else {
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
						//======================================================================
						if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 0 ) { // 2007.10.25
							if ( !Scheduler_Check_BM_OTHERSLOT_HAS_WAFER( BM_IN , -1 ) ) { // 2007.10.25
								FM_P_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_P_TSlot , &FM_P_TSlot2 , TRUE ); // 2007.10.25
							}
						}
						//
						if ( FM_P_Buffer != 0 ) { // 2007.10.25
							//======================================================================
							// 2007.10.25
							//================
							// FM1 : Pick CM    FM_Slot2 => Place FM_P_Buffer FM_P_TSlot2
							// FM2 : Pick CM    FM_Slot  => Place FM_P_Buffer FM_P_TSlot
							//======================================================================
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , FM_P_Buffer , FM_P_TSlot2 ) ) {
								return -1;
							}
							//==================================================================================
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , FM_P_Buffer , FM_P_TSlot ) ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return -1;
							}
							//==================================================================================
							if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 ) ) {
								return -1;
							}
							//==================================================================================
						}
						else {
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) { // 2008.06.25
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
									return -1;
								}
								//==================================================================================
								if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
									_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
									return -1;
								}
								//==================================================================================
								if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 ) ) {
									return -1;
								}
								//==================================================================================
							}
							else {
								buffering = 2;
							}
						}
					}
				}
				//==========================================================================================
				if ( FM_P_Buffer != 0 ) {
					//===================================================================================
					// 2007.10.25
					//===================================================================================
					_SCH_MODULE_Set_BM_SIDE( FM_P_Buffer , FM_P_TSlot , FM_Side );
					_SCH_MODULE_Set_BM_POINTER( FM_P_Buffer , FM_P_TSlot , FM_Pointer );
					_SCH_MODULE_Set_BM_WAFER_STATUS( FM_P_Buffer , FM_P_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
					//
					_SCH_MODULE_Set_BM_SIDE( FM_P_Buffer , FM_P_TSlot2 , FM_Side2 );
					_SCH_MODULE_Set_BM_POINTER( FM_P_Buffer , FM_P_TSlot2 , FM_Pointer2 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( FM_P_Buffer , FM_P_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_P_Buffer ) ) {
						if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.01.02
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_P_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
								}
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_P_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
								}
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2008.09.30
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_P_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
									if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_P_Buffer , BUFFER_INWAIT_STATUS ) > 0 ) ) { // 2009.03.28
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2251 );
									}
									else {
//										if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) { // 2008.09.30 // 2015.07.02
										if ( SCHEDULING_CHECK_FM_No_More_DBL( -1 ) ) { // 2015.07.02
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_P_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
											}
										}
									}
								}
								else {
									if ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) { // 2009.02.16
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2151 );
									}
								}
							}
							else {
								// not prepared
							}
							//
						}
						else {
//							if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) { // 2008.12.04 // 2015.07.02
							if ( SCHEDULING_CHECK_FM_No_More_DBL( -1 ) ) { // 2015.07.02
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
							}
							else { // 2008.12.04
								//if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_P_Buffer , FALSE , FALSE ) ) { // 2009.02.16
								if ( Scheduler_Check_BM_FULL_ALL_FOR_IN_DBL_PATHTHRU( FM_P_Buffer ) ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , k , FM_P_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2111 );
								}
							}
						}
					}
					//===================================================================================
				}
				else {
					_SCH_MODULE_Set_BM_SIDE( BM_IN , FM_OSlot , FM_Side );
					_SCH_MODULE_Set_BM_POINTER( BM_IN , FM_OSlot , FM_Pointer );
					_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot , FM_Slot , BUFFER_INWAIT_STATUS );
					//
					BUFFER_IN_ACT_MODE[ FM_OSlot ] = ( buffering == 0 ) || ( buffering == 2 ) ? TRUE : FALSE; // 2008.06.26
					//
					_SCH_MODULE_Set_BM_SIDE( BM_IN , FM_OSlot2 , FM_Side2 );
					_SCH_MODULE_Set_BM_POINTER( BM_IN , FM_OSlot2 , FM_Pointer2 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
					//
					BUFFER_IN_ACT_MODE[ FM_OSlot2 ] = ( buffering == 1 ) || ( buffering == 2 ) ? TRUE : FALSE; // 2008.06.26
				}
				//==========================================================================================
			}
			else if ( FM_Slot > 0 ) {
				if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO1 ) ) {
					//======================================================================
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
					//======================================================================
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
						return -1;
					}
					//======================================================================
				}
				else {
					//======================================================================
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2007.09.05
					//======================================================================
					if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) { // 2008.06.25
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
							return -1;
						}
						//======================================================================
					}
					else {
						if ( ( FM_OSlot % 2 ) == 0 ) { // 2008.07.28
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , BM_IN , FM_OSlot ) ) {
								return -1;
							}
						}
						else {
							buffering = 0; // 2008.06.25
						}
					}
				}
				//==========================================================================================
				_SCH_MODULE_Set_BM_SIDE( BM_IN , FM_OSlot , FM_Side );
				_SCH_MODULE_Set_BM_POINTER( BM_IN , FM_OSlot , FM_Pointer );
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot , FM_Slot , BUFFER_INWAIT_STATUS );
				//
				BUFFER_IN_ACT_MODE[ FM_OSlot ] = ( buffering == 0 ) || ( buffering == 2 ) ? TRUE : FALSE; // 2008.06.26
				//==========================================================================================
			}
			else if ( FM_Slot2 > 0 ) {
				if ( Scheduler_Check_Pick_CM_FM1_Use( FM_CO2 ) ) {
					//======================================================================
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
					//======================================================================
					if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) != 3 ) { // 2008.06.25
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
							return -1;
						}
					}
					else {
						if ( ( FM_OSlot % 2 ) == 0 ) { // 2008.07.28
							if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
								return -1;
							}
						}
						else {
							buffering = 1; // 2008.06.25
						}
					}
					//======================================================================
				}
				else {
					//======================================================================
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2007.09.05
					//======================================================================
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , BM_IN , FM_OSlot2 ) ) {
						return -1;
					}
					//======================================================================
				}
				//==========================================================================================
				_SCH_MODULE_Set_BM_SIDE( BM_IN , FM_OSlot2 , FM_Side2 );
				_SCH_MODULE_Set_BM_POINTER( BM_IN , FM_OSlot2 , FM_Pointer2 );
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
				//
				BUFFER_IN_ACT_MODE[ FM_OSlot2 ] = ( buffering == 1 ) || ( buffering == 2 ) ? TRUE : FALSE; // 2008.06.26
				//==========================================================================================
			}
			runaction = TRUE;
			//
			FM_Pick_Running_Blocking_style_0 = 0; // 2008.10.23
			//
		}
//		} // 2008.01.15
		//
		_SCH_LOG_DEBUG_PRINTF( curside , 0 , "FEM STEP 110 [%d]= [res=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][P_Buf=%d]\n" , k , res , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_P_Buffer );
		//
	}
	if ( runaction ) return 1;
	return 0;
}


int Scheduler_Arm_Intlks_Check_For_BM( int side , int tms , int BM_IN , int BM_OUT ) {
	int i;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return 1;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( BM_IN , -1 ) > 0 ) return 2;
	//
	if ( Scheduler_Check_ALL_TM_Free() ) return 3;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( i == BM_IN ) continue;
		if ( i == BM_OUT ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_WAIT_STATION ) continue; // 2015.10.23
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , FALSE ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
//		if ( !SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( -1 , i , 1 ) ) continue; // 2015.07.02
		if ( !SCHEDULING_CHECK_FM_No_More_DBL( i ) ) continue; // 2015.07.02
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,i ) == 0 ) {
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms,i ) == 0 ) {
				continue;
			}
		}
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , i , 1 , TRUE , 0 , 3005 );
		//
	}
	return 5;
}


int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_DOUBLE_STYLE_0( int CHECKING_SIDE , int mode ) {
	int k;

	if ( mode != 0 ) return -1;

	SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 0 );
	//-----------------------------------------------
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.01.02
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) == MUF_00_NOTUSE ) continue; // 2015.10.23
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) ) { // 2006.12.21
				if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) {
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , k , -1 , TRUE , 0 , 4 );
						}
					}
				}
			}
		}
	}
	return 0;
}
//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
int USER_DLL_SCH_INF_RUN_CONTROL_FEM_DOUBLE_STYLE_0( int CHECKING_SIDE ) { // 2007.06.21
	int BM_IN , BM_OUT , FM_Count , diff;
	//
	int	FM_Slot;
	int	FM_Slot2;
	int	FM_TSlot , FM_OSlot;
	int FM_CO1 , FM_CO2;
	int	FM_TSlot2 , FM_OSlot2;
	int	FM_Pointer;
	int	FM_Pointer2;
	//
	int	FM_Side;
	int	FM_Side2;
	//
	int FM_Buffer;

	int FM_P_Buffer , FM_Res1 , FM_Res2 , FM_P_Slot , FM_P_Slot2;

	int FM_P_Cooling;

	FM_Buffer = 0;
	FM_Count = 0;
	diff = 0;
	FM_Side = 0;
	FM_Side2 = 0;
	FM_Pointer = 0;
	FM_Pointer2 = 0;
	FM_Slot = 0;
	FM_Slot2 = 0;
	FM_TSlot = 0;
	FM_TSlot2 = 0;
	FM_OSlot = 0;
	FM_OSlot2 = 0;
	FM_CO1 = 0;
	FM_CO2 = 0;
	FM_Res1 = 0;
	FM_Res2 = 0;
	FM_P_Buffer = 0;
	FM_P_Slot = 0;
	FM_P_Slot2 = 0;

	//----------------------------------------------------------------------
	FM_Pick_Running_Blocking_style_0 = 0;
	//---------------------------------------------------------------------

	//======================================================
	BM_IN  = _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER();
	BM_OUT = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
	//======================================================
	if ( ( BM_IN == 0 ) || ( BM_OUT == 0 ) ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Failed for FEM Module Not Define%cWHFMFAIL 1\n" , 9 );
		return 0;
	}
	//======================================================
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM BM_IN & PLACE TO BM PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//

	if ( Scheduler_Check_Possible_Pick_from_BM_Out_First() ) { // 2008.10.23
		FM_Buffer = 0;
	}
	else {
		if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) != 0 ) { // 2008.01.18
			FM_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_TSlot , &FM_TSlot2 , TRUE ); // 2008.01.15
		}
		else {
			FM_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_TSlot , &FM_TSlot2 , FALSE ); // 2008.01.15
		}
	}

	if ( FM_Buffer != 0 ) {

		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		FM_Count = Scheduler_Check_Possible_Pick_from_BUFFER_BM( CHECKING_SIDE , FALSE , BM_IN , &FM_OSlot , &FM_OSlot2 );

		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 03 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( FM_Count == -1 ) { // 2011.01.23
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Failed for BM_OUT Pick Check 1%cWHFMFAIL 1\n" , 9 );
			return 0;
		}
		//
		if ( FM_Count == 99 ) {
			return 1;
		}
		//
		if ( FM_Count == 98 ) { // 2008.01.15
			if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) != 0 ) { // 2008.01.18
				return 1;
			}
			else {
				if ( Scheduler_Check_Possible_Plae_to_BM( &FM_TSlot , &FM_TSlot2 , TRUE ) != 0 ) {
					return 1;
				}
				FM_Count = 0;
			}
		}
		//
		if      ( FM_Count == 1 ) {
			if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 0 ) { // 2008.01.18
				//
				FM_P_Buffer = Scheduler_Check_Possible_Plae_to_BM( &FM_Res1 , &FM_Res2 , TRUE ); // 2008.01.15
				//
				if ( ( FM_P_Buffer == FM_Buffer ) && ( FM_Res1 == FM_TSlot ) && ( FM_Res2 == FM_TSlot2 ) ) {
					//
					FM_Count = Scheduler_Check_Possible_Pick_from_BUFFER_BM( CHECKING_SIDE , FALSE , BM_IN , &FM_Res1 , &FM_Res2 );
					//
					if ( FM_Count == -1 ) { // 2011.01.23
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Failed for BM_OUT Pick Check 2%cWHFMFAIL 2\n" , 9 );
						return 0;
					}
					//
					if ( FM_Count >= 98 ) {
						return 1;
					}
					if ( ( FM_Count == 1 ) && ( FM_Res1 == FM_OSlot ) && ( FM_Res2 == FM_OSlot2 ) ) {
						//
					}
					else { // 2008.01.15
						FM_Count = 0;
					}
				}
				else { // 2008.01.15
					FM_Count = 0;
				}
			}
			//
			if ( FM_Count != 0 ) {
				FM_Side    = _SCH_MODULE_Get_BM_SIDE( BM_IN , FM_OSlot );
				FM_Pointer = _SCH_MODULE_Get_BM_POINTER( BM_IN , FM_OSlot );
				FM_Slot    = _SCH_MODULE_Get_BM_WAFER( BM_IN , FM_OSlot );
				//
				FM_Last_Moving_style_0 = FALSE; // 2010.02.20
				//
				if ( BUFFER_IN_ACT_MODE[ FM_OSlot ] ) { // 2008.06.26
					//==========================================================================================
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , FM_Buffer , FM_TSlot ) ) {
						return 0;
					}
					//==========================================================================================
				}
				else {
					//==========================================================================================
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_IN , FM_OSlot , FM_Buffer , FM_TSlot ) ) {
						return 0;
					}
					//==========================================================================================
				}
				//
				_SCH_MODULE_Set_BM_SIDE( FM_Buffer , FM_TSlot , FM_Side );
				_SCH_MODULE_Set_BM_POINTER( FM_Buffer , FM_TSlot , FM_Pointer );
				_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
				//==========================================================================================
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot , 0 , -1 );
				//==========================================================================================
			}
		}
		else if ( FM_Count >= 2 ) {
			FM_Side     = _SCH_MODULE_Get_BM_SIDE( BM_IN , FM_OSlot );
			FM_Pointer  = _SCH_MODULE_Get_BM_POINTER( BM_IN , FM_OSlot );
			FM_Slot     = _SCH_MODULE_Get_BM_WAFER( BM_IN , FM_OSlot );

			FM_Side2    = _SCH_MODULE_Get_BM_SIDE( BM_IN , FM_OSlot2 );
			FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( BM_IN , FM_OSlot2 );
			FM_Slot2    = _SCH_MODULE_Get_BM_WAFER( BM_IN , FM_OSlot2 );
			//
			FM_Last_Moving_style_0 = FALSE; // 2010.02.20
			//
			//==================================================================================
			if ( BUFFER_IN_ACT_MODE[ FM_OSlot2 ] ) { // 2008.06.26
				if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , FM_Slot2 , FM_Buffer , FM_TSlot2 ) ) {
					return 0;
				}
			}
			else {
				if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_IN , FM_OSlot2 , FM_Buffer , FM_TSlot2 ) ) {
					return 0;
				}
			}
			//==================================================================================
			if ( BUFFER_IN_ACT_MODE[ FM_OSlot ] ) { // 2008.06.26
				if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot , FM_Buffer , FM_TSlot ) ) {
					_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
					return 0;
				}
			}
			else {
				if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_IN , FM_OSlot , FM_Buffer , FM_TSlot ) ) {
					_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
					return 0;
				}
			}
			//==================================================================================
			if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 ) ) {
				return 0;
			}
			//==================================================================================
			//==========================================================================================
			_SCH_MODULE_Set_BM_SIDE( FM_Buffer , FM_TSlot , FM_Side );
			_SCH_MODULE_Set_BM_POINTER( FM_Buffer , FM_TSlot , FM_Pointer );
			_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
			//==========================================================================================
			_SCH_MODULE_Set_BM_SIDE( FM_Buffer , FM_TSlot2 , FM_Side2 );
			_SCH_MODULE_Set_BM_POINTER( FM_Buffer , FM_TSlot2 , FM_Pointer2 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
			//==========================================================================================
			_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot , 0 , -1 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( BM_IN , FM_OSlot2 , 0 , -1 );
			//==========================================================================================
		}
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 04 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( FM_Count != 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
				if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.01.02
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2008.09.30
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2008.01.02
							if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , BUFFER_INWAIT_STATUS ) > 0 ) ) { // 2009.03.28
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2052 );
							}
							else {
//								if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) { // 2008.09.30 // 2015.07.02
								if ( SCHEDULING_CHECK_FM_No_More_DBL( -1 ) ) { // 2015.07.02
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP , TRUE ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
									}
								}
							}
						}
						else {
							if ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) { // 2009.02.20
								//--------------------------
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2051 );
								//--------------------------
							}
						}
					}
					else {
						// not prepared
					}
					//
				}
				else {
//					if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) { // 2008.12.04 // 2015.07.02
					if ( SCHEDULING_CHECK_FM_No_More_DBL( -1 ) ) { // 2015.07.02
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
					}
					else {
						//if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( FM_Buffer , FALSE , FALSE ) ) { // 2008.12.04 // 2009.02.16
						if ( Scheduler_Check_BM_FULL_ALL_FOR_IN_DBL_PATHTHRU( FM_Buffer ) ) { // 2009.02.16
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2001 );
						}
					}
				}
			}
		}
	}
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM CM & PLACE TO BM_IN PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	FM_Res1 = Scheduler_Pick_from_CM_to_BM_IN( CHECKING_SIDE , BM_IN , BM_OUT );
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	if ( FM_Res1 == -1 ) {
		return 0;
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM BM & PLACE TO BM_OUT PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 21 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	FM_Count = Scheduler_Check_Possible_Place_to_BUFFER_BM( BM_OUT , TRUE , 1 , &FM_OSlot , &FM_OSlot2 , &diff );
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	if ( FM_Count > 0 ) {
		//
		diff = Scheduler_Check_Possible_Pick_from_BM( CHECKING_SIDE , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE );
		//
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 23 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) && ( FM_OSlot > 0 ) && ( FM_OSlot2 <= 0 ) ) {
			//
		}
		else {
			//======================================================================
			if ( diff == 99 ) { // 2007.09.05
				return 1;
			} // 2007.09.05
			//======================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2011.01.24
				FM_P_Cooling = 1;
			}
			else { // 2011.01.24
				FM_P_Cooling = 0;
			}
			//
			if ( diff == 1 ) { // 2015.01.20
				//==========================================================================
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) && ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) ) diff = 90;
				//
				if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0;
				//
				if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) > 0 ) diff = 91;
				//
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) && ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) != BUFFER_FM_STATION ) ) diff = 92;
				//
				if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0;
				//
				if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) > 0 ) diff = 93;
				//==========================================================================
			}
			//
			if ( diff == 1 ) {
				//
				FM_Last_Moving_style_0 = FALSE; // 2010.02.20
				//
				if ( FM_TSlot2 <= 0 ) FM_Count = 1;
				if      ( FM_Count == 1 ) {
					//==========================================================================
//					if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0; // 2009.04.14 // 2015.01.20
					//==========================================================================
					FM_Side    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
					FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
					FM_Slot    = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
					//======================================================================
					switch( _SCH_MACRO_FM_OPERATION( 1 , MACRO_PICK , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 ) ) {
					case -1 :
						return 0;
						break;
					}
					//==========================================================================================
					_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
					//==========================================================================================
					if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2002 );
						}
					}
					//==========================================================================================
					switch( _SCH_MACRO_FM_OPERATION( 1 , MACRO_PLACE , BM_OUT , FM_Slot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 ) ) {
					case -1 :
						return 0;
						break;
					}
					//==========================================================================================
					_SCH_MODULE_Set_BM_SIDE( BM_OUT , FM_OSlot , FM_Side );
					_SCH_MODULE_Set_BM_POINTER( BM_OUT , FM_OSlot , FM_Pointer );
					_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot , FM_Slot , BUFFER_OUTWAIT_STATUS );
					//==========================================================================================
					if ( FM_P_Cooling == 1 ) { // 2011.01.23
						_SCH_MODULE_Set_MFIC_RUN_TIMER( FM_OSlot , 0 , _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , FALSE );
					}
					//==========================================================================================
				}
				else if ( FM_Count >= 2 ) {
					//======================================================================
					FM_Side     = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
					FM_Pointer  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
					FM_Slot     = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
					//======================================================================
					FM_Side2    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot2 );
					FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot2 );
					FM_Slot2    = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 );
					//======================================================================
					//
					FM_P_Buffer = 0;
					//
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 24 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
					//
					//======================================================================
					// 2007.11.14
					//======================================================================
					if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 0 ) {
						//======================================================================
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( BM_OUT , -1 ) <= 0 ) {
							//
							_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side , FM_Pointer , -1 , &FM_CO1 , &FM_P_Slot );
							_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side2 , FM_Pointer2 , -1 , &FM_CO2 , &FM_P_Slot2 );
							//
							//
							if ( FM_P_Cooling == 1 ) { // 2011.01.21
								//
								FM_P_Buffer = 0;
								//
							}
							else {
								if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO1 ) ) {
									if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO2 ) ) {
										if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 1 ) {
											FM_P_Buffer = 1; // FM1,FM1 : Direct_CM
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( BM_IN , -1 ) <= 0 ) {
												FM_P_Buffer = 1; // FM1,FM1 : Direct_CM
											}
											else {
												FM_P_Buffer = 0; // FM1,FM1 : Always_BM_OUT
											}
										}
									}
									else {
										FM_P_Buffer = 0; // FM1,FM2
									}
								}
								else {
									if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO2 ) ) {
										FM_P_Buffer = 3; // FM2,FM1 : Direct_CM
									}
									else {
										if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) > 1 ) {
											FM_P_Buffer = 2; // FM2,FM2 : Direct_CM
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( BM_IN , -1 ) <= 0 ) {
												FM_P_Buffer = 2; // FM2,FM2 : Direct_CM
											}
											else {
												FM_P_Buffer = 0; // FM2,FM2 : Always_BM_OUT
											}
										}
									}
								}
							}
							//
						}
						//======================================================================
					}
					//
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 25 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
					//
					//========================================================================================================
					if ( FM_P_Buffer == 0 ) { // Normal
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot , BM_OUT , FM_OSlot ) ) {
							return 0;
						}
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FM_Buffer , FM_TSlot2 , BM_OUT , FM_OSlot2 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						diff = 0;
						while( TRUE ) {
							FM_Res1 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 1 );
							FM_Res2 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 0 );
							if ( FM_Res1 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return 0;
							}
							if ( FM_Res2 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
								return 0;
							}
							//
							if ( diff == 0 ) {
								if ( ( ( FM_Res1 == 0 ) || ( FM_Res1 == 2 ) ) && ( ( FM_Res2 == 0 ) || ( FM_Res2 == 2 ) ) ) {
									//==========================================================================================
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
									//==========================================================================================
									diff = 1;
									//==========================================================================================
									if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2003 );
										}
									}
								}
							}
							if ( ( FM_Res1 == 0 ) && ( FM_Res2 == 0 ) ) break;
							Sleep(1);
						}
						//==========================================================================================
						_SCH_MODULE_Set_BM_SIDE( BM_OUT , FM_OSlot , FM_Side );
						_SCH_MODULE_Set_BM_POINTER( BM_OUT , FM_OSlot , FM_Pointer );
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot , FM_Slot , BUFFER_OUTWAIT_STATUS );
						//==========================================================================================
						_SCH_MODULE_Set_BM_SIDE( BM_OUT , FM_OSlot2 , FM_Side2 );
						_SCH_MODULE_Set_BM_POINTER( BM_OUT , FM_OSlot2 , FM_Pointer2 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot2 , FM_Slot2 , BUFFER_OUTWAIT_STATUS );
						//==========================================================================================
//							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 ); // 2008.01.22
//							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 ); // 2008.01.22
						//==========================================================================================
						if ( FM_P_Cooling == 1 ) {
							_SCH_MODULE_Set_MFIC_RUN_TIMER( FM_OSlot , FM_OSlot2 , _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , FALSE );
						}
						//==========================================================================================
						if ( diff == 0 ) {
							//
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 ); // 2008.01.22
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 ); // 2008.01.22
							//
							if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2004 );
								}
							}
						}
						//==========================================================================================
					}
					//========================================================================================================
					else if ( FM_P_Buffer == 1 ) { // FM1
						// FM2 pick BM S1 to BM_OUT + FM1 pick BM S2 to CM2
						// FM1 pick BM_OUT to CM1
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot , BM_OUT , FM_OSlot ) ) {
							return 0;
						}
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FM_Buffer , FM_TSlot2 , FM_CO2 , FM_P_Slot2 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						diff = 0;
						while( TRUE ) {
							FM_Res1 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 1 );
							FM_Res2 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 0 );
							if ( FM_Res1 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return 0;
							}
							if ( FM_Res2 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
								return 0;
							}
							//
							if ( diff == 0 ) {
								if ( ( ( FM_Res1 == 0 ) || ( FM_Res1 == 2 ) ) && ( ( FM_Res2 == 0 ) || ( FM_Res2 == 2 ) ) ) {
									//==========================================================================================
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
									//==========================================================================================
									diff = 1;
									//==========================================================================================
									if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2005 );
										}
									}
								}
							}
							if ( ( FM_Res1 == 0 ) && ( FM_Res2 == 0 ) ) break;
							Sleep(1);
						}
						//==========================================================================================
						_SCH_MODULE_Set_BM_SIDE( BM_OUT , FM_OSlot , FM_Side );
						_SCH_MODULE_Set_BM_POINTER( BM_OUT , FM_OSlot , FM_Pointer );
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot , FM_Slot , BUFFER_OUTWAIT_STATUS );
						//==========================================================================================
						if ( diff == 0 ) { // 2008.01.22
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
						}
						//==========================================================================================
						_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( 0 , 0 , 0 , 0 , 0 , FM_Slot2 , FM_CO2 , FM_P_Slot2 , FM_Side2 , FM_Pointer2 , TRUE );
						//==========================================================================================
						/*
						// 2008.01.08
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_OSlot , FM_CO1 , FM_P_Slot ) ) {
							return 0;
						}
						//==========================================================================================
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot , 0 , BUFFER_READY_STATUS );
						//==========================================================================================
						_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_Slot , FM_CO1 , FM_P_Slot , FM_Side , FM_Pointer , FM_Slot2 , FM_CO2 , FM_P_Slot2 , FM_Side2 , FM_Pointer2 , TRUE );
						//==========================================================================================
						*/
						//==========================================================================================
						if ( diff == 0 ) {
							if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2006 );
								}
							}
						}
						//==========================================================================================
					}
					//========================================================================================================
					else if ( FM_P_Buffer == 2 ) { // FM2
						// FM2 pick BM S1 to CM1 + FM1 pick BM S2 to BM_OUT
						// FM2 pick BM_OUT to CM2
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot , FM_CO1 , FM_P_Slot ) ) {
							return 0;
						}
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FM_Buffer , FM_TSlot2 , BM_OUT , FM_OSlot2 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						diff = 0;
						while( TRUE ) {
							FM_Res1 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 1 );
							FM_Res2 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 0 );
							if ( FM_Res1 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return 0;
							}
							if ( FM_Res2 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
								return 0;
							}
							//
							if ( diff == 0 ) {
								if ( ( ( FM_Res1 == 0 ) || ( FM_Res1 == 2 ) ) && ( ( FM_Res2 == 0 ) || ( FM_Res2 == 2 ) ) ) {
									//==========================================================================================
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
									//==========================================================================================
									diff = 1;
									//==========================================================================================
									if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2007 );
										}
									}
								}
							}
							if ( ( FM_Res1 == 0 ) && ( FM_Res2 == 0 ) ) break;
							Sleep(1);
						}
						//==========================================================================================
						_SCH_MODULE_Set_BM_SIDE( BM_OUT , FM_OSlot2 , FM_Side2 );
						_SCH_MODULE_Set_BM_POINTER( BM_OUT , FM_OSlot2 , FM_Pointer2 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot2 , FM_Slot2 , BUFFER_OUTWAIT_STATUS );
						//==========================================================================================
						if ( diff == 0 ) { // 2008.01.22
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
						}
						//==========================================================================================
						_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_Slot , FM_CO1 , FM_P_Slot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , TRUE );
						//==========================================================================================
						/*
						// 2008.01.08
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_OUT , FM_OSlot2 , FM_CO2 , FM_P_Slot2 ) ) {
							return 0;
						}
						//==========================================================================================
						_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_OSlot2 , 0 , BUFFER_READY_STATUS );
						//==========================================================================================
						_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_Slot , FM_CO1 , FM_P_Slot , FM_Side , FM_Pointer , FM_Slot2 , FM_CO2 , FM_P_Slot2 , FM_Side2 , FM_Pointer2 , TRUE );
						//==========================================================================================
						*/
						//==========================================================================================
						if ( diff == 0 ) {
							if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2008 );
								}
							}
						}
						//==========================================================================================
					}
					//========================================================================================================
					else if ( FM_P_Buffer == 3 ) { // All
						// FM2 pick BM S1 to CM1 + FM1 pick BM S2 to CM2
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot , FM_CO1 , FM_P_Slot ) ) {
							return 0;
						}
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FM_Buffer , FM_TSlot2 , FM_CO2 , FM_P_Slot2 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						diff = 0;
						while( TRUE ) {
							FM_Res1 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 1 );
							FM_Res2 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 0 );
							if ( FM_Res1 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return 0;
							}
							if ( FM_Res2 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
								return 0;
							}
							//
							if ( diff == 0 ) {
								if ( ( ( FM_Res1 == 0 ) || ( FM_Res1 == 2 ) ) && ( ( FM_Res2 == 0 ) || ( FM_Res2 == 2 ) ) ) {
									//==========================================================================================
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , 0 , -1 );
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , 0 , -1 );
									//==========================================================================================
									diff = 1;
									//==========================================================================================
									if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2009 );
										}
									}
								}
							}
							if ( ( FM_Res1 == 0 ) && ( FM_Res2 == 0 ) ) break;
							Sleep(1);
						}
						//==================================================================================
						_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_Slot , FM_CO1 , FM_P_Slot , FM_Side , FM_Pointer , FM_Slot2 , FM_CO2 , FM_P_Slot2 , FM_Side2 , FM_Pointer2 , TRUE );
						//==========================================================================================
						if ( diff == 0 ) {
							if ( Scheduler_Check_BM_Out_to_go_TMSIDE( FM_Buffer , BM_IN ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2010 );
								}
							}
						}
						//==========================================================================================
					}
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM CM & PLACE TO BM_IN PART (2)
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 31 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( BM_IN , -1 ) <= 0 ) {
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 32 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		FM_Res1 = Scheduler_Pick_from_CM_to_BM_IN( -1 , BM_IN , BM_OUT );
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 33 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( FM_Res1 == -1 ) {
			return 0;
		}
	}
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM BM_OUT & PLACE TO CM PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 41 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	FM_Res1 = Scheduler_Check_Possible_BM_BUFFER_MOVEMENT( CHECKING_SIDE , BM_IN , BM_OUT , TRUE );
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 41b = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//
	if ( FM_Res1 == 0 ) { // 2007.09.21
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 42 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		FM_Count = Scheduler_Check_Possible_Pick_from_BUFFER_BM( CHECKING_SIDE , TRUE , BM_OUT , &FM_TSlot , &FM_TSlot2 );
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 43 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( FM_Count == -1 ) { // 2011.01.23
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Failed for BM_OUT Pick Check 3%cWHFMFAIL 3\n" , 9 );
			return 0;
		}
		//
		if ( FM_Count >= 98 ) {
			return 1;
		}

		if ( FM_Count > 0 ) {
			//
			FM_Last_Moving_style_0 = FALSE; // 2010.02.20
			//
			if ( FM_TSlot2 <= 0 ) {
				//======================================================================
				FM_Side    = _SCH_MODULE_Get_BM_SIDE( BM_OUT , FM_TSlot );
				FM_Pointer = _SCH_MODULE_Get_BM_POINTER( BM_OUT , FM_TSlot );
				FM_Slot    = _SCH_MODULE_Get_BM_WAFER( BM_OUT , FM_TSlot );
				//
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side , FM_Pointer , -1 , &FM_CO1 , &FM_OSlot ); // 2007.07.11
				//======================================================================
				if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO1 ) ) {
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
						return 0;
					}
				}
				else {
					if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
						return 0;
					}
				}
				//==========================================================================================
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_TSlot , 0 , -1 );
				//==========================================================================================
			}
			else {
				//======================================================================
				FM_Side    = _SCH_MODULE_Get_BM_SIDE( BM_OUT , FM_TSlot );
				FM_Pointer = _SCH_MODULE_Get_BM_POINTER( BM_OUT , FM_TSlot );
				FM_Slot    = _SCH_MODULE_Get_BM_WAFER( BM_OUT , FM_TSlot );
				//
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side , FM_Pointer , -1 , &FM_CO1 , &FM_OSlot ); // 2007.07.11
				//======================================================================
				FM_Side2    = _SCH_MODULE_Get_BM_SIDE( BM_OUT , FM_TSlot2 );
				FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( BM_OUT , FM_TSlot2 );
				FM_Slot2    = _SCH_MODULE_Get_BM_WAFER( BM_OUT , FM_TSlot2 );
				//
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side2 , FM_Pointer2 , -1 , &FM_CO2 , &FM_OSlot2 ); // 2007.07.11
				//======================================================================
				if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO1 ) ) {
					if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO2 ) ) {
						diff = FALSE;
					}
					else {
						diff = TRUE;
					}
				}
				else {
					if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO2 ) ) {
						diff = TRUE;
					}
					else {
						diff = FALSE;
					}
				}
				//======================================================================
				if ( !diff ) {
					if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO1 ) ) {
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
							return 0;
						}
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_OUT , FM_TSlot2 , FM_CO2 , FM_OSlot2 ) ) {
							return 0;
						}
						//======================================================================
					}
					else {
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
							return 0;
						}
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_OUT , FM_TSlot2 , FM_CO2 , FM_OSlot2 ) ) {
							return 0;
						}
						//======================================================================
					}
				}
				else {
					if ( Scheduler_Check_Place_CM_FM1_Use( FM_CO1 ) ) {
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_OUT , FM_TSlot2 , FM_CO2 , FM_OSlot2 ) ) {
							return 0;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 ) ) {
							return 0;
						}
						//==================================================================================
					}
					else {
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 1 , FM_Slot , FM_Side , FM_Pointer , BM_OUT , FM_TSlot , FM_CO1 , FM_OSlot ) ) {
							return 0;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_STARTING( 0 , FM_Slot2 , FM_Side2 , FM_Pointer2 , BM_OUT , FM_TSlot2 , FM_CO2 , FM_OSlot2 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						if ( !_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 ) ) {
							return 0;
						}
						//==================================================================================
					}
				}
				//==========================================================================================
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_TSlot , 0 , -1 );
				//==========================================================================================
				_SCH_MODULE_Set_BM_WAFER_STATUS( BM_OUT , FM_TSlot2 , 0 , -1 );
				//==========================================================================================
			}
			//=================================================================================
			_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_TSlot , FM_CO1 , FM_OSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_CO2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 , TRUE ); // 2007.07.11
			//=================================================================================
		}
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// ARM INTERLOCK CHECK
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	Scheduler_Arm_Intlks_Check_For_BM( CHECKING_SIDE , 0 , BM_IN , BM_OUT );
	//-------------------------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 51 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
	//


	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// MOVE PICK FROM BM // 2010.02.20
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 121 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d][%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 , FM_Last_Moving_style_0 );
	//
	if ( !FM_Last_Moving_style_0 && _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
		//
		FM_Count = Scheduler_Check_Possible_Place_to_BUFFER_BM( BM_OUT , TRUE , 1 , &FM_OSlot , &FM_OSlot2 , &diff );
		//
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 122 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
		//
		if ( FM_Count > 0 ) {
			//
			diff = Scheduler_Check_Possible_Pick_from_BM( CHECKING_SIDE , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE );
			//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 123 = [FM_Buffer=%d][FM_Count=%d][diff=%d][FM_Side=%d,%d][FM_Pointer=%d,%d][FM_Slot=%d,%d][FM_TSlot=%d,%d][FM_OSlot=%d,%d][FM_CO=%d,%d][Res=%d,%d][P_Buf=%d,P_Sl=%d,%d]\n" , FM_Buffer , FM_Count , diff , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_Slot , FM_Slot2 , FM_TSlot , FM_TSlot2 , FM_OSlot , FM_OSlot2 , FM_CO1 , FM_CO2 , FM_Res1 , FM_Res2 , FM_P_Buffer , FM_P_Slot , FM_P_Slot2 );
			//
			if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) && ( FM_OSlot > 0 ) && ( FM_OSlot2 <= 0 ) ) {
				//
			}
			else {
				//======================================================================
				if ( diff == 1 ) {
					//
					FM_Last_Moving_style_0 = TRUE; // 2010.02.20
					//
					if ( FM_TSlot2 <= 0 ) FM_Count = 1;
					if      ( FM_Count == 1 ) {
						//==========================================================================
						FM_Side    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
						FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
						FM_Slot    = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
						//======================================================================
						switch( _SCH_MACRO_FM_MOVE_OPERATION( 1 , FM_Side , 0 , FM_Buffer , FM_Slot , 0 ) ) {
						case SYS_ABORTED :
							return 0;
							break;
						}
						//==========================================================================================
					}
					else if ( FM_Count >= 2 ) {
						//======================================================================
						FM_Side     = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
						FM_Pointer  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
						FM_Slot     = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
						//======================================================================
						FM_Side2    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot2 );
						FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot2 );
						FM_Slot2    = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 );
						//======================================================================
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 101 , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot , 0 , 0 ) ) {
							return 0;
						}
						if ( !_SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( 100 , FM_Slot2 , FM_Side2 , FM_Pointer2 , FM_Buffer , FM_TSlot2 , 0 , 0 ) ) {
							_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
							return 0;
						}
						//==================================================================================
						while( TRUE ) {
							FM_Res1 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 1 );
							FM_Res2 = _SCH_MACRO_FM_OPERATION_THREAD_RESULT( 0 );
							if ( FM_Res1 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 0 );
								return 0;
							}
							if ( FM_Res2 == -1 ) {
								_SCH_MACRO_FM_OPERATION_THREAD_WAITING( 1 );
								return 0;
							}
							//
							if ( ( FM_Res1 == 0 ) && ( FM_Res2 == 0 ) ) break;
							Sleep(1);
						}
						//==================================================================================
					}
				}
			}
		}
	}

	return 1;
}






#endif