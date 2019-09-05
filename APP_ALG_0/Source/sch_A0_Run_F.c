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
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_commonuser.h"
#include "INF_sch_sub.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_default.h"
#include "sch_A0_F.h"
#include "sch_A0_param.h"
#include "sch_A0_init.h"
#include "sch_A0_sub.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub_F_dbl1.h"
//================================================================================
//=================================================================================================================================
int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_0( int CHECKING_SIDE , int mode ) {
	if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
			if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) {
				//
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_SWITCH_STYLE_0( CHECKING_SIDE , mode );
				//
			}
			else {
#ifdef PM_CHAMBER_30
				//
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_DOUBLE_STYLE_0( CHECKING_SIDE , mode );
				//
#else
				_IO_CIM_PRINTF( "This Algorithm is not Support for Scheduler 12/60PM type(Use 30 PM Type)[101]\n" );
				return SYS_ABORTED;
#endif
			}
		}
		else {
			if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) {
				//
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_DEFAULT_STYLE_0( CHECKING_SIDE , mode );
				//
			}
			else {
#ifdef PM_CHAMBER_30
				//
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_DOUBLE_STYLE_0( CHECKING_SIDE , mode );
				//
#else
				_IO_CIM_PRINTF( "This Algorithm is not Support for Scheduler 12/60PM type(Use 30 PM Type)[101]\n" );
				return SYS_ABORTED;
#endif
			}
		}
	}
	return -1;
}
//=================================================================================================================================
int USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_0( int CHECKING_SIDE ) {


/*

2018.07.03 AL ERROR Check

		int Res;
		int Wafer = 1;
		int Side = CHECKING_SIDE;
		int Pointer = 0;


		if ( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , -1 , Wafer , Wafer , Side , Pointer , -1 , -1 , -1 , -1 , -1 , 0 ) == -1 ) return FALSE;
		_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , Wafer , Wafer , Side , Pointer , -1 , -1 , -1 , -1 , -1 , 0 , 0 );




		//
		Res = _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , F_AL , Wafer , 1 , Side , Pointer , -1 , -1 , -1 , -1 , -1 , 0 );
		//





		printf( "================[%d]===========================\n" , Res );





		return 0;







*/













	if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
			if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) {
				//
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_SWITCH_STYLE_0( CHECKING_SIDE );
				//
			}
			else {
#ifdef PM_CHAMBER_30
				//
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_DOUBLE_STYLE_0( CHECKING_SIDE );
				//
#else
				_IO_CIM_PRINTF( "This Algorithm is not Support for Scheduler 12/60PM type(Use 30 PM Type)[101]\n" );
				return SYS_ABORTED;
#endif
			}
		}
		else {
			if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() == 0 ) {
				//
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_DEFAULT_STYLE_0( CHECKING_SIDE );
				//
			}
			else {
#ifdef PM_CHAMBER_30
				//
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_DOUBLE_STYLE_0( CHECKING_SIDE );
				//
#else
				_IO_CIM_PRINTF( "This Algorithm is not Support for Scheduler 12/60PM type(Use 30 PM Type)[101]\n" );
				return SYS_ABORTED;
#endif
			}
		}
	}
	return 0;
}
//=================================================================================================================================




