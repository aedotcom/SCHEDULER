#ifndef USER_PARAMETER_H
#define USER_PARAMETER_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
extern char    COMMON_BLANK_CHAR[2];
//----------------------------------------------------------------------------
#define MAKE_DEFAULT_INT_PARAMETER( ITEM , DEFAULT ) int ITEM ; \
	void _i_SCH_PRM_SET_##ITEM##( int data )	{ ITEM = data; } \
	int  _i_SCH_PRM_GET_##ITEM##()				{ return ITEM; }

#define MAKE_DEFAULT_INT_PARAMETER2( ITEM , ITEM2 ) int ITEM[ITEM2]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , int data2 )	{ ITEM[data] = data2; } \
	int  _i_SCH_PRM_GET_##ITEM##( int data )				{ return ITEM[data]; }

#define MAKE_DEFAULT_INT_PARAMETER3( ITEM , ITEM2 , ITEM3 ) int ITEM[ITEM2][ITEM3]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 )	{ ITEM[data][data2] = data3; } \
	int  _i_SCH_PRM_GET_##ITEM##( int data , int data2 )				{ return ITEM[data][data2]; }

#define MAKE_DEFAULT_DOUBLE_PARAMETER( ITEM , DEFAULT ) double ITEM = DEFAULT; \
	void _i_SCH_PRM_SET_##ITEM##( double data )	{ ITEM = data; } \
	double _i_SCH_PRM_GET_##ITEM##()				{ return ITEM; }

#define MAKE_DEFAULT_DOUBLE_PARAMETER2( ITEM , ITEM2 ) double ITEM[ITEM2]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , double data2 )	{ ITEM[data] = data2; } \
	double _i_SCH_PRM_GET_##ITEM##( int data )					{ return ITEM[data]; }

#define MAKE_DEFAULT_DOUBLE_PARAMETER3( ITEM , ITEM2 , ITEM3 ) double ITEM[ITEM2][ITEM3]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , double data3 )	{ ITEM[data][data2] = data3; } \
	double _i_SCH_PRM_GET_##ITEM##( int data , int data2 )					{ return ITEM[data][data2]; }

#define MAKE_DEFAULT_DOUBLE_PARAMETER4( ITEM , ITEM2 , ITEM3 , ITEM4 ) double ITEM[ITEM2][ITEM3][ITEM4]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 , double data4 )	{ ITEM[data][data2][data3] = data4; } \
	double _i_SCH_PRM_GET_##ITEM##( int data , int data2 , int data3 )					{ return ITEM[data][data2][data3]; }

#define MAKE_DEFAULT_DOUBLE_PARAMETER5( ITEM , ITEM2 , ITEM3 , ITEM4 , ITEM5 ) double ITEM[ITEM2][ITEM3][ITEM4][ITEM5]; \
	void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 , int data4 , double data5 )	{ ITEM[data][data2][data3][data4] = data5; } \
	double _i_SCH_PRM_GET_##ITEM##( int data , int data2 , int data3 , int data4 )					{ return ITEM[data][data2][data3][data4]; }

/*
//
// 2015.04.09
//
#define MAKE_DEFAULT_CHAR_PARAMETER( ITEM ) char *##ITEM##; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( char *data )	{ \
		int l = strlen( data ); \
		if ( Init_##ITEM## == 0 ) { \
			ITEM = NULL; \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM != NULL ) { \
			free( ITEM ); \
			ITEM = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM != NULL ) strcpy( ITEM , data ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##() { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM; \
	}

#define MAKE_DEFAULT_CHAR_PARAMETER2( ITEM , ITEM2 ) char *##ITEM##[##ITEM2##]; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( int data , char *data2 )	{ \
		int i , l = strlen( data2 ); \
		if ( Init_##ITEM## == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) ITEM[i] = NULL; \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM[data] != NULL ) { \
			free( ITEM[data] ); \
			ITEM[data] = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM[data] = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM[data] != NULL ) strcpy( ITEM[data] , data2 ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##( int data ) { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM[data] == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM[data]; \
	}

#define MAKE_DEFAULT_CHAR_PARAMETER3( ITEM , ITEM2 , ITEM3 ) char *##ITEM##[##ITEM2##][##ITEM3##]; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( int data0 , int data , char *data2 )	{ \
		int i , j , l = strlen( data2 ); \
		if ( Init_##ITEM## == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) { \
				for ( j = 0 ; j < ITEM3 ; j++ ) { \
					ITEM[i][j] = NULL; \
				} \
			} \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM[data0][data] != NULL ) { \
			free( ITEM[data0][data] ); \
			ITEM[data0][data] = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM[data0][data] = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM[data0][data] != NULL ) strcpy( ITEM[data0][data] , data2 ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##( int data0 , int data ) { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM[data0][data] == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM[data0][data]; \
	}




*/

