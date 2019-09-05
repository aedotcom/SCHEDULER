#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "Robot_Handling.h"
#include "IO_Part_Log.h"
#include "DLL_Interface.h"
#include "MR_Data.h"
#include "multijob.h"
#include "sch_CassetteSupply.h"
#include "sch_CassetteResult.h"
#include "sch_prepost.h"
#include "sch_prm.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_Cassmap.h"
#include "sch_restart.h"
#include "RcpFile_Handling.h"
#include "iolog.h"
#include "utility.h"
#include "Errorcode.h"
#include "File_ReadInit.h"

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"

void OPERATE_CONTROL( int side , int mode );

int SCHEDULER_GET_SIDE_CM( int side ); // 2018.08.30

BOOL SCHEDULER_SIDE_IN_POSSIBLE( int , int side ); // 2010.01.19
BOOL SCHEDULER_SIDE_OUT_POSSIBLE( BOOL , int side ); // 2010.01.19

//------------------------------------------------------------------------------------------
CLUSTERStepTemplate		G_CLUSTER_RECIPE_INF; // 2008.07.24
CLUSTERStepTemplate		G_CLUSTER_RECIPE_INF_ORG; // 2008.01.11 // 2008.07.24
//
CLUSTERStepTemplate2	G_CLUSTER_RECIPE; // 2008.07.24
//------------------------------------------------------------------------------------------
int				MULTI_RECIPE_SELECT = -1; // 2006.07.03
int				CLUSTER_INDEX_SELECT = -1; // 2006.07.03
//int				CLUSTER_INDEX_LAST_SIDE = -1; // 2008.02.26 // 2008.04.17
char			CLUSTER_INDEX_CRECIPE[256] = { 0 , }; // 2006.07.03
char			CLUSTER_INDEX_LRECIPE[256] = { 0 , }; // 2008.07.09
CLUSTERStepTemplate2 CLUSTER_INDEX_SRECIPE; // 2008.07.16
int				CLUSTER_INDEX_SIDE = -1; // 2014.01.24
int				CLUSTER_INDEX_PATHIN = -1; // 2014.10.31
int				CLUSTER_INDEX_SLOTIN = -1; // 2014.10.31
//
int				CLUSTER_INDEX_SELECT_BUFFER; // 2008.06.11
char			CLUSTER_INDEX_CRECIPE_BUFFER[256] = { 0 , }; // 2008.06.11
char			CLUSTER_INDEX_LRECIPE_BUFFER[256] = { 0 , }; // 2008.07.09
CLUSTERStepTemplate2 CLUSTER_INDEX_SRECIPE_BUFFER; // 2008.07.16
int				CLUSTER_INDEX_SIDE_BUFFER = -1; // 2014.01.24
int				CLUSTER_INDEX_PATHIN_BUFFER = -1; // 2014.10.31
int				CLUSTER_INDEX_SLOTIN_BUFFER = -1; // 2014.10.31

int				CARRIER_G_INDEX[MAX_CASSETTE_SIDE]; // 2011.08.16

//int				CARRIER_INDEX_SELECT = 0; // 2011.08.10 // 2011.09.14

// 2014.02.05
//
//#define	MAX_USER_APPEND_RECIPE	256 // 2015.03.30
#define	MAX_USER_APPEND_RECIPE	1024 // 2015.03.30
//
int	 USER_APPEND_RECIPE_COUNT[MAX_CASSETTE_SIDE];
int  USER_APPEND_RECIPE_MODE[MAX_CASSETTE_SIDE][MAX_USER_APPEND_RECIPE];
char *USER_APPEND_RECIPE[MAX_CASSETTE_SIDE][MAX_USER_APPEND_RECIPE];



void SCHEDULER_Reset_CARRIER_INDEX() {
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE; i++ ) CARRIER_G_INDEX[i] = -1;
	//
//	CARRIER_INDEX_SELECT = 0; // 2011.09.14
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE; i++ ) USER_APPEND_RECIPE_COUNT[i] = 0; // 2014.02.06
	//
}
//
void SCHEDULER_Set_CARRIER_INDEX( int side , int data ) {
	CARRIER_G_INDEX[side] = data;
}

int SCHEDULER_Get_CARRIER_INDEX( int side ) {
	return CARRIER_G_INDEX[side]; // 2011.08.16
}

/*
// 2011.09.14
void SCHEDULER_Make_CARRIER_INDEX( int side ) {
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() != 0 ) {
		CARRIER_INDEX_SELECT++;
		CARRIER_G_INDEX[side] = ( CARRIER_INDEX_SELECT * 10 ) + side;
	}
}
*/

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi Select Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Set_MULTI_RECIPE_SELECT( int lastseldata ) { // 2006.07.03
	MULTI_RECIPE_SELECT = lastseldata;
}
//
int SCHEDULER_Get_MULTI_RECIPE_SELECT() { // 2006.07.03
	return MULTI_RECIPE_SELECT;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Scheduler_ROBOT_FINGER_HAS_WAFER( int TMATM , int STATION ) {
	if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
		if ( WAFER_STATUS_IN_TM( TMATM , STATION ) != 0 ) return TRUE;
		if ( WAFER_STATUS_IN_TM2( TMATM , STATION ) != 0 ) return TRUE;
		return FALSE;
	}
	else {
		if ( WAFER_STATUS_IN_TM( TMATM , STATION ) != 0 ) return TRUE;
		else                                      return FALSE;
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Cluster Index Select Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_Init_CLUSTER_INDEX_Data( CLUSTERStepTemplate2 *sr ) { // 2008.07.16
	int i , j;

	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			sr->MODULE[i][j] = 0;
			//
			sr->MODULE_PR_RECIPE2[i][j] = NULL;
			sr->MODULE_IN_RECIPE2[i][j] = NULL;
			sr->MODULE_OUT_RECIPE2[i][j] = NULL;
			//
		}
	}
}
//
void SCHEDULER_Copy_CLUSTER_INDEX_Data( CLUSTERStepTemplate2 *tr , CLUSTERStepTemplate2 *sr ) { // 2008.07.16
	int i , j , len;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			tr->MODULE[i][j] = sr->MODULE[i][j];
			//
			if ( sr->MODULE_PR_RECIPE2[i][j] == NULL ) {
				if ( tr->MODULE_PR_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_PR_RECIPE2[i][j] );
					tr->MODULE_PR_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( tr->MODULE_PR_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_PR_RECIPE2[i][j] );
				}
				len = (signed) strlen( sr->MODULE_PR_RECIPE2[i][j] );
				tr->MODULE_PR_RECIPE2[i][j] = calloc( len + 1 , sizeof( char ) );
				if ( tr->MODULE_PR_RECIPE2[i][j] == NULL ) {
					_IO_CIM_PRINTF( "SCHEDULER_Copy_CLUSTER_INDEX_Data Memory Allocate Error\n" );
				}
				else {
					strncpy( tr->MODULE_PR_RECIPE2[i][j] , sr->MODULE_PR_RECIPE2[i][j] , len );
				}
			}
			//
			if ( sr->MODULE_IN_RECIPE2[i][j] == NULL ) {
				if ( tr->MODULE_IN_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_IN_RECIPE2[i][j] );
					tr->MODULE_IN_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( tr->MODULE_IN_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_IN_RECIPE2[i][j] );
				}
				len = (signed) strlen( sr->MODULE_IN_RECIPE2[i][j] );
				tr->MODULE_IN_RECIPE2[i][j] = calloc( len + 1 , sizeof( char ) );
				if ( tr->MODULE_IN_RECIPE2[i][j] == NULL ) {
					_IO_CIM_PRINTF( "SCHEDULER_Copy_CLUSTER_INDEX_Data Memory Allocate Error\n" );
				}
				else {
					strncpy( tr->MODULE_IN_RECIPE2[i][j] , sr->MODULE_IN_RECIPE2[i][j] , len );
				}
			}
			//
			if ( sr->MODULE_OUT_RECIPE2[i][j] == NULL ) {
				if ( tr->MODULE_OUT_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_OUT_RECIPE2[i][j] );
					tr->MODULE_OUT_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( tr->MODULE_OUT_RECIPE2[i][j] != NULL ) {
					free( tr->MODULE_OUT_RECIPE2[i][j] );
				}
				len = (signed) strlen( sr->MODULE_OUT_RECIPE2[i][j] );
				tr->MODULE_OUT_RECIPE2[i][j] = calloc( len + 1 , sizeof( char ) );
				if ( tr->MODULE_OUT_RECIPE2[i][j] == NULL ) {
					_IO_CIM_PRINTF( "SCHEDULER_Copy_CLUSTER_INDEX_Data Memory Allocate Error\n" );
				}
				else {
					strncpy( tr->MODULE_OUT_RECIPE2[i][j] , sr->MODULE_OUT_RECIPE2[i][j] , len );
				}
			}
		}
	}
}
//
BOOL SCHEDULER_Same_CLUSTER_INDEX_Data( CLUSTERStepTemplate2 *tr , CLUSTERStepTemplate2 *sr , BOOL detail ) { // 2008.07.16
	int i , j;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			if ( tr->MODULE[i][j] != sr->MODULE[i][j] ) return FALSE;
			//
			if ( detail ) {
				//
				if ( ( tr->MODULE_PR_RECIPE2[i][j] == NULL ) && ( sr->MODULE_PR_RECIPE2[i][j] != NULL ) ) return FALSE;
				if ( ( tr->MODULE_PR_RECIPE2[i][j] != NULL ) && ( sr->MODULE_PR_RECIPE2[i][j] == NULL ) ) return FALSE;
				//
				if ( ( tr->MODULE_IN_RECIPE2[i][j] == NULL ) && ( sr->MODULE_IN_RECIPE2[i][j] != NULL ) ) return FALSE;
				if ( ( tr->MODULE_IN_RECIPE2[i][j] != NULL ) && ( sr->MODULE_IN_RECIPE2[i][j] == NULL ) ) return FALSE;
				//
				if ( ( tr->MODULE_OUT_RECIPE2[i][j] == NULL ) && ( sr->MODULE_OUT_RECIPE2[i][j] != NULL ) ) return FALSE;
				if ( ( tr->MODULE_OUT_RECIPE2[i][j] != NULL ) && ( sr->MODULE_OUT_RECIPE2[i][j] == NULL ) ) return FALSE;
				//
				if ( tr->MODULE_PR_RECIPE2[i][j] != NULL ) {
					if ( !STRCMP_L( tr->MODULE_PR_RECIPE2[i][j] , sr->MODULE_PR_RECIPE2[i][j] ) ) return FALSE;
				}
				//
				if ( tr->MODULE_IN_RECIPE2[i][j] != NULL ) {
					if ( !STRCMP_L( tr->MODULE_IN_RECIPE2[i][j] , sr->MODULE_IN_RECIPE2[i][j] ) ) return FALSE;
				}
				//
				if ( tr->MODULE_OUT_RECIPE2[i][j] != NULL ) {
					if ( !STRCMP_L( tr->MODULE_OUT_RECIPE2[i][j] , sr->MODULE_OUT_RECIPE2[i][j] ) ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}
//
void SCHEDULER_Set_CLUSTER_INDEX_SELECT_VALUE( int lastseldata ) { // 2019.05.29
	//
	CLUSTER_INDEX_SELECT = lastseldata;
	//
}
//
void SCHEDULER_Set_CLUSTER_INDEX_SELECT( BOOL init , int lastseldata ) { // 2006.07.03
	//
//	CLUSTER_INDEX_SELECT = lastseldata; // 2019.05.29
	SCHEDULER_Set_CLUSTER_INDEX_SELECT_VALUE( lastseldata ); // 2019.05.29
	//
	if ( init ) {
		SCHEDULER_Init_CLUSTER_INDEX_Data( &CLUSTER_INDEX_SRECIPE ); // 2008.07.16
		SCHEDULER_Init_CLUSTER_INDEX_Data( &CLUSTER_INDEX_SRECIPE_BUFFER ); // 2008.07.16
	}
}
//
//
void SCHEDULER_Set_CLUSTER_INDEX_BUFFER_SELECT() { // 2008.06.09
	CLUSTER_INDEX_SELECT_BUFFER = CLUSTER_INDEX_SELECT;
	//
	CLUSTER_INDEX_SIDE_BUFFER = CLUSTER_INDEX_SIDE; // 2014.01.24
	//
	CLUSTER_INDEX_PATHIN_BUFFER = CLUSTER_INDEX_PATHIN; // 2014.10.31
	CLUSTER_INDEX_SLOTIN_BUFFER = CLUSTER_INDEX_SLOTIN; // 2014.10.31
	//
	strcpy( CLUSTER_INDEX_CRECIPE_BUFFER , CLUSTER_INDEX_CRECIPE );
	//
	strcpy( CLUSTER_INDEX_LRECIPE_BUFFER , CLUSTER_INDEX_LRECIPE ); // 2008.07.09
	//
//	if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( &CLUSTER_INDEX_SRECIPE_BUFFER , &CLUSTER_INDEX_SRECIPE ); // 2008.07.16 // 2014.01.24
	if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( &CLUSTER_INDEX_SRECIPE_BUFFER , &CLUSTER_INDEX_SRECIPE ); // 2008.07.16 // 2014.01.24
}
//
//int SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *cluster_recipe , int *idx , char *lbstr , char *cbstr , CLUSTERStepTemplate2 *clsr ) { // 2008.07.09 // 2014.01.24
//int SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( int side , char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *cluster_recipe , int *idx , char *lbstr , char *cbstr , CLUSTERStepTemplate2 *clsr , int *rside ) { // 2008.07.09 // 2014.01.24 // 2014.10.31
//int SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( int side , int pathin , int slot , char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *cluster_recipe , int *idx , char *lbstr , char *cbstr , CLUSTERStepTemplate2 *clsr , int *rside , int *rpathin , int *rslotin ) { // 2008.07.09 // 2014.01.24 // 2014.10.31 // 2016.12.10
int SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( int side , int pathin , int slot , int Force_control , char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *cluster_recipe , int *idx , char *lbstr , char *cbstr , CLUSTERStepTemplate2 *clsr , int *rside , int *rpathin , int *rslotin ) { // 2008.07.09 // 2014.01.24 // 2014.10.31 // 2016.12.10
	BOOL notsame; // 2014.10.31
	//
	notsame = FALSE; // 2014.10.31
	//
//printf( "[XXX:1:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	if ( *idx == -1 ) {
//printf( "[XXX:2:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
		strncpy( cbstr , cluster_recipe_name , 255 );
		//
		strncpy( lbstr , lot_recipe_name , 255 );
		//
		if ( Force_control == 0 ) {
	//		if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
			if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
		}
		else {
			SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe );
		}
		//
//		*rside = side; // 2014.01.24 // 2014.10.31
		//
		*idx = 0;
		//
		notsame = TRUE; // 2014.10.31
		//
//printf( "[XXX:3:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	}
	else {
		switch( Force_control ) { // 2016.12.10 // 1:same 2:diff 0:default
		case 1 : // same
			//
			strncpy( cbstr , cluster_recipe_name , 255 );
			//
			strncpy( lbstr , lot_recipe_name , 255 );
			//
			if ( !SCHEDULER_Same_CLUSTER_INDEX_Data( clsr , cluster_recipe , TRUE ) ) {
				//
				*idx = *idx + 10;
				//
				notsame = TRUE;
				//
			}
			//
			SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe );
			//
			break;
		case 2 : // diff
			//
			strncpy( cbstr , cluster_recipe_name , 255 );
			//
			strncpy( lbstr , lot_recipe_name , 255 );
			//
			SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe );
			//
			*idx = *idx + 10;
			//
			notsame = TRUE;
			//
			break;
		default :
	//printf( "[XXX:4:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
			if ( !STRCMP_L( cbstr , cluster_recipe_name ) ) {
				//
	//printf( "[XXX:5:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	//			if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) >= 1 ) { // 2008.07.16 // 2014.01.24
				if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) { // 2008.07.16 // 2014.01.24
	//printf( "[XXX:6:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	//				if ( !SCHEDULER_Same_CLUSTER_INDEX_Data( clsr , cluster_recipe , ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) == 1 ) ) ) { // 2014.01.24
					if ( !SCHEDULER_Same_CLUSTER_INDEX_Data( clsr , cluster_recipe , ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) == 1 ) ) ) { // 2014.01.24
	//printf( "[XXX:7:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
						strncpy( cbstr , cluster_recipe_name , 255 );
						strncpy( lbstr , lot_recipe_name , 255 );
						//
	//printf( "[XXX:8:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
						SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16
						//
	//printf( "[XXX:9:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	//					*rside = side; // 2014.01.24 // 2014.10.31
						//
						*idx = *idx + 10;
						//
						notsame = TRUE; // 2014.10.31
						//
					}
				}
				else {
	//printf( "[XXX:10:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
					strncpy( cbstr , cluster_recipe_name , 255 );
					strncpy( lbstr , lot_recipe_name , 255 );
					//
	//printf( "[XXX:11:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	//				if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
					if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
					//
	//				*rside = side; // 2014.01.24 // 2014.10.31
					//
	//printf( "[XXX:12:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
					*idx = *idx + 10;
					//
					notsame = TRUE; // 2014.10.31
					//
				}
				//
			}
			else {
	//printf( "[XXX:13:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	//			if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() % 2 ) == 1 ) { // 2014.01.24
				if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() % 3 ) == 1 ) { // 2014.01.24
	//printf( "[XXX:14:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
					if ( !STRCMP_L( lbstr , lot_recipe_name ) ) {
	//printf( "[XXX:15:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
						if ( !STRCMP_L( lbstr , "" ) ) {
	//printf( "[XXX:16:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
							if ( !STRCMP_L( lot_recipe_name , "" ) ) {
	//printf( "[XXX:17:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
								strncpy( cbstr , cluster_recipe_name , 255 );
								strncpy( lbstr , lot_recipe_name , 255 );
								//
	//							if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 2 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
								if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe ); // 2008.07.16 // 2014.01.24
								//
	//							*rside = side; // 2014.01.24 // 2014.10.31
								//
	//printf( "[XXX:18:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
								*idx = *idx + 10;
								//
								notsame = TRUE; // 2014.10.31
								//
							}
							else {
								strncpy( lbstr , lot_recipe_name , 255 );
							}
						}
						else {
							strncpy( lbstr , lot_recipe_name , 255 );
						}
					}
				}
				else if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() % 3 ) == 2 ) { // 2014.01.24
	//printf( "[XXX:19:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
					if ( side != *rside ) {
						//
	//printf( "[XXX:20:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
						strncpy( cbstr , cluster_recipe_name , 255 );
						strncpy( lbstr , lot_recipe_name , 255 );
						//
	//printf( "[XXX:21:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
						if ( ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() / 3 ) >= 1 ) SCHEDULER_Copy_CLUSTER_INDEX_Data( clsr , cluster_recipe );
						//
	//					*rside = side; // 2014.10.31
						//
						*idx = *idx + 10;
						//
						notsame = TRUE; // 2014.10.31
						//
	//printf( "[XXX:22:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
					}
				}
			}
			break;
		}
	}
	//
//printf( "[XXX:23:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );
	if ( !notsame ) { // 2014.10.31
		//
//printf( "[XXX:24:%d:%d:%d:%s:%s:%d:%d:%d]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name , *rside , *rpathin , *rslotin );
		if ( !SCHMULTI_RUNJOB_CHECK_TUNING_INFO_SAME( *rside , *rpathin , *rslotin , side , pathin , slot , _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD2() ) ) {
			//
//printf( "[XXX:25:%d:%d:%d:%s:%s:%d:%d:%d]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name , *rside , *rpathin , *rslotin );
			*idx = *idx + 10;
			//
		}
		//
//printf( "[XXX:26:%d:%d:%d:%s:%s:%d:%d:%d]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name , *rside , *rpathin , *rslotin );
	}
	//
//printf( "[XXX:27:%d:%d:%d:%s:%s:%d:%d:%d]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name , *rside , *rpathin , *rslotin );

	//
	*rside = side; // 2014.10.31
	*rpathin = pathin; // 2014.10.31
	*rslotin = slot; // 2014.10.31
	//
//printf( "[XXX:28:%d:%d:%d:%s:%s]" , side , pathin , slot , lot_recipe_name , cluster_recipe_name );

	return *idx;
}

//int  SCHEDULER_Make_CLUSTER_INDEX_SELECT( int side , int pathin , int slot , char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *CLUSTER_RECIPE , BOOL precheck ) { // 2006.07.03 // 2016.12.10
int  SCHEDULER_Make_CLUSTER_INDEX_SELECT( int side , int pathin , int slot , int Force_control , char *lot_recipe_name , char *cluster_recipe_name , CLUSTERStepTemplate2 *CLUSTER_RECIPE , BOOL precheck ) { // 2006.07.03 // 2016.12.10
	if ( precheck ) { // 2008.06.11
//		return SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( lot_recipe_name , cluster_recipe_name , CLUSTER_RECIPE , &CLUSTER_INDEX_SELECT_BUFFER , CLUSTER_INDEX_LRECIPE_BUFFER , CLUSTER_INDEX_CRECIPE_BUFFER , &CLUSTER_INDEX_SRECIPE_BUFFER ); // 2014.01.24
		return SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( side , pathin , slot , Force_control , lot_recipe_name , cluster_recipe_name , CLUSTER_RECIPE , &CLUSTER_INDEX_SELECT_BUFFER , CLUSTER_INDEX_LRECIPE_BUFFER , CLUSTER_INDEX_CRECIPE_BUFFER , &CLUSTER_INDEX_SRECIPE_BUFFER , &CLUSTER_INDEX_SIDE_BUFFER , &CLUSTER_INDEX_PATHIN_BUFFER , &CLUSTER_INDEX_SLOTIN_BUFFER ); // 2014.01.24 // 2014.10.31
	}
	else {
//		return SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( lot_recipe_name , cluster_recipe_name , CLUSTER_RECIPE , &CLUSTER_INDEX_SELECT , CLUSTER_INDEX_LRECIPE , CLUSTER_INDEX_CRECIPE , &CLUSTER_INDEX_SRECIPE ); // 2014.01.24
		return SCHEDULER_Make_CLUSTER_INDEX_SUB_SELECT( side , pathin , slot , Force_control , lot_recipe_name , cluster_recipe_name , CLUSTER_RECIPE , &CLUSTER_INDEX_SELECT , CLUSTER_INDEX_LRECIPE , CLUSTER_INDEX_CRECIPE , &CLUSTER_INDEX_SRECIPE , &CLUSTER_INDEX_SIDE , &CLUSTER_INDEX_PATHIN , &CLUSTER_INDEX_SLOTIN ); // 2014.01.24 // 2014.10.31
	}
}



//void SCHEDULER_Make_CLUSTER_INDEX_END( int side ) { // 2008.02.26 // 2008.04.17
//	if ( CLUSTER_INDEX_SELECT != -1 ) { // 2008.04.14
//		if ( ( CLUSTER_INDEX_LAST_SIDE != -1 ) && ( CLUSTER_INDEX_LAST_SIDE == side ) ) { // 2008.04.14
//			CLUSTER_INDEX_SELECT = CLUSTER_INDEX_SELECT + 10; // 2008.04.14
//			CLUSTER_INDEX_LAST_SIDE = -1; // 2008.04.14
//		} // 2008.04.14
//	} // 2008.04.14
	//-------------------------------------------------------------------------------------------
	// 2008.04.14
	//-------------------------------------------------------------------------------------------
//	CLUSTER_INDEX_LAST_SIDE = side; // 2008.04.17
	//-------------------------------------------------------------------------------------------
//} // 2008.04.17
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_CONTROL_RECIPE_SETTING_DATA( int mode , int side ) {
	// mode = 0 : Init
	// mode = 1 : Start
	// mode = 2 : End
	// mode = 3 : Start(only)
	// mode = 4 : End(only)
	if ( ( mode == 0 ) || ( mode == 3 ) ) {
		SCHEDULER_Set_MULTI_RECIPE_SELECT( -1 ); // 2006.07.03
		SCHEDULER_Set_CLUSTER_INDEX_SELECT( ( mode == 0 ) , -1 ); // 2006.07.03
	}
	//
	if ( mode == 0 ) { // 2011.08.10
		SCHEDULER_Reset_CARRIER_INDEX();
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
// 2014.02.03
void Scheduler_CONTROL_Parallel_ReSetting( int side , int pt , int cldx , int idx , CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2007.02.21
	int i , id;
	int dt = -1 , ov100 = FALSE , ov50 = FALSE;
	char data[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		id = CLUSTER_RECIPE->MODULE[idx][i] % 1000; // 2014.01.28
		//
		if ( ( id > 0 ) && ( id < 100 ) ) {
			//
			if ( id >= 50 ) break; // 2014.01.31
			//
			if ( dt == -1 ) {
				dt = id;
			}
			else {
				if ( id != dt ) {
					break;
				}
			}
		}
		else if ( id >= 100 ) { // 2007.07.29
			ov100 = TRUE; // 2007.07.29
		} // 2007.07.29
	}
	if ( !ov100 ) { // 2007.07.29
		if ( i >= MAX_PM_CHAMBER_DEPTH ) return;
	}
	//===========================================================
	//===========================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		id = CLUSTER_RECIPE->MODULE[idx][i] % 1000; // 2014.01.28
		//
		if      ( id <= 0 ) {
			data[i] = 0;
		}
		else { // 2007.07.29
			data[i] = id;
		}
	}
	_i_SCH_CLUSTER_Set_PathProcessParallel( side , pt , cldx , data );
}

void Scheduler_CONTROL_Parallel_ReSetting2( int side , int pt , int cldx , int idx , CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2014.01.28
	int i;
	char data[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( CLUSTER_RECIPE->MODULE[idx][i] >= 1000 ) break;
	}
	//
	if ( i >= MAX_PM_CHAMBER_DEPTH ) return;
	//===========================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		data[i] = CLUSTER_RECIPE->MODULE[idx][i] / 1000;
		//
	}
	_i_SCH_CLUSTER_Set_PathProcessDepth( side , pt , cldx , data );
	//===========================================================
}

*/



// 2014.02.03
void Scheduler_CONTROL_Parallel_ReSetting( int side , int pt , int cldx , int idx , CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2007.02.21
	int i , id;
//	int dt = -1; // 2018.06.05
	int ov50 = FALSE;
	char data[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if      ( CLUSTER_RECIPE->MODULE[idx][i] >= 1000 ) { // Slot+Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i] % 1000;
		}
		else if ( ( CLUSTER_RECIPE->MODULE[idx][i] >= 100 ) && ( CLUSTER_RECIPE->MODULE[idx][i] < 1000 ) ) { // Slot Only
			continue;
		}
		else if ( CLUSTER_RECIPE->MODULE[idx][i] >= 0 ) { // Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i];
		}
		else {
			continue;
		}
		//
		if ( id <= 0 ) continue;
		//
		if ( id >= 50 ) break;
		//
		// 2018.06.05
		//
		/*
		//
		if ( dt == -1 ) {
			dt = id;
		}
		else {
			if ( id != dt ) {
				break;
			}
		}
		//
		*/
		//
		if ( id != 1 ) break; // 2018.06.05
		//
	}
	//
	if ( i >= MAX_PM_CHAMBER_DEPTH ) return;
	//
	//===========================================================
	//===========================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if      ( CLUSTER_RECIPE->MODULE[idx][i] >= 1000 ) { // Slot+Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i] % 1000;
		}
		else if ( ( CLUSTER_RECIPE->MODULE[idx][i] >= 100 ) && ( CLUSTER_RECIPE->MODULE[idx][i] < 1000 ) ) { // Slot Only
			id = 0;
		}
		else if ( CLUSTER_RECIPE->MODULE[idx][i] >= 0 ) { // Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i];
		}
		else {
			id = 0;
		}
		//
		data[i] = id;
		//
	}
	//
	_i_SCH_CLUSTER_Set_PathProcessParallelF( side , pt , cldx , data );
	//
}


void Scheduler_CONTROL_Parallel_ReSetting2( int side , int pt , int cldx , int idx , CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2014.01.28
	int i , id;
	char data[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if      ( CLUSTER_RECIPE->MODULE[idx][i] >= 1000 ) { // Slot+Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i] / 1000;
		}
		else if ( ( CLUSTER_RECIPE->MODULE[idx][i] >= 100 ) && ( CLUSTER_RECIPE->MODULE[idx][i] < 1000 ) ) { // Slot Only
			id = CLUSTER_RECIPE->MODULE[idx][i] % 100;
		}
		else if ( CLUSTER_RECIPE->MODULE[idx][i] >= 0 ) { // Parallel
			id = 0;
		}
		else {
			id = 0;
		}
		//
		if ( id > 0 ) break;
		//
	}
	//
	if ( i >= MAX_PM_CHAMBER_DEPTH ) return;
	//===========================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if      ( CLUSTER_RECIPE->MODULE[idx][i] >= 1000 ) { // Slot+Parallel
			id = CLUSTER_RECIPE->MODULE[idx][i] / 1000;
		}
		else if ( ( CLUSTER_RECIPE->MODULE[idx][i] >= 100 ) && ( CLUSTER_RECIPE->MODULE[idx][i] < 1000 ) ) { // Slot Only
			id = CLUSTER_RECIPE->MODULE[idx][i] % 100;
		}
		else if ( CLUSTER_RECIPE->MODULE[idx][i] >= 0 ) { // Parallel
			id = 0;
		}
		else {
			id = 0;
		}
		//
		data[i] = id;
		//
	}
	//
	_i_SCH_CLUSTER_Set_PathProcessDepthF( side , pt , cldx , data );
	//===========================================================
}



void Scheduler_CONTROL_CLUSTERStepTemplate2_Init( CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2007.03.08
	int i , j;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE->MODULE[i][j] = 0;
			CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] = NULL;
		}
	}
	CLUSTER_RECIPE->EXTRA_STATION = 0;
	CLUSTER_RECIPE->EXTRA_TIME    = 0;
	CLUSTER_RECIPE->HANDLING_SIDE = 0;
}

void Scheduler_CONTROL_CLUSTERStepTemplate2_Free( CLUSTERStepTemplate2 *CLUSTER_RECIPE ) { // 2007.03.08
	int i , j;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE->MODULE[i][j] = 0;
			if ( CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] != NULL ) free( CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
			CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] = NULL;
			if ( CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] != NULL ) free( CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
			CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] = NULL;
			if ( CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] != NULL ) free( CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] );
			CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] = NULL;
		}
	}
	CLUSTER_RECIPE->EXTRA_STATION = 0;
	CLUSTER_RECIPE->EXTRA_TIME    = 0;
	CLUSTER_RECIPE->HANDLING_SIDE = 0;
}

//void Scheduler_CONTROL_CLUSTERStepTemplate12_Set( CLUSTERStepTemplate *CLUSTER_RECIPE , CLUSTERStepTemplate *CLUSTER_RECIPE_ORG , CLUSTERStepTemplate2 *CLUSTER_RECIPE2 , int lotspecial ) { // 2007.03.08 // 014.06.23
void Scheduler_CONTROL_CLUSTERStepTemplate12_Set( CLUSTERStepTemplate *CLUSTER_RECIPE , CLUSTERStepTemplate *CLUSTER_RECIPE_ORG , CLUSTERStepTemplate2 *CLUSTER_RECIPE2 , int lotspecial , int clusterspecial ) { // 2007.03.08 // 014.06.23
	int i , j;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE->MODULE[i][j] = CLUSTER_RECIPE2->MODULE[i][j];
			if ( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] == NULL ) {
				strcpy( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j] , "" );
			}
			else {
				strncpy( CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j] , CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] , 64 );
			}
			if ( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] == NULL ) {
				strcpy( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] , "" );
			}
			else {
				strncpy( CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] , CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] , 64 );
			}
			if ( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] == NULL ) {
				strcpy( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j] , "" );
			}
			else {
				strncpy( CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j] , CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] , 64 );
			}
			//
			CLUSTER_RECIPE_ORG->MODULE[i][j] = CLUSTER_RECIPE->MODULE[i][j]; // 2008.01.11
			strcpy( CLUSTER_RECIPE_ORG->MODULE_PR_RECIPE[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j] ); // 2008.01.11
			strcpy( CLUSTER_RECIPE_ORG->MODULE_IN_RECIPE[i][j] , CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] ); // 2008.01.11
			strcpy( CLUSTER_RECIPE_ORG->MODULE_OUT_RECIPE[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j] ); // 2008.01.11
		}
	}
