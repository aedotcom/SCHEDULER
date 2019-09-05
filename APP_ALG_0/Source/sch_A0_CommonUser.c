#include "INF_Scheduler.h"

#include "sch_A0_default.h"
#include "sch_A0_init.h"
#include "sch_A0_dll.h"
#include "sch_A0_log.h"
#include "sch_A0_param.h"
#include "sch_A0_event.h"
#include "sch_A0_sub.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub_sp2.h"
#include "sch_A0_sub_sp4.h"
#include "sch_A0_sub_F_sw.h"

//===========================================================================================================
int ALG_DATA_SUB_STYLE_0 = 0;
//===========================================================================================================

int BatchAll_Unuse_BM[MAX_BM_CHAMBER_DEPTH]; // 2010.04.01
int MULTI_PM_USE; // 2014.01.10
BOOL MULTI_ALL_PM_FULLSWAP = FALSE; // 2018.10.20
int  MULTI_ALL_PM_LAST_PICK[MAX_TM_CHAMBER_DEPTH]; // 2018.10.20


int RUN_NOT_SELECT_COUNT[MAX_TM_CHAMBER_DEPTH]; // 2018.12.06

//----------------------------------------------------------------------------
// LEVEL 1
//----------------------------------------------------------------------------
BOOL _SCH_COMMON_CONFIG_INITIALIZE_STYLE_0( int runalg , int subalg , int guialg , int maxpm ) {
	//===============================================================================
	ALG_DATA_SUB_STYLE_0 = subalg;
	//===============================================================================
	Set_Sub_Alg_Data_STYLE_0( subalg );
	return TRUE;
}
//===========================================================================================================
void _SCH_COMMON_REVISION_HISTORY_STYLE_0( int mode , char *data , int count ) {
	USER_REVISION_HISTORY_AL0_PART( mode , data , count );
}
//===========================================================================================================
void _SCH_COMMON_INITIALIZE_PART_STYLE_0( int apmode , int side ) {
	int i , j , f;

	INIT_SCHEDULER_AL0_PART_DATA( apmode , side );
	//
	if ( apmode == 3 ) { // 2010.04.01
		f = FALSE;
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			BatchAll_Unuse_BM[i] = 0;
			if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( BM1 + i ) == 0 ) ) {
				for ( j = 0 ; j < _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
					if ( _SCH_IO_GET_MODULE_STATUS( BM1 + i , j + 1 ) > 0 ) {
						BatchAll_Unuse_BM[i] = 1;
						break;
					}
				}
				if ( j == _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ) f = TRUE;
			}
			else {
				BatchAll_Unuse_BM[i] = 2;
			}
		}
		//
		if ( !f ) {
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) BatchAll_Unuse_BM[i] = 0;
		}
		//
		// 2018.12.08
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) RUN_NOT_SELECT_COUNT[i] = 0;
		//
		// 2014.01.10
		//
		MULTI_PM_USE = FALSE;
		//
		for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
			//
			if ( !_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( i ) > 1 ) {
				MULTI_PM_USE = TRUE;
				break;
			}
			//
		}
		//
		//
		// 2018.10.20
		//
		MULTI_ALL_PM_FULLSWAP = FALSE; // 2018.10.20
		//
		for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
			//
			if ( !_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( i ) > 1 ) {
				MULTI_ALL_PM_FULLSWAP = TRUE;
			}
			else {
				MULTI_ALL_PM_FULLSWAP = FALSE;
				break;
			}
			//
		}
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) MULTI_ALL_PM_LAST_PICK[i] = 0;
		//
	}
	else if ( apmode == 1 ) { // 2010.04.01
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( BatchAll_Unuse_BM[i] == 2 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( BM1 + i ) == 0 ) ) {
					for ( j = 0 ; j < _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
						if ( _SCH_IO_GET_MODULE_STATUS( BM1 + i , j + 1 ) > 0 ) {
							BatchAll_Unuse_BM[i] = 1;
							break;
						}
					}
					if ( j == _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ) BatchAll_Unuse_BM[i] = 0;
				}
			}
		}
	}
	//
}
//===========================================================================================================
int SCHEDULER_CHECK_BATCHALL_SWITCH_BM_ONLY( int side , char *errorstring ) { // 2010.01.19
	int i , in = -1 , out = -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			//
			if ( in == -1 ) {
				in = _SCH_CLUSTER_Get_PathIn( side , i );
			}
			else {
				if ( in != _SCH_CLUSTER_Get_PathIn( side , i ) ) break;
			}
			//
			if ( out == -1 ) {
				out = _SCH_CLUSTER_Get_PathOut( side , i );
			}
			else {
				if ( out != _SCH_CLUSTER_Get_PathOut( side , i ) ) break;
			}
			//
		}
	}
	//
	if ( in  == -1 ) return ERROR_CASS_INPATH;
	if ( out == -1 ) return ERROR_CASS_OUTPATH;
	//
	if ( i < MAX_CASSETTE_SLOT_SIZE ) {
		SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( side , BATCHALL_SWITCH_LP_2_LP );
		return -1;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( _SCH_SYSTEM_USING_GET( i ) < 5 ) continue;
		//
		if      ( ( in <  PM1 ) && ( out <  PM1 ) ) { // LP -> LP 0
			switch( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( i ) ) {
			case BATCHALL_SWITCH_LL_2_LL : // LL -> LL 1
//			case BATCHALL_SWITCH_LP_2_LL : // LP -> LL 2 // 2010.05.04
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_JOB_PARAM_ERROR;
				break;
			}
		}
		else if ( ( in >= BM1 ) && ( out >= BM1 ) ) { // LL -> LL 1
			switch( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( i ) ) {
			case BATCHALL_SWITCH_LP_2_LP : // LP -> LP 0
			case BATCHALL_SWITCH_LP_2_LL : // LP -> LL 2
//			case BATCHALL_SWITCH_LL_2_LP : // LL -> LP 3 // 2010.05.04
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_JOB_PARAM_ERROR;
				break;
			}
		}
		else if ( ( in <  PM1 ) && ( out >= BM1 ) ) { // LP -> LL 2
			switch( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( i ) ) {
			case BATCHALL_SWITCH_LL_2_LL : // LL -> LL 1
			case BATCHALL_SWITCH_LL_2_LP : // LL -> LP 3
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_JOB_PARAM_ERROR;
				break;
			}
		}
		else if ( ( in >= BM1 ) && ( out <  PM1 ) ) { // LL -> LP 3
			switch( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( i ) ) {
			case BATCHALL_SWITCH_LP_2_LP : // LP -> LP 0
			case BATCHALL_SWITCH_LP_2_LL : // LP -> LL 2
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_JOB_PARAM_ERROR;
				break;
			}
		}
	}
	//
	if      ( ( in <  PM1 ) && ( out <  PM1 ) ) { // LP -> LP 0
		SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( side , BATCHALL_SWITCH_LP_2_LP );
	}
	else if ( ( in >= BM1 ) && ( out >= BM1 ) ) { // LL -> LL 1
		SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( side , BATCHALL_SWITCH_LL_2_LL );
	}
	else if ( ( in <  PM1 ) && ( out >= BM1 ) ) { // LP -> LL 2
		SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( side , BATCHALL_SWITCH_LP_2_LL );
	}
	else if ( ( in >= BM1 ) && ( out <  PM1 ) ) { // LL -> LP 3
		SCHEDULER_Set_BATCHALL_SWITCH_BM_ONLY( side , BATCHALL_SWITCH_LL_2_LP );
	}
	//
	if ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) == BATCHALL_SWITCH_LP_2_LP ) return -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( _SCH_SYSTEM_USING_GET( i ) < 5 ) continue;
		//
		if ( in >= BM1 ) {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == in ) {
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_CASS_INPATH;
			}
		}
		if ( out >= BM1 ) {
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == out ) {
				sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
				return ERROR_CASS_OUTPATH;
			}
		}
		//
		if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) > 0 ) {
			if ( in >= BM1 ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) == in ) {
					sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
					return ERROR_CASS_INPATH;
				}
			}
			if ( out >= BM1 ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) == out ) {
					sprintf( errorstring , "%s|%s" , _SCH_SYSTEM_GET_MODULE_NAME( in ) , _SCH_SYSTEM_GET_MODULE_NAME( out ) );
					return ERROR_CASS_OUTPATH;
				}
			}
		}
		//
	}
	//
	switch ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) ) {
	case BATCHALL_SWITCH_LL_2_LL : // LL -> LL 1
		SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , in );
		//
		_SCH_MODULE_Set_Mdl_Use_Flag( side , in , MUF_01_USE );
		//
		if ( in == out ) {
			SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
		}
		else {
			SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , out );
			//
			_SCH_MODULE_Set_Mdl_Use_Flag( side , out , MUF_01_USE );
			//
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2010.05.28
			if ( ( ( BM1 + i ) != in ) && ( ( BM1 + i ) != out ) ) {
				_SCH_MODULE_Set_Mdl_Use_Flag( side , BM1 + i , MUF_00_NOTUSE );
			}
		}
		break;
	case BATCHALL_SWITCH_LP_2_LL : // LP -> LL 2
		SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , out );
		SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
		//
		_SCH_MODULE_Set_Mdl_Use_Flag( side , out , MUF_01_USE );
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2010.05.28
			if ( ( BM1 + i ) != out ) {
				_SCH_MODULE_Set_Mdl_Use_Flag( side , BM1 + i , MUF_00_NOTUSE );
			}
		}
		break;
	case BATCHALL_SWITCH_LL_2_LP : // LL -> LP 3
		SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , in );
		SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
		//
		_SCH_MODULE_Set_Mdl_Use_Flag( side , in , MUF_01_USE );
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2010.05.28
			if ( ( BM1 + i ) != in ) {
				_SCH_MODULE_Set_Mdl_Use_Flag( side , BM1 + i , MUF_00_NOTUSE );
			}
		}
		break;
	}
	//
	return ERROR_NONE;
}
//===========================================================================================================
BOOL SCHEDULER_CONTROL_Arm_Intlks_Same( int bm1 , int bm2 ) { // 2015.10.02
	//
	if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0 , bm1 )  != _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0 , bm2 ) ) return FALSE;
	if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0 , bm1 ) != _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0 , bm2 ) ) return FALSE;
	//
	return TRUE;
	//
}



