#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "sch_preprcs.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_Multi_ALIC.h"
#include "sch_sub.h"
#include "system_tag.h"

#include "INF_sch_CommonUser.h"


BOOL SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( int side , int pointer ); // 2017.09.08

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//	파일 불가 문자 : \ / : * ? " < > |
//
//						\ / |
//								* ? >
//										: " < 
//
//			내용					위치
//
//	'*'		사용함					전체 첫문자
//	'?'		SKIP					전체 첫문자
//	'>'		Multi 파일 구분자		중간
//
//
//  +:\		SHMEM 사용				파일이름중 첫3문자
//  -:\		RcpLock 사용안함		파일이름중 첫3문자
//  T*		Simulation Time			파일이름중 첫2문자
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


CRITICAL_SECTION CR_CLUSTERDB;

int _i_SCH_MODULE_GET_USE_ENABLE_Sub( int Chamber , BOOL Always , int side );

int _i_SCH_MODULE_Get_Mdl_Use_Flag( int side , int ch ); // 2017.02.12
int _i_SCH_CLUSTER_Unuse_Chamber_Restore( int side , int pointer ); // 2017.02.12

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _SCH_INF_FLOW_CONTROL[ MAX_CASSETTE_SIDE ];
//
Scheduling_Path	_SCH_INF_CLUSTER_DATA_AREA[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
Scheduling_Info	_SCH_INF_CLUSTER_DATA2_AREA[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2011.07.21

/*
//
// 2015.04.09
//
#define SCH_CLUSTER_Make_Str_API( Name )	void _i_SCH_CLUSTER_Set_##Name##( int side , int SchPointer , char *data ) { \
	int l; \
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
		free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## ); \
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## = NULL; \
	} \
	if ( data == NULL ) return; \
	l = strlen( data ); \
	if ( l > 0 ) { \
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## = calloc( l + 1 , sizeof( char ) ); \
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
			strncpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## , data , l ); \
		} \
		else { \
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_%s Memory Allocate Error[%d,%d]\n" , #Name , side , SchPointer ); \
		} \
	} \
} \
char *_i_SCH_CLUSTER_Get_##Name##( int side , int SchPointer ) { \
	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name##); \
}

#define SCH_CLUSTER_Make_Str_Ex_API( Name )	void _i_SCH_CLUSTER_Set_Ex_##Name##( int side , int SchPointer , char *data ) { \
	int l; \
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
		free( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## ); \
		_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## = NULL; \
	} \
	if ( data == NULL ) return; \
	l = strlen( data ); \
	if ( l > 0 ) { \
		_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## = calloc( l + 1 , sizeof( char ) ); \
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
			strncpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## , data , l ); \
		} \
		else { \
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_Ex_%s Memory Allocate Error[%d,%d]\n" , #Name , side , SchPointer ); \
		} \
	} \
} \
char *_i_SCH_CLUSTER_Get_Ex_##Name##( int side , int SchPointer ) { \
	return( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name##); \
}

*/


//
// 2015.04.09
//
#define SCH_CLUSTER_Make_Str_API( Name )	void _i_SCH_CLUSTER_Set_##Name##( int side , int SchPointer , char *data ) { \
	unsigned int l; \
	if ( data == NULL ) { \
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
			strcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## , "" ); \
		} \
		return; \
	} \
	l = strlen( data ); \
	if ( l <= 0 ) { \
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
			strcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## , "" ); \
		} \
		return; \
	} \
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
		if ( l <= (_msize( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## )-1) ) { \
			strcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## , data ); \
			return; \
		} \
		free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## ); \
	} \
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## = calloc( l + 1 , sizeof( char ) ); \
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## != NULL ) { \
		strncpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name## , data , l ); \
	} \
	else { \
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_%s Memory Allocate Error[%d,%d]\n" , #Name , side , SchPointer ); \
	} \
} \
char *_i_SCH_CLUSTER_Get_##Name##( int side , int SchPointer ) { \
	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].##Name##); \
}

#define SCH_CLUSTER_Make_Str_Ex_API( Name )	void _i_SCH_CLUSTER_Set_Ex_##Name##( int side , int SchPointer , char *data ) { \
	unsigned int l; \
	if ( data == NULL ) { \
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
			strcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## , "" ); \
		} \
		return; \
	} \
	l = strlen( data ); \
	if ( l <= 0 ) { \
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
			strcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## , "" ); \
		} \
		return; \
	} \
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
		if ( l <= (_msize( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## )-1) ) { \
			strcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## , data ); \
			return; \
		} \
		free( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## ); \
	} \
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## = calloc( l + 1 , sizeof( char ) ); \
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## != NULL ) { \
		strncpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name## , data , l ); \
	} \
	else { \
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_Ex_%s Memory Allocate Error[%d,%d]\n" , #Name , side , SchPointer ); \
	} \
} \
char *_i_SCH_CLUSTER_Get_Ex_##Name##( int side , int SchPointer ) { \
	return( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].##Name##); \
}


int _i_SCH_CLUSTER_FlowControl( int side ) { // 2009.02.03
	return _SCH_INF_FLOW_CONTROL[side];
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Basic Structure Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Level
//				Copy Source to Target(Default)	Copy Source to Target(Parallel,Special,Tune,User)	ClearSource
// 0 : COPY		O								X													X
// 1 : COPY		O								O													X
// 2 : MOVE		O								X													O
// 3 : MOVE		O								O													O
// 4 : REMOVE	X								X													O

int _i_SCH_CLUSTER_Copy_Cluster_Data( int tside , int tpointer , int sside , int spointer , int level ) { // 2006.03.10
	int i , j , k;

	if ( ( level < 0 ) || ( level > 4 ) ) return 2; // 2008.11.03
	if ( ( tside == sside ) && ( tpointer == spointer ) ) return 2; // 2007.12.17

	if ( level != 4 ) {
		//
		if ( tside < 0 ) return 3; // 2010.02.04
		if ( tside >= MAX_CASSETTE_SIDE ) return 3; // 2010.02.04
		if ( tpointer < 0 ) return 3; // 2010.02.04
		if ( tpointer >= MAX_CASSETTE_SLOT_SIZE ) return 3; // 2010.02.04
		//
		if ( sside < 0 ) return 4; // 2010.02.04
		if ( sside >= MAX_CASSETTE_SIDE ) return 4; // 2010.02.04
		if ( spointer < 0 ) return 4; // 2010.02.04
		if ( spointer >= MAX_CASSETTE_SLOT_SIZE ) return 4; // 2010.02.04
		//
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathRange = _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathRange;
		//
		for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
			//----------------------------------------------------------------------
			//
			_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathRun[i] = _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathRun[i];
			//
			if ( ( level == 1 ) || ( level == 3 ) ) {

/*
// 2015.04.09
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathProcessParallel[i] ) ) {
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 1 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
				}
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].PathProcessDepth[i] ) ) { // 2014.01.28
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 11 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
				}
*/
//
// 2015.04.09
//

				if ( _SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] != NULL ) {
					if ( _SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] != NULL ) {
						memcpy( _SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] , _SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] , MAX_PM_CHAMBER_DEPTH + 1 );
					}
					else {
						_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i][MAX_PM_CHAMBER_DEPTH] = 0;
					}
				}
				else {
					if ( _SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] != NULL ) {
						//
						_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
						//
						if ( _SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 1 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
						}
						else {
							memcpy( _SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] , _SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] , MAX_PM_CHAMBER_DEPTH + 1 );
						}
						//
					}
					else {
						//
					}
				}
				//
				if ( _SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] != NULL ) {
					if ( _SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] != NULL ) {
						memcpy( _SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] , _SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] , MAX_PM_CHAMBER_DEPTH + 1 );
					}
					else {
						_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i][MAX_PM_CHAMBER_DEPTH] = 0;
					}
				}
				else {
					if ( _SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] != NULL ) {
						//
						_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
						//
						if ( _SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 11 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
						}
						else {
							memcpy( _SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] , _SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] , MAX_PM_CHAMBER_DEPTH + 1 );
						}
						//
					}
					else {
						//
					}
				}
				//
			}
			else {
//				_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] = NULL; // 2010.02.02

/*
// 2015.04.09
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] ) , NULL ) ) { // 2010.02.02
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 2 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
				}
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] ) , NULL ) ) { // 2014.01.28
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 21 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
				}
*/
//
// 2015.04.09
//
				if ( _SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i] != NULL ) {
					_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessParallel[i][MAX_PM_CHAMBER_DEPTH] = 0;
				}
				if ( _SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i] != NULL ) {
					_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PathProcessDepth[i][MAX_PM_CHAMBER_DEPTH] = 0;
				}
//
			}
			//
			//----------------------------------------------------------------------
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//----------------------------------------------------------------------
				_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathProcess[i][j] = _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathProcess[i][j]; 
				//----------------------------------------------------------------------
				for ( k = 0 ; k < 3 ; k++ ) {
					if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].PathProcessRecipe[i][j][k] ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathProcessRecipe[ i ][ j ][ k ] ) ) { // 2010.03.25
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 3 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
					}
				}
				//----------------------------------------------------------------------
			}
		}
		//
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathStatus			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathStatus; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathDo				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathDo; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathIn				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathIn; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathOut				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathOut; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].PathHSide			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathHSide; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].SlotIn				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].SlotIn; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].SlotOut				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].SlotOut; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].SwitchInOut			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].SwitchInOut; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].FailOut				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].FailOut; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].CPJOB_CONTROL		= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].CPJOB_CONTROL; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].CPJOB_PROCESS		= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].CPJOB_PROCESS; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].Buffer				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].Buffer; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].Stock				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].Stock; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].Extra				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].Extra; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].LotSpecial			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].LotSpecial; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].StartTime			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].StartTime; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].EndTime				= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].EndTime; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].Optimize			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].Optimize; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].ClusterIndex		= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].ClusterIndex; 
		_SCH_INF_CLUSTER_DATA_AREA[ tside ][ tpointer ].SupplyID			= _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].SupplyID; 
		//
		_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].ClusterSpecial		= _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].ClusterSpecial;  // 2014.06.23
		//
		if ( ( level == 1 ) || ( level == 3 ) ) {
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].LotUserSpecial ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].LotUserSpecial ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 11 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterUserSpecial ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].ClusterUserSpecial ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 12 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterTuneData ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].ClusterTuneData ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 13 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].UserArea ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 14 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea2 ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].UserArea2 ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 15 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea3 ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].UserArea3 ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 16 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea4 ) , _SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].UserArea4 ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 17 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].EILInfo ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].EILInfo ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 20 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].MtlOut = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].MtlOut; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].CarrierIndex = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].CarrierIndex; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].OutCarrierIndex = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].OutCarrierIndex; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].DisableSkip = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].DisableSkip; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].UserDummy = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].UserDummy; // 2015.10.12
			//
			// 2011.07.21
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].JobName ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].JobName ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 21 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].RecipeName ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].RecipeName ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 22 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PPID ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].PPID ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 24 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].WaferID ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].WaferID ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 25 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].MaterialID ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].MaterialID ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 26 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].OutMaterialID ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].OutMaterialID ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 27 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			// 2012.09.06
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].UserControl_Mode = _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].UserControl_Mode;
			//
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].UserControl_Data ) , _SCH_INF_CLUSTER_DATA2_AREA[ sside ][ spointer ].UserControl_Data ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 28 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
		}
		else {
			//
//			_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].LotUserSpecial = NULL; // 2010.02.02
//			_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterUserSpecial = NULL; // 2010.02.02
//			_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterTuneData = NULL; // 2010.02.02
//			_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea = NULL; // 2010.02.02
			//
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].LotUserSpecial ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 111 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterUserSpecial ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 112 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].ClusterTuneData ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 113 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 114 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea2 ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 115 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea3 ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 116 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[tside][tpointer].UserArea4 ) , NULL ) ) { // 2010.02.02
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 117 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].EILInfo ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 120 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].MtlOut = 0; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].CarrierIndex = -1;
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].OutCarrierIndex = -1;
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].DisableSkip = 0; 
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].UserDummy = 0; // 2015.10.12
			//
			// 2011.07.21
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].JobName ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 121 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].RecipeName ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 122 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].PPID ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 124 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].WaferID ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 125 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].MaterialID ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 126 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].OutMaterialID ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 127 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			// 2012.09.06
			_SCH_INF_CLUSTER_DATA2_AREA[ tside ][ tpointer ].UserControl_Mode = 0;
			//
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[tside][tpointer].UserControl_Data ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 128 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			//
		}
	}
	else {
		//
		if ( sside < 0 ) return 4; // 2010.02.04
		if ( sside >= MAX_CASSETTE_SIDE ) return 4; // 2010.02.04
		if ( spointer < 0 ) return 4; // 2010.02.04
		if ( spointer >= MAX_CASSETTE_SLOT_SIZE ) return 4; // 2010.02.04
		//
	}
	//
	if ( ( level == 2 ) || ( level == 3 ) || ( level == 4 ) ) { // 2008.11.03
		//
		_i_SCH_CLUSTER_Clear_PathProcessData( sside , spointer , TRUE );
		//

/*
// 2015.04.09

		for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] ) , NULL ) ) {
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 210 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] ) , NULL ) ) { // 2014.01.28
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 210-2 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
			}
		}
*/
//
// 2015.04.09
//


		for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
			if ( _SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i] != NULL ) {
				_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].PathProcessParallel[i][MAX_PM_CHAMBER_DEPTH] = 0;
			}
			if ( _SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i] != NULL ) {
				_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PathProcessDepth[i][MAX_PM_CHAMBER_DEPTH] = 0;
			}
		}
//
		//
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].LotUserSpecial ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 211 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].ClusterUserSpecial ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 212 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].ClusterTuneData ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 213 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].UserArea ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 214 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].UserArea2 ) , NULL ) ) { // 2010.02.02
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 215 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].UserArea3 ) , NULL ) ) { // 2010.02.02
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 216 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[sside][spointer].UserArea4 ) , NULL ) ) { // 2010.02.02
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 217 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		//
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].EILInfo ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 220 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		// 2011.07.21
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].JobName ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 221 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].RecipeName ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 222 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].PPID ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 224 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].WaferID ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 225 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].MaterialID ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 226 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].OutMaterialID ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 227 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		// 2012.09.06
		if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA2_AREA[sside][spointer].UserControl_Data ) , NULL ) ) {
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Copy_Cluster_Data 228 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , tside , tpointer , sside , spointer , level );
		}
		//
	}
	//
	if ( level == 4 ) { // 2008.11.03
		_SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathRange = -7;
		_SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathStatus = 0;
		_SCH_INF_CLUSTER_DATA_AREA[ sside ][ spointer ].PathDo = 0;
	}
	return 0;
}
//----------------------------------------------------
int  _i_SCH_CLUSTER_Get_PathIn( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathIn ); }
void _i_SCH_CLUSTER_Set_PathIn( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathIn = data; }