//
// 2015.04.09
//
#define MAKE_DEFAULT_CHAR_PARAMETER( ITEM ) char *##ITEM##; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( char *data )	{ \
		unsigned int l = strlen( data ); \
		if ( Init_##ITEM## == 0 ) { \
			ITEM = NULL; \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM != NULL ) { \
			if ( l <= (_msize( ITEM )-1) ) { \
				strcpy( ITEM , data ); \
				return; \
			} \
			free( ITEM ); \
			ITEM = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM != NULL ) strcpy( ITEM , data ); \
			else 				_IO_CIM_PRINTF( "_i_SCH_PRM_SET_?? Memory Allocation Fail 1\n" ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##() { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM; \
	}

#define MAKE_DEFAULT_CHAR_PARAMETER2( ITEM , ITEM2 ) char *##ITEM##[##ITEM2##]; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( int data , char *data2 )	{ \
		int i; \
		unsigned int l = strlen( data2 ); \
		if ( Init_##ITEM## == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) ITEM[i] = NULL; \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM[data] != NULL ) { \
			if ( l <= (_msize( ITEM[data]  )-1) ) { \
				strcpy( ITEM[data] , data2 ); \
				return; \
			} \
			free( ITEM[data] ); \
			ITEM[data] = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM[data] = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM[data] != NULL ) strcpy( ITEM[data] , data2 ); \
			else 					_IO_CIM_PRINTF( "_i_SCH_PRM_SET_?? Memory Allocation Fail 2\n" ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##( int data ) { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM[data] == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM[data]; \
	}

#define MAKE_DEFAULT_CHAR_PARAMETER3( ITEM , ITEM2 , ITEM3 ) char *##ITEM##[##ITEM2##][##ITEM3##]; \
	char Init_##ITEM## = 0; \
	void _i_SCH_PRM_SET_##ITEM##( int data0 , int data , char *data2 )	{ \
		int i , j; \
		unsigned int l = strlen( data2 ); \
		if ( Init_##ITEM## == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) { \
				for ( j = 0 ; j < ITEM3 ; j++ ) { \
					ITEM[i][j] = NULL; \
				} \
			} \
			Init_##ITEM## = 1; \
		} \
		if ( ITEM[data0][data] != NULL ) { \
			if ( l <= (_msize( ITEM[data0][data] )-1) ) { \
				strcpy( ITEM[data0][data] , data2 ); \
				return; \
			} \
			free( ITEM[data0][data] ); \
			ITEM[data0][data] = NULL; \
		} \
		if ( l > 0 ) { \
			ITEM[data0][data] = calloc( l + 1 , sizeof( char ) ); \
			if ( ITEM[data0][data] != NULL ) strcpy( ITEM[data0][data] , data2 ); \
			else 							_IO_CIM_PRINTF( "_i_SCH_PRM_SET_?? Memory Allocation Fail 3\n" ); \
		} \
	} \
	char *_i_SCH_PRM_GET_##ITEM##( int data0 , int data ) { \
		if ( Init_##ITEM## == 0 ) return COMMON_BLANK_CHAR; \
		if ( ITEM[data0][data] == NULL ) return COMMON_BLANK_CHAR; \
		return ITEM[data0][data]; \
	}