int _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_0( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	int i , j , k , RunWafer;

	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) ) { // 2007.10.03
				//
				k = _SCH_PRM_GET_MODULE_GROUP( i );
				//
				if ( k >= 0 ) { // 2014.09.24
					//
					if ( (*TMATM) < k ) *TMATM = k;
					for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
						if ( k >= _SCH_PRM_GET_MODULE_GROUP( j ) ) {
							if ( _SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) )
								_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
						}
					}
					//
				}
				//
			}
		}
	}
	//=========================================================================================
	// 2007.01.18
	//=========================================================================================
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , -1 ) ) break;
		}
	}
	if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( j , 0 ) ) continue; // 2015.07.16
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) continue;
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PICK , -1 ) ) continue;
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
			k = _SCH_PRM_GET_MODULE_GROUP( j );
			if ( (*TMATM) < k ) *TMATM = k;
		}
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) break;
		}
	}
	if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( j , 0 ) ) continue; // 2015.07.16
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) continue;
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PLACE , -1 ) ) continue;
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
			k = _SCH_PRM_GET_MODULE_GROUP( j );
			if ( (*TMATM) < k ) *TMATM = k;
		}
	}
	//------------------------------------------------------------------------------
	// 2006.05.25
	//------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) ) { // 2007.11.21
				for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
					if ( _SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) == 1 ) {
						if ( (*TMATM) < j ) *TMATM = j;
					}
				}
			}
		}
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) == 1 ) {
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) == 1 ) {
					if ( (*TMATM) < j ) *TMATM = j;
				}
			}
		}
	}
	//------------------------------------------------------------------------------
	for ( i = 0 ; i <= (*TMATM) ; i++ ) {
		_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
	}
	//===============================================================================================
	// 2010.03.14
	//===============================================================================================
	if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				//
				for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2010.04.01
					if ( BatchAll_Unuse_BM[i] != 0 ) {
						_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , BM1 + i , MUF_00_NOTUSE );
					}
				}
				//
				i = SCHEDULER_CHECK_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE , errorstring );
				if ( i != -1 ) return i;
			}
		}
	}
	//===============================================================================================
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
		switch( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( CHECKING_SIDE ) ) {
		case 0 :
			RunWafer = -1; // 2002.10.29
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2006.12.21
					if ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() > _SCH_PRM_GET_MODULE_SIZE( i ) ) {
						RunWafer = 1;
						break;
					}
				}
			}
			if ( RunWafer == -1 ) {
				//==================================================================================
				// 2007.01.05
				//==================================================================================
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
					if ( !Scheduler_Separate_Check_for_Batch_All( CHECKING_SIDE ) ) {
						//==================================================================================
						if ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) {
							RunWafer = 0;
						}
						else { // 2009.02.19
							if ( !_SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) RunWafer = 9998;
						}
						//==================================================================================
					}
					else {
						RunWafer = 9999;
					}
				}
			}
			else {
				//==================================================================================
				// 2007.10.29
				//==================================================================================
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) { // 2004.03.10
					switch( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) ) {
					case 0 :
					case 1 :
						_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE , 0 );
						break;
					case 2 :
					case 3 :
						_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE , 2 );
						break;
					case 4 :
					case 5 :
						_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE , 4 );
						break;
					}
				}
				//==================================================================================
			}
			//==========================================================================================
			if      ( RunWafer == 9999 ) {
				_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() , MUF_01_USE );
				_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch() , MUF_01_USE );
			}
			else if ( RunWafer == 9998 ) { // 2009.02.19 // batch
				//==================================
				SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , 0 );
				//==================================
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
						//
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[i-BM1] == 0 ) ) { // 2010.04.01
							_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
						}
						//
					}
				}
				//==================================
				SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , 0 );
				//==================================
			}
			else {
				if ( RunWafer == -1 ) {
					switch ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) / 2 ) {
					case 0 :
						RunWafer = CHECKING_SIDE + BM1;
						break;
					case 1 :
						RunWafer = 0 + BM1;
						break;
					case 2 :
						RunWafer = 1 + BM1;
						break;
					}
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[RunWafer-BM1] == 0 ) ) { // 2010.04.01
						if ( _SCH_MODULE_GET_USE_ENABLE( RunWafer , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( RunWafer ) == 0 ) ) { // 2006.12.21
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( RunWafer , G_PLACE , 0 ) ) { // 2013.07.26
								for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
									if ( j != CHECKING_SIDE ) {
										if ( _SCH_SYSTEM_USING_GET( j ) >= 5 ) {
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( j ) == RunWafer ) {
												RunWafer = -1;
												break;
											}
										}
									}
								}
							}
							else {
								RunWafer = -1; // 2013.07.26
							}
						}
						else {
							RunWafer = -1; // 2004.11.09
						}
					}
					else { // 2010.04.01
						RunWafer = -1;
					}
					//==========================================================================================
					if ( RunWafer < 0 ) {
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							//
							if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
							//
							if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[i-BM1] == 0 ) ) { // 2010.04.01
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2006.12.21
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) { // 2013.07.26
										RunWafer = i;
										for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
											if ( j != CHECKING_SIDE ) {
												if ( _SCH_SYSTEM_USING_GET( j ) >= 5 ) {
													if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( j ) == i ) {
														RunWafer = -1;
														j = 9999;
													}
												}
											}
										}
										if ( RunWafer >= 0 ) {
											break;
										}
									}
								}
							}
						}
					}
					if ( RunWafer < 0 ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2006.09.05
							RunWafer = 0;
						}
						else {
							j = -1;
							for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
								if ( i != CHECKING_SIDE ) {
									if ( _SCH_SYSTEM_USING_GET( i ) >= 5 ) {
										if ( ( j == -1 ) || ( j > _SCH_SYSTEM_RUNORDER_GET( i ) ) ) {
											j = _SCH_SYSTEM_RUNORDER_GET( i );
											RunWafer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i );
										}
									}
								}
							}
							if ( RunWafer < 0 ) {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									//
									if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
									//
									if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[i-BM1] == 0 ) ) { // 2010.04.01
										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2006.12.21
											if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) { // 2013.07.26
												RunWafer = i;
												break;
											}
										}
									}
								}
							}
						}
					}
					else { // 2008.10.30
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
							if ( ( lc_target_wafer_count / 1000 ) > 1 ) {
								RunWafer = 0;
							}
							else {
								/*
								// 2008.12.12
								for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
									if ( i != CHECKING_SIDE ) {
										if ( _SCH_SYSTEM_USING_GET( i ) >= 5 ) {
											RunWafer = 0;
											break;
										}
									}
								}
								*/
							}
						}
					}
					//
					if ( RunWafer < 0 ) return ERROR_TARGET_CHAMBER_ERROR;
					//==================================
					SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , RunWafer );
					//==================================
					if ( RunWafer == 0 ) { // 2006.09.05
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							//
							if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
							//
							if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[i-BM1] == 0 ) ) { // 2010.04.01
								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) { // 2013.07.26
										//
										_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
										//
										if ( _SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() == 0 ) { // 2018.03.21
											//
											if ( _SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) { // 2013.05.23
												if ( ( ( lc_target_wafer_count / 1000 ) <= 1 ) && ( ( lc_target_wafer_count % 1000 ) == 1 ) ) {
													//
													for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
														if ( i != j ) {
															if ( SCHEDULER_CONTROL_Arm_Intlks_Same( i , j ) ) { // 2015.10.02
																if ( _SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PLACE , 0 ) ) { // 2014.07.10
																	_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_00_NOTUSE );
																}
															}
														}
													}
													//
													break;
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
					else {
						_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , RunWafer , MUF_01_USE );
					}
					//==================================
					SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , 0 ); // 2003.11.28
				}
				else { // 2002.10.29
					SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , 0 );
					//==================================
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
						//
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( BatchAll_Unuse_BM[i-BM1] == 0 ) ) { // 2010.04.01
							if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
								//
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) { // 2013.07.26
									//
									_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
									//
									if ( _SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() == 0 ) { // 2018.03.21
										//
										if ( _SCH_SYSTEM_USING_STARTING_ONLY( CHECKING_SIDE ) ) { // 2013.05.23
											if ( ( ( lc_target_wafer_count / 1000 ) <= 1 ) && ( ( lc_target_wafer_count % 1000 ) == 1 ) ) {
												//
												for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
													if ( i != j ) {
														if ( SCHEDULER_CONTROL_Arm_Intlks_Same( i , j ) ) { // 2015.10.02
															if ( _SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PLACE , 0 ) ) { // 2014.07.10
																_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_00_NOTUSE );
															}
														}
													}
												}
												//
												break;
											}
										}
										//
									}
									//
								}
								//
							}
						}
					}
					SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , 0 ); // 2003.11.28
				}
			}
			break;
		default :
			if ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( CHECKING_SIDE ) > MAX_BM_CHAMBER_DEPTH ) { // 2003.11.28 // Cross
				RunWafer = _SCH_PRM_GET_MODULE_SWITCH_BUFFER( CHECKING_SIDE ) - MAX_BM_CHAMBER_DEPTH - 1 + BM1;
				//==================================
				if ( !_SCH_MODULE_GET_USE_ENABLE( RunWafer , FALSE , -1 ) ) return ERROR_TARGET_CHAMBER_ERROR; // 2013.04.10
				//==================================
				SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , RunWafer );
				//==================================
				_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , RunWafer , MUF_01_USE );
				//==================================
				if ( ( ( RunWafer - BM1 ) % 2 ) == 0 ) {
					//==================================
					if ( !_SCH_MODULE_GET_USE_ENABLE( RunWafer + 1 , FALSE , -1 ) ) return ERROR_TARGET_CHAMBER_ERROR; // 2013.04.10
					//==================================
					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , RunWafer + 1 , MUF_01_USE );
					SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , RunWafer + 1 );
				}
				else {
					//==================================
					if ( !_SCH_MODULE_GET_USE_ENABLE( RunWafer - 1 , FALSE , -1 ) ) return ERROR_TARGET_CHAMBER_ERROR; // 2013.04.10
					//==================================
					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , RunWafer - 1 , MUF_01_USE );
					SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , RunWafer - 1 );
				}
				//==================================
			}
			else {
				//
				RunWafer = _SCH_PRM_GET_MODULE_SWITCH_BUFFER( CHECKING_SIDE ) - 1 + BM1;
				//==================================
				if ( !_SCH_MODULE_GET_USE_ENABLE( RunWafer , FALSE , -1 ) ) return ERROR_TARGET_CHAMBER_ERROR; // 2013.04.10
				//==================================
				SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( CHECKING_SIDE , RunWafer );
				//==================================
				_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , RunWafer , MUF_01_USE );
				//==================================
				SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( CHECKING_SIDE , 0 ); // 2003.11.28
				//

				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2013.04.10
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
					//
					//==================================
					if ( i == RunWafer ) continue;
					//==================================
					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
					//==================================
				}
				//
			}
			break;
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		for ( i = 0 ; i <= (*TMATM) ; i++ ) {
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
		}
	}
	else {
		//=====================================================================================================
		// 2009.09.19
		//=====================================================================================================
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_00_NOTUSE ) continue;
				//
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
						if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , i , CHECKING_SIDE , j , BUFFER_INWAIT_STATUS ) ) break;
						if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , i , CHECKING_SIDE , j , BUFFER_OUTWAIT_STATUS ) ) break;
					}
				}
				//
				if ( j == MAX_CASSETTE_SLOT_SIZE ) {
					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
				}
			}
		}
		//=====================================================================================================
		//=====================================================================================================
		// 2007.12.08
		//=====================================================================================================
		//
		if ( _SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() == 0 ) { // 2018.03.21
			//
			if ( ( lc_target_wafer_count % 1000 ) == 1 ) {
				//
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( i == CHECKING_SIDE ) continue;
					if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) {
	//					continue; // 2010.03.01
						if ( !_SCH_SYSTEM_USING_STARTING( i ) ) continue; // 2010.03.01
					}
					else { // 2010.06.17
						if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
							k = 0;
							for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
								//
								if ( SCHEDULING_ThisIs_BM_OtherChamber( j , 0 ) ) continue; // 2015.07.16
								//
								if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , j ) == MUF_01_USE ) { // 2010.07.07
	//							if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) { // 2010.07.07
									if ( _SCH_PRM_GET_MODULE_GROUP( j ) == 0 ) { // 2010.07.07
	//								if ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) { // 2010.07.07
	//									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) { // 2010.07.07
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PLACE , -1 ) ) { // 2010.07.07
	//										k = 1; // 2010.07.07
	//										break; // 2010.07.07
											k++; // 2010.07.07
										}
									}
								}
							}
							//
	//						if ( k == 0 ) continue; // 2010.07.07
							if ( k <= 1 ) continue; // 2010.07.07
						}
					}
	//				if ( !_SCH_MODULE_Chk_FM_Finish_Status( i ) ) break; // 2010.02.23
					break; // 2010.02.23
				}
				if ( i == MAX_CASSETTE_SIDE ) {
					j = 0;
					k = 0; // 2010.06.24
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
							if ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) {
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) {
									if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) == _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
	//									if ( j == 0 ) { // 2010.06.24
	//										j = i;
	//									}
	//									else {
	//										j = 100 + i;
	//									}
										if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) { // 2010.06.24
											//
											_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
											//
											if ( k == 0 ) k = i;
										}
										else {
											if ( j == 0 ) { // 2010.06.24
												j = i;
											}
											else {
												_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
											}
										}
									}
								}
							}
						}
					}
					//
					if ( ( j == 0 ) && ( k != 0 ) ) _SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , k , MUF_01_USE ); // 2010.06.24
					//
	//				if ( j >= 100 ) { // 2010.06.24
	//					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j - 100 , MUF_00_NOTUSE ); // 2010.06.24
	//				} // 2010.06.24
					//
					j = 0;
					k = 0; // 2010.06.24
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2015.07.16
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
							if ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) {
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , -1 ) ) {
									if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) == _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
	//									if ( j == 0 ) { // 2010.06.24
	//										j = i;
	//									}
	//									else {
	//										j = 100 + i;
	//									}
										if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) { // 2010.06.24
											//
											_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
											//
											if ( k == 0 ) k = i;
										}
										else {
											if ( j == 0 ) { // 2010.06.24
												j = i;
											}
											else {
												_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
											}
										}
									}
								}
							}
						}
					}
					//
					if ( ( j == 0 ) && ( k != 0 ) ) _SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , k , MUF_01_USE ); // 2010.06.24
					//
	//				if ( j >= 100 ) { // 2010.06.24
	//					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j - 100 , MUF_00_NOTUSE ); // 2010.06.24
	//				} // 2010.06.24
					//
				}
			}
		}