int  _i_SCH_CLUSTER_Get_PathOut( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathOut ); }
void _i_SCH_CLUSTER_Set_PathOut( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathOut = data; }

int  _i_SCH_CLUSTER_Get_PathHSide( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathHSide ); }
void _i_SCH_CLUSTER_Set_PathHSide( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathHSide = data; }

int  _i_SCH_CLUSTER_Get_SlotIn( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SlotIn ); }
void _i_SCH_CLUSTER_Set_SlotIn( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SlotIn = data; }

int  _i_SCH_CLUSTER_Get_SlotOut( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SlotOut ); }
void _i_SCH_CLUSTER_Set_SlotOut( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SlotOut = data; }

int  _i_SCH_CLUSTER_Get_PathDo( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo ); }
void _i_SCH_CLUSTER_Set_PathDo( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo = data; }
void _i_SCH_CLUSTER_Inc_PathDo( int side , int SchPointer ) { if ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo != PATHDO_WAFER_RETURN ) _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo++; }
void _i_SCH_CLUSTER_Dec_PathDo( int side , int SchPointer ) { // 2006.01.05
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo != PATHDO_WAFER_RETURN ) _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo--;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo < 0 ) _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathDo = 0;
 }

int  _i_SCH_CLUSTER_Get_PathStatus( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathStatus ); }
void _i_SCH_CLUSTER_Set_PathStatus( int side , int SchPointer , int data ) { // 2004.12.03
	if ( ( data == SCHEDULER_READY ) || ( data == SCHEDULER_CM_END ) ) {
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathStatus = data;
	}
	else {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathStatus != SCHEDULER_RETURN_REQUEST ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathStatus = data;
		}
	}
}

int  _i_SCH_CLUSTER_Get_PathRange( int side , int SchPointer )  { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathRange ); }
void _i_SCH_CLUSTER_Set_PathRange( int side , int SchPointer , int data )  { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].PathRange = data; }


int  _i_SCH_CLUSTER_Get_PathOrder( int side , int SchPointer , int No , int ch ) { // 2006.01.12
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == ch ) return i;
		if ( -(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] ) == ch ) return i; // 2008.02.01
	}
	return -1;
}

int	 _i_SCH_CLUSTER_Get_PathRun( int side , int SchPointer , int No , int mode ) { // 2006.07.13
	// 9876543210
	//         -- 0
	//        -   1
	//      --    2
	//    --      3
	//  --        4
	// -          5
	switch( mode ) {
	case 0 :
		return( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) % 100 );
		break;
	case 1 :
		return( ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) % 1000 ) / 100 );
		break;
	case 2 :
		return( ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) % 100000 ) / 1000 );
		break;
	case 3 :
		return( ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) % 10000000 ) / 100000 );
		break;
	case 4 :
		return( ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) % 1000000000 ) / 10000000 );
		break;
	case 5 :
		return( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] ) / 1000000000 );
		break;
	default :
		return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] );
		break;
	}
}

void _i_SCH_CLUSTER_Set_PathRun( int side , int SchPointer , int No , int mode , int data ) { // 2006.07.13
	int dl;
	int dh;
	// 9876543210
	//         -- 0 : Run Process Chamber
	//        -   1 : Update Count in DLL for Cluster Recipe
	//      --    2 : No
	//					0 : Supply Count
	//					1 : Reserved
	//					2 : Reserved
	//					3 : Reserved
	//					4 : Reserved
	//					5 : Reserved
	//					6 : Reserved
	//					7 : Reserved
	//					8 : Reserved
	//					9 : Reserved
	//					10 : Reserved
	//					11 : Return Slot
	//					12 : Wait Style
	//					13 : Next Style
	//					14 : Make Mode
	//					15 : Make Dummy Style for Tag12 in Lot PrePost
	//					16 : Lot PrePost Count
	//					17 : End PM
	//					18 : End PM Order
	//					19 : End PM XIndex
	//					20 : End PM Reuse Recipe
	//					21 : Param
	//					22 : Recipe Read Style
	//					23 : InsideTag for First Wafer Check
	//    --      3
	//  --        4
	// -          5
	switch( mode ) {
	case 0 :
		if ( data <  0 ) data = 0;
		if ( data > 99 ) data = 99;
		//
		dh = ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] / 100 ) * 100;
		dl = 0;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + data;
		break;
	case 1 :
		if ( data <  0 ) data = 0;
		if ( data >  9 ) data = 9;
		//
		dh = ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] / 1000 ) * 1000;
		dl =   _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] % 100;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + ( data * 100 );
		break;
	case 2 :
		if ( data <  0 ) data = 0;
		if ( data > 99 ) data = 99;
		//
		if ( ( No == 10 ) && ( data > 0 ) ) _SCH_INF_FLOW_CONTROL[side] = TRUE;
		//
		dh = ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] / 100000 ) * 100000;
		dl =   _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] % 1000;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + ( data * 1000 );
		break;
	case 3 :
		if ( data <  0 ) data = 0;
		if ( data > 99 ) data = 99;
		//
		if ( ( No == 10 ) && ( data > 0 ) ) _SCH_INF_FLOW_CONTROL[side] = TRUE;
		//
		dh = ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] / 10000000 ) * 10000000;
		dl =   _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] % 100000;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + ( data * 100000 );
		break;
	case 4 :
		if ( data <  0 ) data = 0;
		if ( data > 99 ) data = 99;
		//
		dh = ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] / 1000000000 ) * 1000000000;
		dl =   _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] % 10000000;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + ( data * 10000000 );
		break;
	case 5 :
		if ( data <  0 ) data = 0;
		if ( data >  1 ) data = 1;
		//
		dh = 0;
		dl = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] % 1000000000;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = dh + dl + ( data * 1000000000 );
		break;
	default :
		if ( data <= 0 ) _SCH_INF_FLOW_CONTROL[side] = FALSE;
		//
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathRun[No] = data;
		break;
	}
}

int	 _i_SCH_CLUSTER_Get_PathProcessChamber( int side , int SchPointer , int No , int No2 ) {
	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] );
}

void _i_SCH_CLUSTER_Set_PathProcessChamber( int side , int SchPointer , int No , int No2 , int data ) {
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = data;
}

char *_i_SCH_CLUSTER_Get_PathProcessRecipe( int side , int SchPointer , int No , int No2 , int PSide ) {
//	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][PSide] ); // 2010.03.25
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][PSide] == NULL ) return COMMON_BLANK_CHAR; // 2010.03.25
	return _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][PSide]; // 2010.03.25
}

char *_i_SCH_CLUSTER_Get_PathProcessRecipe2( int side , int SchPointer , int No , int ch , int PSide ) {
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == ch ) {
//			return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][PSide] ); // 2010.03.25
			if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][PSide] == NULL ) return COMMON_BLANK_CHAR; // 2010.03.25
			return _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][PSide]; // 2010.03.25
		}
	}
//	return NULL; // 2010.12.17
	return COMMON_BLANK_CHAR; // 2010.12.17
}

void _i_SCH_CLUSTER_Disable_PathProcessRecipe_MultiData( int side , int SchPointer , int NoC , int ch , int PSide , int cutindex ) {
	char Retdata[256];
	char TotStr[4096];
	int x , sx , i , l , z1 , z2;
	//
//	if ( cutindex < 0 ) return; // 2009.02.01
	//
	if ( ( cutindex + 1 ) == 99 ) sx = 0;
	else                          sx = NoC;
	//
	for ( x = sx ; x <= NoC ; x++ ) { 
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
	//		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == ch ) { // 2009.02.01
			if ( ( cutindex < 0 ) || ( ( cutindex + 1 ) == 99 ) || ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[x][i] == ch ) ) { // 2009.02.01
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[x][i][PSide] != NULL ) {
					//
					z1 = 0;
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[x][i][PSide] , '|' , Retdata , z1 , 255 );
//					if ( z1 == z2 ) return; // 2009.03.02
					if ( z1 == z2 ) break; // 2009.03.02
					z1 = z2;
					//
					if ( Retdata[0] == '*' ) {
						strcpy( TotStr , Retdata );
					}
					else {
						strcpy( TotStr , "*" );
						strcat( TotStr , Retdata );
					}
					//
					l = 0;
					//
					while( TRUE ) {
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[x][i][PSide] , '|' , Retdata , z1 , 255 );
						if ( z1 == z2 ) break;
						z1 = z2;
						l++;
						strcat( TotStr , "|" );
	//					if ( cutindex >= l ) { // 2009.02.01
						if ( ( cutindex < 0 ) || ( ( cutindex + 1 ) == 99 ) || ( cutindex >= l ) ) { // 2009.02.01
							if ( Retdata[0] != '*' ) strcat( TotStr , "*" );
						}
						strcat( TotStr , Retdata );
					}
					//
					if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[x][i][PSide] ) , TotStr ) ) { // 2010.03.25
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Disable_PathProcessRecipe_MultiData Memory Allocate Error[%d,%d,%d,%d,%d,%d]\n" , side , SchPointer , NoC , ch , PSide , cutindex );
					}
				}
				if ( ( cutindex >= 0 ) && ( ( cutindex + 1 ) != 99 ) ) // 2009.02.01
					return;
			}
		}
	}
}

BOOL _i_SCH_CLUSTER_PathProcessRecipe_MultiData_Unused( int side , int SchPointer , int No ) {
	char Retdata[256];
	int i , z1 , z2;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == 0 ) continue;
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][0] == NULL ) continue;
		//
		z1 = 0;
		while( TRUE ) {
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][0] , '|' , Retdata , z1 , 255 );
			if ( z1 == z2 ) break;
			z1 = z2;
			if ( Retdata[0] != '*' ) return FALSE;
		}
	}
	//
	return TRUE;
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( int side , int SchPointer , int No , int ch ) { // 2004.03.18
	int i , j;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		j = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
		if ( j > 0 ) { // 2006.02.02
			if ( j != ch ) {
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = -j;
			}
		}
	}
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( int side , int SchPointer , int No , int ch ) { // 2006.02.28
	int i , j;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		j = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
		if ( j > 0 ) {
			if ( j == ch ) {
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = -j;
			}
		}
	}
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Enable( int side , int SchPointer , int No , int ch ) { // 2013.03.28
	int i , j;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		j = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
		if ( j < 0 ) {
			if ( -j == ch ) {
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = -j;
			}
		}
	}
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( int side , int SchPointer , int No , int No2 ) { // 2003.05.26
	int i;
	i = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2];
	if ( i > 0 ) {
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = -i;
	}
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( int side , int SchPointer , int No , int No2 ) { // 2003.05.26
	int i;
	i = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2];
	if ( i < 0 ) {
		_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = -i;
	}
}

void	_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( int side , int SchPointer , int No , int No2 ) { // 2006.02.10
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = 0;
}


/*
// 2015.04.09
void _i_SCH_CLUSTER_Clear_PathProcessData( int side , int SchPointer , BOOL frm ) { // 2006.03.09
	int i , j , k;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[i][j] = 0; // 2009.02.01
			for ( k = 0 ; k < 3 ; k++ ) {
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[i][j][k] != NULL ) {
					if ( frm ) free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[i][j][k] );
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[i][j][k] = NULL;
				}
			}
		}
	}
}
*/

// 2015.04.09
void _i_SCH_CLUSTER_Clear_PathProcessData( int side , int SchPointer , BOOL prcreset ) { // 2006.03.09
	int i , j , k;
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[i][j] = 0; // 2009.02.01
			//
			if ( prcreset ) {
				for ( k = 0 ; k < 3 ; k++ ) {
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[i][j][k] != NULL ) {
						strcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[i][j][k] , "" );
					}
				}
			}
			//
		}
	}
}

int _i_SCH_CLUSTER_Check_PathProcessData_OnlyOnePM( int side , int SchPointer , int cldepth , int pmc ) { // 2009.02.02
	int i , j , ch , f = 0 , o = 0;
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		//
		if ( ( cldepth != -1 ) && ( cldepth != i ) ) continue;
		//
		f = 0;
		o = 0;
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			ch = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[i][j];
			if ( ch > 0 ) {
				if ( ch == pmc ) {
					f = 1;
				}
				else {
					o++;
				}
			}
		}
		//
		if ( f == 1 ) {
			if ( o == 0 ) return 1;
			else          return 2;
		}
	}
	//
	return 0;
}

void _i_SCH_CLUSTER_Set_PathProcessData( int side , int SchPointer , int No , int No2 , int chamber , char *r1 , char *r2 , char *r3 ) {
//	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = chamber; // 2009.04.23
	if ( chamber <= 0 ) _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = chamber; // 2009.04.23
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_IN] ) , r1 ) ) { // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData 1 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , SchPointer , No , No2 , chamber );
	}
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) , r2 ) ) { // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData 2 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , SchPointer , No , No2 , chamber );
	}
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] ) , r3 ) ) { // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData 3 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , SchPointer , No , No2 , chamber );
	}
	if ( chamber >  0 ) _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][No2] = chamber; // 2009.04.23
}
//


BOOL _i_SCH_CLUSTER_Set_PathProcessData2( int side , int SchPointer , int No , int chamber , char *r1 , char *r2 , char *r3 ) {
	int j , k;
	if ( chamber == 0 ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][j] = 0;
			for ( k = 0 ; k < 3 ; k++ ) {
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][j][k] != NULL ) {
					free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][j][k] );
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][j][k] = NULL;
				}
			}
		}
		return TRUE;
	}
	else {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][j] == chamber ) break;
		}
		if ( j != MAX_PM_CHAMBER_DEPTH ) {
			_i_SCH_CLUSTER_Set_PathProcessData( side , SchPointer , No , j , chamber , r1 , r2 , r3 );
			return TRUE;
		}
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][j] == 0 ) break;
		}
		if ( j != MAX_PM_CHAMBER_DEPTH ) {
			_i_SCH_CLUSTER_Set_PathProcessData( side , SchPointer , No , j , chamber , r1 , r2 , r3 );
			return TRUE;
		}
	}
	return FALSE;
}



