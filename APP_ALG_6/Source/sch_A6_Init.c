//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_preprcs.h"
#include "INF_sch_prepost.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
#include "INF_sch_CassetteSupply.h"
//================================================================================
#include "sch_A6_default.h"
#include "sch_A6_sub.h"
#include "sch_A6_subBM.h"
#include "sch_A6_Init.h"
//================================================================================

extern int BM_WAIT_CHECK_6[MAX_CHAMBER]; // 2018.10.02
extern int PM_WAIT_LAST_STS_6[MAX_CHAMBER]; // 2018.10.02
extern int PM_WAIT_FUNCTION_ADDRESS[MAX_CHAMBER]; // 2018.10.02

//================================================================================
BOOL MODULE_LAST_SWITCH_6[MAX_CHAMBER]; // 2013.07.25
//================================================================================
//================================================================================
int  MODULE_MAX_SIZE_OF_TMPM[MAX_TM_CHAMBER_DEPTH]; // 2014.11.14
//================================================================================
int Scheduler_PM_Get_First_for_CYCLON( int tms , BOOL );
int Scheduler_BM_Get_First_for_CYCLON( int tms );
int SCHEDULER_CONTROL_ST6_GET_SLOT( int pmc , int side , int pt );

void SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PICKING( int t , int pmc , int s , int p ); // 2013.11.26

void CYCLON_TM_ACT_LOCK_INIT( int side ); // 2012.09.03


CRITICAL_SECTION CR_CYCLON_GROUP;

int CYCLON_GROUP_BM_GINDEX = 0;

int CYCLON_GROUP_BM_LASTSUPPLY_GROUP = 0;

int CYCLON_GROUP_BM_CONTROL[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP];
int CYCLON_GROUP_BM_INDEX[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP];
int CYCLON_GROUP_BM_NOMORE[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP]; // 2013.04.01
//
int CYCLON_GROUP_BM_COUNT[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP];
int CYCLON_GROUP_BM_S[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP][MAX_CASSETTE_SLOT_SIZE];
int CYCLON_GROUP_BM_P[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP][MAX_CASSETTE_SLOT_SIZE];
int CYCLON_GROUP_BM_D[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP][MAX_CASSETTE_SLOT_SIZE];
int CYCLON_GROUP_BM_PMS[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP][MAX_CASSETTE_SLOT_SIZE];
int CYCLON_GROUP_BM_PMM[MAX_TM_CHAMBER_DEPTH][MAX_SUPPLY_GROUP][MAX_CASSETTE_SLOT_SIZE];

//_SCH_CLUSTER_Set_Stock

int SCH_PM_MOVE_SLOT_INFO[MAX_CHAMBER];
int SCH_PM_MOVE_ACT_INFO[MAX_CHAMBER];

int SCH_TM_RUN_PM_RUN[MAX_TM_CHAMBER_DEPTH];
int SCH_TM_RUN_BM_RUN[MAX_TM_CHAMBER_DEPTH];
int SCH_TM_RUN_BM_STOCK[MAX_TM_CHAMBER_DEPTH];
int SCH_TM_RUN_RES[MAX_TM_CHAMBER_DEPTH];


int SCH_CYCLON2_STATUS_D1 = 0;
int SCH_CYCLON2_STATUS_D2 = 0;
int SCH_CYCLON2_STATUS_D3 = 0;
int SCH_CYCLON2_STATUS_D4 = 0;


//
/*
//
// 2014.06.11
//
int BM_SUPPLY_LOCK_FOR_AL6[MAX_TM_CHAMBER_DEPTH]; // 2013.10.30
//
void SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( int tms , int data ) { // 2013.10.30
	BM_SUPPLY_LOCK_FOR_AL6[tms] = data;
}
int SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( int tms ) { // 2013.10.30
	return BM_SUPPLY_LOCK_FOR_AL6[tms];
}
//
*/

/*
// 2014.07.10
//
// 2014.06.11
//
int BM_SUPPLY_LOCK_FOR_AL6[MAX_TM_CHAMBER_DEPTH]; // 2013.10.30
int BM_SUPPLY_LOCKX_SIDE_FOR_AL6[MAX_TM_CHAMBER_DEPTH]; // 2014.06.11
int BM_SUPPLY_LOCKX_PT_FOR_AL6[MAX_TM_CHAMBER_DEPTH]; // 2014.06.11
//
void SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( int tms , int data ) { // 2013.10.30
	if ( data <= 0 ) {
		BM_SUPPLY_LOCK_FOR_AL6[tms] = data;
		BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] = -1;
		BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] = -1;
	}
	else {
		BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] = -1;
		BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] = -1;
		BM_SUPPLY_LOCK_FOR_AL6[tms] = data;
	}
}
//
void SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( int tms , int data , int side , int pt ) { // 2014.06.11
	//
	int i , s , sels;
	//
	if ( data <= 0 ) {
		BM_SUPPLY_LOCK_FOR_AL6[tms] = data;
		BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] = -1;
		BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] = -1;
	}
	else {
		//
		BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] = side;
		BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] = pt;
		//
		if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
			if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				sels = -1;
				//
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					//==========================================================================================
					if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
					//================================================================================================
					//
					i = _SCH_MODULE_Get_FM_DoPointer( s );
					//
					if ( i >= MAX_CASSETTE_SLOT_SIZE ) continue;
					//
					if ( _SCH_CLUSTER_Get_PathRange( s , i ) <= 0 ) continue;
					if ( _SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_READY ) continue;
					//
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s , i , side , pt ) ) continue;
					//
					if ( sels == -1 ) {
						sels = s;
					}
					else {
						break;
					}
					//
				}
				//
				if ( ( s == MAX_CASSETTE_SIDE ) && ( sels != -1 ) ) {
					for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
						//==========================================================================================
						if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
						//================================================================================================
						if ( s != sels ) _SCH_CASSETTE_Set_Side_Monitor_SupplyDone( s , 1 );
					}
				}
				//
			}
		}
		//
		BM_SUPPLY_LOCK_FOR_AL6[tms] = data;
		//
	}
}
//
int SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( int tms ) { // 2013.10.30
	return BM_SUPPLY_LOCK_FOR_AL6[tms];
}

int SCH_BM_SUPPLY_LOCK_GET2_FOR_AL6( int tms , int side , int pt ) { // 2014.06.11
	//
	if ( BM_SUPPLY_LOCK_FOR_AL6[tms] <= 0 ) return FALSE;
	//
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( ( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] >= 0 ) && ( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] < MAX_CASSETTE_SIDE ) ) {
				if ( ( BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] >= 0 ) && ( BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] < MAX_CASSETTE_SLOT_SIZE ) ) {
					//
					if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[tms] , BM_SUPPLY_LOCKX_PT_FOR_AL6[tms] , side , pt ) ) return FALSE;
					//
				}
			}
		}
	}
	//
	return TRUE;
}
*/


// 2014.07.10
//
int BM_SUPPLY_LOCK_FOR_AL6[MAX_CHAMBER];
int BM_SUPPLY_LOCKX_SIDE_FOR_AL6[MAX_CHAMBER];
int BM_SUPPLY_LOCKX_PT_FOR_AL6[MAX_CHAMBER];
//
int BM_SUPPLY_LOCK_FM_SIDE_FOR_AL6; // 2015.12.15
//
void SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( int bmc ) {
	BM_SUPPLY_LOCKX_SIDE_FOR_AL6[bmc] = -1;
	BM_SUPPLY_LOCKX_PT_FOR_AL6[bmc] = -1;
	BM_SUPPLY_LOCK_FOR_AL6[bmc] = 1;
}

void SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( int bmc ) {
	int i;
	if ( bmc <= 0 ) {
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			BM_SUPPLY_LOCK_FOR_AL6[i] = 0;
			BM_SUPPLY_LOCKX_SIDE_FOR_AL6[i] = -1;
			BM_SUPPLY_LOCKX_PT_FOR_AL6[i] = -1;
		}
		//
		BM_SUPPLY_LOCK_FM_SIDE_FOR_AL6 = -1; // 2015.12.15
		//
	}
	else {
		BM_SUPPLY_LOCK_FOR_AL6[bmc] = 0;
		BM_SUPPLY_LOCKX_SIDE_FOR_AL6[bmc] = -1;
		BM_SUPPLY_LOCKX_PT_FOR_AL6[bmc] = -1;
	}
}
//
void SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( int bmc , int side , int pt ) {
	//
	int i , s , sels;
	//
	if ( bmc == 99 ) { // 2015.12.15
		//
		BM_SUPPLY_LOCK_FM_SIDE_FOR_AL6 = side; // 2015.12.15
		//
		if ( pt > 0 ) { // 2016.02.26
			BM_SUPPLY_LOCK_FOR_AL6[pt] = 0;
			BM_SUPPLY_LOCKX_SIDE_FOR_AL6[pt] = -1;
			BM_SUPPLY_LOCKX_PT_FOR_AL6[pt] = -1;
		}
		//
		return;
	}
	//
	BM_SUPPLY_LOCKX_SIDE_FOR_AL6[bmc] = side;
	BM_SUPPLY_LOCKX_PT_FOR_AL6[bmc] = pt;
	//
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			sels = -1;
			//
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
				//==========================================================================================
				if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
				//================================================================================================
				//
				i = _SCH_MODULE_Get_FM_DoPointer( s );
				//
				if ( i >= MAX_CASSETTE_SLOT_SIZE ) continue;
				//
				if ( _SCH_CLUSTER_Get_PathRange( s , i ) <= 0 ) continue;
				if ( _SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_READY ) continue;
				//
				if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s , i , side , pt ) ) continue;
				//
				if ( sels == -1 ) {
					sels = s;
				}
				else {
					break;
				}
				//
			}
			//
			if ( ( s == MAX_CASSETTE_SIDE ) && ( sels != -1 ) ) {
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					//==========================================================================================
					if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
					//================================================================================================
					if ( s != sels ) _SCH_CASSETTE_Set_Side_Monitor_SupplyDone( s , 1 );
				}
			}
			//
		}
	}
	//
	BM_SUPPLY_LOCK_FOR_AL6[bmc] = 1;
	//
}
//
int SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( int bmc ) { // 2013.10.30
	if ( bmc == 99 ) { // 2015.12.15
		return BM_SUPPLY_LOCK_FM_SIDE_FOR_AL6;
	}
	//
	return BM_SUPPLY_LOCK_FOR_AL6[bmc];
}

int SCH_BM_SUPPLY_LOCK_GET2_FOR_AL6( int tms , int side , int pt ) { // 2014.06.11
	//
	int i;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( BM_SUPPLY_LOCK_FOR_AL6[i] == 0 ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					if ( !SCHEDULING_ThisIs_BM_OtherChamber6( i , 0 ) ) {
						return FALSE;
					}
				}
			}
		}
		//
	}
	//
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				//
				if ( BM_SUPPLY_LOCK_FOR_AL6[i] != 0 ) {
					//
					if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
						//
						if ( ( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[i] >= 0 ) && ( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[i] < MAX_CASSETTE_SIDE ) ) {
							if ( ( BM_SUPPLY_LOCKX_PT_FOR_AL6[i] >= 0 ) && ( BM_SUPPLY_LOCKX_PT_FOR_AL6[i] < MAX_CASSETTE_SLOT_SIZE ) ) {
								//
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( BM_SUPPLY_LOCKX_SIDE_FOR_AL6[i] , BM_SUPPLY_LOCKX_PT_FOR_AL6[i] , side , pt ) ) return FALSE;
								//
							}
						}
						//
					}
				}
				//
			}
			//
		}
	}
	//
	return TRUE;
}



extern BOOL CYCLON_TM_ACT_LOCK[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH]; // 2018.10.02


void SCH_CYCLON2_STATUS_VIEW_SUB( char *filename , BOOL append , int data , int t , int d1 , int d2 ) {
	int i , j , k;
	//
	FILE *fpt;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return; // 2014.11.14
	//
	if ( append ) {
		fpt = fopen( filename , "a" );
	}
	else {
		fpt = fopen( "scheduler\\status.log" , "w" );
	}
	//
	if ( data != -1 ) {
		SCH_CYCLON2_STATUS_D1 = data;
		SCH_CYCLON2_STATUS_D2 = t;
		SCH_CYCLON2_STATUS_D3 = d1;
		SCH_CYCLON2_STATUS_D4 = d2;
	}
	//
	if ( fpt == NULL ) return;

	//
	fprintf( fpt , "H	-	[%d]	T=%d	D=%d,%d	G=%d	L=%d\n" , SCH_CYCLON2_STATUS_D1 , SCH_CYCLON2_STATUS_D2 , SCH_CYCLON2_STATUS_D3 , SCH_CYCLON2_STATUS_D4 , CYCLON_GROUP_BM_GINDEX , CYCLON_GROUP_BM_LASTSUPPLY_GROUP );

	//T	No	Sts	Index	Count	data
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//=========================================================================================================================
		fprintf( fpt , "			\n" );
		//=========================================================================================================================
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( CYCLON_GROUP_BM_CONTROL[i][j] == CYCLON_GROUP_BM_IDLE ) {
				if ( CYCLON_GROUP_BM_INDEX[i][j] == -1 ) {
					continue;
				}
			}
			//
			fprintf( fpt , "%d	" , i + 1 );
			//
			switch( CYCLON_GROUP_BM_CONTROL[i][j] ) {
			case CYCLON_GROUP_BM_IDLE :
				fprintf( fpt , "%d	IDLE" , j + 1 );
				break;
			case CYCLON_GROUP_BM_SUPPLYING :
				fprintf( fpt , "%d	SUPPLY" , j + 1 );
				if ( CYCLON_GROUP_BM_NOMORE[i][j] ) fprintf( fpt , "[NM]" );
				break;
			case CYCLON_GROUP_BM_SUPPLYING_DUMMY :
				fprintf( fpt , "%d	DUMMY" , j + 1 );
				break;
			case CYCLON_GROUP_BM_SUPPLY_FINISH :
				fprintf( fpt , "%d	FINISH" , j + 1 );
				break;
			case CYCLON_GROUP_BM_SUPPLY_FINISH_BM :
				fprintf( fpt , "%d	FI(ST)" , j + 1 );
				break;
			case CYCLON_GROUP_BM_PROCESSING :
				fprintf( fpt , "%d	PRCS  " , j + 1 );
				break;
			default :
				fprintf( fpt , "%d	??" , j + 1 );
				break;
			}
			//
			fprintf( fpt , "	%d	%d" , CYCLON_GROUP_BM_INDEX[i][j] , CYCLON_GROUP_BM_COUNT[i][j] );
			//
			for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
				//
				if ( CYCLON_GROUP_BM_D[i][j][k] ) {
					fprintf( fpt , "	D%d,%d,%d,%d" , CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] , CYCLON_GROUP_BM_PMS[i][j][k] , CYCLON_GROUP_BM_PMM[i][j][k] );
				}
				else {
					fprintf( fpt , "	%d,%d,%d,%d" , CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] , CYCLON_GROUP_BM_PMS[i][j][k] , CYCLON_GROUP_BM_PMM[i][j][k] );
				}
				//
			}
			fprintf( fpt , "\n" );
			//=========================================================================================================================
		}
		//=========================================================================================================================
	}
	//=========================================================================================================================

	fprintf( fpt , "			\n" );

	for ( i = 0 ; i < 2 ; i++ ) {
		//
		fprintf( fpt , "PM%d		%d	" , i + 1 , SCH_PM_MOVE_SLOT_INFO[PM1+i] );
		//
		switch( SCH_PM_MOVE_ACT_INFO[PM1+i] ) {
		case SYS_SUCCESS :	fprintf( fpt , "S" );	break;
		case SYS_ABORTED :	fprintf( fpt , "A" );	break;
		case SYS_ERROR :	fprintf( fpt , "E" );	break;
		case SYS_RUNNING :	fprintf( fpt , "R" );	break;
		default			 :	fprintf( fpt , "?%d" , SCH_PM_MOVE_ACT_INFO[PM1+i] );	break;
		}
		fprintf( fpt , "\n" );
		//
	}
	//=========================================================================================================================

	fprintf( fpt , "			\n" );

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( i + TM ) ) continue;
		//
		fprintf( fpt , "TM%d		" , i + 1 );
		//
		if ( SCH_TM_RUN_PM_RUN[i] == -1 ) {
			fprintf( fpt , "-	" );
		}
		else {
			fprintf( fpt , "%s	" , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(SCH_TM_RUN_PM_RUN[i]) );
		}
		//
		if ( SCH_TM_RUN_BM_RUN[i] == -1 ) {
			fprintf( fpt , "-	" );
		}
		else {
			fprintf( fpt , "%s	" , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(SCH_TM_RUN_BM_RUN[i]) );
		}
		//
		if ( SCH_TM_RUN_BM_STOCK[i] == -1 ) {
			fprintf( fpt , "-	%d\n" , SCH_TM_RUN_RES[i] );
		}
		else {
			fprintf( fpt , "%s	%d\n" , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(SCH_TM_RUN_BM_STOCK[i]) , SCH_TM_RUN_RES[i] );
		}
		//
	}
	//
	// 2018.10.02
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			if ( CYCLON_TM_ACT_LOCK[i][j] ) {
				fprintf( fpt , "CYCLON_TM_ACT_LOCK	S=%d,T=%d\n" , i , j );
			}
		}
	}
	//
	//
	fclose( fpt );
}


