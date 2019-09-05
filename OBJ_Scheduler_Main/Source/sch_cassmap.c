#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Equip_Handling.h"
#include "sch_sub.h"
#include "sch_prm.h"
#include "iolog.h"
#include "MR_Data.h"
#include "Errorcode.h"
#include "FA_Handling.h"
#include "IO_Part_data.h"

#include "INF_sch_CommonUser.h"

//------------------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_HMAPPING[MAX_CASSETTE_SIDE];
//
//------------------------------------------------------------------------------------------
int Sch_Map_Buf_Data1[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2005.03.02
int Sch_Map_Buf_Data2[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2005.03.02
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Scheduler_Get_Max_Slot( int side , int in , int out , int opt , BOOL *maxunuse ) { // 2010.12.17
	if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 7 , side , in , out , opt ) ) { // 2010.12.17
		*maxunuse = FALSE;
		return _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT();
	}
	else {
		*maxunuse = TRUE;
		return MAX_CASSETTE_SLOT_SIZE;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_Mapping_Buffer_Data_Reset( int side ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		Sch_Map_Buf_Data1[side][i] = 0;
		Sch_Map_Buf_Data2[side][i] = 0;
	}
}
//
int Scheduler_Mapping_Buffer_Data_Get1( int side , int i ) {
	return Sch_Map_Buf_Data1[side][i];
}
//
int Scheduler_Mapping_Buffer_Data_Get2( int side , int i ) {
	return Sch_Map_Buf_Data2[side][i];
}
//
void Scheduler_Mapping_Buffer_Data_Set( int side , int i , int mode , int data ) {
	switch( mode ) {
	case 0 : // Recipe
		Sch_Map_Buf_Data1[side][i] = data;
		break;
	case 1 : // Map Data before Mapping
		if ( Sch_Map_Buf_Data1[side][i] == 0 ) {
			Sch_Map_Buf_Data1[side][i] = 10 + data;
		}
		else {
			Sch_Map_Buf_Data1[side][i] = data;
		}
		Sch_Map_Buf_Data2[side][i] = 10 + data; // 2007.12.12
		break;
	case 2 : // Map Data After Mapping
		Sch_Map_Buf_Data2[side][i] = 10 + data;
		break;
	case 3 : // Recipe After Mapping
		if ( data == 1 ) {
			if ( Sch_Map_Buf_Data2[side][i] >= 10 ) {
				Sch_Map_Buf_Data2[side][i] = Sch_Map_Buf_Data2[side][i] - 10;
			}
		}
		break;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int SIM_UNLOADLOAD_OPTION; // x noloop waiting noloop+waiting 2018.12.13
extern int SIM_WAITING_TAG[MAX_CASSETTE_SIDE]; // 0:wait 1:success 2:error 3:abort

BOOL SIM_MAPPING_Function( int side , int *Res ) {
	int c = 0;
	//
	if ( SIM_UNLOADLOAD_OPTION == 0 ) return FALSE; 
	//
	Sleep(500);
	//
	*Res = SYS_SUCCESS;
	return TRUE;
	//
	//
	SIM_WAITING_TAG[side] = 0;
	//
	while ( TRUE ) {
		//
		if ( SIM_WAITING_TAG[side] == 1 ) {
			*Res = SYS_SUCCESS;
			break;
		}
		if ( SIM_WAITING_TAG[side] == 2 ) {
			*Res = SYS_ERROR;
			break;
		}
		if ( SIM_WAITING_TAG[side] == 3 ) {
			*Res = SYS_ABORTED;
			break;
		}
		//
		Sleep(250);
		//
		c++;
		if ( ( c % 40 ) == 0 ) printf( "[M-CM%d]" , side + 1 );
	}
	//
	return TRUE;
	//
}
//

void Scheduler_Carrier_Mapping( int CHECKING_SIDE ) {
	int id , MaxSlot , maxunuse , Res;
//	BOOL force = FALSE; // 2013.06.13
	BOOL cioset; // 2013.06.13

	EnterCriticalSection( &CR_HMAPPING[(CHECKING_SIDE%100)] );
	FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%100 , 2 );
	//
	//----------------------------------------------------------------------
	//
	cioset = FA_MAIN_BUTTON_SET_CHECK( CHECKING_SIDE%100 );
	//----------------------------------------------------------------------
	//
	BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%100) + CM1 , CTC_RUNNING );
	BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%100) , _MS_4_MAPPING );
	if ( CHECKING_SIDE >= 100 ) FA_ACCEPT_MESSAGE( CHECKING_SIDE%100 , FA_MAPPING , 0 );
	//
	MaxSlot = Scheduler_Get_Max_Slot( (CHECKING_SIDE%100) , (CHECKING_SIDE%100) + 1 , (CHECKING_SIDE%100) + 1 , 21 , &maxunuse ); // 2010.12.17
	//
	if ( !SIM_MAPPING_Function( CHECKING_SIDE%100 , &id ) ) {
		id = EQUIP_CARRIER_MAPPING( (CHECKING_SIDE%100) , TRUE , TRUE, TRUE , FALSE , (CHECKING_SIDE%100) + 1 , (CHECKING_SIDE%100) + 1 , TRUE , TRUE , 0 , MaxSlot , 1 , &Res ); // 2005.07.08 // 2010.12.17
	}
	//
	if ( CHECKING_SIDE >= 100 ) {
		if ( id == SYS_SUCCESS ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%100) + CM1 , _MS_5_MAPPED ); // 2011.09.07
			//
			FA_RESULT_MESSAGE( CHECKING_SIDE%100 , FA_MAP_COMPLETE , 0 );
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%100 , 1 );
			BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%100) , _MS_5_MAPPED );
		}
		else {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%100) + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
			//
