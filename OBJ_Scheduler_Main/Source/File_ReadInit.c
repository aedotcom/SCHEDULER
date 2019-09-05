#include "default.h"

#include "EQ_Enum.h"

#include "system_tag.h"
#include "sch_prepost.h"
#include "multijob.h"
#include "sch_ProcessMonitor.h"
#include "Gui_Handling.h"
#include "File_ReadInit.h"
#include "User_Parameter.h"
#include "FA_Handling.h"
#include "Robot_Handling.h"
#include "Alarm_Analysis.h"
#include "Utility.h"
//
#include "INF_sch_CommonUser.h"

#include "SchCFGIntf.h"	// 2013.08.22


extern int EXTEND_SOURCE_USE; // 2018.09.05

void FILE_PARAM_SM_SCHEDULER_SETTING(); // 2013.08.22

//==============================================================================
//==============================================================================
#define MAX_MESSAGE_TABLE_SIZE		22


//==============================================================================
//==============================================================================
//
// 2017.04.21
//
char SCH_SW_BM_SCHEDULING_FACTOR_ORDER[64] = { 0 , };

void SCH_SW_BM_SCHEDULING_FACTOR_ORDER_SET( char *data ) {
	strcpy( SCH_SW_BM_SCHEDULING_FACTOR_ORDER , data );
}

char *SCH_SW_BM_SCHEDULING_FACTOR_ORDER_GET() {
	return SCH_SW_BM_SCHEDULING_FACTOR_ORDER;
}

//============================================================================================================
//char  _SCH_Message_Table_BLANK[2] = { 0 , 0 };
char *_SCH_Message_Table[ MAX_MESSAGE_TABLE_SIZE ];
//============================================================================================================
char *SCH_Get_String_Table( int index ) {
	if ( ( index < 0 ) || ( index >= MAX_MESSAGE_TABLE_SIZE ) ) return COMMON_BLANK_CHAR;
	if ( _SCH_Message_Table[ index ] == NULL ) return COMMON_BLANK_CHAR;
	return _SCH_Message_Table[ index ];
}
//============================================================================================================
BOOL SCH_Set_String_Table( int index , char *Data ) {
	int i;
	if ( ( index < 0 ) || ( index >= MAX_MESSAGE_TABLE_SIZE ) ) return FALSE;
	if ( _SCH_Message_Table[ index ] != NULL ) free( _SCH_Message_Table[ index ] );
	i = strlen( Data );
	_SCH_Message_Table[ index ] = calloc( i + 1 , sizeof( char ) );
	if ( _SCH_Message_Table[ index ] == NULL ) return FALSE;
	memcpy( _SCH_Message_Table[ index ] , Data , i + 1 );
	return TRUE;
}
//============================================================================================================
void _SCH_PRM_MAKE_MODULE_PICK_ORDERING() { // 2012.09.21
	int i;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i ) > 0 ) {
			_i_SCH_PRM_SET_MODULE_PICK_ORDERING_USE( 1 );
			return;
		}
	}
	_i_SCH_PRM_SET_MODULE_PICK_ORDERING_USE( 0 );
}
//-------------------------------------------------------------------------
// Function = FILE_SYSTEM_SETUP
//-------------------------------------------------------------------------
BOOL FILE_MESSAGE_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	char datastr[256];
	int group , i , l , m , n;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	//-----------------------------------------------------------------------------
	// Default Session
	//-----------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_MESSAGE_TABLE_SIZE ; i++ ) _SCH_Message_Table[ i ] = NULL;
	SCH_Set_String_Table( ST_INDEX_LOAD_ENTER				, "Running Load Enter Function" );
	SCH_Set_String_Table( ST_INDEX_LOAD_LOAD				, "If you need to run for Load Function ,\nPress [Load] Button.\nIf Load Operation has been done ,\nPress [Load Complete] Button !!" );
	SCH_Set_String_Table( ST_INDEX_LOAD_COMPLETE			, "If Load Operation has been done ,\nPress [Load Complete] Button !!" );
	SCH_Set_String_Table( ST_INDEX_LOAD_RUNNING				, "Running Load Function\nWait Load Function Success\nIf you want to stop(abort) ,\nPress [Abort] Button" );
	SCH_Set_String_Table( ST_INDEX_LOAD_CONFIRM_COMPLETE	, "Do you want to Set Load Complete?" );
	SCH_Set_String_Table( ST_INDEX_LOAD_CONFIRM_ABORT		, "Do you want to Abort or Cancel?" );
	SCH_Set_String_Table( ST_INDEX_LOAD_CONFIRM_START		, "Do you want to Start Load Function?" );
	SCH_Set_String_Table( ST_INDEX_LOAD_AGAIN				, "Do you want to load operation again???\n(Already Load Completed)" );
	SCH_Set_String_Table( ST_INDEX_LOAD_AGAIN2				, "Do you want to send Load/Request again???\n(Already Load Request)" );
	SCH_Set_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_ABORT	, "Do you want to Abort or Cancel?" );
	SCH_Set_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_NO		, "No & Wait Complete?" );
	SCH_Set_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_YES		, "Yes & Wait Complete?" );

	SCH_Set_String_Table( ST_INDEX_UNLOAD_ENTER				, "Running Unload Enter Function" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_UNLOAD			, "If you need to run for Unload Function ,\nPress [Unload] Button.\nIf Unload Operation has been done ,\nPress [Unload Complete] Button !!" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_UNLOADM			, "If you need to run for Unload Function ,\nPress [Unload] Button." );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_COMPLETE			, "If Unload Operation has been done ,\nPress [Unload Complete] Button !!" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_RUNNING			, "Running Unload Function\nWait Unload Function Success\nIf you want to stop(abort) ,\nPress [Abort] Button" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_CONFIRM_COMPLETE	, "Do you want to Set Unload Complete?" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_CONFIRM_ABORT		, "Do you want to Abort or Cancel?" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_CONFIRM_LATER		, "Do you want to Unload later?" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_CONFIRM_START		, "Do you want to Start Unload Function?" );
	SCH_Set_String_Table( ST_INDEX_UNLOAD_AGAIN				, "Do you want to unload operation ???\n(Not Properly Situation for Unload)" );
	//-----------------------------------------------------------------------------
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return TRUE;
	//-----------------------------------------------------------------------------
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt >= 2 ) {
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_lclose( hFile );
				return TRUE;
			}
			if      ( STRCMP_L( chr_return , "LOAD_ENTER"              ) ) group = ST_INDEX_LOAD_ENTER;
			else if ( STRCMP_L( chr_return , "LOAD_LOAD"               ) ) group = ST_INDEX_LOAD_LOAD;
			else if ( STRCMP_L( chr_return , "LOAD_COMPLETE"           ) ) group = ST_INDEX_LOAD_COMPLETE;
			else if ( STRCMP_L( chr_return , "LOAD_RUNNING"            ) ) group = ST_INDEX_LOAD_RUNNING;
			else if ( STRCMP_L( chr_return , "LOAD_CONFIRM_COMPLETE"   ) ) group = ST_INDEX_LOAD_CONFIRM_COMPLETE;
			else if ( STRCMP_L( chr_return , "LOAD_CONFIRM_ABORT"      ) ) group = ST_INDEX_LOAD_CONFIRM_ABORT;
			else if ( STRCMP_L( chr_return , "LOAD_CONFIRM_START"      ) ) group = ST_INDEX_LOAD_CONFIRM_START;
			else if ( STRCMP_L( chr_return , "LOAD_AGAIN"              ) ) group = ST_INDEX_LOAD_AGAIN;
			else if ( STRCMP_L( chr_return , "LOAD_AGAIN2"             ) ) group = ST_INDEX_LOAD_AGAIN2;
			else if ( STRCMP_L( chr_return , "LOAD_AGAIN2_CONF_ABORT"  ) ) group = ST_INDEX_LOAD_AGAIN2_CONF_ABORT;
			else if ( STRCMP_L( chr_return , "LOAD_AGAIN2_CONF_NO"     ) ) group = ST_INDEX_LOAD_AGAIN2_CONF_NO;
			else if ( STRCMP_L( chr_return , "LOAD_AGAIN2_CONF_YES"    ) ) group = ST_INDEX_LOAD_AGAIN2_CONF_YES;

			else if ( STRCMP_L( chr_return , "UNLOAD_ENTER"            ) ) group = ST_INDEX_UNLOAD_ENTER;
			else if ( STRCMP_L( chr_return , "UNLOAD_UNLOAD"           ) ) group = ST_INDEX_UNLOAD_UNLOAD;
			else if ( STRCMP_L( chr_return , "UNLOAD_UNLOAD(M)"        ) ) group = ST_INDEX_UNLOAD_UNLOADM;
			else if ( STRCMP_L( chr_return , "UNLOAD_COMPLETE"         ) ) group = ST_INDEX_UNLOAD_COMPLETE;
			else if ( STRCMP_L( chr_return , "UNLOAD_RUNNING"          ) ) group = ST_INDEX_UNLOAD_RUNNING;
			else if ( STRCMP_L( chr_return , "UNLOAD_CONFIRM_COMPLETE" ) ) group = ST_INDEX_UNLOAD_CONFIRM_COMPLETE;
			else if ( STRCMP_L( chr_return , "UNLOAD_CONFIRM_ABORT"    ) ) group = ST_INDEX_UNLOAD_CONFIRM_ABORT;
			else if ( STRCMP_L( chr_return , "UNLOAD_CONFIRM_LATER"    ) ) group = ST_INDEX_UNLOAD_CONFIRM_LATER;
			else if ( STRCMP_L( chr_return , "UNLOAD_CONFIRM_START"    ) ) group = ST_INDEX_UNLOAD_CONFIRM_START;
			else if ( STRCMP_L( chr_return , "UNLOAD_AGAIN"            ) ) group = ST_INDEX_UNLOAD_AGAIN;
			else                                                           group = -1;
			if ( group != -1 ) {
				if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					_lclose( hFile );
					return TRUE;
				}
				l = strlen( chr_return );
				if ( ( l > 0 ) && ( chr_return[0] == '"' ) && ( chr_return[l-1] == '"' ) ) {
					chr_return[l-1] = 0;
					m = 0;
					n = 0;
					for ( i = 1 ; i < ( l - 1 ) ; i++ ) {
						if ( chr_return[i] == '\\' ) {
							if ( n == 0 ) {
								n = 1;
							}
							else {
								datastr[m] = '\\'; m++;
							}
						}
						else {
							if ( n == 0 ) {
								datastr[m] = chr_return[i]; m++;
							}
							else {
								if      ( chr_return[i] == 'n' ) {
									datastr[m] = 13; m++;
									datastr[m] = 10; m++;	n = 0;
								}
								else if ( chr_return[i] == 't' ) {
									datastr[m] = 9; m++;	n = 0;
								}
								else {
									datastr[m] = '\\'; m++;
									datastr[m] = chr_return[i]; m++; n = 0;
								}
							}
						}
					}
					datastr[m] = 0;
					SCH_Set_String_Table( group	, datastr );
				}
			}
		}
	}
	_lclose( hFile );
	return TRUE;
}
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
int Get_LOT_ID_From_String( char *mdata , BOOL );
int Get_Module_ID_From_String( int mode , char *mdlstr , int badch );
	// 0 : All
	// 1 : All + CTC
	// 2 : C , P , B , CTC
	// 3 : B , IC
//==============================================================================
void FILE_SUB_CHANGE_STRING_TO_SPACE( char *data ) {
	int i , l;
	l = strlen( data );
	for ( i = 0 ; i < l ; i++ ) {
		if ( data[i] <= 30 ) data[i] = ' ';
	}
}

BOOL FILE_SUB_CHECK_MODULE_STRING_ALL( char *data , int *index ) {
	int i;
	i = strlen( data );
	if ( data[i-1] == ')' ) data[i-1] = 0;
	*index = Get_Module_ID_From_String( 0 , data , -1 );
	if ( *index < 0 ) return FALSE;
	return TRUE;
}

BOOL FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( char *data , int *index ) {
	int i;
	i = strlen( data );
	if ( data[i-1] == ')' ) data[i-1] = 0;
	*index = Get_LOT_ID_From_String( data , FALSE ) + CM1;
	if ( *index < 0 ) return FALSE;
	return TRUE;
}

BOOL FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( char *data , int *index , int MAXDATA ) {
	int i;
	i = strlen( data );
	if ( data[i-1] == ')' ) data[i-1] = 0;
	*index = atoi( data ) - 1;
	if ( *index >= 0 ) {
		if ( *index < MAXDATA ) return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------------
// Function = FILE_SYSTEM_SETUP
//-------------------------------------------------------------------------
int FILE_SYSTEM_SETUP_FOR_GET_ALG( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[64];
	int  alg = 0;
	int  ReadCnt;
	BOOL FileEnd = TRUE;

	//-----------------------------------------------------------------------------
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return -1;
	//-----------------------------------------------------------------------------
	while ( FileEnd ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		//
		if ( ReadCnt < 2 ) continue;
		//
		if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) break;
		//
		if      ( STRCMP_L( chr_return , "SYSTEM_ALGORITHM"					) ) {
		}
		else if ( STRCMP_L( chr_return , "SYSTEM_MODE"						) ) {
		}
		else {
			continue;
		}
		//
		if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) break;
		//
		alg = atoi( chr_return );
		//
	}
	//
	_lclose( hFile );
	//
	return alg;
}


//-------------------------------------------------------------------------
// Function = FILE_SYSTEM_SETUP
//-------------------------------------------------------------------------

int GetRealDispFunction( int ch , int *Slot ); // 2016.04.26
BOOL SLOTPROCESSING = FALSE; // 2016.04.26

//
BOOL SCHFILE_USE_END_DELIMITER = FALSE; // 2017.02.09
//
int  SCHFILE_USE_END_DELIMITER_ALARM = 0; // 2017.11.02
//

BOOL FILE_SYSTEM_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[64];
	int group , index , i , j , k , o , m;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;
//	BOOL SLOTPROCESSING = TRUE; // 2016.04.26

	//-----------------------------------------------------------------------------
	// Default Session
	//-----------------------------------------------------------------------------
	// Begin
	//
	_i_SCH_PRM_SET_DFIX_METHOD_NAME( "WAFER" );
	_i_SCH_PRM_SET_DFIM_MAX_MODULE_GROUP( 0 );
	_i_SCH_PRM_SET_DFIX_CARR_AUTO_HANDLER( "" );
	//
	for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) {
		_i_SCH_PRM_SET_MODULE_PROCESS_NAME( Line , "" );
		//
		_i_SCH_PRM_SET_MODULE_COMMSTATUS_IO_NAME( Line , "" ); // 2003.09.03
		//
		sprintf( Buffer , "SCHEDULER_PROCESS_PM%d" , Line + 1 );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_DFIX_PROCESS_FUNCTION_NAME( i , Line , Buffer );
		//
		sprintf( Buffer , "PM%d.PROCESS_LOG" , Line + 1 );
		_i_SCH_PRM_SET_DFIX_PROCESS_LOG_FUNCTION_NAME( Line , Buffer );
		//
		_i_SCH_PRM_SET_STATION_PICK_HANDLING( Line , TRUE );
		_i_SCH_PRM_SET_STATION_PLACE_HANDLING( Line , TRUE );

		_i_SCH_PRM_SET_MODULE_MODE( Line , FALSE );
		_i_SCH_PRM_SET_MODULE_SIZE( Line , 1 );
		_i_SCH_PRM_SET_MODULE_SIZE_CHANGE( Line , FALSE );
		//
		_i_SCH_PRM_SET_MODULE_GROUP( Line , 0 );
		_i_SCH_PRM_SET_MODULE_INTERFACE( Line , 0 );
		//
		_i_SCH_PRM_SET_MODULE_STOCK( Line , FALSE ); // 2006.01.05
		//
		for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH + 2 ) ; i++ ) _i_SCH_PRM_SET_MODULE_PICK_GROUP( i , Line , -1 ); // 2006.01.04
		for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH + 2 ) ; i++ ) _i_SCH_PRM_SET_MODULE_PLACE_GROUP( i , Line , -1 ); // 2006.01.04
		//
		_i_SCH_PRM_SET_MODULE_TMS_GROUP( Line , -1 ); // 2013.03.19
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , Line , BUFFER_OUT_MODE );
		//
		_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( Line , TRUE );
		//
		_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( Line , 0 ); // 2003.06.02
		//
		_i_SCH_PRM_SET_MODULE_MODE_for_SW( Line , 0 );
		_i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( Line , FALSE );
		//
	}
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		_i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( i , 0 );
	}
	//
	_i_SCH_PRM_SET_DFIX_MAX_FINGER_TM( 2 );
	//
//	_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 0 ); // 2013.02.06
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2013.02.06
		_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( i , 0 );
	}
	//
	_i_SCH_PRM_SET_DFIX_FM_DOUBLE_CONTROL( 0 ); // 2007.06.21

	_i_SCH_PRM_SET_DFIX_CASSETTE_READ_MODE( 0 ); // 2006.02.16
	_i_SCH_PRM_SET_DFIX_CASSETTE_ABSENT_RUN_DATA( 0 ); // 2010.03.10
	//
	_i_SCH_PRM_SET_DFIX_MAX_CASSETTE_SLOT( 25 );
	//
	_i_SCH_PRM_SET_DFIX_IOINTLKS_USE( FALSE ); // 2007.01.23
	//
	_i_SCH_PRM_SET_DFIX_CHAMBER_INTLKS_IOUSE( FALSE ); // 2013.11.21
	//
	_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 0 ); // 2008.10.15
	//
	_i_SCH_PRM_SET_DUMMY_PROCESS_CHAMBER( 0 );
	_i_SCH_PRM_SET_DUMMY_PROCESS_SLOT( 1 );
	//
	_i_SCH_PRM_SET_MODULE_DUMMY_BUFFER_CHAMBER( 0 );
	_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( 0 );
	//
	_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 0 ); // 2003.06.11
	_i_SCH_PRM_SET_MODULE_COMMSTATUS2_MODE( 0 ); // 2012.07.21
	_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 0 ); // 2003.06.25
	//
	for ( i = 0 ; i < MAX_SDUMMY_DEPTH ; i++ ) { // 2002.05.09
		_i_SCH_PRM_SET_SDUMMY_PROCESS_CHAMBER( i , 0 );
		_i_SCH_PRM_SET_SDUMMY_PROCESS_SLOT( i , 1 );
	}
	//
	_i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 0 );
	_i_SCH_PRM_SET_DFIX_FAL_MULTI_FNAME( "" );
	//
	_i_SCH_PRM_SET_DFIX_FIC_MULTI_CONTROL( 0 );
	_i_SCH_PRM_SET_DFIX_FIC_MULTI_FNAME( "" );
	//