void SCH_CYCLON2_STATUS_VIEW( int data , int t , int d1 , int d2 ) {
	char filename[256];
	//
	strcpy( filename , "scheduler\\status.log" );

	SCH_CYCLON2_STATUS_VIEW_SUB( filename , FALSE , data , t , d1 , d2 );
}


void INIT_SCHEDULER_AL6_PART_DATA( int apmode , int side ) {
	int i , j , k;
	//
	if ( apmode != 0 ) {
		CYCLON_TM_ACT_LOCK_INIT( side ); // 2012.09.03
	}
	//
	if ( apmode == 0 ) { // 2013.01.19
		InitializeCriticalSection( &CR_CYCLON_GROUP );
	}
	//
	if ( apmode == 3 ) { // 2013.01.19
		//
		// 2018.10.02
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			BM_WAIT_CHECK_6[i] = 0;
			PM_WAIT_LAST_STS_6[i] = 0;
			PM_WAIT_FUNCTION_ADDRESS[i] = -2;
		}
		//
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			//
			SCH_PM_MOVE_SLOT_INFO[i] = -1;
			SCH_PM_MOVE_ACT_INFO[i] = SYS_SUCCESS;
		}
		//
		CYCLON_GROUP_BM_GINDEX = 0;
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			BM_SUPPLY_LOCK_FOR_AL6[i] = FALSE; // 2013.10.30
			//
			for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
				//
				CYCLON_GROUP_BM_CONTROL[i][j] = CYCLON_GROUP_BM_IDLE;
				//
				CYCLON_GROUP_BM_INDEX[i][j] = -1;
				//
				CYCLON_GROUP_BM_COUNT[i][j] = 0;
				//
				for ( k = 0 ; k < MAX_CASSETTE_SLOT_SIZE ; k++ ) {
					CYCLON_GROUP_BM_S[i][j][k] = 0;
					CYCLON_GROUP_BM_P[i][j][k] = 0;
					CYCLON_GROUP_BM_D[i][j][k] = 0;
					CYCLON_GROUP_BM_PMS[i][j][k] = 0;
					CYCLON_GROUP_BM_PMM[i][j][k] = 0;
				}
			}
			//
			SCH_TM_RUN_PM_RUN[i] = -1;
			SCH_TM_RUN_BM_RUN[i] = -1;
			SCH_TM_RUN_BM_STOCK[i] = -1;
			SCH_TM_RUN_RES[i] = -1;
			//
		}
		//
		SCH_CYCLON2_STATUS_VIEW( 0 , 0 , 0 , 0 );
		//
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) { // 2013.07.25
		//
		//=============================================================================================
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2014.11.14
			MODULE_MAX_SIZE_OF_TMPM[i] = 0;
		}
		//
		for ( i = PM1 ; i < AL ; i++ ) {
			//
			if ( _SCH_PRM_GET_MODULE_MODE( i ) ) continue;
			//
			k = _SCH_PRM_GET_MODULE_GROUP( i );
			//
			if ( ( k < 0 ) || ( k >= MAX_TM_CHAMBER_DEPTH ) ) continue;
			//
			j = _SCH_PRM_GET_MODULE_SIZE( i );
			//
			if ( j <= 1 ) j = MAX_CYCLON_PM_DEPTH;
			//
			if ( j > MODULE_MAX_SIZE_OF_TMPM[ k ] ) MODULE_MAX_SIZE_OF_TMPM[ k ] = j;
			//
		}
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2014.11.14
			if ( MODULE_MAX_SIZE_OF_TMPM[i] == 0 ) MODULE_MAX_SIZE_OF_TMPM[i] = MAX_CYCLON_PM_DEPTH;
		}
		//=============================================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			MODULE_LAST_SWITCH_6[i] = FALSE;
		}
		//
		SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( -1 ); // 2014.07.10
		//
	}
	//
}

//==============================================================================================================================================
//==============================================================================================================================================
//==============================================================================================================================================
//==============================================================================================================================================

int SCH_CYCLON2_Get_PM_SLOT_INFO( int pmc ) {
	return SCH_PM_MOVE_SLOT_INFO[pmc];
}

int SCH_CYCLON2_Get_PM_RESULT_INFO( int pmc ) {
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return SYS_RUNNING;
	return SCH_PM_MOVE_ACT_INFO[pmc];
}