//			FA_REJECT_MESSAGE( CHECKING_SIDE%100 , FA_MAPPING , 0 , "" ); // 2007.01.26
			FA_REJECT_MESSAGE( CHECKING_SIDE%100 , FA_MAPPING , 104 , "" ); // 2007.01.26
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%100 , 0 );
			BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%100) , _MS_6_MAPFAIL );
		}
	}
	else {
		if ( id == SYS_SUCCESS ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%100) + CM1 , _MS_5_MAPPED ); // 2011.09.07
			//
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%100 , 1 );
			BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%100) , _MS_5_MAPPED );
		}
		else {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%100) + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
			//
			BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%100) , _MS_6_MAPFAIL );
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%100 , 0 );
		}
	}
	BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%100) + CM1 , CTC_IDLE );
//	if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%100) , CTC_IDLE ); // 2013.06.13
	if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%100) , CTC_IDLE ); // 2013.06.13
	LeaveCriticalSection( &CR_HMAPPING[(CHECKING_SIDE%100)] );
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Mapping Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Scheduler_Mapping_Real_Part( int CHECKING_SIDE , BOOL First , int ch1 , int ch2 , int ss , int es ) {
	int Res;
	BOOL xc1 , xc2;
	//
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) return ERROR_NONE; // 2010.10.12 // 2017.10.10
	if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) return ERROR_NONE; // 2010.10.12 // 2017.10.10
	//
	if ( ( ch1 < 0 ) && ( ch2 < 0 ) ) return ERROR_NONE; // 2010.01.19
	//
	_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Part Start%cMAPPINGSTART %d:%d:%d:%d\n" , 9 , ch1 , ch2 , ss , es );
	//
	if ( ch1 >= 0 ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch1 ) == 1 ) {
			xc1 = TRUE;
		}
		else {
			xc1 = FALSE;
		}
	}
	else {
		xc1 = FALSE;
	}
	if ( ch2 >= 0 ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch2 ) == 1 ) {
			xc2 = TRUE;
		}
		else {
			xc2 = FALSE;
		}
	}
	else {
		xc2 = FALSE;
	}
	// Mapping