//	_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_BATCH_ALL ); // 2003.11.07 // 2013.03.18
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2013.03.18
		_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( i , 0 );
	}
	//
	SCHMULTI_CTJOB_USE_SET( FALSE );
	//
	_i_SCH_PRM_SET_MULTI_GROUP_AUTO_ARRANGE( 0 ); // 2006.02.08
	//
	_i_SCH_PRM_SET_LOT_END_ESTIMATION( 0 ); // 2006.08.11
	//
	_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 0 ); // 2006.05.04
	//
	_i_SCH_PRM_SET_PICKPLACE_CANCEL_MESSAGE( 0 ); // 2008.02.01
	//
	_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
	//
	_i_SCH_PRM_SET_PROCESS_MESSAGE_STYLE( 0 ); // 2010.09.02
	//
	_i_SCH_PRM_SET_WAFER_ANIM_STATUS_CHECK( 0 ); // 2010.09.13
	//
	_i_SCH_PRM_SET_MAINT_MESSAGE_STYLE( 0 ); // 2010.09.21
	//
	_i_SCH_PRM_SET_EIL_INTERFACE( 0 ); // 2010.09.13
	_i_SCH_PRM_SET_EIL_INTERFACE_JOBLOG( "" ); // 2011.06.20
	for ( i = 0 ; i < 256 ; i++ ) _i_SCH_PRM_SET_EIL_INTERFACE_FLOW_USER_OPTION( i , -1 ); // 2012.09.03
	//
	for ( i = 0 ; i < 256 ; i++ ) {
		_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTION( i , 0 ); // 2012.12.05
		_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( i , 0 ); // 2013.08.01
	}
	//
	_i_SCH_PRM_SET_DUMMY_MESSAGE( 0 ); // 2010.09.13
	//
	_i_SCH_PRM_SET_MTLOUT_INTERFACE( 0 ); // 2011.05.20
	//
	PROCESS_DATALOG_Init_BUFFER_SIZE(); // 2007.10.23
	//
	SCHMULTI_RCPTUNE_EXTEND_USE_SET( 0 );
	//
	_i_SCH_PRM_SET_MESSAGE_MAINTINTERFACE( 0 ); // 2013.04.25
	//
	_i_SCH_PRM_SET_USE_EXTEND_OPTION( 0 ); // 2013.06.20
	//
	_i_SCH_PRM_SET_OPTIMIZE_MODE( 0 ); // 2017.12.07
	//
	/*
	// 2012.09.21
	_i_SCH_PRM_SET_MODULE_PICK_ORDERING_USE( 0 ); // 2010.10.15
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _i_SCH_PRM_SET_MODULE_PICK_ORDERING( i , 0 );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) _i_SCH_PRM_SET_MODULE_PICK_ORDERING_SKIP( i , 0 ); // 2012.08.23
	*/
	// End
	//-----------------------------------------------------------------------------
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , Filename );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt > 0 ) {

			if ( ReadCnt < 2 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Parameter Error[%s] - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 110 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			group = -1;
			index = -1;
			if      ( STRCMP_L( chr_return , "DESCRIPTION"           ) ) group = 0;									// M6.1.1
			else if ( STRCMP_L( chr_return , "DATE"                  ) ) group = 0;									// M6.1.1
			else if ( STRCMP_L( chr_return , "VERSION"               ) ) group = 0;									// M6.1.1
			else if ( STRCMP_L( chr_return , "REVISION"              ) ) group = 0;									// M6.1.1
			else if ( STRCMP_L( chr_return , "INITIAL"               ) ) group = 0;									// M6.1.1

			else if ( STRNCMP_L( chr_return , "Module_Mode(" , 12     ) ) {											// M6.1.2
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 12 , &index ) ) group = 1;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Service_Mode(" , 13    ) ) {											// M6.1.3
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 13 , &index ) ) group = 2;
				else                                                      group = 0;
			}
			if      ( STRCMP_L( chr_return , "STATION_HANDLING"           ) ) { group = 11; }						// M6.1.11
			else if ( STRCMP_L( chr_return , "STATION_PICK_HANDLING"      ) ) { group = 12; }						// M6.1.11
			else if ( STRCMP_L( chr_return , "STATION_PLACE_HANDLING"     ) ) { group = 13; }						// M6.1.11
			else if ( STRNCMP_L( chr_return , "STATION_HANDLING(" , 17   ) ) {										// M6.1.11
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 17 , &index ) ) group = 14;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "STATION_PICK_HANDLING(" , 22   ) ) {									// M6.1.11
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 22 , &index ) ) group = 15;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "STATION_PLACE_HANDLING(" , 23   ) ) {								// M6.1.11
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 23 , &index ) ) group = 16;
				else                                                   group = 0;
			}
			if      ( STRCMP_L( chr_return , "ROBOT_ROTATE_WHEN_PREPARE"     ) ) { group = 21; }					// M6.1.12
			else if ( STRNCMP_L( chr_return , "ROBOT_ROTATE_WHEN_PREPARE(" , 26   ) ) {								// M6.1.12
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 26 , &index ) ) group = 22;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "ANIMATION_ROTATE_PREPARE"     ) ) { group = 21; }						// M6.1.12
			else if ( STRNCMP_L( chr_return , "ANIMATION_ROTATE_PREPARE(" , 25   ) ) {								// M6.1.12
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 25 , &index ) ) group = 22;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Module_Log_Name(" , 16   ) ) {										// M6.1.13
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 16 , &index ) ) group = 30;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Module_Name(" , 12   ) ) {											// M6.1.14
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 12 , &index ) ) group = 31;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_Log_Function(" , 21   ) ) {									// M6.1.15
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 21 , &index ) ) group = 32;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "Process_Log_Time" ) ) {												// M6.1.16
				group = 33;
			}
			else if ( STRCMP_L( chr_return , "Process_Log_Mode" ) ) {												// M6.1.17
				group = 34;
			}
			else if ( STRNCMP_L( chr_return , "Module_CommStatus_io(" , 21   ) ) { // 2003.09.03					// M6.1.27
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 21 , &index ) ) group = 35;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C1_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 41;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C2_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 42;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C3_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 43;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C4_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 44;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C5_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 45;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C6_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 46;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C7_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 47;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Process_C8_Function(" , 20   ) ) {									// M6.1.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 48;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "Module_SW_Control"				) ) group = 101;					// M6.1.2
			else if ( STRCMP_L( chr_return , "MAX_CASSETTE_SLOT"				) ) group = 102;					// M6.1.4
			else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE_PARALLEL_SUPPORT"	) ) group = 103;					// M6.1.5
			else if ( STRCMP_L( chr_return , "CONTROL_RECIPE_TYPE"				) ) group = 104;					// M6.1.6
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_CASSETTE_IN"		) ) group = 105;					// M6.1.7
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_CASSETTE_OUT"		) ) group = 106;					// M6.1.8
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_SLOT_START"			) ) group = 107;					// M6.1.9
			else if ( STRCMP_L( chr_return , "CONTROL_FIXED_SLOT_END"			) ) group = 108;					// M6.1.10
			else if ( STRCMP_L( chr_return , "SYSTEM_ALGORITHM"					) ) group = 109;					// M6.1.19
			else if ( STRCMP_L( chr_return , "SYSTEM_MODE"						) ) group = 109;					// M6.1.19
			else if ( STRCMP_L( chr_return , "DUMMY_PROCESS"					) ) group = 110;					// M6.1.20
			else if ( STRCMP_L( chr_return , "METHOD_NAME"						) ) group = 111;					// M6.1.22
			else if ( STRCMP_L( chr_return , "SYSTEM_SUB_ALGORITHM"				) ) group = 112; // 2005.05.18		// M6.1.31
			else if ( STRCMP_L( chr_return , "SYSTEM_SUB_MODE"					) ) group = 112; // 2005.05.18		// M6.1.31
			else if ( STRCMP_L( chr_return , "CASSETTE_GROUP_READING"			) ) group = 113; // 2006.02.16		// M6.1.32
			else if ( STRCMP_L( chr_return , "GROUP"							) ) group = 200;					// M6.1.23
			else if ( STRCMP_L( chr_return , "GROUP(G)"							) ) group = 201;
			else if ( STRCMP_L( chr_return , "GROUP(P)"							) ) group = 202;
			else if ( STRCMP_L( chr_return , "GROUP(GX)"						) ) group = 203;
			else if ( STRCMP_L( chr_return , "GROUP(PX)"						) ) group = 204;
			else if ( STRCMP_L( chr_return , "GROUP(T)"							) ) group = 205; // 2013.03.19
			//
/*
// 2012.09.21
			else if ( STRCMP_L( chr_return , "PICK_ORDERING_L"					) ) group = 205; // 2010.10.15
			else if ( STRCMP_L( chr_return , "PICK_ORDERING_F"					) ) group = 206; // 2010.10.15
			else if ( STRCMP_L( chr_return , "PICK_ORDERING_SKIP"				) ) group = 207; // 2012.08.23
*/
			//
			else if ( STRCMP_L( chr_return , "CONTROLPROCESS_JOB"				) ) group = 300;					// M6.1.24
			else if ( STRCMP_L( chr_return , "CARRIER_AUTO_MONITOR"				) ) group = 301;					// M6.1.25

			else if ( STRCMP_L( chr_return , "Module_CommStatus"				) ) group = 302; // 2003.06.11		// M6.1.26
			else if ( STRCMP_L( chr_return , "TRANSFER_CONTROL_MODE"			) ) group = 303; // 2003.06.25		// M6.1.28

			else if ( STRCMP_L( chr_return , "LOT_END_DELAY_TIME"				) ) group = 304; // 2003.08.04		// M6.1.29

			else if ( STRCMP_L( chr_return , "PRJOB_END_DELAY_TIME"				) ) group = 305; // 2005.04.06		// M6.1.30

			else if ( STRCMP_L( chr_return , "MULTI_GROUP_AUTO_ARRANGE"			) ) group = 306; // 2006.02.08		// M6.1.33

			else if ( STRCMP_L( chr_return , "PROCESS_ZONE_RUN_TYPE"			) ) group = 307; // 2006.05.04		// M6.1.34

			else if ( STRCMP_L( chr_return , "LOT_END_ESTIMATION_TIME"			) ) group = 308; // 2006.08.11		// M6.1.35

			else if ( STRCMP_L( chr_return , "IO_INTLRLOCK_FOR_PICKPLACE"		) ) group = 309; // 2007.01.23
			else if ( STRCMP_L( chr_return , "IO_INTERLOCK_FOR_PICKPLACE"		) ) group = 309; // 2013.07.26

			else if ( STRNCMP_L( chr_return , "PROCESS_DATALOG_BUFFER_SIZE(" , 28 ) ) { // 2007.10.23
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 28 , &index ) ) {
					group = 310; // 2007.10.23
				}
				else {
					index = -1; // 2007.10.23
					group = 310; // 2007.10.23
				}
			}

			else if ( STRCMP_L( chr_return , "JOB_EXTEND_TUNE_BUFFER_USE"      ) ) group = 311; // 2007.11.28

			else if ( STRCMP_L( chr_return , "PICKPLACE_CANCEL_MESSAGE_USE"    ) ) group = 312; // 2008.02.01

			else if ( STRCMP_L( chr_return , "TIMER_RESET_WHEN_NEW_START"      ) ) group = 313; // 2008.04.17

			else if ( STRCMP_L( chr_return , "CHECK_DIFFERENT_METHOD"          ) ) group = 314; // 2008.07.09

			else if ( STRCMP_L( chr_return , "CHECK_DIFFERENT_METHOD2"         ) ) group = 1314; // 2018.08.22

			else if ( STRCMP_L( chr_return , "LOT_LOG_MODE"				       ) ) group = 315; // 2008.10.15

			else if ( STRCMP_L( chr_return , "PROCESS_MESSAGE_STYLE"	       ) ) group = 316; // 2010.09.02

			else if ( STRCMP_L( chr_return , "WAFER_ANIM_STATUS_CHECK"	       ) ) group = 317; // 2010.09.13

			else if ( STRCMP_L( chr_return , "MAINT_MESSAGE_STYLE"			   ) ) group = 318; // 2010.09.21

			else if ( STRCMP_L( chr_return , "EIL_INTERFACE"			       ) ) group = 319; // 2010.09.13

			else if ( STRCMP_L( chr_return , "DUMMY_MESSAGE"			       ) ) group = 320; // 2010.11.30

			else if ( STRCMP_L( chr_return , "MTLOUT_INTERFACE"			       ) ) group = 321; // 2011.05.20
			else if ( STRCMP_L( chr_return , "MTRLOUT_INTERFACE"		       ) ) group = 321; // 2011.05.20

			else if ( STRNCMP_L( chr_return , "EIL_INTERFACE_FLOW_USER_OPTION(" , 31 ) ) { // 2012.09.03
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 31 , &index , 256 ) ) group = 322;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MFI_INTERFACE_FLOW_USER_OPTION(" , 31 ) ) { // 2012.12.05
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 31 , &index , 255 ) ) group = 323;
				else                                                   group = 0;
			}

			else if ( STRCMP_L( chr_return , "CHAMBER_INTLRLOCK_IO_USE"		   ) ) group = 324; // 2013.11.21
			else if ( STRCMP_L( chr_return , "CHAMBER_INTERLOCK_IO_USE"		   ) ) group = 324; // 2013.11.21

			else if ( STRNCMP_L( chr_return , "SDUMMY_PROCESS(" , 15   ) ) { // 2002.05.09							// M6.1.21
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 15 , &index , MAX_SDUMMY_DEPTH ) ) group = 400;
				else                                                   group = 0;
			}

			else if ( STRCMP_L( chr_return , "MAX_FINGER_TM"			       ) ) group = 500;

			else if ( STRCMP_L( chr_return , "ALARM_ANALYSIS"			       ) ) group = 600; // 2012.02.01 

			else if ( STRCMP_L( chr_return , "MESSAGE_MAINTINTERFACE"          ) ) group = 601; // 2013.04.25

			else if ( STRCMP_L( chr_return , "USE_EXTEND_OPTION"               ) ) group = 602; // 2013.06.20

			else if ( STRCMP_L( chr_return , "SLOT_PROCESSING"                 ) ) group = 603; // 2016.04.26

			else if ( STRCMP_L( chr_return , "END_DELIMITER_CHECK"             ) ) group = 701; // 2017.02.09

			else if ( STRCMP_L( chr_return , "OPTIMIZE_MODE"                   ) ) group = 702; // 2017.12.07

			else if ( STRCMP_L( chr_return , "EXTEND_SOURCE_USE"               ) ) group = 703; // 2018.09.05

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 120 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			switch ( group ) {
			case 0 :
				break;
			case 1 :
				if      ( STRCMP_L( chr_return , "On"  ) ) {
					_i_SCH_PRM_SET_MODULE_MODE( index , TRUE );
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 1 );
					//==========================================================================
					if ( index >= PM1 && index < AL ) {
						if ( _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() < ( index - PM1 + 1 ) ) {
							_i_SCH_PRM_SET_DFIM_MAX_PM_COUNT( index - PM1 + 1 );
						}
					}
					//==========================================================================
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					_i_SCH_PRM_SET_MODULE_MODE( index , FALSE );
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 0 );
				}
				else group = -1;
				//
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 130 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( index == F_AL ) || ( index == AL ) ) {
						if ( STRCMP_L( chr_return , "2" ) ) {
							_i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 1 );
						}
						else {
							_i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 3 );
							_i_SCH_PRM_SET_DFIX_FAL_MULTI_FNAME( chr_return );
						}
						//===================================================================
						// 2008.08.29
						//===================================================================
						if ( ReadCnt > 3 ) {
							if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 140 - [%s]-[%d]\n" , Filename , Line );
								_lclose( hFile );
								return FALSE;
							}
							i = atoi( chr_return );
							if ( i <= 0 ) i = 1;
							_i_SCH_PRM_SET_MODULE_SIZE( F_AL , i );
						}
						//===================================================================
					}
					else if ( ( index == F_IC ) || ( index == IC ) ) {
						//======================================================================================
						_i_SCH_PRM_SET_DFIX_FIC_MULTI_CONTROL( 1 );
						//==============================================================================
						// 2005.09.06
						//==============================================================================
						if ( STRCMP_L( chr_return , "F_AL" ) || STRCMP_L( chr_return , "FAL" ) || STRCMP_L( chr_return , "AL" ) ) { // 2010.12.22
							_i_SCH_PRM_SET_DFIX_FIC_MULTI_CONTROL( 2 );
							_i_SCH_PRM_SET_MODULE_SIZE( F_IC , 1 );
						}
						else {
							if ( ( chr_return[0] >= '0' ) && ( chr_return[0] <= '9' ) ) {
								i = atoi( chr_return );
								//--------------------------------------------
								if      ( i <= 0 )
									_i_SCH_PRM_SET_MODULE_SIZE( F_IC , 1 );
	//							else if ( i > MAX_CASSETTE_SLOT_SIZE ) // 2007.06.21
	//								_i_SCH_PRM_SET_MODULE_SIZE( F_IC , MAX_CASSETTE_SLOT_SIZE ); // 2007.06.21
								else if ( i > ( MAX_CASSETTE_SLOT_SIZE * 2 ) ) // 2007.06.21
									_i_SCH_PRM_SET_MODULE_SIZE( F_IC , MAX_CASSETTE_SLOT_SIZE * 2 ); // 2007.06.21
								else
									_i_SCH_PRM_SET_MODULE_SIZE( F_IC , i );
								//--------------------------------------------
								_i_SCH_PRM_SET_DFIX_FIC_MULTI_FNAME( "" );
							}
							else {
								//--------------------------------------------
								_i_SCH_PRM_SET_MODULE_SIZE( F_IC , 1 );
								//--------------------------------------------
								_i_SCH_PRM_SET_DFIX_FIC_MULTI_FNAME( chr_return );
								//--------------------------------------------
							}
						}
					}
					else {
//						i = atoi( chr_return ); // 2007.10.07
//						if ( i <= 0 ) i = 1; // 2007.10.07
//						_i_SCH_PRM_SET_MODULE_SIZE( index , i ); // 2007.10.07
						//======================================================================================
						/*
						// 2013.02.06
						if ( index == TM ) {
//							if ( i > 1 ) _i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 1 ); // 2007.10.07
//							else		 _i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 0 ); // 2007.10.07
							//
							// 2007.10.07
							//
							if      ( STRCMP_L( chr_return , "2" ) ) {  // PM OneBody
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 1 );
							}
							else if ( STRCMP_L( chr_return , "2N" ) ) { // PM OneBody and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 2 );
							}
							else if ( STRCMP_L( chr_return , "2C" ) ) { // PM OneBody and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 3 );
							}
							else if ( STRCMP_L( chr_return , "2M" ) ) { // PM OneBody But Message Separate
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 4 );
							}
							else if ( STRCMP_L( chr_return , "2MN" ) ) { // PM OneBody But Message Separate and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 5 );
							}
							else if ( STRCMP_L( chr_return , "2MC" ) ) { // PM OneBody But Message Separate and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 6 );
							}
							else if ( STRCMP_L( chr_return , "2S" ) ) { // PM DiffBody
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 7 );
							}
							else if ( STRCMP_L( chr_return , "2SN" ) ) { // PM DiffBody and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 8 );
							}
							else if ( STRCMP_L( chr_return , "2SC" ) ) { // PM DiffBody and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 9 );
							}
							else {
								_i_SCH_PRM_SET_MODULE_SIZE( index , 1 );
								_i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( 0 );
							}
						}
						*/
						//
						// DATA		CONFIG	SEP		ONEBODY	DIFFBODY	TM Next Only(NT)	MESSAGE
						//												  /Odd,Even(OE)		SEPARATE
						//
						//	0		""		O		-		-			-					-
						//
						//	1		"2"		X		O		X			-					X					
						//	2		"2N"	X		O		X			NT					X
						//	3		"2C"	X		O		X			  OE				X
						//	4		"2M"	X		O		X			-					O
						//	5		"2MN"	X		O		X			NT					O
						//	6		"2MC"	X		O		X			  OE				O
						//
						//	7		"2S"	X		X		O			-					O
						//	8		"2SN"	X		X		O			NT					O
						//	9		"2SC"	X		X		O			  OE				O
						//
						//	10		"2D"	X		O		O			-					O
						//	11		"2DN"	X		O		O			NT					O
						//	12		"2DC"	X		O		O			  OE				O
						//
						//	TM Next Only(NT): TM에 BM에서 Pick할때 연속된 Slot일 경우만 2장을 Pick 할 수 있음
						//	TM Odd/Even(OE)	: BM에서 1,3 Slot , 2,4 Slot이 한 쌍으로 Pick,Place 
						//
						//
						//
						if ( ( index >= TM ) && ( index < FM ) ) { // 2013.02.06
							if      ( STRCMP_L( chr_return , "2" ) ) {  // PM OneBody
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 1 );
							}
							else if ( STRCMP_L( chr_return , "2N" ) ) { // PM OneBody and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 2 );
							}
							else if ( STRCMP_L( chr_return , "2C" ) ) { // PM OneBody and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 3 );
							}
							else if ( STRCMP_L( chr_return , "2M" ) ) { // PM OneBody But Message Separate
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 4 );
							}
							else if ( STRCMP_L( chr_return , "2MN" ) ) { // PM OneBody But Message Separate and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 5 );
							}
							else if ( STRCMP_L( chr_return , "2MC" ) ) { // PM OneBody But Message Separate and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 6 );
							}
							else if ( STRCMP_L( chr_return , "2S" ) ) { // PM DiffBody
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 7 );
							}
							else if ( STRCMP_L( chr_return , "2SN" ) ) { // PM DiffBody and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 8 );
							}
							else if ( STRCMP_L( chr_return , "2SC" ) ) { // PM DiffBody and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 9 );
							}
							else if ( STRCMP_L( chr_return , "2D" ) ) { // PM DiffBody,FixSep
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 10 );
							}
							else if ( STRCMP_L( chr_return , "2DN" ) ) { // PM DiffBody,FixSep and TM Next Only
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 11 );
							}
							else if ( STRCMP_L( chr_return , "2DC" ) ) { // PM DiffBody,FixSep and TM Odd/Even
								_i_SCH_PRM_SET_MODULE_SIZE( index , 2 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 12 );
							}
							else {
								_i_SCH_PRM_SET_MODULE_SIZE( index , 1 );
								_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( index - TM , 0 );
							}
						}
						else if ( index == FM ) { // 2007.06.21
							i = atoi( chr_return );
							if ( i <= 0 ) i = 1;
							_i_SCH_PRM_SET_MODULE_SIZE( index , i );
							//
							if ( i > 1 ) _i_SCH_PRM_SET_DFIX_FM_DOUBLE_CONTROL( i - 1 );
							else		 _i_SCH_PRM_SET_DFIX_FM_DOUBLE_CONTROL( 0 );
						}
						else {
							i = atoi( chr_return ); // 2007.10.07
							if ( i <= 0 ) i = 1; // 2007.10.07
							_i_SCH_PRM_SET_MODULE_SIZE( index , i ); // 2007.10.07
						}
					}
					//======================================================================================
					//======================================================================================
					//======================================================================================
				}
				if ( ReadCnt > 3 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 150 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( index >= BM1 ) && ( index < TM ) ) { // 2006.01.05
						//=======================================================================
						if      ( STRNCMP_L( chr_return , "In" , 2 ) ) {
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_IN_MODE );
							j = 2;
						}
						else if ( STRNCMP_L( chr_return , "Out" , 3 ) ) {
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_OUT_MODE );
							j = 3;
						}
						else if ( STRNCMP_L( chr_return , "Switch" , 6 ) ) {
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_SWITCH_MODE );
							j = 6;
						}
						else if ( STRNCMP_L( chr_return , "Cassette" , 8 ) ) { // 2002.07.05
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_OUT_CASSETTE );
							j = 8;
						}
						else if ( STRNCMP_L( chr_return , "Dummy" , 5 ) ) { // 2002.09.12
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_OTHER_STYLE ); // 2007.03.20
							j = 5;
							//----------------------------------
							if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == 0 ) {
								_i_SCH_PRM_SET_MODULE_DUMMY_BUFFER_CHAMBER( index );
							}
							else { // 2009.02.12
								if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() > index ) {
									_i_SCH_PRM_SET_MODULE_DUMMY_BUFFER_CHAMBER( index );
								}
							}
							//----------------------------------
						}
						else if ( STRNCMP_L( chr_return , "Align" , 5 ) ) { // 2007.01.08
							for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ )	_i_SCH_PRM_SET_MODULE_BUFFER_MODE( i , index , BUFFER_OTHER_STYLE2 ); // 2007.03.20
							j = 5;
							//----------------------------------
							_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( index );
						}
						else {
							group = -1;
							j = 0;
						}
						//=======================================================================
						if      ( STRCMP_L( chr_return + j , "(F)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 1 );
						}
						else if ( STRCMP_L( chr_return + j , "(L)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 2 );
						}
						else if ( STRCMP_L( chr_return + j , "(FL)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 3 );
						}
						else if ( STRCMP_L( chr_return + j , "(LF)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 4 );
						}
						else if ( STRCMP_L( chr_return + j , "(F2)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 5 );
						}
						else if ( STRCMP_L( chr_return + j , "(L2)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 6 );
						}
						else if ( STRCMP_L( chr_return + j , "(FL2)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 7 );
						}
						else if ( STRCMP_L( chr_return + j , "(LF2)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 8 );
						}
						else {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( index , 0 );
						}
						//============================================================================
					}
					else if ( ( index >= PM1 ) && ( index < AL ) ) { // 2006.01.05
						if      ( STRCMP_L( chr_return , "BUFFER"  ) ) {
							_i_SCH_PRM_SET_MODULE_STOCK( index , TRUE );
						}
						else if ( STRCMP_L( chr_return , "STOCK" ) ) {
							_i_SCH_PRM_SET_MODULE_STOCK( index , TRUE );
						}
						else if ( STRCMP_L( chr_return , "NORMAL" ) ) {
							_i_SCH_PRM_SET_MODULE_STOCK( index , FALSE );
						}
						else group = -1;
					}
					//===========================================================================
					// 2007.01.22
					//===========================================================================
					else if ( index == F_IC ) {
						if ( STRCMP_L( chr_return , "Align" ) ) {
							_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( index );
						}
					}
					//===========================================================================
				}
				if ( ReadCnt > 4 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 4 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 160 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( index >= BM1 ) && ( index < TM ) ) { // 2006.01.05
						if      ( STRCMP_L( chr_return , "Immediately"  ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , FALSE );
						}
						else if ( STRCMP_L( chr_return , "Direct"  ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , FALSE );
						}
						else if ( STRCMP_L( chr_return , "Full" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , TRUE );
						}
						else if ( STRCMP_L( chr_return , "Full(FS)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , TRUE );
							//
//							if ( _i_SCH_PRM_GET_MODULE_SIZE( index ) <= 1 ) { // 2011.06.27 // 2018.11.27
////								_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_SINGLESWAP ); // 2013.03.18 // 2018.11.27
//								_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( _i_SCH_PRM_GET_MODULE_GROUP( index ) , BUFFER_SWITCH_SINGLESWAP ); // 2013.03.18 // 2018.11.27
//							} // 2018.11.27
//							else { // 2018.11.27
//								_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_FULLSWAP ); // 2003.11.07 // 2013.03.18
								_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( _i_SCH_PRM_GET_MODULE_GROUP( index ) , BUFFER_SWITCH_FULLSWAP ); // 2003.11.07 // 2013.03.18
//							} // 2018.11.27
						}
						else if ( STRCMP_L( chr_return , "Full(MS)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , TRUE );
							//
							if ( _i_SCH_PRM_GET_MODULE_SIZE( index ) <= 1 ) { // 2011.06.27
//								_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_SINGLESWAP ); // 2013.03.18
								_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( _i_SCH_PRM_GET_MODULE_GROUP( index ) , BUFFER_SWITCH_SINGLESWAP ); // 2013.03.18
							}
							else {
//								_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_MIDDLESWAP ); // 2006.02.02 // 2013.03.18
								_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( _i_SCH_PRM_GET_MODULE_GROUP( index ) , BUFFER_SWITCH_MIDDLESWAP ); // 2006.02.02 // 2013.03.18
							}
						}
						else if ( STRCMP_L( chr_return , "Full(1S)" ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_FULLRUN( index , TRUE );
//							_i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( BUFFER_SWITCH_SINGLESWAP ); // 2003.11.07 // 2013.03.18
							_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( _i_SCH_PRM_GET_MODULE_GROUP( index ) , BUFFER_SWITCH_SINGLESWAP ); // 2003.11.07 // 2013.03.18
						}
						else group = -1;
					}
				}
				break;

			case 2 :
				if      ( STRCMP_L( chr_return , "Remote" ) ) _i_SCH_PRM_SET_MODULE_SERVICE_MODE( index , TRUE );
				else if ( STRCMP_L( chr_return , "Local"  ) ) _i_SCH_PRM_SET_MODULE_SERVICE_MODE( index , FALSE );
				else group = -1;
				break;

			case 11 :
				if ( STRCMP_L( chr_return , "Normal"  ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( i , TRUE );
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( i , TRUE );
				}
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) || STRCMP_L( chr_return , "PM_PlacePick" ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( i , FALSE );
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( i , FALSE );
				}
				else group = -1;
				break;

			case 12 :
				if ( STRCMP_L( chr_return , "Normal"  ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( i , TRUE );
				}
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) || STRCMP_L( chr_return , "PM_PlacePick" ) || STRCMP_L( chr_return , "PM_Pick" ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( i , FALSE );
				}
				else group = -1;
				break;

			case 13 :
				if ( STRCMP_L( chr_return , "Normal"  ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( i , TRUE );
				}
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) || STRCMP_L( chr_return , "PM_PlacePick" ) || STRCMP_L( chr_return , "PM_Place" ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( i , FALSE );
				}
				else group = -1;
				break;

			case 14 :
				if      ( STRCMP_L( chr_return , "Normal"       ) ) {
					_i_SCH_PRM_SET_STATION_PICK_HANDLING( index , TRUE );
					_i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , TRUE );
				}
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) ) {
					_i_SCH_PRM_SET_STATION_PICK_HANDLING( index , FALSE );
					_i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , FALSE );
				}
				else if ( STRCMP_L( chr_return , "PM_PlacePick" ) ) {
					_i_SCH_PRM_SET_STATION_PICK_HANDLING( index , FALSE );
					_i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , FALSE );
				}
				else group = -1;
				break;

			case 15 :
				if      ( STRCMP_L( chr_return , "Normal"       ) ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( index , TRUE );
				else if ( STRCMP_L( chr_return , "PM_Pick"      ) ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( index , FALSE );
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( index , FALSE );
				else if ( STRCMP_L( chr_return , "PM_PlacePick" ) ) _i_SCH_PRM_SET_STATION_PICK_HANDLING( index , FALSE );
				else group = -1;
				break;

			case 16 :
				if      ( STRCMP_L( chr_return , "Normal"       ) ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , TRUE );
				else if ( STRCMP_L( chr_return , "PM_Place"     ) ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , FALSE );
				else if ( STRCMP_L( chr_return , "PM_PickPlace" ) ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , FALSE );
				else if ( STRCMP_L( chr_return , "PM_PlacePick" ) ) _i_SCH_PRM_SET_STATION_PLACE_HANDLING( index , FALSE );
				else group = -1;
				break;

			case 21 :
				if ( STRCMP_L( chr_return , "On"  ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( i , TRUE );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( i , FALSE );
				}
				else group = -1;
				break;

			case 22 :
				if      ( STRCMP_L( chr_return , "On"  ) ) { _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( index , TRUE );  }
				else if ( STRCMP_L( chr_return , "Off" ) ) { _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( index , FALSE ); }
				else group = -1;
				break;

			case 30 :
				_i_SCH_SYSTEM_SET_MODULE_NAME( index , chr_return );
				break;

			case 31 :
				_i_SCH_PRM_SET_MODULE_PROCESS_NAME( index , chr_return );
				break;

			case 32 :
				if ( index >= PM1 && index < AL )
					_i_SCH_PRM_SET_DFIX_PROCESS_LOG_FUNCTION_NAME( index - PM1 , chr_return );
				break;

			case 33 :
				index = atoi( chr_return );
				if      ( index < 250  ) PROCESS_MONITOR_Set_MONITORING_TIME( 250 );
				else if ( index > 3000 ) PROCESS_MONITOR_Set_MONITORING_TIME( 3000 );
				else                     PROCESS_MONITOR_Set_MONITORING_TIME( index );
				break;

			case 34 :

				if      ( STRCMP_L( chr_return , "Full"  ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 0 );
				}
				else if ( STRCMP_L( chr_return , "Min" ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 1 );
				}
				else if ( STRCMP_L( chr_return , "Minimum" ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 1 );
				}
				else if ( STRCMP_L( chr_return , "Full.Msec" ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 2 );
				}
				else if ( STRCMP_L( chr_return , "Min.Msec" ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 3 );
				}
				else if ( STRCMP_L( chr_return , "Minimum.Msec" ) ) {
					PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( 3 );
				}
//				PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( FALSE ); // 2002.06.28 // 2019.04.05
				PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( 0 ); // 2002.06.28 // 2019.04.05
				break;

			case 35 :
				_i_SCH_PRM_SET_MODULE_COMMSTATUS_IO_NAME( index , chr_return ); // 2003.09.03
				break;

			case 41 :
			case 42 :
			case 43 :
			case 44 :
			case 45 :
			case 46 :
			case 47 :
			case 48 :
				i = group - 41;
				if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) break;
				if ( index >= PM1 && index < AL )
					_i_SCH_PRM_SET_DFIX_PROCESS_FUNCTION_NAME( i , index - PM1 , chr_return );
				break;

			case 101 :
				if      ( STRCMP_L( chr_return , "On"  ) ) {
					_i_SCH_PRM_SET_DFIX_MODULE_SW_CONTROL( 1 );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					_i_SCH_PRM_SET_DFIX_MODULE_SW_CONTROL( 0 );
				}
				else if ( STRCMP_L( chr_return , "On2"  ) ) {
					_i_SCH_PRM_SET_DFIX_MODULE_SW_CONTROL( 2 );
				}
				else group = -1;
				break;

			case 102 :
				_i_SCH_PRM_SET_DFIX_MAX_CASSETTE_SLOT( atoi( chr_return ) );
				break;

			case 103 :
				// UnUsed from 2002.10.09
				break;

			case 104 :
				if      ( STRCMP_L( chr_return , "ALL_RECIPE"         ) ) _i_SCH_PRM_SET_DFIX_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_ALL );
				else if ( STRCMP_L( chr_return , "LOT_PROCESS_RECIPE" ) ) _i_SCH_PRM_SET_DFIX_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_LOT_PROCESS );
				else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE"     ) ) _i_SCH_PRM_SET_DFIX_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_CLUSTER );
				else if ( STRCMP_L( chr_return , "PROCESS_RECIPE"     ) ) _i_SCH_PRM_SET_DFIX_CONTROL_RECIPE_TYPE( CONTROL_RECIPE_PROCESS );
				else group = -1;
				break;

			case 105 :
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( chr_return , &index ) ) {
					_i_SCH_PRM_SET_DFIX_CONTROL_FIXED_CASSETTE_IN( index );
				}
				else group = -1;
				break;

			case 106 :
				if ( STRCMP_L( chr_return , "IN" ) || STRCMP_L( chr_return , "SAME" ) ) { // 2019.03.03
					_i_SCH_PRM_SET_DFIX_CONTROL_FIXED_CASSETTE_OUT( -99 );
				}
				else {
					if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( chr_return , &index ) ) {
						_i_SCH_PRM_SET_DFIX_CONTROL_FIXED_CASSETTE_OUT( index );
					}
					else group = -1;
				}
				break;

			case 107 :
				_i_SCH_PRM_SET_DFIX_CONTROL_FIXED_SLOT_START( atoi( chr_return ) );
				break;

			case 108 :
				_i_SCH_PRM_SET_DFIX_CONTROL_FIXED_SLOT_END( atoi( chr_return ) );
				break;

			case 109 :
				k = atoi( chr_return );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						SYSTEM_RUN_ALG_STYLE_SET( k , atoi( chr_return ) );
					}
					else {
						SYSTEM_RUN_ALG_STYLE_SET( k , -1 );
					}
				}
				else {
					SYSTEM_RUN_ALG_STYLE_SET( k , -1 );
				}
				//
				if ( !_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG( SYSTEM_RUN_ALG_STYLE_GET() ) ) {
					_IO_CIM_PRINTF( "_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG Error - [%s]-[%d]\n" , Filename , Line );
					_lclose( hFile );
					return FALSE;
				}
				break;

			case 110 :
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return , &index ) ) {
					if ( ( index >= PM1 ) && ( index < AL ) ) {
						_i_SCH_PRM_SET_DUMMY_PROCESS_CHAMBER( index );
						_i_SCH_PRM_SET_DUMMY_PROCESS_SLOT( 1 );
						if ( ReadCnt > 2 ) {
							if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 170 - [%s]-[%d]\n" , Filename , Line );
								_lclose( hFile );
								return FALSE;
							}
							i = atoi(chr_return);
							if ( i <= 0 ) i = 1;
							_i_SCH_PRM_SET_DUMMY_PROCESS_SLOT( i );
						}
					}
					else {
						_i_SCH_PRM_SET_DUMMY_PROCESS_CHAMBER( 0 );
					}
				}
				else {
					_i_SCH_PRM_SET_DUMMY_PROCESS_CHAMBER( 0 );
				}
				break;

			case 111 :
				_i_SCH_PRM_SET_DFIX_METHOD_NAME( chr_return );
				break;

			case 112 : // 2005.05.18
				SYSTEM_SUB_ALG_STYLE_SET( atoi( chr_return ) );
				break;

			case 113 : // 2006.02.16
				if      ( STRCMP_L( chr_return , "Off"  ) ) {
					_i_SCH_PRM_SET_DFIX_CASSETTE_READ_MODE( 0 );
				}
				else if ( STRCMP_L( chr_return , "On"  ) ) {
					_i_SCH_PRM_SET_DFIX_CASSETTE_READ_MODE( 1 );
				}
				else if ( STRCMP_L( chr_return , "group"  ) ) {
					_i_SCH_PRM_SET_DFIX_CASSETTE_READ_MODE( 1 );
				}
				else if ( STRCMP_L( chr_return , "-"  ) ) { // 2010.03.10
				}
				else {
					group = -1;
				}
				//
				if ( ReadCnt > 2 ) { // 2010.03.08
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 180 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					j = atoi(chr_return);
					if ( j > 4 ) _i_SCH_PRM_SET_DFIX_CASSETTE_ABSENT_RUN_DATA( j );
				}
				break;

			case 200 :
				j = atoi( chr_return );
				//
//				if ( j < 0 ) break; // 2014.09.24
				if ( j < -1 ) break; // 2014.09.24
				//
				if ( j >= MAX_TM_CHAMBER_DEPTH ) break;
				//
				for ( i = 2 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 190 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					k = 0;
					o = 0;
					if ( ( chr_return[0] == 'S' ) && ( chr_return[1] == ':' ) ) {
						o = 2;
						k = 2;
					}
					else if ( ( tolower(chr_return[0]) == 'i' ) && ( tolower(chr_return[1]) == 'n' ) && ( chr_return[2] == ':' ) ) { // 2018.07.21
						o = 11;
						k = 3;
					}
					else if ( ( tolower(chr_return[0]) == 'o' ) && ( tolower(chr_return[1]) == 'u' ) && ( tolower(chr_return[2]) == 't' ) && ( chr_return[3] == ':' ) ) { // 2018.07.21
						o = 12;
						k = 4;
					}
					else if ( ( tolower(chr_return[0]) == 'i' ) && ( tolower(chr_return[1]) == 'n' ) && ( tolower(chr_return[2]) == 's' ) && ( chr_return[3] == ':' ) ) { // 2018.07.21
						o = 13;
						k = 4;
					}
					else if ( ( tolower(chr_return[0]) == 'o' ) && ( tolower(chr_return[1]) == 'u' ) && ( tolower(chr_return[2]) == 't' ) && ( tolower(chr_return[3]) == 's' ) && ( chr_return[4] == ':' ) ) { // 2018.07.21
						o = 14;
						k = 5;
					}
					else {
						if ( chr_return[2] == ':' ) {
							if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 3 , &m ) ) {
								if ( ( m >= PM1 ) && ( m < BM1 ) ) {
									o = 1;
									k = 0;
									chr_return[2] = 0;
								}
							}
						}
						else if ( chr_return[3] == ':' ) {
							if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 4 , &m ) ) {
								if ( ( m >= PM1 ) && ( m < BM1 ) ) {
									o = 1;
									k = 0;
									chr_return[3] = 0;
								}
							}
						}
						else if ( chr_return[4] == ':' ) {
							if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 5 , &m ) ) {
								if ( ( m >= PM1 ) && ( m < BM1 ) ) {
									o = 1;
									k = 0;
									chr_return[4] = 0;
								}
							}
						}
					}
					if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + k , &index ) ) {
						if ( index < PM1 ) { group = -1; break; }
						if ( index >= TM ) { group = -1; break; }
						if ( index == AL ) { group = -1; break; }
						if ( index == IC ) { group = -1; break; }
						//-------------
						if ( _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() < j ) _i_SCH_PRM_SET_DFIM_MAX_MODULE_GROUP( j );
						//-------------
						if ( index < BM1 ) {
							if ( o == 1 ) {
								_i_SCH_PRM_SET_MODULE_INTERFACE( index , m );
								_i_SCH_PRM_SET_MODULE_GROUP( m , j );
							}
							_i_SCH_PRM_SET_MODULE_GROUP( index , j );
						}
						else {
							//
							if ( j < 0 ) {
								_i_SCH_PRM_SET_MODULE_GROUP( index , j ); // 2018.12.12
//								break; // 2014.09.24 // 2018.12.12
							}
							//
							else {
								//
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( j , index ) == BUFFER_OUT_CASSETTE ) {
									_IO_CIM_PRINTF( "[ERROR] Cassette Buffer Used at Group(%d)\n" , j );
									group = -1;
								}
								else {
									//
									if ( o == 11 ) { // 2018.07.21 in
										_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_IN_MODE );
									}
									else if ( o == 12 ) { // 2018.07.21 out
										_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_OUT_MODE );
									}
									else if ( o == 13 ) { // 2018.07.21 ins
										_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_IN_S_MODE );
									}
									else if ( o == 14 ) { // 2018.07.21 outs
										_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_OUT_S_MODE );
									}
									else {
										//
										_i_SCH_PRM_SET_MODULE_GROUP( index , j );
										//
										if ( o == 2 ) {
											k = _i_SCH_PRM_GET_MODULE_BUFFER_MODE( j , index );
											if      ( k == BUFFER_IN_MODE ) {
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_IN_S_MODE );
												switch( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( j ) ) {
												case GROUP_HAS_SWITCH_NONE : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_INS ); break;
												case GROUP_HAS_SWITCH_INS  : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_INS ); break;
												case GROUP_HAS_SWITCH_OUTS : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_ALL ); break;
												case GROUP_HAS_SWITCH_ALL  : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_ALL ); break;
												}
											}
											else if ( k == BUFFER_OUT_MODE ) {
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j , index , BUFFER_OUT_S_MODE );
												switch( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( j ) ) {
												case GROUP_HAS_SWITCH_NONE : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_OUTS ); break;
												case GROUP_HAS_SWITCH_INS  : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_ALL ); break;
												case GROUP_HAS_SWITCH_OUTS : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_OUTS ); break;
												case GROUP_HAS_SWITCH_ALL  : _i_SCH_PRM_SET_MODULE_GROUP_HAS_SWITCH( j , GROUP_HAS_SWITCH_ALL ); break;
												}
											}
										}
										if ( j > 0 ) {
											k = _i_SCH_PRM_GET_MODULE_BUFFER_MODE( j , index );
											if      ( k == BUFFER_IN_MODE )
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j - 1 , index , BUFFER_OUT_MODE );
											else if ( k == BUFFER_OUT_MODE )
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j - 1 , index , BUFFER_IN_MODE );
											else if ( k == BUFFER_IN_S_MODE )
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j - 1 , index , BUFFER_IN_S_MODE );
											else if ( k == BUFFER_OUT_S_MODE )
												_i_SCH_PRM_SET_MODULE_BUFFER_MODE( j - 1 , index , BUFFER_OUT_S_MODE );
										}
									}
								}
							}
						}
					}
					else {
						group = -1;
						break;
					}
				}
				break;

			case 201 : // G
			case 202 : // P
			case 203 : // Gx
			case 204 : // Px
			case 205 : // GT // 2013.03.19
				//
				if ( STRCMP_L( chr_return , "f" ) ) { // 2006.05.25
					j = MAX_TM_CHAMBER_DEPTH;
				}
				else if ( STRCMP_L( chr_return , "On" ) ) { // 2013.03.19
					if ( group == 205 ) {
						j = 1;
					}
					else {
						break;
					}
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) { // 2013.03.19
					if ( group == 205 ) {
						j = -1;
					}
					else {
						break;
					}
				}
				else {
					j = atoi( chr_return );
					//
					if ( group == 205 ) {
						if ( j < -1 ) break;
						if ( j > MAX_TM_CHAMBER_DEPTH ) break;
					}
					else {
						if ( j < 0 ) break;
						if ( j > MAX_TM_CHAMBER_DEPTH ) break;
					}
					//
				}
				//
				for ( i = 2 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 200 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return , &index ) ) {
						if ( index < PM1 ) { group = -1; break; }
						if ( index >= TM ) { group = -1; break; }
						if ( index == AL ) { group = -1; break; }
						if ( index == IC ) { group = -1; break; }
						//-------------
						if      ( group == 201 ) {
							_i_SCH_PRM_SET_MODULE_PICK_GROUP( j , index , 1 );
						}
						else if ( group == 202 ) {
							_i_SCH_PRM_SET_MODULE_PLACE_GROUP( j , index , 1 );
						}
						else if ( group == 203 ) {
							_i_SCH_PRM_SET_MODULE_PICK_GROUP( j , index , 0 );
						}
						else if ( group == 204 ) {
							_i_SCH_PRM_SET_MODULE_PLACE_GROUP( j , index , 0 );
						}
						else if ( group == 205 ) { // 2013.03.19
							_i_SCH_PRM_SET_MODULE_TMS_GROUP( index , j );
						}
					}
					else {
						group = -1;
						break;
					}
				}
				break;