//======================================================================================================================

#define MAKE_DEFAULT_INT_PARAMETER_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data ); \
	int  _i_SCH_PRM_GET_##ITEM##();

#define MAKE_DEFAULT_INT_PARAMETER2_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 ); \
	int  _i_SCH_PRM_GET_##ITEM##( int data );

#define MAKE_DEFAULT_INT_PARAMETER3_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 ); \
	int  _i_SCH_PRM_GET_##ITEM##( int data , int data2 );

#define MAKE_DEFAULT_DOUBLE_PARAMETER_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( double data ); \
	double _i_SCH_PRM_GET_##ITEM##();

#define MAKE_DEFAULT_DOUBLE_PARAMETER2_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , double data2 ); \
	double  _i_SCH_PRM_GET_##ITEM##( int data );

#define MAKE_DEFAULT_DOUBLE_PARAMETER3_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , double data3 ); \
	double  _i_SCH_PRM_GET_##ITEM##( int data , int data2 );

#define MAKE_DEFAULT_DOUBLE_PARAMETER4_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 , double data4 ); \
	double  _i_SCH_PRM_GET_##ITEM##( int data , int data2 , int data3 );

#define MAKE_DEFAULT_DOUBLE_PARAMETER5_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 , int data4 , double data5 ); \
	double  _i_SCH_PRM_GET_##ITEM##( int data , int data2 , int data3 , int data4 );

#define MAKE_DEFAULT_CHAR_PARAMETER_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( char *data ); \
	char *_i_SCH_PRM_GET_##ITEM##();

#define MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , char *data2 ); \
	char *_i_SCH_PRM_GET_##ITEM##( int data );

#define MAKE_DEFAULT_CHAR_PARAMETER3_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data0 , int data , char *data2 ); \
	char *_i_SCH_PRM_GET_##ITEM##( int data0 , int data );

//======================================================================================================================

#define MAKE_DEFAULT_INT_PARAMETER_EXTERN( ITEM , ID ) \
	case ID        :                 _i_SCH_PRM_SET_##ITEM##( (int) a1 ); break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##(); break;

#define MAKE_DEFAULT_INT_PARAMETER2_EXTERN( ITEM , ID ) \
	case ID        :                 _i_SCH_PRM_SET_##ITEM##( (int) a1 , (int) a2 ); break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##( (int) a1 ); break;

#define MAKE_DEFAULT_INT_PARAMETER3_EXTERN( ITEM , ID ) \
	case ID        :                 _i_SCH_PRM_SET_##ITEM##( (int) a1 , (int) a2 , (int) a3 ); break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##( (int) a1 , (int) a2 ); break;

// 2012.07.26
#define MAKE_DEFAULT_CHAR_PARAMETER_EXTERN( ITEM , ID ) \
	case ID        :                  _i_SCH_PRM_SET_##ITEM##( (char *) a1 );  break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##(); break;

#define MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN( ITEM , ID ) \
	case ID        :                  _i_SCH_PRM_SET_##ITEM##( (int) a1 , (char *) a2 );  break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##( (int) a1 ); break;

// 2013.08.22
#define MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN_FILE_PARAM_SM1( ITEM , ID ) \
	case ID        :	\
						_i_SCH_PRM_SET_##ITEM##( (int) a1 , (char *) a2 ); \
						FILE_PARAM_SM_SCHEDULER_SETTING2( 1 , (int) a1 ); \
						break; \
	case ID + 1000 : return (LPARAM) _i_SCH_PRM_GET_##ITEM##( (int) a1 ); break;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Module Fixed Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MODE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MODE_for_SW )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_SERVICE_MODE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_GROUP )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_GROUP_HAS_SWITCH )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_STOCK );