int SCH_CYCLON2_Set_PM_MOVE_ACTION( int pmc , int data ) {
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) {
		//
		if ( SCH_PM_MOVE_ACT_INFO[pmc] == SYS_RUNNING ) return -1;
		if ( SCH_PM_MOVE_SLOT_INFO[pmc] == data ) return 1;
		//
	}
	//
	SCH_PM_MOVE_SLOT_INFO[pmc] = data;
	SCH_PM_MOVE_ACT_INFO[pmc] = SYS_RUNNING;
//
SCH_CYCLON2_STATUS_VIEW( -1 , 0 , 0 , 0 );
//
	return 0;
}

void SCH_CYCLON2_Set_PM_MOVE_RESULT( int pmc , int data ) {
	SCH_PM_MOVE_ACT_INFO[pmc] = data;
//
SCH_CYCLON2_STATUS_VIEW( -1 , 0 , 0 , 0 );
//
}

//==============================================================================================================================================
//==============================================================================================================================================
//==============================================================================================================================================

void SCH_CYCLON2_Set_MDL_SELECT( int tms , int pmc , int bms , int stockbm , int result ) {
	BOOL Res = FALSE;
	//
	if ( pmc != SCH_TM_RUN_PM_RUN[tms] ) Res = TRUE;
	if ( bms != SCH_TM_RUN_BM_RUN[tms] ) Res = TRUE;
	if ( stockbm != SCH_TM_RUN_BM_STOCK[tms] ) Res = TRUE;
	if ( result != SCH_TM_RUN_RES[tms] ) Res = TRUE;
	//
	if ( Res ) {
		SCH_TM_RUN_PM_RUN[tms] = pmc;
		SCH_TM_RUN_BM_RUN[tms] = bms;
		SCH_TM_RUN_BM_STOCK[tms] = stockbm;
		SCH_TM_RUN_RES[tms] = result;
//
SCH_CYCLON2_STATUS_VIEW( -1 , 0 , 0 , 0 );
//
	}
}

//==============================================================================================================================================
//==============================================================================================================================================
//==============================================================================================================================================


int SCH_CYCLON2_Get_GROUP_INDEX() {
	return CYCLON_GROUP_BM_GINDEX;
}

int SCH_CYCLON2_GROUP_GET_LAST_SUPPLY() {
	//
	return CYCLON_GROUP_BM_LASTSUPPLY_GROUP;
	//
}

BOOL SCH_CYCLON2_GROUP_NOT_BM_NOMORE_SUPPLY( int t ) {
	int i , j;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//==========================================================================================
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//================================================================================================
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			if ( _SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_RETURN_REQUEST ) continue;
			//
			if ( ( _SCH_CLUSTER_Get_Stock( i , j ) % 100 ) == t ) return FALSE;
			//
		}
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( i == t ) {
				//
				switch( CYCLON_GROUP_BM_CONTROL[i][j] ) {
				case CYCLON_GROUP_BM_SUPPLYING :
					if ( CYCLON_GROUP_BM_NOMORE[i][j] ) return FALSE;
					break;
				}
				//
			}
		}
	}
	//
	return TRUE;
}


int SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE_SUB( int t , int s , int p , BOOL all , int *jix ) {
	int i , j;
	int pmc , pms;
	int finishcnt , idlejindex , suppjindex;
	int runcount; // 2015.03.06
	//
	//	1								2												x		jix
	//
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_IDLE				OK1,2		0		1,2(?)
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_SUPPLYING			CHECK2		2		2
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_SUPPLYING_DUMMY		X			-1
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_SUPPLY_FINISH		OK1			0		1

	//	CYCLON_GROUP_BM_SUPPLYING		CYCLON_GROUP_BM_IDLE				CHECK1		1		1
	//	CYCLON_GROUP_BM_SUPPLYING		CYCLON_GROUP_BM_SUPPLYING			X			-1
	//	CYCLON_GROUP_BM_SUPPLYING		CYCLON_GROUP_BM_SUPPLYING_DUMMY		x			-1
	//	CYCLON_GROUP_BM_SUPPLYING		CYCLON_GROUP_BM_SUPPLY_FINISH		CHECK1		1		1

	//	CYCLON_GROUP_BM_SUPPLYING_DUMMY	CYCLON_GROUP_BM_IDLE				X			-1
	//	CYCLON_GROUP_BM_SUPPLYING_DUMMY	CYCLON_GROUP_BM_SUPPLYING			X			-1
	//	CYCLON_GROUP_BM_SUPPLYING_DUMMY	CYCLON_GROUP_BM_SUPPLYING_DUMMY		x			-1
	//	CYCLON_GROUP_BM_SUPPLYING_DUMMY	CYCLON_GROUP_BM_SUPPLY_FINISH		X			-1

	//	CYCLON_GROUP_BM_SUPPLY_FINISH	CYCLON_GROUP_BM_IDLE				OK2			0		2
	//	CYCLON_GROUP_BM_SUPPLY_FINISH	CYCLON_GROUP_BM_SUPPLYING			CHECK2		2		2
	//	CYCLON_GROUP_BM_SUPPLY_FINISH	CYCLON_GROUP_BM_SUPPLYING_DUMMY		X			-1
	//	CYCLON_GROUP_BM_SUPPLY_FINISH	CYCLON_GROUP_BM_SUPPLY_FINISH		X			-1
	//
	//	1								2												x		spp
	//
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_IDLE				OK1,2		0		1,2(?)
	//	CYCLON_GROUP_BM_IDLE			CYCLON_GROUP_BM_SUPPLY_FINISH		OK1			0		1

	//	CYCLON_GROUP_BM_SUPPLY_FINISH	CYCLON_GROUP_BM_IDLE				OK2			0		2
	//
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//==========================================================================================
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//================================================================================================
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			if ( _SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_RETURN_REQUEST ) continue;
			//
			if ( ( _SCH_CLUSTER_Get_Stock( i , j ) % 100 ) == t ) return -1;
			//
		}
	}
	//
	if ( !all ) { // 2013.04.01
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
				//
				if ( i == t ) {
					//
					switch( CYCLON_GROUP_BM_CONTROL[i][j] ) {
					case CYCLON_GROUP_BM_SUPPLYING :
						if ( CYCLON_GROUP_BM_NOMORE[i][j] ) return -2;
						break;
					}
					//
				}
			}
		}
		//
		return 1;
	}
	//
	*jix = -1;
	//
	finishcnt = 0;
	idlejindex = -1;
	suppjindex = -1;
	runcount = 0; // 2015.03.06
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( i == t ) {
				//
				switch( CYCLON_GROUP_BM_CONTROL[i][j] ) {
				case CYCLON_GROUP_BM_IDLE :
					if ( idlejindex == -1 ) {
						idlejindex = j;
					}
					else {
						if ( CYCLON_GROUP_BM_INDEX[t][idlejindex] > CYCLON_GROUP_BM_INDEX[t][j] ) {
							idlejindex = j;
						}
					}
					break;

				case CYCLON_GROUP_BM_SUPPLYING :
					//
					if ( CYCLON_GROUP_BM_COUNT[i][j] >= SIZE_OF_CYCLON_PM_DEPTH(i+TM) ) return -3;
					if ( CYCLON_GROUP_BM_NOMORE[i][j] ) return -4; // 2013.04.01

					//
					if ( suppjindex == -1 ) {
						suppjindex = j;
					}
					else {
						return -5;
					}
					//
					runcount++;
					//
					break;

				case CYCLON_GROUP_BM_SUPPLYING_DUMMY :
					return -6;
					break;

				case CYCLON_GROUP_BM_SUPPLY_FINISH :
					finishcnt++;
					//
					runcount++;
					//
					break;

				case CYCLON_GROUP_BM_SUPPLY_FINISH_BM :
					finishcnt++;
					//
					runcount++;
					//
					break;

				}
				//
			}
			else {
				if ( CYCLON_GROUP_BM_CONTROL[i][j] == CYCLON_GROUP_BM_SUPPLYING ) return -7;
				if ( CYCLON_GROUP_BM_CONTROL[i][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) return -8;
			}
			//
		}
	}
	//
	if ( finishcnt > 0 ) { // 2013.01.28
		//
		if ( finishcnt >= ( ( Scheduler_BM_Get_First_for_CYCLON( t ) == -1 ) ? 2 : 3 ) ) return -9;
		//
	}
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	pmc = Scheduler_PM_Get_First_for_CYCLON( t , FALSE );
	//
	if ( pmc <= 0 ) return -10; // 2013.02.15
	//
	pms = ( pmc <= 0 ) ? 0 : SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s , p );
	//
	if ( suppjindex != -1 ) {
		//
		if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( CYCLON_GROUP_BM_S[t][suppjindex][0] , CYCLON_GROUP_BM_P[t][suppjindex][0] , s , p ) ) {
			*jix = t;
			return 0;
		}
		//
		if ( pms >= 0 ) {
			for ( i = 0 ; i < CYCLON_GROUP_BM_COUNT[t][suppjindex] ; i++ ) {
				if ( CYCLON_GROUP_BM_PMS[t][suppjindex][i] == 0 ) continue;
				if ( CYCLON_GROUP_BM_PMS[t][suppjindex][i] == pms ) {
					*jix = t;
					return 0;
				}
			}
		}
		//
		*jix = suppjindex;
		//
	}
	else if ( idlejindex != -1 ) {
		//
		*jix = idlejindex;
		//
		// 2015.03.06
		//
		j = 0;
		//
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) == t ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					if ( !SCHEDULING_ThisIs_BM_OtherChamber6( i , 0 ) ) {
						j++;
					}
				}
			}
			//
		}
		//
		if ( runcount >= (j+1) ) return -12;
		//
	}
	else {
		return -11;
	}
	//
	return 2;
}