/*
// 2012.09.21
			case 205 : // 2010.10.15
			case 206 : // 2010.10.15
				//
				if ( STRCMP_L( chr_return , "ON" ) ) {
					for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
						_i_SCH_PRM_SET_MODULE_PICK_ORDERING( j , ( group == 205 ) ? 1 : 2 );
					}
				}
				else {
					//
					j = atoi( chr_return );
					//
					if ( ( j < 0 ) || ( j >= MAX_TM_CHAMBER_DEPTH ) ) {
						group = -1;
						break;
					}
					//
					_i_SCH_PRM_SET_MODULE_PICK_ORDERING( j , ( group == 205 ) ? 1 : 2 );
				}
				break;
			case 207 : // 2012.08.23
				//
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 207 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return , &index ) ) {
						_i_SCH_PRM_SET_MODULE_PICK_ORDERING_SKIP( index , 1 );
					}
				}
				break;
*/

			case 300 :
				SCHMULTI_CTJOB_SINGLE_MODE_SET( FALSE );
				//
				if      ( STRCMP_L( chr_return , "On"  ) ) {
					SCHMULTI_CTJOB_USE_SET( TRUE );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					SCHMULTI_CTJOB_USE_SET( FALSE );
				}
				else {
					group = -1;
					break;
				}
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 210 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( STRCMP_L( chr_return , "On"  ) ) {
						SCHMULTI_CTJOB_SINGLE_MODE_SET( TRUE );
					}
					if ( STRCMP_L( chr_return , "Single"  ) ) {
						SCHMULTI_CTJOB_SINGLE_MODE_SET( TRUE );
					}
				}
				break;

			case 301 :
				_i_SCH_PRM_SET_DFIX_CARR_AUTO_HANDLER( chr_return );
				break;

			case 302 :
				if      ( STRCMP_L( chr_return , "default"  ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 0 );
				}
				else if ( STRCMP_L( chr_return , "expand" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 1 );
				}
				else if ( STRCMP_L( chr_return , "expand+Abort" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 2 );
				}
				else if ( STRCMP_L( chr_return , "expand.Abort" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 2 );
				}
				else if ( STRCMP_L( chr_return , "expandAbort" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 2 );
				}
				else if ( STRCMP_L( chr_return , "expand(pm)" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 3 );
				}
				else if ( STRCMP_L( chr_return , "expand+Abort(pm)" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 4 );
				}
				else if ( STRCMP_L( chr_return , "expand.Abort(pm)" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 4 );
				}
				else if ( STRCMP_L( chr_return , "expandAbort(pm)" ) ) {
					_i_SCH_PRM_SET_MODULE_COMMSTATUS_MODE( 4 );
				}
				else {
					group = -1;
					break;
				}
				//
				if ( ReadCnt > 2 ) { // 2012.07.21
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 211 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_MODULE_COMMSTATUS2_MODE( atoi(chr_return) );
				}
				break;

			case 303 :
				// 0 - DEFAULT
				// 1 - PM_ENABLE
				// 2 - ALL_ENABLE
				// 3 - OFF_ARMINTERLOCK
				// 4 - PM_ENABLE+OFF_ARMINTERLOCK
				// 5 - ALL_ENABLE+OFF_ARMINTERLOCK

				// 2005.10.10

				// 6  - BM_ENABLE
				// 7  - BPM_ENABLE
				// 8  - TM_ENABLE
				// 9  - TPM_ENABLE
				// 10 - TBM_ENABLE

				// 11 - BM_ENABLE+OFF_ARMINTERLOCK
				// 12 - BPM_ENABLE+OFF_ARMINTERLOCK
				// 13 - TM_ENABLE+OFF_ARMINTERLOCK
				// 14 - TPM_ENABLE+OFF_ARMINTERLOCK
				// 15 - TBM_ENABLE+OFF_ARMINTERLOCK

//     DEFAULT(0)   PM_ENABLE(1)  BM_ENABLE(6) BPM_ENABLE(7) ALL_ENABLE(2)  TM_ENABLE(8)  TPM_ENABLE(9) TBM_ENABLE(10)
//PM      D/E       E             D/E          E             E              D/E           E             D/E
//BM      D/E       D/E           E            E             E              D/E           D/E           E
//TM      D/E       D/E           D/E          D/E           E              E             E             E


				if      ( STRCMP_L( chr_return , "PM_ENABLE"  ) ) { // PM Enable Possible(Else Disable Possible)
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 1 );
				}
				else if ( STRCMP_L( chr_return , "ALL_ENABLE" ) ) { // PM+Else Enable Possible
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 2 );
				}
				else if ( STRCMP_L( chr_return , "OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 3 );
				}
				else if ( STRCMP_L( chr_return , "PM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 4 );
				}
				else if ( STRCMP_L( chr_return , "ALL_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 5 );
				}
				//======================================================================================================
				// 2005.10.10
				//======================================================================================================
				// 6  - BM_ENABLE
				// 7  - BPM_ENABLE
				// 8  - TM_ENABLE
				// 9  - TPM_ENABLE
				// 10 - TBM_ENABLE

				// 11 - BM_ENABLE+OFF_ARMINTERLOCK
				// 12 - BPM_ENABLE+OFF_ARMINTERLOCK
				// 13 - TM_ENABLE+OFF_ARMINTERLOCK
				// 14 - TPM_ENABLE+OFF_ARMINTERLOCK
				// 15 - TBM_ENABLE+OFF_ARMINTERLOCK
				else if ( STRCMP_L( chr_return , "BM_ENABLE" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 6 );
				}
				else if ( STRCMP_L( chr_return , "BPM_ENABLE" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 7 );
				}
				else if ( STRCMP_L( chr_return , "TM_ENABLE" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 8 );
				}
				else if ( STRCMP_L( chr_return , "TPM_ENABLE" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 9 );
				}
				else if ( STRCMP_L( chr_return , "TBM_ENABLE" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 10 );
				}
				else if ( STRCMP_L( chr_return , "BM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 11 );
				}
				else if ( STRCMP_L( chr_return , "BPM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 12 );
				}
				else if ( STRCMP_L( chr_return , "TM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 13 );
				}
				else if ( STRCMP_L( chr_return , "TPM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 14 );
				}
				else if ( STRCMP_L( chr_return , "TBM_ENABLE+OFF_ARMINTERLOCK" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 15 );
				}
				//======================================================================================================
				else if ( STRCMP_L( chr_return , "DEFAULT" ) ) {
					_i_SCH_PRM_SET_MODULE_TRANSFER_CONTROL_MODE( 0 );
				}
				else {
					group = -1;
				}
				break;

			case 304 :
				index = atoi( chr_return );
				if      ( index < 250   ) Scheduler_End_Set_Delay_Time( 250 );
				else if ( index > 30000 ) Scheduler_End_Set_Delay_Time( 30000 );
				else                      Scheduler_End_Set_Delay_Time( (DWORD) index );
				break;

			case 305 : // 2005.04.06
				index = atoi( chr_return );
				if      ( index < 250   ) PROCESS_PRJOB_End_Set_Delay_Time( 250 );
				else if ( index > 10000 ) PROCESS_PRJOB_End_Set_Delay_Time( 10000 );
				else                      PROCESS_PRJOB_End_Set_Delay_Time( index );
				break;

			case 306 : // 2006.02.08
				if ( STRCMP_L( chr_return , "OFF" ) ) {
					_i_SCH_PRM_SET_MULTI_GROUP_AUTO_ARRANGE( -1 );
				}
				else {
					_i_SCH_PRM_SET_MULTI_GROUP_AUTO_ARRANGE( atoi( chr_return ) );
				}
				break;

			case 307 : // 2006.05.04
				if ( STRCMP_L( chr_return , "MULTI" ) ) {
					_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 1 );
				}
				else if ( STRCMP_L( chr_return , "XPLACE" ) ) {
					_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 2 );
				}
				else if ( STRCMP_L( chr_return , "MULTI_XPLACE" ) ) {
					_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 3 );
				}
				else {
					_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 0 );
				}
				break;

			case 308 : // 2006.08.11
				if ( STRCMP_L( chr_return , "ON" ) ) {
					_i_SCH_PRM_SET_LOT_END_ESTIMATION( 1 );
				}
				else {
					_i_SCH_PRM_SET_LOT_END_ESTIMATION( 0 );
				}
				break;

			case 309 : // 2007.01.23
				if ( STRCMP_L( chr_return , "ON" ) ) {
					_i_SCH_PRM_SET_DFIX_IOINTLKS_USE( TRUE );
				}
				else if ( STRCMP_L( chr_return , "USE" ) ) {
					_i_SCH_PRM_SET_DFIX_IOINTLKS_USE( TRUE );
				}
				else {
					_i_SCH_PRM_SET_DFIX_IOINTLKS_USE( FALSE );
				}
				break;

			case 310 : // 2007.10.23
				i = atoi( chr_return );
				if ( ( index >= PM1 ) && ( index < AL ) ) {
					if      ( i < 512   ) PROCESS_DATALOG_Set_BUFFER_SIZE( index , 512 );
					else if ( i > 10240 ) PROCESS_DATALOG_Set_BUFFER_SIZE( index , 10240 );
					else                  PROCESS_DATALOG_Set_BUFFER_SIZE( index , i );
				}
				else {
					if      ( i < 512   ) PROCESS_DATALOG_Set_BUFFER_SIZE( -1 , 512 );
					else if ( i > 10240 ) PROCESS_DATALOG_Set_BUFFER_SIZE( -1 , 10240 );
					else                  PROCESS_DATALOG_Set_BUFFER_SIZE( -1 , i );
				}
				break;

			case 311 : // 2007.11.28
				if      ( STRCMP_L( chr_return , "ON" ) ) {
					SCHMULTI_RCPTUNE_EXTEND_USE_SET( 1 );
				}
				else if ( STRCMP_L( chr_return , "PJOBEXT" ) ) {
					SCHMULTI_RCPTUNE_EXTEND_USE_SET( 2 );
				}
				else {
					SCHMULTI_RCPTUNE_EXTEND_USE_SET( 0 );
				}
				break;

			case 312 : // 2008.02.01
				if      ( STRCMP_L( chr_return , "ALL" ) ) {
					_i_SCH_PRM_SET_PICKPLACE_CANCEL_MESSAGE( 3 );
				}
				else if ( STRCMP_L( chr_return , "TM" ) ) {
					_i_SCH_PRM_SET_PICKPLACE_CANCEL_MESSAGE( 2 );
				}
				else if ( STRCMP_L( chr_return , "FM" ) ) {
					_i_SCH_PRM_SET_PICKPLACE_CANCEL_MESSAGE( 1 );
				}
				else {
					_i_SCH_PRM_SET_PICKPLACE_CANCEL_MESSAGE( 0 );
				}
				break;

			case 313 : // 2008.04.17
				if      ( STRCMP_L( chr_return , "UnUse" ) ) {
					_i_SCH_PRM_SET_TIMER_RESET_WHEN_NEW_START( 2 );
				}
				else if ( STRCMP_L( chr_return , "NotUse" ) ) {
					_i_SCH_PRM_SET_TIMER_RESET_WHEN_NEW_START( 2 );
				}
				else if ( STRCMP_L( chr_return , "On" ) ) {
					_i_SCH_PRM_SET_TIMER_RESET_WHEN_NEW_START( 1 );
				}
				else {
					_i_SCH_PRM_SET_TIMER_RESET_WHEN_NEW_START( 0 );
				}
				break;

			case 314 : // 2008.07.09
				/*
				// 2014.01.24
				if      ( STRCMP_L( chr_return , "LOT+CLUSTER" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 1 );
				}
				else if ( STRCMP_L( chr_return , "DETAIL" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 2 );
				}
				else if ( STRCMP_L( chr_return , "LOT+DETAIL" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 3 );
				}
				else if ( STRCMP_L( chr_return , "PATH" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 4 );
				}
				else if ( STRCMP_L( chr_return , "LOT+PATH" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 5 );
				}
				else {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 0 );
				}
				*/
				// 2014.01.24
				if      ( STRCMP_L( chr_return , "LOT+CLUSTER" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 1 );
				}
				else if ( STRCMP_L( chr_return , "LOTA+CLUSTER" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 2 );
				}
				else if ( STRCMP_L( chr_return , "DETAIL" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 3 );
				}
				else if ( STRCMP_L( chr_return , "LOT+DETAIL" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 4 );
				}
				else if ( STRCMP_L( chr_return , "LOTA+DETAIL" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 5 );
				}
				else if ( STRCMP_L( chr_return , "PATH" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 6 );
				}
				else if ( STRCMP_L( chr_return , "LOT+PATH" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 7 );
				}
				else if ( STRCMP_L( chr_return , "LOTA+PATH" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 8 );
				}
				else {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD( 0 );
				}
				break;

			case 1314 : // 2018.08.22
				//
				if      ( STRCMP_L( chr_return , "CJ" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD2( 1 );
				}
				else if ( STRCMP_L( chr_return , "PJ" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD2( 2 );
				}
				else if ( STRCMP_L( chr_return , "CJ+PJ" ) ) {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD2( 3 );
				}
				else {
					_i_SCH_PRM_SET_CHECK_DIFFERENT_METHOD2( 0 );
				}
				break;

			case 315 : // 2008.10.15
				if      ( STRCMP_L( chr_return , "DEFAULT" ) ) {
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 0 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "DIRECT" ) ) {
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 1 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "FILE" ) ) {
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 1 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "ENCRYPTION" ) ) { // 2010.04.15
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 0 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				else if ( STRCMP_L( chr_return , "DIRECT.ENCRYPTION" ) ) { // 2010.04.15
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 1 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				else if ( STRCMP_L( chr_return , "FILE.ENCRYPTION" ) ) { // 2010.04.15
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 1 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				//
				else if ( STRCMP_L( chr_return , "REMOVE" ) ) { // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 2 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "DEFAULT.REMOVE" ) ) { // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 2 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "DIRECT.REMOVE" ) ) { // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 3 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "FILE.REMOVE" ) ) { // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 3 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				else if ( STRCMP_L( chr_return , "ENCRYPTION.REMOVE" ) ) { // 2010.04.15 // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 2 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				else if ( STRCMP_L( chr_return , "DIRECT.ENCRYPTION.REMOVE" ) ) { // 2010.04.15 // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 3 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				else if ( STRCMP_L( chr_return , "FILE.ENCRYPTION.REMOVE" ) ) { // 2010.04.15 // 2010.11.16
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 3 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 1 );
				}
				//
				else {
					_i_SCH_PRM_SET_DFIX_LOT_LOG_MODE( 0 );
					_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 ); // 2010.04.15
				}
				break;

			case 316 : // 2010.09.02
				_i_SCH_PRM_SET_PROCESS_MESSAGE_STYLE( atoi( chr_return ) );
				break;

			case 317 : // 2010.09.13
				if ( chr_return[0] == 'P' ) { // 2012.09.03
					_i_SCH_PRM_SET_WAFER_ANIM_STATUS_CHECK( 100000 + atoi( chr_return + 1 ) );
				}
				else {
					_i_SCH_PRM_SET_WAFER_ANIM_STATUS_CHECK( atoi( chr_return ) );
				}
				break;

			case 318 : // 2010.09.21
				_i_SCH_PRM_SET_MAINT_MESSAGE_STYLE( atoi( chr_return ) );
				//
				// OP	LOTPREFORCE		MAINTSCENARIO
				// 0	O(?)			X
				// 1	O				O
				break;

			case 319 : // 2010.09.13
				if ( STRCMP_L( chr_return , "FM" ) ) { // 2012.09.10
					_i_SCH_PRM_SET_EIL_INTERFACE( -1 );
				}
				else {
					i = atoi( chr_return );	if ( i < 0 ) i = 0;
					_i_SCH_PRM_SET_EIL_INTERFACE( i );
				}
				//
				_i_SCH_PRM_SET_EIL_INTERFACE_JOBLOG( "" ); // 2011.06.20
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 220 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_EIL_INTERFACE_JOBLOG( chr_return );
				}
				break;

			case 320 : // 2010.10.30
				_i_SCH_PRM_SET_DUMMY_MESSAGE( atoi( chr_return ) );
				break;

			case 321 : // 2011.05.20
				i = atoi( chr_return );
				if ( i < 0 ) i = 0;
				if ( i > 9 ) i = 9;
				_i_SCH_PRM_SET_MTLOUT_INTERFACE( i );
				break;

			case 322 : // 2012.09.03
				//
				if      ( STRCMP_L( chr_return , "ON" ) ) {
					j = 1;
				}
				else if ( STRCMP_L( chr_return , "OFF" ) ) {
					j = 0;
				}
				else {
					j = atoi( chr_return );
				}
				_i_SCH_PRM_SET_EIL_INTERFACE_FLOW_USER_OPTION( index + 1 , j );
				//
				break;

			case 323 : // 2012.12.05
				//
				if      ( STRCMP_L( chr_return , "-" ) ) {
					//
					j = 0;
					//
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( index + 1 , 2 );
					//
				}
				else if  ( STRCMP_L( chr_return , "ON" ) ) {
					//
					j = 1;
					//
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( index + 1 , 1 );
					//
				}
				else if ( STRCMP_L( chr_return , "OFF" ) ) {
					//
					j = 0;
					//
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( index + 1 , 1 );
					//
				}
				else {
					//
					j = atoi( chr_return );
					//
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( index + 1 , 1 );
					//
				}
				//
				_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTION( index + 1 , j );
				//
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 323 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					UTIL_Extract_Reset_String( chr_return );
					//
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONS( index + 1 , chr_return );
					//
				}
				break;

			case 324 : // 2013.11.21
				if ( STRCMP_L( chr_return , "ON" ) ) {
					_i_SCH_PRM_SET_DFIX_CHAMBER_INTLKS_IOUSE( TRUE );
				}
				else if ( STRCMP_L( chr_return , "USE" ) ) {
					_i_SCH_PRM_SET_DFIX_CHAMBER_INTLKS_IOUSE( TRUE );
				}
				else {
					_i_SCH_PRM_SET_DFIX_CHAMBER_INTLKS_IOUSE( FALSE );
				}
				break;

			case 400 : // 2002.05.09
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return , &i ) ) {
					if ( ( i >= PM1 ) && ( i < AL ) ) {
						_i_SCH_PRM_SET_SDUMMY_PROCESS_CHAMBER( index , i );
						_i_SCH_PRM_SET_SDUMMY_PROCESS_SLOT( index , 1 );
						if ( ReadCnt > 2 ) {
							if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 230 - [%s]-[%d]\n" , Filename , Line );
								_lclose( hFile );
								return FALSE;
							}
							j = atoi(chr_return);
							if ( j <= 0 ) j = 1;
							_i_SCH_PRM_SET_SDUMMY_PROCESS_SLOT( index , j );
						}
					}
					else {
						_i_SCH_PRM_SET_SDUMMY_PROCESS_CHAMBER( index , 0 );
					}
				}
				else {
					_i_SCH_PRM_SET_SDUMMY_PROCESS_CHAMBER( index , 0 );
				}
				break;

			case 500 :
				i = atoi(chr_return);
				if ( ( i > 0 ) && ( i <= MAX_FINGER_TM ) ) {
					_i_SCH_PRM_SET_DFIX_MAX_FINGER_TM( i );
				}
				break;

			case 600 : // 2012.02.01
				//
				i = atoi(chr_return);
				//
				ALARM_INFO_SET_EVENT_RECEIVE_OPTION( i );
				//
				break;

			case 601 : // 2013.04.25
				//
				if      ( STRCMP_L( chr_return , "Same" ) ) {
					i = 0;
				}
				else if ( STRCMP_L( chr_return , "AlwaysUse" ) ) {
					i = 1;
				}
				else if ( STRCMP_L( chr_return , "AlwaysNotUse" ) ) {
					i = 2;
				}
				else if ( STRCMP_L( chr_return , "Same(M)" ) ) {
					i = 3;
				}
				else if ( STRCMP_L( chr_return , "AlwaysUse(M)" ) ) {
					i = 4;
				}
				else if ( STRCMP_L( chr_return , "AlwaysNotUse(M)" ) ) {
					i = 5;
				}
				else {
					i = atoi( chr_return );
				}
				//
				_i_SCH_PRM_SET_MESSAGE_MAINTINTERFACE( i );
				break;

			case 602 : // 2013.06.20
				//
				if      ( STRCMP_L( chr_return , "On" ) ) {
					i = 2;
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					i = 1;
				}
				else {
					i = 0;
				}
				//
				_i_SCH_PRM_SET_USE_EXTEND_OPTION( i );
				break;

			case 603 : // 2016.04.26
				//
				if      ( STRCMP_L( chr_return , "On" ) ) {
					SLOTPROCESSING = TRUE;
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					SLOTPROCESSING = FALSE;
				}
				else {
					SLOTPROCESSING = FALSE;
				}
				//
				_i_SCH_PRM_SET_USE_EXTEND_OPTION( i );
				break;

			case 701 : // 2017.02.09
				//
				if      ( STRCMP_L( chr_return , "On" ) ) {
					SCHFILE_USE_END_DELIMITER = TRUE;
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					SCHFILE_USE_END_DELIMITER = FALSE;
				}
				else {
					SCHFILE_USE_END_DELIMITER = FALSE;
				}
				//
				if ( ReadCnt > 2 ) { // 2017.11.02
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 701 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					UTIL_Extract_Reset_String( chr_return );
					//
					SCHFILE_USE_END_DELIMITER_ALARM = atoi( chr_return );
					//
				}
				//
				break;

			case 702 : // 2017.12.07
				//
				if      ( STRCMP_L( chr_return , "On" ) ) {
					_i_SCH_PRM_SET_OPTIMIZE_MODE( 1 );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					_i_SCH_PRM_SET_OPTIMIZE_MODE( 0 );
				}
				else {
					_i_SCH_PRM_SET_OPTIMIZE_MODE( atoi(chr_return) );
				}
				//
				break;

			case 703 : // 2018.09.05
				//
				if      ( STRCMP_L( chr_return , "On" ) ) {
					EXTEND_SOURCE_USE = 1;
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					EXTEND_SOURCE_USE = 0;
				}
				else {
					EXTEND_SOURCE_USE = atoi(chr_return);
				}
				//
				break;

			}
			if ( group == -1 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "[SKIP] Unknown Parameter or Downgrade Version(%s) - [%s]-[%d]\n" , Buffer , Filename , Line );
			//	_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , Buffer , Filename , Line ); // 2005.01.24
			//	_lclose( hFile ); // 2005.01.24
			//	return FALSE; // 2005.01.24
			}
		}
	}
	_lclose( hFile );

	if ( !_i_SCH_PRM_GET_MODULE_MODE( TM ) && !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) return FALSE;
	//
	//=======================================================================================================
	//
	// 2016.04.26
	//
	if ( SLOTPROCESSING ) { // 2016.04.26
		//
		for ( Line = 0 ; Line < MAX_PM_CHAMBER_DEPTH ; Line++ ) {
			//
			index = GetRealDispFunction( PM1 + Line , &j );
			//
			sprintf( Buffer , "SCHEDULER_PROCESS_PM%d_%d" , index + 1 , j );
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_DFIX_PROCESS_FUNCTION_NAME( i , Line , Buffer );
			//
			sprintf( Buffer , "PM%d_%d.PROCESS_LOG" , index + 1 , j );
			_i_SCH_PRM_SET_DFIX_PROCESS_LOG_FUNCTION_NAME( Line , Buffer );
			//
		}
	}
	//
	//=======================================================================================================
	if ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() > 0 ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) ) {
			_i_SCH_PRM_SET_DUMMY_PROCESS_CHAMBER( 0 );
		}
	}
	for ( i = 0 ; i < MAX_SDUMMY_DEPTH ; i++ ) { // 2002.05.09
		if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( i ) > 0 ) {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( i ) ) ) {
				_i_SCH_PRM_SET_SDUMMY_PROCESS_CHAMBER( i , 0 );
			}
		}
	}
	//==============================================================================================================	
	// 2006.01.06
	//==============================================================================================================	
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) == -1 ) { // 2013.03.19
			index = 0;
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) == 1 ) {
					index++;
				}
			}
			//
			_i_SCH_PRM_SET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i , index );
			//
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) == -1 ) { // 2013.03.19
			//
			index = 0;
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) == 1 ) {
					index++;
				}
			}
			//
			_i_SCH_PRM_SET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i , index );
			//
		}
	}
	//==============================================================================================================	
	/*
	// 2012.09.21
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i ) > 0 ) {
			_i_SCH_PRM_SET_MODULE_PICK_ORDERING_USE( 1 );
			break;
		}
	}
	*/
	//==============================================================================================================	
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) { // 2010.12.22
		if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) {
			_i_SCH_PRM_SET_DFIX_FIC_MULTI_CONTROL( 3 );
		}
	}
	//==============================================================================================================	
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2012.09.03
		//
		for ( i = 0 ; i < 256 ; i++ ) _i_SCH_PRM_SET_EIL_INTERFACE_FLOW_USER_OPTION( i , -1 ); // 2012.09.03
		//
	}
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------
	return TRUE;
}