MAKE_DEFAULT_INT_PARAMETER3_HEADER( MODULE_PICK_GROUP ) // 2006.01.04
MAKE_DEFAULT_INT_PARAMETER3_HEADER( MODULE_PLACE_GROUP ) // 2006.01.04
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_TMS_GROUP ) // 2013.03.19

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_INTERFACE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_SIZE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_SIZE_CHANGE )

MAKE_DEFAULT_INT_PARAMETER3_HEADER( MODULE_BUFFER_MODE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_BUFFER_FULLRUN )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_BUFFER_LASTORDER ) // 2003.06.02

//MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_BUFFER_SWITCH_SWAPPING ) // 2003.11.07 // 2013.03.18

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_BUFFER_M_SWITCH_SWAPPING ) // 2013.03.18

#define _i_SCH_PRM_SET_MODULE_BUFFER_SWITCH_SWAPPING( data )	_i_SCH_PRM_SET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 , data )
#define _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING()			_i_SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_DOUBLE_WHAT_SIDE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_DUMMY_BUFFER_CHAMBER )

MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_ALIGN_BUFFER_CHAMBER )

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( MODULE_PROCESS_NAME )

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( MODULE_COMMSTATUS_IO_NAME )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_PREPRECV )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_PREPSEND )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_POSTRECV )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_POSTSEND )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_GATE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_MESSAGE_NOTUSE_CANCEL )	// 2016.11.24

MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_COMMSTATUS_MODE ); // 2003.06.11
MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_COMMSTATUS2_MODE ); // 2012.07.21
MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_TRANSFER_CONTROL_MODE ); // 2003.06.25

MAKE_DEFAULT_INT_PARAMETER_HEADER( MULTI_GROUP_AUTO_ARRANGE ); // 2006.02.08
MAKE_DEFAULT_INT_PARAMETER_HEADER( PRCS_ZONE_RUN_TYPE ); // 2006.05.04

MAKE_DEFAULT_INT_PARAMETER_HEADER( LOT_END_ESTIMATION ); // 2006.08.11

MAKE_DEFAULT_INT_PARAMETER_HEADER( PICKPLACE_CANCEL_MESSAGE ); // 2008.02.01

MAKE_DEFAULT_INT_PARAMETER_HEADER( TIMER_RESET_WHEN_NEW_START ); // 2008.04.17

MAKE_DEFAULT_INT_PARAMETER_HEADER( CHECK_DIFFERENT_METHOD ); // 2008.07.09
MAKE_DEFAULT_INT_PARAMETER_HEADER( CHECK_DIFFERENT_METHOD2 ); // 2018.08.22

MAKE_DEFAULT_INT_PARAMETER_HEADER( LOG_ENCRIPTION ); // 2010.04.14

MAKE_DEFAULT_INT_PARAMETER_HEADER( PROCESS_MESSAGE_STYLE ); // 2010.09.01

MAKE_DEFAULT_INT_PARAMETER_HEADER( WAFER_ANIM_STATUS_CHECK ); // 2010.09.13

MAKE_DEFAULT_INT_PARAMETER_HEADER( MAINT_MESSAGE_STYLE ); // 2010.09.21

MAKE_DEFAULT_INT_PARAMETER_HEADER( EIL_INTERFACE ); // 2010.09.13
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( EIL_INTERFACE_JOBLOG ) // 2011.06.20
MAKE_DEFAULT_INT_PARAMETER2_HEADER( EIL_INTERFACE_FLOW_USER_OPTION ) // 2012.09.03

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MFI_INTERFACE_FLOW_USER_OPTION ); // 2012.12.05
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MFI_INTERFACE_FLOW_USER_OPTIONF ); // 2013.08.01
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( MFI_INTERFACE_FLOW_USER_OPTIONS ); // 2013.08.01

MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_MESSAGE ); // 2010.11.30

MAKE_DEFAULT_INT_PARAMETER_HEADER( MODULE_PICK_ORDERING_USE ); // 2010.10.15
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_PICK_ORDERING ) // 2010.10.15
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_PICK_ORDERING_SKIP ) // 2012.08.23