//	CLUSTER_RECIPE->EXTRA_STATION = CLUSTER_RECIPE2->EXTRA_STATION; // 2007.04.13

//	CLUSTER_RECIPE->EXTRA_STATION = CLUSTER_RECIPE2->EXTRA_STATION + ( lotspecial * 1000000 ); // 2007.04.13 // 2014.06.23
	//
	// 2014.06.23
	//
	if ( lotspecial > 0 ) {
		CLUSTER_RECIPE->EXTRA_STATION = CLUSTER_RECIPE2->EXTRA_STATION + ( (lotspecial-1) * 1000000 );
	}
	else {
		if ( clusterspecial > 0 ) {
			CLUSTER_RECIPE->EXTRA_STATION = CLUSTER_RECIPE2->EXTRA_STATION + ( (clusterspecial-1) * 1000000 );
		}
		else {
			CLUSTER_RECIPE->EXTRA_STATION = CLUSTER_RECIPE2->EXTRA_STATION + ( 0 * 1000000 );
		}
	}
	//
	CLUSTER_RECIPE->EXTRA_TIME    = CLUSTER_RECIPE2->EXTRA_TIME;
	CLUSTER_RECIPE->HANDLING_SIDE = CLUSTER_RECIPE2->HANDLING_SIDE;
	//
	CLUSTER_RECIPE_ORG->EXTRA_STATION = CLUSTER_RECIPE->EXTRA_STATION; // 2008.01.11
	CLUSTER_RECIPE_ORG->EXTRA_TIME    = CLUSTER_RECIPE->EXTRA_TIME; // 2008.01.11
	CLUSTER_RECIPE_ORG->HANDLING_SIDE = CLUSTER_RECIPE->HANDLING_SIDE; // 2008.01.11
}

int Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( CLUSTERStepTemplate *CLUSTER_RECIPE , CLUSTERStepTemplate *CLUSTER_RECIPE_ORG , CLUSTERStepTemplate2 *CLUSTER_RECIPE2 , int side , int pt ) { // 2007.03.08
	char ProcessMainRecipe[256];
	char ProcessPostRecipe[256];
	char ProcessPreRecipe[256];
	int i , j;
	short c;
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		//
		c = 0;
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			Get_ECR_Recipe_Data_From_Mixed_String( ProcessMainRecipe , ProcessPostRecipe , ProcessPreRecipe , 255 , CLUSTER_RECIPE->MODULE_IN_RECIPE[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE[i][j] , 64 );
			//
			//-------------------------------------------------------------------------------------------------
			// 2008.01.11
			//-------------------------------------------------------------------------------------------------
			if ( CLUSTER_RECIPE_ORG->MODULE[i][j] == CLUSTER_RECIPE->MODULE[i][j] ) {
				if ( STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_IN_RECIPE[i][j] , ProcessMainRecipe ) ) {
					if ( STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_OUT_RECIPE[i][j] , ProcessPostRecipe ) ) {
						if ( STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_PR_RECIPE[i][j] , ProcessPreRecipe ) ) {
							continue;
						}
					}
				}
			}
			//-------------------------------------------------------------------------------------------------
			c++;
			//-------------------------------------------------------------------------------------------------
			//
			CLUSTER_RECIPE2->MODULE[i][j] = CLUSTER_RECIPE->MODULE[i][j];
			//
			if ( strlen( ProcessPreRecipe ) <= 0 ) {
				if ( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] != NULL ) {
					free( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] );
					CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] == NULL ) {
					if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j]) , ProcessPreRecipe ) ) return -1;
				}
				else {
					if ( !STRCMP_L( ProcessPreRecipe , CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] ) ) {
						if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j]) , ProcessPreRecipe ) ) return -1;
					}
				}
			}
			//
			if ( strlen( ProcessMainRecipe ) <= 0 ) {
				if ( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] != NULL ) {
					free( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] );
					CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] == NULL ) {
					if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j]) , ProcessMainRecipe ) ) return -1;
				}
				else {
					if ( !STRCMP_L( ProcessMainRecipe , CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] ) ) {
						if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j]) , ProcessMainRecipe ) ) return -1;
					}
				}
			}
			//
			if ( strlen( ProcessPostRecipe ) <= 0 ) {
				if ( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] != NULL ) {
					free( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] );
					CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] = NULL;
				}
			}
			else {
				if ( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] == NULL ) {
					if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j]) , ProcessPostRecipe ) ) return -1;
				}
				else {
					if ( !STRCMP_L( ProcessPostRecipe , CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] ) ) {
						if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j]) , ProcessPostRecipe ) ) return -1;
					}
				}
			}
			//-------------------------------------------------------------------------------------------------
		}
		//-------------------------------------------------------------------------------------------------
		// 2008.01.11
		//-------------------------------------------------------------------------------------------------
		if ( c > 0 ) {
			if ( pt != -1 ) { // 2009.04.23
				_i_SCH_CLUSTER_Set_PathRun( side , pt , i , 1 , ( c > 9 ) ? 9 : c );
			}
		}
		//-------------------------------------------------------------------------------------------------
	}
//	CLUSTER_RECIPE2->EXTRA_STATION = CLUSTER_RECIPE->EXTRA_STATION; // 2007.04.14
//	CLUSTER_RECIPE2->EXTRA_STATION = CLUSTER_RECIPE->EXTRA_STATION % 1000000; // 2007.04.14 // 2008.11.03
	CLUSTER_RECIPE2->EXTRA_STATION = CLUSTER_RECIPE->EXTRA_STATION % 100000; // 2007.04.14 // 2008.11.03
	CLUSTER_RECIPE2->EXTRA_TIME    = CLUSTER_RECIPE->EXTRA_TIME;
	CLUSTER_RECIPE2->HANDLING_SIDE = CLUSTER_RECIPE->HANDLING_SIDE;
	return 1;
}

void Scheduler_CONTROL_CLUSTERStepTemplate12_Clear( int side ) { // 2014.02.05
	int i;
	for ( i = 0 ; i < USER_APPEND_RECIPE_COUNT[side] ; i++ ) {
		free( USER_APPEND_RECIPE[side][i]);
	}
	//
	USER_APPEND_RECIPE_COUNT[side] = 0;
	//
	for ( i = 0 ; i < MAX_USER_APPEND_RECIPE ; i++ ) {
		USER_APPEND_RECIPE[side][i] = NULL;
	}
	//
}

void Scheduler_CONTROL_CLUSTERStepTemplate12_AppendCheck( int side , CLUSTERStepTemplate *CLUSTER_RECIPE_ORG , CLUSTERStepTemplate2 *CLUSTER_RECIPE2 ) { // 2014.02.05
	int i , j , k , id;
	//
//	if ( SCHEDULER_GET_RECIPE_LOCKING( side ) != 2 ) return; // 2015.07.20
	if ( SCHEDULER_GET_RECIPE_LOCKING( side ) < 2 ) return; // 2015.07.20
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			//-------------------------------------------------------------------------------------------------
			if ( CLUSTER_RECIPE2->MODULE[i][j] <= 0 ) continue;
			//-------------------------------------------------------------------------------------------------
			if ( !STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_IN_RECIPE[i][j] , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] ) ) ) {
				//
				if ( STR_MEM_SIZE( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] ) > 0 ) {
					//
					if ( USER_APPEND_RECIPE_COUNT[side] >= MAX_USER_APPEND_RECIPE ) return;
					//
					id = ( j * 10000 ) + ( i * 10 ) + 0;
					//
					for ( k = 0 ; k < USER_APPEND_RECIPE_COUNT[side] ; k++ ) {
						if ( id == USER_APPEND_RECIPE_MODE[side][k] ) {
							if ( STRCMP_L( STR_MEM_GET_STR( USER_APPEND_RECIPE[side][k] ) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] ) ) ) {
								break;
							}
						}
					}
					//
					if ( k == USER_APPEND_RECIPE_COUNT[side] ) {
						USER_APPEND_RECIPE_MODE[side][USER_APPEND_RECIPE_COUNT[side]] = id;
						if ( STR_MEM_MAKE_COPY2( &(USER_APPEND_RECIPE[side][USER_APPEND_RECIPE_COUNT[side]]) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_IN_RECIPE2[i][j] ) ) ) USER_APPEND_RECIPE_COUNT[side]++;
					}
					//
				}
				//
			}
			if ( !STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_OUT_RECIPE[i][j] , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] ) ) ) {
				//
				if ( STR_MEM_SIZE( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] ) > 0 ) {
					//
					if ( USER_APPEND_RECIPE_COUNT[side] >= MAX_USER_APPEND_RECIPE ) return;
					//
					id = ( j * 10000 ) + ( i * 10 ) + 1;
					//
					for ( k = 0 ; k < USER_APPEND_RECIPE_COUNT[side] ; k++ ) {
						if ( id == USER_APPEND_RECIPE_MODE[side][k] ) {
							if ( STRCMP_L( STR_MEM_GET_STR( USER_APPEND_RECIPE[side][k] ) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] ) ) ) {
								break;
							}
						}
					}
					//
					if ( k == USER_APPEND_RECIPE_COUNT[side] ) {
						USER_APPEND_RECIPE_MODE[side][USER_APPEND_RECIPE_COUNT[side]] = id;
						if ( STR_MEM_MAKE_COPY2( &(USER_APPEND_RECIPE[side][USER_APPEND_RECIPE_COUNT[side]]) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_OUT_RECIPE2[i][j] ) ) ) USER_APPEND_RECIPE_COUNT[side]++;
					}
					//
				}
				//
			}
			if ( !STRCMP_L( CLUSTER_RECIPE_ORG->MODULE_PR_RECIPE[i][j] , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] ) ) ) {
				//
				if ( STR_MEM_SIZE( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] ) > 0 ) {
					//
					if ( USER_APPEND_RECIPE_COUNT[side] >= MAX_USER_APPEND_RECIPE ) return;
					//
					id = ( j * 10000 ) + ( i * 10 ) + 2;
					//
					for ( k = 0 ; k < USER_APPEND_RECIPE_COUNT[side] ; k++ ) {
						if ( id == USER_APPEND_RECIPE_MODE[side][k] ) {
							if ( STRCMP_L( STR_MEM_GET_STR( USER_APPEND_RECIPE[side][k] ) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] ) ) ) {
								break;
							}
						}
					}
					//
					if ( k == USER_APPEND_RECIPE_COUNT[side] ) {
						USER_APPEND_RECIPE_MODE[side][USER_APPEND_RECIPE_COUNT[side]] = id;
						if ( STR_MEM_MAKE_COPY2( &(USER_APPEND_RECIPE[side][USER_APPEND_RECIPE_COUNT[side]]) , STR_MEM_GET_STR( CLUSTER_RECIPE2->MODULE_PR_RECIPE2[i][j] ) ) ) USER_APPEND_RECIPE_COUNT[side]++;
					}
					//
				}
				//
			}
		}
	}
}

int Scheduler_CONTROL_CLUSTERStepTemplate12_AppendFind( int side , int mode , int depth , int pmindex , char *filename ) { // 2014.02.05
	int i;
	//
	switch ( SCHEDULER_GET_RECIPE_LOCKING( side ) ) {
	case 1 :
		return 1;
		break;
	case 2 :
		for ( i = 0 ; i < USER_APPEND_RECIPE_COUNT[side] ; i++ ) {
			//
			if ( ( ( pmindex * 10000 ) + ( depth * 10 ) + mode ) != USER_APPEND_RECIPE_MODE[side][i] ) continue;
			//
			if ( !STRCMP_L( STR_MEM_GET_STR( filename ) , STR_MEM_GET_STR( USER_APPEND_RECIPE[side][i] ) ) ) continue;
			//
			return 1;
			//
		}
		//
		return 2;
		//
		break;
	case 3 : // 2015.07.20
		for ( i = 0 ; i < USER_APPEND_RECIPE_COUNT[side] ; i++ ) {
			//
			if ( ( ( pmindex * 10000 ) + ( depth * 10 ) + mode ) != USER_APPEND_RECIPE_MODE[side][i] ) continue;
			//
			if ( !STRCMP_L( STR_MEM_GET_STR( filename ) , STR_MEM_GET_STR( USER_APPEND_RECIPE[side][i] ) ) ) continue;
			//
			return 1;
			//
		}
		//
		return 3;
		//
		break;
	}
	//
	return 0;
}

//void Scheduler_CONTROL_CLUSTERStepTemplate12_AppendView( int side ) { // 2014.02.05
//	int i;
//	//
//	for ( i = 0 ; i < USER_APPEND_RECIPE_COUNT[side] ; i++ ) {
//		//
//		printf( "[%d] [PM%d][mode=%d][depth=%d][%s]\n" , i , USER_APPEND_RECIPE_MODE[side][i] % 10000 , USER_APPEND_RECIPE_MODE[side][i] % 10 , ( USER_APPEND_RECIPE_MODE[side][i] % 10000 ) / 10 , STR_MEM_GET_STR( USER_APPEND_RECIPE[side][i] ) );
//		//
//	}
//}

/*
// 2007.07.06
void Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( int side , int pt , int ch , int trgmode , int *currmode , int *lrch , int mdlcnt , int progress , int *progress2 , BOOL intlksset ) {
	(*progress2)++;
	if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) {
		if ( ( trgmode == HANDLING_ALL_SIDE ) && ( *currmode == HANDLING_ALL_SIDE ) ) {
			if ( ( mdlcnt <= 1 ) || ( *lrch != ( ch ) ) ) {
				*lrch = ( ch );
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY )
					*currmode = HANDLING_A_SIDE_ONLY;
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY )
					*currmode = HANDLING_B_SIDE_ONLY;
				//========
				if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
				//========
			}
			else {
				(*progress2)--;
				_i_SCH_CLUSTER_Set_PathProcessData( side , pt , progress , *progress2 , 0 , "" , "" , "" );
			}
		}
		else {
			if (
				( ( *currmode == HANDLING_A_SIDE_ONLY ) && ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY ) ) ||
				( ( *currmode == HANDLING_B_SIDE_ONLY ) && ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY ) )
				) {
				(*progress2)--;
				_i_SCH_CLUSTER_Set_PathProcessData( side , pt , progress , *progress2 , 0 , "" , "" , "" );
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY )
					*currmode = HANDLING_A_SIDE_ONLY;
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY )
					*currmode = HANDLING_B_SIDE_ONLY;
				//========
				if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
				//========
			}
		}
	}
	else {
		if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
	}
}
*/



int SCHEDULER_CONTROL_Check_Duplicate_Use( int CHECKING_SIDE , int mode , BOOL PreChecking , char *RunAllRecipeName ) {
	//
	int i , j , k;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < CM1 ) continue; // 2011.04.14
		//
		if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= PM1 ) && ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < BM1 ) ) continue; // 2010.01.19
		//
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			//
			if ( ( j != CHECKING_SIDE ) && ( _i_SCH_SYSTEM_USING_GET( j ) > 4 ) ) {
				//
				for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( j , k ) < 0 ) continue; // 2006.03.03
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( j , k ) == SCHEDULER_CM_END ) continue; // 2011.04.20
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( j , k ) < CM1 ) continue; // 2011.04.14
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( j , k ) < PM1 ) { // 2010.11.03 // 2011.04.14
						//
						if ( mode == 0 ) {
							//
							if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathIn( j , k ) ) { // 2011.04.20
								if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotIn( j , k ) ) {
									//
									if ( _i_SCH_CLUSTER_Get_PathIn( j , k ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) { // 2011.04.20
										if ( _i_SCH_CLUSTER_Get_SlotIn( j , k ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) { // 2011.04.20
											//
											if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( j , k ) % 100 ) < _MOTAG_10_NEED_UPDATE ) { // 2012.04.06
												//
												if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Path/Slot 1(II) [%s:%d]\tRECIPEFAIL %s:%d,%d,%d,%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) , RunAllRecipeName , CHECKING_SIDE , i , j , k , ERROR_USECASS_SELECT ); // 2011.04.20
												return ERROR_USECASS_SELECT;
											}
											//
										}
									}
									//
								}
							} // 2011.04.20
							//
							if ( _i_SCH_CLUSTER_Get_PathOut( j , k ) >= CM1 ) { // 2011.04.14
								if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) {
									if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) {
										//
										if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( j , k ) % 100 ) < _MOTAG_10_NEED_UPDATE ) { // 2012.04.06
											//
											if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Path/Slot 2(IO) [%s:%d]\tRECIPEFAIL %s:%d,%d,%d,%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) , RunAllRecipeName , CHECKING_SIDE , i , j , k , ERROR_USECASS_SELECT ); // 2011.04.20
											return ERROR_USECASS_SELECT;
											//
										}
										//
									}
								}
							}
							//
							if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) >= CM1 ) { // 2011.04.14
								if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathIn( j , k ) ) {
									if ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotIn( j , k ) ) {
										//
										if ( _i_SCH_CLUSTER_Get_PathIn( j , k ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) { // 2011.04.20
											if ( _i_SCH_CLUSTER_Get_SlotIn( j , k ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) { // 2011.04.20
												//
												if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( j , k ) % 100 ) < _MOTAG_10_NEED_UPDATE ) { // 2012.04.06
													//
													if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Path/Slot 3(OI) [%s:%d]\tRECIPEFAIL %s:%d,%d,%d,%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) , RunAllRecipeName , CHECKING_SIDE , i , j , k , ERROR_USECASS_SELECT ); // 2011.04.20
													return ERROR_USECASS_SELECT;
													//
												}
												//
											}
										}
										//
									}
								}
								//
								if ( _i_SCH_CLUSTER_Get_PathOut( j , k ) >= CM1 ) { // 2011.04.14
									if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) {
										if ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) {
											//
											if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( j , k ) % 100 ) < _MOTAG_10_NEED_UPDATE ) { // 2012.04.06
												//
												if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Path/Slot 4(OO) [%s:%d]\tRECIPEFAIL %s:%d,%d,%d,%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) , RunAllRecipeName , CHECKING_SIDE , i , j , k , ERROR_USECASS_SELECT ); // 2011.04.20
												return ERROR_USECASS_SELECT;
												//
											}
											//
										}
									}
								}
								//
							}
							//
						}
					}
					else if ( _i_SCH_CLUSTER_Get_PathIn( j , k ) >= BM1 ) { // 2010.11.03
						//
						if ( mode != 0 ) {
							//
							if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathIn( j , k ) ) {
								if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotIn( j , k ) ) {
									if ( _i_SCH_CLUSTER_Get_PathStatus( j , k ) != SCHEDULER_CM_END ) { // 2010.11.03
										return ERROR_USECASS_SELECT;
									}
								}
							}
							//
							if ( _i_SCH_CLUSTER_Get_PathOut( j , k ) >= CM1 ) { // 2011.04.14
								if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) {
									if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) {
										if ( _i_SCH_CLUSTER_Get_PathStatus( j , k ) != SCHEDULER_CM_END ) { // 2010.11.03
											return ERROR_USECASS_SELECT;
										}
									}
								}
							}
							//
							if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) >= CM1 ) { // 2011.04.14
								if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathIn( j , k ) ) {
									if ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotIn( j , k ) ) {
										if ( _i_SCH_CLUSTER_Get_PathStatus( j , k ) != SCHEDULER_CM_END ) { // 2010.11.03
											return ERROR_USECASS_SELECT;
										}
									}
								}
								//
								if ( _i_SCH_CLUSTER_Get_PathOut( j , k ) >= CM1 ) { // 2011.04.14
									if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( j , k ) ) {
										if ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_SlotOut( j , k ) ) {
											if ( _i_SCH_CLUSTER_Get_PathStatus( j , k ) != SCHEDULER_CM_END ) { // 2010.11.03
												return ERROR_USECASS_SELECT;
											}
										}
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
	}
	//
	return ERROR_NONE;
}



void Scheduler_CONTROL_Set_PPID( int side , int pointer , BOOL grp , char *name ) { // 2011.09.01
	char buf[256];

	_i_SCH_CLUSTER_Set_Ex_PPID( side , pointer , name ); // 2012.09.13
	//
	if ( grp ) {
		if      ( STRCMP_L( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , "" ) ) {
			strcpy( buf , name );
		}
		else if ( STRCMP_L( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , "." ) ) {
			strcpy( buf , name );
		}
		else {
			sprintf( buf , "%s:(%s)" , name , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) );
		}
	}
	else {
		strcpy( buf , name );
	}
//	_i_SCH_CLUSTER_Set_Ex_PPID( side , pointer , buf ); // 2012.09.13
	_i_SCH_CLUSTER_Set_Ex_RecipeName( side , pointer , buf ); // 2012.09.13
	//--------------------------------------------------------------------------------------------------
}

int Scheduler_CONTROL_Get_Find_Free_Space( BOOL PreChecking , int side , int preappendcount , int mdl , int slarm , int mode ) { // 2011.09.28
	int i , pt = -1 , rmdl , rslarm , io_source , io_status , io_result , Res;
	//
	if ( SCH_RESTART_GET_IO_DATA( mdl , slarm , &io_source , &io_status , &io_result ) <= 0 ) return FALSE;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( i < preappendcount ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) {
			pt = i;
			break;
		}
	}
	//
	if ( pt == -1 ) {
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( side , "Restart Data Skipped 2(NoSpace) at %s:%d (%c%d,Mode=%d)\tRESTART 1:%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( mdl ) , slarm , io_source + 'A' , io_status , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slarm );
	}
	else {
		//
		Res = SCH_RESTART_GET_DATA_ALL_INFO( _i_SCH_SYSTEM_RESTART_GET( side ) , side , pt , mdl , slarm , io_source , io_status , &rmdl , &rslarm );
		//
		if ( Res == 0 ) {
			//
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( side , "Restart Data Setted to %s:%d (%c%d:%d,SEL=%s:%d,PT=%d,MODE=%d)\tRESTART 0:%s:%d%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( mdl ) , slarm , io_source + 'A' , io_status , io_result , _i_SCH_SYSTEM_GET_MODULE_NAME( rmdl ) , rslarm , pt , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slarm , 9 , io_status );
			//
			switch( io_result ) {
			case 0 :	SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , -2 );	break; // present
			case 1 :	SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , 0 );	break; // success;
			case 2 :	SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , 1 );	break; // fail
			case 3 :	SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , -2 );	break; // processing
			default :	SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , -2 );	break;
			}
			//
			if      ( mdl < PM1 ) {
				SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( side , pt , TRUE , -1 );
			}
			else if ( ( mdl >= PM1 ) && ( mdl < AL ) ) {
				if ( io_result == 0 ) Scheduler_Restart_Process_Set( side , pt , mdl ); /* Present */
			}
			//
			return TRUE;
		}
		else if ( Res == 99 ) { // 2012.07.20
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( side , "Restart Data Skipped 2(OtherSide) at %s:%d (%c%d,PT=%d,RES=%d,MODE=%d)\tRESTART 3:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( mdl ) , slarm , io_source + 'A' , io_status , pt , Res , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slarm );
			return TRUE;
		}
		else {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( side , "Restart Data Skipped 1(CanNotFind) at %s:%d (%c%d,PT=%d,RES=%d,MODE=%d)\tRESTART 2:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( mdl ) , slarm , io_source + 'A' , io_status , pt , Res , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slarm );
		}
		//
	}
	return FALSE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Recipe Handling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//int Scheduler_CONTROL_RECIPE_SETTING_Sub( CLUSTERStepTemplate2 *CLUSTER_RECIPE , int CHECKING_SIDE , char *RunAllRecipeName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int loopcount ) { // 2014.05.13
int Scheduler_CONTROL_RECIPE_SETTING_Sub( CLUSTERStepTemplate2 *CLUSTER_RECIPE , int CHECKING_SIDE , char *RunAllRecipeName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int *loopcount ) { // 2014.05.13
	int UserMode;
	int MX , Pt , Slot , RunWafer , xprogress , xprogress2 , single_chamber , postyes;
	LOTStepTemplate LOT_RECIPE;
	LOTStepTemplate1G LOT_RECIPE_GROUP; // 2009.09.28
	LOTStepTemplate2 LOT_RECIPE_DUMMY;
	LOTStepTemplate3 LOT_RECIPE_SPECIAL;
	//
	LOTStepTemplate_Dummy_Ex LOT_RECIPE_DUMMY_EX; // 2016.07.13
	//
//	CLUSTERStepTemplate CLUSTER_RECIPE_INF; // 2008.07.24
//	CLUSTERStepTemplate CLUSTER_RECIPE_INF_ORG; // 2008.01.11 // 2008.07.24
	//
	CLUSTERStepTemplate3 CLUSTER_RECIPE_SPECIAL;
	//
	char RunName[256];
	char DirName[256];
//	char InName[256]; // 2002.05.10
//	char OutName[256]; // 2002.05.10
//	char BufName[256]; // 2002.05.10

	char Selected_LotRecipe_Name[256]; // 2008.07.09

	char Per_Run1Name[256]; // 2004.09.24
	char Per_Run2Name[256]; // 2004.11.08
	char Per_Run3Name[256]; // 2004.11.08

	char ManualCJName[256]; // 2004.04.27
	char ManualPJName[256]; // 2004.04.27

	BOOL ManualmultiCJmode; // 2004.04.27
	int  ManualSelectCJIndex; // 2004.04.27

	int i , j , k , m , l , n , n2 , count; // , cm[MAX_CASSETTE_SLOT_SIZE];
	//
//	int cs[2][MAX_CASSETTE_SLOT_SIZE]; // 2011.05.21
//	int cs[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2011.03.10 // 2011.05.21 // 2011.08.01
	int cs[MAX_CHAMBER][MAX_CASSETTE_SLOT_SIZE]; // 2011.03.10 // 2011.05.21 // 2011.08.01
	int cm[MAX_CHAMBER]; // 2011.08.01
	//
	int restart_check[MAX_CHAMBER][MAX_CASSETTE_SLOT_SIZE+1]; // 2011.09.23

	int target_wafer_count;
	BOOL do_b , do_b2;
	int  AllDis = 0;
//	BOOL FindAnotherRun , FindAnotherRun2 , FindAnotherDummy; // 2017.07.03
	BOOL FindAnotherRun , FindAnotherDummy; // 2017.07.03
	int FindAnotherRun2; // 2017.07.03
	BOOL ch_buffer[ MAX_PM_CHAMBER_DEPTH ];
//	BOOL PM_MODE; // 2005.07.06
//	int  Clmode , T_Clmode , L_R_Chamber , M_Count; // 2007.07.06
	int  Clmode , M_Count; // 2007.07.06
	int  DUMMY_USE , dws;
	int  DUMMY_SKIP_USE;

	int G_lspdata , G_pmmodeop;

	int selgrp; // 2006.07.03
	int multigroupfind = FALSE; // 2006.07.03
//	int mdlareaflag[MAX_CHAMBER]; // 2008.01.18 // 2008.03.06
	//
	int preappendcount;
	int MaxSlot , maxunuse; // 2010.12.17
	int rcpcheck; // 2012.01.01

	int curres; // 2013.01.16
	int flock; // 2014.02.06
	//
	int RealMtl; // 2015.10.12
	int RealSlot; // 2015.10.12
	//
	int DllRes; // 2015.10.12
	int DllPathIn , DllPathOut; // 2015.10.12
	int DllSlotIn , DllSlotOut; // 2015.10.12
	//
	int StartIndex , EndIndex , RangeCheckFail; // 2015.10.12
	//
	BOOL hasSlotFix; // 2015.10.12
	BOOL mjoblotread; // 2015.10.12

	BOOL LOTMAPMODE = FALSE; // 2016.12.10
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Initialize
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//
	*TMATM = 0;

	strcpy( errorstring , "" ); // 2011.04.20

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	Scheduler_CONTROL_CLUSTERStepTemplate12_Clear( CHECKING_SIDE ); // 2014.02.05
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	SCHEDULER_Set_CLUSTER_INDEX_BUFFER_SELECT(); // 2008.06.11
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Initial Data Setting(User Mode for DLL Interface)
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

	G_lspdata = 0; // 2005.07.18
	//
	if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
		UserMode = USER_RECIPE_DATA_MODE();
//		PM_MODE  = USER_RECIPE_PM_MODE( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE ); // 2005.07.06
//		_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , USER_RECIPE_PM_MODE( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE ) ); // 2005.07.06 // 2005.07.18
		if ( ( UserMode < 0 ) || ( UserMode > 8 ) ) UserMode = 0;
	}
	else {
		UserMode = USER_RECIPE_MANUAL_DATA_MODE();
//		PM_MODE  = USER_RECIPE_MANUAL_PM_MODE( CHECKING_SIDE , CHECKING_SIDE ); // 2005.07.06
//		_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , USER_RECIPE_MANUAL_PM_MODE( CHECKING_SIDE , CHECKING_SIDE ) ); // 2005.07.06 // 2005.07.18
		if ( ( UserMode < 0 ) || ( UserMode > 8 ) ) UserMode = 0;
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Another Job Running Check
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	FindAnotherRun = FALSE;
//	FindAnotherRun2 = FALSE; // 2017.07.03
	FindAnotherRun2 = 0; // 2017.07.03
	FindAnotherDummy = FALSE;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != CHECKING_SIDE ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) >= 4 ) {
				FindAnotherRun = TRUE;
				if ( SYSTEM_DUMMY_GET( i ) ) FindAnotherDummy = TRUE;
//				if ( _i_SCH_SYSTEM_USING_GET( i ) >= 10 ) FindAnotherRun2 = TRUE; // 2005.07.29
//				if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) FindAnotherRun2 = TRUE;  // 2005.07.29 // 2017.06.26
//				if ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) FindAnotherRun2 = TRUE; // 2005.07.29 // 2017.06.26 // 2017.07.03
				//
				if ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) { // 2017.07.03
					if      ( FindAnotherRun2 != 2 ) {
						if ( SYSTEM_BEGIN_GET( i ) < 4 ) {
							FindAnotherRun2 = 2;
						}
						else {
							FindAnotherRun2 = 1;
						}
					}
				}
				//
			}
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Recipe Name Analysis for Group or UnGroup
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	rcpcheck = _SCH_COMMON_USER_RECIPE_CHECK( ( 11 + ( PreChecking * 1000 ) ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ); // 2012.01.12
	if ( ( rcpcheck ) && ( curres != 0 ) ) {
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(11) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
		return curres;
	}
	//
	if ( ( !rcpcheck ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.09.23
		//
		if ( CPJOB != 1 ) {
			STR_SEPERATE_CHAR( RunAllRecipeName , ':' , RunName , DirName , 255 );
			i = strlen( DirName );
			if ( i <= 0 ) {
				_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , "." );
			}
			else {
				if ( DirName[ i - 1 ] == ')' ) DirName[ i - 1 ] = 0;
				if ( DirName[ 0 ] == '(' ) {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName + 1 );
				}
				else {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName );
				}
			}
		}
		else {
			//=========================================================================
			// 2004.06.26
			//=========================================================================
			STR_SEPERATE_CHAR( RunAllRecipeName , ':' , RunName , DirName , 255 );
			i = strlen( DirName );
			if ( i <= 0 ) {
				strcpy( RunName , RunAllRecipeName );
				strcpy( DirName , SCHMULTI_DEFAULT_GROUP_GET() );
				i = strlen( DirName );
				if ( i <= 0 ) {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , "." );
				}
				else {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName );
				}
			}
			else {
				if ( DirName[ i - 1 ] == ')' ) DirName[ i - 1 ] = 0;
				if ( DirName[ 0 ] == '(' ) {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName + 1 );
				}
				else {
					_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName );
				}
			}
			//=========================================================================
	//		strcpy( RunName , RunAllRecipeName );
	//		strcpy( DirName , SCHMULTI_DEFAULT_GROUP_GET() );
	//		i = strlen( DirName );
	//		if ( i <= 0 ) {
	//			_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , "." );
	//		}
	//		else {
	//			_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE , DirName );
	//		}
			//=========================================================================
		}
		//
		FILE_PARAM_SM_SCHEDULER_SETTING2( 1 , CHECKING_SIDE ); // 2013.08.22
		//
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
	//	if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Run Recipe %s Read Start with(%d,%s,%s)\tRECIPESTART %d:%s:%s:%s\n" , RunAllRecipeName , CHECKING_SIDE , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) , CHECKING_SIDE , RunAllRecipeName , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ); / 2007.10.23
		if ( !PreChecking ) {
			//
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Run Recipe %s Read Start with(%d,%s,%s:%d)\tRECIPESTART %d:%s:%s:%s\n" , RunAllRecipeName , CHECKING_SIDE , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) , SYSTEM_RUN_TIME_GET() , CHECKING_SIDE , RunAllRecipeName , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ); // 2007.10.23
			//
			OPERATE_CONTROL( CHECKING_SIDE , 1 );
			//
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Data Reset
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		_i_SCH_SUB_Reset_Scheduler_Data( CHECKING_SIDE );
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 101 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(101) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Just Single Chamber find for Single Chamber Process Architecture
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		/*
		// 2012.02.01
		switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
		case CONTROL_RECIPE_LOT_PROCESS :
		case CONTROL_RECIPE_PROCESS :
			single_chamber = Get_RunPrm_MODULE_GET_SINGLE_CHAMBER();
	//		if ( single_chamber < 0 ) { // 2002.05.20
	//			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run is not single chamber mode%cRECIPEFAIL %s:%d\n" , 9 , RunName , ERROR_NOT_SINGLE_CHAMBER ); // 2002.05.20
	//			return ERROR_NOT_SINGLE_CHAMBER; // 2002.05.20
	//		} // 2002.05.20
			break;
		}
		*/
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Data Setting Part 1) Lot Recipe Read(if need)
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//====================================================================================================
		strcpy( Selected_LotRecipe_Name , "" ); // 2008.07.09
		//====================================================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2009.09.28
			//
			strcpy( LOT_RECIPE_GROUP.CLUSTER_GROUP[i] , "" );
			LOT_RECIPE_GROUP.CLUSTER_USING[i] = 0;
			//
			LOT_RECIPE_SPECIAL.USER_SLOTFIX_MODE[i] = 0; // 2015.10.12
			LOT_RECIPE_SPECIAL.USER_SLOTFIX_SLOT[i] = 0; // 2015.10.12
			//
		}
		//====================================================================================================