//-------------------------------------------------------------------------
// Function = FILE_SCHEDULE_SETUP
//-------------------------------------------------------------------------
//BOOL FILE_SCHEDULE_SETUP( char *Filename ) { // 2015.01.23
//BOOL FILE_SCHEDULE_SETUP( char *Filename_Org ) { // 2015.01.23 // 2017.02.09
BOOL FILE_SCHEDULE_SETUP_SUB( char *Filename_Org ) { // 2015.01.23 // 2017.02.09
	HFILE hFile;
	char Buffer[256];
	char B1s[32] , B2s[32];
//	char chr_return[64]; // 2011.11.18
	char chr_return[256]; // 2011.11.18
	char Imsi_Buffer[256];
	int  group , index , i , j;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;
	char Filename[256]; // 2015.01.23
	//
	int ins_FILE_END_DELIMITER = 0; // 2017.02.09

	// System Session
	_i_SCH_PRM_SET_SYSTEM_LOG_STYLE( 0 ); // 2006.02.07

	// Default Session
	for ( Line = 0 ; Line < MAX_CASSETTE_SIDE ; Line++ ) {
		_i_SCH_PRM_SET_MAPPING_SKIP( CM1 + Line , 0 );
		_i_SCH_PRM_SET_AUTO_HANDOFF( CM1 + Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_FA_EXTEND_HANDOFF( CM1 + Line , 0 );
		//
		_i_SCH_PRM_SET_FA_MID_READ_POINT( Line , 0 );
		_i_SCH_PRM_SET_FA_JID_READ_POINT( Line , 0 );
		//
		_i_SCH_PRM_SET_FA_LOAD_COMPLETE_MESSAGE( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_FA_UNLOAD_COMPLETE_MESSAGE( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_FA_MAPPING_AFTERLOAD( Line , 0 ); // 2002.05.05
	}
	//
	_i_SCH_PRM_SET_UTIL_MAPPING_WHEN_TMFREE( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_TM_GATE_SKIP_CONTROL( FALSE );
	_i_SCH_PRM_SET_UTIL_TM_MOVE_SR_CONTROL( FALSE );
	_i_SCH_PRM_SET_UTIL_FM_MOVE_SR_CONTROL( FALSE );
	_i_SCH_PRM_SET_UTIL_SCH_START_OPTION( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_LOOP_MAP_ALWAYS( 0 );
	_i_SCH_PRM_SET_UTIL_FIRST_MODULE_PUT_DELAY_TIME( 0 );
	_i_SCH_PRM_SET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME( 0 );
	_i_SCH_PRM_SET_UTIL_ABORT_MESSAGE_USE_POINT( 0 );
	_i_SCH_PRM_SET_UTIL_CANCEL_MESSAGE_USE_POINT( 0 );
	_i_SCH_PRM_SET_UTIL_END_MESSAGE_SEND_POINT( 0 );
	_i_SCH_PRM_SET_UTIL_CHAMBER_PUT_PR_CHECK( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_CHAMBER_GET_PR_CHECK( 0 ); // 2002.05.30
	_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME2( 0 ); // 2011.04.27
	_i_SCH_PRM_SET_UTIL_FAL_MULTI_WAITTIME( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_FIC_MULTI_TIMEMODE( 0 ); // 2005.12.01
	//
	_i_SCH_PRM_SET_CLSOUT_BM_SINGLE_ONLY( 0 ); // 2004.05.06
	_i_SCH_PRM_SET_CLSOUT_AL_TO_PM( 0 ); // 2005.12.19
	_i_SCH_PRM_SET_CLSOUT_IC_TO_PM( 0 ); // 2005.12.19
	_i_SCH_PRM_SET_MAINTINF_DB_USE( 0 ); // 2012.05.04
	//
	_i_SCH_PRM_SET_READY_MULTIJOB_MODE( 0 ); // 2007.03.15
	//
	for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) {
		//
		_i_SCH_PRM_SET_SIM_TIME1( Line , 0 ); // 2016.12.09
		_i_SCH_PRM_SET_SIM_TIME2( Line , 0 ); // 2016.12.09
		_i_SCH_PRM_SET_SIM_TIME3( Line , 0 ); // 2016.12.09
		//
		_i_SCH_PRM_SET_Getting_Priority( Line , 0 );
		_i_SCH_PRM_SET_Putting_Priority( Line , 0 );
		//
		_i_SCH_PRM_SET_Process_Run_In_Mode( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_Process_Run_In_Time( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_Process_Run_Out_Mode( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_Process_Run_Out_Time( Line , 0 ); // 2002.05.05
		//
		_i_SCH_PRM_SET_READY_RECIPE_USE( Line , 0 );
		_i_SCH_PRM_SET_READY_RECIPE_TYPE( Line , 0 ); // 2017.10.16
		_i_SCH_PRM_SET_READY_RECIPE_MINTIME( Line , 0 ); // 2005.08.18
		_i_SCH_PRM_SET_READY_RECIPE_NAME( Line , "" );
		_i_SCH_PRM_SET_END_RECIPE_USE( Line , 0 );
		_i_SCH_PRM_SET_END_RECIPE_MINTIME( Line , 0 ); // 2005.08.18
		_i_SCH_PRM_SET_END_RECIPE_NAME( Line , "" );
		//
		_i_SCH_PRM_SET_PRCS_TIME_WAIT_RANGE( Line , 0 );
		_i_SCH_PRM_SET_PRCS_TIME_REMAIN_RANGE( Line , 0 ); // 2006.05.04
		//
		_i_SCH_PRM_SET_CLSOUT_USE( Line , 0 );
		_i_SCH_PRM_SET_CLSOUT_DELAY( Line , 0 );
		_i_SCH_PRM_SET_CLSOUT_MESSAGE_USE( Line , FALSE );
		//
		_i_SCH_PRM_SET_FAIL_PROCESS_SCENARIO( Line , 0 );
		_i_SCH_PRM_SET_STOP_PROCESS_SCENARIO( Line , 0 );
		//
		_i_SCH_PRM_SET_MRES_SUCCESS_SCENARIO( Line , 0 ); // 2003.09.25
		//
		_i_SCH_PRM_SET_MRES_FAIL_SCENARIO( Line , 0 ); // 2002.05.05
		_i_SCH_PRM_SET_MRES_ABORT_SCENARIO( Line , 0 ); // 2002.05.05
		//
		_i_SCH_PRM_SET_FIXCLUSTER_PRE_MODE( Line , 0 );
		_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_USE( Line , FALSE );
		_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_USE( Line , FALSE );
		_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_RECIPE_NAME( Line , "" );
		_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( Line , "" );
		//
		_i_SCH_PRM_SET_FIXCLUSTER_POST_MODE( Line , 0 );
		_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_USE( Line , FALSE );
		_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_USE( Line , FALSE );
		_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_RECIPE_NAME( Line , "" );
		_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_RECIPE_NAME( Line , "" );
		//
		_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( Line , 0 );
		//
		_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE( Line , 0 ); // 2002.07.10
		_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( Line , 0 ); // 2002.07.10
		_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( Line , 0 ); // 2002.07.10
		//
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPRECV( Line , FALSE ); // 2002.10.16
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPSEND( Line , FALSE ); // 2002.10.16
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTRECV( Line , FALSE ); // 2002.10.16
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTSEND( Line , FALSE ); // 2002.10.16
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_GATE( Line , 0 ); // 2002.10.16
		//
		_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_CANCEL( Line , 0 ); // 2016.11.24
		//
	}
	//
	_i_SCH_PRM_SET_MRES_ABORT_ALL_SCENARIO( 0 ); // 2002.07.23
	_i_SCH_PRM_SET_DISABLE_MAKE_HOLE_GOTOSTOP( 0 ); // 2004.05.14
	//
	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 0 , 0 );
	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 1 , 0 );
	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 0 , 0 );
	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 1 , 0 );
	//
	for ( Line = 0 ; Line < MAX_CASSETTE_SIDE ; Line++ ) {
		_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER( Line , 0 );
		_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( Line , 0 );
	}
	//
	GUI_GROUP_SELECT_SET( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_UTIL_SCH_LOG_DIRECTORY_FORMAT( 0 );
	_i_SCH_PRM_SET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN( FALSE );
	//
	for ( Line = 0 ; Line < MAX_CASSETTE_SIDE ; Line++ ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_i_SCH_PRM_SET_INTERLOCK_PM_RUN_FOR_CM( Line , i , 0 ); // 2002.05.05
			_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( Line , i , 0 ); // 2002.05.05
		}
	}
	for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) {
		//
//		_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( Line , 0 ); // 2013.05.28
//		_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( Line , 0 ); // 2013.05.28
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( i , Line , 0 ); // 2013.05.28
			_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( i , Line , 0 ); // 2013.05.28
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2016.11.14
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( Line , i , 0 );
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( Line , i , 0 ); // 2018.12.06
		}
		//
		_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( Line , 0 ); // 2006.09.26
		_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( Line , 0 ); // 2006.09.26
		//
		_i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( Line , 0 ); // 2002.08.27
		_i_SCH_PRM_SET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( Line , 0 ); // 2004.01.30 // None Pick Place
	}
	for ( Line = 0 ; Line < MAX_TM_CHAMBER_DEPTH ; Line++ ) { // 2002.05.05
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_i_SCH_PRM_SET_INTERLOCK_TM_RUN_FOR_ALL( Line , i , 0 );
		}
		_i_SCH_PRM_SET_INTERLOCK_TM_SINGLE_RUN( Line , 0 ); // 2007.01.16
	}
	//
	_i_SCH_PRM_SET_INTERLOCK_FM_BM_PLACE_SEPARATE( FALSE ); // 2007.11.15
//	_i_SCH_PRM_SET_INTERLOCK_TM_BM_OTHERGROUP_SWAP( FALSE ); // 2013.04.25 // 2017.07.13
	_i_SCH_PRM_SET_INTERLOCK_TM_BM_OTHERGROUP_SWAP( 0 ); // 2013.04.25 // 2017.07.13
	//
	_i_SCH_PRM_SET_DUMMY_PROCESS_MODE( 1 );
	//
	_i_SCH_PRM_SET_DUMMY_PROCESS_FIXEDCLUSTER( 0 ); // 2002.06.12
	//
	_i_SCH_PRM_SET_DUMMY_PROCESS_MULTI_LOT_ACCESS( 0 ); // 2003.02.08
	//
	for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) {
		_i_SCH_PRM_SET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( Line , FALSE );
		//
		_i_SCH_PRM_SET_PRE_RECEIVE_WITH_PROCESS_RECIPE( Line , FALSE );
		//
		_i_SCH_PRM_SET_NEXT_FREE_PICK_POSSIBLE( Line , 0 );
		_i_SCH_PRM_SET_PREV_FREE_PICK_POSSIBLE( Line , 0 );
	}
	//
	_i_SCH_PRM_SET_FA_LOADUNLOAD_SKIP( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_MAPPING_SKIP( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_NORMALUNLOAD_NOTUSE( FALSE ); // 2007.08.29

	_i_SCH_PRM_SET_FA_SINGLE_CASS_MULTI_RUN( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_STARTEND_STATUS_SHOW( 0 ); // 2002.05.05
	//
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REJECT( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT( 0 ); // 2002.05.05

	_i_SCH_PRM_SET_FA_WINDOW_NORMAL_CHECK_SKIP( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_WINDOW_ABORT_CHECK_SKIP( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_PROCESS_STEPCHANGE_CHECK_SKIP( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SEND_MESSAGE_DISPLAY( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_EXPAND_MESSAGE_USE( 0 ); // 2002.05.05
	_i_SCH_PRM_SET_FA_SUBSTRATE_WAFER_ID( 0 ); // 2002.10.10
	_i_SCH_PRM_SET_FA_REJECTMESSAGE_DISPLAY( 0 ); // 2004.06.16
	//
	SCHMULTI_DEFAULT_GROUP_SET( "" );
	SCHMULTI_CHECK_OVERTIME_HOUR_SET( 0 ); // 2012.04.20
	//
	_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_SWITCH( 0 ); // 2002.09.05
	//
	_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_ORDER( 0 ); // 2004.06.24
	//
	_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( 0 ); // 2004.08.14
	SCH_SW_BM_SCHEDULING_FACTOR_ORDER_SET( "" ); // 2017.04.21

	if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() != 0 ) {
		for ( Line = 0 ; Line < MAX_CHAMBER ; Line++ ) {
			if ( Line < TM ) { // 2002.06.24
				_i_SCH_PRM_SET_MODULE_MODE_for_SW( Line , 0 );
			}
		}
		_i_SCH_PRM_SET_MODULE_MODE_for_SW( F_AL , 0 ); // 2004.09.03
		_i_SCH_PRM_SET_MODULE_MODE_for_SW( F_IC , 0 ); // 2004.09.03
	}
	//
	_i_SCH_PRM_SET_UTIL_CM_SUPPLY_MODE( 0 ); // 2002.10.02
	_i_SCH_PRM_SET_UTIL_PREPOST_PROCESS_DEPAND( 0 ); // 2003.01.08
	//
	_i_SCH_PRM_SET_UTIL_START_PARALLEL_CHECK_SKIP( 0 ); // 2003.01.11
	//
	_i_SCH_PRM_SET_UTIL_WAFER_SUPPLY_MODE( 0 ); // 2002.10.02
	//
	_i_SCH_PRM_SET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE( 0 ); // 2003.10.09
	//
	_i_SCH_PRM_SET_UTIL_BMEND_SKIP_WHEN_RUNNING( 0 ); // 2004.02.19
	_i_SCH_PRM_SET_UTIL_PMREADY_SKIP_WHEN_RUNNING( 0 ); // 2006.03.28
	//
	_i_SCH_PRM_SET_UTIL_ADAPTIVE_PROGRESSING( 0 ); // 2008.02.29
	//
	_i_SCH_PRM_SET_UTIL_LOT_LOG_PRE_POST_PROCESS( 0 ); // 2004.05.11
	//
	_i_SCH_PRM_SET_BATCH_SUPPLY_INTERRUPT( 0 ); // 2006.09.05
	//
	_i_SCH_PRM_SET_DIFF_LOT_NOTSUP_MODE( 0 ); // 2007.07.05
	//
	_i_SCH_PRM_SET_METHOD1_TO_1BM_USING_MODE( 0 ); // 2018.03.21
	//
	// 2012.09.21
	_i_SCH_PRM_SET_MODULE_PICK_ORDERING_USE( 0 ); // 2010.10.15
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _i_SCH_PRM_SET_MODULE_PICK_ORDERING( i , 0 );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) _i_SCH_PRM_SET_MODULE_PICK_ORDERING_SKIP( i , 0 ); // 2012.08.23
	//
	File_Buffering_For_Open( Filename_Org , Filename ); // 2015.01.23
	//
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , Filename );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt > 0 ) {
			if ( ReadCnt < 2 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Parameter Error[%s] - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 240 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			group = -1;
			index = -1;
			if      ( STRNCMP_L( chr_return , "Module_SW_Mode(" , 15   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 15 , &index ) )			group = 1;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Module_SW_Size(" , 15   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 15 , &index ) )			group = 2;
				else																		group = 0;
			}

			if      ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_PREPRECV(" , 31   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 31 , &index ) )			group = 11;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_PREPSEND(" , 31   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 31 , &index ) )			group = 12;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_POSTRECV(" , 31   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 31 , &index ) )			group = 13;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_POSTSEND(" , 31   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 31 , &index ) )			group = 14;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_GATE(" , 27   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 27 , &index ) )			group = 15;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MODULE_MESSAGE_NOTUSE_CANCEL(" , 29   ) ) { // 2016.11.24
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 29 , &index ) )			group = 16;
				else																		group = 0;
			}


			if      ( STRCMP_L( chr_return , "ROBOT_ROTATE_WHEN_PREPARE"     ) ) { group = 21; }
			else if ( STRNCMP_L( chr_return , "ROBOT_ROTATE_WHEN_PREPARE(" , 26   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 26 , &index ) )			group = 22;
				else																		group = 0;
			}
			else if ( STRCMP_L( chr_return , "ANIMATION_ROTATE_PREPARE"     ) ) { group = 21; }
			else if ( STRNCMP_L( chr_return , "ANIMATION_ROTATE_PREPARE(" , 25   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 25 , &index ) )			group = 22;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "Module_Name(" , 12   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 12 , &index ) )			group = 31;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_RUN_GAP(" , 16   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 16 , &index ) )			group = 41;
				else																		group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_REMAIN_GAP(" , 19   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 19 , &index ) )			group = 42;
				else																		group = 0;
			}

			if      ( STRCMP_L( chr_return , "MAPPING_SKIP"               ) )							group = 100;
			else if ( STRNCMP_L( chr_return , "MAPPING_SKIP_" , 13   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( chr_return + 13 , &index ) )	group = 101;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "AUTO_HANDOFF"               ) )							group = 110;
			else if ( STRNCMP_L( chr_return , "AUTO_HANDOFF_" , 13   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( chr_return + 13 , &index ) )	group = 111;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "EXTEND_HANDOFF"             ) )							group = 120;
			else if ( STRNCMP_L( chr_return , "EXTEND_HANDOFF_" , 15   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_CM( chr_return + 15 , &index ) )	group = 121;
				else																					group = 0;
			}

			if      ( STRCMP_L( chr_return , "MID_READ_POINT"             ) )							group = 130;
			else if ( STRNCMP_L( chr_return , "MID_READ_POINT_CM" , 17   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 17 , &index , MAX_CASSETTE_SIDE ) )	group = 131;
				else																					group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MID_READ_POINT_" , 15   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 15 , &index , MAX_CASSETTE_SIDE ) )	group = 131;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "JID_READ_POINT"             ) )							group = 140;
			else if ( STRNCMP_L( chr_return , "JID_READ_POINT_" , 15   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 15 , &index , MAX_CASSETTE_SIDE ) )	group = 141;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "LOAD_COMPLETE_MESSAGE"      ) )							group = 150;
			else if ( STRNCMP_L( chr_return , "LOAD_COMPLETE_MESSAGE_" , 22   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 22 , &index , MAX_CASSETTE_SIDE ) )	group = 151;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "UNLOAD_COMPLETE_MESSAGE"      ) )							group = 160;
			else if ( STRNCMP_L( chr_return , "UNLOAD_COMPLETE_MESSAGE_" , 24   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 24 , &index , MAX_CASSETTE_SIDE ) )	group = 161;
				else																					group = 0;
			}
			if      ( STRCMP_L( chr_return , "MAPPING_AFTERLOAD"          ) )							group = 170;
			else if ( STRNCMP_L( chr_return , "MAPPING_AFTERLOAD_" , 18   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 18 , &index , MAX_CASSETTE_SIDE ) )	group = 171;
				else																					group = 0;
			}

			if      ( STRCMP_L( chr_return , "MAPPING_TMFREE"             ) ) group = 200;
			else if ( STRCMP_L( chr_return , "CONTROL_GATE_TM"            ) ) group = 201;
			else if ( STRCMP_L( chr_return , "CONTROL_MOVE_SR_TM"         ) ) group = 202;
			else if ( STRCMP_L( chr_return , "CONTROL_MOVE_SR_FM"         ) ) group = 203;
			else if ( STRCMP_L( chr_return , "SCH_START_OPTION"           ) ) group = 204;
			else if ( STRCMP_L( chr_return , "LOOP_MAP_ALWAYS"            ) ) group = 205;
			else if ( STRCMP_L( chr_return , "SCH_LOG_DIRECTORY_FORMAT"   ) ) group = 206;
			else if ( STRCMP_L( chr_return , "SCH_CHANGE_PROCESS_WAFER"   ) ) group = 207;
			else if ( STRCMP_L( chr_return , "FIRST_MODULE_PUT_DELAY"     ) ) group = 208;
			else if ( STRCMP_L( chr_return , "ABORT_MESSAGE_USE_POINT"    ) ) group = 209;
			else if ( STRCMP_L( chr_return , "CANCEL_MESSAGE_USE_POINT"   ) ) group = 210;
			else if ( STRCMP_L( chr_return , "END_MESSAGE_SEND_POINT"     ) ) group = 211;
			else if ( STRCMP_L( chr_return , "CASSETTE_SUPPLY_DEV_TIME"   ) ) group = 212;
			else if ( STRCMP_L( chr_return , "CHAMBER_PUT_PR_CHECK"       ) ) group = 213;
			else if ( STRCMP_L( chr_return , "MULTI_COOLER_WAITTIME"      ) ) group = 214;
			else if ( STRCMP_L( chr_return , "MULTI_ALIGNER_WAITTIME"     ) ) group = 215;
			else if ( STRCMP_L( chr_return , "CHAMBER_GET_PR_CHECK"       ) ) group = 216;

			else if ( STRCMP_L( chr_return , "MULTI_COOLER_TIMEMODE"      ) ) group = 217; // 2005.12.01

			if      ( STRCMP_L( chr_return , "PRIORITY"                   ) )		group = 300;
			else if ( STRNCMP_L( chr_return , "PRIORITY(" , 9   ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 9 , &index ) )	group = 301;
				else																group = 0;
			}
			else if ( STRCMP_L( chr_return , "PRIORITY_PICK"              ) )		group = 310;
			else if ( STRNCMP_L( chr_return , "PRIORITY_PICK(" , 14  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 14 , &index ) )	group = 311;
				else																group = 0;
			}
			else if ( STRCMP_L( chr_return , "PRIORITY_PLACE"             ) )		group = 320;
			else if ( STRNCMP_L( chr_return , "PRIORITY_PLACE(" , 15  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 15 , &index ) )	group = 321;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_RUN_IN(" , 15  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 15 , &index ) ) group = 400;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_RUN_OUT(" , 16  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 16 , &index ) )	group = 401;
				else																group = 0;
			}

			else if ( STRNCMP_L( chr_return , "READY_RECIPE(" , 13  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 13 , &index ) )	group = 410;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "END_RECIPE(" , 11  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 11 , &index ) )	group = 411;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "READY_RECIPE_MINTIME(" , 21  ) ) { // 2005.08.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 21 , &index ) )	group = 420;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "END_RECIPE_MINTIME(" , 19  ) ) { // 2005.08.18
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 19 , &index ) )	group = 421;
				else																group = 0;
			}

			else if ( STRCMP_L( chr_return , "READY_MULTIJOB_MODE"    ) )			group = 451; // 2007.03.15

			else if ( STRCMP_L( chr_return , "CLEAR_OUT_CASS"    ) )				group = 500;
			else if ( STRNCMP_L( chr_return , "CLEAR_OUT(" , 10  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 10 , &index ) )	group = 501;
				else																 group = 0;
			}
			else if ( STRCMP_L( chr_return , "CLEAR_OUT_BM_SINGLE"    ) )			group = 502; // 2004.05.06

			else if ( STRCMP_L( chr_return , "CLEAR_MOVE_AL_TO_PM"    ) )			group = 503; // 2005.12.19
			else if ( STRCMP_L( chr_return , "CLEAR_MOVE_IC_TO_PM"    ) )			group = 504; // 2005.12.19

			else if ( STRCMP_L( chr_return , "MAINTINF_DB_USE"        ) )			group = 505; // 2012.05.04

			else if ( STRNCMP_L( chr_return , "FAIL_PROCESS_SCENARIO(" , 22  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 22 , &index ) ) group = 600;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "STOP_PROCESS_SCENARIO(" , 22  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 22 , &index ) )	group = 601;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MRES_FAIL_SCENARIO(" , 19  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 19 , &index ) )	group = 602;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MRES_ABORT_SCENARIO(" , 20  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) )	group = 603;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "MRES_SUCCESS_SCENARIO(" , 22  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 22 , &index ) )	group = 604;
				else																group = 0;
			}
			else if ( STRCMP_L( chr_return , "MRES_ABORT_ALL_SCENARIO" ) )			group = 605;

			else if ( STRCMP_L( chr_return , "DISABLE_MAKE_HOLE_GOTOSTOP" ) )		group = 606; // 2004.05.14

			else if ( STRNCMP_L( chr_return , "FIXED_CLUSTER_PRE_PART(" , 23  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 23 , &index ) ) group = 700;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "FIXED_CLUSTER_PRE_PART2(" , 24  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 24 , &index ) )	group = 701;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "FIXED_CLUSTER_POST_PART(" , 24  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 24 , &index ) ) group = 702;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "FIXED_CLUSTER_POST_PART2(" , 25  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 25 , &index ) )	group = 703;
				else																group = 0;
			}

			else if ( STRNCMP_L( chr_return , "DOUBLE_CHAMBER_WHAT_SIDE_USE(" , 29  ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 29 , &index ) )	group = 800;
				else																group = 0;
			}
			else if ( STRCMP_L( chr_return , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(A1)") ) group = 801;
			else if ( STRCMP_L( chr_return , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(A2)") ) group = 802;
			else if ( STRCMP_L( chr_return , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(B1)") ) group = 803;
			else if ( STRCMP_L( chr_return , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(B2)") ) group = 804;

			else if ( STRNCMP_L( chr_return , "BUFFER_CHAMBER_WHAT_SIDE_SWITCH" , 31   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 31 , &index , MAX_CASSETTE_SIDE ) )	group = 805;
				else																					group = 0;
			}
			else if ( STRNCMP_L( chr_return , "BUFFER_CHAMBER_ACCESS_SWITCH" , 28   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 28 , &index , MAX_CASSETTE_SIDE ) )	group = 806;
				else																					group = 0;
			}
			
			else if ( STRCMP_L( chr_return , "GUI_GROUP_SELECT"                 ) ) group = 900;

			else if ( STRCMP_L( chr_return , "SYSTEM_LOG_STYLE"                 ) ) group = 901; // 2006.02.07

			else if ( STRCMP_L( chr_return , "FA_LOADUNLOAD_SKIP"               ) ) group = 1000;
			else if ( STRCMP_L( chr_return , "FA_MAPPING_SKIP"                  ) ) group = 1001;
			else if ( STRCMP_L( chr_return , "FA_SINGLE_CASS_MULTI_RUN"         ) ) group = 1002;
			else if ( STRCMP_L( chr_return , "FA_STARTEND_STATUS_SHOW"			) ) group = 1003;
			else if ( STRCMP_L( chr_return , "FA_AUTO_HANDOFF_WINDOW_STATUS"	) ) group = 1004;
			else if ( STRCMP_L( chr_return , "FA_NORMALUNLOAD_NOTUSE"           ) ) group = 1005; // 2007.08.29

			else if ( STRCMP_L( chr_return , "FA_SYSTEM_MESSAGE_LOAD"			) ) group = 1100;
			else if ( STRCMP_L( chr_return , "FA_SYSTEM_MESSAGE_UNLOAD"			) ) group = 1101;
			else if ( STRCMP_L( chr_return , "FA_SYSTEM_MESSAGE_MAPPING"		) ) group = 1102;

			else if ( STRCMP_L( chr_return , "FA_WINDOW_NORMAL_CHECK"			) ) group = 1200;
			else if ( STRCMP_L( chr_return , "FA_WINDOW_ABORT_CHECK"			) ) group = 1201;
			else if ( STRCMP_L( chr_return , "FA_PROCESS_STEPCHANGE_CHECK"		) ) group = 1202;
			else if ( STRCMP_L( chr_return , "FA_SEND_MESSAGE_DISPLAY"			) ) group = 1203;
			else if ( STRCMP_L( chr_return , "FA_EXPAND_MESSAGE_USE"			) ) group = 1204;
			else if ( STRCMP_L( chr_return , "FA_SUBSTRATE_WAFER_ID"			) ) group = 1205;
			else if ( STRCMP_L( chr_return , "FA_REJECTMESSAGE_DISPLAY"			) ) group = 1206; // 2004.06.16

			else if ( STRCMP_L( chr_return , "FA_SERVER"				   ) ) group = 2000;
			else if ( STRNCMP_L( chr_return , "FA_AGV" , 6   ) ) {
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 6 , &index , MAX_CASSETTE_SIDE ) )	group = 2001;
				else																				group = 0;
			}
			else if ( STRCMP_L( chr_return , "FA_FUNCTION_INTERFACE"	   ) )	group = 2002;

			else if ( STRNCMP_L( chr_return , "CHAMBER_PM_INTERLOCK_RUN_FOR_" , 29  ) ) {
				STR_SEPERATE_CHAR( chr_return + 29 , '(' , B1s , B2s , 31 );
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( B1s , &index ) ) {
					if ( ( index >= CM1 ) && ( index < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
						group = 3000 + index - CM1;
						if ( !FILE_SUB_CHECK_MODULE_STRING_ALL( B2s , &index ) ) group = 0;
					}
					else {
						group = 0;
					}
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_SLOT_FROM_CM_OFFSET_" , 28 ) ) {
				STR_SEPERATE_CHAR( chr_return + 28 , '_' , B1s , B2s , 31 );
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( B1s , &index ) ) {
					if ( ( index >= CM1 ) && ( index < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
						group = 3100 + index - CM1;
						if ( FILE_SUB_CHECK_MODULE_STRING_ALL( B2s , &index ) ) {
							if      ( index <  AL ) group = 0;
							else if ( index == TM ) group = 0; 
							else if ( index == FM ) group = 0; 
						}
						else {
							group = 0;
						}
					}
					else {
						group = 0;
					}
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_SLOT_FROM_ALL_OFFSET_" , 29 ) ) { // 2002.08.27
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 29 , &index ) )	group = 3199;
				else                                                                group = 0;
			}
			else if ( STRCMP_L( chr_return , "DUMMY_PROCESS_MODE" ) ) { group = 3200; }
			else if ( STRNCMP_L( chr_return , "DUMMY_PROCESS_NOT_DEPAND_CHAMBER(" , 33 ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 33 , &index ) )	group = 3201;
				else																group = 0;
			}
			else if ( STRCMP_L( chr_return , "DUMMY_PROCESS_FIXEDCLUSTER" ) ) { group = 3202; }
			else if ( STRCMP_L( chr_return , "DUMMY_PROCESS_MULTILOTACCESS" ) ) { group = 3203; }
			else if ( STRNCMP_L( chr_return , "PRE_RECEIVE_WITH_PROCESS_RECIPE(" , 32 ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 32 , &index ) )	group = 3300;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_PM_PICK_DENY_FOR_" , 25 ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 25 , &index ) ) {
//					if ( ( index >= CM1 ) && ( index < ( MAX_CASSETTE_SIDE + CM1 ) ) ) { // 2003.02.05
						group = 3400;
//					}
//					else {
//						group = 0;
//					}
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_PM_PLACE_DENY_FOR_" , 26 ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 26 , &index ) ) {
//					if ( ( index >= CM1 ) && ( index < ( MAX_CASSETTE_SIDE + CM1 ) ) ) { // 2003.02.05
						group = 3401;
//					}
//					else {
//						group = 0;
//					}
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_ROBOT_MULTI_DENY_FOR_ARM_" , 33 ) ) { // 2004.01.30
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 33 , &index ) ) {
					group = 3402;
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_FM_PICK_DENY_FOR_" , 25 ) ) { // 2006.09.26
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 25 , &index ) ) {
					group = 3403;
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_FM_PLACE_DENY_FOR_" , 26 ) ) { // 2006.09.26
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 26 , &index ) ) {
					group = 3404;
				}
				else {
					group = 0;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_TM_INTERLOCK_" , 21  ) ) { // 2002.04.27
				STR_SEPERATE_CHAR( chr_return + 21 , '(' , B1s , B2s , 31 );
				index = atoi(B1s) - 1;
				if ( ( index < 0 ) || ( index >= MAX_TM_CHAMBER_DEPTH ) ) {
					group = 0;
				}
				else {
					group = 3500 + index;
					if ( !FILE_SUB_CHECK_MODULE_STRING_ALL( B2s , &index ) ) group = 0;
				}
			}
			//
			else if ( STRNCMP_L( chr_return , "CHAMBER_TM_SINGLE_RUN_INTLKS_" , 29  ) ) { // 2007.01.15
				index = atoi(chr_return+29) - 1;
				if ( ( index < 0 ) || ( index >= MAX_TM_CHAMBER_DEPTH ) ) {
					group = 0;
				}
				else {
					group = 3600 + index;
				}
			}
			//
			else if ( STRCMP_L( chr_return , "INTERLOCK_FM_BM_PLACE_SEPARATE" ) ) { // 2007.11.15
				group = 3701;
			}
			else if ( STRCMP_L( chr_return , "INTERLOCK_TM_BM_OTHERGROUP_SWAP" ) ) { // 2013.04.25
				group = 3702;
			}
			//
			else if ( STRNCMP_L( chr_return , "CHAMBER_TM_PICK_ORDERING_" , 25  ) ) { // 2012.09.21
				index = atoi(chr_return+25) - 1;
				if ( ( index < 0 ) || ( index >= MAX_TM_CHAMBER_DEPTH ) ) {
					group = 0;
				}
				else {
					group = 3801;
				}
			}
			else if ( STRNCMP_L( chr_return , "CHAMBER_TM_PICK_ORDER_SKIP_" , 27 ) ) { // 2012.09.21
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 27 , &index ) ) {
					group = 3802;
				}
				else {
					group = 0;
				}
			}
			//
			else if ( STRCMP_L( chr_return , "CPJOB_DEFAULT_GROUP" ) ) {
				group = 4001;
			}
			else if ( STRCMP_L( chr_return , "CPJOB_OVERTIME_CHECK_HOUR" ) ) { // 2012.04.20
				group = 4002;
			}
			//
			else if ( STRCMP_L( chr_return , "MESSAGE_USE_WHEN_SWITCH" ) ) {
				group = 4100;
			}
			//
			else if ( STRCMP_L( chr_return , "MESSAGE_USE_WHEN_ORDER" ) ) { // 2004.06.24
				group = 4101;
			}
			//
			else if ( STRCMP_L( chr_return , "SW_BM_SCHEDULING_FACTOR" ) ) { // 2004.08.14
				group = 4102;
			}
			//
			else if ( STRNCMP_L( chr_return , "NEXT_FREE_PICK_POSSIBLE(" , 24 ) ) {
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 24 , &index ) )	group = 5001;
				else																group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PREV_FREE_PICK_POSSIBLE(" , 24 ) ) { // 2006.07.12
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 24 , &index ) )	group = 5002;
				else																group = 0;
			}
			//
			else if ( STRCMP_L( chr_return , "SDUMMY_PROCESS_DATA" ) ) { // 2002.05.11
				group = 5100; // Not Use from 2008.02.05
			}
			else if ( STRCMP_L( chr_return , "SDUMMY_PROCESS_RUN" ) ) { // 2002.05.11
				group = 5101; // Not Use from 2008.02.05
			}
			else if ( STRNCMP_L( chr_return , "ERROR_OUT_CHAMBER_FOR_CASSETTE_" , 31 ) ) { // 2002.07.05
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 31 , &index ) ) {
					if ( ( index >= CM1 ) && ( index < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
						group = 5200;
					}
					else {
						group = 0;
					}
				}
				else {
					group = 0;
				}
			}
			else if ( STRCMP_L( chr_return , "CM_SUPPLY_NOT_DELAY" ) ) { // 2002.10.02
				group = 5201;
			}
			else if ( STRCMP_L( chr_return , "PRE_PROCESS_DEPAND" ) ) { // 2003.01.08
				group = 5202;
			}
			else if ( STRCMP_L( chr_return , "START_PARALLEL_CHECK_SKIP" ) ) { // 2003.01.11
				group = 5203;
			}
			else if ( STRCMP_L( chr_return , "PROCESS_LOG_WITH_CPJOB" ) ) { // 2002.12.27
				group = 5300;
			}
			else if ( STRCMP_L( chr_return , "WAFER_SUPPLY_MODE" ) ) { // 2003.06.11
				group = 5301;
			}
			else if ( STRCMP_L( chr_return , "CPJOB_DEFAULT_LOTRECIPE_READ" ) ) { // 2003.09.24
				group = 5302;
			}
			else if ( STRCMP_L( chr_return , "CLUSTER_INCLUDE_FOR_DISABLE" ) ) { // 2003.10.09
				group = 5303;
			}
			else if ( STRCMP_L( chr_return , "BMEND_SKIP_WHEN_RUNNING" ) ) {  // 2004.02.19
				group = 5304;
			}
			else if ( STRCMP_L( chr_return , "LOT_LOG_PRE_POST_PROCESS" ) ) {  // 2004.05.11
				group = 5305;
			}
			else if ( STRCMP_L( chr_return , "CPJOB_MAKE_MANUAL_CJPJMODE" ) ) {  // 2004.05.11
				group = 5306;
			}
			else if ( STRCMP_L( chr_return , "CPJOB_JOBNAME_DISPLAYMODE" ) ) {  // 2005.05.11
				group = 5307;
			}
			else if ( STRCMP_L( chr_return , "PMREADY_SKIP_WHEN_RUNNING" ) ) {  // 2006.03.28
				group = 5308;
			}
			else if ( STRCMP_L( chr_return , "CPJOB_MESSAGE_SCENARIO" ) ) {  // 2006.04.13
				group = 5309;
			}
			else if ( STRCMP_L( chr_return , "BATCH_SUPPLY_INTERRUPT" ) ) { // 2006.09.05
				group = 5310;
			}
			else if ( STRCMP_L( chr_return , "DIFF_LOT_NOTSUP_MODE" ) ) { // 2007.07.05
				group = 5311;
			}
			else if ( STRCMP_L( chr_return , "ADAPTIVE_PROGRESSING" ) ) {  // 2008.02.29
				group = 5312;
			}
			else if ( STRNCMP_L( chr_return , "MFI_INTERFACE_FLOW_USER_OPTION(" , 31 ) ) { // 2013.08.01
				if ( FILE_SUB_FILE_SUB_CHECK_MODULE_STRING_ALL_ONLY_ID( chr_return + 31 , &index , 255 ) ) group = 5313;
				else                                                   group = 0;
			}
			else if ( STRCMP_L( chr_return , "RECIPE_LOCKING" ) ) { // 2013.09.13
				group = 5314;
			}
			else if ( STRCMP_L( chr_return , "METHOD1_TO_1BM_USING_MODE" ) ) { // 2018.03.21
				group = 5315;
			}
			//
			else if ( STRCMP_L( chr_return , "END_DELIMITER_CHECK"       ) ) group = 9001; // 2017.02.09
			//
			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 250 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			//
			//
			if ( SCHFILE_USE_END_DELIMITER ) { // 2017.02.09
				if ( ins_FILE_END_DELIMITER == 1 ) {
					if ( group > 0 ) {
						_IO_CIM_PRINTF( "END_DELIMITER Error - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;

					}
				}
			}
			//
			//
			switch(group) {
			case 0 :
				break;

			case 1 :
				if      ( STRCMP_L( chr_return , "Off"  ) ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 0 );
				else if ( STRCMP_L( chr_return , "On" ) ) 	_i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 1 );
				else if ( STRCMP_L( chr_return , "Off2" ) ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 2 );
				else if ( STRCMP_L( chr_return , "On2" ) ) 	_i_SCH_PRM_SET_MODULE_MODE_for_SW( index , 3 );
				else group = -1;
				break;

			case 2 : // 2002.11.01
				i = atoi(chr_return);
				if ( index == F_IC ) { // 2007.08.14
					if ( ( i > 0 ) && ( i <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) {
						if ( i != _i_SCH_PRM_GET_MODULE_SIZE( index ) ) {
							_i_SCH_PRM_SET_MODULE_SIZE( index , i );
							_i_SCH_PRM_SET_MODULE_SIZE_CHANGE( index , TRUE );
						}
					}
				}
				else {
					if ( ( i > 0 ) && ( i <= MAX_CASSETTE_SLOT_SIZE ) ) {
						if ( i != _i_SCH_PRM_GET_MODULE_SIZE( index ) ) {
							_i_SCH_PRM_SET_MODULE_SIZE( index , i );
							_i_SCH_PRM_SET_MODULE_SIZE_CHANGE( index , TRUE );
						}
					}
				}
				break;

			case 11 : // 2002.10.16
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPRECV( index , atoi(chr_return) );
				break;
			case 12 : // 2002.10.16
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPSEND( index , atoi(chr_return) );
				break;
			case 13 : // 2002.10.16
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTRECV( index , atoi(chr_return) );
				break;
			case 14 : // 2002.10.16
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTSEND( index , atoi(chr_return) );
				break;
			case 15 : // 2002.10.16
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_GATE( index , atoi(chr_return) );
				break;
			case 16 : // 2016.11.24
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_CANCEL( index , atoi(chr_return) );
				break;

			case 21 :
				if ( STRCMP_L( chr_return , "On"  ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( i , TRUE );
				}
				else if ( STRCMP_L( chr_return , "Off" ) ) {
					for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( i , FALSE );
				}
				else group = -1;
				break;

			case 22 :
				if      ( STRCMP_L( chr_return , "On"  ) ) { _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( index , TRUE );  }
				else if ( STRCMP_L( chr_return , "Off" ) ) { _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( index , FALSE ); }
				else                                       { _i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( index , atoi( chr_return ) ); }
				break;

			case 31 :	_i_SCH_PRM_SET_MODULE_PROCESS_NAME( index , chr_return );		break;

			case 41 :	_i_SCH_PRM_SET_PRCS_TIME_WAIT_RANGE( index , atoi( chr_return ) );			break;
			case 42 :	_i_SCH_PRM_SET_PRCS_TIME_REMAIN_RANGE( index , atoi( chr_return ) );			break; // 2006.05.04

			case 100 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_MAPPING_SKIP( CM1 + i , atoi( chr_return ) );	break;
			case 101 :	_i_SCH_PRM_SET_MAPPING_SKIP( index , atoi( chr_return ) );	break;

			case 110 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_AUTO_HANDOFF( CM1 + i , atoi( chr_return ) );	break;
			case 111 :	_i_SCH_PRM_SET_AUTO_HANDOFF( index , atoi( chr_return ) );	break;

			case 120 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_EXTEND_HANDOFF( CM1 + i , atoi( chr_return ) );	break;
			case 121 :	_i_SCH_PRM_SET_FA_EXTEND_HANDOFF( index , atoi( chr_return ) );	break;

			case 130 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_MID_READ_POINT( i , atoi( chr_return ) ); break;
			case 131 :	_i_SCH_PRM_SET_FA_MID_READ_POINT( index , atoi( chr_return ) );	break;

			case 140 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_JID_READ_POINT( i , atoi( chr_return ) ); break;
			case 141 :	_i_SCH_PRM_SET_FA_JID_READ_POINT( index , atoi( chr_return ) );	break;

			case 150 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_LOAD_COMPLETE_MESSAGE( i , atoi( chr_return ) );	break;
			case 151 :	_i_SCH_PRM_SET_FA_LOAD_COMPLETE_MESSAGE( index , atoi( chr_return ) );	break;

			case 160 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_UNLOAD_COMPLETE_MESSAGE( i , atoi( chr_return ) );	break;
			case 161 :	_i_SCH_PRM_SET_FA_UNLOAD_COMPLETE_MESSAGE( index , atoi( chr_return ) );	break;

			case 170 :	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_PRM_SET_FA_MAPPING_AFTERLOAD( i , atoi( chr_return ) );	break;
			case 171 :	_i_SCH_PRM_SET_FA_MAPPING_AFTERLOAD( index , atoi( chr_return ) );	break;
			
			case 200 :	_i_SCH_PRM_SET_UTIL_MAPPING_WHEN_TMFREE( atoi( chr_return ) );					break;
			case 201 :	_i_SCH_PRM_SET_UTIL_TM_GATE_SKIP_CONTROL( atoi( chr_return ) );				break;
			case 202 :	_i_SCH_PRM_SET_UTIL_TM_MOVE_SR_CONTROL( atoi( chr_return ) );					break;
			case 203 :	_i_SCH_PRM_SET_UTIL_FM_MOVE_SR_CONTROL( atoi( chr_return ) );					break;
			case 204 :	_i_SCH_PRM_SET_UTIL_SCH_START_OPTION( atoi( chr_return ) );					break;
			case 205 :	_i_SCH_PRM_SET_UTIL_LOOP_MAP_ALWAYS( atoi( chr_return ) );						break;
			case 206 :	_i_SCH_PRM_SET_UTIL_SCH_LOG_DIRECTORY_FORMAT( atoi( chr_return ) );			break;
			case 207 :	_i_SCH_PRM_SET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN( atoi( chr_return ) );		break;
			case 208 :	_i_SCH_PRM_SET_UTIL_FIRST_MODULE_PUT_DELAY_TIME( atoi( chr_return ) );			break;
			case 209 :	_i_SCH_PRM_SET_UTIL_ABORT_MESSAGE_USE_POINT( atoi( chr_return ) );				break;
			case 210 :	_i_SCH_PRM_SET_UTIL_CANCEL_MESSAGE_USE_POINT( atoi( chr_return ) );			break;
			case 211 :	_i_SCH_PRM_SET_UTIL_END_MESSAGE_SEND_POINT( atoi( chr_return ) );				break;
			case 212 :	_i_SCH_PRM_SET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME( atoi( chr_return ) );		break;
			case 213 :	_i_SCH_PRM_SET_UTIL_CHAMBER_PUT_PR_CHECK( atoi( chr_return ) );				break;

			case 214 :
						_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME( atoi( chr_return ) );
						_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME2( 0 ); // 2011.04.27
						//
						if ( ReadCnt > 2 ) { // 2011.04.27
							if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 260 - [%s]-[%d]\n" , Filename , Line );
								_lclose( hFile );
								return FALSE;
							}
							//
							_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME2( atoi( chr_return ) );
							//
						}
						//
						break;

			case 215 :	_i_SCH_PRM_SET_UTIL_FAL_MULTI_WAITTIME( atoi( chr_return ) );					break;

			case 216 :	_i_SCH_PRM_SET_UTIL_CHAMBER_GET_PR_CHECK( atoi( chr_return ) );				break;

			case 217 :	_i_SCH_PRM_SET_UTIL_FIC_MULTI_TIMEMODE( atoi( chr_return ) );					break; // 2005.12.01

			case 300 :
				j = atoi( chr_return );
				for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_Getting_Priority( i , j );
				for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_Putting_Priority( i , j );
				break;
			case 301 :
				j = atoi( chr_return );
				_i_SCH_PRM_SET_Getting_Priority( index , j );
				_i_SCH_PRM_SET_Putting_Priority( index , j );
				break;
			case 310 :
				j = atoi( chr_return );
				for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_Getting_Priority( i , j );
				break;
			case 311 :
				j = atoi( chr_return );
				_i_SCH_PRM_SET_Getting_Priority( index , j );
				break;
			case 320 :
				j = atoi( chr_return );
				for ( i = CM1 ; i < TM ; i++ ) _i_SCH_PRM_SET_Putting_Priority( i , j );
				break;
			case 321 :
				j = atoi( chr_return );
				_i_SCH_PRM_SET_Putting_Priority( index , j );
				break;

			case 400 :
				if ( STRCMP_L( chr_return , "RUN" ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 0 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 270 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "RUNA" ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 1 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 280 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "SIM" ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 2 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 290 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "SIMA" ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 3 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 300 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "NOTUSE" ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 4 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 310 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
					}
				}
				else {
					_i_SCH_PRM_SET_Process_Run_In_Mode( index , 2 );
					_i_SCH_PRM_SET_Process_Run_In_Time( index , atoi( chr_return ) );
				}
				break;

			case 401 :
				if ( STRCMP_L( chr_return , "RUN" ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 0 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 320 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "RUNA" ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 1 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 330 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "SIM" ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 2 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 340 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "SIMA" ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 3 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 350 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
					}
				}
				else if ( STRCMP_L( chr_return , "NOTUSE" ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 4 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , 0 );
					if ( ReadCnt > 2 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 360 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
					}
				}
				else {
					_i_SCH_PRM_SET_Process_Run_Out_Mode( index , 2 );
					_i_SCH_PRM_SET_Process_Run_Out_Time( index , atoi( chr_return ) );
				}
				break;

			case 410 :
				_i_SCH_PRM_SET_READY_RECIPE_NAME( index , "" );
				_i_SCH_PRM_SET_READY_RECIPE_USE( index , atoi( chr_return ) );

				_i_SCH_PRM_SET_READY_RECIPE_TYPE( index , 0 ); // 2017.10.16

				strcpy( Imsi_Buffer , "" ); // 2002.08.02
				for ( i = 2 ; i < ReadCnt ; i++ ) { // 2002.08.02
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 370 - [%s]-[%d][%d][%d]\n" , Filename , Line , ReadCnt , i );
						_lclose( hFile );
						return FALSE;
					}
					if ( i != 2 ) {
						strcat( Imsi_Buffer , " " );
					}
					strcat( Imsi_Buffer , chr_return );
				}
				if ( ReadCnt > 2 ) { // 2002.08.02
					_i_SCH_PRM_SET_READY_RECIPE_NAME( index , Imsi_Buffer );
				}
//				if ( ReadCnt > 2 ) { // 2002.08.02
//					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
//						_IO_CIM_PRINTF( "Unknown Error 380 - [%s]-[%d]\n" , Filename , Line );
//						_lclose( hFile );
//						return FALSE;
//					}
//					_i_SCH_PRM_SET_READY_RECIPE_NAME( index , chr_return );
//				}
				break;

			case 411 :
				_i_SCH_PRM_SET_END_RECIPE_NAME( index , "" );
				_i_SCH_PRM_SET_END_RECIPE_USE( index , atoi( chr_return ) );
				strcpy( Imsi_Buffer , "" ); // 2002.08.02
				for ( i = 2 ; i < ReadCnt ; i++ ) { // 2002.08.02
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 390 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( i != 2 ) {
						strcat( Imsi_Buffer , " " );
					}
					strcat( Imsi_Buffer , chr_return );
				}
				if ( ReadCnt > 2 ) { // 2002.08.02
					_i_SCH_PRM_SET_END_RECIPE_NAME( index , Imsi_Buffer );
				}
//				if ( ReadCnt > 2 ) { // 2002.08.02
//					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
//						_IO_CIM_PRINTF( "Unknown Error 400 - [%s]-[%d]\n" , Filename , Line );
//						_lclose( hFile );
//						return FALSE;
//					}
//					_i_SCH_PRM_SET_END_RECIPE_NAME( index , chr_return );
//				}
				break;

			case 420 : // 2005.08.18
				_i_SCH_PRM_SET_READY_RECIPE_MINTIME( index , atoi( chr_return ) );
				break;

			case 421 : // 2005.08.18
				_i_SCH_PRM_SET_END_RECIPE_MINTIME( index , atoi( chr_return ) );
				break;

			case 451 : // 2007.03.15
				_i_SCH_PRM_SET_READY_MULTIJOB_MODE( atoi( chr_return ) );
				break;

			case 500 :
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					_i_SCH_PRM_SET_CLSOUT_USE( CM1 + i , 1 );	_i_SCH_PRM_SET_CLSOUT_DELAY( CM1 + i , atoi( chr_return ) );
				}
				break;

			case 501 :
				_i_SCH_PRM_SET_CLSOUT_USE( index , atoi( chr_return ) );
				_i_SCH_PRM_SET_CLSOUT_DELAY( index , 0 );
				_i_SCH_PRM_SET_CLSOUT_MESSAGE_USE( index , 0 );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 410 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_CLSOUT_DELAY( index , atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 420 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_CLSOUT_MESSAGE_USE( index , atoi( chr_return ) );
					}
				}
				break;

			case 502 :
				_i_SCH_PRM_SET_CLSOUT_BM_SINGLE_ONLY( atoi( chr_return ) ); // 2004.05.06
				break;

			case 503 :
				_i_SCH_PRM_SET_CLSOUT_AL_TO_PM( atoi( chr_return ) ); // 2005.12.19
				break;

			case 504 :
				_i_SCH_PRM_SET_CLSOUT_IC_TO_PM( atoi( chr_return ) ); // 2005.12.19
				break;

			case 505 :
				_i_SCH_PRM_SET_MAINTINF_DB_USE( atoi( chr_return ) ); // 2012.05.04
				break;

			case 600 :	_i_SCH_PRM_SET_FAIL_PROCESS_SCENARIO( index , atoi( chr_return ) );	break;
			case 601 :	_i_SCH_PRM_SET_STOP_PROCESS_SCENARIO( index , atoi( chr_return ) );	break;
			case 602 :	_i_SCH_PRM_SET_MRES_FAIL_SCENARIO( index , atoi( chr_return ) );		break;
			case 603 :	_i_SCH_PRM_SET_MRES_ABORT_SCENARIO( index , atoi( chr_return ) );		break;
			case 604 :	_i_SCH_PRM_SET_MRES_SUCCESS_SCENARIO( index , atoi( chr_return ) );	break; // 2003.09.25
			case 605 :	_i_SCH_PRM_SET_MRES_ABORT_ALL_SCENARIO( atoi( chr_return ) );			break; // 2002.07.23
			case 606 :	_i_SCH_PRM_SET_DISABLE_MAKE_HOLE_GOTOSTOP( atoi( chr_return ) );		break; // 2004.05.14

			case 700 :
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_MODE( index , atoi( chr_return ) );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_USE( index , FALSE );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_RECIPE_NAME( index , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 430 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_USE( index , atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 440 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_RECIPE_NAME( index , chr_return );
					}
				}
				break;
			case 701 :
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_USE( index , FALSE );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( index , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 450 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_USE( index , atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 460 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( index , chr_return );
					}
				}
				break;
			case 702 :
				_i_SCH_PRM_SET_FIXCLUSTER_POST_MODE( index , atoi( chr_return ) );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_USE( index , FALSE );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_RECIPE_NAME( index , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 470 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_USE( index , atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 480 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_RECIPE_NAME( index , chr_return );
					}
				}
				break;
			case 703 :
				_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_USE( index , FALSE );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_RECIPE_NAME( index , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 490 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_USE( index , atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 500 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_RECIPE_NAME( index , chr_return );
					}
				}
				break;

			case 800 :	_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( index , atoi( chr_return ) );				break;

			case 801 :	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 0 , atoi( chr_return ) );			break;
			case 802 :	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 1 , atoi( chr_return ) );			break;
			case 803 :	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 0 , atoi( chr_return ) );			break;
			case 804 :	_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 1 , atoi( chr_return ) );			break;
			case 805 :	_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER( index , atoi( chr_return ) );	break;
			case 806 :	_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( index , atoi( chr_return ) );		break;

			case 900 :	GUI_GROUP_SELECT_SET( atoi( chr_return ) );									break;

			case 901 :	_i_SCH_PRM_SET_SYSTEM_LOG_STYLE( atoi( chr_return ) );								break; // 2006.02.07

			case 1000 :	_i_SCH_PRM_SET_FA_LOADUNLOAD_SKIP( atoi( chr_return ) );							break;
			case 1001 :	_i_SCH_PRM_SET_FA_MAPPING_SKIP( atoi( chr_return ) );								break;
			case 1002 :	_i_SCH_PRM_SET_FA_SINGLE_CASS_MULTI_RUN( atoi( chr_return ) );						break;
			case 1003 :	_i_SCH_PRM_SET_FA_STARTEND_STATUS_SHOW( atoi( chr_return ) );						break;
			case 1004 :	break;
			case 1005 :	_i_SCH_PRM_SET_FA_NORMALUNLOAD_NOTUSE( atoi( chr_return ) );						break; // 2007.08.29

			case 1100 :
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST( atoi( chr_return ) );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE( FALSE );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REJECT( FALSE );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 510 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE( atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 520 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REJECT( atoi( chr_return ) );
					}
				}
				break;

			case 1101 :
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST( atoi( chr_return ) );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE( FALSE );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT( FALSE );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 530 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE( atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 540 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT( atoi( chr_return ) );
					}
				}
				break;

			case 1102 :
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST( atoi( chr_return ) );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE( FALSE );
				_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT( FALSE );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 550 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE( atoi( chr_return ) );
					if ( ReadCnt > 3 ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
							_IO_CIM_PRINTF( "Unknown Error 560 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT( atoi( chr_return ) );
					}
				}
				break;

			case 1200 :
				_i_SCH_PRM_SET_FA_WINDOW_NORMAL_CHECK_SKIP( atoi( chr_return ) );
				break;

			case 1201 :
				_i_SCH_PRM_SET_FA_WINDOW_ABORT_CHECK_SKIP( atoi( chr_return ) );
				break;

			case 1202 :
				_i_SCH_PRM_SET_FA_PROCESS_STEPCHANGE_CHECK_SKIP( atoi( chr_return ) );
				break;

			case 1203 :
				_i_SCH_PRM_SET_FA_SEND_MESSAGE_DISPLAY( atoi( chr_return ) );
				break;

			case 1204 :
				_i_SCH_PRM_SET_FA_EXPAND_MESSAGE_USE( atoi( chr_return ) );
				break;

			case 1205 :
				_i_SCH_PRM_SET_FA_SUBSTRATE_WAFER_ID( atoi( chr_return ) );
				break;

			case 1206 :
				_i_SCH_PRM_SET_FA_REJECTMESSAGE_DISPLAY( atoi( chr_return ) );
				break;

			case 2000 :
				j = atoi( chr_return );
				strcpy( chr_return , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 570 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
				switch( j ) {
				case 0 : // disable
					FA_SERVER_FUNCTION_SET( 2 , chr_return );
					break;
				case 1 : // enable
					FA_SERVER_FUNCTION_SET( 1 , chr_return );
					break;
				case 2 :
					FA_SERVER_FUNCTION_SET( 0 , "" );
					break;
				}
				break;

			case 2001 :
				j = atoi( chr_return );
				strcpy( chr_return , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 580 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
				switch( j ) {
				case 0 : // disable
					FA_AGV_FUNCTION_SET( index , 2 , chr_return );
					break;
				case 1 : // enable
					FA_AGV_FUNCTION_SET( index , 1 , chr_return );
					break;
				case 2 :
					FA_AGV_FUNCTION_SET( index , 0 , "" );
					break;
				}
				break;

			case 2002 :
				FA_FUNCTION_INTERFACE_SET( atoi( chr_return ) );
				break;

			case 3000 :
			case 3001 :
			case 3002 :
			case 3003 :
			case 3004 :
			case 3005 :
			case 3006 :
			case 3007 :
			case 3008 :
			case 3009 :
			case 3010 :
			case 3011 :
			case 3012 :
			case 3013 :
			case 3014 :
			case 3015 :
			case 3016 :
			case 3017 :
			case 3018 :
			case 3019 :
			case 3020 :
			case 3021 :
			case 3022 :
			case 3023 :
			case 3024 :
			case 3025 :
			case 3026 :
			case 3027 :
			case 3028 :
			case 3029 :
				_i_SCH_PRM_SET_INTERLOCK_PM_RUN_FOR_CM( group - 3000 + CM1 , index , atoi( chr_return ) );
				break;

			case 3100 :
			case 3101 :
			case 3102 :
			case 3103 :
			case 3104 :
			case 3105 :
			case 3106 :
			case 3107 :
			case 3108 :
			case 3109 :
			case 3110 :
			case 3111 :
			case 3112 :
			case 3113 :
			case 3114 :
			case 3115 :
			case 3116 :
			case 3117 :
			case 3118 :
			case 3119 :
			case 3120 :
			case 3121 :
			case 3122 :
			case 3123 :
			case 3124 :
			case 3125 :
			case 3126 :
			case 3127 :
			case 3128 :
			case 3129 :
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( group - 3100 + CM1 , index , atoi( chr_return ) );
				break;

			case 3199 : // 2002.08.27
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( index , atoi( chr_return ) );
				break;

			case 3200 :
				_i_SCH_PRM_SET_DUMMY_PROCESS_MODE( atoi( chr_return ) );
				break;
			case 3201 :
				_i_SCH_PRM_SET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( index , atoi( chr_return ) );
				break;
			case 3202 :
				_i_SCH_PRM_SET_DUMMY_PROCESS_FIXEDCLUSTER( atoi( chr_return ) );
				break;
			case 3203 : // 2003.02.08
				_i_SCH_PRM_SET_DUMMY_PROCESS_MULTI_LOT_ACCESS( atoi( chr_return ) );
				break;

			case 3300 :
				_i_SCH_PRM_SET_PRE_RECEIVE_WITH_PROCESS_RECIPE( index , atoi( chr_return ) );
				break;

			case 3400 :
				//
				_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( index , atoi( chr_return ) );
				//
				for ( j = 2 ; j < ReadCnt; j++ ) { // 2013.05.28
					//
					if ( (j-1) >= MAX_TM_CHAMBER_DEPTH ) break;
					//
					if ( !Get_Data_From_String( Buffer , chr_return , j ) ) {
						_IO_CIM_PRINTF( "Unknown Error 3400 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j - 1 , index , atoi( chr_return ) );
					//
				}
				//
				break;

			case 3401 :
				//
				_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( index , atoi( chr_return ) );
				//
				for ( j = 2 ; j < ReadCnt; j++ ) { // 2013.05.28
					//
					if ( (j-1) >= MAX_TM_CHAMBER_DEPTH ) break;
					//
					if ( !Get_Data_From_String( Buffer , chr_return , j ) ) {
						_IO_CIM_PRINTF( "Unknown Error 3401 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j - 1 , index , atoi( chr_return ) );
					//
				}
				//
				break;

			case 3402 : // 2004.01.30
				_i_SCH_PRM_SET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( index , atoi( chr_return ) );
				break;

			case 3403 : // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( index , atoi( chr_return ) );
				break;

			case 3404 : // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( index , atoi( chr_return ) );
				break;

			case 3500 :
			case 3501 :
			case 3502 :
			case 3503 :
			case 3504 :
			case 3505 :
			case 3506 :
			case 3507 :
			case 3508 :
			case 3509 :
			case 3510 :
			case 3511 :
			case 3512 :
			case 3513 :
			case 3514 :
			case 3515 :
			case 3516 :
			case 3517 :
			case 3518 :
			case 3519 :
				_i_SCH_PRM_SET_INTERLOCK_TM_RUN_FOR_ALL( group - 3500 , index , atoi( chr_return ) );
				break;

			case 3600 :
			case 3601 :
			case 3602 :
			case 3603 :
			case 3604 :
			case 3605 :
			case 3606 :
			case 3607 :
			case 3608 :
			case 3609 :
			case 3610 :
			case 3611 :
			case 3612 :
			case 3613 :
			case 3614 :
			case 3615 :
			case 3616 :
			case 3617 :
			case 3618 :
			case 3619 :
				_i_SCH_PRM_SET_INTERLOCK_TM_SINGLE_RUN( group - 3600 , atoi( chr_return ) ); // 2007.01.16
				break;

			case 3701 : // 2007.11.05
				_i_SCH_PRM_SET_INTERLOCK_FM_BM_PLACE_SEPARATE( atoi( chr_return ) );
				break;

			case 3702 : // 2013.04.25
				_i_SCH_PRM_SET_INTERLOCK_TM_BM_OTHERGROUP_SWAP( atoi( chr_return ) );
				break;

			case 3801 : // 2012.09.12
				_i_SCH_PRM_SET_MODULE_PICK_ORDERING( index , atoi( chr_return ) );
				break;

			case 3802 : // 2012.09.12
				_i_SCH_PRM_SET_MODULE_PICK_ORDERING_SKIP( index , atoi( chr_return ) );
				break;

			case 4001 :
				strcpy( chr_return , "" );
				if ( ReadCnt > 2 ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 590 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
				}
				SCHMULTI_DEFAULT_GROUP_SET( chr_return );
				break;

			case 4002 : // 2012.04.20
				SCHMULTI_CHECK_OVERTIME_HOUR_SET( atoi( chr_return ) );
				break;

			case 4100 : // 2002.09.05
				_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_SWITCH( atoi( chr_return ) );
				break;

			case 4101 : // 2004.06.24
				_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_ORDER( atoi( chr_return ) );
				break;

			case 4102 : // 2004.08.14
				_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( atoi( chr_return ) );
				//
				if ( ReadCnt > 2 ) { // 2017.04.21
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
						_IO_CIM_PRINTF( "Unknown Error 590 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					SCH_SW_BM_SCHEDULING_FACTOR_ORDER_SET( chr_return );
					//
					j = strlen( chr_return );
					//
					for ( i = 0 ; i < j ; i++ ) {
						//
						if ( i >= MAX_BM_CHAMBER_DEPTH ) break;
						//
						if ( ( chr_return[i] >= '0' ) && ( chr_return[i] <= '9' ) ) {
							_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( BM1 + i , chr_return[i] - '0' );
						}
						//
					}
					//
				}
				//
				break;

			case 5001 :
				_i_SCH_PRM_SET_NEXT_FREE_PICK_POSSIBLE( index , atoi( chr_return ) );
				break;

			case 5002 :
				_i_SCH_PRM_SET_PREV_FREE_PICK_POSSIBLE( index , atoi( chr_return ) );
				break;

			case 5100 : // 2002.05.11 // 2002.06.17 // Not Use 2008.02.05
				break;
			case 5101 : // 2002.05.11 // Not Use 2008.02.05
				break;

			case 5200 : // 2002.07.10
				i = atoi( chr_return ) - 1;
				if ( ( i >= 0 ) && ( i < MAX_BM_CHAMBER_DEPTH ) ) {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , i + BM1 ) == BUFFER_OUT_CASSETTE ) {
						_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE( index , i + BM1 );
						_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( index , 0 );
						_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( index , 0 );
						if ( ReadCnt >= 3 ) {
							if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
								_IO_CIM_PRINTF( "Unknown Error 600 - [%s]-[%d]\n" , Filename , Line );
								_lclose( hFile );
								return FALSE;
							}
							i = atoi( chr_return );
							_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( index , i );
							if ( ReadCnt >= 4 ) {
								if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
									_IO_CIM_PRINTF( "Unknown Error 610 - [%s]-[%d]\n" , Filename , Line );
									_lclose( hFile );
									return FALSE;
								}
								i = atoi( chr_return );
								_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( index , i );
							}
						}
					}
				}
				break;
			case 5201 : // 2002.10.02
				_i_SCH_PRM_SET_UTIL_CM_SUPPLY_MODE( atoi( chr_return ) );
				break;

			case 5202 : // 2003.01.08
				_i_SCH_PRM_SET_UTIL_PREPOST_PROCESS_DEPAND( atoi( chr_return ) );
				break;

			case 5203 : // 2003.01.11
				_i_SCH_PRM_SET_UTIL_START_PARALLEL_CHECK_SKIP( atoi( chr_return ) );
				break;

			case 5300 : // 2002.12.27
				PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( atoi( chr_return ) );
				break;

			case 5301 : // 2003.06.11
				_i_SCH_PRM_SET_UTIL_WAFER_SUPPLY_MODE( atoi( chr_return ) );
				break;

			case 5302 : // 2003.09.24
//				SCHMULTI_DEFAULT_LOTRECIPE_READ_SET( atoi( chr_return ) ); // 2014.06.23
				_i_SCH_MULTIJOB_SET_LOTRECIPE_READ( atoi( chr_return ) ); // 2014.06.23
				break;

			case 5303 : // 2003.10.09
				_i_SCH_PRM_SET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE( atoi( chr_return ) ); // 2003.10.09
				break;

			case 5304 : // 2004.02.19
				_i_SCH_PRM_SET_UTIL_BMEND_SKIP_WHEN_RUNNING( atoi( chr_return ) ); // 2003.10.09
				break;

			case 5305 : // 2004.05.11
				_i_SCH_PRM_SET_UTIL_LOT_LOG_PRE_POST_PROCESS( atoi( chr_return ) );
				break;

			case 5306 : // 2004.05.11
				SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_SET( atoi( chr_return ) );
				break;

			case 5307 : // 2005.05.11
				SCHMULTI_JOBNAME_DISPLAYMODE_SET( atoi( chr_return ) );
				break;

			case 5308 : // 2006.03.28
				_i_SCH_PRM_SET_UTIL_PMREADY_SKIP_WHEN_RUNNING( atoi( chr_return ) );
				break;

			case 5309 : // 2006.04.13
				SCHMULTI_MESSAGE_SCENARIO_SET( atoi( chr_return ) );
				break;

			case 5310 : // 2006.09.05
				_i_SCH_PRM_SET_BATCH_SUPPLY_INTERRUPT( atoi( chr_return ) );
				break;

			case 5311 : // 2007.07.05
				_i_SCH_PRM_SET_DIFF_LOT_NOTSUP_MODE( atoi( chr_return ) );
				break;

			case 5312 : // 2008.02.29
				_i_SCH_PRM_SET_UTIL_ADAPTIVE_PROGRESSING( atoi( chr_return ) );
				break;

			case 5313 : // 2013.08.01
				if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( index + 1 ) == 2 ) {
					_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTION( index + 1 , atoi( chr_return ) );
				}
				break;

			case 5314 : // 2013.09.13
				//
				i = atoi( chr_return );
				//
				if ( !_i_SCH_PRM_GET_DFIX_RECIPE_LOCKING_MODE() ) {
					if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == i ) {
						break;
					}
				}
				//
				_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING_MODE( TRUE );
				_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( i );
				break;

			case 5315 : // 2018.03.21
				_i_SCH_PRM_SET_METHOD1_TO_1BM_USING_MODE( atoi( chr_return ) );
				break;

			case 9001 : // 2017.02.09
				//
				ins_FILE_END_DELIMITER = 1;
				//
				break;

			}

			if ( group == -1 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "[SKIP] Unknown Parameter or Downgrade Version(%s) - [%s]-[%d]\n" , Buffer , Filename , Line );
			}
		}
	}
	_lclose( hFile );
	//
	if ( SCHFILE_USE_END_DELIMITER ) { // 2017.02.09
		if ( ins_FILE_END_DELIMITER == 0 ) return FALSE;
	}
	//
	_SCH_PRM_MAKE_MODULE_PICK_ORDERING(); // 2012.09.21
	//
	return TRUE;
}