MAKE_DEFAULT_INT_PARAMETER_HEADER( MTLOUT_INTERFACE ); // 2011.05.20

MAKE_DEFAULT_INT_PARAMETER_HEADER( MESSAGE_MAINTINTERFACE ) // 2013.04.25

MAKE_DEFAULT_INT_PARAMETER_HEADER( USE_EXTEND_OPTION ) // 2013.06.20

MAKE_DEFAULT_INT_PARAMETER_HEADER( OPTIMIZE_MODE ) // 2017.12.07
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Side Area Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_SWITCH_BUFFER )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( MODULE_SWITCH_BUFFER_ACCESS_TYPE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( BATCH_SUPPLY_INTERRUPT ); // 2006.09.05
MAKE_DEFAULT_INT_PARAMETER_HEADER( DIFF_LOT_NOTSUP_MODE ); // 2007.07.05

MAKE_DEFAULT_INT_PARAMETER_HEADER( METHOD1_TO_1BM_USING_MODE ); // 2018.03.21
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Module Factor Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( Getting_Priority )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( Putting_Priority )

MAKE_DEFAULT_INT_PARAMETER_HEADER( PRCS_TIME_X_PLACE ) // 2006.05.04

MAKE_DEFAULT_INT_PARAMETER2_HEADER( PRCS_TIME_WAIT_RANGE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( PRCS_TIME_REMAIN_RANGE ) // 2006.05.04

MAKE_DEFAULT_INT_PARAMETER2_HEADER( STATION_PICK_HANDLING )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( STATION_PLACE_HANDLING )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( ANIMATION_ROTATE_PREPARE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( Process_Run_In_Mode )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( Process_Run_In_Time )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( Process_Run_Out_Mode )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( Process_Run_Out_Time )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MAPPING_SKIP )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( AUTO_HANDOFF )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( PRE_RECEIVE_WITH_PROCESS_RECIPE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( NEXT_FREE_PICK_POSSIBLE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( PREV_FREE_PICK_POSSIBLE )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( ERROR_OUT_CHAMBER_FOR_CASSETTE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Offset Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER3_HEADER( OFFSET_SLOT_FROM_CM )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( OFFSET_SLOT_FROM_ALL )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Interlock Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER3_HEADER( INTERLOCK_PM_RUN_FOR_CM )
MAKE_DEFAULT_INT_PARAMETER3_HEADER( INTERLOCK_TM_RUN_FOR_ALL )

//MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_PM_PICK_DENY_FOR_MDL ) // 2013.05.28
//MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_PM_PLACE_DENY_FOR_MDL ) // 2013.05.28

MAKE_DEFAULT_INT_PARAMETER3_HEADER( INTERLOCK_PM_M_PICK_DENY_FOR_MDL ) // 2013.05.28
MAKE_DEFAULT_INT_PARAMETER3_HEADER( INTERLOCK_PM_M_PLACE_DENY_FOR_MDL ) // 2013.05.28

#define _i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( ch , data )	_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0 , ch , data )
#define _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( ch )			_i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0 , ch )

#define _i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( ch , data )	_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0 , ch , data )
#define _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( ch )		_i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0 , ch )

MAKE_DEFAULT_INT_PARAMETER3_HEADER( INTERLOCK_FM_ROBOT_FINGER )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_FM_PICK_DENY_FOR_MDL ) // 2006.09.26
MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_FM_PLACE_DENY_FOR_MDL ) // 2006.09.26

MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM ); // 2004.01.30 // None Pick Place
MAKE_DEFAULT_INT_PARAMETER2_HEADER( INTERLOCK_TM_SINGLE_RUN ); // 2007.01.15
MAKE_DEFAULT_INT_PARAMETER_HEADER( INTERLOCK_FM_BM_PLACE_SEPARATE ); // 2007.11.15
MAKE_DEFAULT_INT_PARAMETER_HEADER( INTERLOCK_TM_BM_OTHERGROUP_SWAP ); // 2013.04.25
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Dummy Process Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_PROCESS_CHAMBER )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_PROCESS_SLOT )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_PROCESS_MODE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_PROCESS_FIXEDCLUSTER )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( DUMMY_PROCESS_NOT_DEPAND_CHAMBER )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DUMMY_PROCESS_MULTI_LOT_ACCESS )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( SDUMMY_PROCESS_CHAMBER )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( SDUMMY_PROCESS_SLOT )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Fixed Cluster Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_PRE_MODE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_PRE_IN_USE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_PRE_OUT_USE )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( FIXCLUSTER_PRE_IN_RECIPE_NAME )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( FIXCLUSTER_PRE_OUT_RECIPE_NAME )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_POST_MODE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_POST_IN_USE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FIXCLUSTER_POST_OUT_USE )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( FIXCLUSTER_POST_IN_RECIPE_NAME )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( FIXCLUSTER_POST_OUT_RECIPE_NAME )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pre/Post Run Recipe Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( READY_RECIPE_NAME )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( READY_RECIPE_USE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( READY_RECIPE_MINTIME )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( READY_RECIPE_TYPE ) // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( END_RECIPE_NAME )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( END_RECIPE_USE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( END_RECIPE_MINTIME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( READY_MULTIJOB_MODE );

//
// 2016.12.09
//

//		IN(1)	OUT(2)	MOVE(3)

//FM	PICK	PLACE	ROTATE
//BM	TMSIDE	FMSIDE	x
//TM	PICK	PLACE	ROTATE
//PM	MAIN	POST	PRE

MAKE_DEFAULT_INT_PARAMETER2_HEADER( SIM_TIME1 )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( SIM_TIME2 )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( SIM_TIME3 )


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Fail Scenario Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FAIL_PROCESS_SCENARIO )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( STOP_PROCESS_SCENARIO )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MRES_SUCCESS_SCENARIO ) // 2003.09.25

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MRES_FAIL_SCENARIO )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( MRES_ABORT_SCENARIO )