//
void _i_SCH_CLUSTER_Set_PathProcessData_JustIn( int side , int SchPointer , int No , int No2 , char *r ) { // 2011.09.22
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_IN] ) , r ) ) {
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_JustIn Memory Allocate Error[%d,%d,%d,%d]\n" , side , SchPointer , No , No2 );
	}
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_JustPost( int side , int SchPointer , int No , int No2 , char *r ) {
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) , r ) ) { // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_JustPost Memory Allocate Error[%d,%d,%d,%d]\n" , side , SchPointer , No , No2 );
	}
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_JustPre( int side , int SchPointer , int No , int No2 , char *r ) {
	if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] ) , r ) ) { // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_JustPre Memory Allocate Error[%d,%d,%d,%d]\n" , side , SchPointer , No , No2 );
	}
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPre( int side , int SchPointer , int No , int No2 , BOOL OnlyuseSelectPM ) {
	int i;
	char Buffer[4096+1];

	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] != NULL ) {
		if ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR][0] != '*' ) && ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR][0] != '?' ) ) { // 2005.10.19
			strcpy(  Buffer , "*" );
			strncat( Buffer , _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] , 4096 );
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] ) , Buffer ) ) { // 2010.03.25
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_UsedPre Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , SchPointer , No , No2 , OnlyuseSelectPM );
			}
		}
	}
	if ( OnlyuseSelectPM ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
			if ( i != No2 ) {
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] > 0 ) {
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = - _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
				}
			}
		}
	}
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPre( int side , int ch ) { // 2002.03.25
	SCHEDULER_GLOBAL_Set_Chamber_Pre_NotUse_Flag( ch , TRUE );
}
//
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPre( int side , int SchPointer , int No , int No2 , int ch , BOOL Action ) {
	if ( ch >= 0 ) { // 2008.11.22
		if ( SCHEDULER_GLOBAL_Get_Chamber_Pre_NotUse_Flag( ch ) ) { // 2002.03.25
			if ( Action ) SCHEDULER_GLOBAL_Set_Chamber_Pre_NotUse_Flag( ch , FALSE );
			return FALSE;
		}
	}
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] == NULL ) return FALSE;
	if ( strlen( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] ) <= 0 ) return FALSE;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR][0] == '*' ) return FALSE;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR][0] == '?' ) return FALSE; // 2005.10.19
	return TRUE;
}
//
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPre_GlobalOnly( int ch ) {
	return SCHEDULER_GLOBAL_Get_Chamber_Pre_NotUse_Flag( ch );
}
//
BOOL _i_SCH_CLUSTER_Check_Already_Run_UsedPre( int side , int SchPointer , int No , int No2 ) { // 2005.05.11
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] == NULL ) return FALSE;
	if ( strlen( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR] ) <= 0 ) return FALSE;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_PR][0] == '*' ) return TRUE;
	return FALSE;
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( int side , int SchPointer , int No ) {
	int i , j;
	char Buffer[4096+1];

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_PR] != NULL ) {
			if ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_PR][0] == '*' ) || ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_PR][0] == '?' ) ) { // 2005.10.19
				strncpy( Buffer , _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_PR] , 4096 );
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_PR] ) , Buffer + 1 ) ) { // 2010.03.25
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_UsedPre Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , No );
				}
			}
		}
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] < 0 ) {
			j = - _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
			if ( _i_SCH_MODULE_GET_USE_ENABLE( j , FALSE , side ) ) { // 2003.12.04
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = j;
			}
		}
	}
}
//
//====================================================================================================
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPost( int side , int SchPointer , int No , int No2 ) {
	char Buffer[4096+1];

	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] != NULL ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] == '*' ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] = '?';
		}
		else if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] != '?' ) {
			strcpy(  Buffer , "?" );
			strncat( Buffer , _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] , 4096 );
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) , Buffer ) ) { // 2010.03.25
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_SkipPost Memory Allocate Error[%d,%d,%d,%d]\n" , side , SchPointer , No , No2 );
			}
		}
	}
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPost2( int side , int SchPointer , int No , int ch ) { // 2006.01.12
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == ch ) {
			_i_SCH_CLUSTER_Set_PathProcessData_SkipPost( side , SchPointer , No , i );
		}
	}
}

BOOL _i_SCH_CLUSTER_Check_Possible_UsedPost( int side , int SchPointer , int No , int No2 ) { // 2002.03.25 // 2002.06.03
//	BOOL FileUse; // 2006.01.09
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] == NULL ) {
		return FALSE; // 2006.01.09
//		FileUse = FALSE; // 2006.01.09
	}
	else {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] == '*' ) return FALSE;
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] == '?' ) return FALSE; // 2005.10.19
		//
		if ( strlen( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) <= 0 ) return FALSE; // 2006.01.09
//		if ( strlen( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) <= 0 ) { // 2006.01.09
//			FileUse = FALSE; // 2006.01.09
//		} // 2006.01.09
//		else { // 2006.01.09
//			FileUse = TRUE; // 2006.01.09
//		} // 2006.01.09
	}

//	switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( ch ) ) { // 2006.01.09
//	case 0 : // 2006.01.09
//	case 2 : // 2006.01.09
//		if ( FileUse ) // 2006.01.09
//			return TRUE; // 2006.01.09
//		else // 2006.01.09
//			return FALSE; // 2006.01.09
//		break; // 2006.01.09
//	default : // 2006.01.09
//		return TRUE; // 2006.01.09
//		break; // 2006.01.09
//	} // 2006.01.09
	return TRUE;
}
//
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPost2( int side , int SchPointer , int ch , int No , int *No2 ) { // 2006.01.11
	int i;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == ch ) || ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] == -ch ) ) {
			if ( _i_SCH_CLUSTER_Check_Possible_UsedPost( side , SchPointer , No , i ) ) {
				*No2 = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPost_Restore( int side , int SchPointer , int No ) { // 2002.03.25
	int i , j;
	char Buffer[4096+1];
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i ++ ) {
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_OUT] != NULL ) {
			if ( ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_OUT][0] == '*' ) || ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_OUT][0] == '?' ) ) { // 2005.10.19
				strncpy( Buffer , _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_OUT] , 4096 );
				if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i][SCHEDULER_PROCESS_RECIPE_OUT] ) , Buffer + 1 ) ) { // 2010.03.25
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessData_UsedPost_Restore Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , No );
				}
			}
		}
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] < 0 ) {
			j = - _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i];
			if ( _i_SCH_MODULE_GET_USE_ENABLE( j , FALSE , side ) ) { // 2003.12.04
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i] = j;
			}
		}
	}
}
//
BOOL _i_SCH_CLUSTER_Check_HasProcessData_Post( int side , int SchPointer , int No , int No2 ) { // 2002.05.20
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] == NULL ) return FALSE;
	if ( strlen( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT] ) <= 0 ) return FALSE;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] == '*' ) return FALSE;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][No2][SCHEDULER_PROCESS_RECIPE_OUT][0] == '?' ) return FALSE; // 2005.10.19
	return TRUE;
}
//
void _i_SCH_CLUSTER_Swap_PathProcessChamber( int side , int SchPointer , int No , int i1 , int i2 ) {
	int i;
	char *buffer;

	i = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i1];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i1] = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i2];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcess[No][i2] = i;

	buffer = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_IN];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_IN] = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_IN];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_IN] = buffer;

	buffer = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_OUT];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_OUT] = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_OUT];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_OUT] = buffer;

	buffer = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_PR];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i1][SCHEDULER_PROCESS_RECIPE_PR] = _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_PR];
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][i2][SCHEDULER_PROCESS_RECIPE_PR] = buffer;
}
//
void _i_SCH_CLUSTER_Move_PathProcessChamber( int side , int SchPointer , int No , int No2 ) {
	int j , k;
	//----------------------------------------------------------------------
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		//----------------------------------------------------------------------
		_SCH_INF_CLUSTER_DATA_AREA[ side ][ SchPointer ].PathProcess[No][j] = _SCH_INF_CLUSTER_DATA_AREA[ side ][ SchPointer ].PathProcess[No2][j]; 
		//----------------------------------------------------------------------
		for ( k = 0 ; k < 3 ; k++ ) {
			if ( !STR_MEM_MAKE_COPY2( &(_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessRecipe[No][j][k] ) , _SCH_INF_CLUSTER_DATA_AREA[ side ][ SchPointer ].PathProcessRecipe[ No2 ][ j ][ k ] ) ) { // 2010.03.25
				_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Move_PathProcessChamber Memory Allocate Error[%d,%d,%d,%d]\n" , side , SchPointer , No , No2 );
			}
		}
		//----------------------------------------------------------------------
		_SCH_INF_CLUSTER_DATA_AREA[ side ][ SchPointer ].PathProcess[No2][j] = 0;
	}
	//----------------------------------------------------------------------
}

/*
// 2013.01.05
int _i_SCH_CLUSTER_Get_Next_PM_String( int side , int SchPointer , int nextdo , char *NextPM , int maxsize ) { // 2002.07.16
	int k , ct = 0 , ChkTrg , sz , l;
	char Buffer[8];
	//
	strcpy( NextPM , "" );
	sz = 0;
	if ( nextdo < _i_SCH_CLUSTER_Get_PathRange( side , SchPointer ) ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			ChkTrg = _i_SCH_CLUSTER_Get_PathProcessChamber( side , SchPointer , nextdo , k );
			if ( ChkTrg > 0 ) {
				//
				if      ( ( ChkTrg - PM1 + 1 ) > 30 ) { // 2012.02.07
					if ( ct == 0 ) {
						sprintf( Buffer , "%d" , ChkTrg - PM1 + 1 );
					}
					else {
						sprintf( Buffer , "|%d" , ChkTrg - PM1 + 1 );
					}
				}
				else if ( ( ChkTrg - PM1 + 1 ) >= 10 ) { // 2007.04.10
					if ( ct == 0 ) {
						sprintf( Buffer , "%c" , ( ChkTrg - PM1 + 1 ) - 10 + 'A' );
					}
					else {
						sprintf( Buffer , "|%c" , ( ChkTrg - PM1 + 1 ) - 10 + 'A'  );
					}
				}
				else {
					if ( ct == 0 ) {
						sprintf( Buffer , "%d" , ChkTrg - PM1 + 1 );
					}
					else {
						sprintf( Buffer , "|%d" , ChkTrg - PM1 + 1 );
					}
				}
				//=======================================================
				l = strlen( Buffer ); // 2007.01.04
				if ( l + sz > maxsize ) return ct; // 2007.01.04
				sz = sz + l; // 2007.01.04
				//=======================================================
				strcat( NextPM , Buffer );
				ct++;
			}
		}
	}
	else {
		strcpy( NextPM , "0" );
	}
	return ct;
}
*/


// 2013.01.05
int _i_SCH_CLUSTER_Get_Next_PM_String( int side , int SchPointer , int nextdo , char *NextPM , int maxsize ) {
	int i , k , ct , ChkTrg , sz , l , f;
	char Buffer[8];
	//
	f = 0;
	//
	for ( i = nextdo ; i < _i_SCH_CLUSTER_Get_PathRange( side , SchPointer ) ; i++ ) {
		//
		strcpy( NextPM , "" );
		sz = 0;
		ct = 0;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			ChkTrg = _i_SCH_CLUSTER_Get_PathProcessChamber( side , SchPointer , i , k );
			//
			if ( ChkTrg > 0 ) {
				//
				if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) continue; // 2013.01.05
				if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ChkTrg ) >= 2 ) continue; // 2013.01.05
				//
				f = 1;
				//
				if      ( ( ChkTrg - PM1 + 1 ) > 30 ) { // 2012.02.07
					if ( ct == 0 ) {
						sprintf( Buffer , "%d" , ChkTrg - PM1 + 1 );
					}
					else {
						sprintf( Buffer , "|%d" , ChkTrg - PM1 + 1 );
					}
				}
				else if ( ( ChkTrg - PM1 + 1 ) >= 10 ) { // 2007.04.10
					if ( ct == 0 ) {
						sprintf( Buffer , "%c" , ( ChkTrg - PM1 + 1 ) - 10 + 'A' );
					}
					else {
						sprintf( Buffer , "|%c" , ( ChkTrg - PM1 + 1 ) - 10 + 'A'  );
					}
				}
				else {
					if ( ct == 0 ) {
						sprintf( Buffer , "%d" , ChkTrg - PM1 + 1 );
					}
					else {
						sprintf( Buffer , "|%d" , ChkTrg - PM1 + 1 );
					}
				}
				//=======================================================
				l = strlen( Buffer ); // 2007.01.04
				if ( l + sz > maxsize ) return ct; // 2007.01.04
				sz = sz + l; // 2007.01.04
				//=======================================================
				strcat( NextPM , Buffer );
				ct++;
			}
		}
		//
		if ( f == 1 ) break; // 2013.01.05
		//
	}
	//
	if ( f == 0 ) { // 2013.01.05
		strcpy( NextPM , "0" );
		ct = 0;
	}
	//
	return ct;
}

int  _i_SCH_CLUSTER_Get_SwitchInOut( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SwitchInOut ); }
void _i_SCH_CLUSTER_Set_SwitchInOut( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SwitchInOut = data; }

int  _i_SCH_CLUSTER_Get_FailOut( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].FailOut ); }
void _i_SCH_CLUSTER_Set_FailOut( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].FailOut = data; }

int  _i_SCH_CLUSTER_Get_CPJOB_CONTROL( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].CPJOB_CONTROL ); }
void _i_SCH_CLUSTER_Set_CPJOB_CONTROL( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].CPJOB_CONTROL = data; }

int  _i_SCH_CLUSTER_Get_CPJOB_PROCESS( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].CPJOB_PROCESS ); }
void _i_SCH_CLUSTER_Set_CPJOB_PROCESS( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].CPJOB_PROCESS = data; }

int  _i_SCH_CLUSTER_Get_Buffer( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Buffer ); }
void _i_SCH_CLUSTER_Set_Buffer( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Buffer = data; }

int  _i_SCH_CLUSTER_Get_Stock( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Stock ); } // 2006.04.13
void _i_SCH_CLUSTER_Set_Stock( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Stock = data; } // 2006.04.13
void _i_SCH_CLUSTER_Inc_Stock( int side , int SchPointer ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Stock++; } // 2006.04.13
void _i_SCH_CLUSTER_Dec_Stock( int side , int SchPointer ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Stock--; } // 2006.04.13

int  _i_SCH_CLUSTER_Get_Optimize( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Optimize ); } // 2006.04.04
void _i_SCH_CLUSTER_Set_Optimize( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Optimize = data; } // 2006.04.04

