//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_param.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub.h"
#include "sch_A0_sub_sp2.h"
#include "sch_A0_sub_sp4.h"
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_init.h"


extern int FM_PLACE_LOCKEDBM; // 2017.02.18 L.A.M 확인중

//================================================================================
BOOL MODULE_LAST_SWITCH_0[MAX_CHAMBER]; // 2010.05.21
//================================================================================
int  Dual_AL_Dual_FM_Second_Check; // 2014.03.05
//================================================================================
int	subalg_Style_0;
//================================================================================
void Set_Sub_Alg_Data_STYLE_0( int subalg ) {
	subalg_Style_0 = subalg;
}

int  Get_Sub_Alg_Data_STYLE_0() {
	return subalg_Style_0;
}

//================================================================================

void INIT_SCHEDULER_AL0_PART_DATA( int apmode , int side ) {
	int i , j , k , m , o , x;
	//============================================================================================================
	SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB2( apmode , side ); // 2005.10.07
	SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB4( apmode , side ); // 2005.10.07
	//============================================================================================================
	if ( apmode == 0 ) {
		InitializeCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
		InitializeCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) InitializeCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[i] ); // 2006.02.08
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) InitializeCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) InitializeCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[i] ); // 2013.03.19
		//
		InitializeCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2007.10.04
		InitializeCriticalSection( &CR_SINGLEBM_STYLE_0 );
		//=========================================================================================
	}
	//
	if ( apmode == 0 ) {
		//--------------------------------------------------------------------------------------------------------------
		KPLT0_SET_PROCESS_TIME_OUT_INIT();
		//--------------------------------------------------------------------------------------------------------------
		Set_Prm_MODULE_BUFFER_USE_MODE( FALSE );
		//--------------------------------------------------------------------------------------------------------------
		Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
		//--------------------------------------------------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
			//=========================================================================
			Set_Prm_MODULE_BUFFER_USE_MODE( TRUE );
			//=========================================================================
			i = 0;
			o = 0;
			j = 0;
			m = 0;
			x = 0;
			//
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				if ( _SCH_PRM_GET_MODULE_MODE( k ) ) {
					if ( _SCH_PRM_GET_MODULE_GROUP( k ) == 0 ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_IN_MODE ) i++;
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_OUT_MODE ) o++;
						if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_SWITCH_MODE ) x++;
						j++;
						m = k;
					}
				}
			}
			//===========================================================================
			if ( _SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2015.08.26
				if ( j == 1 ) {
					if ( _SCH_PRM_GET_MODULE_SIZE( m ) == 1 ) {
						Set_Prm_MODULE_BUFFER_SINGLE_MODE( m );
					}
					else {
						Set_Prm_MODULE_BUFFER_SINGLE_MODE( 0 );
					}
				}
				else {
					Set_Prm_MODULE_BUFFER_SINGLE_MODE( 0 );
				}
			}
			else { // 2015.08.26
				Set_Prm_MODULE_BUFFER_SINGLE_MODE( 0 );
			}
			//===========================================================================
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
				if ( x > 0 ) {
					//=================================================================================================
					i = 0;
					o = 0;
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( _SCH_PRM_GET_MODULE_MODE( k ) ) {
							if ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_IN_MODE ) i++;
								if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_OUT_MODE ) o++;
							}
						}
					}
					//=================================================================================================
					if ( ( i > 0 ) && ( o > 0 ) ) {
						Set_Prm_MODULE_BUFFER_SWITCH_MODE( 2 );
					}
					else {
						Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
					}
					//=================================================================================================
				}
				else {
					//=================================================================================================
					Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
					//=================================================================================================
					if      ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , BM1 ) == BUFFER_IN_MODE ) {
						SCHEDULER_Set_PATHTHRU_MODE( 1 );
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM1 ) ) SCHEDULER_Set_PATHTHRU_MODE( 2 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , BM1 ) == BUFFER_OUT_MODE ) {
						SCHEDULER_Set_PATHTHRU_MODE( 3 );
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM1 ) ) SCHEDULER_Set_PATHTHRU_MODE( 4 );
					}
					//=================================================================================================
				}
			}
		}
		else {
			//=======================================================================================
			i = 0;
			o = 0;
			j = 0;
			x = 0;
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				if ( _SCH_PRM_GET_MODULE_MODE( k ) ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 1 , k ) == BUFFER_IN_MODE ) i++;
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 1 , k ) == BUFFER_OUT_MODE ) o++;
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 1 , k ) == BUFFER_SWITCH_MODE ) x++;
					j++;
				}
			}
			if ( j > 1 ) {
				if ( x > 0 ) {
					//=================================================================================================
					Set_Prm_MODULE_BUFFER_USE_MODE( TRUE );
					//=================================================================================================
					Set_Prm_MODULE_BUFFER_SWITCH_MODE( 1 );
					//=================================================================================================
				}
				else {
					if ( ( i > 0 ) && ( o > 0 ) ) {
						//=================================================================================================
						Set_Prm_MODULE_BUFFER_USE_MODE( TRUE );
						//=================================================================================================
						Set_Prm_MODULE_BUFFER_SWITCH_MODE( 0 );
						//=================================================================================================
						if      ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 1 , BM1 ) == BUFFER_IN_MODE ) {
							SCHEDULER_Set_PATHTHRU_MODE( 1 );
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM1 ) ) SCHEDULER_Set_PATHTHRU_MODE( 2 );
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 1 , BM1 ) == BUFFER_OUT_MODE ) {
							SCHEDULER_Set_PATHTHRU_MODE( 3 );
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM1 ) ) SCHEDULER_Set_PATHTHRU_MODE( 4 );
						}
						//=================================================================================================
					}
				}
			}
			//=======================================================================================
		}
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( _SCH_PRM_GET_MODULE_MODE( i ) ) {
						if ( _SCH_PRM_GET_MODULE_SIZE( i ) < _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) {
							_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_BATCH_PART );
							break;
						}
					}
				}
			}
		}
		//
	}
	//
	if ( ( apmode == 1 ) || ( apmode == 3 ) ) {
		SIGNAL_MODE_APPEND_END_SET( side , 0 );
	}
	if ( apmode == 3 ) {
		//===========================================================================
		Set_RunPrm_TM_INTERLOCK_USE(); // 2002.04.27
		//===========================================================================
		Set_RunPrm_FM_PM_CONTROL_USE(); // 2014.11.09
		//===========================================================================
		SCHEDULER_CONTROL_BMSCHEDULING_FACTOR_REMAPPING(); // 2004.08.14
		//===========================================================================
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//------------------------------------------------------------------------------------
		SCHEDULER_Set_MULTI_GROUP_PLACE( -1 , -1 ); // 2006.02.08
		SCHEDULER_Set_MULTI_GROUP_PICK( -1 , -1 ); // 2006.02.08
		//
		SCHEDULER_Init_MULTI_GROUP_TMS(); // 2013.03.19
		//------------------------------------------------------------------------------------
		SCHEDULER_Set_FULLY_RETURN_MODE( 0 ); // 2006.02.01
		//------------------------------------------------------------------------------------
		KPLT0_INIT_CASSETTE_TIME_SUPPLY();
		//------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Cls_BM_Switch_Use();
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			SCHEDULER_Set_TM_MidCount( i , 0 );
			SCHEDULER_Set_TM_LastMove_Signal( i , -1 );
			//
			SCHEDULER_Set_TM_Pick_BM_Signal( i , 0 );
			//
			SCHEDULER_Set_PM_MidCount( i , 0 ); // 2002.11.05
			//
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2010.05.21
			MODULE_LAST_SWITCH_0[i] = FALSE;
		}
		//
		Dual_AL_Dual_FM_Second_Check = 0; // 2014.03.05
		//
		FM_PLACE_LOCKEDBM = 0; // 2017.02.18 L.A.M 확인중
		//
	}
	//
	if ( apmode == -1 ) {
		DeleteCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
		DeleteCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) DeleteCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[i] ); // 2006.02.08
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) DeleteCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) DeleteCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[i] ); // 2013.03.19
		DeleteCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2007.10.04
		DeleteCriticalSection( &CR_SINGLEBM_STYLE_0 );
	}
	//============================================================================================================
	SCHEDULER_CONTROL_INIT_SWITCH_PART_DATA( apmode , side );
	//============================================================================================================
}