/*
		//=====================================================================================================
		// 2009.08.27
		//=====================================================================================================
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_00_NOTUSE ) continue;
				//
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
						if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , i , CHECKING_SIDE , j , BUFFER_INWAIT_STATUS ) ) break;
						if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , i , CHECKING_SIDE , j , BUFFER_OUTWAIT_STATUS ) ) break;
					}
				}
				//
				if ( j == MAX_CASSETTE_SLOT_SIZE ) {
					_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
				}
			}
		}
		//=====================================================================================================
*/
	}
	//==========================================================================================================================================================================================================
	//==========================================================================================================================================================================================================
	//==========================================================================================================================================================================================================
	//==========================================================================================================================================================================================================
	if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2006.12.21
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
				//========================================================================
				RunWafer = 0;
				j = 0;
				strcpy( errorstring , "UNDEFINED" );
				//========================================================================
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2010.08.11
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , -1 ) ) break;
					}
				}
				if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
					//-------------------------------------------
					if ( RunWafer == 0 ) RunWafer = 1;
					//
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( _SCH_PRM_GET_MODULE_MODE( i ) ) { // 2007.12.17
							//
							if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2010.08.17
							//
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , -2 ) ) {
								//
								if ( j != 0 ) strcat( errorstring , "|" );
								else          strcpy( errorstring , "" );
								strcat( errorstring , _SCH_SYSTEM_GET_MODULE_NAME( i ) );
								j++;
								//
							}
						}
					}
					//-------------------------------------------
				}
				//========================================================================
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2010.08.11
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) break;
					}
				}
				if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
					//-------------------------------------------
					if ( RunWafer == 0 ) RunWafer = 2;
					//
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( _SCH_PRM_GET_MODULE_MODE( i ) ) { // 2007.12.17
							//
							if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2010.08.17
							//
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -2 ) ) {
								//
								if ( j != 0 ) strcat( errorstring , "|" );
								else          strcpy( errorstring , "" );
								strcat( errorstring , _SCH_SYSTEM_GET_MODULE_NAME( i ) );
								j++;
								//
							}
						}
					}
					//-------------------------------------------
				}
				//========================================================================
				if      ( RunWafer == 1 ) {
//						if ( !PreChecking ) _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber Buffer Out try access%cRECIPEFAIL %s:%d\n" , 9 , RunNameAll , ERROR_TARGET_CHAMBER_ERROR );
					return ERROR_TARGET_CHAMBER_ERROR;
				}
				else if ( RunWafer == 2 ) {
//						if ( !PreChecking ) _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber Buffer In try access%cRECIPEFAIL %s:%d\n" , 9 , RunNameAll , ERROR_TARGET_CHAMBER_ERROR );
					return ERROR_TARGET_CHAMBER_ERROR;
				}
				//========================================================================
			}
		}
		else {
			//===========================================================================
			// 2008.03.10
			//===========================================================================
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				//
				i = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE );
				j = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE );
				//
				if ( i == 0 ) {
					j = 0;
					strcpy( errorstring , "UNDEFINED" );
					//
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2010.08.11
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
							RunWafer = 1;
							break;
						}
						if ( _SCH_PRM_GET_MODULE_MODE( i ) ) {
							//
							if ( j != 0 ) strcat( errorstring , "|" );
							else          strcpy( errorstring , "" );
							strcat( errorstring , _SCH_SYSTEM_GET_MODULE_NAME( i ) );
							j++;
							//
						}
					}
					if ( RunWafer == 0 ) return ERROR_TARGET_CHAMBER_ERROR;
				}
				else {
					//===========================================================================
					if ( j == 0 ) j = i;
					//===========================================================================
					if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
						strcpy( errorstring , _SCH_SYSTEM_GET_MODULE_NAME( i ) );
						return ERROR_TARGET_CHAMBER_ERROR;
					}
					//===========================================================================
					if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) {
						strcpy( errorstring , _SCH_SYSTEM_GET_MODULE_NAME( j ) );
						return ERROR_TARGET_CHAMBER_ERROR;
					}
					//===========================================================================
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( ( k != i ) && ( k != j ) ) {
							_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , k , MUF_00_NOTUSE );
						}
					}
					//===========================================================================
				}
			}
			//===========================================================================
		}
	}
	return ERROR_NONE;
}