BOOL FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( char *Filename_Org ) { // 2017.11.02
	HFILE hFile;
	char Buffer[256];
	char chr_return[256];
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;
	char Filename[256];
	//
	File_Buffering_For_Open( Filename_Org , Filename );
	//
	hFile = _lopen( Filename , OF_READ );
	//
	if ( hFile == -1 ) return FALSE;
	//
	for ( Line = 1 ; FileEnd ; Line++ ) {
		//
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt );
		//
		if ( ReadCnt >= 2 ) {
			if ( ReadCnt < 2 ) {
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_lclose( hFile );
				return FALSE;
			}
			//
			if ( STRCMP_L( chr_return , "END_DELIMITER_CHECK"       ) ) {
				_lclose( hFile );
				return TRUE;
			}
			//
		}
	}
	_lclose( hFile );
	return FALSE;
}



BOOL FILE_SCHEDULE_SETUP( char *Filename_Org ) { // 2017.02.09
	//
	char FileName[256];
	//
	if ( FILE_SCHEDULE_SETUP_SUB( Filename_Org ) ) return TRUE;
	//
	if ( SCHFILE_USE_END_DELIMITER ) {
		//
		sprintf( FileName , "%s.bak" , Filename_Org );
		//
		if ( FILE_SCHEDULE_SETUP_SUB( FileName ) ) {
			//
			FILE_SCHEDULE_SETUP_SAVE( Filename_Org , FALSE );
			//
			return TRUE;
		}
		//
	}
	//
	return FALSE;
	//
}
//-------------------------------------------------------------------------
// Function = FILE_FILE_SETUP
//-------------------------------------------------------------------------
BOOL FILE_FILE_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[64];
	int  group , index;
	int  i , ReadCnt , Line;
	BOOL FileEnd = TRUE;

	//=========================================================
	// 2007.03.07
	//=========================================================
	// 2012.08.22
