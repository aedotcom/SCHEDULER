#ifndef SYSTEM_TAG_H
#define SYSTEM_TAG_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SIGNAL_MODE_DATA( int apmode , int side );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( ITEM ) void _i_SCH_SYSTEM_SET_##ITEM##( int data , char *data2 ); \
	char *_i_SCH_SYSTEM_GET_##ITEM##( int data );

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
//
2015.04.09
//
#define MAKE_SYSTEM_CHAR_PARAMETER2( ITEM , ITEM2 ) char *sp_##ITEM##_sp[ITEM2]; \
	char Init_##ITEM##_sp = 0; \
	void _i_SCH_SYSTEM_SET_##ITEM##( int data , char *data2 )	{ \
		int i , l = strlen( data2 ); \
		if ( Init_##ITEM##_sp == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) sp_##ITEM##_sp[i] = NULL; \
			Init_##ITEM##_sp = 1; \
		} \
		if ( sp_##ITEM##_sp[data] != NULL ) { \
			free( sp_##ITEM##_sp[data] ); \
			sp_##ITEM##_sp[data] = NULL; \
		} \
		if ( l > 0 ) { \
			sp_##ITEM##_sp[data] = calloc( l + 1 , sizeof( char ) ); \
			if ( sp_##ITEM##_sp[data] != NULL ) strcpy( sp_##ITEM##_sp[data] , data2 ); \
		} \
	} \
	char *_i_SCH_SYSTEM_GET_##ITEM##( int data ) { \
		if ( Init_##ITEM##_sp == 0 ) return COMMON_BLANK_CHAR; \
		if ( sp_##ITEM##_sp[data] == NULL ) return COMMON_BLANK_CHAR; \
		return sp_##ITEM##_sp[data]; \
	}
*/

//
//2015.04.09
//
#define MAKE_SYSTEM_CHAR_PARAMETER2( ITEM , ITEM2 ) char *sp_##ITEM##_sp[ITEM2]; \
	char Init_##ITEM##_sp = 0; \
	void _i_SCH_SYSTEM_SET_##ITEM##( int data , char *data2 )	{ \
		int i; \
		unsigned int l = strlen( data2 ); \
		if ( Init_##ITEM##_sp == 0 ) { \
			for ( i = 0 ; i < ITEM2 ; i++ ) sp_##ITEM##_sp[i] = NULL; \
			Init_##ITEM##_sp = 1; \
		} \
		if ( sp_##ITEM##_sp[data] != NULL ) { \
			if ( l <= (_msize( sp_##ITEM##_sp[data] )-1) ) { \
				strcpy( sp_##ITEM##_sp[data] , data2 ); \
				return; \
			} \
			free( sp_##ITEM##_sp[data] ); \
			sp_##ITEM##_sp[data] = NULL; \
		} \
		if ( l > 0 ) { \
			sp_##ITEM##_sp[data] = calloc( l + 1 , sizeof( char ) ); \
			if ( sp_##ITEM##_sp[data] != NULL ) strcpy( sp_##ITEM##_sp[data] , data2 ); \
			else 								_IO_CIM_PRINTF( "_i_SCH_SYSTEM_SET_?? Memory Allocation Fail\n" ); \
		} \
	} \
	char *_i_SCH_SYSTEM_GET_##ITEM##( int data ) { \
		if ( Init_##ITEM##_sp == 0 ) return COMMON_BLANK_CHAR; \
		if ( sp_##ITEM##_sp[data] == NULL ) return COMMON_BLANK_CHAR; \
		return sp_##ITEM##_sp[data]; \
	}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SYSTEM_RUN_TIME_GET(); // 2007.10.23

void SYSTEM_RUN_ALG_STYLE_SET( int id , int id2 );
int  SYSTEM_RUN_ALG_STYLE_GET();

int  SYSTEM_GUI_ALG_STYLE_GET();

void SYSTEM_SUB_ALG_STYLE_SET( int id ); // 2005.05.18
int  SYSTEM_SUB_ALG_STYLE_GET(); // 2005.05.18

void SYSTEM_BEGIN_SET( int Side , int );
int  SYSTEM_BEGIN_GET( int Side );

int  SYSTEM_PREPARE_GET( int Side );
void SYSTEM_PREPARE_SET( int Side , int data );

void SYSTEM_DUMMY_SET( int Side , int );
int  SYSTEM_DUMMY_GET( int Side );

void SYSTEM_MODE_BUFFER_SET( int Side , int );
int  SYSTEM_MODE_BUFFER_GET( int Side );

void SYSTEM_MODE_FIRST_SET( int Side , int ); // 2005.03.10
int  SYSTEM_MODE_FIRST_GET( int Side ); // 2005.03.10

void SYSTEM_MODE_FIRST_SET_BUFFER( int Side , int ); // 2005.06.15
int  SYSTEM_MODE_FIRST_GET_BUFFER( int Side ); // 2005.06.15

int  SYSTEM_RID_GET( int Side );
void SYSTEM_RID_SET( int Side , int data );

int  SYSTEM_CANCEL_DISABLE_GET( int Side );
void SYSTEM_CANCEL_DISABLE_SET( int Side , int data );

void SYSTEM_ENDCHECK_SET( int Side , int );
int  SYSTEM_ENDCHECK_GET( int Side );

void SYSTEM_THDCHECK_SET( int Side , int ); // 2003.05.23
int  SYSTEM_THDCHECK_GET( int Side ); // 2003.05.23

int  SYSTEM_IN_MODULE_GET( int Side ); // 2010.01.19
void SYSTEM_IN_MODULE_SET( int Side , int data ); // 2010.01.19

int  SYSTEM_OUT_MODULE_GET( int Side ); // 2010.01.19
void SYSTEM_OUT_MODULE_SET( int Side , int data ); // 2010.01.19

DWORD SYSTEM_RUN_TIMER_GET( int Side ); // 2010.12.02
void  SYSTEM_RUN_TIMER_SET( int Side ); // 2010.12.02
BOOL SYSTEM_PRGS_TIME_GET( int Side , long timeclock , char *data ); // 2014.02.04

BOOL _SCH_SYSTEM_SIDE_CLOSING_GET( int Side ); // 2013.10.02
void _SCH_SYSTEM_SIDE_CLOSING_SET( int Side , BOOL data ); // 2013.10.02

int  _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_GET( int Side ); // 2014.06.26
void _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_SET( int Side , int data ); // 2014.06.26

BOOL _SCH_SYSTEM_NO_PICK_FROM_CM_GET( int Side ); // 2016.12.05
void _SCH_SYSTEM_NO_PICK_FROM_CM_SET( int Side , BOOL data ); // 2016.12.05

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  SIGNAL_MODE_FMTMSIDE_GET( BOOL , int ch );
char *SIGNAL_MODE_FMTMRECIPE_GET( int ch );
int  SIGNAL_MODE_FMTMPRESKIPSIDE_GET( int ch ); // 2014.12.18
int  SIGNAL_MODE_FMTMEXTMODE_GET( int ch ); // 2018.06.22
void SIGNAL_MODE_FMTMSIDE_INIT( BOOL );
//void SIGNAL_MODE_FMTMSIDE_SET( int ch , int mode , char *rcp , int PMSKIPPRESIDE ); // 2018.06.22
void SIGNAL_MODE_FMTMSIDE_SET( int ch , int mode , char *rcp , int PMSKIPPRESIDE , int extmode ); // 2018.06.22
//
int  _i_SCH_SYSTEM_HAS_USER_PROCESS_GET( int ch ); // 2017.07.17

int  SIGNAL_MODE_DISAPPEAR_GET( int );
void SIGNAL_MODE_DISAPPEAR_INIT( int );
void SIGNAL_MODE_DISAPPEAR_ENTER();
void SIGNAL_MODE_DISAPPEAR_SET();
void SIGNAL_MODE_DISAPPEAR_CLEAR( int );
//
void _i_SCH_SYSTEM_FLAG_RESET( int );

int  _i_SCH_SYSTEM_PAUSE_GET( int CHECKING_SIDE );
void _i_SCH_SYSTEM_PAUSE_SET( int CHECKING_SIDE , int data );

int  _i_SCH_SYSTEM_PAUSE_SW_GET();
void _i_SCH_SYSTEM_PAUSE_SW_SET( int data );

void SIGNAL_MODE_ABORT_RESET( int );
//BOOL SIGNAL_MODE_ABORT_GET( int ); // 2016.01.12
int  SIGNAL_MODE_ABORT_GET( int ); // 2016.01.12

BOOL SIGNAL_MODE_WAITR_CHECK( int CHECKING_SIDE ); // 2005.09.15

BOOL SIGNAL_MODE_RECIPE_READ_SET( int side , int filetype , int callstyle , char *filename );
BOOL SIGNAL_MODE_RECIPE_READ_GET( int side , int *filetype , int *callstyle , char *filename );
BOOL SIGNAL_MODE_RECIPE_READ_CLEAR( int );
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void SYSTEM_TAG_DATA_INIT();


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( LOTLOGDIRECTORY )
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( LOTLOGDIRECTORY_LOOP )
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( JOB_ID )
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( MID_ID )
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( MODULE_NAME )
MAKE_SYSTEM_CHAR_PARAMETER2_HEADER( MODULE_NAME_for_SYSTEM )
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  _i_SCH_SYSTEM_BLANK_GET( int Side ); // 2011.04.25
void _i_SCH_SYSTEM_BLANK_SET( int Side , int data ); // 2011.04.25

int  _i_SCH_SYSTEM_RESTART_GET( int Side ); // 2011.09.23
void _i_SCH_SYSTEM_RESTART_SET( int Side , int data ); // 2011.09.23

int  _i_SCH_SYSTEM_DBUPDATE_GET( int Side ); // 2011.04.25
void _i_SCH_SYSTEM_DBUPDATE_SET( int Side , int data ); // 2011.04.25

int  _i_SCH_SYSTEM_FINISH_GET( int Side ); // 2011.04.27
void _i_SCH_SYSTEM_FINISH_SET( int Side , int data ); // 2011.04.27

void _i_SCH_SYSTEM_USING_SET( int Side , int );
int  _i_SCH_SYSTEM_USING_GET( int Side );
//
void _i_SCH_SYSTEM_RUN_TAG_SET( int Side , int ); // 2011.04.22
int  _i_SCH_SYSTEM_RUN_TAG_GET( int Side ); // 2011.04.22
//
void _i_SCH_SYSTEM_TM_SET( int TMATM , int Side , int );
int  _i_SCH_SYSTEM_TM_GET( int TMATM , int Side );
//
void _i_SCH_SYSTEM_LASTING_SET( int Side , int );
int  _i_SCH_SYSTEM_LASTING_GET( int Side );
//
void _i_SCH_SYSTEM_USING2_SET( int Side , int );
int  _i_SCH_SYSTEM_USING2_GET( int Side );
//
void _i_SCH_SYSTEM_FA_SET( int Side , BOOL );
BOOL _i_SCH_SYSTEM_FA_GET( int Side );
//
int  _i_SCH_SYSTEM_CPJOB_GET( int Side );
void _i_SCH_SYSTEM_CPJOB_SET( int Side , int data );
//
int  _i_SCH_SYSTEM_CPJOB_ID_GET( int Side );
void _i_SCH_SYSTEM_CPJOB_ID_SET( int Side , int data );
//
void _i_SCH_SYSTEM_MODE_SET( int Side , int );
int  _i_SCH_SYSTEM_MODE_GET( int Side );
//
int  _i_SCH_SYSTEM_RUNORDER_GET( int Side );
void _i_SCH_SYSTEM_RUNORDER_SET( int Side , int data );
//
void _i_SCH_SYSTEM_RUNCHECK_SET( int );
int  _i_SCH_SYSTEM_RUNCHECK_GET();
//
void _i_SCH_SYSTEM_RUNCHECK_FM_SET( int );
int  _i_SCH_SYSTEM_RUNCHECK_FM_GET();

int  _i_SCH_SYSTEM_PMMODE_GET( int Side ); // 2005.07.06
void _i_SCH_SYSTEM_PMMODE_SET( int Side , int data ); // 2005.07.06

int  _i_SCH_SYSTEM_MOVEMODE_GET( int Side ); // 2013.09.03
void _i_SCH_SYSTEM_MOVEMODE_SET( int Side , int data ); // 2013.09.03

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SYSTEM_USING_RUNNING( int Side ); // 2005.07.29
BOOL _i_SCH_SYSTEM_USING_ANOTHER_RUNNING( int Side ); // 2003.11.10
//
BOOL _i_SCH_SYSTEM_USING_STARTING_ONLY( int Side ); // 2006.12.19
BOOL _i_SCH_SYSTEM_USING_STARTING( int Side ); // 2006.12.19
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SYSTEM_PAUSE_ABORT_CHECK( int );
BOOL _i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( int );
BOOL _i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( int CHECKING_SIDE , int pt ); // 2012.07.12 // 2016.11.04
int  _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( int CHECKING_SIDE ); // 2007.08.09

int  _i_SCH_SYSTEM_MODE_END_GET( int );
void _i_SCH_SYSTEM_MODE_END_SET( int , int );
//
BOOL _i_SCH_SYSTEM_MODE_RESUME_GET( int CHECKING_SIDE );
void _i_SCH_SYSTEM_MODE_RESUME_SET( int CHECKING_SIDE , BOOL data );
//
BOOL _i_SCH_SYSTEM_MODE_WAITR_GET( int CHECKING_SIDE );
void _i_SCH_SYSTEM_MODE_WAITR_SET( int CHECKING_SIDE , BOOL data );
int  _i_SCH_SYSTEM_MODE_LOOP_GET( int CHECKING_SIDE );
void _i_SCH_SYSTEM_MODE_LOOP_SET( int CHECKING_SIDE , int data );
void _i_SCH_SYSTEM_MODE_ABORT_SET( int );

char *_i_SCH_SYSTEM_GET_MID_ID( int data );
char *_i_SCH_SYSTEM_GET_JOB_ID( int data );
char *_i_SCH_SYSTEM_GET_MODULE_NAME( int data );
char *_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( int data );

int  _i_SCH_SYSTEM_APPEND_END_GET( int CHECKING_SIDE ); // 2010.09.13
void _i_SCH_SYSTEM_APPEND_END_SET( int CHECKING_SIDE , int data ); // 2010.09.13

int  _i_SCH_SYSTEM_EQUIP_RUNNING_GET( int ch ); // 2012.10.31
void _i_SCH_SYSTEM_EQUIP_RUNNING_SET( int ch , int data ); // 2012.10.31

void _i_SCH_SYSTEM_INFO_LOG( char *data , char *disp ); // 2013.02.20

int  _i_SCH_SYSTEM_ESOURCE_ID_GET( int CHECKING_SIDE ); // 2019.02.26
void _i_SCH_SYSTEM_ESOURCE_ID_SET( int CHECKING_SIDE , int data ); // 2019.02.26

#endif