void _SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_0( int side , int bmc , int bmmode ) {
}

//----------------------------------------------------------------------------
// LEVEL 2
//----------------------------------------------------------------------------
void _SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_0( HINSTANCE dllpt , int unload ) {
	INIT_SCHEDULER_AL0_PART_SET_DLL_INSTANCE( dllpt , unload );
}

BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_0( int chg_ALG_DATA_RUN ) {
	return TRUE;
}

BOOL _SCH_COMMON_PRE_PART_OPERATION_STYLE_0( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
	int		i;

	if      ( mode == 0 ) {
	}
	else if ( mode == 1 ) {
		/*
		// 2013.05.03
		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) { // 2006.12.21
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( dataflag[i] != 0 ) { // 2002.08.27
						if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) ) ) { // 2006.12.21
							if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_MODE ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_MODE ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_S_MODE ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
							else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_S_MODE ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
						}
					}
				}
			}
			else { // 2013.03.18
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( dataflag[i] != 0 ) {
						//
//						if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) { // 2013.04.30
							if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_SWITCH_MODE ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
//						} // 2013.04.30
					}
				}
			}
		}
		*/
		// 2013.05.03
		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( dataflag[i] != 0 ) { // 2002.08.27
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_S_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_S_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					//
				}
			}
		}
	}
	else if ( mode == 2 ) {
	}
	else {
		return FALSE;
	}
	
	return TRUE;
}

BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_0( int side , int WaitParam , int option ) {
	return FALSE;
}

