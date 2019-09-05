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
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
#include "INF_sch_CommonUser.h"
//================================================================================
#include "sch_A0_default.h"
#include "sch_A0_param.h"
#include "sch_A0_equip.h"
#include "sch_A0_init.h"
#include "sch_A0_sub.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub_sp2.h" // 2005.10.07
#include "sch_A0_sub_sp3.h" // 2006.03.07
#include "sch_A0_sub_sp4.h" // 2006.03.07
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_sub_F_dbl1.h"
#include "sch_A0_CommonUser.h"
//================================================================================
/*

_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 )

더이상 갈 곳이 없을때 상태를 어떻게 할것인가?	W(Wait기다린다)	R(회수한다)

		(3)									0	1	2	3	4

													-(2) Pick BM이 갈때 없을때 Pick 안함

기본 Option이 Return 일때					R	R	W	R	R
	(Balance.R)

기본 Option이 Return이 아닐때
	(Balance.R 이 아닐때)

	내부적으로 아무일도 안할때				W	R	W	R	W
	내부적으로 Return이 필요할때(SM)		R	R	W	W	W

*/

extern BOOL MULTI_ALL_PM_FULLSWAP; // 2018.10.20
extern int  MULTI_ALL_PM_LAST_PICK[MAX_TM_CHAMBER_DEPTH];
extern int RUN_NOT_SELECT_COUNT[MAX_TM_CHAMBER_DEPTH]; // 2018.12.06



int  SCHEDULER_FM_All_No_More_Supply(); // 2017.02.10
BOOL SCHEDULER_FM_Current_No_More_Supply( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Current_No_More_Supply2( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply( int Side , int option ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply2( int Side , int option , int option2 ); // 2014.11.07
BOOL SCHEDULER_FM_FM_Finish_Status( int Side ); // 2014.11.07
int SCHEDULER_CONTROL_Get_PathProcessChamber( int s , int p , int pd , int k ); // 2015.11.10

int SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( int s , int p , BOOL whenalldis , BOOL dataonly , int log ); // 2016.02.19

extern int MULTI_PM_USE; // 2014.01.10

extern BOOL FM_Pick_Running_Blocking_style_0; // 2006.04.28

extern BOOL MODULE_LAST_SWITCH_0[MAX_CHAMBER]; // 2010.05.21

extern int  FM_Place_Waiting_for_BM_style_0; // 2014.01.03

int SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( int TMATM , int arm , int NextPlacech , int mode , BOOL hasnotwaferr );

BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Next_Group( int TMATM ); // 2018.06.12

BOOL SCHEDULING_CHECK_for_BM_Has_Next_Group( int TMATM , int side ); // 2018.06.12

BOOL SCHEDULING_CHECK_for_TM_ARM_Place_ArmIntlks_Impossible( TMATM , Finger ); // 2017.02.13

BOOL SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( int tmside , int side , int *bmc );

int SCHEDULER_INTERLOCK_ALL_BM_PICK_DENY_FOR_MDL( int TMATM , int CHECKING_SIDE ); // 2015.08.20

//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
void SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( int side , int tms , int bmc , int log ) { // 2014.12.26
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
		if ( tms == 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) >= BUFFER_SWITCH_SINGLESWAP ) {
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
					//-------------------------------------------------------------------------------
					SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( side , tms , bmc );
					//-------------------------------------------------------------------------------
				}
			}
		}
	}
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , 1 , TRUE , tms+1 , log );
	//
}



int SCHEDULING_MOVE_OPERATION_ALG0( int tms , int mode , int side , int pt , int mdl , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer ) {
	//
	switch( tms ) {
	case 0 :	return _SCH_MACRO_TM_OPERATION_0( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
	case 1 :	return _SCH_MACRO_TM_OPERATION_1( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
	case 2 :	return _SCH_MACRO_TM_OPERATION_2( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
	case 3 :	return _SCH_MACRO_TM_OPERATION_3( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
#ifndef PM_CHAMBER_12
	case 4 :	return _SCH_MACRO_TM_OPERATION_4( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
	case 5 :	return _SCH_MACRO_TM_OPERATION_5( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
#ifndef PM_CHAMBER_30
	case 6 :	return _SCH_MACRO_TM_OPERATION_6( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
	case 7 :	return _SCH_MACRO_TM_OPERATION_7( mode , side , pt , mdl , arm , slot , depth , Switch , incm , ForceOption , PrePostPartSkip , dide , dointer );	break;
#endif
#endif
	}
	return SYS_ABORTED;
	//
}

int _SCH_MACRO_TM_MOVE_OPERATION_ALG0( int tms , int side , int mode , int ch , int arm , int sts , int slot ) {
	//
	switch( tms ) {
	case 0 :	return _SCH_MACRO_TM_MOVE_OPERATION_0( side , mode , ch , arm , sts , slot );	break;
	case 1 :	return _SCH_MACRO_TM_MOVE_OPERATION_1( side , mode , ch , arm , sts , slot );	break;
	case 2 :	return _SCH_MACRO_TM_MOVE_OPERATION_2( side , mode , ch , arm , sts , slot );	break;
	case 3 :	return _SCH_MACRO_TM_MOVE_OPERATION_3( side , mode , ch , arm , sts , slot );	break;
#ifndef PM_CHAMBER_12
	case 4 :	return _SCH_MACRO_TM_MOVE_OPERATION_4( side , mode , ch , arm , sts , slot );	break;
	case 5 :	return _SCH_MACRO_TM_MOVE_OPERATION_5( side , mode , ch , arm , sts , slot );	break;
#ifndef PM_CHAMBER_30
	case 6 :	return _SCH_MACRO_TM_MOVE_OPERATION_6( side , mode , ch , arm , sts , slot );	break;
	case 7 :	return _SCH_MACRO_TM_MOVE_OPERATION_7( side , mode , ch , arm , sts , slot );	break;
#endif
#endif
	}
	return SYS_ABORTED;
	//
}
//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
int SCHEDULING_MACRO_POST_PROCESS_OPERATION( int side , int pt , int arm , int ch ) { // 2011.02.28
	int order , rpd , Res;
	char waferpost[1024] , lotpost[1024];
	//
	if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO4 ) {
		return _SCH_MACRO_POST_PROCESS_OPERATION( side , pt , arm , ch );
	}
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -11;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return -12;
	//
	rpd = _SCH_CLUSTER_Get_PathDo( side , pt ) - 2;
	//
	if ( ( rpd < 0 ) || ( rpd >= _SCH_CLUSTER_Get_PathRange( side , pt ) ) ) return -13;
	//
	order = _SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch );
	//
	STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , '|' , waferpost , lotpost , 1023 );
	//
	if ( strlen( lotpost ) > 0 ) {
		//
		if ( strlen( waferpost ) > 0 ) {
			_SCH_CLUSTER_Set_PathProcessData_JustPost( side , pt , rpd , order , waferpost );
			Res = _SCH_MACRO_POST_PROCESS_OPERATION( side , pt , arm , ch );
		}
		else {
			Res = 0;
		}
		//
		_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( side ,
			ch ,
			side + 1 , 0 , 0 ,
			lotpost ,
			12 ,
			0 , "" , 0 ,
			1 , 1461 );
		//
		return Res;
	}
	//
	return _SCH_MACRO_POST_PROCESS_OPERATION( side , pt , arm , ch );
}


BOOL SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() { // 2008.10.23
	if ( 0 == _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return FALSE;
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) return FALSE;
	return TRUE;
}

BOOL Scheduler_Curr_Side_PM_Aborted( int side ) { // 2007.06.27
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i + PM1 ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i + PM1 ) >= MUF_90_USE_to_XDEC_FROM ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i + PM1 ) == -1 ) return TRUE;
		}
	}
	return FALSE;
}


BOOL SCHEDULING_CHECK_USE_TM_is_LOCK( int TMATM , int arm ) { // 2008.05.14
	int k , m , s , p;
	//
	/*
	// 2014.07.17
//	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2013.03.09
	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) > 0 ) { // 2013.03.09
		s = _SCH_MODULE_Get_TM_SIDE( TMATM,arm );
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) % 100 ) > 0 ) {
			p = _SCH_MODULE_Get_TM_POINTER( TMATM,arm );
		}
		else {
			p = _SCH_MODULE_Get_TM_POINTER2( TMATM,arm );
		}
	}
	else {
		s = _SCH_MODULE_Get_TM_SIDE( TMATM,arm );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM,arm );
	}
	//
	if ( p >= 100 ) return FALSE;
	*/
	//
	// 2014.07.17
	s = _SCH_MODULE_Get_TM_SIDE( TMATM,arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM,arm );
	//
	if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= _SCH_CLUSTER_Get_PathRange( s , p ) ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
			if ( m > 0 ) return FALSE;
		}
	}
	//
	return TRUE;
}


void SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_WHEN_PLACE_TO_OTHER_BM2( int TMATM , int s , int p , int d , BOOL tmmode , BOOL bmmode , int ch ) { // 2008.06.21
	int k , c , m , z , cr , or;
	//
	if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) return; // 2008.07.04
	//
	if ( !tmmode ) { // 2008.06.25
		c = 0;
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
			if ( m > 0 ) {
				c++;
			}
		}
		//
		if ( c <= 1 ) return;
		//
		if ( bmmode ) { // 2008.06.24
			cr = 0; // 2008.06.26
			or = 0; // 2008.06.26
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { // 2008.06.26
				m  = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
				//
				if ( m > 0 ) {
					if      ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
						cr++;
					}
					else if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
						or++;
					}
				}
			}
			//
			if ( ( cr == 1 ) && ( or == 1 ) ) { // 2008.06.26
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m  = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
					//
					if ( m > 0 ) {
						if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
							_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , m ); // 2008.06.23
							_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k ); // 2008.06.23
						}
					}
					else if ( m < 0 ) { // 2008.06.23
						if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , -m , G_PLACE ) ) {
							_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , -m );
							_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k );
						}
					}
				}
				return; // 2008.06.26
			}
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( ch ) < 2 ) return;
		}
		else {
			c = 0;
			z = 0;
			//
			cr = 0; // 2008.06.26
			or = 0; // 2008.06.26
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m  = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
				//
				if ( m > 0 ) {
					if ( m != ch ) {
						if      ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
							cr++;
//							if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
							if ( SCH_PM_IS_ABSENT( m , s , p , d ) ) {
								c = 99;
								break;
							}
						}
						else if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
							or++;
//							if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) c++; // 2014.01.10
							if ( SCH_PM_IS_ABSENT( m , s , p , d ) ) c++;
						}
					}
					else {
						cr++;
						z = 1;
					}
				}
			}
			//
			if ( c < 2 ) {
				if ( z == 0 ) return;
			}
			//
			if ( ( z == 1 ) && ( cr == 1 ) && ( or == 1 ) ) return; // 2008.06.26
			//
			if ( ( c != 99 ) && ( z == 1 ) && ( cr > 1 ) && ( or > 1 ) ) return; // 2008.07.01
		}
	}
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m  = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k );
		//
		if ( m > 0 ) {
			if ( bmmode ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
//					_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , d , k ); // 2008.06.23
					_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , m ); // 2008.06.23
					_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k ); // 2008.06.23
				}
			}
			else {
				if ( m != ch ) {
					if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
//						_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , d , k ); // 2008.06.23
						_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , m ); // 2008.06.23
						_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k ); // 2008.06.23
					}
				}
			}
		}
		else if ( m < 0 ) { // 2008.06.23
			if ( bmmode ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , -m , G_PLACE ) ) {
					_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , -m );
					_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k );
				}
			}
			else {
				if ( -m != ch ) {
					if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , -m , G_PLACE ) ) {
						_SCH_CLUSTER_Set_PathProcessChamber( s , p , MAX_CLUSTER_DEPTH - 1 , k , -m );
						_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , d , k );
					}
				}
			}
		}
	}
}

BOOL SCHEDULING_CHECK_Other_TM_Waiting_Return( int CHECKING_SIDE , int TMATM , int bch ) { // 2008.07.04
	int i , p;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return FALSE; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return FALSE;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return FALSE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return FALSE;
		}
	}

	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return FALSE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) continue;
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != ( TMATM + 1 ) ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( i ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) return TRUE;
			}
		}
	}
	return FALSE;
}

void SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_BEFORE_PLACE_TO_OTHER_BPM( int TMATM , int arm , BOOL bmmode , int ch ) { // 2008.06.21
	int i , s , p , d , sc , lc , bmc;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	d = _SCH_CLUSTER_Get_PathDo( s , p );
	//
	if ( d <= _SCH_CLUSTER_Get_PathRange( s , p ) ) {
		SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_WHEN_PLACE_TO_OTHER_BM2( TMATM , s , p , d - 1 , TRUE , bmmode , ch );
	}
	//
	bmc = _SCH_MODULE_Get_TM_OUTCH( TMATM , arm ); // 2002.04.23
	//
	if ( bmc < BM1 ) return;
	if ( bmc > ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) return;
	//
	if ( _SCH_PRM_GET_MODULE_GROUP( bmc ) != TMATM ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.01.23
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( bmc );
	}
	else {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , TRUE , &sc , &lc ); // in
	}
	//
	for ( i = sc ; i <= lc ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) != BUFFER_INWAIT_STATUS ) continue; // 2017.01.23
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
		p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
		//
		if ( TMATM == 0 ) {
			d = _SCH_CLUSTER_Get_PathDo( s , p );
		}
		else {
			d = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
		}
		//
		if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_WHEN_PLACE_TO_OTHER_BM2( TMATM , s , p , d , FALSE , bmmode , ch );
		//
	}
}


BOOL SCHEDULING_CHECK_SWITCHING_USE_SINGLEARM_TM_is_LOCK( int TMATM , int side ) { // 2008.05.06
	int arm;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return FALSE;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return FALSE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return FALSE;
		}
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return FALSE;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) return FALSE;
		arm = TA_STATION;
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) return FALSE;
		arm = TB_STATION;
	}
	//
	return SCHEDULING_CHECK_USE_TM_is_LOCK( TMATM , arm ); // 2008.05.14
}



/*
//
// 2017.09.07
//
void SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM( int TMATM , int side ) { // 2010.04.20
	int i , j , l , k , m , s , p , selid , f;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		//
		if ( s != side ) continue;
		//
		if ( p >= MAX_CASSETTE_SLOT_SIZE ) continue;
		//
		j = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
		//
		if ( j <= 0 ) continue;
		if ( j >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , (j - 1) ) == NULL ) continue;
		//
		f = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , j , k );
			//
			if ( m > 0 ) {
				f = TRUE;
				break;
			}
		}
		//
		if ( f ) continue;
		//
		for ( ; j < _SCH_CLUSTER_Get_PathRange( s , p ) ; j++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , j ) == NULL ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , j , k );
				//
				if ( m < 0 ) {
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) {
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , -m ) == MUF_01_USE ) { // 2010.04.23
							//
							_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , j , k );
							//
//							selid = _SCH_CLUSTER_Get_PathProcessParallel( s , p , j )[ -m - PM1 ]; // 2015.04.10
							selid = _SCH_CLUSTER_Get_PathProcessParallel2( s , p , j , ( -m - PM1 ) ); // 2015.04.10
							//
							if ( selid < 0 ) {
								_SCH_CLUSTER_Set_PathProcessParallel2( s , p , j , (-m - PM1) , (char) ( -selid ) );
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
	for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue; // 2014.01.10
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( l = 0 ; l < _SCH_PRM_GET_MODULE_SIZE( i ) ; l++ ) { // 2014.01.10
			//
			if ( _SCH_MODULE_Get_PM_WAFER( i , l ) <= 0 ) continue; // 2014.01.10
			if ( _SCH_MODULE_Get_PM_PICKLOCK( i , l ) > 0 ) continue; // 2014.01.10
			//
//			s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//			p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
			//
			s = _SCH_MODULE_Get_PM_SIDE( i , l ); // 2014.01.10
			p = _SCH_MODULE_Get_PM_POINTER( i , l ); // 2014.01.10
			//
			if ( s != side ) continue;
			//
			if ( p >= MAX_CASSETTE_SLOT_SIZE ) continue;
			//
			j = _SCH_CLUSTER_Get_PathDo( s , p );
			//
			if ( j <= 0 ) continue;
			if ( j >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
			//
			if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , (j - 1) ) == NULL ) continue;
			//
//			selid = _SCH_CLUSTER_Get_PathProcessParallel( s , p , (j - 1) )[ i - PM1 ]; // 2015.04.10
			selid = _SCH_CLUSTER_Get_PathProcessParallel2( s , p , (j - 1) , ( i - PM1 ) ); // 2015.04.10
			//
			if ( ( selid <= 1 ) || ( selid >= 100 ) ) continue;
			//
			f = FALSE;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , j , k );
				//
				if ( m > 0 ) {
					f = TRUE;
					break;
				}
			}
			//
			if ( f ) continue;
			//
			for ( ; j < _SCH_CLUSTER_Get_PathRange( s , p ) ; j++ ) {
				//
				if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , j ) == NULL ) continue;
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , j , k );
					//
					if ( m < 0 ) {
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) {
							//
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , -m ) == MUF_01_USE ) { // 2010.04.23
								//
								_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , j , k );
								//
//								selid = _SCH_CLUSTER_Get_PathProcessParallel( s , p , j )[ -m - PM1 ]; // 2015.04.10
								selid = _SCH_CLUSTER_Get_PathProcessParallel2( s , p , j , ( -m - PM1 ) ); // 2015.04.10
								//
								if ( selid < 0 ) {
									_SCH_CLUSTER_Set_PathProcessParallel2( s , p , j , (-m - PM1) , (char) ( -selid ) );
								}
								//
							}
						}
					}
				}
			}
		//
		}
	}
}
*/




//
// 2017.09.07
//
int SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM_SUB( int s , int p , int WillPathDo , int pickch ) {
	int k , m , selid , f;
	//
	if ( WillPathDo <= 0 ) return 1;
	if ( WillPathDo >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return 2;
	//
	if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , WillPathDo-1 ) == NULL ) return 3;
	//
	if ( pickch >= PM1 ) {
		//
		selid = _SCH_CLUSTER_Get_PathProcessParallel2( s , p , WillPathDo-1 , ( pickch - PM1 ) );
		//
		if ( ( selid <= 1 ) || ( selid >= 100 ) ) return 4;
		//
	}
	//
	f = FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , WillPathDo , k );
		//
		if ( m > 0 ) {
			f = TRUE;
			break;
		}
	}
	//
	if ( f ) return 5;
	//
	for ( ; WillPathDo < _SCH_CLUSTER_Get_PathRange( s , p ) ; WillPathDo++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathProcessParallel( s , p , WillPathDo ) == NULL ) continue;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , WillPathDo , k );
			//
			if ( m < 0 ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( -m , FALSE , s ) ) {
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , -m ) == MUF_01_USE ) {
						//
						_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , WillPathDo , k );
						//
						selid = _SCH_CLUSTER_Get_PathProcessParallel2( s , p , WillPathDo , ( -m - PM1 ) );
						//
						if ( selid < 0 ) {
							_SCH_CLUSTER_Set_PathProcessParallel2( s , p , WillPathDo , (-m - PM1) , (char) ( -selid ) );
						}
						//
					}
				}
			}
		}
	}
	//
	return 0;
}
//
void SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM( int TMATM , int side ) { // 2010.04.20
	int i ,l , s , p;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		//
		if ( s != side ) continue;
		//
		if ( p >= MAX_CASSETTE_SLOT_SIZE ) continue;
		//
		SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM_SUB( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , 0 );
		//
	}
	//
	for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
		//
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue; // 2014.01.10
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( l = 0 ; l < _SCH_PRM_GET_MODULE_SIZE( i ) ; l++ ) { // 2014.01.10
			//
			if ( _SCH_MODULE_Get_PM_WAFER( i , l ) <= 0 ) continue; // 2014.01.10
			if ( _SCH_MODULE_Get_PM_PICKLOCK( i , l ) > 0 ) continue; // 2014.01.10
			//
//			s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//			p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
			//
			s = _SCH_MODULE_Get_PM_SIDE( i , l ); // 2014.01.10
			p = _SCH_MODULE_Get_PM_POINTER( i , l ); // 2014.01.10
			//
			if ( s != side ) continue;
			//
			if ( p >= MAX_CASSETTE_SLOT_SIZE ) continue;
			//
			SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM_SUB( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , i );
			//
		}
	}
	//
}

int SCHEDULING_CHECK_NOSWITCHING_INVALID_RETURN_OPERATION( int TMATM , int side , int *arm , int *retbmc ) { // 2008.05.14
	int i , f = FALSE;
	//
	*arm = -1;
	*retbmc = -1;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return 1; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) return 1;
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return 2;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2009.01.21
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return 3;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_NO ) ) continue;
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( SCHEDULING_CHECK_TM_All_Free_Status( 0 ) ) { // 2008.07.14 // 2012.08.30
			if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) { // 2008.05.21
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return 3; // 2012.06.20
					//
					if ( SCHEDULER_FM_Current_No_More_Supply2( side ) ) {
						if ( SCHEDULER_FM_Another_No_More_Supply( side , -1 ) ) {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1699 );
							return 99;
						}
					}
				}
			}
		}
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == 0 ) return 4;
		//
		if      ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == 1 ) { // a
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
				if ( !SCHEDULING_CHECK_USE_TM_is_LOCK( TMATM , TB_STATION ) ) {
					return 5;
				}
				f = TRUE;
				*arm = TB_STATION;
			}
		}
		else if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) == 2 ) { // b
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
				if ( !SCHEDULING_CHECK_USE_TM_is_LOCK( TMATM , TA_STATION ) ) {
					return 6;
				}
				f = TRUE;
				*arm = TA_STATION;
			}
		}
		else  {
			return 7;
		}
	}
	//
	if ( !f ) return 8;
	//
	f = FALSE;
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2009.01.21
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return 11;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) continue;
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue;
		//
		*retbmc = i;
		f = TRUE;
		//
	}
	//
	if ( !f ) return 12;
	//
	return 0;
}


int SCHEDULING_RUN_NOSWITCHING_PLACE_BM( int TMATM , int arm , int ch ) { // 2008.05.14
	int CHECKING_SIDE , BM_Side2 , pt , pt2 , wsa , wsb , SCH_Slot , SCH_Slot2 , Function_Result;
	//=========================================================================================
	CHECKING_SIDE = _SCH_MODULE_Get_TM_SIDE( TMATM,arm );
	pt = _SCH_MODULE_Get_TM_POINTER( TMATM,arm );
	SCH_Slot = 1;
	//
	if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
		//
		BM_Side2 = _SCH_MODULE_Get_TM_SIDE2( TMATM,arm );
		pt2 = _SCH_MODULE_Get_TM_POINTER2( TMATM,arm );
		//=========================================================================================
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) % 100 ) <= 0 ) {
			SCH_Slot2 = SCH_Slot + 1;
			SCH_Slot = 0;
			wsa = _SCH_MODULE_Get_TM_WAFER( TMATM,arm );
			wsb = SCH_Slot + SCH_Slot2*100;
		}
		else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) / 100 ) <= 0 ) {
			SCH_Slot2 = 0;
			wsa = _SCH_MODULE_Get_TM_WAFER( TMATM,arm );
			wsb = SCH_Slot + SCH_Slot2*100;
		}
		else {
			SCH_Slot2 = SCH_Slot + 1;
			wsa = _SCH_MODULE_Get_TM_WAFER( TMATM,arm );
			wsb = SCH_Slot + SCH_Slot2*100;
		}
		//=========================================================================================
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
			_SCH_MODULE_Dec_TM_DoubleCount( CHECKING_SIDE );
		//=========================================================================================
	}
	else {
		BM_Side2 = CHECKING_SIDE;
		pt2 = 0;
		SCH_Slot2 = 0;
		wsa = _SCH_MODULE_Get_TM_WAFER( TMATM,arm );
		wsb = SCH_Slot;
	}
	//-----------------------------------------------------------------------
	_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END );
	if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
		_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_BM_END );
	}
	//-----------------------------------------------------------------------
	Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ch , arm , wsa , wsb , 0 , -1 , FALSE , 0 , BM_Side2 , pt2 );
	if ( Function_Result == SYS_ABORTED ) {
//		return FALSE; // 2008.12.12
		return -1; // 2008.12.12
	}
	else if ( Function_Result == SYS_ERROR ) {
//		return FALSE; // 2008.12.12
		return 0; // 2008.12.12
	}
	//-----------------------------------------------------------------------
	if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
		_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
		_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( 0,arm ) % 100 , BUFFER_OUTWAIT_STATUS );
		_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , BM_Side2 , pt2 );
		_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( 0,arm ) / 100 , BUFFER_OUTWAIT_STATUS );
		//
		_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
		_SCH_MODULE_Inc_TM_InCount( BM_Side2 ); // 2008.06.10
	}
	else if ( SCH_Slot > 0 ) {
		_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
		_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( 0,arm ) % 100 , BUFFER_OUTWAIT_STATUS );
		//
		_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
	}
	else if ( SCH_Slot2 > 0 ) {
		_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , CHECKING_SIDE , pt );
		_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( 0,arm ) / 100 , BUFFER_OUTWAIT_STATUS );
		//
		_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
	}
	//=================================================================================
	_SCH_MODULE_Set_TM_WAFER( 0 , arm , 0 );
//	_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
	//=================================================================================
	_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , CHECKING_SIDE , pt );
	if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
		_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , BM_Side2 , pt2 );
//		if ( CHECKING_SIDE != BM_Side2 ) _SCH_MODULE_Inc_TM_InCount( BM_Side2 ); // 2008.06.10
	}
	//=================================================================================
//	return TRUE; // 2008.12.12
	return 1; // 2008.12.12
}

BOOL SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION( int TMATM , int side , int arm , int retbmc ) { // 2008.05.14
	int i;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	if ( ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) % 100 ) > 0 ) ) { // 2008.06.25

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 1 in SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION %d,%d\n" , TMATM + 1 , arm , retbmc );
	//----------------------------------------------------------------------

		_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE( TMATM , arm ) , _SCH_MODULE_Get_TM_POINTER( TMATM , arm ) , -1 ); // 2008.06.25
		_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE2( TMATM , arm ) , _SCH_MODULE_Get_TM_POINTER2( TMATM , arm ) , -1 ); // 2008.06.25
	}
	else {

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 2 in SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION %d,%d\n" , TMATM + 1 , arm , retbmc );
	//----------------------------------------------------------------------

		_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE( TMATM , arm ) , _SCH_MODULE_Get_TM_POINTER( TMATM , arm ) , -1 ); // 2008.06.25
	}
	//
	_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_OUT_MODE );
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( retbmc ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( retbmc , i ) > 0 ) {
			_SCH_MODULE_Set_BM_STATUS( retbmc , i , BUFFER_OUTWAIT_STATUS );

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 3 in SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION %d,%d,%d\n" , TMATM + 1 , arm , retbmc , i );
	//----------------------------------------------------------------------

			_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( retbmc , i ) , _SCH_MODULE_Get_BM_POINTER( retbmc , i ) , -1 );
		}
	}
	//
	_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
	//
	while ( TRUE ) {
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 101 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
		//----------------------------------------------------------------------
		//=====================================================================================================================
		if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( retbmc ) ) {
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 102 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
			//----------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( retbmc ) && ( _SCH_MODULE_Get_BM_FULL_MODE( retbmc ) == BUFFER_TM_STATION ) ) {
				if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , retbmc , 1 , TRUE , 0 , 1011 ) == SYS_ABORTED ) {
					_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
					return FALSE;
				}
			}
			while ( TRUE ) {
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 103 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
				//----------------------------------------------------------------------
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( retbmc ) ) {
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( retbmc ) || ( _SCH_MODULE_Get_BM_FULL_MODE( retbmc ) == BUFFER_TM_STATION ) ) {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( retbmc ) <= 0 ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( retbmc ) < 0 ) {
								_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
								return FALSE;
							}
							break;
						}
					}
				}
				switch( _SCH_SYSTEM_ABORT_PAUSE_CHECK( side ) ) {
				case -1 :
					_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
					return FALSE;
					break;
				}
				Sleep(1);
			}
			//
		}
		else {
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 104 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
			//----------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( retbmc ) && ( _SCH_MODULE_Get_BM_FULL_MODE( retbmc ) == BUFFER_FM_STATION ) ) {
				if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , retbmc , 1 , TRUE , 0 , 1012 ) == SYS_ABORTED ) {
					_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
					return FALSE;
				}
			}
		}
		//=====================================================================================================================
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 105 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
		//----------------------------------------------------------------------
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) <= 0 ) {
			_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( retbmc ) && ( _SCH_MODULE_Get_BM_FULL_MODE( retbmc ) == BUFFER_TM_STATION ) ) { // 2008.05.19
				if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , retbmc , 1 , TRUE , 0 , 1013 ) == SYS_ABORTED ) {
					return FALSE;
				}
			}
			return TRUE;
		}
		//=====================================================================================================================
		// place arm to retbmc
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 106 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
		//----------------------------------------------------------------------
//		if ( !SCHEDULING_RUN_NOSWITCHING_PLACE_BM( TMATM , arm , retbmc ) ) {
//			_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
//			return TRUE;
//		}
		switch( SCHEDULING_RUN_NOSWITCHING_PLACE_BM( TMATM , arm , retbmc ) ) {
		case 0 :
			_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
			return TRUE;
			break;
		case 1 :
			break;
		default :
			return FALSE;
			break;
		}
		//=====================================================================================================================
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 107 SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION arm=%d,retbmc=%d\n" , arm , retbmc );
		//----------------------------------------------------------------------
		if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , retbmc , 1 , TRUE , 0 , 1014 ) == SYS_ABORTED ) {
			_SCH_PRM_SET_MODULE_BUFFER_MODE( TMATM , retbmc , BUFFER_IN_MODE );
			return FALSE;
		}
		//
	}
	//
	return TRUE;
}



BOOL SCHEDULING_CHECK_SWITCHING_MANY_BM_SLOT_AFTER_PICK_or_PLACE( int TMATM , int side , int bch ) { // 2008.05.13
	int i , k , m , s , p , sc , lc , f;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return FALSE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return FALSE;
		}
	}
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return FALSE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return FALSE;
	if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( bch , G_PICK , 0 ) ) return FALSE;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( bch , FALSE , -1 ) ) return TRUE;
	//
//	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bch , FALSE , &sc , &lc ); // out
	//
//	for ( p = sc ; p <= lc ; p++ ) {
//		if ( _SCH_MODULE_Get_BM_WAFER( bch , p ) <= 0 ) return FALSE;
//	}
//	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.01.23
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( bch );
	}
	else {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bch , TRUE , &sc , &lc ); // in
	}
	//
	f = FALSE;
	//
	for ( i = sc ; i <= lc ; i++ ) { // 2008.06.22
		if ( _SCH_MODULE_Get_BM_WAFER( bch , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bch , i ) != BUFFER_INWAIT_STATUS ) continue; // 2017.01.23
		//

		s = _SCH_MODULE_Get_BM_SIDE( bch , i );
		p = _SCH_MODULE_Get_BM_POINTER( bch , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
				if ( m > 0 ) {
//					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( m , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) ) ) {
						return FALSE;
					}
				}
			}
		}
		f = TRUE;
	}
	//
	return f;
}


BOOL SCHEDULING_CHECK_SWITCHING_USE_PM_is_LOCK( int TMATM , int side , int pm ) { // 2008.05.02
	int i , j;
	//
//	if ( _SCH_MODULE_Get_PM_WAFER( pm , 0 ) <= 0 ) return FALSE; // 2014.01.10
	if ( !SCH_PM_IS_PICKING( pm ) ) return FALSE;
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return FALSE;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( i == TMATM ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( i , pm , G_PLACE ) ) continue;
		//
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( i,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( i,TB_STATION ) ) return FALSE;
		//
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) continue;
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( i , j , G_PLACE , G_MCHECK_SAME ) ) continue;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( j ) == BUFFER_TM_STATION ) return FALSE;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) > 0 ) return FALSE;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( j ) ) return FALSE;
			//
		}
	}
	//
	return TRUE;
}

BOOL SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1( int TMATM , int side ) { // 2008.05.14
	int Result , arm , retbmc;

	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return TRUE; // 2008.05.14

	Result = SCHEDULING_CHECK_NOSWITCHING_INVALID_RETURN_OPERATION( TMATM , side , &arm , &retbmc );

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 01 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
	//----------------------------------------------------------------------

	if ( Result == 0 ) { // 2008.05.14
		//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 02 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
		//----------------------------------------------------------------------
		//
		_SCH_SYSTEM_ENTER_FEM_CRITICAL();
		//
		if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
			_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 03 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
			//----------------------------------------------------------------------
			return TRUE;
		}
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( retbmc ) > 0 ) {
			_SCH_SYSTEM_LEAVE_FEM_CRITICAL();
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 04 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
			//----------------------------------------------------------------------
			return TRUE;
		}
		//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 05 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
		//----------------------------------------------------------------------
		if ( !SCHEDULING_RUN_NOSWITCHING_INVALID_RETURN_OPERATION( TMATM , side , arm , retbmc ) ) {
			return FALSE;
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 06 SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1 Result=%d,arm=%d,retbmc=%d\n" , Result , arm , retbmc );
		//----------------------------------------------------------------------
	}
	return TRUE;
}


void SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB2( int TMATM , int side ) { // 2012.08.10
	int i , selbm;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) return;
	//
	i = 0;
	//
	if ( FM_Pick_Running_Blocking_style_0 == 0 ) {
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == 0 ) i++;
		}
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == 0 ) i++;
		}
	}
	//
	if ( i == 0 ) return;
	//
	selbm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue;
		//
		if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
		//
		//-------------------------------------
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) continue;
		//
		//-------------------------------------
		//
		if ( selbm == 0 ) selbm = i;
		//
		//-------------------------------------
		//
	}
	//
	i = 0;
	//
	if ( FM_Pick_Running_Blocking_style_0 == 0 ) {
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == 0 ) i++;
		}
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == 0 ) i++;
		}
	}
	//
	if ( i == 0 ) return;
	//
	if ( selbm != 0 ) {
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1614 );
	}
	//
}

int SCHEDULING_CHECK_SWITCHING_LOCK_MIDDLE_IN_SWAP_RETURN_NEED_MOVE( int TMATM , int startbm ) { // 2011.07.26
	int i , p , sc , lc;
	int f;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return -1; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return -1; // 2016.10.30
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return -1; // 2013.03.09 // 2016.07.22
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22 // 2017.01.23
//		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) { // 2017.01.23
//			return -1; // 2017.01.23
//		} // 2017.01.23
//	} // 2017.01.23
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return -1; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	for ( i = startbm + 1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		f = 0;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		for ( p = sc ; p <= lc ; p++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( i , p ) == BUFFER_OUTWAIT_STATUS ) {
					f = 1;
					break;
				}
			}
			//
			p++;
			//
		}
		//
		if ( f == 0 ) continue;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		for ( p = sc ; p <= lc ; p++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
				f = 0;
				break;
			}
		}
		//
		if ( f == 0 ) continue;
		//
		return i;
	}
	//
	return -1;
}


BOOL SCHEDULING_CHECK_SWITCHING_LOCK_MIDDLE_IN_SWAP_RETURN_RUN_MOVE( int TMATM , int side , int bmc ) {
	int p , sc , lc;
	int f , sf , arm , Function_Result;
	int s1 , p1 , w1 , t1;
	int s2 , p2 , w2 , t2;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	arm = -1;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) {
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,bmc ) ) {
		case 0 : // a
		case 2 : // ab
			//
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,bmc ) ) {
			case 0 : // a
			case 2 : // ab
				arm = TA_STATION;
				break;
			}
			//
			break;
		}

	}
	//
	if ( arm == -1 ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) {
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,bmc ) ) {
			case 1 : // b
			case 2 : // ab
				//
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,bmc ) ) {
				case 1 : // b
				case 2 : // ab
					arm = TB_STATION;
					break;
				}
				//
				break;
			}

		}
	}
	//
	if ( arm == -1 ) return TRUE;
	//
	sf = 0;
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , TRUE , &sc , &lc ); // in
	//
	for ( p = sc ; p <= lc ; p++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , p ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , p ) == BUFFER_OUTWAIT_STATUS ) {
				sf = p;
				break;
			}
		}
		p++;
	}
	//
	if ( sf == 0 ) return TRUE;
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , FALSE , &sc , &lc ); // out
	//
	s1 = _SCH_MODULE_Get_BM_SIDE( bmc , sf );
	p1 = _SCH_MODULE_Get_BM_POINTER( bmc , sf );
	w1 = _SCH_MODULE_Get_BM_WAFER( bmc , sf );
	t1 = _SCH_MODULE_Get_BM_STATUS( bmc , sf );
	//
	s2 = _SCH_MODULE_Get_BM_SIDE( bmc , sf+1 );
	p2 = _SCH_MODULE_Get_BM_POINTER( bmc , sf+1 );
	w2 = _SCH_MODULE_Get_BM_WAFER( bmc , sf+1 );
	t2 = _SCH_MODULE_Get_BM_STATUS( bmc , sf+1 );
	//
	Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , s1 , p1 , bmc , arm , w1 + ( w2 * 100 ) , ( w2 > 0 ) ? ( sf + ( (sf+1) * 100 ) ) : sf , 0 , -1 , FALSE , 0 , s2 , p2 );
	//
	if ( Function_Result == SYS_ABORTED ) {
		return FALSE;
	}
	else if ( Function_Result == SYS_ERROR ) {
		return FALSE;
	}
	//
	Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , s1 , p1 , bmc , arm , w1 + ( w2 * 100 ) , ( w2 > 0 ) ? ( sc + ( (sc+1) * 100 ) ) : sc , 0 , -1 , FALSE , 0 , s2 , p2 );
	if ( Function_Result == SYS_ABORTED ) {
		return FALSE;
	}
	else if ( Function_Result == SYS_ERROR ) {
		return FALSE;
	}
	//
	_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , sf     , 0 , -1 );
	_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , sf + 1 , 0 , -1 );
	//
	_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , sc     , s1 , p1 );	_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , sc     , w1 , t1 );
	_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , sc + 1 , s2 , p2 );	_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , sc + 1 , w2 , t2 );
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmc , TRUE , &sc , &lc ); // in
	//
	f = 0;
	//
	for ( p = sc ; p <= lc ; p++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , p ) > 0 ) {
			f = 1;
			break;
		}
	}
	//
	if ( f == 0 ) {
//		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , 1 , TRUE , TMATM+1 , 1714 ); // 2014.12.26
		SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( side , TMATM , bmc , 1714 ); // 2014.12.26
	}
	//
	return TRUE;
}

BOOL SCHEDULING_CHECK_SWITCHING_EIL_LOCK_MAKE_FREE_SUB11( int TMATM , int side ) { // 2011.07.26
	int bmc;

	if ( _SCH_PRM_GET_EIL_INTERFACE() <= 0 ) return TRUE;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return TRUE;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return TRUE;
	//
	bmc = BM1 - 1;
	//
	while ( TRUE ) {
		//
		bmc = SCHEDULING_CHECK_SWITCHING_LOCK_MIDDLE_IN_SWAP_RETURN_NEED_MOVE( TMATM , bmc );
		//
		if ( bmc < 0 ) return TRUE;
		//
		if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MIDDLE_IN_SWAP_RETURN_RUN_MOVE( TMATM , side , bmc ) ) return FALSE;
		//
	}
	//
	return TRUE;
}



BOOL Scheduler_Process_Single( int s , int p ) { // 2008.06.23
	int m , k , r = 0 , c = 0;
	//
	if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 0 ) return FALSE; // 2008.06.24
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		if      ( m > 0 ) {
			r++;
			c++;
		}
		else if ( m < 0 ) {
			c++;
		}
	}
	if ( r > 1 ) return FALSE;
	if ( c > 1 ) return FALSE;
	return TRUE;
}

BOOL Scheduler_Process_Invalid( int s , int p ) { // 2008.06.23
	int m , k;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) return FALSE;
		}
	}
	return TRUE;
}

void SCHEDULING_CHECK_DISABLE_CLOSE_EVENT( int TMATM , int side ) { // 2010.05.21
	int i;
	for ( i = PM1 ; i < TM ; i++ ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( MODULE_LAST_SWITCH_0[i] ) {
			//
			MODULE_LAST_SWITCH_0[i] = FALSE;
			//
			SCHEDULING_EQ_GATE_CLOSE_STYLE_0( TMATM , i );
			//
		}
	}
	//
}


BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( int TMATM , BOOL );
BOOL SCHEDULING_CHECK_for_PM_Has_Return_Wafer( int TMATM );


void SCHEDULING_CHECK_DEFAULT_LOCK_MAKE_FREE_SUB0( int TMATM , int side ) { // 2014.10.08
	//
	int i , j , xfind , sc , lc;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	if ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) return;
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) {
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
		}
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_FULLSWAP ) {
			sc = 1;
			lc = _SCH_PRM_GET_MODULE_SIZE( i );
		}
		else {
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		}
		//
		xfind = FALSE;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) {
				xfind = TRUE;
				break;
			}
			//
		}
		//
		if ( xfind ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_FULLSWAP ) {
			sc = 1;
			lc = _SCH_PRM_GET_MODULE_SIZE( i );
		}
		else {
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		}
		//
		xfind = FALSE;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
				//
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
				//
				xfind = TRUE;
				break;
			}
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_OUTWAIT_STATUS ) {
				xfind = TRUE;
				break;
			}
			//
		}
		//
		if ( xfind ) continue;
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 3013 );
	}
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB0( int TMATM , int side ) { // 2008.07.04
	int i , j , s , p , d , sc , lc;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() != 12 ) return;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		if ( ( _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		if ( Scheduler_Process_Invalid( s , p ) ) _SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( s , p , 0 );
	}

	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) {
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
		}
		//
		if ( TMATM == 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.01.23
				sc = 1;
				lc = _SCH_PRM_GET_MODULE_SIZE( i );
			}
			else {
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
			}
		}
		else {
			sc = 1;
			lc = _SCH_PRM_GET_MODULE_SIZE( i );
		}
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
			//
			if ( TMATM == 0 ) { // 2017.01.23
				if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.01.23
					if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_INWAIT_STATUS ) continue; // 2017.01.23
				}
			}
			//
			s = _SCH_MODULE_Get_BM_SIDE( i , j );
			p = _SCH_MODULE_Get_BM_POINTER( i , j );
			//
			if ( TMATM == 0 ) {
				d = _SCH_CLUSTER_Get_PathDo( s , p );
			}
			else {
				d = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
			}
			//
			if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
			//
			if ( Scheduler_Process_Invalid( s , p ) ) _SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( s , p , 0 );
		}
	}
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1( int TMATM , int side ) { // 2008.04.18
	int i , k , m , s , p , sc , lc , singlelock , cross , runmode , srunmode , c;
	int selp; // 2018.09.10
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22 // 2017.01.23
//		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) { // 2017.01.23
//			return; // 2017.01.23
//		} // 2017.01.23
//	} // 2017.01.23
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	singlelock = SCHEDULING_CHECK_SWITCHING_USE_SINGLEARM_TM_is_LOCK( TMATM , side ); // 2008.05.06
	//
	for ( s = 0 ; s < 2 ; s++ ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue; // 2008.04.25
				//
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
				//
//				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) return; // 2008.05.21
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) { // 2008.05.21
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue; // 2008.05.21
					return;
				}
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return; // 2008.12.12
				//
				//
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
				//
				for ( p = sc ; p <= lc ; p++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , p ) ) continue; // 2018.04.03
						//
						return;
					}
					if ( _SCH_MODULE_Get_BM_STATUS( i , p ) != BUFFER_OUTWAIT_STATUS ) return; // 2017.01.11
				}
				//
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
				//
				cross = -1; // 2008.06.20
				runmode = FALSE; // 2008.06.20
				//
				if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2018.09.10
					//
					//
					//
					for ( p = sc ; p <= lc ; p++ ) {
						//
						if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
							selp = p;
						}
						else if ( _SCH_MODULE_Get_BM_WAFER( i , p+1 ) > 0 ) {
							selp = p+1;
						}
						else {
							p++;
							continue;
						}
						//
						if ( _SCH_MODULE_Get_BM_STATUS( i , selp ) != BUFFER_OUTWAIT_STATUS ) {
							//
							if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , selp ) , _SCH_MODULE_Get_BM_POINTER( i , selp ) ) != PATHDO_WAFER_RETURN ) {
								//
								srunmode = FALSE;
								//
								c = 0;
								//
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									m = SCHEDULER_CONTROL_Get_PathProcessChamber( _SCH_MODULE_Get_BM_SIDE( i , selp ) , _SCH_MODULE_Get_BM_POINTER( i , selp ) , 0 , k );
									if ( m > 0 ) {
										//
										c++;
										//
										if ( !singlelock ) {
											if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
												if ( !SCHEDULING_CHECK_SWITCHING_USE_PM_is_LOCK( TMATM , side , m ) ) {
													srunmode = TRUE;
												}
												//
												if      ( cross == -1 ) {
													cross = 1;
												}
												else if ( cross ==  0 ) {
													cross = 2;
												}
												//
											}
											else {
												if ( SCH_PM_IS_ABSENT( m , _SCH_MODULE_Get_BM_SIDE( i , selp ) , _SCH_MODULE_Get_BM_POINTER( i , selp ) , 0 ) ) {
													srunmode = TRUE;
												}
												//
												if      ( cross == -1 ) {
													cross = 0;
												}
												else if ( cross ==  1 ) {
													cross = 2;
												}
												//
											}
										}
									}
								}
								//
								if ( s != 0 ) {
									if ( !srunmode ) { // 2008.06.20
										if ( Scheduler_Process_Single( _SCH_MODULE_Get_BM_SIDE( i , selp ) , _SCH_MODULE_Get_BM_POINTER( i , selp ) ) ) { // 2008.06.23

		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 41 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1 %d,%d\n" , TMATM + 1 , i , selp );
		//----------------------------------------------------------------------

											_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( i , selp ) , _SCH_MODULE_Get_BM_POINTER( i , selp ) , -1 );
										}
									}
								}
								//
								if ( srunmode ) runmode = TRUE;
								//
								if ( c > 1 ) cross = -1;
							}
						}
						//
						p++;
						//
					}
					//
					//
					//
				}
				else {
					//
					for ( p = sc ; p <= lc ; p++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) continue;
						//
						if ( _SCH_MODULE_Get_BM_STATUS( i , p ) != BUFFER_OUTWAIT_STATUS ) {
							if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) ) != PATHDO_WAFER_RETURN ) {
								//
								srunmode = FALSE; // 2008.06.20
								//
								c = 0; // 2008.06.27
								//
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									m = SCHEDULER_CONTROL_Get_PathProcessChamber( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , 0 , k );
									if ( m > 0 ) {
										//
										c++; // 2008.06.27
										//
										if ( !singlelock ) { // 2008.05.06
											if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , m , G_PLACE ) ) {
												if ( !SCHEDULING_CHECK_SWITCHING_USE_PM_is_LOCK( TMATM , side , m ) ) { // 2008.05.02
													srunmode = TRUE; // 2008.06.20
													//return; // 2008.06.20
												}
												//
												if      ( cross == -1 ) { // 2008.06.20
													cross = 1; // 2008.06.20
												} // 2008.06.20
												else if ( cross ==  0 ) { // 2008.06.20
													cross = 2; // 2008.06.20
												} // 2008.06.20
												//
											}
											else { // 2008.05.02
	//											if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
												if ( SCH_PM_IS_ABSENT( m , _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , 0 ) ) {
													srunmode = TRUE; // 2008.06.20
													//return; // 2008.06.20
												}
												//
												if      ( cross == -1 ) { // 2008.06.20
													cross = 0; // 2008.06.20
												} // 2008.06.20
												else if ( cross ==  1 ) { // 2008.06.20
													cross = 2; // 2008.06.20
												} // 2008.06.20
												//
											}
										}
									}
								}
								//
								if ( s != 0 ) {
									if ( !srunmode ) { // 2008.06.20
										if ( Scheduler_Process_Single( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) ) ) { // 2008.06.23

		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 4 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1 %d,%d\n" , TMATM + 1 , i , p );
		//----------------------------------------------------------------------

											_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , -1 );
										}
									}
								}
								//
								if ( srunmode ) runmode = TRUE; // 2008.06.20
								//
								if ( c > 1 ) cross = -1; // 2008.06.27
							}
						}
					}
				}
				//
				if ( ( cross != 2 ) && ( runmode ) ) return; // 2008.06.20
				//
				if ( s != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1608 );
			}
		}
		//
		if ( s != 0 ) return;
		//
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
				if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 ) return;
			}
			else {
				if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,i ) ) continue;
				if ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) <= 0 ) continue;
			}
			//
			/*
			// 2014.07.17
			//
			if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				sc = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
				if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,i ) % 100 ) > 0 ) {
					lc = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
				}
				else {
					lc = _SCH_MODULE_Get_TM_POINTER2( TMATM,i );
				}
			}
			else {
				sc = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
				lc = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
			}
			//
			if ( lc >= 100 ) return;
			*/
			//
			// 2014.07.17
			//
			sc = _SCH_MODULE_Get_TM_SIDE( TMATM,i );
			lc = _SCH_MODULE_Get_TM_POINTER( TMATM,i );
			//
			if ( ( sc < 0 ) || ( sc >= MAX_CASSETTE_SIDE ) ) return;
			if ( ( lc < 0 ) || ( lc >= MAX_CASSETTE_SLOT_SIZE ) ) return;
			//
//			if ( _SCH_CLUSTER_Get_PathDo( sc , lc ) <= _SCH_CLUSTER_Get_PathRange( sc , lc ) ) return; // 2008.05.06
			if ( _SCH_CLUSTER_Get_PathDo( sc , lc ) <= _SCH_CLUSTER_Get_PathRange( sc , lc ) ) { // 2008.05.06
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { // 2008.05.06
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( sc , lc , _SCH_CLUSTER_Get_PathDo( sc , lc ) - 1 , k );
					if ( m > 0 ) return;
				}
			}
			//
		}
		//
	}
}

/*
//
// 2014.12.12

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_2( int TMATM , int side ) { // 2012.01.30
	int i , k , m , p , sc , lc , c;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue; // 2008.04.25
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) {
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue;
			//
			return;
			//
		}
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		for ( p = sc ; p <= lc ; p++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) return;
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		c = 0;
		//
		for ( p = sc ; p <= lc ; p++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , p ) == BUFFER_OUTWAIT_STATUS ) continue;
			//
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) ) == PATHDO_WAFER_RETURN ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , 0 , k );
				//
				if ( m > 0 ) break;
				//
			}
			//
			if ( k == MAX_PM_CHAMBER_DEPTH ) {
				c++;
			}
			else {
				c = 0;
				break;
			}
			//
		}
		//
		if ( c != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1618 );
		//
	}
}
*/

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_2( int TMATM , int side ) { // 2012.01.30
	int i , k , m , p , sc , lc , c;
	BOOL hasreturnwafer; // 2014.12.12
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22 // 2017.01.23
//		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) { // 2017.01.23
//			return; // 2017.01.23
//		} // 2017.01.23
//	} // 2017.01.23
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	hasreturnwafer = SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE );
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue; // 2008.04.25
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) {
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue;
			//
			return;
			//
		}
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		for ( p = sc ; p <= lc ; p++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) {
				//
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , p ) ) continue; // 2018.04.03
				//
				return;
			}
			if ( _SCH_MODULE_Get_BM_STATUS( i , p ) != BUFFER_OUTWAIT_STATUS ) return; // 2017.01.11
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		c = 0;
		//
		for ( p = sc ; p <= lc ; p++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , p ) == BUFFER_OUTWAIT_STATUS ) continue;
			//
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) ) == PATHDO_WAFER_RETURN ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , 0 , k );
				//
				if ( m > 0 ) {
					//
					//----------------------------------------------------------------------------------
					//
					// 2014.12.12
					//
					if ( hasreturnwafer ) {
						//
						switch( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) ) {
						case 2 : // Wafer Free
						case 3 : // Process+Wafer Free
						case 7 : // Process+Wafer Free(ALL)
						case 9 : // Wafer Free(LX)
						case 10 : // Process+Wafer Free(LX)
						case 12 : // Wafer Free(A1)
						case 13 : // Process+Wafer Free(A1)
						case 15 : // Wafer Free(A1.G0)
						case 16 : // Process+Wafer Free(A1.G0)
							//
							if ( !SCH_PM_IS_FREE( m ) ) continue;
							//
							break;
						}
						//
						switch( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) ) {
						case 1 : // Process Free
						case 3 : // Process+Wafer Free
						case 7 : // Process+Wafer Free(ALL)
						case 8 : // Process Free(LX)
						case 10 : // Process+Wafer Free(LX)
						case 11 : // Process Free(A1)
						case 13 : // Process+Wafer Free(A1)
						case 14 : // Process Free(A1.G0)
						case 16 : // Process+Wafer Free(A1.G0)
							//
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) > 0 ) continue;
							//
							break;
						}
						//
					}
					//----------------------------------------------------------------------------------
					//
					break;
					//
				}
				//
			}
			//
			if ( k == MAX_PM_CHAMBER_DEPTH ) {
				c++;
			}
			else {
				c = 0;
				break;
			}
			//
		}
		//
		if ( c != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1618 );
		//
	}
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_3( int TMATM , int side ) { // 2012.01.30
	int i , k , m , p , sc , lc , c , pd , offset , sl;
	BOOL swmode; // 2015.08.28
	int swc; // 2015.08.28
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) return; // 2013.04.11 // 2015.08.28
	//
	swmode = FALSE; // 2015.08.28
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) { // 2015.08.28
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2015.08.28
			return;
		}
		else {
			swmode = TRUE; // 2015.08.28
		}
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( i );
		//
		c = 0;
		//
		swc = swmode ? 1 : 0; // 2015.08.28
		//
		for ( p = sc ; p <= lc ; p++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) {
				//
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , p ) ) continue; // 2018.04.03
				//
				swc = 0; // 2015.08.25
				//
				continue;
			}
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , p ) == BUFFER_OUTWAIT_STATUS ) continue;
			//
			swc = 0; // 2015.08.25
			//
			pd = _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) );
			//
			if ( pd == PATHDO_WAFER_RETURN ) continue;
			//
			if ( TMATM == 0 ) {
				offset = 0;
			}
			else {
				offset = 1;
			}
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , pd - offset , k );
				//
				if ( m > 0 ) {
					//
					// 2018.12.06
					//
					if ( _SCH_CLUSTER_Get_PathProcessDepth( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , pd - offset ) != NULL ) {
						//
						sl = _SCH_CLUSTER_Get_PathProcessDepth2( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , pd - offset , (m - PM1) );
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
					//
					break;
				}
				//
			}
			//
			if ( k == MAX_PM_CHAMBER_DEPTH ) {
				//
				c++;
				//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 5 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_3 %d,%d\n" , TMATM + 1 , i , p );
	//----------------------------------------------------------------------

				_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , -1 );
				//
				SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_BM_SIDE( i , p ) , _SCH_MODULE_Get_BM_POINTER( i , p ) , FALSE , TRUE , 1 ); // 2016.02.19
				//
				_SCH_MODULE_Set_BM_STATUS( i , p , BUFFER_OUTWAIT_STATUS );
				//
			}
			else {
				c = 0;
				break;
			}
			//
		}
		//
		if ( c != 0 ) {
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1678 );
		}
		else { // 2015.08.28
			if ( swc != 0 ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1679 );
			}
		}
		//
	}
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2( int TMATM , int side ) { // 2008.04.24
	int i , p , sc , lc;
	BOOL fm; // 2008.07.12
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22 // 2017.01.23
//		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) { // 2017.01.23
//			return; // 2017.01.23
//		} // 2017.01.23
//	} // 2017.01.23
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
//	if ( !SCHEDULING_CHECK_TM_All_Free_Status() ) return; // 2008.07.14
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue; // 2016.10.30
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		}
	}
	//
	if ( !SCHEDULING_CHECK_TM_All_Free_Status( 0 ) ) { // 2008.07.14 // 2012.08.30
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , 0 ) > 0 ) return;
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , 1 ) > 0 ) return;
		//
		for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue; // 2014.01.10
			if ( !SCH_PM_IS_PICKING( i ) ) continue; // 2014.01.10
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) return;
		}
		//
		if ( SCHEDULER_FM_Current_No_More_Supply2( side ) ) {
			if ( SCHEDULER_FM_Another_No_More_Supply( side , -2 ) ) return;
		}
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
				//
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
				//
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) continue;
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1613 );
			}
		}
	}
	else {
		//
		fm = FALSE; // 2008.07.12
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue; // 2008.04.25
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
				//
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) {
					fm = TRUE; // 2008.07.12
					continue;
				}
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return; // 2008.12.12
				//
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) fm = TRUE; // 2008.07.12
				//
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
				//
				for ( p = sc ; p <= lc ; p++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
//						break; // 2017.01.11
						if ( _SCH_MODULE_Get_BM_STATUS( i , p ) == BUFFER_OUTWAIT_STATUS ) break; // 2017.01.11
					}
				}
				if ( p > lc ) continue;
				//
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
				//
				for ( p = sc ; p <= lc ; p++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) break;
				}
				if ( p <= lc ) continue;
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1611 );
			}
		}
		//------------------------------------------------------------
		// 2008.07.12
		//------------------------------------------------------------
		if ( !fm ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
					//
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
					//
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) continue;
					//
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1612 );
				}
			}
		}
		//------------------------------------------------------------
	}
}

int SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( int , BOOL ); // 2008.08.27

BOOL SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3( int TMATM , int side ) { // 2008.05.19
	int i , j , k , s , p , w , a , c , f , fs[30] , fs2[30] , fscnt , fs2cnt , sc , lc , arm , Function_Result;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	if ( !SCHEDULER_FM_FM_Finish_Status( side ) ) return TRUE;
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return TRUE;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return TRUE;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) arm = TA_STATION;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) arm = TB_STATION;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue;
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc );
			//
			for ( p = sc ; p <= lc ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_SIDE( i , p ) == side ) {
						return TRUE;
					}
				}
			}
		}
	}
	//
	f = 0;
	fscnt = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			c = 0;
			a = 0; // 2008.06.21
			//
/*
// 2008.06.18
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( i ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_SIDE( i , p ) == side ) {
						fs[fscnt] = p;
						fscnt++;
						if ( fscnt >= 30 ) break;
						c++;
					}
				}
			}
			//
			if ( c == 0 ) continue;
*/
			//----------------------------------------------------------------
			// 2008.06.18
			//----------------------------------------------------------------
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( i ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
					a++; // 2008.06.21
					if ( _SCH_MODULE_Get_BM_SIDE( i , p ) == side ) {
						c++;
					}
					//
					fs[fscnt] = p;
					fscnt++;
					if ( fscnt >= 30 ) break;
					//
				}
			}
			//
//			if ( c == 0 ) { // 2008.06.21
			if ( ( c == 0 ) || ( a > 1 ) ) { // 2008.06.21
				fscnt = 0; // 2008.06.18
				continue;
			}
			//----------------------------------------------------------------
			//
			if ( f != 0 ) return TRUE;
			//
			f = i;
			//
			if ( _SCH_MODULE_Get_TM_OutCount( side ) != ( _SCH_MODULE_Get_TM_InCount( side ) + c ) ) return TRUE;
		}
	}
	if ( f == 0 ) return TRUE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			if ( SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( i , FALSE ) < 0 ) continue; // 2008.08.27
			//
			//--------------------------------------------------------------------------------------
			// 2008.06.22
			//--------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
			//
			for ( j = sc ; j <= lc ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( i , j );
					p = _SCH_MODULE_Get_BM_POINTER( i , j );
					//
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							fs2cnt = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
							if ( fs2cnt > 0 ) {
//								if ( _SCH_MODULE_Get_PM_WAFER( fs2cnt , 0 ) <= 0 ) { // 2014.01.10
								if ( SCH_PM_IS_ABSENT( fs2cnt , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) ) ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( fs2cnt ) <= 0 ) {
										return TRUE;
									}
								}
							}
						}
					}
					//
				}
			}
			//--------------------------------------------------------------------------------------
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
			//
			fs2cnt = 0;
			for ( p = sc ; p <= lc ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , p ) ) continue; // 2018.04.03
					//
					fs2[fs2cnt] = p;
					fs2cnt++;
				}
			}
			if ( fs2cnt == 0 ) continue;
			//
			for ( c = 0 ; ( c < fscnt ) && ( c < fs2cnt ) ; c++ ) {
				// move f/fs[c] arm to i/fs2[c]
				s = _SCH_MODULE_Get_BM_SIDE( f , fs[c] ); // 2008.06.18
				p = _SCH_MODULE_Get_BM_POINTER( f , fs[c] );
				w = _SCH_MODULE_Get_BM_WAFER( f , fs[c] );
				//
//				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side , p , f , arm , w , fs[c] , 0 , -1 , FALSE , 0 , -1 , -1 ); // 2008.06.18
				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , s , p , f , arm , w , fs[c] , 0 , -1 , FALSE , 0 , -1 , -1 ); // 2008.06.18
				if ( Function_Result == SYS_ABORTED ) {
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) {
					return FALSE;
				}
				//
//				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PICK , f , arm , fs[c] , 0 , side , p , 0 , 0 , 0 , 0 ); // 2008.06.18
				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PICK , f , arm , fs[c] , 0 , s , p , 0 , 0 , 0 , 0 ); // 2008.06.18
				//
//				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side , p , i , arm , w , fs2[c] , 0 , -1 , FALSE , 0 , -1 , -1 ); // 2008.06.18
				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , s , p , i , arm , w , fs2[c] , 0 , -1 , FALSE , 0 , -1 , -1 ); // 2008.06.18
				if ( Function_Result == SYS_ABORTED ) {
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) {
					return FALSE;
				}
				//
//				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PLACE , i , arm , fs2[c] , 0 , side , p , 0 , 0 , 0 , 0 ); // 2008.06.18
				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PLACE , i , arm , fs2[c] , 0 , s , p , 0 , 0 , 0 , 0 ); // 2008.06.18
				//
				_SCH_MODULE_Set_BM_STATUS( i , fs2[c] , BUFFER_OUTWAIT_STATUS );
				//
//				_SCH_MODULE_Inc_TM_InCount( side ); // 2008.06.18
				_SCH_MODULE_Inc_TM_InCount( s ); // 2008.06.18
			}
			//
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( f ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( f , p ) > 0 ) break;
			}
			if ( p > _SCH_PRM_GET_MODULE_SIZE( f ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , f , 1 , TRUE , TMATM+1 , 1631 );
			}
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1632 );
			//
			return TRUE; // 2008.05.20
		}
	}
	//
	return TRUE;
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4( int TMATM , int side ) { // 2008.06.20
	int i , cb = -1;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return;
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) return;
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) return;
	}
	//
	for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue; // 2014.01.10
		if ( !SCH_PM_IS_PICKING( i ) ) continue; // 2014.01.10
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) return;
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) return;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return; // 2008.12.12
		//
		cb = i;
	}
	//
	if ( cb != -1 ) {
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , cb , 1 , TRUE , TMATM+1 , 1612 );
	}
}


BOOL SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5( int TMATM , int side ) { // 2008.06.25
	int i , j , k , s , p , w , p_bm , p_sl , p_cn , sc , lc , arm , outmode;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return TRUE;
	if ( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() != 0 ) && ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() != 12 ) )  return TRUE;

//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return TRUE;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) arm = TA_STATION;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) arm = TB_STATION;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	w = _SCH_MODULE_Get_TM_WAFER( TMATM , arm );
	//
	if ( ( _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) != 0 ) return TRUE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		sc = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , i );
		if ( sc > 0 ) {
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , sc , G_PLACE ) ) return TRUE;
//			if ( _SCH_MODULE_Get_PM_WAFER( sc , 0 ) <= 0 ) { // 2014.01.10
			if ( SCH_PM_IS_ABSENT( sc , s , p , 0 ) ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( sc ) <= 0 ) return TRUE;
				if ( _SCH_MODULE_GET_USE_ENABLE( sc , FALSE , -1 ) ) return TRUE; // 2008.07.02
			}
		}
	}
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
//		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
		if ( SCH_PM_IS_PICKING( i ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) break;
		}
	}
	if ( i == ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) return TRUE;
	//
	p_bm = -1;
	outmode = FALSE;
	//
	if ( TMATM == 0 ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				//
				if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue;
				//
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
				//
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc );
				//
				k = 0;
				for ( j = sc ; j <= lc ; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
						//
						k++;
					}
				}
				if ( k > 0 ) {
					for ( j = sc ; j <= lc ; j++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
							//
							if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
							//
							if ( p_bm == -1 ) {
								p_bm = i;
								p_sl = j;
								p_cn = k;
							}
							else {
								if ( p_cn > k ) {
									p_bm = i;
									p_sl = j;
									p_cn = k;
								}
							}
						}
					}
				}
			}
		}
	}
	else { // 2008.07.08
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
				//
				if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue;
				if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue;
				//
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
				//
				k = 0;
				for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
						//
						k++;
					}
				}
				if ( k > 0 ) {
					for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
							//
							if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
							//
							if ( p_bm == -1 ) {
								p_bm = i;
								p_sl = j;
								p_cn = k;
							}
							else {
								if ( p_cn > k ) {
									p_bm = i;
									p_sl = j;
									p_cn = k;
								}
							}
						}
					}
				}
			}
		}
		//
		if ( p_bm == -1 ) { // 2008.07.09
			//
			outmode = TRUE;
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					//
					if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue;
					if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue;
					//
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
					//
					k = 0;
					for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
							//
							if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
							//
							k++;
						}
					}
					if ( k > 0 ) {
						for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
								//
								if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
								//
								if ( p_bm == -1 ) {
									p_bm = i;
									p_sl = j;
									p_cn = k;
								}
								else {
									if ( p_cn > k ) {
										p_bm = i;
										p_sl = j;
										p_cn = k;
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
	if ( p_bm == -1 ) return TRUE;
	//
	if      ( TMATM == 0 ) {
		if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , s , p , p_bm , arm , w , p_sl , 0 , -1 , FALSE , 0 , -1 , -1 ) != SYS_SUCCESS ) {
			return FALSE;
		}
		//
		_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PLACE , p_bm , arm , p_sl , 0 , s , p , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
		//
		_SCH_CLUSTER_Dec_PathDo( s , p );
	}
	else if ( TMATM == 1 ) { // 2008.07.08
		if ( _SCH_MACRO_TM_OPERATION_1( MACRO_PLACE , s , p , p_bm , arm , w , p_sl , 0 , -1 , FALSE , 0 , -1 , -1 ) != SYS_SUCCESS ) {
			return FALSE;
		}
		//
		if ( outmode ) { // 2008.07.09
			_SCH_MACRO_TM_DATABASE_OPERATION( 1 , MACRO_PLACE , p_bm , arm , p_sl , 0 , s , p , 0 , 0 , BUFFER_OUTWAIT_STATUS , BUFFER_OUTWAIT_STATUS );
			//
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( p_bm ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( p_bm , j ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( p_bm , j ) ) continue; // 2018.04.03
					//
					break;
				}
			}
			if ( j > _SCH_PRM_GET_MODULE_SIZE( p_bm ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s , p_bm , 1 , TRUE , TMATM+1 , 1691 );
			}
			//
		}
		else {
			_SCH_MACRO_TM_DATABASE_OPERATION( 1 , MACRO_PLACE , p_bm , arm , p_sl , 0 , s , p , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
		}
	}
	else if ( TMATM == 2 ) { // 2008.07.08
		if ( _SCH_MACRO_TM_OPERATION_2( MACRO_PLACE , s , p , p_bm , arm , w , p_sl , 0 , -1 , FALSE , 0 , -1 , -1 ) != SYS_SUCCESS ) {
			return FALSE;
		}
		//
		if ( outmode ) { // 2008.07.09
			_SCH_MACRO_TM_DATABASE_OPERATION( 2 , MACRO_PLACE , p_bm , arm , p_sl , 0 , s , p , 0 , 0 , BUFFER_OUTWAIT_STATUS , BUFFER_OUTWAIT_STATUS );
			//
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( p_bm ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( p_bm , j ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( p_bm , j ) ) continue; // 2018.04.03
					//
					break;
				}
			}
			if ( j > _SCH_PRM_GET_MODULE_SIZE( p_bm ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s , p_bm , 1 , TRUE , TMATM+1 , 1692 );
			}
			//
		}
		else {
			_SCH_MACRO_TM_DATABASE_OPERATION( 2 , MACRO_PLACE , p_bm , arm , p_sl , 0 , s , p , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS );
		}
	}
	//
	return TRUE;
}

BOOL SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB6( int TMATM , int side ) { // 2008.07.01
	int i , j , k , s , p , w , a = 0 , c = 0 , f , fs[30] , fs2[30] , fscnt , fs2cnt , sc , lc , arm , Function_Result , freeyes , suppyes;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 2 ) return TRUE;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return TRUE;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return TRUE;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) arm = TA_STATION;
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) arm = TB_STATION;
	//
	f = 0;
	fscnt = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			a = 0; // 2008.07.15
			c = 0; // 2008.07.15
			//
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( i ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) > 0 ) {
					a++;
					if ( _SCH_MODULE_Get_BM_SIDE( i , p ) == side ) {
						c++;
					}
					//
					fs[fscnt] = p;
					fscnt++;
					if ( fscnt >= 30 ) break;
					//
				}
			}
			//
			if ( ( c == 0 ) || ( a <= 0 ) ) {
				fscnt = 0;
				continue;
			}
			//
			if ( f != 0 ) return TRUE;
			//
			f = i;
			//
		}
	}
	//
	if ( f == 0 ) return TRUE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			if ( SCHEDULING_CHECK_PICK_from_BM_MID2_SEM_CHECK( i , FALSE ) < 0 ) continue; // 2008.08.27
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
			//
			freeyes = FALSE; // 2008.07.04
			suppyes = FALSE; // 2008.07.04
			//
			for ( j = sc ; j <= lc ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					//
					suppyes = TRUE; // 2008.07.04
					//
					s = _SCH_MODULE_Get_BM_SIDE( i , j );
					p = _SCH_MODULE_Get_BM_POINTER( i , j );
					//
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							fs2cnt = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
							if ( fs2cnt > 0 ) {
								if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , fs2cnt , G_PLACE ) ) return TRUE;
							}
						}
					}
					//
				}
				else {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
					//
					freeyes = TRUE; // 2008.07.04
				}
			}
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
			//
			fs2cnt = 0;
			for ( p = sc ; p <= lc ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , p ) <= 0 ) {
					//
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , p ) ) continue; // 2018.04.03
					//
					fs2[fs2cnt] = p;
					fs2cnt++;
				}
			}
			if ( fs2cnt == 0 ) continue;
			//
			for ( c = 0 ; ( c < fscnt ) && ( c < fs2cnt ) ; c++ ) {
				// move f/fs[c] arm to i/fs2[c]
				s = _SCH_MODULE_Get_BM_SIDE( f , fs[c] );
				p = _SCH_MODULE_Get_BM_POINTER( f , fs[c] );
				w = _SCH_MODULE_Get_BM_WAFER( f , fs[c] );
				//
				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , s , p , f , arm , w , fs[c] , 0 , -1 , FALSE , 0 , -1 , -1 );
				if ( Function_Result == SYS_ABORTED ) {
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) {
					return FALSE;
				}
				//
				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PICK , f , arm , fs[c] , 0 , s , p , 0 , 0 , 0 , 0 );
				//
				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , s , p , i , arm , w , fs2[c] , 0 , -1 , FALSE , 0 , -1 , -1 );
				if ( Function_Result == SYS_ABORTED ) {
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) {
					return FALSE;
				}
				//
				_SCH_MACRO_TM_DATABASE_OPERATION( 0 , MACRO_PLACE , i , arm , fs2[c] , 0 , s , p , 0 , 0 , 0 , 0 );
				//
				_SCH_MODULE_Set_BM_STATUS( i , fs2[c] , BUFFER_OUTWAIT_STATUS );
				//
				_SCH_MODULE_Inc_TM_InCount( s );
			}
			//
			for ( p = 1 ; p <= _SCH_PRM_GET_MODULE_SIZE( f ) ; p++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( f , p ) > 0 ) break;
			}
			if ( p > _SCH_PRM_GET_MODULE_SIZE( f ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , f , 1 , TRUE , TMATM+1 , 1681 );
			}
			//
			if ( !suppyes || !freeyes ) { // 2008.07.04
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1682 );
			}
			//
			return TRUE;
		}
	}
	//
	return TRUE;
}



BOOL SCHEDULING_CHECK_SUB7_BM_FULL_AND_PICK_IMPOSSIBLE_FOR_PM_PLACE_ARM_INTLKS( int TMATM , int ch , int farm ) { // 2017.02.16
	int i , k , s, p , m , armint , intlk;
	//
	armint = FALSE;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				s = _SCH_MODULE_Get_BM_SIDE( ch , i );
				p = _SCH_MODULE_Get_BM_POINTER( ch , i );
				//
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 0 ) return FALSE;
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
					//
					if ( m > 0 ) {
						//
						intlk = _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM , m );
						//
						if ( ( intlk == 0 ) || ( intlk == farm ) ) return FALSE;
						//
						armint = TRUE;
						//
					}
				}
				//
			}
		}
		else {
			//
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
			//
			return FALSE;
		}
	}
	//
	return armint;
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB7( int TMATM , int side ) { // 2017.02.16
	int i , s , p , pd , pr , arm , farm , x , selbm;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() != 0 ) return;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	if      ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) {
		arm = TA_STATION;
		farm = TB_STATION;
	}
	else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) {
		arm = TB_STATION;
		farm = TA_STATION;
	}
	else {
		return;
	}
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	pd = _SCH_CLUSTER_Get_PathDo( s , p );
	pr = _SCH_CLUSTER_Get_PathRange( s , p );
	//
	if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) && ( pd <= pr ) ) return;
	//
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		selbm = -1;
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
			//
			if ( !SCHEDULING_CHECK_SUB7_BM_FULL_AND_PICK_IMPOSSIBLE_FOR_PM_PLACE_ARM_INTLKS( TMATM , i , farm ) ) return;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			if ( x == 1 ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 70215 );
			}
			//
			selbm = i;
			//
		}
		//
		if ( selbm == -1 ) return;
	}
	//
}



BOOL SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_FTM_DOUBLE( int TMATM , int side ) { // 2009.05.07
	int i , j , s1 , p1 , w1 , s2 , p2 , w2 , sc , lc , arm , bmc;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return TRUE; // 2012.09.03
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return TRUE;
	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return TRUE;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return TRUE; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return TRUE; // 2016.10.30
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return TRUE;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return TRUE; // 2013.03.09 // 2016.07.22 // 2017.01.23
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return TRUE;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return TRUE;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return TRUE;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) arm = TA_STATION;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) arm = TB_STATION;
	//
	w1 = 0;
	w2 = 0;
	//
	/*
	// 2014.07.17
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) % 100 ) > 0 ) {
		s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
		p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
		w1 = _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) % 100;
		//
		if ( ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) - 1 ) != 0 ) return TRUE;
		if ( !Scheduler_Process_Invalid( s1 , p1 ) ) return TRUE;
	}
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) / 100 ) > 0 ) {
		s2 = _SCH_MODULE_Get_TM_SIDE2( TMATM , arm );
		p2 = _SCH_MODULE_Get_TM_POINTER2( TMATM , arm );
		w2 = _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) / 100;
		//
		if ( ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 ) != 0 ) return TRUE;
		if ( !Scheduler_Process_Invalid( s2 , p2 ) ) return TRUE;
	}
	*/
	//
	// 2014.07.17
	//
	s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	w1 = _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) % 100;
	//
	if ( ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) - 1 ) != 0 ) return TRUE;
	if ( !Scheduler_Process_Invalid( s1 , p1 ) ) return TRUE;
	//
	if ( ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) % 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) / 100 ) > 0 ) ) {
		//
		s2 = _SCH_MODULE_Get_TM_SIDE2( TMATM , arm );
		p2 = _SCH_MODULE_Get_TM_POINTER2( TMATM , arm );
		w2 = _SCH_MODULE_Get_TM_WAFER( TMATM , arm ) / 100;
		//
		if ( ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 ) != 0 ) return TRUE;
		if ( !Scheduler_Process_Invalid( s2 , p2 ) ) return TRUE;
		//
	}
	//
	bmc = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return TRUE;
		//
		if      ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return TRUE; // 2012.06.20
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) return TRUE;
			continue;
		}
		else if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) {
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return TRUE; // 2012.06.20
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( i ) > 0 ) return TRUE;
		}
		else {
			continue;
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
				if ( !Scheduler_Process_Invalid( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) ) ) return TRUE;
			}
			//
		}
		if ( bmc == -1 ) bmc = i;
	}
	//
	if ( bmc != -1 ) {
		i = bmc;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 6 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_FTM_DOUBLE %d,%d\n" , TMATM + 1 , i , j );
	//----------------------------------------------------------------------
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) , -1 );
			}
			//
		}
		//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 7 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_FTM_DOUBLE %d,%d,%d %d,%d,%d\n" , TMATM + 1 , w1 , s1 , p1 , w2 , s2 , p2 );
	//----------------------------------------------------------------------


		if ( w1 > 0 ) _SCH_CLUSTER_Check_and_Make_Return_Wafer( s1 , p1 , -1 );
		if ( w2 > 0 ) _SCH_CLUSTER_Check_and_Make_Return_Wafer( s2 , p2 , -1 );
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1912 );
	}
	return TRUE;
}

/*
// 2014.09.01

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE( int TMATM , int side ) { // 2009.08.07
	int i , j , s , p , sc , lc , arm , bmc , bms , sid;
	int selbm; // 2014.08.18
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return;
//	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return; // 2014.08.18
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09
	//
//	if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10 ) == 0 ) return; // 2014.08.18
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) arm = TA_STATION;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) arm = TB_STATION;
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	if ( ( ( _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) == 0 ) && ( !Scheduler_Process_Invalid( s , p ) ) ) return;
	//
	if ( !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( s , &selbm ) ) { // 2014.08.14
		//
		if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return; // 2014.08.18
		//
		if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10 ) == 0 ) return;
		//
		selbm = -1;
		//
	}
	//
	bmc = -1;
	bms = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( ( selbm != -1 ) && ( selbm != i ) ) continue; // 2014.08.18
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return; // 2012.06.20
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) && ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) <= 0 ) ) {
				return;
			}
			//
			j++;
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		sid = 0;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
				}
				else {
					sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) );
				}
			}
			else {
				return;
			}
			//
			j++;
		}
		if ( bmc == -1 ) {
			bmc = i;
			bms = sid;
		}
		else {
			if ( bms > sid ) {
				bmc = i;
				bms = sid;
			}
		}
	}
	//
	if ( bmc != -1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , 1 , TRUE , 1 , 1915 );
}

*/

// 2014.09.01

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE_SUB( int TMATM , int s , int p , int onlyselbm ) {
	int i , j , sc , lc , bmc , bms , sid;
	//
	if ( ( ( _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) == 0 ) && ( !Scheduler_Process_Invalid( s , p ) ) ) return;
	//
	if ( onlyselbm == -1 ) {
		//
		if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return;
		//
		if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( TMATM ) % 10 ) == 0 ) return;
		//
	}
	//
	bmc = -1;
	bms = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( ( onlyselbm != -1 ) && ( onlyselbm != i ) ) continue; // 2014.08.18
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) && ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) <= 0 ) ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j+1 ) ) { // 2018.04.03
					//
					return;
					//
				}
				//
			}
			//
			j++;
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		sid = 0;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
				}
				else {
					sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) );
				}
			}
			else {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j+1 ) ) { // 2018.04.03
					//
					return;
					//
				}
				//
			}
			//
			j++;
		}
		if ( bmc == -1 ) {
			bmc = i;
			bms = sid;
		}
		else {
			if ( bms > sid ) {
				bmc = i;
				bms = sid;
			}
		}
	}
	//
	if ( bmc != -1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s , bmc , 1 , TRUE , TMATM+1 , 1915 );
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE( int TMATM , int side ) { // 2009.08.07
	int s1 , p1 , s2 , p2;
	int selbm1 , selbm2;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
//	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return;
//	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return;
	//
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
		s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
		p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
	}
	else {
		s1 = -1;
		p1 = -1;
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
		s2 = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
		p2 = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
	}
	else {
		s2 = -1;
		p2 = -1;
	}
	//
	if ( ( s1 >= 0 ) && ( s2 >= 0 ) ) {
		//
		if ( s1 == s2 ) return; // 2014.09.08
		//
		if ( !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , s1 , &selbm1 ) ) selbm1 = -1;
		if ( !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , s2 , &selbm2 ) ) selbm2 = -1;
		//
		if ( ( selbm1 == -1 ) || ( selbm2 == -1 ) ) return;
		//
	}
	else if ( s1 >= 0 ) {
		if ( !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , s1 , &selbm1 ) ) selbm1 = -1;
	}
	else if ( s2 >= 0 ) {
		if ( !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , s2 , &selbm2 ) ) selbm2 = -1;
	}
	else {
		return;
	}
	//
	if ( s1 >= 0 ) SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE_SUB( TMATM , s1 , p1 , selbm1 );
	if ( s2 >= 0 ) SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE_SUB( TMATM , s2 , p2 , selbm2 );
	//
}

/*
//
// 2018.04.20
//
BOOL SCHEDULING_FIRST_SUPPLY_IS_USERMODE() { // 2017.03.16
	int i , cm , pt , f;
	//
	f = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
			//
			if ( _SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( i , &cm , &pt ) ) return FALSE;
			if ( _SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( i , &cm , &pt ) ) f++;
			//
		}
	}
	//
	return ( f > 0 );
	//
}
//
*/
//
// 2018.04.20
//
BOOL SCHEDULING_FIRST_SUPPLY_IS_USERMODE() { // 2017.03.16 // 2018.04.20
	// 2018.04.20
	//
	// 마지막 Wafer가 TM에서 BM에 Place되고 더이상 정상 Wafer가 공급될것이 없을때
	// 다음 User Mode 의 것이 공급되지 않을때 이 후 에 있을 새로운 Lot을 위해 BM을 FMSide로 변경하는 기능
	// -> FM Side로 변경시 모든 Lot이 끝나고 BM이 Gate 를 연 상태로 기다리는 경우가 있어
	//    이 부분을 정상 Wafer가 공급될 것이 있는 경우만 FM Side 로 변경 하도록 수정
	//    (마지막으로 끝날때 가능항 TMSIDE로 남겨 두도록)
	//
	int i , cm , pt;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
			if ( _SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( i , &cm , &pt ) ) return FALSE;
		}
	}
	//
	return TRUE;
	//
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3_FTM_DOUBLE( int TMATM , int side ) { // 2011.09.16
	int i , j;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return;
//	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return; // 2016.05.16
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2013.03.09 // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	j = FALSE;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
			if ( !_SCH_MODULE_Chk_TM_Free_Status2( i , 1 ) ) return;
			j = TRUE;
		}
	}
	if ( !j ) return;
	//
	if ( SCHEDULING_FIRST_SUPPLY_IS_USERMODE() ) return; // 2017.03.16
	//
	for ( j = 0 ; j < 2 ; j++ ) {
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue; // 2016.10.30
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
			//
			if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) return;
			//
//			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return; // 2016.05.16
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue; // 2016.05.16
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return; // 2012.06.20
			//
			if ( j == 1 ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1917 );
			}
			//
		}
		//
	}
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4_FTM_DOUBLE( int TMATM , int side ) { // 2015.02.16
	int i , arm , s , p , pd , pr , selbm;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return;
	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) return;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	arm = -1; // 2015.07.03
	//
	if      ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) {
		arm = TA_STATION;
	}
	else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) ) {
		arm = TB_STATION;
	}
//	else { // 2015.07.03
//		return; // 2015.07.03
//	} // 2015.07.03
	//
//	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) return; // 2015.07.03
	//
//	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm ); // 2015.07.03
//	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm ); // 2015.07.03
	//
//	pd = _SCH_CLUSTER_Get_PathDo( s , p ); // 2015.07.03
//	pr = _SCH_CLUSTER_Get_PathRange( s , p ); // 2015.07.03
	//
//	if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) && ( pd <= pr ) ) return; // 2015.07.03
	//
	if ( arm != -1 ) { // 2015.07.03
		//
		if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , arm ) ) return;
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		pr = _SCH_CLUSTER_Get_PathRange( s , p );
		//
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , arm ) != SCHEDULER_MOVING_OUT ) && ( pd <= pr ) ) return;
		//
	}
	else { // 2015.07.03
		//
		if ( arm == -1 ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) {
				//
				if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
					//
					s = _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION );
					p = _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION );
					//
					if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , TA_STATION ) == SCHEDULER_MOVING_OUT ) || ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) ) {
						arm = TA_STATION;
					}
					//
				}
				//
			}
			//
		}
		//
		if ( arm == -1 ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) {
				//
				if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
					//
					s = _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION );
					p = _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION );
					//
					if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM , TB_STATION ) == SCHEDULER_MOVING_OUT ) || ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) ) {
						arm = TB_STATION;
					}
					//
				}
				//
			}
			//
		}
		//
		if ( arm == -1 ) return;
		//
	}
	//
	selbm = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue; // 2016.10.30
		//
		if      ( arm == TA_STATION ) {
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 1 ) continue;
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,i ) != 1 ) continue;
		}
		else if ( arm == TB_STATION ) {
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( TMATM,i ) != 2 ) continue;
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,i ) != 2 ) continue;
		}
		else {
			continue;
		}
		//
		if ( selbm == -1 )
			selbm = i;
		else
			return;
		//
	}
	//
	if ( selbm > 0 ) {
		//
		if ( !SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( selbm , TRUE ) ) return;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( selbm ) != BUFFER_TM_STATION ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( selbm ) > 0 ) return;
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1932 );
		//
	}
	//
}


//
void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_BATCH( int TMATM , int side ) { // 2009.09.23
	int bmc , i , c;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_BATCH_ALL ) return; // 2013.03.09
	//
	switch ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( side ) ) { // 2010.02.01
	case BATCHALL_SWITCH_LL_2_LL :
	case BATCHALL_SWITCH_LP_2_LL :
		return;
		break;
	}
	//
	bmc = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side );
	//
	if ( bmc == 0 ) return;
	//
	if ( !_SCH_MODULE_Chk_TM_Free_Status( side ) ) return;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc , FALSE , -1 ) ) return;
	//
	if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( bmc , G_PICK , 0 ) ) return;
	//
	if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( bmc ) ) return;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) > 0 ) return;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) > 0 ) return; // 2012.06.20
	//
	c = 0;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			c++;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) != BUFFER_OUTWAIT_STATUS ) {
				if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( bmc , i ) , _SCH_MODULE_Get_BM_POINTER( bmc , i ) ) != PATHDO_WAFER_RETURN ) return;
//				return; // 2009.11.20
			}
			//
		}
	}
	//
	if ( c <= 0 ) return;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i++ ) { // 2009.11.20
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			_SCH_MODULE_Set_BM_STATUS( bmc , i , BUFFER_OUTWAIT_STATUS );
		}
	}
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , 1 , TRUE , TMATM+1 , 1688 );
}
//



BOOL SCHEDULER_CONTROL_OTHER_GROUP_CAN_GO_AND_HAVE_SPACE2( int TMATM , int bmc , int s , int p ) {
	int hasother , i , k , m;
	//
	if ( _SCH_CLUSTER_Get_PathRange( s , p ) <= 0 ) return TRUE;
	//
	hasother = FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		//
		if ( m <= 0 ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( m ) == TMATM ) continue;
		//
		hasother = TRUE;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) != 0 ) continue;
//		if ( _SCH_MODULE_Get_PM_WAFER( m,0 ) > 0 ) continue; // 2014.01.10
		if ( !SCH_PM_IS_ABSENT( m , s , p , 0 ) ) continue;
		//
		return TRUE;
		//
	}
	//
	if ( !hasother ) return FALSE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( i == bmc ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != (TMATM+1) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) continue;
		//
		return TRUE;
	}
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_OTHER_GROUP_CAN_GO_AND_HAVE_SPACE( int TMATM , int bmc ) {
	int k;
	//
	for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; k++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc,k ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bmc,k ) != BUFFER_INWAIT_STATUS ) continue;
		//
		if ( SCHEDULER_CONTROL_OTHER_GROUP_CAN_GO_AND_HAVE_SPACE2( TMATM , bmc , _SCH_MODULE_Get_BM_SIDE( bmc,k ) , _SCH_MODULE_Get_BM_POINTER( bmc,k ) ) ) return TRUE;
		//
	}
	//
	return FALSE;
	//
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_FORCEVENT( int TMATM , int side ) { // 2013.03.26
	int i , j;
	//
	if ( TMATM != 0 ) return;
	//
	if ( !_SCH_PRM_GET_MODULE_MODE( TM + 1 ) ) return; // 2016.07.06
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) return;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PLACE ) ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		//
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( j ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_01_USE ) break;
			//
		}
		//
		if ( j >= MAX_CASSETTE_SIDE ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) return;
		//
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) { // 2013.05.29
			//
			// 2016.12.13
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
				//
				if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) {
					//
					if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( TMATM,i ) == 0 ) {
						continue;
					}
					//
				}
			}
			//
			//
			if ( ( SCHEDULER_Get_TM_Pick_BM_Signal( TMATM ) % 1000 ) == i ) continue; // 2013.11.14
			//
			if ( SCHEDULER_CONTROL_OTHER_GROUP_CAN_GO_AND_HAVE_SPACE( TMATM , i ) ) continue;
			//
		}
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1831 );
		//
	}
	//
}

BOOL SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( int side , int ch );
BOOL SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( int tmside , int side , int *bmc ) { // 2014.08.14
	int i , selbm;
	//
	*bmc = 0;
	//
	selbm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
//		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tmside , i , G_PICK , G_MCHECK_SAME ) ) continue; // 2014.09.08
//		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tmside , i , G_PLACE , G_MCHECK_SAME ) ) continue; // 2014.09.08
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue; // 2014.09.08
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2014.09.08
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( selbm == 0 ) {
			selbm = i;
		}
		else {
			selbm = -1;
		}
		//
	}
	//
	if ( selbm >= BM1 ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( i == side ) continue;
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( SCHEDULER_CONTROL_Check_BM_Not_Using_with_Option( i , selbm ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , selbm ) == MUF_01_USE ) break;
			//
		}
		//
		if ( i >= MAX_CASSETTE_SIDE ) {
			*bmc = selbm;
			return TRUE;
		}
		//
	}
	//
	return FALSE;
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_FIRSTGROUP( int TMATM , int side ) { // 2013.03.19
	int i , selbm;
	//
	if ( TMATM != 0 ) return;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) return;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) {
				return;
			}
		}
	}
	//
	if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , FALSE ) ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( i , -1 , TRUE ) ) return;
	}
	//
	if ( SCHEDULER_FM_Current_No_More_Supply2( side ) ) {
		//
		if ( SCHEDULER_FM_Another_No_More_Supply( side , -2 ) ) return;
		//
		if ( SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( TMATM , side , &selbm ) ) return; // 2014.08.14
		//
		//------------------------------------------------------------------------------
	}
	//
	selbm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) return; // 2015.07.03
		//
		if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue;
		//
//		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) return; // 2015.07.03
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( selbm == 0 ) selbm = i;
		//
	}
	//
	if ( selbm != 0 ) {
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1916 );
		//
	}
	//
}

int sch_DUALTM_for_NEXTGROUP_Flag[MAX_TM_CHAMBER_DEPTH]; // 2013.03.19
BOOL OtherArm_Place_Curr_Get_BM( int tms , int Chkjp , int GetChamber ); // 2018.10.20

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_NEXTGROUP( int TMATM , int side ) { // 2013.03.19
	int i , selbm;
	BOOL freew; // 2018.06.12
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) {
		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
		return; // 2012.09.03
	}
	//
//	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) { // 2018.05.08
//		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
//		return;
//	}
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) {
		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
		return;
	}
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) { // 2016.10.30
		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM + 1 ) != BUFFER_SWITCH_FULLSWAP ) {
		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
		return;
	}
	//
	selbm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != (TMATM+1) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
		//
//		if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue; // 2018.06.12
		//
		freew = SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ); // 2018.06.12
		//
		if ( freew ) { // 2018.06.12
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) {
				sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
				return;
			}
			//
		}
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( selbm == 0 ) {
			//
			// 2018.06.12
			//
			if ( freew ) {
				//
//				if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) { // 2018.09.17 // 2018.12.06
					if ( SCHEDULING_CHECK_for_TM_ARM_Has_Next_Group( TMATM ) ) continue;
//				} // 2018.12.06
				//
				if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM + 1 , TRUE ) ) {
					if ( !SCHEDULING_CHECK_for_PM_Has_Return_Wafer( TMATM + 1 ) ) {
						if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM + 1 , -1 , -1 , 5 , FALSE ) != 1 ) {
							//
							continue;
							//
						}
					}
				}
			}
			else {
				//
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) > 0 ) continue;
				//
				if ( SCHEDULING_CHECK_for_TM_ARM_Has_Next_Group( TMATM ) ) continue;
				//
				if ( SCHEDULING_CHECK_for_BM_Has_Next_Group( TMATM , side ) ) continue;
				//
			}
			//
			if ( OtherArm_Place_Curr_Get_BM( TMATM , 1 , i ) ) continue; // 2018.10.20
			//
			selbm = i;
			//
		}
		//
	}
	//
	if ( selbm != 0 ) {
		//
//		if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM + 1 , TRUE ) ) { // 2018.06.12
//			if ( !SCHEDULING_CHECK_for_PM_Has_Return_Wafer( TMATM + 1 ) ) { // 2018.06.12
//				if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM + 1 , -1 , -1 , 5 , FALSE ) != 1 ) { // 2013.04.25 // 2018.06.12
//					selbm = 0; // 2018.06.12
//				} // 2018.06.12
//			} // 2018.06.12
//			//
//		} // 2018.06.12
		//
//		if ( selbm != 0 ) { // 2018.06.12
			//
			sch_DUALTM_for_NEXTGROUP_Flag[TMATM]++;
			//
			if ( sch_DUALTM_for_NEXTGROUP_Flag[TMATM] >= 3 ) {
				//
				sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
				//
//				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1917 ); // 2017.03.15
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1919 ); // 2017.03.15
				//
			}
			//
//		} // 2018.06.12
//		else { // 2018.06.12
//			sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0; // 2018.06.12
//		} // 2018.06.12
		//	
	}
	else {
		sch_DUALTM_for_NEXTGROUP_Flag[TMATM] = 0;
	}
	//
}


int sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[MAX_TM_CHAMBER_DEPTH]; // 2018.09.17

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_SINGLEARM_LOOP_LOCK_FULLSWAP( int TMATM , int side ) { // 2018.09.17
	int i , lockfind;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM + 1 ) != BUFFER_SWITCH_FULLSWAP ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM+1,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM+1,TB_STATION ) ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Next_Group( TMATM ) ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM + 1 , TRUE ) ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	lockfind = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != (TMATM+1) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) {
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
				sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
				return;
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
				sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
				return;
			}
			//
			lockfind++;
			//
		}
		//
		if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM + 1 , i , G_PLACE , G_MCHECK_ALL ) ) {
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
				sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
				return;
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
				sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
				return;
			}
			//
			lockfind++;
			//
		}
	}
	//
	if ( lockfind == 0 ) {
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		return;
	}
	//
	sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM]++;
	//
	if ( sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] >= 3 ) {
		//
		sch_DUALTM_for_SINGLEARM_LOOP_LOCK_Flag[TMATM] = 0;
		//
		if      ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) ) {
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION ) , -1 );
		}
		else if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) ) {
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION ) , -1 );
		}
		//
	}
	//
}


int SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[MAX_TM_CHAMBER_DEPTH]; // 2018.09.17

BOOL SCHEDULING_CHECK_for_BM_Has_Next_Group_and_Has_Out( int tmside , BOOL outchk , int bm , int *supid ) { // 2018.09.17
	//
	int s , p , k , j , m;
	BOOL hasout = FALSE;
	BOOL hasother = FALSE;
	//
	if ( supid != NULL ) *supid = -1;
	//
	for ( j = _SCH_PRM_GET_MODULE_SIZE( bm ) ; j >= 1 ; j-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , j ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_STATUS( bm , j ) != BUFFER_INWAIT_STATUS ) {
			hasout = TRUE;
			continue;
		}
		//
		s = _SCH_MODULE_Get_BM_SIDE( bm , j );
		p = _SCH_MODULE_Get_BM_POINTER( bm , j );
		//
		if ( tmside == 0 ) { // 2018.09.19
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		}
		else { // 2018.09.19
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		}
		//
		if ( supid != NULL ) {
			if ( *supid == -1 ) {
				*supid = _SCH_CLUSTER_Get_SupplyID( s , p );
			}
			else {
				if ( *supid < _SCH_CLUSTER_Get_SupplyID( s , p ) ) {
					*supid = _SCH_CLUSTER_Get_SupplyID( s , p );
				}
			}
		}
		//
//		if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) { // 2018.09.19
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				if ( tmside == 0 ) { // 2018.09.19
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
				}
				else {
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
				}
				//
				if ( m > 0 ) {
					//
					if ( _SCH_PRM_GET_MODULE_GROUP( m ) > tmside ) {
						hasother = TRUE;
					}
					//
				}
				//
			}
		// }
	}
	//
	if ( !hasother ) return FALSE;
	if ( outchk ) {
		if ( !hasout ) return FALSE;
	}
	return TRUE;
}

BOOL SCHEDULING_CHECK_for_BM_Has_Same_Group_and_Has_Run( int side , int tmside , int bm ) { // 2018.09.19
	//
	int s , p , k , j , m , pd;
	BOOL find = FALSE;
	//
	for ( j = _SCH_PRM_GET_MODULE_SIZE( bm ) ; j >= 1 ; j-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bm , j ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_BM_SIDE( bm , j );
		p = _SCH_MODULE_Get_BM_POINTER( bm , j );
		//
		if ( tmside == 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		}
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( tmside == 0 ) {
				pd = _SCH_CLUSTER_Get_PathDo( s , p );
			}
			else {
				pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
			}
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k );
			//
			if ( m > 0 ) {
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) == tmside ) {
					//
					find = TRUE;
					//
					if ( SCH_PM_IS_ABSENT( m , s , p , pd ) ) {
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
							//
							return FALSE;
							//
						}
					}
				}
				//
			}
			//
		}
		//
	}
	//
	return find;
	//
}

BOOL SCHEDULING_CHECK_for_BM_Has_Next_Group_All_return( int side , int tmside , int bm ) {
	int i;
	//
	if ( bm >= BM1 ) { // 2018.09.19
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( TM + tmside + 1 ) ) return FALSE; // 2018.09.19
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tmside + 1 ) != BUFFER_SWITCH_FULLSWAP ) return FALSE;
		//
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside,TB_STATION ) ) return FALSE;
		//
		if ( !SCHEDULING_CHECK_for_BM_Has_Next_Group_and_Has_Out( tmside , FALSE , bm , NULL ) ) return FALSE;
		//
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != (tmside+1) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tmside , i , G_PLACE , G_MCHECK_ALL ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) return FALSE;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return FALSE;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) return FALSE;
		//
	}
	//
	return TRUE;
	//
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP( int TMATM , int side ) { // 2018.09.17
	int i , lockfind , selbm , selsup , supid , chk;
	//
	if ( TMATM != 0 ) return;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM + 1 ) != BUFFER_SWITCH_FULLSWAP ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	lockfind = 0;
	selbm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) {
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
				SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
				return;
			}
			//
			if ( !SCHEDULING_CHECK_for_BM_Has_Next_Group_and_Has_Out( TMATM , TRUE , i , &supid ) ) continue;
			//
			if ( selbm == 0 ) {
				selbm = i;
				selsup = supid;
			}
			else {
				if ( selsup < supid ) {
					selbm = i;
					selsup = supid;
				}
			}
			//
		}
		else if ( _SCH_PRM_GET_MODULE_GROUP( i ) == ( TMATM + 1 ) ) {
			//
			chk = 0;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) chk = 1;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
				SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
				return;
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) {
				SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
				return;
			}
			//
			if ( chk == 1 ) {
				if ( !SCHEDULING_CHECK_for_BM_Has_Next_Group_All_return( side , TMATM + 1 , i ) ) {
					if ( !SCHEDULING_CHECK_for_BM_Has_Same_Group_and_Has_Run( side , TMATM + 1 , i ) ) { // 2018.09.19
						SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
						return;
					}
				}
			}
			//
			lockfind++;
			//
		}
		//
	}
	//
	if ( ( selbm == 0 ) || ( lockfind == 0 ) ) {
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		return;
	}
	//
	SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM]++;
	//
	if ( SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] >= 3 ) {
		//
		SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP[TMATM] = 0;
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM , 1921 );
		//
	}
	//
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_DUALTM_for_SAMEGROUP( int TMATM , int side ) { // 2013.03.19
	int i , selbm;
	//
	if ( TMATM == 0 ) return;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
//	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) return; // 2018.05.08
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) return;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return;
//	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return; // 2013.05.29
	//
	if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE ) ) {
		return;
	}
	else {
		//
		if ( SCHEDULING_CHECK_for_PM_Has_Return_Wafer( TMATM ) ) {
			return;
		}
		else {
			if ( SCHEDULING_CHECK_for_TM_ARM_Next_Place_Impossible( TMATM , -1 , -1 , 5 , FALSE ) == 1 ) { // 2013.04.25
				return;
			}
		}
		//
	}
	//
	selbm = 0;
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
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) continue;
		//
		if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( selbm == 0 ) selbm = i;
		//
	}
	//
	if ( selbm != 0 ) {
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1918 );
		//
	}
	//
}



void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_DUALTM_for_OTHERGROUPGO( int TMATM , int side ) { // 2013.03.26
	int i , k , s , p , f , bms;
	int lock_returncm , f_s , f_p;
	//
	if ( TMATM == 0 ) return;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
//	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) return; // 2018.05.08
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) return;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM-1,TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( TMATM-1,TB_STATION ) > 0 ) ) {
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM-1,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM-1,TB_STATION ) > 0 ) ) return;
		if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM-1 , TRUE ) ) return;
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != ( TMATM - 1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM - 1 , i , G_PICK , G_MCHECK_NO ) ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) return;
		//
	}
	//
	f = -1;
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
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_NO ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) > 0 ) return;
			//
		}
		//
		s = -1;
		bms = -1;
		//
		for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( i ) ; k++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i,k ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i,k ) != BUFFER_INWAIT_STATUS ) {
				s = -1;
				break;
			}
			//
			s = _SCH_MODULE_Get_BM_SIDE( i,k );
			p = _SCH_MODULE_Get_BM_POINTER( i,k );
			//
			if ( bms == -1 ) bms = k;
			//
		}
		//
		if ( s == -1 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		if ( SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS_CURR_TM( TMATM , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , i , TRUE ) ) continue; // 2016.12.13
		//
		if ( SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS( TMATM - 1 , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , i , 0 , TRUE ) ) { // 2013.07.13
			//
			if ( f == -1 ) {
				f = i;
				f_s = s;
				f_p = p;
			}
			//
		}
		//
	}
	//
	if ( f != -1 ) {
		//
		lock_returncm = !SCHEDULER_CONTROL_DUALTM_for_PICK_FROM_BM_POSSIBLE_WHEN_USE_ARMINTLKS( TMATM - 1 , f_s , f_p , _SCH_CLUSTER_Get_PathDo( f_s , f_p ) - 1 , f , 1 , FALSE ); // 2013.07.13
		//
		for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( f ) ; k++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( f,k ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( f,k ) == BUFFER_INWAIT_STATUS ) {
				_SCH_MODULE_Set_BM_STATUS( f,k , BUFFER_OUTWAIT_STATUS );
				//
				if ( lock_returncm ) {
					//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d RETURN WAFER 8 in SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_DUALTM_for_OTHERGROUPGO %d,%d\n" , TMATM + 1 , f , k );
	//----------------------------------------------------------------------

					_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_BM_SIDE( f , k ) , _SCH_MODULE_Get_BM_POINTER( f , k ) , -1 );
					//
				}
				//
			}
			//
		}
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , f , 1 , TRUE , TMATM+1 , 1920 );
		//
	}
}




void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_TM_SINGLE( int TMATM , int side ) { // 2016.10.20 // TM Single All run , BM Not go Space
	int i , j , k , s , p , m , f ,sc , lc;
	int pd; // 2018.09.19
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( TMATM ) == 0 ) return;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2018.09.19
//		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) { // 2018.09.19
//			return; // 2018.09.19
//		} // 2018.09.19
//	} // 2018.09.19
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2017.01.23
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		//--------------------------------------------------------------------------------------
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		f = 0;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
			}
			else if ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) {
			}
			else {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j+1 ) ) { // 2018.04.03
					//
					f = 1;
					break;
					//
				}
			}
			//
			j++;
		}
		//
		if ( f == 1 ) continue;
		//
		//--------------------------------------------------------------------------------------
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		f = 0;
		//
		for ( j = sc ; j <= lc ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
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
			if ( f == 0 ) f = 1;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				if ( TMATM == 0 ) { // 2018.09.19
					pd = _SCH_CLUSTER_Get_PathDo( s , p );
				}
				else { // 2018.09.19
					pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
				}
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k ); // 2018.09.19
				//
				if ( m > 0 ) {
					//
					if ( SCH_PM_IS_ABSENT( m , s , p , pd ) ) { // 2018.09.19
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
							if ( _SCH_PRM_GET_MODULE_GROUP( m ) > TMATM ) { // 2018.09.19
								if ( !SCHEDULING_CHECK_for_BM_Has_Next_Group_All_return( side , TMATM , -1 ) ) { // 2018.09.19
									//
									f = 2;
									//
									j = 9999;
									//
									break;
								}
							}
							else {
								//
								f = 2;
								//
								j = 9999;
								//
								break;
							}
						}
					}
					//
				}
			}
			//
		}
		//
		if ( f != 1 ) continue;
		//
		//--------------------------------------------------------------------------------------
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1922 );
		//
		//
	}
}



//
void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB9_FULLSWAP( int TMATM , int side ) { // 2018.10.31
	int i , k , ok , sel;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) return;
	//
	if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE ) ) return;
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , -1 ) ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		if ( SCH_PM_IS_PICKING( i ) ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) return;
		}
	}
	//
	sel = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) == ( TMATM + 1 ) ) { // 2018.11.27
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_ALL ) ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
			//
			ok = 0;
			//
			for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( i ) ; k++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i,k ) <= 0 ) continue;
				//
				if ( _SCH_MODULE_Get_BM_STATUS( i,k ) != BUFFER_OUTWAIT_STATUS ) {
					ok = 0;
					break;
				}
				//
				ok = 1;
				//
			}
			//
			if ( ok == 0 ) continue;
			//
			return;
		}
		else if ( _SCH_PRM_GET_MODULE_GROUP( i ) == TMATM ) {
			//
			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_ALL ) ) continue;
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			ok = 0;
			//
			for ( k = 1 ; k <= _SCH_PRM_GET_MODULE_SIZE( i ) ; k++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i,k ) <= 0 ) continue;
				//
				if ( _SCH_MODULE_Get_BM_STATUS( i,k ) != BUFFER_OUTWAIT_STATUS ) {
					ok = 0;
					break;
				}
				//
				ok = 1;
				//
			}
			//
			if ( ok != 0 ) {
				//
				if ( sel == -1 ) sel = i;
				//
			}
			//
		}
		//
	}
	//
	if ( sel != -1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , sel , 1 , TRUE , TMATM+1 , 81031 );
}
//



BOOL SCHEDULER_NEXT_GO_PM_FREE( int s , int p , int *onepm , BOOL stchk ) { // 2013.05.07
	int m , k , f = FALSE , pc = -1 , prsts;
	//
	*onepm = -1;
	//
	if ( stchk ) { // 2013.06.11
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return TRUE;
	}
	//
	if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 0 ) return FALSE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) {
				if      ( *onepm == -1 ) {
					*onepm = m;
				}
				else if ( *onepm != 0 ) {
					*onepm = 0;
				}
				prsts = _SCH_MACRO_CHECK_PROCESSING_INFO( m );
				if ( ( prsts <= 0 ) && ( prsts != -1 ) ) {
//					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( m , s , p , 0 ) ) {
						f = TRUE;
					}
				}
			}
		}
	}
	//
	return f;
}

BOOL SCHEDULER_HAS_GO_OTHER_PM( int s , int pm ) {
	int p , m , k;
	//
	if ( !_SCH_SUB_Check_Complete_Return( s ) ) return FALSE;
	if ( !_SCH_SUB_Remain_for_FM( s ) ) return FALSE;
	if ( _SCH_SYSTEM_MODE_END_GET( s ) != 0 ) return FALSE;
	//======================================================================================================================================
	p = _SCH_MODULE_Get_FM_DoPointer( s );
	if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	if ( pm == -2 ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) {
				if ( pm != m ) {
//					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) { // 2014.01.10
					if ( SCH_PM_IS_ABSENT( m , s , p , 0 ) ) {
						return TRUE;
					}
				}
			}
		}
	}
	//
	return FALSE;
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_ONEFMTMARM_SWITCH( int TMATM , int side ) { // 2013.05.07
	int k , s , p , w , op , selbm , freecount , onepm;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() % 2 ) == 0 ) return; // 2013.05.28
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return;
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() != 0 ) return;
	if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) return;
	//
//	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 ) return; // 2016.10.30
	if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( ( TMATM != 0 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) ) ) return; // 2016.10.30
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
			return;
		}
	}
	//
//	if ( SCHEDULER_FM_Another_No_More_Supply( -1 , -1 ) ) return; // 2017.02.10
	if ( SCHEDULER_FM_All_No_More_Supply() != -1 ) return; // 2017.02.10
	//
	selbm = -1;
	freecount = 0;
	onepm = 0;
	//
	for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( k ) != 0 ) ) continue;
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , k ) == MUF_00_NOTUSE ) return;
			//
		}
		//
		if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) return;
		//
		w = 0;
		//
		if ( _SCH_MODULE_Get_BM_WAFER( k,1 ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( k,1 ) != BUFFER_INWAIT_STATUS ) return;
			//
			s = _SCH_MODULE_Get_BM_SIDE( k,1 );
			p = _SCH_MODULE_Get_BM_POINTER( k,1 );
			//
			if ( SCHEDULER_NEXT_GO_PM_FREE( s , p , &op , TRUE ) ) {
				w = 2;
			}
			else {
				w = 1;
				//
				onepm = op;
				//
			}
			//
		}
		else {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) return;
		}
		//
		if ( w != 1 ) freecount++;
		//
		if ( w != 0 ) continue;
		//
		if ( selbm == -1 ) {
			selbm = k;
		}
		else {
			if ( _SCH_TIMER_GET_PROCESS_TIME_START( 1 /* BUFFER_TM_STATION */ , selbm ) > _SCH_TIMER_GET_PROCESS_TIME_START( 1 /* BUFFER_TM_STATION */ , k ) ) {
				selbm = k;
			}
		}
		//
	}
	//
	if ( freecount < 1 ) {
		return;
	}
	//
	if ( freecount == 1 ) {
		//
		if ( onepm <= 0 ) return;
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//
			if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
			//
			if ( SCHEDULER_HAS_GO_OTHER_PM( s , onepm ) ) break;
			//
		}
		//
		if ( s == MAX_CASSETTE_SIDE ) return;
	}
	//
	if ( selbm != -1 ) {
//		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbm , 1 , TRUE , TMATM+1 , 1988 ); // 2014.12.26
		SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( side , TMATM , selbm , 1988 ); // 2014.12.26
	}
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_WAIT_BM_SWITCH( int TMATM , int side ) { // 2014.01.03
	int bmc;
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) return;
	//
	bmc = FM_Place_Waiting_for_BM_style_0;
	//
	if ( bmc < BM1 ) return;
	if ( bmc >= TM ) return;
	//
	if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( bmc ) ) return;
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) > 0 ) return;
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , -1 ) > 0 ) return;
	if ( FM_Place_Waiting_for_BM_style_0 != bmc ) return;
	//=====================================================================
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , TMATM+1 , 1301 );
	//=====================================================================
	FM_Place_Waiting_for_BM_style_0 = 0;
	//=====================================================================
}
//

//BOOL SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( int s , int p , int d , int ci , int pmc , BOOL *hasotherci ) { // 2014.01.10 // 2018.12.06
BOOL SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( int s , int p , int ci , int pmc , BOOL *hasotherci ) { // 2014.01.10 // 2018.12.06
	int i , k , r;
	//

	if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) return FALSE; // 2018.12.06
	//
//	r = _SCH_CLUSTER_Get_PathRange( s , p ); // 2016.05.03 // 2018.12.06
	//
//	if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE; // 2016.05.03
//	if ( d >= r ) return FALSE; // 2016.05.03 // 2018.12.06
	//
	if ( ci != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) {
		*hasotherci = TRUE;
		return FALSE;
	}
	//
	// 2016.05.03
	//
//	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//		//
//		if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , d , k ) == pmc ) return TRUE;
//		//
//	}
	//
	// 2016.05.03
	//
	//
	r = _SCH_CLUSTER_Get_PathRange( s , p ); // 2018.12.06
	//
//	for ( i = d ; i < r ; i++ ) { // 2018.12.06
	for ( i = 0 ; i < r ; i++ ) { // 2018.12.06
		//
		if ( _SCH_CLUSTER_Get_PathRun( s , p , i , 0 ) != 0 ) continue; // 2018.12.06
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
//			if ( SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , i , k ) == pmc ) return TRUE; // 2018.12.06
			if ( _SCH_CLUSTER_Get_PathProcessChamber( s , p , i , k ) == pmc ) return TRUE; // 2018.12.06
			//
		}
		//
		break; // 2018.12.06
		//
	}
	//
	return FALSE;
}


/*
//
// 2018.12.05
//
BOOL SCHEDULING_CHECK_FBTM_HAS_SAME_CLUSTER_INDEX_MULTI( int TMATM , int pmc , int ci , BOOL *hasotherci ) { // 2014.01.10
	int i , j , s , p , x , chksum[2];
	//
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		*hasotherci = FALSE;
		//
		chksum[x] = 0;
		//
		for ( j = 0 ; j < 2 ; j++ ) {
			//
			if ( _SCH_MODULE_Get_FM_WAFER( j ) <= 0 ) continue;
			//
			chksum[x] += ( FM * 1000 ) + j;
			//
			s = _SCH_MODULE_Get_FM_SIDE( j );
			p = _SCH_MODULE_Get_FM_POINTER( j );
			//
			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , ci , pmc , hasotherci ) ) return TRUE;
			//
		}
		//
		for ( j = 1 ; j <= 2 ; j++ ) {
			//
			if ( _SCH_MODULE_Get_MFALS_WAFER( j ) <= 0 ) continue;
			//
			chksum[x] += ( F_AL * 1000 ) + j;
			//
			s = _SCH_MODULE_Get_MFALS_SIDE( j );
			p = _SCH_MODULE_Get_MFALS_POINTER( j );
			//
			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , ci , pmc , hasotherci ) ) return TRUE;
			//
		}
		//
		/ *
		//
		// 2018.05.08
		//
		for ( j = 0 ; j < 2 ; j++ ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( TMATM , j ) <= 0 ) continue;
			//
			chksum[x] += ( (TM+TMATM) * 1000 ) + j;
			//
			s = _SCH_MODULE_Get_TM_SIDE( TMATM , j );
			p = _SCH_MODULE_Get_TM_POINTER( TMATM , j );
			//
			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , ci , pmc , hasotherci ) ) return TRUE;
			//
		}
		* /
		//
		// 2018.05.08
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			for ( j = 0 ; j < 2 ; j++ ) {
				//
				if ( _SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) continue;
				//
				chksum[x] += ( (TM+i) * 1000 ) + j;
				//
				s = _SCH_MODULE_Get_TM_SIDE( i , j );
				p = _SCH_MODULE_Get_TM_POINTER( i , j );
				//
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , ci , pmc , hasotherci ) ) return TRUE;
				//
			}
		}
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
//			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue; // 2018.06.01
//			if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PLACE , G_MCHECK_SAME ) ) continue; // 2018.06.01
			//
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
				//
				chksum[x] += ( i * 1000 ) + j;
				//
				s = _SCH_MODULE_Get_BM_SIDE( i , j );
				p = _SCH_MODULE_Get_BM_POINTER( i , j );
				//
//				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ( TMATM == 0 ) ? _SCH_CLUSTER_Get_PathDo( s , p ) : _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , ci , pmc , hasotherci ) ) return TRUE; // 2018.06.01
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ? _SCH_CLUSTER_Get_PathDo( s , p ) : _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , ci , pmc , hasotherci ) ) return TRUE; // 2018.06.01
				//
			}
		}
		//
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) { // 2016.05.30
			//
			if ( i == pmc ) continue; // 2018.05.08
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			for ( j = 0 ; j < _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				if ( _SCH_MODULE_Get_PM_WAFER( i , j ) <= 0 ) continue;
				//
				chksum[x] += ( i * 1000 ) + j;
				//
				s = _SCH_MODULE_Get_PM_SIDE( i , j );
				p = _SCH_MODULE_Get_PM_POINTER( i , j );
				//
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , ci , pmc , hasotherci ) ) return TRUE;
				//
			}
		}
	}
	//
	if ( chksum[0] != chksum[1] ) return TRUE;
	//
	return FALSE;
}

*/

BOOL SCHEDULING_CHECK_LOT_HAS_SAME_CLUSTER_INDEX_MULTI_WAIT( int s , int p , int ci , int ch ) {
	int i , k , pts;
	//
//	if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2018.12.19
//		pts = _SCH_MODULE_Get_TM_DoPointer( s ); // 2018.12.19
//	} // 2018.12.19
//	else { // 2018.12.19
//		pts = _SCH_MODULE_Get_FM_DoPointer( s ); // 2018.12.19
//	} // 2018.12.19
	//
	pts = 0; // 2018.12.19
	//
	for ( i = pts ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue;
		//
		if ( _SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_READY ) {
			if ( _SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_SUPPLY ) { // 2018.12.19
				continue;
			}
		}
		//
		if ( ci != _SCH_CLUSTER_Get_ClusterIndex( s ,i ) ) return FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( ch == SCHEDULER_CONTROL_Get_PathProcessChamber( s , i , 0 , k ) ) {
				return TRUE;
			}
			//
		}
		//
		return FALSE;
		//
	}
	return FALSE;
}

BOOL SCHEDULING_CHECK_FBTM_HAS_SAME_CLUSTER_INDEX_MULTI( int TMATM , int pmc , int ci , BOOL *hasotherci ) { // 2014.01.10
	int i , j , s , p , w , x;
	int chksum1[2] , chksum2[2];
	//
//	int pd;
//	int d_m[32] , d_l[32] , d_s[32] , d_p[32] , d_w[32] , d_d[32] , dc;
	//

//dc = 0;


	for ( x = 0 ; x < 2 ; x++ ) {
		//
		//===================================================================================================================
		//===================================================================================================================
		//
		*hasotherci = FALSE;
		//
		chksum1[x] = 0;
		chksum2[x] = 0;
		//
		//===================================================================================================================
		//===================================================================================================================
		//


		for ( j = 0 ; j < 2 ; j++ ) {
			//
			w = _SCH_MODULE_Get_FM_WAFER( j );
			//
			if ( w <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_FM_SIDE( j );
			p = _SCH_MODULE_Get_FM_POINTER( j );
//			pd = _SCH_CLUSTER_Get_PathDo( s , p );
			//
//			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , pd , ci , pmc , hasotherci ) ) return TRUE;
			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ci , pmc , hasotherci ) ) return TRUE;
			//
			chksum1[x] += ( ( s * 1000000 ) + ( p * 1000 ) + w );
			chksum2[x] += ( ( FM * 1000000 ) + ( (j+1) * 1000 ) + 1 );
			//


//			d_m[dc] = ( FM * 10 ) + 1;
//			d_l[dc] = j;
//			d_s[dc] = s;
//			d_p[dc] = p;
//			d_w[dc] = w;
//			d_d[dc] = pd;
//			dc++;


		}
		//
		//===================================================================================================================
		//===================================================================================================================
		//
		for ( j = 1 ; j <= 2 ; j++ ) {
			//
			w = _SCH_MODULE_Get_MFALS_WAFER( j );
			//
			if ( w <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_MFALS_SIDE( j );
			p = _SCH_MODULE_Get_MFALS_POINTER( j );
//			pd = _SCH_CLUSTER_Get_PathDo( s , p );
			//
//			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , pd , ci , pmc , hasotherci ) ) return TRUE;
			if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ci , pmc , hasotherci ) ) return TRUE;
			//
			chksum1[x] += ( ( s * 1000000 ) + ( p * 1000 ) + w );
			chksum2[x] += ( ( F_AL * 1000000 ) + ( (j+1) * 1000 ) + 2 );
			//


//			d_m[dc] = ( F_AL * 10 ) + 2;
//			d_l[dc] = j;
//			d_s[dc] = s;
//			d_p[dc] = p;
//			d_w[dc] = w;
//			d_d[dc] = pd;
//			dc++;



		}
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( !_SCH_PRM_GET_MODULE_MODE( TM + i ) ) continue;
			//
			for ( j = 0 ; j < 2 ; j++ ) {
				//
				w = _SCH_MODULE_Get_TM_WAFER( i , j );
				//
				if ( w <= 0 ) continue;
				//
				s = _SCH_MODULE_Get_TM_SIDE( i , j );
				p = _SCH_MODULE_Get_TM_POINTER( i , j );
//				pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
				//
//				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , pd , ci , pmc , hasotherci ) ) return TRUE;
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ci , pmc , hasotherci ) ) return TRUE;
				//
				chksum1[x] += ( ( s * 1000000 ) + ( p * 1000 ) + w );
				chksum2[x] += ( ( (TM+i) * 1000000 ) + ( (j+1) * 1000 ) + 3 );
				//




//			d_m[dc] = ( TM+i ) * 10 + 3;
//			d_l[dc] = j;
//			d_s[dc] = s;
//			d_p[dc] = p;
//			d_w[dc] = w;
//			d_d[dc] = pd;
//			dc++;








			}
		}
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
			//
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				w = _SCH_MODULE_Get_BM_WAFER( i , j );
				//
				if ( w <= 0 ) continue;
				//
				s = _SCH_MODULE_Get_BM_SIDE( i , j );
				p = _SCH_MODULE_Get_BM_POINTER( i , j );
				//
//				pd = ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ? _SCH_CLUSTER_Get_PathDo( s , p ) : _SCH_CLUSTER_Get_PathDo( s , p ) - 1; // Synch 문제
//				if ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) {
//					pd = _SCH_CLUSTER_Get_PathDo( s , p );
//					if ( pd <= 1 ) pd = 0;
//				}
//				else {
//					pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
//				}
				//
//				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , pd , ci , pmc , hasotherci ) ) return TRUE;
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ci , pmc , hasotherci ) ) return TRUE;
				//
				chksum1[x] += ( ( s * 1000000 ) + ( p * 1000 ) + w );
				chksum2[x] += ( ( i * 1000000 ) + ( (j+1) * 1000 ) + 4 );
				//


//			d_m[dc] = (i*10) + 4;
//			d_l[dc] = j;
//			d_s[dc] = s;
//			d_p[dc] = p;
//			d_w[dc] = w;
//			d_d[dc] = pd;
//			dc++;



			}
		}
		//
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) { // 2016.05.30
			//
			if ( i == pmc ) continue; // 2018.05.08
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
			for ( j = 0 ; j < _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				if ( _SCH_MODULE_Get_PM_PICKLOCK( i , j ) > 0 ) continue;
				//
				w = _SCH_MODULE_Get_PM_WAFER( i , j );
				//
				if ( w <= 0 ) continue;
				//
				s = _SCH_MODULE_Get_PM_SIDE( i , j );
				p = _SCH_MODULE_Get_PM_POINTER( i , j );
//				pd = _SCH_CLUSTER_Get_PathDo( s , p );
				//
//				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , pd , ci , pmc , hasotherci ) ) return TRUE;
				if ( SCHEDULING_CHECK_BTM_HAS_SAME_PM_MULTI( s , p , ci , pmc , hasotherci ) ) return TRUE;
				//
				chksum1[x] += ( ( s * 1000000 ) + ( p * 1000 ) + w );
				chksum2[x] += ( ( i * 1000000 ) + ( (j+1) * 1000 ) + 5 );
				//

//			d_m[dc] = (i*10) + 5;
//			d_l[dc] = j;
//			d_s[dc] = s;
//			d_p[dc] = p;
//			d_w[dc] = w;
//			d_d[dc] = pd;
//			dc++;


			}
		}
	}
	//
	if ( chksum1[0] != chksum1[1] ) return TRUE;
	if ( chksum2[0] != chksum2[1] ) return TRUE;
	//



//	for ( i = 0 ; i < dc ; i++ ) {
//		printf( "[%d] [M=%d][L=%d] [S=%d][P=%d][W=%d] [D=%d]\n" , i , d_m[i] , d_l[i] , d_s[i] , d_p[i] , d_w[i] , d_d[i] ); 
//	}

	*hasotherci = FALSE; // 2018.12.19

	return FALSE;
}

void SCHEDULING_CHECK_SWITCHING_MULTI_PM_SPAWN_PROCESS( int TMATM , int side ) { // 2014.01.10
	int i , l , pr , ci , s , p;
	BOOL hasotherci , hasspace;
	//
	if ( !MULTI_PM_USE ) return;
	//
	for ( i = PM1 ; i < MAX_PM_CHAMBER_DEPTH + PM1 ; i++ ) {
		//
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PICK ) ) continue;
		if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , i , G_PLACE ) ) continue;
		//
//		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue; // 2018.10.20
//		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -2 ) ) continue; // 2018.10.20 // 2019.05.20
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		pr = FALSE;
		//
		hasspace = FALSE;
		//
		for ( l = 0 ; l < _SCH_PRM_GET_MODULE_SIZE( i ) ; l++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( i , l ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , l + 1 ) ) hasspace = TRUE; // 2018.10.20
				//
				continue;
				//
			}
			//
			if ( _SCH_MODULE_Get_PM_PICKLOCK( i , l ) <= 0 ) {
				pr = FALSE;
				break;
			}
			//
			// 2018.10.20
			//
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , l + 1 ) ) {

				if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 8 ) == 1 ) {

					_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_PM_SIDE( i , l ) , _SCH_MODULE_Get_PM_POINTER( i , l ) , -1 );
					//
					_SCH_MODULE_Set_PM_PICKLOCK( i , l , 0 );
					//
					pr = FALSE;
					//
					break;
				}

			}
			//
			//
			if ( !pr ) {
				//
				s = _SCH_MODULE_Get_PM_SIDE( i , l );
				p = _SCH_MODULE_Get_PM_POINTER( i , l );
				//
				ci = _SCH_CLUSTER_Get_ClusterIndex( s , p );
				//
				pr = TRUE;
				//
			}
			//
		}
		//
		if ( !pr ) continue;
		//
		//==================================================================================
		//
		hasotherci = TRUE; // 2018.12.19
		//
		if ( !hasspace || !SCHEDULING_CHECK_FBTM_HAS_SAME_CLUSTER_INDEX_MULTI( TMATM , i , ci , &hasotherci ) ) {
			//
			if ( SCH_PM_IS_ALL_PLACED_PROCESSING( i , FALSE , SCHEDULING_CHECK_LOT_HAS_SAME_CLUSTER_INDEX_MULTI_WAIT( s , p , ci , i ) , &hasotherci ) ) {
				//
				_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( s , p , -1 , i , ( TMATM * 100 ) + 0 , -1 , 1901 );
				//
			}
			//
		}
		//
		//==================================================================================
	}
	//
}

//BOOL Scheduler_Process_Valid_And_Running( int s , int p ) { // 2014.01.25 // 2014.03.18
BOOL Scheduler_Process_Valid_And_Running( int s , int p , int tms ) { // 2014.01.25 // 2014.03.18
	int m , k;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , k );
		//
		if ( m > 0 ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) {
				return FALSE;
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tms , m , G_PLACE ) ) { // 2014.03.18
					return FALSE;
				}
			}
			//
		}
	}
	return TRUE;
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_TM_DOUBLE( int TMATM , int side ) { // 2014.01.25
	int i , j , s , p , sc , lc , arm , bmc , bms , cs , cido , cidi;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return; // 2016.07.22 // 2016.07.29
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2016.07.22
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) { // 2016.07.22
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2017.01.23
	//
	//
	// 2016.10.26
	//
	if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
		if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
			return;
		}
		if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
			return;
		}
	}
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) >  0 ) ) return;
//	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) <= 0 ) ) return; // 2014.03.18
	//
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
		arm = TA_STATION;
	}
	else if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
		arm = TB_STATION;
	}
	else { // 2014.03.18
		return;
	}
	//
//	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm ); // 2014.03.18
//	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm ); // 2014.03.18
	//
	/*
	// 2014.07.17
	//
	// 2014.03.18
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) % 100 ) > 0 ) {
		//
		s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
		p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
		//
	}
	else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,arm ) / 100 ) > 0 ) {
		//
		s = _SCH_MODULE_Get_TM_SIDE2( TMATM , arm );
		p = _SCH_MODULE_Get_TM_POINTER2( TMATM , arm );
		//
	}
	else {
		return;
	}
	*/
	//
	// 2014.07.17
	//
	s = _SCH_MODULE_Get_TM_SIDE( TMATM , arm );
	p = _SCH_MODULE_Get_TM_POINTER( TMATM , arm );
	//
	if ( ( _SCH_CLUSTER_Get_PathDo( s , p ) - 1 ) != 0 ) return;
//	if ( !Scheduler_Process_Valid_And_Running( s , p ) ) return; // 2014.03.18
	if ( !Scheduler_Process_Valid_And_Running( s , p , TMATM ) ) return; // 2014.03.18
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_TM_DOUBLE %d,%d\n" , TMATM + 1 , s , p );
	//----------------------------------------------------------------------
	bmc = -1;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , i ) == MUF_00_NOTUSE ) continue; // 2014.06.27
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) return;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		cidi = -1;
		cs = 0;
		//
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) { // 2016.07.22
			//
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					cidi = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
					cs = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
					//
					// 2016.10.26
					//
					if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) ) == 0 ) {
						//
						if ( !Scheduler_Process_Valid_And_Running( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) , TMATM ) ) return;
						//
					}
					//
				}
				//
			}
			//
		}
		else {
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
						cidi = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
						cs = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
						//
						// 2016.10.26
						//
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) ) == 0 ) {
							//
							if ( !Scheduler_Process_Valid_And_Running( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) , TMATM ) ) return;
							//
						}
						//
					}
					else {
						cidi = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) );
						cs = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) );
						//
						// 2016.10.26
						//
						if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) ) == 0 ) {
							//
							if ( !Scheduler_Process_Valid_And_Running( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) , TMATM ) ) return;
							//
						}
						//
					}
				}
	//			else { // 2014.02.25
	//				return; // 2014.02.25
	//			} // 2014.02.25
				//
				j++;
			}
		}
		//
//		if ( cidi == -1 ) break; // 2014.02.25 // 2014.03.18
		if ( cidi == -1 ) continue; // 2014.02.25 // 2014.03.18
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		cido = -1;
		//
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) { // 2016.07.22
			//
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) { // 2017.01.11
						cido = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
					}
				}
				//
			}
			//
		}
		else {
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
						cido = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j ) , _SCH_MODULE_Get_BM_POINTER( i , j ) );
					}
					else {
						cido = _SCH_CLUSTER_Get_ClusterIndex( _SCH_MODULE_Get_BM_SIDE( i , j + 1 ) , _SCH_MODULE_Get_BM_POINTER( i , j + 1 ) );
					}
				}
	//			else { // 2014.02.25
	//				return; // 2014.02.25
	//			} // 2014.02.25
				//
				j++;
			}
		}
		//
//		if ( cido == -1 ) break; // 2014.02.25 // 2014.03.18
		if ( cido == -1 ) continue; // 2014.02.25 // 2014.03.18
		//
//		if ( cidi != cido ) { // 2014.02.25
			if ( bmc == -1 ) {
				bmc = i;
				bms = cs;
			}
			else {
				if ( bms < cs ) {
					bmc = i;
					bms = cs;
				}
			}
//		} // 2014.02.25
	}
	//
	if ( bmc != -1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , 1 , TRUE , TMATM+1 , 1903 );
	//
}



int SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( int s , int p , BOOL whenalldis , BOOL dataonly , int log ); // 2015.06.10

BOOL SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER2( int mode , int s1 , int p1 , int s2 , int p2 ) { // 2015.12.03
	int i , ch , fail;
	int pmc[MAX_PM_CHAMBER_DEPTH];
	//
	if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) > 1 ) return FALSE;
	//
	fail = FALSE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = 0;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		ch = _SCH_CLUSTER_Get_PathProcessChamber( s1 , p1 , 0 , i );
		//
		if ( ch > 0 ) {
			//
			if ( ch >= Get_RunPrm_FM_PM_START_MODULE() ) continue;
			//
			pmc[ch-PM1] = 1;
			//
		}
		//
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( pmc[i] || pmc[i+1] ) { // 2015.12.21
			//
			if      ( mode == 0 ) {
				//
				if      ( !pmc[i] && pmc[i+1] ) {
					fail = TRUE;
					break;
				}
				else if ( pmc[i] && !pmc[i+1] ) {
					fail = TRUE;
					break;
				}
				//
			}
			else if ( mode == 1 ) {
				if ( !pmc[i] ) {
					fail = TRUE;
					break;
				}
			}
			else if ( mode == 2 ) {
				if ( !pmc[i+1] ) {
					fail = TRUE;
					break;
				}
			}
		}
		//
		i++;
		//
	}
	//
	if ( fail ) {
		//
		if ( s1 >= 0 ) _SCH_CLUSTER_Check_and_Make_Return_Wafer( s1 , p1 , -1 );
		if ( s2 >= 0 ) _SCH_CLUSTER_Check_and_Make_Return_Wafer( s2 , p2 , -1 );
		//
		return TRUE;
		//
	}
	//
	return FALSE;
}

void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3_TM_DOUBLE( int TMATM , int side ) { // 2015.06.10
	int i , j , s1 , p1 , s2 , p2 , mode;
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( TMATM , i , G_PICK , G_MCHECK_SAME ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			//
			s1 = -1;
			s2 = -1;
			//
			if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) && ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
				if ( ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) && ( _SCH_MODULE_Get_BM_STATUS( i , j + 1 ) == BUFFER_INWAIT_STATUS ) ) {
					//
					s1 = _SCH_MODULE_Get_BM_SIDE( i , j );
					p1 = _SCH_MODULE_Get_BM_POINTER( i , j );
					//
					s2 = _SCH_MODULE_Get_BM_SIDE( i , j+1 );
					p2 = _SCH_MODULE_Get_BM_POINTER( i , j+1 );
					//
				}
			}
			else if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) {
					//
					s1 = _SCH_MODULE_Get_BM_SIDE( i , j );
					p1 = _SCH_MODULE_Get_BM_POINTER( i , j );
					//
				}
			}
			else if ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( i , j + 1 ) == BUFFER_INWAIT_STATUS ) {
					//
					s2 = _SCH_MODULE_Get_BM_SIDE( i , j+1 );
					p2 = _SCH_MODULE_Get_BM_POINTER( i , j+1 );
					//
				}
			}
			//
			if ( s1 >= 0 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s1 , p1 , TRUE , FALSE , 2 );
			if ( s2 >= 0 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s2 , p2 , TRUE , FALSE , 3 );
			//
			j++;
		}
		//
	}
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		s1 = -1;
		s2 = -1;
		//
		if ( ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) % 100 ) > 0 ) ) {
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
			p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
			//
			s2 = _SCH_MODULE_Get_TM_SIDE2( TMATM , i );
			p2 = _SCH_MODULE_Get_TM_POINTER2( TMATM , i );
			//
			mode = 0; // 2015.12.03
			//
		}
		else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) / 100 ) > 0 ) {
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
			p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
			//
			mode = 2; // 2015.12.03
			//
		}
		else if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) % 100 ) > 0 ) {
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
			p1 = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
			//
			mode = 1; // 2015.12.03
			//
		}
		else {
			continue; // 2015.12.03
		}
		//
		if ( !SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER2( mode , s1 , p1 , s2 , p2 ) ) { // 2015.12.03
			//
			if ( s1 >= 0 ) {
				if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) <= 1 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s1 , p1 , TRUE , FALSE , 4 ); // 2015.06.22
			}
			if ( s2 >= 0 ) {
				if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) <= 1 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s2 , p2 , TRUE , FALSE , 5 ); // 2015.06.22
			}
			//
		}
		else { // 2016.02.15
			//
			if ( s1 >= 0 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s1 , p1 , FALSE , TRUE , 6 );
			//
			if ( s2 >= 0 ) SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( s2 , p2 , FALSE , TRUE , 7 );
			//
		}
		//
	}
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4_TM_DOUBLE( int TMATM , int side ) { // 2016.10.31
	int i , j , sc , lc , bmfullTMwait , bmoutFMwait , hasin , hasout , hasfm;
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			return;
		}
	}
	//
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2017.01.23
	//
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) != 0 ) return;
	}
	else {
		return;
	}
	//
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) != 0 ) return;
	}
	else {
		return;
	}
	//
	bmfullTMwait = 0;
	bmoutFMwait = FALSE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		hasin = FALSE;
		hasout = FALSE;
		//
		if      ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) {
			hasfm = FALSE;
		}
		else if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
			hasfm = TRUE;
		}
		else {
			continue;
		}
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
		//
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
			//
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					hasin = TRUE;
				}
				//
			}
			//
		}
		else {
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
					hasin = TRUE;
				}
				//
				j++;
			}
		}
		//
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
		//
		if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
			//
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) { // 2017.01.11
						hasout = TRUE;
					}
				}
				//
			}
			//
		}
		else {
			for ( j = sc ; j <= lc ; j++ ) {
				//
				if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
					hasout = TRUE;
				}
				//
				j++;
			}
		}
		//
		if ( hasin && hasout ) {
			if ( hasfm ) {
				return;
			}
			else {
				bmfullTMwait = i;
			}
		}
		else if ( hasin ) {
			return;
		}
		else if ( hasout ) {
			if ( hasfm ) {
				bmoutFMwait = TRUE;
			}
			else {
				return;
			}
		}
		else {
			return;
		}
	}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4_TM_DOUBLE %d,%d\n" , TMATM + 1 , bmfullTMwait , bmoutFMwait );
	//----------------------------------------------------------------------
	//
	if ( ( bmfullTMwait > 0 ) && ( bmoutFMwait ) ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmfullTMwait , 1 , TRUE , TMATM+1 , 61031 );
	//
}


void SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE( int TMATM , int side ) { // 2016.12.13
	int i , j , sc , lc , NoMore , hasw , hasx , hasbm0 , hasbm1 , hasbm2 , hasbm3 , hasbm1nm , hasbm2nm , hasbm3nm , HasReturn;
	int Hastmwafer , HasOut , HasIn; // 2017.10.16
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) {
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_FULLSWAP ) { // 2018.10.20
				return;
			}
		}
	}
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_MIDDLESWAP ) return; // 2017.01.23 // 2017.01.26
	//
	// 2017.10.16 AKRA+SMART
	//
	Hastmwafer = FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) {
				Hastmwafer = TRUE;
				break;
			}
		}
	}
	//
//	if ( !SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE ) ) return; // 2017.01.18
	HasReturn = SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( TMATM , TRUE ); // 2017.01.18
	//
//	NoMore = SCHEDULER_FM_Another_No_More_Supply( -1 , -1 ); // 2017.02.10
	NoMore = SCHEDULER_FM_All_No_More_Supply(); // 2017.02.10
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE %d,%d\n" , TMATM + 1 , NoMore , HasReturn );
	//----------------------------------------------------------------------

	hasbm0 = 0;
	//
	hasbm1 = 0;
	hasbm2 = 0;
	hasbm3 = 0;
	//
	hasbm1nm = 0;
	hasbm2nm = 0;
	hasbm3nm = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_SIZE( i ) <= 1 ) continue; // 2017.01.11
		//
		//
		//
		// FMSIDE 가 하나라도 있거나 Runnign 중이면 취소 
		//
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.10.16 AKRA+SMART
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
//				if ( NoMore ) continue; // 2019.04.03 // 2019.05.30
				if ( NoMore != -1 ) continue; // 2019.04.03 // 2019.05.30
				return;
			}
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
			//
		}
		else {
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
//				if ( NoMore ) continue; // 2019.04.03 // 2019.05.30
				if ( NoMore != -1 ) continue; // 2019.04.03 // 2019.05.30
				return;
			}
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return;
			//
		}
		//
		HasOut = FALSE;
		HasIn = FALSE; // 2019.01.29
		//
		hasw = 0;
		hasx = 0;
		//
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) == BUFFER_SWITCH_FULLSWAP ) { // 2018.10.20
			//
			sc = 1;
			lc = _SCH_PRM_GET_MODULE_SIZE( i );
			//
			//
			if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
				//
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) {
							hasw++;
							HasIn = TRUE; // 2019.01.29
						}
						else if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) {
							hasw++;
							HasOut = TRUE;
						}
						else {
							hasx++;
						}
					}
					else {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue;
						//
						hasx++;
					}
					//
				}
				//
			}
			else {
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
						//
						hasw++;
						//
						if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) ) {
							HasOut = TRUE;
						}
						if ( ( _SCH_MODULE_Get_BM_WAFER( i , j+1 ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( i , j+1 ) == BUFFER_OUTWAIT_STATUS ) ) {
							HasOut = TRUE;
						}
						//
						if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) ) { // 2019.01.29
							HasIn = TRUE;
						}
						if ( ( _SCH_MODULE_Get_BM_WAFER( i , j+1 ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( i , j+1 ) == BUFFER_INWAIT_STATUS ) ) { // 2019.01.29
							HasIn = TRUE;
						}
						//
					}
					else {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j + 1 ) ) {
							//
							hasx++;
							//
						}
						//
					}
					//
					j++;
				}
			}
			//
		}
		else {
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
			//
			hasw = 0;
			hasx = 0;
			//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(A) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
				//
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_INWAIT_STATUS ) { // 2017.01.20
							hasw++;
							HasIn = TRUE; // 2019.01.29
						}
						else {
							hasx++;
						}
					}
					else {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
						//
						hasx++;
					}
					//
				}
				//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(B) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			}
			else {
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
						hasw++;
						HasIn = TRUE; // 2019.01.29
					}
					else {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j + 1 ) ) { // 2018.04.03
							//
							hasx++;
							//
						}
						//
					}
					//
					j++;
				}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(C) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			}
			//
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
			//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(D) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
				//
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) { // 2017.01.11
							hasw++;
							HasOut = TRUE;
						}
						else { // 2017.01.11
							hasx++;
						}
					}
					else {
						//
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue; // 2018.04.03
						//
						hasx++;
					}
					//
				}
				//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(E) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			}
			else {
				for ( j = sc ; j <= lc ; j++ ) {
					//
					if ( ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) || ( _SCH_MODULE_Get_BM_WAFER( i , j + 1 ) > 0 ) ) {
						hasw++;
						HasOut = TRUE;
					}
					else {
						//
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) && !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j + 1 ) ) { // 2018.04.03
							//
							hasx++;
							//
						}
						//
					}
					//
					j++;
				}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(F) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
		//----------------------------------------------------------------------
			}
		}
		//
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//============================================================================================================================================
		//
		// hasw		:	IN,OUT Wafer 수
		// hasx		:	빈 공간 수
		// hasbm0	:	[전체]	모두 비어있는 BM 수
		//
		if ( ( hasw > 0 ) && ( hasx <= 0 ) ) { // 모두 채워져 있을때
			//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(G) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
	//----------------------------------------------------------------------
			if ( HasReturn && HasOut ) { // 2017.01.18 // return Wafer가 있고 Out 될 Wafer가 있을때
				//
				/*
				// 2019.01.29
				//
				if      ( hasbm1 == 0 ) hasbm1 = i;
				else if ( hasbm2 == 0 ) hasbm2 = i;
				else if ( hasbm3 == 0 ) hasbm3 = i;
				//
				*/
				//
				// 2019.01.29
				//
				if ( !HasIn ) {
					if      ( hasbm1 == 0 ) hasbm1 = i;
					else if ( hasbm2 == 0 ) hasbm2 = i;
					else if ( hasbm3 == 0 ) hasbm3 = i;
				}
				else {
					//
					if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
						if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) ) {
							if      ( hasbm1 == 0 ) hasbm1 = i;
							else if ( hasbm2 == 0 ) hasbm2 = i;
							else if ( hasbm3 == 0 ) hasbm3 = i;
						}
					}
					else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) ) {
						if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TA_STATION ) > 0 ) {
							if      ( hasbm1 == 0 ) hasbm1 = i;
							else if ( hasbm2 == 0 ) hasbm2 = i;
							else if ( hasbm3 == 0 ) hasbm3 = i;
						}
					}
					else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) {
						if ( _SCH_MODULE_Get_TM_WAFER( TMATM,TB_STATION ) > 0 ) {
							if      ( hasbm1 == 0 ) hasbm1 = i;
							else if ( hasbm2 == 0 ) hasbm2 = i;
							else if ( hasbm3 == 0 ) hasbm3 = i;
						}
					}
					else {
					}
					//
				}
				//
			}
			//
		}
		else if ( ( hasw <= 0 ) && ( hasx > 0 ) ) { // 모두 비워져 있을때
			//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(H) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
	//----------------------------------------------------------------------
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2018.11.23
			//
			hasbm0++;
			//
//			if ( !NoMore ) { // 2017.01.18
			if ( !HasReturn && ( NoMore == -1 ) ) { // 2017.01.18 // return Wafer가 없고 공급준비가 가능한 상태
				//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(I) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
	//----------------------------------------------------------------------
				//
				if ( !Hastmwafer || ( hasbm1nm == 0 ) ) { // 2017.10.16
					//
					if      ( hasbm1nm == 0 ) hasbm1nm = i;
					else if ( hasbm2nm == 0 ) hasbm2nm = i;
					else if ( hasbm3nm == 0 ) hasbm3nm = i;
					//
				}
				//
			}
		}
		else {
			//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE(J) %d,%d,%d %d,%d\n" , TMATM + 1 , i , sc , lc , hasw , hasx );
	//----------------------------------------------------------------------
			hasbm0++;
			//
		}
		//
 	}
	//
	if ( hasbm0 <= 0 ) {
		if ( hasbm1 != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm1 , 1 , TRUE , TMATM+1 , 61213 );
		if ( hasbm2 != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm2 , 1 , TRUE , TMATM+1 , 61213 );
		if ( hasbm3 != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm3 , 1 , TRUE , TMATM+1 , 61213 );
	}
	//
	if ( hasbm1nm != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm1nm , 1 , TRUE , TMATM+1 , 61214 );
	if ( hasbm2nm != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm2nm , 1 , TRUE , TMATM+1 , 61214 );
	if ( hasbm3nm != 0 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , hasbm3nm , 1 , TRUE , TMATM+1 , 61214 );
}


//
BOOL SCHEDULING_CHECK_LOCKING_MAKE_FREE( int TMATM , int side ) { // 2008.04.18
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 2 ) == 1 ) { // 2016.01.04
		//
		SCHEDULING_CHECK_DISABLE_CLOSE_EVENT( TMATM , side ); // 2010.05.21
		//
	}
	//

	SCHEDULING_CHECK_DEFAULT_LOCK_MAKE_FREE_SUB0( TMATM , side ); // 2014.10.08

//	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB0( TMATM , side ); // 2008.07.04 // 2014.12.12

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d STEP SCHEDULING_CHECK_LOCKING_MAKE_FREE START\n" , TMATM + 1 );
	//----------------------------------------------------------------------
	if ( !SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB1( TMATM , side ) ) return FALSE;
	//
	SCHEDULING_CHECK_NONSWITCHING_LOCK_MAKE_FREE_SUB2( TMATM , side ); // 2012.08.10
	//
	if ( !SCHEDULING_CHECK_SWITCHING_EIL_LOCK_MAKE_FREE_SUB11( TMATM , side ) ) return FALSE;
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB0( TMATM , side ); // 2008.07.04
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1( TMATM , side );
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_2( TMATM , side ); // 2012.01.30
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_3( TMATM , side ); // 2018.12.06
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2( TMATM , side );
	if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3( TMATM , side ) ) return FALSE;
	//
	if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5( TMATM , side ) ) return FALSE;
	//
	if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB6( TMATM , side ) ) return FALSE; // 2008.07.01
	//

	//
//	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB7( TMATM , side ); // 2017.02.16 // pm diff arm interlock	유진텍 확인중
	//

//
//	if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_FTM_DOUBLE( TMATM , side ) ) return FALSE; // 2009.05.06
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_FTM_DOUBLE( TMATM , side ); // 2009.08.07
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3_FTM_DOUBLE( TMATM , side ); // 2011.09.16
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4_FTM_DOUBLE( TMATM , side ); // 2015.02.16
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_BATCH( TMATM , side ); // 2009.09.23
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_FIRSTGROUP( TMATM , side ); // 2013.03.19 // 2016.10.26
	//
	if ( _SCH_PRM_GET_MODULE_MODE( TM + TMATM + 1 ) ) { // 2016.02.18
//		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_FIRSTGROUP( TMATM , side ); // 2013.03.19 // 2016.10.26
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_NEXTGROUP( TMATM , side ); // 2013.03.19
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_FORCEVENT( TMATM , side ); // 2013.03.26
		//
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_SINGLEARM_LOOP_LOCK_FULLSWAP( TMATM , side ); // 2018.09.17
		//
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_SINGLEARM_TM1_NEXTGRP_LOCK_FULLSWAP( TMATM , side ); // 2018.09.17
		//
	}
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_ONEFMTMARM_SWITCH( TMATM , side ); // 2013.05.07
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_WAIT_BM_SWITCH( TMATM , side ); // 2014.01.03
	//
	SCHEDULING_CHECK_SWITCHING_MULTI_PM_SPAWN_PROCESS( TMATM , side ); // 2014.01.10
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_TM_DOUBLE( TMATM , side ); // 2014.01.25
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB3_TM_DOUBLE( TMATM , side ); // 2015.06.10
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4_TM_DOUBLE( TMATM , side ); // 2016.10.31
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5_TM_DOUBLE( TMATM , side ); // 2016.12.13
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_TM_SINGLE( TMATM , side ); // 2016.10.20 // TM Single All run , BM Not go Space
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB9_FULLSWAP( TMATM , side ); // 2018.10.31
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d STEP SCHEDULING_CHECK_LOCKING_MAKE_FREE END\n" , TMATM + 1 );
	//----------------------------------------------------------------------
	return TRUE;
}

BOOL SCHEDULING_CHECK_LOCKING_MAKE_FREE_TM2( int TMATM , int side ) { // 2008.06.20
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d STEP SCHEDULING_CHECK_LOCKING_MAKE_FREE START\n" , TMATM + 1 );
	//----------------------------------------------------------------------
	SCHEDULING_CHECK_DEFAULT_LOCK_MAKE_FREE_SUB0( TMATM , side ); // 2014.10.08

	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB0( TMATM , side ); // 2008.07.04
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB1_3( TMATM , side ); // 2013.04.11
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB4( TMATM , side );
	//
	if ( !SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB5( TMATM , side ) ) return FALSE; // 2008.07.08
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_DUALTM_for_OTHERGROUPGO( TMATM , side ); // 2013.03.26
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB2_DUALTM_for_SAMEGROUP( TMATM , side ); // 2013.03.19
	//
	if ( _SCH_PRM_GET_MODULE_MODE( TM + TMATM + 1 ) ) { // 2017.04.01
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_NEXTGROUP( TMATM , side );
		//
		SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB_DUALTM_for_SINGLEARM_LOOP_LOCK_FULLSWAP( TMATM , side ); // 2018.09.17
		//
	}
	//
	SCHEDULING_CHECK_SWITCHING_MULTI_PM_SPAWN_PROCESS( TMATM , side ); // 2014.01.10
	//
	SCHEDULING_CHECK_SWITCHING_LOCK_MAKE_FREE_SUB9_FULLSWAP( TMATM , side ); // 2018.10.31
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM%d STEP SCHEDULING_CHECK_LOCKING_MAKE_FREE END\n" , TMATM + 1 );
	//----------------------------------------------------------------------
	return TRUE;
}

void SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( int TMATM , int side , int orgch , int mode ) { // 2007.11.15
	int i , j , sd;
	//
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) return; // 2012.09.03
	//
	if ( mode == 2 ) { // 2008.07.28
		if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,TB_STATION ) ) return;
		//
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) >  0 ) ) return;
		if ( ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) <= 0 ) ) return;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TA_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_TM_SIDE( TMATM , TA_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TA_STATION ) ) != 1 ) return;
		}
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , TB_STATION ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_TM_SIDE( TMATM , TB_STATION ) , _SCH_MODULE_Get_TM_POINTER( TMATM , TB_STATION ) ) != 1 ) return;
		}
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( i == orgch ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue; // 2016.10.30
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) > 0 ) {
							if      ( mode == 1 ) {
//								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1502 ); // 2014.12.26
								SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( side , TMATM , i , 1502 ); // 2014.12.26
							}
							else if ( mode == 0 ) {
								sd = -1;
								for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
									if ( ( sd != -1 ) && ( _SCH_MODULE_Get_BM_SIDE( i , j ) != sd ) ) break;
									sd = _SCH_MODULE_Get_BM_SIDE( i , j );
									if ( !_SCH_MODULE_Chk_TM_Free_Status( sd ) ) break;
								}
								//
								if ( j > _SCH_PRM_GET_MODULE_SIZE( i ) ) {
//									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1503 );
									SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( side , TMATM , i , 1503 ); // 2014.12.26
								}
							}
							else if ( mode == 2 ) { // 2008.07.28
// 2008.07.30
//								if ( side != _SCH_MODULE_Get_BM_SIDE( i , j ) ) {
//									if ( _SCH_MODULE_Chk_TM_Free_Status( _SCH_MODULE_Get_BM_SIDE( i , j ) ) ) {
//										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1504 );
//									}
//								}
// 2008.07.30
								for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
									//
									if ( side != _SCH_MODULE_Get_BM_SIDE( i , j ) ) {
										if ( _SCH_MODULE_Chk_TM_Free_Status( _SCH_MODULE_Get_BM_SIDE( i , j ) ) ) {
//											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , 1 , TRUE , TMATM+1 , 1504 );
											SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( side , TMATM , i , 1504 ); // 2014.12.26
											break;
										}
									}
								}
// 2008.07.30
							}
						}
					}
				}
			}
		}
	}
}


BOOL SCHEDULING_CHECK_for_Global_Deadlock_Operation( int TMATM , int side ) { // 2007.11.03
	return TRUE;
}
//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================

int USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_0( int CHECKING_SIDE , int mode ) {
	int i , j , k , SubCheck;
	int l;

	if ( mode == 0 ) {
		//==============================================================================================================
		// 2005.10.06
		//==============================================================================================================
		//==============================================================================================================
		if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
			SCHEDULER_CONTROL_Reset_CASSETTE_DATA_A0SUB2( CHECKING_SIDE , -1 , TRUE );
		}
		//==============================================================================================================
		return -1;
	}
	else if ( mode == 2 ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) {
//				if ( Scheduler_Main_Waiting_for_Switch( CHECKING_SIDE ) ) { // 2008.09.30
					if ( ( Scheduler_Main_Waiting_for_Switch( CHECKING_SIDE ) ) && ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) ) { // 2008.09.30
						return SYS_RUNNING;
					}
				}
				else { // 2009.02.12
					if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) { // 2009.02.12
						return SYS_RUNNING;
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 1\n" );
		//----------------------------------------------------------------------
		//###################################################################################
		// Cassette Skip Check (2005.07.19)
		//###################################################################################
		if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( _SCH_CASSETTE_Chk_SKIP( CHECKING_SIDE ) != 0 ) {
			}
		}
		//###################################################################################
		//###################################################################################
		// Cassette Append Check (2005.10.07)
		//###################################################################################
		if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
			SCHEDULER_CONTROL_Chk_CASSETTE_APPEND_A0SUB2( CHECKING_SIDE );
			SCHEDULER_CONTROL_Chk_CASSETTE_DISABLE_A0SUB2( CHECKING_SIDE );
			//
			SCHEDULER_CONTROL_Chk_CASSETTE_REFRESH_A0SUB2( CHECKING_SIDE ); // 2007.11.21
		}
		//###################################################################################

		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 2\n" );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		//###################################################################################
		// Disapear Check (2001.11.28)
		//###################################################################################
		if ( _SCH_SUB_DISAPPEAR_OPERATION( 0 , 0 ) ) {
		}
		//###################################################################################

		SCHEDULING_CHECK_PARALLEL_CHECK_IN_TPM( 0 , CHECKING_SIDE ); // 2010.04.20

		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 3\n" );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		//###################################################################################
		// Force FM SIDE Check (2007.10.19)
		//###################################################################################
		if ( _SCH_SUB_Get_FORCE_FMSIDE_DATA( &k ) ) {
//printf( "[[0=%d]]" , k );
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP MF = (k=%d)\n" , k );
			//----------------------------------------------------------------------
//			_SCH_SYSTEM_ENTER_FEM_CRITICAL(); // 2007.10.30

			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( ( k != -1 ) && ( k != i ) ) continue;
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
				//
				// 2015.08.17
				//
				//if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL( i ) ) continue; // 2015.08.17
				//if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) <= 0 ) continue; // 2015.08.17
				//if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue; // 2015.08.17
				//
				//
				// 2015.08.20
				//
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue;
				}
				//
				_SCH_SUB_Make_FORCE_FMSIDE( 0 );
				//
//				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 1024 ); // 2014.12.26
				SCHEDULING_SPAWN_WAITING_BM_LOCK_FM_SIDE( CHECKING_SIDE , 0 , i , 1024 ); // 2014.12.26
				//
			}
//			_SCH_SYSTEM_LEAVE_FEM_CRITICAL(); // 2007.10.30
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 4\n" );
		//----------------------------------------------------------------------
		//###################################################################################
		//====================================================================================================
		// 2006.02.02
		//====================================================================================================
		if ( SCHEDULER_Get_FULLY_RETURN_MODE() == 1 ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				_SCH_SYSTEM_MODE_END_SET( i , 3 );
			}
			SCHEDULER_Set_FULLY_RETURN_MODE( 2 );
		}
		else if ( SCHEDULER_Get_FULLY_RETURN_MODE() == 2 ) {
			k = 0;
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( i , FALSE ) == SYS_RUNNING ) {
					k = 1;
					break;
				}
				else {
					if ( SCHEDULER_Get_FULLY_RETURN_CH( i ) == 1 ) {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
							k = 1;
							break;
						}
					}
					else {
//						if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
						if ( !SCH_PM_IS_EMPTY( i ) ) { // 2014.01.10
							k = 1;
							break;
						}
						else {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) {
								k = 1;
								break;
							}
						}
					}
				}
			}
			if ( k == 0 ) {
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if      ( _SCH_MODULE_Get_TM_WAFER( i , TA_STATION ) > 0 ) {
						k = 1;
						break;
					}
					else if ( _SCH_MODULE_Get_TM_WAFER( i , TB_STATION ) > 0 ) {
						k = 1;
						break;
					}
				}
			}
			if ( k == 0 ) {
				if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
					k = 1;
				}
				else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
					k = 1;
				}
				else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
					k = 1;
				}
			}
			if ( k == 0 ) {
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( i , FALSE ) == SYS_RUNNING ) {
						k = 1;
						break;
					}
				}
				if ( k == 0 ) {
					if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( FM , FALSE ) == SYS_RUNNING ) k = 1;
				}
			}
			if ( k == 0 ) {
				//=====================================================
				Sleep(1000);
				//=====================================================
				if ( k == 0 ) {
					if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
						k = 1;
					}
					else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
						k = 1;
					}
					else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
						k = 1;
					}
				}
				if ( k == 0 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( i , FALSE ) == SYS_RUNNING ) {
							k = 1;
							break;
						}
					}
					if ( k == 0 ) {
						if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( FM , FALSE ) == SYS_RUNNING ) k = 1;
					}
				}
			}
			if ( k == 0 ) {
				for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _SCH_SYSTEM_ENTER_TM_CRITICAL( i );
				//
				for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
					if ( SCHEDULER_Get_FULLY_RETURN_CH( i ) == 1 ) {
// 2014.01.10
//						if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) {
//							SCHEDULING_EQ_SET_FAIL_CONFIRMATION_STYLE_0( i , _SCH_MODULE_Get_PM_WAFER( i , 0 ) );
//						}
						//
						for ( l = 0 ; l < _SCH_PRM_GET_MODULE_SIZE( i ) ; l++ ) {
							if ( _SCH_MODULE_Get_PM_WAFER( i , l ) > 0 ) {
								SCHEDULING_EQ_SET_FAIL_CONFIRMATION_STYLE_0( i , _SCH_MODULE_Get_PM_WAFER( i , l ) );
							}
						}
						//
					}
				}
				for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
					if ( SCHEDULER_Get_FULLY_RETURN_CH( i ) == 1 ) {
// 2014.01.10
//						if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) {
						//
						for ( l = 0 ; l < _SCH_PRM_GET_MODULE_SIZE( i ) ; l++ ) {
							if ( _SCH_MODULE_Get_PM_WAFER( i , l ) > 0 ) {
								//
								switch( _SCH_EQ_INTERFACE_FUNCTION_STATUS( i , TRUE ) ) {
								case SYS_SUCCESS :
									SCHEDULER_Set_FULLY_RETURN_CH( i , 2 );
									break;
								case SYS_ABORTED :
//									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted with Confirmation at PM%d(%d)%cWHTMFAIL\n" , i - PM1 + 1 , _SCH_MODULE_Get_PM_WAFER( i , 0 ) , 9 ); // 2014.01.10
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted with Confirmation at PM%d:%d(%d)%cWHTMFAIL\n" , i - PM1 + 1 , l+1 , _SCH_MODULE_Get_PM_WAFER( i , l ) , 9 ); // 2014.01.10
									for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
									return SYS_ABORTED;
									break;
								case SYS_ERROR :
									// 2014.01.10
//									j = _SCH_MODULE_Get_PM_SIDE( i , 0 );
//									_SCH_MODULE_Set_PM_WAFER( i , 0 , 0 );
									//
									j = _SCH_MODULE_Get_PM_SIDE( i , l );
									_SCH_MODULE_Set_PM_WAFER( i , l , 0 );
									//
									if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) _SCH_MODULE_Inc_FM_InCount( j );
									_SCH_MODULE_Inc_TM_InCount( j );
									break;
								}
								//
							}
						}
					}
				}
				//
				for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 5\n" );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		// Begin
		// Modify 2001.12.28
		//if ( !Get_Prm_MODULE_BUFFER_SWITCH_MODE() || ( _SCH_SYSTEM_USING2_GET( CHECKING_SIDE ) >= 6 ) ) {
//		if ( !Get_Prm_MODULE_BUFFER_SWITCH_MODE() || ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 3 ) ) { // 2006.12.21
//		if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 3 ) ) { // 2006.12.21 // 2009.07.28
		if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_BATCH_ALL ) || ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) == BUFFER_SWITCH_BATCH_ALL ) && ( _SCH_MODULE_Get_FM_SwWait( CHECKING_SIDE ) >= 3 ) ) ) { // 2006.12.21 // 2009.07.28 // 2013.03.09
			SubCheck = 0;
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 5-1\n" );
			//----------------------------------------------------------------------
			if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) SubCheck = 1;
//			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
			if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
				if      ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) { // 2003.11.07
					SubCheck = 1;
				}
				else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_PART ) { // 2004.05.06
					SubCheck = 1;
				}
				else {
					SubCheck = 2;
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 5-2\n" );
			//----------------------------------------------------------------------
			if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
				if ( ( SubCheck != 0 ) && _SCH_SYSTEM_MODE_RESUME_GET( CHECKING_SIDE ) ) {
					if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) ||
						 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) ) {
						_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
						_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
						_SCH_SYSTEM_MODE_RESUME_SET( CHECKING_SIDE , FALSE );
					}
					else if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 5 ) ||
							  ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) ) {
						if ( SubCheck == 1 ) {
							if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
								_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop( CHECKING_SIDE , FALSE );
								_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
								_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
								_SCH_SYSTEM_MODE_RESUME_SET( CHECKING_SIDE , FALSE );
							}
						}
						else {
							if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
								_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop( CHECKING_SIDE , TRUE );
								_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
								_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
								_SCH_SYSTEM_MODE_RESUME_SET( CHECKING_SIDE , FALSE );
							}
						}
					}
					else {
						_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
						_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
						_SCH_SYSTEM_MODE_RESUME_SET( CHECKING_SIDE , FALSE );
					}
					_SCH_IO_SET_MAIN_BUTTON( CHECKING_SIDE , CTC_RUNNING );
				}
				else {
					if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) ||
						 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) ) {
						if ( !_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop( CHECKING_SIDE , ( SubCheck != 1 ) , -1 ) ) {
							if      ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) _SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 1 );
							else if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) _SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 2 );
						}
						else {
							if      ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) _SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 5 );
							else if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) _SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 6 );
						}
						_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
					}
					else if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 7 ) { // 2003.12.01
						_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop( CHECKING_SIDE , ( SubCheck != 1 ) , -1 );
						//=============================================================================
						// 2006.08.14
						//=============================================================================
						j = 0;
						if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
								if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) {
									j = 1;
									//
									_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
									_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 );
									_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
									//
									SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_RETURN_BATCH_ALL( CHECKING_SIDE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) );
								}
							}
						}
						//=============================================================================
						if ( j == 0 ) {
							_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 1 );
							_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
						}
						//=============================================================================
					}
					else {
						//=============================================================================
						// 2006.08.03
						//=============================================================================
						j = 0;
//						if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
						if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
							if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
								if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) {
									j = 1;
									//
									_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
									_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 );
									_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
									//
									SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_RETURN_BATCH_ALL( CHECKING_SIDE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) );
								}
								//
								if (
									( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) == BATCHALL_SWITCH_LL_2_LL ) ||
									( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) == BATCHALL_SWITCH_LL_2_LP )
									) { // 2010.05.12
									_SCH_MODULE_Set_TM_End_Status( CHECKING_SIDE );
								}
							}
							//
						}
						//=============================================================================
						if ( j == 0 ) {
							if ( SubCheck == 1 ) {
								j = 0;
//								if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
								if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
	// 2004.05.06
	//								if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH ) { // 2003.11.07
	//									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2004.03.11
	//										if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE ) ) {
	//											if ( _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() > _SCH_PRM_GET_MODULE_SIZE( i ) ) {
	//												j = 1;
	//												break;
	//											}
	//										}
	//									}
	//								}
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_PART ) { // 2004.05.06
										j = 1;
									}
								}
								if ( j == 1 ) {
									SubCheck = 2;
								}
								else {
									if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
										if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 1 ) ||
											 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) ||
											 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 5 ) ) {
											_SCH_MODULE_Set_TM_End_Status( CHECKING_SIDE );
											_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
											_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 );
											_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
											//=============================================================================================
											// 2007.03.27
											//=============================================================================================
											if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
												if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
//													if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 2 ) == 1 ) { // 2008.01.15
													if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) { // 2008.01.15
														_SCH_MODULE_Set_FM_End_Status( CHECKING_SIDE );
													}
												}
											}
											//=============================================================================================
										}
										else {
											//=============================================================================================
											// 2009.05.14
											//=============================================================================================
											if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ) {
												if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) { // 2009.05.14
													if ( !_SCH_SYSTEM_MODE_WAITR_GET( CHECKING_SIDE ) ) {
														_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , TRUE );
														_SCH_IO_SET_MAIN_BUTTON( CHECKING_SIDE , CTC_PAUSED );
													}
												}
											}
											else {
												if ( !_SCH_SYSTEM_MODE_WAITR_GET( CHECKING_SIDE ) ) {
													_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , TRUE );
													_SCH_IO_SET_MAIN_BUTTON( CHECKING_SIDE , CTC_PAUSED );
												}
											}
										}
									}
								}
							}
							if ( SubCheck == 2 ) {
								if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
									if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 1 ) ||
										 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) ||
										 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 5 ) ) {
										_SCH_MODULE_Set_FM_End_Status( CHECKING_SIDE );
										_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
										_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 );
										_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
									}
									else {
										if ( !_SCH_SYSTEM_MODE_WAITR_GET( CHECKING_SIDE ) ) {
											_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , TRUE );
											_SCH_IO_SET_MAIN_BUTTON( CHECKING_SIDE , CTC_PAUSED );
										}
									}
								}
							}
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 5-3\n" );
			//----------------------------------------------------------------------
			if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				//====================================================================================================
				if ( ( SIGNAL_MODE_APPEND_END_GET( CHECKING_SIDE ) != 0 ) || ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO2 ) ) { // 2005.10.25
				//====================================================================================================
					if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
						return SYS_SUCCESS;
					}
					else {
						if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
							_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , TRUE );
						}
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 6\n" );
		//----------------------------------------------------------------------
		//=====================================================================================
		// 2007.11.08
		//=====================================================================================
		_SCH_SUB_Chk_GLOBAL_STOP( -1 ); // 2007.11.08
		//=====================================================================================
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 7\n" );
		//----------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) < 0 ) {
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted 1 ....%cWHTMFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 7-1\n" );
			//----------------------------------------------------------------------
			if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) && ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
//					if ( !Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
					if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) { // 2006.12.21
						if ( SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , TRUE , FALSE , -1 ) ) { // 2003.05.27
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
//								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_PRM_GET_MODULE_xGROUP( i ) == 0 ) ) {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) {
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( i , CHECKING_SIDE , &j , &k , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 91 FIND_REMAIN_BM = Chamber(BM%d)\n" , i - BM1 + 1 );
												//----------------------------------------------------------------------
												//
												if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
													//
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 1 );
													//
												}
												if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
											}
										}
									}
									if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE( i , CHECKING_SIDE , FALSE ) > 0 ) {
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 92 FIND_REMAIN_BM 2 = Chamber(BM%d)\n" , i - BM1 + 1 );
												//----------------------------------------------------------------------
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 1 );
												//
												if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
											}
										}
									}
								}
							}
						}
					}
					else { // 2008.12.29
						if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( _SCH_MODULE_Get_BM_SIDE( i , 1 ) != CHECKING_SIDE ) continue;
								if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) continue;
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) continue;
								if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 91 FIND_REMAIN_BM 2 = Chamber(BM%d)\n" , i - BM1 + 1 );
								//----------------------------------------------------------------------
								if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 2 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 4 ) || ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) ) { // 2009.01.09
									for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
										if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
											if ( _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , j ) != SCHEDULER_READY ) {
												_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , j , 0 );
											}
										}
									}
								}
								//
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 3001 );
								}
								//
							}
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 7-2\n" );
			//----------------------------------------------------------------------
			if ( SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
//				_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , TRUE ); // 2008.05.23
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 93 CHECK_FINISH_1 = TRUE\n" );
				//----------------------------------------------------------------------
//				if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) { // 2003.11.06
				if ( _SCH_MODULE_Chk_TM_Free_Status2( CHECKING_SIDE , _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) ) ) { // 2003.11.06
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 93-2 CHECK_FINISH_1 = TRUE\n" );
					//----------------------------------------------------------------------
					if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
						if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) == 0 ) {
							//_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Finished ....%cWHTMSUCCESS\n" , 9 ); // 2002.07.10
							return SYS_SUCCESS;
						}
					}
//					if ( !Get_Prm_MODULE_BUFFER_SWITCH_MODE() && !_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) { // 2006.12.21
					if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) && ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) ) { // 2006.12.21
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 93-3 CHECK_FINISH_1 = TRUE\n" );
						//----------------------------------------------------------------------
//						if ( SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , TRUE , FALSE , -1 ) ) { // 2003.05.27 // 2007.08.16
						if ( SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , TRUE , FALSE , -1 ) || SCHEDULING_CHECK_TM_All_Free_Status( 0 ) ) { // 2003.05.27 // 2007.08.16 // 2012.08.30
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 93-4 CHECK_FINISH_1 = TRUE\n" );
							//----------------------------------------------------------------------
							if ( FM_Pick_Running_Blocking_style_0 == 0 ) { // 2006.04.28
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 )  && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( FM_Pick_Running_Blocking_style_0 != 0 ) break; // 2006.04.28
		//								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _SCH_PRM_GET_MODULE_xGROUP( i ) == 0 ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) {
												if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( i , CHECKING_SIDE , &j , &k , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( i ) ) ) > 0 ) {
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 94 FIND_REMAIN_BM 3= Chamber(BM%d)\n" , i - BM1 + 1 );
														//----------------------------------------------------------------------
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 2 );
														}
														//
														if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
													}
												}
											}
											if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) {
												//==========================================================================
												EnterCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
												//==========================================================================
												if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
													if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE( i , CHECKING_SIDE , FALSE ) > 0 ) {
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 95 FIND_REMAIN_BM 2 = Chamber(BM%d)\n" , i - BM1 + 1 );
														//----------------------------------------------------------------------
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 1 , 2 );
														//
														if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
													}
												}
												//==========================================================================
												LeaveCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
												//==========================================================================
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
//				if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) <= 0 ) { // 2002.11.21
				if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) < 0 ) { // 2002.11.21
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted 2 ....%cWHTMFAIL\n" , 9 );
					return SYS_ABORTED;
				}
			}
		}
		//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  ENTER2 9\n" );
		//----------------------------------------------------------------------
		//--------------------------------------------------
//		if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) && ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) ) { // 2007.04.02 // 2008.07.21
//		if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) && ( ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) || ( _SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() >= 10 ) ) ) { // 2007.04.02 // 2008.07.21 // 2008.07.29
//		if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) ) { // 2007.04.02 // 2008.07.21 // 2008.07.29 // 2018.11.29

//			SCHEDULING_CHECK_PRE_END_RUN( 0 , CHECKING_SIDE ); // 2006.07.20 // 2018.11.29

//		} // 2018.11.29
		//
		_SCH_SUB_CHECK_PRE_END( CHECKING_SIDE , 0 , -1 , FALSE ); // 2018.11.29
		//
		// End
		return 0;
	}
	return -1;
}
//
/*
//
// 2019.02.08
//
//int _SCHEDULER_PROCESSING_STATUS[MAX_CHAMBER];
BOOL SCHEDULER_PROCESSING_STATUS_DIFFERENT( BOOL read ) { // 2012.12.05
	int i;
	//
	if ( read ) {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			_SCHEDULER_PROCESSING_STATUS[i] = ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) ? TRUE : FALSE;
		}
	}
	else {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCHEDULER_PROCESSING_STATUS[i] != ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) ? TRUE : FALSE ) ) return TRUE;
		}
	}
	return FALSE;
}
//
*/
//
//
// 2019.02.08
//
int _SCHEDULER_PROCESSING_STATUS[MAX_TM_CHAMBER_DEPTH][MAX_CHAMBER];
BOOL SCHEDULER_PROCESSING_STATUS_DIFFERENT( int tms , BOOL read ) {
	int i;
	//
	if ( read ) {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			_SCHEDULER_PROCESSING_STATUS[tms][i] = ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) ? TRUE : FALSE;
		}
	}
	else {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCHEDULER_PROCESSING_STATUS[tms][i] != ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) ? TRUE : FALSE ) ) return TRUE;
		}
	}
	return FALSE;
}
//
//
//

int SCH_SWAP_SLOT1[MAX_CHAMBER];
int SCH_SWAP_SLOT2[MAX_CHAMBER];

void SCH_PICK_BM_SWAPPING( int ch , int l , int l2 ) {
	int s , p , w , t;
	//
	s = _SCH_MODULE_Get_BM_SIDE( ch , l2 );
	p = _SCH_MODULE_Get_BM_POINTER( ch , l2 );
	w = _SCH_MODULE_Get_BM_WAFER( ch , l2 );
	t = _SCH_MODULE_Get_BM_STATUS( ch , l2 );
	//
	_SCH_MODULE_Set_BM_SIDE( ch , l2 , _SCH_MODULE_Get_BM_SIDE( ch , l ) );
	_SCH_MODULE_Set_BM_POINTER( ch , l2 , _SCH_MODULE_Get_BM_POINTER( ch , l ) );
	_SCH_MODULE_Set_BM_WAFER_STATUS( ch , l2 , _SCH_MODULE_Get_BM_WAFER( ch , l ) , _SCH_MODULE_Get_BM_STATUS( ch , l ) );
	//
	_SCH_MODULE_Set_BM_SIDE( ch , l , s );
	_SCH_MODULE_Set_BM_POINTER( ch , l , p );
	_SCH_MODULE_Set_BM_WAFER_STATUS( ch , l , w , t );
	//
}

void SCH_PICK_BM_SWAPPING_SET( int tms , int ch , int *rs , int *rp , int *rslot , BOOL set ) {
	int l;
	//
	l = *rslot;
	//
	if ( set ) {
		if ( ( l % 2 ) == 0 ) {
			//
			SCH_SWAP_SLOT1[ch] = l - 1;
			SCH_SWAP_SLOT2[ch] = l;
			//
			SCH_PICK_BM_SWAPPING( ch , l - 1 , l );
			//
			if ( _SCH_MODULE_Get_BM_WAFER( ch , l - 1 ) > 0 ) {
				*rs = _SCH_MODULE_Get_BM_SIDE( ch , l - 1 );
				*rp = _SCH_MODULE_Get_BM_POINTER( ch , l - 1 );
				*rslot = l - 1;
			}
			else { // 2017.09.05
				*rs = _SCH_MODULE_Get_BM_SIDE( ch , l );
				*rp = _SCH_MODULE_Get_BM_POINTER( ch , l );
				*rslot = l;
			}
			//
		}
		else {
			//
			SCH_SWAP_SLOT1[ch] = l + 1;
			SCH_SWAP_SLOT2[ch] = l;
			//
			SCH_PICK_BM_SWAPPING( ch , l + 1 , l );
			//
			if ( _SCH_MODULE_Get_BM_WAFER( ch , l ) > 0 ) {
				*rs = _SCH_MODULE_Get_BM_SIDE( ch , l );
				*rp = _SCH_MODULE_Get_BM_POINTER( ch , l );
				*rslot = l;
			}
			else { // 2017.07.13
				*rs = _SCH_MODULE_Get_BM_SIDE( ch , l + 1 );
				*rp = _SCH_MODULE_Get_BM_POINTER( ch , l + 1 );
				*rslot = l + 1;
			}
			//
		}
	}
	else {
		//
		SCH_PICK_BM_SWAPPING( ch , SCH_SWAP_SLOT1[ch] , SCH_SWAP_SLOT2[ch] );
		//
	}
}

int SCHEDULER_RUN_PLACE_TO_BM_POST_ACTION( int tms , int s1 , int p1 , int w1 , int l1 , int s2 , int p2 , int w2 , int l2 , int PlaceCh , int PlaceArm ,
										  int JumpMode , int PathThruModeS , int PathThruModeD ); // 2013.10.31


//

BOOL SCHEDULER_More_Supply_or_BM_One_is_not_FREE_FMSIDE( int TMATM , int side , int bmc ) { // 2017.10.16 AKRA+SMART
	int i;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( TMATM ) != BUFFER_SWITCH_SINGLESWAP ) return TRUE;
	//
	if ( !SCHEDULER_FM_Another_No_More_Supply( -1 , -2 ) ) return TRUE;
	//
	// 2017.10.19
	//
	if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER( 2 ) > 0 ) ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 )  return TRUE;
	if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() )  return TRUE;
	//

	// 2017.10.19 for TEST
	return FALSE;




	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( i == bmc ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) return FALSE;
		//
	}
	//
	return TRUE;
	//
}


int SCHEDULER_RUN_PICK_FROM_BM_SUB( int tms , int CHECKING_SIDE , int SCH_SchPt , int SCH_Chamber , int SCH_Slot , int SCH_JumpMode , int SCH_Order , int SCH_No /*nextpm*/ , int *skip_b1 , int *skip_b2 , BOOL *Run_Doing ) { // 2013.02.07
	int Checking_Side_Changed;
	int cf;
	char StringBuffer[256];

	int k , j , mb , pt;
	int BM_Side2 , pt2 , SCH_Slot2;
	int wsa , wsb;

	int Sav_Int_1 , Sav_Int_2 , Sav_Int_3 , Sav_Int_4 , Sav_Int_5 , Sav_Int_6 , Sav_Int_7 , Sav_Int_8;

	int Result , swmode , Function_Result;
	//
	BOOL PathThruModeS , PathThruModeD;

	//=============================================================================================
	if ( tms == 0 ) {
		PathThruModeS = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 );
		PathThruModeD = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
	}
	else {
		PathThruModeS = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
		PathThruModeD = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
	}
	//=============================================================================================
	Checking_Side_Changed = -1;
	//=============================================================================================
	// Pick from BM
	//========================================================================================================================================================
	if ( SCH_Chamber >= BM1 ) {
		//==============================================================================================================
		// 2007.01.02
		//==============================================================================================================
		if ( _SCH_PRM_GET_Getting_Priority( SCH_Chamber ) < 0 ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) > 0 ) return 0;
		}
		//==============================================================================================================
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < 0 ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 );
			return -1;
		}
		//===========================================================================================
		if ( Scheduler_Curr_Side_PM_Aborted( CHECKING_SIDE ) ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because PM Abort%cWHPMFAIL\n" , tms + 1 , 9 );
			return -1;
		}
		//===========================================================================================
	}
	//===========================================================================================================================================
	if ( ( !PathThruModeS ) && ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) ) { // 2013.03.09
		_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 );
		KPLT0_RESET_CASSETTE_TIME_SUPPLY();
	}
	//===========================================================================================================================================
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 18 RUN_TYPE_11(PICKfromBM)[%d,%d]\n" , tms + 1 , SCH_JumpMode , SCH_No );
	//----------------------------------------------------------------------
	//============================================================================
	// Pick from BM
	//============================================================================
	cf = -1;
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) { // 2015.08.27
		if ( ( SCH_JumpMode == 0 ) && ( SCH_No > 0 ) ) {
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms,SCH_No ) ) {
			case 0 :
				//===============================================================================
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
				case 0 :
					cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_Chamber );
					break;
				case 1 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
					break;
				case 2 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
					break;
				}
				break;
			case 1 :
				//===============================================================================
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
				case 0 :
				case 1 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
					break;
				case 2 :
					cf = -1;
					break;
				}
				//===============================================================================
				break;
			case 2 :
				//===============================================================================
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
				case 0 :
				case 2 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
					break;
				case 1 :
					cf = -1;
					break;
				}
				//===============================================================================
				break;
			}
		}
		else {
			/*
			// 2013.08.01
			//===============================================================================
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
			case 0 :
				cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_Chamber );
				break;
			case 1 :
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
				break;
			case 2 :
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
				break;
			}
			//===============================================================================
			*/
			//
			if ( SCH_JumpMode != 0 ) {
				//
				cf = SCHEDULER_CONTROL_DUALTM_for_GET_PROPERLY_ARM_WHEN_PICK_FROM_BM_POSSIBLE( tms , CHECKING_SIDE , SCH_SchPt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , SCH_SchPt ) - 1 , SCH_Chamber );
				//
			}
			else {
				//
				cf = _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber );
				//
			}
			//
			switch( cf ) {
			case 0 :
				cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_Chamber );
				break;
			case 1 :
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
				break;
			case 2 :
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
				break;
			default :
				cf = -1;
				break;
			}
			//
			//===============================================================================
		}
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) ) { // 2015.08.27
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) { // 2015.08.27
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
	}
	else { // 2015.08.27
		cf = -1;
	}
	//
	if ( cf == -1 ) return 0;
	//
	//-----------------------------------------------------------------------
	_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
	SCHEDULER_Set_TM_LastMove_Signal( tms , -1 );
	//-----------------------------------------------------------------------
	pt = SCH_SchPt;
	//-----------------------------------------------------------------------
	BM_Side2 = CHECKING_SIDE;
	pt2 = pt; // 2014.07.11
	//-----------------------------------------------------------------------
	if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2013.03.09
		//
		if ( ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( tms != _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) ) ) { // 2013.05.06
			SCH_PICK_BM_SWAPPING_SET( tms , SCH_Chamber , &CHECKING_SIDE , &pt , &SCH_Slot , TRUE );
		}
		//
		if ( SCH_JumpMode == 0 ) {
			SCHEDULING_CHECK_Chk_BM_HAS_IN_ANOTHER_WAFER_for_DOUBLE( SCH_Chamber , CHECKING_SIDE , SCH_Slot , &BM_Side2 , &pt2 , &SCH_Slot2 , !PathThruModeS , ( PathThruModeS ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) != BUFFER_SWITCH_BATCH_ALL ) , ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) , FALSE ); // 2013.03.09
		}
		else {
			SCHEDULING_CHECK_Chk_BM_HAS_IN_ANOTHER_WAFER_for_DOUBLE( SCH_Chamber , CHECKING_SIDE , SCH_Slot , &BM_Side2 , &pt2 , &SCH_Slot2 , !PathThruModeD , ( PathThruModeD ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) != BUFFER_SWITCH_BATCH_ALL ) , ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) , TRUE ); // 2013.03.09
		}
		//

	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 1 %d %d %d,%d,%d  %d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 , SCH_JumpMode );
	//----------------------------------------------------------------------

		if ( ( SCH_Slot == 2 ) && ( ( ( SCH_JumpMode == 0 ) && ( PathThruModeS ) ) || ( ( SCH_JumpMode != 0 ) && ( PathThruModeD ) ) ) ) {
			//
			if ( SCH_Slot2 > 0 ) {
				//=============================================================================================
				Checking_Side_Changed = CHECKING_SIDE;
				//=============================================================================================
				k = CHECKING_SIDE;
				CHECKING_SIDE = BM_Side2;
				BM_Side2 = k;
				//
				SCH_Slot  = 1;
				SCH_Slot2 = SCH_Slot + 1;
				//
				k = pt2;
				pt2 = pt;
				pt = k;
				//=============================================================================================
//				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt) + _SCH_CLUSTER_Get_SlotIn(BM_Side2,pt2)*100; // 2013.05.06
//				wsb = SCH_Slot + (SCH_Slot2*100); // 2013.05.06
				//=============================================================================================
				if ( tms == 0 ) { // 2013.02.07
					//=============================================================================================
					if ( SCH_JumpMode == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
						if ( _SCH_SUB_Chk_Last_Out_Status( BM_Side2 , pt2 ) ) {
							_SCH_SUB_Set_Last_Status( BM_Side2 , 1 );
						}
					}
					//=============================================================================================
					if ( SCH_JumpMode == 0 ) {
						//-----------------------------------------------------------------------
						Sav_Int_3 = _SCH_CLUSTER_Get_PathStatus( BM_Side2 , pt2 );
						Sav_Int_4 = _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 );
						Sav_Int_5 = _SCH_MODULE_Get_TM_DoPointer( BM_Side2 );
						//-----------------------------------------------------------------------
						//----------------------------------------------------------------------------------
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_STARTING );
						_SCH_CLUSTER_Inc_PathDo( BM_Side2 , pt2 );
						//----------------------------------------------------------------------------------
						_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , pt2 );
						//----------------------------------------------------------------------------------
					}
				}
				//=============================================================================================
			}
			else {
				SCH_Slot2 = SCH_Slot;
				SCH_Slot = 0;
//				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt)*100; // 2013.05.06
//				wsb = SCH_Slot2 * 100; // 2013.05.06
				//
				if ( tms == 0 ) { // 2013.02.07
					if ( SCH_JumpMode == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
					}
				}
				//
			}
		}
		else {
			//-----------------------------------------------------------------------
			k = 0;
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) {
				//==================================================================
				//
				if ( ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() < 1 ) || ( tms == 0 ) ) { // 2017.09.05
					//
					if      ( ( _SCH_CLUSTER_Get_PathHSide( CHECKING_SIDE , pt ) == HANDLING_A_SIDE_ONLY ) && ( SCH_Slot2 > 0 ) ) {
						SCH_Slot2 = 0;
					}
					else if ( _SCH_CLUSTER_Get_PathHSide( CHECKING_SIDE , pt ) == HANDLING_B_SIDE_ONLY ) {
						SCH_Slot2 = SCH_Slot;
						SCH_Slot = 0;
					}
//					else if ( ( _SCH_CLUSTER_Get_PathHSide( CHECKING_SIDE , pt ) == HANDLING_END_SIDE ) && ( SCH_Slot2 > 0 ) ) { // 2018.11.22
//						SCH_Slot2 = 0; // 2018.11.22
//					} // 2018.11.22
					//
				}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 11 %d %d %d,%d,%d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 );
	//----------------------------------------------------------------------
				//===========================================================================================================
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 <= 0 ) ) {
					if ( ( PathThruModeS ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) != BUFFER_SWITCH_BATCH_ALL ) ) { // 2013.03.09
						if ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 0 ) {
							if ( ( SCH_Slot % 2 ) == 0 ) {
								if ( _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot - 1 ) > 0 ) {
									if ( _SCH_MODULE_Get_BM_STATUS( SCH_Chamber , SCH_Slot - 1 ) != BUFFER_OUTWAIT_STATUS ) {
										//=============================================================================================
										Checking_Side_Changed = CHECKING_SIDE;
										//=============================================================================================
										k = 1;
										//
										k         = SCH_Slot - 1;
										SCH_Slot2 = SCH_Slot;
										BM_Side2  = CHECKING_SIDE;
										pt2       = pt;
										//
										SCH_Slot      = k;
										CHECKING_SIDE = _SCH_MODULE_Get_BM_SIDE( SCH_Chamber , k );
										pt            = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , k );
										//
										k = 1;
									}
									else {
										SCH_Slot2 = SCH_Slot;
										SCH_Slot = 0;
									}
								}
								else {
									SCH_Slot2 = SCH_Slot;
									SCH_Slot = 0;
								}
							}
						}
					}
				}
				//===========================================================================================================
				else if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( ( PathThruModeS ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) != BUFFER_SWITCH_BATCH_ALL ) ) { // 2009.03.06 // 2013.03.09
						if ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 0 ) {
//							if ( ( SCH_Slot + 1 ) != SCH_Slot2 ) SCH_Slot2 = 0; // 2017.07.13
							if ( ( SCH_Slot + 1 ) != SCH_Slot2 ) { // 2017.07.13
								if ( ( SCH_Slot2 + 1 ) != SCH_Slot ) { // 2017.07.13
									SCH_Slot2 = 0;
								}
							}
						}
					}
				}
				//===========================================================================================================
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 12 %d %d %d,%d,%d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 );
	//----------------------------------------------------------------------
			}
			else {
				//==================================================================
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 <= 0 ) ) {
					if ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING(tms) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2013.03.09
						if ( ( SCH_Slot > ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) ) {
							if ( _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) > 0 ) {
								if ( _SCH_MODULE_Get_BM_STATUS( SCH_Chamber , SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) != BUFFER_OUTWAIT_STATUS ) {
									//=============================================================================================
									Checking_Side_Changed = CHECKING_SIDE;
									//=============================================================================================
									k = 1;
									//
									k         = SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 );
									SCH_Slot2 = SCH_Slot;
									BM_Side2  = CHECKING_SIDE;
									pt2       = pt;
									//
									SCH_Slot      = k;
									CHECKING_SIDE = _SCH_MODULE_Get_BM_SIDE( SCH_Chamber , k );
									pt            = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , k );
									//
									k = 1;
								}
								else {
									SCH_Slot2 = SCH_Slot;
									SCH_Slot = 0;
								}
							}
							else {
								SCH_Slot2 = SCH_Slot;
								SCH_Slot = 0;
							}
						}
					}
					else {
						if ( ( SCH_Slot % 2 ) == 0 ) {
							if ( _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot - 1 ) > 0 ) {
//								if ( _SCH_MODULE_Get_BM_STATUS( SCH_Chamber , SCH_Slot - 1 ) != BUFFER_OUTWAIT_STATUS ) { // 2013.04.04
									//=============================================================================================
									Checking_Side_Changed = CHECKING_SIDE;
									//=============================================================================================
									k = 1;
									//
									k         = SCH_Slot - 1;
									SCH_Slot2 = SCH_Slot;
									BM_Side2  = CHECKING_SIDE;
									pt2       = pt;
									//
									SCH_Slot      = k;
									CHECKING_SIDE = _SCH_MODULE_Get_BM_SIDE( SCH_Chamber , k );
									pt            = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , k );
									//
									k = 1;
//								} // 2013.04.04
//								else { // 2013.04.04
//									SCH_Slot2 = SCH_Slot; // 2013.04.04
//									SCH_Slot = 0; // 2013.04.04
//								} // 2013.04.04
							}
							else {
								SCH_Slot2 = SCH_Slot;
								SCH_Slot = 0;
							}
						}
					}
				}
				//==================================================================
			}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 21 %d %d %d,%d,%d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 );
	//----------------------------------------------------------------------
			//==================================================================
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
				if ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 1 ) {
//					if ( ( SCH_Slot + 1 ) != SCH_Slot2 ) SCH_Slot2 = 0; // 2017.07.13
					if ( ( SCH_Slot + 1 ) != SCH_Slot2 ) { // 2017.07.13
						if ( ( SCH_Slot2 + 1 ) != SCH_Slot ) { // 2017.07.13
							SCH_Slot2 = 0;
						}
					}
				}
			}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 22 %d %d %d,%d,%d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 );
	//----------------------------------------------------------------------
			//==================================================================
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
				//
				if ( tms == 0 ) { // 2013.02.07
					if ( SCH_JumpMode == 0 ) {
						//-----------------------------------------------------------------------
						Sav_Int_3 = _SCH_CLUSTER_Get_PathStatus( BM_Side2 , pt2 );
						Sav_Int_4 = _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 );
						Sav_Int_5 = _SCH_MODULE_Get_TM_DoPointer( BM_Side2 );
						//-----------------------------------------------------------------------
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_STARTING );
						_SCH_CLUSTER_Inc_PathDo( BM_Side2 , pt2 );
						//-----------------------------------------------------------------------
						_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , pt2 );
					}
				}
				//
//				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt) + _SCH_CLUSTER_Get_SlotIn(BM_Side2,pt2)*100; // 2013.05.06
				//
				if ( tms == 0 ) { // 2013.02.07
					if ( SCH_JumpMode == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
						if ( _SCH_SUB_Chk_Last_Out_Status( BM_Side2 , pt2 ) ) {
							_SCH_SUB_Set_Last_Status( BM_Side2 , 1 );
						}
					}
				}
			}
			else if ( SCH_Slot > 0 ) {
				pt2 = 0;
//				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt); // 2013.05.06
				//
				if ( tms == 0 ) { // 2013.02.07
					if ( SCH_JumpMode == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
					}
				}
			}
			else {
				pt2 = 0;
//				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt) * 100; // 2013.05.06
				//
				if ( tms == 0 ) { // 2013.02.07
					if ( SCH_JumpMode == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
					}
				}
			}
//			wsb = SCH_Slot + (SCH_Slot2*100); // 2013.05.06
		}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 23 %d %d %d,%d,%d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 );
	//----------------------------------------------------------------------
		//===========================================================================================================
		// 2013.05.06
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
			wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt) + _SCH_CLUSTER_Get_SlotIn(BM_Side2,pt2)*100;
		}
		else if ( SCH_Slot > 0 ) {
			wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt);
		}
		else {
			wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt) * 100;
		}
		wsb = SCH_Slot + (SCH_Slot2*100);
		//===========================================================================================================
		if ( tms == 0 ) { // 2013.02.07
			if ( SCH_JumpMode == 0 ) {
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
					_SCH_MODULE_Inc_TM_DoubleCount( CHECKING_SIDE );
			}
		}
		//===========================================================================================================
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 24 %d %d %d,%d,%d  %d %d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 , wsa , wsb );
	//----------------------------------------------------------------------
	}
	else {
		pt2 = 0;
		SCH_Slot2 = 0;
		wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt);
		wsb = SCH_Slot;
		//
		if ( tms == 0 ) { // 2013.02.07
			if ( SCH_JumpMode == 0 ) {
				if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
				}
			}
		}
	}
	//
	//-----------------------------------------------------------------------
	// User Interface
	// 2016.04.19
	//-----------------------------------------------------------------------
	switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 102 , CHECKING_SIDE , pt , SCH_Chamber , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) , -1 , tms , cf ) ) {
	case 0 :
		//===============================================================================================================================
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s USER_FLOW_NOTIFICATION 102 Fail%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 );
		//===============================================================================================================================
		return -1;
		break;
	case 1 :
		//=================================================================================================
		if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
		//=================================================================================================
		if ( *skip_b1 == 0 ) { *skip_b1 = SCH_Chamber; return 2; } // 2016.05.12
		if ( *skip_b2 == 0 ) { *skip_b2 = SCH_Chamber; return 2; } // 2016.05.12
		return 121;
		break;
	}
	//
	//
	//==========================================================================================================
	if ( tms == 0 ) { // 2013.02.07
		if ( SCH_JumpMode == 0 ) {
			//-----------------------------------------------------------------------
			Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
			Sav_Int_2 = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt );
			//-----------------------------------------------------------------------
			_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_STARTING );
			_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
			//-----------------------------------------------------------------------
		}
		//-----------------------------------------------------------------------
		Sav_Int_6 = _SCH_SUB_Get_Last_Status( CHECKING_SIDE );
		Sav_Int_7 = _SCH_MODULE_Get_BM_MidCount();
		Sav_Int_8 = _SCH_MODULE_Get_TM_DoubleCount( CHECKING_SIDE );
	}
	//==========================================================================================================
	if ( SCH_Chamber >= BM1 ) {
		//-------------------------------------------------------------------------------
		if ( SCH_JumpMode == 1 ) {
			if ( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 0 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) ) {
				_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( CHECKING_SIDE , pt , 0 );
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2013.02.07
					_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( BM_Side2 , pt2 , 0 );
				}
			}
		}
		//-------------------------------------------------------------------------------
		if ( SCH_JumpMode != 1 ) {
			if ( PathThruModeS ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
					if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
						if ( _SCH_MODULE_Get_BM_MidCount() != 2 ) {
							if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , FALSE , 1 , 4  ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Fail(GoTMSide) When Picking from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL BM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_Chamber - BM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
								return -1;
							}
							_SCH_MODULE_Set_BM_MidCount( 2 );
						}
					}
				}
			}
			else {
				if ( tms == 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
						//=============================================================================================
						SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( CHECKING_SIDE , _SCH_CLUSTER_Get_ClusterIndex( CHECKING_SIDE , pt ) );
						//=============================================================================================
						switch( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) ) {
						case BATCHALL_SWITCH_LL_2_LL :
						case BATCHALL_SWITCH_LL_2_LP :
							//
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , pt , 0 , 0 , 0 );
							//
							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_START , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , pt ) , pt , "" , "" );
							//
							if ( _SCH_SUB_Chk_First_Out_Status( CHECKING_SIDE , pt ) ) {
								_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_FIRST , pt , -1 , -1 , -1 , -1 , "" , "" );
							}
							//
							if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
								//
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( BM_Side2 , pt2 , 0 , 0 , 0 );
								//
								_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_START , _SCH_CLUSTER_Get_SlotIn( BM_Side2 , pt2 ) , _SCH_CLUSTER_Get_PathIn( BM_Side2 , pt2 ) , _SCH_CLUSTER_Get_PathOut( BM_Side2 , pt2 ) , _SCH_CLUSTER_Get_SlotOut( BM_Side2 , pt2 ) , pt2 , "" , "" ); // 2010.03.10 // 2012.02.18
								//
								if ( _SCH_SUB_Chk_First_Out_Status( BM_Side2 , pt2 ) ) {
									_SCH_LOG_TIMER_PRINTF( BM_Side2 , TIMER_FIRST , pt2 , -1 , -1 , -1 , -1 , "" , "" );
								}
								//
							}
							//=================================================================================================
							break;
						default :
							if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , pt , 30 , -1 , -1 );
							}
							else {
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , pt , 10 , 2 , 1 );
							}
							//=================================================================================================
							if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
								if ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 1 ) {
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( BM_Side2 , pt2 , 30 , -1 , -1 );
								}
								else {
									_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( BM_Side2 , pt2 , 10 , 2 , 1 );
								}
							}
							//=================================================================================================
							break;
						}
					}
				}
			}
		}
		else {
			if ( PathThruModeD ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
					if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
						if ( _SCH_MODULE_Get_BM_MidCount() != 2 ) {
							//
							if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
								if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , FALSE , 1 , 3 ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Fail(GoFMSide) When Picking from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL BM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_Chamber - BM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
									return -1;
								}
							}
							//
							_SCH_MODULE_Set_BM_MidCount( 2 );
						}
					}
				}
			}
		}
	}
	//========================================================================================================================
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP PICK_FROM_BM 51 %d %d %d,%d,%d  %d %d\n" , tms+1 , SCH_Slot , pt , SCH_Slot2 , BM_Side2 , pt2 , wsa , wsb );
	//----------------------------------------------------------------------
	//========================================================================================================================
	//========================================================================================================================
	//========================================================================================================================
	if ( SCH_Chamber >= BM1 ) {
		//========================================================================================================================
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
			SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( tms , CHECKING_SIDE , SCH_Chamber , 0 );
		}
		//========================================================================================================================
		Result = SCHEDULING_CHECK_Switch_PLACE_From_TM( tms , CHECKING_SIDE , cf , SCH_Chamber );
		//-------------------------
		if ( Result == 0 ) {
			swmode = 0;
			if ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) {
				if ( ( SCHEDULER_Get_TM_Pick_BM_Signal( tms ) % 1000 ) == SCH_Chamber ) {
					swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( 1 , 1 );
				}
			}
		}
		else {
//			swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); // 2013.10.31
			swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 3 ); // 2013.10.31
		}
		//
		if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) { // 2013.10.31
			if ( swmode == 10 ) swmode = 1;
		}
		//-------------------------
		if ( swmode == 10 ) { // 2013.10.31
			Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PICKPLACE , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , BM_Side2 , pt2 );
		}
		else {
			Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , BM_Side2 , pt2 );
		}
		//-----------------------------------------------------------------------
		MODULE_LAST_SWITCH_0[SCH_Chamber] = Result;
		//-----------------------------------------------------------------------
		if ( Function_Result == SYS_ABORTED ) {
			return -1;
		}
		//=============================================================================================================
		else if ( Function_Result == SYS_ERROR ) {
			if ( tms == 0 ) { // 2013.02.07
				if ( SCH_JumpMode == 0 ) {
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
					_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , Sav_Int_2 );
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_3 );
						_SCH_CLUSTER_Set_PathDo( BM_Side2 , pt2 , Sav_Int_4 );
						_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , Sav_Int_5 );
					}
				}
				_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int_6 );
				_SCH_MODULE_Set_BM_MidCount( Sav_Int_7 );
				_SCH_MODULE_Set_TM_DoubleCount( CHECKING_SIDE , Sav_Int_8 );
				//
			}
			//=================================================================================================
			if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
			//=================================================================================================
			//
			if ( ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( tms != _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) ) ) { // 2013.05.06
				SCH_PICK_BM_SWAPPING_SET( tms , SCH_Chamber , &CHECKING_SIDE , &pt , &SCH_Slot , FALSE );
			}
			//
			return 0;
		}
		//=============================================================================================================
		if ( Result ) _SCH_MODULE_Set_TM_Switch_Signal( tms , SCH_Chamber );
		//-----------------------------------------------------------------------
		_SCH_MODULE_Set_TM_PATHORDER( tms , cf , SCH_Order );
		_SCH_MODULE_Set_TM_TYPE( tms , cf , SCHEDULER_MOVING_IN );
		_SCH_MODULE_Set_TM_OUTCH( tms , cf , SCH_Chamber );

		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
			_SCH_MODULE_Set_TM_SIDE_POINTER( tms , cf , CHECKING_SIDE , pt , BM_Side2 , pt2 );
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ) + _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot2 )*100 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , 0 , -1 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot2 , 0 , -1 );
		}
		else if ( SCH_Slot > 0 ) {
			_SCH_MODULE_Set_TM_SIDE_POINTER( tms , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ) );
			_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , 0 , -1 );
		}
		else if ( SCH_Slot2 > 0 ) {
			_SCH_MODULE_Set_TM_SIDE_POINTER( tms , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot2 ) * 100 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot2 , 0 , -1 );
		}
	}
	else {
		//-----------------------------------------------------------------------
		swmode = 0; // 2013.10.31
		//-----------------------------------------------------------------------
		_SCH_IO_SET_MODULE_SOURCE( SCH_Chamber , 1 , MAX_CASSETTE_SIDE );
		_SCH_IO_SET_MODULE_RESULT( SCH_Chamber , 1 , 0 );
		_SCH_IO_SET_MODULE_STATUS( SCH_Chamber , 1 , CHECKING_SIDE + 1 );
		//-----------------------------------------------------------------------
		Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , SCHEDULING_CHECK_Switch_PLACE_From_TM( tms , CHECKING_SIDE , cf , SCH_Chamber ) , -1 , FALSE , 0 , -1 , -1 );
		if ( Function_Result == SYS_ABORTED ) {
			return -1;
		}
		//=============================================================================================================
		else if ( Function_Result == SYS_ERROR ) {
			if ( tms == 0 ) { // 2013.02.07
				if ( SCH_JumpMode == 0 ) {
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
					_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , Sav_Int_2 );
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_3 );
						_SCH_CLUSTER_Set_PathDo( BM_Side2 , pt2 , Sav_Int_4 );
						_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , Sav_Int_5 );
					}
				}
				_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int_6 );
				_SCH_MODULE_Set_BM_MidCount( Sav_Int_7 );
				_SCH_MODULE_Set_TM_DoubleCount( CHECKING_SIDE , Sav_Int_8 );
			}
			//=================================================================================================
			if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
			//=================================================================================================
			return 0;
		}
		//=============================================================================================================
		_SCH_MODULE_Set_TM_OUTCH( tms , cf , SCH_Chamber );
		_SCH_MODULE_Set_TM_PATHORDER( tms , cf , SCH_Order );
		_SCH_MODULE_Set_TM_TYPE( tms , cf , SCHEDULER_MOVING_IN );
		_SCH_MODULE_Set_TM_SIDE_POINTER( tms , cf , CHECKING_SIDE , pt , BM_Side2 , pt2 );
		_SCH_MODULE_Set_TM_WAFER( tms , cf , wsa );
	}
	//-----------------------------------------------------------------------
	if ( swmode == 10 ) { // 2013.10.31

		//===========================================================================================================================
		// 2014.01.02
		//===========================================================================================================================
		if ( tms == 0 ) {
			if ( SCH_JumpMode == 0 ) {
				//
				switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , SCH_Chamber ) ) {
				case BUFFER_IN_S_MODE :
				case BUFFER_OUT_S_MODE :
					break;
				default :
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
						_SCH_MODULE_Inc_TM_OutCount( BM_Side2 );
					}
					else {
						_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
					}
					break;
				}
				//=======================================================================================================
				if ( SCH_Slot > 0 ) {
					if ( _SCH_IO_GET_WID_NAME_FROM_READ( CHECKING_SIDE , 0 , StringBuffer ) ) {
						_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_WID_SET , wsa % 100 , pt , -1 , -1 , -1 , StringBuffer , "" );
					}
				}
				if ( SCH_Slot2 > 0 ) {
					if ( SCH_Slot > 0 ) {
						if ( _SCH_IO_GET_WID_NAME_FROM_READ( BM_Side2 , 1 , StringBuffer ) ) {
							_SCH_LOG_TIMER_PRINTF( BM_Side2 , TIMER_WID_SET , wsa / 100 , pt2 , -1 , -1 , -1 , StringBuffer , "" );
						}
					}
					else {
						if ( _SCH_IO_GET_WID_NAME_FROM_READ( CHECKING_SIDE , 1 , StringBuffer ) ) {
							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_WID_SET , wsa / 100 , pt2 , -1 , -1 , -1 , StringBuffer , "" );
						}
					}
				}
				//
				//=======================================================================================================
				//
//				_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE ); // 2016.08.26
				//
				// 2016.08.26
				//
				if ( !_SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
					_SCH_MODULE_Set_TM_DoPointer( CHECKING_SIDE , 0 );
					_SCH_SUB_Remain_for_CM( CHECKING_SIDE );
				}
				//=======================================================================================================
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( BM_Side2 != CHECKING_SIDE ) {
						//
//						_SCH_MODULE_Inc_TM_DoPointer( BM_Side2 ); // 2016.08.26
						//
						// 2016.08.26
						//
						if ( !_SCH_MODULE_Chk_TM_Finish_Status( BM_Side2 ) ) {
							_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , 0 );
							_SCH_SUB_Remain_for_CM( BM_Side2 );
						}
					}
				}
				//=======================================================================================================
			}
		}
		//


		Checking_Side_Changed = -1;

		cf = _SCH_ROBOT_GET_SWAP_PLACE_ARM( cf );
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms,cf ) <= 0 ) return -1;
		//
		if ( _SCH_MODULE_Get_TM_SIDE( tms,cf ) != CHECKING_SIDE ) {
			Checking_Side_Changed = CHECKING_SIDE;
			CHECKING_SIDE = _SCH_MODULE_Get_TM_SIDE( tms,cf );
		}
		//
		pt = _SCH_MODULE_Get_TM_POINTER( tms,cf );
		//
		//-----------------------------------------------------------------------
		pt2 = 0;
		SCH_Slot = 1;
		SCH_Slot2 = 0;
		wsa = _SCH_MODULE_Get_TM_WAFER( tms,cf );
		wsb = SCH_Slot;
		//
		BM_Side2 = -1;
		pt2 = -1;
		//

		j = SCHEDULER_RUN_PLACE_TO_BM_POST_ACTION( tms , CHECKING_SIDE , pt , wsa , SCH_Slot , BM_Side2 , pt2 , wsb , SCH_Slot2 , SCH_Chamber , cf ,
										  SCH_JumpMode , PathThruModeS , PathThruModeD );
		if ( j < 0 ) return -1;

		//
		_SCH_MODULE_Set_TM_Switch_Signal( tms , -1 );
		//


	}
	//-----------------------------------------------------------------------
	if ( swmode != 10 ) { // 2013.10.31
		if ( SCH_Chamber >= BM1 ) {
			//
			if ( SCH_JumpMode != 1 ) {
				//
				if ( PathThruModeS ) {
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
						//
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCH_Chamber ) ) {
							//
							if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , SCH_Chamber , G_PLACE , G_MCHECK_NO ) ) { // 2018.06.01
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 4001 );
									}
									//
								}
								else {
									_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
								}
							}
							else {
								//
								if ( ( tms == 0 ) && ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) ) { // 2013.02.07
									//
									if ( !SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_AFTER_PICK_A0SUB4( CHECKING_SIDE , pt , SCH_Chamber ) ) {
										if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 4 );
											}
											//
										}
										else {
											_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
										}
									}
								}
								else {
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 4 );
										}
										//
									}
									else {
										_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
									}
								}
								//
							}
						}
					}
				}
				else {
					//
	//				if ( tms == 0 ) { // 2013.02.07 // 2013.03.19
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) >= BUFFER_SWITCH_FULLSWAP ) { // 2013.03.09
							//
							if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( tms , SCH_Chamber , TRUE ) ) {
								//
								if ( tms == 0 ) {
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
										//-------------------------------------------------------------------------------
										SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7001 );
										}
										//
									}
									else {
										_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
									}
								}
								else { // 2013.03.19
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7002 );
									}
									//
								}
							}
							else {
								if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCH_Chamber ) ) {
										//
										if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , SCH_Chamber , G_PLACE , G_MCHECK_NO ) ) { // 2018.06.01
											if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
												//-------------------------------------------------------------------------------
												SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
												//-------------------------------------------------------------------------------
												//
												if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7011 );
												}
												//
											}
											else {
												_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
											}
										}
										else {
											if ( ( tms == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) == SCH_Chamber )  && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2013.05.03
												if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , FALSE ) ) {
													if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
														//-------------------------------------------------------------------------------
														SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
														//-------------------------------------------------------------------------------
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 71 );
														}
														//
													}
													else {
														_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
													}
												}
											}
											else {
	//											if ( SCHEDULING_CHECK_TM_Has_More_Space( CHECKING_SIDE , tms , cf , -1 ) ) { // 2014.05.30
												if ( SCHEDULING_CHECK_TM_Has_More_Space( CHECKING_SIDE , tms , cf , SCH_Chamber ) ) { // 2014.05.30
													if ( !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
														if ( !SCHEDULING_CHECK_Other_TM_Waiting_Return( CHECKING_SIDE , tms , SCH_Chamber ) ) {
															//===========================================================================================================
															if ( _SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE ) <= 0 ) {
																switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
																case 6 :
																case 7 :
																	break;
																default :
																	//
																	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) break; // 2016.07.22
																	if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) break; // 2018.04.10
																	//
																	if ( tms == 0 ) {
																		if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , FALSE ) ) {
																			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																				//-------------------------------------------------------------------------------
																				SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																				//-------------------------------------------------------------------------------
																				//
																				if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																				}
																				//
																			}
																			else {
																				_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																			}
																		}
																	}
																	else { // 2013.03.19
																		//
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																		}
																		//
																	}
																	break;
																}
															}
															else {
																if ( tms == 0 ) {
																	if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , FALSE ) ) {
																		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																			//-------------------------------------------------------------------------------
																			SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																			//-------------------------------------------------------------------------------
																			//
																			if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																			}
																			//
																		}
																		else {
																			_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																		}
																	}
																}
																else { // 2013.03.19
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																	}
																	//
																}
															}
															//===========================================================================================================
														}
													}
													else {
														if ( !SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , -2 ) ) {
															//
															if ( tms == 0 ) {
																//
																if ( !SCHEDULER_FM_Current_No_More_Supply2( CHECKING_SIDE ) || !SCHEDULING_CHECK_SIDE_USE_ONLY_ONE_BM( tms , CHECKING_SIDE , &j ) ) { // 2014.08.16
																	//
																	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																		//-------------------------------------------------------------------------------
																		SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																		//-------------------------------------------------------------------------------
																		//
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 6 );
																		}
																		//
																	}
																	else {
																		_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																	}
																	//
																}
															}
															else { // 2013.03.19
																//
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 6 );
																}
																//
															}
														}
														else {
															if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.03.09
																if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
																	//
																	if ( tms == 0 ) {
																		while( TRUE ) {
																			k = 0;
																			mb = 0;
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++;
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++;
																			//
																			Sleep(1);
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++;
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++;
																			//
																			if ( k != mb ) continue;
																			//
																			j = mb;
																			//
																			Sleep(1);
																			//
																			k = 0;
																			mb = 0;
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++; // 2008.10.23
																			//
																			Sleep(1); // 2008.03.31
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++; // 2008.10.23
																			//
																			if ( k == mb ) {
																				if ( j == mb ) break;
																			}
																		}
																	}
																	else { // 2013.03.19
																		while( TRUE ) {
																			k = 0;
																			mb = 0;
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) mb++;
																			//
																			Sleep(1);
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) k++;
																			//
																			if ( k != mb ) continue;
																			//
																			j = mb;
																			//
																			Sleep(1);
																			//
																			k = 0;
																			mb = 0;
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) mb++;
																			//
																			Sleep(1); // 2008.03.31
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) k++;
																			//
																			if ( k == mb ) {
																				if ( j == mb ) break;
																			}
																		}
																	}
																	//
																	if ( ( mb > 1 ) || ( ( mb == 1 ) && ( SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_MIDDLE_SWAP( SCH_Chamber ) ) ) ) {
																		//
																		if ( tms == 0 ) {
																			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																				//-------------------------------------------------------------------------------
																				SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																				//-------------------------------------------------------------------------------
																				//
																				if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																				}
																				//
																			}
																			else {
																				_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																			}
																		}
																		else { // 2013.03.19
																			//
																			if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
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
									else {
										if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP ) { // 2013.03.09
											if ( SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( SCH_Chamber ) ) {
												if ( SCHEDULER_CONTROL_Chk_BM_FULL_OUT_MIDDLESWAP( tms , SCH_Chamber ) ) {
													//
													if ( tms == 0 ) {
														if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
															//-------------------------------------------------------------------------------
															SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
															//-------------------------------------------------------------------------------
															//
															if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 501 );
															}
															//
														}
														else {
															_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
														}
													}
													else { // 2013.03.19
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 501 );
														}
														//
													}
												}
												else {
													if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) {
//														if ( SCHEDULING_CHECK_TM_Has_More_Space( CHECKING_SIDE , tms , cf , -1 ) ) { // 2014.05.30
														if ( SCHEDULING_CHECK_TM_Has_More_Space( CHECKING_SIDE , tms , cf , SCH_Chamber ) ) { // 2014.05.30
															if ( !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
																//
																if ( tms == 0 ) {
																	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																		//-------------------------------------------------------------------------------
																		SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																		//-------------------------------------------------------------------------------
																		//
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 502 );
																		}
																		//
																	}
																	else {
																		_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																	}
																}
																else { // 2013.03.19
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 502 );
																	}
																	//
																}
															}
															else {
																if ( !SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , -2 ) ) {
																	if ( tms == 0 ) {
																		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																			//-------------------------------------------------------------------------------
																			SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																			//-------------------------------------------------------------------------------
																			if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 602 );
																			}
																			//
																		}
																		else {
																			_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																		}
																	}
																	else { // 2013.03.19
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 602 );
																		}
																		//
																	}
																}
																else {
																	if ( tms == 0 ) {
																		while( TRUE ) {
																			k = 0;
																			mb = 0;
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++; // 2008.10.23
																			//
																			Sleep(1);
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++; // 2008.10.23
																			//
																			if ( k != mb ) continue;
																			//
																			j = mb;
																			//
																			Sleep(1); // 2008.03.31
																			//
																			k = 0;
																			mb = 0;
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++; // 2008.10.23
																			//
																			Sleep(1);
																			//
																			if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++; // 2008.03.31
																			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++;
																			if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++; // 2008.10.23
																			//
																			if ( k == mb ) {
																				if ( j == mb ) break;
																			}
																		}
																	}
																	else { // 2013.03.19
																		while( TRUE ) {
																			k = 0;
																			mb = 0;
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) mb++;
																			//
																			Sleep(1);
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) k++;
																			//
																			if ( k != mb ) continue;
																			//
																			j = mb;
																			//
																			Sleep(1);
																			//
																			k = 0;
																			mb = 0;
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) mb++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) mb++;
																			//
																			Sleep(1); // 2008.03.31
																			//
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TA_STATION ) > 0 ) k++;
																			if ( _SCH_MODULE_Get_TM_WAFER( tms-1 , TB_STATION ) > 0 ) k++;
																			//
																			if ( k == mb ) {
																				if ( j == mb ) break;
																			}
																		}
																	}
																	//
																	if ( ( mb > 1 ) || ( ( mb == 1 ) && ( SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_MIDDLE_SWAP( SCH_Chamber ) ) ) ) { // 2008.03.31
																		//
																		if ( tms == 0 ) {
																			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																				//-------------------------------------------------------------------------------
																				SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																				//-------------------------------------------------------------------------------
																				//-------------------------------------------------------------------------------
																				//
																				if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																				}
																				//
																			}
																			else {
																				_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																			}
																		}
																		else { // 2013.03.19
																			//
																			if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																			}
																			//
																		}
																	}
																}
															}
														}
													}
													else {
														//=============================================================================================
														if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , SCH_Chamber , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , cf , FALSE ) ) { // 2007.02.05
															if ( !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
																if ( tms == 0 ) {
																	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																		//-------------------------------------------------------------------------------
																		SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																		//-------------------------------------------------------------------------------
																		//-------------------------------------------------------------------------------
																		//
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 502 );
																		}
																		//
																	}
																	else {
																		_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																	}
																}
																else { // 2013.03.19
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 502 );
																	}
																	//
																}
															}
															else {
																if ( !SCHEDULER_FM_Another_No_More_Supply( CHECKING_SIDE , -2 ) ) {
																	if ( tms == 0 ) {
																		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																			//-------------------------------------------------------------------------------
																			SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																			//-------------------------------------------------------------------------------
																			//-------------------------------------------------------------------------------
																			//
																			if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 602 );
																			}
																			//
																		}
																		else {
																			_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																		}
																	}
																	else { // 2013.03.19
																		//
																		if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 602 );
																		}
																		//
																	}
																}
															}
														}
														//=============================================================================================
													}
												}
											}
											else {
												//=========================================================================================================================
												if ( SCHEDULING_CHECK_SWITCHING_MANY_BM_SLOT_AFTER_PICK_or_PLACE( tms , CHECKING_SIDE , SCH_Chamber ) ) {
													if ( SCHEDULER_CONTROL_Chk_BM_OUT_IS_LAST_FOR_MIDDLESWAP( SCH_Chamber ) ) {
														if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , SCH_Chamber , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , cf , FALSE ) ) { // 2008.06.18
															if ( tms == 0 ) {
																if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																	//-------------------------------------------------------------------------------
																	SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																	//-------------------------------------------------------------------------------
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																	}
																	//
																	//-------------------------------------------------------------------------------
																}
																else {
																	_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																}
															}
															else { // 2013.03.19
																//
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																}
																//
															}
														}
													}
												}
												//=========================================================================================================================
											}
										}
									}
								}
							}
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_SINGLESWAP ) { // 2013.03.09
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
								mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP2( CHECKING_SIDE , SCH_Chamber , TRUE , tms );
								if ( mb == 0 ) {
									if ( SCHEDULING_CHECK_TM_Has_More_Space( CHECKING_SIDE , tms , cf , SCH_Chamber ) ) {
										if ( !SCHEDULER_FM_FM_Finish_Status( CHECKING_SIDE ) ) {
											//
											if ( SCHEDULER_More_Supply_or_BM_One_is_not_FREE_FMSIDE( tms , CHECKING_SIDE , SCH_Chamber ) ) { // 2017.10.16 AKRA+SMART
												//
												if ( _SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE ) <= 0 ) {
													switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) {
													case 6 :
													case 7 :
														break;
													default :
														//
														if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) break; // 2016.07.22
														if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) break; // 2018.04.10
														//
														if ( tms == 0 ) {
															if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , FALSE ) ) {
																if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																	//-------------------------------------------------------------------------------
																	SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																	//-------------------------------------------------------------------------------
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																	}
																	//
																}
																else {
																	_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
																}
															}
														}
														else { // 2013.03.19
															//
															if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
															}
															//
														}
														break;
													}
												}
												else {
													if ( tms == 0 ) {
														if ( !SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( CHECKING_SIDE , -1 , FALSE ) ) { // 2004.11.09
															if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																//-------------------------------------------------------------------------------
																SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																//-------------------------------------------------------------------------------
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
																}
																//
															}
															else {
																_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
															}
														}
													}
													else { // 2013.03.19
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 7 );
														}
														//
													}
												}
											}
										}
										else {
											if ( !SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( CHECKING_SIDE , SCH_Chamber , 0 ) ) {
												if ( tms == 0 ) {
													if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
														//-------------------------------------------------------------------------------
														SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
														//-------------------------------------------------------------------------------
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 8 );
														}
														//
													}
													else {
														_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
													}
												}
												else { // 2013.03.19
													//
													if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 8 );
													}
													//
												}
											}
											else { // 2009.03.11
												if ( tms == 0 ) {
													//
													if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) || ( _SCH_MODULE_Get_MFALS_WAFER( 2 ) > 0 ) ) {
														if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
															//-------------------------------------------------------------------------------
															SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
															//-------------------------------------------------------------------------------
															//
															if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 8 );
															}
															//
														}
														else {
															_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
														}
													}
													else { // 2011.04.27
														while( TRUE ) {
															k = 0;
															mb = 0;
															//
															if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++; // 2008.03.31
															if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
															if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
															if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
															if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++; // 2008.10.23
															//
															Sleep(1);
															//
															if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++; // 2008.03.31
															if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
															//if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++; // 2011.05.31
															//if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++; // 2011.05.31
															if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++; // 2011.05.31
															if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++; // 2011.05.31
															if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++; // 2008.10.23
															//
															if ( k != mb ) continue;
															//
															j = mb;
															//
															Sleep(1); // 2008.03.31
															//
															k = 0;
															mb = 0;
															//
															if ( FM_Pick_Running_Blocking_style_0 != 0 ) mb++; // 2008.03.31
															if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) mb++;
															if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) mb++;
															if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) mb++;
															if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) mb++; // 2008.10.23
															//
															Sleep(1);
															//
															if ( FM_Pick_Running_Blocking_style_0 != 0 ) k++; // 2008.03.31
															if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) k++;
															if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k++;
															if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k++;
															if ( SCHEDULING_CHECK_ALIGN_BUFFER_CHAMBER_Has_Wafer() ) k++; // 2008.10.23
															//
															if ( k == mb ) {
																if ( j == mb ) break;
															}
														}
														//
														if ( ( mb > 1 ) || ( ( mb == 1 ) && ( SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_SINGLE_SWAP( SCH_Chamber ) ) ) ) { // 2008.03.31
															if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
																//-------------------------------------------------------------------------------
																SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
																//-------------------------------------------------------------------------------
																//-------------------------------------------------------------------------------
																//
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
																}
																//
															}
															else {
																_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_FM_STATION );
															}
														}
														//
													}
												}
												else { // 2013.03.19
													while( TRUE ) {
														k = 0;
														mb = 0;
														//
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TA_STATION ) > 0 ) mb++;
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TB_STATION ) > 0 ) mb++;
														//
														Sleep(1);
														//
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TA_STATION ) > 0 ) k++;
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TB_STATION ) > 0 ) k++;
														//
														if ( k != mb ) continue;
														//
														j = mb;
														//
														Sleep(1); // 2008.03.31
														//
														k = 0;
														mb = 0;
														//
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TA_STATION ) > 0 ) mb++;
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TB_STATION ) > 0 ) mb++;
														//
														Sleep(1);
														//
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TA_STATION ) > 0 ) k++;
														if ( _SCH_MODULE_Get_TM_WAFER( tms-1,TB_STATION ) > 0 ) k++;
														//
														if ( k == mb ) {
															if ( j == mb ) break;
														}
													}
													//
													if ( ( mb > 1 ) || ( ( mb == 1 ) && ( SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_SINGLE_SWAP( SCH_Chamber ) ) ) ) { // 2008.03.31
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 603 );
														}
													}
												}
											}
										}
									}
								}
								else if ( mb == 1 ) {
									if ( tms == 0 ) {
										//-------------------------------------------------------------------------------
										SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
										//-------------------------------------------------------------------------------
									}
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 9 );
									}
									//
								}
							}
						}
						//
						else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2008.07.01 // 2013.03.09
							if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) { // 2008.07.12
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCH_Chamber ) ) {
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
										if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != 0 ) {
											if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) != SCH_Chamber ) {
												//
												if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 19 );
												}
												//
											}
										}
									}
									//
									if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) ) {
										if ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() == SCH_Chamber ) {
											SCHEDULER_CONTROL_Set_BM_Switch_Separate( 0 , 0 , 0 , 0 , 0 );
											//
										}
									}
									//
								}
							}
						}
						//
	//				} // 2013.03.19
				}
			}
			else {
				if ( PathThruModeD ) { // 2006.12.21
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCH_Chamber ) ) {
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
	//							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 5 ); // 2013.05.23
//								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms , 5 ); // 2013.05.23 // 2016.10.30
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 5 ); // 2013.05.23 // 2016.10.30
							}
							else {
								_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_TM_STATION );
							}
						}
					}
				}
				else { // 2013.05.23
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( SCH_Chamber ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCH_Chamber ) ) {
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) ) >= BUFFER_SWITCH_FULLSWAP ) {
								//
								if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) , SCH_Chamber , G_PICK , G_MCHECK_SAME ) ) {
									//
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
//										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms , 51 ); // 2016.10.30
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 51 ); // 2016.10.30
									}
									else {
										_SCH_MODULE_Set_BM_FULL_MODE( SCH_Chamber , BUFFER_TM_STATION );
									}
									//
								}
								//
							}
						}
					}
				}
			}
			//===========================================================================================================================
			if ( tms == 0 ) {
				if ( SCH_JumpMode == 0 ) {
					//
					switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , SCH_Chamber ) ) { // 2008.12.03
					case BUFFER_IN_S_MODE :
					case BUFFER_OUT_S_MODE :
						break;
					default :
						if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2008.06.10
							_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE ); // 2008.06.10
							_SCH_MODULE_Inc_TM_OutCount( BM_Side2 ); // 2008.06.10
						}
						else {
							_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE ); // 2008.06.10
						}
						break;
					}
					//=======================================================================================================
					if ( SCH_Slot > 0 ) {
						if ( _SCH_IO_GET_WID_NAME_FROM_READ( CHECKING_SIDE , 0 , StringBuffer ) ) {
							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_WID_SET , wsa % 100 , pt , -1 , -1 , -1 , StringBuffer , "" );
						}
					}
					if ( SCH_Slot2 > 0 ) {
						if ( SCH_Slot > 0 ) {
							if ( _SCH_IO_GET_WID_NAME_FROM_READ( BM_Side2 , 1 , StringBuffer ) ) { // 2007.04.18
								_SCH_LOG_TIMER_PRINTF( BM_Side2 , TIMER_WID_SET , wsa / 100 , pt2 , -1 , -1 , -1 , StringBuffer , "" ); // 2007.04.18
							}
						}
						else {
							if ( _SCH_IO_GET_WID_NAME_FROM_READ( CHECKING_SIDE , 1 , StringBuffer ) ) {
								_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_WID_SET , wsa / 100 , pt2 , -1 , -1 , -1 , StringBuffer , "" );
							}
						}
					}
				}
			}
			//===========================================================================================================================
			switch( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( tms ) ) {
			case GROUP_HAS_SWITCH_NONE : break;
			case GROUP_HAS_SWITCH_INS :
				switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , SCH_Chamber ) ) {
				case BUFFER_IN_MODE :
					if ( SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( tms , CHECKING_SIDE , 1 , pt ) ) {
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );
					}
					else {
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_INS );
					}
					break;
				case BUFFER_IN_S_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );	break;
				case BUFFER_OUT_S_MODE :	_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );	break;
				case BUFFER_OUT_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );	break;
				}
				break;
			case GROUP_HAS_SWITCH_OUTS :
				switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , SCH_Chamber ) ) {
				case BUFFER_IN_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );	break;
				case BUFFER_IN_S_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );	break;
				case BUFFER_OUT_S_MODE :	_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );	break;
				case BUFFER_OUT_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUTS );	break;
				}
				break;
			case GROUP_HAS_SWITCH_ALL :
				switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , SCH_Chamber ) ) {
				case BUFFER_IN_MODE :
					if ( SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( tms , CHECKING_SIDE , 1 , pt ) ) {
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );
					}
					else {
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_INS );
					}
					break;
				case BUFFER_IN_S_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );	break;
				case BUFFER_OUT_S_MODE :	_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );	break;
				case BUFFER_OUT_MODE :		_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUTS );	break;
				}
				break;
			}
		}
		else {
			switch( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( tms ) ) {
			case GROUP_HAS_SWITCH_NONE : break;
			case GROUP_HAS_SWITCH_INS :
			case GROUP_HAS_SWITCH_OUTS :
			case GROUP_HAS_SWITCH_ALL :
				if ( SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( tms , CHECKING_SIDE , 2 , pt ) ) {
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_RUN );
				}
				else {
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );
				}
				break;
			}
			_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
		}
		//-----------------------------------------------------------------------
		if ( tms == 0 ) { // 2013.02.07
			if ( SCH_JumpMode == 0 ) {
				//
				//
//				_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE ); // 2016.08.26
				//
				// 2016.08.26
				//
				if ( !_SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
					_SCH_MODULE_Set_TM_DoPointer( CHECKING_SIDE , 0 );
					_SCH_SUB_Remain_for_CM( CHECKING_SIDE );
				}
				//=======================================================================================================
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( BM_Side2 != CHECKING_SIDE ) {
						//
//						_SCH_MODULE_Inc_TM_DoPointer( BM_Side2 ); // 2016.08.26
						//
						// 2016.08.26
						//
						if ( !_SCH_MODULE_Chk_TM_Finish_Status( BM_Side2 ) ) {
							_SCH_MODULE_Set_TM_DoPointer( BM_Side2 , 0 );
							_SCH_SUB_Remain_for_CM( BM_Side2 );
						}
					}
				}
				//=======================================================================================================
			}
		}
		//-----------------------------------------------------------------------
		_SCH_TIMER_SET_ROBOT_TIME_START( tms , cf );
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 19 END_TYPE_11(PICKfromBM) = Chamber(BM%d) finger(%d) pt(%d,%d) slot(%d,%d) BMSide2(%d)\n" , tms + 1 , SCH_Chamber - BM1 + 1 , cf , pt , pt2 , SCH_Slot , SCH_Slot2 , BM_Side2 );
		//----------------------------------------------------------------------
	}
	//====================================================================================================
	SCHEDULER_Set_TM_Pick_BM_Signal( tms , 0 );
	//====================================================================================================
	if ( Checking_Side_Changed != -1 ) {
		CHECKING_SIDE = Checking_Side_Changed;
	}
	else {
	//=================================================================================================
		//-----------------------------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( tms ) != -1 ) ) {
//			return 2; // 2013.10.31
			if ( swmode != 10 ) return 2; // 2013.10.31
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------
	*Run_Doing = TRUE;
	//----------------------------------------------------------
	return 0;
}


int SCHEDULER_RUN_PICK_FROM_BM( int tms , int CHECKING_SIDE , int SCH_SchPt , int SCH_Chamber , int SCH_Slot , int SCH_JumpMode , int SCH_Order , int SCH_No /*nextpm*/ , int *skip_b1 , int *skip_b2 , BOOL *Run_Doing ) { // 2013.03.19
	int Res , tmdata;
	//
	_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK( SCH_Chamber , -1 ); // 2015.06.01
	//
	tmdata = _SCH_PRM_GET_MODULE_TMS_GROUP( SCH_Chamber );
	//
	if ( tmdata >= 0 ) {
		//
		EnterCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
		if ( SCHEDULER_Get_MULTI_GROUP_TMS( tmdata ) ) {
			//
//			_IO_CIM_PRINTF( "MULTI_GROUP_TMS[%d] LOCKING(%d) REJECT at %s in SCHEDULER_RUN_PICK_FROM_BM\n" , tms + 1 , tmdata , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( SCH_Chamber ) );
			//
			LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
			return 0;
		}
		SCHEDULER_Set_MULTI_GROUP_TMS( tmdata );
		LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
	}
	//
	Res = SCHEDULER_RUN_PICK_FROM_BM_SUB( tms , CHECKING_SIDE , SCH_SchPt , SCH_Chamber , SCH_Slot , SCH_JumpMode , SCH_Order , SCH_No , skip_b1 , skip_b2 , Run_Doing );
	//
	if ( tmdata >= 0 ) SCHEDULER_ReSet_MULTI_GROUP_TMS( tmdata );
	//
	return Res;
}


//int SCHEDULER_Select_OneChamber_and_OtherBuffering_OVERTMS( int side , int pointer , int cldepth , int tms ); // 2013.02.07 // 2013.03.26


int SCHEDULER_RUN_PLACE_TO_BM_POST_ACTION( int tms , int s1 , int p1 , int w1 , int l1 , int s2 , int p2 , int w2 , int l2 , int PlaceCh , int PlaceArm ,
										  int JumpMode , int PathThruModeS , int PathThruModeD ) {
	int mb;
	int bmrskip;
	//-----------------------------------------------------------------------
	if ( PlaceCh >= BM1 ) {
		if ( JumpMode != 1 ) {
			//-----------------------------------------------------------------------
			if ( PathThruModeS ) {
				if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 , s1 , p1 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 , s2 , p2 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
				}
				else if ( l1 > 0 ) {
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 , s1 , p1 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
				}
				else if ( l2 > 0 ) {
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 , s1 , p1 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
				}
			}
			else {
				if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s2 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p2 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
					else {
						//====================================================================================================
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) {
							_SCH_MODULE_Set_BM_SIDE( PlaceCh , 1 , s1 );
						}
						//====================================================================================================
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						//==================================================================================================================
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
						//==================================================================================================================
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p2 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p2 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
				}
				else if ( l1 > 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						//
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
					}
					else {
						//====================================================================================================
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) {
							_SCH_MODULE_Set_BM_SIDE( PlaceCh , 1 , s1 );
						}
						//====================================================================================================
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_OUTWAIT_STATUS );
					}
				}
				else if ( l2 > 0 ) {
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s1 );
						//
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
					else {
						//====================================================================================================
						if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) {
							_SCH_MODULE_Set_BM_SIDE( PlaceCh , 1 , s1 );
						}
						//====================================================================================================
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p1 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
				}
			}
			//
			_SCH_MODULE_Set_TM_WAFER( tms , PlaceArm , 0 );
			//
			if ( tms == 0 ) {
				switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , PlaceCh ) ) {
				case BUFFER_IN_S_MODE :
				case BUFFER_OUT_S_MODE :
					break;
				default :
					if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
						_SCH_MODULE_Inc_TM_InCount( s1 );
						_SCH_MODULE_Inc_TM_InCount( s2 );
					}
					else {
						_SCH_MODULE_Inc_TM_InCount( s1 );
					}
					break; // 2014.10.08
				}
				//=================================================================================
				_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , s1 , p1 );
				if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , s2 , p2 );
				}
			}
			//=================================================================================
			if ( PathThruModeS ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( PlaceCh ) ) {
					if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) {
						if ( _SCH_PRM_GET_MODULE_SIZE( PlaceCh ) > 2 ) {
							if ( SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( PlaceCh , FALSE ) ) {
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 13 );
								}
								//
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
								//
							}
							else {
								if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( s1 , tms , PlaceCh , 0 , FALSE , -1 , FALSE ) ) {
									if ( SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( tms , FALSE , TRUE , -1 ) != 0 ) {
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 1302 );
										}
										//
										if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
									}
								}
							}
						}
						else {
							//
							if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 13 );
							}
							//
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( PlaceCh ) ) {
							//
							if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 13 );
							}
							//
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
						}
					}
				}
			}
			else {
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) <= BUFFER_SWITCH_BATCH_PART ) {
						//----------------------------------------------------------------
						if ( w1 > 0 ) {
							//-------------------------------------------------------
							bmrskip = FALSE;
							//-------------------------------------------------------
							if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) {
								//
								switch ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( s1 ) ) {
								case BATCHALL_SWITCH_LL_2_LL :
								case BATCHALL_SWITCH_LP_2_LL :
									bmrskip = TRUE;
									break;
								}
								//
								if ( bmrskip || ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 0 ) ) {
									//
									if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) {
										if ( ( w1 % 100 ) > 0 ) _SCH_LOG_TIMER_PRINTF( s1 , TIMER_CM_END , w1 % 100 , -1 , p1 , -1 , -1 , "" , "" );
										if ( ( w1 / 100 ) > 0 ) _SCH_LOG_TIMER_PRINTF( s1 , TIMER_CM_END , w1 / 100 , -1 , p1 , -1 , -1 , "" , "" );
									}
									else {
										_SCH_LOG_TIMER_PRINTF( s1 , TIMER_CM_END , w1 , -1 , p1 , -1 , -1 , "" , "" );
									}
									//
									_SCH_MULTIJOB_PROCESSJOB_END_CHECK( s1 , p1 );
									//
								}
							}
							//
							mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( s1 , PlaceCh , FALSE , FALSE , -1 , -1 );
							//
							if ( mb == 99 ) {
								if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(s1) == 0 ) || ( ( tms == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) ) ) { // 2006.03.21 // 2013.05.03
									if ( bmrskip ) {
										_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
									}
									else {
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 12 );
										}
										//
									}
								}
								else {
									//===============================================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) {
										if ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) {
											if ( bmrskip ) {
												_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
											}
											else {
												//
												if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 255 );
												}
												//
											}
										}
									}
									//===============================================================================================
								}
							}
							else {
								//=====================================================================================
								if (
									( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
									( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == s1 ) &&
									( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() == PlaceCh )
									) {
									if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCHALL( s1 , PlaceCh , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSCnt() , ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) && ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) ) ) { // 2007.03.27
										if ( bmrskip ) { // 2010.02.03
											_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
										}
										else {
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 253 );
											}
											//
										}
									}
								}
								else {
									if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
										if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(s1) == 0 ) || ( ( tms == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) ) ) { // 2006.03.21 // 2013.05.03
											if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(s1) != 0 ) && ( ( tms != 0 ) || ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( s1 ) != 0 ) ) ) { // 2006.08.04 // 2013.05.03
												if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_OK_OUT_BATCH_ALL( s1 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(s1) ) ) { // 2006.08.04
													if ( bmrskip ) {
														_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
													}
													else {
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 251 );
														}
														//
													}
												}
											}
											else {
												if ( bmrskip ) {
													_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
												}
												else {
													//
													if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 25 );
													}
													//
												}
											}
										}
										else {
											//===============================================================================================
											if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
												if ( mb == 1 ) {
													if ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) {
														if ( bmrskip ) { // 2010.02.03
															_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
														}
														else {
															//
															if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 25 );
															}
															//
														}
													}
												}
												else { // 2007.03.27
													if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) {
														if ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) {
															if ( bmrskip ) { // 2010.02.03
																_SCH_MODULE_Set_BM_FULL_MODE( PlaceCh , BUFFER_FM_STATION );
															}
															else {
																//
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 254 );
																}
																//
															}
														}
													}
												}
											}
											//===============================================================================================
										}
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
							if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
								if ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) {
									if ( !_SCH_COMMON_CHECK_END_PART( s1 , 1 ) ) {
										return -1;
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) >= BUFFER_SWITCH_FULLSWAP ) { // 2004.03.22 // 2013.03.09
						mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( s1 , PlaceCh , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP , ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) , tms , ( PlaceArm == TA_STATION ) ? TB_STATION : TA_STATION ); // 2004.03.22 // 2013.03.09 // 2013.03.09
						if ( mb == 99 ) { // 2004.03.22
							//-------------------------------------------------------------------------------
							if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
							//-------------------------------------------------------------------------------
							//
							if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 14 );
							}
							//
							//========================================================================================================================
							SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( tms , s1 , PlaceCh , 2 );
							//========================================================================================================================
						}
						else if ( mb == 1 ) { // 2004.03.22
							if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
								if ( SCHEDULING_CHECK_TM_Another_Free_Status( s1 , FALSE , TRUE , PlaceCh ) ) { // 2004.03.24
									//-------------------------------------------------------------------------------
									if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
									//-------------------------------------------------------------------------------
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 15 );
									}
									//
									//========================================================================================================================
									SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( tms , s1 , PlaceCh , 1 );
									//========================================================================================================================
								}
							}
							else { // 2007.02.15
								if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( s1 , tms , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , -1 , FALSE ) ) {
									//
									if ( !SCHEDULING_CHECK_Chk_OTHERGROUPBM_WAITING_HASRETURN( s1 , tms , FALSE , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) ) { // 2018.05.08
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 152 );
										}
										//
									}
								}
								else {
									//===========================================================================
									if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( tms , PlaceCh , TRUE ) ) {
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 153 );
										}
										//
									}
									//===========================================================================
								}
							}
						}
						else if ( mb == -1 ) {
							//=========================================================================================================================
							if ( SCHEDULING_CHECK_SWITCHING_MANY_BM_SLOT_AFTER_PICK_or_PLACE( tms , s1 , PlaceCh ) ) {
								if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
									if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( s1 , tms , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , -1 , TRUE ) ) {
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 154 );
										}
										//
										//-------------------------------------------------------------------------------
									}
								}
							}
							//=========================================================================================================================
						}
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_SINGLESWAP ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( PlaceCh ) ) {
							if ( _SCH_PRM_GET_MODULE_SIZE( PlaceCh ) == 1 ) {
								//-------------------------------------------------------------------------------
								if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
								//-------------------------------------------------------------------------------
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 17 );
								}
								//
							}
							else {
								mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP2( s1 , PlaceCh , FALSE , tms );
								if ( mb == 1 ) {
									//-------------------------------------------------------------------------------
									if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
									//-------------------------------------------------------------------------------
									//-------------------------------------------------------------------------------
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 16 );
									}
									//
								}
								else if ( mb > 0 ) {
									if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( s1 , tms , PlaceCh );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) {
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 17 );
										}
										//
									}
								}
							}
						}
					}
			}
		}
		else {
			//
			if ( ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( tms != _SCH_PRM_GET_MODULE_GROUP( PlaceCh ) ) ) {
				if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 , s1 , p1 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
					_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 , s2 , p2 );
					_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
				}
				else if ( l1 > 0 ) {
					if ( ( l1 % 2 ) == 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 - 1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 - 1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
					}
					else {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 + 1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 + 1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
					}
				}
				else if ( l2 > 0 ) {
					if ( ( l2 % 2 ) == 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 - 1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 - 1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
					else {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 + 1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 + 1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
				}
			}
			else {
				if ( PathThruModeD ) {
					if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 , s2 , p2 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
					else if ( l1 > 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l1 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
					}
					else if ( l2 > 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( PlaceCh , l2 , s1 , p1 );
						_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
				}
				else {
					if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s2 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p2 );
						if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
					else if ( l1 > 0 ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l1 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l1 , p1 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l1 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) % 100 , BUFFER_INWAIT_STATUS );
					}
					else if ( l2 > 0 ) {
						_SCH_MODULE_Set_BM_SIDE( PlaceCh , l2 , s1 );
						_SCH_MODULE_Set_BM_POINTER( PlaceCh , l2 , p1 );
						if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) == PATHDO_WAFER_RETURN )
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
						else
							_SCH_MODULE_Set_BM_WAFER_STATUS( PlaceCh , l2 , _SCH_MODULE_Get_TM_WAFER( tms,PlaceArm ) / 100 , BUFFER_INWAIT_STATUS );
					}
				}
			}
			//-----------------------------------------------------------------------
			_SCH_MODULE_Set_TM_WAFER( tms , PlaceArm , 0 );
			//-----------------------------------------------------------------------
			if ( PathThruModeD ) { // 2006.12.21
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( PlaceCh ) ) {
					if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( s1 , tms , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , TRUE , -1 , FALSE ) ) { // 2006.12.06 // 2013.03.09
						//
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 9 );
						//
						if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
					}
				}
			}
			else { // 2013.03.18
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( PlaceCh ) ) {
					if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( s1 , tms , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , TRUE , -1 , FALSE ) ) { // 2013.03.09
						//
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_FULLSWAP ) { // 2018.05.08
							if ( !SCHEDULING_CHECK_Chk_OTHERGROUPBM_WAITING_HASRETURN( s1 , tms , TRUE , PlaceCh , Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) ) { // 2018.05.08
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 9 );
							}
						}
						else {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , s1 , PlaceCh , w1 , TRUE , tms+1 , 9 );
						}
						//
					}
				}
			}
			//-----------------------------------------------------------------------
		}
		//-----------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( 0 ) != GROUP_HAS_SWITCH_NONE ) {
			_SCH_CLUSTER_Set_SwitchInOut( s1 , p1 , SWITCH_WILL_GO_READY );
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( s1 , tms+1 , "TM%d STEP 21 END_TYPE_12(PLACEtoBM) = Chamber(BM%d) finger(%d) p1(%d,%d) slot(%d,%d)\n" , tms+1 , PlaceCh - BM1 + 1 , PlaceArm , p1 , p2 , l1 , l2 );
		//----------------------------------------------------------------------
		SCHEDULER_Set_TM_MidCount( tms , 0 );
		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
	}
	else {
		_SCH_MODULE_Set_TM_WAFER( tms , PlaceArm , 0 );
		//
		if ( ( l1 > 0 ) && ( l2 > 0 ) ) {
			_SCH_MODULE_Inc_TM_InCount( s1 );
			_SCH_MODULE_Inc_TM_InCount( s2 );
		}
		else {
			_SCH_MODULE_Inc_TM_InCount( s1 );
		}
		//-----------------------------------------------------------------------
		SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( s1 , -1 );
		//----------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( tms ) != GROUP_HAS_SWITCH_NONE ) {
			_SCH_CLUSTER_Set_SwitchInOut( s1 , p1 , SWITCH_WILL_GO_READY );
		}
		//-----------------------------------------------------------------------
	}
	//====================================================================================================
	if ( tms == 0 ) {
		if ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) {
			if ( ( PlaceCh >= BM1 ) && ( JumpMode != 1 ) ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status( s1 ) ) {
					if ( _SCH_MODULE_Chk_TM_Finish_Status( s1 ) ) {
						_SCH_PREPOST_LotPost_Part_for_All_BM_Return( s1 );
					}
				}
			}
		}
	}
	//====================================================================================================
	return 0;
}



int SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( int s , int p , BOOL whenalldis , BOOL dataonly , int log ) { // 2015.06.10
	int i , ch , find;
	//
	if ( !Get_RunPrm_FM_PM_CONTROL_USE() ) return 0; // 2016.02.19
	//
	if ( whenalldis ) {
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
			//
			_SCH_LOG_DEBUG_PRINTF( s , 1 , "TM STEP FM_PM_CONTROL_RETURN_RECOVER(%d) 2 s=%d,p=%d,when=%d,dataonly=%d\n" , log , s , p , whenalldis , dataonly );
			//
			return 0;
		}
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
//			ch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , i ); // 2015.12.01
			ch = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , i ); // 2015.12.01
			//
			if ( ch > 0 ) {
				//
				if ( ch >= Get_RunPrm_FM_PM_START_MODULE() ) continue;
				//
				_SCH_LOG_DEBUG_PRINTF( s , 1 , "TM STEP FM_PM_CONTROL_RETURN_RECOVER(%d) 3 s=%d,p=%d,when=%d,dataonly=%d\n" , log , s , p , whenalldis , dataonly );
				//
				return 0;
				//
			}
			//
		}
	}
	//
	if ( !dataonly ) {
		//
		_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
		//
		_SCH_CLUSTER_Set_Optimize( s , p , 1000 + log ); // 2016.02.24
		//
	}
	else {
		//
		_SCH_CLUSTER_Set_Optimize( s , p , log ); // 2016.02.24
		//
	}
	//
	find = 0;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
//		ch = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , 0 , i ); // 2015.12.01
		ch = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , i ); // 2015.12.01
		//
		if ( ch > 0 ) {
			//
			if ( ch >= Get_RunPrm_FM_PM_START_MODULE() ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , 0 , i ); // 2015.12.02
				continue;
			}
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , ch ) != MUF_01_USE ) {
				//
				_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , 0 , i );
				//
				find = ( find * 10 ) + 1;
				//
			}
			else {
				if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) {
					//
					_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , p , 0 , i );
					//
					find = ( find * 10 ) + 2;
					//
				}
			}
		}
		else if ( ch < 0 ) {
			//
			if ( (-ch) >= Get_RunPrm_FM_PM_START_MODULE() ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Delete( s , p , 0 , i ); // 2015.12.02
				continue;
			}
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , (-ch) ) == MUF_01_USE ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( (-ch) , FALSE , -1 ) ) {
					//
					_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , p , 0 , i );
					//
					find = ( find * 10 ) + 3;
					//
				}
				//
			}
		}
		//
	}
	//
	_SCH_LOG_DEBUG_PRINTF( s , 1 , "TM STEP FM_PM_CONTROL_RETURN_RECOVER(%d) 1 s=%d,p=%d,when=%d,dataonly=%d,find=%d\n" , log , s , p , whenalldis , dataonly , find );
	//
	return find;
}

int SCHEDULER_RUN_PLACE_TO_BMPM_SUB( int tms , int CHECKING_SIDE , int SCH_No , int SCH_Chamber , int SCH_Slot , int SCH_Type , int SCH_RETMODE , int SCH_JumpMode , int SCH_Order , int SCH_SWITCH , BOOL *Run_Doing ) { // 2013.02.07
	int Checking_Side_Changed , ch;

	int j , pt;
	int BM_Side2 , pt2 , SCH_Slot2 , SCH_Order2;
	int wsa , wsb;
//	int mb; // 2013.10.31
//	int bmrskip; // 2013.10.31
	int currpathdo , R_Retmode , prcs_time , post_time;
	char NextPM[256];
	int pmslot , m_f_side , m_f_pt;
	BOOL multiprcs;

	int Sav_Int_1 , Sav_Int_2 , Sav_Int_3 , Sav_Int_4 , Sav_Int_5;

	int Result , swmode , Function_Result;
	//
	BOOL PathThruModeS , PathThruModeD;

	//=============================================================================================
	if ( _SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK( tms + TM , SCH_No ) == 1 ) return 1; // 2015.06.15
	//=============================================================================================
	if ( tms == 0 ) {
		PathThruModeS = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 );
		PathThruModeD = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
	}
	else {
		PathThruModeS = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
		PathThruModeD = ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 1 );
	}
	//=============================================================================================
	Checking_Side_Changed = -1;
	//=============================================================================================
	//-----------------------------------------------------------------------
	pt = _SCH_MODULE_Get_TM_POINTER( tms , SCH_No );
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 20 RUN_TYPE_12(PLACEtoBM/PM) %d\n" , tms+1 , pt );
	//----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	if ( SCH_Type == SCHEDULER_MOVING_OUT ) {
		//--------------------------------------------------------------------------------
		//
		if ( SCH_RETMODE ) {
			//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 21 RUN_TYPE_12(PLACEtoBM/PM) %d SCH_RETMODE\n" , tms+1 , pt );
	//----------------------------------------------------------------------

			_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
			//
			SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 8 ); // 2016.02.19
			//
			_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
			//===============================================================================
			/*
			// 2014.07.17
			if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2013.03.09
				if ( ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) % 100 ) > 0 ) ) { // 2007.04.18
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No ) , -1 ); // 2009.04.06
				}
			}
			*/
			//
			// 2014.07.17
			//
			if ( ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) % 100 ) > 0 ) ) { // 2007.04.18


	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 22 RUN_TYPE_12(PLACEtoBM/PM) %d SCH_RETMODE\n" , tms+1 , SCH_No );
	//----------------------------------------------------------------------

				_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No ) , -1 ); // 2009.04.06
				//
				SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No ) , FALSE , TRUE , 9 ); // 2016.02.19
				//
			}
			//===============================================================================
		}
		else { // 2018.09.18
			//
			if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) ) {
				//
				if ( ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) % 100 ) > 0 ) ) {
					//
					BM_Side2 = _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No );
					pt2 = _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No );
					//
					if      ( ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN ) && ( _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) != PATHDO_WAFER_RETURN ) ) {
						//
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( BM_Side2 , pt2 , -1 );
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( BM_Side2 , pt2 , FALSE , TRUE , 9 );
						//
					}
					else if ( ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) != PATHDO_WAFER_RETURN ) && ( _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) == PATHDO_WAFER_RETURN ) ) {
						//
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 9 );
						//
					}
					//
				}
				//
			}
			//
		}
		//--------------------------------------------------------------------------------
		//-----------------------------------------------------------------------
		ch = SCH_Chamber;
		//==============================================================================================================
		if ( _SCH_PRM_GET_Putting_Priority( ch ) < 0 ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) return 0;
		}
		//==============================================================================================================
		//-----------------------------------------------------------------------
		if ( SCH_JumpMode == 0 ) { // 2002.12.16
			//-----------------------------------------------------------------------
			Sav_Int_3 = SCHEDULER_Get_PM_MidCount( tms );
			//-----------------------------------------------------------------------
			SCHEDULER_Set_PM_MidCount( tms , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) ); // 2002.12.16
		} // 2002.12.16
		//-----------------------------------------------------------------------
		Sav_Int_4 = _SCH_MODULE_Get_BM_MidCount();
		Sav_Int_5 = SCHEDULER_Get_TM_MidCount( tms );
		//-----------------------------------------------------------------------
		if ( ch >= BM1 ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) < 0 ) { // 2003.09.09
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 );
				return -1;
			}
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
				EnterCriticalSection( &CR_SINGLEBM_STYLE_0 );
				SCHEDULER_Set_TM_MidCount( tms , 11 );
				if ( _SCH_MODULE_Get_BM_WAFER( Get_Prm_MODULE_BUFFER_SINGLE_MODE() , 1 ) > 0 ) {
					SCHEDULER_Set_TM_MidCount( tms , 0 );
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
					return 0;
				}
			}
			else {
				if ( SCH_JumpMode == 1 ) {
					SCHEDULER_Set_TM_MidCount( tms , 12 );
				}
			}
		}
		//-----------------------------------------------------------------------
		_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
		SCHEDULER_Set_TM_LastMove_Signal( tms , -1 );
		//-----------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) ) { // 2013.03.09
			//
			BM_Side2 = _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ); // 2007.04.18
			pt2 = _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No );
			//=========================================================================================
			if ( PathThruModeS ) { // 2007.02.22
				if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) <= 0 ) {
					SCH_Slot2 = SCH_Slot + 1; // 2006.08.01
					SCH_Slot = 0;
					wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
					wsb = SCH_Slot + SCH_Slot2*100;
				}
				else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) <= 0 ) {
					SCH_Slot2 = 0;
					wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
					wsb = SCH_Slot + SCH_Slot2*100;
				}
				else {
					SCH_Slot2 = SCH_Slot + 1;
					wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
					wsb = SCH_Slot + SCH_Slot2*100;
				}
			}
			else {
				if ( ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( CHECKING_SIDE ) % 2 ) == 0 ) { // 2007.02.22
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) <= 0 ) {
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2007.11.05 // 2013.03.09
							SCH_Slot2 = SCH_Slot + 2; // 2007.11.05
						}
						else {
							SCH_Slot2 = SCH_Slot + 1; // 2006.08.01
						}
						SCH_Slot = 0;
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
					else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) <= 0 ) {
						SCH_Slot2 = 0;
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
					else {
						if ( ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2007.11.05 // 2013.03.09
							SCH_Slot2 = SCH_Slot + 2;
						}
						else {
							SCH_Slot2 = SCH_Slot + 1;
						}
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
				}
				else { // 2007.02.22 // Overlapped
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) <= 0 ) {
						SCH_Slot2 = _SCH_CLUSTER_Get_SlotIn( BM_Side2 , pt2 ); // 2007.04.18
						SCH_Slot = 0;
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
					else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) <= 0 ) {
						SCH_Slot2 = 0;
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
					else {
						SCH_Slot2 = _SCH_CLUSTER_Get_SlotIn( BM_Side2 , pt2 ); // 2007.04.18
						wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
						wsb = SCH_Slot + SCH_Slot2*100;
					}
				}
			}
			//=========================================================================================
			if ( ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( tms != _SCH_PRM_GET_MODULE_GROUP( ch ) ) ) { // 2013.05.06
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( ch ) >= 0 ) { // 2014.09.24
					//
					wsa = ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) * 100 ) + ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 );
					//
				}
				//
			}
			//=========================================================================================
			if ( tms == 0 ) { // 2013.02.07
				if ( SCH_JumpMode == 0 ) {
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
						_SCH_MODULE_Dec_TM_DoubleCount( CHECKING_SIDE );
				}
			}
			//
		}
		else {
			BM_Side2 = CHECKING_SIDE; // 2007.04.18
			pt2 = 0;
			SCH_Slot2 = 0;
			wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
			wsb = SCH_Slot;
		}
		//
		//
		//-----------------------------------------------------------------------
		// User Interface
		// 2016.04.19
		//-----------------------------------------------------------------------
		switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 102 , CHECKING_SIDE , pt , ch , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order , tms , SCH_No ) ) {
		case 0 :
			//===============================================================================================================================
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s USER_FLOW_NOTIFICATION 102 Fail%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
			//===============================================================================================================================
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
			//===============================================================================================================================
			return -1;
			break;
		case 1 :
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
			return 122;
			break;
		}
		//
		//
		//-----------------------------------------------------------------------
		if ( ch >= BM1 ) {
			if ( PathThruModeS ) { // 2006.12.21
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
					if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
						if ( _SCH_MODULE_Get_BM_MidCount() != 2 ) {
							if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 8 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Fail(GoTMSide) When Placing to %s(%d:%d)[F%c]%cWHTM%dPLACEFAIL BM%d:%d:%d:%c%c%d\n" , tms +1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsb , wsa , SCH_No + 'A' , 9 , tms + 1 , ch - BM1 + 1 , wsb , wsa , SCH_No + 'A' , 9 , wsa );
								return -1;
							}
							_SCH_MODULE_Set_BM_MidCount( 2 );
						}
					}
				}
			}
		}
		//-----------------------------------------------------------------------
		if ( SCH_JumpMode == 0 ) {
			//-----------------------------------------------------------------------
			Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
			//-----------------------------------------------------------------------
			_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END );
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
				//-----------------------------------------------------------------------
				Sav_Int_2 = _SCH_CLUSTER_Get_PathStatus( BM_Side2 , pt2 ); // 2007.04.18
				//-----------------------------------------------------------------------
				_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_BM_END ); // 2007.04.18
			}
		}
		//-----------------------------------------------------------------------
		Result = 0; // 2009.07.17
		//-----------------------------------------------------------------------
		if ( ch >= BM1 ) {
			if ( SCH_JumpMode == 1 ) { // 2008.06.21
				//-----------------------------------------------------------------------
				SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_BEFORE_PLACE_TO_OTHER_BPM( tms , SCH_No , TRUE , ch ); // 2008.06.21
				//-----------------------------------------------------------------------
			}
			//=========================================================================================================
			Result = 0;
			swmode = 0;
			//
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) {
				if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
					if ( !SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , 0 , SCH_No , TRUE ) ) {
						//
						if ( tms == 0 ) { // 2013.02.07
							//-------------------------------------------------
							if ( SCH_Slot > 0 ) {
								_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , 1 , BUFFER_OUTWAIT_STATUS );
							}
							if ( SCH_Slot2 > 0 ) {
								_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , CHECKING_SIDE , pt );
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , 2 , BUFFER_OUTWAIT_STATUS );
							}
							//-------------------------------------------------
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( CHECKING_SIDE , ch , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP , ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) , tms , ( SCH_No == TA_STATION ) ? TB_STATION : TA_STATION ) != 99 ) { // 2009.07.29 // 2013.03.09
								//
								Result = 1;
								//-------------------------
								swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 1 ); // 2007.07.25
								//-------------------------
							}
							//-------------------------------------------------
							if ( SCH_Slot > 0 ) _SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , 0 , -1 );
							if ( SCH_Slot2 > 0 ) _SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , 0 , -1 );
							//-------------------------------------------------
						}
						else {
							Result = 1;
							swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 1 );
						}
					}
				}
			}
			//-------------------------
			if ( Result == 0 ) {
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( SCH_SWITCH , 0 );
			}
			//-------------------------
			//=========================================================================================================
			Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , wsb , Result , -1 , FALSE , swmode , BM_Side2 , pt2 );
			//-----------------------------------------------------------------------
			MODULE_LAST_SWITCH_0[ch] = Result; // 2010.05.21
			//-----------------------------------------------------------------------
			if ( Function_Result == SYS_ABORTED ) { // 2005.11.25
				SCHEDULER_Set_TM_MidCount( tms , 0 );
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
				return -1;
			}
			//===========================================================================================================
			else if ( Function_Result == SYS_ERROR ) {
				if ( SCH_JumpMode == 0 ) {
					SCHEDULER_Set_PM_MidCount( tms , Sav_Int_3 );
					//
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_2 ); // 2007.04.18
					}
				}
				_SCH_MODULE_Set_BM_MidCount( Sav_Int_4 );
				SCHEDULER_Set_TM_MidCount( tms , Sav_Int_5 );
				//
				return 0;
			}
			//===========================================================================================================
		}
		else {
			Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , wsb , 0 , -1 , FALSE , 0 , BM_Side2 , pt2 );
			//-----------------------------------------------------------------------
			MODULE_LAST_SWITCH_0[ch] = 0; // 2010.05.21
			//-----------------------------------------------------------------------
			if ( Function_Result == SYS_ABORTED ) { // 2005.11.25
				return -1;
			}
			//===========================================================================================================
			// 2005.11.25
			//===========================================================================================================
			else if ( Function_Result == SYS_ERROR ) {
				if ( SCH_JumpMode == 0 ) {
					SCHEDULER_Set_PM_MidCount( tms , Sav_Int_3 );
					//
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
						_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_2 ); // 2007.04.18
					}
				}
				_SCH_MODULE_Set_BM_MidCount( Sav_Int_4 );
				SCHEDULER_Set_TM_MidCount( tms , Sav_Int_5 );
				//
				return 0;
			}
		}
		//-----------------------------------------------------------------------


		j = SCHEDULER_RUN_PLACE_TO_BM_POST_ACTION( tms , CHECKING_SIDE , pt , wsa , SCH_Slot , BM_Side2 , pt2 , wsb , SCH_Slot2 , ch , SCH_No ,
										  SCH_JumpMode , PathThruModeS , PathThruModeD ); // 2013.10.31
		if ( j < 0 ) return -1;

/*
// 2013.10.31
		if ( ch >= BM1 ) {
			if ( SCH_JumpMode != 1 ) {
				//-----------------------------------------------------------------------
				if ( PathThruModeS ) { // 2006.12.21
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , BM_Side2 , pt2 ); // 2007.04.18
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
					else if ( SCH_Slot > 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
					}
					else if ( SCH_Slot2 > 0 ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , CHECKING_SIDE , pt );
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
					}
				}
				else {
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE );
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot2 , BM_Side2 ); // 2007.04.18
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt2 );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
						}
						else {
							//====================================================================================================
							// 2006.03.21
							//====================================================================================================
							if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2006.03.21
								_SCH_MODULE_Set_BM_SIDE( ch , 1 , CHECKING_SIDE );
							}
							//====================================================================================================
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE ); // 2007.07.17
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							//==================================================================================================================
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
							//==================================================================================================================
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot2 , CHECKING_SIDE ); // 2007.07.17
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt2 );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt2 ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
						}
					}
					else if ( SCH_Slot > 0 ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE );
							// 2004.03.24
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
						}
						else {
							//====================================================================================================
							if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2006.03.21
								_SCH_MODULE_Set_BM_SIDE( ch , 1 , CHECKING_SIDE );
							}
							//====================================================================================================
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE ); // 2007.07.17
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_OUTWAIT_STATUS );
						}
					}
					else if ( SCH_Slot2 > 0 ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) > BUFFER_SWITCH_BATCH_PART ) { // 2003.11.07 // 2013.03.09
							_SCH_MODULE_Set_BM_SIDE( SCH_Chamber , SCH_Slot2 , CHECKING_SIDE );
							// 2004.03.24
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
						}
						else {
							//====================================================================================================
							if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) { // 2006.03.21
								_SCH_MODULE_Set_BM_SIDE( ch , 1 , CHECKING_SIDE );
							}
							//====================================================================================================
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot2 , CHECKING_SIDE ); // 2007.07.17
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_OUTWAIT_STATUS );
						}
					}
				}
				_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
				//
				if ( tms == 0 ) { // 2013.02.07
					switch( _SCH_PRM_GET_MODULE_BUFFER_MODE( tms , ch ) ) { // 2008.12.03
					case BUFFER_IN_S_MODE :
					case BUFFER_OUT_S_MODE :
						break;
					default :
						if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2008.06.10
							_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
							_SCH_MODULE_Inc_TM_InCount( BM_Side2 ); // 2008.06.10
						} // 2008.06.10
						else { // 2008.06.10
							_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
						} // 2008.06.10
					}
					//=================================================================================
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , CHECKING_SIDE , pt );
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , BM_Side2 , pt2 ); // 2007.04.18
					}
				}
				//=================================================================================
				if ( PathThruModeS ) { // 2006.12.21
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
						if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2013.03.09
							if ( _SCH_PRM_GET_MODULE_SIZE( ch ) > 2 ) { // 2007.06.28
								if ( SCHEDULING_CHECK_BM_FULL_ALL_TMDOUBLE( ch ) ) { // 2007.06.28
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 13 );
									}
									//
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
								}
								else {
									if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , 0 , FALSE , -1 , FALSE ) ) {
										if ( SCHEDULER_CONTROL_Chk_WAIT_SUPPLY_OR_RETURN_WAFER( tms , FALSE , TRUE , -1 ) != 0 ) { // 2007.08.29
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 1302 );
											}
											//
											if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
										}
									}
								}
							}
							else {
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 13 );
								}
								//
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
							}
						}
						else {
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( ch ) ) { // 2006.12.06 // 2007.02.16
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , wsa , TRUE , tms+1 , 13 );
								}
								//
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
							}
						}
					}
				}
				else {
//					if ( tms == 0 ) { // 2013.02.07 // 2013.03.18
						if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) <= BUFFER_SWITCH_BATCH_PART ) { // 2004.03.22 // 2013.03.09
							//----------------------------------------------------------------
							// 2004.03.22
							//----------------------------------------------------------------
							if ( wsa > 0 ) {
								//-------------------------------------------------------
								bmrskip = FALSE;
								//-------------------------------------------------------
								if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2004.05.06 // 2013.03.09
									//
									switch ( SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( CHECKING_SIDE ) ) { // 2010.02.01
									case BATCHALL_SWITCH_LL_2_LL :
									case BATCHALL_SWITCH_LP_2_LL :
										bmrskip = TRUE;
										break;
									}
									//
									if ( bmrskip || ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() / 4 ) == 0 ) ) { // 2008.01.15 // 2010.03.10
										//
										if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2006.04.26 // 2013.03.09
											if ( ( wsa % 100 ) > 0 ) _SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_END , wsa % 100 , -1 , pt , -1 , -1 , "" , "" ); // 2012.02.18
											if ( ( wsa / 100 ) > 0 ) _SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_END , wsa / 100 , -1 , pt , -1 , -1 , "" , "" ); // 2012.02.18
										}
										else {
											_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_END , wsa , -1 , pt , -1 , -1 , "" , "" ); // 2012.02.18
										}
										//
										_SCH_MULTIJOB_PROCESSJOB_END_CHECK( CHECKING_SIDE , pt );
										//
									}
								}
								//
								mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( CHECKING_SIDE , ch , FALSE , FALSE , -1 , -1 ); // 2004.03.22
								//
								if ( mb == 99 ) { // 2004.03.22
//									if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2006.03.21 // 2013.05.03
									if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( ( tms == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) ) { // 2006.03.21 // 2013.05.03
										if ( bmrskip ) { // 2010.02.03
											_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
										}
										else {
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 12 );
											}
											//
										}
									}
									else {
										//===============================================================================================
										// 2007.01.25
										//===============================================================================================
										if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
											if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
												if ( bmrskip ) { // 2010.02.03
													_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
												}
												else {
													//
													if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 255 );
													}
													//
												}
											}
										}
										//===============================================================================================
									}
								}
								else { // 2004.05.10
									//=====================================================================================
									// 2007.02.05
									//=====================================================================================
									if (
										( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
										( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == CHECKING_SIDE ) &&
										( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() == ch )
										) {
										if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCHALL( CHECKING_SIDE , ch , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSCnt() , ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) && ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) ) ) { // 2007.03.27
											if ( bmrskip ) { // 2010.02.03
												_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
											}
											else {
												//
												if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 253 );
												}
												//
											}
										}
									}
									else {
										if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
//											if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2006.03.21 // 2013.05.03
											if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) == 0 ) || ( ( tms == 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) ) { // 2006.03.21 // 2013.05.03
//												if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) { // 2006.08.04 // 2013.05.03
												if ( ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) != 0 ) && ( ( tms != 0 ) || ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( CHECKING_SIDE ) != 0 ) ) ) { // 2006.08.04 // 2013.05.03
													if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_OK_OUT_BATCH_ALL( CHECKING_SIDE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(CHECKING_SIDE) ) ) { // 2006.08.04
														if ( bmrskip ) { // 2010.02.03
															_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
														}
														else {
															//
															if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 251 );
															}
															//
														}
													}
												}
												else {
													if ( bmrskip ) { // 2010.02.03
														_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
													}
													else {
														//
														if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
															_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 25 );
														}
														//
													}
												}
											}
											else {
												//===============================================================================================
												if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
													if ( mb == 1 ) {
														if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
															if ( bmrskip ) { // 2010.02.03
																_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
															}
															else {
																//
																if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 25 );
																}
																//
															}
														}
													}
													else { // 2007.03.27
														if ( ( ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() % 4 ) / 2 ) == 1 ) {
															if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
																if ( bmrskip ) { // 2010.02.03
																	_SCH_MODULE_Set_BM_FULL_MODE( ch , BUFFER_FM_STATION );
																}
																else {
																	//
																	if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
																		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 254 );
																	}
																	//
																}
															}
														}
													}
												}
												//===============================================================================================
											}
										}
									}
								}
							}
							//----------------------------------------------------------------
							// 2007.10.04
							//-----------------------------------------------------------------------------------------
							if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_BATCH_ALL ) { // 2013.03.09
								if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
									if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
										if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 1 ) ) {
											return -1;
										}
									}
								}
							}
							//-----------------------------------------------------------------------------------------
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) >= BUFFER_SWITCH_FULLSWAP ) { // 2004.03.22 // 2013.03.09
							mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( CHECKING_SIDE , ch , _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP , ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) , tms , ( SCH_No == TA_STATION ) ? TB_STATION : TA_STATION ); // 2004.03.22 // 2013.03.09 // 2013.03.09
							if ( mb == 99 ) { // 2004.03.22
								//-------------------------------------------------------------------------------
								if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
								//-------------------------------------------------------------------------------
								//
								if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 14 );
								}
								//
								//========================================================================================================================
								SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( tms , CHECKING_SIDE , SCH_Chamber , 2 );
								//========================================================================================================================
							}
							else if ( mb == 1 ) { // 2004.03.22
								if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
									if ( SCHEDULING_CHECK_TM_Another_Free_Status( CHECKING_SIDE , FALSE , TRUE , ch ) ) { // 2004.03.24
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 15 );
										}
										//
										//========================================================================================================================
										SCHEDULING_CHECK_Waiting_OtherBM_BM_Make_FM_SIDE( tms , CHECKING_SIDE , SCH_Chamber , 1 );
										//========================================================================================================================
									}
								}
								else { // 2007.02.15
									if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , -1 , FALSE ) ) {
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 152 );
										}
										//
									}
									else {
										//===========================================================================
										if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( tms , SCH_Chamber , TRUE ) ) {
											//-------------------------------------------------------------------------------
											if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
											//-------------------------------------------------------------------------------
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 153 );
											}
											//
										}
										//===========================================================================
									}
								}
							}
							else if ( mb == -1 ) { // 2008.05.13
								//=========================================================================================================================
								// 2008.05.13
								//=========================================================================================================================
								if ( SCHEDULING_CHECK_SWITCHING_MANY_BM_SLOT_AFTER_PICK_or_PLACE( tms , CHECKING_SIDE , ch ) ) { // 2008.05.13
									if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
										if ( SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , FALSE , -1 , TRUE ) ) {
											//-------------------------------------------------------------------------------
											if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
											//-------------------------------------------------------------------------------
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 154 );
											}
											//
											//-------------------------------------------------------------------------------
										}
									}
								}
								//=========================================================================================================================
							}
						}
						else if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_SINGLESWAP ) { // 2003.11.07 // 2013.03.09
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
								if ( _SCH_PRM_GET_MODULE_SIZE( ch ) == 1 ) { // 2007.10.04
									//-------------------------------------------------------------------------------
									if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
									//-------------------------------------------------------------------------------
									//
									if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 17 );
									}
									//
								}
								else {
									mb = SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP2( CHECKING_SIDE , ch , FALSE , tms );
									if ( mb == 1 ) {
										//-------------------------------------------------------------------------------
										if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
										//-------------------------------------------------------------------------------
										//-------------------------------------------------------------------------------
										//
										if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
											_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 16 );
										}
										//
									}
									else if ( mb > 0 ) {
										if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
											//-------------------------------------------------------------------------------
											// 2004.11.26
											//-------------------------------------------------------------------------------
											if ( tms == 0 ) SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( CHECKING_SIDE , tms , SCH_Chamber );
											//-------------------------------------------------------------------------------
											//-------------------------------------------------------------------------------
											//
											if ( _SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( 1 ) != 1 ) { // 2012.09.03
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 17 );
											}
											//
										}
									}
								}
							}
						}
//					} // 2013.03.18
				}
			}
			else {
				//
				if ( _SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() && ( tms != _SCH_PRM_GET_MODULE_GROUP( ch ) ) ) { // 2013.05.06
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , CHECKING_SIDE , pt );
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
						_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , BM_Side2 , pt2 ); // 2007.04.18
						_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
					}
					else if ( SCH_Slot > 0 ) {
						if ( ( SCH_Slot % 2 ) == 0 ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot - 1 , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot - 1 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
						}
						else {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot + 1 , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot + 1 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
						}
					}
					else if ( SCH_Slot2 > 0 ) {
						if ( ( SCH_Slot2 % 2 ) == 0 ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 - 1 , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 - 1 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
						else {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 + 1 , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 + 1 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
					}
				}
				else {
					if ( PathThruModeD ) { // 2006.12.21
						if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , BM_Side2 , pt2 ); // 2007.04.18
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
						else if ( SCH_Slot > 0 ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
						}
						else if ( SCH_Slot2 > 0 ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( ch , SCH_Slot2 , CHECKING_SIDE , pt );
							_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
					}
					else {
						if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE );
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot2 , BM_Side2 ); // 2007.04.18
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt2 );
							if ( _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) == PATHDO_WAFER_RETURN ) // 2007.04.18
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
						else if ( SCH_Slot > 0 ) {
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot , CHECKING_SIDE );
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot , pt );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 , BUFFER_INWAIT_STATUS );
						}
						else if ( SCH_Slot2 > 0 ) {
							_SCH_MODULE_Set_BM_SIDE( ch , SCH_Slot2 , CHECKING_SIDE );
							_SCH_MODULE_Set_BM_POINTER( ch , SCH_Slot2 , pt );
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN )
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
							else
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , SCH_Slot2 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 , BUFFER_INWAIT_STATUS );
						}
					}
				}
				//-----------------------------------------------------------------------
				_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
				//-----------------------------------------------------------------------
				if ( PathThruModeD ) { // 2006.12.21
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
						if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , TRUE , -1 , FALSE ) ) { // 2006.12.06 // 2013.03.09
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 9 );
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 1 );
						}
					}
				}
				else { // 2013.03.18
					if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( ch ) ) {
						if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || SCHEDULING_CHECK_Chk_FULL_ALL_BM_OR_NOMORE_AFTER_PLACE( CHECKING_SIDE , tms , ch , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , TRUE , -1 , FALSE ) ) { // 2013.03.09
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , ch , wsa , TRUE , tms+1 , 9 );
						}
					}
				}
				//-----------------------------------------------------------------------
				// 2013.03.26
//				SCHEDULER_Select_OneChamber_and_OtherBuffering_OVERTMS( CHECKING_SIDE , pt , 0 , tms ); // 2013.02.07
//				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
//					SCHEDULER_Select_OneChamber_and_OtherBuffering_OVERTMS( BM_Side2 , pt2 , 0 , tms ); // 2013.02.07
//				}
				//-----------------------------------------------------------------------
			}
			//-----------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( 0 ) != GROUP_HAS_SWITCH_NONE ) {
				_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_READY );
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 21 END_TYPE_12(PLACEtoBM) = Chamber(BM%d) finger(%d) pt(%d,%d) slot(%d,%d)\n" , tms+1 , ch - BM1 + 1 , SCH_No , pt , pt2 , SCH_Slot , SCH_Slot2 );
			//----------------------------------------------------------------------
			SCHEDULER_Set_TM_MidCount( tms , 0 );
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
		}
		else {
			_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
			//
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2008.06.10
				_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
				_SCH_MODULE_Inc_TM_InCount( BM_Side2 ); // 2008.06.10
			}
			else {
				_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); // 2008.06.10
			}
			//-----------------------------------------------------------------------
			SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( CHECKING_SIDE , -1 );
			//----------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( tms ) != GROUP_HAS_SWITCH_NONE ) {
				_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_READY );
			}
			//-----------------------------------------------------------------------
		}
		//====================================================================================================
		if ( tms == 0 ) { // 2013.02.07
			if ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) { // 2007.04.02
				if ( ( ch >= BM1 ) && ( SCH_JumpMode != 1 ) ) {
					if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
						if ( _SCH_MODULE_Chk_TM_Finish_Status( CHECKING_SIDE ) ) {
							_SCH_PREPOST_LotPost_Part_for_All_BM_Return( CHECKING_SIDE ); // 2007.04.06
						}
					}
				}
			}
		}
//
// 2013.10.31
*/
		//====================================================================================================
		if ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) {
			//
			_SCH_MODULE_Set_TM_Switch_Signal( tms , -1 );
			//
			if ( Result ) { // 2009.07.17
				if ( SCH_JumpMode == 1 ) {
					SCHEDULER_Set_TM_Pick_BM_Signal( tms , ch + 1000 );
				}
				else {
					SCHEDULER_Set_TM_Pick_BM_Signal( tms , ch );
				}
				return 2;
			}
		}
		//====================================================================================================
	}
	else { // Place to PM Start
		//-----------------------------------------------------------------------
		currpathdo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt );
		//-----------------------------------------------------------------------
//		if ( currpathdo == PATHDO_WAFER_RETURN ) return 0; // 2016.02.24
		if ( currpathdo == PATHDO_WAFER_RETURN ) return 101; // 2016.02.24
		//-----------------------------------------------------------------------
		ch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order );
		//-----------------------------------------------------------------------
		if ( !SCH_PM_HAS_ABSENT( ch , CHECKING_SIDE , pt , currpathdo - 1 , &pmslot ) ) return 0; // 2014.01.10
		//-------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( TRUE , CHECKING_SIDE , tms , SCH_No , &ch , &R_Retmode ); // 2007.11.09
		//-------------------------------------------------------------------------------------
//		if ( ch <= 0 ) return 0; // 2016.02.24
		if ( ch <= 0 ) return 102; // 2016.02.24
		//==============================================================================================================
		if ( _SCH_PRM_GET_Putting_Priority( ch ) < 0 ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) return 0;
		}
		//==============================================================================================================
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms,ch ) ) { // 2003.02.05
		case 1 :
			if ( SCH_No != TA_STATION ) ch = -1;
			break;
		case 2 :
			if ( SCH_No != TB_STATION ) ch = -1;
			break;
		}
//		if ( ch == -1 ) return 0; // 2016.02.24
		if ( ch == -1 ) return 103; // 2016.02.24
		//==============================================================================================================
		if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
			EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
			//==============================================================================
			if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != tms ) ) { // 2006.06.14
			//==============================================================================
				LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
//				return 0; // 2016.02.24
				return 104; // 2016.02.24
			}
//			if ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) ) { // 2014.01.10
			if ( ( _SCH_MODULE_Get_PM_WAFER( ch , pmslot ) > 0 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) ) { // 2014.01.10
				LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
//				return 0; // 2016.02.24
				return 105; // 2016.02.24
			}
			SCHEDULER_Set_MULTI_GROUP_PLACE( ch , tms );
			LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
			//==============================================================================================================
//			if ( _SCH_MODULE_Get_TM_Switch_Signal( tms ) != ch ) { // 2006.07.21 // 2013.03.15
//				SCHEDULING_CHECK_MULTI_PLACE_GROUP_RESET( tms , ch ); // 2006.06.14 // 2013.03.15
//			} // 2013.03.15
			//==============================================================================================================
		}
		//==============================================================================================================
		//-----------------------------------------------------------------------
		Sav_Int_3 = SCHEDULER_Get_PM_MidCount( tms );
		//-----------------------------------------------------------------------
		//-----------------------------------------------------------------------
		SCHEDULER_Set_PM_MidCount( tms , currpathdo ); // 2002.11.05
		//
		_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
		//-----------------------------------------------------------------------
		SCHEDULER_Set_TM_LastMove_Signal( tms , ch );
		//-----------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) ) { // 2013.03.09
			//
			BM_Side2 = _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ); // 2007.04.18
			pt2 = _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No );
			//
			if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) <= 0 ) {
				SCH_Slot = 0;
				SCH_Slot2 = pmslot + 2; // 2014.01.10
				//
				wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
				wsb = SCH_Slot + SCH_Slot2*100;
			}
			else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) <= 0 ) {
				SCH_Slot = pmslot + 1; // 2014.01.10
				SCH_Slot2 = 0;
				//
				wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
				wsb = SCH_Slot + SCH_Slot2*100;
			}
			else {
				SCH_Slot = pmslot + 1; // 2014.01.10
				SCH_Slot2 = pmslot + 2; // 2014.01.10
				//
				wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
				wsb = SCH_Slot + SCH_Slot2*100;
			}
			//============================================================================================================
			//
			// 2015.05.27
			//
			if ( Get_RunPrm_FM_PM_CONTROL_USE() ) {
				//
				switch ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) ) {
				case 1 :
					//
					//if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) > 0 ) return -1; // 2015.06.09
					//
					// 2015.06.09
					//
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) > 0 ) {
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_TM_SIDE( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER( tms,SCH_No ) , FALSE , FALSE , 10 ); // 2015.06.22
						//
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) > 0 ) {
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No ) , FALSE , FALSE , 11 ); // 2015.06.22
							//
						}
						//
//						return 0; // 2016.02.24
						return 106; // 2016.02.24
						//
					}
					//
					break;
				case 2 :
					//
					//if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) > 0 ) return -1; // 2015.06.09
					//
					// 2015.06.09
					//
					if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ) > 0 ) {
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_TM_SIDE( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER( tms,SCH_No ) , FALSE , FALSE , 12 ); // 2015.06.22
						//
						if ( ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) > 0 ) {
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( _SCH_MODULE_Get_TM_SIDE2( tms,SCH_No ) , _SCH_MODULE_Get_TM_POINTER2( tms,SCH_No ) , FALSE , FALSE , 13 ); // 2015.06.22
							//
						}
						//
//						return 0; // 2016.02.24
						return 107; // 2016.02.24
						//
					}
					//
					break;
				}
			}
			//============================================================================================================
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
				if      ( R_Retmode == 0 ) { // allreturn;
					//
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
					//
					SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 14 ); // 2016.02.19
					//
					_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
					//
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
						//
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( BM_Side2 , pt2 , -1 );
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( BM_Side2 , pt2 , FALSE , TRUE , 15 ); // 2016.02.19
						//
						_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
					}
//					return 0; // 2016.02.24
					return 111; // 2016.02.24
				}
				else if ( R_Retmode == 1 ) { // lowreturn;
					if ( SCH_Slot > 0 ) {
						SCH_Slot = 0;
						wsa = ( wsa / 100 ) * 100;
						wsb = ( wsb / 100 ) * 100;
						//
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
						//
						SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 16 ); // 2016.02.19
						//
						_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
						//
						if ( SCH_Slot2 > 0 ) {
							//=============================================================================================
							Checking_Side_Changed = CHECKING_SIDE; // 2008.04.08
							//=============================================================================================
							CHECKING_SIDE = BM_Side2;
							pt = pt2;
						}
					}
				}
				else if ( R_Retmode == 2 ) { // highreturn;
					if ( SCH_Slot2 > 0 ) {
						SCH_Slot2 = 0;
						wsa = wsa % 100;
						wsb = wsb % 100;
						if ( SCH_Slot > 0 ) {
							//
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( BM_Side2 , pt2 , -1 );
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( BM_Side2 , pt2 , FALSE , TRUE , 17 ); // 2016.02.19
							//
							_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
						}
						else {
							//
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 18 ); // 2016.02.19
							//
							_SCH_MODULE_Set_TM_TYPE( tms , SCH_No , SCHEDULER_MOVING_OUT );
						}
					}
				}
			}
		}
		else {
			pt2 = 0;
			SCH_Slot = pmslot+1; // 2014.01.10
			SCH_Slot2 = 0;
			wsa = _SCH_MODULE_Get_TM_WAFER( tms,SCH_No );
			wsb = SCH_Slot;
		}
		//-----------------------------------------------------------------------
		switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) ) {
		case -1 :
		case -3 :
		case -5 :
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
			//===============================================================================================================================
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
			//===============================================================================================================================
			return -1;
			break;
		}
		//
		// 2015.11.27
		//
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() == 3 ) {
			//
			if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
				//
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + 1 ) ) {
				case -1 :
				case -3 :
				case -5 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted S1 Because %s Abort%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
					//===============================================================================================================================
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
					//===============================================================================================================================
					return -1;
					break;
				}
				//
			}
			else {
				//
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch - 1 ) ) {
				case -1 :
				case -3 :
				case -5 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted S2 Because %s Abort%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
					//===============================================================================================================================
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
					//===============================================================================================================================
					return -1;
					break;
				}
				//
			}
		}
		//
		//-----------------------------------------------------------------------
		// User Interface
		// 2015.05.29
		//-----------------------------------------------------------------------
		switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PLACE , 101 , CHECKING_SIDE , pt , ch , currpathdo - 1 , SCH_Order , tms , SCH_No ) ) {
		case 0 :
			//===============================================================================================================================
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s USER_FLOW_NOTIFICATION 101 Fail%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
			//===============================================================================================================================
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
			//===============================================================================================================================
			return -1;
			break;
		case 1 :
			//===============================================================================================================================
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
			//===============================================================================================================================
			if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
			//===============================================================================================================================
//			return 0; // 2016.02.24
			return 121; // 2016.02.24
			break;
		}
		//-----------------------------------------------------------------------
		// Pre Process Part Begin
		//-----------------------------------------------------------------------
		if ( SCH_PM_IS_EMPTY( ch ) ) { // 2014.01.10
			//
//			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2015.10.27
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() >= 2 ) { // 2015.10.27
				if      ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2008.03.24
					j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , SCH_No , ch );
					if      ( ( j == 0 ) || ( j == 1 ) ) {
						//-----------------------------------------------------------------------
						j = _SCH_MACRO_PRE_PROCESS_OPERATION( BM_Side2 , pt2 , ( tms * 100 ) + SCH_No , ch + 1 );
						if ( j != -1 ) {
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//=================================================================================================
							if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
							//=================================================================================================
//							return 0; // 2016.02.24
							return 131; // 2016.02.24
						}
					}
				}
				else if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 <= 0 ) ) { // 2008.03.24
					j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + SCH_No , ch );
					if      ( ( j == 0 ) || ( j == 1 ) ) {
						//===============================================================================================================================
						if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
						//=================================================================================================
						if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
						//=================================================================================================
//						return 0; // 2016.02.24
						return 132; // 2016.02.24
					}
				}
				else if ( ( SCH_Slot <= 0 ) && ( SCH_Slot2 > 0 ) ) { // 2008.03.24
					j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + SCH_No , ch + 1 );
					if      ( ( j == 0 ) || ( j == 1 ) ) {
						//===============================================================================================================================
						if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
						//===============================================================================================================================
						//=================================================================================================
						// 2008.04.08
						//=================================================================================================
						if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
						//=================================================================================================
//						return 0; // 2016.02.24
						return 133; // 2016.02.24
					}
				}
			}
			else {
				j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + SCH_No , ch );
				if      ( ( j == 0 ) || ( j == 1 ) ) {
					//===============================================================================================================================
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
					//===============================================================================================================================
					if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
					//=================================================================================================
//					return 0; // 2016.02.24
					return 134; // 2016.02.24
				}
			}
			if ( j == -1 ) { // 2008.03.24
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) && ( CHECKING_SIDE != BM_Side2 ) ) { // 2007.04.19
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Place Fail to %s(%d)[F%c]%cWHTM%dPLACEFAIL PM%d:%d:%d:%c\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt + 1 , SCH_No + 'A' , 9 , tms + 1 , ch - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' );
					_SCH_LOG_LOT_PRINTF( BM_Side2      , "TM%d Handling Place Fail to %s(%d)[F%c]%cWHTM%dPLACEFAIL PM%d:%d:%d:%c\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt + 1 , SCH_No + 'A' , 9 , tms + 1 , ch - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' );
				}
				else {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Place Fail to %s(%d)[F%c]%cWHTM%dPLACEFAIL PM%d:%d:%d:%c\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt + 1 , SCH_No + 'A' , 9 , tms + 1 , ch - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' );
				}
				//===============================================================================================================================
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
				//===============================================================================================================================
				return -1;
			}
		}
		//-----------------------------------------------------------------------
		// End
		//-----------------------------------------------------------------------
		SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_BEFORE_PLACE_TO_OTHER_BPM( tms , SCH_No , FALSE , ch ); // 2008.06.21
		//-----------------------------------------------------------------------
		Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
		//-----------------------------------------------------------------------
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING ); // Move Here // 2002.07.23
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
			//-----------------------------------------------------------------------
			Sav_Int_2 = _SCH_CLUSTER_Get_PathStatus( BM_Side2 , pt2 ); // 2007.04.18
			//-----------------------------------------------------------------------
			_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_RUNNING ); // Move Here // 2002.07.23 // 2007.04.18
		}
		//-----------------------------------------------------------------------
		//-------------------------
		swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( SCH_SWITCH , 0 ); // 2007.07.25
		//-------------------------
		Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , wsb , 0 , -1 , FALSE , swmode , BM_Side2 , pt2 );
		//-----------------------------------------------------------------------
		MODULE_LAST_SWITCH_0[ch] = 0; // 2010.05.21
		//-----------------------------------------------------------------------
		if ( Function_Result == SYS_ABORTED ) { // 2002.07.18 // 2005.11.25
			//===============================================================================================================================
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
			//===============================================================================================================================
			return -1;
		}
		//===========================================================================================================
		// 2005.11.25
		//===========================================================================================================
		else if ( Function_Result == SYS_ERROR ) {
			_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
				_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_2 ); // 2007.04.18
			}
			SCHEDULER_Set_PM_MidCount( tms , Sav_Int_3 );
			//===============================================================================================================================
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
				if ( !SCHEDULER_CONTROL_PLACE_PICK_ACTION_WITH_ERROR_A0SUB4( tms , CHECKING_SIDE , pt , ch , SCH_No , wsa , wsb , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) ) ) {
					if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
					return -1;
				}
			}
			//===============================================================================================================================
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
			//===============================================================================================================================
			//=================================================================================================
			// 2008.04.08
			//=================================================================================================
			if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
			//=================================================================================================
			return 0;
		}
		//===========================================================================================================
		//-----------------------------------------------------------------------
		// 2002.05.28
		//-----------------------------------------------------------------------
		if ( _SCH_CLUSTER_Check_HasProcessData_Post( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order ) ) {
			if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 1 ) {
				_SCH_CLUSTER_Set_PathProcessData_SkipPost( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order );
				_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , ch ); // 2006.02.10
			}
			else { // 2006.02.10
				if ( post_time >= 0 ) {
					_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , ch , post_time );
				}
			}
			if ( prcs_time >= 0 ) { // 2006.02.13
				_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , ch , prcs_time );
			}
		}
		//-----------------------------------------------------------------------
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 ); // 2002.07.23
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2002.07.23 // 2007.04.18
			_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_RUNNING2 ); // 2002.07.23 // 2007.04.18
		}
		//-----------------------------------------------------------------------
		SCHEDULER_CONTROL_PLACE_PICK_REPORT_WITH_ERROR_A0SUB4( tms , ch , CHECKING_SIDE , pt , SCH_No , SCH_Order ); // 2007.01.03
		//==============================================================================================================
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
			//===============================================================================================
			EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[ch] );
			//===============================================================================================
			SCHEDULER_Set_MULTI_GROUP_PICK( ch , tms );
			//===============================================================================================
			//===============================================================================================
			SCHEDULING_CHECK_REMAKE_GROUPPING_FLOWDATA( tms , SCH_No , CHECKING_SIDE , pt , ch ); // 2006.03.27
			//===============================================================================================
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
//				_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , BM_Side2 , pt2 ); // 2014.01.10
				_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
				_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
				_SCH_MODULE_Set_PM_SIDE( ch , pmslot+1 , BM_Side2 ); // 2014.01.10
				_SCH_MODULE_Set_PM_POINTER( ch , pmslot+1 , pt2 ); // 2014.01.10
			}
			else if ( SCH_Slot > 0 ) {
//				_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
				_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
				_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
			}
			else if ( SCH_Slot2 > 0 ) {
//				_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
				_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
				_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
			}
//			_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
			_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
			//
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) ); // 2014.01.10
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
				}
				else if ( SCH_Slot > 0 ) {
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ); // 2014.01.10
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , ( _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) / 100 ) * 100 );
				}
				else if ( SCH_Slot2 > 0 ) {
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) * 100 ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) * 100 ); // 2014.01.10
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , _SCH_MODULE_Get_TM_WAFER( tms , SCH_No ) % 100 );
				}
			}
			else {
//				_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
				_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
				_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
			}
			//
			_SCH_MODULE_Set_PM_PICKLOCK( ch , pmslot , 1 ); // 2014.01.10
			//
			//==============================================================================================================
			if ( SCH_PM_IS_ALL_PLACED_PROCESSING( ch , TRUE , FALSE , &multiprcs ) ) { // 2014.01.10
				//
				if ( multiprcs ) {
					//
					if ( !SCH_PM_GET_FIRST_SIDE_PT( ch , &m_f_side , &m_f_pt ) ) {
						m_f_side = CHECKING_SIDE;
						m_f_pt = pt;
					}
					//
					_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( m_f_side , m_f_pt , -1 , ch , ( tms * 100 ) + SCH_No , -1 , 1103 );
					//
				}
				else {
					//
//					if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { // 2007.10.07 // 2015.10.27
					if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() >= 2 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { // 2007.10.07 // 2015.10.27
						if ( SCH_Slot > 0 ) {
							_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
							_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
								  ch , -1 ,
								  wsa % 100 , -1 , ( tms * 100 ) + SCH_No , -1 ,
								  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
								  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
								  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
								  -1 , -1 , -1 , 0 ,
								  4 , 1104 );
						}
						else {
							BM_Side2 = CHECKING_SIDE;
							pt2 = pt;
						}
						if ( SCH_Slot2 > 0 ) {
							SCH_Order2 = _SCH_CLUSTER_Get_PathOrder( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , ch + 1 );
							_SCH_CLUSTER_Get_Next_PM_String( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 + 1 , NextPM , 255 ); // 2002.07.16
							_SCH_MACRO_MAIN_PROCESS( BM_Side2 , pt2 ,
								  ch + 1 , -1 ,
								  wsa / 100 , -1 , ( tms * 100 ) + SCH_No , -1 ,
								  _SCH_CLUSTER_Get_PathProcessRecipe( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , SCH_Order2 , 0 ) ,
								  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
								  _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , SCH_Order2 , ( _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) >= _SCH_CLUSTER_Get_PathRange( BM_Side2 , pt2 ) ) ,
								  -1 , -1 , -1 , 0 ,
								  4 , 1114 );
						}
					}
					//==============================================================================================================
					else {
						_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
						_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
							  ch , -1 ,
							  wsa , -1 , ( tms * 100 ) + SCH_No , -1 ,
							  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
							  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
							  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
							  BM_Side2 , pt2 , -1 , 0 ,
							  0 , 104 );
					}
				}
			}
			//===============================================================================================
			SCHEDULER_Set_MULTI_GROUP_PICK( ch , -1 );
			//===============================================================================================
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[ch] );
			//===============================================================================================
			if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
				if ( !SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( tms , CHECKING_SIDE , pt , 1 ) ) { // 2006.04.11
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
				}
			}
		}
		else {
			//=============================================================
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
				if      ( R_Retmode == 0 ) { // allreturn;
				}
				else if ( R_Retmode == 1 ) { // lowreturn;
//					_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//					_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) * 100 ); // 2014.01.10
					//
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
					_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) * 100 ); // 2014.01.10
					//
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 );
				}
				else if ( R_Retmode == 2 ) { // highreturn;
//					_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//					_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ); // 2014.01.10
					//
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
					_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) % 100 ); // 2014.01.10
					//
					_SCH_MODULE_Set_TM_SIDE_POINTER( tms , SCH_No , BM_Side2 , pt2 , CHECKING_SIDE , 0 );
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , ( _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) / 100 ) * 100 );
				}
				else {
//					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
//						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , BM_Side2 , pt2 ); // 2014.01.10
//					else if ( SCH_Slot > 0 )
//						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//					else if ( SCH_Slot2 > 0 )
//						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//					_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//					_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
//
					if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2014.01.10
						_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
						_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
						_SCH_MODULE_Set_PM_SIDE( ch , pmslot+1 , BM_Side2 ); // 2014.01.10
						_SCH_MODULE_Set_PM_POINTER( ch , pmslot+1 , pt2 ); // 2014.01.10
					}
					else if ( SCH_Slot > 0 ) { // 2014.01.10
						_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
						_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
					}
					else if ( SCH_Slot2 > 0 ) { // 2014.01.10
						_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
						_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
					}
					_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
					_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
					//
					_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
				}
				//============================================================================================================
			}
			else {
//				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
//					_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , BM_Side2 , pt2 ); // 2014.01.10
//				else if ( SCH_Slot > 0 )
//					_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//				else if ( SCH_Slot2 > 0 )
//					_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//				_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//				_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
				//
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2014.01.10
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot+1 , BM_Side2 ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot+1 , pt2 ); // 2014.01.10
				}
				else if ( SCH_Slot > 0 ) { // 2014.01.10
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
				}
				else if ( SCH_Slot2 > 0 ) { // 2014.01.10
					_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
					_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
				}
				_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
				_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,SCH_No ) ); // 2014.01.10
				//
				_SCH_MODULE_Set_TM_WAFER( tms , SCH_No , 0 );
				//
			}
			//
			_SCH_MODULE_Set_PM_PICKLOCK( ch , pmslot , 1 ); // 2014.01.10
			//
			//==============================================================================================================
			if ( SCH_PM_IS_ALL_PLACED_PROCESSING( ch , TRUE , FALSE , &multiprcs ) ) { // 2014.01.10
				//
				if ( multiprcs ) {
					//
					if ( !SCH_PM_GET_FIRST_SIDE_PT( ch , &m_f_side , &m_f_pt ) ) {
						m_f_side = CHECKING_SIDE;
						m_f_pt = pt;
					}
					//
					_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( m_f_side , m_f_pt , -1 , ch , ( tms * 100 ) + SCH_No , -1 , 1106 );
					//
				}
				else {
					//
//					if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { // 2007.10.07 // 2015.10.27
					if ( ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() >= 2 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { // 2007.10.07 // 2015.10.27
						if ( SCH_Slot > 0 ) {
							_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
							_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
								  ch , -1 ,
								  wsa % 100 , -1 , ( tms * 100 ) + SCH_No , -1 ,
								  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
								  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
								  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
								  -1 , -1 , -1 , 0 ,
								  4 , 1105 );
						}
						else {
							BM_Side2 = CHECKING_SIDE;
							pt2 = pt;
						}
						if ( SCH_Slot2 > 0 ) {
							SCH_Order2 = _SCH_CLUSTER_Get_PathOrder( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , ch + 1 );
							_SCH_CLUSTER_Get_Next_PM_String( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 + 1 , NextPM , 255 ); // 2002.07.16
							_SCH_MACRO_MAIN_PROCESS( BM_Side2 , pt2 ,
								  ch + 1 , -1 ,
								  wsa / 100 , -1 , ( tms * 100 ) + SCH_No , -1 ,
								  _SCH_CLUSTER_Get_PathProcessRecipe( BM_Side2 , pt2 , _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , SCH_Order2 , 0 ) ,
								  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
								  _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) - 1 , SCH_Order2 , ( _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ) >= _SCH_CLUSTER_Get_PathRange( BM_Side2 , pt2 ) ) ,
								  -1 , -1 , -1 , 0 ,
								  4 , 1115 );
						}
					}
					//==============================================================================================================
					else {
						_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
						_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
							  ch , -1 ,
							  wsa , -1 , ( tms * 100 ) + SCH_No , -1 ,
							  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
							  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
							  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
							  BM_Side2 , pt2 , -1 , 0 ,
							  0 , 105 );
					}
				}
			}
			//=============================================================
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
				if      ( R_Retmode == 0 ) { // allreturn;
					//=================================================================================================
					// 2008.04.08
					//=================================================================================================
					if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
					//=================================================================================================
					return 0;
				}
				else if ( R_Retmode == 1 ) { // lowreturn;
				}
				else if ( R_Retmode == 2 ) { // highreturn;
				}
				else {
				}
				//============================================================================================================
			}
			else {
			}
			//==============================================================================================================
			if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
				if ( !SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( tms , CHECKING_SIDE , pt , 1 ) ) { // 2006.04.11
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
				}
			}
			//===============================================================================================
			if ( SCHEDULER_CONTROL_Next_All_Stock_and_Nothing_to_Go( tms , CHECKING_SIDE , pt , ch ) ) {
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
			}
			//===============================================================================================
		}
		//==============================================================================================================
		_SCH_CLUSTER_After_Place_For_Parallel_Resetting( CHECKING_SIDE , pt , currpathdo - 1 , ch );
		//==============================================================================================================
		SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( CHECKING_SIDE , ch );
		//===============================================================================================================================
		if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
		//===============================================================================================================================
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 22 END_TYPE_12(PLACEtoPM) = Chamber(PM%d) finger(%d) pt(%d,%d) slot(%d,%d)\n" , tms + 1 , ch - PM1 + 1 , SCH_No , pt , pt2 , SCH_Slot , SCH_Slot2 );
		//----------------------------------------------------------------------
		if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
		//=================================================================================================
	}
	//----------------------------------------------------------
	*Run_Doing = TRUE;
	//----------------------------------------------------------
	return 0;
}



int SCHEDULER_RUN_PLACE_TO_BMPM( int tms , int CHECKING_SIDE , int SCH_No , int SCH_Chamber , int SCH_Slot , int SCH_Type , int SCH_RETMODE , int SCH_JumpMode , int SCH_Order , int SCH_SWITCH , BOOL *Run_Doing ) { // 2013.02.07
	int Res , tmdata;
	//
	tmdata = _SCH_PRM_GET_MODULE_TMS_GROUP( SCH_Chamber );
	//
	if ( tmdata >= 0 ) {
		//
		EnterCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
		if ( SCHEDULER_Get_MULTI_GROUP_TMS( tmdata ) ) {
			//
//			_IO_CIM_PRINTF( "MULTI_GROUP_TMS[%d] LOCKING(%d) REJECT at %s in SCHEDULER_RUN_PLACE_TO_BMPM\n" , tms + 1 , tmdata , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( SCH_Chamber ) );
			//
			LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
			return 0;
		}
		SCHEDULER_Set_MULTI_GROUP_TMS( tmdata );
		LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
	}
	//
	Res = SCHEDULER_RUN_PLACE_TO_BMPM_SUB( tms , CHECKING_SIDE , SCH_No , SCH_Chamber , SCH_Slot , SCH_Type , SCH_RETMODE , SCH_JumpMode , SCH_Order , SCH_SWITCH , Run_Doing );
	//
	//
	if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
		if ( Res >= 100 ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with RUN_PLACE_TO_BMPM[%d]=> PM%d,SCH_No=%d,SCH_Slot=%d,SCH_Type=%d,SCH_RETMODE=%d,SCH_JumpMode=%d,SCH_Order=%d,SCH_SWITCH=%d\t\t\n" , Res , SCH_Chamber - PM1 + 1 , SCH_No , SCH_Slot , SCH_Type , SCH_RETMODE , SCH_JumpMode , SCH_Order , SCH_SWITCH );
			Res = 0;
		}
	}
	//
	if ( tmdata >= 0 ) SCHEDULER_ReSet_MULTI_GROUP_TMS( tmdata );
	//
	return Res;
}


int SCHEDULER_CHECK_SUB_ALG6_FORCE_B_ARM( int tms , int ch , int pmslot ); // 2018.07.12


BOOL Multi_Expect_Pick_Slot_for_Place( int tms , int ch , int *pmslot ) { // 2018.12.06
	int s , p , d , i , k;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( tms , i );
		p = _SCH_MODULE_Get_TM_POINTER( tms , i );
		d = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
		//
		if ( d >= _SCH_CLUSTER_Get_PathRange( s , p ) ) continue;
		//
		if ( _SCH_CLUSTER_Get_PathProcessDepth( s , p , d ) == NULL ) continue;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( ch == _SCH_CLUSTER_Get_PathProcessChamber( s , p , d , k ) ) break;
			//
		}
		//
		if ( k == MAX_PM_CHAMBER_DEPTH ) continue;
		//
		k = _SCH_CLUSTER_Get_PathProcessDepth2( s , p , d , (ch - PM1) );
		//
		if ( k > 0 ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , k - 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_PICKLOCK( ch , k - 1 ) <= 0 ) {
					//
					*pmslot = k - 1;
					//
					return TRUE;
					//
				}
			}
		}
	}
	//
	return FALSE;
	//
}


int SCHEDULER_RUN_PICK_FROM_PM_SUB( int tms , int CHECKING_SIDE , int SCH_W_Prcs , int SCH_No , int SCH_Type , int SCH_NextFinger , int SCH_Order , int SCH_Chamber , int SCH_Switchable , int SCH_Halfpick , int SCH_EndMode , int SCH_SWITCH , int *skip_p1 , int *skip_p2 , BOOL *Run_Doing ) { // 2013.02.07
	int Checking_Side_Changed , pt , Result , cf , swmode , Function_Result , currpathdo , prcs_time , post_time;
	int SCH_Out_Not_Run , SCH_Out_Th_Check;
	int R_Next_Move , R_Path_Order , R_Next_Chamber , R_Wait_Process , R_EndMode , R_WY , R_NextFinger , R_Halfpick;
	int Sav_Int_1 , Sav_Int_2 , Sav_Int_3 , Sav_Int_4;
	int BM_Side2 , pt2 , SCH_Slot , SCH_Slot2 , wsa , wsb;
	char NextPM[256];
	int pmslot , m_f_side , m_f_pt;
	BOOL multiprcs;
	BOOL switchX;

	//========================================================================================================================================================
	if ( SCH_W_Prcs ) return 0;
	//==============================================================================================================
	if ( _SCH_PRM_GET_Getting_Priority( SCH_No ) < 0 ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) > 0 ) return 0;
	}
	//==============================================================================================================
	if ( !Multi_Expect_Pick_Slot_for_Place( tms , SCH_No , &pmslot ) ) { // 2018.12.06
		if ( !SCH_PM_HAS_PICKING( SCH_No , &pmslot ) ) return 0; // 2014.01.10
	}
	//==============================================================================================================
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
		EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
		if ( SCHEDULER_Get_MULTI_GROUP_PICK( SCH_No ) != -1 ) {
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
			if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2013.03.11
			if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2013.03.11
			return 0;
		}
//		if ( _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) <= 0 ) { // 2014.01.10
		if ( _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) <= 0 ) { // 2014.01.10
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
			if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2013.03.11
			if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2013.03.11
			return 0;
		}
		SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , tms );
		LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
	}
	//==============================================================================================================
	SCH_Out_Not_Run = 0;
	SCH_Out_Th_Check = 0;
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 11 RUN_TYPE_0(PICKfromPM) = Chamber(PM%d)[PM%d][%d][%d][%d]\n" , tms+1 , SCH_No - PM1 + 1 , SCH_Chamber - PM1 + 1 , SCH_Type , SCH_NextFinger , SCH_Switchable ); // 2013.01.05
	//----------------------------------------------------------------------
	//-----------------------------------------------------------------------
//	pt = _SCH_MODULE_Get_PM_POINTER( SCH_No , 0 ); // 2014.01.10
	pt = _SCH_MODULE_Get_PM_POINTER( SCH_No , pmslot ); // 2014.01.10
	//
	switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) ) {
	case -1 : // All Stop
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 );
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
		return -1;
		break;
	case -2 : // Cluster - Continue , Process - Continue
		break;
	case -3 : // // Cluster - Continue , Process - Disable
		SCH_Out_Not_Run = 1;
		break;
	case -4 : // Cluster - Out , Process - Continue
		//-----------------------
		SCH_Type = SCHEDULER_MOVING_OUT;
		//-----------------------
		break;
	case -5 : // Cluster - Out , Process - Disable
		//-----------------------
		SCH_Type = SCHEDULER_MOVING_OUT;
		//-----------------------
		SCH_Out_Not_Run = 1;
		break;
	}
	//
	// 2015.11.27
	//
	if ( _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() == 3 ) {
		//
		if ( ( ( SCH_No - PM1 ) % 2 ) == 0 ) {
			//
			switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No + 1 ) ) {
			case -1 : // All Stop
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted S1 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 );
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
				return -1;
				break;
			case -2 : // Cluster - Continue , Process - Continue
				break;
			case -3 : // // Cluster - Continue , Process - Disable
				SCH_Out_Not_Run = 1;
				break;
			case -4 : // Cluster - Out , Process - Continue
				//-----------------------
				SCH_Type = SCHEDULER_MOVING_OUT;
				//-----------------------
				break;
			case -5 : // Cluster - Out , Process - Disable
				//-----------------------
				SCH_Type = SCHEDULER_MOVING_OUT;
				//-----------------------
				SCH_Out_Not_Run = 1;
				break;
			}
			//
		}
		else {
			//
			switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No - 1 ) ) {
			case -1 : // All Stop
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted S2 Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 );
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
				return -1;
				break;
			case -2 : // Cluster - Continue , Process - Continue
				break;
			case -3 : // // Cluster - Continue , Process - Disable
				SCH_Out_Not_Run = 1;
				break;
			case -4 : // Cluster - Out , Process - Continue
				//-----------------------
				SCH_Type = SCHEDULER_MOVING_OUT;
				//-----------------------
				break;
			case -5 : // Cluster - Out , Process - Disable
				//-----------------------
				SCH_Type = SCHEDULER_MOVING_OUT;
				//-----------------------
				SCH_Out_Not_Run = 1;
				break;
			}
			//
		}
	}
	//
	//-----------------------------------------------------------------------
	// 2014.01.22
	//
	switchX = FALSE;
	//
	switch( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) ) {
	case 3 :
	case 4 :
	case 5 :
	case 6 :
	case 7 :
		if ( ( _SCH_PRM_GET_STOP_PROCESS_SCENARIO( SCH_No ) % 2 ) == 1 ) {
			//-----------------------
			switchX = TRUE;
			//-----------------------
			SCH_Type = SCHEDULER_MOVING_OUT;
			//-----------------------
		}
		break;
	}
	//-----------------------------------------------------------------------
	if ( SCH_Type == SCHEDULER_MOVING_OUT ) {
		if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
			SCH_Out_Th_Check = 1;
			EnterCriticalSection( &CR_SINGLEBM_STYLE_0 );
			SCHEDULER_Set_TM_MidCount( tms , 11 );
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
				if ( SCHEDULER_Get_MULTI_GROUP_PICK( SCH_No ) == -1 ) {
					EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] ); // 2006.02.08
					Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( tms , CHECKING_SIDE , SCH_No , pmslot , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , &R_EndMode , &R_WY , &R_NextFinger , &R_Halfpick , 0 );
					LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] ); // 2006.02.08
				}
				else {
					Result = -9999;
				}
			}
			else {
				Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( tms , CHECKING_SIDE , SCH_No , pmslot , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( CHECKING_SIDE ) , &R_EndMode , &R_WY , &R_NextFinger , &R_Halfpick , 0 );
			}
			if ( Result < 0 ) {
				SCHEDULER_Set_TM_MidCount( tms , 0 );
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
				LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
				return 0;
			}
		}
	}
	//-----------------------------------------------------------------------
	// 2015.08.18 (Move Here)
	switch ( _SCH_CLUSTER_Get_FailOut( CHECKING_SIDE , pt ) ) {
	case 2 : // All
		SCH_Type = SCHEDULER_MOVING_OUT;
		SCH_Out_Not_Run = 1;
		break;
	case 3 : // Out
		SCH_Type = SCHEDULER_MOVING_OUT;
		break;
	}
	//
	//============================================================================================================================
	//
	// 2018.06.19
	//
	cf = -2;
	//
	switch( SCHEDULER_CHECK_SUB_ALG6_FORCE_B_ARM( tms , SCH_No , pmslot ) ) { // 2018.07.12
	case -1 :
		cf = -1;
		break;
	case 1 :
		cf = TB_STATION;
		break;
	}
	//

//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	A	[%d,%d] [%d] [%d] [%d] [%d] (%d,%d)\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf , _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) , _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) );

	//============================================================================================================================
	//
	if ( cf == -2 ) { // 2018.06.19
		//
		if ( SCH_NextFinger != -1 ) {
			cf = SCH_NextFinger;
		}
		else {
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) { // 2015.08.27
				//
				switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_No ) ) { // 2003.02.05
				case 0 :
					//
					//cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_No ); // 2015.08.20
					//
					// 2015.08.20
					//
					if ( SCH_Type != SCHEDULER_MOVING_OUT ) {
						cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_No );


//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	B	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );


					}
					else {
						switch( SCHEDULER_INTERLOCK_ALL_BM_PICK_DENY_FOR_MDL( tms , CHECKING_SIDE ) ) {
						case 0 :
							cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_No );
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	B	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
							break;
						case 1 :
							cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	C	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
							if ( cf == -1 ) cf = TA_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	D	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
							break;
						case 2 :
							cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	E	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
							if ( cf == -1 ) cf = TA_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	F	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
							break;
						}
						//
					}
					//
					break;
				case 1 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	G	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
					break;
				case 2 :
					cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	H	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
					break;
				} // 2003.02.05
			}
			else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) ) { // 2015.08.27
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	I	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
			}
			else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) { // 2015.08.27
				cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	J	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
			}
			else { // 2015.08.27
				cf = -1;
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	K	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );
			}
			//
		}
		//
	}
	//
//printf( "SCHEDULER_RUN_PICK_FROM_PM_SUB	Z	[%d,%d] [%d] [%d] [%d] [%d]\n" , CHECKING_SIDE , pt , SCH_NextFinger , SCH_No , pmslot , cf );

	//
	if ( cf == -1 ) {
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
		return 0;
	}
	//============================================================================================================================
	/*
	// 2015.08.18
	switch ( _SCH_CLUSTER_Get_FailOut( CHECKING_SIDE , pt ) ) {
	case 2 : // All
		SCH_Type = SCHEDULER_MOVING_OUT;
		SCH_Out_Not_Run = 1;
		break;
	case 3 : // Out
		SCH_Type = SCHEDULER_MOVING_OUT;
		break;
	}
	*/
	//
	if ( switchX ) { // 2014.01.28
		Result = 0;
	}
	else {
		if ( ( SCH_Out_Not_Run != 0 ) || ( _SCH_CLUSTER_Check_Possible_UsedPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , SCH_No ) ) ) {
			Result = 0;
		}
		else {
			if ( !_SCH_MODULE_GET_USE_ENABLE( SCH_No , FALSE , CHECKING_SIDE ) ) {
				Result = 0;
			}
			else {
				Result = SCHEDULING_CHECK_Switch_PLACE_From_TM( tms , CHECKING_SIDE , cf , SCH_No );
			}
		}
	}
	//-----------------------------------------------------------------------
	if ( SCH_Switchable ) {
		if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
			EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
			if ( SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_No ) != -1 ) {
				LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
				if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2013.03.11
				if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2013.03.11
				return 0;
			}
			SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , tms );
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Data Protected2 to %s%cWHTM%dPICKPROTECT2 PM%d%c\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , tms+1 , SCH_No - PM1 + 1 , 9 );
			LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
		}
		else {
			SCH_Switchable = FALSE;
		}
		SCH_Chamber = -1;
	}
	else {
		//
		if ( ( SCH_Type != SCHEDULER_MOVING_OUT ) && ( SCH_Chamber != -1 ) ) {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_Chamber ) >= 2 ) {
				EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_Chamber] );
				//
//				if ( SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_Chamber ) != -1 ) { // 2013.03.15
				if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_Chamber ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_Chamber ) != tms ) ) { // 2013.03.15
					//
					if ( Result != 1 ) { // 2013.03.15
						//
						LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_Chamber] );
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
						if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2013.03.11
						if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2013.03.11
						return 0;
						//
					}
					//
				}
				else {
					if ( Result != 1 ) { // 2013.01.07
						//
						if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( tms ) <= 1 ) {
							//
//							if ( _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 ) > 0 ) { // 2007.01.16 // 2014.01.10
							if ( SCH_PM_IS_PICKING( SCH_Chamber ) ) { // 2014.01.10
								//
								LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_Chamber] );
								if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
								if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2013.03.11
								if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2013.03.11
								return 0;
							}
						}
						//
					}
					//
					SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , tms ); // 2013.03.15
					//
				}
//				SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , tms ); // 2013.03.15
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Data Protected to %s%cWHTM%dPICKPROTECT PM%d:%d%c\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , tms+1 , SCH_Chamber - PM1 + 1 , SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_Chamber ) , 9 );
				LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_Chamber] );
			}
			else {
				//
				SCH_Chamber = -1;
				//
			}
			//
			/*
			if ( ( Result == 1 ) && ( SCH_Chamber != SCH_No ) ) { // 2013.03.11
				if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
					EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
					if ( SCHEDULER_Get_MULTI_GROUP_PLACE( SCH_No ) != -1 ) {
						SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , tms );
					}
					LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
				}
			}
			*/
			//
		}
		//
		if ( Result == 1 ) { // 2013.03.16
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
				EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
				SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , tms );
				LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[SCH_No] );
			}
		}
		//
	}
	//============================================================================================================
	_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
	SCHEDULER_Set_TM_LastMove_Signal( tms , -1 );
	//-----------------------------------------------------------------------
	if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) ) { // 2013.03.09
		//
//		BM_Side2 = _SCH_MODULE_Get_PM_SIDE( SCH_No , 1 ); // 2007.04.18 // 2014.01.10
//		pt2 = _SCH_MODULE_Get_PM_POINTER( SCH_No , 1 ); // 2014.01.10
		//
		BM_Side2 = _SCH_MODULE_Get_PM_SIDE( SCH_No , pmslot+1 ); // 2007.04.18 // 2014.01.10
		pt2 = _SCH_MODULE_Get_PM_POINTER( SCH_No , pmslot+1 ); // 2014.01.10
		//
//		if ( ( _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) % 100 ) <= 0 ) { // 2014.01.10
		if ( ( _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) % 100 ) <= 0 ) { // 2014.01.10
			//
			SCH_Slot = 0;
//			SCH_Slot2 = 2; // 2014.01.10
			SCH_Slot2 = pmslot + 2; // 2014.01.10
			//
//			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ); // 2014.01.10
			wsb = SCH_Slot + SCH_Slot2*100;
		}
//		else if ( ( _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) / 100 ) <= 0 ) { // 2014.01.10
		else if ( ( _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) / 100 ) <= 0 ) { // 2014.01.10
			//
//			SCH_Slot = 1; // 2014.01.10
			SCH_Slot = pmslot + 1; // 2014.01.10
			SCH_Slot2 = 0;
			//
//			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ); // 2014.01.10
			wsb = SCH_Slot + SCH_Slot2*100;
		}
		else {
			//
//			SCH_Slot = 1; // 2014.01.10
//			SCH_Slot2 = 2; // 2014.01.10
			SCH_Slot = pmslot + 1; // 2014.01.10
			SCH_Slot2 = pmslot + 2; // 2014.01.10
			//
//			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ); // 2014.01.10
			wsb = SCH_Slot + SCH_Slot2*100;
		}
	}
	else {
		BM_Side2 = CHECKING_SIDE; // 2007.04.18
		pt2 = 0;
//		SCH_Slot = 1; // 2014.01.10
		SCH_Slot = pmslot + 1; // 2014.01.10
		SCH_Slot2 = 0;
//		wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
		wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ); // 2014.01.10
		wsb = SCH_Slot;
	}
	//-----------------------------------------------------------------------
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
		if ( SCH_Halfpick ) {
			if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
				SCH_Slot2 = 0;
				wsa = wsa % 100;
				wsb = wsb % 100;
			}
			else {
				SCH_Halfpick = FALSE;
			}
		}
	}
	//
	//-----------------------------------------------------------------------
	// User Interface
	// 2016.04.19
	//-----------------------------------------------------------------------
	switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 101 , CHECKING_SIDE , pt , SCH_No , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) , -1 , tms , cf ) ) {
	case 0 :
		//===============================================================================================================================
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s USER_FLOW_NOTIFICATION 101 Fail%cWHPM%dFAIL\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 );
		//===============================================================================================================================
		return -1;
		break;
	case 1 :
		//-----------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
		if ( ( SCH_Type != SCHEDULER_MOVING_OUT ) && ( SCH_Chamber != -1 ) ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , -1 );
		if ( SCH_Switchable ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , -1 );
		//-----------------------------
		if ( *skip_p1 == 0 ) { *skip_p1 = SCH_No; return 2; } // 2016.05.12
		if ( *skip_p2 == 0 ) { *skip_p2 = SCH_No; return 2; } // 2016.05.12
		return 121;
		break;
	}
	//
	//
	//-----------------------------------------------------------------------
	Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
	Sav_Int_2 = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt );
	//-----------------------------------------------------------------------
	_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING );
	_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
	//
	if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
		//-----------------------------------------------------------------------
		Sav_Int_3 = _SCH_CLUSTER_Get_PathStatus( BM_Side2 , pt2 ); // 2007.04.18
		Sav_Int_4 = _SCH_CLUSTER_Get_PathDo( BM_Side2 , pt2 ); // 2007.04.18
		//-----------------------------------------------------------------------
		_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , SCHEDULER_WAITING ); // 2007.04.18
		_SCH_CLUSTER_Inc_PathDo( BM_Side2 , pt2 ); // 2007.04.18
	}
	//-----------------------------------------------------------------------
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) { // 2006.02.11
		if ( SCH_SWITCH ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Data Refiltering from %s(%d:%d)[F%c]%cWHTM%dPICKREFIL PM%d:%d:%d:%c%c%d\n" , tms+1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsb , wsa , cf + 'A' , 9 , tms+1 , SCH_No - PM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
		}
	}
	//-----------------------------
	if ( _SCH_PRM_GET_MODULE_STOCK( SCH_No ) ) _SCH_CLUSTER_Inc_Stock( CHECKING_SIDE , pt );
	//-----------------------------
	//-------------------------
//	swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); // 2007.07.25 // 2013.03.15
	swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 2 ); // 2007.07.25 // 2013.03.15
	//-------------------------
	if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) || ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) ) { // 2013.01.22 // 2013.03.09
		if ( swmode == 10 ) swmode = 1;
	}
	//
	if ( swmode == 10 ) { // 2013.01.22
		Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PICKPLACE , CHECKING_SIDE , pt , SCH_No , cf , wsa , wsb , Result , -1 , FALSE , swmode , BM_Side2 , pt2 ); // 2007.07.13
	}
	else {
		Function_Result = SCHEDULING_MOVE_OPERATION_ALG0( tms , MACRO_PICK , CHECKING_SIDE , pt , SCH_No , cf , wsa , wsb , Result , -1 , FALSE , swmode , BM_Side2 , pt2 ); // 2007.07.13
	}
	//-----------------------------------------------------------------------
	MODULE_LAST_SWITCH_0[SCH_No] = Result; // 2010.05.21
	//-----------------------------------------------------------------------
	if ( Function_Result == SYS_ABORTED ) { // 2002.07.18 // 2005.11.25
		if ( SCH_Out_Th_Check == 1 ) {
			SCHEDULER_Set_TM_MidCount( tms , 0 );
			LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
		}
//		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08 // 2013.03.11
//		if ( ( SCH_Type != SCHEDULER_MOVING_OUT ) && ( SCH_Chamber != -1 ) ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , -1 ); // 2006.03.27 // 2013.03.11
//		if ( SCH_Switchable ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , -1 ); // 2007.01.15 // 2013.03.11
		return -1;
	}
	//=========================================================================================
	// 2005.11.25
	//=========================================================================================
	else if ( Function_Result == SYS_ERROR ) { // 2005.11.25
		//-----------------------------------------------------------------------
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
		_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , Sav_Int_2 );
		//-----------------------------------------------------------------------
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) { // 2007.04.18
			_SCH_CLUSTER_Set_PathStatus( BM_Side2 , pt2 , Sav_Int_3 ); // 2007.04.18
			_SCH_CLUSTER_Set_PathDo( BM_Side2 , pt2 , Sav_Int_4 ); // 2007.04.18
		}
		//-----------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
		if ( ( SCH_Type != SCHEDULER_MOVING_OUT ) && ( SCH_Chamber != -1 ) ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , -1 ); // 2006.03.27
		if ( SCH_Switchable ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , -1 ); // 2007.01.15
		//-----------------------------
		if ( _SCH_PRM_GET_MODULE_STOCK( SCH_No ) ) _SCH_CLUSTER_Dec_Stock( CHECKING_SIDE , pt );
		//-----------------------------
		return 0;
	}
	//=========================================================================================
	if ( Result ) _SCH_MODULE_Set_TM_Switch_Signal( tms , SCH_No );
	//-----------------------------------------------------------------------
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
		if ( SCH_SWITCH ) SCHEDULING_CHECK_FIXED_GROUPPING_FLOWDATA( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , tms );
	}
	//-----------------------------------------------------------------------
	switch ( _SCH_CLUSTER_Get_FailOut( CHECKING_SIDE , pt ) ) {
	case 4 : // Reset All
		SCH_Out_Not_Run = 1;
		break;
	}
	//============================================================================================================================
	Sav_Int_4 = 0; // 2010.02.01
	//
	if ( SCH_Out_Not_Run == 0 ) {
		//
		if ( SCH_PM_IS_WILL_EMPTY( SCH_No , pmslot ) ) { // 2014.01.10
			//------------------------------------------------------------------------------------------------------------------------
			// 2008.03.24
			//------------------------------------------------------------------------------------------------------------------------
//			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2015.10.27
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() >= 2 ) { // 2015.10.27
				if      ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( SCH_No , FALSE , -1 ) ) SCHEDULING_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + cf , SCH_No );
					if ( _SCH_MODULE_GET_USE_ENABLE( SCH_No + 1 , FALSE , -1 ) ) SCHEDULING_MACRO_POST_PROCESS_OPERATION( BM_Side2 , pt2 , ( tms * 100 ) + cf , SCH_No + 1 );
				}
				else if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 <= 0 ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( SCH_No , FALSE , -1 ) ) SCHEDULING_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + cf , SCH_No );
				}
				else if ( ( SCH_Slot <= 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( ( ( SCH_No - PM1 ) % 2 == 0 ) ? SCH_No + 1 : SCH_No , FALSE , -1 ) ) SCHEDULING_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + cf , ( ( SCH_No - PM1 ) % 2 == 0 ) ? SCH_No + 1 : SCH_No ); // 2008.06.11
				}
			}
			else {
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 12-1 END_TYPE_0(PICKfromPM) = Chamber(PM%d) finger(%d) pt(%d,%d) Slot(%d,%d) [%d][%d][%d][%d][%d] [%d]\n" , tms+1 , SCH_No - PM1 + 1 , cf , pt , pt2 , SCH_Slot , SCH_Slot2 , Sav_Int_2 , Sav_Int_4 , SCH_Out_Not_Run , Result , swmode , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) );
				//----------------------------------------------------------------------
				Sav_Int_4 = SCHEDULING_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , ( tms * 100 ) + cf , SCH_No );
			}
			//------------------------------------------------------------------------------------------------------------------------
		}
	}
	//============================================================================================================================
	switch ( _SCH_CLUSTER_Get_FailOut( CHECKING_SIDE , pt ) ) {
	case 4 : // Reset All
	case 5 : // Reset
		SCH_Type = SCHEDULER_MOVING_NEXT;
		_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , 1 );
		_SCH_CLUSTER_Set_FailOut( CHECKING_SIDE , pt , 0 );
		break;
	}
	//============================================================================================================================
	// 2010.02.01
	//============================================================================================================================
	Sav_Int_1 = _SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , pt , 20 , 4 );
	if ( Sav_Int_1 > 0 ) {
		if ( ( Sav_Int_1 + 1 ) == _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) ) {
			_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) + 1 );
		}
	}
	//============================================================================================================================
	if ( SCH_EndMode ) {
		switch( _SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( 0 ) ) {
		case GROUP_HAS_SWITCH_NONE :
			break;
		case GROUP_HAS_SWITCH_INS :
			_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );
			break;
		case GROUP_HAS_SWITCH_OUTS :
		case GROUP_HAS_SWITCH_ALL :
			if ( SCHEDULING_CHECK_Enable_Free_Target_BM_when_BMS_Present( tms , CHECKING_SIDE , 0 , 0 ) ) {
				_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUT );
			}
			else {
				_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , SWITCH_WILL_GO_OUTS );
			}
			break;
		}
	}
	//-----------------------------------------------------------------------
	if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
		_SCH_MODULE_Set_TM_SIDE_POINTER( tms,cf , CHECKING_SIDE , pt , BM_Side2 , pt2 );
	else if ( SCH_Slot > 0 )
		_SCH_MODULE_Set_TM_SIDE_POINTER( tms,cf , CHECKING_SIDE , pt , 0 , 0 );
	else if ( SCH_Slot2 > 0 )
		_SCH_MODULE_Set_TM_SIDE_POINTER( tms,cf , CHECKING_SIDE , pt , 0 , 0 );
	_SCH_MODULE_Set_TM_PATHORDER( tms , cf , SCH_Order );
	_SCH_MODULE_Set_TM_TYPE( tms , cf , SCH_Type );
	_SCH_MODULE_Set_TM_OUTCH( tms , cf , SCH_No ); // 2002.04.23
	//-----------------------------------------------------------------------
	/*
	// 2014.01.10
	//
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); // 2007.11.07
		else if ( SCH_Slot > 0 )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) % 100 ); // 2007.11.07
		else if ( SCH_Slot2 > 0 )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , ( _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) / 100 ) * 100 ); // 2007.11.07
		//
		if ( ( ( SCH_No - PM1 ) % 2 ) != 0 ) { // pm2
			if ( SCH_Slot2 > 0 ) {
				_SCH_MODULE_Set_PM_WAFER( SCH_No - 1 , 0 , _SCH_MODULE_Get_PM_WAFER( SCH_No - 1 , 0 ) % 100 );
			}
		}
		else { // 2008.01.07
			if ( SCH_Slot2 > 0 ) {
				_SCH_MODULE_Set_PM_WAFER( SCH_No + 1 , 0 , 0 );
			}
		}
		//
		_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 );
	}
	else {
		_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); // 2007.11.07
		_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 );
	}
	*/
	//
	// 2014.01.10
	//
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2007.10.07
		if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) ); // 2007.11.07
		else if ( SCH_Slot > 0 )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) % 100 ); // 2007.11.07
		else if ( SCH_Slot2 > 0 )
			_SCH_MODULE_Set_TM_WAFER( tms , cf , ( _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) / 100 ) * 100 ); // 2007.11.07
		//
		if ( ( ( SCH_No - PM1 ) % 2 ) != 0 ) { // pm2
			if ( SCH_Slot2 > 0 ) {
				_SCH_MODULE_Set_PM_WAFER( SCH_No - 1 , pmslot , _SCH_MODULE_Get_PM_WAFER( SCH_No - 1 , pmslot ) % 100 );
			}
		}
		else { // 2008.01.07
			if ( SCH_Slot2 > 0 ) {
				_SCH_MODULE_Set_PM_WAFER( SCH_No + 1 , pmslot , 0 );
			}
		}
		//
		_SCH_MODULE_Set_PM_WAFER( SCH_No , pmslot , 0 );
	}
	else {
		_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , pmslot ) ); // 2007.11.07
		_SCH_MODULE_Set_PM_WAFER( SCH_No , pmslot , 0 );
	}
	//-----------------------------------------------------------------------
	KPLT0_SET_PROCESS_TIME_OUT( SCH_No );
	_SCH_TIMER_SET_ROBOT_TIME_START( tms , cf );
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms+1 , "TM%d STEP 12-2 END_TYPE_0(PICKfromPM) = Chamber(PM%d) finger(%d) pt(%d,%d) Slot(%d,%d) [%d][%d][%d][%d][%d] [%d]\n" , tms+1 , SCH_No - PM1 + 1 , cf , pt , pt2 , SCH_Slot , SCH_Slot2 , Sav_Int_2 , Sav_Int_4 , SCH_Out_Not_Run , Result , swmode , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) );
	//----------------------------------------------------------------------
	if ( SCH_Out_Th_Check == 1 ) {
		SCHEDULER_Set_TM_MidCount( tms , 0 );
		LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
	}
	//
	if ( swmode == 10 ) { // 2013.01.22LE
		//
		Checking_Side_Changed = -1;
		//
		cf = _SCH_ROBOT_GET_SWAP_PLACE_ARM( cf );
		//
//		if ( _SCH_MODULE_Get_TM_WAFER( tms,cf ) < 0 ) return -1; // 2013.10.31
		if ( _SCH_MODULE_Get_TM_WAFER( tms,cf ) <= 0 ) return -1; // 2013.10.31
		//
		if ( _SCH_MODULE_Get_TM_SIDE( tms,cf ) != CHECKING_SIDE ) {
			Checking_Side_Changed = CHECKING_SIDE;
			CHECKING_SIDE = _SCH_MODULE_Get_TM_SIDE( tms,cf );
		}
		//
		pt = _SCH_MODULE_Get_TM_POINTER( tms,cf );
		//
//		SCH_Order = _SCH_MODULE_Get_TM_PATHORDER( tms , cf ); // 2014.01.21
		//
		//-----------------------------------------------------------------------
		currpathdo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt );
		//-----------------------------------------------------------------------
		if ( currpathdo == PATHDO_WAFER_RETURN ) return -1;
		//===============================================================================================
		SCHEDULER_Set_PM_MidCount( tms , currpathdo );
		//===============================================================================================
		_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
		//-----------------------------------------------------------------------
		SCHEDULER_Set_TM_LastMove_Signal( tms , SCH_No );
		//===============================================================================================
		pt2 = 0;
		SCH_Slot = 1;
		SCH_Slot2 = 0;
		wsa = _SCH_MODULE_Get_TM_WAFER( tms,cf );
		wsb = SCH_Slot;
		//
		BM_Side2 = -1;
		pt2 = -1;
		//===============================================================================================
		SCH_Order = _SCH_CLUSTER_Get_PathOrder( CHECKING_SIDE , pt , currpathdo - 1 , SCH_No ); // 2014.01.21
		//===============================================================================================
		SCHEDULING_CHANG_MAKE_OTHER_TM_DATA_BEFORE_PLACE_TO_OTHER_BPM( tms , cf , FALSE , SCH_No );
		//===============================================================================================
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING );
		//===============================================================================================
		MODULE_LAST_SWITCH_0[SCH_No] = 0;
		//===============================================================================================
		if ( _SCH_CLUSTER_Check_HasProcessData_Post( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order ) ) {
			if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( SCH_No , &prcs_time , &post_time ) % 10 ) == 1 ) {
				_SCH_CLUSTER_Set_PathProcessData_SkipPost( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order );
				_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , SCH_No );
			}
			else {
				if ( post_time >= 0 ) {
					_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , SCH_No , post_time );
				}
			}
			if ( prcs_time >= 0 ) {
				_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , SCH_No , prcs_time );
			}
		}
		//===============================================================================================
		_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 );
		//===============================================================================================
		SCHEDULER_CONTROL_PLACE_PICK_REPORT_WITH_ERROR_A0SUB4( tms , SCH_No , CHECKING_SIDE , pt , cf , SCH_Order );
		//===============================================================================================
//		_SCH_MODULE_Set_PM_SIDE_POINTER( SCH_No , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//		_SCH_MODULE_Set_PM_PATHORDER( SCH_No , 0 , SCH_Order ); // 2014.01.10
//		_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , _SCH_MODULE_Get_TM_WAFER( tms,cf ) ); // 2014.01.10
		//
		_SCH_MODULE_Set_PM_SIDE( SCH_No , pmslot , CHECKING_SIDE ); // 2014.01.10
		_SCH_MODULE_Set_PM_POINTER( SCH_No , pmslot , pt ); // 2014.01.10
		_SCH_MODULE_Set_PM_PATHORDER( SCH_No , pmslot , SCH_Order ); // 2014.01.10
		_SCH_MODULE_Set_PM_WAFER( SCH_No , pmslot , _SCH_MODULE_Get_TM_WAFER( tms,cf ) ); // 2014.01.10
		//
		_SCH_MODULE_Set_TM_WAFER( tms , cf , 0 );
		//===============================================================================================
		//
		_SCH_MODULE_Set_PM_PICKLOCK( SCH_No , pmslot , 1 ); // 2014.01.10
		//
		//==============================================================================================================
		if ( SCH_PM_IS_ALL_PLACED_PROCESSING( SCH_No , TRUE , FALSE , &multiprcs ) ) { // 2014.01.10
			//
			if ( multiprcs ) {
				//
				if ( !SCH_PM_GET_FIRST_SIDE_PT( SCH_No , &m_f_side , &m_f_pt ) ) {
					m_f_side = CHECKING_SIDE;
					m_f_pt = pt;
				}
				//
				_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( m_f_side , m_f_pt , -1 , SCH_No , (tms*100)+cf , -1 , 106 );
				//
			}
			else {
				//
				_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 );
				_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
					  SCH_No , -1 ,
					  wsa , -1 , (tms*100)+cf , -1 ,
					  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
					  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( tms , cf ) , NextPM , PROCESS_DEFAULT_MINTIME ,
					  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
					  BM_Side2 , pt2 , -1 , 0 ,
					  0 , 105 );
			}
		}
		//==============================================================================================================
		if ( _SCH_PRM_GET_MODULE_STOCK( SCH_No ) ) {
			if ( !SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( tms , CHECKING_SIDE , pt , 1 ) ) {
				_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
			}
		}
		//===============================================================================================
		if ( SCHEDULER_CONTROL_Next_All_Stock_and_Nothing_to_Go( tms , CHECKING_SIDE , pt , SCH_No ) ) {
			_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
		}
		//===============================================================================================
		_SCH_CLUSTER_After_Place_For_Parallel_Resetting( CHECKING_SIDE , pt , currpathdo - 1 , SCH_No );
		//==============================================================================================================
		SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( CHECKING_SIDE , SCH_No );
		//===============================================================================================================================
		_SCH_MODULE_Set_TM_Switch_Signal( tms , -1 );
		//===============================================================================================================================
//		if ( ( SCH_Type != SCHEDULER_MOVING_OUT ) && ( SCH_Chamber != -1 ) ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_Chamber , -1 ); // 2013.03.11 // 2013.03.15
//		if ( SCH_Switchable ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , -1 ); // 2013.03.11 // 2013.03.18
		if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( SCH_No , -1 ); // 2013.03.11 // 2013.03.18
		//===============================================================================================================================
		if ( Checking_Side_Changed != -1 ) CHECKING_SIDE = Checking_Side_Changed;
		//=================================================================================================
		*Run_Doing = TRUE;
		//=================================================================================================
	}
	//
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 ); // 2006.02.08
	//
	if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( tms ) != -1 ) ) {
		if ( swmode != 10 ) return 2; // 2002.10.15 // 2013.01.22
	}
	//==============================================================================================
	//----------------------------------------------------------
	*Run_Doing = TRUE;
	//----------------------------------------------------------
	return 0;
}



int SCHEDULER_RUN_PICK_FROM_PM( int tms , int CHECKING_SIDE , int SCH_W_Prcs , int SCH_No , int SCH_Type , int SCH_NextFinger , int SCH_Order , int SCH_Chamber , int SCH_Switchable , int SCH_Halfpick , int SCH_EndMode , int SCH_SWITCH , int *skip_p1 , int *skip_p2 , BOOL *Run_Doing ) { // 2013.02.07
	int Res , tmdata;
	//
	tmdata = _SCH_PRM_GET_MODULE_TMS_GROUP( SCH_No );
	//
	if ( tmdata >= 0 ) {
		//
		EnterCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
		if ( SCHEDULER_Get_MULTI_GROUP_TMS( tmdata ) ) {
			//
//			_IO_CIM_PRINTF( "MULTI_GROUP_TMS[%d] LOCKING(%d) REJECT at %s in SCHEDULER_RUN_PICK_FROM_PM\n" , tms + 1 , tmdata , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( SCH_No ) );
			//
			LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
			return 0;
		}
		SCHEDULER_Set_MULTI_GROUP_TMS( tmdata );
		LeaveCriticalSection( &CR_MULTI_GROUP_TMS_STYLE_0[tmdata] );
	}
	//
	Res = SCHEDULER_RUN_PICK_FROM_PM_SUB( tms , CHECKING_SIDE , SCH_W_Prcs , SCH_No , SCH_Type , SCH_NextFinger , SCH_Order , SCH_Chamber , SCH_Switchable , SCH_Halfpick , SCH_EndMode , SCH_SWITCH , skip_p1 , skip_p2 , Run_Doing );
	//
	if ( tmdata >= 0 ) SCHEDULER_ReSet_MULTI_GROUP_TMS( tmdata );
	//
	return Res;
}

/*
// 2013.12.18
int SCHEDULER_RUN_MOVE_FOR_GET( int tms , int CHECKING_SIDE , int SCH_Chamber , int SCH_NextFinger , int SCH_No ) {
	int pt , wsa , cf;

	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 18 RUN_TYPE_10(MOVEtoPM)\n" , tms + 1 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , SCH_Chamber );
	//
	if ( SCHEDULER_Get_TM_LastMove_Signal( tms ) == SCH_Chamber ) return 0;
	//
	pt = _SCH_MODULE_Get_PM_POINTER( SCH_Chamber , 0 );
	//
	wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt);
	//
	if ( SCH_NextFinger != -1 ) {
		cf = SCH_NextFinger;
	}
	else {
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_No ) ) {
		case 0 :
			cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_No );
			break;
		case 1 :
			cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
			break;
		case 2 :
			cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
			break;
		}
	}
	//
	if ( cf == -1 ) return 0;
	//
	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 0 , SCH_Chamber , cf , wsa , 1 ) == SYS_ABORTED ) {
		return -1;
	}
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 19 END_TYPE_10(MOVEtoPM) = Chamber(PM%d) finger(%d)\n" , tms + 1 , SCH_Chamber - PM1 + 1 , cf );
	//
	return 0;
}
*/

// 2013.12.18
int SCHEDULER_RUN_MOVE_FOR_GET( int tms , int CHECKING_SIDE , int nextch , int SCH_NextFinger , int ch ) {
	int pt , wsa , cf;
	int side , pmslot;

	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 18 RUN_TYPE_10(MOVEtoPM)\n" , tms + 1 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , ch );
	//
	if ( SCHEDULER_Get_TM_LastMove_Signal( tms ) == ch ) return 0;
	//
	if ( !SCH_PM_HAS_PICKING( ch , &pmslot ) ) return 0; // 2014.01.10
	//
//	pt = _SCH_MODULE_Get_PM_POINTER( ch , 0 ); // 2014.01.10
	//
	side = _SCH_MODULE_Get_PM_SIDE( ch , pmslot ); // 2014.01.10
	pt = _SCH_MODULE_Get_PM_POINTER( ch , pmslot ); // 2014.01.10
	//
//	wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt); // 2014.01.10
	wsa = _SCH_CLUSTER_Get_SlotIn(side,pt); // 2014.01.10
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,ch ) ) {
	case 0 :
		if ( SCH_NextFinger != -1 ) {
			cf = SCH_NextFinger;
		}
		else {
			cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , ch );
		}
		break;
	case 1 :
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
		break;
	case 2 :
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
		break;
	}
	//
	if ( cf == -1 ) return 0;
	//
//	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 0 , ch , cf , wsa , 1 ) == SYS_ABORTED ) { // 2014.01.10
	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , side , 0 , ch , cf , wsa , 1 ) == SYS_ABORTED ) { // 2014.01.10
		return -1;
	}
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 19 END_TYPE_10(MOVEtoPM) = Chamber(PM%d) finger(%d)\n" , tms + 1 , ch - PM1 + 1 , cf );
	//
	return 0;
}
//

int SCHEDULER_RUN_MOVE_FOR_PUT( int tms , int CHECKING_SIDE , int SCH_Chamber , int SCH_No ) {
	int pt , wsa;
	int side;

	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 29 RUN_TYPE_22(MOVEtoPM)\n" , tms + 1 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , SCH_Chamber );
	//
	if ( SCHEDULER_Get_TM_LastMove_Signal( tms ) == SCH_Chamber ) return 0;
	//
	pt = _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( SCH_Chamber );
	if ( ( pt == 5 ) || ( pt == 6 ) ) return 0;

	side = _SCH_MODULE_Get_TM_SIDE( tms , SCH_No ); // 2014.01.10
	pt  = _SCH_MODULE_Get_TM_POINTER( tms , SCH_No );
	//
//	wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE,pt); // 2014.01.10
	wsa = _SCH_CLUSTER_Get_SlotIn(side,pt); // 2014.01.10
	//
//	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 1 , SCH_Chamber , SCH_No , wsa , 1 ) == SYS_ABORTED ) { // 2014.01.10
	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , side , 1 , SCH_Chamber , SCH_No , wsa , 1 ) == SYS_ABORTED ) { // 2014.01.10
		return -1;
	}
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 29 END_TYPE_22(MOVEtoPM) = Chamber(PM%d) finger(%d)\n" , tms + 1 , SCH_Chamber - PM1 + 1 , SCH_No );
	//
	return 0;
}


int SCHEDULER_RUN_MOVE_FOR_BM_GET( int tms , int CHECKING_SIDE , int SCH_Chamber , int SCH_Slot ) {
	int k , wsb , cf , SCH_Slot2 , BM_Side2 , pt2;
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 20 RUN_TYPE_20(MOVEtoBMforGet)\n" , tms + 1 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , SCH_Chamber );
	if ( SCHEDULER_Get_TM_LastMove_Signal( tms ) == SCH_Chamber ) return 0;
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
	case 0 :
		cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , SCH_Chamber );
		break;
	case 1 :
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) ? -1 : TA_STATION;
		break;
	case 2 :
		cf = ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) ? -1 : TB_STATION;
		break;
	}
	if ( cf == -1 ) return 0;
	//
	if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2013.03.09
		SCHEDULING_CHECK_Chk_BM_HAS_IN_ANOTHER_WAFER_for_DOUBLE( SCH_Chamber , CHECKING_SIDE , SCH_Slot , &BM_Side2 , &pt2 , &SCH_Slot2 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , TRUE , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) , FALSE );
		if ( ( SCH_Slot == 2 ) && ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) ) {
			if ( SCH_Slot2 > 0 ) {
				SCH_Slot = 1;
				SCH_Slot2 = 2;
			}
			else {
				SCH_Slot2 = SCH_Slot;
				SCH_Slot = 0;
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 <= 0 ) ) {
					if ( ( SCH_Slot > ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) ) {
						if ( _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) > 0 ) {
							if ( _SCH_MODULE_Get_BM_STATUS( SCH_Chamber , SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 ) ) != BUFFER_OUTWAIT_STATUS ) {
								k         = SCH_Slot - ( _SCH_PRM_GET_MODULE_SIZE( SCH_Chamber ) / 2 );
								SCH_Slot2 = SCH_Slot;
								SCH_Slot  = k;
							}
							else {
								SCH_Slot2 = SCH_Slot;
								SCH_Slot = 0;
							}
						}
						else {
							SCH_Slot2 = SCH_Slot;
							SCH_Slot = 0;
						}
					}
				}
				if ( ( SCH_Slot > 0 ) && ( SCH_Slot2 > 0 ) ) {
					if ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 1 ) {
						if ( ( SCH_Slot + 1 ) != SCH_Slot2 ) SCH_Slot2 = 0;
					}
				}
			}
			else {
				SCH_Slot2 = 0;
			}
		}
	}
	else {
		SCH_Slot2 = 0;
	}
	//
	wsb = SCH_Slot + (SCH_Slot2*100);
	//=============================================================================================
//	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 0 , SCH_Chamber , cf , 1 , wsb ) == SYS_ABORTED ) { // 2014.06.25
	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 0 , SCH_Chamber , cf , 0 , wsb ) == SYS_ABORTED ) { // 2014.06.25
		return -1;
	}
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 21 END_TYPE_20(MOVEtoBMforGet) = Chamber(BM%d) finger(%d)\n" , tms + 1 , SCH_Chamber - BM1 + 1 , cf );
	//
	return 0;
}


int SCHEDULER_RUN_MOVE_FOR_BM_PUT( int tms , int CHECKING_SIDE , int SCH_Chamber , int SCH_No , int SCH_Slot ) {
	int wsb , cf , SCH_Slot2;
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 22 RUN_TYPE_21(MOVEtoBMforPut)\n" , tms + 1 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , SCH_Chamber );
	//
	if ( SCHEDULER_Get_TM_LastMove_Signal( tms ) == SCH_Chamber ) return 0;
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms,SCH_Chamber ) ) {
	case 0 :
		cf = SCH_No;
		break;
	case 1 :
		cf = ( SCH_No != TA_STATION ) ? -1 : TA_STATION;
		break;
	case 2 :
		cf = ( SCH_No != TB_STATION ) ? -1 : TB_STATION;
		break;
	}
	//
	if ( cf == -1 ) return 0;
	//
	//=============================================================================================
	if ( ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) { // 2013.03.09
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) {
			if ( ( _SCH_MODULE_Get_TM_WAFER( 0,SCH_No ) % 100 ) <= 0 ) {
				SCH_Slot2 = SCH_Slot + 1;
				SCH_Slot = 0;
			}
			else if ( ( _SCH_MODULE_Get_TM_WAFER( 0,SCH_No ) / 100 ) <= 0 ) {
				SCH_Slot2 = 0;
			}
			else {
				SCH_Slot2 = SCH_Slot + 1;
			}
		}
		else {
			if ( ( _SCH_MODULE_Get_TM_WAFER( 0,SCH_No ) % 100 ) <= 0 ) {
				if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) {
					SCH_Slot2 = SCH_Slot + 2;
				}
				else {
					SCH_Slot2 = SCH_Slot + 1;
				}
				SCH_Slot = 0;
			}
			else if ( ( _SCH_MODULE_Get_TM_WAFER( 0,SCH_No ) / 100 ) <= 0 ) {
				SCH_Slot2 = 0;
			}
			else {
				if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) {
					SCH_Slot2 = SCH_Slot + 2;
				}
				else {
					SCH_Slot2 = SCH_Slot + 1;
				}
			}
		}
	}
	else {
		SCH_Slot2 = 0;
	}
	//
	wsb = SCH_Slot + (SCH_Slot2*100);
	//=============================================================================================
//	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 1 , SCH_Chamber , cf , 1 , wsb ) == SYS_ABORTED ) { // 2014.06.25
	if ( _SCH_MACRO_TM_MOVE_OPERATION_ALG0( tms , CHECKING_SIDE , 1 , SCH_Chamber , cf , 0 , wsb ) == SYS_ABORTED ) { // 2014.06.25
		return -1;
	}
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tms + 1 , "TM%d STEP 23 END_TYPE_20(MOVEtoBMforGet) = Chamber(BM%d) finger(%d)\n" , tms + 1 , SCH_Chamber - BM1 + 1 , cf );
	//
	return 0;
}


BOOL SKIP_BM_CHECK_RESETTING( int ch , int *Skip1 , int *Skip2 , int *Skip3 , int *Skip4 , int *Skip5 ) { // 2013.12.06
	if ( ch == -99 ) {
		*Skip1 = -1;
		*Skip2 = -1;
		*Skip3 = -1;
		*Skip4 = -1;
		*Skip5 = -1;
	}
	else {
		//
		if ( ch < BM1 ) return FALSE;
		if ( ch >= TM ) return FALSE;
		//
		if ( *Skip1 == -1 ) {
			*Skip1 = ch;
			return TRUE;
		}
		else if ( *Skip2 == -1 ) {
			*Skip2 = ch;
			return TRUE;
		}
		else if ( *Skip3 == -1 ) {
			*Skip3 = ch;
			return TRUE;
		}
		else if ( *Skip4 == -1 ) {
			*Skip4 = ch;
			return TRUE;
		}
		else if ( *Skip5 == -1 ) {
			*Skip5 = ch;
			return TRUE;
		}
		//
	}
	return FALSE;
}
//

int SCHEDULING_CHECK_for_Multi_Pick_First_Check( int tms , int ch , int slot ) {
	int s , p , pd , pr , k , m;
	int ts , tp , placing , td , tr;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TA_STATION ) ) return -1;
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms,TB_STATION ) ) return -2;

	if ( ( _SCH_MODULE_Get_TM_WAFER( tms,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms,TB_STATION ) <= 0 ) ) {
		ts = -1;
	}
	else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms,TB_STATION ) <= 0 ) ) {
		ts = _SCH_MODULE_Get_TM_SIDE( tms,TA_STATION );
		tp = _SCH_MODULE_Get_TM_POINTER( tms,TA_STATION );
		if ( ( ts < 0 ) || ( ts >= MAX_CASSETTE_SIDE ) ) return -3;
		if ( ( tp < 0 ) || ( tp >= MAX_CASSETTE_SLOT_SIZE ) ) return -4;
	}
	else if ( ( _SCH_MODULE_Get_TM_WAFER( tms,TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms,TB_STATION ) > 0 ) ) {
		ts = _SCH_MODULE_Get_TM_SIDE( tms,TB_STATION );
		tp = _SCH_MODULE_Get_TM_POINTER( tms,TB_STATION );
		if ( ( ts < 0 ) || ( ts >= MAX_CASSETTE_SIDE ) ) return -5;
		if ( ( tp < 0 ) || ( tp >= MAX_CASSETTE_SLOT_SIZE ) ) return -6;
	}
	else {
		return -9;
	}
	//
	if ( ch == -1 ) { // cm
		//
		s = slot;
		p = _SCH_MODULE_Get_TM_DoPointer( s );
		if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return -11;
		//
	}
	else {
		if      ( ( ch >= BM1 ) && ( ch < TM ) ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( ch , slot ) <= 0 ) return -21;
			//
			s = _SCH_MODULE_Get_BM_SIDE( ch , slot );
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return -22;
			//
			p = _SCH_MODULE_Get_BM_POINTER( ch , slot );
			if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return -23;
			//
		}
		else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
			//
			if ( !SCH_PM_HAS_PICKING( ch , &slot ) ) return -31;
			//
			s = _SCH_MODULE_Get_PM_SIDE( ch , slot );
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return -32;
			//
			p = _SCH_MODULE_Get_PM_POINTER( ch , slot );
			if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return -33;
			//
		}
		else {
			return -34;
		}
	}
	//
	if ( ts == -1 ) {
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		pr = _SCH_CLUSTER_Get_PathRange( s , p );
		//
		if ( ( ( pd - 1 ) >= 0 ) && ( ( pd - 1 ) < pr ) ) {
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd-1 , k );
				//
				if ( m > 0 ) {
					if ( _SCH_PRM_GET_MODULE_SIZE( m ) < 2 ) { // 2018.07.13
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) > 0 ) {
							return 10; // Pick X , Place O
						}
					}
				}
				//
			}
		}
		//
		if ( pd >= pr ) return 11;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k );
			//
			if ( m > 0 ) {
//				if ( SCH_PM_IS_EMPTY( m ) ) { // 2018.07.10
				if ( SCH_PM_IS_FREE( m ) ) { // 2018.07.10
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
						return 21; // Pick O , Place X
					}
				}
			}
			//
		}
		//
		return 20; // Pick X , Place O
		//
	}
	else {
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		pr = _SCH_CLUSTER_Get_PathRange( s , p );
		//
		td = _SCH_CLUSTER_Get_PathDo( ts , tp );
		tr = _SCH_CLUSTER_Get_PathRange( ts , tp );
		//
		if ( pd == ( td - 1 ) ) {
			//
			if ( pd < pr ) {
				//
				placing = 0;
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k );
					//
					if ( m > 0 ) {
						if ( _SCH_PRM_GET_MODULE_SIZE( m ) >= 2 ) { // 2018.05.23
							if ( _SCH_MODULE_Get_PM_WAFER( m , -1 ) > 0 ) {
								if ( SCH_PM_IS_FREE( m ) ) {
									placing = 1;
									break;
								}
							}
						}
						else {
							if ( _SCH_MODULE_Get_PM_WAFER( m , -1 ) > 0 ) {
								placing = 1;
								break;
							}
						}
					}
					//
				}
				//
				if ( placing == 1 ) {
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						m = SCHEDULER_CONTROL_Get_PathProcessChamber( s , p , pd , k );
						//
						if ( m > 0 ) {
							if ( SCH_PM_IS_FREE( m ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
									return 30; // Pick X , Place O
								}
							}
						}
						//
					}
					//
				}
				//
			}
			//
			if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) == _SCH_CLUSTER_Get_ClusterIndex( ts , tp ) ) {
				if ( ( ch >= PM1 ) && ( ch < AL ) ) { // 2018.05.16
					if ( _SCH_PRM_GET_MODULE_SIZE( ch ) >= 2 ) {
						//
						if ( SCH_PM_IS_FREE( ch ) ) {
							if ( (pd-1) >= 0 ) {
								if      ( _SCH_CLUSTER_Get_PathRun( ts , tp , pd-1 , 0 ) == ch ) {
									return 31; // Pick O , Place X
								}
								else if ( _SCH_CLUSTER_Get_PathRun( ts , tp , pd-1 , 0 ) == -ch ) {
									return 41; // Pick O , Place X
								}
							}
						}
						//
					}
					else {
						return 51; // Pick O , Place X
					}
				}
				else {
					return 61; // Pick O , Place X
				}
			}
		}
		//
		if ( td > tr ) return 12; // Pick X , Place O
		//
		// 2018.07.05
		//
		if ( pd == td ) {
			if ( _SCH_CLUSTER_Get_ClusterIndex( s , p ) == _SCH_CLUSTER_Get_ClusterIndex( ts , tp ) ) {
				if ( ( ch >= PM1 ) && ( ch < AL ) ) {
					if ( _SCH_PRM_GET_MODULE_SIZE( ch ) < 2 ) {
						//
						placing = 0;
						//
						tr = 0;
						//
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							//
							m = SCHEDULER_CONTROL_Get_PathProcessChamber( ts , tp , td - 1 , k );
							//
							if ( m > 0 ) {
								//
								placing++;
								//
								if ( m == ch ) tr++;
								//
							}
							//
						}
						//
						if ( ( tr == 1 ) && ( placing == 1 ) ) return 71; // Pick O , Place X
						//
					}
					else { // 2018.07.10
						//
						placing = 0;
						//
						tr = 0;
						//
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							//
							m = SCHEDULER_CONTROL_Get_PathProcessChamber( ts , tp , td - 1 , k );
							//
							if ( m > 0 ) {
								//
								if ( SCH_PM_IS_FREE( m ) ) placing++;
								//
								if ( m == ch ) tr++;
								//
							}
							//
						}
						//
						if ( ( tr > 0 ) && ( placing == 0 ) ) return 81; // Pick O , Place X
						//
					}
				}
			}
		}
		//
		return 40; // Pick X , Place O
		//
	}
	return -1;
	//
}


int MULTIL_BM_PM_SUPPLYID_BM_FAST_CHECK( int pm , int bmsupid ) { // 2018.10.20
	// bm = 0 , pm = 1
	int pmslot , psu;
	//
	if ( !SCH_PM_HAS_PICKING( pm , &pmslot ) ) return -1;
	//
	if ( SCH_PM_IS_PLACING( pm ) ) return 1;
	//
	psu = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_PM_SIDE( pm , pmslot ) , _SCH_MODULE_Get_PM_POINTER( pm , pmslot ) );
	//
	if ( psu > bmsupid ) return 0;
	if ( psu < bmsupid ) return 1;
	//
	return -1;
	//
}


BOOL Multi_Expect_Pick_and_Place_Skip( int tms , int arm , int placech ) { // 2018.10.20
	int s , p , d;
	//
	if ( !MULTI_ALL_PM_FULLSWAP ) return FALSE;
	//
	if ( placech < PM1 ) return FALSE;
	if ( placech >= AL ) return FALSE;
	//
	s = _SCH_MODULE_Get_TM_SIDE( tms , arm );
	p = _SCH_MODULE_Get_TM_POINTER( tms , arm );
	d = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
	//
	return !SCH_PM_HAS_PLACING_EXPECT( placech , s , p , d );
	//
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_RUN_CONTROL_TM_1_STYLE_0( int CHECKING_SIDE ) {
	int i , j , k , sl , Result , pres , ps , pe , bspickx , bmoutf , /* global_bmckmode , // 2008.10.28 */ currpathdo;
	int wta , wtb , wsa , wsb , pt , pt2 , cf , ch , Enter_Side , Waiting_Motion , swmode;
	int R_Curr_Chamber , R_Next_Chamber , R_Slot , R_Next_Move , R_Path_Order , R_Wait_Process , R_Schpt , R_JumpMode , R_EndMode , R_Switch , R_NextFinger , R_Retmode , R_Switchable , R_ArmIntlks , R_Halfpick;
	int SCH_Mode = -1 , SCH_No = -1 , SCH_Type , SCH_Order , SCH_Chamber , SCH_Slot , SCH_W_Prcs , SCH_WY , SCH_EndMode , SCH_NextFinger , SCH_RETMODE , SCH_PICKCM_ARMINT , SCH_Halfpick;
	int SCH_Small_Time = 999999 , SCH_Buff_Time , SCH_SchPt , SCH_JumpMode , SCH_SWITCH , SCH_Switchable;
	long SCH_R_Time = 0;
	int nomorerun_tag;
	//
	int  SCH_Out_Not_Run = 0;
	int  SCH_Out_Th_Check = 0;
	char StringBuffer[256];
	int ml , mb;
	char NextPM[256];
	BOOL SCH_PR_RUN;
	int prcs_time , post_time;
	int ret_rcm , ret_pt; // 2007.02.01

	int Function_Result; // 2005.11.25
	int Sav_Int_1; // 2005.11.25
	int Sav_Int_2; // 2005.11.25
	int Sav_Int_3; // 2005.11.25
	//==============================================================================================================
	int Run_Doing;
	int pca , pcaarm[4]; // 2009.09.10
	int Skip_Pick_PM1 , Skip_Pick_PM2 , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Check_Again; // 2013.03.11
	int Skip_Pick_BM1 , Skip_Pick_BM2; // 2016.05.12
	int pmslot , m_f_side , m_f_pt;
	BOOL multiprcs;

	BOOL MULTIPICK_FIRST; // 2018.07.10
	int  MULTI_FIRST_Res; // 2018.07.10
	//
	//================================================================================================
	// 2008.01.08
	//================================================================================================
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
		SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_SEPARATE_MIXING( 0 , CHECKING_SIDE );
	}
	//================================================================================================
	//================================================================================================
	// 2008.04.18
	//================================================================================================
	if ( !SCHEDULING_CHECK_LOCKING_MAKE_FREE( 0 , CHECKING_SIDE ) ) { // 2008.04.18
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted 3 ....%cWHTMFAIL\n" , 9 );
		return 0;
	}
	//================================================================================================

	Skip_Pick_BM1 = 0; // 2016.05.12
	Skip_Pick_BM2 = 0; // 2016.05.12

	Skip_Pick_PM1 = 0; // 2013.03.11
	Skip_Pick_PM2 = 0; // 2013.03.11

	//----------------------------------------------------------
	Run_Doing = FALSE;
	//----------------------------------------------------------
	while( TRUE ) {
		//================================================================================================
		if ( ( RUN_NOT_SELECT_COUNT[0] >= 50 ) || _SCH_SYSTEM_PAUSE_GET( CHECKING_SIDE ) == 2 ) { // 2013.05.31
			SCHEDULER_Set_TM_Pick_BM_Signal( 0 , 0 );
			_SCH_MODULE_Set_TM_Switch_Signal( 0 , -1 );
			RUN_NOT_SELECT_COUNT[0] = 0;
			return 1;
		}
		//================================================================================================
		//================================================================================================
		// 2004.12.06
		//================================================================================================
		//================================================================================================
//		global_bmckmode = 0; // 2008.10.28
		//================================================================================================
		//================================================================================================
		//================================================================================================
		bmoutf = 0;
		for ( ml = 0 ; ml < 2 ; ml++ ) { // 2002.07.12
			//
			SCH_Mode       = -1;
			SCH_No         = -1;
			SCH_Small_Time = 999999;
			SCH_R_Time     = 0;
			SCH_WY         = FALSE;
			SCH_SWITCH     = FALSE;
			//
			Enter_Side	   = -1;
			Waiting_Motion = -1;
			//
			SCH_PR_RUN	   = FALSE;
			//
			bspickx		   = FALSE;
			//
			pres		   = -99; // 2005.01.12
			//
			MULTIPICK_FIRST = FALSE; // 2018.05.10
			//
//			SCHEDULER_PROCESSING_STATUS_DIFFERENT( TRUE ); // 2012.12.05 // 2019.02.08
			SCHEDULER_PROCESSING_STATUS_DIFFERENT( 0 , TRUE ); // 2012.12.05 // 2019.02.08
			//
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2002.05.30
				if ( ml == 0 ) {
					pres = SCHEDULING_CHECK_Enable_PICK_from_PM_for_GETPR( 0 , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , &SCH_PR_RUN , &bspickx , &bmoutf );
					switch( pres ) {
					case -3 :
						ps = -1;
						pe = -1;
						break;
					case -2 :
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) != -1 ) ) { // 2002.10.15
							ps = -1;
							pe = -1;
						}
						else {
							ps = PM1;
							pe = ( PM1 + MAX_PM_CHAMBER_DEPTH );
						}
						break;
					case -1 :
						ps = -1;
						pe = -1;
						break;
					default :
						//========================================================
						ps = pres; // 2005.08.30
						//========================================================
						pe = ps + 1;
						break;
					}
				}
				else {
					if ( bmoutf == 0 ) {
						ps = PM1;
						pe = ( PM1 + MAX_PM_CHAMBER_DEPTH );
					}
					else if ( bmoutf < 0 ) {
//							SCHEDULER_Set_PM_MidCount( 0 , -1 ); // 2002.11.14
						ps = -1;
						pe = -1;
					}
					else {
						ps = bmoutf;
						pe = ps + 1;
					}
				}
			}
			else {
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) != -1 ) ) { // 2002.10.15
					ps = -1;
					pe = -1;
				}
				else {
					ps = PM1;
					pe = ( PM1 + MAX_PM_CHAMBER_DEPTH );
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 201 (ml=%d) CHECK_PICK_from_PM_for_GETPR = (pres=%d,ps=%d,pe=%d,bspickx=%d,bmoutf=%d)\n" , ml , pres , ps , pe , bspickx , bmoutf );
			//----------------------------------------------------------------------
			for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
				if ( _SCH_SYSTEM_USING_GET(k) < 10 ) continue;
				//========================================================================================================================
				// *CHECK* : PICK FROM PM
				//========================================================================================================================
				if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) { // 2007.02.01
					for ( i = ps ; i < pe ; i++ ) {
						//
						if ( Skip_Pick_PM1 == i ) continue; // 2013.03.11
						if ( Skip_Pick_PM2 == i ) continue; // 2013.03.11
						//
//						if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE ) || ( _SCH_PRM_GET_MODULE_xGROUP( i ) != 0 ) ) continue; // 2003.10.17
//						if ( _SCH_PRM_GET_MODULE_xGROUP( i ) != 0 ) continue;
						if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , i , G_PICK ) ) continue;
						if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , k ) ) {
							if ( k == CHECKING_SIDE ) { // 2003.10.17
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (Disable Chamber Aborted) ....%cWHTMFAIL\n" , 9 );
									return 0;
								}
							}
							continue; // 2003.10.17
						}
						//----------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
							if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) {
								EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
								Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( 0 , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , ml );
								if ( Result > 0 ) { // 2007.01.15
									j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , i , -1 , R_Next_Chamber , &R_Switchable );
									if ( j > 0 ) Result = -191 - j;
								}
								LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
							}
							else {
								Result = -9999;
							}
						}
						else {
							Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( 0 , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , ml );
							if ( Result > 0 ) { // 2007.01.15
								j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , i , -1 , R_Next_Chamber , &R_Switchable );
								if ( j > 0 ) Result = -192 - j;
							}
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 01 (k=%d) CHECK_PICK_from_PM = Res=%d,PM=%d,R_Next_Move=%d,[SK=%d,%d],SCH_Small_Time=%d,SCH_Mode=%d\n" , k , Result , i , R_Next_Move , Skip_Pick_PM1 , Skip_Pick_PM2 , SCH_Small_Time , SCH_Mode );
						//----------------------------------------------------------------------
						if ( Result >= 0 ) {
							//----------------------------------------------------------------------
							// 2006.07.10
							//----------------------------------------------------------------------
							if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_PM[k=%d,%d]=> PM%d,NextMove=%d,NextCh=%d,WaitPrcs=%d,EndMode=%d,SCHWY=%d,NextFinger=%d\t\t\n" , k , Result , i - PM1 + 1 , R_Next_Move , R_Next_Chamber , R_Wait_Process , R_EndMode , SCH_WY , R_NextFinger );
							}
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							j = 0;
							if ( _SCH_MODULE_FIRST_CONTROL_GET() && ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() > 0 ) ) {
								if ( !_SCH_MODULE_FIRST_EXPEDITE_GET( i ) ) {
									for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
//										if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( j ) != 0 ) ) continue;
										if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , j , G_PICK ) ) continue;
										if ( _SCH_MODULE_FIRST_EXPEDITE_GET( j ) ) {
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) > 0 ) {
												wtb = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , j );
												SCH_Buff_Time = _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME();
												if ( wtb < ( SCH_Buff_Time / 2 ) ) {
													j = 9999;
												}
											}
											else {
//												if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) { // 2014.01.10
												if ( !SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
													j = 9999;
												}
											}
										}
									}
								}
							}
							//
							//
							// 2018.07.10
							//
							MULTI_FIRST_Res = 0;
							//
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO5 ) {
								//
								MULTI_FIRST_Res = SCHEDULING_CHECK_for_Multi_Pick_First_Check( 0 , i , -1 );
								//
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 1001 (k=%d) SCHEDULING_CHECK_for_Multi_Pick_First_Check [%d]= MULTIPICK_FIRST=%d,P=%d,%d,%d\n" , k , MULTI_FIRST_Res , MULTIPICK_FIRST , 0 , i , -1 );
								//
								if ( MULTI_FIRST_Res >= 0 ) {
									switch ( MULTI_FIRST_Res % 10 ) {
									case 1 :
										break;
									case 0 :
										//
										j = 9999;
										//
										break;
									case 2 :
										//
										j = 9999;
										//
										MULTIPICK_FIRST = FALSE;
										//
										break;
									}
								}
							}
							//
							if ( j < 9999 ) {
								SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( i );
								//printf( "0(CrossForm_PICK_from_PM)---------------%d-%d\n" , SCH_Buff_Time , i );
								if ( SCH_Buff_Time < SCH_Small_Time ) {
									Enter_Side	   = k;
									SCH_Mode       = 0;
									SCH_No         = i;
									SCH_Small_Time = SCH_Buff_Time;
									SCH_Type       = R_Next_Move;
									SCH_Order      = R_Path_Order;
									SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
									SCH_W_Prcs	   = R_Wait_Process;
									SCH_EndMode	   = R_EndMode;
									SCH_NextFinger = R_NextFinger;
									if ( SCH_W_Prcs ) {
										Waiting_Motion = SCH_No;
									}
									else {
										Waiting_Motion = -1;
									}
									SCH_SWITCH     = FALSE; // 2006.02.10
									SCH_Chamber    = R_Next_Chamber; // 2006.03.27
									SCH_Switchable = R_Switchable; // 2007.01.16
									SCH_Halfpick   = R_Halfpick; // 2007.11.09
									//
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 01 (k=%d) CHECK_PICK_from_PM [OK1]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 );
									//
									if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
									//
								}
								else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 0 ) ) {
									if ( !SCH_PM_IS_PLACING( SCH_No ) && SCH_PM_IS_PLACING( i ) ) { // 2018.10.20
										Enter_Side	   = k;
										SCH_Mode       = 0;
										SCH_No         = i;
										SCH_Small_Time = SCH_Buff_Time;
										SCH_Type       = R_Next_Move;
										SCH_Order      = R_Path_Order;
										SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
										SCH_W_Prcs	   = R_Wait_Process;
										SCH_EndMode	   = R_EndMode;
										SCH_NextFinger = R_NextFinger;
										if ( SCH_W_Prcs ) {
											Waiting_Motion = SCH_No;
										}
										else {
											Waiting_Motion = -1;
										}
										SCH_SWITCH     = FALSE; // 2006.02.10
										SCH_Chamber    = R_Next_Chamber; // 2006.03.27
										SCH_Switchable = R_Switchable; // 2007.01.16
										SCH_Halfpick   = R_Halfpick; // 2007.11.09
										//
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 01 (k=%d) CHECK_PICK_from_PM [OK2]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 );
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
									else if ( SCH_PM_IS_PLACING( SCH_No ) == SCH_PM_IS_PLACING( i ) ) {
										if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) { // 2008.01.08
											if ( SCHEDULER_CONTROL_Check_2Module_Same_Body_TMDOUBLE_CURR_is_OLD( SCH_No , SCH_R_Time , i , _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) ) { // 2008.01.08
												Enter_Side	   = k;
												SCH_Mode       = 0;
												SCH_No         = i;
												SCH_Small_Time = SCH_Buff_Time;
												SCH_Type       = R_Next_Move;
												SCH_Order      = R_Path_Order;
												SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
												SCH_W_Prcs	   = R_Wait_Process;
												SCH_EndMode	   = R_EndMode;
												SCH_NextFinger = R_NextFinger;
												if ( SCH_W_Prcs ) {
													Waiting_Motion = SCH_No;
												}
												else {
													Waiting_Motion = -1;
												}
												SCH_SWITCH     = FALSE; // 2006.02.10
												SCH_Chamber    = R_Next_Chamber; // 2006.03.27
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Halfpick   = R_Halfpick; // 2007.11.09
												//
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 01 (k=%d) CHECK_PICK_from_PM [OK2]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 );
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
										}
										else {
											if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) {
												Enter_Side	   = k;
												SCH_Mode       = 0;
												SCH_No         = i;
												SCH_Small_Time = SCH_Buff_Time;
												SCH_Type       = R_Next_Move;
												SCH_Order      = R_Path_Order;
												SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
												SCH_W_Prcs	   = R_Wait_Process;
												SCH_EndMode	   = R_EndMode;
												SCH_NextFinger = R_NextFinger;
												if ( SCH_W_Prcs ) {
													Waiting_Motion = SCH_No;
												}
												else {
													Waiting_Motion = -1;
												}
												SCH_SWITCH     = FALSE; // 2006.02.10
												SCH_Chamber    = R_Next_Chamber; // 2006.03.27
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Halfpick   = R_Halfpick; // 2007.11.09
												//
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 01 (k=%d) CHECK_PICK_from_PM [OK3]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 );
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
										}
									}
								}
							}
						}
					}
				}
				//========================================================================================================================
				//========================================================================================================================
				//========================================================================================================================
				// *CHECK* : PICK FROM CM & PLACE FROM TM (Not FM Mode)
				//========================================================================================================================
//					if ( !_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2007.02.01
				if ( !_SCH_PRM_GET_MODULE_MODE( FM ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2007.02.01
					if ( _SCH_SYSTEM_MODE_END_GET( k ) == 0 ) { // 2005.01.04 // 2018.05.05
//						if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) { // 2005.01.04
						if ( Waiting_Motion == -1 ) {
							if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_CM( k , &ret_rcm , &ret_pt ) >= 0 ) { // 2007.02.01
//								if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( k , TRUE , 0 ) ) { // 2003.04.23 // 2003.06.03 // 2005.01.12 // 2007.02.01
								if ( KPLT0_CHECK_CASSETTE_TIME_SUPPLY( _SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() ) ) {
									if ( _SCH_MODULE_Need_Do_Multi_TAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.09.08 
										Result = -103;
									}
									else {
//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_CM( 0 , k , &R_Curr_Chamber , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , &R_NextFinger , FALSE ); // 2004.11.23
										Result = SCHEDULING_CHECK_for_Enable_PICK_from_CM( 0 , k , &R_Curr_Chamber , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , &R_NextFinger , &R_ArmIntlks , FALSE , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx ); // 2004.11.23
									}
								}
								else {
									Result = -192;
								}
							}
							else {
								//
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2011.01.21
									if ( _SCH_MODULE_Get_TM_DoPointer( k ) >= MAX_CASSETTE_SLOT_SIZE ) {
										_SCH_MODULE_Set_TM_DoPointer( k , 0 );
									}
								}
								//
								Result = -101;
							}
						}
						else {
							Result = -193;
						}
						//----------------------------------------------------------------------
						if ( Result > 0 ) { // 2005.01.06
							//----------------------------------------------------------------------
							// 2005.01.04
							//----------------------------------------------------------------------
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) {
								if ( SCHEDULING_CHECK_CM_Another_No_More_Supply( k , _SCH_MODULE_Get_TM_DoPointer( k ) ) ) { // 2005.01.03
									//----------------------------------------------------------------------
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( k , 1 ); // 2003.05.28
									//----------------------------------------------------------------------
									Result = -194;
								}
							}
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
						}
					}
					else {
						Result = -195;
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 02 (k=%d) CHECK_PICK_from_CM = Res=%d,R_Curr_Chamber=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_ArmIntlks=%d,ret_rcm=%d,ret_pt=%d\n" , k , Result , R_Curr_Chamber , R_Next_Chamber , R_Next_Move , R_ArmIntlks , ret_rcm , ret_pt );
					//----------------------------------------------------------------------
					if ( Result >= 0 ) {
						j = 0;
						if ( _SCH_MODULE_FIRST_CONTROL_GET() && ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() > 0 ) ) {
							for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
//									if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( j ) != 0 ) ) continue;
								if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) ) continue;
								if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , j , G_PLACE ) ) continue;
								if ( _SCH_MODULE_FIRST_EXPEDITE_GET( j ) ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) > 0 ) {
										if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , j ) < _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() ) {
											j = 9999;
										}
									}
									else {
//										if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) { // 2014.01.10
										if ( !SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
											j = 9999;
										}
									}
								}
							}
							if ( _SCH_MODULE_Get_TM_WAFER(0,TA_STATION) > 0 ) j = 9999;
							if ( _SCH_MODULE_Get_TM_WAFER(0,TB_STATION) > 0 ) j = 9999;
							if ( j < 9999 ) {
								for ( wta = 0 , wtb = 0 , j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
//										if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( j ) != 0 ) ) continue;
									if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , j , G_PLACE ) ) continue;
									if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) ) continue;
									if ( _SCH_MODULE_FIRST_EXPEDITE_GET( j ) ) {
//										if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) { // 2014.01.10
										if ( !SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
											wta++;
										}
									}
									if ( _SCH_MODULE_SECOND_EXPEDITE_GET( j ) ) {
//										if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) <= 0 ) { // 2014.01.10
										if ( SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
											wtb++;
										}
									}
								}
								if ( wtb <= 0 ) {
									if ( wta > 1 ) j = 9999;
								}
								else {
									if ( wta > wtb ) j = 9999;
								}
							}
						}
						//=============================================================================================================
						// 2005.10.07
						//=============================================================================================================
						if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
							if ( ( j < 9999 ) && ( R_Curr_Chamber < PM1 ) ) {
//								if ( !_SCH_MODULE_GET_USE_ENABLE( R_Curr_Chamber , TRUE , k ) ) { // 2011.01.18
								if ( !_SCH_MODULE_GET_USE_ENABLE( R_Curr_Chamber , FALSE , k ) ) { // 2011.01.18
									j = 9999999;
									//
									SCHEDULER_CONTROL_Chk_DISABLE_DATABASE_SKIP_A0SUB2( k , _SCH_MODULE_Get_TM_DoPointer( k ) ); // 2011.01.18
									//
								}
								else {
									j = SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( R_Curr_Chamber , TRUE , TRUE , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_DoPointer( k ) ) , R_NextFinger , R_Curr_Chamber , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_DoPointer( k ) ) , _SCH_COMMON_GET_METHOD_ORDER( k , _SCH_MODULE_Get_TM_DoPointer( k ) ) );
									if      ( j == SYS_SUCCESS ) {
										j = 0;
									}
									else if ( j == SYS_ERROR ) {
										j = 9999999;
										Sleep(100);
									}
									else {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (PICK POSSIBLE Aborted) ....%cWHTMFAIL\n" , 9 );
										return 0;
									}
								}
							}
						}
						//=============================================================================================================
						//
						// 2018.07.10
						//
						MULTI_FIRST_Res = 0;
						//
						if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO5 ) {
							//
							MULTI_FIRST_Res = SCHEDULING_CHECK_for_Multi_Pick_First_Check( 0 , -1 , k );
							//
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 1002 (k=%d) SCHEDULING_CHECK_for_Multi_Pick_First_Check [%d]= MULTIPICK_FIRST=%d,P=%d,%d,%d\n" , k , MULTI_FIRST_Res , MULTIPICK_FIRST , 0 , -1 , k );
							//
							if ( MULTI_FIRST_Res >= 0 ) {
								//
								switch ( MULTI_FIRST_Res % 10 ) {
								case 1 :
									break;
								case 0 :
									//
									j = 9999;
									//
									break;
								case 2 :
									//
									j = 9999;
									//
									MULTIPICK_FIRST = FALSE;
									//
									break;
								}
								//
							}
						}
						//
						if ( j < 9999 ) {
							SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( R_Curr_Chamber );
							//printf( "1(CrossForm_PICK_from_CM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
							if ( SCH_Buff_Time < SCH_Small_Time ) {
								Enter_Side			= k;
								SCH_Mode			= 1;
								SCH_No				= R_Next_Chamber;
								SCH_Order			= R_Path_Order;
								SCH_Small_Time		= SCH_Buff_Time;
								SCH_NextFinger		= R_NextFinger;
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 )
									SCH_R_Time	= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
								else
									SCH_R_Time	= 0;
								SCH_PICKCM_ARMINT	= R_ArmIntlks;
								SCH_Chamber			= R_Curr_Chamber; // 2018.05.10
								//
								if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
								//
							}
							else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 1 ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 ) {
									if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) {
										Enter_Side			= k;
										SCH_Mode			= 1;
										SCH_No				= R_Next_Chamber;
										SCH_Order			= R_Path_Order;
										SCH_Small_Time		= SCH_Buff_Time;
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
										SCH_NextFinger		= R_NextFinger;
										SCH_PICKCM_ARMINT	= R_ArmIntlks;
										SCH_Chamber			= R_Curr_Chamber; // 2018.05.10
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
								}
								else {
									//
//									wta = _SCH_MODULE_Get_PM_WAFER( R_Next_Chamber , 0 ); // 2014.01.10
//									wtb = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
									//
									wta = SCH_PM_IS_FREE( R_Next_Chamber ) ? 0 : 1; // 2014.01.10
									wtb = SCH_PM_IS_FREE( SCH_No ) ? 0 : 1; // 2014.01.10
									//
									if      ( ( wta <= 0 ) && ( wtb > 0 ) ) {
										Enter_Side			= k;
										SCH_Mode			= 1;
										SCH_No				= R_Next_Chamber;
										SCH_Order			= R_Path_Order;
										SCH_Small_Time		= SCH_Buff_Time;
										SCH_R_Time			= 0;
										SCH_NextFinger		= R_NextFinger;
										SCH_PICKCM_ARMINT	= R_ArmIntlks;
										SCH_Chamber			= R_Curr_Chamber; // 2018.05.10
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
									else if ( ( wta <= 0 ) && ( wtb <= 0 ) ) {
										if ( _SCH_PRM_GET_Putting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Putting_Priority( SCH_No ) ) {
											Enter_Side			= k;
											SCH_Mode			= 1;
											SCH_No				= R_Next_Chamber;
											SCH_Order			= R_Path_Order;
											SCH_Small_Time		= SCH_Buff_Time;
											SCH_R_Time			= 0;
											SCH_NextFinger		= R_NextFinger;
											SCH_PICKCM_ARMINT	= R_ArmIntlks;
											SCH_Chamber			= R_Curr_Chamber; // 2018.05.10
											//
											if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
											//
										}
									}
									else if ( ( wta > 0 ) && ( wtb > 0 ) ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) <= 0 ) {
											if ( _SCH_PRM_GET_Getting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Getting_Priority( SCH_No ) ) {
												Enter_Side			= k;
												SCH_Mode			= 1;
												SCH_No				= R_Next_Chamber;
												SCH_Order			= R_Path_Order;
												SCH_Small_Time		= SCH_Buff_Time;
												SCH_R_Time			= 99999999;
												SCH_NextFinger		= R_NextFinger;
												SCH_PICKCM_ARMINT	= R_ArmIntlks;
												SCH_Chamber			= R_Curr_Chamber; // 2018.05.10
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
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
				if ( ( !_SCH_PRM_GET_MODULE_MODE( FM ) || ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO7 ) ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2018.12.07
//					for ( pca = 0 ; pca < 2 ; pca++ ) { // 2009.09.10
					for ( pca = ( MULTIPICK_FIRST ? 2 : 0 ) ; pca < 2 ; pca++ ) { // 2009.09.10 // 2018.05.10
						//
						for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
							//==================================================================================
							// 2009.09.10
							//==================================================================================
							if ( pca == 0 ) {
								pcaarm[i] = FALSE;
							}
							else {
								//
								if ( pcaarm[0] || !pcaarm[1] ) break;
								//
								if ( pcaarm[i] ) continue;
								//
							}
							//==================================================================================
							if ( ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 0 ) || ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) ) { // 2006.05.04
								if ( Waiting_Motion != -1 ) break;
							}
							//==================================================================================
							if ( _SCH_MODULE_Need_Do_Multi_TAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.09.08 
								if ( _SCH_MODULE_Get_MFAL_POINTER() == i ) continue;
							}
							//==================================================================================
							// 2007.01.24
							//==================================================================================
							if (
								( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) &&
								( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) &&
								( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
								( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == k )
								) { // 2007.01.24
								Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , FALSE , FALSE , 0 , &R_JumpMode , -1 , &R_Retmode , -1 );
							}
							//=================================================================================================
							else {
								if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { // 2002.07.23
									Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , FALSE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( 0 ) , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
								}
								else {
									Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , FALSE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
								}
							}
							//=================================================================================================
//							if ( ( Result == -99 ) && ( !SCH_WY ) ) { // 2013.10.25
//								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (No More Run Chamber 1) ....%cWHTMFAIL\n" , 9 ); // 2013.10.25
//								return 0; // 2013.10.25
//							} // 2013.10.25
							//==========================================================================================================
							// 2006.03.27
							//==========================================================================================================
							if ( ( Result >= 0 ) && ( R_Next_Chamber >= PM1 ) ) {
								if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , R_Next_Chamber ) >= 2 ) {
									if ( !SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( 0 , k , i , R_Next_Chamber ) ) Result = -1001;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM = Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d,SCH_Mode=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time , SCH_Mode );
							//----------------------------------------------------------------------
							//==========================================================================================================
							if ( Result >= 0 ) {
								//
								pcaarm[i] = TRUE; // 2009.09.10
								//
								//----------------------------------------------------------------------
								// 2006.07.10
								//----------------------------------------------------------------------
								if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PLACE_From_TM[k=%d,%d]=> NextCh=%d,Slot=%d,NextMove=%d,PathOrder=%d,JumpMode=%d,Retmode=%d\t\t\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_Path_Order , R_JumpMode , R_Retmode );
								}
								//----------------------------------------------------------------------
								//----------------------------------------------------------------------
								j = 0;
								if ( R_Next_Chamber < PM1 ) {
									if ( _SCH_MODULE_Need_Do_Multi_TAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.09.08 
										j = 9999;
									}
									else {
										if ( _SCH_MODULE_FIRST_CONTROL_GET() && ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() > 0 ) ) {
											if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) > 0 ) {
												for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
	//													if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( j ) != 0 ) ) continue;
													if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , j , G_PLACE ) ) continue;
													if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) ) continue;
													if ( _SCH_MODULE_FIRST_EXPEDITE_GET( j ) ) {
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) > 0 ) {
															if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , j ) < ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() / 2 ) ) {
																j = 9999;
															}
														}
														else {
//															if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) { // 2014.01.10
															if ( !SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
																j = 9999;
															}
														}
													}
												}
											}
										}
									}
								}
								else {
									if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
										if ( _SCH_MODULE_FIRST_CONTROL_GET() && ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() > 0 ) ) {
											if ( _SCH_MODULE_FIRST_EXPEDITE_GET( R_Next_Chamber ) ) {
												if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) > 0 ) {
													for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
	//														if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( j ) != 0 ) ) continue;
														if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE , k ) ) continue;
														if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , j , G_PLACE ) ) continue;
														if ( _SCH_MODULE_FIRST_EXPEDITE_GET( j ) ) {
															if ( _SCH_MACRO_CHECK_PROCESSING_INFO( j ) > 0 ) {
																wta = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , j );
																wtb = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , j );
																SCH_Buff_Time = _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME();
																if ( ( wta < SCH_Buff_Time ) && ( wtb < SCH_Buff_Time ) ) {
																	j = 9999;
																}
																else if ( wta < SCH_Buff_Time ) {
																	j = 9999;
																}
																else if ( wtb < SCH_Buff_Time ) {
																	j = 9999;
																}
															}
															else {
//																if ( _SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) { // 2014.01.10
																if ( !SCH_PM_IS_EMPTY( j ) ) { // 2014.01.10
																	j = 9999;
																}
																else {
																	if ( KPLT0_GET_PROCESS_TIME_OUT_WAIT( j ) < _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() ) j = 9999;
																}
															}
														}
													}
												}
											}
										}
									}
								}
								//=============================================================================================================
								// 2005.10.07
								//=============================================================================================================
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
									if ( ( j < 9999 ) && ( R_Next_Chamber < PM1 ) ) {
										//=============================================================================================================
										// 2007.01.15
										//=============================================================================================================
										j = 9999;
										if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) {
											if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) == R_Next_Chamber ) {
												for ( sl = 0 ; sl < 30 ; sl++ ) {
//													if ( !_SCH_MODULE_GET_USE_ENABLE( R_Next_Chamber , TRUE , k ) ) { // 2010.09.01
													if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , R_Next_Chamber , G_PLACE ) || !_SCH_MODULE_GET_USE_ENABLE( R_Next_Chamber , TRUE , k ) ) { // 2010.09.01
														j = 9999;
														break;
													}
													j = SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( R_Next_Chamber , FALSE , TRUE , R_Slot , i , _SCH_CLUSTER_Get_PathIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_COMMON_GET_METHOD_ORDER( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) );
													if      ( j == SYS_SUCCESS ) {
														_SCH_CLUSTER_Set_PathOut( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) , R_Next_Chamber );
														j = 0;
														break;
													}
													else if ( j == SYS_ERROR ) {
														j = 9999;
													}
													else {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (PLACE POSSIBLE Aborted) ....%cWHTMFAIL\n" , 9 );
														return 0;
													}
													Sleep(1000);
												}
											}
										}
										//=============================================================================================================
										if ( j >= 9999 ) { // 2007.01.15
										//=============================================================================================================
											if ( _SCH_CLUSTER_Get_PathHSide( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) ) { // 2008.04.02
												//
												for ( mb = 0 ; mb < 2 ; mb++ ) {
													//
													for ( sl = 0 ; sl < MAX_CASSETTE_SIDE ; sl++ ) {
														//
														if ( mb == 0 ) {
															if ( R_Next_Chamber != ( CM1 + sl ) ) continue;
														}
														else {
															if ( R_Next_Chamber == ( CM1 + sl ) ) continue;
														}
														//
														if ( !_SCH_MODULE_GET_USE_ENABLE( CM1 + sl , TRUE , k ) ) continue;
														if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , CM1 + sl , G_PLACE ) ) continue; // 2010.09.01
														//
														j = SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( CM1 + sl , FALSE , TRUE , R_Slot , i , _SCH_CLUSTER_Get_PathIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_COMMON_GET_METHOD_ORDER( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) );
														//
														if      ( j == SYS_SUCCESS ) {
															_SCH_CLUSTER_Set_PathOut( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) , CM1 + sl );
															mb = 99;
															j = 0;
															break;
														}
														else if ( j == SYS_ERROR ) {
															j = 9999;
															Sleep(100);
														}
														else {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (PLACE POSSIBLE Aborted) ....%cWHTMFAIL\n" , 9 );
															return 0;
														}
													}
												}
											}

	/*
	// 2008.04.02

											if ( !_SCH_MODULE_GET_USE_ENABLE( R_Next_Chamber , TRUE , k ) ) {
												if ( _SCH_CLUSTER_Get_PathHSide( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) ) {
													if ( R_Next_Chamber == CM1 ) {
														R_Next_Chamber = CM1 + 1;
														j = 0; // 2007.01.15
													}
													else {
														R_Next_Chamber = CM1;
														j = 0; // 2007.01.15
													}
													if ( !_SCH_MODULE_GET_USE_ENABLE( R_Next_Chamber , TRUE , k ) ) {
														j = 999999;
													}
												}
												else {
													j = 999999;
												}
											}
											else { // 2007.01.15
												j = 0; // 2007.01.15
											} // 2007.01.15
											if ( j < 9999 ) {
												j = SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( R_Next_Chamber , FALSE , TRUE , R_Slot , i , _SCH_CLUSTER_Get_PathIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_COMMON_GET_METHOD_ORDER( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) );
												if      ( j == SYS_SUCCESS ) {
													_SCH_CLUSTER_Set_PathOut( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) , R_Next_Chamber );
													j = 0;
												}
												else if ( j == SYS_ERROR ) {
													if ( _SCH_CLUSTER_Get_PathHSide( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) ) {
														if ( R_Next_Chamber == CM1 ) {
															R_Next_Chamber = CM1 + 1;
														}
														else {
															R_Next_Chamber = CM1;
														}
														if ( !_SCH_MODULE_GET_USE_ENABLE( R_Next_Chamber , TRUE , k ) ) {
															j = 999999;
															Sleep(100);
														}
														else {
															j = SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( R_Next_Chamber , FALSE , TRUE , R_Slot , i , _SCH_CLUSTER_Get_PathIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_CLUSTER_Get_SlotIn( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) , _SCH_COMMON_GET_METHOD_ORDER( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) ) );
															if      ( j == SYS_SUCCESS ) {
																_SCH_CLUSTER_Set_PathOut( k , _SCH_MODULE_Get_TM_POINTER( 0,i ) , R_Next_Chamber );
																j = 0;
															}
															else if ( j == SYS_ERROR ) {
																j = 999999;
																Sleep(100);
															}
															else {
																_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (PLACE POSSIBLE Aborted) ....%cWHTMFAIL\n" , 9 );
																return 0;
															}
														}
													}
													else {
														j = 999999;
														Sleep(100);
													}
												}
												else {
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted (PLACE POSSIBLE Aborted) ....%cWHTMFAIL\n" , 9 );
													return 0;
												}
											}
	*/

										}
									}
								}
								//=============================================================================================================
								if ( j < 9999 ) {
									SCH_Buff_Time = _SCH_PRM_GET_Putting_Priority( R_Next_Chamber );
									R_Switch = FALSE;
									if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) {
										if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) == R_Next_Chamber ) {
											R_Switch = TRUE;
											SCH_Buff_Time = -9999;
										}
									}
									//=================================================================================
									// 2006.07.20
									//=================================================================================
									if ( SCH_Buff_Time != -9999 ) {
										if ( SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( 0 , i ) ) SCH_Buff_Time = -9999;
									}
									//=================================================================================
									//printf( "2(CrossForm_PLACE_From_TM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
									if ( SCH_Buff_Time < SCH_Small_Time ) {
										if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
											Enter_Side		= k;
											SCH_Mode		= 2;
											SCH_No			= i;
											SCH_Chamber		= R_Next_Chamber;
											SCH_Small_Time	= SCH_Buff_Time;
											SCH_Type        = R_Next_Move;
											SCH_Order		= R_Path_Order;
											SCH_JumpMode	= R_JumpMode;
											SCH_R_Time	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
											SCH_SWITCH		= R_Switch;
//											SCH_RETMODE		= R_Retmode; // 2003.10.21 // 2019.01.30
											SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2003.10.21 // 2019.01.30
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK1]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
											//----------------------------------------------------------------------
										}
										else {
											//====================================================================================================================================
											// 2005.12.15
											//====================================================================================================================================
											Enter_Side		= k;
											SCH_Mode		= 12;
											SCH_No			= i;
											SCH_Chamber		= R_Next_Chamber;
											SCH_Slot		= R_Slot;
											SCH_Small_Time	= SCH_Buff_Time;
											SCH_Type        = R_Next_Move;
											SCH_Order		= R_Path_Order;
											SCH_R_Time	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
											SCH_JumpMode	= R_JumpMode;
											SCH_SWITCH		= R_Switch;
//											SCH_RETMODE		= R_Retmode; // 2019.01.30
											SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
											if ( R_Switch ) i = 99;
											//====================================================================================================================================
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK2]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
											//----------------------------------------------------------------------
										}
									}
	//									else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 2 ) ) { // 2005.12.15
									else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( ( ( SCH_Mode == 2 ) && ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) ) || ( ( SCH_Mode == 12 ) && ( ( R_JumpMode == 1 ) || ( R_Next_Chamber >= BM1 ) ) ) ) ) { // 2005.12.15
										if ( ( Enter_Side == k ) && !( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) ) { // 2003.05.26 // 2004.01.29
											//-----------------------------------------------------------------------------
											//----------- 2002.04.08
											//-----------------------------------------------------------------------------
											if ( ( SCH_Chamber == R_Next_Chamber ) && ( SCH_Type == R_Next_Move ) && ( SCH_Type == SCHEDULER_MOVING_OUT ) ) { // 2002.04.08
												switch( _SCH_ROBOT_GET_FAST_PLACE_FINGER( 0 , R_Next_Chamber ) ) {
												case TA_STATION :
													j = 9999;
													break;
												case TB_STATION :
													j = 9999;
													if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
														Enter_Side		= k;
														SCH_Mode		= 2;
														SCH_No			= i;
														SCH_Chamber		= R_Next_Chamber;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_Type        = R_Next_Move;
														SCH_Order		= R_Path_Order;
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
														SCH_JumpMode	= R_JumpMode;
														SCH_SWITCH		= R_Switch;
			//											SCH_RETMODE		= R_Retmode; // 2019.01.30
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK3]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
														//----------------------------------------------------------------------
													}
													else { // 2005.12.15
														Enter_Side		= k;
														SCH_Mode		= 12;
														SCH_No			= i;
														SCH_Chamber		= R_Next_Chamber;
														SCH_Slot		= R_Slot;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_Type        = R_Next_Move;
														SCH_Order		= R_Path_Order;
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
														SCH_JumpMode	= R_JumpMode;
														SCH_SWITCH		= R_Switch;
			//											SCH_RETMODE		= R_Retmode; // 2019.01.30
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														if ( R_Switch ) i = 99;
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK4]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
														//----------------------------------------------------------------------
													}
													break;
												}
											}
										}
										//-----------------------------------------------------------------------------
										if ( j < 9999 ) {
											wsa = _SCH_MODULE_Get_TM_SIDE( 0,SCH_No );
											wsb = _SCH_MODULE_Get_TM_SIDE( 0,i );
											if ( wsa == wsb ) {
												/*
												//
												// 2016.08.26
												//
												//
												wta = _SCH_MODULE_Get_TM_POINTER(0,SCH_No) + _SCH_MODULE_Get_TM_POINTER2(0,SCH_No)*100;
												wtb = _SCH_MODULE_Get_TM_POINTER(0,i) + _SCH_MODULE_Get_TM_POINTER2(0,i)*100;
												if      ( ( wta / 100 ) >= ( wta % 100 ) ) wta = wta / 100;
												else if ( ( wta / 100 ) <= ( wta % 100 ) ) wta = wta % 100;
												if      ( ( wtb / 100 ) >= ( wtb % 100 ) ) wtb = wtb / 100;
												else if ( ( wtb / 100 ) <= ( wtb % 100 ) ) wtb = wtb % 100;
												//
												*/
												//
												//
												// 2016.08.26
												//
												//
												wta = _SCH_CLUSTER_Get_SupplyID( wsa , _SCH_MODULE_Get_TM_POINTER(0,SCH_No) );
												wtb = _SCH_CLUSTER_Get_SupplyID( wsb , _SCH_MODULE_Get_TM_POINTER(0,i) );
												//
												if ( wta > wtb ) {
													if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
														Enter_Side		= k;
														SCH_Mode		= 2;
														SCH_No			= i;
														SCH_Chamber		= R_Next_Chamber;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_Type        = R_Next_Move;
														SCH_Order		= R_Path_Order;
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
														SCH_JumpMode	= R_JumpMode;
														SCH_SWITCH		= R_Switch;
			//											SCH_RETMODE		= R_Retmode; // 2019.01.30
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK5]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
														//----------------------------------------------------------------------
													}
													else { // 2005.12.15
														Enter_Side		= k;
														SCH_Mode		= 12;
														SCH_No			= i;
														SCH_Chamber		= R_Next_Chamber;
														SCH_Slot		= R_Slot;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_Type        = R_Next_Move;
														SCH_Order		= R_Path_Order;
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
														SCH_JumpMode	= R_JumpMode;
														SCH_SWITCH		= R_Switch;
		//												SCH_RETMODE		= R_Retmode; // 2019.01.30
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														if ( R_Switch ) i = 99;
														//----------------------------------------------------------------------
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK6]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
														//----------------------------------------------------------------------
													}
												}
											}
											else {
												if ( SCH_Chamber == R_Next_Chamber ) {
													if ( _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , SCH_No ) < _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , i ) ) {
														if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
															Enter_Side		= k;
															SCH_Mode		= 2;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Small_Time	= SCH_Buff_Time;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
															SCH_SWITCH		= R_Switch;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK7]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
															//----------------------------------------------------------------------
														}
														else { // 2005.12.15
															Enter_Side		= k;
															SCH_Mode		= 12;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Slot		= R_Slot;
															SCH_Small_Time	= SCH_Buff_Time;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
															SCH_SWITCH		= R_Switch;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
															if ( R_Switch ) i = 99;
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK8]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
															//----------------------------------------------------------------------
														}
													}
												}
												else {
													if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ) ) {
														if ( ( R_JumpMode == 0 ) && ( R_Next_Chamber < BM1 ) ) { // 2005.12.15
															Enter_Side		= k;
															SCH_Mode		= 2;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Small_Time	= SCH_Buff_Time;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
															SCH_SWITCH		= R_Switch;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK9]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
															//----------------------------------------------------------------------
														}
														else { // 2005.12.15
															Enter_Side		= k;
															SCH_Mode		= 12;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Slot		= R_Slot;
															SCH_Small_Time	= SCH_Buff_Time;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
															SCH_SWITCH		= R_Switch;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
															if ( R_Switch ) i = 99;
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 03 (k=%d) CHECK_PLACE_From_TM [OK10]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time );
															//----------------------------------------------------------------------
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
						//====================================================================================================================================
					} // 2009.09.10
					//====================================================================================================================================
					if ( _SCH_MODULE_Need_Do_Multi_TAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.09.08 
						if ( _SCH_MODULE_Get_MFAL_SIDE() == k ) {
							R_Retmode = _SCH_MACRO_CHECK_TM_MALIGNING( CHECKING_SIDE , FALSE );
							if ( R_Retmode != SYS_RUNNING ) {
								Enter_Side		= k;
								SCH_Mode		= 51;
								SCH_RETMODE		= R_Retmode;
								SCH_Small_Time	= -99999999;
							}
						}
					}
					//====================================================================================================================================
				}
				//========================================================================================================================
				// *CHECK* : PICK FROM BM (FM Mode)
				//========================================================================================================================
				if ( _SCH_PRM_GET_MODULE_MODE( FM ) || Get_Prm_MODULE_BUFFER_USE_MODE() ) { // 2005.12.15
//					else { // FM mode // 2005.12.15
					if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) != -1 ) ) { // 2002.10.15
						Result = -193;
					}
					else {
						//
						SKIP_BM_CHECK_RESETTING( -99 , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
						//
						while ( TRUE ) { // 2013.12.06
							//
							Skip_Check_Again = FALSE; // 2013.12.06
							//
							R_Curr_Chamber = -1; // 2012.09.27
							//
	//							if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
							if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
	//								if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) ) { // 2003.11.09 // 2004.06.01
	//								if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_BATCH_PART ) ) { // 2003.11.09 // 2004.06.01 // 2005.01.12
								if ( ( _SCH_SYSTEM_MODE_END_GET( k ) == 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_BATCH_PART ) ) { // 2003.11.09 // 2004.06.01 // 2005.01.14
									if ( Waiting_Motion == -1 ) {
										if ( KPLT0_CHECK_CASSETTE_TIME_SUPPLY( _SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() ) ) {
											//=================================================================================================
											// 2006.09.07
											//=================================================================================================
											if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
												if ( _SCH_MODULE_Chk_TM_Finish_Status( k ) ) { // 2007.03.27
													Result = -198; // 2007.03.27
												} // 2007.03.27
												else {
													if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( k , TRUE , 2 ) ) {
														if ( ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) && ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == k ) ) { // 2007.01.24
	//														Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , 0 , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); // 2013.11.12
															Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , 0 , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
														}
														else {
	//														Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); // 2013.11.12
															Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
														}
													}
													else {
														Result = -199;
													}
												}
												if ( Result > 0 ) { // 2007.10.03
													j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable );
													if ( j > 0 ) {
														//
														Result = -194 - j;
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); // 2012.09.26
														//
														Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
														//
													}
													else {
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); // 2012.11.09
														//
													}
												}
												else { // 2012.09.27 // 2012.11.09
													if ( ( R_Curr_Chamber >= BM1 ) && ( R_Curr_Chamber < TM ) ) {
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber );
														//
														Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
														//
													}
												}
												//
												if ( Result > 0 ) { // 2007.10.03
													//
													j = _SCH_COMMON_CHECK_PICK_FROM_FM( k , R_Schpt , 2 );
													if ( j < 0 ) {
														//
														Result = -10000 + j; // 2007.11.15
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); // 2012.09.27
														//
													}
												}
											}
											//=================================================================================================
											else {
												if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) { // 2007.02.01
													//============================================================================
	//												Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); // 2013.11.12

													if ( MULTI_ALL_PM_FULLSWAP && ( SCH_Mode == 0 ) && ( MULTI_ALL_PM_LAST_PICK[0] == SCH_No ) ) { // 2018.10.20
														Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
													}
													else {
														Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
													}

													//============================================================================
												}
												else { // 2007.02.01
													if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) >= 1000 ) { // 2007.02.01
														//============================================================================
	//													Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE ); // 2013.11.12

														if ( MULTI_ALL_PM_FULLSWAP && ( SCH_Mode == 0 ) && ( MULTI_ALL_PM_LAST_PICK[0] == SCH_No ) ) { // 2018.10.20
															Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
														}
														else {
															Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
														}

														//============================================================================
													}
													else {
														//============================================================================
	//													Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE ); // 2013.11.12
														Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
														//============================================================================
													}
												}
												if ( Result < 0 ) { // 2004.10.14
													if ( k == CHECKING_SIDE ) {
														SCHEDULING_CHECK_BM_Change_FMSIDE_for_Switch( 0 , CHECKING_SIDE );
													}
													//
													if ( ( R_Curr_Chamber >= BM1 ) && ( R_Curr_Chamber < TM ) ) { // 2012.09.27 // 2012.11.09
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber );
														//
														Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
														//
													}
													//
												}
												else { // 2007.01.15
													j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable );
													if ( j > 0 ) {
														//
														Result = -195 - j;
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); // 2012.09.26
														//
														Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
														//
													}
													else {
														//
														_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); // 2012.11.09
														//
													}
												}
												//============================================================================
											}
										}
										else {
											Result = -192;
										}
									}
									else {
										Result = -193;
									}
								}
								else {
									Result = -194;
								}
								if ( Result >= 0 ) { // 2004.06.15
									//
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [A]= Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d,SCH_Mode=%d,[%d,%d,%d,%d,%d,%d,%d,%d]\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time , SCH_Mode , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 , Skip_Check_Again ); // 2014.10.28
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2004.06.15
										if (
	//										( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) || // 2011.04.08
	//										( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) 
											( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) // 2011.04.08
											) { // 2010.05.03
											//
											//if ( SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( k , R_Curr_Chamber , R_Next_Move ) ) { // 2004.06.15 // 2011.04.08
//											j = SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( k , R_Curr_Chamber , R_Next_Move ); // 2011.04.08 // 2016.10.20
											j = SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( k , 0 , R_Curr_Chamber , R_Next_Move ); // 2011.04.08 // 2016.10.20
											//
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [B]= j=%d\n" , k , j ); // 2014.10.28
//
											if ( j > 0 ) {
		//										Result = -195; // 2008.09.26
//												Result = -197; // 2008.09.26 // 2017.01.11
												Result = -197 - j; // 2008.09.26 // 2017.01.11
												//
												_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); // 2012.09.27
												//
												Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
												//
											}
										}
									}
								}
							}
							else {
								if ( Waiting_Motion == -1 ) {
									if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) { // 2007.02.01
	//									Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); // 2013.11.12
										Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
									}
									else { // 2007.02.01
										if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) >= 1000 ) { // 2007.02.01
	//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE ); // 2013.11.12
											Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
										}
										else { // 2007.02.01
	//										Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE ); // 2013.11.12
											Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
										}
									}
									if ( Result > 0 ) { // 2007.01.15
										j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable );
										//
										if ( j > 0 ) {
											//
											Result = -196 - j;
											//
											_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); // 2012.09.26
											//
											Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
											//
										}
										else {
											//
											_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); // 2012.11.09
											//
										}
									}
									else { // 2012.09.27 // 2012.11.09
										if ( ( R_Curr_Chamber >= BM1 ) && ( R_Curr_Chamber < TM ) ) {
											//
											_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber );
											//
											Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); // 2013.12.06
											//
										}
									}
								}
								else {
									Result = -196;
								}
							}
							//
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d,SCH_Mode=%d,[%d,%d,%d,%d,%d,%d,%d,%d]\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time , SCH_Mode , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 , Skip_Check_Again ); // 2014.10.28
							//----------------------------------------------------------------------
							if ( !Skip_Check_Again ) break; // 2013.12.06
							//
						}
					}
					//----------------------------------------------------------------------
//					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d,SCH_Mode=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time , SCH_Mode );
					//----------------------------------------------------------------------
					//
					//
					//
					// 2018.07.10
					//
					MULTI_FIRST_Res = 0;
					//
					if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO5 ) {
						//
						if ( Result >= 0 ) {
							//
							MULTI_FIRST_Res = SCHEDULING_CHECK_for_Multi_Pick_First_Check( 0 , R_Curr_Chamber  , R_Slot );
							//
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 1003 (k=%d) SCHEDULING_CHECK_for_Multi_Pick_First_Check [%d]= MULTIPICK_FIRST=%d,P=%d,%d,%d\n" , k , MULTI_FIRST_Res , MULTIPICK_FIRST , 0 , R_Curr_Chamber , R_Slot );
							//
							if ( MULTI_FIRST_Res >= 0 ) {
								//
								switch ( MULTI_FIRST_Res % 10 ) {
								case 1 :
									break;
								case 0 :
									//
									Result = -180712;
									//
									break;
								case 2 :
									//
									Result = -180712;
									//
									MULTIPICK_FIRST = FALSE;
									//
									break;
								}
								//
							}
						}
					}
					//
					if ( MULTI_ALL_PM_FULLSWAP && ( Result >= 0 ) && ( SCH_Mode == 0 ) && ( R_JumpMode > 0 ) ) { // 2018.10.20
						//
						switch ( MULTIL_BM_PM_SUPPLYID_BM_FAST_CHECK( SCH_No , _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) ) ) {
						case 0 : // bm
							//
							if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_BM[M][k=%d,%d]=> BM%d,Slot=%d,NextCh=%d,NextMove=%d,PathOrder=%d,Schpt=%d,JumpMode=%d\t\t\n" , k , Result , R_Curr_Chamber - BM1 + 1 , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode );
							}
							//
							Enter_Side			= k;
							SCH_Mode			= 11;
							SCH_Chamber			= R_Curr_Chamber;
							SCH_No				= R_Next_Chamber;
							SCH_Slot			= R_Slot;
							SCH_Order			= R_Path_Order;
							SCH_Small_Time		= SCH_Buff_Time;
							SCH_SchPt			= R_Schpt;
							SCH_JumpMode		= R_JumpMode;
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 )
								SCH_R_Time	= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
							else
								SCH_R_Time	= 0;
							SCH_Switchable = R_Switchable; // 2007.01.16
							SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK1] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
							//----------------------------------------------------------------------
							//
							if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
							//
							//
							Result = -1;
							//
							break;

						case 1 : // pm
							//
							Result = -1;
							//
							break;
						}
						//
					}
					//
					if ( Result >= 0 ) {
						//----------------------------------------------------------------------
						// 2006.07.10
						//----------------------------------------------------------------------
						if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_BM[k=%d,%d]=> BM%d,Slot=%d,NextCh=%d,NextMove=%d,PathOrder=%d,Schpt=%d,JumpMode=%d\t\t\n" , k , Result , R_Curr_Chamber - BM1 + 1 , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode );
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( R_Curr_Chamber );
						//printf( "11(CrossForm_PICK_from_BM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
						if ( SCH_Buff_Time < SCH_Small_Time ) {
							Enter_Side			= k;
							SCH_Mode			= 11;
							SCH_Chamber			= R_Curr_Chamber;
							SCH_No				= R_Next_Chamber;
							SCH_Slot			= R_Slot;
							SCH_Order			= R_Path_Order;
							SCH_Small_Time		= SCH_Buff_Time;
							SCH_SchPt			= R_Schpt;
							SCH_JumpMode		= R_JumpMode;
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 )
								SCH_R_Time	= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
							else
								SCH_R_Time	= 0;
							SCH_Switchable = R_Switchable; // 2007.01.16
							SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK1] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
							//----------------------------------------------------------------------
							//
							if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
							//
						}
						else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 11 ) ) {
							if ( ( Enter_Side != k ) && ( SCH_Chamber == R_Curr_Chamber ) ) { // 2003.05.29
								if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.07.04
									Enter_Side			= k;
									SCH_Mode			= 11;
									SCH_Chamber			= R_Curr_Chamber;
									SCH_No				= R_Next_Chamber;
									SCH_Slot			= R_Slot;
									SCH_Order			= R_Path_Order;
									SCH_Small_Time		= SCH_Buff_Time;
									SCH_SchPt			= R_Schpt;
									SCH_JumpMode		= R_JumpMode;
									SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
									SCH_Switchable = R_Switchable; // 2007.01.16
									SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK2] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
									//----------------------------------------------------------------------
									//
									if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
									//
								}
								else {
									//
									if ( !MULTI_ALL_PM_FULLSWAP ) { // 2019.01.08
										//
										if ( SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( R_Curr_Chamber ) ) == 2 ) { // 2003.06.02
											if ( SCH_Slot < R_Slot ) {
												Enter_Side			= k;
												SCH_Mode			= 11;
												SCH_Chamber			= R_Curr_Chamber;
												SCH_No				= R_Next_Chamber;
												SCH_Slot			= R_Slot;
												SCH_Order			= R_Path_Order;
												SCH_Small_Time		= SCH_Buff_Time;
												SCH_SchPt			= R_Schpt;
												SCH_JumpMode		= R_JumpMode;
												SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK3] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
												//----------------------------------------------------------------------
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
										}
										else {
											if ( SCH_Slot > R_Slot ) {
												Enter_Side			= k;
												SCH_Mode			= 11;
												SCH_Chamber			= R_Curr_Chamber;
												SCH_No				= R_Next_Chamber;
												SCH_Slot			= R_Slot;
												SCH_Order			= R_Path_Order;
												SCH_Small_Time		= SCH_Buff_Time;
												SCH_SchPt			= R_Schpt;
												SCH_JumpMode		= R_JumpMode;
												SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK4] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
												//----------------------------------------------------------------------
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
										}
										//
									}
								}
							}
							else if ( ( Enter_Side != k ) && ( SCH_Chamber != R_Curr_Chamber ) ) { // 2004.12.08
								//========================================================================
								// 2007.12.04
								//========================================================================
								if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) ) {
									if ( ( SCH_Type > 0 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) > 0 ) ) { // 2008.07.04
										if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.04.17
											Enter_Side			= k;
											SCH_Mode			= 11;
											SCH_Chamber			= R_Curr_Chamber;
											SCH_No				= R_Next_Chamber;
											SCH_Slot			= R_Slot;
											SCH_Order			= R_Path_Order;
											SCH_Small_Time		= SCH_Buff_Time;
											SCH_SchPt			= R_Schpt;
											SCH_JumpMode		= R_JumpMode;
											SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
											SCH_Switchable = R_Switchable; // 2007.01.16
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK5] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
											//----------------------------------------------------------------------
											//
											if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
											//
										}
									}
									else {
										if ( SCH_Type < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) ) {
											Enter_Side			= k;
											SCH_Mode			= 11;
											SCH_Chamber			= R_Curr_Chamber;
											SCH_No				= R_Next_Chamber;
											SCH_Slot			= R_Slot;
											SCH_Order			= R_Path_Order;
											SCH_Small_Time		= SCH_Buff_Time;
											SCH_SchPt			= R_Schpt;
											SCH_JumpMode		= R_JumpMode;
											SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
											SCH_Switchable = R_Switchable; // 2007.01.16
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK6] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
											//----------------------------------------------------------------------
											//
											if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
											//
										}
										//else { // 2008.01.14
										else if ( SCH_Type == SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) ) { // 2008.01.14
											if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.04.17
												Enter_Side			= k;
												SCH_Mode			= 11;
												SCH_Chamber			= R_Curr_Chamber;
												SCH_No				= R_Next_Chamber;
												SCH_Slot			= R_Slot;
												SCH_Order			= R_Path_Order;
												SCH_Small_Time		= SCH_Buff_Time;
												SCH_SchPt			= R_Schpt;
												SCH_JumpMode		= R_JumpMode;
												SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK7] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
												//----------------------------------------------------------------------
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
											else if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) == _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.04.17
												if ( _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , SCH_Chamber ) < _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , R_Curr_Chamber ) ) {
													Enter_Side			= k;
													SCH_Mode			= 11;
													SCH_Chamber			= R_Curr_Chamber;
													SCH_No				= R_Next_Chamber;
													SCH_Slot			= R_Slot;
													SCH_Order			= R_Path_Order;
													SCH_Small_Time		= SCH_Buff_Time;
													SCH_SchPt			= R_Schpt;
													SCH_JumpMode		= R_JumpMode;
													SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
													SCH_Switchable = R_Switchable; // 2007.01.16
													SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
													//----------------------------------------------------------------------
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK8] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
													//----------------------------------------------------------------------
													//
													if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
													//
												}
											}
										}
									}
								}
								//========================================================================
								else {
									if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.04.17
										Enter_Side			= k;
										SCH_Mode			= 11;
										SCH_Chamber			= R_Curr_Chamber;
										SCH_No				= R_Next_Chamber;
										SCH_Slot			= R_Slot;
										SCH_Order			= R_Path_Order;
										SCH_Small_Time		= SCH_Buff_Time;
										SCH_SchPt			= R_Schpt;
										SCH_JumpMode		= R_JumpMode;
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
										SCH_Switchable = R_Switchable; // 2007.01.16
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK9] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
										//----------------------------------------------------------------------
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
									else if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) == _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { // 2008.04.17
										if ( _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , SCH_Chamber ) < _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , R_Curr_Chamber ) ) {
											Enter_Side			= k;
											SCH_Mode			= 11;
											SCH_Chamber			= R_Curr_Chamber;
											SCH_No				= R_Next_Chamber;
											SCH_Slot			= R_Slot;
											SCH_Order			= R_Path_Order;
											SCH_Small_Time		= SCH_Buff_Time;
											SCH_SchPt			= R_Schpt;
											SCH_JumpMode		= R_JumpMode;
											SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
											SCH_Switchable = R_Switchable; // 2007.01.16
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK10] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
											//----------------------------------------------------------------------
											//
											if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
											//
										}
									}
								}
							}
							else {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 ) {
									if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) {
										Enter_Side			= k;
										SCH_Mode			= 11;
										SCH_Chamber			= R_Curr_Chamber;
										SCH_No				= R_Next_Chamber;
										SCH_Slot			= R_Slot;
										SCH_Order			= R_Path_Order;
										SCH_Small_Time		= SCH_Buff_Time;
										SCH_SchPt			= R_Schpt;
										SCH_JumpMode		= R_JumpMode;
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
										SCH_Switchable = R_Switchable; // 2007.01.16
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK11] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
										//----------------------------------------------------------------------
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
								}
								else {
									//
//									wta = _SCH_MODULE_Get_PM_WAFER( R_Next_Chamber , 0 ); // 2014.01.10
//									wtb = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); // 2014.01.10
									//
									wta = SCH_PM_IS_FREE( R_Next_Chamber ) ? 0 : 1; // 2014.01.10
									wtb = SCH_PM_IS_FREE( SCH_No ) ? 0 : 1; // 2014.01.10
									//
									if      ( ( wta <= 0 ) && ( wtb > 0 ) ) {
										Enter_Side			= k;
										SCH_Mode			= 11;
										SCH_Chamber			= R_Curr_Chamber;
										SCH_No				= R_Next_Chamber;
										SCH_Slot			= R_Slot;
										SCH_Order			= R_Path_Order;
										SCH_Small_Time		= SCH_Buff_Time;
										SCH_SchPt			= R_Schpt;
										SCH_JumpMode		= R_JumpMode;
										SCH_R_Time			= 0;
										SCH_Switchable = R_Switchable; // 2007.01.16
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK12] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
										//----------------------------------------------------------------------
										//
										if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
										//
									}
									else if ( ( wta <= 0 ) && ( wtb <= 0 ) ) {
										if ( _SCH_PRM_GET_Putting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Putting_Priority( SCH_No ) ) {
											Enter_Side			= k;
											SCH_Mode			= 11;
											SCH_Chamber			= R_Curr_Chamber;
											SCH_No				= R_Next_Chamber;
											SCH_Slot			= R_Slot;
											SCH_Order			= R_Path_Order;
											SCH_Small_Time		= SCH_Buff_Time;
											SCH_SchPt			= R_Schpt;
											SCH_JumpMode		= R_JumpMode;
											SCH_R_Time			= 0;
											SCH_Switchable = R_Switchable; // 2007.01.16
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK13] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
											//----------------------------------------------------------------------
											//
											if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
											//
										}
									}
									else if ( ( wta > 0 ) && ( wtb > 0 ) ) {
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) <= 0 ) {
											if ( _SCH_PRM_GET_Getting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Getting_Priority( SCH_No ) ) {
												Enter_Side			= k;
												SCH_Mode			= 11;
												SCH_Chamber			= R_Curr_Chamber;
												SCH_No				= R_Next_Chamber;
												SCH_Slot			= R_Slot;
												SCH_Order			= R_Path_Order;
												SCH_Small_Time		= SCH_Buff_Time;
												SCH_SchPt			= R_Schpt;
												SCH_JumpMode		= R_JumpMode;
												SCH_R_Time			= 99999999;
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); // 2007.12.05
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [OK14] = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d\n" , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time );
												//----------------------------------------------------------------------
												//
												if ( ( MULTI_FIRST_Res % 10 ) == 1 ) MULTIPICK_FIRST = TRUE;
												//
											}
										}
									}
									//
								}
							}
						}
						//
					}
				} // 2005.12.15
				//=======================================================================================================================================
				//=======================================================================================================================================
				// *CHECK* : PLACE FROM TM (FM Mode)
				//========================================================================================================================
//				if ( _SCH_PRM_GET_MODULE_MODE( FM ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2005.12.15 // 2007.02.01 // 2018.12.07
				if ( ( _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2018.12.07
//					if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2005.12.15 // 2007.02.01
					//
					nomorerun_tag = -1; // 2008.01.08
					//
//					for ( pca = 0 ; pca < 2 ; pca++ ) { // 2009.09.10
					for ( pca = ( MULTIPICK_FIRST ? 2 : 0 ) ; pca < 2 ; pca++ ) { // 2009.09.10 // 2018.05.10
						//
						for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
							//==================================================================================
							// 2009.09.10
							//==================================================================================
							if ( pca == 0 ) {
								pcaarm[i] = FALSE;
							}
							else {
								//
								if ( pcaarm[0] || !pcaarm[1] ) break;
								//
								if ( pcaarm[i] ) continue;
								//
							}
							//==================================================================================
							//==================================================================================
							if ( ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 0 ) || ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) ) { // 2006.05.04
								if ( Waiting_Motion != -1 ) break;
							}
							//==================================================================================
							//================================================================================================
							//================================================================================================
							// 2004.12.06
							//================================================================================================
							//================================================================================================
	/*
	// 2008.10.28
							if ( global_bmckmode == 0 ) { // 2004.12.06
								global_bmckmode = 2;
	//								if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() ) { // 2006.12.21
								if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
									if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
										if ( SCHEDULING_CHECK_AllBM_Pump_and_hasPlaceSpace_for_Switch_SingleSwap() ) global_bmckmode = 1;
									}
								}
							}
	*/
							//================================================================================================
							// 2007.01.24
							//================================================================================================
							if (
								( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) &&
								( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) &&
								( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
								( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == k )
								) { // 2007.01.24
								Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , 0 , &R_JumpMode , -1 , &R_Retmode , -1 );
							}
							//================================================================================================
							else {
	/*
	// 2008.10.28
								if ( global_bmckmode == 1 ) { // 2004.12.06
									if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) >= BM1 ) {
										Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode );
									}
									else {
										if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { // 2002.07.23
											Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( 0 ) , &R_Retmode );
										}
										else {
											Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode );
										}
									}
								}
								else {
	*/
									if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { // 2002.07.23
										Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( 0 ) , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
									}
									else {
										Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
									}
	//							} // 2008.10.28
							}
							//----------------------------------------------------------------------
							if ( ( Result == -99 ) && ( !SCH_WY ) ) {
								if ( nomorerun_tag == -1 ) nomorerun_tag = i;
	//								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted from Arm %c(No More Run Chamber 2) ....%cWHTMFAIL\n" , i + 'A' , 9 ); // 2008.01.08
	//								return 0; // 2008.01.08
							}
							else {
								nomorerun_tag = -1;
							}
							//==========================================================================================================
							// 2006.03.27
							//==========================================================================================================
							if ( ( Result >= 0 ) && ( R_Next_Chamber >= PM1 ) ) {
								if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , R_Next_Chamber ) >= 2 ) {
									if ( !SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( 0 , k , i , R_Next_Chamber ) ) Result = -1001;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM = Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d,SCH_Mode=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time , SCH_Mode );
							//----------------------------------------------------------------------
							if ( Result >= 0 ) {
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
									if ( R_Next_Move == SCHEDULER_MOVING_OUT ) {
										if ( _SCH_MODULE_Get_FM_MidCount() == 11 ) {
											Result = -1;
										}
									}
								}
							}
							if ( Result >= 0 ) {
								//
								pcaarm[i] = TRUE; // 2009.09.10
								//
								//----------------------------------------------------------------------
								// 2006.07.10
								//----------------------------------------------------------------------
								if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PLACE_From_TM[k=%d,%d]=> NextCh=%d,Slot=%d,Next_Move=%d,Path_Order=%d,JumpMode=%d,Retmode=%d\t\t\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_Path_Order , R_JumpMode , R_Retmode );
								}
								//----------------------------------------------------------------------
								//----------------------------------------------------------------------
								//================================================================================================
								//================================================================================================
								// 2004.12.06
								//================================================================================================
								//================================================================================================
	/*
	// 2008.10.28
								if ( global_bmckmode == 1 ) {
									if ( R_Next_Chamber >= BM1 ) {
										if ( SCH_Mode == 11 ) {
											SCH_Small_Time = 999999;
										}
									}
								}
	*/
								//================================================================================================
								//================================================================================================
								//================================================================================================
								SCH_Buff_Time = _SCH_PRM_GET_Putting_Priority( R_Next_Chamber );
								//printf( "2(CrossForm_PLACE_From_TM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
								R_Switch = FALSE;
								if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) {
									if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) == R_Next_Chamber ) {
										R_Switch = TRUE;
										SCH_Buff_Time = -9999;
									}
								}
								//=================================================================================
								// 2006.07.20
								//=================================================================================
								if ( SCH_Buff_Time != -9999 ) {
									if ( SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( 0 , i ) ) SCH_Buff_Time = -9999;
								}
								//=================================================================================
								// 2010.09.28
								//=================================================================================
								if ( Multi_Expect_Pick_and_Place_Skip( 0 , i , R_Next_Chamber ) ) continue; // 2018.10.20
								//
								if ( ( SCH_Mode == 12 ) && ( SCH_SWITCH ) && ( SCH_Chamber != R_Next_Chamber ) ) {
									continue;
								}
								if ( ( SCH_Mode == 12 ) && ( !SCH_SWITCH ) && ( R_Switch ) ) { // 2010.09.28
									Enter_Side		= k;
									SCH_Mode		= 12;
									SCH_No			= i;
									SCH_Chamber		= R_Next_Chamber;
									SCH_Slot		= R_Slot;
									SCH_Small_Time	= SCH_Buff_Time;
									SCH_Type        = R_Next_Move;
									SCH_Order		= R_Path_Order;
									SCH_R_Time	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
									SCH_JumpMode	= R_JumpMode;
									SCH_SWITCH		= R_Switch;
//									SCH_RETMODE		= R_Retmode; // 2019.01.30
									SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK1]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
									//----------------------------------------------------------------------
									continue;
								}
								//=================================================================================
								if ( SCH_Buff_Time < SCH_Small_Time ) {
									Enter_Side		= k;
									SCH_Mode		= 12;
									SCH_No			= i;
									SCH_Chamber		= R_Next_Chamber;
									SCH_Slot		= R_Slot;
									SCH_Small_Time	= SCH_Buff_Time;
									SCH_Type        = R_Next_Move;
									SCH_Order		= R_Path_Order;
									SCH_R_Time	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
									SCH_JumpMode	= R_JumpMode;
									SCH_SWITCH		= R_Switch;
//									SCH_RETMODE		= R_Retmode; // 2019.01.30
									SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
							//		if ( R_Switch ) i = 99; // 2002.10.15 // 2010.09.28
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK2]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
									//----------------------------------------------------------------------
								}
								else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 12 ) ) {
									if ( ( Enter_Side == k ) && !( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) ) { // 2003.05.26
										//-----------------------------------------------------------------------------
										//----------- 2002.04.08
										//-----------------------------------------------------------------------------
										if ( ( SCH_Chamber == R_Next_Chamber ) && ( SCH_Type == R_Next_Move ) && ( SCH_Type == SCHEDULER_MOVING_OUT ) ) { // 2002.04.08
											switch( _SCH_ROBOT_GET_FAST_PLACE_FINGER( 0 , R_Next_Chamber ) ) {
											case TA_STATION :
												Result = -1;
												break;
											case TB_STATION :
												Result = -1;
												Enter_Side		= k;
												SCH_Mode		= 12;
												SCH_No			= i;
												SCH_Chamber		= R_Next_Chamber;
												SCH_Slot		= R_Slot;
												SCH_Small_Time	= SCH_Buff_Time;
												SCH_Type        = R_Next_Move;
												SCH_Order		= R_Path_Order;
												SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
												SCH_JumpMode	= R_JumpMode;
												SCH_SWITCH		= R_Switch;
//												SCH_RETMODE		= R_Retmode; // 2019.01.30
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
								//				if ( R_Switch ) i = 99; // 2002.10.15 // 2010.09.28
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK3]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
												//----------------------------------------------------------------------
												break;
											}
										}
									}
									//-----------------------------------------------------------------------------
									if ( Result >= 0 ) {
										wsa = _SCH_MODULE_Get_TM_SIDE( 0,SCH_No );
										wsb = _SCH_MODULE_Get_TM_SIDE( 0,i );
										if ( wsa == wsb ) {
											/*
											//
											// 2016.08.26
											//
											//
											wta = _SCH_MODULE_Get_TM_POINTER(0,SCH_No) + _SCH_MODULE_Get_TM_POINTER2(0,SCH_No)*100;
											wtb = _SCH_MODULE_Get_TM_POINTER(0,i) + _SCH_MODULE_Get_TM_POINTER2(0,i)*100;
											if      ( ( wta / 100 ) >= ( wta % 100 ) ) wta = wta / 100;
											else if ( ( wta / 100 ) <= ( wta % 100 ) ) wta = wta % 100;
											if      ( ( wtb / 100 ) >= ( wtb % 100 ) ) wtb = wtb / 100;
											else if ( ( wtb / 100 ) <= ( wtb % 100 ) ) wtb = wtb % 100;
											//
											*/
											//
											//
											// 2016.08.26
											//
											//
											wta = _SCH_CLUSTER_Get_SupplyID( wsa , _SCH_MODULE_Get_TM_POINTER(0,SCH_No) );
											wtb = _SCH_CLUSTER_Get_SupplyID( wsb , _SCH_MODULE_Get_TM_POINTER(0,i) );
											//
											if ( wta > wtb ) {
												Enter_Side		= k;
												SCH_Mode		= 12;
												SCH_No			= i;
												SCH_Chamber		= R_Next_Chamber;
												SCH_Slot		= R_Slot;
												SCH_Small_Time	= SCH_Buff_Time;
												SCH_Type        = R_Next_Move;
												SCH_Order		= R_Path_Order;
												SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
												SCH_JumpMode	= R_JumpMode;
												SCH_SWITCH		= R_Switch;
	//											SCH_RETMODE		= R_Retmode; // 2019.01.30
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
//												if ( R_Switch ) i = 99; // 2002.10.15 // 2010.09.28
												//----------------------------------------------------------------------
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK4]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
												//----------------------------------------------------------------------
											}
										}
										else {
											if ( SCH_Chamber == R_Next_Chamber ) {
												if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_TM_SIDE( 0 , SCH_No ) , _SCH_MODULE_Get_TM_POINTER( 0 , SCH_No ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_TM_SIDE( 0 , i ) , _SCH_MODULE_Get_TM_POINTER( 0 , i ) ) ) { // 2010.09.28
												//if ( _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , SCH_No ) < _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , i ) ) { // 2010.09.28
													Enter_Side		= k;
													SCH_Mode		= 12;
													SCH_No			= i;
													SCH_Chamber		= R_Next_Chamber;
													SCH_Slot		= R_Slot;
													SCH_Small_Time	= SCH_Buff_Time;
													SCH_Type        = R_Next_Move;
													SCH_Order		= R_Path_Order;
													SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
													SCH_JumpMode	= R_JumpMode;
													SCH_SWITCH		= R_Switch;
		//											SCH_RETMODE		= R_Retmode; // 2019.01.30
													SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
//													if ( R_Switch ) i = 99; // 2002.10.15 // 2010.09.28
													//----------------------------------------------------------------------
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK5]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
													//----------------------------------------------------------------------
												}
											}
											else {
												if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ) ) {
													Enter_Side		= k;
													SCH_Mode		= 12;
													SCH_No			= i;
													SCH_Chamber		= R_Next_Chamber;
													SCH_Slot		= R_Slot;
													SCH_Small_Time	= SCH_Buff_Time;
													SCH_Type        = R_Next_Move;
													SCH_Order		= R_Path_Order;
													SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber );
													SCH_JumpMode	= R_JumpMode;
													SCH_SWITCH		= R_Switch;
		//											SCH_RETMODE		= R_Retmode; // 2019.01.30
													SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
//													if ( R_Switch ) i = 99; // 2002.10.15 // 2010.09.28
													//----------------------------------------------------------------------
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 05 (k=%d) CHECK_PLACE_From_TM [OK6]= Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,BMS=%d,SCH_Small_Time=%d\n" , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , SCH_Small_Time );
													//----------------------------------------------------------------------
												}
											}
										}
									}
								}
							}
						}
						//===================================
					} // 2009.09.10
/*
// 2008.01.17
					if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) { // 2008.01.08
						if ( nomorerun_tag != -1 ) { // 2008.01.08
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted from Arm %c(No More Run Chamber 2) ....%cWHTMFAIL\n" , nomorerun_tag + 'A' , 9 ); // 2008.01.08
							return 0; // 2008.01.08
						}
					}
					//===================================
*/
				}
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//===================================================================================================================================================
				//=======================================================================================================================================
				//=======================================================================================================================================
				// *CHECK* : MOVE
				//========================================================================================================================
//					if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() ) { // 2004.10.11
//					if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2004.10.11 // 2007.02.01
				if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2004.10.11 // 2007.02.01
					if ( Waiting_Motion != -1 ) {
						if ( _SCH_MODULE_Get_TM_Move_Signal( 0 ) == -1 ) {
							Enter_Side	   = k;
							SCH_Mode       = 10;
							SCH_Chamber    = Waiting_Motion;
						}
					}
					else if ( SCH_Mode < 0 ) {
						if ( ( _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) ) || !_SCH_MODULE_Need_Do_Multi_TAL() || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2004.09.08 
							for ( i = ps ; i < pe ; i++ ) {
								//
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[A] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
								if ( Skip_Pick_PM1 == i ) continue; // 2013.03.11
								if ( Skip_Pick_PM2 == i ) continue; // 2013.03.11
								//
//									if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , k ) || ( _SCH_PRM_GET_MODULE_xGROUP( i ) != 0 ) ) continue;
								if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , k ) ) continue;
								if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , i , G_PICK ) ) continue;
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[B] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
								if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
									if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) {
										EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
										Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( 0 , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , TRUE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , 0 );
										if ( Result > 0 ) { // 2007.01.15
											if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , i , -1 , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -197;
										}
										LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); // 2006.02.08
									}
									else {
										Result = -9999;
									}
								}
								else {
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[C] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( 0 , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , TRUE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , 0 );
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[D] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
									if ( Result > 0 ) { // 2007.01.15
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[E] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
										if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , i , -1 , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -198;
//----------------------------------------------------------------------
//_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM[F] = Res=%d (%d,%d)\n" , k , Result , Skip_Pick_PM1 , Skip_Pick_PM2 );
//----------------------------------------------------------------------
									}
								}
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 51 (k=%d) CHECK_PICK_from_PM = Res=%d\n" , k , Result );
								//----------------------------------------------------------------------
								if ( Result >= 0 ) {
									//----------------------------------------------------------------------
									// 2006.07.10
									//----------------------------------------------------------------------
//										if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
//											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_PM(M)[k=%d,%d]\t\t\n" , k , Result );
//										}
									//----------------------------------------------------------------------
									if ( _SCH_MODULE_Get_TM_Move_Signal( 0 ) == -1 ) {
										SCH_Buff_Time = 9999;
										//printf( "10(CrossForm_PICK_from_PM)---------------%d-%d\n" , SCH_Buff_Time , i );
										if ( SCH_Buff_Time < SCH_Small_Time ) {
											Enter_Side	   = k;
											SCH_Mode       = 10;
//											SCH_Chamber    = i; // 2013.12.18
											SCH_No		   = i; // 2013.12.18
											SCH_Small_Time = SCH_Buff_Time;
											SCH_Type       = R_Next_Move;
											SCH_Order      = R_Path_Order;
											SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i );
											SCH_EndMode	   = R_EndMode;
											SCH_NextFinger = R_NextFinger;
											SCH_Switchable = R_Switchable; // 2007.01.16
											SCH_Halfpick   = R_Halfpick; // 2007.11.09
											SCH_Chamber    = R_Next_Chamber; // 2013.12.18
										}
										else if ( SCH_Buff_Time == SCH_Small_Time ) {
											if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) ) {
												Enter_Side	   = k;
												SCH_Mode       = 10;
//												SCH_Chamber    = i; // 2013.12.18
												SCH_No		   = i; // 2013.12.18
												SCH_Small_Time = SCH_Buff_Time;
												SCH_Type       = R_Next_Move;
												SCH_Order      = R_Path_Order;
												SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i );
												SCH_EndMode	   = R_EndMode;
												SCH_NextFinger = R_NextFinger;
												SCH_Switchable = R_Switchable; // 2007.01.16
												SCH_Halfpick   = R_Halfpick; // 2007.11.09
												SCH_Chamber    = R_Next_Chamber; // 2013.12.18
											}
										}
									}
								}
							}
							for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
								if (
									( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) &&
									( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) &&
									( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) &&
									( SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() == k )
									) { // 2007.01.24
									Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , TRUE , 0 , &R_JumpMode , -1 , &R_Retmode , -1 );
								}
								else {
									if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { // 2004.03.24
										Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , TRUE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( 0 ) , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
									}
									else {
										Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( 0 , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , TRUE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[0] : 0 );
									}
								}
								//==========================================================================================================
								// 2006.03.27
								//==========================================================================================================
								if ( ( Result >= 0 ) && ( R_Next_Chamber >= PM1 ) ) {
									if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , R_Next_Chamber ) >= 2 ) {
										if ( !SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( 0 , k , i , R_Next_Chamber ) ) Result = -1001;
									}
								}
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 55 (k=%d) CHECK_PLACE_From_TM = Res=%d\n" , k , Result );
								//----------------------------------------------------------------------
								//==========================================================================================================
								if ( ( Result >= 0 ) && ( _SCH_MODULE_Get_TM_Move_Signal( 0 ) == -1 ) ) {
									//----------------------------------------------------------------------
									if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) < PROCESS_INDICATOR_POST ) ) SCH_Buff_Time = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , R_Next_Chamber ); // 2005.09.13
									else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) >= PROCESS_INDICATOR_POST ) SCH_Buff_Time = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , R_Next_Chamber ); // 2005.09.13
									else                                                                    continue;
									if ( 9999 < SCH_Small_Time ) {
										Enter_Side		= k;
										SCH_Mode		= 22;
										SCH_No			= i;
										SCH_Chamber		= R_Next_Chamber;
										SCH_Slot		= R_Slot;
										SCH_Small_Time	= 9999;
										SCH_Type		= R_Next_Move;
										SCH_Order		= R_Path_Order;
										SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
										SCH_JumpMode	= R_JumpMode;
//										SCH_RETMODE		= R_Retmode; // 2019.01.30
										SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
									}
									else if ( 9999 == SCH_Small_Time ) {
										if ( SCH_Mode == 22 ) {
											//=================================================================================
											// 2006.07.20
											//=================================================================================
											if ( SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( 0 , i ) ) {
												if ( ( i == TA_STATION ) || ( ( i == TB_STATION ) && ( !SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( 0 , TA_STATION ) ) ) ) {
													Result = -1;
													Enter_Side		= k;
													SCH_Mode		= 22;
													SCH_No			= i;
													SCH_Chamber		= R_Next_Chamber;
													SCH_Slot		= R_Slot;
													SCH_Small_Time	= 9999;
													SCH_Type        = R_Next_Move;
													SCH_Order		= R_Path_Order;
													SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
													SCH_JumpMode	= R_JumpMode;
		//											SCH_RETMODE		= R_Retmode; // 2019.01.30
													SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
												}
											}
											//=================================================================================
											else {
												if ( Enter_Side	== k ) { // 2003.05.26
													//-----------------------------------------------------------------------------
													//----------- 2002.05.05
													//-----------------------------------------------------------------------------
													if ( ( SCH_Chamber == R_Next_Chamber ) && ( SCH_Type == R_Next_Move ) && ( SCH_Type == SCHEDULER_MOVING_OUT ) ) { // 2002.05.05
														switch( _SCH_ROBOT_GET_FAST_PLACE_FINGER( 0 , R_Next_Chamber ) ) {
														case TA_STATION :
															Result = -1;
															break;
														case TB_STATION :
															Result = -1;
															Enter_Side		= k;
															SCH_Mode		= 22;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Slot		= R_Slot;
															SCH_Small_Time	= 9999;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
															break;
														}
													}
												}
											}
											//-----------------------------------------------------------------------------
											if ( Result >= 0 ) {
												wsa = _SCH_MODULE_Get_TM_SIDE( 0 , SCH_No );
												wsb = _SCH_MODULE_Get_TM_SIDE( 0 , i );
												if ( wsa == wsb ) {
													wta = _SCH_MODULE_Get_TM_POINTER(0,SCH_No) + _SCH_MODULE_Get_TM_POINTER2(0,SCH_No)*100;
													wtb = _SCH_MODULE_Get_TM_POINTER(0,i) + _SCH_MODULE_Get_TM_POINTER2(0,i)*100;
													if      ( ( wta / 100 ) >= ( wta % 100 ) ) wta = wta / 100;
													else if ( ( wta / 100 ) <= ( wta % 100 ) ) wta = wta % 100;
													if      ( ( wtb / 100 ) >= ( wtb % 100 ) ) wtb = wtb / 100;
													else if ( ( wtb / 100 ) <= ( wtb % 100 ) ) wtb = wtb % 100;
													if ( wta > wtb ) {
														Enter_Side		= k;
														SCH_Mode		= 22;
														SCH_No			= i;
														SCH_Chamber		= R_Next_Chamber;
														SCH_Slot		= R_Slot;
														SCH_Small_Time	= 9999;
														SCH_Type        = R_Next_Move;
														SCH_Order		= R_Path_Order;
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
														SCH_JumpMode	= R_JumpMode;
			//											SCH_RETMODE		= R_Retmode; // 2019.01.30
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
													}
												}
												else {
													if ( SCH_Chamber == R_Next_Chamber ) {
														if ( _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , SCH_No ) < _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , i ) ) {
															Enter_Side		= k;
															SCH_Mode		= 22;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Slot		= R_Slot;
															SCH_Small_Time	= 9999;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														}
													}
													else {
														if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) {
															Enter_Side		= k;
															SCH_Mode		= 22;
															SCH_No			= i;
															SCH_Chamber		= R_Next_Chamber;
															SCH_Slot		= R_Slot;
															SCH_Small_Time	= 9999;
															SCH_Type        = R_Next_Move;
															SCH_Order		= R_Path_Order;
															SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
															SCH_JumpMode	= R_JumpMode;
				//											SCH_RETMODE		= R_Retmode; // 2019.01.30
															SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
														}
													}
												}
											}
										}
										else {
											if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) {
												Enter_Side		= k;
												SCH_Mode		= 22;
												SCH_No			= i;
												SCH_Chamber		= R_Next_Chamber;
												SCH_Slot		= R_Slot;
												SCH_Small_Time	= 9999;
												SCH_Type		= R_Next_Move;
												SCH_Order		= R_Path_Order;
												SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber );
												SCH_JumpMode	= R_JumpMode;
	//											SCH_RETMODE		= R_Retmode; // 2019.01.30
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; // 2019.01.30
											}
										}
									}
								}
							}
						}
//							if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2005.12.15
						if ( _SCH_PRM_GET_MODULE_MODE( FM ) || Get_Prm_MODULE_BUFFER_USE_MODE() ) { // 2005.12.15
							wsa = 0;
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2002.06.21
								if ( !SCHEDULING_CHECK_Enable_MOVE_BM_for_GETPR( 0 ) ) wsa = 1;
							}
							if ( wsa == 0 ) {
								//====================================================================================================================
								// 2006.09.07
								//====================================================================================================================
//									if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ) { // 2006.12.21
								if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ) { // 2006.12.21
									Result = -2;
								}
								//====================================================================================================================
								else {
//									Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 1 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); // 2013.11.12
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( 0 , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 1 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); // 2013.11.12
									if ( Result > 0 ) { // 2007.01.15
										if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( 0 , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -199;
									}
								}
							}
							else {
								Result = -1;
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 52 (k=%d) CHECK_PICK_from_BM = Res=%d\n" , k , Result );
							//----------------------------------------------------------------------
							if ( Result >= 0 ) {
								//----------------------------------------------------------------------
								// 2006.07.10
								//----------------------------------------------------------------------
//									if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) {
//										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Select Log with PICK_from_BM(M)[k=%d,%d]\t\t\n" , k , Result );
//									}
								//----------------------------------------------------------------------
								if ( _SCH_MODULE_Get_TM_Move_Signal( 0 ) == -1 ) {
									SCH_Buff_Time = 9999;
									//printf( "20(CrossForm_PICK_from_BM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
									if ( SCH_Buff_Time < SCH_Small_Time ) {
										Enter_Side		= k;
										SCH_Mode		= 20;
										SCH_Chamber		= R_Curr_Chamber;
										SCH_Slot		= R_Slot;
										SCH_Small_Time	= SCH_Buff_Time;
										SCH_SchPt		= R_Schpt;
										SCH_JumpMode	= R_JumpMode;
										SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber );
										SCH_Switchable = R_Switchable; // 2007.01.16
									}
									else if ( SCH_Buff_Time == SCH_Small_Time ) {
										if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber ) ) {
											Enter_Side		= k;
											SCH_Mode		= 20;
											SCH_Chamber		= R_Curr_Chamber;
											SCH_Slot		= R_Slot;
											SCH_Small_Time	= SCH_Buff_Time;
											SCH_SchPt		= R_Schpt;
											SCH_JumpMode	= R_JumpMode;
											SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber );
											SCH_Switchable = R_Switchable; // 2007.01.16
										}
									}
								}
							}
							else if ( ( wsa == 0 ) && ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) != 2 ) ) {
								if ( _SCH_MODULE_Get_TM_Move_Signal( 0 ) == -1 ) {
									if ( _SCH_MODULE_Get_FM_MidCount() == 11 ) {
//										if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) > 0 ) { // 2008.06.06
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( 0 ) > 1 ) ) { // 2008.06.06
											R_Curr_Chamber = SCHEDULING_CHECK_Get_BM_FREE_IN_CHAMBER_FOR_PICK_BM( CHECKING_SIDE , 0 );
											if ( R_Curr_Chamber >= 0 ) {
												SCH_Buff_Time = 9999;
												//printf( "20(CrossForm_PICK_from_BM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
												if ( SCH_Buff_Time < SCH_Small_Time ) {
													Enter_Side		= k;
													SCH_Mode		= 20;
													SCH_Chamber		= R_Curr_Chamber;
													SCH_Slot		= 1;
													SCH_Small_Time	= SCH_Buff_Time;
													SCH_SchPt		= R_Schpt;
													SCH_JumpMode	= R_JumpMode;
													SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber );
													SCH_Switchable = R_Switchable; // 2007.01.16
												}
												else if ( SCH_Buff_Time == SCH_Small_Time ) {
													if ( SCH_R_Time > ( _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber ) ) ) {
														Enter_Side		= k;
														SCH_Mode		= 20;
														SCH_Chamber		= R_Curr_Chamber;
														SCH_Slot		= 1;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_SchPt		= R_Schpt;
														SCH_JumpMode	= R_JumpMode;
														SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber );
														SCH_Switchable = R_Switchable; // 2007.01.16
													}
												}
											}
										}
									}
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
										if ( _SCH_MODULE_Get_FM_MidCount() == 2 ) {
											wsb = -1; // 2005.12.21
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( 0,TB_STATION ) != 0 ) && ( _SCH_MODULE_Get_TM_TYPE( 0,TB_STATION ) == SCHEDULER_MOVING_OUT ) ) wsb = TB_STATION; // 2005.12.21
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( 0,TA_STATION ) != 0 ) && ( _SCH_MODULE_Get_TM_TYPE( 0,TA_STATION ) == SCHEDULER_MOVING_OUT ) ) wsb = TA_STATION; // 2005.12.21
											if ( wsb != -1 ) { // 2005.12.21
												SCH_Buff_Time = 9999;
												//printf( "21(CrossForm_PICK_from_BM)---------------%d-%d\n" , SCH_Buff_Time , R_Next_Chamber );
												if ( SCH_Buff_Time < SCH_Small_Time ) {
													Enter_Side		= k;
													SCH_Mode		= 21;
													SCH_No			= wsb; // 2005.12.21
													SCH_Chamber		= Get_Prm_MODULE_BUFFER_SINGLE_MODE();
													SCH_Slot		= 1;
													SCH_Small_Time	= SCH_Buff_Time;
													SCH_SchPt		= R_Schpt;
													SCH_JumpMode	= R_JumpMode;
													SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() );
													SCH_Switchable  = R_Switchable; // 2007.01.16
												}
												else if ( SCH_Buff_Time == SCH_Small_Time ) {
													if ( SCH_R_Time > ( _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() ) ) ) {
														Enter_Side		= k;
														SCH_Mode		= 21;
														SCH_No			= wsb; // 2005.12.21
														SCH_Chamber		= Get_Prm_MODULE_BUFFER_SINGLE_MODE();
														SCH_Slot		= 1;
														SCH_Small_Time	= SCH_Buff_Time;
														SCH_SchPt		= R_Schpt;
														SCH_JumpMode	= R_JumpMode;
														SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( -1 , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() );
														SCH_Switchable  = R_Switchable; // 2007.01.16
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
				//=======================================================================
				// 2006.02.13
				//=======================================================================
				//=======================================================================================================================================
				// *CHECK* : AUTO ARRANGE
				//========================================================================================================================
//					if ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) { // 2007.02.01
				if ( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) == 0 ) ) { // 2007.02.01
					if ( SCH_Mode < 0 ) {
						if ( SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( 0 , 1 ) ) {
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 101 (k=%d) CHECK_PICK_from_PM_MG = s=%d,e=%d\n" , k , ps , pe );
							//----------------------------------------------------------------------
							R_Retmode = FALSE; // 2006.03.22
							for ( i = ps ; i < pe ; i++ ) {
								//
								if ( Skip_Pick_PM1 == i ) continue; // 2013.03.11
								if ( Skip_Pick_PM2 == i ) continue; // 2013.03.11
								//
								if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) continue;
								if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) {
									EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] );
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM_MG( 0 , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , 0 , &R_Retmode );
									LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] );
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 101 (k=%d) CHECK_PICK_from_PM_MG = Res=%d,PM=%d,R_Next_Move=%d\n" , k , Result , i , R_Next_Move );
									//----------------------------------------------------------------------
									if ( Result >= 0 ) {
										SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( i );
										if ( SCH_Buff_Time < SCH_Small_Time ) {
											Enter_Side	   = k;
											SCH_Mode       = 0;
											SCH_No         = i;
											SCH_Small_Time = SCH_Buff_Time;
											SCH_Type       = R_Next_Move;
											SCH_Order      = R_Path_Order;
											SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
											SCH_W_Prcs	   = R_Wait_Process;
											SCH_EndMode	   = R_EndMode;
											SCH_NextFinger = R_NextFinger;
											if ( SCH_W_Prcs ) {
												Waiting_Motion = SCH_No;
											}
											else {
												Waiting_Motion = -1;
											}
											SCH_SWITCH     = TRUE;
											SCH_Chamber    = R_Next_Chamber; // 2006.03.27
											SCH_Halfpick   = R_Halfpick; // 2007.11.09
										}
										else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 0 ) ) {
											if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) {
												Enter_Side	   = k;
												SCH_Mode       = 0;
												SCH_No         = i;
												SCH_Small_Time = SCH_Buff_Time;
												SCH_Type       = R_Next_Move;
												SCH_Order      = R_Path_Order;
												SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i );
												SCH_W_Prcs	   = R_Wait_Process;
												SCH_EndMode	   = R_EndMode;
												SCH_NextFinger = R_NextFinger;
												if ( SCH_W_Prcs ) {
													Waiting_Motion = SCH_No;
												}
												else {
													Waiting_Motion = -1;
												}
												SCH_SWITCH     = TRUE;
												SCH_Chamber    = R_Next_Chamber; // 2006.03.27
												SCH_Halfpick   = R_Halfpick; // 2007.11.09
											}
										}
									}
								}
							}
							if ( ( SCH_Mode < 0 ) && ( R_Retmode ) ) SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( 0 , 0 ); // 2006.03.23
						}
					}
					else {
						SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( 0 , 0 );
					}
				}
				//=======================================================================
			}
			if ( Enter_Side	== -1 ) { // 2002.07.12
				SCH_Mode = -1;
//						if ( bs ) break; // 2002.11.04
				//===============================================================================================
				// 2007.02.01
				//===============================================================================================
				if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) != 0 ) {
					SCHEDULER_Set_TM_Pick_BM_Signal( 0 , 0 );
					break;
				}
				//===============================================================================================
				if ( ( ps == PM1 ) && ( pe == ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
					//=============================================================================================================================================
					// 2006.02.10
					//=============================================================================================================================================
//					if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 6 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) { // 2016.07.22
//					if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 6 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) ) { // 2016.07.22 // 2018.04.10
					if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 6 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 3 ) == 3 ) ) { // 2016.07.22 // 2018.04.10
						if ( _SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE ) > 0 ) {
							Result = SCHEDULING_CHECK_for_Check_PLACE_Deadlock( 0 , CHECKING_SIDE );
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 68 CHECK_PLACE_DEADLOCK 1 = Res=%d\n" , Result );
						}
						else {
							if ( !SCHEDULING_CHECK_for_Global_Deadlock_Operation( 0 , CHECKING_SIDE ) ) { // 2007.11.03
								return FALSE;
							}
						}
					}
					else {
						if ( !SCHEDULING_CHECK_for_Global_Deadlock_Operation( 0 , CHECKING_SIDE ) ) { // 2007.11.03
							return FALSE;
						}
					}
					//=============================================================================================================================================
					break;
				}
				else { // 2002.09.13
//							if ( ps != -1 ) { // 2002.11.05
						if ( SCH_PR_RUN ) break;
//							}
				}
			}
			else {
				//
//				if ( SCHEDULER_PROCESSING_STATUS_DIFFERENT( FALSE ) ) return 1; // 2012.12.05 // 2019.02.08
				if ( SCHEDULER_PROCESSING_STATUS_DIFFERENT( 0 , FALSE ) ) return 1; // 2012.12.05 // 2019.02.08
				//
				if ( Enter_Side	!= CHECKING_SIDE ) {
				//	SCH_Mode = -1; // 2008.09.17
				//	break; // 2005.01.12 // 2008.09.17
//					return -2; // 2008.09.17 // 2008.09.27
					//
					if ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_DEFAULT ) return 1; // 2013.03.18
					//
					return -100-Enter_Side; // 2008.09.17 // 2008.09.27
				}
				else {
					//
					if ( SCHEDULER_Get_TM_Pick_BM_Signal( 0 ) != 0 ) {
//						SCHEDULER_Set_TM_Pick_BM_Signal( 0 , 0 ); // 2009.07.15
						if ( SCH_Mode != 11 ) SCHEDULER_Set_TM_Pick_BM_Signal( 0 , 0 ); // 2009.07.15
						break;
					}
//						_SCH_MODULE_Set_TM_Switch_Signal( 0 , -1 ); // 2005.01.12
					//
//						_SCH_SYSTEM_RUNCHECK_SET( CHECKING_SIDE + 1 ); // 2002.07.10 // 2005.01.12
					//
					if ( ( SCH_Mode != 10 ) && ( SCH_Mode != 20 ) && ( SCH_Mode != 21 ) && ( SCH_Mode != 22 ) ) break; // 2005.01.12
				}
//					break; // 2005.01.12
			}
		} // 2002.07.12

		if ( SCH_Mode != -1 ) { // 2005.01.12
			_SCH_MODULE_Set_TM_Switch_Signal( 0 , -1 );
			//_SCH_SYSTEM_RUNCHECK_SET( CHECKING_SIDE + 1 ); // 2008.06.04
			RUN_NOT_SELECT_COUNT[0] = 0;
		}
		else {
			RUN_NOT_SELECT_COUNT[0]++;
		}
		//======================================================================================================================
		switch ( SCH_Mode ) {
			case -1 :
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 12 );
				//-------------------------------------------
						break;
			case 0 :
				//
				//----------------------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 13 );
				//----------------------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_PICK_FROM_PM( 0 , CHECKING_SIDE , SCH_W_Prcs , SCH_No , SCH_Type , SCH_NextFinger , SCH_Order , SCH_Chamber , SCH_Switchable , SCH_Halfpick , SCH_EndMode , SCH_SWITCH , &Skip_Pick_PM1 , &Skip_Pick_PM2 , &Run_Doing ); // 2013.02.07

				if ( Run_Doing ) { // 2018.10.20
					if ( SCH_PM_IS_PICKING( SCH_No ) ) {
						MULTI_ALL_PM_LAST_PICK[0] = SCH_No;
					}
					else {
						MULTI_ALL_PM_LAST_PICK[0] = 0;
					}
				}

				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 1 :

				//========================================================================================================================================================
				// Pick from CM
				//========================================================================================================================================================
				//----------------------------------------------------------------------------
				//----------------------------------------------------------------------------
//					_SCH_CASSETTE_Reset_Side_Monitor( CHECKING_SIDE , 0 ); // 2007.11.27
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 ); // 2003.04.23
				//----------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				KPLT0_RESET_CASSETTE_TIME_SUPPLY();
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 14 );
				//-------------------------------------------
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 13 RUN_TYPE_1(PICKfromCM)\n" );
					//----------------------------------------------------------------------
					//-----------------------------------------------------------------------
					pt = _SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE );
					pt2 = 0;
					wsa = _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt );
					//
					ch = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt );
					//===============================================================================================================
					// 2004.05.10
					//===============================================================================================================
					if ( ch < PM1 ) { // 2004.05.10
						if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , pt ) ) {
							break;
						}
					}
					//===============================================================================================================
					if ( SCH_NextFinger != -1 ) {
						cf = SCH_NextFinger;
					}
					else {
						switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0,SCH_No ) ) { // 2003.02.05
						case 0 :
							cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( 0 , ch );
							break;
						case 1 :
							cf = ( _SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION ) > 0 ) ? -1 : TA_STATION;
							break;
						case 2 :
							cf = ( _SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION ) > 0 ) ? -1 : TB_STATION;
							break;
						} // 2003.02.05
					}
					if ( cf == -1 ) break;
					//-----------------------------------------------------------------------
					if ( ch >= PM1 ) { // 2003.02.13
						if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , _SCH_PRM_GET_DUMMY_PROCESS_SLOT() ) <= 0 ) { // 2003.02.13
							_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) + 1 );
							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_CM_END );
							_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
							break;
						}
						_SCH_MODULE_Set_Use_Interlock_Map( CHECKING_SIDE );
					}
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( 0 , -1 );
					SCHEDULER_Set_TM_LastMove_Signal( 0 , -1 );
					//-----------------------------------------------------------------------
					// 2005.11.25
					//-----------------------------------------------------------------------
					Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
					Sav_Int_2 = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt );
					Sav_Int_3 = _SCH_SUB_Get_Last_Status( CHECKING_SIDE );
					//-----------------------------------------------------------------------
//							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_STARTING ); // 2006.11.27
					_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
					//-----------------------------------------------------------------------
//							if ( ch < PM1 ) { // 2002.06.24 // 2004.05.10
//								SCHMULTI_MESSAGE_PROCESSJOB_START_CHECK( CHECKING_SIDE , pt ); // 2004.05.10
//							} // 2004.05.10

					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , pt , 1 , 1 , 0 ); // 2006.11.27
					//-----------------------------------------------------------------------
					// Move Below 2001.12.27
					//_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					if ( ch < PM1 ) {
						//=============================================================================================================
						Result = SCHEDULING_CHECK_Switch_PLACE_From_TM( 0 , CHECKING_SIDE , cf , ch );
						//-----------------------------------------------------------------------
//						_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , 1 , cf , wsa , wsa ); // 2014.01.20
						//=============================================================================================================
						// 2006.04.27
						//=========================================================================================================
						Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , CHECKING_SIDE , pt , ( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) >= 8 ) ? ( ( PM1 + _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) - 8 ) * 1000 ) + ch : ch , cf , wsa , 1 , Result , -1 , ( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) >= 8 ) ? TRUE : FALSE , 0 , -1 , -1 );
						//=========================================================================================================
						if ( Function_Result == SYS_ABORTED ) { // 2005.11.25
							return 0;
						}
						//=============================================================================================================
						// 2005.11.25
						//=============================================================================================================
						else if ( Function_Result == SYS_ERROR ) {
							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
							_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , Sav_Int_2 );
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int_3 );
							//
							break;
						}
						//=============================================================================================================
						if ( Result ) _SCH_MODULE_Set_TM_Switch_Signal( 0 , ch ); // 2007.01.15
						//=============================================================================================================
						// 2005.10.07
						//=============================================================================================================
						if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
							if ( SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( ch , TRUE , FALSE , wsa , cf , ch , wsa , _SCH_COMMON_GET_METHOD_ORDER( CHECKING_SIDE , pt ) ) != SYS_SUCCESS ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Pick Complete Fail from %s(%d)[F%c]%cWHTMPICKCOMPLETEFAIL CM%d:%d:%c%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , cf + 'A' , 9 , ch - CM1 + 1 , wsa , cf + 'A' , 9 , wsa );
								return 0;
							}
						}
					}
					else {
						//-----------------------------------------------------------------------
						_SCH_IO_SET_MODULE_SOURCE( ch , 1 , MAX_CASSETTE_SIDE );
						_SCH_IO_SET_MODULE_RESULT( ch , 1 , 0 );
						_SCH_IO_SET_MODULE_STATUS( ch , 1 , CHECKING_SIDE + 1 );
						//-----------------------------------------------------------------------
						//-----------------------------------------------------------------------
						Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , CHECKING_SIDE , pt , ch , cf , wsa , _SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , ch , TRUE , 0 , -1 , -1 );
						if ( Function_Result == SYS_ABORTED ) { // 2002.07.18 // 2005.11.25
							return 0;
						}
						//=============================================================================================================
						// 2005.11.25
						//=============================================================================================================
						else if ( Function_Result == SYS_ERROR ) {
							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
							_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , pt , Sav_Int_2 );
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int_3 );
							//
							break;
						}
						//=============================================================================================================
					}
					//-----------------------------------------------------------------------
					if ( _SCH_MODULE_Need_Do_Multi_TAL() ) { // 2004.09.08 
						_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( CHECKING_SIDE , cf , -1 );
						_SCH_MODULE_Set_MFAL_WAFER( wsa );
					}
					_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_SIDE_POINTER( 0 , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
					_SCH_MODULE_Set_TM_PATHORDER( 0 , cf , SCH_Order );
					_SCH_MODULE_Set_TM_TYPE( 0 , cf , SCHEDULER_MOVING_IN );
					_SCH_MODULE_Set_TM_OUTCH( 0 , cf , ch ); // 2002.04.23
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_WAFER( 0 , cf , wsa );
					//-----------------------------------------------------------------------
					if ( ch < PM1 ) {
						_SCH_IO_SET_MODULE_STATUS( ch , wsa , CWM_ABSENT );
						if ( _SCH_IO_GET_WID_NAME_FROM_READ( CHECKING_SIDE , 0 , StringBuffer ) ) {
							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_WID_SET , wsa , pt , -1 , -1 , -1 , StringBuffer , "" );
						}
					}
					//-----------------------------------------------------------------------
					// Moved Here 2001.12.27
					_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					_SCH_TIMER_SET_ROBOT_TIME_START( 0 , cf );
					//=============================================================================================================
					// 2006.03.03
					//=============================================================================================================
					if ( ch < PM1 ) {
						if      ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) { // 2007.09.06
							switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
							case 6 :
							case 7 :
								if ( SCH_PICKCM_ARMINT != 0 ) {
									SCHEDULER_CONTROL_Chk_ARMINLTKS_PICK_FROM_CM_A0SUB2( SCH_PICKCM_ARMINT , CHECKING_SIDE , pt );
								}
								else {
									SCHEDULER_CONTROL_Chk_ARMINLTKS_RECOVER_AFTER_PLACE_A0SUB2( 0 );
								}
								break;
							}
						}
						else if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO3 ) {
							SCHEDULER_CONTROL_REMAP_AFTER_PICKCM_A0SUB3( CHECKING_SIDE , pt , 0 , cf );
						}
					}
					//----------------------------------------------------------
					Run_Doing = TRUE;
					//----------------------------------------------------------
					//-----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 14 END_TYPE_1(PICKfromCM) = Chamber(CM%d) finger(%d) pt(%d,%d)\n" , ch - CM1 + 1 , cf , pt , pt2 );
					//----------------------------------------------------------------------
					break;

			case 2 :
				//========================================================================================================================================================
				// Place to CM/PM
				//========================================================================================================================================================
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 15 );
				//-------------------------------------------
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 15 RUN_TYPE_2(PLACEtoCM/PM)\n" );
					//----------------------------------------------------------------------

					_SCH_MODULE_Set_TM_Move_Signal( 0 , -1 );

					pt = _SCH_MODULE_Get_TM_POINTER( 0 , SCH_No );

					pt2 = 0;
					wsa = _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt );
					//-----------------------------------------------------------------------
					if ( SCH_Type == SCHEDULER_MOVING_OUT ) {
						//--------------------------------------------------------------------------------
						if ( SCH_RETMODE ) { // 2003.10.21
							//
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , -1 );
							//
							SCHEDULER_FM_PM_CONTROL_RETURN_RECOVER( CHECKING_SIDE , pt , FALSE , TRUE , 19 ); // 2016.02.19
							//
							_SCH_MODULE_Set_TM_TYPE( 0 , SCH_No , SCHEDULER_MOVING_OUT );
						}
						//
						//
						// 2018.12.27
						//
						k = _SCH_SUB_GET_OUT_CM_AND_SLOT2( CHECKING_SIDE , pt , -1 , &ch , &wsb , &i );
						//
						if ( i || ( k == -1 ) || ( ch <= 0 ) ) {
							break;
						}
						//
						//--------------------------------------------------------------------------------
						// 2005.11.25
						//-----------------------------------------------------------------------
						Sav_Int_2 = SCHEDULER_Get_PM_MidCount( 0 );
						//-----------------------------------------------------------------------
						SCHEDULER_Set_PM_MidCount( 0 , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) ); // 2004.11.23
						//--------------------------------------------------------------------------------
//						_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , pt , -1 , &ch , &wsb ); // 2007.07.11 // 2018.12.27
						//-----------------------------------------------------------------------
						SCHEDULER_Set_TM_LastMove_Signal( 0 , -1 );
						//-----------------------------------------------------------------------
						//-----------------------------------------------------------------------
						// 2005.11.25
						//-----------------------------------------------------------------------
						Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
						//-----------------------------------------------------------------------
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_CM_END );
						//-----------------------------------------------------------------------
						if ( ch < PM1 ) {
							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) != PATHDO_WAFER_RETURN ) { // 2005.05.17
								_SCH_SUB_PLACE_CM_for_ERROROUT( CHECKING_SIDE , pt , &ch , &wsb , CHECKING_SIDE , 0 , &pt2 , &pt2 ); // 2005.05.17
							}
							if ( ch >= BM1 ) { // 2005.05.17
								//===========================================================================================================
								// 2005.05.18
								//===========================================================================================================
								Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsb , wsb , 0 , -1 , FALSE , 0 , -1 , -1 );
								if ( Function_Result == SYS_ABORTED ) { // 2005.11.25
									return 0;
								}
								//===========================================================================================================
								// 2005.11.25
								//===========================================================================================================
								else if ( Function_Result == SYS_ERROR ) {
									_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
									SCHEDULER_Set_PM_MidCount( 0 , Sav_Int_2 );
									//
									break;
								}
							} // 2005.05.17
							else if ( ch < PM1 ) { // 2005.05.17
								//-----------------------------------------------------------------------
								// 2005.11.25
								//-----------------------------------------------------------------------
								Sav_Int_3 = _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No );
								//=========================================================================================================
								// 2004.10.12
								//=========================================================================================================
								Result = 0;
								swmode = 0;
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) {
									//
									_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
									//
									if ( SCHEDULING_CHECK_PICK_FROM_CM_POSSIBLE_FOR_SWITCHMESSAGE( CHECKING_SIDE ) ) {
										//-------------------------
										Result = 1;
										//-------------------------
										swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( 1 , 1 ); // 2007.07.25
										//-------------------------
									}
								}
								//=========================================================================================================
								Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) >= 8 ) ? ( ( PM1 + _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) - 8 ) * 1000 ) + ch : ch , SCH_No , wsb , 1 , Result , -1 , ( _SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( ch ) >= 8 ) ? TRUE : FALSE , swmode , -1 , -1 );
								//=========================================================================================================
								if ( Function_Result == SYS_ABORTED ) { // 2004.10.14 // 2005.11.25
									return 0;
								}
								//===========================================================================================================
								// 2005.11.25
								//===========================================================================================================
								else if ( Function_Result == SYS_ERROR ) {
									_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
									SCHEDULER_Set_PM_MidCount( 0 , Sav_Int_2 );
									_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , Sav_Int_3 );
									//
									break;
								}
								//===========================================================================================================
								// 2005.10.07
								//=============================================================================================================
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
									if ( SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( ch , FALSE , FALSE , wsb , SCH_No , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) , _SCH_COMMON_GET_METHOD_ORDER( CHECKING_SIDE , pt ) ) != SYS_SUCCESS ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Place Complete Fail to %s(%d)[F%c]%cWHTMPLACECOMPLETEFAIL CM%d:%d:%c%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsb , SCH_No + 'A' , 9 , ch - CM1 + 1 , wsb , SCH_No + 'A' , 9 , wsb );
										return 0;
									}
								}
								//=============================================================================================================
							}
						}
						else {
							Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsb , _SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , -1 , FALSE , 0 , -1 , -1 );
							if ( Function_Result == SYS_ABORTED ) { // 2002.07.18 // 2005.11.25
								return 0;
							}
							//===========================================================================================================
							// 2005.11.25
							//===========================================================================================================
							else if ( Function_Result == SYS_ERROR ) {
								_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 );
								SCHEDULER_Set_PM_MidCount( 0 , Sav_Int_2 );
								//
								break;
							}
						}
						//=================================================================================
						// Move Down 2005.03.19
						//=================================================================================
						//_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
						//_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE );
						//=================================================================================
						if ( ch < PM1 ) {
							//=================================================================================
							// Move Here 2005.03.19
							//=================================================================================
//							if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) == PATHDO_WAFER_RETURN ) { // 2019.03.05
//								_SCH_IO_SET_MODULE_STATUS( ch , wsb , CWM_PRESENT );
//							}
//							else {
//								_SCH_IO_SET_MODULE_STATUS( ch , wsb , _SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( _SCH_CLUSTER_Get_PathIn(CHECKING_SIDE,pt) , wsa ) );
//							}
							//=================================================================================
//							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_END , wsa , -1 , -1 , -1 , -1 , "" , "" ); // 2012.02.18
							_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_CM_END , wsa , -1 , pt , -1 , -1 , "" , "" ); // 2012.02.18
							_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_EVERYOUT , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , pt ) , 0 , 0 , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) );
							if ( SCHEDULER_CONTROL_Chk_Last_Finish_Status_TM_0( CHECKING_SIDE , pt , 1 ) ) {
								_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_LASTOUT , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , pt ) , _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , pt ) , 0 , 0 , _SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , pt ) );
							}
							_SCH_MULTIJOB_PROCESSJOB_END_CHECK( CHECKING_SIDE , pt );
							//-----------------------------------------------------------------------
						}
						else {
							if ( ch >= BM1 ) { // 2005.05.18
								_SCH_MODULE_Set_BM_WAFER_STATUS( ch , wsb , wsa , BUFFER_INWAIT_STATUS );
							}
							else {
								//----------------------------------------------------------------
								SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( CHECKING_SIDE , -1 );
								//----------------------------------------------------------------
							}
						}
						//=================================================================================
						// Move Here 2005.03.19
						//=================================================================================
						_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
						_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE );
						//
						if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2018.12.07
							_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE );
						}
						//=================================================================================
						// 2006.12.22
						//=================================================================================
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) || ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 12 ) , CHECKING_SIDE , pt );
						//=================================================================================
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 16 END_TYPE_2(PLACEtoCM) = Chamber(CM%d) finger(%d) pt(%d,%d)\n" , ch - CM1 + 1 , SCH_No , pt , pt2 );
						//----------------------------------------------------------------------
						//=============================================================================================================
						// 2005.10.07
						//=============================================================================================================
						if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO2 ) {
							SCHEDULER_CONTROL_Return_CASSETTE_Operation_A0SUB2( CHECKING_SIDE , pt , 0 , ch );
						}
						//=============================================================================================================
					}
					else {
						//-----------------------------------------------------------------------
						currpathdo = _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ); // 2008.07.08
						//-----------------------------------------------------------------------
						if ( currpathdo == PATHDO_WAFER_RETURN ) break; // 2008.07.08
						//-----------------------------------------------------------------------
						ch = SCHEDULER_CONTROL_Get_PathProcessChamber( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order );
						//-------------------------------------------------------------------------------------
						if ( !SCH_PM_HAS_ABSENT( ch , CHECKING_SIDE , pt , currpathdo - 1 , &pmslot ) ) break; // 2014.01.10
						//-------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Check_2Module_Same_Body_Odd_Check_PM_H( TRUE , CHECKING_SIDE , 0 , SCH_No , &ch , &R_Retmode ); // 2007.11.09
						//-------------------------------------------------------------------------------------
						//==============================================================================================================
						// 2007.01.02
						//==============================================================================================================
						if ( _SCH_PRM_GET_Putting_Priority( ch ) < 0 ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) break;
						}
						//==============================================================================================================
						switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0,ch ) ) { // 2003.02.05
						case 1 :
							if ( SCH_No != TA_STATION ) ch = -1;
							break;
						case 2 :
							if ( SCH_No != TB_STATION ) ch = -1;
							break;
						}
						if ( ch == -1 ) break;
						//==============================================================================================================
						// 2006.02.08
						//==============================================================================================================
						if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
							EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
							//===============================================================================
							// 2006.06.14
							//===============================================================================
//									if ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != -1 ) { // 2006.06.14
							//===============================================================================
							if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != 0 ) ) { // 2006.06.14
							//===============================================================================
								LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
								break;
							}
//							if ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) ) { // 2014.01.10
							if ( ( _SCH_MODULE_Get_PM_WAFER( ch , pmslot ) > 0 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) ) { // 2014.01.10
								LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
								break;
							}
							SCHEDULER_Set_MULTI_GROUP_PLACE( ch , 0 );
							LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
							//==============================================================================================================
							if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) != ch ) { // 2006.07.21
								SCHEDULING_CHECK_MULTI_PLACE_GROUP_RESET( 0 , ch ); // 2006.06.14
							}
							//==============================================================================================================
						}
						//-----------------------------------------------------------------------
						// 2005.11.25
						//-----------------------------------------------------------------------
						Sav_Int_2 = SCHEDULER_Get_PM_MidCount( 0 );
						//-----------------------------------------------------------------------
						SCHEDULER_Set_PM_MidCount( 0 , currpathdo ); // 2002.11.05
						//-----------------------------------------------------------------------
						//-----------------------------------------------------------------------
						//-----------------------------------------------------------------------
						SCHEDULER_Set_TM_LastMove_Signal( 0 , ch );
						//-----------------------------------------------------------------------
						switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) ) {
						case -1 :
						case -3 :
						case -5 :
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//===============================================================================================================================
							return 0;
							break;
						}
						//
						// 2015.11.27
						//
						if ( _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0() == 3 ) {
							//
							if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
								//
								switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + 1 ) ) {
								case -1 :
								case -3 :
								case -5 :
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted S1 Because %s Abort%cWHPM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
									//===============================================================================================================================
									if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
									//===============================================================================================================================
									return 0;
									break;
								}
								//
							}
							else {
								//
								switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch - 1 ) ) {
								case -1 :
								case -3 :
								case -5 :
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted S2 Because %s Abort%cWHPM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
									//===============================================================================================================================
									if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
									//===============================================================================================================================
									return 0;
									break;
								}
								//
							}
						}
						//
						//-----------------------------------------------------------------------
						// Pre Process Part Begin
						//-----------------------------------------------------------------------
						j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , SCH_No , ch );
						if      ( ( j == 0 ) || ( j == 1 ) ) {
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//===============================================================================================================================
							break;
						}
						else if ( j == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Place Fail to %s(%d)[F%c]%cWHTMPLACEFAIL PM%d:%d:%d:%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt + 1 , SCH_No + 'A' , 9 , ch - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' );
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//===============================================================================================================================
							return 0;
						}
						//-----------------------------------------------------------------------
						// End
						//-----------------------------------------------------------------------
						//-----------------------------------------------------------------------
						// 2005.11.25
						//-----------------------------------------------------------------------
//								Sav_Int_1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); // 2005.12.10
						//-----------------------------------------------------------------------
//								_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING ); // Move Here // 2002.07.23 // 2005.12.10
						//-----------------------------------------------------------------------
						//-------------------------
						swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( SCH_SWITCH , 0 ); // 2007.07.25
						//-------------------------
//						Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , 1 , 0 , -1 , FALSE , swmode , -1 , -1 ); // 2014.01.10
						Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , pmslot + 1 , 0 , -1 , FALSE , swmode , -1 , -1 ); // 2014.01.10
						if ( Function_Result == SYS_ABORTED ) { // 2002.07.18 // 2005.11.25
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//===============================================================================================================================
							return 0;
						}
						//===========================================================================================================
						// 2005.11.25
						//===========================================================================================================
						else if ( Function_Result == SYS_ERROR ) {
							//
//									_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int_1 ); // 2005.12.10
							SCHEDULER_Set_PM_MidCount( 0 , Sav_Int_2 );
							//
							//===============================================================================================================================
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
							//===============================================================================================================================
							break;
						}
						//===========================================================================================================
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING ); // Move Here // 2002.07.23 // 2005.12.10
						//===========================================================================================================
						//-----------------------------------------------------------------------
						// 2002.05.28
						//-----------------------------------------------------------------------
						if ( _SCH_CLUSTER_Check_HasProcessData_Post( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order ) ) {
							if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 1 ) {
								_SCH_CLUSTER_Set_PathProcessData_SkipPost( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order );
								_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , ch ); // 2006.02.10
							}
							else { // 2006.02.10
								if ( post_time >= 0 ) {
									_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , ch , post_time );
								}
							}
							if ( prcs_time >= 0 ) { // 2006.02.13
								_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , ch , prcs_time );
							}
						}
						//==============================================================================================================
						// 2006.03.27 Move Below
						//==============================================================================================================
						//=============================================================================================================
						// 2006.02.10 Remake Groupping Flow Data
						//=============================================================================================================
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
							//===============================================================================================
							EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[ch] );
							//===============================================================================================
							SCHEDULER_Set_MULTI_GROUP_PICK( ch , 0 );
							//===============================================================================================
							//===============================================================================================
							// 2006.03.27 Move Here
							//===============================================================================================
							SCHEDULING_CHECK_REMAKE_GROUPPING_FLOWDATA( 0 , SCH_No , CHECKING_SIDE , pt , ch ); // 2006.03.27
							//===============================================================================================
							//==============================================================================================================
							// 2006.03.27 Move Here
							//==============================================================================================================
							//-----------------------------------------------------------------------
							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 ); // 2002.07.23
							//-----------------------------------------------------------------------
							//
							// 2012.04.06
//							_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//							_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//							_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) ); // 2014.01.10

							_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
							_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
							_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
							_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) ); // 2014.01.10

							_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
							//
							//===============================================================================================
							//
							_SCH_MODULE_Set_PM_PICKLOCK( ch , pmslot , 1 ); // 2014.01.10
							//
							//==============================================================================================================
							if ( SCH_PM_IS_ALL_PLACED_PROCESSING( ch , TRUE , FALSE , &multiprcs ) ) { // 2014.01.10
								//
								if ( multiprcs ) {
									//
									if ( !SCH_PM_GET_FIRST_SIDE_PT( ch , &m_f_side , &m_f_pt ) ) {
										m_f_side = CHECKING_SIDE;
										m_f_pt = pt;
									}
									//
									_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( m_f_side , m_f_pt , -1 , ch , SCH_No , -1 , 107 );
									//
								}
								else {
									//
									_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
									_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
										  ch , -1 ,
										  wsa , -1 , SCH_No , -1 ,
										  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
										  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
										  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
										  -1 , -1 , -1 , 0 ,
										  0 , 101 );
								}
							}
							//==============================================================================================================
							/*
							// 2012.04.06
							_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 );
							_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order );
							_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) );
							_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
							*/
							//===============================================================================================
							// 2006.03.27 Move Above
							//===============================================================================================
//									SCHEDULING_CHECK_REMAKE_GROUPPING_FLOWDATA( 0 , SCH_No , CHECKING_SIDE , pt , ch ); // 2006.03.27
							//===============================================================================================
							SCHEDULER_Set_MULTI_GROUP_PICK( ch , -1 );
							//===============================================================================================
							LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[ch] );
							//===============================================================================================
							//==============================================================================================================
							// 2006.01.05 // 2006.04.24 Move Here
							//==============================================================================================================
							if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
								if ( !SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( 0 , CHECKING_SIDE , pt , 1 ) ) {
									_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
								}
							}
						}
						else {
							//==============================================================================================================
							// 2006.03.27 Move Here
							//==============================================================================================================
							//-----------------------------------------------------------------------
							_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 ); // 2002.07.23
							//-----------------------------------------------------------------------
							// 2012.04.06
//							_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 ); // 2014.01.10
//							_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); // 2014.01.10
//							_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) ); // 2014.01.10
							//
							_SCH_MODULE_Set_PM_SIDE( ch , pmslot , CHECKING_SIDE ); // 2014.01.10
							_SCH_MODULE_Set_PM_POINTER( ch , pmslot , pt ); // 2014.01.10
							_SCH_MODULE_Set_PM_PATHORDER( ch , pmslot , SCH_Order ); // 2014.01.10
							_SCH_MODULE_Set_PM_WAFER( ch , pmslot , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) ); // 2014.01.10
							//
							_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
							//-----------------------------------------------------------------------
							//
							//===============================================================================================
							//
							_SCH_MODULE_Set_PM_PICKLOCK( ch , pmslot , 1 ); // 2014.01.10
							//
							//==============================================================================================================
							if ( SCH_PM_IS_ALL_PLACED_PROCESSING( ch , TRUE , FALSE , &multiprcs ) ) { // 2014.01.10
								//
								if ( multiprcs ) {
									//
									if ( !SCH_PM_GET_FIRST_SIDE_PT( ch , &m_f_side , &m_f_pt ) ) {
										m_f_side = CHECKING_SIDE;
										m_f_pt = pt;
									}
									//
									_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( m_f_side , m_f_pt , -1 , ch , SCH_No , -1 , 108 );
									//
								}
								else {
									//
									_SCH_CLUSTER_Get_Next_PM_String( CHECKING_SIDE , pt , currpathdo - 1 + 1 , NextPM , 255 ); // 2002.07.16
									_SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
										  ch , -1 ,
										  wsa , -1 , SCH_No , -1 ,
										  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , currpathdo - 1 , SCH_Order , 0 ) ,
										  _SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , SCH_No ) , NextPM , PROCESS_DEFAULT_MINTIME ,
										  currpathdo - 1 , SCH_Order , ( currpathdo >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
										  -1 , -1 , -1 , 0 ,
										  0 , 102 );
								}
							}
							//===============================================================================================
							// Move Here 2006.01.05
							//===============================================================================================
							/*
							// 2012.04.06
							_SCH_MODULE_Set_PM_SIDE_POINTER( ch , CHECKING_SIDE , pt , 0 , 0 );
							_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order );
							_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( 0 , SCH_No ) );
							_SCH_MODULE_Set_TM_WAFER( 0 , SCH_No , 0 );
							*/
							//===============================================================================================
							//==============================================================================================================
							// 2006.01.05 // 2006.04.24 Move Here
							//==============================================================================================================
							if ( _SCH_PRM_GET_MODULE_STOCK( ch ) ) {
								if ( !SCHEDULER_CONTROL_Current_All_Stock_and_Nothing_to_Go( 0 , CHECKING_SIDE , pt , 1 ) ) {
									_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
								}
							}
							//===============================================================================================
							// 2006.04.11
							//===============================================================================================
							if ( SCHEDULER_CONTROL_Next_All_Stock_and_Nothing_to_Go( 0 , CHECKING_SIDE , pt , ch ) ) {
								_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
							}
							//===============================================================================================
						}
						//==============================================================================================================
						// 2007.02.21
						//==============================================================================================================
						_SCH_CLUSTER_After_Place_For_Parallel_Resetting( CHECKING_SIDE , pt , currpathdo - 1 , ch );
						//==============================================================================================================
						SCHEDULING_CHECK_INTERLOCK_PM_CLEAR( CHECKING_SIDE , ch );
						//===============================================================================================================================
						if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 ); // 2006.02.08
						//===============================================================================================================================
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 17 END_TYPE_2(PLACEtoPM) = Chamber(PM%d) finger(%d) pt(%d,%d)\n" , ch - PM1 + 1 , SCH_No , pt , pt2 );
						//----------------------------------------------------------------------
					}
					//----------------------------------------------------------
					Run_Doing = TRUE;
					//----------------------------------------------------------
					break;

			case 11 :
				//
				//----------------------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 16 );
				//----------------------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_PICK_FROM_BM( 0 , CHECKING_SIDE , SCH_SchPt , SCH_Chamber , SCH_Slot , SCH_JumpMode , SCH_Order , SCH_No , &Skip_Pick_BM1 , &Skip_Pick_BM2 , &Run_Doing ); // 2013.02.07

				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 12 :
				//
				//----------------------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 );
				//----------------------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_PLACE_TO_BMPM( 0 , CHECKING_SIDE , SCH_No , SCH_Chamber , SCH_Slot , SCH_Type , SCH_RETMODE , SCH_JumpMode , SCH_Order , SCH_SWITCH , &Run_Doing ); // 2013.02.07
				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 10 :
				//========================================================================================================================================================
				// Move to PM
				//========================================================================================================================================================
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 18 );
				//-------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_MOVE_FOR_GET( 0 , CHECKING_SIDE , SCH_Chamber , SCH_NextFinger , SCH_No );
				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 20 :
				//========================================================================================================================================================
				// Move to BM for Get
				//========================================================================================================================================================
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 19 );
				//-------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_MOVE_FOR_BM_GET( 0 , CHECKING_SIDE , SCH_Chamber , SCH_Slot );
				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 21 :
				//========================================================================================================================================================
				// Move to BM for Put
				//========================================================================================================================================================
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 20 );
				//-------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_MOVE_FOR_BM_PUT( 0 , CHECKING_SIDE , SCH_Chamber , SCH_No , SCH_Slot );
				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 22 :
				//========================================================================================================================================================
				// Move to PM
				//========================================================================================================================================================
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 21 );
				//-------------------------------------------
				//
				Function_Result = SCHEDULER_RUN_MOVE_FOR_PUT( 0 , CHECKING_SIDE , SCH_Chamber , SCH_No );
				//
				if ( Function_Result == -1 ) return 0;
				if ( Function_Result ==  1 ) return 1;
				if ( Function_Result ==  2 ) continue;
				//
				//----------------------------------------------------------
				//
				break;

			case 51 : // 2004.09.08
					//========================================================================================================================================================
					// Pick from AL
					//========================================================================================================================================================
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM  STEP 31 RUN_TYPE_1(PICKfromAL)\n" );
					//----------------------------------------------------------------------
					if ( SCH_RETMODE != SYS_SUCCESS ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because AL Abort%cWHALFAIL\n" , 9 );
						return 0;
					}
					//
					cf = _SCH_MODULE_Get_MFAL_POINTER();
					wsa = _SCH_MODULE_Get_TM_WAFER( 0 , cf );
					pt = _SCH_MODULE_Get_TM_POINTER( 0 , cf );
					ch = _SCH_MODULE_Get_TM_SIDE( 0 , cf ) + CM1;
					//
					if ( _SCH_EQ_PICK_FROM_TM_MAL( CHECKING_SIDE , cf , wsa , ch , _SCH_COMMON_GET_METHOD_ORDER( CHECKING_SIDE , pt ) ) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Pick Fail from AL(%d)[F%c]%cWHTMPICKFAIL AL:%d:%c%c%d\n" , wsa , cf + 'A' , 9 , wsa , cf + 'A' , 9 , wsa );
						return 0;
					}
					_SCH_MODULE_Set_MFAL_WAFER( 0 );
					break;

			default :
					break;
		}
		break;
	}
	//----------------------------------------------------------
	if ( Run_Doing ) {
		//
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) {
				// 2009.03.03
				// SEMES Batch Before Wafer Supply , End Check
				return 1;
			}
		}
		//
		return -3;
	}
	//----------------------------------------------------------
	return 1;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run for Sub TM
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define MAKE_SCHEDULER_MAIN_SUB_TM_for_0( ZZZ , ZZZ2 ) int USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ2##_STYLE_0( int CHECKING_SIDE ) { \
	int i , j , k , Result , pres , ps , pe , bspickx , bmoutf; \
	int wta , wtb , wsa , wsb , Enter_Side, Waiting_Motion , ml; \
	int R_Curr_Chamber , R_Next_Chamber , R_Slot , R_Next_Move , R_Path_Order , R_Wait_Process , R_Schpt , R_JumpMode , R_EndMode , R_Switch , R_NextFinger , R_Retmode , R_Switchable , R_Halfpick; \
	int SCH_Mode = -1 , SCH_No = -1 , SCH_Type , SCH_Order , SCH_Chamber , SCH_Slot , SCH_W_Prcs , SCH_WY , SCH_EndMode , SCH_NextFinger , SCH_Halfpick; \
	int SCH_Small_Time = 999999 , SCH_Buff_Time , SCH_SchPt , SCH_JumpMode , SCH_SWITCH , SCH_RETMODE , SCH_Switchable; \
	long SCH_R_Time = 0; \
	int  SCH_Out_Th_Check = 0; \
	int  SCH_Out_Not_Run = 0; \
	BOOL SCH_PR_RUN; \
	int Function_Result; /* 2006.09.04 */ \
	int Run_Doing; \
	\
	int Skip_Pick_PM1 , Skip_Pick_PM2 , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Check_Again; /* 2013.03.11 */ \
	int Skip_Pick_BM1 , Skip_Pick_BM2; /* 2016.05.12 */ \
\
	if ( _SCH_SUB_DISAPPEAR_OPERATION( ##ZZZ## , 0 ) ) { \
	} \
	/* if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) && ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 1 ) ) { */ /* 2006.07.20 2007.04.02 */ \
	/* if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) && ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) ) { */ /* 2006.07.20 2007.04.02 2008.07.21 */ \
	/* if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) && ( ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) || ( _SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() >= 10 ) ) ) { */ /* 2006.07.20 2007.04.02 2008.07.21 */ \
	/* if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) ) { */ /* 2008.07.29 */ /* 2018.11.29 */ \
	/*	SCHEDULING_CHECK_PRE_END_RUN( ##ZZZ## , CHECKING_SIDE ); */ /* 2006.07.20 */ /* 2018.11.29 */ \
	/* } */ /* 2006.07.20 */ /* 2018.11.29 */ \
\
\
	_SCH_SUB_CHECK_PRE_END( CHECKING_SIDE , ##ZZZ## , -1 , FALSE ); /* 2018.11.29 */ \
\
\
\
	if ( !SCHEDULING_CHECK_LOCKING_MAKE_FREE_TM2( ##ZZZ## , CHECKING_SIDE ) ) { /* 2008.06.20 */ \
		return FALSE; \
	} \
\
	Skip_Pick_BM1 = 0; /* 2016.05.12 */ \
	Skip_Pick_BM2 = 0; /* 2016.05.12 */ \
\
	Skip_Pick_PM1 = 0; /* 2013.03.11 */ \
	Skip_Pick_PM2 = 0; /* 2013.03.11 */ \
\
	Run_Doing = FALSE; \
\
	while( TRUE ) { \
\
		if ( ( RUN_NOT_SELECT_COUNT[##ZZZ##] >= 50 ) || _SCH_SYSTEM_PAUSE_GET( CHECKING_SIDE ) == 2 ) { /* 2013.05.31 */ \
			SCHEDULER_Set_TM_Pick_BM_Signal( ##ZZZ## , 0 ); \
			_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , -1 ); \
			RUN_NOT_SELECT_COUNT[##ZZZ##] = 0; \
			return 1; \
		} \
\
		bmoutf = 0; \
		for ( ml = 0 ; ml < 2 ; ml++ ) { \
			SCH_Mode       = -1; \
			SCH_No         = -1; \
			SCH_Small_Time = 999999; \
			SCH_R_Time     = 0; \
			SCH_WY         = FALSE; \
			SCH_SWITCH     = FALSE; \
			Enter_Side	   = -1; \
			Waiting_Motion = -1; \
			SCH_PR_RUN	   = FALSE; \
			bspickx		   = FALSE; \
			pres		   = -99; \
			\
			\
			SCHEDULER_PROCESSING_STATUS_DIFFERENT( ##ZZZ## , TRUE ); /* 2019.02.08 */ \
			\
			\
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { \
				if ( ml == 0 ) { \
					pres = SCHEDULING_CHECK_Enable_PICK_from_PM_for_GETPR( ##ZZZ## , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , &SCH_PR_RUN , &bspickx , &bmoutf ); \
					switch( pres ) { \
					case -3 : \
						ps = -1; \
						pe = -1; \
						break; \
					case -2 : \
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) != -1 ) ) { \
							ps = -1; \
							pe = -1; \
						} \
						else { \
							ps = PM1; \
							pe = ( PM1 + MAX_PM_CHAMBER_DEPTH ); \
						} \
						break; \
					case -1 : \
						ps = -1; \
						pe = -1; \
						break; \
					default : \
						ps = pres; /* 2005.08.30 */ \
						pe = ps + 1; \
						break; \
					} \
				} \
				else { \
					if ( bmoutf == 0 ) { \
						ps = PM1; \
						pe = ( PM1 + MAX_PM_CHAMBER_DEPTH ); \
					} \
					else if ( bmoutf < 0 ) { \
						/*SCHEDULER_Set_PM_MidCount( ##ZZZ## , -1 );*/ \
						ps = -1; \
						pe = -1; \
					} \
					else { \
						ps = bmoutf; \
						pe = ps + 1; \
					} \
				} \
			} \
			else { \
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) != -1 ) ) { \
					ps = -1; \
					pe = -1; \
				} \
				else { \
					ps = PM1; \
					pe = ( PM1 + MAX_PM_CHAMBER_DEPTH ); \
				} \
			} \
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 201 (ml=%d) CHECK_PICK_from_PM_for_GETPR = (pres=%d,ps=%d,pe=%d,bspickx=%d,bmoutf=%d)\n" , ##ZZZ## + 1 , ml , pres , ps , pe , bspickx , bmoutf ); \
			for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) { \
				if ( _SCH_SYSTEM_USING_GET(k) < 10 ) continue; \
				if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) { /* 2007.02.01 */ \
					for ( i = ps ; i < pe ; i++ ) { \
						if ( Skip_Pick_PM1 == i ) continue; /* 2013.03.11 */ \
						if ( Skip_Pick_PM2 == i ) continue; /* 2013.03.11 */ \
						if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( ##ZZZ## , i , G_PICK ) ) continue; \
						if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , k ) ) { \
							if ( k == CHECKING_SIDE ) { \
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) == -1 ) { \
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted(Disable Chamber Aborted) ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); \
									return 0; \
								} \
							} \
							continue; \
						} \
						if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) { \
							if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) { \
								EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); /* 2006.02.08 */ \
								Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( ##ZZZ## , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , ml ); \
								if ( Result > 0 ) { /* 2007.01.15 */ \
									j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , i , -1 , R_Next_Chamber , &R_Switchable ); \
									if ( j > 0 ) Result = -191 - j; \
								} \
								LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); /* 2006.02.08 */ \
							} \
							else { \
								Result = -9999; \
							} \
						} \
						else { \
							Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( ##ZZZ## , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , ml ); \
							if ( Result > 0 ) { /* 2007.01.15 */ \
								j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , i , -1 , R_Next_Chamber , &R_Switchable ); \
								if ( j > 0 ) Result = -192 - j; \
							} \
						} \
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 01 (k=%d) CHECK_PICK_from_PM = Res=%d,PM=%d,R_Next_Move=%d,[SK=%d,%d],SCH_Small_Time=%d,SCH_Mode=%d\n" , ##ZZZ## + 1 , k , Result , i , R_Next_Move , Skip_Pick_PM1 , Skip_Pick_PM2 , SCH_Small_Time , SCH_Mode ); \
						if ( Result >= 0 ) { \
							if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { \
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Select Log with PICK_from_PM[k=%d,%d]=> PM%d,NextMove=%d,NextCh=%d,WaitPrcs=%d,EndMode=%d,SCHWY=%d,NextFinger=%d\t\t\n" , ##ZZZ## + 1 , k , Result , i - PM1 + 1 , R_Next_Move , R_Next_Chamber , R_Wait_Process , R_EndMode , SCH_WY , R_NextFinger ); \
							} \
							SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( i ); \
							if ( SCH_Buff_Time < SCH_Small_Time ) { \
								Enter_Side	   = k; \
								SCH_Mode       = 0; \
								SCH_No         = i; \
								SCH_Small_Time = SCH_Buff_Time; \
								SCH_Type       = R_Next_Move; \
								SCH_Order      = R_Path_Order; \
								SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
								SCH_W_Prcs	   = R_Wait_Process; \
								SCH_EndMode	   = R_EndMode; \
								SCH_NextFinger = R_NextFinger; \
								if ( SCH_W_Prcs ) { \
									Waiting_Motion = SCH_No; \
								} \
								else { \
									Waiting_Motion = -1; \
								} \
								SCH_SWITCH     = FALSE; /* 2006.02.10 */ \
								SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
								SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
								SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 01 (k=%d) CHECK_PICK_from_PM [OK11]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , ##ZZZ## + 1 , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 ); \
							} \
							else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 0 ) ) { \
								if ( !SCH_PM_IS_PLACING( SCH_No ) && SCH_PM_IS_PLACING( i ) ) { /* 2018.10.20 */ \
									Enter_Side	   = k; \
									SCH_Mode       = 0; \
									SCH_No         = i; \
									SCH_Small_Time = SCH_Buff_Time; \
									SCH_Type       = R_Next_Move; \
									SCH_Order      = R_Path_Order; \
									SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
									SCH_W_Prcs	   = R_Wait_Process; \
									SCH_EndMode	   = R_EndMode; \
									SCH_NextFinger = R_NextFinger; \
									if ( SCH_W_Prcs ) { \
										Waiting_Motion = SCH_No; \
									} \
									else { \
										Waiting_Motion = -1; \
									} \
									SCH_SWITCH     = FALSE; /* 2006.02.10 */ \
									SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
									SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
									SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 01 (k=%d) CHECK_PICK_from_PM [OK12]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , ##ZZZ## + 1 , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 ); \
								} \
								else if ( SCH_PM_IS_PLACING( SCH_No ) == SCH_PM_IS_PLACING( i ) ) { /* 2018.10.20 */ \
									if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) { \
										Enter_Side	   = k; \
										SCH_Mode       = 0; \
										SCH_No         = i; \
										SCH_Small_Time = SCH_Buff_Time; \
										SCH_Type       = R_Next_Move; \
										SCH_Order      = R_Path_Order; \
										SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
										SCH_W_Prcs	   = R_Wait_Process; \
										SCH_EndMode	   = R_EndMode; \
										SCH_NextFinger = R_NextFinger; \
										if ( SCH_W_Prcs ) { \
											Waiting_Motion = SCH_No; \
										} \
										else { \
											Waiting_Motion = -1; \
										} \
										SCH_SWITCH     = FALSE; /* 2006.02.10 */ \
										SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 01 (k=%d) CHECK_PICK_from_PM [OK13]= Res=%d,PM=%d,R_Next_Move=%d,SCH_Small_Time=%d,[SK=%d,%d]\n" , ##ZZZ## + 1 , k , Result , i , R_Next_Move , SCH_Small_Time , Skip_Pick_PM1 , Skip_Pick_PM2 ); \
									} \
								} \
							} \
						} \
					} \
				} \
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) && ( _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) != -1 ) ) { \
					Result = -99; \
				} \
				else { \
\
					SKIP_BM_CHECK_RESETTING( -99 , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 ); /* 2013.12.06 */ \
\
					while ( TRUE ) { /* 2013.12.06 */ \
\
						Skip_Check_Again = FALSE; /* 2013.12.06 */ \
\
						R_Curr_Chamber = -1; /* 2012.09.27 */ \
						if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 1 ) { \
							if ( ( _SCH_SYSTEM_MODE_END_GET( k ) == 0 ) || ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( ##ZZZ## ) >= BUFFER_SWITCH_BATCH_PART ) ) {  /* 2013.03.09 */ \
								if ( Waiting_Motion == -1 ) { \
									if ( KPLT0_CHECK_CASSETTE_TIME_SUPPLY( _SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() ) ) { \
										if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) { \
											/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); */ /* 2013.11.12 */ \
											if ( MULTI_ALL_PM_FULLSWAP && ( SCH_Mode == 0 ) && ( MULTI_ALL_PM_LAST_PICK[##ZZZ##] == SCH_No ) ) { /* 2018.10.20 */ \
												Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
											} \
											else { \
												Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
											} \
										} \
										else { \
											if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) >= 1000 ) { \
												/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE ); */ /* 2013.11.12 */ \
												if ( MULTI_ALL_PM_FULLSWAP && ( SCH_Mode == 0 ) && ( MULTI_ALL_PM_LAST_PICK[##ZZZ##] == SCH_No ) ) { /* 2018.10.20 */ \
													Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
												} \
												else { \
													Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
												} \
											} \
											else { \
												/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE ); */ /* 2013.11.12 */ \
												Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
											} \
										} \
										if ( Result < 0 ) { \
											if ( k == CHECKING_SIDE ) { \
												SCHEDULING_CHECK_BM_Change_FMSIDE_for_Switch( ##ZZZ## , CHECKING_SIDE ); \
											} \
											if ( ( R_Curr_Chamber >= BM1 ) && ( R_Curr_Chamber < TM ) ) { /* 2012.09.27 */ /* 2012.11.09 */ \
												_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); /* 2012.11.09 */ \
												Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 );  /* 2013.12.06 */ \
											} \
										} \
										else { /* 2007.01.15 */ \
											j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable ); \
											if ( j > 0 ) { \
												Result = -193 - j; \
												_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); /* 2012.09.26 */ \
												Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 );  /* 2013.12.06 */ \
											} \
											else { \
												_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); /* 2012.11.09 */ \
											} \
										} \
									} \
									else { \
										Result = -99; \
									} \
								} \
								else { \
									Result = -99; \
								} \
							} \
							else { \
								Result = -99; \
							} \
						} \
						else { \
							if ( Waiting_Motion == -1 ) { \
								if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) { \
									/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); */ /* 2013.11.12 */ \
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
								} \
								else { \
									if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) >= 1000 ) { \
										/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE ); */ /* 2013.11.12 */ \
										Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , FALSE , TRUE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
									} \
									else { \
										/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE ); */ /* 2013.11.12 */ \
										Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 0 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_Schpt , &R_JumpMode , TRUE , FALSE , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
									} \
								} \
								if ( Result > 0 ) { /* 2007.01.15 */ \
									j = SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable ); \
									if ( j > 0 ) { \
										Result = -194 - j; \
										_SCH_TIMER_SET_PROCESS_TIME_SKIP( 11 , R_Curr_Chamber ); /* 2012.09.26 */ \
										Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 );  /* 2013.12.06 */ \
									} \
									else { \
										_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); /* 2012.11.09 */ \
									} \
								} \
								else { /* 2012.09.27 */ /* 2012.11.09 */ \
									if ( ( R_Curr_Chamber >= BM1 ) && ( R_Curr_Chamber < TM ) ) { \
										_SCH_TIMER_SET_PROCESS_TIME_SKIP( 10 , R_Curr_Chamber ); \
										Skip_Check_Again = SKIP_BM_CHECK_RESETTING( R_Curr_Chamber , &Skip1 , &Skip2 , &Skip3 , &Skip4 , &Skip5 );  /* 2013.12.06 */ \
									} \
								} \
							} \
							else { \
								Result = -99; \
							} \
						} \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 04 (k=%d) CHECK_PICK_from_BM = Res=%d,R_Curr_Chamber=%d,R_Slot=%d,R_Next_Chamber=%d,R_Next_Move=%d,R_Path_Order=%d,R_Schpt=%d,R_JumpMode=%d,SCH_Small_Time=%d,SCH_Mode=%d,[%d,%d,%d,%d,%d]\n" , ##ZZZ## + 1 , k , Result , R_Curr_Chamber , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , SCH_Small_Time , SCH_Mode , Skip1 , Skip2 , Skip_Pick_BM1 , Skip_Pick_BM2 , Skip_Check_Again ); \
\
						if ( !Skip_Check_Again ) break; /* 2013.12.06 */ \
\
					} \
				} \
/*				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 02 (k=%d) CHECK_PICK_from_BM = %d\n" , ##ZZZ## + 1 , k , Result ); */ \
				if ( Result >= 0 ) { \
					if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { \
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Select Log with PICK_from_BM[k=%d,%d]=> BM%d,Slot=%d,NextCh=%d,NextMove=%d,PathOrder=%d,Schpt=%d,JumpMode=%d,BMMode=%d\t\t\n" , ##ZZZ## + 1 , k , Result , R_Curr_Chamber - BM1 + 1 , R_Slot , R_Next_Chamber , R_Next_Move , R_Path_Order , R_Schpt , R_JumpMode , _SCH_MODULE_Get_BM_FULL_MODE( R_Curr_Chamber ) ); \
					} \
					SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( R_Curr_Chamber ); \
					if ( SCH_Buff_Time < SCH_Small_Time ) { \
						Enter_Side			= k; \
						SCH_Mode			= 11; \
						SCH_Chamber			= R_Curr_Chamber; \
						SCH_No				= R_Next_Chamber; \
						SCH_Slot			= R_Slot; \
						SCH_Order			= R_Path_Order; \
						SCH_Small_Time		= SCH_Buff_Time; \
						SCH_JumpMode		= R_JumpMode; \
						SCH_SchPt			= R_Schpt; \
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 ) \
							SCH_R_Time	= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
						else \
							SCH_R_Time	= 0; \
						SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
						SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
					} \
					else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 11 ) ) { \
						if ( ( Enter_Side != k ) && ( SCH_Chamber == R_Curr_Chamber ) ) { \
							if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { /* 2013.04.10 */ \
								Enter_Side			= k; \
								SCH_Mode			= 11; \
								SCH_Chamber			= R_Curr_Chamber; \
								SCH_No				= R_Next_Chamber; \
								SCH_Slot			= R_Slot; \
								SCH_Order			= R_Path_Order; \
								SCH_Small_Time		= SCH_Buff_Time; \
								SCH_SchPt			= R_Schpt; \
								SCH_JumpMode		= R_JumpMode; \
								SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
								SCH_Switchable = R_Switchable; \
								SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
							} \
							else { \
								if ( SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_TM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( R_Curr_Chamber ) ) == 2 ) { \
									if ( SCH_Slot < R_Slot ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
									} \
								} \
								else { \
									if ( SCH_Slot > R_Slot ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
									} \
								} \
							} \
						} \
						else if ( ( Enter_Side != k ) && ( SCH_Chamber != R_Curr_Chamber ) ) { /* 2013.04.10 */ \
							if ( ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( ##ZZZ## ) > BUFFER_SWITCH_BATCH_PART ) ) { \
								if ( ( SCH_Type > 0 ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) > 0 ) ) { \
									if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
										SCH_Switchable = R_Switchable; \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
									} \
								} \
								else { \
									if ( SCH_Type < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
										SCH_Switchable = R_Switchable; \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
									} \
									else if ( SCH_Type == SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ) ) { \
										if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { \
											Enter_Side			= k; \
											SCH_Mode			= 11; \
											SCH_Chamber			= R_Curr_Chamber; \
											SCH_No				= R_Next_Chamber; \
											SCH_Slot			= R_Slot; \
											SCH_Order			= R_Path_Order; \
											SCH_Small_Time		= SCH_Buff_Time; \
											SCH_SchPt			= R_Schpt; \
											SCH_JumpMode		= R_JumpMode; \
											SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
											SCH_Switchable = R_Switchable; \
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
										} \
										else if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) == _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { \
											if ( _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , SCH_Chamber ) < _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , R_Curr_Chamber ) ) { \
												Enter_Side			= k; \
												SCH_Mode			= 11; \
												SCH_Chamber			= R_Curr_Chamber; \
												SCH_No				= R_Next_Chamber; \
												SCH_Slot			= R_Slot; \
												SCH_Order			= R_Path_Order; \
												SCH_Small_Time		= SCH_Buff_Time; \
												SCH_SchPt			= R_Schpt; \
												SCH_JumpMode		= R_JumpMode; \
												SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
												SCH_Switchable = R_Switchable; \
												SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
											} \
										} \
									} \
								} \
							} \
							else { \
								if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) < _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { \
									Enter_Side			= k; \
									SCH_Mode			= 11; \
									SCH_Chamber			= R_Curr_Chamber; \
									SCH_No				= R_Next_Chamber; \
									SCH_Slot			= R_Slot; \
									SCH_Order			= R_Path_Order; \
									SCH_Small_Time		= SCH_Buff_Time; \
									SCH_SchPt			= R_Schpt; \
									SCH_JumpMode		= R_JumpMode; \
									SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
									SCH_Switchable = R_Switchable; \
									SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
								} \
								else if ( _SCH_CLUSTER_Get_SupplyID( k , R_Schpt ) == _SCH_CLUSTER_Get_SupplyID( Enter_Side , SCH_SchPt ) ) { \
									if ( _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , SCH_Chamber ) < _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , R_Curr_Chamber ) ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
										SCH_Switchable = R_Switchable; \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); \
									} \
								} \
							} \
						} \
						else { \
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) > 0 ) { \
								if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) { \
									Enter_Side			= k; \
									SCH_Mode			= 11; \
									SCH_Chamber			= R_Curr_Chamber; \
									SCH_No				= R_Next_Chamber; \
									SCH_Slot			= R_Slot; \
									SCH_Order			= R_Path_Order; \
									SCH_Small_Time		= SCH_Buff_Time; \
									SCH_SchPt			= R_Schpt; \
									SCH_JumpMode		= R_JumpMode; \
									SCH_R_Time			= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
									SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
									SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
								} \
							} \
							else { \
								/* wta = _SCH_MODULE_Get_PM_WAFER( R_Next_Chamber , 0 ); */ /* 2014.01.10 */ \
								/* wtb = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); */ /* 2014.01.10 */ \
								wta = SCH_PM_IS_FREE( R_Next_Chamber ) ? 0 : 1; /* 2014.01.10 */ \
								wtb = SCH_PM_IS_FREE( SCH_No ) ? 0 : 1; /* 2014.01.10 */ \
								if      ( ( wta <= 0 ) && ( wtb > 0 ) ) { \
									Enter_Side			= k; \
									SCH_Mode			= 11; \
									SCH_Chamber			= R_Curr_Chamber; \
									SCH_No				= R_Next_Chamber; \
									SCH_Slot			= R_Slot; \
									SCH_Order			= R_Path_Order; \
									SCH_Small_Time		= SCH_Buff_Time; \
									SCH_SchPt			= R_Schpt; \
									SCH_JumpMode		= R_JumpMode; \
									SCH_R_Time			= 0; \
									SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
									SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
								} \
								else if ( ( wta <= 0 ) && ( wtb <= 0 ) ) { \
									if ( _SCH_PRM_GET_Putting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Putting_Priority( SCH_No ) ) { \
										Enter_Side			= k; \
										SCH_Mode			= 11; \
										SCH_Chamber			= R_Curr_Chamber; \
										SCH_No				= R_Next_Chamber; \
										SCH_Slot			= R_Slot; \
										SCH_Order			= R_Path_Order; \
										SCH_Small_Time		= SCH_Buff_Time; \
										SCH_SchPt			= R_Schpt; \
										SCH_JumpMode		= R_JumpMode; \
										SCH_R_Time			= 0; \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
									} \
								} \
								else if ( ( wta > 0 ) && ( wtb > 0 ) ) { \
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) <= 0 ) { \
										if ( _SCH_PRM_GET_Getting_Priority( R_Next_Chamber ) < _SCH_PRM_GET_Getting_Priority( SCH_No ) ) { \
											Enter_Side			= k; \
											SCH_Mode			= 11; \
											SCH_Chamber			= R_Curr_Chamber; \
											SCH_No				= R_Next_Chamber; \
											SCH_Slot			= R_Slot; \
											SCH_Order			= R_Path_Order; \
											SCH_Small_Time		= SCH_Buff_Time; \
											SCH_SchPt			= R_Schpt; \
											SCH_JumpMode		= R_JumpMode; \
											SCH_R_Time			= 99999999; \
											SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
											SCH_Type       = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( R_Curr_Chamber , BUFFER_OUTWAIT_STATUS ); /* 2013.04.10 */ \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
				if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) { /* 2007.02.01 */ \
					for ( i = TA_STATION ; i <= TB_STATION ; i++ ) { \
						if ( ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 0 ) || ( _SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) ) { /* 2006.05.04 */ \
							if ( Waiting_Motion != -1 ) break; \
						} \
						if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { \
							Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( ##ZZZ## , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[##ZZZ##] : 0 ); \
						} \
						else { \
							Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( ##ZZZ## , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , TRUE , FALSE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[##ZZZ##] : 0 ); \
						} \
/*						if ( ( Result == -99 ) && ( !SCH_WY ) ) { */ /* 2013.10.25 */ \
/*							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted(No More Run Chamber 3) ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); */ /* 2013.10.25 */ \
/*							return 0; */ /* 2013.10.25 */ \
/*						} */ /* 2013.10.25 */ \
						if ( ( Result >= 0 ) && ( R_Next_Chamber >= PM1 ) ) { /* 2006.03.27 */ \
							if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , R_Next_Chamber ) >= 2 ) { \
								if ( !SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( ##ZZZ## , k , i , R_Next_Chamber ) ) Result = -1001; \
							} \
						} \
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 03 (k=%d) CHECK_PLACE_from_TM = Res=%d,R_Next_Chamber=%d,R_Slot=%d,R_Next_Move=%d,R_JumpMode=%d,R_Retmode=%d,SCH_Small_Time=%d,SCH_Mode=%d\n" , ##ZZZ## + 1 , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_JumpMode , R_Retmode , SCH_Small_Time , SCH_Mode ); \
						if ( Result >= 0 ) { \
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) { \
								if ( R_Next_Move == SCHEDULER_MOVING_OUT ) { \
									if ( SCHEDULER_Get_TM_MidCount( ##ZZZ## - 1 ) == 11 ) { \
										Result = -1; \
									} \
								} \
							} \
						} \
						if ( Result >= 0 ) { \
							if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { \
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Select Log with PLACE_From_TM[k=%d,%d]=> NextCh=%d,Slot=%d,NextMove=%d,PathOrder=%d,JumpMode=%d,Retmode=%d\t\t\n" , ##ZZZ## + 1 , k , Result , R_Next_Chamber , R_Slot , R_Next_Move , R_Path_Order , R_JumpMode , R_Retmode ); \
							} \
							\
							if ( Multi_Expect_Pick_and_Place_Skip( ##ZZZ## , i , R_Next_Chamber ) ) continue; /* 2018.10.20 */ \
							\
							SCH_Buff_Time = _SCH_PRM_GET_Putting_Priority( R_Next_Chamber ); \
							R_Switch = FALSE; \
							if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) { \
								if ( _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) == R_Next_Chamber ) { \
									R_Switch = TRUE; \
									SCH_Buff_Time = -9999; \
								} \
							} \
							if ( SCH_Buff_Time != -9999 ) { /* 2006.07.20 */ \
								if ( SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( ##ZZZ## , i ) ) SCH_Buff_Time = -9999; \
							} \
							if ( SCH_Buff_Time < SCH_Small_Time ) { \
								Enter_Side		= k; \
								SCH_Mode		= 12; \
								SCH_No			= i; \
								SCH_Chamber		= R_Next_Chamber; \
								SCH_Slot		= R_Slot; \
								SCH_Small_Time	= SCH_Buff_Time; \
								SCH_Type        = R_Next_Move; \
								SCH_Order		= R_Path_Order; \
								SCH_R_Time	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ); \
								SCH_JumpMode	= R_JumpMode; \
								SCH_SWITCH		= R_Switch; \
								SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
								if ( R_Switch ) i = 99; \
							} \
							else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 12 ) ) { \
								if ( ( Enter_Side == k ) && !( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) ) { \
									if ( ( SCH_Chamber == R_Next_Chamber ) && ( SCH_Type == R_Next_Move ) && ( SCH_Type == SCHEDULER_MOVING_OUT ) ) { \
										switch( _SCH_ROBOT_GET_FAST_PLACE_FINGER( ##ZZZ## , R_Next_Chamber ) ) { \
										case TA_STATION : \
											Result = -1; \
											break; \
										case TB_STATION : \
											Result = -1; \
											Enter_Side		= k; \
											SCH_Mode		= 12; \
											SCH_No			= i; \
											SCH_Chamber		= R_Next_Chamber; \
											SCH_Slot		= R_Slot; \
											SCH_Small_Time	= SCH_Buff_Time; \
											SCH_Type        = R_Next_Move; \
											SCH_Order		= R_Path_Order; \
											SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ); \
											SCH_JumpMode	= R_JumpMode; \
											SCH_SWITCH		= R_Switch; \
											SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
											if ( R_Switch ) i = 99; \
											break; \
										} \
									} \
								} \
								if ( Result >= 0 ) { \
									wsa = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ); \
									wsb = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , i ); \
									if ( wsa == wsb ) { \
										\
										/* 2019.02.08 */ \
										\
										/* wta = _SCH_MODULE_Get_TM_POINTER( ##ZZZ## , SCH_No ) + _SCH_MODULE_Get_TM_POINTER2(##ZZZ##,SCH_No)*100; */ \
										/* wtb = _SCH_MODULE_Get_TM_POINTER( ##ZZZ## , i ) + _SCH_MODULE_Get_TM_POINTER2(##ZZZ##,i)*100; */ \
										/* if      ( ( wta / 100 ) >= ( wta % 100 ) ) wta = wta / 100; */ \
										/* else if ( ( wta / 100 ) <= ( wta % 100 ) ) wta = wta % 100; */ \
										/* if      ( ( wtb / 100 ) >= ( wtb % 100 ) ) wtb = wtb / 100; */ \
										/* else if ( ( wtb / 100 ) <= ( wtb % 100 ) ) wtb = wtb % 100; */ \
										\
										/* 2019.02.08 */ \
										\
										\
										wta = _SCH_CLUSTER_Get_SupplyID( wsa , _SCH_MODULE_Get_TM_POINTER(##ZZZ##,SCH_No) ); \
										wtb = _SCH_CLUSTER_Get_SupplyID( wsb , _SCH_MODULE_Get_TM_POINTER(##ZZZ##,i) ); \
										\
										if ( wta > wtb ) { \
											Enter_Side		= k; \
											SCH_Mode		= 12; \
											SCH_No			= i; \
											SCH_Chamber		= R_Next_Chamber; \
											SCH_Slot		= R_Slot; \
											SCH_Small_Time	= SCH_Buff_Time; \
											SCH_Type        = R_Next_Move; \
											SCH_Order		= R_Path_Order; \
											SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ); \
											SCH_JumpMode	= R_JumpMode; \
											SCH_SWITCH		= R_Switch; \
											SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
											if ( R_Switch ) i = 99; \
										} \
									} \
									else { \
										if ( SCH_Chamber == R_Next_Chamber ) { \
											if ( _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , SCH_No ) < _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , i ) ) { \
												Enter_Side		= k; \
												SCH_Mode		= 12; \
												SCH_No			= i; \
												SCH_Chamber		= R_Next_Chamber; \
												SCH_Slot		= R_Slot; \
												SCH_Small_Time	= SCH_Buff_Time; \
												SCH_Type        = R_Next_Move; \
												SCH_Order		= R_Path_Order; \
												SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ); \
												SCH_JumpMode	= R_JumpMode; \
												SCH_SWITCH		= R_Switch; \
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
												if ( R_Switch ) i = 99; \
											} \
										} \
										else { \
											if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ) ) { \
												Enter_Side		= k; \
												SCH_Mode		= 12; \
												SCH_No			= i; \
												SCH_Chamber		= R_Next_Chamber; \
												SCH_Slot		= R_Slot; \
												SCH_Small_Time	= SCH_Buff_Time; \
												SCH_Type        = R_Next_Move; \
												SCH_Order		= R_Path_Order; \
												SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_END( 0 , R_Next_Chamber ); \
												SCH_JumpMode	= R_JumpMode; \
												SCH_SWITCH		= R_Switch; \
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
												if ( R_Switch ) i = 99; \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
				if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() && ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) ) { /* 2007.02.01 */ \
					if ( Waiting_Motion != -1 ) { \
						if ( _SCH_MODULE_Get_TM_Move_Signal( ##ZZZ## ) == -1 ) { \
							Enter_Side	   = k; \
							SCH_Mode       = 10; \
							SCH_Chamber    = Waiting_Motion; \
						} \
					} \
					else if ( SCH_Mode < 0 ) { \
						for ( i = ps ; i < pe ; i++ ) { \
							if ( Skip_Pick_PM1 == i ) continue; /* 2013.03.11 */ \
							if ( Skip_Pick_PM2 == i ) continue; /* 2013.03.11 */ \
							if ( !_SCH_MODULE_GET_USE_ENABLE( i , TRUE , k ) ) continue; \
							if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( ##ZZZ## , i , G_PICK ) ) continue; \
							if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) { \
								if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) { \
									EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); /* 2006.02.08 */ \
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( ##ZZZ## , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , TRUE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , 0 ); \
									if ( Result > 0 ) { /* 2007.01.15 */ \
										if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , i , -1 , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -195; \
									} \
									LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); /* 2006.02.08 */ \
								} \
								else { \
									Result = -9999; \
								} \
							} \
							else { \
								Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( ##ZZZ## , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , TRUE , &R_Wait_Process , 0 , 0 , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , 0 ); \
								if ( Result > 0 ) { /* 2007.01.15 */ \
									if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , i , -1 , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -196; \
								} \
							} \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 51 (k=%d) CHECK_PICK_from_PM = %d\n" , ##ZZZ## + 1 , k , Result ); \
							if ( Result >= 0 ) { \
								if ( _SCH_MODULE_Get_TM_Move_Signal( ##ZZZ## ) == -1 ) { \
									SCH_Buff_Time = 9999; \
									if ( SCH_Buff_Time < SCH_Small_Time ) { \
										Enter_Side	   = k; \
										SCH_Mode       = 10; \
										/* SCH_Chamber    = i; */ /* 2013.12.18 */ \
										SCH_No         = i; /* 2013.12.18 */ \
										SCH_Small_Time = SCH_Buff_Time; \
										SCH_Type       = R_Next_Move; \
										SCH_Order      = R_Path_Order; \
										SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ); \
										SCH_EndMode	   = R_EndMode; \
										SCH_NextFinger = R_NextFinger; \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
										SCH_Chamber    = R_Next_Chamber; /* 2013.12.18 */ \
									} \
									else if ( SCH_Buff_Time == SCH_Small_Time ) { \
										if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) ) { \
											Enter_Side	   = k; \
											SCH_Mode       = 10; \
											/* SCH_Chamber    = i; */ /* 2013.12.18 */ \
											SCH_No         = i; /* 2013.12.18 */ \
											SCH_Small_Time = SCH_Buff_Time; \
											SCH_Type       = R_Next_Move; \
											SCH_Order      = R_Path_Order; \
											SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ); \
											SCH_EndMode	   = R_EndMode; \
											SCH_NextFinger = R_NextFinger; \
											SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
											SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
											SCH_Chamber    = R_Next_Chamber; /* 2013.12.18 */ \
										} \
									} \
								} \
							} \
						} \
						for ( i = TA_STATION ; i <= TB_STATION ; i++ ) { \
							Result = SCHEDULING_CHECK_for_Enable_PLACE_From_TM( ##ZZZ## , k , i , &R_Next_Chamber , &R_Slot , &R_Next_Move , &R_Path_Order , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , TRUE , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_JumpMode , -1 , &R_Retmode , MULTI_ALL_PM_FULLSWAP ? MULTI_ALL_PM_LAST_PICK[##ZZZ##] : 0 ); \
							if ( ( Result >= 0 ) && ( R_Next_Chamber >= PM1 ) ) { /* 2006.03.27 */ \
								if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , R_Next_Chamber ) >= 2 ) { \
									if ( !SCHEDULING_CHECK_for_MG_Post_PLACE_From_TM( ##ZZZ## , k , i , R_Next_Chamber ) ) Result = -1001; \
								} \
							} \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 55 (k=%d) CHECK_PLACE_From_TM = %d\n" , ##ZZZ## + 1 , k , Result ); \
							if ( ( Result >= 0 ) && ( _SCH_MODULE_Get_TM_Move_Signal( ##ZZZ## ) == -1 ) ) { \
								if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) < PROCESS_INDICATOR_POST ) ) SCH_Buff_Time = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , R_Next_Chamber ); \
								else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( R_Next_Chamber ) >= PROCESS_INDICATOR_POST ) SCH_Buff_Time = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , R_Next_Chamber ); \
								else                                                                    continue; \
								if ( 9999 < SCH_Small_Time ) { \
									Enter_Side	   = k; \
									SCH_Mode       = 22; \
									SCH_No		   = i; \
									SCH_Chamber    = R_Next_Chamber; \
									SCH_Slot	   = R_Slot; \
									SCH_Small_Time = 9999; \
									SCH_Type       = R_Next_Move; \
									SCH_Order      = R_Path_Order; \
									SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
									SCH_JumpMode   = R_JumpMode; \
									SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
								} \
								else if ( 9999 == SCH_Small_Time ) { \
									if ( SCH_Mode == 22 ) { \
										if ( SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( ##ZZZ## , i ) ) { \
											if ( ( i == TA_STATION ) || ( ( i == TB_STATION ) && ( !SCHEDULING_CHECK_ALREADY_PRE_BEFORE_PLACE( ##ZZZ## , TA_STATION ) ) ) ) { \
												Result = -1; \
												Enter_Side		= k; \
												SCH_Mode		= 22; \
												SCH_No			= i; \
												SCH_Chamber		= R_Next_Chamber; \
												SCH_Slot		= R_Slot; \
												SCH_Small_Time	= 9999; \
												SCH_Type        = R_Next_Move; \
												SCH_Order		= R_Path_Order; \
												SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
												SCH_JumpMode	= R_JumpMode; \
												SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
											} \
										} \
										else { \
											if ( Enter_Side	== k ) { \
												if ( ( SCH_Chamber == R_Next_Chamber ) && ( SCH_Type == R_Next_Move ) && ( SCH_Type == SCHEDULER_MOVING_OUT ) ) { \
													switch( _SCH_ROBOT_GET_FAST_PLACE_FINGER( ##ZZZ## , R_Next_Chamber ) ) { \
													case TA_STATION : \
														Result = -1; \
														break; \
													case TB_STATION : \
														Result = -1; \
														Enter_Side		= k; \
														SCH_Mode		= 22; \
														SCH_No			= i; \
														SCH_Chamber		= R_Next_Chamber; \
														SCH_Slot		= R_Slot; \
														SCH_Small_Time	= 9999; \
														SCH_Type        = R_Next_Move; \
														SCH_Order		= R_Path_Order; \
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
														SCH_JumpMode	= R_JumpMode; \
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
														break; \
													} \
												} \
											} \
										} \
										if ( Result >= 0 ) { \
											wsa = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ); \
											wsb = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , i ); \
											if ( wsa == wsb ) { \
												wta = _SCH_MODULE_Get_TM_POINTER(##ZZZ##,SCH_No) + _SCH_MODULE_Get_TM_POINTER2(##ZZZ##,SCH_No)*100; \
												wtb = _SCH_MODULE_Get_TM_POINTER(##ZZZ##,i) + _SCH_MODULE_Get_TM_POINTER2(##ZZZ##,i)*100; \
												if      ( ( wta / 100 ) >= ( wta % 100 ) ) wta = wta / 100; \
												else if ( ( wta / 100 ) <= ( wta % 100 ) ) wta = wta % 100; \
												if      ( ( wtb / 100 ) >= ( wtb % 100 ) ) wtb = wtb / 100; \
												else if ( ( wtb / 100 ) <= ( wtb % 100 ) ) wtb = wtb % 100; \
												if ( wta > wtb ) { \
													Enter_Side		= k; \
													SCH_Mode		= 22; \
													SCH_No			= i; \
													SCH_Chamber		= R_Next_Chamber; \
													SCH_Slot		= R_Slot; \
													SCH_Small_Time	= 9999; \
													SCH_Type        = R_Next_Move; \
													SCH_Order		= R_Path_Order; \
													SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
													SCH_JumpMode	= R_JumpMode; \
													SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
												} \
											} \
											else { \
												if ( SCH_Chamber == R_Next_Chamber ) { \
													if ( _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , SCH_No ) < _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , i ) ) { \
														Enter_Side		= k; \
														SCH_Mode		= 22; \
														SCH_No			= i; \
														SCH_Chamber		= R_Next_Chamber; \
														SCH_Slot		= R_Slot; \
														SCH_Small_Time	= 9999; \
														SCH_Type        = R_Next_Move; \
														SCH_Order		= R_Path_Order; \
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
														SCH_JumpMode	= R_JumpMode; \
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
													} \
												} \
												else { \
													if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) { \
														Enter_Side		= k; \
														SCH_Mode		= 22; \
														SCH_No			= i; \
														SCH_Chamber		= R_Next_Chamber; \
														SCH_Slot		= R_Slot; \
														SCH_Small_Time	= 9999; \
														SCH_Type        = R_Next_Move; \
														SCH_Order		= R_Path_Order; \
														SCH_R_Time 	    = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
														SCH_JumpMode	= R_JumpMode; \
														SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
													} \
												} \
											} \
										} \
									} \
									else { \
										if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ) ) { \
											Enter_Side	   = k; \
											SCH_Mode       = 22; \
											SCH_No		   = i; \
											SCH_Chamber    = R_Next_Chamber; \
											SCH_Slot	   = R_Slot; \
											SCH_Small_Time = 9999; \
											SCH_Type       = R_Next_Move; \
											SCH_Order      = R_Path_Order; \
											SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Next_Chamber ); \
											SCH_JumpMode   = R_JumpMode; \
											SCH_RETMODE		= ( R_Retmode > 0 ) ? 1 : 0; /* 2019.01.30 */ \
										} \
									} \
								} \
							} \
						} \
						if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) { \
							wsa = 0; \
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { \
								if ( !SCHEDULING_CHECK_Enable_MOVE_BM_for_GETPR( ##ZZZ## ) ) wsa = 1; \
							} \
							if ( wsa == 0 ) { \
								/* Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 1 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE ); */ /* 2013.11.12 */ \
								Result = SCHEDULING_CHECK_for_Enable_PICK_from_BM( ##ZZZ## , k , 0 , &R_Curr_Chamber , &R_Slot , &R_Next_Chamber , &R_Next_Move , &R_Path_Order , 1 , Get_Prm_MODULE_BUFFER_SWITCH_MODE() , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(k) , &R_Schpt , &R_JumpMode , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) && ( ps != -1 ) && ( ( ps + 1 ) == pe ) , bspickx , TRUE , Skip1 , Skip2 , Skip3 , Skip4 , Skip5 , Skip_Pick_BM1 , Skip_Pick_BM2 ); /* 2013.11.12 */ \
								if ( Result > 0 ) { /* 2007.01.15 */ \
									if ( SCHEDULING_CHECK_Switch_SINGLE_PICK_REJECT( ##ZZZ## , k , R_Curr_Chamber , R_Slot , R_Next_Chamber , &R_Switchable ) > 0 ) Result = -197; \
								} \
							} \
							else { \
								Result = -1; \
							} \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 52 (k=%d) CHECK_PICK_from_BM = %d\n" , ##ZZZ## + 1 , k , Result ); \
							if ( Result >= 0 ) { \
								if ( _SCH_MODULE_Get_TM_Move_Signal( ##ZZZ## ) == -1 ) { \
									SCH_Buff_Time = 9999; \
									if ( SCH_Buff_Time < SCH_Small_Time ) { \
										Enter_Side		= k; \
										SCH_Mode		= 20; \
										SCH_Chamber		= R_Curr_Chamber; \
										SCH_Slot		= R_Slot; \
										SCH_Small_Time	= SCH_Buff_Time; \
										SCH_SchPt		= R_Schpt; \
										SCH_JumpMode	= R_JumpMode; \
										SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber ); \
										SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
									} \
									else if ( SCH_Buff_Time == SCH_Small_Time ) { \
										if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber ) ) { \
											Enter_Side		= k; \
											SCH_Mode		= 20; \
											SCH_Chamber		= R_Curr_Chamber; \
											SCH_Slot		= R_Slot; \
											SCH_Small_Time	= SCH_Buff_Time; \
											SCH_SchPt		= R_Schpt; \
											SCH_JumpMode	= R_JumpMode; \
											SCH_R_Time		= _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , R_Curr_Chamber ); \
											SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
										} \
									} \
								} \
							} \
							else if ( wsa == 0 ) { \
								if ( _SCH_MODULE_Get_TM_Move_Signal( ##ZZZ## ) == -1 ) { \
									if ( ( SCHEDULER_Get_TM_MidCount( ##ZZZ## - 1 ) == 11 ) || ( SCHEDULER_Get_TM_MidCount( ##ZZZ## - 1 ) == 12 ) ) { \
										if ( _SCH_ROBOT_GET_FINGER_FREE_COUNT( ##ZZZ## ) > 0 ) { \
											R_Curr_Chamber = SCHEDULING_CHECK_Get_BM_FREE_IN_CHAMBER_FOR_PICK_BM( CHECKING_SIDE , ##ZZZ## ); \
											if ( R_Curr_Chamber >= 0 ) { \
												SCH_Buff_Time = 9999; \
												if ( SCH_Buff_Time < SCH_Small_Time ) { \
													Enter_Side		= k; \
													SCH_Mode		= 20; \
													SCH_Chamber		= R_Curr_Chamber; \
													SCH_Slot		= 1; \
													SCH_Small_Time	= SCH_Buff_Time; \
													SCH_SchPt		= R_Schpt; \
													SCH_JumpMode	= R_JumpMode; \
													SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## - 1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber ); \
													SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
												} \
												else if ( SCH_Buff_Time == SCH_Small_Time ) { \
													if ( SCH_R_Time > ( _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## - 1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber ) ) ) { \
														Enter_Side		= k; \
														SCH_Mode		= 20; \
														SCH_Chamber		= R_Curr_Chamber; \
														SCH_Slot		= 1; \
														SCH_Small_Time	= SCH_Buff_Time; \
														SCH_SchPt		= R_Schpt; \
														SCH_JumpMode	= R_JumpMode; \
														SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## - 1 , 0 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , R_Curr_Chamber ); \
														SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
													} \
												} \
											} \
										} \
									} \
									if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) { \
										if ( SCHEDULER_Get_TM_MidCount( ##ZZZ## - 1 ) == 2 ) { \
											SCH_No = -1; \
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( ##ZZZ## , TB_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , TB_STATION ) != 0 ) && ( _SCH_MODULE_Get_TM_TYPE( ##ZZZ## , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) SCH_No = TB_STATION; \
											if ( ( _SCH_ROBOT_GET_FINGER_HW_USABLE( ##ZZZ## , TA_STATION ) ) && ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , TA_STATION ) != 0 ) && ( _SCH_MODULE_Get_TM_TYPE( ##ZZZ## , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) SCH_No = TA_STATION; \
											if ( SCH_No != -1 ) { \
												SCH_Buff_Time = 9999; \
												if ( SCH_Buff_Time < SCH_Small_Time ) { \
													Enter_Side		= k; \
													SCH_Mode		= 21; \
													SCH_Chamber		= Get_Prm_MODULE_BUFFER_SINGLE_MODE(); \
													SCH_Slot		= 1; \
													SCH_Small_Time	= SCH_Buff_Time; \
													SCH_SchPt		= R_Schpt; \
													SCH_JumpMode	= R_JumpMode; \
													SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## - 1 , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() ); \
													SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
												} \
												else if ( SCH_Buff_Time == SCH_Small_Time ) { \
													if ( SCH_R_Time > ( _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() ) ) ) { \
														Enter_Side		= k; \
														SCH_Mode		= 21; \
														SCH_Chamber		= Get_Prm_MODULE_BUFFER_SINGLE_MODE(); \
														SCH_Slot		= 1; \
														SCH_Small_Time	= SCH_Buff_Time; \
														SCH_SchPt		= R_Schpt; \
														SCH_JumpMode	= R_JumpMode; \
														SCH_R_Time		= _SCH_TIMER_GET_ROBOT_TIME_REMAIN( ##ZZZ## , 1 ) + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , Get_Prm_MODULE_BUFFER_SINGLE_MODE() ); \
														SCH_Switchable = R_Switchable; /* 2007.01.16 */ \
													} \
												} \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
				if ( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) && ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) == 0 ) ) { /* 2007.02.01 */ \
					if ( SCH_Mode < 0 ) { /* 2006.02.13 */ \
						if ( SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( ##ZZZ## , 1 ) ) { \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 101 (k=%d) CHECK_PICK_from_PM_MG = s=%d,e=%d\n" , ##ZZZ## + 1 , k , ps , pe ); \
							R_Retmode = FALSE; \
							for ( i = ps ; i < pe ; i++ ) { \
								if ( Skip_Pick_PM1 == i ) continue; /* 2013.03.11 */ \
								if ( Skip_Pick_PM2 == i ) continue; /* 2013.03.11 */ \
								if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) continue; \
								if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) { \
									EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); \
									Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM_MG( ##ZZZ## , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) && ( Get_Sub_Alg_Data_STYLE_0() != A0_SUB_ALGORITHM_NO7 ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_Halfpick , ml , &R_Retmode ); \
									LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] ); \
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 101 (k=%d) CHECK_PICK_from_PM_MG = Res=%d,PM=%d,R_Next_Move=%d\n" , ##ZZZ## + 1 , k , Result , i , R_Next_Move ); \
									if ( Result >= 0 ) { \
										SCH_Buff_Time = _SCH_PRM_GET_Getting_Priority( i ); \
										if ( SCH_Buff_Time < SCH_Small_Time ) { \
											Enter_Side	   = k; \
											SCH_Mode       = 0; \
											SCH_No         = i; \
											SCH_Small_Time = SCH_Buff_Time; \
											SCH_Type       = R_Next_Move; \
											SCH_Order      = R_Path_Order; \
											SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
											SCH_W_Prcs	   = R_Wait_Process; \
											SCH_EndMode	   = R_EndMode; \
											SCH_NextFinger = R_NextFinger; \
											if ( SCH_W_Prcs ) { \
												Waiting_Motion = SCH_No; \
											} \
											else { \
												Waiting_Motion = -1; \
											} \
											SCH_SWITCH     = TRUE; \
											SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
											SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
										} \
										else if ( ( SCH_Buff_Time == SCH_Small_Time ) && ( SCH_Mode == 0 ) ) { \
											if ( !SCH_PM_IS_PLACING( SCH_No ) && SCH_PM_IS_PLACING( i ) ) { /* 2018.10.20 */ \
												Enter_Side	   = k; \
												SCH_Mode       = 0; \
												SCH_No         = i; \
												SCH_Small_Time = SCH_Buff_Time; \
												SCH_Type       = R_Next_Move; \
												SCH_Order      = R_Path_Order; \
												SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
												SCH_W_Prcs	   = R_Wait_Process; \
												SCH_EndMode	   = R_EndMode; \
												SCH_NextFinger = R_NextFinger; \
												if ( SCH_W_Prcs ) { \
													Waiting_Motion = SCH_No; \
												} \
												else { \
													Waiting_Motion = -1; \
												} \
												SCH_SWITCH     = TRUE; \
												SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
												SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
											} \
											else if ( SCH_PM_IS_PLACING( SCH_No ) == SCH_PM_IS_PLACING( i ) ) { /* 2018.10.20 */ \
												if ( SCH_R_Time > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ) ) { \
													Enter_Side	   = k; \
													SCH_Mode       = 0; \
													SCH_No         = i; \
													SCH_Small_Time = SCH_Buff_Time; \
													SCH_Type       = R_Next_Move; \
													SCH_Order      = R_Path_Order; \
													SCH_R_Time	   = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , i ); \
													SCH_W_Prcs	   = R_Wait_Process; \
													SCH_EndMode	   = R_EndMode; \
													SCH_NextFinger = R_NextFinger; \
													if ( SCH_W_Prcs ) { \
														Waiting_Motion = SCH_No; \
													} \
													else { \
														Waiting_Motion = -1; \
													} \
													SCH_SWITCH     = TRUE; \
													SCH_Chamber    = R_Next_Chamber; /* 2006.03.27 */ \
													SCH_Halfpick   = R_Halfpick; /* 2007.11.09 */ \
												} \
											} \
										} \
									} \
								} \
							} \
							if ( ( SCH_Mode < 0 ) && ( R_Retmode ) ) SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( ##ZZZ## , 0 ); \
						} \
					} \
					else { \
						SCHEDULING_CHECK_for_Check_PICK_from_PM_MG( ##ZZZ## , 0 ); \
					} \
				} \
			} \
			if ( Enter_Side	== -1 ) { \
				SCH_Mode = -1; \
				if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) != 0 ) { /* 2007.02.01 */ \
					SCHEDULER_Set_TM_Pick_BM_Signal( ##ZZZ## , 0 ); \
					break; \
				} \
				if ( ( ps == PM1 ) && ( pe == ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { \
					break; \
				} \
				else { \
					if ( SCH_PR_RUN ) break; \
				} \
			} \
			else { \
				\
				\
				if ( SCHEDULER_PROCESSING_STATUS_DIFFERENT( ##ZZZ## , FALSE ) ) { /* 2019.02.08 */ \
					SCH_Mode = -1; /* 2019.02.08 */ \
					break; /* 2019.02.08 */ \
				} /* 2019.02.08 */ \
				\
				\
				if ( Enter_Side	!= CHECKING_SIDE ) { \
					SCH_Mode = -1; \
					break; \
				} \
				else { \
					if ( SCHEDULER_Get_TM_Pick_BM_Signal( ##ZZZ## ) != 0 ) { /* 2007.02.01 */ \
						SCHEDULER_Set_TM_Pick_BM_Signal( ##ZZZ## , 0 ); \
						break; \
					} \
					/* _SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , -1 ); */ /* 2006.07.21 */ \
					if ( ( SCH_Mode != 10 ) && ( SCH_Mode != 20 ) && ( SCH_Mode != 21 ) && ( SCH_Mode != 22 ) ) break; \
				} \
			} \
		} \
		if ( SCH_Mode != -1 ) { /* 2006.07.21 */ \
			_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , -1 ); \
			RUN_NOT_SELECT_COUNT[##ZZZ##] = 0; \
		} \
		else { \
			RUN_NOT_SELECT_COUNT[##ZZZ##]++; \
		} \
		switch ( SCH_Mode ) { \
			case -1 : \
						break; \
\
			case 0 : /* Pick From PM */ \
\
				Function_Result = SCHEDULER_RUN_PICK_FROM_PM( ##ZZZ## , CHECKING_SIDE , SCH_W_Prcs , SCH_No , SCH_Type , SCH_NextFinger , SCH_Order , SCH_Chamber , SCH_Switchable , SCH_Halfpick , SCH_EndMode , SCH_SWITCH , &Skip_Pick_PM1 , &Skip_Pick_PM2 , &Run_Doing ); \
\
				if ( Run_Doing ) { /* 2018.10.20 */ \
					if ( SCH_PM_IS_PICKING( SCH_No ) ) { \
						MULTI_ALL_PM_LAST_PICK[##ZZZ##] = SCH_No; \
					} \
					else { \
						MULTI_ALL_PM_LAST_PICK[##ZZZ##] = 0; \
					} \
				} \
 \
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 11 : /* Pick From BM */ \
\
				Function_Result = SCHEDULER_RUN_PICK_FROM_BM( ##ZZZ## , CHECKING_SIDE , SCH_SchPt , SCH_Chamber , SCH_Slot , SCH_JumpMode , SCH_Order , SCH_No , &Skip_Pick_BM1 , &Skip_Pick_BM2 , &Run_Doing ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 12 : /* Place to BM/PM */ \
\
				Function_Result = SCHEDULER_RUN_PLACE_TO_BMPM( ##ZZZ## , CHECKING_SIDE , SCH_No , SCH_Chamber , SCH_Slot , SCH_Type , SCH_RETMODE , SCH_JumpMode , SCH_Order , SCH_SWITCH , &Run_Doing ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 10 : \
\
				Function_Result = SCHEDULER_RUN_MOVE_FOR_GET( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , SCH_NextFinger , SCH_No ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 20 : \
\
				Function_Result = SCHEDULER_RUN_MOVE_FOR_BM_GET( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , SCH_Slot ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 21 : \
\
				Function_Result = SCHEDULER_RUN_MOVE_FOR_BM_PUT( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , SCH_No , SCH_Slot ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			case 22 : \
\
				Function_Result = SCHEDULER_RUN_MOVE_FOR_PUT( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , SCH_No ); \
\
				if ( Function_Result == -1 ) return 0; \
				if ( Function_Result ==  1 ) return 1; \
				if ( Function_Result ==  2 ) continue; \
\
				break; \
\
			default : \
					break; \
		} \
		break; \
	} \
	if ( Run_Doing ) return -3; \
	return 1; \
}


MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 1 , 2 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 2 , 3 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 3 , 4 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 4 , 5 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 5 , 6 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 6 , 7 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_0( 7 , 8 );
#endif
#endif