//	_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATH( "" );
	//
	_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATHF( "" );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATH( i , "" );
		_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATHM( i , "" );
	}
	//
	_i_SCH_PRM_SET_DFIX_LOT_RECIPE_PATH( "" );
	_i_SCH_PRM_SET_DFIX_CLUSTER_RECIPE_PATH( "" );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_PATHF( i , "" ); // 2012.08.22
		_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_PATH( i , "" );
		_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_TYPE( i , 0 );
		_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_FILE( i , 0 );
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2014.02.08
		_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING_SKIP( i , FALSE );
	}
	//=========================================================
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , Filename );
		return FALSE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt > 0 ) {
			//
//			if ( ReadCnt != 2 ) { // 2014.02.13
			if ( ReadCnt < 2 ) { // 2014.02.13
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Parameter Error[%s] - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 700 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			group = -1;
			index = -1;
			if      ( STRCMP_L( chr_return , "MAIN_RECIPE_PATH"       ) ) group = 1;							// M6.2.1
			else if ( STRCMP_L( chr_return , "LOT_RECIPE_PATH"        ) ) group = 2;							// M6.2.2
			else if ( STRCMP_L( chr_return , "CLUSTER_RECIPE_PATH"    ) ) group = 3;							// M6.2.3

			else if ( STRNCMP_L( chr_return , "PROCESS_RECIPE_PATH(" , 20 ) ) {									// M6.2.4
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 4;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_RECIPE_TYPE(" , 20 ) ) {	// 2007.03.07					//
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 5;
				else                                                   group = 0;
			}
			else if ( STRNCMP_L( chr_return , "PROCESS_RECIPE_FILE(" , 20 ) ) {	// 2007.06.19					//
				if ( FILE_SUB_CHECK_MODULE_STRING_ALL( chr_return + 20 , &index ) ) group = 6;
				else                                                   group = 0;
			}

			else if ( STRCMP_L( chr_return , "RECIPE_LOCKING"         ) ) group = 7;							// M6.2.5

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 710 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			switch( group ) {
			case 0 : break;
			case 1 :	_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATHF( chr_return );		break;
			case 2 :	_i_SCH_PRM_SET_DFIX_LOT_RECIPE_PATH( chr_return );			break;
			case 3 :	_i_SCH_PRM_SET_DFIX_CLUSTER_RECIPE_PATH( chr_return );		break;
			case 4 :	_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_PATH( index , chr_return );	break;
			case 5 :
				if      ( STRCMP_L( chr_return , "Name"    ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_TYPE( index , 1 );
				else if ( STRCMP_L( chr_return , "NameAll" ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_TYPE( index , 2 );
				else if ( STRCMP_L( chr_return , "File"    ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_TYPE( index , 0 );
				else                                           _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_TYPE( index , 0 );
				break;
			case 6 : // 2007.06.19
				if      ( STRCMP_L( chr_return , "Default" ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_FILE( index , 0 );
				else if ( STRCMP_L( chr_return , "Text"    ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_FILE( index , 1 );
				else if ( STRCMP_L( chr_return , "Source"  ) ) _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_FILE( index , 2 );
				else                                           _i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_FILE( index , 0 );
				break;
			case 7 :
				//
				_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING_MODE( FALSE ); // 2013.09.13
				//
				if      ( STRCMP_L( chr_return , "On(S)"      ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 3 ); // 2015.07.20
				else if ( STRCMP_L( chr_return , "Lock(S)"    ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 3 ); // 2015.07.20
				else if ( STRCMP_L( chr_return , "Locking(S)" ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 3 ); // 2015.07.20
				else if ( STRCMP_L( chr_return , "On(A)"      ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 2 );
				else if ( STRCMP_L( chr_return , "Lock(A)"    ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 2 );
				else if ( STRCMP_L( chr_return , "Locking(A)" ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 2 );
				else if ( STRCMP_L( chr_return , "On"         ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 1 );
				else if ( STRCMP_L( chr_return , "Lock"       ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 1 );
				else if ( STRCMP_L( chr_return , "Locking"    ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 1 );
				else if ( STRCMP_L( chr_return , "Off"        ) ) _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 0 );
				else                                              _i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( 0 );
				//
				for ( i = 2 ; i < ReadCnt ; i++ ) { // 2014.02.08
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 711 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					//
					index = atoi( chr_return );
					//
					if ( ( index >= 0 ) && ( index < MAX_CASSETTE_SIDE ) ) {
						_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING_SKIP( index , TRUE );
					}
					//
				}
				break;
			}
			if ( group == -1 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
		}
	}
	_lclose( hFile );

	for ( Line = 0 ; Line < MAX_CASSETTE_SIDE ; Line++ ) _i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( Line , "." );

	// 2012.08.22
	//
	for ( index = 0 ; index < MAX_CHAMBER ; index++ ) {
		//
		_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATH( index , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
		_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATHM( index , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
		_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_PATHF( index , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH(index) );
		//
	}
	//
	for ( index = 0 ; index < MAX_CHAMBER ; index++ ) {
		if ( ( tolower( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( index )[0] ) >= 'a' ) && ( tolower( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( index )[0] ) <= 'z' ) ) {
			if ( tolower( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( index )[1] ) == ':' ) {
				//
				sprintf( chr_return , "%s/%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( index ) , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
				//
				_i_SCH_PRM_SET_DFIX_PROCESS_RECIPE_PATH( index , "." );
				//
				_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATH( index , chr_return );
				//
				UTIL_CHANGE_FILE_VALID_STYLE( chr_return );
				_i_SCH_PRM_SET_DFIX_MAIN_RECIPE_PATHM( index , chr_return );
				//
			}
		}
	}
	//
	return TRUE;
}


//-------------------------------------------------------------------------
// Function = FILE_ROBOT_SETUP
//-------------------------------------------------------------------------

int ROBOT_AUTO_MULTI_UPDOWN = 0; // 2018.04.30 // 0:x 1:FM/TM 2:FM 3:TM


//BOOL FILE_ROBOT_SETUP( char *Filename ) { // 2015.01.25
BOOL FILE_ROBOT_SETUP( char *Filename_Org ) { // 2015.01.25
	HFILE hFile;
	char Buffer[2048];
	char chr_return[64];
	int  group , i , j , Chamber , Index , Side , tm;
	int  fmode , fcm , fbm , fpm , ftm , cmcnt; // 2010.10.20
	int  ReadCnt , Line;
	double f;
	BOOL FileEnd = TRUE;
	char Filename[256]; // 2015.01.23

	int startal; // 2005.12.14

	fmode = 0; // 2010.10.20

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		_i_SCH_PRM_SET_RB_HOME_POSITION( i , 0 );
		_i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i , 1 );
		_i_SCH_PRM_SET_RB_SYNCH_CHECK( i , FALSE );
		_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( i , FALSE );
		_i_SCH_PRM_SET_RB_ROTATION_STYLE( i , ROTATION_STYLE_HOME_CCW );
		//
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i , j , ( j == 0 ) ? TRUE : FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i , j , ( j == 0 ) ? TRUE : FALSE );
		}
		//
		_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( i , FALSE );
		//
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			for ( tm = 0 ; tm < 6 ; tm++ ) {
				_i_SCH_PRM_SET_RB_RNG(  i , j , tm , 0.5 );
			}
		}
	}
	//================================================================================================================
	//================================================================================================================
	//================================================================================================================
	//================================================================================================================
	for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
		_i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( i , TRUE );
		//
		_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( i , TRUE );
		//
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( i , j , ( j == 0 ) ? TRUE : FALSE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( i , j , ( j == 0 ) ? TRUE : FALSE );
		}
		//
		_i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( i , 0 );
		_i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( i , ROTATION_STYLE_HOME_CCW );
		_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( i , 0 ); // 2003.01.15
		//================================================================================================================
		_i_SCH_PRM_SET_RB_FM_HOME_POSITION( i , 0 );
		//================================================================================================================
		for ( j = 0 ; j < 6 ; j++ ) _i_SCH_PRM_SET_RB_FM_RNG( i , j , 0.5 );
		//================================================================================================================
	}
	//================================================================================================================
	//================================================================================================================
	//================================================================================================================
	//
	File_Buffering_For_Open( Filename_Org , Filename ); // 2015.01.23
	//
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , Filename );
		return TRUE;
	}
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 2040 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt > 0 ) {
			if ( ReadCnt < 2 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Parameter Error[%s] - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 720 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			group = -1;
			if      ( STRCMP_L( chr_return , "ROBOT_ANIMATION"      ) ) group = 1;
			else if ( STRCMP_L( chr_return , "ROBOT_SYNCH_CHECK"    ) ) group = 2;
			else if ( STRCMP_L( chr_return , "ROTATION_STYLE"       ) ) group = 3;
			else if ( STRCMP_L( chr_return , "ARM_STYLE"            ) ) group = 4;
			else if ( STRCMP_L( chr_return , "ARM_SEPERATE"         ) ) group = 5;
			else if ( STRCMP_L( chr_return , "ARM_DISPLAY"          ) ) group = 6;
			else if ( STRCMP_L( chr_return , "HOME_POSITION"        ) ) group = 7;
			else if ( STRCMP_L( chr_return , "HOME"                 ) ) group = 7;

			else if ( STRCMP_L( chr_return , "ROBOT_MODULE"         ) ) group = 99; // 2010.10.20
			//
			else if ( STRCMP_L( chr_return , "ROBOT_AUTO_MULTI_UPDOWN" ) ) group = 100; // 2018.04.30

			else if ( STRCMP_L( chr_return , "ROBOT_FM_ANIMATION"   ) ) { group = 11; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROBOT_FM_SYNCH_CHECK" ) ) { group = 12; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROTATION_FM_STYLE"    ) ) { group = 13; tm = 0; }
			else if ( STRCMP_L( chr_return , "ARM_FM_STYLE"         ) ) { group = 14; tm = 0; }
			else if ( STRCMP_L( chr_return , "ARM_FM_SEPERATE"      ) ) { group = 15; tm = 0; }
			else if ( STRCMP_L( chr_return , "ARM_FM_DISPLAY"       ) ) { group = 16; tm = 0; }
			else if ( STRCMP_L( chr_return , "HOME_FM_POSITION"     ) ) { group = 17; tm = 0; }
			else if ( STRCMP_L( chr_return , "HOME_FM"              ) ) { group = 17; tm = 0; }
			else if ( STRCMP_L( chr_return , "ARMB_FM_DUAL"         ) ) { group = 18; tm = 0; }

			else if ( STRCMP_L( chr_return , "ROBOT_FM2_ANIMATION"  ) ) { group = 11; tm = 1; }
			else if ( STRCMP_L( chr_return , "ROBOT_FM2_SYNCH_CHECK") ) { group = 12; tm = 1; }
			else if ( STRCMP_L( chr_return , "ROTATION_FM2_STYLE"   ) ) { group = 13; tm = 1; }
			else if ( STRCMP_L( chr_return , "ARM_FM2_STYLE"        ) ) { group = 14; tm = 1; }
			else if ( STRCMP_L( chr_return , "ARM_FM2_SEPERATE"     ) ) { group = 15; tm = 1; }
			else if ( STRCMP_L( chr_return , "ARM_FM2_DISPLAY"      ) ) { group = 16; tm = 1; }
			else if ( STRCMP_L( chr_return , "HOME_FM2_POSITION"    ) ) { group = 17; tm = 1; }
			else if ( STRCMP_L( chr_return , "HOME_FM2"             ) ) { group = 17; tm = 1; }
			else if ( STRCMP_L( chr_return , "ARMB_FM2_DUAL"        ) ) { group = 18; tm = 1; }

			else if ( STRCMP_L( chr_return , "ROTATE_FM"         ) ) { group = 22; Side = 0; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND_FM"         ) ) { group = 22; Side = 0; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT_FM"        ) ) { group = 22; Side = 0; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP_FM"             ) ) { group = 22; Side = 0; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN_FM"           ) ) { group = 22; Side = 0; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE_FM"           ) ) { group = 22; Side = 0; Index = 6; tm = 0; }

			else if ( STRCMP_L( chr_return , "ROTATE_FM2"        ) ) { group = 22; Side = 0; Index = 1; tm = 1; }
			else if ( STRCMP_L( chr_return , "EXTEND_FM2"        ) ) { group = 22; Side = 0; Index = 2; tm = 1; }
			else if ( STRCMP_L( chr_return , "RETRACT_FM2"       ) ) { group = 22; Side = 0; Index = 3; tm = 1; }
			else if ( STRCMP_L( chr_return , "UP_FM2"            ) ) { group = 22; Side = 0; Index = 4; tm = 1; }
			else if ( STRCMP_L( chr_return , "DOWN_FM2"          ) ) { group = 22; Side = 0; Index = 5; tm = 1; }
			else if ( STRCMP_L( chr_return , "MOVE_FM2"          ) ) { group = 22; Side = 0; Index = 6; tm = 1; }

			else if ( STRCMP_L( chr_return , "ROTATE"            ) ) { group = 21; Side = 0; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND"            ) ) { group = 21; Side = 0; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT"           ) ) { group = 21; Side = 0; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP"                ) ) { group = 21; Side = 0; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN"              ) ) { group = 21; Side = 0; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE"              ) ) { group = 21; Side = 0; Index = 6; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROTATE_A"          ) ) { group = 21; Side = 0; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND_A"          ) ) { group = 21; Side = 0; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT_A"         ) ) { group = 21; Side = 0; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP_A"              ) ) { group = 21; Side = 0; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN_A"            ) ) { group = 21; Side = 0; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE_A"            ) ) { group = 21; Side = 0; Index = 6; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROTATE_B"          ) ) { group = 21; Side = 1; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND_B"          ) ) { group = 21; Side = 1; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT_B"         ) ) { group = 21; Side = 1; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP_B"              ) ) { group = 21; Side = 1; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN_B"            ) ) { group = 21; Side = 1; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE_B"            ) ) { group = 21; Side = 1; Index = 6; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROTATE_C"          ) ) { group = 21; Side = 2; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND_C"          ) ) { group = 21; Side = 2; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT_C"         ) ) { group = 21; Side = 2; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP_C"              ) ) { group = 21; Side = 2; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN_C"            ) ) { group = 21; Side = 2; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE_C"            ) ) { group = 21; Side = 2; Index = 6; tm = 0; }
			else if ( STRCMP_L( chr_return , "ROTATE_D"          ) ) { group = 21; Side = 3; Index = 1; tm = 0; }
			else if ( STRCMP_L( chr_return , "EXTEND_D"          ) ) { group = 21; Side = 3; Index = 2; tm = 0; }
			else if ( STRCMP_L( chr_return , "RETRACT_D"         ) ) { group = 21; Side = 3; Index = 3; tm = 0; }
			else if ( STRCMP_L( chr_return , "UP_D"              ) ) { group = 21; Side = 3; Index = 4; tm = 0; }
			else if ( STRCMP_L( chr_return , "DOWN_D"            ) ) { group = 21; Side = 3; Index = 5; tm = 0; }
			else if ( STRCMP_L( chr_return , "MOVE_D"            ) ) { group = 21; Side = 3; Index = 6; tm = 0; }

			else if ( STRNCMP_L( chr_return , "ROTATE_T"    , 8  ) ) { group = 21; Side = 0; Index = 1; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "EXTEND_T"    , 8  ) ) { group = 21; Side = 0; Index = 2; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "RETRACT_T"   , 9  ) ) { group = 21; Side = 0; Index = 3; tm = atoi(chr_return+9); }
			else if ( STRNCMP_L( chr_return , "UP_T"        , 4  ) ) { group = 21; Side = 0; Index = 4; tm = atoi(chr_return+4); }
			else if ( STRNCMP_L( chr_return , "DOWN_T"      , 6  ) ) { group = 21; Side = 0; Index = 5; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "MOVE_T"      , 6  ) ) { group = 21; Side = 0; Index = 6; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "ROTATE_A_T"  , 10 ) ) { group = 21; Side = 0; Index = 1; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "EXTEND_A_T"  , 10 ) ) { group = 21; Side = 0; Index = 2; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "RETRACT_A_T" , 11 ) ) { group = 21; Side = 0; Index = 3; tm = atoi(chr_return+11); }
			else if ( STRNCMP_L( chr_return , "UP_A_T"      , 6  ) ) { group = 21; Side = 0; Index = 4; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "DOWN_A_T"    , 8  ) ) { group = 21; Side = 0; Index = 5; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "MOVE_A_T"    , 8  ) ) { group = 21; Side = 0; Index = 6; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "ROTATE_B_T"  , 10 ) ) { group = 21; Side = 1; Index = 1; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "EXTEND_B_T"  , 10 ) ) { group = 21; Side = 1; Index = 2; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "RETRACT_B_T" , 11 ) ) { group = 21; Side = 1; Index = 3; tm = atoi(chr_return+11); }
			else if ( STRNCMP_L( chr_return , "UP_B_T"      , 6  ) ) { group = 21; Side = 1; Index = 4; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "DOWN_B_T"    , 8  ) ) { group = 21; Side = 1; Index = 5; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "MOVE_B_T"    , 8  ) ) { group = 21; Side = 1; Index = 6; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "ROTATE_C_T"  , 10 ) ) { group = 21; Side = 2; Index = 1; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "EXTEND_C_T"  , 10 ) ) { group = 21; Side = 2; Index = 2; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "RETRACT_C_T" , 11 ) ) { group = 21; Side = 2; Index = 3; tm = atoi(chr_return+11); }
			else if ( STRNCMP_L( chr_return , "UP_C_T"      , 6  ) ) { group = 21; Side = 2; Index = 4; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "DOWN_C_T"    , 8  ) ) { group = 21; Side = 2; Index = 5; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "MOVE_C_T"    , 8  ) ) { group = 21; Side = 2; Index = 6; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "ROTATE_D_T"  , 10 ) ) { group = 21; Side = 3; Index = 1; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "EXTEND_D_T"  , 10 ) ) { group = 21; Side = 3; Index = 2; tm = atoi(chr_return+10); }
			else if ( STRNCMP_L( chr_return , "RETRACT_D_T" , 11 ) ) { group = 21; Side = 3; Index = 3; tm = atoi(chr_return+11); }
			else if ( STRNCMP_L( chr_return , "UP_D_T"      , 6  ) ) { group = 21; Side = 3; Index = 4; tm = atoi(chr_return+6); }
			else if ( STRNCMP_L( chr_return , "DOWN_D_T"    , 8  ) ) { group = 21; Side = 3; Index = 5; tm = atoi(chr_return+8); }
			else if ( STRNCMP_L( chr_return , "MOVE_D_T"    , 8  ) ) { group = 21; Side = 3; Index = 6; tm = atoi(chr_return+8); }

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 730 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			switch( group ) {
			case 99 : // 2010.10.20
				if ( ReadCnt >= 5 ) {
					for ( i = 1 ; i < 5 ; i++ ) {
						if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
							_IO_CIM_PRINTF( "Unknown Error 740 - [%s]-[%d]\n" , Filename , Line );
							_lclose( hFile );
							return FALSE;
						}
						//
						if      ( i == 1 ) fcm = atoi( chr_return );
						else if ( i == 2 ) fpm = atoi( chr_return );
						else if ( i == 3 ) fbm = atoi( chr_return );
						else if ( i == 4 ) ftm = atoi( chr_return );
					}
					//
					fmode = 1;
				}
				break;

			case 100 : // 2018.04.30

				ROBOT_AUTO_MULTI_UPDOWN = atoi( chr_return ); // 2018.04.30

				break;


			case 1 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 750 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "Off"     ) ) _i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i - 1 , 0 );
						else if ( STRCMP_L( chr_return , "Off(L)"  ) ) _i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i - 1 , 2 ); // 2006.11.09
						else if ( STRCMP_L( chr_return , "Off(R)"  ) ) _i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i - 1 , 3 ); // 2006.11.09
						else if ( STRCMP_L( chr_return , "Off(LR)" ) ) _i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i - 1 , 4 ); // 2006.11.09
						else                                           _i_SCH_PRM_SET_RB_ROBOT_ANIMATION( i - 1 , 1 );
					}
				}
				break;

			case 2 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 760 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "On"  ) ) _i_SCH_PRM_SET_RB_SYNCH_CHECK( i - 1 , TRUE );
						else                                       _i_SCH_PRM_SET_RB_SYNCH_CHECK( i - 1 , FALSE );
					}
				}
				break;

			case 3 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 770 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "CW_ROTATION"  ) ) _i_SCH_PRM_SET_RB_ROTATION_STYLE( i - 1 , ROTATION_STYLE_CW );
						else if ( STRCMP_L( chr_return , "CCW_ROTATION" ) ) _i_SCH_PRM_SET_RB_ROTATION_STYLE( i - 1 , ROTATION_STYLE_CCW );
						else if ( STRCMP_L( chr_return , "MINIMUM"      ) ) _i_SCH_PRM_SET_RB_ROTATION_STYLE( i - 1 , ROTATION_STYLE_MINIMUM );
						else if ( STRCMP_L( chr_return , "CW_HOMEBASED" ) ) _i_SCH_PRM_SET_RB_ROTATION_STYLE( i - 1 , ROTATION_STYLE_HOME_CW );
						else                                                _i_SCH_PRM_SET_RB_ROTATION_STYLE( i - 1 , ROTATION_STYLE_HOME_CCW );
					}
				}
				break;

			case 4 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 780 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "USING_A"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_B"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 1 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_C"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_D"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AB" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 1 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_CD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ACD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BCD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABCD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( i - 1 , j , ( j == 0 ) ? TRUE : FALSE );
						}
					}
				}
				break;

			case 5 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 790 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "ON"  ) ) _i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( i - 1 , TRUE );
						else                                       _i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( i - 1 , FALSE );
					}
				}
				break;

			case 6 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 800 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						if      ( STRCMP_L( chr_return , "USING_A"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_B"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 1 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_C"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_D"  ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AB" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 1 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_AD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_CD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABC" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ACD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_BCD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else if ( STRCMP_L( chr_return , "USING_ABCD" ) ) {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
						}
						else {
							for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) _i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( i - 1 , j , ( j == 0 ) ? TRUE : FALSE );
						}
					}
				}
				break;

			case 7 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 810 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( ( i - 1 ) < MAX_TM_CHAMBER_DEPTH ) {
						_i_SCH_PRM_SET_RB_HOME_POSITION( i - 1 , atof( chr_return ) );
						if ( _i_SCH_PRM_GET_RB_HOME_POSITION( i - 1 ) < 0 || _i_SCH_PRM_GET_RB_HOME_POSITION( i - 1 ) > 359 ) _i_SCH_PRM_SET_RB_HOME_POSITION( i - 1 , 0 );
					}
				}
				break;

			case 11 :
				if      ( STRCMP_L( chr_return , "Off"     ) ) _i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( tm , 0 );
				else if ( STRCMP_L( chr_return , "Off(L)"  ) ) _i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( tm , 2 ); // 2006.11.09
				else if ( STRCMP_L( chr_return , "Off(R)"  ) ) _i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( tm , 3 ); // 2006.11.09
				else if ( STRCMP_L( chr_return , "Off(LR)" ) ) _i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( tm , 4 ); // 2006.11.09
				else                                           _i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( tm , 1 );
				break;

			case 12 :
				if      ( STRCMP_L( chr_return , "Off"  ) ) _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 0 );
				else if ( STRCMP_L( chr_return , "On"   ) ) _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 1 );
				else if ( STRCMP_L( chr_return , "On2"  ) ) _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 2 );
				else if ( STRCMP_L( chr_return , "On3"  ) ) _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 3 ); // 2007.05.31
				else if ( STRCMP_L( chr_return , "On4"  ) ) _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 4 ); // 2007.05.31
				else                                        _i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( tm , 0 );
				break;

			case 13 :
				if      ( STRCMP_L( chr_return , "CW_ROTATION"  ) ) _i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( tm , ROTATION_STYLE_CW );
				else if ( STRCMP_L( chr_return , "CCW_ROTATION" ) ) _i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( tm , ROTATION_STYLE_CCW );
				else if ( STRCMP_L( chr_return , "MINIMUM"      ) ) _i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( tm , ROTATION_STYLE_MINIMUM );
				else if ( STRCMP_L( chr_return , "CW_HOMEBASED" ) ) _i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( tm , ROTATION_STYLE_HOME_CW );
				else                                                _i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( tm , ROTATION_STYLE_HOME_CCW );
				break;

			case 14 :
				if      ( STRCMP_L( chr_return , "USING_A"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_B"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 1 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_C"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_D"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AB" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 1 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_CD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ACD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BCD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABCD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( tm , j , ( j == 0 ) ? TRUE : FALSE );
				}
				break;

			case 15 :
				if      ( STRCMP_L( chr_return , "ON"  ) ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( tm , TRUE );
				else                                       _i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( tm , FALSE );
				break;

			case 16 :
				if      ( STRCMP_L( chr_return , "USING_A"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_B"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 1 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_C"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_D"  ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AB" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 1 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_AD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_CD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABC" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ACD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_BCD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else if ( STRCMP_L( chr_return , "USING_ABCD" ) ) {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) || ( j == 1 ) || ( j == 2 ) || ( j == 3 ) ? TRUE : FALSE );
				}
				else {
					for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) _i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( tm , j , ( j == 0 ) ? TRUE : FALSE );
				}
				break;

			case 17 :
				_i_SCH_PRM_SET_RB_FM_HOME_POSITION( tm , atof( chr_return ) );
				if ( _i_SCH_PRM_GET_RB_FM_HOME_POSITION( tm ) < 0 || _i_SCH_PRM_GET_RB_FM_HOME_POSITION( tm ) > 359 ) _i_SCH_PRM_SET_RB_FM_HOME_POSITION( tm , 0 );
				break;

			case 18 : // 2003.01.15
				_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( tm , atoi( chr_return ) );
				break;

			case 21 :
				if ( tm >= MAX_TM_CHAMBER_DEPTH ) break;
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 820 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( !STRCMP_L( chr_return , "-"  ) ) {
						if ( i == ( ReadCnt - 1 ) ) {
							f = atof( chr_return );
							if      ( f <=    0 ) f = 0.5;
							else if ( f >= 1000 ) f = 1000;
							else {
								if      ( Index == 1 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_ROTATE , f );
								else if ( Index == 2 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_EXTEND , f );
								else if ( Index == 3 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_RETRACT , f );
								else if ( Index == 4 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_UP , f );
								else if ( Index == 5 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_DOWN , f );
								else if ( Index == 6 ) _i_SCH_PRM_SET_RB_RNG( tm , Side , RB_ANIM_MOVE , f );
							}
						}
						else {
							if ( fmode == 1 ) { // 2010.10.20
								//
								startal = fcm+fpm+1;
								//
								cmcnt = fcm;
								//
							}
							else {
								if ( ReadCnt == 14 ) { // C4,P6,AL
									startal = 11; // 2005.12.14
								}
								else if ( ReadCnt == 24 ) { // C4,P12,AL,IC,B4 // 2002.07.18
									startal = 17; // 2005.12.14
								}
								else if ( ReadCnt == 48 ) { // C4,P30,AL,IC,B10 // 2002.07.18
									startal = 35; // 2005.12.14
								}
								else if ( ( ReadCnt == 78 ) || ( ReadCnt == 88 ) ) { // C4,P60,AL,IC,B20 // 2002.07.18 // 2005.12.14
									startal = 65; // 2005.12.14
								}
								else { // C4,P?,AL,IC,B?
									startal = MAX_PM_CHAMBER_DEPTH + 5; // 2005.12.14
								}
								//
								cmcnt = 4; // 2010.10.20
								//
							}
							//==================================================================================================
							// 2005.12.14
							//==================================================================================================
							if ( ( i >= 1 ) && ( i <= cmcnt ) ) {
								Chamber = CM1 + ( i - 1 );
							}
							else if ( ( i >= (cmcnt+1) ) && ( i <= ( startal - 1 ) ) ) {
								if ( MAX_PM_CHAMBER_DEPTH > ( i - (cmcnt+1) ) ) {
									Chamber = PM1 + ( i - (cmcnt+1) );
								}
								else {
									Chamber = -1;
								}
							}
							else if ( i == startal ) {
								Chamber = AL;
							}
							else if ( i == ( startal + 1 ) ) {
								Chamber = IC;
							}
							else if ( i >= ( startal + 2 ) ) {
								if ( MAX_BM_CHAMBER_DEPTH > ( i - ( startal + 2 ) ) ) {
									Chamber = BM1 + i - ( startal + 2 );
								}
								else {
									Chamber = -1;
								}
							}
							else {
								Chamber = -1;
							}
							//==================================================================================================
//							f = atof( chr_return ); // 2005.12.14
							if ( Chamber != -1 ) { // 2002.07.18
								f = atof( chr_return ); // 2005.12.14
								if      ( Index == 1 ) {
									if ( f >=   0 && f < 360 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_ROTATE , f );
								}
								else if ( Index == 2 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_EXTEND , f );
								}
								else if ( Index == 3 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_RETRACT , f );
								}
								else if ( Index == 4 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_UP , f );
								}
								else if ( Index == 5 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_DOWN , f );
								}
								else if ( Index == 6 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_POSITION( tm , Side , Chamber , RB_ANIM_MOVE , f );
								}
							}
						}
					}
				}
				break;

			case 22 :
				for ( i = 1 ; i < ReadCnt ; i++ ) {
					if ( !Get_Data_From_String( Buffer , chr_return , i ) ) {
						_IO_CIM_PRINTF( "Unknown Error 830 - [%s]-[%d]\n" , Filename , Line );
						_lclose( hFile );
						return FALSE;
					}
					if ( !STRCMP_L( chr_return , "-"  ) ) {
						if ( i == ( ReadCnt - 1 ) ) {
							f = atof( chr_return );
							if      ( f <=   0 ) f = 0.5;
							else if ( f >= 100 ) f = 100;
							else {
								if      ( Index == 1 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_ROTATE , f );
								else if ( Index == 2 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_EXTEND , f );
								else if ( Index == 3 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_RETRACT , f );
								else if ( Index == 4 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_UP , f );
								else if ( Index == 5 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_DOWN , f );
								else if ( Index == 6 ) _i_SCH_PRM_SET_RB_FM_RNG( tm , RB_ANIM_MOVE , f );
							}
						}
						else {
							if ( fmode == 1 ) { // 2010.10.20
								//
								startal = fcm+fpm+1;
								//
								cmcnt = fcm;
								//
							}
							else {
								if ( ReadCnt == 14 ) { // C4,P6,AL
									startal = 11; // 2005.12.14
								}
								else if ( ReadCnt == 24 ) { // C4,P12,AL,IC,B4 // 2002.07.18
									startal = 17; // 2005.12.14
								}
								else if ( ReadCnt == 48 ) { // C4,P30,AL,IC,B10 // 2002.07.18
									startal = 35; // 2005.12.14
								}
								else if ( ( ReadCnt == 78 ) || ( ReadCnt == 88 ) ) { // C4,P60,AL,IC,B20 // 2002.07.18 // 2005.12.14
									startal = 65; // 2005.12.14
								}
								else { // C4,P?,AL,IC,B?
									startal = MAX_PM_CHAMBER_DEPTH + 5; // 2005.12.14
								}
								//
								cmcnt = 4; // 2010.10.20
								//
							}
							//==================================================================================================
							// 2005.12.14
							//==================================================================================================
							if ( ( i >= 1 ) && ( i <= cmcnt ) ) {
								Chamber = CM1 + ( i - 1 );
							}
							else if ( ( i >= (cmcnt+1) ) && ( i <= ( startal - 1 ) ) ) {
								if ( MAX_PM_CHAMBER_DEPTH > ( i - (cmcnt+1) ) ) {
									Chamber = PM1 + ( i - (cmcnt+1) );
								}
								else {
									Chamber = -1;
								}
							}
							else if ( i == startal ) {
								Chamber = AL;
							}
							else if ( i == ( startal + 1 ) ) {
								Chamber = IC;
							}
							else if ( i >= ( startal + 2 ) ) {
								if ( MAX_BM_CHAMBER_DEPTH > ( i - ( startal + 2 ) ) ) {
									Chamber = BM1 + i - ( startal + 2 );
								}
								else {
									Chamber = -1;
								}
							}
							else {
								Chamber = -1;
							}
							//==================================================================================================
							if ( Chamber != -1 ) { // 2002.07.18
								f = atof( chr_return ); // 2005.12.14
								if      ( Index == 1 ) {
									if ( f >=   0 && f < 360 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_ROTATE , f );
								}
								else if ( Index == 2 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_EXTEND , f );
								}
								else if ( Index == 3 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_RETRACT , f );
								}
								else if ( Index == 4 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_UP , f );
								}
								else if ( Index == 5 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_DOWN , f );
								}
								else if ( Index == 6 ) {
									if ( f >=-1000 && f <= 1000 ) _i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_MOVE , f );
								}
							}
						}
					}
				}
				break;
			}
			if ( group == -1 ) {
				FILE_SUB_CHANGE_STRING_TO_SPACE( Buffer );
				_IO_CIM_PRINTF( "Unknown Parameter(%s) - [%s]-[%d]\n" , Buffer , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
		}
	}
	_lclose( hFile );

	if ( ( ROBOT_AUTO_MULTI_UPDOWN == 1 ) || ( ROBOT_AUTO_MULTI_UPDOWN == 2 ) ) { // 2018.04.30
		for ( tm = 0 ; tm < MAX_FM_CHAMBER_DEPTH ; tm++ ) {
			for ( Chamber = 0 ; Chamber < MAX_CHAMBER ; Chamber++ ) {
				//
				f = _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_UP ) - _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_DOWN );
				//
				for ( i = 2 ; i <= MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_ROTATE , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_ROTATE ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_EXTEND , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_EXTEND ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_RETRACT , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_RETRACT ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_UP , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_UP ) + ( (double) (i-1) * f ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_DOWN , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_DOWN ) + ( (double) (i-1) * f ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_MOVE , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_MOVE ) );
				}
			}
		}
	}
	else {
		for ( tm = 0 ; tm < MAX_FM_CHAMBER_DEPTH ; tm++ ) {
			for ( i = 2 ; i <= MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				for ( Chamber = 0 ; Chamber < MAX_CHAMBER ; Chamber++ ) {
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_ROTATE , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_ROTATE ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_EXTEND , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_EXTEND ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_RETRACT , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_RETRACT ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_UP , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_UP ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_DOWN , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_DOWN ) );
					_i_SCH_PRM_SET_RB_FM_POSITION( tm , Chamber , i , RB_ANIM_MOVE , _i_SCH_PRM_GET_RB_FM_POSITION( tm , Chamber , 1 , RB_ANIM_MOVE ) );
				}
			}
		}
	}
	//
	for ( tm = 0 ; tm < MAX_TM_CHAMBER_DEPTH ; tm++ ) { // 2011.07.26
		for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
			//
			if ( _i_SCH_PRM_GET_RB_POSITION( tm , i , F_AL , RB_ANIM_EXTEND ) == 0 ) {
				//
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_ROTATE	, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_ROTATE ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_EXTEND	, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_EXTEND ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_RETRACT	, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_RETRACT ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_UP		, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_UP ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_DOWN	, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_DOWN ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_AL , RB_ANIM_MOVE	, _i_SCH_PRM_GET_RB_POSITION( tm , i , AL , RB_ANIM_MOVE ) );
				//
			}
			//
			if ( _i_SCH_PRM_GET_RB_POSITION( tm , i , F_IC , RB_ANIM_EXTEND ) == 0 ) {
				//
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_ROTATE	, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_ROTATE ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_EXTEND	, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_EXTEND ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_RETRACT	, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_RETRACT ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_UP		, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_UP ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_DOWN	, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_DOWN ) );
				_i_SCH_PRM_SET_RB_POSITION( tm , i , F_IC , RB_ANIM_MOVE	, _i_SCH_PRM_GET_RB_POSITION( tm , i , IC , RB_ANIM_MOVE ) );
				//
			}
			//
		}
	}
	//
	return TRUE;
}


//-------------------------------------------------------------------------
// Function = FILE_ROBOT2_SETUP
//-------------------------------------------------------------------------

BOOL FILE_ROBOT2_SETUP( char *Filename ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return[64];
	int ch , slot , x;
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;

	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return TRUE;
	for ( Line = 1 ; FileEnd ; Line++ ) {
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		if ( ReadCnt > 0 ) {
			if ( ReadCnt < 8 ) continue;
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 900 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}
			x = 0; // 2007.06.07
			ch = -1;
			slot = -1;
			//
			if ( STRNCMP_L( chr_return , "[2]" , 3 ) ) {
				x = 3;
			}
			//
			if ( STRNCMP_L( chr_return + x , "CM" , 2   ) ) {
				ch = atoi( chr_return + x + 2 ) - 1 + CM1;
				if ( ch >= CM1 ) {
					if ( ( ch - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ch = -1;
				}
				else ch = -1;
			}
			else if ( STRNCMP_L( chr_return + x , "PM" , 2   ) ) {
				ch = atoi( chr_return + x + 2 ) - 1 + PM1;
				if ( ch >= PM1 ) {
					if ( ( ch - PM1 + 1 ) > MAX_PM_CHAMBER_DEPTH ) ch = -1;
				}
				else ch = -1;
			}
			else if ( STRNCMP_L( chr_return + x , "BM" , 2   ) ) {
				ch = atoi( chr_return + x + 2 ) - 1 + BM1;
				if ( ch >= BM1 ) {
					if ( ( ch - BM1 + 1 ) > MAX_BM_CHAMBER_DEPTH ) ch = -1;
				}
				else ch = -1;
			}
			else if ( STRCMP_L( chr_return + x , "AL"   ) ) ch = AL;
			else if ( STRCMP_L( chr_return + x , "IC"   ) ) ch = IC;

			if ( ch == -1 ) continue;

			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 910 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			slot = atoi( chr_return );

			if ( ( slot < 1 ) || ( slot > MAX_CASSETTE_SLOT_SIZE ) ) continue;

			if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 920 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_ROTATE , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_ROTATE , atof( chr_return ) );
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 3 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 930 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_EXTEND , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_EXTEND , atof( chr_return ) );
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 4 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 940 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_RETRACT , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_RETRACT , atof( chr_return ) );
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 5 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 950 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_UP , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_UP , atof( chr_return ) );
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 6 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 960 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_DOWN , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_DOWN , atof( chr_return ) );
			}

			if ( !Get_Data_From_String( Buffer , chr_return , 7 ) ) {
				_IO_CIM_PRINTF( "Unknown Error 970 - [%s]-[%d]\n" , Filename , Line );
				_lclose( hFile );
				return FALSE;
			}

			if ( x == 0 ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( 0 , ch , slot , RB_ANIM_MOVE , atof( chr_return ) );
			}
			else {
				_i_SCH_PRM_SET_RB_FM_POSITION( 1 , ch , slot , RB_ANIM_MOVE , atof( chr_return ) );
			}
		}
	}
	_lclose( hFile );
	return TRUE;
}