//----------------------------------------------------------------------------
// LEVEL 3
//----------------------------------------------------------------------------
int _SCH_COMMON_GUI_INTERFACE_STYLE_0( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
	// style
	// 0	:	ITEM_DISPLAY
	// 1	:	DATA_DISPLAY
	// 2	:	DATA_CONTROL
	// 3	:	DATA_GET
	// mode
	// 0	:	N/A
	// 1	:	Put Priority Check
	// 2	:	Get Priority Check
	// 3	:	BM Scheduling Factor
	// 4	:	Switching Side
	// 5	:	Next Pick
	// 6	:	Prev Pick

	switch( style ) {
		case 0 :
			//
			switch( mode ) {
				case 1 : strcpy( Message , "Chamber Put Priority Check" );	return 1;	break;		// Put Priority Check
				case 2 : strcpy( Message , "Chamber Get Priority Check" );	return 1;	break;		// Get Priority Check
				case 3 : strcpy( Message , "BM Scheduling Factor" );		return 1;	break;		// BM Scheduling Factor
				case 4 :
					if ( data == 0 ) { // 2010.07.22
						strcpy( Message, "-" ); // 2010.07.22
					}
					else if ( data <= MAX_CASSETTE_SIDE ) {
						if      ( data == 1 ) {
							strcpy( Message, "LP->LL" ); // 2010.07.22
						}
						else if ( data == 2 ) {
							strcpy( Message, "LL->LP" ); // 2010.07.22
						}
						else {
							strcpy( Message, "-" ); // 2010.07.22
						}
					}
					else {
						sprintf( Message, "B/PM%d" , data - MAX_CASSETTE_SIDE ); // 2010.07.22
					}
																			return 1;	break;		// Switching Side
				case 5 : strcpy( Message , "" );							return 1;	break;		// Next Pick
				case 6 : strcpy( Message , "" );							return 1;	break;		// Prev Pick
			}
			//
			break;

		case 1 :
			//
			switch( mode ) {
				case 1 : //	Put Priority Check
					//======================================================================================================================================================================
						strcpy( list , "Priority Only|Priority after Switch|Priority Only(A)|Priority after Swh(A)" );
						strcpy( Message , "????" );
						return 1;
					break;
					//======================================================================================================================================================================
				case 2 : // Get Priority Check
					//======================================================================================================================================================================
						strcpy( list , "Priority Only|First Ordering|Priority(CM Switch)|First(CM Switch)" );
						strcpy( Message , "????" );
						return 1;
					break;
					//======================================================================================================================================================================
				case 3 : // BM Scheduling Factor
					//======================================================================================================================================================================
//						strcpy( list , ".|PT<->Batch|PT<->FullSwap|PT<->SingleSwap|Batch|FullSwap|SingleSwap|PT<->MiddleSwap|MidleSwap" ); // 2015.03.02
						strcpy( list , ".|PT<->Batch|PT<->FullSwap|PT<->SingleSwap|Batch|FullSwap|SingleSwap|PT<->MiddleSwap|MidleSwap|PathThru1|PathThru2" ); // 2015.03.02
						strcpy( Message , "????" );
						return 1;
					break;
					//======================================================================================================================================================================
				case 4 : // Switching Side
					//======================================================================================================================================================================
//						strcpy( list , "Chamber All|Chamber 1 Only|Chamber 2 Only" );
//						strcpy( Message , "????" );
//						return 1;
					if ( *retdata == 0 ) { // 2010.07.22
						strcpy( list, "-" ); // 2010.07.22
						strcpy( Message, "-" ); // 2010.07.22
					}
					else if ( *retdata <= MAX_CASSETTE_SIDE ) {
						if      ( *retdata == 1 ) {
							strcpy( list , "Separate|LL Mode|LP Mode" );
							strcpy( Message , "????" );
						}
						else if ( *retdata == 2 ) {
							strcpy( list , "Separate|LL Mode|LP Mode" );
							strcpy( Message , "????" );
						}
						else {
							strcpy( list, "-" );
							strcpy( Message, "-" );
						}
					}
					else {
						strcpy( list , "Chamber All|Chamber 1 Only|Chamber 2 Only" );
						strcpy( Message , "????" );
					}
					return 1;
					break;
					//======================================================================================================================================================================
				case 5 : // Next Pick
					//======================================================================================================================================================================
						strcpy( list , "" );
						switch( data ) {
							case 0 :	strcpy( Message , "Always" );						break;
							case 1 :	strcpy( Message , "Process Free" );					break;
							case 2 :	strcpy( Message , "Wafer Free" );					break;
							case 3 :	strcpy( Message , "Process+Wafer Free" );			break;
							case 4 :	strcpy( Message , "Out Process Pick Deny" );		break;
							case 5 :	strcpy( Message , "Out Process Pick+Move Deny" );	break;
							case 6 :	strcpy( Message , "Out Process Only Move Deny" );	break;
							case 7 :	strcpy( Message , "Process+Wafer Free(ALL)" );		break;	// 2004.12.14
							case 8 :	strcpy( Message , "Process Free(LX)" );				break;	// 2005.03.29
							case 9 :	strcpy( Message , "Wafer Free(LX)" );				break;	// 2005.03.29
							case 10 :	strcpy( Message , "Process+Wafer Free(LX)" );		break;	// 2005.03.29
							case 11 :	strcpy( Message , "Process Free(A1)" );				break;	// 2006.01.26
							case 12 :	strcpy( Message , "Wafer Free(A1)" );				break;	// 2006.01.26
							case 13 :	strcpy( Message , "Process+Wafer Free(A1)" );		break;	// 2006.01.26
							case 14 :	strcpy( Message , "Process Free(A1.G0)" );			break;	// 2006.02.08
							case 15 :	strcpy( Message , "Wafer Free(A1.G0)" );			break;	// 2006.02.08
							case 16 :	strcpy( Message , "Process+Wafer Free(A1.G0)" );	break;	// 2006.02.08
							case 17 :	strcpy( Message , "Count Free" );					break;	// 2006.07.13
							case 18 :	strcpy( Message , "Direct Switchable" );			break;	// 2007.12.12
							case 101 :	strcpy( Message , "User Define 001" );				break;	// 2006.04.15
							case 102 :	strcpy( Message , "User Define 002" );				break;	// 2006.04.15
							default :	strcpy( Message , "????" );							break;
						}
						return 2;
					break;
					//======================================================================================================================================================================
				case 6 : // Prev Pick
					//======================================================================================================================================================================
					strcpy( list , "Always|Process Free|Wafer Free|Process+Wafer Free" );
					strcpy( Message , "Always" );
					return 1;
					break;
					//======================================================================================================================================================================
			}
			break;

		case 2 :
			//
			switch( mode ) {
				case 1 : //	Put Priority Check
						strcpy( Message , "Chamber Put Priority Check Order?" );
						strcpy( list , "Priority Only|Priority Check after Switch|Priority Only(Aging)|Priority Check after Switch(Aging)" );
						return 1;
						break;
				case 2 : // Get Priority Check
						strcpy( Message , "Chamber Get Priority Check Order?" );
						strcpy( list , "Priority Only|First Ordering|Priority Only(CM Switch)|First Ordering(CM Switch)" );
						return 1;
						break;
				case 3 : // BM Scheduling Factor
						switch( *retdata ) {
							case 1  : *retdata = 1; break; // PathThru<->Batch
							case 2  : *retdata = 2; break; // PathThru<->FullSwap
							case 3  : *retdata = 3; break; // PathThru<->SingleSwap
							case 4  : *retdata = 5; break; // Batch
							case 5  : *retdata = 6; break; // FullSwap
							case 6  : *retdata = 7; break; // SingleSwap
							case 7  : *retdata = 4; break; // PathThru<->MiddleSwap
							case 8  : *retdata = 8; break; // MiddleSwap
							case 9  : *retdata = 9; break; // PathThru1 // 2015.03.03
							case 10 : *retdata = 10; break; // PathThru2 // 2015.03.03
							default : *retdata = 0; break; // Default
						}

						strcpy( Message , "BM Switch Scheduling Factor?" );
//						strcpy( list , "Default|PathThru<->Batch|PathThru<->FullSwap|PathThru<->SingleSwap|PathThru<->MiddleSwap|Batch|FullSwap|SingleSwap|MiddleSwap" ); // 2015.03.03
						strcpy( list , "Default|PathThru<->Batch|PathThru<->FullSwap|PathThru<->SingleSwap|PathThru<->MiddleSwap|Batch|FullSwap|SingleSwap|MiddleSwap|PathThru1|PathThru2" ); // 2015.03.03
						return 4;
						break;
				case 4 : // Switching Side
						*retdata = 2;
						return 3;
						break;
				case 5 : // Next Pick
						if      ( *retdata == 0 ) *retdata = 1;
						else if ( *retdata == 1 ) *retdata = 2;
						else if ( *retdata == 2 ) *retdata = 3;
						else if ( *retdata == 3 ) *retdata = 4;
						else if ( *retdata == 4 ) *retdata = 5;
						else if ( *retdata == 5 ) *retdata = 6;
						else if ( *retdata == 6 ) *retdata = 7;
						else if ( *retdata == 7 ) *retdata = 8;		// 2004.12.14
						else if ( *retdata == 8 ) *retdata = 9;		// 2005.03.29
						else if ( *retdata == 9 ) *retdata = 10;	// 2005.03.29
						else if ( *retdata ==10 ) *retdata = 11;	// 2005.03.29
						else if ( *retdata ==11 ) *retdata = 12;	// 2006.01.26
						else if ( *retdata ==12 ) *retdata = 13;	// 2006.01.26
						else if ( *retdata ==13 ) *retdata = 14;	// 2006.01.26
						else if ( *retdata ==14 ) *retdata = 15;	// 2006.02.08
						else if ( *retdata ==15 ) *retdata = 16;	// 2006.02.08
						else if ( *retdata ==16 ) *retdata = 17;	// 2006.04.15
						else if ( *retdata ==17 ) *retdata = 18;	// 2007.12.12
						else if ( *retdata ==18 ) *retdata = 101;	// 2006.07.13
						else if ( *retdata ==101) *retdata = 102;	// 2006.04.15
						else if ( *retdata ==102) *retdata = 0;		// 2006.04.15
						return 2;
					break;

				case 6 : // Prev Pick
						*retdata = 3;
						return 3;
					break;
			}
			break;

		case 3 :
			//
			switch( mode ) {
				case 1 : //	Put Priority Check
						return 0;
					break;
				case 2 : // Get Priority Check
						return 0;
					break;
				case 3 : // BM Scheduling Factor
						return 0;
					break;
				case 4 : // Switching Side
					if ( *retdata == 0 ) { // 2010.07.22
						sprintf( Message , "-" );
					}
					else if ( *retdata <= MAX_CASSETTE_SIDE ) {
						if      ( *retdata == 1 ) {
							switch( data )	{
								case 1 :	sprintf( Message , "LL Mode" );		break;
								case 2 :	sprintf( Message , "LP Mode" );		break;
								default :	sprintf( Message , "Separate" );	break;
							}
						}
						else if ( *retdata == 2 ) {
							switch( data )	{
								case 1 :	sprintf( Message , "LL Mode" );		break;
								case 2 :	sprintf( Message , "LP Mode" );		break;
								default :	sprintf( Message , "Separate" );	break;
							}
						}
						else {
							sprintf( Message , "-" );
						}
					}
					else {
						if  ( _SCH_PRM_GET_MODULE_MODE( ( *retdata - MAX_CASSETTE_SIDE - 1 ) + PM1 ) ) {
							switch( data )	{
								case 1 :	sprintf( Message , "Chamber 1 Only" );	break;
								case 2 :	sprintf( Message , "Chamber 2 Only" );	break;
								default :	sprintf( Message , "Chamber All" );		break;
							}
						}
						else {
							sprintf( Message , "-" );
						}
					}
					return 1;
					break;

				case 5 : // Next Pick
						switch ( data ) {
							case 0 : sprintf( Message , "" );			break;
							case 1 : sprintf( Message , "PF" );			break;
							case 2 : sprintf( Message , "WF" );			break;
							case 3 : sprintf( Message , "AF" );			break;
							case 4 : sprintf( Message , "OPF" );		break;
							case 5 : sprintf( Message , "OAF" );		break;
							case 6 : sprintf( Message , "OMF" );		break;
							case 7 : sprintf( Message , "FF" );			break; // 2004.12.14
							case 8 : sprintf( Message , "PF.lx" );		break; // 2005.03.29
							case 9 : sprintf( Message , "WF.lx" );		break; // 2005.03.29
							case 10 : sprintf( Message , "AF.lx" );		break; // 2005.03.29
							case 11 : sprintf( Message , "PF.A1" );		break; // 2006.01.26
							case 12 : sprintf( Message , "WF.A1" );		break; // 2006.01.26
							case 13 : sprintf( Message , "AF.A1" );		break; // 2006.01.26
							case 14 : sprintf( Message , "PF.A1G0" );	break; // 2006.02.08
							case 15 : sprintf( Message , "WF.A1G0" );	break; // 2006.02.08
							case 16 : sprintf( Message , "AF.A1G0" );	break; // 2006.02.08
							case 17 : sprintf( Message , "CF" );		break; // 2006.07.13
							case 18 : sprintf( Message , "DSWITCH" );	break; // 2007.12.12
							case 101: sprintf( Message , "UD001" );		break; // 2006.04.15
							case 102: sprintf( Message , "UD002" );		break; // 2006.04.15
							default : sprintf( Message , "????" );		break;
						}
						return 1;
					break;

				case 6 : // Prev Pick
						switch ( data ) {
							case 0 : sprintf( Message , "" );		break;
							case 1 : sprintf( Message , "PF" );		break;
							case 2 : sprintf( Message , "WF" );		break;
							case 3 : sprintf( Message , "AF" );		break;
							default : sprintf( Message , "????" );	break;
						}
						return 1;
					break;
			}
			break;

		case 4 :
			//
			switch( mode ) {
				case 1 : //	Put Priority Check
						return 0;
					break;
				case 2 : // Get Priority Check
						return 0;
					break;
				case 3 : // BM Scheduling Factor
						switch( *retdata ) 
						{
							case 1  : *retdata = 1; break; // PathThru<->Batch
							case 2  : *retdata = 2; break; // PathThru<->FullSwap
							case 3  : *retdata = 3; break; // PathThru<->SingleSwap
							case 4  : *retdata = 7; break; // PathThru<->MiddleSwap
							case 5  : *retdata = 4; break; // Batch
							case 6  : *retdata = 5; break; // FullSwap
							case 7  : *retdata = 6; break; // SingleSwap
							case 8  : *retdata = 8; break; // MiddleSwap
							case 9  : *retdata = 9; break; // PathThru1
							case 10 : *retdata = 10; break; // PathThru2
							default : *retdata = 0; break; // Default
						}
						return 1;
					break;
				case 4 : // Switching Side
						return 0;
					break;
				case 5 : // Next Pick
						return 0;
					break;
				case 6 : // Prev Pick
						return 0;
					break;
			}
			break;

		default :
			break;
	}
	return 0;
}