//		if ( ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) || ( RunAllRecipeName[0] != 0 ) ) { // 2010.10.12 // 2017.10.10
		if ( ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) || ( RunAllRecipeName[0] != 0 ) ) { // 2010.10.12 // 2017.10.10
			//
			switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
			case CONTROL_RECIPE_ALL :
			case CONTROL_RECIPE_LOT_PROCESS :
				//====================================================================================================
				// Lot Recipe Read
				//====================================================================================================
				if ( CPJOB == 1 ) {
/*
					//
					// 2015.10.12
					//
					if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , &i ) ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 1%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
						return ERROR_LOT_RECIPE_DATA;
					}
					//
					*/
					//
					/*
					// 2012.06.28
					if ( ( i == 1 ) || ( i == 3 ) ) { // 2003.09.24 // 2004.09.24
						j = 0;
						while( SCHMULTI_RECIPE_LOT_DATA_SINGLE_READ( CHECKING_SIDE , j , Per_Run3Name , ManualPJName , &l , &k ) ) { // 2004.09.24
							//============================================================================================================================
							//============================================================================================================================
							// 2004.11.08 (Need More Check)
							//============================================================================================================================
							//============================================================================================================================
							STR_SEPERATE_CHAR( Per_Run3Name , ':' , Per_Run1Name , Per_Run2Name , 255 );
							//============================================================================================================================
							//============================================================================================================================
							if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , i , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &G_lspdata , Per_Run1Name , l , ManualPJName ) ) { // 2004.09.24
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 2%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
								return ERROR_LOT_RECIPE_DATA;
							}
							if ( ( j == 0 ) && ( l == -1 ) ) break;
							j = k;
							//============================================================================================================================
						}
					}
					else if ( ( i == 2 ) || ( i == 4 ) ) { // 2003.09.24 // 2004.09.24 (Dummy Only)
						//====================================================================================================
						strcpy( Selected_LotRecipe_Name , RunName ); // 2008.07.09
						//====================================================================================================
						if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , i , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &G_lspdata , RunName , -1 , NULL ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 3%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
					*/
					// 2012.06.28
					//
					i = SCHMULTI_RECIPE_LOT_DATA_MODE( CHECKING_SIDE ); // 2015.10.12
					//
					if ( ( i == 1 ) || ( i == 2 ) ) {
						//
						// 2015.10.12
						//
						mjoblotread = 0; // 2015.10.12
						//
						if ( SCHMULTI_RECIPE_LOT_DATA_SINGLE_GET_NAME( CHECKING_SIDE , Per_Run3Name , ManualPJName ) ) { // 2015.10.12
							//
							//============================================================================================================================
							STR_SEPERATE_CHAR( Per_Run3Name , ':' , Per_Run1Name , Per_Run2Name , 255 );
							//============================================================================================================================
//							if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &G_lspdata , Per_Run1Name , -1 , ManualPJName , &hasSlotFix ) ) { // 2016.07.13
//							if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , Per_Run1Name , -1 , ManualPJName , &hasSlotFix ) ) { // 2016.07.13 // 2016.12.10
							if ( !RECIPE_FILE_LOT_DATA_READ( FALSE , NULL , CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , Per_Run1Name , -1 , ManualPJName , &hasSlotFix ) ) { // 2016.07.13 // 2016.12.10
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 2-1%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
								return ERROR_LOT_RECIPE_DATA;
							}
							//
							if ( hasSlotFix ) {
								//
								mjoblotread = 1;
								//
								SCHMULTI_RECIPE_LOT_DATA_REMAP( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_SPECIAL );
								//
							}
							//
						}
						//
						if ( mjoblotread == 0 ) { // 2015.10.12
							//
//							if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
							if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 1-1%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
								return ERROR_LOT_RECIPE_DATA;
							}
							//
							j = 0;
							//
							while( SCHMULTI_RECIPE_LOT_DATA_SINGLE_READ( CHECKING_SIDE , j , Per_Run3Name , ManualPJName , &l , &k ) ) { // 2004.09.24
								//============================================================================================================================
								//============================================================================================================================
								// 2004.11.08 (Need More Check)
								//============================================================================================================================
								//============================================================================================================================
								STR_SEPERATE_CHAR( Per_Run3Name , ':' , Per_Run1Name , Per_Run2Name , 255 );
								//============================================================================================================================
								//============================================================================================================================
//								if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , i , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &G_lspdata , Per_Run1Name , l , ManualPJName , &hasSlotFix ) ) { // 2004.09.24 // 2016.07.13
//								if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , i , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , Per_Run1Name , l , ManualPJName , &hasSlotFix ) ) { // 2004.09.24 // 2016.07.13 // 2016.12.10
								if ( !RECIPE_FILE_LOT_DATA_READ( FALSE , NULL , CHECKING_SIDE , i , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , Per_Run1Name , l , ManualPJName , &hasSlotFix ) ) { // 2004.09.24 // 2016.07.13 // 2016.12.10
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 2%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
									return ERROR_LOT_RECIPE_DATA;
								}
								//
								if ( ( j == 0 ) && ( l == -1 ) ) break;
								//
								j = k;
								//============================================================================================================================
							}
							//
						}
					}
					else {
						// 2015.10.12
						//
//						if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
						if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 1%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
						//
					}
					//
				}
				else {
					//====================================================================================================
					strcpy( Selected_LotRecipe_Name , RunName ); // 2008.07.09
					//====================================================================================================
					if ( ( UserMode == 0 ) || ( UserMode == 1 ) || ( UserMode == 2 ) || ( UserMode == 6 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
//						if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &G_lspdata , RunName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13
//						if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , RunName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
						if ( !RECIPE_FILE_LOT_DATA_READ( TRUE , &LOTMAPMODE , CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &G_lspdata , RunName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 4%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
				}
				if ( ( UserMode == 3 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 6 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
						if ( !USER_RECIPE_LOT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE , RunName ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 5%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
						if ( !USER_RECIPE_LOT_DUMMY_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE_DUMMY , RunName ) ) { // 2003.09.24
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 6%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
					else {
						if ( !USER_RECIPE_MANUAL_LOT_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE , RunName ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 7%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
						if ( !USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE_DUMMY , RunName ) ) { // 2003.09.24
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 8%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
				}
				break;
			case CONTROL_RECIPE_CLUSTER :
			case CONTROL_RECIPE_PROCESS :
				//====================================================================================================
				// Lot Recipe Set Default
				//====================================================================================================
				for ( Pt = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) {
					strcpy( LOT_RECIPE.CLUSTER_RECIPE[Pt] , RunName );
					strcpy( LOT_RECIPE.CLUSTER_RECIPE2[Pt] , "" );
					LOT_RECIPE.CLUSTER_USING[Pt] = 0;
				}
				strcpy( LOT_RECIPE_DUMMY.CLUSTER_RECIPE , "" );
				LOT_RECIPE_DUMMY.CLUSTER_USING = 0;
				//====================================================================================================
				// Lot Recipe Read // 2002.05.20
				//====================================================================================================
				if ( CPJOB == 1 ) {
//					if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , &i ) ) { // 2015.10.12
//					if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
					if ( !SCHMULTI_RECIPE_LOT_DATA_READ( CHECKING_SIDE , &LOT_RECIPE , &LOT_RECIPE_GROUP , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) ) ) { // 2015.10.12 // 2016.07.13
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 9%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
						return ERROR_LOT_RECIPE_DATA;
					}
				}
				if ( ( UserMode == 3 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 6 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
						if ( !USER_RECIPE_LOT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE , RunName ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 10%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
						if ( !USER_RECIPE_LOT_DUMMY_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE_DUMMY , RunName ) ) { // 2003.09.24
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 11%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
					else {
						if ( !USER_RECIPE_MANUAL_LOT_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE , RunName ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 12%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
						if ( !USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE_DUMMY , RunName ) ) { // 2003.09.24
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run Recipe %s Read Error 13%cRECIPEFAIL %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , ERROR_LOT_RECIPE_DATA );
							return ERROR_LOT_RECIPE_DATA;
						}
					}
				}
				break;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Data Setting Part PM Mode Setting
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( _SCH_COMMON_USER_RECIPE_CHECK( 16 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
			if ( curres != ERROR_NONE ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(16) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
				return curres;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 111 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(111) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}

		//------------------------------------------------------------------------------
		// 2005.07.18
		//------------------------------------------------------------------------------
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
			G_pmmodeop = USER_RECIPE_PM_MODE( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , RunAllRecipeName , RunName , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) );
		}
		else {
			G_pmmodeop = USER_RECIPE_MANUAL_PM_MODE( CHECKING_SIDE , CHECKING_SIDE , RunAllRecipeName , RunName , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) );
		}
		//
		if      ( G_pmmodeop == 0 ) {
			_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 0 );
		}
		else if ( G_pmmodeop == 1 ) {
			_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 1 );
		}
		else if ( G_pmmodeop == 2 ) {
			if ( G_lspdata == 0 ) {
				_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 0 );
			}
			else {
				_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 1 );
			}
		}
		else { // 2005.09.15
			if ( G_lspdata >= ( G_pmmodeop - 1 ) ) {
				_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 1 );
			}
			else {
				_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 0 );
			}
		}
		//------------------------------------------------------------------------------
		// 2012.02.01
		//------------------------------------------------------------------------------
		switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
		case CONTROL_RECIPE_LOT_PROCESS :
		case CONTROL_RECIPE_PROCESS :
			//
			single_chamber = Get_RunPrm_MODULE_GET_SINGLE_CHAMBER( _i_SCH_SYSTEM_PMMODE_GET(CHECKING_SIDE) );
			//
			if ( single_chamber < 0 ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Run is not single chamber mode%cRECIPEFAIL %s:%d\n" , 9 , RunName , ERROR_NOT_SINGLE_CHAMBER );
				return ERROR_NOT_SINGLE_CHAMBER;
			}
			//
			break;
		}
		//------------------------------------------------------------------------------
		if ( _SCH_COMMON_USER_RECIPE_CHECK( 21 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
			if ( curres != ERROR_NONE ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(21) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
				return curres;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 112 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(112) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Data Setting Part 2.1) Cluster Recipe Read(if need) for Dummy Process
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
	//	DM_NOT_USE ,
	//	DM_DEFAULT ,
	//	DM_OPTION1 ,
	//	DM_OPTION2 ,
	//	DM_OPTION3 ,
	//	DM_DEFAULT_DMFIX ,
	//	DM_OPTION1_DMFIX ,
	//	DM_OPTION2_DMFIX ,
	//	DM_OPTION3_DMFIX ,
	//
	//  LOT | DWafer| Result             | DmFix
	//
	// Default
	//------+-------+--------------------+-------------------
	//    O |    O  | Run(Dummy)         | Run(Dummy)
	//------+-------+--------------------+-------------------
	//    X |    X  | Run(Normal)        | Run(Normal)(DSkip)
	//------+-------+--------------------+-------------------
	//    O |    X  | Run(Normal)        | Run(Normal)(DSkip)
	//------+-------+--------------------+-------------------
	//    X |    O  | Run(Normal)(DSkip) | Run(Normal)(DSkip)
	//---
	// Option1
	//------+-------+--------------------+-------------------
	//    O |    X  | Run(Normal)        | Run(Normal)(DSkip)
	//    X |    O  | RunX               | RunX
	//---
	// Option2
	//------+-------+--------------------+-------------------
	//    O |    X  | RunX               | RunX
	//    X |    O  | Run(Normal)(DSkip) | Run(Normal)(DSkip)
	//---
	// Option3
	//------+-------+--------------------+-------------------
	//    O |    X  | RunX               | RunX
	//    X |    O  | RunX               | RunX

	//	L_R_Chamber    = -1; // 2007.07.06
		DUMMY_USE      = 0;
		DUMMY_SKIP_USE = 0;
		//
		preappendcount = 0; // 2010.10.12
		//
	//	Pt = 0; // 2010.05.08
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
		if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
			//
			if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
				//
				preappendcount = USER_RECIPE_DATA_PRE_APPEND( CHECKING_SIDE , 0 ); // 2010.05.08
				//
				if ( preappendcount <= 0 ) Pt = 0; // 2010.05.08
				else                       Pt = preappendcount; // 2010.05.08
				//
				if (
					( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) ||
					( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) ) {
					DUMMY_USE      = 0;
					DUMMY_SKIP_USE = 0;
				}
				else if ( FindAnotherRun && ( _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() == 0 ) ) { // 2003.02.12
					DUMMY_USE = 0;
					if ( FindAnotherDummy )	DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
					else 					DUMMY_SKIP_USE = 0;
				}
				else {
					if ( !_i_SCH_MODULE_GET_USE_ENABLE( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , FALSE , CHECKING_SIDE ) ) {
						DUMMY_USE = 0;
						DUMMY_SKIP_USE = 0;
					}
					else {
						dws = FALSE;
						if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ) > 0 ) { // 2003.02.13
							dws = TRUE; // 2003.02.13
						} // 2003.02.13
						else { // 2003.02.13
							if ( _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() == 1 ) {
								if ( FindAnotherRun && FindAnotherDummy ) { // 2003.02.13
									dws = TRUE; // 2003.02.13
								} // 2003.02.13
								else { // 2003.02.13
									dws = FALSE; // 2003.02.13
								} // 2003.02.13
							} // 2003.02.13
							else if ( _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() == 2 ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ) > 0 ) { // 2003.02.13
									dws = TRUE; // 2003.02.13
								} // 2003.02.13
								else { // 2003.02.13
									dws = FALSE; // 2003.02.13
								} // 2003.02.13
							} // 2003.02.13
						} // 2003.02.13
						if (
							( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
							( dws )
							) {
							DUMMY_USE      = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
							DUMMY_SKIP_USE = 0;
						}
						else if (
							( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
							( !dws )
							) {
							DUMMY_USE      = 0;
							if ( FindAnotherRun && FindAnotherDummy ) { // 2003.02.12
								DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER(); // 2003.02.12
							} // 2003.02.12
							else { // 2003.02.12
								switch( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() ) {
								case DM_DEFAULT :
								case DM_OPTION1 :
								case DM_OPTION2 :
								case DM_OPTION3 :
									DUMMY_SKIP_USE = 0;
									break;
								case DM_DEFAULT_DMFIX :
								case DM_OPTION1_DMFIX :
								case DM_OPTION2_DMFIX :
								case DM_OPTION3_DMFIX :
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
									break;
								}
							}
						}
						else {
							switch( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() ) {
							case DM_DEFAULT :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = 0;
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
								}
								break;
							case DM_DEFAULT_DMFIX :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
								}
								break;
							case DM_OPTION1 :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = 0;
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Dummy Chamber has a Wafer at lot not select%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
									strcpy( errorstring , "Dummy" ); // 2007.12.05
									return ERROR_CHAMBER_HAS_WAFER;
								}
								break;
							case DM_OPTION1_DMFIX :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Dummy Chamber has a Wafer at lot not select%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
									strcpy( errorstring , "Dummy" ); // 2007.12.05
									return ERROR_CHAMBER_HAS_WAFER;
								}
								break;
							case DM_OPTION2 :
							case DM_OPTION2_DMFIX :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Dummy Chamber has not a Wafer at lot select%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_NOT_WAFER );
									strcpy( errorstring , "Dummy" ); // 2007.12.05
									return ERROR_CHAMBER_HAS_NOT_WAFER;
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									DUMMY_USE      = 0;
									DUMMY_SKIP_USE = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
								}
								break;
							case DM_OPTION3 :
							case DM_OPTION3_DMFIX :
								if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING != 0 ) &&
									( !dws ) ) {
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Dummy Chamber has not a Wafer at lot select%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_NOT_WAFER );
									strcpy( errorstring , "Dummy" ); // 2007.12.05
									return ERROR_CHAMBER_HAS_NOT_WAFER;
								}
								else if ( ( LOT_RECIPE_DUMMY.CLUSTER_USING == 0 ) &&
									( dws ) ) {
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Dummy Chamber has a Wafer at lot not select%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
									strcpy( errorstring , "Dummy" ); // 2007.12.05
									return ERROR_CHAMBER_HAS_WAFER;
								}
								break;
							}
						}
					}
				}
			}
		}

		MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , CHECKING_SIDE + 1 , CHECKING_SIDE + 1 , 1 , &maxunuse ); // 2010.12.17

	//	if ( ( DUMMY_USE != 0 ) || ( DUMMY_SKIP_USE != 0 ) ) // 2003.02.13
		if ( DUMMY_USE != 0 ) // 2003.02.13
			SYSTEM_DUMMY_SET( CHECKING_SIDE , TRUE );
		else
			SYSTEM_DUMMY_SET( CHECKING_SIDE , FALSE );
	//	if ( !FindAnotherRun ) { // 2003.02.13
	//		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.13
	//			SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( i+PM1 , FALSE ); // 2003.02.13
	//		} // 2003.02.13
	//	} // 2003.02.13
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.13
			SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , i+PM1 , FALSE ); // 2003.02.13
			_i_SCH_MODULE_Set_Use_Interlock_Run( CHECKING_SIDE , i+PM1 , FALSE ); // 2003.02.13
		} // 2003.02.13
		//=======================================================================================================================================
		if ( DUMMY_USE != 0 ) {
			_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE  , Pt , MaxSlot + 1 ); // 2010.12.17
			_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , MaxSlot + 1 ); // 2010.12.17
			_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE  , Pt , _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() );
			_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , 0 ); // 2004.11.16
			_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , "" ); // 2005.01.24
			_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , 0 ); // 2014.06.23
			_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , "" ); // 2005.01.24
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , "" ); // 2007.06.01
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL ); // 2007.02.21
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , Pt , -1 , NULL ); // 2014.01.28
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , 0 ); // 2011.12.08
			_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , NULL ); // 2011.12.08
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , Pt , 0 ); // 2012.04.14
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , Pt , 0 ); // 2015.10.12
			//-----------------------------------------------------------------------
			switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
			case CONTROL_RECIPE_ALL :
			case CONTROL_RECIPE_CLUSTER :
				//====================================================================================================
				// Cluster Recipe Read
				//====================================================================================================
				if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , NULL , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , LOT_RECIPE_DUMMY.CLUSTER_RECIPE , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &selgrp , &multigroupfind , NULL ) ) {
					_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 14%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY.CLUSTER_RECIPE , 9 , LOT_RECIPE_DUMMY.CLUSTER_RECIPE , ERROR_CLST_RECIPE_DATA );
					return ERROR_CLST_RECIPE_DATA;
				}
				//--------------------------------------------------------------------------------------------------
				SCHEDULER_Set_MULTI_RECIPE_SELECT( selgrp ); // 2006.07.03
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA <= 0 ) ? 0 : ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA - 1 ) ); // 2014.06.23
				//
				_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA ); // 2005.01.24
				if ( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA ); // 2007.06.01
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA ); // 2007.06.01
				if ( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA ); // 2007.06.01
				//--------------------------------------------------------------------------------------------------
				if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) { // 2008.10.31
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
					}
				}
				else {
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) ); // 2006.02.06
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
					}
				}
				//-----------------------------------------------------------------------
				//--------------------------------------------------------------------------------------------------
				//---------------------------------
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE > 0 ) { // 2011.12.08
					//
					_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE );
					_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
					//
				}
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
				//---------------------------------
				//--------------------------------------------------------------------------------------------------
				//-----------------------------------------------------------------------
				// Append 2001.12.17
	//			for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) { // 2003.08.09
	//				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) { // 2003.08.09
	//					if ( CLUSTER_RECIPE.MODULE[i][j] && !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.08.09
	//						CLUSTER_RECIPE.MODULE[i][j] = 0; // 2003.08.09
	//					} // 2003.08.09
	//				} // 2003.08.09
	//			} // 2003.08.09
				//---------------------------------
	//			T_Clmode = CLUSTER_RECIPE->HANDLING_SIDE; // 2007.07.06
				Clmode   = CLUSTER_RECIPE->HANDLING_SIDE;
				//====================================================================================================
				// Buffer Item Count Initialize
				//====================================================================================================
				xprogress = 0;
				xprogress2 = 0;

				if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() == 0 ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() == 1 ) ) { // 2002.06.12
					//====================================================================================================
					// Pre Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
					for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
								ch_buffer[ j ] = TRUE;
								i = MAX_CLUSTER_DEPTH;
							}
						}
					}
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//
					//======================================================================================================
					// 2005.10.16
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					//
					do_b = FALSE;
	//				for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
	//					if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
	//					if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
		//					STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , ':' , InName , BufName , 255 ); // 2002.05.10
		//					STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , ':' , OutName , BufName , 255 ); // 2002.05.10
		//					do_b2 = TRUE; // 2002.05.10
		//					if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , OutName , "" ); // 2002.05.10
		//					else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , "" , "" ); // 2002.05.10
		//					else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , OutName , "" ); // 2002.05.10
		//					else do_b2 = FALSE; // 2002.05.10
		//					if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , InName ); // 2002.05.10
		//					} // 2002.05.10
							do_b2 = TRUE; // 2002.05.10
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
							else do_b2 = FALSE; // 2002.05.10
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
							} // 2002.05.10
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
	//							Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , TRUE ); // 2007.07.06
								xprogress2++; // 2007.07.06
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE ); // 2007.07.06
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
				}
				//====================================================================================================
				// Main Cluster File Append
				//====================================================================================================
				for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
					do_b = FALSE;
	//				for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) M_Count++; // 2007.07.06
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
							//===================================================================================================================
							// 2007.01.29
							//===================================================================================================================
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
							}
							//===================================================================================================================
							do_b = TRUE;
							//==========================
	//						Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , TRUE ); // 2007.07.06
							xprogress2++; // 2007.07.06
							SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE ); // 2007.07.06
							//==========================
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0; xprogress++;
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
	//						ch_buffer[ j ] = CLUSTER_RECIPE.MODULE[i][j]; // 2007.02.21
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) ch_buffer[ j ] = TRUE; // 2007.02.21
							else                                    ch_buffer[ j ] = FALSE; // 2007.02.21
						}
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
				}
				if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() == 0 ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() == 2 ) ) { // 2002.06.12
					//====================================================================================================
					// Post Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//======================================================================================================
					// 2005.10.16
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					do_b = FALSE;
	//				for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
	//					if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
	//					if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
		//					STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , ':' , InName , BufName , 255 ); // 2002.05.10
		//					STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , ':' , OutName , BufName , 255 ); // 2002.05.10
		//					do_b2 = TRUE; // 2002.05.10
		//					if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , OutName , "" ); // 2002.05.10
		//					else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , "" , "" ); // 2002.05.10
		//					else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , OutName , "" ); // 2002.05.10
		//					else do_b2 = FALSE; // 2002.05.10
		//					if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
		//						_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , InName ); // 2002.05.10
		//					} // 2002.05.10
							do_b2 = TRUE; // 2002.05.10
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
							else do_b2 = FALSE; // 2002.05.10
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
							} // 2002.05.10
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
	//							Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , TRUE ); // 2007.07.06
								xprogress2++; // 2007.07.06
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE ); // 2007.07.06
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
				}
				//====================================================================================================
				// Total Item Count Setting
				//====================================================================================================
	//			if ( Clmode == HANDLING_ALL_SIDE ) Clmode = HANDLING_A_SIDE_ONLY; // 2007.07.06
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , xprogress );
				_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
				//
				Scheduler_CONTROL_Set_PPID( CHECKING_SIDE , Pt , TRUE , LOT_RECIPE_DUMMY.CLUSTER_RECIPE ); // 2011.09.01
				//
				//====================================================================================================
				// Scheduler Item Increase
				//====================================================================================================
				Pt++;
				break;
			}
		}

		//=======================================================================================================================================
		//
		// 2016.07.13 // 2016.08.25
		//
		// PRE
		//
		for ( n = 0 ; n < MAX_CASSETTE_SLOT_SIZE; n++ ) {
			//
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 0 ) continue;
			//
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 1 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 2 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 3 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 4 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 5 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 6 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 7 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 8 ) continue;
			//
			DllPathIn  = 0;
			DllPathOut = 0;
			//
			DllSlotIn =  n + _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1;
			DllSlotOut = n + _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1;
			//
			// call dll
			//
			if ( !USER_RECIPE_LOT_SLOTFIX_SETTING( CHECKING_SIDE , Pt , PreChecking , -991 , &DllPathIn , &DllSlotIn , &DllPathOut , &DllSlotOut ) ) {
				//
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
				//
				if ( !PreChecking ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-19%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_SLOTFIX_FAIL1 );
				}
				//
				return ERROR_SLOTFIX_FAIL1;
				//
			}
			//
			DllRes = 0;
			//
			if ( ( DllSlotIn <= 0 ) || ( DllSlotIn > MAX_CASSETTE_SLOT_SIZE ) ) {
				DllRes = 1;
			}
			else {
				//
				if ( DllPathIn < CM1 ) {
					DllRes = 2;
				}
				else if ( DllPathIn < PM1 ) {
					//
				}
				else if ( DllPathIn >= BM1 ) {
					//
					if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathIn , FALSE , -1 ) ) DllRes = 5;
					//
					if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathIn , DllSlotIn ) <= 0 ) DllRes = 6;
					//

				}
				else {
					DllRes = 7;
				}
				//
			}
			//
			if ( DllRes == 0 ) {
				//
				if ( DllPathOut == 0 ) DllPathOut = DllPathIn;
				//
				if ( DllSlotOut <= 0 ) {
					DllSlotOut = DllSlotIn;
				}
				else {
					if ( ( DllSlotOut <= 0 ) || ( DllSlotOut > MAX_CASSETTE_SLOT_SIZE ) ) DllRes = 11;
				}
				//
				if ( DllRes == 0 ) {
					//
					if ( ( DllPathOut != DllPathIn ) || ( DllSlotOut != DllSlotIn ) ) {
						//
						if ( DllPathOut < CM1 ) {
							DllRes = 12;
						}
						else if ( DllPathOut < PM1 ) {
							//
						}
						else if ( DllPathOut >= BM1 ) {
							//
							if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathOut , FALSE , -1 ) ) DllRes = 15;
							//
							if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathOut , DllSlotOut ) > 0 ) DllRes = 16;
							//
						}
						else {
							DllRes = 17;
						}
						//
					}
				}
			}
			//
			if ( DllRes != 0 ) {
				//
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
				//
				if ( !PreChecking ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-20[%d]%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , DllRes , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_SLOTFIX_FAIL2 );
				}
				return ERROR_SLOTFIX_FAIL2;
			}
			//
			//================================================================
			_i_SCH_CASSETTE_LAST_RESET( DllPathIn , DllSlotIn );
			//
			_i_SCH_CASSETTE_LAST_RESET2( CHECKING_SIDE , Pt );
			//================================================================
			_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , DllSlotIn );
			_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , DllSlotOut );
			_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , DllPathIn );
			_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , DllPathOut );
			//
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , "" );
			_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , "" );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , "" );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , Pt , -1 , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
			case CONTROL_RECIPE_ALL :
			case CONTROL_RECIPE_CLUSTER :
				//====================================================================================================
				// Cluster Recipe Read
				//====================================================================================================
				if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , NULL , -1 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &selgrp , &multigroupfind , NULL ) ) {
					_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 14%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_CLST_RECIPE_DATA );
					return ERROR_CLST_RECIPE_DATA;
				}
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA <= 0 ) ? 0 : ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA - 1 ) );
				//
				_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
				if ( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
				if ( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
				//--------------------------------------------------------------------------------------------------
				if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) {
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
					}
				}
				else {
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 );
					}
				}
				//--------------------------------------------------------------------------------------------------
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE > 0 ) {
					//
					_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE );
					_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
					//
				}
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
				//--------------------------------------------------------------------------------------------------
				Clmode   = CLUSTER_RECIPE->HANDLING_SIDE;
				//====================================================================================================
				// Buffer Item Count Initialize
				//====================================================================================================
				xprogress = 0;
				xprogress2 = 0;
				//
				switch( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] ) {
					//
				case 2 :
				case 4 :
				case 6 :
				case 8 :
				case 10 :
				case 12 :
				case 14 :
				case 16 :
					//====================================================================================================
					// Pre Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
					for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
								ch_buffer[ j ] = TRUE;
								i = MAX_CLUSTER_DEPTH;
							}
						}
					}
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					//
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) {
							do_b2 = TRUE;
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , "" , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else do_b2 = FALSE;
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) );
							}
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
								xprogress2++;
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
					//
					break;
					//
				}
				//====================================================================================================
				// Main Cluster File Append
				//====================================================================================================
				for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
							//===================================================================================================================
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
							}
							//===================================================================================================================
							do_b = TRUE;
							//==========================
							xprogress2++;
							SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
							//==========================
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0; xprogress++;
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) ch_buffer[ j ] = TRUE;
							else                                    ch_buffer[ j ] = FALSE;
						}
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
				}
				//
				//
				switch( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] ) {
					//
				case 3 :
				case 4 :
				case 7 :
				case 8 :
				case 11 :
				case 12 :
				case 15 :
				case 16 :
					//====================================================================================================
					// Post Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) {
							do_b2 = TRUE;
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , "" , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else do_b2 = FALSE;
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) );
							}
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
								xprogress2++;
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
					//
					break;
					//
				}
				//====================================================================================================
				// Total Item Count Setting
				//====================================================================================================
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , xprogress );
				_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
				//
				Scheduler_CONTROL_Set_PPID( CHECKING_SIDE , Pt , TRUE , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] );
				//
				//====================================================================================================
				// Scheduler Item Increase
				//====================================================================================================
				Pt++;
				break;
			}
		}
		//
		// 2016.07.13
		//
		//=======================================================================================================================================



		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Data Setting Part 2.2) Cluster Recipe Read(if need)
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------

		if ( _SCH_COMMON_USER_RECIPE_CHECK( 26 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
			if ( curres != ERROR_NONE ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(26) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
				return curres;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 121 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(121) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}
		//=======================================================================================================================================
		// 2009.03.06
		//=======================================================================================================================================
//		if ( ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) || ( RunAllRecipeName[0] != 0 ) ) { // 2010.10.12 // 2017.10.10
		if ( ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) || ( RunAllRecipeName[0] != 0 ) ) { // 2010.10.12 // 2017.10.10
			//
			if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
				//------------------------
				Clmode = -1;
				//------------------------
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					if (
						( LOT_RECIPE.CLUSTER_USING[i] == 0 ) &&
						( i >= (StartSlot-1) ) &&
						( i < EndSlot )
						) {
						if ( Clmode == -1 ) {
							l = _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i );
							Clmode = i + 1;
						}
						else {
							if ( Clmode > ( i + 1 ) ) {
								if ( l == _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) ) {
									Clmode = i + 1;
								}
							}
						}
					}
				}
				//------------------------
				if ( Clmode != -1 ) {
					j = ( ( Clmode - 1 ) % ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) );
					if ( j != 0 ) {
						//
						k = ( ( ( Clmode - 1 ) / ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) ) * ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
						if ( k < 0 ) k = 0;
						//=============================================================================
						// 2009.05.06
						//=============================================================================
						for ( j = k + 1 ; j < StartSlot ; j++ ) {
							if ( LOT_RECIPE.CLUSTER_USING[j-1] == 0 ) LOT_RECIPE.CLUSTER_USING[j-1] = 1;
						}
						//=============================================================================
						StartSlot = k + 1;
						//=============================================================================
					}
				}
			}
			//
			//------------------------------------------------------------------------------
			//
			// 2015.10.12
			//
			StartIndex = _i_SCH_IO_GET_START_INDEX( CHECKING_SIDE );
			EndIndex   = _i_SCH_IO_GET_END_INDEX( CHECKING_SIDE );
			//
			//
			//------------------------------------------------------------------------------
			//
		//	for ( RunWafer = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) { // 2003.09.29
			for ( RunWafer = 0 , MX = 0 ; MX < MAX_CASSETTE_SLOT_SIZE ; MX++ ) { // 2003.09.29
				//---------------------------------------------------------------------------------------
				for ( Slot = -1 , i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//			if ( _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , i ) != -999999 ) { // 2007.07.11
					if ( _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , i ) > -999999 ) { // 2007.07.11
						if ( Slot == -1 ) {
							Slot = i;
						}
						else {
							if ( _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , i ) < _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , Slot ) ) {
								Slot = i;
							}
						}
					}
				}
				//---------------------------------------------------------------------------------------
				if ( Slot == -1 ) continue; // 2003.09.29
				//---------------------------------------------------------------------------------------
				//-------------------------------------
				// Mapping to Scheduler data from user select data(Cass In/Out,Slot Start/End)
				//-------------------------------------
		//		_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , Slot , -999999 ); // 2007.07.11
				if ( _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , Slot ) != -999999 ) {
					if      ( _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , Slot ) < 0 ) {
						_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , Slot , -20000000 + _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , Slot ) );
					}
					else {
						_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , Slot , -10000000 - _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , Slot ) );
					}
				}
				//
				// 2015.10.12
				//
				RealMtl = LOT_RECIPE_SPECIAL.USER_SLOTFIX_MODE[Slot];
				//
				if ( RealMtl > 0 ) {
					RealSlot = LOT_RECIPE_SPECIAL.USER_SLOTFIX_SLOT[Slot] - 1;
				}
				else {
					RealSlot = Slot;
				}
				//
				//-------------------------------------