MAKE_DEFAULT_INT_PARAMETER_HEADER( MRES_ABORT_ALL_SCENARIO )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DISABLE_MAKE_HOLE_GOTOSTOP )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Clear Scenario Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( CLSOUT_USE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( CLSOUT_DELAY )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( CLSOUT_MESSAGE_USE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( CLSOUT_BM_SINGLE_ONLY )

MAKE_DEFAULT_INT_PARAMETER_HEADER( MAINTINF_DB_USE ) // 2012.05.04

MAKE_DEFAULT_INT_PARAMETER_HEADER( CLSOUT_AL_TO_PM ) // 2005.12.16
MAKE_DEFAULT_INT_PARAMETER_HEADER( CLSOUT_IC_TO_PM ) // 2005.12.16
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Robot Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_ROBOT_ANIMATION )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_FINGER_ARM_SEPERATE )

MAKE_DEFAULT_INT_PARAMETER3_HEADER( RB_FINGER_ARM_STYLE )
MAKE_DEFAULT_INT_PARAMETER3_HEADER( RB_FINGER_ARM_DISPLAY )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_SYNCH_CHECK )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_ROTATION_STYLE )

MAKE_DEFAULT_DOUBLE_PARAMETER2_HEADER( RB_HOME_POSITION )

MAKE_DEFAULT_DOUBLE_PARAMETER5_HEADER( RB_POSITION )

MAKE_DEFAULT_DOUBLE_PARAMETER4_HEADER( RB_RNG )

//

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_ROBOT_FM_ANIMATION )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_FM_FINGER_ARM_SEPERATE )

MAKE_DEFAULT_INT_PARAMETER3_HEADER( RB_FM_FINGER_ARM_STYLE )
MAKE_DEFAULT_INT_PARAMETER3_HEADER( RB_FM_FINGER_ARM_DISPLAY )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_FM_SYNCH_CHECK )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_FM_ROTATION_STYLE )