int  _i_SCH_CLUSTER_Get_SupplyID( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SupplyID ); } // 2007.09.05
void _i_SCH_CLUSTER_Set_SupplyID( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].SupplyID = data; } // 2007.09.05

int  _i_SCH_CLUSTER_Get_ClusterIndex( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].ClusterIndex ); } // 2006.07.04
void _i_SCH_CLUSTER_Set_ClusterIndex( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].ClusterIndex = data; } // 2006.07.04
void _i_SCH_CLUSTER_Dec_ClusterIndex( int side , int SchPointer ) { (_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].ClusterIndex)--; } // 2007.09.07

int  _i_SCH_CLUSTER_Get_Extra( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra ); } // 2006.02.06
void _i_SCH_CLUSTER_Set_Extra( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = data; } // 2006.02.06
//
int  _i_SCH_CLUSTER_Set_Extra_Flag( int side , int SchPointer , int flag , int data ) { // 2008.10.31
	int bdh , bdl;
	//=======================================================================================
	// xxxxxxxxx
	// 876543210
	// -----3210 // 2007.04.13
	// 87654---- // 2008.10.31 Extra_Time Value
	//---------------------------------------------------------------------------------------
	//         0 - xxxx.xx.xx Not Use
	//        1  - 2006.02.06 Cooling Use [0:N/A , 1:Cooling , 2:Cooling Skip] 2011.11.30 [3:Cooling Left] [4:Cooling Right] 2014.08.26 [5:ALSkip+Cooling , 6:ALSkip+Cooling Skip , 7:ALSkip+Cooling Left , 8:ALSkip+Cooling Right]
	//       2   - User
	//      3    - 2011.04.15 No Process for Manual(Stop) [0:N/A , 1:stop]
	//     4     -
	//   else    -
	//=======================================================================================
	switch( flag ) {
	case 0 :
		if ( ( data < 0 ) || ( data > 9 ) ) return 2;
		//
		bdl = 0;
		bdh = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra / 10 ) * 10;
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = bdh + bdl + ( data * 1 );
		//
		return 0;
		break;
	case 1 :
		if ( ( data < 0 ) || ( data > 9 ) ) return 2;
		//
		bdl = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 10 );
		bdh = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra / 100 ) * 100;
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = bdh + bdl + ( data * 10 );
		//
		return 0;
		break;
	case 2 :
		if ( ( data < 0 ) || ( data > 9 ) ) return 2;
		//
		bdl = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 100 );
		bdh = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra / 1000 ) * 1000;
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = bdh + bdl + ( data * 100 );
		//
		return 0;
		break;
	case 3 :
		if ( ( data < 0 ) || ( data > 9 ) ) return 2;
		//
		bdl = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 1000 );
		bdh = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra / 10000 ) * 10000;
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = bdh + bdl + ( data * 1000 );
		//
		return 0;
		break;
		//
	case 4 :
		if ( ( data < 0 ) || ( data > 99999 ) ) return 2;
		//
		bdl = ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 10000 );
		bdh = 0;
		_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra = bdh + bdl + ( data * 10000 );
		//
		return 0;
		break;
		//
	}
	return 1;
}
//==========================================================================================================
int  _i_SCH_CLUSTER_Get_Extra_Flag( int side , int SchPointer , int flag ) { // 2006.02.06
	//=======================================================================================
	// xxxxxxxxx
	// 876543210
	// -----3210 // 2007.04.13
	// 87654---- // 2008.10.31 Extra_Time Value
	//---------------------------------------------------------------------------------------
	//         0 - xxxx.xx.xx Not Use
	//        1  - 2006.02.06 Cooling Use [0:N/A , 1:Cooling , 2:Cooling Skip] 2011.11.30 [3:Cooling Left] [4:Cooling Right] 2014.08.26 [5:ALSkip+Cooling , 6:ALSkip+Cooling Skip , 7:ALSkip+Cooling Left , 8:ALSkip+Cooling Right]
	//       2   - User
	//      3    - 2011.04.15 No Process for Manual(Stop) [0:N/A , 1:stop]
	//     4     -
	//   else    -
	//=======================================================================================
	switch( flag ) {
	case 0 :	return ( ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 10          ) / 1 ); // 2008.10.31
	case 1 :	return ( ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 100         ) / 10 );
	case 2 :	return ( ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 1000        ) / 100 );
	case 3 :	return ( ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra % 10000       ) / 1000 );
		//
	case 4 :	return ( ( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra               ) / 10000 ); // 2008.10.31
		//
	}
	return _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].Extra;
}
//==========================================================================================================
void _i_SCH_CLUSTER_Dec_Extra_Flag( int side , int SchPointer , int flag ) {
	int i;
	i = _i_SCH_CLUSTER_Get_Extra_Flag( side , SchPointer , flag ) - 1;
	if ( i >= 0 ) _i_SCH_CLUSTER_Set_Extra_Flag( side , SchPointer , flag , i );
}
//==========================================================================================================
long _i_SCH_CLUSTER_Get_StartTime( int side , int SchPointer ) {
	return _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].StartTime;
}
long _i_SCH_CLUSTER_Get_EndTime( int side , int SchPointer ) {
	return _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].EndTime;
}
void _i_SCH_CLUSTER_Do_StartTime( int side , int SchPointer ) { // 2006.03.29
	_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].StartTime = GetTickCount();
}
void _i_SCH_CLUSTER_Do_EndTime( int side , int SchPointer ) { // 2006.03.29
	_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].EndTime = GetTickCount();
}
void _i_SCH_CLUSTER_Init_AllTime( int side , int SchPointer ) { // 2006.03.29
	_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].StartTime = 0;
	_SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].EndTime = 0;
}
//==========================================================================================================
int  _i_SCH_CLUSTER_Get_LotSpecial( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].LotSpecial ); } // 2004.11.16
void _i_SCH_CLUSTER_Set_LotSpecial( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA_AREA[side][ SchPointer ].LotSpecial = data; } // 2004.11.16
//==========================================================================================================
int  _i_SCH_CLUSTER_Get_ClusterSpecial( int side , int SchPointer ) { return( _SCH_INF_CLUSTER_DATA2_AREA[side][ SchPointer ].ClusterSpecial ); } // 2014.06.23
void _i_SCH_CLUSTER_Set_ClusterSpecial( int side , int SchPointer , int data ) { _SCH_INF_CLUSTER_DATA2_AREA[side][ SchPointer ].ClusterSpecial = data; } // 2014.06.23
//==========================================================================================================

SCH_CLUSTER_Make_Str_API( LotUserSpecial )
SCH_CLUSTER_Make_Str_API( ClusterUserSpecial )

SCH_CLUSTER_Make_Str_API( ClusterTuneData )

SCH_CLUSTER_Make_Str_API( UserArea )
SCH_CLUSTER_Make_Str_API( UserArea2 )
SCH_CLUSTER_Make_Str_API( UserArea3 )
SCH_CLUSTER_Make_Str_API( UserArea4 )

//==========================================================================================================
char CL_Get_ConvPathChar( char c ) {
	switch( c ) {
	case '1' :	return 1;
	case '2' :	return 2;
	case '3' :	return 3;
	case '4' :	return 4;
	case '5' :	return 5;
	case '6' :	return 6;
	case '7' :	return 7;
	case '8' :	return 8;
	case '9' :	return 9;
	case 'A' :	return 10;
	case 'B' :	return 11;
	case 'C' :	return 12;
	case 'D' :	return 13;
	case 'E' :	return 14;
	case 'F' :	return 15;
	case 'G' :	return 16;
	case 'H' :	return 17;
	case 'I' :	return 18;
	case 'J' :	return 19;
	case 'K' :	return 20;
	case 'L' :	return 21;
	case 'M' :	return 22;
	case 'N' :	return 23;
	case 'O' :	return 24;
	case 'P' :	return 25;
	case 'Q' :	return 26;
	case 'R' :	return 27;
	case 'S' :	return 28;
	case 'T' :	return 29;
	case 'U' :	return 30;
	case 'V' :	return 31;
	case 'W' :	return 32;
	case 'X' :	return 33;
	case 'Y' :	return 34;
	case 'Z' :	return 35;
	case 'a' :	return 36;
	case 'b' :	return 37;
	case 'c' :	return 38;
	case 'd' :	return 39;
	case 'e' :	return 40;
	case 'f' :	return 41;
	case 'g' :	return 42;
	case 'h' :	return 43;
	case 'i' :	return 44;
	case 'j' :	return 45;
	case 'k' :	return 46;
	case 'l' :	return 47;
	case 'm' :	return 48;
	case 'n' :	return 49;
	case 'o' :	return 50;
	case 'p' :	return 51;
	case 'q' :	return 52;
	case 'r' :	return 53;
	case 's' :	return 54;
	case 't' :	return 55;
	case 'u' :	return 56;
	case 'v' :	return 57;
	case 'w' :	return 58;
	case 'x' :	return 59;
	case 'y' :	return 60;
	case 'z' :	return 61;
	}
	return 0;
}

/*
//
// 2015.04.09
//
void _i_SCH_CLUSTER_Set_PathProcessParallel_Sub( BOOL fixed , int side , int SchPointer , int cldepth , char *data ) { // 2007.02.21
	int m , l , ix , s;
	//
	if ( cldepth == -1 ) {
		//
		for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
			if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] != NULL ) {
				free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] );
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = NULL;
			}
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
					//
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] != NULL ) {
						memcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] , data , MAX_PM_CHAMBER_DEPTH );
					}
					else {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					//
				}
				//
			}
			else {
				//
				m = 0;
				l = strlen( data );
				//
				while ( TRUE ) {
					//
					if ( m >= MAX_CLUSTER_DEPTH ) break;
					//
					ix = MAX_PM_CHAMBER_DEPTH * m;
					//
					if      ( l < (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( l > ix ) {
							//
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
							//
							if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
								_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error1[%d,%d,%d]\n" , side , SchPointer , cldepth );
								break;
							}
							//
							for ( s = 0 ; s < ( l - ix ) ; s++ ) {
								_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
							}
							for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
								_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = 0;
							}
							//
						}
						//
						break;
					}
					else if ( l >= (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
						//
						if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error2[%d,%d,%d]\n" , side , SchPointer , cldepth );
							break;
						}
						//
						for ( s = 0 ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
						}
						//
					}
					//
					m++;
					//
				}
			}
		}
	}
	else {
		//
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] != NULL ) {
			free( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] );
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] = NULL;
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] != NULL ) {
					memcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] , data , MAX_PM_CHAMBER_DEPTH );
				}
				else {
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
				}
				//
			}
			else {
				//
				l = strlen( data );
				//
				if ( l > 0 ) {
					//
					if ( l > MAX_PM_CHAMBER_DEPTH ) l = MAX_PM_CHAMBER_DEPTH;
					//
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
					//
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error3[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						//
						for ( s = 0 ; s < l ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][s] = CL_Get_ConvPathChar( data[ s ] );
						}
						for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][s] = 0;
						}
						//
					}
				}
			}
		}
	}
}
*/
//
// 2015.04.09
//
void _i_SCH_CLUSTER_Set_PathProcessParallel_Sub( BOOL fixed , int side , int SchPointer , int cldepth , char *data ) { // 2007.02.21
	int m , l , ix , s;
	//
	if ( cldepth == -1 ) {
		//
		for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
			if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] != NULL ) {
				_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][MAX_PM_CHAMBER_DEPTH] = 0;
			}
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
					//
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
					}
					//
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						memcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] , data , MAX_PM_CHAMBER_DEPTH );
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][MAX_PM_CHAMBER_DEPTH] = 1;
					}
					//
				}
				//
			}
			else {
				//
				m = 0;
				l = strlen( data );
				//
				while ( TRUE ) {
					//
					if ( m >= MAX_CLUSTER_DEPTH ) break;
					//
					ix = MAX_PM_CHAMBER_DEPTH * m;
					//
					if      ( l < (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( l > ix ) {
							//
							if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
								_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
							}
							//
							if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
								_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error1[%d,%d,%d]\n" , side , SchPointer , cldepth );
								break;
							}
							//
							for ( s = 0 ; s < ( l - ix ) ; s++ ) {
								_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
							}
							for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
								_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = 0;
							}
							//
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][MAX_PM_CHAMBER_DEPTH] = 1;
							//
						}
						//
						break;
					}
					else if ( l >= (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
						}
						//
						if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error2[%d,%d,%d]\n" , side , SchPointer , cldepth );
							break;
						}
						//
						for ( s = 0 ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
						}
						//
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[m][MAX_PM_CHAMBER_DEPTH] = 1;
						//
					}
					//
					m++;
					//
				}
			}
		}
	}
	else {
		//
		if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] != NULL ) {
			_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] = 0;
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) {
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
				}
				//
				if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) {
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
				}
				else {
					memcpy( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] , data , MAX_PM_CHAMBER_DEPTH );
					_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] = 1;
				}
				//
			}
			else {
				//
				l = strlen( data );
				//
				if ( l > 0 ) {
					//
					if ( l > MAX_PM_CHAMBER_DEPTH ) l = MAX_PM_CHAMBER_DEPTH;
					//
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) {
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
					}
					//
					if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessParallel Memory Allocate Error3[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						//
						for ( s = 0 ; s < l ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][s] = CL_Get_ConvPathChar( data[ s ] );
						}
						for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][s] = 0;
						}
						//
						_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] = 1;
						//
					}
				}
			}
		}
	}
}

void _i_SCH_CLUSTER_Set_PathProcessParallel( int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	_i_SCH_CLUSTER_Set_PathProcessParallel_Sub( FALSE , side , SchPointer , cldepth , data );
}
void _i_SCH_CLUSTER_Set_PathProcessParallelF( int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	_i_SCH_CLUSTER_Set_PathProcessParallel_Sub( TRUE , side , SchPointer , cldepth , data );
}


void _i_SCH_CLUSTER_Set_PathProcessParallel2( int side , int SchPointer , int cldepth , int pindex , char data ) { // 2007.02.21
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) return;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return; // 2015.04.09
	_SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][pindex] = data;
}

/*
// 2015.04.09
char *_i_SCH_CLUSTER_Get_PathProcessParallel( int side , int SchPointer , int cldepth ) { return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] ); } // 2007.02.21
*/

// 2015.04.09
char *_i_SCH_CLUSTER_Get_PathProcessParallel( int side , int SchPointer , int cldepth ) {
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) return NULL;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return NULL;
	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] );
}