//				if ( // 2015.10.12
//					( LOT_RECIPE.CLUSTER_USING[Slot] != 0 ) || // 2015.10.12
//					( Slot < (StartSlot-1) ) || // 2015.10.12
//					( Slot >= EndSlot ) // 2015.10.12
//					) // 2015.10.12
//				) { // 2015.10.12
				//-------------------------------------
				//
				// 2015.10.12
				//
				RangeCheckFail = 0;
				//
				if ( LOT_RECIPE.CLUSTER_USING[Slot] != 0 ) {
					RangeCheckFail = 1;
				}
				else {
					//
					if ( ( Slot < (StartIndex-1) ) || ( ( EndIndex > 0 ) && ( Slot >= EndIndex ) ) ) {
						RangeCheckFail = 2;
					}
					else {
						//
						if ( ( RealMtl <= 0 ) && ( ( RealSlot < (StartSlot-1) ) || ( RealSlot >= EndSlot ) ) ) RangeCheckFail = 3;
						//
					}
					//
				}
				//
				//
//_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[CHECK] Slot=%d,StartIndex=%d,EndIndex=%d,Pt=%d,RealMtl=%d,RealSlot=%d,RangeCheckFail=%d\n" ,
//					  Slot ,
//					  StartIndex , 
//					  EndIndex , 
//					  Pt , 
//					  RealMtl , 
//					  RealSlot , 
//					  RangeCheckFail );
				//
				//
				if ( RangeCheckFail != 0 ) { // 2015.10.12
					//
					//-------------------------------------
					// Unselected Slot Cancel
					//-------------------------------------
		//			_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 ); // 2003.09.29
		//			_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , -1 ); // 2003.09.29
		//			_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , -1 ); // 2003.09.29
		//			_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , -1 ); // 2003.09.29
		//			_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , -1 ); // 2003.09.29
					//================================================================================================
					// 2006.08.04
					//================================================================================================
					/*
					//
					// 2015.10.12
					//
					if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
						if ( ( Slot >= (StartSlot-1) ) && ( Slot < EndSlot ) ) {
							_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , Slot + 1 );
		//					_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_SlotOut( CHECKING_SIDE , Slot ) ); // 2007.05.18
		//					_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , Slot ) ); // 2007.05.18 // 2007.07.09
//							_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , Slot + 1 ); // 2007.05.18 // 2007.07.09
							_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot ) ); // 2015.10.12
		//					_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , Slot ) ); // 2007.07.09
							_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot ) ); // 2007.07.09 // 2015.10.12
							//-----------------------------------------------------------------------
					*/
					//
					// 2015.10.12
					//
					if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
						if ( ( RealSlot >= (StartSlot-1) ) && ( RealSlot < EndSlot ) ) { // 2015.10.12
							_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , RealSlot + 1 ); // 2015.10.12
							_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , RealSlot + 1 ); // 2007.05.18 // 2007.07.09 // 2015.10.12
							_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot ) ); // 2015.10.12
							_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot ) ); // 2007.07.09 // 2015.10.12
							//-----------------------------------------------------------------------



//							_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2014.06.23
							if ( LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] <= 0 ) {
								_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , 0 );
							}
							else {
								_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.SPECIAL_USER_DATA[Slot]-1 );
							}
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , 0 );
							_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , "" );
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , "" ); // 2007.06.01
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL ); // 2007.02.21
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , Pt , -1 , NULL ); // 2014.01.28
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 );
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , 0 );
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.USER_CONTROL_MODE[Slot] ); // 2011.12.08
							_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.USER_CONTROL_DATA[Slot] ); // 2011.12.08
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , Pt , 0 ); // 2012.04.14
							//-----------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , Pt , 0 ); // 2015.10.12
							//-----------------------------------------------------------------------
							Pt++;
							//-----------------------------------------------------------------------
						}
					}
					//================================================================================================
				}
				else {
					//-------------------------------------
					Scheduler_CONTROL_CLUSTERStepTemplate2_Free( CLUSTER_RECIPE ); // 2007.03.07
					//-------------------------------------
					// Selected Slot Regist
					//-------------------------------------
					if ( RealMtl > 0 ) { // 2015.10.12
						//
						if ( RealMtl == 1 ) {
							DllPathIn  = _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot );
							DllPathOut = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , Slot );
						}
						else {
							DllPathIn  = 0;
							DllPathOut = 0;
						}
						//
						DllSlotIn =  RealSlot + 1;
						DllSlotOut = RealSlot + 1;
						//
						// call dll
						//
						if ( !USER_RECIPE_LOT_SLOTFIX_SETTING( CHECKING_SIDE , Pt , PreChecking , RealMtl-1 , &DllPathIn , &DllSlotIn , &DllPathOut , &DllSlotOut ) ) {
							//
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
							//
							if ( !PreChecking ) {
								if ( LOT_RECIPE_GROUP.CLUSTER_USING[Slot] == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-9%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_SLOTFIX_FAIL1 );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s:(%s) Read Error 1-9%cRECIPEFAIL %s(%s):%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , ERROR_SLOTFIX_FAIL1 );
								}
							}
							//
							return ERROR_SLOTFIX_FAIL1;
							//
						}
						//
						DllRes = 0;
						//
						if ( ( DllSlotIn <= 0 ) || ( DllSlotIn > MAX_CASSETTE_SLOT_SIZE ) ) {
							DllRes = 1;
						}
						else {
							//
							if ( DllPathIn < CM1 ) {
								DllRes = 2;
							}
							else if ( DllPathIn < PM1 ) {
								//
//								if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathIn , FALSE , -1 ) ) DllRes = 3; // 2015.08.25
								//
//								if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathIn , DllSlotIn ) != CWM_PRESENT ) DllRes = 4; // 2015.08.25
								//
							}
							else if ( DllPathIn >= BM1 ) {
								//
								if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathIn , FALSE , -1 ) ) DllRes = 5;
								//
								if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathIn , DllSlotIn ) <= 0 ) DllRes = 6;
								//

							}
							else {
								DllRes = 7;
							}
							//
						}
						//
						if ( DllRes == 0 ) {
							//
							if ( DllPathOut == 0 ) DllPathOut = DllPathIn;
							//
							if ( DllSlotOut <= 0 ) {
								DllSlotOut = DllSlotIn;
							}
							else {
								if ( ( DllSlotOut <= 0 ) || ( DllSlotOut > MAX_CASSETTE_SLOT_SIZE ) ) DllRes = 11;
							}
							//
							if ( DllRes == 0 ) {
								//
								if ( ( DllPathOut != DllPathIn ) || ( DllSlotOut != DllSlotIn ) ) {
									//
									if ( DllPathOut < CM1 ) {
										DllRes = 12;
									}
									else if ( DllPathOut < PM1 ) {
										//
//										if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathOut , FALSE , -1 ) ) DllRes = 13; // 2015.08.25
										//
//										if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathOut , DllSlotOut ) != CWM_ABSENT ) DllRes = 14; // 2015.08.25
										//
									}
									else if ( DllPathOut >= BM1 ) {
										//
										if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathOut , FALSE , -1 ) ) DllRes = 15;
										//
										if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathOut , DllSlotOut ) > 0 ) DllRes = 16;
										//
									}
									else {
										DllRes = 17;
									}
									//
								}
							}
						}
						//
						if ( DllRes != 0 ) {
							//
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
							//
							if ( !PreChecking ) {
								if ( LOT_RECIPE_GROUP.CLUSTER_USING[Slot] == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-10[%d]%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , DllRes , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_SLOTFIX_FAIL2 );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s:(%s) Read Error 1-10[%d]%cRECIPEFAIL %s(%s):%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , DllRes , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , ERROR_SLOTFIX_FAIL2 );
								}
							}
							return ERROR_SLOTFIX_FAIL2;
						}
						//
						//================================================================
						_i_SCH_CASSETTE_LAST_RESET( DllPathIn , DllSlotIn );
						//
						_i_SCH_CASSETTE_LAST_RESET2( CHECKING_SIDE , Pt );
						//================================================================
						_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , DllSlotIn );
						_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , DllSlotOut );
						_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , DllPathIn );
						_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , DllPathOut );
						//
					}
					else {
						_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , Slot + 1 );
						_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_SlotOut( CHECKING_SIDE , Slot ) );
						_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , Slot ) );
						_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , Slot ) );
					}
					//-----------------------------------------------------------------------
//					_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2004.11.16
					//
					if ( LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] <= 0 ) {
						_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , 0 ); // 2004.11.16
					}
					else {
						_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot]-1 ); // 2004.11.16
					}
					//
					_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.SPECIAL_USER_DATA[Slot] ); // 2005.01.24
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , 0 ); // 2014.06.23
					_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , "" ); // 2005.01.24
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , "" ); // 2007.06.01
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL ); // 2007.02.21
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , Pt , -1 , NULL ); // 2014.01.28
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.USER_CONTROL_MODE[Slot] ); // 2011.12.08
					_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , LOT_RECIPE_SPECIAL.USER_CONTROL_DATA[Slot] ); // 2011.12.08
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , Pt , 0 ); // 2012.04.14
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , Pt , ( RealMtl > 1 ) ? 1 : 0 ); // 2015.10.12
					//-----------------------------------------------------------------------
					switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
					case CONTROL_RECIPE_ALL :
					case CONTROL_RECIPE_CLUSTER :
						//====================================================================================================
						// Cluster Recipe Read
						//====================================================================================================
						if ( ( UserMode == 0 ) || ( UserMode == 2 ) || ( UserMode == 3 ) || ( UserMode == 5 ) || ( UserMode == 6 ) || ( UserMode == 8 ) ) {
							//
							if ( LOTMAPMODE ) { // 2016.12.10
								//
								if ( !RECIPE_FILE_CLUSTER_DATA_READ_FOR_MAP( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , Slot + 1 , &selgrp , &multigroupfind ) ) {
									//
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									//
									if ( !PreChecking ) {
										_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									}
									return ERROR_CLST_RECIPE_DATA;
								}
								//
							}
							else {
			//					if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , NULL , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &selgrp , &multigroupfind ) ) { // 2009.09.28
								if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , ( LOT_RECIPE_GROUP.CLUSTER_USING[Slot] == 0 ) ? NULL : LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &selgrp , &multigroupfind , SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( CPJOB , CHECKING_SIDE , Pt ) ) ) { // 2009.09.28
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) {
										if ( LOT_RECIPE_GROUP.CLUSTER_USING[Slot] == 0 ) {
											_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
										}
										else { // 2009.09.28
											_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s:(%s) Read Error 1%cRECIPEFAIL %s(%s):%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE_GROUP.CLUSTER_GROUP[Slot] , ERROR_CLST_RECIPE_DATA );
										}
									}
									return ERROR_CLST_RECIPE_DATA;
								}
							}
							//--------------------------------------------------------------------------------------------------
							SCHEDULER_Set_MULTI_RECIPE_SELECT( selgrp ); // 2006.07.03
							//--------------------------------------------------------------------------------------------------
							//
							_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA <= 0 ) ? 0 : ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA - 1 ) ); // 2014.06.23
							//
							//--------------------------------------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA ); // 2005.01.24
							if ( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA ); // 2007.06.01
							//--------------------------------------------------------------------------------------------------
							//--------------------------------------------------------------------------------------------------
							_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA ); // 2007.06.01
							if ( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA ); // 2007.06.01
							//--------------------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------
							//--------------------------------------------------------------------------------------------------
							//---------------------------------
							if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE > 0 ) { // 2011.12.08
								//
								_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE );
								_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
								//
							}
							//
							if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
							//---------------------------------
							//--------------------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------
						}
						if ( ( UserMode == 1 ) || ( UserMode == 2 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
							if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
//								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2007.03.08 // 2014.06.23
								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2007.03.08 // 2014.06.23
								if ( !USER_RECIPE_CLUSTER_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 2%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt ) ) { // 2007.03.08
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 2-1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								// 2012.06.18
								if ( !USER_RECIPE_CLUSTER_DATA_READ2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , CLUSTER_RECIPE , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 2-2%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								//
								Scheduler_CONTROL_CLUSTERStepTemplate12_AppendCheck( CHECKING_SIDE , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE ); // 2014.02.05
								//
							}
							else {
//								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2007.03.08 // 2014.06.23
								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2007.03.08 // 2014.06.23
								if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 3%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt ) ) { // 2007.03.08
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 3-1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								// 2012.06.18
								if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ2( CHECKING_SIDE , CHECKING_SIDE , CLUSTER_RECIPE , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 3-2%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								//
								Scheduler_CONTROL_CLUSTERStepTemplate12_AppendCheck( CHECKING_SIDE , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE ); // 2014.02.05
								//
							}
						}
						//-----------------------------------------------------------------------
//printf( "[A:%d:%d:%d:%s:%d]" , CHECKING_SIDE , Pt , Slot , Selected_LotRecipe_Name , PreChecking );

						_i_SCH_CLUSTER_Set_ClusterIndex( CHECKING_SIDE , Pt , SCHEDULER_Make_CLUSTER_INDEX_SELECT( CHECKING_SIDE , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , Pt ) , Slot + 1 , LOTMAPMODE ? RECIPE_FILE_GET_MAP_CLUSTERINDEX( Slot + 1 ) : 0 , Selected_LotRecipe_Name , LOT_RECIPE.CLUSTER_RECIPE[Slot] , CLUSTER_RECIPE , PreChecking ) ); // 2006.07.04
						//-----------------------------------------------------------------------
						if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) { // 2008.10.31
							if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
							}
							else {
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
							}
						}
						else {
							if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) ); // 2006.02.06
							}
							else {
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
							}
						}
						//-----------------------------------------------------------------------
						//---------------------------------
						// Append 2001.12.17
		//				for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) { // 2003.08.09
		//					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) { // 2003.08.09
		//						if ( CLUSTER_RECIPE.MODULE[i][j] && !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.08.09
		//							CLUSTER_RECIPE.MODULE[i][j] = 0; // 2003.08.09
		//						} // 2003.08.09
		//					} // 2003.08.09
		//				} // 2003.08.09
						//---------------------------------
		//				T_Clmode = CLUSTER_RECIPE->HANDLING_SIDE; // 2007.07.06
						Clmode   = CLUSTER_RECIPE->HANDLING_SIDE;
						//====================================================================================================
						// Buffer Item Count Initialize
						//====================================================================================================
						RunWafer++;
						xprogress = 0;
						xprogress2 = 0;
						//====================================================================================================
						// Pre Fixed Cluster Append from user edit parameter
						//====================================================================================================
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
							//
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
										ch_buffer[ j ] = TRUE;
										i = MAX_CLUSTER_DEPTH;
									}
								}
							}
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
								switch( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) ) {
								case 0 :
									ch_buffer[ i ] = FALSE;
									break;
								case 1 :
									if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
									else                  ch_buffer[ i ] = TRUE;
									break;
								case 2 :
									ch_buffer[ i ] = TRUE;
									break;
								}
							}
							//======================================================================================================
							// 2005.10.16
							//======================================================================================================
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( ch_buffer[ k ] ) {
									M_Count = 0;
									for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
										for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
											if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
												if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
													i = MAX_CLUSTER_DEPTH;
													j = MAX_PM_CHAMBER_DEPTH;
													M_Count = 1;
													break;
												}
											}
										}
									}
									if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
								}
							}
							//======================================================================================================
							do_b = FALSE;
			//				for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//					if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
			//					if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
								if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
			//						STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , ':' , InName , BufName , 255 ); // 2002.05.10
			//						STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , ':' , OutName , BufName , 255 ); // 2002.05.10
			//						do_b2 = TRUE; // 2002.05.10
			//						if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , OutName , "" ); // 2002.05.10
			//						else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , "" , "" ); // 2002.05.10
			//						else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , OutName , "" ); // 2002.05.10
			//						else do_b2 = FALSE; // 2002.05.10
			//						if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , InName ); // 2002.05.10
			//						} // 2002.05.10
									do_b2 = TRUE; // 2002.05.10
									if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else do_b2 = FALSE; // 2002.05.10
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
									} // 2002.05.10
									if ( do_b2 ) {
										do_b = TRUE;
										//==========================
			//							Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
										xprogress2++; // 2007.07.06
										//==========================
									}
								}
							}
							if ( xprogress2 > 0 ) {
								xprogress2 = 0;
								xprogress++;
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Main Cluster File Append
						//====================================================================================================
						for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
							do_b = FALSE;
		//					for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
								if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
									//
									if      ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 1 ) { // 2013.09.03
										//
										if ( _i_SCH_MODULE_GET_MOVE_MODE( j + PM1 ) != 0 ) continue;
										//
										_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 2 );
										//
									}
									else if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 2 ) { // 2013.09.03
										//
										if ( _i_SCH_MODULE_GET_MOVE_MODE( j + PM1 ) != 1 ) continue;
										//
										_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 2 );
										//
									}
									//===================================================================================================================
									// 2007.01.29
									//===================================================================================================================

//_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "?" , "" , "" ); // TEST


									_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
									//
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
									}
									//===================================================================================================================
									do_b = TRUE;
									//==========================
		//							Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
									xprogress2++; // 2007.07.06
									//==========================
									if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 0 ) { // 2013.09.03
										break;
									}
									//==========================
								}
							}
							if ( xprogress2 > 0 ) {
								//============================================================================
								Scheduler_CONTROL_Parallel_ReSetting( CHECKING_SIDE , Pt , xprogress , i , CLUSTER_RECIPE ); // 2007.02.21
								Scheduler_CONTROL_Parallel_ReSetting2( CHECKING_SIDE , Pt , xprogress , i , CLUSTER_RECIPE ); // 2014.01.28
								//============================================================================
								xprogress2 = 0; xprogress++;
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		//							ch_buffer[ j ] = CLUSTER_RECIPE.MODULE[i][j]; // 2007.02.21
									if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) ch_buffer[ j ] = TRUE; // 2007.02.21
									else                                    ch_buffer[ j ] = FALSE; // 2007.02.21
								}
								//
								if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 0 ) { // 2013.09.03
									break;
								}
								//
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Post Fixed Cluster Append from user edit parameter
						//====================================================================================================
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
							//
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
								switch( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) ) {
								case 0 :
									ch_buffer[ i ] = FALSE;
									break;
								case 1 :
									if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
									else                  ch_buffer[ i ] = TRUE;
									break;
								case 2 :
									ch_buffer[ i ] = TRUE;
									break;
								}
							}
							//======================================================================================================
							// 2005.10.16
							//======================================================================================================
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( ch_buffer[ k ] ) {
									M_Count = 0;
									for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
										for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
											if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
												if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
													i = MAX_CLUSTER_DEPTH;
													j = MAX_PM_CHAMBER_DEPTH;
													M_Count = 1;
													break;
												}
											}
										}
									}
									if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
								}
							}
							//======================================================================================================
							do_b = FALSE;
			//				for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//					if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
			//					if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
								if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
			//						STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , ':' , InName , BufName , 255 ); // 2002.05.10
			//						STR_SEPERATE_CHAR( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , ':' , OutName , BufName , 255 ); // 2002.05.10
			//						do_b2 = TRUE; // 2002.05.10
			//						if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , OutName , "" ); // 2002.05.10
			//						else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , InName , "" , "" ); // 2002.05.10
			//						else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , OutName , "" ); // 2002.05.10
			//						else do_b2 = FALSE; // 2002.05.10
			//						if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
			//							_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , InName ); // 2002.05.10
			//						} // 2002.05.10
									do_b2 = TRUE; // 2002.05.10
									if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else do_b2 = FALSE; // 2002.05.10
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
									} // 2002.05.10
									if ( do_b2 ) {
										do_b = TRUE;
										//==========================
			//							Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
										xprogress2++; // 2007.07.06
										//==========================
									}
								}
							}
							if ( xprogress2 > 0 ) {
								xprogress2 = 0;
								xprogress++;
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Total Item Count Setting
						//====================================================================================================
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , xprogress );
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
						break;

					case CONTROL_RECIPE_LOT_PROCESS :
					case CONTROL_RECIPE_PROCESS :
						//====================================================================================================
						// Cluster Recipe Read // 2002.05.20
						//====================================================================================================
						CLUSTER_RECIPE->EXTRA_TIME    = LOT_RECIPE.CLUSTER_EXTRA_TIME; // 2011.03.29
						CLUSTER_RECIPE->EXTRA_STATION = LOT_RECIPE.CLUSTER_EXTRA_STATION; // 2011.03.29
						CLUSTER_RECIPE->HANDLING_SIDE = 0; // 2011.03.29
						//
						// 2011.03.29
						//
