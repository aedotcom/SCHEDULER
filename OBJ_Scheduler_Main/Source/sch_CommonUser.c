//===========================================================================================================
#include "INF_default.h"
//===========================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_errorcode.h"
#include "INF_User_Parameter.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_Prm.h"
//===========================================================================================================
#include "System_tag.h"
#include "DLL_Interface.h"
#include "User_Parameter.h"
//===========================================================================================================


void USER_RECIPE_TAG_START( int type , int side , int id ); // 2015.06.23
void USER_RECIPE_TAG_END( int type , int side , int id ); // 2015.06.23



#ifndef _SCH_MIF

	#include "sch_A0_commonuser.h"
	#include "sch_A4_commonuser.h"
	#include "sch_A6_commonuser.h"

	//------------------------------------------------------------------------------------------
	enum {
		RUN_ALGORITHM_0_DEFAULT = 0 ,
		RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 = 4 ,
		RUN_ALGORITHM_6_INLIGN_PM2_V1 = 6 ,
	};
	//------------------------------------------------------------------------------------------
	//===========================================================================================================
	int ALG_DATA_RUN = 0;
	//===========================================================================================================
#endif
//===========================================================================================================
//===========================================================================================================
// LEVEL 1
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_CONFIG_INITIALIZE( int runalg , int subalg , int guialg , int maxpm ) {
#ifndef _SCH_MIF
	//===============================================================================
	ALG_DATA_RUN = runalg;
	//===============================================================================
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_CONFIG_INITIALIZE_STYLE_0( runalg , subalg , guialg , maxpm ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_CONFIG_INITIALIZE_STYLE_4( runalg , subalg , guialg , maxpm ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_CONFIG_INITIALIZE_STYLE_6( runalg , subalg , guialg , maxpm ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_CONFIG_INITIALIZE( runalg , subalg , guialg , maxpm ) );
	//============================================================================================================
}
//===========================================================================================================
/*
// 2017.01.05
void _SCH_COMMON_REVISION_HISTORY( int mode , char *data , int count ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_REVISION_HISTORY_STYLE_0( mode , data , count );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		 _SCH_COMMON_REVISION_HISTORY_STYLE_4( mode , data , count );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_REVISION_HISTORY_STYLE_6( mode , data , count );
		break;
	}
#endif
	//============================================================================================================
	( _SCH_INF_DLL_CONTROL_REVISION_HISTORY( mode , data , count ) );
	//============================================================================================================
}
*/
//
void _SCH_COMMON_REVISION_HISTORY( int mode , char *data , int count ) { // 2017.01.05
	char Buffer[128];
	//
	strcpy( data , "" );
	//
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		strcpy( Buffer , "" );		_SCH_COMMON_REVISION_HISTORY_STYLE_0( mode , Buffer , 63 );		if ( Buffer[0] != 0 ) strcat( data , Buffer );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		strcpy( Buffer , "" );		 _SCH_COMMON_REVISION_HISTORY_STYLE_4( mode , Buffer , 63 );	if ( Buffer[0] != 0 ) strcat( data , Buffer );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		strcpy( Buffer , "" );		_SCH_COMMON_REVISION_HISTORY_STYLE_6( mode , Buffer , 63 );		if ( Buffer[0] != 0 ) strcat( data , Buffer );
		break;
	}
#endif
	//============================================================================================================
	strcpy( Buffer , "" );		
	//============================================================================================================
	( _SCH_INF_DLL_CONTROL_REVISION_HISTORY( mode , Buffer , 127 ) );
	//============================================================================================================
	if ( Buffer[0] != 0 ) {
		if ( data[0] != 0 ) strcat( data , "|" );
		strcat( data , Buffer );
	}
	//============================================================================================================
}

//===========================================================================================================
void _SCH_COMMON_INITIALIZE_PART( int apmode , int side ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_INITIALIZE_PART_STYLE_0( apmode , side );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_INITIALIZE_PART_STYLE_4( apmode , side );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_INITIALIZE_PART_STYLE_6( apmode , side );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_RUN_INIT( apmode , side );
	//============================================================================================================
}
//===========================================================================================================
void _DEF_SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) { // 2010.12.07
	int i , j , k , tmcmodify;
	//------------------------------------------------------------------------------
	if ( *TMATM <= 0 ) tmcmodify = TRUE; // 2010.12.07
	else               tmcmodify = FALSE;
	//------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) ) { // 2007.10.03
				k = _SCH_PRM_GET_MODULE_GROUP( i );
				//
				if ( k >= 0 ) { // 2014.09.24
					//
					if ( tmcmodify ) { // 2010.12.07
						if ( (*TMATM) < k ) *TMATM = k;
					}
					//
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
	//------------------------------------------------------------------------------
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , -1 ) ) break;
		}
	}
	if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) continue;
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PICK , -1 ) ) continue;
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
			//
			if ( tmcmodify ) { // 2010.12.07
				k = _SCH_PRM_GET_MODULE_GROUP( j );
				if ( (*TMATM) < k ) *TMATM = k;
			}
		}
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) break;
		}
	}
	if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) continue;
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( j , G_PLACE , -1 ) ) continue;
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
			//
			if ( tmcmodify ) { // 2010.12.07
				k = _SCH_PRM_GET_MODULE_GROUP( j );
				if ( (*TMATM) < k ) *TMATM = k;
			}
		}
	}
	//------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) < 2 ) ) { // 2007.11.21
				for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
					if ( _SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) == 1 ) {
						//
						if ( tmcmodify ) { // 2010.12.07
							if ( (*TMATM) < j ) *TMATM = j;
						}
						//
					}
				}
			}
			else { // 2012.02.09
				for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) == 1 ) || ( _SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) == 1 ) ) {
						//
						if ( tmcmodify ) {
							if ( (*TMATM) < j ) *TMATM = j;
						}
						//
					}
				}
			}
		}
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) == 1 ) {
				if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) == 1 ) {
					//
					if ( tmcmodify ) { // 2010.12.07
						if ( (*TMATM) < j ) *TMATM = j;
					}
					//
				}
			}
		}
	}
	//------------------------------------------------------------------------------
	for ( i = 0 ; i <= (*TMATM) ; i++ ) {
		_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
	}
	//------------------------------------------------------------------------------
}