// 2015.04.10
char _i_SCH_CLUSTER_Get_PathProcessParallel2( int side , int SchPointer , int cldepth , int pindex ) {
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth] == NULL ) return 0;
	if ( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return 0;
	return( _SCH_INF_CLUSTER_DATA_AREA[side][SchPointer].PathProcessParallel[cldepth][pindex] );
}


/*
// 2015.04.09
void _i_SCH_CLUSTER_Set_PathProcessDepth_Sub( BOOL fixed , int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	int m , l , ix , s;
	//
	if ( cldepth == -1 ) {
		for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
			if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] != NULL ) {
				free( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] );
				_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = NULL;
			}
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
					//
					_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] != NULL ) {
						memcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] , data , MAX_PM_CHAMBER_DEPTH );
					}
					else {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					//
				}
				//
			}
			else {
				//
				m = 0;
				l = strlen( data );
				//
				while ( TRUE ) {
					//
					if ( m >= MAX_CLUSTER_DEPTH ) break;
					//
					ix = MAX_PM_CHAMBER_DEPTH * m;
					//
					if      ( l < (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( l > ix ) {
							//
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
							//
							if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
								_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error1[%d,%d,%d]\n" , side , SchPointer , cldepth );
								break;
							}
							//
							for ( s = 0 ; s < ( l - ix ) ; s++ ) {
								_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
							}
							for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
								_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = 0;
							}
							//
						}
						//
						break;
					}
					else if ( l >= (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
						//
						if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error2[%d,%d,%d]\n" , side , SchPointer , cldepth );
							break;
						}
						//
						for ( s = 0 ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
						}
						//
					}
					//
					m++;
					//
				}
			}
		}
	}
	else {
		//
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] != NULL ) {
			free( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] );
			_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = NULL;
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
				if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] != NULL ) {
					memcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] , data , MAX_PM_CHAMBER_DEPTH );
				}
				else {
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
				}
				//
			}
			else {
				//
				l = strlen( data );
				//
				if ( l > 0 ) {
					//
					if ( l > MAX_PM_CHAMBER_DEPTH ) l = MAX_PM_CHAMBER_DEPTH;
					//
					_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH , sizeof( char ) );
					//
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error3[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						//
						for ( s = 0 ; s < l ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][s] = CL_Get_ConvPathChar( data[ s ] );
						}
						for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][s] = 0;
						}
						//
					}
				}
			}
		}
	}
}

*/

//
// 2015.04.09
//
void _i_SCH_CLUSTER_Set_PathProcessDepth_Sub( BOOL fixed , int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	int m , l , ix , s;
	//
	if ( cldepth == -1 ) {
		for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
			if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] != NULL ) {
				_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][MAX_PM_CHAMBER_DEPTH] = 0;
			}
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				for ( m = 0 ; m < MAX_CLUSTER_DEPTH ; m++ ) {
					//
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
					}
					//
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						memcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] , data , MAX_PM_CHAMBER_DEPTH );
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][MAX_PM_CHAMBER_DEPTH] = 1;
					}
					//
				}
				//
			}
			else {
				//
				m = 0;
				l = strlen( data );
				//
				while ( TRUE ) {
					//
					if ( m >= MAX_CLUSTER_DEPTH ) break;
					//
					ix = MAX_PM_CHAMBER_DEPTH * m;
					//
					if      ( l < (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( l > ix ) {
							//
							if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
								_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
							}
							//
							if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
								_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error1[%d,%d,%d]\n" , side , SchPointer , cldepth );
								break;
							}
							//
							for ( s = 0 ; s < ( l - ix ) ; s++ ) {
								_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
							}
							for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
								_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = 0;
							}
							//
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][MAX_PM_CHAMBER_DEPTH] = 1;
							//
						}
						//
						break;
					}
					else if ( l >= (ix+MAX_PM_CHAMBER_DEPTH) ) {
						//
						if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
						}
						//
						if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m] == NULL ) {
							_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error2[%d,%d,%d]\n" , side , SchPointer , cldepth );
							break;
						}
						//
						for ( s = 0 ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][s] = CL_Get_ConvPathChar( data[ ix + s ] );
						}
						//
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[m][MAX_PM_CHAMBER_DEPTH] = 1;
					}
					//
					m++;
					//
				}
			}
		}
	}
	else {
		//
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] != NULL ) {
			_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] = 0;
		}
		//
		if ( data != NULL ) {
			//
			if ( fixed ) { // 2014.02.18
				//
				if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
					_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
				}
				//
				if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
					_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
				}
				else {
					memcpy( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] , data , MAX_PM_CHAMBER_DEPTH );
					_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] = 1;
				}
				//
			}
			else {
				//
				l = strlen( data );
				//
				if ( l > 0 ) {
					//
					if ( l > MAX_PM_CHAMBER_DEPTH ) l = MAX_PM_CHAMBER_DEPTH;
					//
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
					}
					//
					if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
						_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth Memory Allocate Error3[%d,%d,%d]\n" , side , SchPointer , cldepth );
					}
					else {
						//
						for ( s = 0 ; s < l ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][s] = CL_Get_ConvPathChar( data[ s ] );
						}
						for ( ; s < MAX_PM_CHAMBER_DEPTH ; s++ ) {
							_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][s] = 0;
						}
						//
						_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] = 1;
						//
					}
				}
			}
		}
	}
}

void _i_SCH_CLUSTER_Set_PathProcessDepth( int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	_i_SCH_CLUSTER_Set_PathProcessDepth_Sub( FALSE , side , SchPointer , cldepth , data );
}
void _i_SCH_CLUSTER_Set_PathProcessDepthF( int side , int SchPointer , int cldepth , char *data ) { // 2014.01.28
	_i_SCH_CLUSTER_Set_PathProcessDepth_Sub( TRUE , side , SchPointer , cldepth , data );
}

/*
//
// 2018.12.06
//
void _i_SCH_CLUSTER_Set_PathProcessDepth2( int side , int SchPointer , int cldepth , int pindex , char data ) { // 2014.01.28
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) return;
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return; // 2015.04.09
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][pindex] = data;
}
//
//
*/

//
// 2018.12.06
//
void _i_SCH_CLUSTER_Set_PathProcessDepth2( int side , int SchPointer , int cldepth , int pindex , char data ) { // 2014.01.28
	int i;
	//
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
		//
		_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] = calloc( MAX_PM_CHAMBER_DEPTH + 1 , sizeof( char ) );
		//
		if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) {
			//
			_IO_CIM_PRINTF( "_i_SCH_CLUSTER_Set_PathProcessDepth2 Memory Allocate Error[%d,%d,%d]\n" , side , SchPointer , cldepth );
			//
			return;
			//
		}
		//
		for ( i = 0 ; i <= MAX_PM_CHAMBER_DEPTH ; i++ ) {
			_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][i] = 0;
		}
		//
	}
	//
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][pindex] = data;
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] = 1;
	//
}
//


/*
// 2015.04.09
char *_i_SCH_CLUSTER_Get_PathProcessDepth( int side , int SchPointer , int cldepth ) { return( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] ); } // 2014.01.28
*/

// 2015.04.09
char *_i_SCH_CLUSTER_Get_PathProcessDepth( int side , int SchPointer , int cldepth ) {
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) return NULL;
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return NULL;
	return( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] );
}

// 2015.04.10
char _i_SCH_CLUSTER_Get_PathProcessDepth2( int side , int SchPointer , int cldepth , int pindex ) {
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth] == NULL ) return 0;
	if ( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][MAX_PM_CHAMBER_DEPTH] == 0 ) return 0;
	return( _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PathProcessDepth[cldepth][pindex] );
}

//
void _i_SCH_CLUSTER_Set_Ex_MtlOut( int side , int SchPointer , int data ) { // 2011.08.10
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].MtlOut = data;
}
void _i_SCH_CLUSTER_Set_Ex_CarrierIndex( int side , int SchPointer , int data ) { // 2011.08.10
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].CarrierIndex = data;
}
void _i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( int side , int SchPointer , int data ) { // 2012.04.01
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].OutCarrierIndex = data;
}
void _i_SCH_CLUSTER_Set_Ex_PrcsID( int side , int SchPointer , int data ) { // 2013.05.06
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PrcsID = data;
}
void _i_SCH_CLUSTER_Set_Ex_OrderMode( int side , int SchPointer , int data ) { // 2016.08.25
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].OrderMode = data;
}
void _i_SCH_CLUSTER_Set_Ex_MtlOutWait( int side , int SchPointer , int data ) { // 2016.11.02
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].MtlOutWait = data;
}


int  _i_SCH_CLUSTER_Get_Ex_MtlOut( int side , int SchPointer ) { // 2011.08.10
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].MtlOut;
}
int  _i_SCH_CLUSTER_Get_Ex_CarrierIndex( int side , int SchPointer ) { // 2011.08.10
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].CarrierIndex;
}
int  _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( int side , int SchPointer ) { // 2012.04.01
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].OutCarrierIndex;
}
int  _i_SCH_CLUSTER_Get_Ex_PrcsID( int side , int SchPointer ) { // 2013.05.06
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].PrcsID;
}
int  _i_SCH_CLUSTER_Get_Ex_OrderMode( int side , int SchPointer ) { // 2016.08.25
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].OrderMode;
}
int  _i_SCH_CLUSTER_Get_Ex_MtlOutWait( int side , int SchPointer ) { // 2016.11.02
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].MtlOutWait;
}


SCH_CLUSTER_Make_Str_Ex_API( EILInfo )

SCH_CLUSTER_Make_Str_Ex_API( JobName )
SCH_CLUSTER_Make_Str_Ex_API( RecipeName )
SCH_CLUSTER_Make_Str_Ex_API( PPID )
SCH_CLUSTER_Make_Str_Ex_API( WaferID )
SCH_CLUSTER_Make_Str_Ex_API( MaterialID )
SCH_CLUSTER_Make_Str_Ex_API( OutMaterialID )

void _i_SCH_CLUSTER_Set_Ex_DisableSkip( int side , int SchPointer , int data ) { // 2012.04.14
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].DisableSkip = data;
}
int  _i_SCH_CLUSTER_Get_Ex_DisableSkip( int side , int SchPointer ) { // 2012.04.14
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].DisableSkip;
}

void _i_SCH_CLUSTER_Set_Ex_UserDummy( int side , int SchPointer , int data ) { // 2015.10.12
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].UserDummy = data;
}
int  _i_SCH_CLUSTER_Get_Ex_UserDummy( int side , int SchPointer ) { // 2015.10.12
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].UserDummy;
}

void _i_SCH_CLUSTER_Set_Ex_UserControl_Mode( int side , int SchPointer , int data ) { // 2011.12.08
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].UserControl_Mode = data;
}

int  _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( int side , int SchPointer ) { // 2011.12.08
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].UserControl_Mode;
}

SCH_CLUSTER_Make_Str_Ex_API( UserControl_Data )

void _i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( int side , int SchPointer , int data ) { // 2012.09.13
	_SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].EIL_UniqueID = data;
}

int  _i_SCH_CLUSTER_Get_Ex_EIL_UniqueID( int side , int SchPointer ) { // 2012.09.13
	return _SCH_INF_CLUSTER_DATA2_AREA[side][SchPointer].EIL_UniqueID;
}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================



BOOL _i_SCH_CLUSTER_Chk_Parallel_Used_Disable( int side , int pt , int cldx , int ch ) { // 2007.02.21
	if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx ) == NULL ) return FALSE;
//	if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx )[ ch - PM1 ] < 0 ) return TRUE; // 2008.10.01 // 2015.04.10
	if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , cldx , ( ch - PM1 ) ) < 0 ) return TRUE; // 2008.10.01 // 2015.04.10
	return FALSE;
}
//
void _i_SCH_CLUSTER_Set_Parallel_Used_Restore( int side , int pt , int cldx ) { // 2007.02.21
	int k , m;
	char c;
	if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx ) == NULL ) return;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , cldx , k );
		if ( m < 0 ) {
			//
//			c = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx )[ (-m) - PM1 ]; // 2015.04.10
			c = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , cldx , ( (-m) - PM1 ) ); // 2015.04.10
			//
			if ( c < 0 ) { // 2008.10.01
				if ( _i_SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) _i_SCH_CLUSTER_Set_PathProcessChamber_Enable( side , pt , cldx , k );
				_i_SCH_CLUSTER_Set_PathProcessParallel2( side , pt , cldx , (-m) - PM1 , (char) ( -c ) );
			}
		}
	}
}
//
void _i_SCH_CLUSTER_After_Place_For_Parallel_Resetting( int side , int pt , int cldx , int ch ) { // 2007.02.21
	int i , k , m , l;
	char selid , c;
	//====================================================================================
	if ( cldx < 0 ) return;
	if ( cldx >= _i_SCH_CLUSTER_Get_PathRange( side , pt ) ) return;
	//====================================================================================
	if ( pt < 0 ) return;
	if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return;
	//====================================================================================
	if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx ) == NULL ) return;
	//====================================================================================
	if ( ch < PM1 ) return;
	if ( ch >= AL ) return;
	//====================================================================================
//	selid = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , cldx )[ch - PM1]; // 2015.04.10
	selid = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , cldx , (ch - PM1) ); // 2015.04.10
	//
	if ( selid < 0 ) return;
	if ( selid >= 100 ) return;
	//====================================================================================
	for ( i = cldx ; i < _i_SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i ) != NULL ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//				if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ k ] == selid ) break; // 2015.04.10
				if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , k ) == selid ) break; // 2015.04.10