//						if ( single_chamber >= PM1 ) { // 2012.02.01
						if ( ( single_chamber >= PM1 ) && ( single_chamber < AL ) ) { // 2012.02.01
							CLUSTER_RECIPE->MODULE[0][single_chamber-PM1] = 1;
							if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE->MODULE_IN_RECIPE2[0][single_chamber-PM1]) , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
								_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-3%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
								return ERROR_CLST_RECIPE_DATA;
							}
							if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE->MODULE_OUT_RECIPE2[0][single_chamber-PM1]) , LOT_RECIPE.CLUSTER_RECIPE2[Slot] ) ) {
								_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-4%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
								return ERROR_CLST_RECIPE_DATA;
							}
						}
						//
						if ( ( UserMode == 1 ) || ( UserMode == 2 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
							//
	/*
	//
	2011.03.29
							if ( single_chamber >= PM1 ) {
								CLUSTER_RECIPE->MODULE[0][single_chamber-PM1] = 1;
								if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE->MODULE_IN_RECIPE2[0][single_chamber-PM1]) , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-3%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								if ( !STR_MEM_MAKE_COPY( &(CLUSTER_RECIPE->MODULE_OUT_RECIPE2[0][single_chamber-PM1]) , LOT_RECIPE.CLUSTER_RECIPE2[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-4%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
							}
	*/
							//
							if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
//								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2007.03.08 // 2014.06.23
								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2007.03.08 // 2014.06.23
								if ( !USER_RECIPE_CLUSTER_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt ) ) { // 2007.03.08
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								// 2012.06.18
								if ( !USER_RECIPE_CLUSTER_DATA_READ2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , CLUSTER_RECIPE , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 4-2%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								//
								Scheduler_CONTROL_CLUSTERStepTemplate12_AppendCheck( CHECKING_SIDE , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE ); // 2014.02.05
								//
							}
							else {
//								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] ); // 2007.03.08 // 2014.06.23
								Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , LOT_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA[Slot] , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2007.03.08 // 2014.06.23
								if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 5%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt ) ) { // 2007.03.08
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 5-1%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								// 2012.06.18
								if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ2( CHECKING_SIDE , CHECKING_SIDE , CLUSTER_RECIPE , Slot + 1 , LOT_RECIPE.CLUSTER_RECIPE[Slot] ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 5-2%cRECIPEFAIL %s:%d\n" , LOT_RECIPE.CLUSTER_RECIPE[Slot] , 9 , LOT_RECIPE.CLUSTER_RECIPE[Slot] , ERROR_CLST_RECIPE_DATA );
									return ERROR_CLST_RECIPE_DATA;
								}
								//
								Scheduler_CONTROL_CLUSTERStepTemplate12_AppendCheck( CHECKING_SIDE , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE ); // 2014.02.05
								//
							}
						} // 2011.03.29
						//====================================================================================================
						//-----------------------------------------------------------------------
						_i_SCH_CLUSTER_Set_ClusterIndex( CHECKING_SIDE , Pt , SCHEDULER_Make_CLUSTER_INDEX_SELECT( CHECKING_SIDE , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , Pt ) , Slot + 1 , 0 , Selected_LotRecipe_Name , LOT_RECIPE.CLUSTER_RECIPE[Slot] , CLUSTER_RECIPE , PreChecking ) ); // 2011.03.29
						//-----------------------------------------------------------------------
						if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) { // 2008.10.31
							if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
							}
							else {
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
							}
						}
						else {
							if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) ); // 2006.02.06
							}
							else {
								_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
							}
						}
						//-----------------------------------------------------------------------
						//====================================================================================================
						Clmode   = CLUSTER_RECIPE->HANDLING_SIDE;
						//====================================================================================================
						// Buffer Item Count Initialize
						//====================================================================================================
						RunWafer++;
						xprogress = 0;
						xprogress2 = 0;
						//====================================================================================================
						// Pre Fixed Cluster Append from user edit parameter
						//====================================================================================================
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
							//
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
										ch_buffer[ j ] = TRUE;
										i = MAX_CLUSTER_DEPTH;
									}
								}
							}
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
								switch( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) ) {
								case 0 :
									ch_buffer[ i ] = FALSE;
									break;
								case 1 :
									if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
									else                  ch_buffer[ i ] = TRUE;
									break;
								case 2 :
									ch_buffer[ i ] = TRUE;
									break;
								}
							}
							//======================================================================================================
							// 2005.10.16
							//======================================================================================================
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( ch_buffer[ k ] ) {
									M_Count = 0;
									for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
										for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
											if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
												if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
													i = MAX_CLUSTER_DEPTH;
													j = MAX_PM_CHAMBER_DEPTH;
													M_Count = 1;
													break;
												}
											}
										}
									}
									if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
								}
							}
							//======================================================================================================
							do_b = FALSE;
		//					for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		//						if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
		//						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
								if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
									do_b2 = TRUE; // 2002.05.10
									if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else do_b2 = FALSE; // 2002.05.10
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
									} // 2002.05.10
									if ( do_b2 ) {
										do_b = TRUE;
										//==========================
		//								Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
										xprogress2++; // 2007.07.06
										//==========================
									}
								}
							}
							if ( xprogress2 > 0 ) {
								xprogress2 = 0;
								xprogress++;
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Main Cluster File Append
						//====================================================================================================
						for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
							do_b = FALSE;
	//						for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
								if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
									//
									if      ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 1 ) { // 2013.09.03
										//
										if ( _i_SCH_MODULE_GET_MOVE_MODE( j + PM1 ) != 0 ) continue;
										//
										_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 2 );
										//
									}
									else if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 2 ) { // 2013.09.03
										//
										if ( _i_SCH_MODULE_GET_MOVE_MODE( j + PM1 ) != 1 ) continue;
										//
										_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 2 );
										//
									}
									//===================================================================================================================
									// 2007.01.29
									//===================================================================================================================
									_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
									}
									//===================================================================================================================
									do_b = TRUE;
									//==========================
	//								Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
									xprogress2++; // 2007.07.06
									//==========================
									if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 0 ) { // 2013.09.03
										break;
									}
									//==========================
								}
							}
							if ( xprogress2 > 0 ) {
								//============================================================================
								Scheduler_CONTROL_Parallel_ReSetting( CHECKING_SIDE , Pt , xprogress , i , CLUSTER_RECIPE ); // 2007.02.21
								Scheduler_CONTROL_Parallel_ReSetting2( CHECKING_SIDE , Pt , xprogress , i , CLUSTER_RECIPE ); // 2014.01.28
								//============================================================================
								xprogress2 = 0; xprogress++;
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
	//								ch_buffer[ j ] = CLUSTER_RECIPE.MODULE[i][j]; // 2007.02.21
									if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) ch_buffer[ j ] = TRUE; // 2007.02.21
									else                                    ch_buffer[ j ] = FALSE; // 2007.02.21
								}
								//
								if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 0 ) { // 2013.09.03
									break;
								}
								//
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Post Fixed Cluster Append from user edit parameter
						//====================================================================================================
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
							//
							for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
								switch( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) ) {
								case 0 :
									ch_buffer[ i ] = FALSE;
									break;
								case 1 :
									if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
									else                  ch_buffer[ i ] = TRUE;
									break;
								case 2 :
									ch_buffer[ i ] = TRUE;
									break;
								}
							}
							//======================================================================================================
							// 2005.10.16
							//======================================================================================================
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( ch_buffer[ k ] ) {
									M_Count = 0;
									for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
										for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
											if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
												if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
													i = MAX_CLUSTER_DEPTH;
													j = MAX_PM_CHAMBER_DEPTH;
													M_Count = 1;
													break;
												}
											}
										}
									}
									if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
								}
							}
							//======================================================================================================
							do_b = FALSE;
		//					for ( M_Count = 0 , j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) if ( ch_buffer[ j ] ) M_Count++; // 2007.07.06
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		//						if ( ch_buffer[ j ] && _i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE ) ) { // 2003.10.09
		//						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
								if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
									do_b2 = TRUE; // 2002.05.10
									if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , "" , "" ); // 2002.05.10
									else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) ) // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" ); // 2002.05.10
									else do_b2 = FALSE; // 2002.05.10
									if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) { // 2002.05.10
										_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) ); // 2002.05.10
									} // 2002.05.10
									if ( do_b2 ) {
										do_b = TRUE;
										//==========================
		//								Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( CHECKING_SIDE , Pt , j+PM1 , T_Clmode , &Clmode , &L_R_Chamber , M_Count , xprogress , &xprogress2 , FALSE ); // 2007.07.06
										xprogress2++; // 2007.07.06
										//==========================
									}
								}
							}
							if ( xprogress2 > 0 ) {
								xprogress2 = 0;
								xprogress++;
							}
							else {
								if ( do_b ) return ERROR_PM_SIDE_ERROR;
							}
						}
						//====================================================================================================
						// Total Item Count Setting
						//====================================================================================================
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , xprogress );
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );

	/*
	// 2011.03.29
						}
						else { // 2002.05.20
							//
							RunWafer++;
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , 0 , 0 , single_chamber , LOT_RECIPE.CLUSTER_RECIPE[Slot] , LOT_RECIPE.CLUSTER_RECIPE2[Slot] , "" );
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( single_chamber ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , 0 , 0 , LOT_RECIPE.CLUSTER_RECIPE[Slot] );
							}
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , 1 );
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , 0 );
						}
	*/
						//
						break;
					}
					//-----------------------------------------------------------------------------
					Scheduler_CONTROL_Set_PPID( CHECKING_SIDE , Pt , TRUE , LOT_RECIPE.CLUSTER_RECIPE[Slot] ); // 2011.09.01
					//-----------------------------------------------------------------------------
					Pt++; // 2003.09.29
					//-----------------------------------------------------------------------------
				}
			}
			//------------------------------------------------------------------------------
			//------------------------------------------------------------------------------
			// Possible Wafer Check
			//------------------------------------------------------------------------------
			//------------------------------------------------------------------------------
			if ( RunWafer == 0 ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Lot Recipe has no valid data(%d-%d)%cRECIPEFAIL %s:%d\n" , StartSlot , EndSlot , 9 , RunAllRecipeName , ERROR_RECIPE_HAS_NO_WAFER );
				return ERROR_RECIPE_HAS_NO_WAFER;
			}
		}
		//------------------------------------------------------------------------------
		if ( _SCH_COMMON_USER_RECIPE_CHECK( 31 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
			if ( curres != ERROR_NONE ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(31) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
				return curres;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 201 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(201) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}









		//=======================================================================================================================================
		//
		// 2016.08.25
		//
		// POST
		//
		for ( n = 0 ; n < MAX_CASSETTE_SLOT_SIZE; n++ ) {
			//
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 0 ) continue;
			//
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 9 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 10 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 11 ) continue;
			if ( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] == 12 ) continue;
			//
			DllPathIn  = 0;
			DllPathOut = 0;
			//
			DllSlotIn =  n + _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1;
			DllSlotOut = n + _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1;
			//
			// call dll
			//
			if ( !USER_RECIPE_LOT_SLOTFIX_SETTING( CHECKING_SIDE , Pt , PreChecking , -992 , &DllPathIn , &DllSlotIn , &DllPathOut , &DllSlotOut ) ) {
				//
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
				//
				if ( !PreChecking ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-19%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_SLOTFIX_FAIL1 );
				}
				//
				return ERROR_SLOTFIX_FAIL1;
				//
			}
			//
			DllRes = 0;
			//
			if ( ( DllSlotIn <= 0 ) || ( DllSlotIn > MAX_CASSETTE_SLOT_SIZE ) ) {
				DllRes = 1;
			}
			else {
				//
				if ( DllPathIn < CM1 ) {
					DllRes = 2;
				}
				else if ( DllPathIn < PM1 ) {
					//
				}
				else if ( DllPathIn >= BM1 ) {
					//
					if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathIn , FALSE , -1 ) ) DllRes = 5;
					//
					if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathIn , DllSlotIn ) <= 0 ) DllRes = 6;
					//

				}
				else {
					DllRes = 7;
				}
				//
			}
			//
			if ( DllRes == 0 ) {
				//
				if ( DllPathOut == 0 ) DllPathOut = DllPathIn;
				//
				if ( DllSlotOut <= 0 ) {
					DllSlotOut = DllSlotIn;
				}
				else {
					if ( ( DllSlotOut <= 0 ) || ( DllSlotOut > MAX_CASSETTE_SLOT_SIZE ) ) DllRes = 11;
				}
				//
				if ( DllRes == 0 ) {
					//
					if ( ( DllPathOut != DllPathIn ) || ( DllSlotOut != DllSlotIn ) ) {
						//
						if ( DllPathOut < CM1 ) {
							DllRes = 12;
						}
						else if ( DllPathOut < PM1 ) {
							//
						}
						else if ( DllPathOut >= BM1 ) {
							//
							if ( !_i_SCH_MODULE_GET_USE_ENABLE( DllPathOut , FALSE , -1 ) ) DllRes = 15;
							//
							if ( _i_SCH_IO_GET_MODULE_STATUS( DllPathOut , DllSlotOut ) > 0 ) DllRes = 16;
							//
						}
						else {
							DllRes = 17;
						}
						//
					}
				}
			}
			//
			if ( DllRes != 0 ) {
				//
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
				//
				if ( !PreChecking ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 1-20[%d]%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , DllRes , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_SLOTFIX_FAIL2 );
				}
				return ERROR_SLOTFIX_FAIL2;
			}
			//
			//================================================================
			_i_SCH_CASSETTE_LAST_RESET( DllPathIn , DllSlotIn );
			//
			_i_SCH_CASSETTE_LAST_RESET2( CHECKING_SIDE , Pt );
			//================================================================
			_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , DllSlotIn );
			_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , DllSlotOut );
			_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , DllPathIn );
			_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , DllPathOut );
			//
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_LotSpecial( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_LotUserSpecial( CHECKING_SIDE , Pt , "" );
			_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , "" );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , "" );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , Pt , -1 , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , NULL );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			switch( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] ) {
				//
			case 1 :
			case 2 :
			case 3 :
			case 4 :
				_i_SCH_CLUSTER_Set_Ex_OrderMode( CHECKING_SIDE , Pt , 1 );
				break;
			default :
				_i_SCH_CLUSTER_Set_Ex_OrderMode( CHECKING_SIDE , Pt , 0 );
				break;
			}
			//
			switch ( _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() ) {
			case CONTROL_RECIPE_ALL :
			case CONTROL_RECIPE_CLUSTER :
				//====================================================================================================
				// Cluster Recipe Read
				//====================================================================================================
				if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , NULL , -1 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &selgrp , &multigroupfind , NULL ) ) {
					_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , -1 );
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cluster Recipe %s Read Error 14%cRECIPEFAIL %s:%d\n" , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , 9 , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] , ERROR_CLST_RECIPE_DATA );
					return ERROR_CLST_RECIPE_DATA;
				}
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA <= 0 ) ? 0 : ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA - 1 ) );
				//
				_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
				if ( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
				//--------------------------------------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
				if ( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
				//--------------------------------------------------------------------------------------------------
				if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) {
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
					}
				}
				else {
					if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 );
					}
				}
				//--------------------------------------------------------------------------------------------------
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE > 0 ) {
					//
					_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE );
					_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
					//
				}
				if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
				//--------------------------------------------------------------------------------------------------
				Clmode   = CLUSTER_RECIPE->HANDLING_SIDE;
				//====================================================================================================
				// Buffer Item Count Initialize
				//====================================================================================================
				xprogress = 0;
				xprogress2 = 0;
				//
				switch( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] ) {
					//
				case 2 :
				case 4 :
				case 6 :
				case 8 :
				case 10 :
				case 12 :
				case 14 :
				case 16 :
					//====================================================================================================
					// Pre Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE;
					for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
								ch_buffer[ j ] = TRUE;
								i = MAX_CLUSTER_DEPTH;
							}
						}
					}
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					//
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) {
							do_b2 = TRUE;
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) , "" , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else do_b2 = FALSE;
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( j + PM1 ) );
							}
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
								xprogress2++;
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
					//
					break;
					//
				}
				//====================================================================================================
				// Main Cluster File Append
				//====================================================================================================
				for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) {
							//===================================================================================================================
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] );
							}
							//===================================================================================================================
							do_b = TRUE;
							//==========================
							xprogress2++;
							SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
							//==========================
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0; xprogress++;
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) ch_buffer[ j ] = TRUE;
							else                                    ch_buffer[ j ] = FALSE;
						}
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
				}
				//
				//
				switch( LOT_RECIPE_DUMMY_EX.CLUSTER_USING[n] ) {
					//
				case 3 :
				case 4 :
				case 7 :
				case 8 :
				case 11 :
				case 12 :
				case 15 :
				case 16 :
					//====================================================================================================
					// Post Fixed Cluster Append from user edit parameter
					//====================================================================================================
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						switch( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) ) {
						case 0 :
							ch_buffer[ i ] = FALSE;
							break;
						case 1 :
							if ( ch_buffer[ i ] ) ch_buffer[ i ] = FALSE;
							else                  ch_buffer[ i ] = TRUE;
							break;
						case 2 :
							ch_buffer[ i ] = TRUE;
							break;
						}
					}
					//======================================================================================================
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( ch_buffer[ k ] ) {
							M_Count = 0;
							for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( j != k ) ) {
										if ( _i_SCH_PRM_GET_MODULE_GROUP( j + PM1 ) == _i_SCH_PRM_GET_MODULE_GROUP( k + PM1 ) ) {
											i = MAX_CLUSTER_DEPTH;
											j = MAX_PM_CHAMBER_DEPTH;
											M_Count = 1;
											break;
										}
									}
								}
							}
							if ( M_Count == 0 ) ch_buffer[ k ] = FALSE;
						}
					}
					//======================================================================================================
					do_b = FALSE;
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						if ( ch_buffer[ j ] && Get_RunPrm_MODULE_START_ENABLE( j + PM1 , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) {
							do_b2 = TRUE;
							if      ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) && _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) , "" , "" );
							else if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( j + PM1 ) )
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , xprogress , xprogress2 , j + PM1 , "" , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( j + PM1 ) , "" );
							else do_b2 = FALSE;
							if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( j + PM1 ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPre( CHECKING_SIDE , Pt , xprogress , xprogress2 , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( j + PM1 ) );
							}
							if ( do_b2 ) {
								do_b = TRUE;
								//==========================
								xprogress2++;
								SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , TRUE );
								//==========================
							}
						}
					}
					if ( xprogress2 > 0 ) {
						xprogress2 = 0;
						xprogress++;
					}
					else {
						if ( do_b ) return ERROR_PM_SIDE_ERROR;
					}
					//
					break;
					//
				}
				//====================================================================================================
				// Total Item Count Setting
				//====================================================================================================
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , xprogress );
				_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
				//
				Scheduler_CONTROL_Set_PPID( CHECKING_SIDE , Pt , TRUE , LOT_RECIPE_DUMMY_EX.CLUSTER_RECIPE[n] );
				//
				//====================================================================================================
				// Scheduler Item Increase
				//====================================================================================================
				Pt++;
				break;
			}
		}
		//
		// 2016.08.25
		//
		//=======================================================================================================================================














		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( preappendcount > 0 ) { // 2010.05.08
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2010.09.13
				//
				if ( i >= preappendcount ) break;
				//
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , i , -1 ); // 2010.09.27
				//
				_i_SCH_CLUSTER_Set_CPJOB_CONTROL( CHECKING_SIDE , i , -1 );
				_i_SCH_CLUSTER_Set_CPJOB_PROCESS( CHECKING_SIDE , i , -1 );
				//
				_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , i , -1 , NULL ); // 2010.09.28
				//
				_i_SCH_CLUSTER_Set_PathProcessDepth( CHECKING_SIDE , i , -1 , NULL ); // 2014.01.28
				//
				_i_SCH_CLUSTER_Set_Ex_DisableSkip( CHECKING_SIDE , i , 1 ); // 2012.04.14
				//
				_i_SCH_CLUSTER_Set_Ex_UserDummy( CHECKING_SIDE , i , 0 ); // 2015.10.12
				//
			}
			//
			if ( USER_RECIPE_DATA_PRE_APPEND( CHECKING_SIDE , preappendcount ) != 0 ) { // 2010.05.08
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] USER_RECIPE_DATA_PRE_APPEND %d Error%cRECIPEFAIL %d\n" , preappendcount , 9 , preappendcount );
				return ERROR_CLST_RECIPE_DATA;
			}
			//-------------------------------------------------------------------------------
			// 2010.08.06
			//-------------------------------------------------------------------------------
			k = 0;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( i >= preappendcount ) break;
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= BM1 ) {
						k = 1;
						break;
					}
				}
			}
			//
			if ( k == 1 ) {
				//
				j = 0;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( i < preappendcount ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
						j = _i_SCH_CLUSTER_Get_ClusterIndex( CHECKING_SIDE , i );
						break;
					}
				}
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( i < preappendcount ) {
						//
						k = j;
						//
					}
					else {
						//
						k = _i_SCH_CLUSTER_Get_ClusterIndex( CHECKING_SIDE , i );
						//
					}
					//
					_i_SCH_CLUSTER_Set_ClusterIndex( CHECKING_SIDE , i , k + 10 );
					//
				}
				//
				SCHEDULER_Set_CLUSTER_INDEX_SELECT( FALSE , CLUSTER_INDEX_SELECT + 10 );
				//
			}
			//
		}
		//------------------------------------------------------------------------------
		if ( _SCH_COMMON_USER_RECIPE_CHECK( 36 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
			if ( curres != ERROR_NONE ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(36) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
				return curres;
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 211 , PreChecking , 0 ) ) { // 2010.11.09
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(211) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
			return ERROR_BY_USER;
		}
	}
	else { // 2011.09.23

		//--??
		DUMMY_USE      = 0;
		DUMMY_SKIP_USE = 0;
		//
		preappendcount = 0;
		//------------------------------------------------------------------------------
		// Scheduler Data Reset
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		_i_SCH_SUB_Reset_Scheduler_Data( CHECKING_SIDE );
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//====================================================================================================
		strcpy( Selected_LotRecipe_Name , "" );
		//====================================================================================================
		_i_SCH_SYSTEM_PMMODE_SET( CHECKING_SIDE , 0 );
		//
		SYSTEM_DUMMY_SET( CHECKING_SIDE , FALSE );

		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , i+PM1 , FALSE );
			_i_SCH_MODULE_Set_Use_Interlock_Run( CHECKING_SIDE , i+PM1 , FALSE );
		}
		//=======================================================================================================================================
		MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , CHECKING_SIDE + 1 , CHECKING_SIDE + 1 , 1 , &maxunuse );
		//=======================================================================================================================================
		//--??
	}
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 41 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(41) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// EIL
	//------------------------------------------------------------------------------
	// 2012.05.24
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2017.10.10
	if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2017.10.10
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , i , -1 );
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Scheduler Run Chamber Setting Part Init & Setting(just TM/FM)
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
		_i_SCH_MODULE_Set_Mdl_Spd_Flag( CHECKING_SIDE , i , 0 ); // 2006.11.07
	}
	//------------------------------------------------------------------------------
// 2002.06.24 (Move Below after *TMATM Check)
//	if ( _i_SCH_MODULE_GET_USE_ENABLE( TM , FALSE ) )
//		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM , MUF_01_USE );
//	else
//		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM , MUF_00_NOTUSE );

// 2003.08.11
//	if ( _i_SCH_MODULE_GET_USE_ENABLE( FM , FALSE ) )
//		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_01_USE );
//	else
//		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_00_NOTUSE );

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Cassette chamber & slot duplicate or overlap status Check from scheduler data
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
// 2011.05.21
/*
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { cm[i] = 0; cs[0][i] = 0; cs[1][i] = 0; }
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2010.10.12
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( i < preappendcount ) continue; // 2010.05.08
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
	//			if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) { // 2009.01.19
				if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= BM1 ) ) { // 2009.01.19
					if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) {
						//
						cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ] = 1;
						if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) < StartSlot ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > EndSlot ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) ); // 2007.12.05
							return ERROR_CASS_OVERRANGESLOT_SELECT;
						}
						cs[0][_i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i )-1]++;
						//
					}
					else {
						//
						cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ] = 1;
						cm[ _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ] = 1;
	//					if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) < (StartSlot-1) ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) >= EndSlot ) ) {
	//						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out2\tRECIPEFAIL %s:%d\n" , RunName , ERROR_CASS_OVERRANGESLOT_SELECT );
	//						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) ); // 2007.12.05
	//						return ERROR_CASS_OVERRANGESLOT_SELECT;
	//					}
						// Change 2001.11.27
						if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) <= 0 ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > MaxSlot ) ) { // 2010.12.17
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out2\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) ); // 2007.12.05
							return ERROR_CASS_OVERRANGESLOT_SELECT;
						}
						cs[1][_i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i )-1]++;
						//
					}
				}
			}
		}
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( cs[0][i] > 1 ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Slot\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_SLOTDUPLICATE_SELECT );
				return ERROR_CASS_SLOTDUPLICATE_SELECT;
			}
			if ( cs[1][i] > 1 ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Slot 2\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_SLOTDUPLICATE_SELECT );
				return ERROR_CASS_SLOTDUPLICATE_SELECT;
			}
		}
		for ( i = 0 , k = 0; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( cm[i] == 1 ) k++;
		}
		if ( k > 2 ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Cassette\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVER2CASS_SELECT );
			return ERROR_CASS_OVER2CASS_SELECT;
		}
	}
*/
	// 2011.05.21
	// 2011.03.10
	//
/*
// 2011.08.01
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { 
		cm[i] = 0;
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) cs[j][i] = 0;
	}
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2010.10.12
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( i < preappendcount ) continue; // 2010.05.08
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < CM1 ) continue; // 2011.04.14
			//
			if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= BM1 ) ) { // 2009.01.19
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) {
					//
					cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ]++;
					//
					if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) < StartSlot ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > EndSlot ) ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) ); // 2007.12.05
						return ERROR_CASS_OVERRANGESLOT_SELECT;
					}
					//
					cs[_i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i )-CM1][_i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i )-1]++;
					//
					if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) != _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) ) {
						cs[_i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i )-CM1][_i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i )-1]++;
					}
					//
				}
				else {
					//
					cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ]++;
					cs[_i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i )-CM1][_i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i )-1]++;
					//
					if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) < CM1 ) continue; // 2011.04.14
					//
					if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) <= 0 ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > MaxSlot ) ) { // 2010.12.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out2\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) ); // 2007.12.05
						return ERROR_CASS_OVERRANGESLOT_SELECT;
					}
					//
					cm[ _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ]++;
					cs[_i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i )-CM1][_i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i )-1]++;
					//
				}
			}
		}

//		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
//			for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
//				if ( cs[j][i] > 0 ) printf( "[S=%d][L=%d] = [%d]\n" , j , i , cs[j][i] );
//			}
//		}

		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
				if ( cs[j][i] > 1 ) {
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Slot [%d,%d]\tRECIPEFAIL %s:%d\n" , i + 1 , j + 1 , RunAllRecipeName , ERROR_CASS_SLOTDUPLICATE_SELECT );
					return ERROR_CASS_SLOTDUPLICATE_SELECT;
				}
			}
		}
		//
	}
	// 2011.05.21
*/

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2011.09.23
		for ( j = 0 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			restart_check[i][j] = 0;
		}
	}
	//
//	if ( ( !FindAnotherRun2 ) && ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 0 ) ) { // 2011.09.23 // 2017.07.03
	if ( ( FindAnotherRun2 == 0 ) && ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 0 ) ) { // 2011.09.23 // 2017.07.03
		//------------------------------------------------------------------------------
		SCH_RESTART_GET_DATA_INIT();
		//------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( CM1 + i ) ) continue;
			//
			for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				//
				if ( _i_SCH_IO_GET_MODULE_STATUS( CM1 + i , j ) != CWM_PRESENT ) continue;
				//
				if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , CM1 + i , j , 1 ) ) restart_check[CM1 + i][j] = 1;
				//
			}
		}
		//------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( TM + i ) ) continue;
			//
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				//
				if ( j >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
				//
				if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , TM + i , j , 2 ) ) restart_check[TM + i][j] = 1;
				//
			}
		}
		//------------------------------------------------------------------------------
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) continue;
			//
			if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , FM , j , 11 ) ) restart_check[FM][j] = 1;
			//
		}
		//------------------------------------------------------------------------------
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
			//
			for ( j = 1 ; j <= MAX_PM_SLOT_DEPTH ; j++ ) {
				//
				if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , i , j , 3 ) ) restart_check[i][j] = 1;
				//
			}
			//
		}
		//------------------------------------------------------------------------------
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
			//
			if ( SCH_Inside_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
			//
			for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , i , j , 4 ) ) restart_check[i][j] = 1;
				//
			}
			//
		}
		//------------------------------------------------------------------------------
		for ( j = 1 ; j <= 2 ; j++ ) {
			//
			if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , F_AL , j , 12 ) ) restart_check[F_AL][j] = 1;
			//
		}
		//------------------------------------------------------------------------------
		for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; j++ ) {
			//
			if ( Scheduler_CONTROL_Get_Find_Free_Space( PreChecking , CHECKING_SIDE , preappendcount , F_IC , j , 13 ) ) restart_check[F_IC][j] = 1;
			//
		}
		//------------------------------------------------------------------------------
	}

	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 46 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(46) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
// 2011.08.01
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { 
		cm[i] = 0;
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) cs[i][j] = 0;
	}
	//
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
	if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( i < preappendcount ) continue; // 2010.05.08
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < CM1 ) continue; // 2011.04.14
			//
			if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( CHECKING_SIDE , i ) != 0 ) continue; // 2015.10.12
			//
			if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= BM1 ) ) { // 2009.01.19
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) == _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) {
					//
					cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ]++;
					//
					if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) < PM1 ) { // 2015.10.12
						if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) < StartSlot ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > EndSlot ) ) {
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out\tRECIPEFAIL %s:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) ); // 2007.12.05
							return ERROR_CASS_OVERRANGESLOT_SELECT;
						}
					}
					//
					k = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) - 1;
					//
					if ( ( k >= 0 ) && ( k < MAX_CASSETTE_SLOT_SIZE ) ) {
						cs[_i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i )][k]++;
					}
					//
					if ( _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) != _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) ) {
						//
						k = _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) - 1;
						//
						if ( ( k >= 0 ) && ( k < MAX_CASSETTE_SLOT_SIZE ) ) {
							cs[_i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i )][k]++;
						}
					}
					//
				}
				else {
					//
					cm[ _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ]++;
					//
					k = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) - 1;
					//
					if ( ( k >= 0 ) && ( k < MAX_CASSETTE_SLOT_SIZE ) ) {
						cs[_i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i )][k]++;
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) < CM1 ) continue; // 2011.04.14
					//
					if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) < PM1 ) { // 2015.10.12
						if ( ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) <= 0 ) || ( _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) > MaxSlot ) ) { // 2010.12.17
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Overrange Slot for Out2\tRECIPEFAIL %s:%d:%d:%d:%d:%d\n" , RunAllRecipeName , ERROR_CASS_OVERRANGESLOT_SELECT , CHECKING_SIDE , i , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) , MaxSlot );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ) ); // 2007.12.05
							return ERROR_CASS_OVERRANGESLOT_SELECT;
						}
					}
					//
					cm[ _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ]++;
					//
					k = _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) - 1;
					//
					if ( ( k >= 0 ) && ( k < MAX_CASSETTE_SLOT_SIZE ) ) {
						cs[_i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i )][k]++;
					}
					//
				}
			}
		}

		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				if ( cs[i][j] > 1 ) {
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cassette has a Duplicate Slot [%d,%d]\tRECIPEFAIL %s:%d\n" , i + 1 , j + 1 , RunAllRecipeName , ERROR_CASS_SLOTDUPLICATE_SELECT );
					return ERROR_CASS_SLOTDUPLICATE_SELECT;
				}
			}
		}
		//
	}
	// 2011.05.21

	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 51 + ( PreChecking * 1000 ) , CHECKING_SIDE , CPJOB , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(51) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Scheduler Run Chamber Setting Part (just CM) with Upper part data &
	//                       Setting Use Cassette chamber to *map1/2
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
	if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
	//	*map1 = -1; // 2010.01.19
	//	*map2 = -1; // 2010.01.19
		j = -1; // 2010.01.19
		k = -1; // 2010.01.19
		//

// 2011.05.21
/*

		if ( cm[ CHECKING_SIDE + CM1 ] == 1 ) { // 2002.07.11
			j = CHECKING_SIDE + CM1; // 2010.01.19
			_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , CHECKING_SIDE + CM1 , MUF_01_USE );
		}
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( ( i != ( CHECKING_SIDE + CM1 ) ) && ( cm[i] == 1 ) ) { // 2002.07.11
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
				if ( j >= 0 ) k = i; // 2010.01.19
				else          j = i; // 2010.01.19
			}
		}

*/
		// 2011.05.21
		if ( cm[ CHECKING_SIDE + CM1 ] >= 1 ) {
			j = CHECKING_SIDE + CM1;
			_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , CHECKING_SIDE + CM1 , MUF_01_USE );
		}
//		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2011.08.01
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2011.08.01
			if ( ( i != ( CHECKING_SIDE + CM1 ) ) && ( cm[i] >= 1 ) ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
				if ( j >= 0 ) k = i;
				else          j = i;
			}
		}
		// 2011.05.21


		//
		if      ( j == SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) ) { // 2010.01.19
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , k );
		}
		else if ( k == SYSTEM_IN_MODULE_GET(CHECKING_SIDE) ) { // 2010.01.19
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , j );
		}
		else {
			SYSTEM_IN_MODULE_SET( CHECKING_SIDE , j ); // 2010.01.19
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , k ); // 2010.01.19
		}
		//
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Cassette chamber & slot duplicate or overlap status Check from another run
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 0 , CHECKING_SIDE , SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) , SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) , 0 ) ) { // 2008.04.29
			//
			i = SCHEDULER_CONTROL_Check_Duplicate_Use( CHECKING_SIDE , 0 , PreChecking , RunAllRecipeName );
			//
			if ( i != ERROR_NONE ) return i;
			//
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Data Remapping with 1) Cassette Slot Range &
		//                     2) Parallel Processing Chamber reordering with priority
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		//
		AllDis = FALSE; // 2014.02.21
		//
		for ( i = 0 , target_wafer_count = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
	//		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) { // 2006.08.04
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) > 0 ) { // 2006.08.04
				//-------------------------------------
				// Run Cassette Wafer Count Calculate
				//-------------------------------------
	//			if ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) target_wafer_count++; // 2010.01.19
				if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) >= BM1 ) ) target_wafer_count++; // 2010.01.19
				//-------------------------------------
				// 1) Disable Chamber Check
				//                 when Disable Chamber find at Parallel Processing Chamber , set unuse
				//                 when Disable Chamber find at Single   Processing Chamber , return error(AllDis)
				// 2) Scheduler Run Chamber Setting Part (just PM)
				//-------------------------------------
				// Append 2002.02.26 // 2006.08.04
				//-------------------------------------
	//			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) == 0 ) { // 2006.08.04
	//				AllDis = TRUE; // 2006.08.04
	//			} // 2006.08.04
				//-------------------------------------
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) ch_buffer[j] = FALSE; // 2007.12.11
				//-------------------------------------
				for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						l = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
						if ( l > 0 ) {
	//						if ( !_i_SCH_MODULE_GET_USE_ENABLE( l , FALSE ) ) { // 2003.10.09
	//						if ( !Get_RunPrm_MODULE_START_ENABLE( l , PM_MODE ) ) { // 2003.10.09 // 2005.07.06
							if ( !Get_RunPrm_MODULE_START_ENABLE( l , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) { // 2003.10.09 // 2005.07.06
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , i , j , k , 0 , "" , "" , "" );
								ch_buffer[l-PM1] = TRUE; // 2007.12.11
							}
							else if ( DUMMY_USE == l ) {
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , i , j , k , 0 , "" , "" , "" );
								_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , l , MUF_01_USE );
							}
							else if ( DUMMY_SKIP_USE == l ) {
								_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , i , j , k , 0 , "" , "" , "" );
							}
							else {
								_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , l , MUF_01_USE );
							}
						}
					}
					m = 0;
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						l = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
						if ( l > 0 ) {
							m++;
							if ( j == 0 ) SCHEDULER_FIRST_MODULE_EXPEDITE_SET( l , TRUE );
							if ( j == 1 ) SCHEDULER_SECOND_MODULE_EXPEDITE_SET( l , TRUE );
						}
					}
					if ( m == 0 ) {
						//
						if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 8 , CHECKING_SIDE , SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) , SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) , i ) ) { // 2014.02.09
							//
							AllDis = TRUE;
							//========================================================================
							sprintf( errorstring , "%s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) );
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( ch_buffer[k] ) {
									strcat( errorstring , "|" );
									strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( k + PM1 ) );
								}
							}
							//========================================================================
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , i , -1 );
							//========================================================================
							//
						}
						else {
							//========================================================================
							_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , i , -1 ); // 2014.02.21
							//========================================================================
						}
						//
					}
				}
				//-------------------------------------
				// Parallel Processing Chamber reordering with priority
				//-------------------------------------
				if ( !_SCH_COMMON_USER_RECIPE_CHECK( 54 + ( PreChecking * 1000 ) , CHECKING_SIDE , i , 0 , 0 , &curres ) ) { // 2013.03.24
					//
					if ( !AllDis ) {
						for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH - 1 ; k++ ) {
								for ( l = k+1 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
									m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
									n = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , l );
									if ( ( m > 0 ) && ( n > 0 ) ) {
										if      ( _i_SCH_PRM_GET_Putting_Priority( m ) > _i_SCH_PRM_GET_Putting_Priority( n ) ) {
											_i_SCH_CLUSTER_Swap_PathProcessChamber( CHECKING_SIDE , i , j , k , l );
										}
										else if ( _i_SCH_PRM_GET_Putting_Priority( m ) == _i_SCH_PRM_GET_Putting_Priority( n ) ) {
											if ( _i_SCH_PRM_GET_Getting_Priority( m ) > _i_SCH_PRM_GET_Getting_Priority( n ) ) {
												_i_SCH_CLUSTER_Swap_PathProcessChamber( CHECKING_SIDE , i , j , k , l );
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
		//----------------------------------------------
		if ( AllDis ) {
			//
//			if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 8 , CHECKING_SIDE , SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) , SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) , 0 ) ) { // 2014.02.04 // 2014.02.09
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber try access 2 (%s)%cRECIPEFAIL %s:%d\n" , errorstring , 9 , RunAllRecipeName , ERROR_DISABLE_USE );
				return ERROR_DISABLE_USE;
//			} // 2014.02.09
			//
		}
		//===============================================================================================
		// 2007.10.07
		//===============================================================================================
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
					//-----------------------------------------------------------------
					if ( ( ( i - PM1 ) % 2 ) == 0 )
						j = i + 1;
					else
						j = i - 1;
					//-----------------------------------------------------------------
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j ) <= MUF_00_NOTUSE ) { // 2006.11.07
						_i_SCH_MODULE_Set_Mdl_Spd_Flag( CHECKING_SIDE , j , 1 ); // 2006.11.07
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , j , MUF_01_USE );
					}
				}
			}
		}
		//
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 3 ) { // 2016.05.12
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + 1 ) > MUF_00_NOTUSE ) {
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_00_NOTUSE ) {
						_i_SCH_MODULE_Set_Mdl_Spd_Flag( CHECKING_SIDE , i , 1 );
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
					}
					//
					for ( l = 0 ; l < MAX_CASSETTE_SLOT_SIZE ; l++ ) {
						//
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , l ) <= 0 ) continue;
						//
						for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , l ) ; j++ ) {
							//
							m = 0;
							n = 0;
							//
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH - 1 ; k++ ) {
								if      ( _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , l , j , k ) == i ) {
									m = 1;
									break;
								}
								else if ( _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , l , j , k ) == (i+1) ) {
									n = 1;
								}
							}
							//
							if ( !m && n ) {
								_i_SCH_CLUSTER_Set_PathProcessData2( CHECKING_SIDE , l , j , i , "" , "" , "" );
							}
							//
						}
					}
				}
				//
				i++;
				//
			}
		}
		//===============================================================================================
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Scheduler Run Chamber Setting Part (just BM)
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2003.08.11
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_01_USE );
			}
			else {
				if ( SCHEDULER_SIDE_IN_POSSIBLE( 0 , CHECKING_SIDE ) || SCHEDULER_SIDE_OUT_POSSIBLE( FALSE , CHECKING_SIDE ) ) { // 2010.03.15
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_01_USE );
				}
				else { // 2010.03.15
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_00_NOTUSE ); // 2010.03.15
				}
			}
		}
		else {
			_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_00_NOTUSE );
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 56 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(56) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 221 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(221) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//------------------------------------------------------------------------------
	strcpy( errorstring , "" );
	//
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.07.25 // 2017.10.10
	if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2011.07.25 // 2017.10.10
		target_wafer_count = MAX_CASSETTE_SLOT_SIZE;
	}
	else {
		//
		if ( USER_RECIPE_LOOPCOUNT_CHANGE( ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) ? SYSTEM_RID_GET( CHECKING_SIDE ) : CHECKING_SIDE , CHECKING_SIDE , PreChecking , loopcount ) ) { // 2014.05.13
			_i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , *loopcount );
		}
		//
	}
	//