BOOL SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( int t , int s , int p , BOOL all , int *hasinvalid ) {
	int i;
	BOOL Res;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	Res = SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE_SUB( t , s , p , all , &i );
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	if ( Res == 0 ) *hasinvalid = i;
	if ( Res > 0 ) return TRUE;
	return FALSE;
}

BOOL SCH_CYCLON2_GROUP_SET_SUPPLY( int t , int s , int p , int *gidx , BOOL nomore ) {
	int j , pmc;
	BOOL Res;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
		//
		*gidx = 0;
		//
		return TRUE;
	}
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	Res = SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE_SUB( t , s , p , TRUE , &j );
	//
	if ( Res <= 0 ) {
//
SCH_CYCLON2_STATUS_VIEW( 1 , t , s , p );
//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return FALSE;
		//
	}
	//
	if ( ( j >= 0 ) && ( j < MAX_SUPPLY_GROUP ) ) {
		//
		switch( CYCLON_GROUP_BM_CONTROL[t][j] ) {
		case CYCLON_GROUP_BM_IDLE :
			//
			CYCLON_GROUP_BM_LASTSUPPLY_GROUP = t;
			//
			CYCLON_GROUP_BM_GINDEX++;
			//
			CYCLON_GROUP_BM_INDEX[t][j] = CYCLON_GROUP_BM_GINDEX;
			//
			CYCLON_GROUP_BM_S[t][j][0] = s;
			CYCLON_GROUP_BM_P[t][j][0] = p;
			CYCLON_GROUP_BM_D[t][j][0] = 0;
			//
			pmc = Scheduler_PM_Get_First_for_CYCLON( t , TRUE );
			//
			if ( !nomore ) {
				SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PICKING( t , pmc , s , p ); // 2013.11.26
			}
			//
			CYCLON_GROUP_BM_PMS[t][j][0] = ( pmc <= 0 ) ? 0 : SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s , p );
			//
			CYCLON_GROUP_BM_COUNT[t][j] = 1;
			//
			CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLYING;
			CYCLON_GROUP_BM_NOMORE[t][j] = nomore; // 2013.04.01
			//
			*gidx = ( CYCLON_GROUP_BM_INDEX[t][j] * 100 ) + t;
			//
			break;
		case CYCLON_GROUP_BM_SUPPLYING :
			//
			CYCLON_GROUP_BM_LASTSUPPLY_GROUP = t;
			//
			CYCLON_GROUP_BM_S[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = s;
			CYCLON_GROUP_BM_P[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = p;
			CYCLON_GROUP_BM_D[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = 0;
			//
			pmc = Scheduler_PM_Get_First_for_CYCLON( t , TRUE );
			//
			CYCLON_GROUP_BM_PMS[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = ( pmc <= 0 ) ? 0 : SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s , p );
			//
			CYCLON_GROUP_BM_COUNT[t][j]++;
			//
			CYCLON_GROUP_BM_NOMORE[t][j] = nomore; // 2013.04.01
			//
			*gidx = ( CYCLON_GROUP_BM_INDEX[t][j] * 100 ) + t;
			//
			break;
			//
		default :
//
SCH_CYCLON2_STATUS_VIEW( 2 , t , s , p );
//
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			return FALSE;
		}
		//
	}
	else {
//
SCH_CYCLON2_STATUS_VIEW( 3 , t , s , p );
//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		return FALSE;
	}
//
SCH_CYCLON2_STATUS_VIEW( 4 , t , s , p );
//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return TRUE;
}


BOOL SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( int bm , int *maxdumcnt , int *s , int *p ) {
	int t , j , k , hasp;
	//
	// 2015.01.20
	//
	*s = -1;
	*p = -1;
	//
	*maxdumcnt = 0;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bm , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
		//
	}
	else {
		//
		EnterCriticalSection( &CR_CYCLON_GROUP );
		//
		t = _SCH_PRM_GET_MODULE_GROUP( bm );
		//
		j = Scheduler_PM_Get_First_for_CYCLON( t , FALSE ); // 2013.02.15
		//
		if ( j <= 0 ) {
			//
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			//
			return FALSE; // 2013.02.15
		}
		//
		/*
		// 2013.04.10
		if ( !SCH_CYCLON2_GROUP_NOT_BM_NOMORE_SUPPLY( t ) ) {
			//
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			//
			return FALSE; // 2013.02.15
		}
		*/
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) ) {
				//==============================================
				//
				// 2013.04.11
				//
				hasp = FALSE;
				//
				for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[t][j] ; k++ ) {
					//
					if ( !CYCLON_GROUP_BM_D[t][j][k] ) hasp = TRUE;
					//
					if ( _SCH_CLUSTER_Get_PathDo( CYCLON_GROUP_BM_S[t][j][k] , CYCLON_GROUP_BM_P[t][j][k] ) == PATHDO_WAFER_RETURN ) {
						break;
					}
				}
				//
				if ( !hasp || ( k != CYCLON_GROUP_BM_COUNT[t][j] ) ) {
					//
					CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLY_FINISH;
					//
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					//
					return FALSE;
				}
				//==============================================
				if ( CYCLON_GROUP_BM_COUNT[t][j] >= SIZE_OF_CYCLON_PM_DEPTH(t+TM) ) {
					//
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					//
					return FALSE;
				}
				//
				*s = CYCLON_GROUP_BM_S[t][j][0];
				*p = CYCLON_GROUP_BM_P[t][j][0];
				//
				*maxdumcnt = SIZE_OF_CYCLON_PM_DEPTH(t+TM) - CYCLON_GROUP_BM_COUNT[t][j];
				//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return TRUE;
				//
			}
			//
		}
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
	}
	//
	return FALSE;
}


int SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( int t , int *Dummy_count , int s , int p ) {
	int j , c;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return 3;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) ) {
			//
			if ( s >= 0 ) {
				//
				CYCLON_GROUP_BM_S[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = s;
				CYCLON_GROUP_BM_P[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = p;
				CYCLON_GROUP_BM_D[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = 1;
				//
				CYCLON_GROUP_BM_PMS[t][j][CYCLON_GROUP_BM_COUNT[t][j]] = 0;
				//
				CYCLON_GROUP_BM_COUNT[t][j]++;
				//
				c = 1;
				//
				*Dummy_count = ( CYCLON_GROUP_BM_INDEX[t][j] * 100 ) + t;
				//
			}
			else {
				c = *Dummy_count;
			}
			//
			if ( CYCLON_GROUP_BM_COUNT[t][j] >= SIZE_OF_CYCLON_PM_DEPTH(t+TM) ) {
				//
				CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLY_FINISH;
//
SCH_CYCLON2_STATUS_VIEW( 11 , t , c , s );
//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return 1;
			}
			else {
				//
				CYCLON_GROUP_BM_CONTROL[t][j] = ( c > 0 ) ? CYCLON_GROUP_BM_SUPPLYING_DUMMY : CYCLON_GROUP_BM_SUPPLY_FINISH;
//
SCH_CYCLON2_STATUS_VIEW( 12 , t , c , s );
//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return 2;
			}
			//
		}
		//
	}
//
SCH_CYCLON2_STATUS_VIEW( 13 , t , *Dummy_count , s );
//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return 0;
	//
}

int SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( int t , int side , BOOL nomore ) {
	int j , ch , slot , uc;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return 3;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	if ( Scheduler_PM_Get_First_for_CYCLON( t , FALSE ) <= 0 ) { // 2013.02.15
//
SCH_CYCLON2_STATUS_VIEW( 24 , t , side , nomore );
//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return 3;
	}
	//
	for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(t+TM) ; j++ ) {
		//
		if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) ) {
			//
			if ( CYCLON_GROUP_BM_COUNT[t][j] >= SIZE_OF_CYCLON_PM_DEPTH(t+TM) ) {
				//
				CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLY_FINISH;
//
SCH_CYCLON2_STATUS_VIEW( 21 , t , side , nomore );
//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return 1;
			}
			else {
				if ( nomore ) {
					//
					if ( !SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_6( side , &ch , &slot , &uc , -1 ) ) {
						CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLY_FINISH;
					}
					else {
						CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLYING_DUMMY;
					}
//
SCH_CYCLON2_STATUS_VIEW( 22 , t , side , nomore );
//
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					//
					return 1;
				}
			}
		}
	}
	//
//
SCH_CYCLON2_STATUS_VIEW( 25 , t , side , nomore );
//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return 0;
	//
}


int SCH_CYCLON2_GROUP_STOCK_POSSIBLE( int t , int pmc , int bmc , BOOL nomore ) {
	int j , gd , s , p;
	int bic , boc , bgin , bgout , bmmixed;
	int pic , poc , pg , pmmixed;
	int rbsc , phasb;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return -1;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	//=================================================================================================================================================
	//
	bic = 0;
	boc = 0;
	bgin = -1;
	bgout = -1;
	bmmixed = FALSE;
	//
	for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; j++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , j ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( bmc , j );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , j );
			//
			gd = _SCH_CLUSTER_Get_Stock( s , p );
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , j ) == BUFFER_INWAIT_STATUS ) {
				bic++;
				//
				if ( bgin == -1 ) {
					bgin = gd;
				}
				else {
					if ( bgin != gd ) {
						bmmixed = TRUE;
					}
				}
			}
			else {
				boc++;
				//
				if ( bgout == -1 ) {
					bgout = gd;
				}
				else {
					if ( bgout != gd ) {
//						bmmixed = TRUE;
					}
				}
			}
			//
		}
	}
	//
	if ( ( !bmmixed ) && ( bic > 0 ) && ( bgin != -1 ) ) {
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( CYCLON_GROUP_BM_INDEX[t][j] == ( bgin / 100 ) ) {
				//
				if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH ) {
					//
					if ( ( boc == 0 ) && ( CYCLON_GROUP_BM_COUNT[t][j] == bic ) ) {
						//
						CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_SUPPLY_FINISH_BM;
						//
						LeaveCriticalSection( &CR_CYCLON_GROUP );
						return 1;
					}
					//
				}
				else if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH_BM ) {
					//
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					//
					return 2;
				}
			}
		}
		//
	}
	//
	//=================================================================================================================================================
	//
	rbsc = 0;
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( t + BM1 , FALSE , -1 ) && _SCH_MODULE_GET_USE_ENABLE( t + BM1 + 2 , FALSE , -1 ) ) { // 2013.11.22
		if ( ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 ) == BUFFER_TM_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 + 2 ) == BUFFER_TM_STATION ) ) {
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 , BUFFER_INWAIT_STATUS ) + rbsc;
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 + 2 , BUFFER_INWAIT_STATUS ) + rbsc;
		}
		else if ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 ) == BUFFER_TM_STATION ) {
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 , BUFFER_INWAIT_STATUS ) + rbsc;
		}
		else if ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 + 2 ) == BUFFER_TM_STATION ) {
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 + 2 , BUFFER_INWAIT_STATUS ) + rbsc;
		}
	}
	else if ( _SCH_MODULE_GET_USE_ENABLE( t + BM1 , FALSE , -1 ) ) { // 2013.11.22
		if ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 ) == BUFFER_TM_STATION ) {
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 , BUFFER_INWAIT_STATUS ) + rbsc;
		}
	}
	else if ( _SCH_MODULE_GET_USE_ENABLE( t + BM1 + 2 , FALSE , -1 ) ) { // 2013.11.22
		if ( _SCH_MODULE_Get_BM_FULL_MODE( t + BM1 + 2 ) == BUFFER_TM_STATION ) {
			rbsc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( t + BM1 + 2 , BUFFER_INWAIT_STATUS ) + rbsc;
		}
	}
	//
	//=================================================================================================================================================
	phasb = FALSE;
	//
	pg = -1;
	pic = 0;
	poc = 0;
	pmmixed = FALSE;
	//
	for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 100 ) { // temp
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) == 0 ) { // np
					pic++;
				}
				else { // p
					poc++;
				}
				//
				s = _SCH_MODULE_Get_PM_SIDE( pmc , j );
				p = _SCH_MODULE_Get_PM_POINTER( pmc , j );
				//
				gd = _SCH_CLUSTER_Get_Stock( s , p );
				//
				if ( gd == bgin ) phasb = TRUE;
				if ( gd == bgout ) phasb = TRUE;

				if ( pg == -1 ) {
					pg = gd;
				}
				else {
					if ( pg != gd ) {
						pmmixed = TRUE;
					}
				}
				//
			}
		}
	}
	//
	//=================================================================================================================================================
	//
	if ( bic > 0 ) {
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return -11;
		//
	}
	//
	if ( ( rbsc > 0 ) || ( !nomore ) ) {
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return -12;
		//
	}
	//
	if ( ( (bic+boc) <= 0 ) && ( poc > 0 ) ) {
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return 11;
		//
	}
	//
	if ( bmmixed ) {
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return -13;
		//
	}
	//
	if ( phasb ) {
		//
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		//
		return 12;
		//
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return -99;
	//
}
//



