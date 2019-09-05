#include "INF_Scheduler.h"

#include "sch_A6_default.h"
#include "sch_A6_dll.h"
#include "sch_A6_init.h"
#include "sch_A6_log.h"
#include "sch_A6_event.h"
#include "sch_A6_sub.h"



//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 1
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_CONFIG_INITIALIZE_STYLE_6( int runalg , int subalg , int guialg , int maxpm ) {
	return TRUE;
}
//===========================================================================================================
void _SCH_COMMON_REVISION_HISTORY_STYLE_6( int mode , char *data , int count ) {
	USER_REVISION_HISTORY_AL6_PART( mode , data , count );
}
//===========================================================================================================
void _SCH_COMMON_INITIALIZE_PART_STYLE_6( int apmode , int side ) {
	INIT_SCHEDULER_AL6_PART_DATA( apmode , side );
}
//===========================================================================================================
int _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_6( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	int i , j , k;
	//
	*TMATM = 0; // 2004.02.25
	for ( i = TM ; i < FM ; i++ ) { // 2004.02.25
//		if ( _SCH_PRM_GET_MODULE_MODE( i ) ) *TMATM = i - TM + 1; // 2004.02.25 // 2013.01.25
		if ( _SCH_PRM_GET_MODULE_MODE( i ) ) *TMATM = i - TM; // 2004.02.25 // 2013.01.25
	} // 2004.02.25
	//
//	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.25 // 2014.07.10
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) { // 2013.01.25 // 2014.07.10
		//
//		for ( i = 0 ; i < (*TMATM) ; i++ ) { // 2013.01.25
		for ( i = 0 ; i <= (*TMATM) ; i++ ) { // 2013.01.25
			for ( j = PM1 ; j < AL ; j++ ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( j ) == i ) {
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j ) > MUF_00_NOTUSE ) {
						_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , BM1 + i , MUF_01_USE );
						_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
					}
				}
			}
		}
	}
	else {
		for ( i = 0 ; i <= (*TMATM) ; i++ ) { // 2013.01.25
			for ( j = PM1 ; j < AL ; j++ ) {
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( j ) == i ) {
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j ) > MUF_00_NOTUSE ) {
						//
						_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + i , MUF_01_USE );
						//
						for ( k = 0 ; k < 4 ; k++ ) {
							if ( _SCH_PRM_GET_MODULE_GROUP( BM1 + k ) == i ) {
								//
								if ( SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k , 0 ) ) continue; // 2014.07.10
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
									_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , BM1 + k , MUF_01_USE );
								}
							}
						}
						//
					}
					//
				}
			}
		}
	}
	return ERROR_NONE;
}
//===========================================================================================================
void _SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_6( int side , int bmc , int bmmode ) {
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 2
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
void _SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_6( HINSTANCE dllpt , int unload ) {
	INIT_SCHEDULER_AL6_PART_SET_DLL_INSTANCE( dllpt , unload );
}
//===========================================================================================================
BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_6( int chg_ALG_DATA_RUN ) {
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) _SCH_PRM_SET_MODULE_GROUP( PM1 + i , ( i / 2 ) );
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) _SCH_PRM_SET_MODULE_GROUP( BM1 + i , i );
	return TRUE;
}
//===========================================================================================================
BOOL _SCH_COMMON_PRE_PART_OPERATION_STYLE_6( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
	int i;

	if      ( mode == 0 ) {
	}
	else if ( mode == 1 ) {
		//
//		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.19 // 2014.07.10
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) { // 2013.01.19 // 2014.07.10
			//
			for ( i = BM1 ; i < TM ; i++ ) { // 2004.02.25
				if ( _SCH_PRM_GET_MODULE_MODE( i - BM1 + TM ) ) { // 2004.02.25
					if ( dataflag[i] != 0 ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 ); // 2007.12.14
					}
				}
			}
			//
		}
		else {
			//
			for ( i = BM1 ; i < ( BM1 + 4 ) ; i++ ) {
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i ) ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber6( i , 0 ) ) continue; // 2014.07.10
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) { // 2013.11.22
						//
						if ( dataflag[i] != 0 ) {
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
						}
						//
					}
				}
				//
			}
			//
			//
			for ( i = ( BM1 + 4 ) ; i < TM ; i++ ) {
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i ) ) {
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) { // 2013.11.22
						//
						if ( dataflag[i] != 0 ) {
							//
							if ( i != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
							else {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
							}
						}
					}
					//
				}
				//
			}
			//
		}
	}
	else if ( mode == 2 ) {
	}
	else {
		return FALSE;
	}

	return TRUE;
	//
}
//===========================================================================================================
BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_6( int side , int WaitParam , int option ) {
	return FALSE;
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 3
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
int  _SCH_COMMON_GUI_INTERFACE_STYLE_6( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
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
		case 3 : // BM Scheduling Factor
			strcpy( Message , "BM Scheduling Factor" );
			return 1;
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
			strcpy( list , "POST.BMPick(X)[Default]|POST.BMPick(O)[Default]|POST.BMPick(X)[Seq1]|POST.BMPick(O)[Seq1]|POST.BMPick(X)[Priority]|POST.BMPick(O)[Priority]" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			strcpy( list , "CM.First[I:U/O:D]|BM.First[I:U/O:D]|CM.First[I:D/O:U]|BM.First[I:D/O:U]" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			strcpy( list , ".|TB-OneBody|TB-OneBody(NW)|TB-OneBody(TF)|TB-OneBd(NWTF)|TB-CYCLON" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			strcpy( list , "(TM=F/F)(FM=F/F)|(TM=F/L)(FM=F/F)|(TM=L/F)(FM=F/F)|(TM=L/L)(FM=F/F)|(TM=F/F)(FM=F/L)|(TM=F/L)(FM=F/L)|(TM=L/F)(FM=F/L)|(TM=L/L)(FM=F/L)|(TM=F/F)(FM=L/F)|(TM=F/L)(FM=L/F)|(TM=L/F)(FM=L/F)|(TM=L/L)(FM=L/F)|(TM=F/F)(FM=L/L)|(TM=F/L)(FM=L/L)|(TM=L/F)(FM=L/L)|(TM=L/L)(FM=L/L)" );
			strcpy( Message , "????" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			strcpy( list , "" );
			switch( data ) {
			case 0 :
				strcpy( Message , "Always" );					break;
			case 1 :
				strcpy( Message , "Process Free" );					break;
			case 2 :
				strcpy( Message , "Wafer Free" );					break;
			case 3 :
				strcpy( Message , "Process+Wafer Free" );					break;
			case 4 :
				strcpy( Message , "Out Process Pick Deny" );					break;
			case 5 :
				strcpy( Message , "Out Process Pick+Move Deny" );					break;
			case 6 :
				strcpy( Message , "Out Process Only Move Deny" );					break;
			case 7 : // 2004.12.14
				strcpy( Message , "Process+Wafer Free(ALL)" );					break;
			case 8 : // 2005.03.29
				strcpy( Message , "Process Free(LX)" );					break;
			case 9 : // 2005.03.29
				strcpy( Message , "Wafer Free(LX)" );					break;
			case 10 : // 2005.03.29
				strcpy( Message , "Process+Wafer Free(LX)" );					break;
			case 11 : // 2006.01.26
				strcpy( Message , "Process Free(A1)" );					break;
			case 12 : // 2006.01.26
				strcpy( Message , "Wafer Free(A1)" );					break;
			case 13 : // 2006.01.26
				strcpy( Message , "Process+Wafer Free(A1)" );					break;
			case 14 : // 2006.02.08
				strcpy( Message , "Process Free(A1.G0)" );					break;
			case 15 : // 2006.02.08
				strcpy( Message , "Wafer Free(A1.G0)" );					break;
			case 16 : // 2006.02.08
				strcpy( Message , "Process+Wafer Free(A1.G0)" );					break;
			case 17 : // 2006.07.13
				strcpy( Message , "Count Free" );					break;
			case 18 : // 2007.12.12
				strcpy( Message , "Direct Switchable" );					break;
			case 101 : // 2006.04.15
				strcpy( Message , "User Define 001" );					break;
			case 102 : // 2006.04.15
				strcpy( Message , "User Define 002" );					break;
			default :
				strcpy( Message , "????" );
				break;
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
			//======================================================================================================================================================================
			strcpy( Message , "Chamber Option Check?" );
			strcpy( list , "POST is Yes.BMPick(X)[Default]|POST is Yes.BMPick(O)[Default]|POST is Yes.BMPick(X)[Seq1]|POST is Yes.BMPick(O)[Seq1]|POST is Yes.BMPick(X)[Priority]|POST is Yes.BMPick(O)[Priority]" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 2 : // Get Priority Check
			//======================================================================================================================================================================
			strcpy( Message , "Chamber Option Check Order?" );
			strcpy( list , "CM.First[I:Up/O:Down]|BM.First[I:Up/O:Down]|CM.First[I:Down/O:Up]|BM.First[I:Down/O:Up]" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 3 : // BM Scheduling Factor
			//======================================================================================================================================================================
			strcpy( Message , "TM and BM Scheduling Factor?" );
			strcpy( list , "Default|TM/BM-OneBody|TM/BM-OneBody(NoWait)|TM/BM-OneBody(TM Free)|TM/BM-OneBody(NoWait,TM Free)|TM/BM-CYCLON" );
			return 1;
			break;
			//======================================================================================================================================================================
		case 4 : // Switching Side
			//======================================================================================================================================================================
			*retdata = 15;
			return 3;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			if      ( *retdata == 0 ) *retdata = 1;
			else if ( *retdata == 1 ) *retdata = 2;
			else if ( *retdata == 2 ) *retdata = 3;
			else if ( *retdata == 3 ) *retdata = 4;
			else if ( *retdata == 4 ) *retdata = 5;
			else if ( *retdata == 5 ) *retdata = 6;
			else if ( *retdata == 6 ) *retdata = 7;
			else if ( *retdata == 7 ) *retdata = 8; // 2004.12.14
			else if ( *retdata == 8 ) *retdata = 9; // 2005.03.29
			else if ( *retdata == 9 ) *retdata = 10; // 2005.03.29
			else if ( *retdata ==10 ) *retdata = 11; // 2005.03.29
			else if ( *retdata ==11 ) *retdata = 12; // 2006.01.26
			else if ( *retdata ==12 ) *retdata = 13; // 2006.01.26
			else if ( *retdata ==13 ) *retdata = 14; // 2006.01.26
			else if ( *retdata ==14 ) *retdata = 15; // 2006.02.08
			else if ( *retdata ==15 ) *retdata = 16; // 2006.02.08
			else if ( *retdata ==16 ) *retdata = 17; // 2006.04.15
			else if ( *retdata ==17 ) *retdata = 18; // 2007.12.12
			else if ( *retdata ==18 ) *retdata = 101; // 2006.07.13
			else if ( *retdata ==101) *retdata = 102; // 2006.04.15
			else if ( *retdata ==102) *retdata = 0; // 2006.04.15
			return 2;
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			*retdata = 3;
			return 3;
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
			case 1 :	sprintf( Message , "(TM=F/L)(FM=F/F)" ); break;
			case 2 :	sprintf( Message , "(TM=L/F)(FM=F/F)" ); break;
			case 3 :	sprintf( Message , "(TM=L/L)(FM=F/F)" ); break;
			//
			case 4 :	sprintf( Message , "(TM=F/F)(FM=F/L)" ); break; // 2004.11.25
			case 5 :	sprintf( Message , "(TM=F/L)(FM=F/L)" ); break; // 2004.11.25
			case 6 :	sprintf( Message , "(TM=L/F)(FM=F/L)" ); break; // 2004.11.25
			case 7 :	sprintf( Message , "(TM=L/L)(FM=F/L)" ); break; // 2004.11.25
			//
			case 8 :	sprintf( Message , "(TM=F/F)(FM=L/F)" ); break; // 2004.11.25
			case 9 :	sprintf( Message , "(TM=F/L)(FM=L/F)" ); break; // 2004.11.25
			case 10 :	sprintf( Message , "(TM=L/F)(FM=L/F)" ); break; // 2004.11.25
			case 11 :	sprintf( Message , "(TM=L/L)(FM=L/F)" ); break; // 2004.11.25
			//
			case 12 :	sprintf( Message , "(TM=F/F)(FM=L/L)" ); break; // 2004.11.25
			case 13 :	sprintf( Message , "(TM=F/L)(FM=L/L)" ); break; // 2004.11.25
			case 14 :	sprintf( Message , "(TM=L/F)(FM=L/L)" ); break; // 2004.11.25
			case 15 :	sprintf( Message , "(TM=L/L)(FM=L/L)" ); break; // 2004.11.25
			//
			default :	sprintf( Message , "(TM=F/F)(FM=F/F)" ); break;
			}
			return 1;
			break;
			//======================================================================================================================================================================
		case 5 : // Next Pick
			//======================================================================================================================================================================
			switch ( data ) {
			case 0 : sprintf( Message , "" ); break;
			case 1 : sprintf( Message , "PF" ); break;
			case 2 : sprintf( Message , "WF" ); break;
			case 3 : sprintf( Message , "AF" ); break;
			case 4 : sprintf( Message , "OPF" ); break;
			case 5 : sprintf( Message , "OAF" ); break;
			case 6 : sprintf( Message , "OMF" ); break;
			case 7 : sprintf( Message , "FF" ); break; // 2004.12.14
			case 8 : sprintf( Message , "PF.lx" ); break; // 2005.03.29
			case 9 : sprintf( Message , "WF.lx" ); break; // 2005.03.29
			case 10 : sprintf( Message , "AF.lx" ); break; // 2005.03.29
			case 11 : sprintf( Message , "PF.A1" ); break; // 2006.01.26
			case 12 : sprintf( Message , "WF.A1" ); break; // 2006.01.26
			case 13 : sprintf( Message , "AF.A1" ); break; // 2006.01.26
			case 14 : sprintf( Message , "PF.A1G0" ); break; // 2006.02.08
			case 15 : sprintf( Message , "WF.A1G0" ); break; // 2006.02.08
			case 16 : sprintf( Message , "AF.A1G0" ); break; // 2006.02.08
			case 17 : sprintf( Message , "CF" ); break; // 2006.07.13
			case 18 : sprintf( Message , "DSWITCH" ); break; // 2007.12.12
			case 101: sprintf( Message , "UD001" ); break; // 2006.04.15
			case 102: sprintf( Message , "UD002" ); break; // 2006.04.15
			default : sprintf( Message , "????" ); break;
			}
			return 1;
			break;
			//======================================================================================================================================================================
		case 6 : // Prev Pick
			//======================================================================================================================================================================
			switch ( data ) {
			case 0 : sprintf( Message , "" ); break;
			case 1 : sprintf( Message , "PF" ); break;
			case 2 : sprintf( Message , "WF" ); break;
			case 3 : sprintf( Message , "AF" ); break;
			default : sprintf( Message , "????" ); break;
			}
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
int _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_PRE_ANALYSIS_AL6( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
int _SCH_COMMON_EVENT_ANALYSIS_STYLE_6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return USER_EVENT_ANALYSIS_AL6( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
void _SCH_COMMON_SYSTEM_DATA_LOG_STYLE_6( char *filename , int side ) {
	USER_SYSTEM_LOG_AL6( filename , side );
}
//===============================================================================
int  _SCH_COMMON_TRANSFER_OPTION_STYLE_6( int mode , int data ) {
	if      ( mode == 0 ) {
		return 6;
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
BOOL _SCH_COMMON_FINISHED_CHECK_STYLE_6( int side ) {
	return TRUE;
}
//===========================================================================================================
// 0 : Recipe Read / Slot Overlapped
// 1 : Map Read / Map info check and Unuse DB reset
// 2 : Map Read / Slot Overlapped or No Wafer
// 3 : Map First
// 4 : Map Switch
// 5 : Map End / Verification
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_6( int mode , int side , int ch1 , int ch2 , int option ) {
	return TRUE;
}
//===========================================================================================================
BOOL _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_6( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	return FALSE;
}
//===========================================================================================================
void _SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_6( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_SIDE_SUPPLY_STYLE_6( int side , BOOL EndCheck , int Supply_Style , int *Result ) {
	return FALSE;
}

//===============================================================================

BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_6() {
	return FALSE;
}

//===============================================================================

BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_6() {
	return FALSE;
}


//===============================================================================

BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_6( int side , int pt , int ch ) {
	return FALSE;
}



//===============================================================================
int _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_6() { // 2007.10.07
	return 0;
}

//===============================================================================

int _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_6() {
	return 0;
}

//===============================================================================
void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_6( int side , int bmc , char *appstr ) {
}

//===============================================================================
int  _SCH_COMMON_GET_METHOD_ORDER_STYLE_6( int side , int pt ) {
	if ( _SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() == 1 ) {
		if ( pt < 100 ) {
			return _SCH_CLUSTER_Get_PathDo( side , pt );
		}
	}
	return 0;
}

//===============================================================================
int  _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_6( int side , int pt , int subchk ) {
	return 1;
}

//===============================================================================
BOOL _SCH_COMMON_CHECK_END_PART_STYLE_6( int side , int mode ) {
	return TRUE;
}

//===============================================================================
int _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_6( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	// Return Value
	// 1 : Dis+prset
	// 2 : Dis
	// 3 : prset
	*DoMore = FALSE;
	return 0;
}


BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_6( int side ) {
	return FALSE;
}



//===============================================================================
int _SCH_COMMON_CHECK_WAITING_MODE_STYLE_6( int side , int *mode , int lc_precheck , int option ) {
	return FALSE;
}

BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_6( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module ,
						int TuneData_Order ,
						int TuneData_Step ,
						char *TuneData_Name ,
						char *TuneData_Data ,
						int TuneData_Index ,
						char *AppendStr ) {
	return FALSE;
}

BOOL _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_6( int mode , int side , int ch , int slot , int option ) {
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) return FALSE;
	//
	if ( mode == 0 ) {
		if      ( ( ch >= TM ) && ( ch < TM + MAX_TM_CHAMBER_DEPTH ) ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( ch - TM + PM1 , FALSE , side ) ) return TRUE;
		}
		else if ( ( ch >= PM1 ) && ( ch < PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) return TRUE;
		}
		else if ( ( ch >= BM1 ) && ( ch < BM1 + MAX_BM_CHAMBER_DEPTH ) ) {
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.01.29 // 2014.07.10
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) { // 2013.01.29 // 2014.07.10
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( ch - BM1 + PM1 , FALSE , side ) ) return TRUE;
				//
			}
			else {
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_GROUP( ch ) + PM1 , FALSE , side ) ) return TRUE;
				//
			}
			//
		}
	}
	else if ( mode == 1 ) {
		if ( ch > 0 ) return TRUE;
	}
	//
	return FALSE;
}

void _SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_6( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ) { // 2010.01.29
}


BOOL _SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_6( int side , int mode , BOOL precheck , int option ) { // 2010.11.09
	return TRUE;
}
BOOL _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_6( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ) { // 2010.11.23

	if ( *srccass > ( CM1 + MAX_CASSETTE_SIDE ) ) { // 2010.12.01
		//
		if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
		//
		*srccass = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		//
		return TRUE;

	}

	return FALSE;
}

int  _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_6( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ) { // 2011.05.13
	return 0;
}


int  _SCH_COMMON_USE_EXTEND_OPTION_STYLE_6( int mode , int sub ) { // 2011.11.18
	return TRUE;
}