MAKE_DEFAULT_DOUBLE_PARAMETER2_HEADER( RB_FM_HOME_POSITION )
//
MAKE_DEFAULT_DOUBLE_PARAMETER5_HEADER( RB_FM_POSITION )

MAKE_DEFAULT_DOUBLE_PARAMETER3_HEADER( RB_FM_RNG )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( RB_FM_ARM_IS_DUAL_EXTEND_STYLE ) // 2003.01.15

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// FA Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_STARTEND_STATUS_SHOW )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SINGLE_CASS_MULTI_RUN )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_LOADUNLOAD_SKIP )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_MAPPING_SKIP )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_NORMALUNLOAD_NOTUSE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_LOAD_REQUEST )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_LOAD_COMPLETE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_LOAD_REJECT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_UNLOAD_REJECT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_MAPPING_REQUEST )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SYSTEM_MSGSKIP_MAPPING_REJECT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_WINDOW_NORMAL_CHECK_SKIP )
MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_WINDOW_ABORT_CHECK_SKIP )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_PROCESS_STEPCHANGE_CHECK_SKIP )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_EXPAND_MESSAGE_USE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SEND_MESSAGE_DISPLAY )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_EXTEND_HANDOFF )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_MID_READ_POINT )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_JID_READ_POINT )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_LOAD_COMPLETE_MESSAGE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_UNLOAD_COMPLETE_MESSAGE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( FA_MAPPING_AFTERLOAD )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_SUBSTRATE_WAFER_ID )

MAKE_DEFAULT_INT_PARAMETER_HEADER( FA_REJECTMESSAGE_DISPLAY ) // 2004.06.16
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Fixed Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_METHOD_NAME )

MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_SYSTEM_PATH )
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_LOG_PATH )

MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_TMP_PATH ) // 2017.10.30

// 2012.08.22
//MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_MAIN_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_MAIN_RECIPE_PATHF )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_MAIN_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_MAIN_RECIPE_PATHM )

MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_LOT_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_CLUSTER_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_GROUP_RECIPE_PATH )

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_PROCESS_RECIPE_PATHF ) // 2012.08.22
MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_PROCESS_RECIPE_PATH )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( DFIX_PROCESS_RECIPE_TYPE )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( DFIX_PROCESS_RECIPE_FILE )

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( DFIX_PROCESS_LOG_FUNCTION_NAME )
MAKE_DEFAULT_CHAR_PARAMETER3_HEADER( DFIX_PROCESS_FUNCTION_NAME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_IOINTLKS_USE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CHAMBER_INTLKS_IOUSE ) // 2013.11.21

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_RECIPE_LOCKING_MODE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_RECIPE_LOCKING )
MAKE_DEFAULT_INT_PARAMETER2_HEADER( DFIX_RECIPE_LOCKING_SKIP ) // 2014.02.08

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_MODULE_SW_CONTROL )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CASSETTE_READ_MODE ) // 2006.02.16

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CASSETTE_ABSENT_RUN_DATA ) // 2010.03.08

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_MAX_CASSETTE_SLOT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_MAX_FINGER_TM )

MAKE_DEFAULT_INT_PARAMETER2_HEADER( DFIX_TM_M_DOUBLE_CONTROL ) // 2013.03.09

#define _i_SCH_PRM_SET_DFIX_TM_DOUBLE_CONTROL( data )	_i_SCH_PRM_SET_DFIX_TM_M_DOUBLE_CONTROL( 0 , data ) // 2013.03.09
#define _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL()			_i_SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( 0 )		// 2013.03.09

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_FM_DOUBLE_CONTROL )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_FIC_MULTI_CONTROL )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_FAL_MULTI_CONTROL )

MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_FAL_MULTI_FNAME )
MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_FIC_MULTI_FNAME )