int _SCH_COMMON_EVENT_ANALYSIS_STYLE_0( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_ANALYSIS_AL0( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}

int _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_0( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_PRE_ANALYSIS_AL0( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}

void _SCH_COMMON_SYSTEM_DATA_LOG_STYLE_0( char *filename , int side ) {
	USER_SYSTEM_LOG_AL0( filename , side );
}

int  _SCH_COMMON_TRANSFER_OPTION_STYLE_0( int mode , int data ) {
	int i , j;
	if      ( mode == 0 ) {
		return 0;
	}
	else if ( mode == 1 ) {
		return Get_Prm_MODULE_BUFFER_SWITCH_MODE();
	}
	else if ( mode == 2 ) {
		return Get_Prm_MODULE_BUFFER_SINGLE_MODE();
	}
	else if ( mode == 3 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.10.01
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( data , TRUE , &i , &j );
		}
		else {
			i = 1;
		}
		return i;
	}
	else if ( mode == 4 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.10.01
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( data , TRUE , &i , &j );
		}
		else {
			j = _SCH_PRM_GET_MODULE_SIZE( data );
		}
		return j;
	}
	else if ( mode == 5 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.10.01
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( data , FALSE , &i , &j );
		}
		else {
			i = 1;
		}
		return i;
	}
	else if ( mode == 6 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.10.01
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( data , FALSE , &i , &j );
		}
		else {
			j = _SCH_PRM_GET_MODULE_SIZE( data );
		}
		return j;
	}
	else if ( mode == 7 ) {
		return SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( data / 100 , data % 100 );
	}
	else if ( mode == 8 ) {
		return SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( data );
	}

	return 0;
}
//----------------------------------------------------------------------------
// LEVEL 4
//----------------------------------------------------------------------------
BOOL _SCH_COMMON_FINISHED_CHECK_STYLE_0( int side ) {
	if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( _SCH_MODULE_Get_FM_SwWait( side ) >= 3 ) ) { // 2006.12.21
			if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) { // 2006.03.03
				if ( SIGNAL_MODE_APPEND_END_GET( side ) == 0 ) return FALSE;
			}
		}
	}
	return TRUE;
}