int SCH_CYCLON2_GROUP_PROCESS_POSSIBLE( int t , int pmc ) {
	int j , c , g = -1 , gd , s , p;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	c = 0;
	//
	for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 100 ) { // temp
				if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) == 0 ) {
					//
					s = _SCH_MODULE_Get_PM_SIDE( pmc , j );
					p = _SCH_MODULE_Get_PM_POINTER( pmc , j );
					//
					gd = _SCH_CLUSTER_Get_Stock( s , p );
					//
					c++;
					//
					if ( g == -1 ) {
						g = gd;
					}
					else {
						if ( g != gd ) {
							LeaveCriticalSection( &CR_CYCLON_GROUP );
							return FALSE;
						}
					}
					//
				}
			}
		}
	}
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_INDEX[t][j] == ( g / 100 ) ) {
			if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) ) {
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				return FALSE;
			}
			if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH_BM ) ) {
				if ( CYCLON_GROUP_BM_COUNT[t][j] == c ) {
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					return TRUE;
				}
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				return FALSE;
			}
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}


BOOL SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( int s0 , int p0 , int pmc ) {
	int j , c , s , p;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	c = _SCH_CLUSTER_Get_Stock( s0 , p0 ) / 100;
	//
	for ( j = 0 ; j < SIZE_OF_CYCLON_PM_DEPTH(pmc) ; j++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) > 0 ) {
			if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 100 ) { // temp
				//
				s = _SCH_MODULE_Get_PM_SIDE( pmc , j );
				p = _SCH_MODULE_Get_PM_POINTER( pmc , j );
				//
				if ( ( _SCH_CLUSTER_Get_Stock( s , p ) / 100 ) == c ) {
					LeaveCriticalSection( &CR_CYCLON_GROUP );
					return TRUE;
				}
				//
			}
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}

BOOL SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_BM( int s0 , int p0 , int bmc ) {
	int j , c , s , p;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	c = _SCH_CLUSTER_Get_Stock( s0 , p0 ) / 100;
	//
	for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( bmc ) ; j++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , j ) > 0 ) {
			//
			s = _SCH_MODULE_Get_BM_SIDE( bmc , j );
			p = _SCH_MODULE_Get_BM_POINTER( bmc , j );
			//
			if ( ( _SCH_CLUSTER_Get_Stock( s , p ) / 100 ) == c ) {
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				return TRUE;
			}
			//
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}

BOOL SCH_CYCLON2_GROUP_SET_PROCESS( int t , int pmc , int g ) {
	int j;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	SCH_PM_MOVE_SLOT_INFO[pmc] = -1;
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH ) || ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLY_FINISH_BM ) ) {
			//
			if ( CYCLON_GROUP_BM_INDEX[t][j] == ( g / 100 ) ) {
				//
//				CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_IDLE; // 2013.05.28
				CYCLON_GROUP_BM_CONTROL[t][j] = CYCLON_GROUP_BM_PROCESSING; // 2013.05.28
				//
//
SCH_CYCLON2_STATUS_VIEW( 31 , t , pmc , 0 );
//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return TRUE;
				//
			}
			//
		}
		//
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return FALSE;
	//
}


BOOL SCH_CYCLON2_GROUP_SET_SUPPLY_TM( int t , int s , int p ) {
	int j, k;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] != CYCLON_GROUP_BM_IDLE ) {
			//
			for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[t][j] ; k++ ) {
				//
				if ( CYCLON_GROUP_BM_PMM[t][j][k] == 0 ) {
					if ( CYCLON_GROUP_BM_S[t][j][k] == s ) {
						if ( CYCLON_GROUP_BM_P[t][j][k] == p ) {
							CYCLON_GROUP_BM_PMM[t][j][k] = 1;
							//
//
SCH_CYCLON2_STATUS_VIEW( 41 , t , s , p );
//
							LeaveCriticalSection( &CR_CYCLON_GROUP );
							//
							return TRUE;
						}
					}
				}
				//
			}
			//
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return FALSE;
	//


}


BOOL SCH_CYCLON2_GROUP_GET_PM_SLOT_INFO( int pmc , int *data ) {
	int t , j , k;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return TRUE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	t = _SCH_PRM_GET_MODULE_GROUP( pmc );
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] != CYCLON_GROUP_BM_IDLE ) {
			//
			for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[t][j] ; k++ ) {
				//
				if ( CYCLON_GROUP_BM_PMM[t][j][k] == 0 ) {
					if ( _SCH_CLUSTER_Get_PathDo( CYCLON_GROUP_BM_S[t][j][k] , CYCLON_GROUP_BM_P[t][j][k] ) != PATHDO_WAFER_RETURN ) {
						data[CYCLON_GROUP_BM_PMS[t][j][k]] = 1;
					}
				}
			}
			//
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			//
			return TRUE;
		}
		//
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return FALSE;
	//
}










BOOL SCH_CYCLON2_GROUP_GET_NEED_DUMMY( int t0 , int *t ) {
	int i , j;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );

	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( ( t0 != -1 ) && ( t0 != i ) ) continue;
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( CYCLON_GROUP_BM_CONTROL[i][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) {
				//
				if ( !SCH_CYCLON2_GROUP_NOT_BM_NOMORE_SUPPLY( i ) ) continue; // 2013.02.15
				//
				*t = i;
				//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return TRUE;
			}
			//
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return FALSE;
}



BOOL SCH_CYCLON2_GROUP_RETURN( int s , int p ) { // 2013.02.15
	int i , j , k , retmode;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	if ( _SCH_CLUSTER_Get_PathIn( s , p ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
		_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
	}
	//
	retmode = ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN );
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( CYCLON_GROUP_BM_CONTROL[i][j] != CYCLON_GROUP_BM_IDLE ) {
				//
				for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
					//
					if ( ( CYCLON_GROUP_BM_S[i][j][k] == s ) && ( CYCLON_GROUP_BM_P[i][j][k] == p ) ) {
						//
						if ( CYCLON_GROUP_BM_COUNT[i][j] <= 1 ) {
							//
							CYCLON_GROUP_BM_COUNT[i][j] = 0;
							CYCLON_GROUP_BM_CONTROL[i][j] = CYCLON_GROUP_BM_IDLE;
							//
						}
						else {
							//
//							CYCLON_GROUP_BM_CONTROL[i][j] = CYCLON_GROUP_BM_SUPPLYING; // 2013.04.11
							//
							CYCLON_GROUP_BM_COUNT[i][j]--;
							//
							for ( ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
								//
								CYCLON_GROUP_BM_S[i][j][k] = CYCLON_GROUP_BM_S[i][j][k+1];
								CYCLON_GROUP_BM_P[i][j][k] = CYCLON_GROUP_BM_P[i][j][k+1];
								CYCLON_GROUP_BM_D[i][j][k] = CYCLON_GROUP_BM_D[i][j][k+1];
								CYCLON_GROUP_BM_PMS[i][j][k] = CYCLON_GROUP_BM_PMS[i][j][k+1];
								CYCLON_GROUP_BM_PMM[i][j][k] = CYCLON_GROUP_BM_PMM[i][j][k+1];
								//
							}
							//
						}
						//
						if ( retmode ) { // 2013.04.11
							//
							for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
								//
								if ( !CYCLON_GROUP_BM_D[i][j][k] ) continue;
								//
								if ( _SCH_CLUSTER_Get_PathDo( CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] ) != PATHDO_WAFER_RETURN ) {
									_SCH_CLUSTER_Check_and_Make_Return_Wafer( CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] , -1 );
								}
							}
							//
						}