MAKE_DEFAULT_CHAR_PARAMETER_HEADER( DFIX_CARR_AUTO_HANDLER )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CONTROL_RECIPE_TYPE )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CONTROL_FIXED_CASSETTE_IN )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CONTROL_FIXED_CASSETTE_OUT )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CONTROL_FIXED_SLOT_START )
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_CONTROL_FIXED_SLOT_END )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIX_LOT_LOG_MODE )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Maked Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIM_MAX_MODULE_GROUP )

MAKE_DEFAULT_INT_PARAMETER_HEADER( DFIM_MAX_PM_COUNT )

MAKE_DEFAULT_INT_PARAMETER3_HEADER( DFIM_SLOT_NOTUSE )			// 2016.11.14

MAKE_DEFAULT_INT_PARAMETER3_HEADER( DFIM_SLOT_NOTUSE_DATA )		// 2018.12.06

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// System Utility Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CHECK_WAFER_SYNCH )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_MAPPING_WHEN_TMFREE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_TM_GATE_SKIP_CONTROL )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_TM_MOVE_SR_CONTROL )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FIRST_MODULE_PUT_DELAY_TIME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_SCH_START_OPTION )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FM_MOVE_SR_CONTROL )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_LOOP_MAP_ALWAYS )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_SCH_LOG_DIRECTORY_FORMAT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_ABORT_MESSAGE_USE_POINT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CANCEL_MESSAGE_USE_POINT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_END_MESSAGE_SEND_POINT )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CASSETTE_SUPPLY_DEVIATION_TIME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CHAMBER_PUT_PR_CHECK )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CHAMBER_GET_PR_CHECK )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FIC_MULTI_WAITTIME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FIC_MULTI_WAITTIME2 )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FAL_MULTI_WAITTIME )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_FIC_MULTI_TIMEMODE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_MESSAGE_USE_WHEN_SWITCH )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CM_SUPPLY_MODE )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_PREPOST_PROCESS_DEPAND )

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_START_PARALLEL_CHECK_SKIP ) // 2003.01.11

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_WAFER_SUPPLY_MODE ) // 2003.06.11

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_CLUSTER_INCLUDE_FOR_DISABLE ) // 2003.10.09

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_BMEND_SKIP_WHEN_RUNNING ) // 2004.02.19

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_PMREADY_SKIP_WHEN_RUNNING ) // 2006.03.28

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_LOT_LOG_PRE_POST_PROCESS ) // 2004.05.11

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_MESSAGE_USE_WHEN_ORDER ) // 2004.06.24

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_SW_BM_SCHEDULING_FACTOR ) // 2004.08.14

MAKE_DEFAULT_INT_PARAMETER_HEADER( UTIL_ADAPTIVE_PROGRESSING ) // 2008.02.29

MAKE_DEFAULT_INT_PARAMETER_HEADER( SYSTEM_LOG_STYLE ) // 2006.02.07

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Multiple Check Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
int Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( int Chamber , BOOL pmmode );


int Get_RunPrm_MODULE_START_ENABLE( int Chamber , int pmmode );

int Get_RunPrm_MODULE_GET_SINGLE_CHAMBER( int pmmode );

BOOL Get_RunPrm_MODULE_HAS_WAFER();

void Set_RunPrm_Config_Change( int mode , int chamber , int data );

void Set_RunPrm_IO_Setting( int mode );

void Set_RunPrm_Config_Change_for_ChIntlks( int mode , int tms , int chamber , int data ); // 2013.11.21

void Set_RunPrm_Config_Change_for_ChIntlks_IO(); // 2013.11.21

BOOL Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( int Source , int Target );

BOOL Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( int Source , int Target );

int  Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( int CurrSide , int ch , BOOL batch , BOOL seq ); // 2007.05.04

BOOL Get_RunPrm_RUNNING_CLUSTER(); // 2003.06.12
BOOL Get_RunPrm_RUNNING_TRANSFER(); // 2003.06.12
BOOL Get_RunPrm_RUNNING_TRANSFER_EXCEPT_AUTOMAINT(); // 2012.07.28

#endif