//-------------------------------------------------------------------------
// Function = FILE_SCHEDULE_SETUP
//-------------------------------------------------------------------------
//BOOL FILE_SCHEDULE_SETUP_SAVE( char *Filename ) { // 2002.05.05 // Invalid Condition Not Save // 2017.02.09
BOOL FILE_SCHEDULE_SETUP_SAVE( char *Filename , BOOL all ) { // 2002.05.05 // Invalid Condition Not Save // 2017.02.09
	int i , j , k;
	FILE *fpt;
	//
	if ( all ) { // 2017.02.09
		//
		_SCH_PRM_MAKE_MODULE_PICK_ORDERING(); // 2012.09.21
		//
		FILE_PARAM_SM_SCHEDULER_SETTING(); // 2013.08.22
		//
	}
	//
	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21

	if ( ( fpt = fopen( Filename , "w" ) ) == NULL ) {
		_IO_CIM_PRINTF( "[%s] File Not Found\n" , Filename );
		return FALSE;
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MAPPING_SKIP( CM1 + i ) != 0 ) {
			fprintf( fpt , "MAPPING_SKIP_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MAPPING_SKIP( CM1 + i ) );
		}
		if ( _i_SCH_PRM_GET_AUTO_HANDOFF( CM1 + i ) != 0 ) {
			fprintf( fpt , "AUTO_HANDOFF_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_AUTO_HANDOFF( CM1 + i ) );
		}
		if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( CM1 + i ) != 0 ) {
			fprintf( fpt , "EXTEND_HANDOFF_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( CM1 + i ) );
		}
		if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( i ) != 0 ) {
			fprintf( fpt , "MID_READ_POINT_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_MID_READ_POINT( i ) );
		}
		if ( _i_SCH_PRM_GET_FA_JID_READ_POINT( i ) != 0 ) {
			fprintf( fpt , "JID_READ_POINT_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_JID_READ_POINT( i ) );
		}
		if ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE( i ) != 0 ) {
			fprintf( fpt , "LOAD_COMPLETE_MESSAGE_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE( i ) );
		}
		if ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE( i ) != 0 ) {
			fprintf( fpt , "UNLOAD_COMPLETE_MESSAGE_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE( i ) );
		}
		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) != 0 ) {
			fprintf( fpt , "MAPPING_AFTERLOAD_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) );
		}
		if ( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE( CM1 + i ) != 0 ) { // 2002.07.10
			fprintf( fpt , "ERROR_OUT_CHAMBER_FOR_CASSETTE_CM%d	%d	%d	%d\n" , i + 1 , _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE( CM1 + i ) - BM1 + 1 , _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( CM1 + i ) , _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( CM1 + i ) );
		}
	}
		
	if ( _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() != 0 ) {
		fprintf( fpt , "MAPPING_TMFREE		%d\n" , _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() );
	}
	if ( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() != 0 ) {
		fprintf( fpt , "CONTROL_GATE_TM		%d\n" , _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() );
	}
	if ( _i_SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() != 0 ) {
		fprintf( fpt , "CONTROL_MOVE_SR_TM	%d\n" , _i_SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() );
	}
	if ( _i_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() != 0 ) {
		fprintf( fpt , "CONTROL_MOVE_SR_FM	%d\n" , _i_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() );
	}
	if ( _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() != 0 ) {
		fprintf( fpt , "SCH_START_OPTION	%d\n" , _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() );
	}
	if ( _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() != 0 ) {
		fprintf( fpt , "LOOP_MAP_ALWAYS		%d\n" , _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() );
	}
	if ( _i_SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() != 0 ) {
		fprintf( fpt , "FIRST_MODULE_PUT_DELAY		%d\n" , _i_SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() );
	}
	if ( _i_SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() != 0 ) {
		fprintf( fpt , "CASSETTE_SUPPLY_DEV_TIME		%d\n" , _i_SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() );
	}
	if ( _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT() != 0 ) {
		fprintf( fpt , "ABORT_MESSAGE_USE_POINT		%d\n" , _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT() );
	}
	if ( _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() != 0 ) {
		fprintf( fpt , "CANCEL_MESSAGE_USE_POINT	%d\n" , _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() );
	}
	if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() != 0 ) {
		fprintf( fpt , "END_MESSAGE_SEND_POINT	%d\n" , _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() );
	}
	if ( _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() != 0 ) {
		fprintf( fpt , "CHAMBER_PUT_PR_CHECK	%d\n" , _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() );
	}
	if ( _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 0 ) {
		fprintf( fpt , "CHAMBER_GET_PR_CHECK	%d\n" , _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() );
	}
	if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() != 0 ) {
		if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() > 0 ) {
			fprintf( fpt , "MULTI_COOLER_WAITTIME	%d	%d\n" , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() );
		}
		else {
			fprintf( fpt , "MULTI_COOLER_WAITTIME	%d\n" , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() );
		}
	}
	if ( _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() != 0 ) {
		fprintf( fpt , "MULTI_ALIGNER_WAITTIME	%d\n" , _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() );
	}

	if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() != 0 ) { // 2005.12.01
		fprintf( fpt , "MULTI_COOLER_TIMEMODE	%d\n" , _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() );
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_Getting_Priority( CM1 + i ) != 0 ) {
			fprintf( fpt , "PRIORITY_PICK(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Getting_Priority( CM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Getting_Priority( i + PM1 ) != 0 ) {
			fprintf( fpt , "PRIORITY_PICK(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Getting_Priority( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Getting_Priority( i + BM1 ) != 0 ) {
			fprintf( fpt , "PRIORITY_PICK(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Getting_Priority( i + BM1 ) );
		}
	}

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2006.02.09
		if ( _i_SCH_PRM_GET_Getting_Priority( i + TM ) != 0 ) {
			fprintf( fpt , "PRIORITY_PICK(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Getting_Priority( i + TM ) );
		}
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_Putting_Priority( CM1 + i ) != 0 ) {
			fprintf( fpt , "PRIORITY_PLACE(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Putting_Priority( CM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Putting_Priority( i + PM1 ) != 0 ) {
			fprintf( fpt , "PRIORITY_PLACE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Putting_Priority( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Putting_Priority( i + BM1 ) != 0 ) {
			fprintf( fpt , "PRIORITY_PLACE(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_Putting_Priority( i + BM1 ) );
		}
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( CM1 + i ) ) {
		case 1 : fprintf( fpt , "Module_SW_Mode(C%d)	On\n" , i + 1 ); break;
		case 2 : fprintf( fpt , "Module_SW_Mode(C%d)	Off2\n" , i + 1 ); break;
		case 3 : fprintf( fpt , "Module_SW_Mode(C%d)	On2\n" , i + 1 ); break;
		//default: fprintf( fpt , "Module_SW_Mode(C%d)	Off\n" , i + 1 ); break;
		}
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( PM1 + i ) ) {
		case 1 : fprintf( fpt , "Module_SW_Mode(P%d)	On\n" , i + 1 ); break;
		case 2 : fprintf( fpt , "Module_SW_Mode(P%d)	Off2\n" , i + 1 ); break;
		case 3 : fprintf( fpt , "Module_SW_Mode(P%d)	On2\n" , i + 1 ); break;
		//default: fprintf( fpt , "Module_SW_Mode(P%d)	Off\n" , i + 1 ); break;
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( BM1 + i ) ) {
		case 1 : fprintf( fpt , "Module_SW_Mode(B%d)	On\n" , i + 1 ); break;
		case 2 : fprintf( fpt , "Module_SW_Mode(B%d)	Off2\n" , i + 1 ); break;
		case 3 : fprintf( fpt , "Module_SW_Mode(B%d)	On2\n" , i + 1 ); break;
		//default: fprintf( fpt , "Module_SW_Mode(B%d)	Off\n" , i + 1 ); break;
		}
	}
	switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( AL ) ) {
	case 1 : fprintf( fpt , "Module_SW_Mode(AL)	On\n" ); break;
	case 2 : fprintf( fpt , "Module_SW_Mode(AL)	Off2\n" ); break;
	case 3 : fprintf( fpt , "Module_SW_Mode(AL)	On2\n" ); break;
	//default: fprintf( fpt , "Module_SW_Mode(AL)	Off\n" ); break;
	}
	switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( IC ) ) {
	case 1 : fprintf( fpt , "Module_SW_Mode(IC)	On\n" ); break;
	case 2 : fprintf( fpt , "Module_SW_Mode(IC)	Off2\n" ); break;
	case 3 : fprintf( fpt , "Module_SW_Mode(IC)	On2\n" ); break;
	//default: fprintf( fpt , "Module_SW_Mode(IC)	Off\n" ); break;
	}
	switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( F_AL ) ) {
	case 1 : fprintf( fpt , "Module_SW_Mode(FAL)	On\n" ); break;
	case 2 : fprintf( fpt , "Module_SW_Mode(FAL)	Off2\n" ); break;
	case 3 : fprintf( fpt , "Module_SW_Mode(FAL)	On2\n" ); break;
	//default: fprintf( fpt , "Module_SW_Mode(FAL)	Off\n" ); break;
	}
	switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( F_IC ) ) {
	case 1 : fprintf( fpt , "Module_SW_Mode(FIC)	On\n" ); break;
	case 2 : fprintf( fpt , "Module_SW_Mode(FIC)	Off2\n" ); break;
	case 3 : fprintf( fpt , "Module_SW_Mode(FIC)	On2\n" ); break;
	//default: fprintf( fpt , "Module_SW_Mode(FIC)	Off\n" ); break;
	}

	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2012.04.30
		if ( _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( PM1 + i ) ) {
			fprintf( fpt , "Module_SW_Size(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_SIZE( PM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.11.01
		if ( _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( BM1 + i ) ) {
			fprintf( fpt , "Module_SW_Size(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_SIZE( BM1 + i ) );
		}
	}
	if ( _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( F_IC ) ) { // 2007.08.14
		fprintf( fpt , "Module_SW_Size(FIC)	%d\n" , _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) );
	}

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.10.16
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + TM ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPRECV(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + TM ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + TM ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPSEND(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + TM ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + TM ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTRECV(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + TM ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + TM ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTSEND(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + TM ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + TM ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_GATE(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + TM ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + TM ) ) { // 2016.11.24
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_CANCEL(T%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + TM ) );
		}
	}
	//
	//=========================
	// 2005.12.27
	//=========================
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( FM ) != 0 ) {
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPRECV(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( FM ) );
	}
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( FM ) != 0 ) {
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPSEND(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( FM ) );
	}
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( FM ) != 0 ) {
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTRECV(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( FM ) );
	}
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( FM ) != 0 ) {
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTSEND(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( FM ) );
	}
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( FM ) != 0 ) {
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_GATE(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( FM ) );
	}
	if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( FM ) != 0 ) { // 2016.11.24
		fprintf( fpt , "MODULE_MESSAGE_NOTUSE_CANCEL(F)	%d\n" , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( FM ) );
	}
	//=========================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2002.10.16
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + PM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPRECV(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + PM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPSEND(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + PM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTRECV(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + PM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTSEND(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + PM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_GATE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + PM1 ) ) { // 2016.11.24
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_CANCEL(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.10.16
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + BM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPRECV(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + BM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_PREPSEND(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + BM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTRECV(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + BM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_POSTSEND(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + BM1 ) ) {
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_GATE(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + BM1 ) ) { // 2016.11.24
			fprintf( fpt , "MODULE_MESSAGE_NOTUSE_CANCEL(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i + BM1 ) );
		}
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if      ( _i_SCH_PRM_GET_Process_Run_In_Mode( i + PM1 ) == 0 )	{
			if ( _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) != 0 ) {
				fprintf( fpt , "PROCESS_RUN_IN(P%d)	RUN %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) );
			}
		}
		else if ( _i_SCH_PRM_GET_Process_Run_In_Mode( i + PM1 ) == 1 )	{
			fprintf( fpt , "PROCESS_RUN_IN(P%d)	RUNA %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_In_Mode( i + PM1 ) == 2 )	{
			fprintf( fpt , "PROCESS_RUN_IN(P%d)	SIM %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_In_Mode( i + PM1 ) == 3 )	{
			fprintf( fpt , "PROCESS_RUN_IN(P%d)	SIMA %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_In_Mode( i + PM1 ) == 4 ) {
			fprintf( fpt , "PROCESS_RUN_IN(P%d)	NOTUSE %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Process_Run_In_Time( BM1 + i ) != 0 ) {
			fprintf( fpt , "PROCESS_RUN_IN(B%d)	RUN %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_In_Time( BM1 + i ) );
		}
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if      ( _i_SCH_PRM_GET_Process_Run_Out_Mode( i + PM1 ) == 0 ) {
			if ( _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) != 0 ) {
				fprintf( fpt , "PROCESS_RUN_OUT(P%d)	RUN %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) );
			}
		}
		else if ( _i_SCH_PRM_GET_Process_Run_Out_Mode( i + PM1 ) == 1 ) {
			fprintf( fpt , "PROCESS_RUN_OUT(P%d)	RUNA %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_Out_Mode( i + PM1 ) == 2 ) {
			fprintf( fpt , "PROCESS_RUN_OUT(P%d)	SIM %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_Out_Mode( i + PM1 ) == 3 ) {
			fprintf( fpt , "PROCESS_RUN_OUT(P%d)	SIMA %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) );
		}
		else if ( _i_SCH_PRM_GET_Process_Run_Out_Mode( i + PM1 ) == 4 ) {
			fprintf( fpt , "PROCESS_RUN_OUT(P%d)	NOTUSE %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_Process_Run_Out_Time( BM1 + i ) != 0 ) {
			fprintf( fpt , "PROCESS_RUN_OUT(B%d)	RUN %d\n" , i + 1 , _i_SCH_PRM_GET_Process_Run_Out_Time( BM1 + i ) );
		}
		if ( ( _i_SCH_PRM_GET_READY_RECIPE_USE( i + BM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_READY_RECIPE_NAME( i + BM1 ) ) > 0 ) ) {
			fprintf( fpt , "READY_RECIPE(B%d)	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_READY_RECIPE_USE( i + BM1 ) , _i_SCH_PRM_GET_READY_RECIPE_NAME( i + BM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_END_RECIPE_USE( i + BM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_END_RECIPE_NAME( i + BM1 ) ) > 0 ) ) {
			fprintf( fpt , "END_RECIPE(B%d)	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_END_RECIPE_USE( i + BM1 ) , _i_SCH_PRM_GET_END_RECIPE_NAME( i + BM1 ) );
		}
		//
//		if ( ( _i_SCH_PRM_GET_READY_RECIPE_USE( i + BM1 ) != 0 ) || ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + BM1 ) > 0 ) ) { // 2005.08.18 // 2017.02.02
		if ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + BM1 ) > 0 ) { // 2005.08.18 // 2017.02.02
			fprintf( fpt , "READY_RECIPE_MINTIME(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + BM1 ) );
		}
//		if ( ( _i_SCH_PRM_GET_END_RECIPE_USE( i + BM1 ) != 0 ) || ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + BM1 ) > 0 ) ) { // 2005.08.18 // 2017.02.02
		if ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + BM1 ) > 0 ) { // 2005.08.18 // 2017.02.02
			fprintf( fpt , "END_RECIPE_MINTIME(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + BM1 ) );
		}
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i + PM1 ) != 0 ) {
			fprintf( fpt , "PROCESS_RUN_GAP(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i + PM1 ) != 0 ) { // 2006.05.04
			fprintf( fpt , "PROCESS_REMAIN_GAP(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_READY_RECIPE_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_READY_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "READY_RECIPE(P%d)	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_READY_RECIPE_USE( i + PM1 ) , _i_SCH_PRM_GET_READY_RECIPE_NAME( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_END_RECIPE_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_END_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "END_RECIPE(P%d)	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_END_RECIPE_USE( i + PM1 ) , _i_SCH_PRM_GET_END_RECIPE_NAME( i + PM1 ) );
		}
		//
//		if ( ( _i_SCH_PRM_GET_READY_RECIPE_USE( i + PM1 ) != 0 ) || ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + PM1 ) > 0 ) ) { // 2005.08.18 // 2017.02.02
		if ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + PM1 ) > 0 ) { // 2005.08.18 // 2017.02.02
			fprintf( fpt , "READY_RECIPE_MINTIME(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i + PM1 ) );
		}
//		if ( ( _i_SCH_PRM_GET_END_RECIPE_USE( i + PM1 ) != 0 ) || ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + PM1 ) > 0 ) ) { // 2005.08.18 // 2017.02.02
		if ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + PM1 ) > 0 ) { // 2005.08.18 // 2017.02.02
			fprintf( fpt , "END_RECIPE_MINTIME(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_END_RECIPE_MINTIME( i + PM1 ) );
		}
	}

	if ( _i_SCH_PRM_GET_READY_MULTIJOB_MODE() != 0 ) { // 2007.03.15
		fprintf( fpt , "READY_MULTIJOB_MODE	%d\n" , _i_SCH_PRM_GET_READY_MULTIJOB_MODE() );
	}

	if ( ( _i_SCH_PRM_GET_CLSOUT_USE( TM ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_DELAY( TM ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) != 0 ) ) {
		fprintf( fpt , "CLEAR_OUT(TM)	%d	%d	%d\n" , _i_SCH_PRM_GET_CLSOUT_USE( TM ) , _i_SCH_PRM_GET_CLSOUT_DELAY( TM ) , _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) );
	}
	if ( ( _i_SCH_PRM_GET_CLSOUT_USE( FM ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_DELAY( FM ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( FM ) != 0 ) ) {
		fprintf( fpt , "CLEAR_OUT(FM)	%d	%d	%d\n" , _i_SCH_PRM_GET_CLSOUT_USE( FM ) , _i_SCH_PRM_GET_CLSOUT_DELAY( FM ) , _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( FM ) );
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( ( _i_SCH_PRM_GET_CLSOUT_USE( CM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_DELAY( CM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CM1 + i ) != 0 ) ) {
			fprintf( fpt , "CLEAR_OUT(C%d)	%d	%d	%d\n" , i + 1 , _i_SCH_PRM_GET_CLSOUT_USE( CM1 + i ) , _i_SCH_PRM_GET_CLSOUT_DELAY( CM1 + i ) , _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( ( _i_SCH_PRM_GET_CLSOUT_USE( PM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_DELAY( PM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( PM1 + i ) != 0 ) ) {
			fprintf( fpt , "CLEAR_OUT(P%d)	%d	%d	%d\n" , i + 1 , _i_SCH_PRM_GET_CLSOUT_USE( PM1 + i ) , _i_SCH_PRM_GET_CLSOUT_DELAY( PM1 + i ) , _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( PM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( ( _i_SCH_PRM_GET_CLSOUT_USE( BM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_DELAY( BM1 + i ) != 0 ) || ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( BM1 + i ) != 0 ) ) {
			fprintf( fpt , "CLEAR_OUT(B%d)	%d	%d	%d\n" , i + 1 , _i_SCH_PRM_GET_CLSOUT_USE( BM1 + i ) , _i_SCH_PRM_GET_CLSOUT_DELAY( BM1 + i ) , _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( BM1 + i ) );
		}
	}

	if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() != 0 ) { // 2004.05.06
		fprintf( fpt , "CLEAR_OUT_BM_SINGLE	%d\n" , _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() );
	}

	if ( _i_SCH_PRM_GET_CLSOUT_AL_TO_PM() != 0 ) { // 2005.12.19
		fprintf( fpt , "CLEAR_MOVE_AL_TO_PM	%d\n" , _i_SCH_PRM_GET_CLSOUT_AL_TO_PM() );
	}

	if ( _i_SCH_PRM_GET_CLSOUT_IC_TO_PM() != 0 ) { // 2005.12.19
		fprintf( fpt , "CLEAR_MOVE_IC_TO_PM	%d\n" , _i_SCH_PRM_GET_CLSOUT_IC_TO_PM() );
	}

	if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 0 ) { // 2012.05.04
		fprintf( fpt , "MAINTINF_DB_USE	%d\n" , _i_SCH_PRM_GET_MAINTINF_DB_USE() );
	}

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i + PM1 ) != 0 ) {
			fprintf( fpt , "FAIL_PROCESS_SCENARIO(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i + PM1 ) != 0 ) {
			fprintf( fpt , "STOP_PROCESS_SCENARIO(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( i + PM1 ) != 0 ) {
			fprintf( fpt , "MRES_FAIL_SCENARIO(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MRES_ABORT_SCENARIO( i + PM1 ) != 0 ) {
			fprintf( fpt , "MRES_ABORT_SCENARIO(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MRES_ABORT_SCENARIO( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i + PM1 ) != 0 ) {
			fprintf( fpt , "MRES_SUCCESS_SCENARIO(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i + PM1 ) != 0 ) || ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "FIXED_CLUSTER_PRE_PART(P%d)	%d	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "FIXED_CLUSTER_PRE_PART2(P%d)	0	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i + PM1 ) != 0 ) || ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "FIXED_CLUSTER_POST_PART(P%d)	%d	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( i + PM1 ) );
		}
		if ( ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( i + PM1 ) != 0 ) || ( strlen( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( i + PM1 ) ) > 0 ) ) {
			fprintf( fpt , "FIXED_CLUSTER_POST_PART2(P%d)	0	%d	%s\n" , i + 1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( i + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( i + PM1 ) );
		}
	}

	if ( _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() != 0 ) { // 2002.07.23
		fprintf( fpt , "MRES_ABORT_ALL_SCENARIO	%d\n" , _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() );
	}

	if ( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() != 0 ) { // 2004.05.14
		fprintf( fpt , "DISABLE_MAKE_HOLE_GOTOSTOP	%d\n" , _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() );
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( CM1 + i ) != 0 ) {
			fprintf( fpt , "ANIMATION_ROTATE_PREPARE(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( CM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( PM1 + i ) != 0 ) {
			fprintf( fpt , "ANIMATION_ROTATE_PREPARE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( PM1 + i ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( BM1 + i ) != 0 ) {
			fprintf( fpt , "ANIMATION_ROTATE_PREPARE(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( BM1 + i ) );
		}
	}
	if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( AL ) != 0 ) {
		fprintf( fpt , "ANIMATION_ROTATE_PREPARE(AL)	%d\n" , _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( AL ) );
	}
	if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( IC ) != 0 ) {
		fprintf( fpt , "ANIMATION_ROTATE_PREPARE(IC)	%d\n" , _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( IC ) );
	}
	
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( strlen( _i_SCH_PRM_GET_MODULE_PROCESS_NAME( i+PM1 ) ) > 0 ) fprintf( fpt , "Module_Name(P%d)	%s\n" , i + 1 , _i_SCH_PRM_GET_MODULE_PROCESS_NAME( i + PM1 ) );
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2010.07.22
		if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i + CM1 ) != 0 ) { // 2010.07.22
			fprintf( fpt , "DOUBLE_CHAMBER_WHAT_SIDE_USE(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i + CM1 ) ); // 2010.07.22
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i + PM1 ) != 0 ) {
			fprintf( fpt , "DOUBLE_CHAMBER_WHAT_SIDE_USE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i + PM1 ) );
		}
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 0 , 0 ) != 0 ) {
		fprintf( fpt , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(A1)	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 0 , 0 ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 0 , 1 ) != 0 ) {
		fprintf( fpt , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(A2)	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 0 , 1 ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 1 , 0 ) != 0 ) {
		fprintf( fpt , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(B1)	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 1 , 0 ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 1 , 1 ) != 0 ) {
		fprintf( fpt , "DOUBLE_CHAMBER_FM_RB_INTERLOCK(B2)	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( 1 , 1 ) );
	}

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( i ) != 0 ) {
			fprintf( fpt , "BUFFER_CHAMBER_WHAT_SIDE_SWITCH%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( i ) );
		}
		if ( _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( i ) != 0 ) {
			fprintf( fpt , "BUFFER_CHAMBER_ACCESS_SWITCH%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( i ) );
		}
	}
	if ( GUI_GROUP_SELECT_GET() != 0 ) {
		fprintf( fpt , "GUI_GROUP_SELECT	%d\n" , GUI_GROUP_SELECT_GET() );
	}

	if ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() != 0 ) { // 2006.02.07
		fprintf( fpt , "SYSTEM_LOG_STYLE	%d\n" , _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() );
	}

	if ( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() != 0 ) {
		fprintf( fpt , "SCH_LOG_DIRECTORY_FORMAT	%d\n" , _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() );
	}
	if ( _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() != 0 ) {
		fprintf( fpt , "SCH_CHANGE_PROCESS_WAFER	%d\n" , _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() );
	}
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( i + CM1 , j + PM1 ) != 0 ) {
				fprintf( fpt , "CHAMBER_PM_INTERLOCK_RUN_FOR_CM%d(P%d)	%d\n" , i + 1 , j + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( i + CM1 , j + PM1 ) );
			}
		}
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , AL ) != 0 ) {
			fprintf( fpt , "CHAMBER_SLOT_FROM_CM_OFFSET_CM%d_AL	%d\n" , i + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , AL ) );
		}
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , IC ) != 0 ) {
			fprintf( fpt , "CHAMBER_SLOT_FROM_CM_OFFSET_CM%d_IC	%d\n" , i + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , IC ) );
		}
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , F_AL ) != 0 ) {
			fprintf( fpt , "CHAMBER_SLOT_FROM_CM_OFFSET_CM%d_FAL	%d\n" , i + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , F_AL ) );
		}
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , F_IC ) != 0 ) {
			fprintf( fpt , "CHAMBER_SLOT_FROM_CM_OFFSET_CM%d_FIC	%d\n" , i + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , F_IC ) );
		}
		for ( j = 0 ; j < MAX_BM_CHAMBER_DEPTH ; j++ ) {
			if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , j + BM1 ) != 0 ) {
				fprintf( fpt , "CHAMBER_SLOT_FROM_CM_OFFSET_CM%d_BM%d	%d\n" , i + 1 , j + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( i + CM1 , j + BM1 ) );
			}
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + CM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PICK_DENY_FOR_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + CM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + CM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PLACE_DENY_FOR_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + CM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + CM1 ) != 0 ) { // 2006.09.26
			fprintf( fpt , "CHAMBER_FM_PICK_DENY_FOR_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + CM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + CM1 ) != 0 ) { // 2006.09.26
			fprintf( fpt , "CHAMBER_FM_PLACE_DENY_FOR_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + CM1 ) );
		}
		//
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + PM1 ) != 0 ) { // 2008.08.29
			fprintf( fpt , "CHAMBER_FM_PICK_DENY_FOR_PM%d   %d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + PM1 ) != 0 ) { // 2008.08.29
			fprintf( fpt , "CHAMBER_FM_PLACE_DENY_FOR_PM%d  %d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + PM1 ) );
		}
		//
		if ( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i + CM1 ) != 0 ) { // 2004.01.30
			fprintf( fpt , "CHAMBER_ROBOT_MULTI_DENY_FOR_ARM_CM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i + CM1 ) );
		}
	}
	//
	/*
	// 2013.05.28
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.05
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + PM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PICK_DENY_FOR_PM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + PM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PLACE_DENY_FOR_PM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + PM1 ) );
		}
	}
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + BM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PICK_DENY_FOR_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + BM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_PM_PLACE_DENY_FOR_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i + BM1 ) );
		}
	}
	*/
	//
	// 2013.05.28
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		k = -1;
		for ( j = MAX_TM_CHAMBER_DEPTH - 1 ; j >= 0 ; j-- ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i + PM1 ) != 0 ) {
				k = j;
				break;
			}
		}
		if ( k >= 0 ) {
			fprintf( fpt , "CHAMBER_PM_PICK_DENY_FOR_PM%d" , i + 1 );
			for ( j = 0 ; j <= k ; j++ ) fprintf( fpt , "	%d" , _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i + PM1 ) );
			fprintf( fpt , "\n" );
		}
		//
		k = -1;
		for ( j = MAX_TM_CHAMBER_DEPTH - 1 ; j >= 0 ; j-- ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i + PM1 ) != 0 ) {
				k = j;
				break;
			}
		}
		if ( k >= 0 ) {
			fprintf( fpt , "CHAMBER_PM_PLACE_DENY_FOR_PM%d" , i + 1 );
			for ( j = 0 ; j <= k ; j++ ) fprintf( fpt , "	%d" , _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i + PM1 ) );
			fprintf( fpt , "\n" );
		}
		//
	}
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
		//
		k = -1;
		for ( j = MAX_TM_CHAMBER_DEPTH - 1 ; j >= 0 ; j-- ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i + BM1 ) != 0 ) {
				k = j;
				break;
			}
		}
		if ( k >= 0 ) {
			fprintf( fpt , "CHAMBER_PM_PICK_DENY_FOR_BM%d" , i + 1 );
			for ( j = 0 ; j <= k ; j++ ) fprintf( fpt , "	%d" , _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i + BM1 ) );
			fprintf( fpt , "\n" );
		}
		//
		k = -1;
		for ( j = MAX_TM_CHAMBER_DEPTH - 1 ; j >= 0 ; j-- ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i + BM1 ) != 0 ) {
				k = j;
				break;
			}
		}
		if ( k >= 0 ) {
			fprintf( fpt , "CHAMBER_PM_PLACE_DENY_FOR_BM%d" , i + 1 );
			for ( j = 0 ; j <= k ; j++ ) fprintf( fpt , "	%d" , _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i + BM1 ) );
			fprintf( fpt , "\n" );
		}
		//
	}
	//
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + BM1 ) != 0 ) { // 2006.09.26
			fprintf( fpt , "CHAMBER_FM_PICK_DENY_FOR_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + BM1 ) != 0 ) { // 2006.09.26
			fprintf( fpt , "CHAMBER_FM_PLACE_DENY_FOR_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i + BM1 ) );
		}
	}
	//
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 0 ) { // 2006.09.26
		fprintf( fpt , "CHAMBER_FM_PICK_DENY_FOR_FAL	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) != 0 ) { // 2006.09.26
		fprintf( fpt , "CHAMBER_FM_PLACE_DENY_FOR_FAL	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) != 0 ) { // 2006.09.26
		fprintf( fpt , "CHAMBER_FM_PICK_DENY_FOR_FIC	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) );
	}
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) != 0 ) { // 2006.09.26
		fprintf( fpt , "CHAMBER_FM_PLACE_DENY_FOR_FIC	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) );
	}
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.08.27
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( i + BM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_SLOT_FROM_ALL_OFFSET_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( i + BM1 ) );
		}
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( i , j + PM1 ) != 0 ) {
				fprintf( fpt , "CHAMBER_TM_INTERLOCK_%d(P%d)	%d\n" , i + 1 , j + 1 , _i_SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( i , j + PM1 ) );
			}
		}
		//
		if ( _i_SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( i ) != 0 ) {
			fprintf( fpt , "CHAMBER_TM_SINGLE_RUN_INTLKS_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( i ) );
		}
	}
	//
	if ( _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() != 0 ) {
		fprintf( fpt , "INTERLOCK_FM_BM_PLACE_SEPARATE	%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() );
	}
	//
	if ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() != 0 ) { // 2013.04.25
		fprintf( fpt , "INTERLOCK_TM_BM_OTHERGROUP_SWAP	%d\n" , _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() );
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2012.09.21
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i ) != 0 ) {
			fprintf( fpt , "CHAMBER_TM_PICK_ORDERING_%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2012.09.21
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i + PM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_TM_PICK_ORDER_SKIP_PM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2012.09.21
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i + BM1 ) != 0 ) {
			fprintf( fpt , "CHAMBER_TM_PICK_ORDER_SKIP_BM%d	%d\n" , i + 1 , _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i + BM1 ) );
		}
	}
	//
	//
	if ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() != 1 ) {
		fprintf( fpt , "DUMMY_PROCESS_MODE	%d\n" , _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() );
	}
	if ( _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() != 0 ) {
		fprintf( fpt , "DUMMY_PROCESS_FIXEDCLUSTER	%d\n" , _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() );
	}
	if ( _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() != 0 ) { // 2003.02.08
		fprintf( fpt , "DUMMY_PROCESS_MULTILOTACCESS	%d\n" , _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() != 0 ) {
		fprintf( fpt , "CM_SUPPLY_NOT_DELAY	%d\n" , _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() != 0 ) { // 2003.01.08
		fprintf( fpt , "PRE_PROCESS_DEPAND	%d\n" , _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() != 0 ) { // 2003.01.11
		fprintf( fpt , "START_PARALLEL_CHECK_SKIP	%d\n" , _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() != 0 ) {
		fprintf( fpt , "WAFER_SUPPLY_MODE	%d\n" , _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() != 0 ) { // 2003.10.09
		fprintf( fpt , "CLUSTER_INCLUDE_FOR_DISABLE	%d\n" , _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() != 0 ) { // 2004.02.19
		fprintf( fpt , "BMEND_SKIP_WHEN_RUNNING	%d\n" , _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() != 0 ) { // 2006.03.28
		fprintf( fpt , "PMREADY_SKIP_WHEN_RUNNING	%d\n" , _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 ) { // 2004.05.11
		fprintf( fpt , "LOT_LOG_PRE_POST_PROCESS	%d\n" , _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() );
	}
	//
	if ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() != 0 ) { // 2008.02.29
		fprintf( fpt , "ADAPTIVE_PROGRESSING	%d\n" , _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() );
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( i + PM1 ) != 0 ) {
			fprintf( fpt , "DUMMY_PROCESS_NOT_DEPAND_CHAMBER(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( i + PM1 ) );
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( i + PM1 ) != 0 ) {
			fprintf( fpt , "PRE_RECEIVE_WITH_PROCESS_RECIPE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( i + PM1 ) );
		}
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + CM1 ) != 0 ) {
			fprintf( fpt , "NEXT_FREE_PICK_POSSIBLE(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + CM1 ) );
		}
		if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + CM1 ) != 0 ) {
			fprintf( fpt , "PREV_FREE_PICK_POSSIBLE(C%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + CM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + PM1 ) != 0 ) {
			fprintf( fpt , "NEXT_FREE_PICK_POSSIBLE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + PM1 ) );
		}
		if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + PM1 ) != 0 ) {
			fprintf( fpt , "PREV_FREE_PICK_POSSIBLE(P%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + BM1 ) != 0 ) {
			fprintf( fpt , "NEXT_FREE_PICK_POSSIBLE(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i + BM1 ) );
		}
		if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + BM1 ) != 0 ) {
			fprintf( fpt , "PREV_FREE_PICK_POSSIBLE(B%d)	%d\n" , i + 1 , _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i + BM1 ) );
		}
	}
	//
	if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 0 ) {
		fprintf( fpt , "FA_LOADUNLOAD_SKIP	%d\n" , _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() );
	}
	if ( _i_SCH_PRM_GET_FA_MAPPING_SKIP() != 0 ) {
		fprintf( fpt , "FA_MAPPING_SKIP	%d\n" , _i_SCH_PRM_GET_FA_MAPPING_SKIP() );
	}
	if ( _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() != 0 ) {
		fprintf( fpt , "FA_SINGLE_CASS_MULTI_RUN	%d\n" , _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() );
	}
	if ( _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() != 0 ) {
		fprintf( fpt , "FA_STARTEND_STATUS_SHOW	%d\n" , _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() );
	}
	if ( _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() != 0 ) { // 2007.08.29
		fprintf( fpt , "FA_NORMALUNLOAD_NOTUSE	%d\n" , _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() );
	}

	if ( ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REJECT() != 0 ) ) {
		fprintf( fpt , "FA_SYSTEM_MESSAGE_LOAD	%d	%d	%d\n" ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REJECT() );
	}
	if ( ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT() != 0 ) ) {
		fprintf( fpt , "FA_SYSTEM_MESSAGE_UNLOAD	%d	%d	%d\n" ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT() );
	}
	if ( ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE() != 0 ) || ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT() != 0 ) ) {
		fprintf( fpt , "FA_SYSTEM_MESSAGE_MAPPING	%d	%d	%d\n" ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE() ,
			_i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT() );
	}

	if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() != 0 ) {
		fprintf( fpt , "FA_WINDOW_NORMAL_CHECK	%d\n" , _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() );
	}
	if ( _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() != 0 ) {
		fprintf( fpt , "FA_WINDOW_ABORT_CHECK	%d\n" , _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() );
	}
	if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() != 0 ) {
		fprintf( fpt , "FA_PROCESS_STEPCHANGE_CHECK	%d\n" , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() );
	}
	if ( _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() != 0 ) {
		fprintf( fpt , "FA_SEND_MESSAGE_DISPLAY	%d\n" , _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() );
	}
	if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 0 ) {
		fprintf( fpt , "FA_EXPAND_MESSAGE_USE	%d\n" , _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() );
	}
	if ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() != 0 ) {
		fprintf( fpt , "FA_SUBSTRATE_WAFER_ID	%d\n" , _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() );
	}
	if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() != 0 ) { // 2004.06.16
		fprintf( fpt , "FA_REJECTMESSAGE_DISPLAY	%d\n" , _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() );
	}

	if ( FA_SERVER_MODE_GET() != -1 ) fprintf( fpt , "FA_SERVER	%d	%s\n" , FA_SERVER_MODE_GET() , FA_SERVER_FUNCTION_NAME_GET() );

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		switch ( FA_AGV_MODE_GET(i) ) {
		case 1 :	fprintf( fpt , "FA_AGV%d	1	%s\n" , i + 1 , FA_AGV_FUNCTION_NAME_GET(i) );	break;
		case 0 :	fprintf( fpt , "FA_AGV%d	0	%s\n" , i + 1 , FA_AGV_FUNCTION_NAME_GET(i) );	break;
		}
	}
	if ( FA_FUNCTION_INTERFACE_GET() != 0 ) {
		fprintf( fpt , "FA_FUNCTION_INTERFACE	%d\n" , FA_FUNCTION_INTERFACE_GET() );
	}

	if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() != 0 ) { // 2002.09.05
		fprintf( fpt , "MESSAGE_USE_WHEN_SWITCH	%d\n" , _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() );
	}
	
	if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() != 0 ) { // 2004.06.24
		fprintf( fpt , "MESSAGE_USE_WHEN_ORDER	%d\n" , _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() );
	}

	if ( _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() != 0 ) { // 2004.08.14
		if ( SCH_SW_BM_SCHEDULING_FACTOR_ORDER_GET()[0] != 0 ) { // 2017.04.21
			fprintf( fpt , "SW_BM_SCHEDULING_FACTOR	%d	%s\n" , _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() , SCH_SW_BM_SCHEDULING_FACTOR_ORDER_GET() );
		}
		else {
			fprintf( fpt , "SW_BM_SCHEDULING_FACTOR	%d\n" , _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() );
		}
	}

	if ( _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() != 0 ) { // 2006.09.05
		fprintf( fpt , "BATCH_SUPPLY_INTERRUPT	%d\n" , _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() );
	}

	if ( _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() != 0 ) { // 2007.07.05
		fprintf( fpt , "DIFF_LOT_NOTSUP_MODE	%d\n" , _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() );
	}

	if ( _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() != 0 ) { // 2018.03.21
		fprintf( fpt , "METHOD1_TO_1BM_USING_MODE	%d\n" , _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() );
	}

	if ( strlen( SCHMULTI_DEFAULT_GROUP_GET() ) > 0 ) {
		fprintf( fpt , "CPJOB_DEFAULT_GROUP		0	%s\n" , SCHMULTI_DEFAULT_GROUP_GET() );
	}

	if ( SCHMULTI_CHECK_OVERTIME_HOUR_GET() > 0 ) { // 2012.04.20
		fprintf( fpt , "CPJOB_OVERTIME_CHECK_HOUR	%d\n" , SCHMULTI_CHECK_OVERTIME_HOUR_GET() );
	}