// 0 : Recipe Read / Slot Overlapped
// 1 : Map Read / Map info check and Unuse DB reset
// 2 : Map Read / Slot Overlapped or No Wafer
// 3 : Map First
// 4 : Map Switch
// 5 : Map End / Verification
// 6 : CM Module Check
// 7 : CM MaxSlot Check
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_0( int mode , int side , int cm1 , int cm2 , int option ) {
	int RunWafer;

	if      ( mode == 0 ) {
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO3 ) {
			return FALSE;
		}
	}
	else if ( mode == 1 ) {
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
			return FALSE;
		}
	}
	else if ( mode == 2 ) {
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
			return FALSE;
		}
	}
	else if ( mode == 3 ) {
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
			SCHEDULER_CONTROL_CHECK_RUN_START_RESTORE_A0SUB4( side );
		}
		//
		// 2010.02.02
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				if (
					( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) == BATCHALL_SWITCH_LP_2_LL ) ||
					( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) == BATCHALL_SWITCH_LL_2_LP )
					) {
					//
					_SCH_SYSTEM_MODE_LOOP_SET( side , 1 );
					//
					_SCH_IO_SET_LOOP_COUNT( side , 1 );
				}
			}
		}
		//
	}
	else if ( mode == 4 ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2008.10.30
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				//
				if ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) != BATCHALL_SWITCH_LP_2_LP ) { // 2010.02.01
					RunWafer = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side );
					if ( RunWafer != 0 ) {
						SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) );
						_SCH_MODULE_Set_BM_FULL_MODE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) , BUFFER_WAIT_STATION );
						SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , RunWafer );
						_SCH_MODULE_Set_BM_FULL_MODE( RunWafer , BUFFER_WAIT_STATION );
					}
					else {
						RunWafer = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side );
						if ( RunWafer != 0 ) {
							_SCH_MODULE_Set_BM_FULL_MODE( RunWafer , BUFFER_WAIT_STATION );
						}
					}
				}
				else {
					switch( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) ) { // 2009.07.24
					case 0 :
						SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , 0 );
						SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
						break;
					default : // 2009.07.24
						if ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) > MAX_BM_CHAMBER_DEPTH ) {
							RunWafer = _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) - MAX_BM_CHAMBER_DEPTH - 1 + BM1;
							//==================================
							SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , RunWafer );
							//==================================
							_SCH_MODULE_Set_Mdl_Use_Flag( side , RunWafer , MUF_01_USE );
							//==================================
							if ( ( ( RunWafer - BM1 ) % 2 ) == 0 ) {
								_SCH_MODULE_Set_Mdl_Use_Flag( side , RunWafer + 1 , MUF_01_USE );
								SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , RunWafer + 1 );
							}
							else {
								_SCH_MODULE_Set_Mdl_Use_Flag( side , RunWafer - 1 , MUF_01_USE );
								SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , RunWafer - 1 );
							}
							//==================================
						}
						else {
							RunWafer = _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) - 1 + BM1;
							//==================================
							SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , RunWafer );
							//==================================
							_SCH_MODULE_Set_Mdl_Use_Flag( side , RunWafer , MUF_01_USE );
							//==================================
							SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
						}
						break;
					}
				}
			}
		}
		//
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
			SCHEDULER_CONTROL_CHECK_RUN_AGAIN_RESTORE_A0SUB4( side );
		}
	}
	else if ( mode == 6 ) {
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
			return FALSE;
		}
	}
	else if ( mode == 9 ) { // 2015.05.27
		if ( Get_RunPrm_FM_PM_CONTROL_USE() ) return FALSE; // 2015.05.27
	}
	return TRUE;
}

BOOL _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_0( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
		if ( STR_MEM_SIZE( SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( waferindex1 ) ) > 0 ) {
			if ( mode > 3 ) {
				//================================================================================================================================================
				if ( prcs ) {
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_PM%d" , logpath , SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( waferindex1 ) , ch - PM1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_PM%d" , logpath , SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( waferindex1 ) , ch - PM1 + 1 );
				}
				if      ( mode == 4 ) strcat( filename , "_LOTPRE" );
				else if ( mode == 5 ) strcat( filename , "_LOTPOST" );
				//================================================================================================================================================
				if      ( notuse ) strcat( filename , "_TEMP" );
				//================================================================================================================================================
			}
			else {
				//================================================================================================================================================
				if ( prcs ) {
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_PM%d" , logpath , SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( waferindex1 ) , waferindex1 , ch - PM1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_PM%d" , logpath , SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( waferindex1 ) , waferindex1 , ch - PM1 + 1 );
				}
				if      ( mode == 2 ) strcat( filename , "_PRE" );
				else if ( mode == 3 ) strcat( filename , "_POST" );
				//================================================================================================================================================
				if      ( notuse ) strcat( filename , "_TEMP" );
				//================================================================================================================================================
			}
			return TRUE;
		}
	}
	return FALSE;
}

void _SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_0( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
	//
	*nomore = FALSE;
	*rmode = mode;
	*xoffset = 0;
	//
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
		if      ( mode == 0 ) { // Main
			*xoffset = 0; // 2007.06.11
		}
		else if ( mode == 1 ) { // Wafer Post
			if ( x == 2 ) { // Lot pre (M2,X0) // 2010.12.17
				*rmode = 2;
				*xoffset = -2;
				*nomore = TRUE;
			}
			else {
				*xoffset = 0; // 2007.06.11
//				*nomore = TRUE; // 2007.06.11 // 2010.12.17
			}
		}
		else if ( mode == 2 ) { // Wafer Pre
			*xoffset = 0; // 2007.06.11
		}
		else if ( mode == 11 ) { // lot pre
			*rmode = 2;
			//
			*xoffset = 0; // 2007.06.11
			*nomore = TRUE; // 2007.06.11
		}
		else if ( mode == 12 ) { // lot post
			*rmode = 1;
			//
			*xoffset = 1; // 2007.06.11
			*nomore = TRUE; // 2007.06.11
		}
		else if ( mode == 21 ) { // lot pre (force)// 2007.03.07
			*rmode = 2;
			//
			*xoffset = 99; // 2007.06.11
			//xoffset = 3; // 2007.06.11
			*nomore = TRUE; // 2007.06.11
		}
		else if ( mode == 22 ) { // lot post (force)// 2007.03.07
			*rmode = 1;
			//
			*xoffset = 99; // 2007.06.11
			*nomore = TRUE; // 2007.06.11
		}
	}
}

BOOL _SCH_COMMON_SIDE_SUPPLY_STYLE_0( int side , BOOL EndCheck , int Supply_Style , int *Result ) {
	//
	*Result = TRUE; // 2008.08.25
	//
	if      ( Supply_Style == -1 ) { // fm
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) return FALSE;
		return TRUE;
	}
	else if ( Supply_Style == -2 ) { // tm(use FM)
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2004.05.19
				return FALSE;
			}
		}
		return TRUE;
	}
	else if ( Supply_Style == -3 ) { // tm(notuse FM)
		return FALSE;
	}
	else if ( Supply_Style == -99 ) { // Done
		return FALSE;
	}
	//
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
		*Result = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_0() {
	return FALSE;
}

BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_0() {
	return FALSE;
}

BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_0( int side , int pt , int ch ) {
	int al;
	//=====================================================================================================================
	// 2007.09.06
	//=====================================================================================================================
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
		al = _SCH_CLUSTER_Get_Buffer( side , pt ) % 10;
		if ( ( al != 0 ) && ( ( al + PM1 - 1 ) != -(ch) ) ) return TRUE;
	}
	//=====================================================================================================================
	return FALSE;
}