//===========================================================================================================
int _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	int Res;
#ifndef _SCH_MIF
	//
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_0( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );	if ( Res != ERROR_NONE ) return Res;
		Res = _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_4( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );	if ( Res != ERROR_NONE ) return Res;
		Res = _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_6( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );	if ( Res != ERROR_NONE ) return Res;
		Res = _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
		break;
	default :
		Res = _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
		if ( Res == -1 ) _DEF_SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
		break;
	}
	//
#else
	//
	Res = _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
	//
	if ( Res == -1 ) _DEF_SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
	//
#endif
	//
	if ( Res == -1 ) return ERROR_NONE;
	//
	return Res;
}
//===========================================================================================================
void _SCH_COMMON_RUN_PRE_BM_CONDITION( int side , int bmc , int bmmode ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_0( side , bmc , bmmode );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_4( side , bmc , bmmode );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_6( side , bmc , bmmode );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_RUN_PRE_BM_CONDITION( side , bmc , bmmode );
	//============================================================================================================
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 2
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
void _SCH_COMMON_RECIPE_DLL_INTERFACE( HINSTANCE dllpt , int unload ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_0( dllpt , unload );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_4( dllpt , unload );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_6( dllpt , unload );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_SET_INSTANCE( dllpt , unload );
	//============================================================================================================
}
//===========================================================================================================
BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG( int chg_ALG_DATA_RUN ) {
#ifndef _SCH_MIF
	switch( chg_ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_0( chg_ALG_DATA_RUN ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_4( chg_ALG_DATA_RUN ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_6( chg_ALG_DATA_RUN ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_READING_CONFIG_FILE_WHEN_CHANGE_ALG( chg_ALG_DATA_RUN );
	//============================================================================================================
}
//===========================================================================================================
BOOL _SCH_COMMON_PRE_PART_OPERATION( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_PRE_PART_OPERATION_STYLE_0( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_PRE_PART_OPERATION_STYLE_4( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_PRE_PART_OPERATION_STYLE_6( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_RUN_PRE_OPERATION( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring );
	//============================================================================================================
}
//===========================================================================================================
BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE( int side , int WaitParam , int option ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_0( side , WaitParam , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_4( side , WaitParam , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_6( side , WaitParam , option ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_RUN_MFIC_COMPLETE_MACHINE( side , WaitParam , option );
	//============================================================================================================
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 3
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
int  _SCH_COMMON_GUI_INTERFACE( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
#ifndef _SCH_MIF
	switch( alg ) {
	case RUN_ALGORITHM_0_DEFAULT :
		return _SCH_COMMON_GUI_INTERFACE_STYLE_0( style , alg , mode , data , Message , list , retdata );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		return _SCH_COMMON_GUI_INTERFACE_STYLE_4( style , alg , mode , data , Message , list , retdata );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		return _SCH_COMMON_GUI_INTERFACE_STYLE_6( style , alg , mode , data , Message , list , retdata );
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_GUI_INTERFACE( style , alg , mode , data , Message , list , retdata ) );
	//============================================================================================================
}
//===============================================================================
int _SCH_COMMON_EVENT_PRE_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
#ifndef _SCH_MIF
	int Res;
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_0( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != 1 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_4( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != 1 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_6( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != 1 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_EVENT_PRE_ANALYSIS( FullMessage , RunMessage , ElseMessage , ErrorMessage );
	//============================================================================================================
}
//===============================================================================
int _SCH_COMMON_EVENT_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
#ifndef _SCH_MIF
	int Res;
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_EVENT_ANALYSIS_STYLE_0( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != -1 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_EVENT_ANALYSIS_STYLE_4( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != -1 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_EVENT_ANALYSIS_STYLE_6( FullMessage , RunMessage , ElseMessage , ErrorMessage );	if ( Res != -1 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_EVENT_ANALYSIS( FullMessage , RunMessage , ElseMessage , ErrorMessage );
	//============================================================================================================
}
//===============================================================================
void _SCH_COMMON_SYSTEM_DATA_LOG( char *filename , int side ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_SYSTEM_DATA_LOG_STYLE_0( filename , side );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_SYSTEM_DATA_LOG_STYLE_4( filename , side );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_SYSTEM_DATA_LOG_STYLE_6( filename , side );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_SYSTEM_LOG( filename , side );
	//============================================================================================================
}
//===============================================================================
void _SCH_COMMON_SYSTEM_CONTROL_FAIL( BOOL errorfinish , int side , int data1 , int data2 , int data3 ) { // 2017.07.26
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_SYSTEM_FAIL( errorfinish , side , data1 , data2 , data3 );
	//============================================================================================================
}
//===============================================================================
int  _SCH_COMMON_TRANSFER_OPTION( int mode , int data ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		return _SCH_COMMON_TRANSFER_OPTION_STYLE_0( mode , data );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		return _SCH_COMMON_TRANSFER_OPTION_STYLE_4( mode , data );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		return _SCH_COMMON_TRANSFER_OPTION_STYLE_6( mode , data );
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_TRANSFER_OPTION( mode , data );
	//============================================================================================================
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 4
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_FINISHED_CHECK( int side ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_FINISHED_CHECK_STYLE_0( side ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_FINISHED_CHECK_STYLE_4( side ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_FINISHED_CHECK_STYLE_6( side ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_FINISHED_CHECK( side ) );
	//============================================================================================================
}
//===========================================================================================================
// 0 : Recipe Read / Slot Overlapped
// 1 : Map Read / Map info check and Unuse DB reset
// 2 : Map Read / Slot Overlapped or No Wafer
// 3 : Map First
// 4 : Map Switch
// 5 : Map End / Verification
// 6 : CM Module Check
// 7 : CM MaxSlot Check
// 8 : All Disable Check
// 9 : Handling Side Check
// 10 : TM Check
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( int mode , int side , int ch1 , int ch2 , int option ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_0( mode , side , ch1 , ch2 , option ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_4( mode , side , ch1 , ch2 , option ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_6( mode , side , ch1 , ch2 , option ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_CASSETTE_CHECK_INVALID_INFO( mode , side , ch1 , ch2 , option ) );
	//============================================================================================================
}
//===========================================================================================================
BOOL _SCH_COMMON_GET_PROCESS_FILENAME( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_0( logpath , lotdir , prcs , ch , side , waferindex1 , waferindex2 , waferindex3 , filename , dummy , mode , notuse , highappend ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_4( logpath , lotdir , prcs , ch , side , waferindex1 , waferindex2 , waferindex3 , filename , dummy , mode , notuse , highappend ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_6( logpath , lotdir , prcs , ch , side , waferindex1 , waferindex2 , waferindex3 , filename , dummy , mode , notuse , highappend ) ) return TRUE;
		break;
	}	
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME( logpath , lotdir , prcs , ch , side , waferindex1 , waferindex2 , waferindex3 , filename , dummy , mode , notuse , highappend ) );
	//============================================================================================================
}
//===========================================================================================================
BOOL _SCH_COMMON_GET_PROCESS_FILENAME2( char *logpath , char *lotdir , int prcs , int ch , int side , int pt , int *waferindex1 , int *waferindex2 , int *waferindex3 , char *filename , char *foldername , BOOL *dummy , int *mode , BOOL *notuse , BOOL *highappend , BOOL *filenotuse ) {
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME2( logpath , lotdir , prcs , ch , side , pt , waferindex1 , waferindex2 , waferindex3 , filename , foldername , dummy , mode , notuse , highappend , filenotuse ) );
	//============================================================================================================
}
//===========================================================================================================
void _SCH_COMMON_GET_PROCESS_INDEX_DATA( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
	//
	*nomore = FALSE;
	*rmode = mode;
	*xoffset = 0;
	//
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_0( mode , x , rmode , nomore , xoffset );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_4( mode , x , rmode , nomore , xoffset );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_6( mode , x , rmode , nomore , xoffset );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_GET_PROCESS_INDEX_DATA( mode , x , rmode , nomore , xoffset );
	//============================================================================================================
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_SIDE_SUPPLY( int side , BOOL EndCheck , int Supply_Style , int *Result ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_SIDE_SUPPLY_STYLE_0( side , EndCheck , Supply_Style , Result ) ) return TRUE;
		break;

	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_SIDE_SUPPLY_STYLE_4( side , EndCheck , Supply_Style , Result ) ) return TRUE;
		break;

	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_SIDE_SUPPLY_STYLE_6( side , EndCheck , Supply_Style , Result ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_SIDE_SUPPLY( side , EndCheck , Supply_Style , Result ) );
	//============================================================================================================
}

//===============================================================================
BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING() {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_0() ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_4() ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_6() ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_FM_ARM_APLUSB_SWAPPING() );
	//============================================================================================================
}

//===============================================================================

BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_0() ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_4() ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_6() ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_DISABLE_CONTROL() );
	//============================================================================================================
}


//===============================================================================
BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL( int side , int pt , int ch ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_0( side , pt , ch ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_4( side , pt , ch ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_6( side , pt , ch ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_SKIP_CONTROL( side , pt , ch ) );
	//============================================================================================================
}


//===============================================================================
int _SCH_COMMON_PM_2MODULE_SAME_BODY() { // 2007.10.07
#ifndef _SCH_MIF
	int Res;
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0();	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_4();	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_6();	if ( Res != 0 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_PM_2MODULE_SAME_BODY() );
	//============================================================================================================
}

//===============================================================================

int _SCH_COMMON_BM_2MODULE_SAME_BODY() {
#ifndef _SCH_MIF
	int Res;
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_0();	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_4();	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_6();	if ( Res != 0 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_BM_2MODULE_SAME_BODY() );
	//============================================================================================================
}

//===============================================================================
void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING( int side , int bmc , char *appstr ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_0( side , bmc , appstr );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_4( side , bmc , appstr );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_6( side , bmc , appstr );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_BM_2MODULE_END_APPEND_STRING( side , bmc , appstr );
	//============================================================================================================
}

//===============================================================================

int  _SCH_COMMON_GET_METHOD_ORDER( int side , int pt ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		return _SCH_COMMON_GET_METHOD_ORDER_STYLE_0( side , pt );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		return _SCH_COMMON_GET_METHOD_ORDER_STYLE_4( side , pt );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		return _SCH_COMMON_GET_METHOD_ORDER_STYLE_6( side , pt );
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_GET_METHOD_ORDER( side , pt ) );
	//============================================================================================================
}

//===============================================================================
int  _SCH_COMMON_CHECK_PICK_FROM_FM( int side , int pt , int subchk ) {
#ifndef _SCH_MIF
	int Res;
#endif
	//
	// 2016.12.05
	//
	if ( _SCH_SYSTEM_NO_PICK_FROM_CM_GET( side ) ) return -9;
	//
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_0( side , pt , subchk );	if ( Res != 1 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_4( side , pt , subchk );	if ( Res != 1 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_6( side , pt , subchk );	if ( Res != 1 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_CHECK_PICK_FROM_FM( side , pt , subchk ) );
	//============================================================================================================
}

//===============================================================================
BOOL _SCH_COMMON_CHECK_END_PART( int side , int mode ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_CHECK_END_PART_STYLE_0( side , mode ) ) return FALSE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_CHECK_END_PART_STYLE_4( side , mode ) ) return FALSE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_CHECK_END_PART_STYLE_6( side , mode ) ) return FALSE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_CHECK_END_PART( side , mode ) );
	//============================================================================================================
}

//===============================================================================
int _SCH_COMMON_FAIL_SCENARIO_OPERATION( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	// Return Value
	// 1 : Dis+prset
	// 2 : Dis
	// 3 : prset
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		return _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_0( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		return _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_4( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		return _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_6( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore );
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_FAIL_SCENARIO_OPERATION( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore ) );
	//============================================================================================================
}


BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( int side ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_0( side ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_4( side ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_6( side ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_EXTEND_FM_ARM_CROSS_FIXED( side ) );
	//============================================================================================================
}



//===============================================================================
int _SCH_COMMON_CHECK_WAITING_MODE( int side , int *mode , int lc_precheck , int option ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_CHECK_WAITING_MODE_STYLE_0( side , mode , lc_precheck , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_CHECK_WAITING_MODE_STYLE_4( side , mode , lc_precheck , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_CHECK_WAITING_MODE_STYLE_6( side , mode , lc_precheck , option ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_CHECK_WAITING_MODE( side , mode , lc_precheck , option ) );
	//============================================================================================================
}

//===============================================================================

BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module ,
						int TuneData_Order ,
						int TuneData_Step ,
						char *TuneData_Name ,
						char *TuneData_Data ,
						int TuneData_Index ,
						char *AppendStr ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_0( mode , ch , curorder , pjindex , pjno ,
						TuneData_Module ,
						TuneData_Order ,
						TuneData_Step ,
						TuneData_Name ,
						TuneData_Data ,
						TuneData_Index ,
						AppendStr ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_4( mode , ch , curorder , pjindex , pjno ,
						TuneData_Module ,
						TuneData_Order ,
						TuneData_Step ,
						TuneData_Name ,
						TuneData_Data ,
						TuneData_Index ,
						AppendStr ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_6( mode , ch , curorder , pjindex , pjno ,
						TuneData_Module ,
						TuneData_Order ,
						TuneData_Step ,
						TuneData_Name ,
						TuneData_Data ,
						TuneData_Index ,
						AppendStr ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_TUNING_GET_MORE_APPEND_DATA( mode , ch , curorder , pjindex , pjno , TuneData_Module , TuneData_Order , TuneData_Step , TuneData_Name , TuneData_Data , TuneData_Index , AppendStr ) );
	//============================================================================================================
}

//===============================================================================

int _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( int mode , int side , int ch , int slot , int option ) {
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_0( mode , side , ch , slot , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_4( mode , side , ch , slot , option ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_6( mode , side , ch , slot , option ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_METHOD_CHECK_SKIP_WHEN_START( mode , side , ch , slot , option ) );
	//============================================================================================================
}

//===============================================================================
//===============================================================================
//===============================================================================



//void SCHEDULING_DATA_STRUCTURE_LOG( char *filename , char *dispdata , int lotlog , int logpos , int sd );


//void SCHEDULER_USER_LOG( int side , char *Name , int Data ) { // 2016.03.28
//	char Buffer[256];
//	//==========================================================================================================================//
//	sprintf( Buffer , "%s-%d" , Name , Data );
//	//==========================================================================================================================//
//	SCHEDULING_DATA_STRUCTURE_LOG( "" , Buffer , -1 , 0 , 0 );
//	//==========================================================================================================================
//}

//===============================================================================
//===============================================================================


void _SCH_COMMON_ANIMATION_UPDATE_TM( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ) { // 2010.01.29
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		_SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_0( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		_SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_4( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		_SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_6( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
		break;
	}
#endif
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_ANIMATION_UPDATE_TM( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
	//============================================================================================================
}

//===========================================================================================================

BOOL _SCH_COMMON_USER_DATABASE_REMAPPING( int Side , int Mode , BOOL precheck , int option ) { // 2010.11.09
	BOOL result;

	USER_RECIPE_TAG_START( 2 , Side , Mode ); // 2015.06.23

//=========================================================================================================================================================
//SCHEDULER_USER_LOG( Side , "[S]USER_DATABASE_REMAPPING" , Mode ); // 2016.03.28
//=========================================================================================================================================================


#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_0( Side , Mode , precheck , option ) ) {
			//
			USER_RECIPE_TAG_END( 2 , Side , Mode ); // 2015.06.23
			//
			return FALSE;
		}
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_4( Side , Mode , precheck , option ) ) {
			//
			USER_RECIPE_TAG_END( 2 , Side , Mode ); // 2015.06.23
			//
			return FALSE;
		}
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_6( Side , Mode , precheck , option ) ) {
			//
			USER_RECIPE_TAG_END( 2 , Side , Mode ); // 2015.06.23
			//
			return FALSE;
		}
		break;
	}
#endif
	//
	//============================================================================================================
	result = _SCH_INF_DLL_CONTROL_USER_DATABASE_REMAPPING( Side , Mode , precheck , option );
	//============================================================================================================

//=========================================================================================================================================================
//SCHEDULER_USER_LOG( Side , "[E]USER_DATABASE_REMAPPING" , Mode ); // 2016.03.28
//=========================================================================================================================================================

	USER_RECIPE_TAG_END( 2 , Side , Mode ); // 2015.06.23

	return result;


}

//===========================================================================================================

BOOL _SCH_COMMON_ANIMATION_SOURCE_UPDATE( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ) { // 2010.11.23
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_0( when , mode , tmside , station , slot , srccass , srcslot ) ) return TRUE;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_4( when , mode , tmside , station , slot , srccass , srcslot ) ) return TRUE;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_6( when , mode , tmside , station , slot , srccass , srcslot ) ) return TRUE;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_ANIMATION_SOURCE_UPDATE( when , mode , tmside , station , slot , srccass , srcslot );
	//============================================================================================================
}

//===========================================================================================================

int _SCH_COMMON_MTLOUT_DATABASE_MOVE( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ) { // 2011.05.13
#ifndef _SCH_MIF
	int Res;
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		Res = _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_0( mode , tside , tpoint , sside , spoint , tmsupply );	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		Res = _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_4( mode , tside , tpoint , sside , spoint , tmsupply );	if ( Res != 0 ) return Res;
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		Res = _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_6( mode , tside , tpoint , sside , spoint , tmsupply );	if ( Res != 0 ) return Res;
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_MTLOUT_DATABASE_MOVE( mode , tside , tpoint , sside , spoint , tmsupply );
	//============================================================================================================
}

//===========================================================================================================

	// mode
	//
	// 1 : JOB EXTEND LOG
	// 2 : ABORT RETRY
	// 3 : MR DATA LOG


int _SCH_COMMON_USE_EXTEND_OPTION( int mode , int sub ) { // 2011.11.18
	//
	switch( _i_SCH_PRM_GET_USE_EXTEND_OPTION() ) {
	case 1 : // off
		return FALSE;
		break;
	case 2 : // on
		return TRUE;
		break;
	}
	//
	//	return FALSE; // T-2011.11.24
	//
#ifndef _SCH_MIF
	switch( ALG_DATA_RUN ) {
	case RUN_ALGORITHM_0_DEFAULT :
		return _SCH_COMMON_USE_EXTEND_OPTION_STYLE_0( mode , sub );
		break;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 :
		return _SCH_COMMON_USE_EXTEND_OPTION_STYLE_4( mode , sub );
		break;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1 :
		return _SCH_COMMON_USE_EXTEND_OPTION_STYLE_6( mode , sub );
		break;
	}
#endif
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_USE_EXTEND_OPTION( mode , sub );
	//============================================================================================================
}

//===========================================================================================================
//void Scheduler_System_Log( int side , char *logdata , int infolog , int sd );

int _SCH_COMMON_USER_RECIPE_CHECK( int mode , int side , int data1 , int data2 , int data3 , int *Res ) { // 2012.01.12

	//==========================================================================================================================//
//	Scheduler_System_Log( side , "RECIPE" , 0 , 0 ); // 2015.04.28
	//==========================================================================================================================

	int result;

	USER_RECIPE_TAG_START( 1 , side , mode ); // 2015.06.23

//=========================================================================================================================================================
//SCHEDULER_USER_LOG( side , "[S]USER_RECIPE_CHECK" , mode ); // 2016.03.28
//=========================================================================================================================================================
	//============================================================================================================
	result = _SCH_INF_DLL_CONTROL_USER_RECIPE_CHECK( mode , side , data1 , data2 , data3 , Res );
	//============================================================================================================

//=========================================================================================================================================================
//SCHEDULER_USER_LOG( side , "[E]USER_RECIPE_CHECK" , mode ); // 2016.03.28
//=========================================================================================================================================================
	USER_RECIPE_TAG_END( 1 , side , mode ); // 2015.06.23

	return result;
}

//===========================================================================================================

int _SCH_COMMON_ANIMATION_RESULT_UPDATE( int side , int pt , int data , int sub ) { // 2012.03.15
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_ANIMATION_RESULT_UPDATE( side , pt , data , sub );
	//============================================================================================================
}

//===========================================================================================================

int _SCH_COMMON_MESSAGE_CONTROL_FOR_USER( BOOL postmode , int id , int i1 , int i2 , int i3 , int i4 , int i5 , int i6 , int i7 , int i8 , int i9 , int i10 , double d1 , double d2 , double d3 , char *s1 , char *s2 , char *s3 ) {
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_MESSAGE_CONTROL_FOR_USER( postmode , id , i1 , i2 , i3 , i4 , i5 , i6 , i7 , i8 , i9 , i10 , d1 , d2 , d3 , s1 , s2 , s3 ) );
	//============================================================================================================
}

//===========================================================================================================

BOOL _SCH_COMMON_GET_PT_AT_PICK_FROM_CM( int side , int *pt , BOOL fmmode , int sub ) { // 2014.06.16
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_GET_PT_AT_PICK_FROM_CM( side , pt , fmmode , sub );
	//============================================================================================================
}


//===========================================================================================================

void _SCH_COMMON_HANDOFF_ACTION( int cm , int mode , int sub ) { // 2014.06.27
	//============================================================================================================
	_SCH_INF_DLL_CONTROL_HANDOFF_ACTION( cm , mode , sub );
	//============================================================================================================
}

//===============================================================================

int _SCH_COMMON_USER_FLOW_NOTIFICATION( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ) { // 2015.05.29
//=========================================================================================================================================================
//SCHEDULER_USER_LOG( side , "[S]USER_FLOW_NOTIFICATION" , mode ); // 2016.03.28
//=========================================================================================================================================================
	//============================================================================================================
	return ( _SCH_INF_DLL_CONTROL_USER_FLOW_NOTIFICATION( mode , usertag , side , pt , opt1 , opt2 , opt3 , opt4 , opt5 ) );
	//============================================================================================================
}

//===============================================================================

BOOL _SCH_COMMON_GET_UPLOAD_MESSAGE( int mode , int ch , char *message ) { // 2017.01.03
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_GET_UPLOAD_MESSAGE( mode , ch , message );
	//============================================================================================================
}


//===============================================================================

BOOL _SCH_COMMON_GET_DUMMY_METHOD_INDEX( int ch , int mode , int wh , int side , int pt , int *GetIndex ) { // 2017.09.11
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_GET_DUMMY_METHOD_INDEX( ch , mode , wh , side , pt , GetIndex  );
	//============================================================================================================
}

//===============================================================================

BOOL _SCH_COMMON_FA_SEND_MESSAGE( int mode , int side , int pt , char *orgmsg , char *newmessageBuff , int newmessageBuffSize ) { // 2018.06.14
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_FA_SEND_MESSAGE( mode , side , pt , orgmsg , newmessageBuff , newmessageBuffSize );
	//============================================================================================================
}

//===============================================================================

BOOL _SCH_COMMON_FA_ORDER_MESSAGE( int mode , int side , int pt ) { // 2018.06.29
	//============================================================================================================
	return _SCH_INF_DLL_CONTROL_FA_ORDER_MESSAGE( mode , side , pt );
	//============================================================================================================
}