//	i = _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , ( loopcount * 1000 ) + target_wafer_count , errorstring , TMATM ); // 2014.05.13
	i = _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( CHECKING_SIDE , ( *loopcount * 1000 ) + target_wafer_count , errorstring , TMATM ); // 2014.05.13
	//
	if ( i != ERROR_NONE ) {
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber access Error %d%cRECIPEFAIL %s:%d\n" , i , 9 , RunAllRecipeName , i );
		return i;
	}
	//
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.09.07 // 2017.10.10
	if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2011.09.07 // 2017.10.10
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
			}
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Disable Chamber Use Check
	//         When Disable Chamber Use find , return error
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	AllDis = 0;
	//
	if ( !PreChecking ) strcpy( errorstring , "" );
	else                strcpy( errorstring , "DISABLECHK1|" );
	//
	for ( i = CM1 ; i < PM1 ; i++ ) {
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
			if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 6 , CHECKING_SIDE , i , i , 0 ) ) { // 2008.12.12
				if ( ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) ) {
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber (%s) try access 1%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
					if ( AllDis > 0 ) strcat( errorstring , "|" ); // 2007.12.05
					strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2007.12.05
					AllDis++;
				}
			}
		}
	}
	for ( i = PM1 ; i < AL ; i++ ) {
		if ( ( !Get_RunPrm_MODULE_START_ENABLE( i , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) ) { // 2003.10.09 // 2005.07.06
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber (%s) try access 2%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
			if ( AllDis > 0 ) strcat( errorstring , "|" ); // 2007.12.05
			strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2007.12.05
			AllDis++;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// 2003.08.11
	for ( i = TM ; i <= FM ; i++ ) {
		if ( _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 10 , CHECKING_SIDE , i , i , 0 ) ) { // 2018.11.07
			if ( ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) ) {
				if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber (%s) try access 3%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
				if ( AllDis > 0 ) strcat( errorstring , "|" ); // 2007.12.05
				strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2007.12.05
				AllDis++;
			}
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// 2010.03.16
	for ( i = BM1 ; i < TM ; i++ ) {
		if ( ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) != i ) && ( SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) != i ) ) continue;
		if ( ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Disable Chamber (%s) try access 4%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
			if ( AllDis > 0 ) strcat( errorstring , "|" );
			strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
			AllDis++;
		}
	}
	//------------------------------------------------------------------------------
	if ( AllDis > 0 ) return ERROR_TARGET_CHAMBER_ERROR; // 2007.12.10
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 61 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(61) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 231 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(231) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//------------------------------------------------------------------------------
	// Interlock Chamber Use Check
	//         When Interlock Chamber Use find , return error
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//
	if ( !PreChecking ) strcpy( errorstring , "" );
	else                strcpy( errorstring , "DISABLECHK2|" );
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
		//
		// 2013.10.21
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i+PM1 ) <= MUF_00_NOTUSE ) continue;
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) < 0 ) continue;
				//
				if ( ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , j ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , j ) < PM1 ) ) {
					//
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , j ) , i+PM1 ) != 0 ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Interlock Chamber (%s) try access 3%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i+PM1 ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i + PM1 ) ); // 2007.12.05
						return ERROR_TARGET_CHAMBER_ERROR;
					}
					//
				}
				//
				if ( ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , j ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , j ) < PM1 ) ) {
					//
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , j ) , i+PM1 ) != 0 ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Interlock Chamber (%s) try access 4%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i+PM1 ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i + PM1 ) ); // 2007.12.05
						return ERROR_TARGET_CHAMBER_ERROR;
					}
					//
				}
				//
			}
			//
		}
	}
	else {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i+PM1 ) > MUF_00_NOTUSE ) {
				if ( SCHEDULER_SIDE_IN_POSSIBLE( 0 , CHECKING_SIDE ) ) { // 2010.01.19
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) , i+PM1 ) != 0 ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Interlock Chamber (%s) try access 1%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i+PM1 ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i + PM1 ) ); // 2007.12.05
						return ERROR_TARGET_CHAMBER_ERROR;
					}
				}
				if ( SCHEDULER_SIDE_OUT_POSSIBLE( FALSE , CHECKING_SIDE ) ) { // 2010.01.19
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) , i+PM1 ) != 0 ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Interlock Chamber (%s) try access 2%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i+PM1 ) , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i + PM1 ) ); // 2007.12.05
						return ERROR_TARGET_CHAMBER_ERROR;
					}
				}
			}
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// when another job is not running
	//            When ? has a wafer , return error
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
//	if ( !FindAnotherRun2 ) { // 2016.08.12
//	if ( FindAnotherRun2 ) { // 2016.08.12 // 2017.07.03
	if ( FindAnotherRun2 == 1 ) { // 2016.08.12 // 2017.07.03
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			if ( DUMMY_USE != ( j + PM1 ) ) {
				//
				for ( i = 0 ; i < MAX_SDUMMY_DEPTH ; i++ ) {
					//
					if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( i ) == ( j + PM1 ) ) break;
					//
				}
				if ( i == MAX_SDUMMY_DEPTH ) {
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) {
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( j+PM1 ) <= 0 ) {
							//
							if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 2 ) {
								//
								for ( k = 1 ; k <= MAX_PM_SLOT_DEPTH ; k++ ) {
									//
									if ( _i_SCH_MODULE_GET_MOVE_MODE( j+PM1 ) == 1 ) continue;
									//
									if ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , k ) <= 0 ) continue;
									//
									if ( restart_check[j+PM1][k] != 0 ) continue;
									//
									if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( j+PM1 , k ) ) continue; // 2018.04.03
									//
									if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , j + PM1 , k , 0 ) ) {
										//
										if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 2 , CHECKING_SIDE , j + PM1 , k , 0 ) ) {
											//
											if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s:%d)has a Wafer 3%cRECIPEFAIL %s:%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , k , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER , k );
											strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) );
											return ERROR_CHAMBER_HAS_WAFER;
											//
										}
										//
										if ( !_i_SCH_MODULE_GET_USE_ENABLE( j+PM1 , FALSE , CHECKING_SIDE ) ) {
											//
											SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( j + PM1 , ABORTWAIT_FLAG_WAIT );
											//
											_i_SCH_MODULE_Set_PM_SIDE( j+PM1 , k-1 , -1 );
											_i_SCH_MODULE_Set_PM_POINTER( j+PM1 , k-1 , -1 );
											_i_SCH_MODULE_Set_PM_WAFER( j+PM1 , k-1 , 1 );
										}
										else {
											//
											if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s:%d)has a Wafer 4%cRECIPEFAIL %s:%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , k , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER , k );
											strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) );
											return ERROR_CHAMBER_HAS_WAFER;
											//
										}
										//
									}
									//
								}
								//
							}
						}
					}
					//
				}

			}
		}
	}
//	else { // 2016.08.12
	else if ( FindAnotherRun2 == 0 ) { // 2016.08.12 // 2017.07.03
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// TM Robot has a wafer when another job is not running
		//            When Robot has a wafer , return error
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		for ( i = 0 ; i <= (*TMATM) ; i++ ) {
			//
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				//
				if ( j >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
				//
				if ( restart_check[TM+i][j] != 0 ) continue; // 2011.09.23
				//
				if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , j ) && Scheduler_ROBOT_FINGER_HAS_WAFER( i , j ) ) {
					//
					if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , i + TM , j , 0 ) ) { // 2009.06.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Robot has a Wafer at Finger %c%cRECIPEFAIL %s:%d\n" , j + 'A' , 9 , RunAllRecipeName , ERROR_ROBOT_HAS_WAFER );
						return ERROR_ROBOT_HAS_WAFER;
					}
				}
			}
			//
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// FM Robot has a wafer when another job is not running
		//            When Robot has a wafer , return error
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 , TA_STATION ) != 0 ) ) {
				//
				if ( restart_check[FM][TA_STATION] == 0 ) { // 2011.09.23
					//
					if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , FM , TA_STATION , 0 ) ) { // 2009.06.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Robot FM has a Wafer at Finger A%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_ROBOT_HAS_WAFER );
						return ERROR_ROBOT_HAS_WAFER;
					}
					//
				}
				//
			}
			if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 , TB_STATION ) != 0 ) ) {
				//
				if ( restart_check[FM][TB_STATION] == 0 ) { // 2011.09.23
					//
					if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , FM , TB_STATION , 0 ) ) { // 2009.06.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Robot FM has a Wafer at Finger B%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_ROBOT_HAS_WAFER );
						return ERROR_ROBOT_HAS_WAFER;
					}
					//
				}
				//
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// PM Chamber has a wafer when another job is not running
		//            When chamber has a wafer , return error
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( DUMMY_USE != ( j + PM1 ) ) {
				for ( i = 0 ; i < MAX_SDUMMY_DEPTH ; i++ ) { // 2002.05.09
					if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( i ) == ( j + PM1 ) ) break;
				}
				if ( i == MAX_SDUMMY_DEPTH ) {
					//
					/*
					// 2011.07.15
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) && ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , 1 ) > 0 ) ) {
						//
						if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , j + PM1 , 1 , 0 ) ) { // 2009.06.17
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s)has a Wafer%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) ); // 2007.12.05
							return ERROR_CHAMBER_HAS_WAFER;
						}
						//
					}
					*/
					// 2011.07.15
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) {
						//
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 2 ) { // 2013.09.03
							//
							for ( k = 1 ; k <= MAX_PM_SLOT_DEPTH ; k++ ) {
								//
								if ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , k ) > 0 ) break;
								//
							}
							//
							if ( k > MAX_PM_SLOT_DEPTH ) {
								//
								if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s)has not a Wafer 2%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_NOT_WAFER );
								strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) );
								return ERROR_CHAMBER_HAS_NOT_WAFER;
								//
							}
							//
						}
						else {
							for ( k = 1 ; k <= MAX_PM_SLOT_DEPTH ; k++ ) {
								//
								if ( _i_SCH_MODULE_GET_MOVE_MODE( j+PM1 ) == 1 ) continue; // 2013.09.03
								//
								if ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , k ) <= 0 ) continue;
								//
								if ( restart_check[j+PM1][k] != 0 ) continue; // 2011.09.23
								//
								if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( j+PM1 , k ) ) continue; // 2018.04.03
								//
								if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , j + PM1 , k , 0 ) ) {
									//
									if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 2 , CHECKING_SIDE , j + PM1 , k , 0 ) ) {
										//
										if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s:%d)has a Wafer%cRECIPEFAIL %s:%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , k , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER , k );
										strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) );
										return ERROR_CHAMBER_HAS_WAFER;
										//
									}
									//
									if ( !_i_SCH_MODULE_GET_USE_ENABLE( j+PM1 , FALSE , CHECKING_SIDE ) ) {
										//
										SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( j + PM1 , ABORTWAIT_FLAG_WAIT );
										//
										_i_SCH_MODULE_Set_PM_SIDE( j+PM1 , k-1 , -1 );
										_i_SCH_MODULE_Set_PM_POINTER( j+PM1 , k-1 , -1 );
										_i_SCH_MODULE_Set_PM_WAFER( j+PM1 , k-1 , 1 );
									}
									else {
										//
										if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s:%d)has a Wafer 2%cRECIPEFAIL %s:%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , k , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER , k );
										strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) );
										return ERROR_CHAMBER_HAS_WAFER;
										//
									}
									//
								}
								//
							}
							//
						}
					}
					//
				}
				else { // 2002.05.09
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] S-Dummy Chamber used at recipe%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) ); // 2007.12.05
						return ERROR_TARGET_CHAMBER_ERROR;
					}
				}
			}
			else {
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) && ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ) <= 0 ) ) {
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s) has not a Wafer%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_NOT_WAFER );
					strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) ); // 2007.12.05
					return ERROR_CHAMBER_HAS_NOT_WAFER;
				}
			}
		}
		if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( DUMMY_USE != ( j + PM1 ) ) {
					//
					/*
					// 2011.07.15
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) && ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , 2 ) > 0 ) ) {
						//
						if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , j + PM1 , 2 , 0 ) ) { // 2009.06.17
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s) has a Wafer%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
							strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) ); // 2007.12.05
							return ERROR_CHAMBER_HAS_WAFER;
						}
						//
					}
					//
					*/
				}
				else {
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) > MUF_00_NOTUSE ) && ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() + 1 ) <= 0 ) ) {
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Chamber (%s) has not a Wafer%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j+PM1 ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_NOT_WAFER );
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j + PM1 ) ); // 2007.12.05
						return ERROR_CHAMBER_HAS_NOT_WAFER;
					}
				}
			}
		}
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// BM Chamber has a wafer when another job is not running
		//            When chamber has a wafer , return error
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
/*
// 2008.03.06
		RunWafer = 1;
		if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_MODE() != 0 ) { // 2006.12.21
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != CHECKING_SIDE ) {
					if ( _i_SCH_SYSTEM_USING_GET( i ) >= 5 ) {
						if ( _i_SCH_MODULE_Get_BM_Switch_WhatCh( i ) == _i_SCH_MODULE_Get_BM_Switch_WhatCh( CHECKING_SIDE ) ) {
							RunWafer = -1;
							break;
						}
					}
				}
			}
		}
		if ( RunWafer > 0 ) {
*/		
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2013.09.13
			//
			if ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) < BM1 ) { // 2010.03.18
				//
				for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
					//
					if ( SCH_Inside_ThisIs_BM_OtherChamber( j , 1 ) ) continue; // 2009.01.21
					//
					if ( SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) == j ) continue; // 2010.03.14
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j ) > MUF_00_NOTUSE ) {
						//
						for ( k = 1 ; k <= _i_SCH_PRM_GET_MODULE_SIZE( j ) ; k++ ) {
							//
							if ( _i_SCH_IO_GET_MODULE_STATUS( j , k ) > 0 ) {
								//
								if ( restart_check[j][k] != 0 ) continue; // 2011.09.23
								//
								if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( j , k ) ) continue; // 2018.04.03
								//
								if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , j , k , 0 ) ) { // 2009.06.17
									if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Buffer Chamber (%s) has a Wafer%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
									strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) ); // 2007.12.05
									return ERROR_CHAMBER_HAS_WAFER;
								}
							}
						}
					}
				}
			} // 2010.03.10
		}
//		} // 2008.03.06
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// ALIGNER or COOLER has a wafer when another job is not running
		//            When ALIGNER or COOLER has a wafer , return error
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( ( _i_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( AL , 1 ) > 0 ) ) {
			//
			if ( restart_check[AL][1] == 0 ) { // 2011.09.23
				//
				if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , AL , 1 , 0 ) ) { // 2009.06.17
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Aligner has a Wafer%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
					strcpy( errorstring , "AL" ); // 2007.12.05
					return ERROR_CHAMBER_HAS_WAFER;
				}
				//
			}
			//
		}
		if ( ( _i_SCH_MODULE_GET_USE_ENABLE( IC , FALSE , -1 ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( IC , 1 ) > 0 ) ) {
			//
			if ( restart_check[AL][1] == 0 ) { // 2011.09.23
				//
				if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , IC , 1 , 0 ) ) { // 2009.06.17
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cooler has a Wafer%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
					strcpy( errorstring , "IC" ); // 2007.12.05
					return ERROR_CHAMBER_HAS_WAFER;
				}
				//
			}
			//
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2004.08.16
			if ( ( _i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) > 0 ) ) {
				//
				if ( restart_check[F_AL][1] == 0 ) { // 2011.09.23
					//
					if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , F_AL , 1 , 0 ) ) { // 2009.06.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Aligner(F) has a Wafer%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
						strcpy( errorstring , "F_AL" ); // 2007.12.05
						return ERROR_CHAMBER_HAS_WAFER;
					}
					//
				}
				//
			}
			/*
			// 2011.09.23
			if ( ( _i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , 1 ) > 0 ) ) {
				if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , F_IC , 1 , 0 ) ) { // 2009.06.17
					if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cooler(F) has a Wafer%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
					strcpy( errorstring , "IC" ); // 2007.12.05
					return ERROR_CHAMBER_HAS_WAFER;
				}
			}
			*/
			//
			// 2011.09.23
			//
			if ( _i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) {
				//
				for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; j++ ) {
					//
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , j ) <= 0 ) continue;
					//
					if ( restart_check[F_IC][j] != 0 ) continue;
					//
					if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 0 , CHECKING_SIDE , F_IC , j , 0 ) ) { // 2009.06.17
						if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Cooler(F) has a Wafer%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_CHAMBER_HAS_WAFER );
						strcpy( errorstring , "F_IC" ); // 2007.12.05
						return ERROR_CHAMBER_HAS_WAFER;
					}
					//
				}
				//
			}
		}
	}
	//------------------------------------------------------------------------------
	// (POST Data Remap) 3.PM Side Handling data remapping for TM Double Style(only use)(Move Below : 2002.07.11)
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 71 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(71) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 241 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(241) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//------------------------------------------------------------------------------
	// (POST Data Check) 2.PM Process Recipe File Check
	//------------------------------------------------------------------------------
	//Scheduler_CONTROL_CLUSTERStepTemplate12_AppendView( CHECKING_SIDE );
	//------------------------------------------------------------------------------
	if ( !PreChecking ) {
		//
//		RECIPE_FILE_PROCESS_File_Locking_Delete( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , CHECKING_SIDE ); // 2014.02.08
		RECIPE_FILE_PROCESS_File_Locking_Delete( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) , CHECKING_SIDE ); // 2014.02.08
		//
		//------------------------------------------------------------------------------
		for ( Pt = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) >= 0 ) {
				for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
					//
					postyes = FALSE; // 2003.05.22
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						Clmode = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
						//
						if ( Clmode < 0 ) Clmode = -Clmode;// 2015.03.30
						//
						if ( Clmode > 0 ) { // 2003.05.26
							//
							if ( !LOTMAPMODE ) { // 2016.12.10
								//
								flock = Scheduler_CONTROL_CLUSTERStepTemplate12_AppendFind( CHECKING_SIDE , 0 , j , Clmode - PM1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) ); // 2014.02.05
								//
	//							if ( RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) , 0 , 0 ) == 1 ) { // 2014.02.08
								if ( RECIPE_FILE_PROCESS_File_Check( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) , 0 , 0 ) == 1 ) { // 2014.02.08
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] (%s) Process Recipe File %s can not find%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) , 9 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) , ERROR_PM_RECIPE_FILE_ERROR );
									strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) ); // 2007.12.05
									return ERROR_PM_RECIPE_FILE_ERROR;
								}
								//
								flock = Scheduler_CONTROL_CLUSTERStepTemplate12_AppendFind( CHECKING_SIDE , 2 , j , Clmode - PM1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) ); // 2014.02.05
								//
	//							if ( RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) , 2 , 0 ) == 1 ) { // 2014.02.08
								if ( RECIPE_FILE_PROCESS_File_Check( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) , 2 , 0 ) == 1 ) { // 2014.02.08
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] (%s) Process Pre Recipe File %s can not find%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) , 9 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) , ERROR_PM_RECIPE_FILE_ERROR );
									strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) ); // 2007.12.05
									return ERROR_PM_RECIPE_FILE_ERROR;
								}
								//----------------------------------------------------------------------------------------------
								// 2003.05.22 (Post Check & Set for All Option)
								//----------------------------------------------------------------------------------------------
								//
								flock = Scheduler_CONTROL_CLUSTERStepTemplate12_AppendFind( CHECKING_SIDE , 1 , j , Clmode - PM1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) ); // 2014.02.05
								//
	//							l = RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) , 1 , 0 ); // 2014.02.08
								l = RECIPE_FILE_PROCESS_File_Check( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) , flock , CHECKING_SIDE , ( CPJOB == 1 ) ? Pt : -1 , Clmode , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , Pt ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) , 1 , 0 ); // 2014.02.08
								//
							}
							else {
								l = 0;
							}
							//
							if ( l == 1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] (%s) Process Post Recipe File %s can not find%cRECIPEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) , 9 , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) , ERROR_PM_RECIPE_FILE_ERROR );
								strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( Clmode ) ); // 2007.12.05
								return ERROR_PM_RECIPE_FILE_ERROR;
							}
							else if ( l == 0 ) { // yesfile
//								if ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() / 3 ) == 1 ) { // 2005.02.21
								if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() / 3 ) % 2 ) == 1 ) { // 2005.02.21
									postyes = TRUE;
								}
							}
							else if ( l == 2 ) { // Null
								switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Clmode ) ) {
								case 0 : // run
								case 2 : // sim
									break;
								default :
									if ( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) == NULL ) {
										_i_SCH_CLUSTER_Set_PathProcessData_JustPost( CHECKING_SIDE , Pt , j , k , "?" );
									}
									else if ( strlen( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) ) <= 0 ) {
										_i_SCH_CLUSTER_Set_PathProcessData_JustPost( CHECKING_SIDE , Pt , j , k , "?" );
									}
									break;
								}
							}
							//----------------------------------------------------------------------------------------------
						}
					}
					//----------------------------------------------------------------------------------------------
					// 2003.05.22 (Post Check & Set for All Option)
					//----------------------------------------------------------------------------------------------
					if ( postyes ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							if ( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) == NULL ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPost( CHECKING_SIDE , Pt , j , k , "?" );
							}
							else if ( strlen( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) ) <= 0 ) {
								_i_SCH_CLUSTER_Set_PathProcessData_JustPost( CHECKING_SIDE , Pt , j , k , "?" );
							}
						}
					}
					//----------------------------------------------------------------------------------------------
				}
			}
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// when another job is not running
	//            Dummy Go Chamber Setting
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
//	if ( !FindAnotherRun ) { // 2003.02.12
		if ( DUMMY_USE != 0 ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( DUMMY_USE != ( j + PM1 ) ) {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j+PM1 ) == MUF_00_NOTUSE ) {
//						SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( j+PM1 , FALSE ); // 2003.02.12
						SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , FALSE ); // 2003.02.12
					}
					if ( _i_SCH_PRM_GET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( j+PM1 ) ) {
//						SCHEDULER_CONTROL_Set_Chamber_Use_Interlock( j+PM1 , FALSE ); // 2003.02.12
						SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( CHECKING_SIDE , j+PM1 , FALSE ); // 2003.02.12
					}
				}
			}
		}
//	}
	//-----------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------
	// 2003.10.10 Disable Check & Unuse Error
	//-----------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE; // 2006.11.22
	//-----------------------------------------------------------------------------------------------------------
//	l = 1; // 2006.11.22
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
				m = 0;
				dws = 0; // 2006.06.29
				n2 = 0; // 2008.12.16
				//------------------------------------------------------
				//
				if ( !PreChecking ) sprintf( errorstring , "%s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) );
				else                sprintf( errorstring , "DISABLECHK3|%s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) );
				//
				//------------------------------------------------------
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					n = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
//					if ( n >= 0 ) { // 2006.11.22
					if ( n > 0 ) { // 2006.11.22
//						if ( !_i_SCH_MODULE_GET_USE_ENABLE( n , FALSE , CHECKING_SIDE ) ) { // 2018.12.20
						if ( !_i_SCH_MODULE_GET_USE_ENABLE( n , FALSE , CHECKING_SIDE + 1000 ) ) { // 2018.12.20
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( CHECKING_SIDE , i , j , k );
							//
							n2++; // 2008.12.16
							//
							strcat( errorstring , "|" );
							strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( n ) );
							//
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , n , MUF_99_USE_to_DISABLE ); // 2009.10.13
							//
						}
						else {
							if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( n ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( n ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
								m++;
								//==================================================================
								// 2006.11.22
								//==================================================================
								ch_buffer[ n - PM1 ] = TRUE; // 2006.11.22
								//==================================================================
							}
							else { // 2006.06.29
								dws++;
							}
						}
					}
				}
//				if ( m <= 0 ) { // 2006.06.29
				if ( ( m <= 0 ) && ( dws <= 0 ) ) { // 2006.06.29
					switch( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() ) {
					case 1 : // Disable
					case 2 : // DisableHW
					case 3 : // Disable,DisableHW
					case 7 : // Disable
					case 8 : // DisableHW
					case 9 : // Disable,DisableHW
//						if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 1 , CHECKING_SIDE , n2 , 0 , 0 ) ) { // 2009.06.17 // 2010.11.16
						if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 1 , CHECKING_SIDE , n2 , i , 0 ) ) { // 2009.06.17 // 2010.11.16
//						if ( n2 <= 0 ) { // 2008.12.16 // 2009.06.17
							if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] All Layer Disable Chamber Set%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_TARGET_CHAMBER_ERROR );
							return ERROR_TARGET_CHAMBER_ERROR;
						}
						break;
					}
//					l = 0; // 2006.11.22
				}
//				else { // 2006.11.22
//					if ( ( m > l ) && ( l != 0 ) ) l = m; // 2006.11.22
//				} // 2006.11.22
			}
		}
	}
	//====================================================================================================================================
	// 2006.11.22
	//====================================================================================================================================
	l = 0;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( ch_buffer[ i ] ) l++;
	}
	if ( l <= 0 ) l = 1;
	//====================================================================================================================================
//_IO_CIM_PRINTF( "=======================================\n" );
//_IO_CIM_PRINTF( "=[C]====================[%d]===========\n" , l );
//_IO_CIM_PRINTF( "=======================================\n" );
	SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( CHECKING_SIDE , l );
	//-----------------------------------------------------------------------------------------------------------
	if ( PreChecking ) return ERROR_NONE; // 2004.06.16
	//------------------------------------------------------------------------------
	// EIL
	//------------------------------------------------------------------------------
	/*
	// 2012.05.24
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) { // 2010.09.13
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , i , -1 );
		}
	}
	*/
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 81 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(81) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 251 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(251) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//------------------------------------------------------------------------------
	// Control Job Index Setting
	//------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		_i_SCH_CLUSTER_Set_CPJOB_CONTROL( CHECKING_SIDE , i , -1 );
		_i_SCH_CLUSTER_Set_CPJOB_PROCESS( CHECKING_SIDE , i , -1 );
	}
	//------------------------------------------------------------------------------
	// Control Job Manual Index Setting
	//------------------------------------------------------------------------------
	if ( CPJOB == 2 ) { // Manual
		if ( !SCHMULTI_CONTROLJOB_MANUAL_POSSIBLE() ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Control,Process JOB Regist Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_JOB_RUN_ALREADY );
			return ERROR_JOB_RUN_ALREADY;
		}
		//---------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------
		SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_DATA( CHECKING_SIDE , ManualCJName , ManualPJName , &ManualmultiCJmode ); // 2004.05.11
//		strcpy( ManualCJName , "$MANUAL$" ); // 2004.04.27 // 2004.05.11
//		sprintf( ManualPJName , "$MANUAL%d$" , CHECKING_SIDE + 1 ); // 2004.04.27 // 2004.05.11
		//---------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------
		SCHMULTI_SELECTDATA_CLEAR_CHECK_WITH_CJOB_AT_MANUAL( ManualmultiCJmode ); // 2004.04.27
		//------------------------------------------------------------------------------------------------------
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2013.06.12
			//
			strcpy( Per_Run1Name , "" );
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
				//
				RunWafer = _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i );
				//
				if ( RunWafer < CM1 ) continue;
				if ( RunWafer >= PM1 ) continue;
				//
				switch( _i_SCH_PRM_GET_FA_MID_READ_POINT( CHECKING_SIDE ) ) {
				case 1 : // With IO
					IO_GET_MID_NAME_FROM_READ( RunWafer - CM1 , Per_Run1Name );
					break;
				case 2 : // With AfterMap
					break;
				case 3 : // With Current IO
					IO_GET_MID_NAME( RunWafer - CM1 , Per_Run1Name );
					break;
				default : // With Message
					IO_GET_MID_NAME( RunWafer - CM1 , Per_Run1Name );
					break;
				}
				//
				break;
			}
			//
		}
		else {
			strcpy( Per_Run1Name , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
		}
		//
//		RunWafer = SCHMULTI_PROCESSJOB_MANUAL_INSERT_and_SETUP( CHECKING_SIDE , ManualPJName , RunAllRecipeName , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ); // 2013.06.12
		RunWafer = SCHMULTI_PROCESSJOB_MANUAL_INSERT_and_SETUP( CHECKING_SIDE , ManualPJName , RunAllRecipeName , Per_Run1Name ); // 2013.06.12
		if ( RunWafer < 0 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Control,Process JOB Regist Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_JOB_RUN_ALREADY );
			return ERROR_JOB_RUN_ALREADY;
		}
		//------------------------------------------------------------------------------------------------------
		//==========================================================================================
		// 2005.05.11
		//==========================================================================================
//		IO_SET_JID_NAME( CHECKING_SIDE , ManualCJName ); // 2005.05.11

		if ( (_i_SCH_PRM_GET_FA_JID_READ_POINT(CHECKING_SIDE) == 0) ||
			(strlen(_i_SCH_SYSTEM_GET_JOB_ID(CHECKING_SIDE)) <= 0)) { // 2019.03.27
			//
			switch (SCHMULTI_JOBNAME_DISPLAYMODE_GET()) {
			case 1: // P-J(0)
				IO_SET_JID_NAME(CHECKING_SIDE, ManualPJName);
				break;
			case 2: // P-J(A)
				IO_SET_JID_NAME(CHECKING_SIDE, ManualPJName);
				break;
			case 3: // LotID
				IO_SET_JID_NAME(CHECKING_SIDE, RunAllRecipeName);
				break;
			case 4: // MID
				if (strlen(_i_SCH_SYSTEM_GET_MID_ID(CHECKING_SIDE)) <= 0) {
					IO_SET_JID_NAME(CHECKING_SIDE, ManualCJName);
				}
				else {
					IO_SET_JID_NAME(CHECKING_SIDE, _i_SCH_SYSTEM_GET_MID_ID(CHECKING_SIDE));
				}
				break;
			case 5: // C-J|P-J
				sprintf(Per_Run1Name, "%s|%s", ManualCJName, ManualPJName); // 2005.05.23
				IO_SET_JID_NAME(CHECKING_SIDE, Per_Run1Name); // 2005.05.23
				break;
			default: // C-J
				IO_SET_JID_NAME(CHECKING_SIDE, ManualCJName);
				break;
			}
			//
		}
		//==========================================================================================
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		ManualSelectCJIndex = 0;
		if ( ManualmultiCJmode ) { // 2004.06.01
			for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
				if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus < CTLJOB_STS_SELECTED ) {
					ManualSelectCJIndex = i;
					break;
				}
			}
		}
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		SCHMULTI_SELECTDATA_SET_SELECTCJINDEX( CHECKING_SIDE , ManualSelectCJIndex ); // 2003.07.23
		SCHMULTI_SELECTDATA_SET_SELECTCJORDER( CHECKING_SIDE , 0 ); // 2013.09.27
		//------------------------------------------------------------------------------------------------------
//		SCHMULTI_SELECTDATA_SET_CASSETTE( CHECKING_SIDE , CHECKING_SIDE ); // 2018.08.30
		//
		SCHMULTI_SELECTDATA_SET_RUNSIDE( CHECKING_SIDE , CHECKING_SIDE ); // 2018.08.30
		//
		SCHMULTI_SELECTDATA_SET_CASSETTE( CHECKING_SIDE , SCHEDULER_GET_SIDE_CM( CHECKING_SIDE ) ); // 2018.08.30
		//
		SCHMULTI_SELECTDATA_SET_CASSETTEINDEX( CHECKING_SIDE , -1 ); // 2018.05.24
		//
		SCHMULTI_SELECTDATA_SET_CARRIERID( CHECKING_SIDE , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );

