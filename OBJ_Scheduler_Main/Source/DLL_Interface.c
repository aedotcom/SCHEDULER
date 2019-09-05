#include "default.h"

#include "EQ_Enum.h"

#include "lottype.h"
#include "DLL_Interface.h"
#include "sch_cassmap.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_Cluster.h"
#include "User_Parameter.h" // 2015.07.30
#include "System_Tag.h" // 2015.10.12
#include "IO_Part_data.h" // 2015.10.12
#include "sch_sdm.h" // 2015.10.12

#include "INF_sch_CommonUser.h"

int USER_dREAD_FUNCTION( int i );

typedef int  (*DLL_USER_RECIPE_DATA_MODE) ();
typedef void (*DLL_USER_RECIPE_DATA_INIT) ();
typedef BOOL (*DLL_USER_RECIPE_DEFAULT_DATA_READ)    ( int , char * , int * , int * , char * , char * , char * , int * , int * , int * );
typedef BOOL (*DLL_USER_RECIPE_DEFAULT_DATA_REGIST)  ( char * , int * , int * , char * , char * , char * , int * , int * , int * , char * );
typedef int  (*DLL_USER_RECIPE_PM_MODE)              ( int , int );
typedef int  (*DLL_USER_RECIPE_PM_MODE_EX)           ( int , int , char * , char * , char * );
typedef BOOL (*DLL_USER_RECIPE_LOT_DATA_READ)        ( int , int , LOTStepTemplate * , char * );
typedef BOOL (*DLL_USER_RECIPE_LOT_DUMMY_DATA_READ)  ( int , int , LOTStepTemplate2 * , char * );
typedef BOOL (*DLL_USER_RECIPE_CLUSTER_DATA_READ)    ( int , int , CLUSTERStepTemplate * , int , char * );
typedef BOOL (*DLL_USER_RECIPE_CLUSTER_DATA_READ2)   ( int , int , CLUSTERStepTemplate2 * , int , char * ); // 2012.06.18
typedef BOOL (*DLL_USER_RECIPE_INOUT_DATA_READ)      ( int , int , CASSETTETemplate * );
typedef BOOL (*DLL_USER_RECIPE_MAPPING_VERIFICATION) ( int , int , int , CASSETTEVerTemplate );

typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION)	 ( int , int , int , int , CLUSTERStepTemplate ); // 2004.12.14
typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION_EX) ( int , int , int , int , int , int , CLUSTERStepTemplate , int , char * , char * ); // 2005.01.24
typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION_EX2)( int , int , int , int , int , int , CLUSTERStepTemplate2 , int , char * , char * , char * ); // 2005.01.24
typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION_EX3)( int , int , int , int , int , int , CLUSTERStepTemplate2 * , int , char * , char * , char * ); // 2008.07.24
typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION_EX4)( int , int , int , int , int , int , Scheduling_Path * ); // 2009.04.01

typedef BOOL (*DLL_USER_RECIPE_INFO_VERIFICATION_EIL)( int , int , int , int , int , int , char * , char * ); // 2010.09.16

typedef BOOL (*DLL_USER_RECIPE_INFO_FLOWING)		 ( int , int , int , int , int , char * ); // 2010.12.02

typedef BOOL (*DLL_USER_RECIPE_START_MODE_CHANGE)    ( int , int , int , int , int * ); // 2005.08.02
typedef BOOL (*DLL_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE)    ( BOOL , int , int , int , char * , char * , char * , int * , int * , int * , int * ); // 2005.08.02

typedef BOOL (*DLL_USER_RECIPE_ACTION_AFTER_READY)   ( int ); // 2005.10.26

typedef BOOL (*DLL_USER_RECIPE_TUNE_DATA_APPEND)	 ( int , int , int , int , char * , char * ); // 2007.06.11

typedef int  (*DLL_USER_RECIPE_DUMMY_OPERATE)		 ( int , int , int , int , int , int , int , int , int * , int * , char * );

typedef int  (*DLL_USER_RECIPE_DUMMY_INFO_VERIFICATION)( int , int , int , int , int , int , char * , Scheduling_Path * ); // 2009.05.21

typedef int  (*DLL_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY)( int , int , int , int , int , int , int , int , int , char * , Scheduling_Path * ); // 2009.05.21

typedef int  (*DLL_USER_RECIPE_DATA_PRE_APPEND)( int , int ); // 2010.05.08
typedef int  (*DLL_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO) ( int moduleindex , char *ALARM_MODULE , int ALARM_MODULE_INDEX , int ALARM_ID , char *ALARM_NAME , char *ALARM_MESSAGE , int ALARM_LEVEL , int ALARM_STYLE , int ALARM_HAPPEN , char *ALARM_HAPPEN_INFO , int *findex ); // 2012.03.08

typedef BOOL (*DLL_USER_RECIPE_LOOPCOUNT_CHANGE)    ( int , int , int , int * ); // 2014.05.13

