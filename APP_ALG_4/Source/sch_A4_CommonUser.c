#include "INF_Scheduler.h"

#include "sch_A4_default.h"
#include "sch_A4_dll.h"
#include "sch_A4_init.h"
#include "sch_A4_log.h"
#include "sch_A4_event.h"
#include "sch_A4_sub.h"
#include "sch_A4_sdm.h"



//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 1
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_CONFIG_INITIALIZE_STYLE_4( int runalg , int subalg , int guialg , int maxpm ) {
	int i;
	_SCH_PRM_SET_DFIM_MAX_PM_COUNT( MAX_TM_CHAMBER_DEPTH );
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( !_SCH_PRM_GET_MODULE_MODE( PM1 + i ) ) {
			_SCH_PRM_SET_DFIM_MAX_PM_COUNT( i );
			break;
		}
		if ( !_SCH_PRM_GET_MODULE_MODE( BM1 + i ) ) {
			_SCH_PRM_SET_DFIM_MAX_PM_COUNT( i );
			break;
		}
		if ( !_SCH_PRM_GET_MODULE_MODE( TM + i ) ) {
			_SCH_PRM_SET_DFIM_MAX_PM_COUNT( i );
			break;
		}
	}
	return TRUE;
}
//===========================================================================================================
void _SCH_COMMON_REVISION_HISTORY_STYLE_4( int mode , char *data , int count ) {
	USER_REVISION_HISTORY_AL4_PART( mode , data , count );
}
//===========================================================================================================
void _SCH_COMMON_INITIALIZE_PART_STYLE_4( int apmode , int side ) {
	INIT_SCHEDULER_AL4_PART_DATA( apmode , side );
}
//===========================================================================================================
int _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_4( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	int i , j , k , l , RunWafer;
	//----------------------------------------------
//	if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() != 3 ) { // 2007.08.22 // 2009.09.10
	if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 0 ) { // 2007.08.22 // 2009.09.10
		//
		if ( _SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() == 0 ) { // 2018.03.21
			//
			if ( ( lc_target_wafer_count % 1000 ) == 1 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					if ( _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
						if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) {
							for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
								RunWafer = 0;
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									l = _SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
									if ( l > 0 ) {
										if ( _SCH_MODULE_GET_USE_ENABLE( l , FALSE , CHECKING_SIDE ) ) { // 2007.05.11
											if ( RunWafer == 0 ) {
												RunWafer = 1;
											}
											else {
												_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , i , j , k , 0 , "" , "" , "" );
												_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , l , MUF_00_NOTUSE );
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
	//----------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i - PM1 + BM1 , MUF_01_USE );
		}
	}
	//------------------------------------------------------------------------------
	*TMATM = _SCH_PRM_GET_DFIM_MAX_PM_COUNT() - 1;
	//------------------------------------------------------------------------------
	for ( i = 0 ; i <= (*TMATM) ; i++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 ) > MUF_00_NOTUSE ) {
			_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
		}
	}
	return ERROR_NONE;
}
//===========================================================================================================
void _SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_4( int side , int bmc , int bmmode ) {
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 2
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
void _SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_4( HINSTANCE dllpt , int unload ) {
	INIT_SCHEDULER_AL4_PART_SET_DLL_INSTANCE( dllpt , unload );
}
//===========================================================================================================
BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_4( int chg_ALG_DATA_RUN ) {
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) _SCH_PRM_SET_MODULE_GROUP( PM1 + i , i );
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) _SCH_PRM_SET_MODULE_GROUP( BM1 + i , i );
	return TRUE;
}
//===========================================================================================================
BOOL _SCH_COMMON_PRE_PART_OPERATION_STYLE_4( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	int i , j , k , pt , fr1 , fr2 , addr;
	char RunRecipe[513]; // 2007.01.04
	int  sdres;
	int  xinside0_TFM_Tag_for_A4[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.02.17
//###################################################################################################
#endif
//###################################################################################################

//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	if      ( mode == 0 ) {
		if ( _SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) < 2 ) {
			for ( i = PM1 ; i < ( PM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
				if ( dataflag[i] != 0 ) {
					sdres = SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( CHECKING_SIDE , i , &pt , dataflag2[i] , 1 , &k , TRUE ); // 2005.11.11 // 2005.12.21
					if ( sdres >= 0 ) {
						xinside0_TFM_Tag_for_A4[CHECKING_SIDE][i] = TRUE;
					}
					else {
						xinside0_TFM_Tag_for_A4[CHECKING_SIDE][i] = FALSE;
						//===========================================================================================================
						sdres = SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( CHECKING_SIDE , i , &pt , dataflag2[i] , 1 , &k , FALSE ); // 2005.12.21
						if ( sdres >= 0 ) {
							//====================================================================================================================================
							strncpy( RunRecipe , _SCH_SDM_Get_RECIPE( 2 , pt%100 , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , CHECKING_SIDE , i ) , SDM4_PTYPE_LOTFIRST ) , 64 ); // 2005.10.26
							//====================================================================================================================================
							_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
									 i ,
									 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 ,
									 RunRecipe ,
									 2 ,
									 1 , "F" , PROCESS_DEFAULT_MINTIME ,
									 0 , 102 );
						}
						//===========================================================================================================
					}
				}
				else {
					xinside0_TFM_Tag_for_A4[CHECKING_SIDE][i] = FALSE;
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
						if ( ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S1_TAG( i ) == -1 ) && ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S2_TAG( i ) == -1 ) ) { // 2006.01.14
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTAPPEND( CHECKING_SIDE , i );
						}
					}
				}
			}
			for ( i = PM1 ; i < ( PM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
				if ( xinside0_TFM_Tag_for_A4[CHECKING_SIDE][i] ) {
					//===========================================================================================================
					// 2005.11.11
					//===========================================================================================================
					//----------------------------------------------------------------
					_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i - PM1 + TM , &addr );
					if ( addr < 0 ) fr1 = SYS_SUCCESS;
					else            fr1 = _dREAD_FUNCTION( addr );
					//
					_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i , &addr );
					if ( addr < 0 ) fr2 = SYS_SUCCESS;
					else            fr2 = _dREAD_FUNCTION( addr );
					//----------------------------------------------------------------
					//
					if ( ( _SCH_MACRO_CHECK_PROCESSING( i ) > 0 ) || SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( i ) || ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( i , &j ) != 0 ) || ( fr1 == SYS_RUNNING ) || ( fr2 == SYS_RUNNING ) ) { // 2006.01.17
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Pre Process Wait at %s%cPROCESS_PRE_WAIT PM%d:::3-1\n" , _SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - PM1 + 1 );
						while( TRUE ) {
							if ( MANAGER_ABORT() ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Pre Process Wait Fail at %s%cPROCESS_PRE_WAIT_FAIL PM%d:::3-1\n" , _SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - PM1 + 1 );
//								return SYS_ABORTED; // 2010.08.23
								return FALSE; // 2010.08.23
							}
							//----------------------------------------------------------------
							_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i - PM1 + TM , &addr );
							if ( addr < 0 ) fr1 = SYS_SUCCESS;
							else            fr1 = _dREAD_FUNCTION( addr );
							//
							_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i , &addr );
							if ( addr < 0 ) fr2 = SYS_SUCCESS;
							else            fr2 = _dREAD_FUNCTION( addr );
							//----------------------------------------------------------------
							if ( ( _SCH_MACRO_CHECK_PROCESSING( i ) <= 0 ) && !SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( i ) && ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( i , &j ) == 0 ) && ( fr1 != SYS_RUNNING ) && ( fr2 != SYS_RUNNING ) ) { // 2006.01.17
								Sleep(250);
								//----------------------------------------------------------------
								_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i - PM1 + TM , &addr );
								if ( addr < 0 ) fr1 = SYS_SUCCESS;
								else            fr1 = _dREAD_FUNCTION( addr );
								//
								_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i , &addr );
								if ( addr < 0 ) fr2 = SYS_SUCCESS;
								else            fr2 = _dREAD_FUNCTION( addr );
								//----------------------------------------------------------------
								if ( ( _SCH_MACRO_CHECK_PROCESSING( i ) <= 0 ) && !SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( i ) && ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( i , &j ) == 0 ) && ( fr1 != SYS_RUNNING ) && ( fr2 != SYS_RUNNING ) ) { // 2006.01.17
									Sleep(100);
									//----------------------------------------------------------------
									_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i - PM1 + TM , &addr );
									if ( addr < 0 ) fr1 = SYS_SUCCESS;
									else            fr1 = _dREAD_FUNCTION( addr );
									//
									_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( i , &addr );
									if ( addr < 0 ) fr2 = SYS_SUCCESS;
									else            fr2 = _dREAD_FUNCTION( addr );
									//----------------------------------------------------------------
									if ( ( _SCH_MACRO_CHECK_PROCESSING( i ) <= 0 ) && !SCHEDULER_CONTROL_Get_SDM_4_RERUN_END_S3_TAG( i ) && ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( i , &j ) == 0 ) && ( fr1 != SYS_RUNNING ) && ( fr2 != SYS_RUNNING ) ) break; // 2006.01.17
								}
							}
							Sleep(10);
						}
					}
					//===========================================================================================================
					sdres = SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( CHECKING_SIDE , i , &pt , dataflag2[i] , 1 , &k , FALSE ); // 2005.12.21
					if ( sdres >= 0 ) {
						//====================================================================================================================================
						strncpy( RunRecipe , _SCH_SDM_Get_RECIPE( 2 , pt%100 , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , CHECKING_SIDE , i ) , SDM4_PTYPE_LOTFIRST ) , 64 ); // 2005.10.26
						//====================================================================================================================================
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
								 i ,
								 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 ,
								 RunRecipe ,
								 2 ,
								 1 , "F" , PROCESS_DEFAULT_MINTIME ,
								 0 , 103 );
					}
					else {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Pre Process Reject at %s(D?)[?]%cPROCESS_PRE_REJECT PM%d:?:?:3-2%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - PM1 + 1 , 9 );
					}
				}
			}
		}
	}
	else if ( mode == 1 ) {
		if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
			for ( i = BM1 ; i < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
				if ( dataflag[i] != 0 ) {
					//========================================================================================
					// 2005.09.09
					//========================================================================================
					if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() != 0 ) {
						if ( ( ( i - BM1 ) % 2 ) == 0 ) { // 2006.01.14
							if ( dataflag[i+1] != 0 ) {
								SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( i , TRUE );
								i++;
								//========================================================================================
								if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 3 ) {
									SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( i , TRUE );
									i++;
									SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( i , TRUE );
									i++;
								}
								//========================================================================================
								SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( i , TRUE ); // 2005.09.09
								//========================================================================================
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 ); // 2007.12.14
								//
								SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( i , TRUE ); // 2005.10.05
							}
							else {
								//========================================================================================
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 ); // 2007.12.14
								//========================================================================================
								i++;
							}
						}
					}
					else {
						if ( SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( i - BM1 + PM1 , &j ) == 0 ) { // 2006.01.17
							//========================================================================================
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 ); // 2007.12.14
							//========================================================================================
						}
					}
					//
				}
			}
		}
	}
	else if ( mode == 2 ) {
		if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
			if ( _SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) >= 2 ) {
				for ( i = PM1 ; i < ( PM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
					if ( dataflag2[i] != 0 ) {
						if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( CHECKING_SIDE , i , &pt , dataflag3[i] , 2 , &k , FALSE ) >= 0 ) {
							//====================================================================================================================================
							strncpy( RunRecipe , _SCH_SDM_Get_RECIPE( 2 , pt%100 , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , CHECKING_SIDE , i ) , SDM4_PTYPE_LOTFIRST ) , 64 ); // 2005.10.26
							//====================================================================================================================================
							SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , CHECKING_SIDE , i ) , i ); // 2005.08.27 // 2005.10.26
							//====================================================================================================================================
							_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
									 i ,
									 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 ,
									 RunRecipe ,
									 2 ,
									 1 , "F" , PROCESS_DEFAULT_MINTIME ,
									 0 , 104 );
						}
					}
					else {
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
							if ( ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S1_TAG( i ) == -1 ) && ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S2_TAG( i ) == -1 ) ) { // 2006.01.14
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTAPPEND( CHECKING_SIDE , i );
							}
						}
					}
				}
			}
			else { // 2005.11.13
				for ( i = PM1 ; i < ( PM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) { // 2005.11.13
					if ( dataflag[i] == 0 ) { // 2005.11.13
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) { // 2005.11.13
							if ( ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S1_TAG( i ) == -1 ) && ( SCHEDULER_CONTROL_Get_SDM_4_RERUN_FST_S2_TAG( i ) == -1 ) ) { // 2006.01.14
								if ( !SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTAPPEND( CHECKING_SIDE , i ) ) { // 2005.11.13 // 2006.09.02
									if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) { // 2006.09.02
										if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_ENDFINISH( i ) ) { // 2006.09.02
											SCHEDULER_CONTROL_Reg_SDM_4_CHAMER_ENDFINISH( i ); // 2006.09.02
										}
									}
								}
							}
						} // 2005.11.13
					} // 2005.11.13
				} // 2005.11.13
			} // 2005.11.13
		}
	}
	else {
		return FALSE;
	}