int _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() {
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

int _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_0() {
	return 0;
}

void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_0( int side , int bmc , char *appstr ) {
}

int _SCH_COMMON_GET_METHOD_ORDER_STYLE_0( int side , int pt ) {
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO2 ) {
		return ( _SCH_CLUSTER_Get_Buffer( side , pt ) / 10 );
	}
	else {
		switch ( _SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() ) {
		case 1 :
			if ( pt < MAX_CASSETTE_SLOT_SIZE ) {
				return _SCH_CLUSTER_Get_PathDo( side , pt );
			}
			break;
		case 2 : // 2006.04.13
			if ( pt < MAX_CASSETTE_SLOT_SIZE ) {
				return _SCH_CLUSTER_Get_Stock( side , pt );
			}
			break;
		case 3 : // 2009.07.07
			if ( pt < MAX_CASSETTE_SLOT_SIZE ) {
				return ( _SCH_CLUSTER_Get_PathDo( side , pt ) + ( _SCH_CLUSTER_Get_Extra( side , pt ) * 100 ) );
			}
			break;
		}
		return 0;
	}
	return 0;
}

int _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_0( int side , int pt , int subchk ) {
	int i;
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) { // 2007.02.09
		//
		if      ( subchk == 0 ) {
			if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , 0 ) > 0 )  return 1;
			else                                                                     return -1;
		}
		else if ( subchk == 1 ) {
			for ( i = 0 ; i < 3 ; i++ ) {
				if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , 1 ) > 0 ) return 1;
			}
		}
		else if ( subchk == 2 ) {
			return SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , 2 );
		}
		else if ( subchk == 10 ) { // 2011.01.27
			if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , 10 ) > 0 )  return 1;
			else                                                                      return -1;
		}
		else if ( subchk == -101 ) { // Pre // 2011.05.11
			//======================================================================================================================================
//			if ( !SCHEDULER_CONTROL_PICK_FROM_FM_DUMMYWAITCHECK_A0SUB4( side , pt ) ) return -1; // 2011.05.11 // 2011.06.13
			//======================================================================================================================================
			return 1;
		}
		else {
			return SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , subchk );
		}
	}
	return 1;
}

BOOL _SCH_COMMON_CHECK_END_PART_STYLE_0( int side , int mode ) {
	int i;
	//
	if      ( ( mode >= 2000 ) && ( mode < 3000 ) ) { // 2011.04.29
		if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) { // 2010.09.15
			return ( SCHEDULER_CONTROL_END_WAIT_A0SUB4( side , mode , mode - 2000 ) );
		}
	}
	else if ( ( mode >= 3000 ) && ( mode < 4000 ) ) { // 2011.04.29
		for ( i = BM1 ; i < TM ; i++ ) {
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_02_USE_to_END ) {
				if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
					_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_WAIT_STATION );
				}
			}
		}
	}
	else {
		switch ( mode ) {
		case 1001 :
			if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) { // 2010.09.15
				return ( SCHEDULER_CONTROL_END_WAIT_A0SUB4( side , mode , -1 ) );
			}
			break;
		case 1998 :
			for ( i = BM1 ; i < TM ; i++ ) {
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_02_USE_to_END ) {
					if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
						_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_WAIT_STATION );
					}
				}
			}
			break;
		case 1999 :
			break;
/*
// 2011.04.29
		case 2001 :
			if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) { // 2010.09.15
				return ( SCHEDULER_CONTROL_END_WAIT_A0SUB4( side , mode ) );
			}
			break;
		case 2999 :
			for ( i = BM1 ; i < TM ; i++ ) {
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_02_USE_to_END ) {
					if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
						_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_WAIT_STATION );
					}
				}
			}
			break;
*/
		default :
			if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
				return ( SCHEDULER_CONTROL_END_CHECK_A0SUB4( side , mode ) );
			}
			break;
		}
	}
	return TRUE;
}

int _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_0( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	// Return Value
	// 1 : Dis+prset
	// 2 : Dis
	// 3 : prset
	if      ( loopindex == 0 ) {
		*DoMore = TRUE;
		//
		*DisableHWMode = 0;
		*CheckMode = 0;
		*AbortWaitTag = ABORTWAIT_FLAG_MODULE_SET_ONLY;
		return 2;
	}
	else if ( loopindex == 1 ) {
		*DoMore = FALSE;
		//
		SCHEDULER_Set_FULLY_RETURN_CH( ch , 1 );
		//
		*PrcsResult = 0;
		return 3;
	}
	else {
		*DoMore = FALSE;
		return 0;
	}
	*DoMore = FALSE;
	return 0;
}

BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_0( int side ) {
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
			if ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) > MAX_BM_CHAMBER_DEPTH ) {
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					if ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

int  _SCH_COMMON_CHECK_WAITING_MODE_STYLE_0( int side , int *mode , int lc_precheck , int option ) {
	int i , k;
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
			switch( option ) {
			case 0 : 
				return FALSE;
				break;
			case 1 :
				return TRUE;
				break;
			case 2 :
				return TRUE;
				break;
			case 3 : // 2009.03.06
				if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != 0 ) {
					if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) != 0 ) {
						if ( ( ( *mode % 10 ) == 1 ) || ( ( *mode % 10 ) == 3 ) ) {
							for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
								if ( k != side ) {
									if ( _SCH_SYSTEM_USING_RUNNING( k ) ) {
										if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) ) {
											if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) ) ) {
												if ( _SCH_MODULE_Get_BM_SIDE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) , 1 ) != side ) return TRUE;
											}
											else { // 2009.04.27
												if ( _SCH_SYSTEM_RUNORDER_GET( k ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) {
													if ( _SCH_MODULE_Chk_TM_Free_Status( k ) ) {
														if ( !_SCH_MODULE_Chk_TM_Finish_Status( k ) ) return TRUE;
													}
												}
												//
												if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return TRUE;
											}
										}
									}
								}
							}
						}
					}
				}
				else { // 2009.04.24
					if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) == 0 ) {
						//
						if ( ( ( *mode % 10 ) == 1 ) || ( ( *mode % 10 ) == 3 ) ) {
							//
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
								//
								if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) ) continue;
								//
								if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2013.07.26
//								if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) continue;
								//
								for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
									//
									if ( k != side ) {
										//
										if ( _SCH_SYSTEM_USING_RUNNING( k ) ) {
											//
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) == 0 ) continue;
											//
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) != i ) continue;
											//
											if ( _SCH_MODULE_Chk_TM_Free_Status( k ) && _SCH_MODULE_Chk_TM_Finish_Status( k ) ) continue;
											//
											break;
										}
									}
								}
								//
								if ( k == MAX_CASSETTE_SIDE ) return FALSE;
								//
							}
							//
							return TRUE;
							//
						}
					}
				}
				//
				return FALSE;
				break;
			}
		}
	}
	return FALSE;
}

BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_0( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index ,
						char *AppendStr ) {
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
		return ( SCHEDULER_CONTROL_TUNING_GET_MORE_APPEND_DATA_A0SUB4( mode , ch , curorder , pjindex , pjno , TuneData_Module , TuneData_Order , TuneData_Step , TuneData_Name , TuneData_Data , TuneData_Index , AppendStr ) );
	}
	return FALSE;
}



BOOL _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_0( int mode , int side , int ch , int slot , int option ) {
	return FALSE;
}

void _SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_0( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ) { // 2010.01.29
}

BOOL _SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_0( int side , int mode , BOOL precheck , int option ) { // 2010.11.09
	return TRUE;
}

BOOL _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_0( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ) { // 2010.11.23
	int cs;
	//
	//
	// 2011.05.07
	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
		if ( when == 3 ) { // After Process & After Set IO
			//
			cs = _SCH_CLUSTER_Get_PathIn( mode , tmside );
			//
			if ( cs >= BM1 ) {
				*srccass = -2;
				return TRUE;
			}
			//
		}
		else if ( when == 1 ) { // Set Result
			//
			if ( *srccass >= ( MAX_CASSETTE_SIDE + CM1 ) ) {
				*srccass = 1001;
				*srcslot = -1;
				return TRUE;
			}
			//
		}
	}
	//
	return FALSE;
}

int  _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_0( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ) { // 2011.05.13
	return 0;
}

int  _SCH_COMMON_USE_EXTEND_OPTION_STYLE_0( int mode , int sub ) { // 2011.11.18
	//
//	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) return FALSE; // 2012.09.27 (for Test)
	//
	return TRUE;
}