//		SCHMULTI_SELECTDATA_SET_OUTCARRIERID( CHECKING_SIDE , "" ); // 2011.02.15(Testing) // 2011.05.21

		SCHMULTI_SELECTDATA_SET_MTLCJNAME( CHECKING_SIDE , ManualCJName );
		SCHMULTI_SELECTDATA_SET_MTLCJSTARTMODE( CHECKING_SIDE , 0 );
		SCHMULTI_SELECTDATA_SET_MTLCOUNT( CHECKING_SIDE , 0 );
		SCHMULTI_SELECTDATA_SET_RUNSTATUS( CHECKING_SIDE , CASSRUNJOB_STS_WAITING );
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( SCHMULTI_SELECTDATA_FULL_MTLCOUNT( CHECKING_SIDE ) ) break; // 2018.05.10
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
				//
				count = SCHMULTI_SELECTDATA_GET_MTLCOUNT( CHECKING_SIDE );
				//
				SCHMULTI_SELECTDATA_SET_MTLSIDE( CHECKING_SIDE , count , CHECKING_SIDE ); // 2018.05.10
				SCHMULTI_SELECTDATA_SET_MTLPOINTER( CHECKING_SIDE , count , i ); // 2018.05.10
				//
				SCHMULTI_SELECTDATA_SET_MTLPJID( CHECKING_SIDE , count , RunWafer );
				SCHMULTI_SELECTDATA_SET_MTLPJNAME( CHECKING_SIDE , count , ManualPJName );
				//
				SCHMULTI_SELECTDATA_SET_MTLPJSTARTMODE( CHECKING_SIDE , count , 0 );
				//
				SCHMULTI_SELECTDATA_SET_MTLINCASSETTE( CHECKING_SIDE , count , 0 ); // 2018.05.10
				SCHMULTI_SELECTDATA_SET_MTLINCARRIERID( CHECKING_SIDE , count , "" ); // 2018.05.10
				SCHMULTI_SELECTDATA_SET_MTLINSLOT( CHECKING_SIDE , count , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) );
				SCHMULTI_SELECTDATA_SET_MTLINCARRIERINDEX( CHECKING_SIDE , count , -1 ); // 2018.05.10
				//
				if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) == 0 ) { // 2018.07.17
					//
					SCHMULTI_SELECTDATA_SET_MTLOUTCASSETTE( CHECKING_SIDE , count , 0 );
				}
				else if ( _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) <= -100 ) { // 2012.02.18
					//
					SCHMULTI_SELECTDATA_SET_MTLOUTCASSETTE( CHECKING_SIDE , count , ( -_i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) % 100 ) );
				}
				else {
					SCHMULTI_SELECTDATA_SET_MTLOUTCASSETTE( CHECKING_SIDE , count , _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , i ) ); // 2011.03.30(Testing) // 2011.05.21
				}
				//
				SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERID( CHECKING_SIDE , count , "" ); // 2011.02.15(Testing) // 2011.05.21
				SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERINDEX( CHECKING_SIDE , count , -1 ); // 2018.05.10

				SCHMULTI_SELECTDATA_SET_MTLOUTSLOT( CHECKING_SIDE , count , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , i ) );
				//
				SCHMULTI_SELECTDATA_SET_MTLRECIPEMODE( CHECKING_SIDE , count , 0 );
				SCHMULTI_SELECTDATA_SET_MTLRECIPENAME( CHECKING_SIDE , count , LOT_RECIPE.CLUSTER_RECIPE[_i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i )-1] );
				//
				SCHMULTI_SELECTDATA_SET_MTLCOUNT( CHECKING_SIDE , count + 1 );
				//
				SCHMULTI_PROCESSJOB_MANUAL_SET_MTLSLOT( RunWafer , i , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) );
				//
			}
		}
		SCHMULTI_SELECTDATA_SET_RUNSTATUS( CHECKING_SIDE , CASSRUNJOB_STS_RUNNING );
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------
		// Make CJM
		//------------------------------------------------------------------------------------------------------
		if ( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->ControlStatus < CTLJOB_STS_SELECTED ) {
			strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->JobID , ManualCJName );
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->StartMode = 0;
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->PRJobDelete = 2;
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->OrderMode = 1;
			//
			for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_Use[i] = FALSE;
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_ProcessJobID[i] , "" );
				//
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_RuleValue[i] = 0;
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_OutValue[i] = 0;
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_RuleName[i] , "" );
				//
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassInUse[i] = 0;
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassInID[i] , "" );
				//
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassOutUse[i] = 0;
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSrcID[i] , "" );
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassOutID[i] , "" );
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSlotIn[i][j] = 0;
					Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSlotOut[i][j] = 0;
				}
			}
			for ( i = 0 ; i < MAX_MULTI_CTJOB_EVENTINFO ; i++ ) {
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->EventInfo[i] = 0;
			}
			//
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_Use[CHECKING_SIDE] = TRUE;
			strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_ProcessJobID[CHECKING_SIDE] , ManualPJName );
			//
			if ( ( SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) >= 0 ) && ( SYSTEM_IN_MODULE_GET( CHECKING_SIDE ) != SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) ) ) { // 2009.04.08
				//
				Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassOutUse[CHECKING_SIDE] = ( SYSTEM_OUT_MODULE_GET( CHECKING_SIDE ) * 1000 ) + 1;
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSrcID[CHECKING_SIDE] , "" );
				strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassOutID[CHECKING_SIDE] , "" );
				//
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSlotIn[CHECKING_SIDE][j] = 1+j;
					Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CassSlotOut[CHECKING_SIDE][j] = 1+j;
				}
			}
			//
			SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_INSERT , 1 , ManualCJName , -1 , -1 );
			//
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->ControlStatus = CTLJOB_STS_SELECTED;

			//===================================================================================================
			// 2008.03.13
			//===================================================================================================
			for ( i = 0 ; i < 1000 ; i++ ) {
				if ( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->ControlStatus != CTLJOB_STS_SELECTED ) break;
				Sleep(10);
			}
			//===================================================================================================
		}
		else {
			//
			Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_Use[CHECKING_SIDE] = TRUE;
			strcpy( Sch_Multi_ControlJob_Select_Data[ManualSelectCJIndex]->CtrlSpec_ProcessJobID[CHECKING_SIDE] , ManualPJName );
			//
		}
	}
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 91 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(91) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 261 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(261) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//------------------------------------------------------------------------------
	if ( CPJOB != 0 ) SCHMULTI_RUNJOB_SET_DATABASE_LINK( CHECKING_SIDE );
	//------------------------------------------------------------------------------
	// Target Wafer Setting
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Run Recipe %s Read Success%cRECIPESUCCESS %s:%d\n" , RunAllRecipeName , 9 , RunAllRecipeName , target_wafer_count );
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	LOG_MTL_SET_TARGET_WAFER_COUNT( CHECKING_SIDE , target_wafer_count );
	//----------------------------------------------------------------------------------------
	if ( preappendcount > 0 ) { // 2012.03.30
		//
		j = -1;
		//
		for ( i = preappendcount ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( CHECKING_SIDE , i ) < 0 ) continue;
			if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( CHECKING_SIDE , i ) < 0 ) continue;
			//
			j = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( CHECKING_SIDE , i );
			//
			break;
		}
		//
		for ( i = 0 ; i < preappendcount ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( CHECKING_SIDE , i ) >= 0 ) continue;
			if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( CHECKING_SIDE , i ) >= 0 ) continue;
			//
			_i_SCH_CLUSTER_Set_CPJOB_CONTROL( CHECKING_SIDE , i , j );
			//
		}
	}
	//
	//==============================================================================================================
	// 2005.03.02
	//==============================================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
			Scheduler_Mapping_Buffer_Data_Set( CHECKING_SIDE , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) - 1 , 0 , 1 );
		}
	}
	//------------------------------------------------------------------------------
	if ( _SCH_COMMON_USER_RECIPE_CHECK( 96 + ( PreChecking * 1000 ) , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2013.01.16
		if ( curres != ERROR_NONE ) {
			if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_RECIPE_CHECK(96) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , curres );
			return curres;
		}
	}
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if ( !_SCH_COMMON_USER_DATABASE_REMAPPING( CHECKING_SIDE , 291 , PreChecking , 0 ) ) { // 2010.11.09
		if ( !PreChecking ) _i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] _SCH_COMMON_USER_DATABASE_REMAPPING(0) Fail%cRECIPEFAIL %s:%d\n" , 9 , RunAllRecipeName , ERROR_BY_USER );
		return ERROR_BY_USER;
	}
	//==============================================================================================================
	//
	SCHEDULER_CONTROL_Check_OrderMode_SupplyID( CHECKING_SIDE ); // 2016.08.26
	//
	//==============================================================================================================
	return ERROR_NONE;
}

//int Scheduler_CONTROL_RECIPE_SETTING( int CHECKING_SIDE , char *RunAllRecipeName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int lc ) { // 2014.05.13
int Scheduler_CONTROL_RECIPE_SETTING( int CHECKING_SIDE , char *RunAllRecipeName , int StartSlot , int EndSlot , int *TMATM , int CPJOB , BOOL PreChecking , char *errorstring , int *lc ) { // 2014.05.13
	int res;
//	CLUSTERStepTemplate2 CLUSTER_RECIPE; // 2008.07.24
	Scheduler_CONTROL_CLUSTERStepTemplate2_Init( &G_CLUSTER_RECIPE );
	//
//	res = Scheduler_CONTROL_RECIPE_SETTING_Sub( &G_CLUSTER_RECIPE , CHECKING_SIDE , RunAllRecipeName , StartSlot , EndSlot , TMATM , CPJOB , PreChecking , errorstring , lc ); // 2014.05.13
	res = Scheduler_CONTROL_RECIPE_SETTING_Sub( &G_CLUSTER_RECIPE , CHECKING_SIDE , RunAllRecipeName , StartSlot , EndSlot , TMATM , CPJOB , PreChecking , errorstring , lc ); // 2014.05.13
	//
	Scheduler_CONTROL_CLUSTERStepTemplate2_Free( &G_CLUSTER_RECIPE );
	return res;
}


/*
// 2007.07.06
void Scheduler_CONTROL_RECIPE_SUB_HANDLINGSIDE_SETTING( int side , int pt , int ch , int trgmode , int *currmode , int *lrch , int mdlcnt , int progress , int *progress2 , BOOL intlksset ) {
	(*progress2)++;
	if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ) {
		if ( ( trgmode == HANDLING_ALL_SIDE ) && ( *currmode == HANDLING_ALL_SIDE ) ) {
			if ( ( mdlcnt <= 1 ) || ( *lrch != ( ch ) ) ) {
				*lrch = ( ch );
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY )
					*currmode = HANDLING_A_SIDE_ONLY;
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY )
					*currmode = HANDLING_B_SIDE_ONLY;
				//========
				if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
				//========
			}
			else {
				(*progress2)--;
				_i_SCH_CLUSTER_Set_PathProcessData( side , pt , progress , *progress2 , 0 , "" , "" , "" );
			}
		}
		else {
			if (
				( ( *currmode == HANDLING_A_SIDE_ONLY ) && ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY ) ) ||
				( ( *currmode == HANDLING_B_SIDE_ONLY ) && ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY ) )
				) {
				(*progress2)--;
				_i_SCH_CLUSTER_Set_PathProcessData( side , pt , progress , *progress2 , 0 , "" , "" , "" );
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_A_SIDE_ONLY )
					*currmode = HANDLING_A_SIDE_ONLY;
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( ch ) == HANDLING_B_SIDE_ONLY )
					*currmode = HANDLING_B_SIDE_ONLY;
				//========
				if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
				//========
			}
		}
	}
	else {
		if ( intlksset ) SCHEDULER_CONTROL_Set_Chamber_Use_Interlock_Pre( side , ch , TRUE );
	}
}
*/
/*
//
//
// 2018.11.22
//
BOOL Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_CONFLICT_CHECK( int CHECKING_SIDE , int Pt , int mode ) {
	int j , k , m;
	//
	if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) return FALSE; // 2007.10.29
	//
	for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
			if ( m > 0 ) {
				//
				if ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) { // 2018.10.04
					if      ( mode == HANDLING_A_SIDE_ONLY ) {
						if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_A_SIDE_ONLY ) return TRUE;
					}
					else if ( mode == HANDLING_B_SIDE_ONLY ) {
						if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_B_SIDE_ONLY ) return TRUE;
					}
				}
				else {
					if      ( mode == HANDLING_A_SIDE_ONLY ) {
						if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_B_SIDE_ONLY ) return TRUE;
					}
					else if ( mode == HANDLING_B_SIDE_ONLY ) {
						if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_A_SIDE_ONLY ) return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


int Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_GET_STYLE( int CHECKING_SIDE , int Pt ) {
	int j , k , m , ccsA , ccsB , ccsL , csA , csB , csL;
	//======================================================================================================
	ccsA = TRUE;
	ccsB = TRUE;
	ccsL = TRUE;
	//======================================================================================================
	for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
		csA = FALSE;
		csB = FALSE;
		csL = FALSE;
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
			csL = TRUE;
		}
		else {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
				if ( m > 0 ) {
					//
					if ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) { // 2018.10.04
						if      ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_B_SIDE_ONLY ) csA = TRUE;
						else if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_A_SIDE_ONLY ) csB = TRUE;
						else                                                                     csL = TRUE;
					}
					else {
						if      ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_A_SIDE_ONLY ) csA = TRUE;
						else if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) == HANDLING_B_SIDE_ONLY ) csB = TRUE;
						else                                                                     csL = TRUE;
					}
				}
				else if ( m < 0 ) { // 2012.07.24
					//
					if ( ( _i_SCH_PRM_GET_MODULE_GROUP( -m ) % 2 ) == 1 ) { // 2018.10.04
						if      ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( -m ) == HANDLING_B_SIDE_ONLY ) csA = TRUE;
						else if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( -m ) == HANDLING_A_SIDE_ONLY ) csB = TRUE;
						else                                                                     csL = TRUE;
					}
					else {
						if      ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( -m ) == HANDLING_A_SIDE_ONLY ) csA = TRUE;
						else if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( -m ) == HANDLING_B_SIDE_ONLY ) csB = TRUE;
						else                                                                     csL = TRUE;
					}
				}
			}
		}
		if ( !csA ) ccsA = FALSE;
		if ( !csB ) ccsB = FALSE;
		if ( !csL ) ccsL = FALSE;
	}
	//======================================================================================================
/ *
	if ( !csA && !csB &&  csL ) return 1;
	if ( !csA &&  csB && !csL ) return 2;
	if ( !csA &&  csB &&  csL ) return 3;
	if (  csA && !csB && !csL ) return 4;
	if (  csA && !csB &&  csL ) return 5;
	if (  csA &&  csB && !csL ) return 6;
	if (  csA &&  csB &&  csL ) return 7;
* /
	if ( !ccsA && !ccsB &&  ccsL ) return 1;
	if ( !ccsA &&  ccsB && !ccsL ) return 2;
	if ( !ccsA &&  ccsB &&  ccsL ) return 3;
	if (  ccsA && !ccsB && !ccsL ) return 4;
	if (  ccsA && !ccsB &&  ccsL ) return 5;
	if (  ccsA &&  ccsB && !ccsL ) return 6;
	if (  ccsA &&  ccsB &&  ccsL ) return 7;
	//======================================================================================================
	return 0;
}

int Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_CURRENT_CHECK( int CHECKING_SIDE , int Pt ) {
	int i , j , m , ch;
	int allch[MAX_PM_CHAMBER_DEPTH];
	if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
		//
		ch = HANDLING_ALL_SIDE;
		//
		for ( i = 0 ; i < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; i++ ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) allch[j] = 0;
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , i , j );
				if ( m > 0 ) allch[m-PM1] = 1;
			}
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j = j+2 ) {
				if     ( ( allch[j] == 1 ) && ( allch[j+1] == 0 ) ) {
					if      ( ch == HANDLING_ALL_SIDE ) {
						ch = HANDLING_A_SIDE_ONLY;
					}
					else if ( ch == HANDLING_B_SIDE_ONLY ) {
						_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( CHECKING_SIDE , Pt , i , j );
					}
				}
				else if ( ( allch[j] == 0 ) && ( allch[j+1] == 1 ) ) {
					if      ( ch == HANDLING_ALL_SIDE ) {
						ch = HANDLING_B_SIDE_ONLY;
					}
					else if ( ch == HANDLING_A_SIDE_ONLY ) {
						_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( CHECKING_SIDE , Pt , i , j );
					}
				}
			}
			//
		}
	}
	else {
		ch = _i_SCH_CLUSTER_Get_PathHSide( CHECKING_SIDE , Pt );
	}
	//
	switch( ch ) {
	case HANDLING_A_SIDE_ONLY :
		return HANDLING_A_SIDE_ONLY;
		break;
	case HANDLING_B_SIDE_ONLY :
		return HANDLING_B_SIDE_ONLY;
		break;
	default :
		switch( Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_GET_STYLE( CHECKING_SIDE , Pt ) ) {
		case 1 :	return HANDLING_ALL_SIDE;		break;
		case 2 :	return HANDLING_B_SIDE_ONLY;	break;
		case 3 :	return HANDLING_END_SIDE;		break;
		case 4 :	return HANDLING_A_SIDE_ONLY;	break;
		case 5 :	return HANDLING_END_SIDE;		break;
		case 6 :	return HANDLING_END_SIDE;		break;
		case 7 :	return HANDLING_END_SIDE;		break;
		default :	return -1;	break;
		}
		break;
	}
	return -1;
}

int Scheduler_CONTROL_RECIPE_DOUBLE_GET_MULTI_DATA( int CHECKING_SIDE , int Pt , int old_SelRs ) {
	int j , k , m , result , data;
	//======================================================================================================
	data = Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_GET_STYLE( CHECKING_SIDE , Pt );
	//======================================================================================================
	switch( data ) {
	//==============================================================
	case 3 : // ( !csA &&  csB &&  csL )
		if      ( old_SelRs == -1 ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == -2 ) {
			result = HANDLING_B_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_A_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_B_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_ALL_SIDE ) {
			result = HANDLING_ALL_SIDE;
		}
		break;
	//==============================================================
	case 5 : // (  csA && !csB &&  csL )
		if      ( old_SelRs == -1 ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == -2 ) {
			result = HANDLING_A_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_A_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_B_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_ALL_SIDE ) {
			result = HANDLING_ALL_SIDE;
		}
		break;
	//==============================================================
	case 6 : // (  csA &&  csB && !csL )
		if      ( old_SelRs == -1 ) {
			result = HANDLING_A_SIDE_ONLY;
		}
		else if ( old_SelRs == -2 ) {
			result = HANDLING_B_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_A_SIDE_ONLY ) {
			result = HANDLING_B_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_B_SIDE_ONLY ) {
			result = HANDLING_A_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_ALL_SIDE ) {
			result = HANDLING_A_SIDE_ONLY;
		}
		break;
	//==============================================================
	case 7 : // (  csA &&  csB &&  csL )
		if      ( old_SelRs == -1 ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == -2 ) {
			result = HANDLING_A_SIDE_ONLY;
		}
		else if ( old_SelRs == HANDLING_A_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_B_SIDE_ONLY ) {
			result = HANDLING_ALL_SIDE;
		}
		else if ( old_SelRs == HANDLING_ALL_SIDE ) {
			result = HANDLING_ALL_SIDE;
		}
		break;
	//==============================================================
	case 1 :	return HANDLING_ALL_SIDE;		break;
	case 2 :	return HANDLING_B_SIDE_ONLY;	break;
	case 4 :	return HANDLING_A_SIDE_ONLY;	break;
	default :	return -1;						break;
	//==============================================================
	}
	//======================================================================================================
	for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
			if ( m > 0 ) {
				//
				if ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) { // 2018.10.04
					switch( result ) {
					case HANDLING_A_SIDE_ONLY :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_B_SIDE_ONLY :
							break;
						case HANDLING_A_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						default :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						}
						break;
					case HANDLING_B_SIDE_ONLY :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_B_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						case HANDLING_A_SIDE_ONLY :
							break;
						default :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						}
						break;
					default :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_B_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						case HANDLING_A_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						default :
							break;
						}
						break;
					}
				}
				else {
					switch( result ) {
					case HANDLING_A_SIDE_ONLY :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_A_SIDE_ONLY :
							break;
						case HANDLING_B_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						default :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						}
						break;
					case HANDLING_B_SIDE_ONLY :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_A_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						case HANDLING_B_SIDE_ONLY :
							break;
						default :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						}
						break;
					default :
						switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
						case HANDLING_A_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						case HANDLING_B_SIDE_ONLY :
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							break;
						default :
							break;
						}
						break;
					}
				}
			}
		}
	}
	//======================================================================================================
	return result;
}

BOOL Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING( int CHECKING_SIDE ) {
	int Pt , old_Pt , old_SelRs;
	//
	if ( !_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 9 , CHECKING_SIDE , 0 , 0 , 0 ) ) return TRUE; // 2015.05.07
	//
	old_Pt = -1;
	old_SelRs = -1;

	for ( Pt = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) {

		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) > 0 ) {

			switch( Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_CURRENT_CHECK( CHECKING_SIDE , Pt ) ) {
			case -1 :
				return FALSE;
				break;
			case HANDLING_A_SIDE_ONLY :
				if ( old_Pt != -1 ) _i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , old_Pt , HANDLING_END_SIDE );
				if ( Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_CONFLICT_CHECK( CHECKING_SIDE , Pt , HANDLING_A_SIDE_ONLY ) ) return FALSE;
				_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , HANDLING_A_SIDE_ONLY );
				old_Pt = -1;
				old_SelRs = -1;
				break;

			case HANDLING_B_SIDE_ONLY :
				if ( old_Pt != -1 ) _i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , old_Pt , HANDLING_END_SIDE );
				if ( Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_CONFLICT_CHECK( CHECKING_SIDE , Pt , HANDLING_B_SIDE_ONLY ) ) return FALSE;
				_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , HANDLING_B_SIDE_ONLY );
				old_Pt = -1;
				old_SelRs = -1;
				break;

			case HANDLING_ALL_SIDE :
				if ( old_Pt == -1 ) {
					old_Pt = Pt;
					old_SelRs = -1;
				}
				else {
					if ( !_i_SCH_SUB_Check_ClusterIndex_CPJob_Same( CHECKING_SIDE , old_Pt , CHECKING_SIDE , Pt ) ) {
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , old_Pt , HANDLING_END_SIDE );
						old_Pt = Pt;
						old_SelRs = -1;
					}
					else {
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , old_Pt , HANDLING_ALL_SIDE );
						old_Pt = Pt;
						old_SelRs = -1;
					}
				}
				break;

			default : // multi
				if ( old_Pt == -1 ) {
					old_SelRs = Scheduler_CONTROL_RECIPE_DOUBLE_GET_MULTI_DATA( CHECKING_SIDE , Pt , old_SelRs );
					if ( old_SelRs == HANDLING_ALL_SIDE ) {
						old_Pt = Pt;
					}
					else {
						old_Pt = -1;
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , old_SelRs );
					}
				}
				else {
					if ( !_i_SCH_SUB_Check_ClusterIndex_CPJob_Same( CHECKING_SIDE , old_Pt , CHECKING_SIDE , Pt ) ) {
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , old_Pt , HANDLING_END_SIDE );
						old_SelRs = Scheduler_CONTROL_RECIPE_DOUBLE_GET_MULTI_DATA( CHECKING_SIDE , Pt , -1 );
						if ( old_SelRs == HANDLING_ALL_SIDE ) {
							old_Pt = Pt;
						}
						else {
							old_Pt = -1;
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , old_SelRs );
						}
					}
					else {
						old_SelRs = Scheduler_CONTROL_RECIPE_DOUBLE_GET_MULTI_DATA( CHECKING_SIDE , Pt , old_SelRs );
						if ( old_SelRs == HANDLING_ALL_SIDE ) {
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , HANDLING_END_SIDE );
							old_SelRs = -2;
						}
						else {
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , old_SelRs );
						}
						old_Pt = -1;
					}
				}
				break;
			}
		}
	}
	return TRUE;
}
//
*/
//

// 2018.11.22


int Scheduler_CONTROL_RECIPE_DOUBLE_GET_HSIDE_DATA( int hside , int m ) {
	//
	switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( m ) ) {
	case HANDLING_A_SIDE_ONLY :
		//
		switch( hside ) {
		case HANDLING_B_SIDE_ONLY :
			return -1;
			break;
		}
		//
		if ( ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) ) { // B
			return HANDLING_B_SIDE_ONLY;
		}
		//
		return HANDLING_A_SIDE_ONLY;
		break;
		//
	case HANDLING_B_SIDE_ONLY :
		//
		switch( hside ) {
		case HANDLING_A_SIDE_ONLY :
			return -1;
			break;
		}
		//
		if ( ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) ) { // A
			return HANDLING_A_SIDE_ONLY;
		}
		//
		return HANDLING_B_SIDE_ONLY;
		break;
		//
	default :
		//
		switch( hside ) {
		case HANDLING_A_SIDE_ONLY :
			//
			if ( ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) ) { // B
				return HANDLING_B_SIDE_ONLY;
			}
			//
			return HANDLING_A_SIDE_ONLY;
			break;

		case HANDLING_B_SIDE_ONLY :
			//
			if ( ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( ( _i_SCH_PRM_GET_MODULE_GROUP( m ) % 2 ) == 1 ) ) { // A
				return HANDLING_A_SIDE_ONLY;
			}
			//
			return HANDLING_B_SIDE_ONLY;
			break;
		}
		//
	}
	//
	return HANDLING_ALL_SIDE;
}


void Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING_WITH_SELECT( int CHECKING_SIDE , int Pt , int Select_hSide , int org_hSide ) {
	int chSide , j , k , m;
	//
	for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
			//
			if ( m > 0 ) {
				//
				chSide = Scheduler_CONTROL_RECIPE_DOUBLE_GET_HSIDE_DATA( org_hSide , m );
				//
				if ( chSide != Select_hSide ) {
					_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
				}
				//
			}
		}
		//
	}
}
//

BOOL Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING( int CHECKING_SIDE ) {
	int Pt , Last_hSide , org_hSide , chSide , j , k , m;
	int pmhside_a , pmhside_b , pmhside_l , findok , finddel;
	//
	//
	//
	//			all	:	all		0
	//	a			:	a		1
	//		b		:	b		2
	//
	//	a	b		:	mix		3
	//	a		all	:	mix		4
	//		b	all	:	mix		5
	//	a	b	all	:	mix		6
	//
	//
	if ( !_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 9 , CHECKING_SIDE , 0 , 0 , 0 ) ) return TRUE; // 2015.05.07
	//
	for ( Pt = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) <= 0 ) continue;
		//
		//===============================================================================================================================================
		//
		org_hSide = _i_SCH_CLUSTER_Get_PathHSide( CHECKING_SIDE , Pt ) % 10;
		//
		//
		Last_hSide = -1;
		//
		for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
			//
			pmhside_a = 0;
			pmhside_b = 0;
			pmhside_l = 0;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
				//
				if ( m > 0 ) {
					//
					switch( Scheduler_CONTROL_RECIPE_DOUBLE_GET_HSIDE_DATA( org_hSide , m ) ) {
					case HANDLING_A_SIDE_ONLY :
						pmhside_a++;
						break;

					case HANDLING_B_SIDE_ONLY :
						pmhside_b++;
						break;

					default :
						pmhside_l++;
						break;
					}
					//
				}
			}
			//
	//			all	:	all		0
	//	a			:	a		1
	//		b		:	b		2
	//
	//	a	b		:	mix		3
	//	a		all	:	mix		4
	//		b	all	:	mix		5
	//	a	b	all	:	mix		6
			//
			if ( Last_hSide == -1 ) {
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 3;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 4;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 5;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 6;
				}
				else {
					Last_hSide = 0;
				}
			}
			else if ( Last_hSide == 1 ) { // a
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					return FALSE;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
				}
			}
			else if ( Last_hSide == 2 ) { // b
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					return FALSE;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
				}
			}
			else if ( Last_hSide == 3 ) { //	a	b		:	mix		3
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					return FALSE;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
					return FALSE;
				}
			}
			else if ( Last_hSide == 4 ) { //	a		all	:	mix		4
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					return FALSE;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
					return FALSE;
				}
			}
			else if ( Last_hSide == 5 ) { //		b	all	:	mix		5
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					return FALSE;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
					return FALSE;
				}
			}
			else if ( Last_hSide == 6 ) { //	a	b	all	:	mix		6
				if      ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 1;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 2;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 0;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l <= 0 ) ) {
					Last_hSide = 3;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b <= 0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 4;
				}
				else if ( ( pmhside_a <= 0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
					Last_hSide = 5;
				}
				else if ( ( pmhside_a >  0 ) && ( pmhside_b >  0 ) && ( pmhside_l >  0 ) ) {
				}
				else {
					return FALSE;
				}
			}
			//
		}
		//
		//===============================================================================================================================================
		//
		if ( Last_hSide == -1 ) return FALSE;
		//
		for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
			//
			findok = 0;
			finddel = 0;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k );
				//
				if ( m > 0 ) {
					//
	//			all	:	all		0
	//	a			:	a		1
	//		b		:	b		2
	//
	//	a	b		:	mix		3
	//	a		all	:	mix		4
	//		b	all	:	mix		5
	//	a	b	all	:	mix		6
	//
					chSide = Scheduler_CONTROL_RECIPE_DOUBLE_GET_HSIDE_DATA( org_hSide , m );
					//
					switch( Last_hSide ) {
					case 0 : //			all	:	all		0
						//
						switch( chSide ) {
						case HANDLING_ALL_SIDE :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
							//
						}
						//
						break;

					case 1 : //	a			:	a		1

						switch( chSide ) {
						case HANDLING_A_SIDE_ONLY :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;

					case 2 : //		b		:	b		2

						switch( chSide ) {
						case HANDLING_B_SIDE_ONLY :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;

					case 3 : //	a	b		:	mix		3

						switch( chSide ) {
						case HANDLING_A_SIDE_ONLY :
						case HANDLING_B_SIDE_ONLY :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;

					case 4 : //	a		all	:	mix		4

						switch( chSide ) {
						case HANDLING_A_SIDE_ONLY :
						case HANDLING_ALL_SIDE :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;

					case 5 : //		b	all	:	mix		5

						switch( chSide ) {
						case HANDLING_B_SIDE_ONLY :
						case HANDLING_ALL_SIDE :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;

					case 6 : //	a	b	all	:	mix		6

						switch( chSide ) {
						case HANDLING_A_SIDE_ONLY :
						case HANDLING_B_SIDE_ONLY :
						case HANDLING_ALL_SIDE :
							//
							findok++;
							//
							break;

						default :
							//
							finddel++;
							//
							_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , j , k , 0 , "" , "" , "" );
							//
							break;
						}
						//
						break;
					}
				}
			}
			//
			//===============================================================================================================================================
			//
			if ( ( finddel > 0 ) && ( findok <= 0 ) ) return FALSE;
			//
			//===============================================================================================================================================
			//
		}
		//
	//			all	:	all		0
	//	a			:	a		1
	//		b		:	b		2
	//
	//	a	b		:	mix		3
	//	a		all	:	mix		4
	//		b	all	:	mix		5
	//	a	b	all	:	mix		6
		//
//	********
//	-			:	초기값
//	 -			:	종류 0:ALL,A,B  1:MIX_AB  2:MIX_AL  3:MIX_BL  4:MIX_ABL
//	  ------	:	상태 0:ALL  1:A  2:B  9:NotDefine
		//
		if ( Last_hSide >= 3 ) {
			_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , ( org_hSide * 10000000 ) + ( ( Last_hSide - 2 ) * 1000000 ) + 9 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , ( org_hSide * 10000000 ) + Last_hSide );
		}
		//
	}
	return TRUE;
}
//