//
SCH_CYCLON2_STATUS_VIEW( 51 , i , s , p );
//
						//
						LeaveCriticalSection( &CR_CYCLON_GROUP );
						//
						return TRUE;

					}
				}
				//
			}
			//
		}
	}
//
SCH_CYCLON2_STATUS_VIEW( 52 , 0 , s , p );
//
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	//
	return FALSE;
}


void SCH_CYCLON2_GROUP_SUPPLY_STOP( int s , int p ) {
	int i , j , k;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
			//
			if ( CYCLON_GROUP_BM_CONTROL[i][j] != CYCLON_GROUP_BM_IDLE ) {
				//
				for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
					//
					if ( ( CYCLON_GROUP_BM_S[i][j][k] == s ) && ( CYCLON_GROUP_BM_P[i][j][k] == p ) ) {
						//
						for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[i][j] ; k++ ) {
							//
							if ( !CYCLON_GROUP_BM_D[i][j][k] ) continue;
							//
							if ( _SCH_CLUSTER_Get_PathDo( CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] ) != PATHDO_WAFER_RETURN ) {
								_SCH_CLUSTER_Check_and_Make_Return_Wafer( CYCLON_GROUP_BM_S[i][j][k] , CYCLON_GROUP_BM_P[i][j][k] , -1 );
							}
						}
						//
						CYCLON_GROUP_BM_CONTROL[i][j] = CYCLON_GROUP_BM_SUPPLY_FINISH;
						//
//
SCH_CYCLON2_STATUS_VIEW( 61 , i , s , p );
//
						//
						LeaveCriticalSection( &CR_CYCLON_GROUP );
						//
						return;

					}
				}
				//
			}
			//
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
}



BOOL SCH_CYCLON2_GROUP_SET_SUPPLY_MORE_PM( int t , int s , int p , int pmc , int ss ) {
	int j , k , f , selj , selk;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	selj = -1;
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] != CYCLON_GROUP_BM_IDLE ) {
			//
			for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[t][j] ; k++ ) {
				//
				if ( CYCLON_GROUP_BM_S[t][j][k] == s ) {
					if ( CYCLON_GROUP_BM_P[t][j][k] == p ) {
						selj = j;
						selk = k;
						break;
					}
				}
				//
			}
			//
		}
		//
		if ( selj != -1 ) break;
		//
	}
	//
	if ( selj == -1 ) {
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		return FALSE;
	}
	//
	for ( k = 0 ; k < CYCLON_GROUP_BM_COUNT[t][selj] ; k++ ) {
		//
		if ( selk == k ) continue;
		//
		f = 0;
		//
		for ( j = 0 ; j < ss ; j++ ) {
			//
			if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) <= 0 ) continue;
			//
			if ( CYCLON_GROUP_BM_S[t][selj][k] == _SCH_MODULE_Get_PM_SIDE( pmc , j ) ) {
				if ( CYCLON_GROUP_BM_P[t][selj][k] == _SCH_MODULE_Get_PM_POINTER( pmc , j ) ) {
					f = 1;
					break;
				}
			}
			//
		}
		//
		if ( f == 0 ) {
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			return TRUE;
		}
		//
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}

BOOL SCH_CYCLON2_GROUP_WAIT_ANOTHER_WAFER_IN_PM( int t , int side , int pt , int pmc , int ss ) { // 2013.05.28
	int j , sel;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	sel = _SCH_CLUSTER_Get_Stock( side , pt ) / 100;
	//
	for ( j = 0 ; j < ss ; j++ ) {
		//
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_PM_STATUS( pmc , j ) != 0 ) continue;
		//
		if ( sel != ( _SCH_CLUSTER_Get_Stock( _SCH_MODULE_Get_PM_SIDE( pmc , j ) , _SCH_MODULE_Get_PM_POINTER( pmc , j ) ) / 100 ) ) {
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			return TRUE;
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}



BOOL SCH_CYCLON2_GROUP_UNMATCHED_COUNT_RUN_STATUS( int t ) { // 2013.05.28
	int j , count;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	if ( Scheduler_BM_Get_First_for_CYCLON( t ) == -1 ) return FALSE; // 2013.09.02
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	count = -1;
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_IDLE ) continue;
		if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING ) continue;
		if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_SUPPLYING_DUMMY ) continue;
		//
		if ( count == -1 ) {
			count = CYCLON_GROUP_BM_COUNT[t][j];
		}
		else {
			if ( count != CYCLON_GROUP_BM_COUNT[t][j] ) {
				//
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return TRUE;
			}
		}
		//
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}


BOOL SCH_CYCLON2_GROUP_AFTER_PICK_PM( int t , int side , int pt , int pmc , int ss ) { // 2013.05.28
	int j , sel , f;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) return FALSE;
	//
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	f = -1;
	sel = _SCH_CLUSTER_Get_Stock( side , pt ) / 100;
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] == CYCLON_GROUP_BM_PROCESSING ) {
			//
			if ( CYCLON_GROUP_BM_INDEX[t][j] == sel ) {
				//
				f = j;
				//
				break;
			}
		}
	}
	//
	if ( f == -1 ) {
		LeaveCriticalSection( &CR_CYCLON_GROUP );
		return FALSE;
	}
	//
	for ( j = 0 ; j < ss ; j++ ) {
		//
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , j ) <= 0 ) continue;
		//
		if ( sel == ( _SCH_CLUSTER_Get_Stock( _SCH_MODULE_Get_PM_SIDE( pmc , j ) , _SCH_MODULE_Get_PM_POINTER( pmc , j ) ) / 100 ) ) {
			LeaveCriticalSection( &CR_CYCLON_GROUP );
			return TRUE;
		}
	}
	//
	CYCLON_GROUP_BM_CONTROL[t][f] = CYCLON_GROUP_BM_IDLE;
//
SCH_CYCLON2_STATUS_VIEW( 71 , t , side , pt );
//
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return FALSE;
	//
}

int SCH_CYCLON2_GROUP_COUNT( int t , int index ) { // 2013.06.26
	int j , Res;
	EnterCriticalSection( &CR_CYCLON_GROUP );
	//
	for ( j = 0 ; j < MAX_SUPPLY_GROUP ; j++ ) {
		//
		if ( CYCLON_GROUP_BM_CONTROL[t][j] != CYCLON_GROUP_BM_IDLE ) {
			//
			if ( CYCLON_GROUP_BM_INDEX[t][j] == index ) {
				//
				Res = CYCLON_GROUP_BM_COUNT[t][j];
				LeaveCriticalSection( &CR_CYCLON_GROUP );
				//
				return Res;
				break;
			}
		}
	}
	//
	LeaveCriticalSection( &CR_CYCLON_GROUP );
	return 0;
}