//				if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ k ] == 1 ) break; // 2015.04.10
				if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , k ) == 1 ) break; // 2015.04.10
			}
			if ( k < MAX_PM_CHAMBER_DEPTH ) { // find
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , i , k );
					if      ( m > 0 ) {
//						c = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ m - PM1 ]; // 2015.04.10
						c = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , ( m - PM1 ) ); // 2015.04.10
						if ( ( c > 0 ) && ( c < 100 ) && ( c != selid ) && ( c != 1 ) ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , i , k );
							_i_SCH_CLUSTER_Set_PathProcessParallel2( side , pt , i , m - PM1 , (char) ( -c ) );
						}
					}
					else if ( m < 0 ) {
//						c = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ (-m) - PM1 ]; // 2015.04.10
						c = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , ( (-m) - PM1 ) ); // 2015.04.10
						if ( ( c > 0 ) && ( c < 100 ) && ( c != selid ) && ( c != 1 ) ) {
							_i_SCH_CLUSTER_Set_PathProcessParallel2( side , pt , i , (-m) - PM1 , (char) ( -c ) );
						}
					}
				}
			}
			else { // notfind
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , i , k );
					if      ( m > 0 ) {
//						c = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ m - PM1 ]; // 2015.04.10
						c = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , ( m - PM1 ) ); // 2015.04.10
						if ( ( c > 0 ) && ( c < 100 ) ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , i , k );
							_i_SCH_CLUSTER_Set_PathProcessParallel2( side , pt , i , m - PM1 , (char) ( -c ) );
						}
					}
					else if ( m < 0 ) {
//						c = _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ (-m) - PM1 ]; // 2015.04.10
						c = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , ( (-m) - PM1 ) ); // 2015.04.10
						if ( ( c > 0 ) && ( c < 100 ) ) {
							_i_SCH_CLUSTER_Set_PathProcessParallel2( side , pt , i , (-m) - PM1 , (char) ( -c ) );
						}
					}
				}
			}
		}
	}
	//====================================================================================
	l = 0;
	//====================================================================================
	for ( i = cldx + 1 ; i < _i_SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i ) == NULL ) break;
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , i , k );
			if      ( m > 0 ) break;
			else if ( m < 0 ) {
//				if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pt , i )[ (-m) - PM1 ] > 0 ) break; // 2015.04.10
				if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pt , i , ( (-m) - PM1 ) ) > 0 ) break; // 2015.04.10
			}
		}
		if ( k < MAX_PM_CHAMBER_DEPTH ) break;
		l++; // 2008.10.17
	}
	//====================================================================================
	for ( i = 0 ; i < l ; i++ ) _i_SCH_CLUSTER_Inc_PathDo( side , pt );
	//====================================================================================
}

//
void _SCH_CLUSTER_Parallel_Check_Curr_DisEna( int side , int pt , int ch , BOOL disable ) { // 2010.04.10
	int i , j , k , m , m2 , unusesel[100] , ss , es;
	char selid;

	for ( i = 0 ; i < 100 ; i++ ) unusesel[i] = 0;
	//
	if ( pt == -1 ) {
		ss = 0;
		es = MAX_CASSETTE_SLOT_SIZE - 1;
	}
	else {
		ss = pt;
		es = pt;
	}
	//
	for ( i = ss ; i <= es ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		//
		for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , i , j ) == NULL ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
				//
				if ( ch == -1 ) { // 2011.06.27
					if ( m >= 0 ) continue;
					m2 = -m;
				}
				else {
					if ( ( m != ch ) && ( -m != ch ) ) continue;
					m2 = ch;
				}
				//
//				selid = _i_SCH_CLUSTER_Get_PathProcessParallel( side , i , j )[ m2 - PM1 ]; // 2015.04.10
				selid = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , i , j , ( m2 - PM1 ) ); // 2015.04.10
				//
				if ( ( selid > 1 ) && ( selid < 100 ) ) {
					unusesel[selid] = 1;
				}
				//
			}
			//
		}
	}
	//
	for ( i = ss ; i <= es ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		//
		for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , i , j ) == NULL ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
				//
				if ( disable ) {
					if ( m > 0 ) {
						//
//						selid = _i_SCH_CLUSTER_Get_PathProcessParallel( side , i , j )[ m - PM1 ]; // 2015.04.10
						selid = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , i , j , ( m - PM1 ) ); // 2015.04.10
						//
						if ( ( selid > 1 ) && ( selid < 100 ) ) {
							if ( ( unusesel[selid] ) == 1 ) {
								_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , i , j , k );
							}
						}
					}
				}
				else {
					if ( m < 0 ) {
						//
						if ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( -m , FALSE , side ) ) { // 2010.05.27
							//
//							selid = _i_SCH_CLUSTER_Get_PathProcessParallel( side , i , j )[ -m - PM1 ]; // 2015.04.10
							selid = _i_SCH_CLUSTER_Get_PathProcessParallel2( side , i , j , ( -m - PM1 ) ); // 2015.04.10
							//
							if ( ( selid > 1 ) && ( selid < 100 ) ) {
								if ( ( unusesel[selid] ) == 1 ) {
									_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( side , i , j , k );
								}
							}
							else if ( selid < 0 ) {
								if ( ( unusesel[-selid] ) == 1 ) {
									_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( side , i , j , k );
									//
									_i_SCH_CLUSTER_Set_PathProcessParallel2( side , i , j , -m , (char) ( -selid ) );
								}
							}
						}
					}
				}
			}
			//
		}
	}
}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================

BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer( int side , int pointer , int dotrg ) { // 2003.08.11 // 2005.01.26
	if ( side    <    0 ) return FALSE;
	if ( side    >= MAX_CASSETTE_SIDE ) return FALSE;
	if ( pointer <    0 ) return FALSE;
	if ( pointer >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	EnterCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
	//
	if ( dotrg == -1 ) {
		if ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , PATHDO_WAFER_RETURN ); // 2005.01.26
			_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RETURN_REQUEST ); // 2005.01.26
		}
	}
	else if ( dotrg == -2 ) { // 2009.01.19
		_i_SCH_CLUSTER_Set_PathDo( side , pointer , PATHDO_WAFER_RETURN );
		_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RETURN_REQUEST );
	}
	else {
		if ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) == dotrg ) {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , PATHDO_WAFER_RETURN ); // 2005.01.26
			_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RETURN_REQUEST ); // 2005.01.26
			//
			LeaveCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
			//
			return TRUE;
		}
	}
//
	LeaveCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
	//
	return FALSE;
}


void SCHEDULER_Check_and_Make_Restore_Wafer_After_Place_CM_Reset_Sub( int side ) { // 2010.02.02
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( !_i_SCH_MODULE_Chk_TM_Finish_Status( side ) ) return;
		_i_SCH_MODULE_Set_TM_DoPointer( side , 0 );
	}
	else {
		if ( !_i_SCH_MODULE_Chk_FM_Finish_Status_Sub( side ) ) return;
		_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , 0 );
	}
}
//----------------------------------------------------------------------------
void _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( int side , int point ) { // 2004.12.01
	int ob; // 2017.02.12
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	ob = _i_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( side , point , 0 ); // 2008.01.21
	//
	if ( _i_SCH_CLUSTER_Get_PathDo( side , point ) == PATHDO_WAFER_RETURN ) {
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , point ) == SCHEDULER_RETURN_REQUEST ) {

			if ( _i_SCH_CLUSTER_Get_PathIn( side , point ) >= BM1 ) { // 2015.02.13
//				if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , point ) , 0 ) ) { // 2017.09.11
				if ( SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( side , point ) , 0 , 97 , side , point ) >= 0 ) { // 2017.09.11
					_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_CM_END );
				}
				else {
					_i_SCH_CLUSTER_Set_PathDo( side , point , 0 );
					_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_READY );
					_i_SCH_MODULE_Set_TM_DoPointer( side , 0 );
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , 0 );
					//
					if ( ob != 0 ) _i_SCH_CLUSTER_Unuse_Chamber_Restore( side , point ); // 2017.02.12
					//
					SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( side , point ); // 2017.09.08
					//
				}
			}
			else {
				//
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2016.07.28
					_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_CM_END );
				}
				else {
					_i_SCH_CLUSTER_Set_PathDo( side , point , 0 );
					_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_READY );
					_i_SCH_MODULE_Set_TM_DoPointer( side , 0 );
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , 0 );
					//
					if ( ob != 0 ) _i_SCH_CLUSTER_Unuse_Chamber_Restore( side , point ); // 2017.02.12
					//
					SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( side , point ); // 2017.09.08
					//
				}
			}
		}
		else {
			_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_CM_END );
		}
	}
	else {
		_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_CM_END );
	}
	//
	switch ( _i_SCH_CLUSTER_Get_PathRun( side , point , 13 , 2 ) ) { // 2008.10.31
	case 1 :
	case 5 :
	case 9 :
		if ( _i_SCH_CLUSTER_Get_PathRun( side , point , 0 , 2 ) >= _i_SCH_CLUSTER_Get_PathRun( side , point , 10 , 2 ) ) { // 2009.02.14
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 0 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 1 );
			_i_SCH_CLUSTER_Set_PathDo( side , point , 0 );
			_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_READY );
			//
			SCHEDULER_Check_and_Make_Restore_Wafer_After_Place_CM_Reset_Sub( side ); // 2010.02.02
			//
			SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( side , point ); // 2017.09.08
			//
		}
		//
		break;
	case 2 :
	case 6 :
	case 10 :
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-DELETE\t[Side=%d][Pt=%d]\n" , side , point );
//------------------------------------------------------------------------------------------------------------------
		_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 0 );
		//
		if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , point ) , 1 ) ) _i_SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , side , point , 4 );
		break;
	case 3 :
	case 7 :
	case 11 :
		if ( _i_SCH_CLUSTER_Get_PathRun( side , point , 0 , 2 ) >= _i_SCH_CLUSTER_Get_PathRun( side , point , 10 , 2 ) ) { // 2009.02.14
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 0 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 3 );
			_i_SCH_CLUSTER_Set_PathDo( side , point , 0 );
			_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_READY );
			SCHEDULER_Check_and_Make_Restore_Wafer_After_Place_CM_Reset_Sub( side ); // 2010.02.02
		}
		break;
	case 4 :
	case 8 :
	case 12 :
		if ( _i_SCH_CLUSTER_Get_PathRun( side , point , 0 , 2 ) >= _i_SCH_CLUSTER_Get_PathRun( side , point , 10 , 2 ) ) { // 2009.02.14
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 0 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathRun( side , point , 13 , 2 , 4 );
			_i_SCH_CLUSTER_Set_PathDo( side , point , 0 );
			_i_SCH_CLUSTER_Set_PathStatus( side , point , SCHEDULER_READY );
			SCHEDULER_Check_and_Make_Restore_Wafer_After_Place_CM_Reset_Sub( side ); // 2010.02.02
		}
		break;
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
}


//BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( int CHECKING_SIDE , int *rpt , BOOL FMMode , BOOL slotmode , int slot_pt ) { // 2013.12.13
BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( int CHECKING_SIDE , int *rpt , BOOL FMMode , BOOL slotmode , int slot_pt , BOOL rerun ) { // 2013.12.13
	int i , j , k , l , m , sp , ep , x , sslot , lslot , selslot;
	BOOL Find;
	//
	EnterCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
	//
	//
	*rpt = -1;
	//
	if ( slotmode ) {
		//
		sp = 0;
		//
		if ( slot_pt > 0 ) {
			ep = MAX_CASSETTE_SLOT_SIZE;
		}
		else {
			//
			/*
			// 2014.01.28
			//
			if ( FMMode ) {
//				ep = _i_SCH_MODULE_Get_FM_DoPointer_Sub( CHECKING_SIDE ); // 2012.11.14
				ep = _i_SCH_MODULE_Get_FM_DoPointer_Sub( CHECKING_SIDE ) + 1; // 2012.11.14
			}
			else {
//				ep = _i_SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ); // 2012.11.14
				ep = _i_SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE ) + 1; // 2012.11.14
			}
			//
			if ( ep > MAX_CASSETTE_SLOT_SIZE ) ep = MAX_CASSETTE_SLOT_SIZE; // 2012.11.14
			//
			*/
			// 2014.01.28
			if ( FMMode ) {
				ep = _i_SCH_MODULE_Get_FM_DoPointer_Sub( CHECKING_SIDE );
			}
			else {
				ep = _i_SCH_MODULE_Get_TM_DoPointer( CHECKING_SIDE );
			}
			//
			if ( ep == 0 ) {
				//
				ep = MAX_CASSETTE_SLOT_SIZE;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) == SCHEDULER_READY ) {
						ep = i;
						break;
					}
					//
				}
				//
			}
			//
			if ( ep > MAX_CASSETTE_SLOT_SIZE ) ep = MAX_CASSETTE_SLOT_SIZE;
			//
		}
	}
	else { // 2012.04.01(Testing)
		//
		sp = slot_pt;
		ep = sp+1;
		//
	}
	//
	selslot = 0; // 2012.04.01
	//
	for ( Find = FALSE , i = sp ; i < ep ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
		//
		if ( slotmode ) {
			if ( slot_pt > 0 ) {
				if ( slot_pt != _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) ) continue;
			}
		}
		//
		selslot = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ); // 2012.04.01
		//
		switch( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) ) { // 2003.10.09
		case SCHEDULER_READY	:
			//
			if ( slotmode && ( slot_pt <= 0 ) ) {
//				l = -1; // 2018.09.10
				l = -5; // 2018.09.10
			}
			else {
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2017.08.18
					l = -4;
				}
				else {
					l = -3;
				}
			}
			//
			/*
			// 2012.04.01
			if ( slot > 0 ) { // 2007.11.08
				l = -3;
			}
			else {
				l = -1;
			}
			*/
			//
			break;
		case SCHEDULER_SUPPLY	:
		case SCHEDULER_STARTING	:
			l = -1;
			break;
		case SCHEDULER_BM_END	:
		case SCHEDULER_CM_END	:
		case SCHEDULER_RETURN_REQUEST :
			l = -2;
			break;
		case SCHEDULER_RUNNING	:
			l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
			break;
		case SCHEDULER_RUNNINGW	:
			l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
			break;
		case SCHEDULER_RUNNING2	:
			l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
			break;
		case SCHEDULER_WAITING	:
			l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i ) - 1; // 2003.10.09
			break;
		default :
			l = -2; // 2018.09.10
			break;
		}
		//
		if      ( l == -1 ) {
			//
			*rpt = i;
			//
			_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
			if ( rerun ) _i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2013.12.13
			//
			Find = TRUE;
		}
		else if ( l == -2 ) {
		}
		else if ( l == -3 ) {
			//
			*rpt = i;
			//
//			_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN ); // 2018.09.10
			//
//			if ( rerun ) // 2013.12.13 // 2018.09.10
//				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2018.09.10
//			else // 2018.09.10
//				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_CM_END ); // 2018.09.10
			//
			if ( !rerun ) {
				_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_CM_END );
			}
			//
			Find = TRUE; // 2011.02.21
		}
		else if ( l == -4 ) { // 2017.08.18
			//
			*rpt = i;
			//
//			_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN ); // 2018.09.10
//			if ( rerun ) // 2018.09.10
//				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2018.09.10
//			else // 2018.09.10
//				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_SUPPLY ); // 2018.09.10
			//
			if ( !rerun ) { // 2018.09.10
				_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_SUPPLY );
			}
			//
			Find = TRUE;
		}
		else if ( l == -5 ) { // 2018.09.10
			//
			*rpt = i;
			//
			if ( !rerun ) {
				_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
				_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_CM_END );
			}
			//
			Find = TRUE;
		}
		else {
			x = FALSE;
			//
			for ( j = 0 ; j < l ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
					if ( m > 0 ) {
						if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_NOTRUN ) {
							if ( ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( m ) % 2 ) != 1 ) { // 2014.01.30
								x = TRUE;
								j = l;
								break;
							}
						}
					}
				}
			}
			//
			if ( !x ) {
				//
				*rpt = i;
				//
				_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
				if ( rerun ) _i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2013.12.13
				//
				Find = TRUE;
			}
		}
		//
		x = _i_SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , i , 13 , 2 ); // 2009.02.16
		if ( ( x != 2 ) && ( x != 6 ) && ( x != 10 ) ) _i_SCH_CLUSTER_Set_PathRun( CHECKING_SIDE , i , 13 , 2 , 0 ); // 2009.02.16
		//
	}
	//===========================================================================
	// 2009.03.25
	//===========================================================================
	if ( !slotmode || ( slot_pt > 0 ) ) {
		if ( ( selslot > 0 ) && _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
			//---------
			j = ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
			sslot = ( ( ( selslot - 1 ) / j ) * j ) + 1;
			lslot = ( ( ( selslot - 1 ) / j ) * j ) + j;
			//---------
			for ( j = sslot ; j <= lslot ; j++ ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( j != _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i ) ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) != 0 ) continue;
					//
					switch( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) ) {
					case SCHEDULER_READY	:
						if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2017.08.18
							l = -4;
						}
						else {
							l = -3;
						}
						break;
					case SCHEDULER_SUPPLY	:
					case SCHEDULER_STARTING	:
						l = -1;
						break;
					case SCHEDULER_BM_END	:
					case SCHEDULER_CM_END	:
					case SCHEDULER_RETURN_REQUEST :
						l = -2;
						break;
					case SCHEDULER_RUNNING	:
						l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
						break;
					case SCHEDULER_RUNNINGW	:
						l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
						break;
					case SCHEDULER_RUNNING2	:
						l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i );
						break;
					case SCHEDULER_WAITING	:
						l = _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i ) - 1;
						break;
					}
					if      ( l == -1 ) {
						//
						_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
						if ( rerun ) _i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2013.12.13
						//
					}
					else if ( l == -2 ) {
					}
					else if ( l == -3 ) {
						//_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN ); // 2018.09.10
						//if ( rerun ) // 2013.12.13 // 2018.09.10
						//	_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2018.09.10
						//else // 2018.09.10
						//	_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_CM_END ); // 2018.09.10
						//
						if ( !rerun ) { // 2018.09.10
							_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
							_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_CM_END );
						}
						//
					}
					else if ( l == -4 ) { // 2017.08.18
						//_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN ); // 2018.09.10
						//if ( rerun ) // 2018.09.10
						//	_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2018.09.10
						//else // 2018.09.10
						//	_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_SUPPLY ); // 2018.09.10
						//
						if ( !rerun ) { // 2018.09.10
							_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
							_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_SUPPLY );
						}
						//
					}
					else {
						_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , PATHDO_WAFER_RETURN );
						if ( rerun ) _i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_RETURN_REQUEST ); // 2013.12.13
					}
					//
					x = _i_SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , i , 13 , 2 );
					if ( ( x != 2 ) && ( x != 6 ) && ( x != 10 ) ) _i_SCH_CLUSTER_Set_PathRun( CHECKING_SIDE , i , 13 , 2 , 0 );
					//
				}
			}
		}
	}
	//=============================================================================
	LeaveCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
	//
	return Find;
}

BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_FDHC( int CHECKING_SIDE , BOOL FMMode , int slot ) {
	int Res;
	int p;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
//	Res = _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( CHECKING_SIDE , &p , FMMode , TRUE , slot ); // 2013.12.13
	Res = _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( CHECKING_SIDE , &p , FMMode , TRUE , slot , FALSE ); // 2013.12.13
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
}

//=======================================================================================
void _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop_FDHC( int CHECKING_SIDE , BOOL FMMode ) {
	int i;
	BOOL check; // 2003.10.21 // 2008.07.29
	//
	EnterCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	check = FALSE; // 2008.07.29
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
			if ( _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i ) == PATHDO_WAFER_RETURN ) {
				if ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) == SCHEDULER_CM_END ) {
					//
					if ( SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , i ) , 1 , 98 , CHECKING_SIDE , i ) < 0 ) { // 2017.09.11 dummy
						//
						check = TRUE; // 2003.10.21
						//
						_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , i , SCHEDULER_READY );
						_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , i , 0 );
						//
						SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( CHECKING_SIDE , i ); // 2017.09.08
						//
					}
					//
				}
			}
			else if ( _i_SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , i ) == 0 ) { // 2004.05.15
				// 2008.01.22
				if ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) == SCHEDULER_READY ) {
					check = TRUE;
				}
			}
		}
	}
	if ( check ) { // 2003.10.21
		if ( FMMode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( CHECKING_SIDE , 0 );
		_i_SCH_MODULE_Set_TM_DoPointer( CHECKING_SIDE , 0 );
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	LeaveCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
}

//=======================================================================================
int _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Event_FDHC( int pathin , int slotin , BOOL restore ) { // 2012.02.07 // 2012.02.16
	int s , i , errcode;
	int checkside;
	BOOL check;
	//
	EnterCriticalSection( &CR_CLUSTERDB );
	//
	check = FALSE;
	//
	errcode = 0;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		//
		checkside = -1;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( s , i ) != pathin ) continue;
			if ( _i_SCH_CLUSTER_Get_SlotIn( s , i ) != slotin ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_READY ) {
				if ( restore ) {
					errcode = 1;
					continue;
				}
			}
			if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_RETURN_REQUEST ) {
				if ( restore ) {
					errcode = 2;
					continue;
				}
			}
			//
			checkside = s;
			//
			if      ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_READY ) {
				if ( !restore ) {
					_i_SCH_CLUSTER_Set_PathStatus( s , i , SCHEDULER_CM_END );
					_i_SCH_CLUSTER_Set_PathDo( s , i , 0 );
				}
			}
			else if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_CM_END ) {
				if ( restore ) {
					_i_SCH_CLUSTER_Set_PathStatus( s , i , SCHEDULER_READY );
					_i_SCH_CLUSTER_Set_PathDo( s , i , 0 );
				}
			}
			else if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_RETURN_REQUEST ) {
				if ( !restore ) {
					_i_SCH_CLUSTER_Set_PathStatus( s , i , SCHEDULER_BM_END );
					_i_SCH_CLUSTER_Set_PathDo( s , i , PATHDO_WAFER_RETURN );
				}
			}
			else {
				if ( restore ) {
					_i_SCH_CLUSTER_Set_PathStatus( s , i , SCHEDULER_RETURN_REQUEST );
					_i_SCH_CLUSTER_Set_PathDo( s , i , PATHDO_WAFER_RETURN );
				}
				else {
					_i_SCH_CLUSTER_Set_PathDo( s , i , PATHDO_WAFER_RETURN );
				}
			}
		}
		//
		if ( checkside != -1 ) {
			//
			check = TRUE;
			//
			_i_SCH_MODULE_Set_FM_DoPointer_Sub( checkside , 0 );
			_i_SCH_MODULE_Set_TM_DoPointer( checkside , 0 );
		}
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	LeaveCriticalSection( &CR_CLUSTERDB );
	//
	if ( check ) {
		return errcode;
	}
	else {
		return errcode + 10;
	}
	//
}

//=======================================================================================
void _i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( int s , int p , int ti ) { // 2013.01.07
	if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) ) {
		if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				if ( ti ) _i_SCH_MODULE_Inc_TM_InCount( s );
				_i_SCH_MODULE_Inc_FM_InCount( s );
			}
			else {
				_i_SCH_MODULE_Inc_TM_InCount( s );
			}
		}
	}
}

int _i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_From_Event_FDHC( int ch , int slot0 ) { // 2012.12.05
	int i , j , s , p , w , ti , slot;
	int check;
	int s2 , p2 , w2; // 2014.07.17
	//
	EnterCriticalSection( &CR_CLUSTERDB );
	//
	if ( ch == MAX_CHAMBER ) { // 2013.01.07
		//
		check = 1;
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			//
			if      ( ( i >= CM1 ) && ( i < PM1 ) ) {
				continue;
			}
			else if ( ( i >= PM1 ) && ( i < AL ) ) {
				//
				for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
					//
					s = _i_SCH_MODULE_Get_PM_SIDE( i , j );
					p = _i_SCH_MODULE_Get_PM_POINTER( i , j );
					w = _i_SCH_MODULE_Get_PM_WAFER( i , j );
					//
					if ( w <= 0 ) continue;
					//
					if ( ( w % 100 ) > 0 ) { // 2013.05.08
						//
						_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
						//
					}
					if ( ( w / 100 ) > 0 ) { // 2013.05.08
						//
						s = _i_SCH_MODULE_Get_PM_SIDE( i , j + 1 );
						p = _i_SCH_MODULE_Get_PM_POINTER( i , j + 1 );
						//
						_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
						//
					}
					//
					_i_SCH_MODULE_Set_PM_WAFER( i , j , 0 );
					//
					check = 0;
					//
				}
				//
			}
			else if ( ( i == AL ) || ( i == F_AL ) ) {
				//
				for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					//
					s = _i_SCH_MODULE_Get_MFALS_SIDE( j );
					p = _i_SCH_MODULE_Get_MFALS_POINTER( j );
					w = _i_SCH_MODULE_Get_MFALS_WAFER( j );
					//
					if ( w <= 0 ) continue;
					//
					_i_SCH_MODULE_Set_MFALS_WAFER( j , 0 );
					//
					_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 0 );
					//
					check = 0;
					//
				}
				//
			}
			else if ( ( i == IC ) || ( i == F_IC ) ) {
				//
				for ( j = 1 ; j <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; j++ ) {
					//
					s = _i_SCH_MODULE_Get_MFIC_SIDE( j );
					p = _i_SCH_MODULE_Get_MFIC_POINTER( j );
					w = _i_SCH_MODULE_Get_MFIC_WAFER( j );
					//
					if ( w <= 0 ) continue;
					//
					_i_SCH_MODULE_Set_MFIC_WAFER( j , 0 );
					//
					_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 0 );
					//
					check = 0;
					//
				}
				//
			}
			else if ( ( i >= BM1 ) && ( i < TM ) ) {
				//
				for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					//
					s = _i_SCH_MODULE_Get_BM_SIDE( i , j );
					p = _i_SCH_MODULE_Get_BM_POINTER( i , j );
					w = _i_SCH_MODULE_Get_BM_WAFER( i , j );
					//
					if ( w <= 0 ) continue;
					//
					_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , 0 );
					//
					_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 0 );
					//
					check = 0;
					//
				}
				//
			}
			else if ( ( i >= TM ) && ( i < FM ) ) {
				//
				for ( j = 0 ; j < TD_STATION ; j++ ) {
					//
					/*
					// 2014.07.17
					s = _i_SCH_MODULE_Get_TM_SIDE( i-TM , j );
					p = _i_SCH_MODULE_Get_TM_POINTER( i-TM , j );
					w = _i_SCH_MODULE_Get_TM_WAFER( i-TM , j ) % 100;
					//
					if ( w > 0 ) {
						//
						_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
						//
						check = 0;
						//
					}
					//
					s = _i_SCH_MODULE_Get_TM_SIDE2( i-TM , j );
					p = _i_SCH_MODULE_Get_TM_POINTER2( i-TM , j );
					w = _i_SCH_MODULE_Get_TM_WAFER( i-TM , j ) / 100;
					//
					if ( w > 0 ) {
						_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
						//
						check = 0;
						//
					}
					*/
					//
					// 2014.07.17
					//
					s = _i_SCH_MODULE_Get_TM_SIDE( i-TM , j );
					p = _i_SCH_MODULE_Get_TM_POINTER( i-TM , j );
					w = _i_SCH_MODULE_Get_TM_WAFER( i-TM , j ) % 100;
					//
					if ( w > 0 ) {
						//
						_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
						//
						check = 0;
						//
						s = _i_SCH_MODULE_Get_TM_SIDE2( i-TM , j );
						p = _i_SCH_MODULE_Get_TM_POINTER2( i-TM , j );
						w = _i_SCH_MODULE_Get_TM_WAFER( i-TM , j ) / 100;
						//
						if ( w > 0 ) {
							//
							_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
							//
							check = 0;
							//
						}
						//
					}
					else {
						s = _i_SCH_MODULE_Get_TM_SIDE( i-TM , j );
						p = _i_SCH_MODULE_Get_TM_POINTER( i-TM , j );
						w = _i_SCH_MODULE_Get_TM_WAFER( i-TM , j ) / 100;
						//
						if ( w > 0 ) {
							//
							_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 1 );
							//
							check = 0;
							//
						}
						//
					}
					//
					_i_SCH_MODULE_Set_TM_WAFER( i-TM , j , 0 );
					//
				}
				//
			}
			else if ( i == FM ) {
				//
				for ( j = 0 ; j < TD_STATION ; j++ ) {
					//
					s = _i_SCH_MODULE_Get_FM_SIDE( j );
					p = _i_SCH_MODULE_Get_FM_POINTER( j );
					w = _i_SCH_MODULE_Get_FM_WAFER( j );
					//
					if ( w <= 0 ) continue;
					//
					_i_SCH_MODULE_Set_FM_WAFER( j , 0 );
					//
					_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , 0 );
					//
					check = 0;
					//
				}
				//
			}
			//
		}
	}
	else {
		//
		check = 0;
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( i == 0 ) slot = slot0 % 100;
			else          slot = slot0 / 100;
			//
			if ( slot <= 0 ) continue;
			//
			ti = 0;
			//
			if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
				continue;
			}
			else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
				//
				ti = 1;
				//
				if ( ( slot <= 0 ) || ( slot > MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				s = _i_SCH_MODULE_Get_PM_SIDE( ch , slot-1 );
				p = _i_SCH_MODULE_Get_PM_POINTER( ch , slot-1 );
				w = _i_SCH_MODULE_Get_PM_WAFER( ch , slot-1 );
				//
				if ( w <= 0 ) continue;
				//
				_i_SCH_MODULE_Set_PM_WAFER( ch , slot-1 , 0 );
				//
			}
			else if ( ( ch == AL ) || ( ch == F_AL ) ) {
				//
				if ( ( slot <= 0 ) || ( slot > MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				s = _i_SCH_MODULE_Get_MFALS_SIDE( slot );
				p = _i_SCH_MODULE_Get_MFALS_POINTER( slot );
				w = _i_SCH_MODULE_Get_MFALS_WAFER( slot );
				//
				if ( w <= 0 ) continue;
				//
				_i_SCH_MODULE_Set_MFALS_WAFER( slot , 0 );
				//
			}
			else if ( ( ch == IC ) || ( ch == F_IC ) ) {
				//
				if ( ( slot <= 0 ) || ( slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				s = _i_SCH_MODULE_Get_MFIC_SIDE( slot );
				p = _i_SCH_MODULE_Get_MFIC_POINTER( slot );
				w = _i_SCH_MODULE_Get_MFIC_WAFER( slot );
				//
				if ( w <= 0 ) continue;
				//
				_i_SCH_MODULE_Set_MFIC_WAFER( slot , 0 );
				//
			}
			else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
				//
				if ( ( slot <= 0 ) || ( slot > MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( ch , slot );
				p = _i_SCH_MODULE_Get_BM_POINTER( ch , slot );
				w = _i_SCH_MODULE_Get_BM_WAFER( ch , slot );
				//
				if ( w <= 0 ) continue;
				//
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch , slot , 0 , 0 );
				//
			}
			else if ( ( ch >= TM ) && ( ch < FM ) ) {
				//
				ti = 1;
				//
				if ( ( (slot-1) < TA_STATION ) || ( (slot-1) > TD_STATION ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				/*
				// 2014.07.17
				//
				if ( i == 0 ) {
					s = _i_SCH_MODULE_Get_TM_SIDE( ch-TM , (slot-1) );
					p = _i_SCH_MODULE_Get_TM_POINTER( ch-TM , (slot-1) );
					w = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) % 100;
				}
				else {
					s = _i_SCH_MODULE_Get_TM_SIDE2( ch-TM , (slot-1) );
					p = _i_SCH_MODULE_Get_TM_POINTER2( ch-TM , (slot-1) );
					w = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) / 100;
				}
				//
				if ( w <= 0 ) continue;
				//
				if ( i == 0 ) {
					//
					_i_SCH_MODULE_Set_TM_WAFER( ch-TM , (slot-1) , ( _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) / 100 ) * 100 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( ch-TM , (slot-1) , ( _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) % 100 ) );
				}
				*/
				//
				// 2014.07.17
				//
				if ( i == 0 ) {
					//
					s = _i_SCH_MODULE_Get_TM_SIDE( ch-TM , (slot-1) );
					p = _i_SCH_MODULE_Get_TM_POINTER( ch-TM , (slot-1) );
					w = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) % 100;
					//
					w2 = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) / 100;
					//
				}
				else {
					//
					w = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) / 100;
					//
					w2 = _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) % 100;
					//
					if ( w2 > 0 ) {
						s = _i_SCH_MODULE_Get_TM_SIDE2( ch-TM , (slot-1) );
						p = _i_SCH_MODULE_Get_TM_POINTER2( ch-TM , (slot-1) );
					}
					else {
						s = _i_SCH_MODULE_Get_TM_SIDE( ch-TM , (slot-1) );
						p = _i_SCH_MODULE_Get_TM_POINTER( ch-TM , (slot-1) );
					}
				}
				//
				if ( w <= 0 ) continue;
				//
				if ( i == 0 ) {
					//
					_i_SCH_MODULE_Set_TM_WAFER( ch-TM , (slot-1) , ( _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) / 100 ) * 100 );
					//
					if ( w2 > 0 ) {
						//
						s2 = _i_SCH_MODULE_Get_TM_SIDE2( ch-TM , (slot-1) );
						p2 = _i_SCH_MODULE_Get_TM_POINTER2( ch-TM , (slot-1) );
						//
						_i_SCH_MODULE_Set_TM_SIDE_POINTER1( ch-TM , (slot-1) , s2 , p2 );
						//
					}
					//
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( ch-TM , (slot-1) , ( _i_SCH_MODULE_Get_TM_WAFER( ch-TM , (slot-1) ) % 100 ) );
				}
				//
			}
			else if (   ch == FM  ) {
				//
				if ( ( (slot-1) < TA_STATION ) || ( (slot-1) > TD_STATION ) ) {
					if ( check == 0 ) check = 1;
					continue;
				}
				//
				s = _i_SCH_MODULE_Get_FM_SIDE( (slot-1) );
				p = _i_SCH_MODULE_Get_FM_POINTER( (slot-1) );
				w = _i_SCH_MODULE_Get_FM_WAFER( (slot-1) );
				//
				if ( w <= 0 ) continue;
				//
//				_i_SCH_MODULE_Set_FM_SIDE_WAFER( (slot-1) , 0 , 0 ); // 2013.01.11
				_i_SCH_MODULE_Set_FM_WAFER( (slot-1) , 0 ); // 2013.01.11
				//
			}
			else {
				return 1;
			}
			//
			_i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_Sub( s , p , ti ); // 2013.01.07
			//
		}
	}
	//
	LeaveCriticalSection( &CR_CLUSTERDB );
	//
	return check;
	//
}