int Scheduler_CONTROL_RECIPE_SETTING_AFTER_MAPPING( int CHECKING_SIDE , BOOL rcpcheck ) {
	int Pt;
	//
	int j , k , n , np;
	BOOL FindCheck;
	int  Clmode;
	int c , p;
	int MaxSlot , maxunuse;
	char JobName[256];
//	char MIDName[256]; // 2013.10.23
//	char PPIDName[256]; // 2011.09.01

	if ( !rcpcheck ) {
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// (POST Data Remap) 3.PM Side Handling data remapping for TM Double Style(only use)(Move Here : 2002.07.11)
		//------------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
			//=============================================================================
			// 2007.07.06
			//=============================================================================
			if ( !Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING( CHECKING_SIDE ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] Double Chamber Config and Cluster Setting is invalid%cMAPFAIL\n" , 9 );
				return FALSE;
			}
			//=============================================================================
		}
		else {
			//
			//
			/*
			//
			//
			// 2018.11.22
			//
			for ( n = -1 , Clmode = 1 , Pt = 0 ; Pt < MAX_CASSETTE_SLOT_SIZE ; Pt++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) >= 0 ) {
					if ( n == -1 ) {
						_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
						n = Pt;
					}
					else if ( n >= 0 ) {
						FindCheck = TRUE;
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) != _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , n ) ) FindCheck = FALSE;
						if ( FindCheck ) {
							for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , Pt ) ; j++ ) {
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									if ( _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , j , k ) != _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , n , j , k ) ) {
										FindCheck = FALSE;
										j = 999999;
										break;
									}
									if ( !STRCMP_L( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , n , j , k , 0 ) ) ) {
										FindCheck = FALSE;
										j = 999999;
										break;
									}
									if ( !STRCMP_L( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , n , j , k , 1 ) ) ) {
										FindCheck = FALSE;
										j = 999999;
										break;
									}
									if ( !STRCMP_L( _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , Pt , j , k , 2 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , n , j , k , 2 ) ) ) {
										FindCheck = FALSE;
										j = 999999;
										break;
									}
								}
							}
						}
						if ( FindCheck ) {
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
						}
						else {
							n = Pt;
							Clmode++;
							_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , Clmode );
						}
					}
				}
			}
			//
			*/
			//
			//------------------------------------------------------------------------------
			// (POST Data Check) Robot Get Ordering Check (2003.10.06)
			//------------------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) {
				j = -1;
				n = -1;
				for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
					if ( j == -1 ) {
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , p ) >= 0 ) {
							j = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p );
							k = _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , p );
							n = j;
						}
					}
					else {
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , p ) >= 0 ) {
							n = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p );
						}
					}
				}
				if ( j == -1 ) return FALSE;
				if ( n == -1 ) return FALSE;
				for ( p = j ; p <= n ; p++ ) {
					switch( _i_SCH_IO_GET_MODULE_STATUS( k , p ) ) {
					case CWM_PRESENT :
					case CWM_ABSENT :
						break;
					default :
						return FALSE;
						break;
					}
				}
			}
			//------------------------------------------------------------------------------
		}

		//=============================================================================
		// 2006.08.04 // 2008.04.22
		//=============================================================================
		if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
			//------------------------
			MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , CHECKING_SIDE + 1 , CHECKING_SIDE + 1 , 2 , &maxunuse ); // 2010.12.17
			//------------------------
			Pt = -1;
			//------------------------
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
					np = _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , j );
					Pt = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , j );
					Clmode = j;
				}
			}
			//------------------------
			if ( Pt != -1 ) {
				p = ( Pt % ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) );
				if ( p != 0 ) {
					FindCheck = FALSE;
					c = ( ( Pt / ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) + 1 ) * ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
					if ( c > MaxSlot ) c = MaxSlot; // 2010.12.17
					for ( j = ( Pt + 1 ) ; j <= c ; j++ ) {
						for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
							if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
								if ( j == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , j ) ) {
									if ( np == _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , j ) ) {
										FindCheck = TRUE;
										break;
									}
								}
							}
						}
						//---------
						if ( FindCheck ) break;
						//---------
					}
					//=======================================================================
					if ( !FindCheck ) {
						for ( j = ( Pt + 1 ) ; j <= c ; j++ ) {
							k = _i_SCH_IO_GET_MODULE_STATUS( np , j );
							if ( ( k != CWM_UNKNOWN ) && ( k != CWM_ABSENT ) ) {
								if ( ( Clmode + 1 ) < MAX_CASSETTE_SLOT_SIZE ) {
									//
									for ( n = 0 ; n < MAX_CASSETTE_SLOT_SIZE ; n++ ) { // 2009.04.30
										if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , n ) >= 0 ) {
	//										if ( ( Clmode + 1 ) == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , n ) ) break; // 2009.05.06
											if ( j == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , n ) ) break; // 2009.05.06
										}
									}
									if ( n == MAX_CASSETTE_SLOT_SIZE ) {
										_i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Clmode + 1 , np );
										_i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Clmode + 1 , _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , Clmode ) );
										_i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Clmode + 1 , j );
										_i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Clmode + 1 , j );
										_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Clmode + 1 , 0 );
										//
										_i_SCH_CASSETTE_LAST_RESET( np , j ); // 2008.04.21
										_i_SCH_CASSETTE_LAST_RESET2( CHECKING_SIDE , Clmode + 1 ); // 2011.04.21
										//
										Clmode++;
									}
								}
							}
						}
					}
					//=============================================================================
				}
			}
			//=============================================================================
			Pt = 0;
			while( TRUE ) {
				//---------------------------------------------------
				FindCheck = FALSE;
				c = 0;
				np = 0;
				//---------------------------------------------------
				for ( j = 0 ; j < ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ; j++ ) {
					//=======================================================================
					p = ( Pt * ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) + j + 1;
					if ( p > MaxSlot ) { // 2010.12.17
						FindCheck = TRUE;
						break;
					}
					//=======================================================================
					for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , k ) > 0 ) {
							if ( p == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , k ) ) {
								c++;
							}
						}
						else if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , k ) == 0 ) {
							if ( p == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , k ) ) {
								np++;
							}
						}
					}
					//=======================================================================
				}
				//---------------------------------------------------
				if ( ( np > 0 ) && ( c == 0 ) ) {
					for ( j = 0 ; j < ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ; j++ ) {
						p = ( Pt * ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) + j + 1;
						if ( p > MaxSlot ) break; // 2010.12.17
						for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
							if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , k ) == 0 ) {
								if ( p == _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , k ) ) {
									_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , k , -1 );
								}
							}
						}
					}
				}
				//---------------------------------------------------
				if ( FindCheck ) break;
				//---------------------------------------------------
				Pt++;
				//---------------------------------------------------
			}
		}
	}
	//==============================================================================================================
	// 2011.08.10
	//==============================================================================================================
	//
	IO_GET_JID_NAME( CHECKING_SIDE , JobName );
//	_i_SCH_IO_GET_RECIPE_FILE( CHECKING_SIDE , PPIDName ); // 
//	IO_GET_MID_NAME( CHECKING_SIDE , MIDName ); // 2012.04.01
	//
	for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) { // 2012.04.01
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , p ) >= 0 ) {
			//
			_i_SCH_CLUSTER_Set_Ex_OutMaterialID( CHECKING_SIDE , p , "" );
			_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( CHECKING_SIDE , p , -1 );
			//
		}
	}
	//
	//==============================================================================================================
//	SCHMRDATA_Data_Setting_for_Starting( CHECKING_SIDE ); // 2011.09.14 // 2012.08.30
	//==============================================================================================================

	//==============================================================================================================
	// 2013.09.03
	//==============================================================================================================
	if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 2 ) {
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , p ) < 0 ) continue;
			//
			FindCheck = 0;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				n = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , p , 0 , k );
				//
				if ( n > 0 ) {
					//
					if ( _i_SCH_MODULE_GET_MOVE_MODE( n ) == 0 ) {
						_i_SCH_CLUSTER_Set_PathProcessChamber( CHECKING_SIDE , p , 0 , k , 0 );
						continue;
					}
					//
					if ( FindCheck != 0 ) {
						_i_SCH_CLUSTER_Set_PathProcessChamber( CHECKING_SIDE , p , 0 , k , 0 );
					}
					else {
						for ( c = 1 ; c <= MAX_PM_SLOT_DEPTH ; c++ ) {
							//
							if ( ( FindCheck == 0 ) && _i_SCH_IO_GET_MODULE_STATUS( n , c ) > 0 ) {
								//
								FindCheck = 1;
								//
								if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
									_i_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
								}
								//
								_i_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
								//
								_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , p , SCHEDULER_RUNNING2 );
								_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , p , 1 );
								_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , p , 1 );
								//
								_i_SCH_MODULE_Set_PM_SIDE( n , c - 1 , CHECKING_SIDE );
								_i_SCH_MODULE_Set_PM_POINTER( n , c - 1 , p );
								_i_SCH_MODULE_Set_PM_WAFER( n , c - 1 , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p ) );
								//
							}
							else {
								_i_SCH_MODULE_Set_PM_WAFER( n , c - 1 , 0 );
							}
							//
						}
						//
						if ( FindCheck != 1 ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber( CHECKING_SIDE , p , 0 , k , 0 );
						}
						else {
							FindCheck = 2;
						}
					}
					//
				}
				else {
					_i_SCH_CLUSTER_Set_PathProcessChamber( CHECKING_SIDE , p , 0 , k , 0 );
				}
			}
			//
			if ( !FindCheck ) {
				_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , p , -19 );
			}
			//
		}
	}
	//==============================================================================================================
	// 2005.03.02
	//==============================================================================================================

	for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , p ) >= 0 ) {
			//
			Scheduler_Mapping_Buffer_Data_Set( CHECKING_SIDE , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p ) - 1 , 3 , 1 );
			//
			// 2011.08.10
			//
			_i_SCH_CLUSTER_Set_Ex_JobName( CHECKING_SIDE , p , JobName );
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
				_i_SCH_CLUSTER_Set_Ex_MtlOut( CHECKING_SIDE , p , 0 );
			}
			else {
				_i_SCH_CLUSTER_Set_Ex_MtlOut( CHECKING_SIDE , p , _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			}
			//
			_i_SCH_CLUSTER_Set_Ex_MtlOutWait( CHECKING_SIDE , p , MR_PRE_0_READY ); // 2016.11.02
			//
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tSET : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [CID=%d][JID=%s][MID=%s]\n" , CHECKING_SIDE , p , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , p ) , c , JobName , MIDName ); // 2011.09.01
//_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tSET : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [JID=%s][MID=%s]\n" , CHECKING_SIDE , p , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , p ) , JobName , MIDName ); // 2011.09.14 // 2013.10.23
_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tSET : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [JID=%s]\n" , CHECKING_SIDE , p , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , p ) , _i_SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , p ) , JobName ); // 2011.09.14 // 2013.10.23
//------------------------------------------------------------------------------------------------------------------
		}
		//
	}
	//==============================================================================================================
	SCHMRDATA_Data_Setting_for_Starting( CHECKING_SIDE ); // 2011.09.14 // 2012.08.30
	//==============================================================================================================
	return TRUE;
}




int Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE_Sub( int mode , int UserMode0 , int mdlchk , int CHECKING_SIDE , int Pt0 , int Chamber , int Slot , char *clusterRecipefile , CLUSTERStepTemplate2 *CLUSTER_RECIPE , char *grpname ) {
	int i , j , k , m , cldepth , pmdepth , setpm , pmcount;
	int UserMode , Pt;
	int pmuse[MAX_PM_CHAMBER_DEPTH];
	int n , n2;
	//
	CLUSTERStepTemplate3 CLUSTER_RECIPE_SPECIAL;
	//====================================================================================================
	if ( UserMode0 == -1 ) {
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
			UserMode = USER_RECIPE_DATA_MODE();
		}
		else {
			UserMode = USER_RECIPE_MANUAL_DATA_MODE();
		}
	}
	else {
		UserMode = UserMode0;
	}
	if ( ( UserMode < 0 ) || ( UserMode > 8 ) ) UserMode = 0;
	//====================================================================================================
	// Cluster Recipe Read
	//====================================================================================================
	if ( ( UserMode == 0 ) || ( UserMode == 2 ) || ( UserMode == 3 ) || ( UserMode == 5 ) || ( UserMode == 6 ) || ( UserMode == 8 ) ) {
		//--------------------------------------------------------------------------------------------------
		if ( !RECIPE_FILE_CLUSTER_DATA_READ( CHECKING_SIDE , CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , grpname , Slot , clusterRecipefile , SCHEDULER_Get_MULTI_RECIPE_SELECT() , &i , &i , NULL ) ) {
			if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 1 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
			if ( mode == 1 ) return 1;
			return ERROR_CLST_RECIPE_DATA;
		}
		//--------------------------------------------------------------------------------------------------
		if ( Pt0 != -1 ) {
			//
			_i_SCH_CLUSTER_Set_ClusterSpecial( CHECKING_SIDE , Pt0 , ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA <= 0 ) ? 0 : ( CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA - 1 ) ); // 2014.06.23
			//
			_i_SCH_CLUSTER_Set_ClusterUserSpecial( CHECKING_SIDE , Pt0 , CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
			if ( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.SPECIAL_USER_DATA );
			//--------------------------------------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_ClusterTuneData( CHECKING_SIDE , Pt0 , CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
			if ( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.RECIPE_TUNE_DATA );
			//--------------------------------------------------------------------------------------------------
			Scheduler_CONTROL_Set_PPID( CHECKING_SIDE , Pt0 , FALSE , clusterRecipefile ); // 2011.09.01
			//--------------------------------------------------------------------------------------------------
			//-----------------------------------------------------------------------
			// 2011.12.08
			_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( CHECKING_SIDE , Pt0 , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_MODE );
			_i_SCH_CLUSTER_Set_Ex_UserControl_Data( CHECKING_SIDE , Pt0 , CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
			if ( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA != NULL ) free( CLUSTER_RECIPE_SPECIAL.USER_CONTROL_DATA );
			//---------------------------------
			//--------------------------------------------------------------------------------------------------
			//-----------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------
	}
	if ( ( UserMode == 1 ) || ( UserMode == 2 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
//			Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , -1 ); // 2014.06.23
			Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , -1 , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2014.06.23
			if ( !USER_RECIPE_CLUSTER_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot , clusterRecipefile ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 2 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 2;
				return ERROR_CLST_RECIPE_DATA;
			}
			//
			if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt0 ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 3 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 3;
				return ERROR_CLST_RECIPE_DATA;
			}
			// 2012.06.18
			if ( !USER_RECIPE_CLUSTER_DATA_READ2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , CLUSTER_RECIPE , Slot , clusterRecipefile ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 3-2 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 2;
				return ERROR_CLST_RECIPE_DATA;
			}
			//
		}
		else {
//			Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , -1 ); // 2014.06.23
			Scheduler_CONTROL_CLUSTERStepTemplate12_Set( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , -1 , CLUSTER_RECIPE_SPECIAL.SPECIAL_INSIDE_DATA ); // 2014.06.23
			if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &G_CLUSTER_RECIPE_INF , Slot , clusterRecipefile ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 4 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 4;
				return ERROR_CLST_RECIPE_DATA;
			}
			if ( !Scheduler_CONTROL_CLUSTERStepTemplate12_ReSet( &G_CLUSTER_RECIPE_INF , &G_CLUSTER_RECIPE_INF_ORG , CLUSTER_RECIPE , CHECKING_SIDE , Pt0 ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 5 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 5;
				return ERROR_CLST_RECIPE_DATA;
			}
			// 2012.06.18
			if ( !USER_RECIPE_MANUAL_CLUSTER_DATA_READ2( CHECKING_SIDE , CHECKING_SIDE , CLUSTER_RECIPE , Slot , clusterRecipefile ) ) {
				if ( Pt0 != -1 ) _i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt0 , -1 );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 5-2 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
				if ( mode == 1 ) return 4;
				return ERROR_CLST_RECIPE_DATA;
			}
			//
		}
	}
	//====================================================================================================
	if ( mdlchk != 2 ) { // 2011.05.12
		//
		if ( Pt0 == -1 ) {
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
				if ( Slot != _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) ) continue;
				if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) , 0 ) ) continue;
				//
				switch( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) ) {
				case SCHEDULER_READY	:
					break;
				case SCHEDULER_SUPPLY	:
					if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) return -1;
					break;
				default	:
					return -1;
					break;
				}
				//
				Pt = i;
				break;
			}
			//
			if ( i == MAX_CASSETTE_SLOT_SIZE ) return -1;
			//
		}
		else {
			//
			Pt = Pt0;
			//
			//-----------------------------------------------------------------------
			if ( mdlchk == 0 ) { // 2010.09.15
				//-----------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_ClusterIndex( CHECKING_SIDE , Pt , SCHEDULER_Make_CLUSTER_INDEX_SELECT( CHECKING_SIDE , _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , Pt ) , Slot , 0 , "" , clusterRecipefile , CLUSTER_RECIPE , FALSE ) );
				//-----------------------------------------------------------------------
			}
			//-----------------------------------------------------------------------
			if ( ( CLUSTER_RECIPE->EXTRA_TIME >= 0 ) && ( CLUSTER_RECIPE->EXTRA_TIME <= 99999 ) ) { // 2008.10.31
				if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
					_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) + ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) );
				}
				else {
					_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_TIME * 10000 ) );
				}
			}
			else {
				if ( ( CLUSTER_RECIPE->EXTRA_STATION >= 0 ) && ( CLUSTER_RECIPE->EXTRA_STATION <= 9999 ) ) { // 2011.04.15
					_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , ( CLUSTER_RECIPE->EXTRA_STATION % 10000 ) ); // 2006.02.06
				}
				else {
					_i_SCH_CLUSTER_Set_Extra( CHECKING_SIDE , Pt , 0 ); // 2006.02.06
				}
			}
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Clear_PathProcessData( CHECKING_SIDE , Pt , TRUE );
			//-----------------------------------------------------------------------
		}
	}
	//-----------------------------------------------------------------------
	if ( mdlchk != 2 ) { // 2011.05.12
		//
		cldepth = 0;
		//
		if ( Pt0 != -1 ) { // 2011.06.21
			_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL );
		}
		for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
			//------------------------------------------------------------------------
			pmdepth = 0;
			pmcount = 0;
			//
	//		if ( Pt0 != -1 ) { // 2011.06.10 // 2011.06.21
	//			_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , -1 , NULL ); // 2011.06.21
	//		} // 2011.06.21
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				if ( CLUSTER_RECIPE->MODULE[i][j] <= 0 ) continue;
				if ( !_i_SCH_PRM_GET_MODULE_MODE( j + PM1 ) ) continue;
				//
				if ( Pt0 == -1 ) {
					//
					pmdepth = -1;
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , i , k );
						if ( ( m == ( j + PM1 ) ) || ( -m == ( j + PM1 ) ) ) {
							pmdepth = k;
							setpm = m;
							break;
						}
					}
					//
					if ( pmdepth == -1 ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							if ( _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , i , k ) == 0 ) {
								pmdepth = k;
								setpm = j + PM1;
								break;
							}
						}
						if ( pmdepth == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_CLUSTER %s Read Error 6 at %d,%d%cRECIPEFAIL %s:%d\n" , clusterRecipefile , Pt0 , Slot , 9 , clusterRecipefile , ERROR_CLST_RECIPE_DATA );
							return ERROR_CLST_RECIPE_DATA;
						}
						//
						m = 0;
						//
						if ( mdlchk == 1 ) { // 2010.09.15
							switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j + PM1 ) ) {
							case MUF_00_NOTUSE :
								if ( !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE , CHECKING_SIDE ) ) {
									setpm = -setpm;
									m = 2;
								}
								else {
									m = 1;
								}
								break;
							case MUF_01_USE :
								if ( !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE , CHECKING_SIDE ) ) setpm = -setpm;
								break;
							case MUF_02_USE_to_END :
							case MUF_03_USE_to_END_DISABLE :
							case MUF_04_USE_to_PREND_EF_LP :
							case MUF_05_USE_to_PRENDNA_EF_LP :
							case MUF_07_USE_to_PREND_DECTAG :
							case MUF_08_USE_to_PRENDNA_DECTAG :
							case MUF_31_USE_to_NOTUSE_SEQMODE :
							case MUF_71_USE_to_PREND_EF_XLP :
								if ( !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE , CHECKING_SIDE ) ) {
									setpm = -setpm;
									m = 2;
								}
								else {
									m = 1;
								}
								break;
							case MUF_81_USE_to_PREND_RANDONLY :
							case MUF_90_USE_to_XDEC_FROM :
							case MUF_97_USE_to_SEQDIS_RAND :
							case MUF_98_USE_to_DISABLE_RAND :
							case MUF_99_USE_to_DISABLE :
								continue;
								break;
							}
							//
							if ( m != 0 ) {
								Scheduler_Begin_One( CHECKING_SIDE , j + PM1 );
								if ( m == 1 ) {
									k = _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE );
									SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( CHECKING_SIDE , k + 1 );
								}
							}
							//
						}
					}
				}
				else {
					//
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE( j + PM1 , FALSE , -1 ) ) continue; // 2011.06.23
					//
					if ( mdlchk == 1 ) { // 2010.09.15
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j + PM1 ) != MUF_01_USE ) {
							if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , j + PM1 ) != MUF_99_USE_to_DISABLE ) { // 2011.06.23
								continue;
							}
						}
					}
					//
					setpm = j + PM1;
				}
				//
				_i_SCH_CLUSTER_Set_PathProcessData( CHECKING_SIDE , Pt , cldepth , pmdepth , setpm , CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] , CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] );
				//
				pmdepth++;
				pmcount++;
				//
			}
			//------------------------------------------------------------------------
			if ( pmcount > 0 ) {
				//
				if ( Pt0 != -1 ) {
//					_i_SCH_CLUSTER_Set_PathProcessParallel( CHECKING_SIDE , Pt , cldepth , NULL ); // 2011.06.10
					//
					Scheduler_CONTROL_Parallel_ReSetting( CHECKING_SIDE , Pt , cldepth , i , CLUSTER_RECIPE ); // 2011.06.10
					Scheduler_CONTROL_Parallel_ReSetting2( CHECKING_SIDE , Pt , cldepth , i , CLUSTER_RECIPE ); // 2014.01.28
					//
					_i_SCH_CLUSTER_Set_PathRun( CHECKING_SIDE , Pt , cldepth , -1 , 0 );
				}
				//
				cldepth++;
				//
			}
			//------------------------------------------------------------------------
		}
		//-----------------------------------------------------------------------
		if ( Pt0 != -1 ) { // 2011.06.23
			//
			for ( i = 0 ; i < cldepth ; i++ ) {
				//
				m = 0;
				n2 = 0;
				//
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					//
					n = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , i , j );
					//
					if ( n > 0 ) {
						//
//						if ( !_i_SCH_MODULE_GET_USE_ENABLE( n , FALSE , CHECKING_SIDE ) ) { // 2016.05.09
//						if ( !_i_SCH_MODULE_GET_USE_ENABLE( n , ( _i_SCH_MODULE_Get_PM_WAFER( n , -1 ) > 0 ) , CHECKING_SIDE ) ) { // 2016.05.09 // 2018.12.20
						if ( !_i_SCH_MODULE_GET_USE_ENABLE( n , ( _i_SCH_MODULE_Get_PM_WAFER( n , -1 ) > 0 ) , CHECKING_SIDE + 1000 ) ) { // 2016.05.09 // 2018.12.20
							//
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( CHECKING_SIDE , Pt , i , j );
							//
							n2++;
							//
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , n , MUF_99_USE_to_DISABLE );
							//
						}
						else {
							m++;
						}
						//
					}
				}
				/*
				// 2011.07.12
				if ( m <= 0 ) {
					switch( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() ) {
					case 1 : // Disable
					case 2 : // DisableHW
					case 3 : // Disable,DisableHW
					case 7 : // Disable
					case 8 : // DisableHW
					case 9 : // Disable,DisableHW
						if ( !_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START( 1 , CHECKING_SIDE , n2 , Pt , 0 ) ) {
							if ( mode == 1 ) return 6;
							return ERROR_TARGET_CHAMBER_ERROR;
						}
						break;
					}
				}
				*/
			}
		}
		//-----------------------------------------------------------------------
		//
		if ( Pt0 != -1 ) {
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , cldepth );
			//
			_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , Pt , SCHEDULER_READY );
			_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , Pt , 0 );
			//-----------------------------------------------------------------------
			if ( Chamber >= 0 ) _i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , Chamber );
			if ( Chamber >= 0 ) _i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , Chamber );
			if ( Slot >= 0 ) _i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , Slot );
			if ( Slot >= 0 ) _i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , Slot );
			//
			_i_SCH_CLUSTER_Set_Buffer( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_Stock( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , Pt , 0 );
			_i_SCH_CLUSTER_Set_FailOut( CHECKING_SIDE , Pt , 0 );
			//
			_i_SCH_CLUSTER_Set_Ex_EILInfo( CHECKING_SIDE , Pt , NULL ); // 2010.10.04
			_i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( CHECKING_SIDE , Pt , 0 ); // 2012.09.13
			//
			_i_SCH_CLUSTER_Set_CPJOB_CONTROL( CHECKING_SIDE , Pt , -1 );
			_i_SCH_CLUSTER_Set_CPJOB_PROCESS( CHECKING_SIDE , Pt , -1 );
			//-----------------------------------------------------------------------
		}
		else {
			//
			_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , cldepth );
			//
			for ( i = 0 ; i < cldepth ; i++ ) {
				//------------------------------------------------------------------------
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					//
					if ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) continue;
					//
					pmdepth = -1;
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , Pt , i , k );
						if ( ( m == ( j + PM1 ) ) || ( -m == ( j + PM1 ) ) ) {
							pmdepth = k;
							break;
						}
					}
					//
					if ( pmdepth != -1 ) {
						_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( CHECKING_SIDE , Pt , i , pmdepth );
					}
				}
				//------------------------------------------------------------------------
			}
		}
	}
	else {
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmuse[i] = 0;
		//
		for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				if ( ( CLUSTER_RECIPE->MODULE[i][j] > 0 ) && ( _i_SCH_PRM_GET_MODULE_MODE( j + PM1 ) ) ) {
					pmuse[j]++;
				}
				//
			}
		}
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			if ( pmuse[i] <= 0 ) {
				//
				switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 ) ) {
				case MUF_01_USE :
				case MUF_81_USE_to_PREND_RANDONLY :
				case MUF_97_USE_to_SEQDIS_RAND :
				case MUF_98_USE_to_DISABLE_RAND :
				case MUF_99_USE_to_DISABLE :
					_i_SCH_MODULE_Dec_Mdl_Run_Flag( i + PM1 );
					break;
				}
				//
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 , MUF_00_NOTUSE );
				//
			}
			else {
				//
				switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 ) ) {
				case MUF_00_NOTUSE :
				case MUF_02_USE_to_END :
				case MUF_03_USE_to_END_DISABLE :
				case MUF_04_USE_to_PREND_EF_LP :
				case MUF_05_USE_to_PRENDNA_EF_LP :
				case MUF_07_USE_to_PREND_DECTAG :
				case MUF_08_USE_to_PRENDNA_DECTAG :
				case MUF_31_USE_to_NOTUSE_SEQMODE :
				case MUF_71_USE_to_PREND_EF_XLP :
					//
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( i + PM1 );
					//
					break;
				}
				//
//				if ( _i_SCH_MODULE_GET_USE_ENABLE( i + PM1 , FALSE , -1 ) ) { // 2016.05.09
//				if ( _i_SCH_MODULE_GET_USE_ENABLE( i + PM1 , ( _i_SCH_MODULE_Get_PM_WAFER( i + PM1 , -1 ) > 0 ) , -1 ) ) { // 2016.05.09 // 2018.12.20
				if ( _i_SCH_MODULE_GET_USE_ENABLE( i + PM1 , ( _i_SCH_MODULE_Get_PM_WAFER( i + PM1 , -1 ) > 0 ) , -2 ) ) { // 2016.05.09 // 2018.12.20
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 , MUF_01_USE );
				}
				else {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i + PM1 , MUF_99_USE_to_DISABLE );
				}
				//
			}
		}
	}
	//-----------------------------------------------------------------------
	if ( mode == 1 ) return 0;
	return ERROR_NONE;
}

int Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( int mode , int UserMode , int mdlchk , int CHECKING_SIDE , int pointer , int Chamber , int Slot , char *clusterRecipefile , char *grpname ) {
	int res;
	Scheduler_CONTROL_CLUSTERStepTemplate2_Init( &G_CLUSTER_RECIPE );
	res = Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE_Sub( mode , UserMode , mdlchk , CHECKING_SIDE , pointer , Chamber , Slot , clusterRecipefile , &G_CLUSTER_RECIPE , grpname );
	Scheduler_CONTROL_CLUSTERStepTemplate2_Free( &G_CLUSTER_RECIPE );
	return res;
}


int Scheduler_CONTROL_RECIPE_SETTING_FROM_LOT_RECIPE( int UserMode0 , int CHECKING_SIDE , char *lotRecipefile ) {
	int i , UserMode;
	int res , gm;
	LOTStepTemplate LOT_RECIPE;
	LOTStepTemplate2 LOT_RECIPE_DUMMY;
	LOTStepTemplate3 LOT_RECIPE_SPECIAL;
	//
	LOTStepTemplate_Dummy_Ex LOT_RECIPE_DUMMY_EX; // 2016.07.13
	//
	char  Buf_rec1[256];
	char  Buf_rec2[256];
	//
	BOOL hasSlotFix; // 2015.10.12
	//====================================================================================================
	if ( UserMode0 == -1 ) {
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
			UserMode = USER_RECIPE_DATA_MODE();
		}
		else {
			UserMode = USER_RECIPE_MANUAL_DATA_MODE();
		}
	}
	else {
		UserMode = UserMode0;
	}
	if ( ( UserMode < 0 ) || ( UserMode > 8 ) ) UserMode = 0;
	//====================================================================================================
	if ( ( UserMode == 0 ) || ( UserMode == 1 ) || ( UserMode == 2 ) || ( UserMode == 6 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
//		if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , NULL , &i , lotRecipefile , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13
//		if ( !RECIPE_FILE_LOT_DATA_READ( CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &i , lotRecipefile , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
		if ( !RECIPE_FILE_LOT_DATA_READ( FALSE , NULL , CHECKING_SIDE , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , NULL , &i , lotRecipefile , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 1%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , ERROR_LOT_RECIPE_DATA );
			return ERROR_LOT_RECIPE_DATA;
		}
	}
	if ( ( UserMode == 3 ) || ( UserMode == 4 ) || ( UserMode == 5 ) || ( UserMode == 6 ) || ( UserMode == 7 ) || ( UserMode == 8 ) ) {
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
			if ( !USER_RECIPE_LOT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE , lotRecipefile ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 2%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , ERROR_LOT_RECIPE_DATA );
				return ERROR_LOT_RECIPE_DATA;
			}
			if ( !USER_RECIPE_LOT_DUMMY_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &LOT_RECIPE_DUMMY , lotRecipefile ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 3%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , ERROR_LOT_RECIPE_DATA );
				return ERROR_LOT_RECIPE_DATA;
			}
		}
		else {
			if ( !USER_RECIPE_MANUAL_LOT_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE , lotRecipefile ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 4%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , ERROR_LOT_RECIPE_DATA );
				return ERROR_LOT_RECIPE_DATA;
			}
			if ( !USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &LOT_RECIPE_DUMMY , lotRecipefile ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 5%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , ERROR_LOT_RECIPE_DATA );
				return ERROR_LOT_RECIPE_DATA;
			}
		}
	}
	//------------------------------------------------------------------------------
	STR_SEPERATE_CHAR( lotRecipefile , ':' , Buf_rec1 , Buf_rec2 , 255 );
	gm = strlen( Buf_rec2 );
	if ( gm > 0 ) {
		if ( ( Buf_rec2[0] == '(' ) && ( Buf_rec2[gm-1] == ')' ) ) {
			Buf_rec2[gm-1] = 0;
			if ( STRCMP_L( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , Buf_rec2 + 1 ) ) {
				gm = 0;
			}
			else {
				gm = 1;
			}
		}
		else {
			gm = 0;
		}
	}
	//------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( LOT_RECIPE.CLUSTER_USING[i] == 0 ) {
			res = Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( 0 , UserMode , 1 , CHECKING_SIDE , -1 , -1 , i + 1 , LOT_RECIPE.CLUSTER_RECIPE[i] , ( gm == 1 ) ? Buf_rec2 + 1 : NULL );
			if ( ( res != ERROR_NONE ) && ( res != -1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "[ERROR] RECIPE_SETTING_FROM_LOT %s Read Error 6%cRECIPEFAIL %s:%d\n" , lotRecipefile , 9 , lotRecipefile , res );
				return res;
			}
		}
	}
	//------------------------------------------------------------------------------
	return ERROR_NONE;
}


int  Scheduler_CONTROL_CHECK_AFTER_MODE_SETTING( int rs , int side , int mode ) { // 2011.05.25
	int i;
	//
	if ( mode > 1 ) return ERROR_NONE;
	//
	i = SCHEDULER_CONTROL_Check_Duplicate_Use( side , 1 , FALSE , "" );
	//
	if ( i != ERROR_NONE ) return i;
	//
	SCHEDULER_CONTROL_Check_Scheduler_Data_for_Run( side , -1 , -1 , -1 , -1 , -1 , -1 ); // 2011.09.19
	//
	return ERROR_NONE;
}