//	if ( EQUIP_CARRIER_MAPPING( CHECKING_SIDE , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , First , TRUE , ch1 , ch2 , xc1 , xc2 , ss , es ) == SYS_ABORTED ) { // 2004.04.27
	if ( EQUIP_CARRIER_MAPPING( CHECKING_SIDE , FALSE , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , First , TRUE , ch1 , ch2 , xc1 , xc2 , ss , es , 0 , &Res ) == SYS_ABORTED ) { // 2004.04.27
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Part Fail[%d]%cMAPPINGFAIL\n" , Res , 9 );
		return ERROR_CASS_MAP;
	}
	_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Part Success%cMAPPINGSUCCESS\n" , 9 );
	return ERROR_NONE;
}
//
int Scheduler_Mapping_Check_Part( int CHECKING_SIDE , int Mode , int ch1 , int ch2 , int ss , int es , int CPJOB ) {
	int Res , curres;
	// Mode 0 : First
	//      1 : Switch
	//      2 : End or Second(Loop Always)

	if ( _SCH_COMMON_USER_RECIPE_CHECK( 191 , CHECKING_SIDE , Mode , ch1 , ch2 , &curres ) ) { // 2012.01.12
		if ( Mode == 1 ) SCHEDULER_CONTROL_Switch_Scheduler_Data( CHECKING_SIDE );
		return curres;
	}
	//
	if ( Mode == 2 ) {
		if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 5 , CHECKING_SIDE , ch1 , ch2 , 0 ) ) { // 2008.04.29
			if ( ch1 > 0 ) {
				if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
					if ( ( _i_SCH_PRM_GET_MAPPING_SKIP( ch1 ) == 3 ) || ( _i_SCH_PRM_GET_MAPPING_SKIP( ch1 ) == 4 ) || ( _i_SCH_PRM_GET_MAPPING_SKIP( ch1 ) == 7 ) ) {
//						if ( !SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch1 ) ) {
						Res = SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch1 );
						if ( Res != 0  ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Check Part21 Fail%cMAPPINGFAIL %d:%d:%d\n" , 9 , ch1 , ch2 , Res );
							return ERROR_CASS_MAP;
						}
					}
				}
/*
// 2010.05.12
				else {
//					if ( !SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch1 ) ) {
					Res = SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch1 );
					if ( Res != 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Check Part22 Fail%cMAPPINGFAIL %d:%d:%d\n" , 9 , ch1 , ch2 , Res );
						return ERROR_CASS_MAP;
					}
				}
*/
			}
			if ( ( ch1 != ch2 ) && ( ch2 > 0 ) ) {
				if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) {
					if ( ( _i_SCH_PRM_GET_MAPPING_SKIP( ch2 ) == 3 ) || ( _i_SCH_PRM_GET_MAPPING_SKIP( ch2 ) == 4 ) || ( _i_SCH_PRM_GET_MAPPING_SKIP( ch2 ) == 7 ) ) {
//						if ( !SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch2 ) ) {
						Res = SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch2 );
						if ( Res != 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Check Part23 Fail%cMAPPINGFAIL %d:%d:%d\n" , 9 , ch1 , ch2 , Res );
							return ERROR_CASS_MAP;
						}
					}
				}
/*
// 2010.05.12
				else {
//					if ( !SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch2 ) ) {
					Res = SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( CHECKING_SIDE , ch2 );
					if ( Res != 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Check Part24 Fail%cMAPPINGFAIL %d:%d:%d\n" , 9 , ch1 , ch2 , Res );
						return ERROR_CASS_MAP;
					}
				}
*/
			}
		}
	}
	else {
		if ( Mode == 1 ) {
			SCHEDULER_CONTROL_Switch_Scheduler_Data( CHECKING_SIDE );
			if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 4 , CHECKING_SIDE , ch1 , ch2 , 0 ) ) { // 2008.05.20
			}
		}
		else {
			if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 3 , CHECKING_SIDE , ch1 , ch2 , 0 ) ) { // 2008.05.20
			}
		}
		//
//		if ( !SCHEDULER_CONTROL_Check_Scheduler_Data_for_Run( CHECKING_SIDE , ( Mode == 1 ) , ss , es , CPJOB , ch1 , ch2 ) ) {
		Res = SCHEDULER_CONTROL_Check_Scheduler_Data_for_Run( CHECKING_SIDE , ( Mode == 1 ) ? 1 : 0 , ss , es , CPJOB , ch1 , ch2 );
		if ( Res != 0 ) {
			// No Wafer or Wafer Synch Error
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier Mapping Part No Wafer of Wafer Synch Error%cMAPPINGFAIL %d:%d:%d\n" , 9 , ch1 , ch2 , Res );
			return ERROR_CASS_MAP2;
		}
	}
	return ERROR_NONE;
}