int _i_SCH_CLUSTER_Check_and_Set_Extra_Flag_From_Event_FDHC( int pathin , int slotin , int flag , int data ) { // 2013.02.25
	int s , i , Res;
	//
	EnterCriticalSection( &CR_CLUSTERDB );
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = 9;
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( s , i ) != pathin ) continue;
			if ( _i_SCH_CLUSTER_Get_SlotIn( s , i ) != slotin ) continue;
			//
			Res =  _i_SCH_CLUSTER_Set_Extra_Flag( s , i , flag , data );
			//
			s = MAX_CASSETTE_SIDE;
			//
			break;
			//
		}
		//
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	LeaveCriticalSection( &CR_CLUSTERDB );
	//
	return Res;
	//
}

//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
//==============================================================================================================
int _i_SCH_CLUSTER_Select_OneChamber_and_OtherBuffering( int side , int pointer , int cldepth , int ch , int ch2 ) { // 2008.01.21
	int i , m;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 2;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , cldepth , i );
		if ( ch != -1 ) {
			if ( ( m != 0 ) && ( ( m == ch ) || ( -m == ch ) ) ) break;
		}
		if ( ch2 != -1 ) {
			if ( ( m != 0 ) && ( ( m == ch2 ) || ( -m == ch2 ) ) ) break;
		}
	}
	//
	if ( i == MAX_PM_CHAMBER_DEPTH ) return 3;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , cldepth , i );
		if ( ( m != 0 ) && ( m != ch ) && ( -m != ch ) && ( m != ch2 ) && ( -m != ch2 ) ) {
			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , m );
			_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , cldepth , i );
		}
	}
	//
	return 0;
}

int _i_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( int side , int pointer , int cldepth ) { // 2008.01.21
	int i , m , c = 0;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 2;
	if ( _i_SCH_CLUSTER_Get_PathRange( side , pointer ) >= MAX_CLUSTER_DEPTH ) return 3; // 2010.03.01
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i );
		if ( m > 0 ) {
			c++;
			if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , cldepth , i , m );
			}
			else {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , cldepth , i , -m );
			}
		}
		else if ( m < 0 ) {
			c++;
			if ( _i_SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , cldepth , i , -m );
			}
			else {
				_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , cldepth , i , m );
			}
		}
		_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , MAX_CLUSTER_DEPTH - 1 , i );
	}
	//====================================================================================================================
	if ( c > 0 ) return 0;
	return 4;
}


int _i_SCH_CLUSTER_Unuse_Chamber_Restore( int side , int pointer ) { // 2017.02.12
	int i , r , m , c = 0 , pr;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 2;
	pr = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
	if ( pr >= MAX_CLUSTER_DEPTH ) return 3;
	//
	for ( r = 0 ; r < pr ; r++ ) {
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , r , i );
			//
			if ( m < 0 ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , -m ) == MUF_01_USE ) {
					//
					if ( _i_SCH_MODULE_GET_USE_ENABLE( -m , FALSE , side ) ) {
						//
						c++;
						//
						_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , r , i , -m );
						//
					}
				}
				//
			}
		}
	}
	//====================================================================================================================
	if ( c > 0 ) return 0;
	return 4;
}



//==============================================================================================================
//==============================================================================================================
int SCHEDULER_CLUSTER_DefaultUse_Data[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2017.09.08

void SCHEDULER_CLUSTER_Set_DefaultUse_Data( int side ) { // 2017.09.08
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		SCHEDULER_CLUSTER_DefaultUse_Data[ side ][i] = _i_SCH_CLUSTER_Get_PathRange( side , i );
		//
	}
	//
}



void _i_SCH_CLUSTER_Init_ClusterData( int side , BOOL switchmode ) {
	int i , j , k , Temp;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( !switchmode ) {
			//
			SCHEDULER_CLUSTER_DefaultUse_Data[ side ][i] = -9999; // 2017.09.08
			//
			_i_SCH_CLUSTER_Set_PathRange( side , i , -1 );
			_i_SCH_CLUSTER_Set_PathDo( side , i , 0 );
			_i_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_READY );
		}
		else {
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
				//
				if ( SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( side , i ) , 1 , 99 , side , i ) >= 0 ) { // 2017.09.11 dummy
					//
					if ( SCHEDULER_CLUSTER_DefaultUse_Data[ side ][i] < 0 ) {
						//
						_i_SCH_CLUSTER_Set_PathRange( side , i , -1 );
						//
					}
					else {
						_i_SCH_CLUSTER_Set_PathDo( side , i , 0 );
						_i_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_READY );
					}
					//
				}
				else {
					_i_SCH_CLUSTER_Set_PathDo( side , i , 0 );
					_i_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_READY );
				}
				//
			}
		}
		for ( j = 0 ; j < MAX_CLUSTER_DEPTH ; j++ ) {
			//
			_i_SCH_CLUSTER_Set_PathRun( side , i , j , -1 , 0 ); // 2006.07.13
			//
			if ( switchmode ) {
				_i_SCH_CLUSTER_Set_Parallel_Used_Restore( side , i , j ); // 2007.02.21
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( side , i , j );
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPost_Restore( side , i , j );
			}
			else {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					_i_SCH_CLUSTER_Set_PathProcessData( side , i , j , k , 0 , "" , "" , "" );
				}
			}
		}
		if ( switchmode ) {
			Temp = _i_SCH_CLUSTER_Get_PathOut( side , i );
			_i_SCH_CLUSTER_Set_PathOut( side , i , _i_SCH_CLUSTER_Get_PathIn( side , i ) );
			_i_SCH_CLUSTER_Set_PathIn( side , i , Temp );
			//
			Temp = _i_SCH_CLUSTER_Get_SlotOut( side , i );
			_i_SCH_CLUSTER_Set_SlotOut( side , i , _i_SCH_CLUSTER_Get_SlotIn( side , i ) );
			_i_SCH_CLUSTER_Set_SlotIn( side , i , Temp );
		}
		_i_SCH_CLUSTER_Set_FailOut( side , i , 0 ); // 2002.07.05
		_i_SCH_CLUSTER_Set_SwitchInOut( side , i , 0 ); // 2005.01.26
		//
		_i_SCH_CLUSTER_Init_AllTime( side , i ); // 2006.03.18
		//
		_i_SCH_CLUSTER_Set_Buffer( side , i , 0 ); // 2006.04.04
		//
		_i_SCH_CLUSTER_Set_Stock( side , i , 0 ); // 2006.04.13
		//
		_i_SCH_CLUSTER_Set_Ex_PrcsID( side , i , 0 ); // 2013.05.06
		//
		if ( !switchmode ) {
			//
			_i_SCH_CLUSTER_Set_Optimize( side , i , -1 ); // 2006.04.04
			_i_SCH_CLUSTER_Set_ClusterIndex( side , i , 0 );
			//
			_i_SCH_CLUSTER_Set_Ex_OrderMode( side , i , 0 ); // 2016.08.25
			//
		}
		//
	}
}



//=======================================================================================
void RECIPE_FILE_LOT_DATA_READ_FOR_MAP_INIT(); // 2016.12.10

void INIT_SCHEDULER_PRM_CLUSTER_DATA( int apmode , int side ) {
	int i , j , l , m;

	if ( apmode == 0 ) {
		//
		InitializeCriticalSection( &CR_CLUSTERDB ); // 2008.07.29
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				_SCH_INF_CLUSTER_DATA_AREA[i][j].LotUserSpecial = NULL;
				_SCH_INF_CLUSTER_DATA_AREA[i][j].ClusterUserSpecial = NULL;
				_SCH_INF_CLUSTER_DATA_AREA[i][j].ClusterTuneData = NULL; // 2007.06.04
				_SCH_INF_CLUSTER_DATA_AREA[i][j].UserArea = NULL; // 2008.01.21
				_SCH_INF_CLUSTER_DATA_AREA[i][j].UserArea2 = NULL; // 2010.02.02
				_SCH_INF_CLUSTER_DATA_AREA[i][j].UserArea3 = NULL; // 2010.02.02
				_SCH_INF_CLUSTER_DATA_AREA[i][j].UserArea4 = NULL; // 2010.02.02
				for ( l = 0 ; l < MAX_CLUSTER_DEPTH ; l++ ) {
					//
					_SCH_INF_CLUSTER_DATA_AREA[i][j].PathProcessParallel[l] = NULL;
					//
					for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
						_SCH_INF_CLUSTER_DATA_AREA[i][j].PathProcessRecipe[l][m][0] = NULL;
						_SCH_INF_CLUSTER_DATA_AREA[i][j].PathProcessRecipe[l][m][1] = NULL;
						_SCH_INF_CLUSTER_DATA_AREA[i][j].PathProcessRecipe[l][m][2] = NULL;
					}
					//
					_SCH_INF_CLUSTER_DATA2_AREA[i][j].PathProcessDepth[l] = NULL; // 2014.01.28
					//
				}
				//
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].EILInfo = NULL;
				// 2011.07.21
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].JobName = NULL;
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].RecipeName = NULL;
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].PPID = NULL;
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].WaferID = NULL;
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].MaterialID = NULL;
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].OutMaterialID = NULL;
				//
				_SCH_INF_CLUSTER_DATA2_AREA[i][j].UserControl_Data = NULL; // 2011.12.08
				//
			}
		}
		//
		RECIPE_FILE_LOT_DATA_READ_FOR_MAP_INIT(); // 2016.12.10
		//
	}
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		SCHEDULER_CONTROL_Inf_Reset_SupplyID();
	}
}
//==========================================================================================================