//###################################################################################################
#endif
//###################################################################################################
		
	return TRUE;
}
//===========================================================================================================
BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_4( int side , int WaitParam , int option ) {
	return FALSE;
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 3
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
int  _SCH_COMMON_GUI_INTERFACE_STYLE_4( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
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
		case 1 : //	Put Priority Check
			strcpy( Message , "Chamber Put Priority Check" );
			return 1;
			break;
		case 2 : // Get Priority Check
			strcpy( Message , "Chamber Get Priority Check" );
			return 1;
			break;
		case 3 : // BM Scheduling Factor
			strcpy( Message , "BM Scheduling Factor" );
			return 1;
			break;
		case 4 : // Switching Side
			strcpy( Message , "" );
			return 1;
			break;
		case 5 : // Next Pick
			strcpy( Message , "" );
			return 1;
			break;
		case 6 : // Prev Pick
			strcpy( Message , "" );
			return 1;
			break;
		}
		//
		break;

	case 1 :
		//
		switch( mode ) {
		case 1 : //	Put Priority Check
			//======================================================================================================================================================================
			strcpy( list , "BM Simple Message|BM Full   Message|BM Simple(A2-Wait)|BM Full  (A2-Wait)|BM Simple(A2-WaitAL)|BM Full  (A2-WaitAL)|BM Simple(A2-WaitPR)|BM Full  (A2-WaitPR)" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			strcpy( list , "CM.First|BM.First|CM.First[SynWait]|BM.First[SynWait]" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			strcpy( list , ".|Thread|Default.OneBody|Thread.OneBody|Default.OneBody(2)|Thread.OneBody(2)|Default.OneBody(PM)|Thread.OneBody(PM)|Default.OneBody(2PM)|Thread.OneBody(2PM)|Default.OneBody(PS)|Thread.OneBody(PS)|Default.OneBody(2PS)|Thread.OneBody(2PS)|Thread.OneBody[S](PS)|Thread.OneBody[S](PSW)" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			strcpy( list , "I.First/O.First|I.First/O.Last|I.Last/O.First|I.Last/O.Last" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			strcpy( list , "Normal In/Out|Normal In/Full Out|Full In/Normal Out|Full In/Full Out" );
			strcpy( Message , "Normal In/Out" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			strcpy( list , "" );
			strcpy( Message , "" );
			return -1;
			break;
			//======================================================================================================================================================================
		}
		break;

	case 2 :
		//
		switch( mode ) {
		case 1 : //	Put Priority Check
			//======================================================================================================================================================================
			strcpy( Message , "Chamber Option Check?" );
			strcpy( list , "BM Simple Message|BM Full Message|BM Simple Message(A2-Wait)|BM Full Message(A2-Wait)|BM Simple Message(A2-Wait AL)|BM Full Message(A2-Wait AL)|BM Simple Message(A2-Wait PR)|BM Full Message(A2-Wait PR)" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			strcpy( Message , "Chamber Option Check Order?" );
			strcpy( list , "CM.First|BM.First|CM.First[SynchWait]|BM.First[SynchWait]" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			strcpy( Message , "Dummy and BM Scheduling Factor?" );
			strcpy( list , "Default|Thread|Default.OneBody|Thread.OneBody|Default.OneBody(2)|Thread.OneBody(2)|Default.OneBody(PM)|Thread.OneBody(PM)|Default.OneBody(2PM)|Thread.OneBody(2PM)|Default.OneBody(PS)|Thread.OneBody(PS)|Default.OneBody(2PS)|Thread.OneBody(2PS)|Thread.OneBody[Single](PS)|Thread.OneBody[Single](PSW)" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			*retdata = 3;
			return 3;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			*retdata = 3;
			return 3;
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			return -1;
			break;
			//======================================================================================================================================================================
		}
		break;
	case 3 :
		//
		switch( mode ) {
		case 1 : //	Put Priority Check
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			switch( data )	{
			case 1 :	sprintf( Message , "I.First/O.Last" ); break;
			case 2 :	sprintf( Message , "I.Last/O.First" ); break;
			case 3 :	sprintf( Message , "I.Last/O.Last" ); break;
			default :	sprintf( Message , "I.First/O.First" ); break;
			}
			return 1;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			switch ( data ) {
			case 1  : sprintf( Message , "NI/FO" ); break;
			case 2  : sprintf( Message , "FI/NO" ); break;
			case 3  : sprintf( Message , "FI/FO" ); break;
			default : sprintf( Message , "NI/NO" ); break;
			}
			return 1;
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			strcpy( Message , "" );
			return 1;
			break;
			//======================================================================================================================================================================
		}
		break;

	case 4 :
		//
		switch( mode ) {
		case 1 : //	Put Priority Check
			//======================================================================================================================================================================
			return 0;
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			return 0;
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			return 0;
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			return 0;
			//======================================================================================================================================================================
			break;
			//======================================================================================================================================================================
		}
		break;

	default :
		break;
	}
	//
	return 0;
}
//===============================================================================
int _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_4( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_PRE_ANALYSIS_AL4( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
int _SCH_COMMON_EVENT_ANALYSIS_STYLE_4( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_ANALYSIS_AL4( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
void _SCH_COMMON_SYSTEM_DATA_LOG_STYLE_4( char *filename , int side ) {
	USER_SYSTEM_LOG_AL4( filename , side );
}
//===============================================================================
int  _SCH_COMMON_TRANSFER_OPTION_STYLE_4( int mode , int data ) {
	if      ( mode == 0 ) {
		return 4;
	}
	return 0;
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 4
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_FINISHED_CHECK_STYLE_4( int side ) {
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN_SIDE( side ) ) { // 2003.06.26
		return FALSE;
	}
//###################################################################################################
#endif
//###################################################################################################
	return TRUE;
}
//===========================================================================================================
// 0 : Recipe Read / Slot Overlapped
// 1 : Map Read / Map info check and Unuse DB reset
// 2 : Map Read / Slot Overlapped or No Wafer
// 3 : Map First
// 4 : Map Switch
// 5 : Map End / Verification
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_4( int mode , int side , int ch1 , int ch2 , int option ) {
	return TRUE;
}
//===========================================================================================================
BOOL _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_4( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	return FALSE;
}
//===========================================================================================================
void _SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_4( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_SIDE_SUPPLY_STYLE_4( int side , BOOL EndCheck , int Supply_Style , int *Result ) {
	return FALSE;
}

//===============================================================================

BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_4() {
	return SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4();
}

//===============================================================================

BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_4() {
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) == 2 ) return TRUE;
	return FALSE;
}


//===============================================================================

BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_4( int side , int pt , int ch ) {
	return FALSE;
}



//===============================================================================
int _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_4() { // 2007.10.07
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) return 1;
	return 0;
}

//===============================================================================

int _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_4() {
	return ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) );
}

//===============================================================================
void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_4( int side , int bmc , char *appstr ) {
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	SCHEDULER_CONTROL_Get_SDM_4_Status_String( side , bmc , appstr ); // 2003.05.29
//###################################################################################################
#endif
//###################################################################################################
}

//===============================================================================
int  _SCH_COMMON_GET_METHOD_ORDER_STYLE_4( int side , int pt ) {
	if ( _SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() == 1 ) {
		if ( pt < 100 ) {
			return _SCH_CLUSTER_Get_PathDo( side , pt );
		}
	}
	return 0;
}

//===============================================================================
int  _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_4( int side , int pt , int subchk ) {
	return 1;
}

//===============================================================================
BOOL _SCH_COMMON_CHECK_END_PART_STYLE_4( int side , int mode ) {
	return TRUE;
}

//===============================================================================
int _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_4( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	// Return Value
	// 1 : Dis+prset
	// 2 : Dis
	// 3 : prset
	*DoMore = FALSE;
	return 0;
}


BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_4( int side ) {
	return FALSE;
}



//===============================================================================
int _SCH_COMMON_CHECK_WAITING_MODE_STYLE_4( int side , int *mode , int lc_precheck , int option ) {
	return 0;
}

BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_4( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module ,
						int TuneData_Order ,
						int TuneData_Step ,
						char *TuneData_Name ,
						char *TuneData_Data ,
						int TuneData_Index ,
						char *AppendStr ) {
	return FALSE;
}

BOOL _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_4( int mode , int side , int ch , int slot , int option ) {
	return FALSE;
}

void _SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_4( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ) { // 2010.01.29
}

BOOL _SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_4( int side , int mode , BOOL precheck , int option ) { // 2010.11.09
	return TRUE;
}
BOOL _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_4( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ) { // 2010.11.23
	return FALSE;
}
int  _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_4( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ) { // 2011.05.13
	return 0;
}

int  _SCH_COMMON_USE_EXTEND_OPTION_STYLE_4( int mode , int sub ) { // 2011.11.18
	return TRUE;
}