//	if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) { // 2002.12.27 // 2019.04.05
	if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() > 0 ) { // 2002.12.27 // 2019.04.05
//		fprintf( fpt , "PROCESS_LOG_WITH_CPJOB		1\n" ); // 2019.04.05
		fprintf( fpt , "PROCESS_LOG_WITH_CPJOB		%d\n" , PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ); // 2019.04.05
	}
	
//	if ( SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() != 0 ) { // 2003.09.24 // 2014.06.23
//		fprintf( fpt , "CPJOB_DEFAULT_LOTRECIPE_READ		%d\n" , SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() ); // 2014.06.23
//	} // 2014.06.23
	//
	if ( _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() != 0 ) { // 2003.09.24 // 2014.06.23
		fprintf( fpt , "CPJOB_DEFAULT_LOTRECIPE_READ		%d\n" , _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ); // 2014.06.23
	} // 2014.06.23
	//
	if ( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() != 0 ) { // 2004.05.11
		fprintf( fpt , "CPJOB_MAKE_MANUAL_CJPJMODE		%d\n" , SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() );
	}

	if ( SCHMULTI_JOBNAME_DISPLAYMODE_GET() != 0 ) { // 2005.05.11
		fprintf( fpt , "CPJOB_JOBNAME_DISPLAYMODE		%d\n" , SCHMULTI_JOBNAME_DISPLAYMODE_GET() );
	}

	if ( SCHMULTI_MESSAGE_SCENARIO_GET() != 0 ) { // 2006.04.13
		fprintf( fpt , "CPJOB_MESSAGE_SCENARIO		%d\n" , SCHMULTI_MESSAGE_SCENARIO_GET() );
	}

	if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING_MODE() ) { // 2013.09.13
		fprintf( fpt , "RECIPE_LOCKING		%d\n" , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() );
	}

	for ( i = 1 ; i <= 255 ; i++ ) { // 2013.08.01
		if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( i ) == 2 ) {
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) != 0 ) {
				fprintf( fpt , "MFI_INTERFACE_FLOW_USER_OPTION(%d)	%d\n" , i , _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) );
			}
		}
	}
	//
	if ( SCHFILE_USE_END_DELIMITER ) fprintf( fpt , "END_DELIMITER_CHECK	On\n" ); // 2017.02.09
	//
	fclose(fpt);
	return TRUE;
}

//-------------------------------------------------------------------------
// Function = FILE_ROBOT_SETUP
//-------------------------------------------------------------------------

BOOL FILE_ROBOT_SETUP_SAVE( char *Filename ) {
	FILE *fpt;
	int i , j , k;

	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21

	if ( ( fpt = fopen( Filename , "w" ) ) == NULL ) {
		printf( "[%s] File Not Found\n" , Filename );
		return TRUE;
	}
	//
	if ( ( MAX_CASSETTE_SIDE != 4 ) || ( ( MAX_PM_CHAMBER_DEPTH != 6 ) && ( MAX_PM_CHAMBER_DEPTH != 12 ) && ( MAX_PM_CHAMBER_DEPTH != 30 ) && ( MAX_PM_CHAMBER_DEPTH != 45 ) && ( MAX_PM_CHAMBER_DEPTH != 60 ) ) ) { // 2010.10.20
		fprintf( fpt , "ROBOT_MODULE	%d	%d	%d	%d	%d\n" , MAX_CASSETTE_SIDE , MAX_PM_CHAMBER_DEPTH , MAX_BM_CHAMBER_DEPTH , MAX_TM_CHAMBER_DEPTH , MAX_CASSETTE_SLOT_SIZE );
	}
	//

	if ( ROBOT_AUTO_MULTI_UPDOWN != 0 ) { // 2018.04.30
		fprintf( fpt , "ROBOT_AUTO_MULTI_UPDOWN	%d\n" , ROBOT_AUTO_MULTI_UPDOWN );
	}

	fprintf( fpt , "ROBOT_ANIMATION" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		switch ( _i_SCH_PRM_GET_RB_ROBOT_ANIMATION(i) ) {
		case 0 :	fprintf( fpt , "	Off" );		break;
		case 2 :	fprintf( fpt , "	Off(L)" );	break; // 2006.11.09
		case 3 :	fprintf( fpt , "	Off(R)" );	break; // 2006.11.09
		case 4 :	fprintf( fpt , "	Off(LR)" );	break; // 2006.11.09
		default :	fprintf( fpt , "	On" );		break;
		}
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "ROBOT_SYNCH_CHECK" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_RB_SYNCH_CHECK(i) ) fprintf( fpt , "	On" );
		else                             fprintf( fpt , "	Off" );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "ROTATION_STYLE" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		switch( _i_SCH_PRM_GET_RB_ROTATION_STYLE( i ) ) {
		case ROTATION_STYLE_CW :		fprintf( fpt , "	CW_ROTATION" );	break;
		case ROTATION_STYLE_CCW :		fprintf( fpt , "	CCW_ROTATION" );	break;
		case ROTATION_STYLE_MINIMUM :	fprintf( fpt , "	MINIMUM" );	break;
		case ROTATION_STYLE_HOME_CW :	fprintf( fpt , "	CW_HOMEBASED" );	break;
		default	:						fprintf( fpt , "	CCW_HOMEBASED" );	break;
		}
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "ARM_STYLE" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if      (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_ABCD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_ABC" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_ABD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_AB" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_ACD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_AC" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_AD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_A" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_BCD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_BC" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_BD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_B" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_CD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_C" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 3 ) )	fprintf( fpt , "	USING_D" );
		else																																																		fprintf( fpt , "	USING_A" );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "ARM_SEPERATE" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( i ) ) fprintf( fpt , "	ON" );
		else									fprintf( fpt , "	OFF" );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "ARM_DISPLAY" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if      (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_ABCD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_ABC" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_ABD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_AB" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_ACD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_AC" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_AD" );
		else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_A" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_BCD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_BC" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_BD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_B" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_CD" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_C" );
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 1 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 2 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( i , 3 ) )	fprintf( fpt , "	USING_D" );
		else																																																				fprintf( fpt , "	USING_A" );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "HOME" );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		fprintf( fpt , "    %.2lf" , _i_SCH_PRM_GET_RB_HOME_POSITION( i ) );
	}
	fprintf( fpt , "\n" );

	//==================================================================================================================================
	//==================================================================================================================================
	//==================================================================================================================================

	for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
		//
		for ( k = 0 ; k < MAX_FINGER_TM ; k++ ) {
			//
			if ( k >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) continue;
			//
			fprintf( fpt , "ROTATE_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_ROTATE ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k , RB_ANIM_ROTATE ) );
			fprintf( fpt , "EXTEND_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_EXTEND ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k , RB_ANIM_EXTEND ) );
			fprintf( fpt , "RETRACT_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_RETRACT ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k, RB_ANIM_RETRACT  ) );
			fprintf( fpt , "UP_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_UP ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k , RB_ANIM_UP ) );
			fprintf( fpt , "DOWN_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_DOWN ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k , RB_ANIM_DOWN ) );
			fprintf( fpt , "MOVE_%c_T%d" , k + 'A' , j );
			for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_POSITION( j , k , i , RB_ANIM_MOVE ) );
			fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_RNG( j , k , RB_ANIM_MOVE ) );
			//
		}
	}
	//==================================================================================================================================
	//==================================================================================================================================
	//==================================================================================================================================

	for ( j = 0 ; j < MAX_FM_CHAMBER_DEPTH ; j++ ) {
		//==================================================================================
		if ( _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() < j ) break;
		//==================================================================================
		switch ( _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION(j) ) {
		case 0 :	fprintf( fpt , "ROBOT_FM%s_ANIMATION		Off\n" , MULMDLSTR[ j ] );		break;
		case 2 :	fprintf( fpt , "ROBOT_FM%s_ANIMATION		Off(L)\n" , MULMDLSTR[ j ] );	break;	// 2006.11.09
		case 3 :	fprintf( fpt , "ROBOT_FM%s_ANIMATION		Off(R)\n" , MULMDLSTR[ j ] );	break;	// 2006.11.09
		case 4 :	fprintf( fpt , "ROBOT_FM%s_ANIMATION		Off(LR)\n" , MULMDLSTR[ j ] );	break;	// 2006.11.09
		default :	fprintf( fpt , "ROBOT_FM%s_ANIMATION		On\n" , MULMDLSTR[ j ] );		break;
		}

		if      ( _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK(j) == 1 )  fprintf( fpt , "ROBOT_FM%s_SYNCH_CHECK	On\n" , MULMDLSTR[ j ] );
		else if ( _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK(j) == 2 )  fprintf( fpt , "ROBOT_FM%s_SYNCH_CHECK	On2\n" , MULMDLSTR[ j ] );
		else if ( _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK(j) == 3 )  fprintf( fpt , "ROBOT_FM%s_SYNCH_CHECK	On3\n" , MULMDLSTR[ j ] );
		else                                           fprintf( fpt , "ROBOT_FM%s_SYNCH_CHECK	Off\n" , MULMDLSTR[ j ] );

		switch( _i_SCH_PRM_GET_RB_FM_ROTATION_STYLE(j) ) {
		case ROTATION_STYLE_CW :		fprintf( fpt , "ROTATION_FM%s_STYLE	CW_ROTATION\n" , MULMDLSTR[ j ] );	break;
		case ROTATION_STYLE_CCW :		fprintf( fpt , "ROTATION_FM%s_STYLE	CCW_ROTATION\n" , MULMDLSTR[ j ] );	break;
		case ROTATION_STYLE_MINIMUM :	fprintf( fpt , "ROTATION_FM%s_STYLE	MINIMUM\n" , MULMDLSTR[ j ] );	break;
		case ROTATION_STYLE_HOME_CW :	fprintf( fpt , "ROTATION_FM%s_STYLE	CW_HOMEBASED\n" , MULMDLSTR[ j ] );	break;
		default	:						fprintf( fpt , "ROTATION_FM%s_STYLE	CCW_HOMEBASED\n" , MULMDLSTR[ j ] );	break;
		}

		if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,1) )	fprintf( fpt , "ARM_FM%s_STYLE	USING_AB\n" , MULMDLSTR[ j ] );
		else if (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,0) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,1) )	fprintf( fpt , "ARM_FM%s_STYLE	USING_A\n" , MULMDLSTR[ j ] );
		else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(j,1) )	fprintf( fpt , "ARM_FM%s_STYLE	USING_B\n" , MULMDLSTR[ j ] );
		else																									fprintf( fpt , "ARM_FM%s_STYLE	USING_A\n" , MULMDLSTR[ j ] );

		if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE(j) ) fprintf( fpt , "ARM_FM%s_SEPERATE	ON\n" , MULMDLSTR[ j ] );
		else                                        fprintf( fpt , "ARM_FM%s_SEPERATE	OFF\n" , MULMDLSTR[ j ] );

		if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,1) )	fprintf( fpt , "ARM_FM%s_DISPLAY	USING_AB\n" , MULMDLSTR[ j ] );
		else if (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,0) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,1) )	fprintf( fpt , "ARM_FM%s_DISPLAY	USING_A\n" , MULMDLSTR[ j ] );
		else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(j,1) )	fprintf( fpt , "ARM_FM%s_DISPLAY	USING_B\n" , MULMDLSTR[ j ] );
		else																										fprintf( fpt , "ARM_FM%s_DISPLAY	USING_A\n" , MULMDLSTR[ j ] );

		if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE(j) != 0 ) fprintf( fpt , "ARMB_FM%s_DUAL	%d\n" , MULMDLSTR[ j ] , _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE(j) );

		fprintf( fpt , "HOME_FM%s     %.2lf\n" , MULMDLSTR[ j ] , _i_SCH_PRM_GET_RB_FM_HOME_POSITION(j) );

		fprintf( fpt , "ROTATE_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_ROTATE ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_ROTATE) );
		fprintf( fpt , "EXTEND_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_EXTEND ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_EXTEND) );
		fprintf( fpt , "RETRACT_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_RETRACT ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_RETRACT) );
		fprintf( fpt , "UP_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_UP ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_UP) );
		fprintf( fpt , "DOWN_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_DOWN ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_DOWN) );
		fprintf( fpt , "MOVE_FM%s" , MULMDLSTR[ j ] );
		for ( i = CM1 ; i < TM ; i++ ) fprintf( fpt , "	%.2lf" , _i_SCH_PRM_GET_RB_FM_POSITION( j , i , 1 , RB_ANIM_MOVE ) );
		fprintf( fpt , "	%.2lf\n" , _i_SCH_PRM_GET_RB_FM_RNG(j,RB_ANIM_MOVE) );
		
	}
	//==================================================================================================================================
	fclose(fpt);
	return TRUE;
}

void FILE_PARAM_SM_SCHEDULER_SETTING() { // 2013.08.22
	int i , Res;
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_LOADUNLOAD_SKIP"		, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_MAPPING_SKIP"			, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_MAPPING_SKIP() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_NORMALUNLOAD_NOTUSE"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_SINGLE_CASS_MULTI_RUN", 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_STARTEND_STATUS_SHOW"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_WINDOW_NORMAL_CHECK"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_WINDOW_ABORT_CHECK"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_PROCESS_STEPCHANGE"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_SEND_MESSAGE_DISPLAY"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_EXPAND_MESSAGE_USE"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_SUBSTRATE_WAFER_ID"	, 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_REJECTMESSAGE_DISPLAY", 0 , 0 , 0 , _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//									 
	Res = _SCH_CONFIG_INTERFACE_SET_STR2( "JOB_DEFAULT_GROUP"		, 0 , 0 , SCHMULTI_DEFAULT_GROUP_GET() );							if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	//									 
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_PRCS_MONITORING"		, 0 , 0 , 0 , PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
//	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_LOTRECIPE_READ"		, 0 , 0 , 0 , SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2014.06.23
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_LOTRECIPE_READ"		, 0 , 0 , 0 , _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2014.06.23
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_MAKE_MANUAL_CJPJMODE", 0 , 0 , 0 , SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_NAME_DISPLAYMODE"	, 0 , 0 , 0 , SCHMULTI_JOBNAME_DISPLAYMODE_GET() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_MESSAGE_SCENARIO"	, 0 , 0 , 0 , SCHMULTI_MESSAGE_SCENARIO_GET() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "JOB_CHECK_OVERTIME_HOUR"	, 0 , 0 , 0 , SCHMULTI_CHECK_OVERTIME_HOUR_GET() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_INT2( "FA_MAPPING_AFTERLOAD" , i , 0 , 0 , _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	}
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "RECIPE_LOCKING"          , 0 , 0 , 0 , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT2( "SYSTEM_LOG_STYLE"        , 0 , 0 , 0 , _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2016.10.21
	//
}										 

void FILE_PARAM_SM_SCHEDULER_SETTING2( int mode , int id ) { // 2013.08.22
	int Res;
	switch ( mode ) {
	case 1 :
		Res = _SCH_CONFIG_INTERFACE_SET_STR2( "GROUP_RECIPE_PATH" , id , 0 , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(id) );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING2 Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
		break;
	case 2 : // 2013.09.03
		Res = _SCH_CONFIG_INTERFACE_SET_INT2( "MTLOUT_INTERFACE" , 0 , 0 , 0 , _i_SCH_PRM_GET_MTLOUT_INTERFACE() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SCHEDULER_SETTING2 Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
		break;
	}
}

BOOL FILE_PARAM_SM_SETUP() { // 2013.08.22
	int Res , i , addr[256];
	//
	//===================================================================================================================
	//
	Res = _SCH_CONFIG_INTERFACE_INITIALIZING( GET_SYSTEM_NAME() , TRUE , 64 ,16 );
	if ( Res != 0 ) {
		_IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_INITIALIZING [%d]\n" , Res );
		return FALSE;
	}
	//
	//===================================================================================================================
	//
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 0 , MAX_CASSETTE_SIDE );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 1 , MAX_CASSETTE_SLOT_SIZE );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 2 , MAX_PM_CHAMBER_DEPTH );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 3 , MAX_BM_CHAMBER_DEPTH );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 4 , MAX_TM_CHAMBER_DEPTH );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 5 , MAX_PM_SLOT_DEPTH );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_HEADER( 6 , MAX_CLUSTER_DEPTH );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_HEADER [%d]\n" , Res );
	//
	//===================================================================================================================
	//-----------
	// SYSTEM
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "DFIX_METHOD_NAME"		, _K_S_IO , 128			, 0		, 0 , &(addr[0]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE DFIX_METHOD_NAME [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "DFIX_LOG_PATH"			, _K_S_IO , 128			, 0		, 0 , &(addr[1]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE DFIX_LOG_PATH [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MAX_CASSETTE_SLOT"		, _K_D_IO , 1			, 0		, 0 , &(addr[2]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MAX_CASSETTE_SLOT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "LOG_ENCRIPTION"		, _K_D_IO , 1			, 0		, 0 , &(addr[3]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE LOG_ENCRIPTION [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "EIL_INTERFACE"			, _K_D_IO , 1			, 0		, 0 , &(addr[4]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE EIL_INTERFACE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MTLOUT_INTERFACE"		, _K_D_IO , 1			, 0		, 0 , &(addr[5]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MTLOUT_INTERFACE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "RECIPE_LOCKING"		, _K_D_IO , 1			, 0		, 0 , &(addr[6]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE RECIPE_LOCKING [%d]\n" , Res );

	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MESSAGE_MAINTINTERFACE", _K_D_IO , 1			, 0		, 0 , &(addr[7]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MESSAGE_MAINTINTERFACE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "CTJOB_USE_SET"			, _K_D_IO , 1			, 0		, 0 , &(addr[8]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE CTJOB_USE_SET [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "TRANSFER_CONTROL_MODE"	, _K_D_IO , 1			, 0		, 0 , &(addr[9]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE TRANSFER_CONTROL_MODE [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MODULE_GROUP"			, _K_D_IO , MAX_CHAMBER	, 0		, 0 , &(addr[10]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MODULE_GROUP [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "DFIX_TMP_PATH"			, _K_S_IO , 128			, 0		, 0 , &(addr[11]) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE DFIX_TMP_PATH [%d]\n" , Res ); // 2017.10.30
	//-----------
	// SCHEDULE
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_LOADUNLOAD_SKIP"		, _K_D_IO , 1			, 0		, 0 , &(addr[21]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_MAPPING_SKIP"			, _K_D_IO , 1			, 0		, 0 , &(addr[22]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_NORMALUNLOAD_NOTUSE"	, _K_D_IO , 1			, 0		, 0 , &(addr[23]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_SINGLE_CASS_MULTI_RUN"	, _K_D_IO , 1			, 0		, 0 , &(addr[24]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_STARTEND_STATUS_SHOW"	, _K_D_IO , 1			, 0		, 0 , &(addr[25]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_WINDOW_NORMAL_CHECK"	, _K_D_IO , 1			, 0		, 0 , &(addr[26]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_WINDOW_ABORT_CHECK"		, _K_D_IO , 1			, 0		, 0 , &(addr[27]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_PROCESS_STEPCHANGE"		, _K_D_IO , 1			, 0		, 0 , &(addr[28]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_SEND_MESSAGE_DISPLAY"	, _K_D_IO , 1			, 0		, 0 , &(addr[29]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_EXPAND_MESSAGE_USE"		, _K_D_IO , 1			, 0		, 0 , &(addr[30]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_SUBSTRATE_WAFER_ID"		, _K_D_IO , 1			, 0		, 0 , &(addr[31]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_REJECTMESSAGE_DISPLAY"	, _K_D_IO , 1			, 0		, 0 , &(addr[32]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );

	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_DEFAULT_GROUP"			, _K_S_IO , 128			, 0		, 0 , &(addr[33]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );

	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_PRCS_MONITORING"		, _K_D_IO , 1			, 0		, 0 , &(addr[34]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_LOTRECIPE_READ"		, _K_D_IO , 1			, 0		, 0 , &(addr[35]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_MAKE_MANUAL_CJPJMODE"	, _K_D_IO , 1			, 0		, 0 , &(addr[36]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_NAME_DISPLAYMODE"		, _K_D_IO , 1			, 0		, 0 , &(addr[37]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_MESSAGE_SCENARIO"		, _K_D_IO , 1			, 0		, 0 , &(addr[38]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "JOB_CHECK_OVERTIME_HOUR"	, _K_D_IO , 1			, 0		, 0 , &(addr[39]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "FA_MAPPING_AFTERLOAD"		, _K_D_IO , MAX_CASSETTE_SIDE , 0 , 0 , &(addr[40]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "SYSTEM_LOG_STYLE"			, _K_D_IO , 1			, 0		, 0 , &(addr[41]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE SYSTEM_LOG_STYLE [%d]\n" , Res ); // 2016.10.21
	//
	//-----------
	// FILE
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MAIN_RECIPE_PATHF"		, _K_S_IO , 128			, 0		, 0 , &(addr[61]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MAIN_RECIPE_PATHF [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MAIN_RECIPE_PATH"		, _K_S_IO , MAX_CHAMBER , 128	, 0 , &(addr[62]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MAIN_RECIPE_PATH [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "MAIN_RECIPE_PATHM"		, _K_S_IO , MAX_CHAMBER , 128	, 0 , &(addr[63]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE MAIN_RECIPE_PATHM [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "LOT_RECIPE_PATH"		, _K_S_IO , 128			, 0		, 0 , &(addr[64]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE LOT_RECIPE_PATH [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "CLUSTER_RECIPE_PATH"	, _K_S_IO , 128			, 0		, 0 , &(addr[65]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE CLUSTER_RECIPE_PATH [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "PROCESS_RECIPE_PATHF"	, _K_S_IO , MAX_CHAMBER , 128	, 0 , &(addr[66]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE PROCESS_RECIPE_PATHF [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "PROCESS_RECIPE_PATH"	, _K_S_IO , MAX_CHAMBER , 128	, 0 , &(addr[67]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE PROCESS_RECIPE_PATH [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "PROCESS_RECIPE_TYPE"	, _K_D_IO , MAX_CHAMBER , 0		, 0 , &(addr[68]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE PROCESS_RECIPE_TYPE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "PROCESS_RECIPE_FILE"	, _K_D_IO , MAX_CHAMBER , 0		, 0 , &(addr[69]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE PROCESS_RECIPE_FILE [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE( "GROUP_RECIPE_PATH"		, _K_S_IO , MAX_CASSETTE_SIDE , 128	, 0 , &(addr[70]) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE GROUP_RECIPE_PATH [%d]\n" , Res );

	//
	//===================================================================================================================
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_PREPARE_COMPLETE();
	if ( Res != 0 ) {
		_IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_PREPARE_COMPLETE [%d]\n" , Res );
		return FALSE;
	}
	//
	//===================================================================================================================
	//-----------
	// SYSTEM
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[0] , 0 , 0 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[1] , 0 , 0 , _i_SCH_PRM_GET_DFIX_LOG_PATH() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[2] , 0 , 0 , 0 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[3] , 0 , 0 , 0 , _i_SCH_PRM_GET_LOG_ENCRIPTION() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[4] , 0 , 0 , 0 , _i_SCH_PRM_GET_EIL_INTERFACE() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[5] , 0 , 0 , 0 , _i_SCH_PRM_GET_MTLOUT_INTERFACE() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[6] , 0 , 0 , 0 , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[7] , 0 , 0 , 0 , _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[8] , 0 , 0 , 0 , SCHMULTI_CTJOB_USE_GET() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[9] , 0 , 0 , 0 , _i_SCH_PRM_GET_MODULE_TRANSFER_CONTROL_MODE() );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[10] , i , 0 , 0 , _i_SCH_PRM_GET_MODULE_GROUP(i) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	}
	//
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[11] , 0 , 0 , _i_SCH_PRM_GET_DFIX_TMP_PATH() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res ); // 2017.10.30
	//
	//-----------
	// SCHEDULE
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[21] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[22] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_MAPPING_SKIP() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[23] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[24] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[25] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[26] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[27] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[28] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() );if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[29] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[30] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[31] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[32] , 0 , 0 , 0 , _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[33] , 0 , 0 , SCHMULTI_DEFAULT_GROUP_GET() );							if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[34] , 0 , 0 , 0 , PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
//	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[35] , 0 , 0 , 0 , SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2014.06.23
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[35] , 0 , 0 , 0 , _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2014.06.23
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[36] , 0 , 0 , 0 , SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[37] , 0 , 0 , 0 , SCHMULTI_JOBNAME_DISPLAYMODE_GET() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[38] , 0 , 0 , 0 , SCHMULTI_MESSAGE_SCENARIO_GET() );					if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[39] , 0 , 0 , 0 , SCHMULTI_CHECK_OVERTIME_HOUR_GET() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[40] , i , 0 , 0 , _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	}
	//
	Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[41] , 0 , 0 , 0 , _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() );				if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res ); // 2016.10.21
	//
	//-----------
	// FILE
	//-----------
	//
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[61] , 0 , 0 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[62] , i , 0 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH(i) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
		Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[63] , i , 0 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM(i) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	}
	//
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[64] , 0 , 0 , _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[65] , 0 , 0 , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[66] , i , 0 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF(i) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
		Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[67] , i , 0 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH(i) );		if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
		Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[68] , i , 0 , 0 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE(i) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
		Res = _SCH_CONFIG_INTERFACE_SET_INT( addr[69] , i , 0 , 0 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE(i) );	if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_INT [%d]\n" , Res );
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		Res = _SCH_CONFIG_INTERFACE_SET_STR( addr[70] , i , 0 , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(i) );			if ( Res != 0 ) _IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_SET_STR [%d]\n" , Res );
	}
	//===================================================================================================================
	//
	Res = _SCH_CONFIG_INTERFACE_CREATE_DATASET_COMPLETE();
	if ( Res != 0 ) {
		_IO_CIM_PRINTF( "FILE_PARAM_SM_SETUP Fail in _SCH_CONFIG_INTERFACE_CREATE_DATASET_COMPLETE [%d]\n" , Res );
		return FALSE;
	}
	//
	return TRUE;
	//
}