typedef BOOL (*DLL_USER_RECIPE_LOT_SLOTFIX_SETTING) ( int , int , int , int , int * , int * , int * , int * ); // 2015.10.12
//-----------------------------------------------------------------------------------
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
typedef LPARAM (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
typedef LPARAM (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
typedef LPARAM (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
typedef LPARAM (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
typedef LPARAM (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 );

typedef void   (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API)( DLL_SCH2_INF_DLL_CONTROL_EXTERN_INTERFACE_A );
typedef void   (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API)( DLL_SCH2_INF_DLL_CONTROL_EXTERN_INTERFACE_B );
typedef void   (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API)( DLL_SCH2_INF_DLL_CONTROL_EXTERN_INTERFACE_C );
typedef void   (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API)( DLL_SCH2_INF_DLL_CONTROL_EXTERN_INTERFACE_D );
typedef void   (*DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API)( DLL_SCH2_INF_DLL_CONTROL_EXTERN_INTERFACE_E );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int USER_RECIPE_START_TAG[USER_RECIPE_MAX_TAG][MAX_CASSETTE_SIDE];
int USER_RECIPE_START_END[USER_RECIPE_MAX_TAG][MAX_CASSETTE_SIDE];
//
void USER_RECIPE_TAG_START( int type , int side , int id ) {
	if ( ( type < 0 ) || ( type >= USER_RECIPE_MAX_TAG ) ) return;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return;
	USER_RECIPE_START_TAG[type][side] = id;
}

void USER_RECIPE_TAG_END( int type , int side , int id ) {
	if ( ( type < 0 ) || ( type >= USER_RECIPE_MAX_TAG ) ) return;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return;
	USER_RECIPE_START_END[type][side] = id;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int DLL_USER_RECIPE_USE = 0;

HINSTANCE DRV_DLL_USER_LOAD_POINT = NULL;

char *DRV_DLL_USER_FILENAME = NULL; // 2008.04.02

DLL_USER_RECIPE_DATA_INIT				DRV_USER_RECIPE_DATA_INIT;
DLL_USER_RECIPE_DATA_MODE				DRV_USER_RECIPE_DATA_MODE;
DLL_USER_RECIPE_DEFAULT_DATA_READ		DRV_USER_RECIPE_DEFAULT_DATA_READ;
DLL_USER_RECIPE_DEFAULT_DATA_REGIST		DRV_USER_RECIPE_DEFAULT_DATA_REGIST;
DLL_USER_RECIPE_PM_MODE					DRV_USER_RECIPE_PM_MODE;
DLL_USER_RECIPE_PM_MODE_EX				DRV_USER_RECIPE_PM_MODE_EX; // 2005.07.18
DLL_USER_RECIPE_LOT_DATA_READ			DRV_USER_RECIPE_LOT_DATA_READ;
DLL_USER_RECIPE_LOT_DUMMY_DATA_READ		DRV_USER_RECIPE_LOT_DUMMY_DATA_READ; // 2003.09.24
DLL_USER_RECIPE_CLUSTER_DATA_READ		DRV_USER_RECIPE_CLUSTER_DATA_READ;
DLL_USER_RECIPE_CLUSTER_DATA_READ2		DRV_USER_RECIPE_CLUSTER_DATA_READ2; // 2012.06.18
DLL_USER_RECIPE_INOUT_DATA_READ  		DRV_USER_RECIPE_INOUT_DATA_READ;
DLL_USER_RECIPE_MAPPING_VERIFICATION  	DRV_USER_RECIPE_MAPPING_VERIFICATION;
//
DLL_USER_RECIPE_DATA_MODE				DRV_USER_RECIPE_MANUAL_DATA_MODE;
DLL_USER_RECIPE_DEFAULT_DATA_READ		DRV_USER_RECIPE_MANUAL_DEFAULT_DATA_READ;
DLL_USER_RECIPE_PM_MODE					DRV_USER_RECIPE_MANUAL_PM_MODE;
DLL_USER_RECIPE_PM_MODE_EX				DRV_USER_RECIPE_MANUAL_PM_MODE_EX; // 2005.07.18
DLL_USER_RECIPE_LOT_DATA_READ			DRV_USER_RECIPE_MANUAL_LOT_DATA_READ;
DLL_USER_RECIPE_LOT_DUMMY_DATA_READ		DRV_USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ; // 2003.09.24
DLL_USER_RECIPE_CLUSTER_DATA_READ		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ;
DLL_USER_RECIPE_CLUSTER_DATA_READ2		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ2; // 2012.06.18
DLL_USER_RECIPE_INOUT_DATA_READ			DRV_USER_RECIPE_MANUAL_INOUT_DATA_READ;
DLL_USER_RECIPE_MAPPING_VERIFICATION  	DRV_USER_RECIPE_MANUAL_MAPPING_VERIFICATION;

DLL_USER_RECIPE_INFO_VERIFICATION		DRV_USER_RECIPE_INFO_VERIFICATION; // 2004.12.14
DLL_USER_RECIPE_INFO_VERIFICATION_EX	DRV_USER_RECIPE_INFO_VERIFICATION_EX; // 2005.01.24
DLL_USER_RECIPE_INFO_VERIFICATION_EX2	DRV_USER_RECIPE_INFO_VERIFICATION_EX2; // 2007.04.25
DLL_USER_RECIPE_INFO_VERIFICATION_EX3	DRV_USER_RECIPE_INFO_VERIFICATION_EX3; // 2008.07.24
DLL_USER_RECIPE_INFO_VERIFICATION_EX4	DRV_USER_RECIPE_INFO_VERIFICATION_EX4; // 2009.04.01

DLL_USER_RECIPE_INFO_VERIFICATION_EIL	DRV_USER_RECIPE_INFO_VERIFICATION_EIL; // 2010.09.16

DLL_USER_RECIPE_INFO_FLOWING			DRV_USER_RECIPE_INFO_FLOWING; // 2010.12.02

DLL_USER_RECIPE_START_MODE_CHANGE		DRV_USER_RECIPE_START_MODE_CHANGE; // 2005.08.02
DLL_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE		DRV_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE; // 2007.03.27

DLL_USER_RECIPE_ACTION_AFTER_READY		DRV_USER_RECIPE_ACTION_AFTER_READY; // 2005.10.26

DLL_USER_RECIPE_TUNE_DATA_APPEND		DRV_USER_RECIPE_TUNE_DATA_APPEND; // 2007.06.11

DLL_USER_RECIPE_DUMMY_OPERATE			DRV_USER_RECIPE_DUMMY_OPERATE;

DLL_USER_RECIPE_DUMMY_INFO_VERIFICATION	DRV_USER_RECIPE_DUMMY_INFO_VERIFICATION;

DLL_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY	DRV_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY;

DLL_USER_RECIPE_DATA_PRE_APPEND			DRV_USER_RECIPE_DATA_PRE_APPEND; // 2010.05.08

DLL_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO			DRV_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO; // 2012.03.08

DLL_USER_RECIPE_LOOPCOUNT_CHANGE		DRV_USER_RECIPE_LOOPCOUNT_CHANGE; // 2014.05.13

DLL_USER_RECIPE_LOT_SLOTFIX_SETTING		DRV_USER_RECIPE_LOT_SLOTFIX_SETTING; // 2015.10.12
//--------------------------------------------------------------------------------------------------------
// 2004.03.15
//--------------------------------------------------------------------------------------------------------
DLL_KNFS_SET_COMMON_PRINTF_API			DRV_KNFS_SET_COMMON_PRINTF_API;
DLL_KNFS_SET_FIND_FROM_STRING_API		DRV_KNFS_SET_FIND_FROM_STRING_API;
DLL_KNFS_SET_dWRITE_FUNCTION_API		DRV_KNFS_SET_dWRITE_FUNCTION_API;
DLL_KNFS_SET_dREAD_FUNCTION_API			DRV_KNFS_SET_dREAD_FUNCTION_API;
DLL_KNFS_SET_dRUN_FUNCTION_ABORT_API	DRV_KNFS_SET_dRUN_FUNCTION_ABORT_API;
DLL_KNFS_SET_dWRITE_FUNCTION_EVENT_API	DRV_KNFS_SET_dWRITE_FUNCTION_EVENT_API;
DLL_KNFS_SET_dREAD_FUNCTION_EVENT_API	DRV_KNFS_SET_dREAD_FUNCTION_EVENT_API;
//--------------------------------------------------------------------------------------------------------
// 2005.04.19
//--------------------------------------------------------------------------------------------------------
DLL_KNFS_SET_dREAD_UPLOAD_MESSAGE_API	DRV_KNFS_SET_dREAD_UPLOAD_MESSAGE_API;
DLL_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API	DRV_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API;

DLL_KNFS_SET_dWRITE_DIGITAL_API			DRV_KNFS_SET_dWRITE_DIGITAL_API;
DLL_KNFS_SET_dREAD_DIGITAL_API			DRV_KNFS_SET_dREAD_DIGITAL_API;
DLL_KNFS_SET_dWRITE_ANALOG_API			DRV_KNFS_SET_dWRITE_ANALOG_API;
DLL_KNFS_SET_dREAD_ANALOGL_API			DRV_KNFS_SET_dREAD_ANALOG_API;
DLL_KNFS_SET_dWRITE_STRING_API			DRV_KNFS_SET_dWRITE_STRING_API;
DLL_KNFS_SET_dREAD_STRING_API			DRV_KNFS_SET_dREAD_STRING_API;

DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API	DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API;
DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API	DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API;
DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API	DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API;
DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API	DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API;
DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API	DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API;

//=========================================================================================
enum {	TYPE_ITEM_USE ,
		TYPE_ITEM_NOTUSE };

enum {	TYPE_LOT_FILE_CLUSTER_FILE ,
		TYPE_LOT_FILE_CLUSTER_DLL ,
		TYPE_LOT_FILE_CLUSTER_ALL ,
		TYPE_LOT_DLL_CLUSTER_FILE ,
		TYPE_LOT_DLL_CLUSTER_DLL ,
		TYPE_LOT_DLL_CLUSTER_ALL ,
		TYPE_LOT_ALL_CLUSTER_FILE ,
		TYPE_LOT_ALL_CLUSTER_DLL ,
		TYPE_LOT_ALL_CLUSTER_ALL };
//=========================================================================================
//=========================================================================================
int USER_RECIPE_USE_TAG_GET() {
	return DLL_USER_RECIPE_USE;
	// -1	:	Not Use
	// 0	:	Not Loaded
	// 1	:	Loaded Enable
	// 2	:	Loaded Disable
}
//=========================================================================================
HINSTANCE USER_RECIPE_GET_DLL_INSTANCE() {
	return DRV_DLL_USER_LOAD_POINT;
}
//=========================================================================================
BOOL USER_RECIPE_DLL_LOAD( char *Filename , int mode ) {
	// mode 
	// 0	:	Init
	// 1	:	Disable
	// 2	:	Enable
	// 3	:	Unload
	// 4	:	Load
	// 5	:	Load Disable
	//================================================================================================
	// 2008.01.18
	//================================================================================================
	if ( ( mode == 1 ) || ( mode == 2 ) ) {
		//
		if ( !DRV_DLL_USER_LOAD_POINT ) return FALSE;
		//
		if ( DLL_USER_RECIPE_USE <= 0 ) return FALSE;
		//
		if ( mode == 1 ) DLL_USER_RECIPE_USE = 2;
		else             DLL_USER_RECIPE_USE = 1;
		//
		return TRUE;
	}
	else if ( mode == 3 ) {
		//
		if ( !DRV_DLL_USER_LOAD_POINT ) return TRUE;
		//
		_SCH_COMMON_RECIPE_DLL_INTERFACE( NULL , TRUE );
		//
		DLL_USER_RECIPE_USE = 0;
		//
		FreeLibrary( DRV_DLL_USER_LOAD_POINT );
		//
		DRV_DLL_USER_LOAD_POINT = NULL;
		//
		return TRUE;
	}
	else if ( ( mode == 4 ) || ( mode == 5 ) ) {
		//
		if ( DRV_DLL_USER_LOAD_POINT ) return FALSE;
		//
		if ( strlen( Filename ) > 0 ) STR_MEM_MAKE_COPY2( &DRV_DLL_USER_FILENAME , Filename ); // 2008.04.02 // 2010.03.25
		//
		if ( STR_MEM_SIZE( DRV_DLL_USER_FILENAME ) > 0 ) { // 2008.04.03
			DLL_USER_RECIPE_USE = 0;
		}
		else {
			DLL_USER_RECIPE_USE = -1;
		}
		//
	}
	else if ( mode == 0 ) {
		//
		STR_MEM_MAKE_COPY2( &DRV_DLL_USER_FILENAME , Filename ); // 2008.04.02 // 2010.03.25
		//
	}
	//================================================================================================
	if ( DRV_DLL_USER_FILENAME == NULL ) { // 2008.04.02
		DRV_DLL_USER_LOAD_POINT = FALSE; // 2008.04.02
	}
	else {
		DRV_DLL_USER_LOAD_POINT = LoadLibrary( DRV_DLL_USER_FILENAME ); // 2008.01.18
	}
	//================================================================================================
	if ( DRV_DLL_USER_LOAD_POINT ) {
		//
		if ( mode == 5 ) DLL_USER_RECIPE_USE = 2;
		else             DLL_USER_RECIPE_USE = 1;
		//
		DRV_USER_RECIPE_DATA_INIT					= (DLL_USER_RECIPE_DATA_INIT)				GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DATA_INIT" );
		//
		DRV_USER_RECIPE_DATA_MODE					= (DLL_USER_RECIPE_DATA_MODE)				GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DATA_MODE" );
		DRV_USER_RECIPE_DEFAULT_DATA_READ			= (DLL_USER_RECIPE_DEFAULT_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DEFAULT_DATA_READ" );
		DRV_USER_RECIPE_DEFAULT_DATA_REGIST			= (DLL_USER_RECIPE_DEFAULT_DATA_REGIST)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DEFAULT_DATA_REGIST" );
		DRV_USER_RECIPE_PM_MODE						= (DLL_USER_RECIPE_PM_MODE)					GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_PM_MODE" );
		DRV_USER_RECIPE_PM_MODE_EX					= (DLL_USER_RECIPE_PM_MODE_EX)				GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_PM_MODE_EX" );
		DRV_USER_RECIPE_LOT_DATA_READ				= (DLL_USER_RECIPE_LOT_DATA_READ)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_LOT_DATA_READ" );
		DRV_USER_RECIPE_LOT_DUMMY_DATA_READ			= (DLL_USER_RECIPE_LOT_DUMMY_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_LOT_DUMMY_DATA_READ" ); // 2003.09.24
		DRV_USER_RECIPE_CLUSTER_DATA_READ			= (DLL_USER_RECIPE_CLUSTER_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_CLUSTER_DATA_READ" );
		DRV_USER_RECIPE_CLUSTER_DATA_READ2			= (DLL_USER_RECIPE_CLUSTER_DATA_READ2)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_CLUSTER_DATA_READ2" ); // 2012.06.18
		DRV_USER_RECIPE_INOUT_DATA_READ				= (DLL_USER_RECIPE_INOUT_DATA_READ)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INOUT_DATA_READ" );
		DRV_USER_RECIPE_MAPPING_VERIFICATION		= (DLL_USER_RECIPE_MAPPING_VERIFICATION)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MAPPING_VERIFICATION" );
		//
		DRV_USER_RECIPE_MANUAL_DATA_MODE			= (DLL_USER_RECIPE_DATA_MODE)				GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_DATA_MODE" );
		DRV_USER_RECIPE_MANUAL_DEFAULT_DATA_READ	= (DLL_USER_RECIPE_DEFAULT_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_DEFAULT_DATA_READ" );
		DRV_USER_RECIPE_MANUAL_PM_MODE				= (DLL_USER_RECIPE_PM_MODE)					GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_PM_MODE" );
		DRV_USER_RECIPE_MANUAL_PM_MODE_EX			= (DLL_USER_RECIPE_PM_MODE_EX)				GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_PM_MODE_EX" );
		DRV_USER_RECIPE_MANUAL_LOT_DATA_READ		= (DLL_USER_RECIPE_LOT_DATA_READ)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_LOT_DATA_READ" );
		DRV_USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ	= (DLL_USER_RECIPE_LOT_DUMMY_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ" ); // 2003.09.24
		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ	= (DLL_USER_RECIPE_CLUSTER_DATA_READ)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_CLUSTER_DATA_READ" );
		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ2	= (DLL_USER_RECIPE_CLUSTER_DATA_READ2)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_CLUSTER_DATA_READ2" ); // 2012.06.18
		DRV_USER_RECIPE_MANUAL_INOUT_DATA_READ		= (DLL_USER_RECIPE_INOUT_DATA_READ)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_INOUT_DATA_READ" );
		DRV_USER_RECIPE_MANUAL_MAPPING_VERIFICATION	= (DLL_USER_RECIPE_MAPPING_VERIFICATION)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_MANUAL_MAPPING_VERIFICATION" );
		//
		DRV_USER_RECIPE_INFO_VERIFICATION			= (DLL_USER_RECIPE_INFO_VERIFICATION)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION" ); // 2004.12.14
		DRV_USER_RECIPE_INFO_VERIFICATION_EX		= (DLL_USER_RECIPE_INFO_VERIFICATION_EX)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION_EX" ); // 2005.01.24
		DRV_USER_RECIPE_INFO_VERIFICATION_EX2		= (DLL_USER_RECIPE_INFO_VERIFICATION_EX2)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION_EX2" ); // 2007.04.25
		DRV_USER_RECIPE_INFO_VERIFICATION_EX3		= (DLL_USER_RECIPE_INFO_VERIFICATION_EX3)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION_EX3" ); // 2008.07.24
		DRV_USER_RECIPE_INFO_VERIFICATION_EX4		= (DLL_USER_RECIPE_INFO_VERIFICATION_EX4)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION_EX4" ); // 2009.04.01
		//
		DRV_USER_RECIPE_INFO_VERIFICATION_EIL		= (DLL_USER_RECIPE_INFO_VERIFICATION_EIL)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_VERIFICATION_EIL" ); // 2010.09.15
		//
		DRV_USER_RECIPE_INFO_FLOWING				= (DLL_USER_RECIPE_INFO_FLOWING)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_INFO_FLOWING" ); // 2010.12.02
		//
		DRV_USER_RECIPE_START_MODE_CHANGE			= (DLL_USER_RECIPE_START_MODE_CHANGE)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_START_MODE_CHANGE" ); // 2005.08.02
		DRV_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE		= (DLL_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_JOB_DEFAULT_MODE_CHANGE" ); // 2007.03.27
		//
		DRV_USER_RECIPE_ACTION_AFTER_READY			= (DLL_USER_RECIPE_ACTION_AFTER_READY)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_ACTION_AFTER_READY" ); // 2005.10.26
		//
		DRV_USER_RECIPE_TUNE_DATA_APPEND			= (DLL_USER_RECIPE_TUNE_DATA_APPEND)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_TUNE_DATA_APPEND" ); // 2007.06.11
		//
		DRV_USER_RECIPE_DUMMY_OPERATE				= (DLL_USER_RECIPE_DUMMY_OPERATE)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DUMMY_OPERATE" );
		//
		DRV_USER_RECIPE_DUMMY_INFO_VERIFICATION		= (DLL_USER_RECIPE_DUMMY_INFO_VERIFICATION)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DUMMY_INFO_VERIFICATION" );
		//
		DRV_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY	= (DLL_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY)GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_PROCESS_FINISH_FOR_DUMMY" );
		//
		DRV_USER_RECIPE_DATA_PRE_APPEND				= (DLL_USER_RECIPE_DATA_PRE_APPEND)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_DATA_PRE_APPEND" ); // 2010.05.08
		//
		DRV_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO	= (DLL_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_GET_MODULE_FROM_ALARM_INFO" ); // 2012.03.08
		//
		DRV_USER_RECIPE_LOOPCOUNT_CHANGE			= (DLL_USER_RECIPE_LOOPCOUNT_CHANGE)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_LOOPCOUNT_CHANGE" ); // 2014.05.13
		//
		DRV_USER_RECIPE_LOT_SLOTFIX_SETTING			= (DLL_USER_RECIPE_LOT_SLOTFIX_SETTING)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_RECIPE_LOT_SLOTFIX_SETTING" ); // 2015.10.12
		//
		//---------------------------------------------------------------------------------------------------------
		// 2004.03.15
		//---------------------------------------------------------------------------------------------------------
		DRV_KNFS_SET_COMMON_PRINTF_API				= (DLL_KNFS_SET_COMMON_PRINTF_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_COMMON_PRINTF" );
		DRV_KNFS_SET_FIND_FROM_STRING_API			= (DLL_KNFS_SET_FIND_FROM_STRING_API)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_FIND_FROM_STRING" );
		DRV_KNFS_SET_dWRITE_FUNCTION_API			= (DLL_KNFS_SET_dWRITE_FUNCTION_API)		GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_FUNCTION" );
		DRV_KNFS_SET_dREAD_FUNCTION_API				= (DLL_KNFS_SET_dREAD_FUNCTION_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_FUNCTION" );
		DRV_KNFS_SET_dRUN_FUNCTION_ABORT_API		= (DLL_KNFS_SET_dRUN_FUNCTION_ABORT_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dRUN_FUNCTION_ABORT" );
		DRV_KNFS_SET_dWRITE_FUNCTION_EVENT_API		= (DLL_KNFS_SET_dWRITE_FUNCTION_EVENT_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_FUNCTION_EVENT" );
		DRV_KNFS_SET_dREAD_FUNCTION_EVENT_API		= (DLL_KNFS_SET_dREAD_FUNCTION_EVENT_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_FUNCTION_EVENT" );
		//
		if ( DRV_KNFS_SET_COMMON_PRINTF_API ) (* DRV_KNFS_SET_COMMON_PRINTF_API ) ( _IO_COMMON_PRINTF );
		if ( DRV_KNFS_SET_FIND_FROM_STRING_API ) (* DRV_KNFS_SET_FIND_FROM_STRING_API ) ( _FIND_FROM_STRING );
		if ( DRV_KNFS_SET_dWRITE_FUNCTION_API ) (* DRV_KNFS_SET_dWRITE_FUNCTION_API ) ( _dWRITE_FUNCTION );
//		if ( DRV_KNFS_SET_dREAD_FUNCTION_API ) (* DRV_KNFS_SET_dREAD_FUNCTION_API ) ( _dREAD_FUNCTION ); // 2013.03.18
		if ( DRV_KNFS_SET_dREAD_FUNCTION_API ) (* DRV_KNFS_SET_dREAD_FUNCTION_API ) ( USER_dREAD_FUNCTION ); // 2013.03.18
		if ( DRV_KNFS_SET_dRUN_FUNCTION_ABORT_API ) (* DRV_KNFS_SET_dRUN_FUNCTION_ABORT_API ) ( _dRUN_FUNCTION_ABORT );
		if ( DRV_KNFS_SET_dWRITE_FUNCTION_EVENT_API ) (* DRV_KNFS_SET_dWRITE_FUNCTION_EVENT_API ) ( _dWRITE_FUNCTION_EVENT );
		if ( DRV_KNFS_SET_dREAD_FUNCTION_EVENT_API ) (* DRV_KNFS_SET_dREAD_FUNCTION_EVENT_API ) ( _dREAD_FUNCTION_EVENT );

		//---------------------------------------------------------------------------------------------------------
		// 2005.04.19
		//---------------------------------------------------------------------------------------------------------

		DRV_KNFS_SET_dREAD_UPLOAD_MESSAGE_API		= (DLL_KNFS_SET_dREAD_UPLOAD_MESSAGE_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_UPLOAD_MESSAGE" );
		DRV_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API		= (DLL_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_UPLOAD_MESSAGE" );

		DRV_KNFS_SET_dWRITE_DIGITAL_API				= (DLL_KNFS_SET_dWRITE_DIGITAL_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_DIGITAL" );
		DRV_KNFS_SET_dREAD_DIGITAL_API				= (DLL_KNFS_SET_dREAD_DIGITAL_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_DIGITAL" );
		DRV_KNFS_SET_dWRITE_ANALOG_API				= (DLL_KNFS_SET_dWRITE_ANALOG_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_ANALOG" );
		DRV_KNFS_SET_dREAD_ANALOG_API				= (DLL_KNFS_SET_dREAD_ANALOGL_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_ANALOG" );
		DRV_KNFS_SET_dWRITE_STRING_API				= (DLL_KNFS_SET_dWRITE_STRING_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dWRITE_STRING" );
		DRV_KNFS_SET_dREAD_STRING_API				= (DLL_KNFS_SET_dREAD_STRING_API)			GetProcAddress( DRV_DLL_USER_LOAD_POINT , "USER_API_SET_dREAD_STRING" );
		//
		if ( DRV_KNFS_SET_dREAD_UPLOAD_MESSAGE_API ) (* DRV_KNFS_SET_dREAD_UPLOAD_MESSAGE_API ) ( _dREAD_UPLOAD_MESSAGE );
		if ( DRV_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API ) (* DRV_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API ) ( _dWRITE_UPLOAD_MESSAGE );
		//
		if ( DRV_KNFS_SET_dWRITE_DIGITAL_API ) (* DRV_KNFS_SET_dWRITE_DIGITAL_API ) ( _dWRITE_DIGITAL );
		if ( DRV_KNFS_SET_dREAD_DIGITAL_API ) (* DRV_KNFS_SET_dREAD_DIGITAL_API ) ( _dREAD_DIGITAL );
		if ( DRV_KNFS_SET_dWRITE_ANALOG_API ) (* DRV_KNFS_SET_dWRITE_ANALOG_API ) ( _dWRITE_ANALOG );
		if ( DRV_KNFS_SET_dREAD_ANALOG_API ) (* DRV_KNFS_SET_dREAD_ANALOG_API ) ( _dREAD_ANALOG );
		if ( DRV_KNFS_SET_dWRITE_STRING_API ) (* DRV_KNFS_SET_dWRITE_STRING_API ) ( _dWRITE_STRING );
		if ( DRV_KNFS_SET_dREAD_STRING_API ) (* DRV_KNFS_SET_dREAD_STRING_API ) ( _dREAD_STRING );
		//-----------------------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------
		// 2009.05.28
		//---------------------------------------------------------------------------------------------------------
		DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API		= (DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_A" );
		DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API		= (DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_B" );
		DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API		= (DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_C" );
		DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API		= (DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_D" );
		DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API		= (DLL_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API)	GetProcAddress( DRV_DLL_USER_LOAD_POINT , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_E" );

		if ( DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API ) (* DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A );
		if ( DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API ) (* DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B );
		if ( DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API ) (* DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C );
		if ( DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API ) (* DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D );
		if ( DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API ) (* DRV_SCH0_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E );
		//-----------------------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------
		if ( DRV_USER_RECIPE_DATA_INIT ) (* DRV_USER_RECIPE_DATA_INIT ) ();
		//---------------------------------------------------------------------------------------------------------
		return TRUE;
		//---------------------------------------------------------------------------------------------------------
	}
	else {
		DRV_USER_RECIPE_DATA_INIT					= FALSE;
		DRV_USER_RECIPE_DATA_MODE					= FALSE;
		DRV_USER_RECIPE_DEFAULT_DATA_READ			= FALSE;
		DRV_USER_RECIPE_DEFAULT_DATA_REGIST			= FALSE;
		DRV_USER_RECIPE_PM_MODE						= FALSE;
		DRV_USER_RECIPE_PM_MODE_EX					= FALSE; // 2005.07.18
		DRV_USER_RECIPE_LOT_DATA_READ				= FALSE;
		DRV_USER_RECIPE_LOT_DUMMY_DATA_READ			= FALSE; // 2003.09.24
		DRV_USER_RECIPE_CLUSTER_DATA_READ			= FALSE;
		DRV_USER_RECIPE_CLUSTER_DATA_READ2			= FALSE; // 2012.06.18
		DRV_USER_RECIPE_INOUT_DATA_READ				= FALSE;
		DRV_USER_RECIPE_MAPPING_VERIFICATION		= FALSE;
		//
		DRV_USER_RECIPE_MANUAL_DATA_MODE			= FALSE;
		DRV_USER_RECIPE_MANUAL_DEFAULT_DATA_READ	= FALSE;
		DRV_USER_RECIPE_MANUAL_PM_MODE				= FALSE;
		DRV_USER_RECIPE_MANUAL_PM_MODE_EX			= FALSE; // 2005.07.18
		DRV_USER_RECIPE_MANUAL_LOT_DATA_READ		= FALSE;
		DRV_USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ	= FALSE; // 2003.09.24
		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ	= FALSE;
		DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ2	= FALSE; // 2012.06.18
		DRV_USER_RECIPE_MANUAL_INOUT_DATA_READ		= FALSE;
		DRV_USER_RECIPE_MANUAL_MAPPING_VERIFICATION	= FALSE;
		//
		DRV_USER_RECIPE_INFO_VERIFICATION			= FALSE; // 2004.12.14
		DRV_USER_RECIPE_INFO_VERIFICATION_EX		= FALSE; // 2005.01.24
		DRV_USER_RECIPE_INFO_VERIFICATION_EX2		= FALSE; // 2007.04.25
		DRV_USER_RECIPE_INFO_VERIFICATION_EX3		= FALSE; // 2008.07.24
		DRV_USER_RECIPE_INFO_VERIFICATION_EX4		= FALSE; // 2009.04.01
		//
		DRV_USER_RECIPE_INFO_VERIFICATION_EIL		= FALSE; // 2010.09.16
		//
		DRV_USER_RECIPE_INFO_FLOWING				= FALSE; // 2010.12.02
		//
		DRV_USER_RECIPE_START_MODE_CHANGE			= FALSE; // 2005.08.02
		DRV_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE		= FALSE; // 2005.08.02
		//
		DRV_USER_RECIPE_ACTION_AFTER_READY			= FALSE; // 2005.10.26

		//
		DRV_USER_RECIPE_TUNE_DATA_APPEND			= FALSE; // 2007.06.11
		//
		DRV_USER_RECIPE_DUMMY_OPERATE				= FALSE;
		//
		DRV_USER_RECIPE_LOOPCOUNT_CHANGE			= FALSE; // 2014.05.13
		//
		DRV_USER_RECIPE_LOT_SLOTFIX_SETTING			= FALSE; // 2015.10.12
		//
		//==================================================================================================================
		if ( STR_MEM_SIZE( DRV_DLL_USER_FILENAME ) > 0 ) { // 2008.04.03
			_IO_CIM_PRINTF( "[WARNING] MESSAGE ANALYSIS DLL(%s) Loading Fail\n" , DRV_DLL_USER_FILENAME );
			DLL_USER_RECIPE_USE = 0;
		}
		else {
			DLL_USER_RECIPE_USE = -1;
		}
		//==================================================================================================================
	}
	//
	return FALSE;
}
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
int USER_RECIPE_DATA_MODE() {
	if ( !DRV_USER_RECIPE_DATA_MODE || ( DLL_USER_RECIPE_USE != 1 ) ) return 0;
	return ( (* DRV_USER_RECIPE_DATA_MODE ) () );
}
//=========================================================================================
BOOL USER_RECIPE_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC ) {
	int side;
	char ElseStr[256] , Else1Str[256] , Else2Str[256];

	if ( !DRV_USER_RECIPE_DEFAULT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		STR_SEPERATE_CHAR( InfoStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			if ( Else1Str[4] == 0 )	side = 0;
			else					side = atoi( Else1Str + 4 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*SC = side;
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			if ( Else1Str[2] == 0 )	side = 0;
			else					side = atoi( Else1Str + 2 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*SC = side;
		}
		else return FALSE;
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			if ( Else1Str[4] == 0 )	side = 0;
			else					side = atoi( Else1Str + 4 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*EC = side;
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			if ( Else1Str[2] == 0 )	side = 0;
			else					side = atoi( Else1Str + 2 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*EC = side;
		}
		else return FALSE;
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , JobStr , Else2Str , 255 );
		STR_SEPERATE_CHAR( Else2Str , '|' , LotStr , ElseStr , 255 );
		STR_SEPERATE_CHAR( ElseStr , '|' , MidStr , Else2Str , 255 );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		*SS = atoi( Else1Str );
		//======
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		*ES = atoi( Else1Str );
		//======
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		*LC = atoi( Else1Str );
		return TRUE;
	}
	return( (* DRV_USER_RECIPE_DEFAULT_DATA_READ ) ( REGID , InfoStr , SC , EC , JobStr , LotStr , MidStr , SS , ES , LC ) );
}
//=========================================================================================
BOOL USER_RECIPE_DEFAULT_DATA_REGIST( char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC , char *DispStr ) {
	int side;
	char ElseStr[256] , Else1Str[256] , Else2Str[256];

	if ( !DRV_USER_RECIPE_DEFAULT_DATA_REGIST || ( DLL_USER_RECIPE_USE != 1 ) ) {
		STR_SEPERATE_CHAR( InfoStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			if ( Else1Str[4] == 0 )	side = 0;
			else					side = atoi( Else1Str + 4 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*SC = side;
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			if ( Else1Str[2] == 0 )	side = 0;
			else					side = atoi( Else1Str + 2 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*SC = side;
		}
		else return FALSE;
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			if ( Else1Str[4] == 0 )	side = 0;
			else					side = atoi( Else1Str + 4 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*EC = side;
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			if ( Else1Str[2] == 0 )	side = 0;
			else					side = atoi( Else1Str + 2 ) - 1;
			if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
			*EC = side;
		}
		else return FALSE;
		//======
		STR_SEPERATE_CHAR( ElseStr , '|' , JobStr , Else2Str , 255 );
		//======
		STR_SEPERATE_CHAR( Else2Str , '|' , LotStr , ElseStr , 255 );
		//======
		STR_SEPERATE_CHAR( ElseStr , '|' , MidStr , Else2Str , 255 );
		//======
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		*SS = atoi( Else1Str );
		//======
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		*ES = atoi( Else1Str );
		//======
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , DispStr , 255 );
		*LC = atoi( Else1Str );
		return TRUE;
	}
	return( (* DRV_USER_RECIPE_DEFAULT_DATA_REGIST ) ( InfoStr , SC , EC , JobStr , LotStr , MidStr , SS , ES , LC , DispStr ) );
}
//=========================================================================================
int USER_RECIPE_PM_MODE( int REGID , int side , char *allname , char *rcpname , char *grpname ) {
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 1 );
	//
//	if ( !DRV_USER_RECIPE_PM_MODE || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE; // 2005.07.18
	if ( (!DRV_USER_RECIPE_PM_MODE && !DRV_USER_RECIPE_PM_MODE_EX) || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 1 );
		//
		return 0; // 2005.07.18
	}
	if ( DRV_USER_RECIPE_PM_MODE_EX ) { // 2005.07.18
//		return( (* DRV_USER_RECIPE_PM_MODE_EX ) ( REGID , side , allname , rcpname , grpname) ); // 2005.07.18 // 2015.06.23
		Res = (* DRV_USER_RECIPE_PM_MODE_EX ) ( REGID , side , allname , rcpname , grpname); // 2005.07.18 // 2015.06.23
	} // 2005.07.18
	else if ( DRV_USER_RECIPE_PM_MODE ) {
//		return( (* DRV_USER_RECIPE_PM_MODE ) ( REGID , side ) ); // 2015.06.23
		Res = (* DRV_USER_RECIPE_PM_MODE ) ( REGID , side ); // 2015.06.23
	}
//	return 0; // 2015.06.23
	//
	USER_RECIPE_TAG_END( 0 , side , 1 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *RunName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 2 );
	//
	if ( !DRV_USER_RECIPE_LOT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 2 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_LOT_DATA_READ ) ( REGID , side , LOT_RECIPE , RunName );
	//
	USER_RECIPE_TAG_END( 0 , side , 2 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *RunName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 3 );
	//
	if ( !DRV_USER_RECIPE_LOT_DUMMY_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 3 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_LOT_DUMMY_DATA_READ ) ( REGID , side , LOT_RECIPE_DUMMY , RunName );
	//
	USER_RECIPE_TAG_END( 0 , side , 3 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 4 );
	//
	if ( !DRV_USER_RECIPE_CLUSTER_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 4 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_CLUSTER_DATA_READ ) ( REGID , side , CLUSTER_RECIPE , Slot , SlotName );
	//
	USER_RECIPE_TAG_END( 0 , side , 4 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_CLUSTER_DATA_READ2( int REGID , int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , int Slot , char *SlotName ) { // 2012.06.18
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 5 );
	//
	if ( !DRV_USER_RECIPE_CLUSTER_DATA_READ2 || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 5 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_CLUSTER_DATA_READ2 ) ( REGID , side , CLUSTER_RECIPE , Slot , SlotName );
	//
	USER_RECIPE_TAG_END( 0 , side , 5 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_INOUT_DATA_READ( int REGID , int side , CASSETTETemplate *CASSETTE ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 6 );
	//
	if ( !DRV_USER_RECIPE_INOUT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 6 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_INOUT_DATA_READ ) ( REGID , side , CASSETTE );
	//
	USER_RECIPE_TAG_END( 0 , side , 6 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 7 );
	//
	if ( !DRV_USER_RECIPE_MAPPING_VERIFICATION || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 7 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MAPPING_VERIFICATION ) ( REGID , side , position , MAPPING_INFO );
	//
	USER_RECIPE_TAG_END( 0 , side , 7 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
int USER_RECIPE_MANUAL_DATA_MODE() {
	if ( !DRV_USER_RECIPE_MANUAL_DATA_MODE || ( DLL_USER_RECIPE_USE != 1 ) ) return 0;
	return ( (* DRV_USER_RECIPE_MANUAL_DATA_MODE ) () );
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC ) {
	if ( !DRV_USER_RECIPE_MANUAL_DEFAULT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		strncpy( JobStr , InfoStr , 255 );
		return TRUE;
	}
	return( (* DRV_USER_RECIPE_MANUAL_DEFAULT_DATA_READ ) ( REGID , InfoStr , SC , EC , JobStr , LotStr , MidStr , SS , ES , LC ) );
}
//=========================================================================================
int USER_RECIPE_MANUAL_PM_MODE( int REGID , int side , char *allname , char *rcpname , char *grpname ) {
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 11 );
	//
//	if ( !DRV_USER_RECIPE_MANUAL_PM_MODE || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE; // 2005.07.18
	if ( (!DRV_USER_RECIPE_MANUAL_PM_MODE && !DRV_USER_RECIPE_MANUAL_PM_MODE_EX) || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 11 );
		//
		return 0; // 2005.07.18
		//
	}
	if ( DRV_USER_RECIPE_MANUAL_PM_MODE_EX ) { // 2005.07.18
		//
		USER_RECIPE_TAG_START( 0 , side , 12 );
		//
		Res = (* DRV_USER_RECIPE_MANUAL_PM_MODE_EX ) ( REGID , side , allname , rcpname , grpname); // 2005.07.18
		//
		USER_RECIPE_TAG_END( 0 , side , 12 );
		//
	} // 2005.07.18
	else if ( DRV_USER_RECIPE_MANUAL_PM_MODE ) {
		//
		Res = (* DRV_USER_RECIPE_MANUAL_PM_MODE ) ( REGID , side );
		//
		USER_RECIPE_TAG_END( 0 , side , 11 );
		//
	}
	//
	return Res;
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_INOUT_DATA_READ( int REGID , int side , CASSETTETemplate *CASSETTE ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 13 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_INOUT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 13 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_INOUT_DATA_READ ) ( REGID , side , CASSETTE );
	//
	USER_RECIPE_TAG_END( 0 , side , 13 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *RunName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 14 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_LOT_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 14 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_LOT_DATA_READ ) ( REGID , side , LOT_RECIPE , RunName );
	//
	USER_RECIPE_TAG_END( 0 , side , 14 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *RunName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 15 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 15 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ ) ( REGID , side , LOT_RECIPE_DUMMY , RunName );
	//
	USER_RECIPE_TAG_END( 0 , side , 15 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *SlotName ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 16 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 16 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ ) ( REGID , side , CLUSTER_RECIPE , Slot , SlotName );
	//
	USER_RECIPE_TAG_END( 0 , side , 16 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ2( int REGID , int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , int Slot , char *SlotName ) { // 2012.06.18
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 17 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ2 || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 17 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_CLUSTER_DATA_READ2 ) ( REGID , side , CLUSTER_RECIPE , Slot , SlotName );
	//
	USER_RECIPE_TAG_END( 0 , side , 17 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
BOOL USER_RECIPE_MANUAL_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 18 );
	//
	if ( !DRV_USER_RECIPE_MANUAL_MAPPING_VERIFICATION || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 18 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_MANUAL_MAPPING_VERIFICATION ) ( REGID , side , position , MAPPING_INFO );
	//
	USER_RECIPE_TAG_END( 0 , side , 18 );
	//
	return Res; // 2015.06.23
}

//=========================================================================================
// 2004.12.14
//=========================================================================================
BOOL USER_RECIPE_INFO_VERIFICATION_API_EXIST() {
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() { // 2005.01.24
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() { // 2007.04.25
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX2 || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() { // 2008.07.24
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX3 || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX4_API_EXIST() { // 2009.04.01
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX4 || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}

BOOL USER_RECIPE_INFO_VERIFICATION( int REGID , int side , int loopcount , int id , CLUSTERStepTemplate CLUSTER_INFO ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 21 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 21 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION ) ( REGID , side , loopcount , id , CLUSTER_INFO );
	//
	USER_RECIPE_TAG_END( 0 , side , 21 );
	//
	return Res; // 2015.06.23
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate CLUSTER_INFO , int LotSpecialData , char *LotUserdata , char *ClusterUserdata ) { // 2005.01.24
	BOOL Res;
	//
	USER_RECIPE_TAG_START( 0 , side , 22 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 22 );
		//
		return FALSE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION_EX ) ( REGID , side , pos , loopcount , orderindex , id , CLUSTER_INFO , LotSpecialData , LotUserdata , ClusterUserdata );
	//
	USER_RECIPE_TAG_END( 0 , side , 22 );
	//
	return Res; // 2015.06.23
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX2( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate2 CLUSTER_INFO , int LotSpecialData , char *LotUserdata , char *ClusterUserdata , char *clusterTuneData ) { // 2007.04.25
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 23 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX2 || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 23 );
		//
		return FALSE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION_EX2 ) ( REGID , side , pos , loopcount , orderindex , id , CLUSTER_INFO , LotSpecialData , LotUserdata , ClusterUserdata , clusterTuneData );
	//
	USER_RECIPE_TAG_END( 0 , side , 23 );
	//
	return Res; // 2015.06.23
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX3( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserdata , char *ClusterUserdata , char *clusterTuneData ) { // 2007.04.25
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 24 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX3 || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 24 );
		//
		return FALSE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION_EX3 ) ( REGID , side , pos , loopcount , orderindex , id , CLUSTER_INFO , LotSpecialData , LotUserdata , ClusterUserdata , clusterTuneData );
	//
	USER_RECIPE_TAG_END( 0 , side , 24 );
	//
	return Res; // 2015.06.23
}
BOOL USER_RECIPE_INFO_VERIFICATION_EX4( int REGID , int side , int pos , int loopcount , int orderindex , int id , Scheduling_Path *CLUSTER_DATA ) { // 2009.04.01
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 25 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EX4 || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 25 );
		//
		return FALSE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION_EX4 ) ( REGID , side , pos , loopcount , orderindex , id , CLUSTER_DATA );
	//
	USER_RECIPE_TAG_END( 0 , side , 25 );
	//
	return Res; // 2015.06.23
}

BOOL USER_RECIPE_INFO_VERIFICATION_EIL( int side , int pos , int bm , int slot , int cm , int wafer , char *clusterfile , char *feedbackstr ) { // 2010.09.16
	BOOL Res;
	//
	USER_RECIPE_TAG_START( 0 , side , 26 );
	//
	if ( !DRV_USER_RECIPE_INFO_VERIFICATION_EIL || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 26 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_INFO_VERIFICATION_EIL ) ( side , pos , bm , slot , cm , wafer , clusterfile , feedbackstr );
	//
	USER_RECIPE_TAG_END( 0 , side , 26 );
	//
	return Res; // 2015.06.23
}


void USER_RECIPE_INFO_FLOWING( int side , int mode , int smode , int precheck , int etc , char *etc2 ) { // 2010.12.02
	//
	USER_RECIPE_TAG_START( 0 , side , 27 );
	//
	if ( !DRV_USER_RECIPE_INFO_FLOWING || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 27 );
		//
		return;
	}
	(* DRV_USER_RECIPE_INFO_FLOWING ) ( side , mode , smode , precheck , etc , etc2 );
	//
	USER_RECIPE_TAG_END( 0 , side , 27 );
	//
}

//=========================================================================================

//=========================================================================================
// 2005.10.26
//=========================================================================================
BOOL USER_RECIPE_ACTION_AFTER_READY( int side ) { // 2005.10.26
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 28 );
	//
	if ( !DRV_USER_RECIPE_ACTION_AFTER_READY || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 28 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_ACTION_AFTER_READY ) ( side );
	//
	USER_RECIPE_TAG_END( 0 , side , 28 );
	//
	return Res; // 2015.06.23
}

//=========================================================================================
// 2005.08.02
//=========================================================================================
BOOL USER_RECIPE_START_MODE_CHANGE_API_EXIST() {
	if ( !DRV_USER_RECIPE_START_MODE_CHANGE || ( DLL_USER_RECIPE_USE != 1 ) ) return FALSE;
	return TRUE;
}
BOOL USER_RECIPE_START_MODE_CHANGE( int REGID , int side , int loopcount , int firstmode , int *startmode ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 29 );
	//
	if ( !DRV_USER_RECIPE_START_MODE_CHANGE || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 29 );
		//
		return TRUE;
	}
	//
	Res = (* DRV_USER_RECIPE_START_MODE_CHANGE ) ( REGID , side , loopcount , firstmode , startmode );
	//
	USER_RECIPE_TAG_END( 0 , side , 29 );
	//
	return Res; // 2015.06.23
}
//=========================================================================================
// 2007.03.27
//=========================================================================================
BOOL USER_RECIPE_JOB_DEFAULT_MODE_CHANGE( BOOL runstyle , int REGID , int cassin , int cassout , char *jobname , char *lotname , char *midname , int *startslot , int *endslot , int *startmode , int *loopcount ) {
	int res , maxslot , maxunuse;
	//
	USER_RECIPE_TAG_START( 0 , cassin , 30 );
	//
	if ( !DRV_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , cassin , 30 );
		//
		return TRUE;
	}
	//
	res = (* DRV_USER_RECIPE_JOB_DEFAULT_MODE_CHANGE ) ( runstyle , REGID , cassin , cassout , jobname , lotname , midname , startslot , endslot , startmode , loopcount );
	//---------------------------------------------------------------------
	if ( *startslot <= 0 ) *startslot = 1;
	if ( *endslot   <= 0 ) *endslot   = 1;
	//
	maxslot = Scheduler_Get_Max_Slot( cassin , cassin + 1 , cassout + 1 , 11 , &maxunuse ); // 2010.12.17
	//
	if ( *startslot > maxslot ) *startslot = maxslot;
	if ( *endslot   > maxslot ) *endslot   = maxslot;
	//
	if ( *startslot > *endslot ) {
		res        = *startslot;
		*startslot = *endslot;
		*endslot   = res;
	}
	//
	if ( *loopcount <= 0 ) *loopcount = 1;
	//
	if ( *startmode < 0 ) *startmode = 0;
	if ( *startmode > 3 ) *startmode = 3;
	//---------------------------------------------------------------------
	//
	USER_RECIPE_TAG_END( 0 , cassin , 30 );
	//
	return res;
}
//=========================================================================================

//=========================================================================================
// 2007.01.12
//=========================================================================================

//=========================================================================================
// 2007.06.11
//=========================================================================================
BOOL USER_RECIPE_TUNE_DATA_APPEND( int side , int mode , int ch , int slot , char *allrcp , char *Buffer ) {
	BOOL Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 31 );
	//
	if ( !DRV_USER_RECIPE_TUNE_DATA_APPEND || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 31 );
		//
		return FALSE;
	}
	//
	Res = (*DRV_USER_RECIPE_TUNE_DATA_APPEND ) ( side , mode , ch , slot , allrcp , Buffer );
	//
	USER_RECIPE_TAG_END( 0 , side , 31 );
	//
	return Res; // 2015.06.23
}


int USER_RECIPE_DUMMY_OPERATE( int side , int pointer , int pathin , int slotin , int mode , int usecount , int selpm , int param , int *bmrealch , int *bmrealslot , char *clusterrecipe ) {
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 32 );
	//
	if ( !DRV_USER_RECIPE_DUMMY_OPERATE || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 32 );
		//
		return 0;
	}
	//
	Res = (*DRV_USER_RECIPE_DUMMY_OPERATE ) ( side , pointer , pathin , slotin , mode , usecount , selpm , param , bmrealch , bmrealslot , clusterrecipe );
	//
	USER_RECIPE_TAG_END( 0 , side , 32 );
	//
	return Res; // 2015.06.23
}

int USER_RECIPE_DUMMY_INFO_VERIFICATION( int side , int pointer , int pathin , int slotin , int selpm , int param , char *clusterrecipe , Scheduling_Path *CLUSTER_DATA ) { // 2009.05.20
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 33 );
	//
	if ( !DRV_USER_RECIPE_DUMMY_INFO_VERIFICATION || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 33 );
		//
		return 0;
	}
	//
	Res = (*DRV_USER_RECIPE_DUMMY_INFO_VERIFICATION ) ( side , pointer , pathin , slotin , selpm , param , clusterrecipe , CLUSTER_DATA );
	//
	USER_RECIPE_TAG_END( 0 , side , 33 );
	//
	return Res; // 2015.06.23
}

int USER_RECIPE_PROCESS_FINISH_FOR_DUMMY( int remap , int side , int pointer , int ch , int runorder , int PathRange , int cutorder , int cutxindex , int order , char *recipe , Scheduling_Path *CLUSTER_DATA ) { // 2009.05.28
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 34 );
	//
	if ( !DRV_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 34 );
		//
		return 0;
	}
	//
	Res = (*DRV_USER_RECIPE_PROCESS_FINISH_FOR_DUMMY ) ( remap , side , pointer , ch , runorder , PathRange , cutorder , cutxindex , order , recipe , CLUSTER_DATA );
	//
	USER_RECIPE_TAG_END( 0 , side , 34 );
	//
	return Res; // 2015.06.23
}


int USER_RECIPE_DATA_PRE_APPEND( int side , int mode ) { // 2010.05.08
	int Res; // 2015.06.23
	//
	USER_RECIPE_TAG_START( 0 , side , 35 );
	//
	if ( !DRV_USER_RECIPE_DATA_PRE_APPEND || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 35 );
		//
		return 0;
	}
	//
	Res = (*DRV_USER_RECIPE_DATA_PRE_APPEND ) ( side , mode );
	//
	USER_RECIPE_TAG_END( 0 , side , 35 );
	//
	return Res; // 2015.06.23
}


int USER_RECIPE_GET_MODULE_FROM_ALARM_INFO( int moduleindex , char *ALARM_MODULE , int ALARM_MODULE_INDEX , int ALARM_ID , char *ALARM_NAME , char *ALARM_MESSAGE , int ALARM_LEVEL , int ALARM_STYLE , int ALARM_HAPPEN , char *ALARM_HAPPEN_INFO , int *findex ) { // 2012.03.08
	if ( !DRV_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO || ( DLL_USER_RECIPE_USE != 1 ) ) return 0;
	return ( (*DRV_USER_RECIPE_GET_MODULE_FROM_ALARM_INFO ) ( moduleindex , ALARM_MODULE , ALARM_MODULE_INDEX , ALARM_ID , ALARM_NAME , ALARM_MESSAGE , ALARM_LEVEL , ALARM_STYLE , ALARM_HAPPEN , ALARM_HAPPEN_INFO , findex ) );
}

//=========================================================================================
// 2014.05.13
//=========================================================================================
BOOL USER_RECIPE_LOOPCOUNT_CHANGE( int REGID , int side , int precheck , int *loopcount ) {
	//
	int lc;
	//
	USER_RECIPE_TAG_START( 0 , side , 36 );
	//
	if ( !DRV_USER_RECIPE_LOOPCOUNT_CHANGE || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 36 );
		//
		return FALSE;
	}
	//
	lc = *loopcount;
	//
	if ( (* DRV_USER_RECIPE_LOOPCOUNT_CHANGE ) ( REGID , side , precheck , &lc ) ) {
		//
		if ( *loopcount != lc ) {
			//
			if ( lc > 0 ) {
				//
				*loopcount = lc;
				//
				USER_RECIPE_TAG_END( 0 , side , 36 );
				//
				return TRUE;
			}
			//
		}
		//
	}
	//
	USER_RECIPE_TAG_END( 0 , side , 36 );
	//
	return FALSE;
}



//=========================================================================================
// 2015.10.12
//=========================================================================================

void USER_RECIPE_LOT_SLOTFIX_SETTING_FOR_DEFAULT( int side , int pt , int PreCheck , int Mode , int *PathIn , int *SlotIn , int *PathOut , int *SlotOut ) {
	//
	int i , j , slotdata[MAX_CASSETTE_SLOT_SIZE] , si;
	//
	if ( Mode <= 0 ) {
		if ( ( Mode != -991 ) && ( Mode != -992 ) ) { // 2016.07.13 // 2016.08.25
			return;
		}
	}
	//
	if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , -1 ) ) return;
	//
	if ( *PathIn == 0 ) *PathIn = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
	//
	if ( *SlotIn != 0 ) return;
	//
	if ( *PathIn != _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) slotdata[i] = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i != side ) {
			if ( !_i_SCH_SYSTEM_USING_STARTING( i ) ) continue;
		}
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_CM_END ) continue;
			//
			si = _i_SCH_CLUSTER_Get_SlotIn( i , j ) - 1;
			//
			if ( si < 0 ) continue;
			//
			slotdata[si]++;
			//
		}
		//
	}
	//
	si = -1;
	j = -1;
	//
	for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
		//
		if ( slotdata[i] > 0 ) continue;
		//
		if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i+1 ) <= 0 ) continue;
		//
//		if ( _i_SCH_SDM_Get_RUN_CHAMBER( i ) == 0 ) continue;
		//
		if ( ( j == -1 ) || ( j > _i_SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
			//
			j = _i_SCH_SDM_Get_RUN_USECOUNT(i);
			si = i;
			//
		}
		//
	}
	//
	if ( si >= 0 ) *SlotIn = si+1;
	//
}

BOOL USER_RECIPE_LOT_SLOTFIX_SETTING( int side , int pt , int PreCheck , int Mode , int *PathIn , int *SlotIn , int *PathOut , int *SlotOut ) {
	//
	USER_RECIPE_TAG_START( 0 , side , 37 );
	//
	if ( !DRV_USER_RECIPE_LOT_SLOTFIX_SETTING || ( DLL_USER_RECIPE_USE != 1 ) ) {
		//
		USER_RECIPE_LOT_SLOTFIX_SETTING_FOR_DEFAULT( side , pt , PreCheck , Mode , PathIn , SlotIn , PathOut , SlotOut );
		//
		USER_RECIPE_TAG_END( 0 , side , 37 );
		//
		return TRUE;
	}
	//
	if ( !(* DRV_USER_RECIPE_LOT_SLOTFIX_SETTING ) ( side , pt , PreCheck , Mode , PathIn , SlotIn , PathOut , SlotOut ) ) {
		//
		USER_RECIPE_TAG_END( 0 , side , 37 );
		//
		return FALSE;
	}
	//
	if ( *PathIn == 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() > 0 ) {
			*PathIn = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		}
	}
	//
	USER_RECIPE_TAG_END( 0 , side , 37 );
	//
	return TRUE;
}
