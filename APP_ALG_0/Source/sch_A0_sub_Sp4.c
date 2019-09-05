//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_prepost.h"
#include "INF_MultiJob.h"
#include "INF_User_Parameter.h"
#include "INF_Timer_Handling.h"
#include "INF_sch_CommonUser.h"
#include "INF_Sch_Multi_ALIC.h" // 2017.01.06
//================================================================================
#include "sch_A0_subBM.h"
#include "sch_A0_param.h"
#include "sch_A0_equip.h"
#include "sch_A0_dll.h"
#include "sch_A0_sub.h"
#include "sch_A0_sub_sp4.h"
//================================================================================
char *Recipe_Pre_OrgBuffer[MAX_CASSETTE_SIDE][ MAX_CASSETTE_SLOT_SIZE ][ MAX_CLUSTER_DEPTH ][ MAX_PM_CHAMBER_DEPTH ]; // 2008.05.20
char *Recipe_Post_OrgBuffer[MAX_CASSETTE_SIDE][ MAX_CASSETTE_SLOT_SIZE ][ MAX_CLUSTER_DEPTH ][ MAX_PM_CHAMBER_DEPTH ]; // 2008.05.20
//================================================================================
//CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4; // 2008.07.24
//CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4_2; // 2009.07.22
CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4; // 2011.03.16
CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4_LPOST; // 2011.03.16
CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4_LPRE; // 2011.03.16
CLUSTERStepTemplate2 G_CLUSTER_INFO_A0SUB4_USER; // 2011.03.16
//================================================================================

extern BOOL FM_Pick_Running_Blocking_style_0; // 2008.04.15

int xinside_A0SUB4_PREMAINSKIP[MAX_CASSETTE_SIDE]; // 2008.11.22
int xinside_A0SUB4_WFRPRESKIP[MAX_CASSETTE_SIDE]; // 2009.06.28
int xinside_A0SUB4_WFRPOSTSKIP[MAX_CASSETTE_SIDE]; // 2009.06.28

//=======================================================================================
// Pre	"LotPre|WaferPre|PreMain"	(Force-LotPre)		$M = 2
//		$X = 0		1		2			3
// Main	"Main"											$M = 0
//		$X = 0
// Post	"WaferPost|LotPost"			(Force-LotPost)		$M = 1 // 2010.12.17
// Post	"WaferPost|LotPost|LotPre"	(Force-LotPost)		$M = 1 // 2010.12.17
//		$X = 0		1		(M2,X0)			2		
//=======================================================================================
int  xinside_A0SUB4_AL_CHAMBER = PM1 + 11 - 1;
int  xinside_A0SUB4_AL_CHAMBER_SIDE;
int  xinside_A0SUB4_AL_CHAMBER_PT;
int  xinside_A0SUB4_AL_CHAMBER_TMS;
int  xinside_A0SUB4_AL_CHAMBER_ARM;
int  xinside_A0SUB4_AL_CHAMBER_ORD;

int  xinside_A0SUB4_LOTPOST_CLIDX[MAX_PM_CHAMBER_DEPTH];
int  xinside_A0SUB4_LOTPOST_ORGSIDE[MAX_PM_CHAMBER_DEPTH]; // 2010.09.26
//char xinside_A0SUB4_LOTPOST_RECIPE[MAX_PM_CHAMBER_DEPTH][256]; // 2008.04.02
char *xinside_A0SUB4_LOTPOST_RECIPE[MAX_PM_CHAMBER_DEPTH]; // 2008.04.02
//
//int  xinside_A0SUB4_LOTPRE_CLIDX[MAX_PM_CHAMBER_DEPTH]; // 2008.08.06
//char xinside_A0SUB4_LOTPRE_RECIPE[MAX_PM_CHAMBER_DEPTH][256]; // 2008.04.02
//char *xinside_A0SUB4_LOTPRE_RECIPE[MAX_PM_CHAMBER_DEPTH]; // 2008.04.02 // 2008.08.06
//
int  xinside_A0SUB4_LOTPRE_CLIDX[MAX_CASSETTE_SIDE][MAX_PM_CHAMBER_DEPTH]; // 2008.08.06
char *xinside_A0SUB4_LOTPRE_RECIPE[MAX_CASSETTE_SIDE][MAX_PM_CHAMBER_DEPTH]; // 2008.04.02 // 2008.08.06
//=======================================================================================
int xinside_A0SUB4_LOT_PRE_CHECK[MAX_CASSETTE_SIDE]; // 2008.05.28
//=======================================================================================
int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE; // 2011.01.25
int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT; // 2011.01.25
int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[MAX_PM_CHAMBER_DEPTH]; // 2011.01.25
//=======================================================================================
int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE; // 2011.01.25
int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT; // 2011.01.25
int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[MAX_PM_CHAMBER_DEPTH]; // 2011.01.25
//=======================================================================================
int xinside_A0SUB4_LOT_ORDER_ALL_INDEX; // 2008.04.17
int xinside_A0SUB4_LOT_ORDER_INDEX[MAX_CASSETTE_SIDE]; // 2008.04.17

int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE; // 2007.02.09
int xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER; // 2008.04.17
int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX; // 2007.02.09
int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT = 0; // 2007.02.09
int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTSIDE; // 2007.11.05
int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTINDEX; // 2007.11.05

BOOL xinside_A0SUB4_PICKFROMCM_RUN[MAX_CASSETTE_SIDE]; // 2008.04.17
int  xinside_A0SUB4_PICKFROMCM_PT[MAX_CASSETTE_SIDE]; // 2008.04.17
int  xinside_A0SUB4_PICKFROMCM_RESULT[MAX_CASSETTE_SIDE]; // 2008.04.17

BOOL xinside_A0SUB4_ENDPART_RUN[MAX_CASSETTE_SIDE]; // 2008.05.21
int  xinside_A0SUB4_ENDPART2_RUN[MAX_CASSETTE_SIDE]; // 2010.02.23
//=======================================================================================
int  xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
//=======================================================================================

#define MAX_LOTPRE_SKIP_SIZE	1024

CRITICAL_SECTION CR_A0SUB4_LOTPRE_SKIP; // 2011.03.16
int   xinside_A0SUB4_LOTPRE_SKIP_SIDE[MAX_LOTPRE_SKIP_SIZE]; // 2011.02.22
int   xinside_A0SUB4_LOTPRE_SKIP_CHAMBER[MAX_LOTPRE_SKIP_SIZE]; // 2011.02.22
int   xinside_A0SUB4_LOTPRE_SKIP_CLINDX[MAX_LOTPRE_SKIP_SIZE]; // 2011.02.22
char *xinside_A0SUB4_LOTPRE_SKIP_RECIPE[MAX_LOTPRE_SKIP_SIZE]; // 2011.02.22

BOOL SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( int side , int Chamber , int clindx , char *rcpname_pre ) { // 2011.02.22
	int i;
	//
//_IO_CIM_PRINTF( "[FIND1] [side=%d][Chamber=%d][clindx=%d]\n" , side , Chamber , clindx );
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
	//
	for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
		if ( xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] == side ) {
			if ( xinside_A0SUB4_LOTPRE_SKIP_CHAMBER[i] == Chamber ) {
				if ( xinside_A0SUB4_LOTPRE_SKIP_CLINDX[i] == clindx ) {
					if ( STR_MEM_SIZE( xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] ) > 0 ) {
						strncpy( rcpname_pre , xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] , 255 );
						//
//_IO_CIM_PRINTF( "[FIND2] [side=%d][Chamber=%d][clindx=%d][%s]\n" , side , Chamber , clindx , rcpname_pre );
						LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
						//
						return TRUE;
					}
					//
					LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
					//
					return FALSE;
				}
			}
		}
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
	//
	return FALSE;
}

void SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( int side , int Chamber , int clindx , char *rcpname_pre ) { // 2011.02.22
	int i;
//	_IO_CIM_PRINTF( "[APPEND1] [side=%d][Chamber=%d][clindx=%d][rcpname_pre=%s]\n" , side , Chamber , clindx , rcpname_pre );

	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
	//
	if ( side == -1 ) {
//		if ( Chamber == 1 ) { // Side Start
			for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
				if ( xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] == clindx ) {
					if ( xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] != NULL ) {
						free( xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] );
					}
					//
					xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] = NULL; // 2011.03.30
					//
					xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] = -1;
				}
			}
//		}
/*
		else { // First
			//
			for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
				if ( xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] != NULL ) {
					free( xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] );
				}
			}
			//
			for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
				xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] = -1;
				xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] = NULL;
			}
			//
		}
*/
	}
	else {
		//
		for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
			if ( xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] == -1 ) {
				//
				xinside_A0SUB4_LOTPRE_SKIP_CHAMBER[i] = Chamber;
				xinside_A0SUB4_LOTPRE_SKIP_CLINDX[i] = clindx;
				//
				if ( !STR_MEM_MAKE_COPY2( &(xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i]) , rcpname_pre ) ) {
					_IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND Memory Allocate Error[%d,%d,%d,%s]\n" , side , Chamber , clindx , rcpname_pre );
				}
				//
	_IO_CONSOLE_PRINTF( "A0S4-LOTPRE_SKIP_APPEND\t[S=%d][C=%d][M=%d][R=%s][X=%d]\n" , side , clindx , Chamber , rcpname_pre , i );
				//
				xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] = side;
				//
				break;
				//
			}
		}
		//
		if ( i == MAX_LOTPRE_SKIP_SIZE ) _IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND Space Error[%d,%d,%d,%s]\n" , side , Chamber , clindx , rcpname_pre );
		//
	}
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
	//
}


/*void Test_XXXX() {
	int i , j , p;
	int casssts[MAX_CASSETTE_SIDE];
	int casschm[MAX_CASSETTE_SIDE];
	char rcpname_pre[256];
	//
	for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
		casssts[j] = 0;
		casschm[j] = 0;
	}

	for ( i = 0 ; i < 4096 ; i++ ) {
		//
//		printf( "[%d]" , i );
		//
		sprintf( rcpname_pre , "RECIPE_FILE_is %d" , i );
		//
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			//
			if ( casssts[j] == 0 ) {
printf( "[i=%d][j=%d][SKIP_APPEND -1] START\n" , i , j );
				SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( -1 , 1 , j , "" );
printf( "[i=%d][j=%d][SKIP_APPEND -1] END\n" , i , j );
			}
			else {
				for ( p = 0 ; p < MAX_PM_CHAMBER_DEPTH ; p++ ) {
printf( "[i=%d][j=%d][SKIP_APPEND %d PM%d] START\n" , i , j , j , p + 1 );
					SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( j , p + PM1 , casschm[j] , rcpname_pre );
printf( "[i=%d][j=%d][SKIP_APPEND %d PM%d] END\n" , i , j , j , p + 1 );
				}
			}
			//
			casssts[j]++;
			casschm[j]++;
			if ( casssts[j] > (j+5) ) casssts[j] = 0;
			if ( casschm[j] > 4 ) casschm[j] = 0;
			//
			for ( p = 0 ; p < MAX_PM_CHAMBER_DEPTH ; p++ ) {
				SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( j , p + PM1 , 0 , rcpname_pre );
				SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( j , p + PM1 , 1 , rcpname_pre );
				SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( j , p + PM1 , 2 , rcpname_pre );
				SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( j , p + PM1 , 3 , rcpname_pre );
				SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( j , p + PM1 , 4 , rcpname_pre );
			}
		}
		//
	}
	//
}
*/

//void SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( int side , int Chamber , int clindx , char *rcpname_pre ) { // 2011.02.22
//	printf( "[START] [side=%d][Chamber=%d][clindx=%d][rcpname_pre=%s]\n" , side , Chamber , clindx , rcpname_pre );
//	SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND_sub( side , Chamber , clindx , rcpname_pre );
//	printf( "[END] [side=%d][Chamber=%d][clindx=%d][rcpname_pre=%s]\n" , side , Chamber , clindx , rcpname_pre );
//}

void SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB4( int apmode , int side ) {
	int i , j , k , l;
	if ( apmode == 0 ) { // 2008.04.02
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				for ( k = 0 ; k < MAX_CLUSTER_DEPTH ; k++ ) {
					for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
						Recipe_Pre_OrgBuffer[i][ j ][ k ][l] = NULL; // 2008.05.20
						Recipe_Post_OrgBuffer[i][ j ][ k ][l] = NULL; // 2008.05.20
					}
				}
			}
		}
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			xinside_A0SUB4_LOTPOST_RECIPE[j] = NULL;
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
//				xinside_A0SUB4_LOTPRE_RECIPE[j] = NULL; // 2008.08.06
				xinside_A0SUB4_LOTPRE_RECIPE[i][j] = NULL; // 2008.08.06
			}
		}
		//
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX = -1;
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT = 0;
		//
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE = -1; // 2011.01.25
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT = -1; // 2011.01.25
		//
		xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE = -1; // 2011.01.25
		xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT = -1; // 2011.01.25
		//
		//==================================================================================
		// 2008.04.17
		//==================================================================================
		xinside_A0SUB4_LOT_ORDER_ALL_INDEX = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			xinside_A0SUB4_LOT_ORDER_INDEX[i] = 0;
			//
			xinside_A0SUB4_PICKFROMCM_RUN[i] = FALSE; // 2008.04.17
			//
			xinside_A0SUB4_ENDPART_RUN[i] = FALSE; // 2008.05.21
			xinside_A0SUB4_ENDPART2_RUN[i] = 0; // 2010.02.23
			//
			xinside_A0SUB4_LOT_PRE_CHECK[i] = 0; // 2008.05.28
		}
		//==================================================================================
		// 2011.02.22
		InitializeCriticalSection( &CR_A0SUB4_LOTPRE_SKIP );
		//
		for ( i = 0 ; i < MAX_LOTPRE_SKIP_SIZE ; i++ ) {
			xinside_A0SUB4_LOTPRE_SKIP_SIDE[i] = -1;
			xinside_A0SUB4_LOTPRE_SKIP_RECIPE[i] = NULL;
		}
		//
		//==================================================================================
	}
	//
	if ( apmode == 3 ) {
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX = -1;
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT = 0;
		//
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE = -1; // 2011.01.25
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT = -1; // 2011.01.25
		//
		xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE = -1; // 2011.01.25
		xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT = -1; // 2011.01.25
		//
		xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
		//
		//==================================================================================
//		SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( -1 , 0 , 0 , "" ); // 2011.02.22 // 2011.03.17
		//==================================================================================
		//
	}
	//
	if ( ( apmode == 1 ) || ( apmode == 3 ) ) {
		//==================================================================================
		// 2008.04.17
		//==================================================================================
		xinside_A0SUB4_LOT_ORDER_INDEX[side] = xinside_A0SUB4_LOT_ORDER_ALL_INDEX;
		xinside_A0SUB4_LOT_ORDER_ALL_INDEX++;
		//
		xinside_A0SUB4_PICKFROMCM_RUN[side] = FALSE; // 2008.04.17
		//
		xinside_A0SUB4_ENDPART_RUN[side] = FALSE; // 2008.05.21
		xinside_A0SUB4_ENDPART2_RUN[side] = 0; // 2010.02.23
		//
		xinside_A0SUB4_LOT_PRE_CHECK[side] = 0; // 2008.05.28
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) { // 2008.08.06
			xinside_A0SUB4_LOTPRE_CLIDX[side][j] = -99; // 2008.08.06
		} // 2008.08.06
		//
		//==================================================================================
		SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( -1 , 1 , side , "" ); // 2011.02.22
		//==================================================================================
	}
}
//===================================================================================================
int  SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Index_A0SUB4( int *side , int *order ) { // 2007.02.09
	*side  = xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE;
	*order = xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER; // 2008.04.17
	return xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX;
}
//=======================================================================================
int  SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Act_A0SUB4( int *side , int *clidx ) { // 2007.02.09
	*side  = xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTSIDE; // 2007.11.05
	*clidx = xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTINDEX; // 2007.11.05
	return xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT;
}
//=======================================================================================
void SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( int data , int side , int clidx ) { // 2007.02.09
	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTSIDE  = side; // 2007.11.05
	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTINDEX = clidx; // 2007.11.05
	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT = data;
}
//=======================================================================================
void SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( int side , int data ) { // 2007.02.09
//	if ( xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX <= data ) { // 2008.02.29 // 2008.03.19
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE  = side;
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER = xinside_A0SUB4_LOT_ORDER_INDEX[ side ]; // 2008.04.17
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX = data;
		xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT = 0;
//	} // 2008.03.19
}
//=======================================================================================
void SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( int side , int id , CLUSTERStepTemplate2 *CLUSTER_INFO , CLUSTERStepTemplate4 *CLUSTER_INFO4 ) { // 2004.12.14
	int j , k , m;
	//
	if ( _SCH_CLUSTER_Get_PathRange( side , id ) >= 0 ) {
		CLUSTER_INFO->HANDLING_SIDE = _SCH_CLUSTER_Get_PathRange( side , id );
		CLUSTER_INFO->EXTRA_STATION = ( _SCH_CLUSTER_Get_PathIn( side , id ) * 100 ) + _SCH_CLUSTER_Get_SlotIn( side , id );
		CLUSTER_INFO->EXTRA_TIME    = ( _SCH_CLUSTER_Get_PathOut( side , id ) * 100 ) + _SCH_CLUSTER_Get_SlotOut( side , id );
		//
		for ( j = 0 ; j < CLUSTER_INFO->HANDLING_SIDE ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = _SCH_CLUSTER_Get_PathProcessChamber( side , id , j , k );
				CLUSTER_INFO->MODULE[j][k] = m;
				if ( m > 0 ) {
					CLUSTER_INFO->MODULE_IN_RECIPE2[j][k]  = _SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 0 );
					CLUSTER_INFO->MODULE_OUT_RECIPE2[j][k] = _SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 1 );
					CLUSTER_INFO->MODULE_PR_RECIPE2[j][k]  = _SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 2 );
				}
				else {
					CLUSTER_INFO->MODULE_IN_RECIPE2[j][k]  = NULL;
					CLUSTER_INFO->MODULE_OUT_RECIPE2[j][k] = NULL;
					CLUSTER_INFO->MODULE_PR_RECIPE2[j][k]  = NULL;
				}
			}
		}
		//====================================================================================
		CLUSTER_INFO4->LOT_SPECIAL_DATA = _SCH_CLUSTER_Get_LotSpecial( side , id ); // 2005.02.17
		//====================================================================================
		CLUSTER_INFO4->LOT_USER_DATA = _SCH_CLUSTER_Get_LotUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->CLUSTER_USER_DATA = _SCH_CLUSTER_Get_ClusterUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->RECIPE_TUNE_DATA = _SCH_CLUSTER_Get_ClusterTuneData( side , id );
		//====================================================================================
	}
	else {
		CLUSTER_INFO->HANDLING_SIDE = -1;
		//
		CLUSTER_INFO4->LOT_SPECIAL_DATA = 0;
		CLUSTER_INFO4->LOT_USER_DATA = NULL;
		CLUSTER_INFO4->CLUSTER_USER_DATA = NULL;
		CLUSTER_INFO4->RECIPE_TUNE_DATA = NULL;
	}
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( int bmc , int side , int pointer ) {
	int k , t , NextChamber;
	if ( _SCH_CLUSTER_Get_PathRange( side , pointer ) <= 0 ) return TRUE;
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		NextChamber = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , k );
		if ( NextChamber > 0 ) {
			for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( t , NextChamber , G_PLACE ) ) {
					if ( _SCH_MODULE_GROUP_TBM_POSSIBLE( t , bmc , G_PICK , G_MCHECK_SAME ) ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//=======================================================================================
void SCHEDULER_CONTROL_CHECK_RUN_START_RESTORE_A0SUB4( int side ) { // 2008.05.20
	int i , j , k;
	//
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				if ( !STR_MEM_MAKE_COPY2( &(Recipe_Pre_OrgBuffer[side][i][j][k]) , _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) ) ) {
					_IO_CIM_PRINTF( "SCHEDULER_CONTROL_CHECK_RUN_START_RESTORE_A0SUB4 1 Memory Allocate Error[%d,%d,%d,%d]\n" , side , i , j , k );
				}
				if ( !STR_MEM_MAKE_COPY2( &(Recipe_Post_OrgBuffer[side][i][j][k]) , _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_OUT ) ) ) {
					_IO_CIM_PRINTF( "SCHEDULER_CONTROL_CHECK_RUN_START_RESTORE_A0SUB4 2 Memory Allocate Error[%d,%d,%d,%d]\n" , side , i , j , k );
				}
			}
		}
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
}
void SCHEDULER_CONTROL_CHECK_RUN_AGAIN_RESTORE_A0SUB4( int side ) { // 2008.05.20
	int i , j , k;

//	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
//		if ( i == side ) continue;
//		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) break;
//	}
//	if ( i == MAX_CASSETTE_SIDE ) return;
	//
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				_SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , j , k , Recipe_Post_OrgBuffer[side][i][j][k] );
				_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , Recipe_Pre_OrgBuffer[side][i][j][k] );
			}
		}
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	xinside_A0SUB4_LOT_ORDER_INDEX[side] = xinside_A0SUB4_LOT_ORDER_ALL_INDEX;
	xinside_A0SUB4_LOT_ORDER_ALL_INDEX++;
	//
}
//=======================================================================================
//=======================================================================================
//=======================================================================================
void SCHEDULER_CONTROL_LOTPOST_REGIST( int mode , int side , int ch , int clidx , char *rcpname ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOST_REGIST\t[M=%d][S=%d][CH=PM%d][C=%d]=>[%s]\n" , mode , side , ch - PM1 + 1 , clidx , rcpname );
//------------------------------------------------------------------------------------------------------------------
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	if ( clidx < 0 )	xinside_A0SUB4_LOTPOST_CLIDX[ ch - PM1 ] = 0;
	else				xinside_A0SUB4_LOTPOST_CLIDX[ ch - PM1 ] = clidx;
//	strncpy( xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ] , rcpname , 255 ); // 2008.04.02
	//
	xinside_A0SUB4_LOTPOST_ORGSIDE[ ch - PM1 ] = -1; // 2010.09.26
	//
	if ( !STR_MEM_MAKE_COPY2( &(xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ]) , rcpname ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPOST_REGIST Memory Allocate Error[%d,%d,%d,%d]\n" , mode , side , ch , clidx );
	}
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
}

BOOL SCHEDULER_CONTROL_LOTPOST_EXTRACT( int side , int ch , int clidx , char *rcpname ) {
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	if ( clidx != xinside_A0SUB4_LOTPOST_CLIDX[ ch - PM1 ] ) {
		LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
		return FALSE;
	}
	if ( STR_MEM_SIZE( xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ] ) > 0 ) { // 2008.04.03
		strncpy( rcpname , xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ] , 255 ); // 2008.04.02
		LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
		return TRUE;
	}
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	return FALSE;
}
//
void SCHEDULER_CONTROL_LOTPOST_REMOVE( int side , int ch , int clidx ) { // 2009.06.28
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	if ( clidx != xinside_A0SUB4_LOTPOST_CLIDX[ ch - PM1 ] ) {
		LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
		return;
	}
	//
	xinside_A0SUB4_LOTPOST_CLIDX[ ch - PM1 ] = 0;
	//
	xinside_A0SUB4_LOTPOST_ORGSIDE[ ch - PM1 ] = -1; // 2010.09.26
	//
	if ( STR_MEM_SIZE( xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ] ) > 0 ) {
		if ( !STR_MEM_MAKE_COPY2( &(xinside_A0SUB4_LOTPOST_RECIPE[ ch - PM1 ]) , "" ) ) { // 2010.03.25
			_IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPOST_REMOVE Memory Allocate Error[%d,%d,%d]\n" , side , ch , clidx );
		}
	}
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
}
//=======================================================================================
void SCHEDULER_CONTROL_LOTPRE_sub_REGIST_FBUF( int mode , int side , int ch , int clidx , char *rcpname ) {

//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPRE_REGIST_FBUF\t[M=%d][S=%d][CH=PM%d][C=%d]=>[%s]\n" , mode , side , ch - PM1 + 1 , clidx , rcpname );
//------------------------------------------------------------------------------------------------------------------
	if ( clidx < 0 )	xinside_A0SUB4_LOTPRE_CLIDX[side][ ch - PM1 ] = 0; // 2008.08.06
	else				xinside_A0SUB4_LOTPRE_CLIDX[side][ ch - PM1 ] = clidx; // 2008.08.06
	if ( !STR_MEM_MAKE_COPY2( &(xinside_A0SUB4_LOTPRE_RECIPE[side][ ch - PM1 ]) , rcpname ) ) { // 2008.04.02 // 2008.08.06 // 2010.03.25
		_IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPRE_REGIST_FBUF Memory Allocate Error[%d,%d,%d,%d]\n" , mode , side , ch , clidx );
	}
}

BOOL SCHEDULER_CONTROL_LOTPRE_sub_EXTRACT_FBUF( int side , int ch , int clidx , char *rcpname ) {
//	if ( clidx != xinside_A0SUB4_LOTPRE_CLIDX[ ch - PM1 ] ) return FALSE; // 2008.08.06
//	if ( STR_MEM_SIZE( xinside_A0SUB4_LOTPRE_RECIPE[ ch - PM1 ] ) > 0 ) { // 2008.04.03 // 2008.08.06
//		strncpy( rcpname , xinside_A0SUB4_LOTPRE_RECIPE[ ch - PM1 ] , 255 ); // 2008.04.02 // 2008.08.06
//		return TRUE; // 2008.08.06
//	} // 2008.08.06
//	return FALSE; // 2008.08.06
	if ( clidx != xinside_A0SUB4_LOTPRE_CLIDX[side][ ch - PM1 ] ) return FALSE; // 2008.08.06
	if ( STR_MEM_SIZE( xinside_A0SUB4_LOTPRE_RECIPE[side][ ch - PM1 ] ) > 0 ) { // 2008.04.03 // 2008.08.06
		strncpy( rcpname , xinside_A0SUB4_LOTPRE_RECIPE[side][ ch - PM1 ] , 255 ); // 2008.04.02 // 2008.08.06
		return TRUE; // 2008.08.06
	} // 2008.08.06
	return FALSE; // 2008.08.06
}

//=======================================================================================
int SCHEDULER_CONTROL_LOTPREPOST_GET_PT( int side , int clix ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		if ( clix == -1 ) return i; // 2007.04.25
		if ( clix == _SCH_CLUSTER_Get_ClusterIndex( side , i ) ) return i;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2008.07.12
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		return i;
	}
	//
	return -1;
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_LOTPRE_EXTRACT( BOOL remap , int side , int ch , int clixorg , char *rcpname , char *rcpname_post , int mode ) {
	int i , j , k , z1 , z2 , l , prch , Res , dismode;
	int clix;
	int find = 0;
//	char lotpre[4096+1]; // 2007.06.11
//	char elsepre[4096+1]; // 2007.06.11
//	char waferpre[4096+1]; // 2007.06.11
//	char premain[4096+1]; // 2007.06.11
	char lotpre[512+1]; // 2007.06.11
	char waferpre[512+1]; // 2007.06.11
	char premain[512+1]; // 2007.06.11
	char *temp; // 2007.06.11

	//===========================================================================================================
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//===========================================================================================================
	if ( remap ) {
//		if ( mode == 1 ) { // 2008.08.01
		if ( ( mode == 1 ) || ( mode == 3 ) ) { // 2008.08.01
			//
//			return SCHEDULER_CONTROL_LOTPRE_sub_EXTRACT_FBUF( side , ch , clixorg , rcpname ); // 2011.03.24
			Res = SCHEDULER_CONTROL_LOTPRE_sub_EXTRACT_FBUF( side , ch , clixorg , rcpname ); // 2011.03.24
			//===========================================================================================================
			LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.24
			//===========================================================================================================
			return Res;
		}
		else {
//			SCHEDULER_CONTROL_LOTPRE_REGIST_FBUF( 0 , side , ch , -1 , "" ); // 2008.08.06
//			if ( mode == 2 ) { // 2008.08.06 // 2009.07.28
//			if ( ( mode == 2 ) || ( mode == 4 ) ) { // 2008.08.06 // 2009.07.28 // 2010.03.17
			if ( ( mode == 2 ) || ( mode == 4 ) || ( mode == 5 ) ) { // 2008.08.06 // 2009.07.28 // 2010.03.17
				if ( !SCHEDULER_CONTROL_LOTPRE_sub_EXTRACT_FBUF( side , ch , clixorg , rcpname ) ) { // 2008.08.06
					SCHEDULER_CONTROL_LOTPRE_sub_REGIST_FBUF( 0 , side , ch , -1 , "" );
				} // 2008.08.06
			}
			else {
				SCHEDULER_CONTROL_LOTPRE_sub_REGIST_FBUF( 0 , side , ch , -1 , "" );
			}
		}
	}
	//===========================================================================================================
	//
	clix = clixorg;

	strcpy( rcpname_post , "" );

	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		if ( clix == -1 ) {
			clix = _SCH_CLUSTER_Get_ClusterIndex( side , i );
		}
		else {
			if ( clix != _SCH_CLUSTER_Get_ClusterIndex( side , i ) ) continue;
		}
		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				prch = _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
				//
				dismode = 0;
				//
				if ( prch < 0 ) { // 2010.10.05
					//
					if ( _SCH_PRM_GET_MAINT_MESSAGE_STYLE() != 0 ) { // 2011.01.28
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , -prch ) == MUF_99_USE_to_DISABLE ) prch = -prch;
						//
					}
					//
					if ( -prch == ch ) { // 2011.04.01
						prch = -prch;
						dismode = 1;
					}
				}
				//
				if ( prch == ch ) {
					//=====================================================================================================
					if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , i , j , k , ch , FALSE ) ) {
						//======================================================================================================================
						// 2007.06.11
						//======================================================================================================================
						z1 = 0;
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , lotpre , z1 , 512 );
						if ( z2 == z1 ) {
							strcpy( lotpre , "" );
							strcpy( waferpre , "" );
							z1 = -1;
						}
						else {
							z1 = z2;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , waferpre , z1 , 512 );
							if ( z2 == z1 ) {
								strcpy( waferpre , "" );
								z1 = -1;
							}
							else {
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , premain , z1 , 512 );
								if ( z2 == z1 ) {
									z1 = -1;
								}
								else {
									if ( strlen( premain ) <= 0 ) z1 = -1;
								}
							}
						}
						//====
						if ( ( ( find == 0 ) && remap ) || ( find != 0 ) ) { // not find or remove
							if      ( z1 >= 0 ) {
								l = strlen( waferpre ) + strlen( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) + z1 ) + 2;
								temp = calloc( l + 1 , sizeof( char ) );
								if ( temp == NULL ) {
									_IO_CIM_PRINTF( "SCHEDULER_CONTROL_LOTPRE_EXTRACT Memory Allocate Error\n" );
									//===========================================================================================================
									LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.24
									//===========================================================================================================
									return FALSE;
								}
								strcpy( temp , "|" );
								strcat( temp , waferpre );
								strcat( temp , "|" );
								strcat( temp , _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) + z1 );
								//
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , temp );
								//
								free( temp );
							}
							else if ( strlen( waferpre ) > 0 ) {
//								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , waferpre ); // 2007.11.05
								sprintf( premain , "|%s" , waferpre ); // 2007.11.05
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , premain ); // 2007.11.05
							}
							else {
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , "" );
							}
						}
						//======================================================================================================================
						if ( !dismode ) { // 2011.04.01
							if ( find == 0 ) { // not find
								if ( strlen( lotpre ) > 0 ) {
									if ( !remap ) {
										strcpy( rcpname , lotpre );
	//									if ( mode == 2 ) return FALSE; // 2007.11.20 // 2009.07.28
	//									if ( ( mode == 2 ) || ( mode == 4 ) ) return FALSE; // 2007.11.20 // 2009.07.28 // 2010.03.17
										if ( ( mode == 2 ) || ( mode == 4 ) || ( mode == 5 ) ) {
											//
											LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
											//
											return FALSE; // 2007.11.20 // 2009.07.28 // 2010.03.17
										}
										//
										LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
										//
										return TRUE;
									}
									find = 1;
								}
								else {
									find = 2;
								}
							}
						}
						//======================================================================================================================
					}
					else {
						if ( !dismode ) { // 2011.04.01
							if ( find == 0 ) { // not find
								strcpy( lotpre , "" );
								find = 2;
							}
						}
					}
					//=====================================================================================================
					//=====================================================================================================
					//=====================================================================================================
					//=====================================================================================================
					if ( _SCH_CLUSTER_Check_Possible_UsedPost( side , i , j , k ) ) {
						STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , waferpre , premain , 512 );
						//
						if ( !dismode ) { // 2011.04.01
							if ( strlen( rcpname_post ) <= 0 ) {
								if ( strlen( premain ) > 0 ) {
									sprintf( rcpname_post , "%s" , premain );
								}
							}
						}
						//
						if ( remap ) {
							if ( mode != -2 ) _SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , j , k , waferpre );
						}
					}
					//=====================================================================================================
				}
			}
		}
	}
	if ( find == 1 ) {
		strcpy( rcpname , lotpre );
		//============================================================================================
//		if ( remap && ( mode == 0 ) ) SCHEDULER_CONTROL_LOTPRE_REGIST_FBUF( 1 , side , ch , clixorg , lotpre ); // 2008.04.23
		if ( mode >= 0 ) { // 2009.06.28
			if ( remap && ( mode != 1 ) ) SCHEDULER_CONTROL_LOTPRE_sub_REGIST_FBUF( 1 , side , ch , clixorg , lotpre ); // 2008.04.23
		}
		//============================================================================================
//		if ( mode == 2 ) return FALSE; // 2007.11.20 // 2009.07.28
//		if ( ( mode == 2 ) || ( mode == 4 ) ) return FALSE; // 2007.11.20 // 2009.07.28 // 2010.03.17
		if ( ( mode == 2 ) || ( mode == 4 ) || ( mode == 5 ) ) {
			//
			LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
			//
			return FALSE; // 2007.11.20 // 2009.07.28 // 2010.03.17
		}
		//============================================================================================
		//
		LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
		//
		//============================================================================================
		return TRUE;
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	return FALSE;
}

void SCHEDULER_DUMMY_WAFER_PRE_POST_CUT( int side , int pt ) { // 2010.01.26
	int i , j;
	char str1[1024+1];
	char str2[1024+1];
	//
	if ( _SCH_CLUSTER_Get_PathRun( side , pt , 16 , 2 ) < 20 ) return;
	//
	//
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			if ( _SCH_CLUSTER_Get_PathProcessChamber(side,pt,i,j) != 0 ) {
				//
				if ( _SCH_CLUSTER_Get_PathProcessRecipe(side,pt,i,j,SCHEDULER_PROCESS_RECIPE_PR) != NULL ) {
					STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe(side,pt,i,j,SCHEDULER_PROCESS_RECIPE_PR) , '|' , str1 , str2 , 1024 );
					//
					if ( ( strlen( str1 ) > 0 ) || ( strlen( str2 ) > 0 ) ) {
						_SCH_CLUSTER_Set_PathProcessData_JustPre( side , pt , i , j , str1 );
					}
					//
				}
				//
				if ( _SCH_CLUSTER_Get_PathProcessRecipe(side,pt,i,j,SCHEDULER_PROCESS_RECIPE_OUT) != NULL ) {
					STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe(side,pt,i,j,SCHEDULER_PROCESS_RECIPE_OUT) , '|' , str1 , str2 , 1024 );
					//
					if ( ( strlen( str1 ) > 0 ) || ( strlen( str2 ) > 0 ) ) {
						_snprintf( str1 , 1024 , "|%s" , str2 );
						_SCH_CLUSTER_Set_PathProcessData_JustPost( side , pt , i , j , str1 );
					}
					//

				}
			}
		}
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
}


BOOL SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( int mode , int side , int ch , int ptmode , int clixorg , BOOL realremove ) {
	int i , j , k , m;
	int clix;
//	char tempstr[4096+1]; // 2007.06.11
//	char elsepre[4096+1]; // 2007.06.11
//	char lotpre[4096+1]; // 2007.06.11
//	char waferpre[4096+1]; // 2007.06.11
	char premain[1024+1]; // 2007.06.11
	char elsepre[1024+1]; // 2007.06.11
	char lotpre[1024+1]; // 2007.06.11
	char waferpre[1024+1]; // 2007.06.11

	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17

	clix = clixorg;

	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( ptmode != -1 ) {
			if ( i != ptmode ) continue;
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) {
				//
				LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
				//
				return FALSE;
			}
		}
		else {
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
			if ( clix == -1 ) {
				clix = _SCH_CLUSTER_Get_ClusterIndex( side , i );
			}
			else {
				if ( clix != _SCH_CLUSTER_Get_ClusterIndex( side , i ) ) continue;
			}
		}
		//
		for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//
// 2011.04.01
//				if ( ch == -1 ) {
//					if ( _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k ) <= 0 ) continue;
//				}
//				else {
//					if ( _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k ) != ch ) continue;
//				}
//
// 2011.04.01
				m = _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
				if ( ch == -1 ) {
					if ( m == 0 ) continue;
				}
				else {
					if ( ( m != ch ) && ( -m != ch ) ) continue;
				}
				//
				if ( mode == 0 ) { // PreMain
					if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , i , j , k , ch , FALSE ) ) {
						STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , lotpre , elsepre , 1024 );
						STR_SEPERATE_CHAR( elsepre , '|' , waferpre , premain , 1024 );
						//
						if ( realremove ) {
							if ( ( strlen( lotpre ) > 0 ) || ( strlen( waferpre ) > 0 ) ) {
								_snprintf( elsepre , 1024 , "%s|%s" , lotpre , waferpre );
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , elsepre );
							}
							else {
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , "" );
							}
						}
						else {
							if ( strlen( premain ) > 0 ) {
								//
								LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
								//
								return TRUE;
							}
						}
					}
				}
				else if ( mode == 1 ) { // WfrPre
					if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , i , j , k , ch , FALSE ) ) {
						STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , lotpre , elsepre , 1024 );
						STR_SEPERATE_CHAR( elsepre , '|' , waferpre , premain , 1024 );
						//
						if ( realremove ) {
							if ( ( strlen( lotpre ) > 0 ) || ( strlen( premain ) > 0 ) ) {
								_snprintf( elsepre , 1024 , "%s||%s" , lotpre , premain );
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , elsepre );
							}
							else {
								_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , j , k , "" );
							}
						}
						else {
							if ( strlen( waferpre ) > 0 ) {
								//
								LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
								//
								return TRUE;
							}
						}
					}
				}
				else if ( mode == 2 ) { // WfrPost
					if ( _SCH_CLUSTER_Check_Possible_UsedPost( side , i , j , k ) ) { // 2006.01.11
						STR_SEPERATE_CHAR( _SCH_CLUSTER_Get_PathProcessRecipe( side , i , j , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , lotpre , premain , 1024 );
						//
						if ( realremove ) {
							if ( strlen( premain ) > 0 ) {
								_snprintf( elsepre , 1024 , "|%s" , premain );
								_SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , j , k , elsepre );
							}
							else {
								_SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , j , k , "" );
							}
						}
						else {
							if ( strlen( lotpre ) > 0 ) {
								//
								LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
								//
								return TRUE;
							}
						}
					}
				}
				//
			}
		}
		//
		if ( ptmode != -1 ) {
			//
			LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
			//
			return TRUE;
		}
		//
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	return FALSE;
}

//=======================================================================================
//===================================================================
int SCHEDULER_CONTROL_PICK_DO_WITH_ERROR_A0SUB4( int tms , int SCH_No , int cf , int ord ) {
	int CHECKING_SIDE , SCH_Slot , SCH_Slot2 , wsa , wsb , pt , Function_Result;
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) {
		EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
		if ( SCHEDULER_Get_MULTI_GROUP_PICK( SCH_No ) != -1 ) {
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
			return 0;
		}
		else if ( _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) <= 0 ) {
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
			return 0;
		}
		else {
			SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , tms );
			LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[SCH_No] );
		}
	}
	// pick
	CHECKING_SIDE = _SCH_MODULE_Get_PM_SIDE( SCH_No , 0 );
	pt = _SCH_MODULE_Get_PM_POINTER( SCH_No , 0 );
	//
	_SCH_MODULE_Set_TM_Move_Signal( tms , -1 );
	SCHEDULER_Set_TM_LastMove_Signal( tms , -1 );
	//
	SCH_Slot = 1;
	SCH_Slot2 = 0;
	//
	wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 );
	wsb = SCH_Slot;
	//
	_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Start from %s(%d:%d)[F%c]%cWHTM%dPICKSTART PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_No - PM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , SCH_No , tms + 1 , cf , wsa , wsa );
	Function_Result = _SCH_EQ_PICK_FROM_CHAMBER( tms , CHECKING_SIDE , SCH_No , cf , wsa , wsb , 0 , _SCH_CLUSTER_Get_PathIn(CHECKING_SIDE,pt) , FALSE , 0 , _SCH_COMMON_GET_METHOD_ORDER( CHECKING_SIDE , pt ) , FALSE );
	if ( Function_Result == SYS_ABORTED ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Fail from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_No - PM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
		_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , SCH_No , tms + 1 , cf , wsa , wsa );
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
		return -1;
	}
	else if ( Function_Result == SYS_ERROR ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Reject from %s(%d:%d)[F%c]%cWHTM%dPICKREJECT PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_No - PM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
		if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
		return -2;
	}
	_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Pick Success from %s(%d:%d)[F%c]%cWHTM%dPICKSUCCESS PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsb , wsa , cf + 'A' , 9 , tms + 1 , SCH_No - PM1 + 1 , wsb , wsa , cf + 'A' , 9 , wsa );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( CHECKING_SIDE , pt , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , SCH_No , tms + 1 , cf , wsa , wsa );
	//
	_SCH_MODULE_Set_TM_SIDE_POINTER( tms , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
	//
	_SCH_MODULE_Set_TM_PATHORDER( tms , cf , ord );
	_SCH_MODULE_Set_TM_TYPE( tms , cf , SCHEDULER_MOVING_IN );
	_SCH_MODULE_Set_TM_OUTCH( tms , cf , SCH_No );
	_SCH_MODULE_Set_TM_WAFER( tms , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) );
	_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 );
	_SCH_IO_MTL_ADD_PICK_COUNT( SCH_No , 1 );
	KPLT0_SET_PROCESS_TIME_OUT( SCH_No );
	_SCH_TIMER_SET_ROBOT_TIME_START( tms , cf );
	if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , SCH_No ) >= 2 ) SCHEDULER_Set_MULTI_GROUP_PICK( SCH_No , -1 );
	return 1;
}



BOOL SCHEDULER_CONTROL_PLACE_PICK_TROUBLE_WITH_ERROR_A0SUB4() {
	int i , k , t , Result;
	int R_Next_Move , R_Path_Order , R_Next_Chamber , R_Wait_Process , R_EndMode , SCH_WY , R_NextFinger , R_halfpick;
	//
	i = xinside_A0SUB4_AL_CHAMBER;
	//
	if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) return FALSE;
	//
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) return FALSE;
	//
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		if ( _SCH_SYSTEM_USING_GET(k) < 10 ) continue;
		for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
			if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( t , i , G_PICK ) ) continue;
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
				if ( SCHEDULER_Get_MULTI_GROUP_PICK( i ) == -1 ) {
					EnterCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] );
					Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( t , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_halfpick , 0 );
					LeaveCriticalSection( &CR_MULTI_GROUP_PICK_STYLE_0[i] );
				}
				else {
//					Result = -9999; // 2007.01.17
					Result = 9999; // 2007.01.17
				}
			}
			else {
				Result = SCHEDULING_CHECK_for_Enable_PICK_from_PM( t , k , i , -1 , &R_Next_Move , &R_Path_Order , &R_Next_Chamber , _SCH_PRM_GET_MODULE_MODE( FM ) , FALSE , &R_Wait_Process , _SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) , _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i ) , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( k ) , &R_EndMode , &SCH_WY , &R_NextFinger , &R_halfpick , 0 );
			}
			if ( Result > 0 ) return FALSE;
		}
	}
	//
	if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) return FALSE;
	//
	return TRUE;
}

BOOL SCHEDULER_CONTROL_PLACE_PICK_ACTION_WITH_ERROR_A0SUB4( int tms , int side , int pt , int orgch , int arm , int wid , int depid , int scm ) {
	int ch , Res , prcs_time , post_time , cid , narm , t , th , retmode;
	char NextPM[256];
	//========================================================================================================
	// 2007.01.23
	//========================================================================================================
	if ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , xinside_A0SUB4_AL_CHAMBER ) < 2 ) {
		if ( SCHEDULING_EQ_SUB4_INTERFACE_PREPARE_PLACE_CANCEL_STYLE_0( tms , orgch , arm , wid , depid , 0 , scm ) != SYS_SUCCESS ) return FALSE; // 2007.04.09
		return TRUE;
	}
	//========================================================================================================
	// 2007.01.16
	//========================================================================================================
	if ( SCHEDULER_Get_MULTI_GROUP_PLACE( xinside_A0SUB4_AL_CHAMBER ) != -1 ) {
		if ( SCHEDULING_EQ_SUB4_INTERFACE_PREPARE_PLACE_CANCEL_STYLE_0( tms , orgch , arm , wid , depid , 0 , scm ) != SYS_SUCCESS ) return FALSE; // 2007.04.09
		return TRUE;
	}
	//========================================================================================================
	Res = _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( orgch , &prcs_time , &post_time ) % 10;
	if ( Res != 2 ) {
		Res = _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( TM + tms , &prcs_time , &post_time ) % 10;
		if ( Res != 2 ) {
			if ( SCHEDULING_EQ_SUB4_INTERFACE_PREPARE_PLACE_CANCEL_STYLE_0( tms , orgch , arm , wid , depid , 0 , scm ) != SYS_SUCCESS ) return FALSE; // 2007.04.09
			return TRUE;
		}
	}
	//========================================================================================================
	if ( SCHEDULING_EQ_SUB4_INTERFACE_PREPARE_PLACE_CANCEL_STYLE_0( tms , orgch , arm , wid , depid , 0 , scm ) != SYS_SUCCESS ) return FALSE; // 2007.04.11
	//========================================================================================================
	_SCH_LOG_LOT_PRINTF( side , "TM%d Handling ReRunning for %s:%d%cWHTM%dRERUN PM%d:%d%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( orgch ) , wid , 9 , tms + 1 , orgch - PM1 + 1 , wid , 9 , wid );
	//========================================================================================================
	ch = xinside_A0SUB4_AL_CHAMBER;
	//========================================================================================================
	cid = 0;
	retmode = 0;
	while( TRUE ) {
		//==============================================================================
		cid++;
		//==============================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return FALSE;
		}
		//==============================================================================
		EnterCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
		if ( ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != -1 ) && ( SCHEDULER_Get_MULTI_GROUP_PLACE( ch ) != tms ) ) {
			LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] );
			cid = 0; // 2007.01.17
			Sleep(1);
			continue;
		}
		SCHEDULER_Set_MULTI_GROUP_PLACE( ch , tms ); // 2007.01.17
		LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
		//=============================================================================
		if ( ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) || ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) ) {
			//=============================================================================
			if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) cid = 0; // 2007.01.17
			//=============================================================================
			if ( cid > 100 ) {
				cid = 0; // 2007.01.17
				if ( SCHEDULER_CONTROL_PLACE_PICK_TROUBLE_WITH_ERROR_A0SUB4() ) {
					for ( th = 0 ; ( ( th < 3 ) && ( retmode == 0 ) ) ; th++ ) {
//						for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) { // 2007.01.17
						for ( t = 0 ; ( ( t < MAX_TM_CHAMBER_DEPTH ) && ( retmode == 0 ) ) ; t++ ) { // 2007.01.17
							if      ( th == 0 ) {
								if ( t != xinside_A0SUB4_AL_CHAMBER_TMS ) continue;
							}
							else if ( th == 1 ) {
								if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( t , ch , G_PICK ) ) continue;
							}
							//
							if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( t , ch , G_PLACE ) ) continue;
							//
							_SCH_SYSTEM_ENTER_TM_CRITICAL( t );
							if ( SCHEDULER_CONTROL_PLACE_PICK_TROUBLE_WITH_ERROR_A0SUB4() ) {
								if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( t , xinside_A0SUB4_AL_CHAMBER_ARM ) && ( _SCH_MODULE_Get_TM_WAFER( t , xinside_A0SUB4_AL_CHAMBER_ARM ) <= 0 ) ) {
									narm = xinside_A0SUB4_AL_CHAMBER_ARM;
								}
								else {
									if ( xinside_A0SUB4_AL_CHAMBER_ARM == TA_STATION ) {
										narm = TB_STATION;
									}
									else {
										narm = TA_STATION;
									}
									if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( t , narm ) && ( _SCH_MODULE_Get_TM_WAFER( t , narm ) <= 0 ) ) {
									}
									else {
										narm = -1;
									}
								}
								if ( narm != -1 ) {
									Res = SCHEDULER_CONTROL_PICK_DO_WITH_ERROR_A0SUB4( t , ch , narm , xinside_A0SUB4_AL_CHAMBER_ORD );
									if      ( Res == 0 ) { // skip
									}
									else if ( Res == 1 ) { // ok
//										_SCH_SYSTEM_LEAVE_TM_CRITICAL( t ); // 2007.01.17
//										SCHEDULER_Set_MULTI_GROUP_PLACE( ch , tms ); // 2007.01.17
//										LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
										retmode = 1;
//										break; // 2007.01.17
									}
									else if ( Res == -1 ) { // abort
//										_SCH_SYSTEM_LEAVE_TM_CRITICAL( t ); // 2007.01.17
//										LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
										retmode = 2;
//										return FALSE; // 2007.01.17
									}
									else if ( Res == -2 ) { // error
//										_SCH_SYSTEM_LEAVE_TM_CRITICAL( t ); // 2007.01.17
//										LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
//										return FALSE; // 2007.01.17
										retmode = 2;
									}
									else {
//										_SCH_SYSTEM_LEAVE_TM_CRITICAL( t ); // 2007.01.17
//										LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
//										return FALSE; // 2007.01.17
										retmode = 2;
									}
								}
							}
							_SCH_SYSTEM_LEAVE_TM_CRITICAL( t );
						}
					}
					if      ( retmode == 1 ) break;
					else if ( retmode == 2 ) return FALSE;
				}
			}
			//=============================================================================
//			LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
			Sleep(1);
			continue;
		}
//		SCHEDULER_Set_MULTI_GROUP_PLACE( ch , tms ); // 2007.01.17
//		LeaveCriticalSection( &CR_MULTI_GROUP_PLACE_STYLE_0[ch] ); // 2007.01.17
		//==============================================================================================================
		break;
	}
	//=========================================================================================================================================
	// Free Check
	//=========================================================================================================================================
	switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) ) {
	case -1 :
	case -3 :
	case -5 :
		SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
		_SCH_LOG_LOT_PRINTF( side , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
		return FALSE;
	}
	//=========================================================================================================================================
	// Place
	//=========================================================================================================================================
	_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Place Start to %s(%d:%d)[F%c]%cWHTM%dPLACESTART PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , tms + 1 , arm , wid , wid );
	if ( _SCH_EQ_PLACE_TO_CHAMBER( tms , side , ch , arm , wid , depid , TRUE , scm , FALSE , 0 , 0 , FALSE ) != SYS_SUCCESS ) {
		_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Place Fail to %s(%d:%d)[F%c]%cWHTM%dPLACEFAIL PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
		_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , tms + 1 , arm , wid , wid );
		SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
		return FALSE;
	}
	_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Place Success to %s(%d:%d)[F%c]%cWHTM%dPLACESUCCESS PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , tms + 1 , arm , wid , wid );
	_SCH_IO_MTL_ADD_PLACE_COUNT( ch , FALSE , side , 1 );
	//=========================================================================================================================================
	// Process
	//=========================================================================================================================================
	_SCH_CLUSTER_Get_Next_PM_String( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , NextPM , 255 );
	if ( _SCH_MACRO_MAIN_PROCESS( side , pt ,
		  ch , -1 ,
		  wid , -1 , ( tms * 100 ) + arm , -1 ,
		  "DummyRun" ,
		  0 , NextPM , PROCESS_DEFAULT_MINTIME ,
		  _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , 0 , 0 ,
		  -1 , -1 , -1 , 0 ,
		  0 , 1101 ) == 1 ) {
		//========================================================================================================
		while( TRUE ) {
			//==============================================================================
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
				return FALSE;
			}
			//==============================================================================
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) {
				break;
			}
			Sleep(1);
		}
		//=========================================================================================================================================
		switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) ) {
		case -1 :
		case -3 :
		case -5 :
			_SCH_LOG_LOT_PRINTF( side , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - PM1 + 1 );
			return FALSE;
		}
		//======================================================
	}
	//=========================================================================================================================================
	// Pick
	//=========================================================================================================================================
	_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Pick Start from %s(%d:%d)[F%cS]%cWHTM%dPICKSTART PM%d:%d:%d:%cS%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , tms + 1 , arm , wid , wid );
	if ( _SCH_EQ_PICK_FROM_CHAMBER( tms , side , ch , arm , wid , depid , FALSE , scm , FALSE , 0 , 0 , FALSE ) != SYS_SUCCESS ) {
		_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Pick Fail from %s(%d:%d)[F%c]%cWHTM%dPICKFAIL PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
		_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , tms + 1 , arm , wid , wid );
		SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
		return FALSE;
	}
	_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Pick Success from %s(%d:%d)[F%c]%cWHTM%dPICKSUCCESS PM%d:%d:%d:%c%c%d\n" , tms + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , depid , wid , arm + 'A' , 9 , tms + 1 , ch - PM1 + 1 , depid , wid , arm + 'A' , 9 , wid );
	_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pt , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , tms + 1 , arm , wid , wid );
	_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 );
	SCHEDULER_Set_MULTI_GROUP_PLACE( ch , -1 );
	return TRUE;
}
//=======================================================================================
//=======================================================================================
void SCHEDULER_CONTROL_PLACE_PICK_REPORT_WITH_ERROR_A0SUB4( int tms , int ch , int side , int pt , int arm , int ord ) {
	if ( xinside_A0SUB4_AL_CHAMBER == ch ) {
		xinside_A0SUB4_AL_CHAMBER_SIDE = side;
		xinside_A0SUB4_AL_CHAMBER_PT = pt;
		xinside_A0SUB4_AL_CHAMBER_TMS = tms;
		xinside_A0SUB4_AL_CHAMBER_ARM = arm;
		xinside_A0SUB4_AL_CHAMBER_ORD = ord;
	}
}

void SCHEDULER_CONTROL_SET_MAL_CHAMBER_A0SUB4( int ch ) {
	xinside_A0SUB4_AL_CHAMBER = ch;
}

//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
int SCHEDULER_CONTROL_A0SUB4_1S_Properly_RunCheck( int old_side , int old_clidx , int cur_side , int cur_clidx , int mode ) { // 2009.10.14
	int i , w1chk;
	int old_bmc , cur_bmc;
	//
	if ( ( old_side < 0 ) || ( old_side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( cur_side < 0 ) || ( cur_side >= MAX_CASSETTE_SIDE ) ) return 2;
	//
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 0 ) return 3;
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) return 4;
	//
	if ( old_side == cur_side ) return 9; // 2010.08.10
	//
	// 2011.03.08
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2011.03.03
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) {
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( old_side , i ) != _SCH_MODULE_Get_Mdl_Use_Flag( cur_side , i ) ) break;
		}
	}
	if ( i == ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) return 10;
	//
	old_bmc = 0;
	cur_bmc = 0;
	//
	w1chk = FALSE;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( old_side , i ) != MUF_00_NOTUSE ) old_bmc++;
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( cur_side , i ) != MUF_00_NOTUSE ) cur_bmc++;
			//
			if ( _SCH_PRM_GET_MODULE_SIZE( i ) == 1 ) w1chk = TRUE;
		}
		//
	}
	//
	if ( !w1chk ) return 5;
	//
	if ( ( ( old_clidx != -1 ) && ( cur_clidx != -1 ) && ( old_clidx != cur_clidx ) ) || ( old_bmc != cur_bmc ) ) {
		if ( ( mode == 0 ) && _SCH_MODULE_Chk_FM_Free_Status( old_side ) ) return 11;
		if ( ( mode != 0 ) && _SCH_MODULE_Chk_TM_Free_Status( old_side ) ) return 12;
		return -1;
	}
	//
	return 6;
}
//=======================================================================================
//=======================================================================================
void SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( int side , int Chamber , int clidx , char *rcpname , int mode ) { // 2012.07.26
	//
	char SrcFile[256];
	char TrgFile[256];
	char FileName[256];
	//
	int i , Slot;
	//

	if ( ( _SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 0 ) || ( _SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) != 0 ) ) return;
	if ( _SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 2 ) return;
	//
	Slot = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		//
		if ( clidx == -1 ) {
			Slot = _SCH_CLUSTER_Get_SlotIn( side , i );
			break;
		}
		if ( clidx == _SCH_CLUSTER_Get_ClusterIndex( side , i ) ) {
			Slot = _SCH_CLUSTER_Get_SlotIn( side , i );
			break;
		}
		//
	}
	//
	sprintf( FileName , "%d$%d$%s$%s$%s" , side , Slot , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , rcpname );	Change_Dirsep_to_Dollar( FileName );
//	sprintf( SrcFile , "%s/tmp/%s" , _SCH_PRM_GET_DFIX_LOG_PATH() , FileName ); // 2017.10.30
	sprintf( SrcFile , "%s/tmp/%s" , _SCH_PRM_GET_DFIX_TMP_PATH() , FileName ); // 2017.10.30
	//
	sprintf( FileName , "%d$%s$%s$%s" , side , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , rcpname );	Change_Dirsep_to_Dollar( FileName );
//	sprintf( TrgFile , "%s/tmp/%s" , _SCH_PRM_GET_DFIX_LOG_PATH() , FileName ); // 2017.10.30
	sprintf( TrgFile , "%s/tmp/%s" , _SCH_PRM_GET_DFIX_TMP_PATH() , FileName ); // 2017.10.30
	//
//	_SCH_LOG_LOT_PRINTF( side , "COPY [%s]->[%s]%cWHPMCOPY\n" , SrcFile , TrgFile , 9 );
	//
	CopyFile( SrcFile , TrgFile , FALSE );
	//
}

//=======================================================================================
//=======================================================================================
void SCHEDULER_CONTROL_A0SUB4_MAINT_WAITING( int side ) { // 2010.10.05
	int Chamber , f , com_curr , com_sel , com_mt;

	while ( TRUE ) {
		//
		f = FALSE;
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) != MUF_99_USE_to_DISABLE ) continue;
			//
			_SCH_COMMSTATUS_GET( Chamber , &com_curr , &com_sel , &com_mt );
			//
			if ( ( com_curr == 9 /*CSTS_CIU_DISABLE_WN*/ ) && ( ( com_sel == 4 /*CSTS_DISABLE*/ ) || ( com_sel == 9 /*CSTS_CIU_DISABLE_WN*/ ) ) && ( ( com_mt == 1 /*MSG_START*/ ) || ( com_mt == 2 /*MSG_END_RUNNING*/ ) ) ) {
				f = TRUE;
				break;
			}
			//
			if ( ( com_curr == 4 /*CSTS_DISABLE*/ ) && ( com_sel == 4 /*CSTS_DISABLE*/ ) ) {
				//
				if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( Chamber , FALSE ) == SYS_RUNNING ) {
					f = TRUE;
					break;
				}
			}
			//
		}
		//
		if ( !f ) break;
		//
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return;
		}
		//
		Sleep(1);
	}
	//
}

void SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( int side , int clindx , BOOL force , int mode , int pos ) {
	int Chamber;
	char rcpname[256];
	//============================================================================================================
	SCHEDULER_CONTROL_A0SUB4_MAINT_WAITING( side ); // 2010.10.05
	//============================================================================================================
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
//			if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( side , Chamber , FALSE ) != SYS_RUNNING ) { // 2010.02.23
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) <= 0 ) { // 2010.02.23
				if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( side , Chamber , clindx , rcpname ) ) {
					//
//					if ( _SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , -1 ) ) { // 2010.08.11 // 2010.09.10
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOT_POST_PROCESS\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][M=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname , force , mode , pos );
	//------------------------------------------------------------------------------------------------------------------
						//
						SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( side , Chamber , clindx , rcpname , force ? 22 : 12 ); // 2012.07.26
						//
//						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( side , // 2010.09.26
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( ( xinside_A0SUB4_LOTPOST_ORGSIDE[ Chamber - PM1 ] < 0 ) ? side : xinside_A0SUB4_LOTPOST_ORGSIDE[ Chamber - PM1 ] , // 2010.09.26
							Chamber ,
	//						0 , 0 , 0 , // 2010.03.10
							side + 1 , 0 , 0 , // 2010.03.10
							rcpname ,
							force ? 22 : 12 ,
							0 , "" , 0 ,
							1 , 1401 );
//					}
//					else { // 2010.08.11
//	//------------------------------------------------------------------------------------------------------------------
//	_IO_CONSOLE_PRINTF( "A0S4-LOT_POST_PROCESS(X)\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][M=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname , force , mode , pos );
//	//------------------------------------------------------------------------------------------------------------------
//					}
				}
			}
			else {
				Chamber--;
				Sleep(1);
			}
		}
	}
}


//int SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( int side , int diffpt , int clindx , int ch ) { // 2015.01.22
int SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( BOOL AllEnd , int side , int diffpt , int clindx , int ch ) { // 2015.01.22
	int Chamber , f;
	//
	if ( diffpt >= 0 ) {
		if ( clindx == -1 ) {
			if ( ( xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE != side ) || ( xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT != diffpt ) ) {
				return 1;
			}
		}
	}
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( ( ch != -1 ) && ( ch != Chamber ) ) continue; // 2011.04.29
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
			//
			if ( diffpt >= 0 ) {
				if ( clindx == -1 ) {
					if ( ( xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE == side ) && ( xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT == diffpt ) ) {
						if ( xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[Chamber-PM1] == 0 ) continue;
					}
				}
				else if ( clindx == -2 ) {
					if ( ( xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE == side ) && ( xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT == diffpt ) ) {
						if ( xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[Chamber-PM1] == 0 ) continue;
					}
				}
			}
			//
//			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) return 0; // 2015.01.22
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) { // 2015.01.22
				if ( AllEnd ) { // 2015.01.22
					if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( side , Chamber , FALSE ) == SYS_RUNNING ) {
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) >= PROCESS_INDICATOR_POST ) return 0;
						//
					}
				}
				else {
					return 0;
				}
			}
		}
	}
	f = FALSE;
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( ( ch != -1 ) && ( ch != Chamber ) ) continue; // 2011.04.29
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) < 0 ) f = TRUE;
		}
	}
	if ( f ) return -1;
	return 1; // 1 : Finish 0 : Running -1 : Fail
}

int SCHEDULER_CONTROL_A0SUB4_LotPre_Process_FinishCheck_Before_Pick( int side , int clindx , BOOL premode , BOOL notset ) { // 2008.05.28 // 2010.01.14
	//-----------------------------------------------------------------------------------------------
	if ( xinside_A0SUB4_LOT_PRE_CHECK[side] == 0 ) return 0; // 2008.05.28
	//-----------------------------------------------------------------------------------------------
	if ( premode ) {
		if ( xinside_A0SUB4_LOT_PRE_CHECK[side] != 1 ) return 0; // 2008.05.28
	}
	else {
		if ( xinside_A0SUB4_LOT_PRE_CHECK[side] != 2 ) return 0; // 2008.05.28
	}
	//-----------------------------------------------------------------------------------------------
//	switch ( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( side , -1 , clindx , -1 ) ) { // 2015.01.22
	switch ( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , side , -1 , clindx , -1 ) ) { // 2015.01.22
	case 1 : // finish
		//-----------------------------------------------------------------------------------------------
		if ( !notset ) { // 2010.01.14
			xinside_A0SUB4_LOT_PRE_CHECK[side] = 0;
		}
		//-----------------------------------------------------------------------------------------------
		return 0;
		break;
	case -1 :
		return -1; // fail
		break;
	}
	return 1; // run
}


int SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( int side , int clindx , int force , int mode , BOOL runonly , int Pos ) {
	BOOL find = FALSE;
	BOOL haspre;
	int Chamber;
	char rcpname_pre[256];
	char rcpname_post[256];
	//-----------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_CHECKING\t[S=%d][C=%d][M=%d][R=%d][POS=%d][force=%d]\n" , side , clindx , mode , runonly , Pos , force );
	//-----------------------------------------------------------------------------------------------
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
			//
//			if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( TRUE , side , Chamber , clindx , rcpname_pre , rcpname_post , mode ) ) { // 2011.02.22
			//
			haspre = FALSE; // 2011.02.22
			//
			if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( TRUE , side , Chamber , clindx , rcpname_pre , rcpname_post , mode ) ) {
				haspre = TRUE; // 2011.02.22
			}
			//
			if ( ( force == 1 ) && !haspre ) { // 2011.02.22
				if ( SCHEDULER_CONTROL_LOTPRE_SKIP_FIND( side , Chamber , clindx , rcpname_pre ) ) {
					haspre = TRUE;
				}
			}
			//
			if ( haspre ) {
//------------------------------------------------------------------------------------------------------------------
//				if ( _SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , -1 ) ) { // 2010.08.11 // 2010.09.10
					if ( mode == 3 ) {
	_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_CHECKING\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname_pre , mode , Pos );
					}
					else {
	_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_PROCESS\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname_pre , mode , Pos );
					}
	//------------------------------------------------------------------------------------------------------------------
					find = TRUE; // 2008.08.01
					//
	//				if ( mode != 3 ) { // 2008.08.01 // 2009.07.28
					if ( ( mode != 3 ) && ( mode != 4 ) ) { // 2008.08.01 // 2009.07.28
						//
						SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( side , Chamber , clindx , rcpname_pre , ( mode == 1 ) ? 21 : 11 ); // 2012.07.26
						//
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( side ,
							Chamber ,
	//						0 , 0 , 0 , // 2010.03.10
							side + 1 , 0 , 0 , // 2010.03.10
							rcpname_pre ,
							( mode == 1 ) ? 21 : 11 ,
							0 , "" , 0 ,
							1 , 1402 );
					}
//				}
//				else { // 2010.08.11
//					if ( mode == 3 ) {
//	_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_CHECKING(X)\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname_pre , mode , Pos );
//					}
//					else {
//	_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_PROCESS(X)\t[PM%d][S=%d][P=%d][C=%d][R=%s][E=%d][POS=%d]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname_pre , mode , Pos );
//					}
	//------------------------------------------------------------------------------------------------------------------
					find = TRUE; // 2008.08.01
					//
//				} // 2010.09.13
			}
//			if ( mode == 0 ) SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , side , Chamber , clindx , rcpname_post ); // 2010.03.17
			if ( ( mode == 0 ) || ( mode == 5 ) ) SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , side , Chamber , clindx , rcpname_post ); // 2010.03.17
		}
	}
	//-----------------------------------------------------------------------------------------------
//	if ( mode == 3 ) { // 2008.08.01 // 2009.07.28
	if ( ( mode == 3 ) || ( mode == 4 ) ) { // 2008.08.01 // 2009.07.28
		if ( find ) return 1;
		return 0;
	}
	//-----------------------------------------------------------------------------------------------
	xinside_A0SUB4_LOT_PRE_CHECK[side] = 0; // 2008.05.28
	//-----------------------------------------------------------------------------------------------
	// 2008.03.21
	//-----------------------------------------------------------------------------------------------
//	if ( mode != 2 ) { // 2008.04.25 // 2010.03.17
	if ( ( mode != 2 ) && ( mode != 5 ) ) { // 2008.04.25 // 2010.03.17
		switch( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ) {
		case 1 :
		case 3 :
		case 5 : // 2010.12.17
		case 7 : // 2010.12.17
			if ( runonly ) { // 2008.05.28
				//-----------------------------------------------------------------------------------------------
				xinside_A0SUB4_LOT_PRE_CHECK[side] = 2; // 2008.05.28
				//-----------------------------------------------------------------------------------------------
			}
			else {
				//-----------------------------------------------------------------------------------------------
				xinside_A0SUB4_LOT_PRE_CHECK[side] = 1; // 2008.05.28
				//-----------------------------------------------------------------------------------------------
/*
// 2008.05.28
				while( TRUE ) {
					switch ( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( side , -1 , clindx ) ) {
					case 1 :
						return 1;
						break;
					case -1 :
						return -1;
						break;
					}
					Sleep(250);
				}
*/
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------------------------
	if ( find ) return 1; // 2008.08.01
	return 0; // 2008.08.01
}

BOOL SCHEDULER_CONTROL_LOTPREPOST_APPEND( int Post_Pre_PreAtPost , int set , int s , int p , int Chamber , char *rcpname ) { // 2010.12.17
	char rcporgname[1024];
	char readrcp[512+1];
	int i , k , m , z1 , z2 , pr , pd;
	//
	if ( rcpname[0] == 0 ) return FALSE;
	//
	//
	EnterCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	pr = _SCH_CLUSTER_Get_PathRange( s , p );
	//
	if ( Post_Pre_PreAtPost == 2 ) {
		//
		if ( Chamber != -1 ) {
			if ( _SCH_MODULE_Get_PM_WAFER( Chamber , 0 ) > 0 ) {
				//
				LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
				//
				return TRUE;
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) {
				//
				LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
				//
				return TRUE;
			}
			//
			if ( _SCH_MODULE_Get_PM_WAFER( Chamber , 0 ) > 0 ) {
				//
				LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
				//
				return TRUE;
			}
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) {
				//
				LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
				//
				return TRUE;
			}
		}
		//
		pd = _SCH_CLUSTER_Get_PathDo( s , p );
		//
		if ( pd > pr ) {
			//
			LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
			//
			return FALSE;
		}
		//
		pd--;
		//
		if ( pd < 0 ) pd = 0;
		//
		//
	}
	else {
		pd = 0;
	}
	//
	for ( i = ( pr - 1 ) ; i >= pd ; i-- ) {
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , i , k );
			//
			if ( ( ( Chamber == -1 ) && ( m > 0 ) ) || ( m == Chamber ) || ( m == -Chamber ) ) {
				//
				if ( set != 0 ) {
					//
					if ( Post_Pre_PreAtPost == 0 ) {
						//
						z1 = 0;
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , readrcp , z1 , 512 );
						if ( z2 == z1 ) {
							//
							strcpy( rcporgname , "|" );
							strcat( rcporgname , rcpname );
							//
						}
						else {
							//
							strcpy( rcporgname , readrcp );
							strcat( rcporgname , "|" );
							strcat( rcporgname , rcpname );
							//
						}
						_SCH_CLUSTER_Set_PathProcessData_JustPost( s , p , i , k , rcporgname );
						//
						LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
						//
						return TRUE;
						//
					}
					else if ( Post_Pre_PreAtPost == 1 ) {
						//
						strcpy( rcporgname , rcpname );
						//
						z1 = 0;
						z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , readrcp , z1 , 512 );
						if ( z2 != z1 ) {
							z1 = z2;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , readrcp , z1 , 512 );
							if ( z2 != z1 ) {
								strcat( rcporgname , "|" );
								strcat( rcporgname , readrcp );
								//
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_PR ) , '|' , readrcp , z1 , 512 );
								if ( z2 != z1 ) {
									strcat( rcporgname , "|" );
									strcat( rcporgname , readrcp );
								}
							}
						}
						//
						_SCH_CLUSTER_Set_PathProcessData_JustPre( s , p , i , k , rcporgname );
						//
						LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
						//
						return TRUE;
						//
					}
					else {
						if ( set == 1 ) {
							z1 = 0;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , readrcp , z1 , 512 );
							if ( z2 == z1 ) {
								//
								strcpy( rcporgname , "||" );
								strcat( rcporgname , rcpname );
								//
							}
							else {
								//
								strcpy( rcporgname , readrcp );
								//
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , readrcp , z1 , 512 );
								if ( z2 == z1 ) {
									//
									strcat( rcporgname , "||" );
									strcat( rcporgname , rcpname );
									//
								}
								else {
									strcat( rcporgname , "|" );
									strcat( rcporgname , readrcp );
									strcat( rcporgname , "|" );
									strcat( rcporgname , rcpname );
								}
							}
							//
							_SCH_CLUSTER_Set_PathProcessData_JustPost( s , p , i , k , rcporgname );
							//
							LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
							//
							return TRUE;
							//
						}
						else { // 2011.01.28
							//
							z1 = 0;
							z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , readrcp , z1 , 512 );
							if ( z2 != z1 ) {
								z1 = z2;
								z2 = STR_SEPERATE_CHAR_WITH_POINTER( _SCH_CLUSTER_Get_PathProcessRecipe( s , p , i , k , SCHEDULER_PROCESS_RECIPE_OUT ) , '|' , readrcp , z1 , 512 );
								if ( z2 != z1 ) {
									if ( readrcp[0] != 0 ) {
										//
										LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
										//
										return TRUE;
									}
								}
							}
							//
						}
					}
				}
				else {
					//
					LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
					//
					return TRUE;
					//
				}
			}
		}
		//
	}
	//
	LeaveCriticalSection( &CR_A0SUB4_LOTPRE_SKIP ); // 2011.03.17
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_A0SUB4_Append_Another_Has_LotPreModule( int skipside , int skippt , int Chamber ) {
	int s , p , i , k , pr , pd , supid;
	//
	supid = _SCH_CLUSTER_Get_SupplyID( skipside , skippt );
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( ( skipside == s ) && ( skippt == p ) ) continue; // 2011.01.25
			//
			pr = _SCH_CLUSTER_Get_PathRange( s , p );
			if ( pr <= 0 ) continue;
			//
			pd = _SCH_CLUSTER_Get_PathStatus( s , p );
			//
			if ( pd == SCHEDULER_READY )  continue;
			if ( pd >= SCHEDULER_BM_END ) continue;
			//
			if ( pd == SCHEDULER_SUPPLY ) { // 2011.01.31
				if ( supid < _SCH_CLUSTER_Get_SupplyID( s , p ) ) continue;
			}
			//
			pd = _SCH_CLUSTER_Get_PathDo( s , p ) - 1;
			if ( pd < 0 ) pd = 0;
			//
			//
			for ( i = pd ; i < pr ; i++ ) {
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					if ( Chamber == _SCH_CLUSTER_Get_PathProcessChamber( s , p , i , k ) ) return TRUE;
					//
				}
			}
			//
		}
		//
	}
	//
	return FALSE;
}


//BOOL SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( int side , int pt ); // 2011.01.26 // 2015.06.25
BOOL SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( int side , int pt , BOOL force ); // 2011.01.26 // 2015.06.25

int SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_Diff_Remap( int os , int oc , int *op , int ns , int nc , int *np , BOOL OneS_Check , int difflogskip ) { // 2010.12.17
	char rcpname_pre[1024];
	char rcpname_post[1024];
	//
	int i , Chamber , sp;
	//
	int Has_OldRun[MAX_CHAMBER];
	int Has_NewRun[MAX_CHAMBER];
	int Has_LotPost[MAX_CHAMBER];
	int Has_LotPre[MAX_CHAMBER];
	int Has_LotPre2 , Has_LotPost2 , AllDiff;

	if ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() < 4 ) return -99;

	if ( os < 0 ) return -1;
	//
	*op = -1;
	*np = -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( os , i ) <= 0 ) continue;
		if ( oc == _SCH_CLUSTER_Get_ClusterIndex( os , i ) ) *op = i;
	}
	//
	if ( *op == -1 ) return -3;
	//
	if ( ns >= 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( ns , i ) <= 0 ) continue;
			if ( nc == _SCH_CLUSTER_Get_ClusterIndex( ns , i ) ) {
				*np = i;
				break;
			}
		}
		//
		if ( *np == -1 ) return -4;
	}
	//
	if ( OneS_Check ) { // 2011.01.26
		//
		if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-NOPASS\t[S=%d][P=%d]\n" , ns , *np );
//------------------------------------------------------------------------------------------------------------------
		}
		//
//		if ( SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( ns , *np ) ) { // 2015.06.25
		if ( SCHEDULER_CONTROL_1S_REJECT_NOPASING_CHECK( ns , *np , FALSE ) ) { // 2015.06.25
			//
			if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-NOPASS\tREJECTED\n" );
//------------------------------------------------------------------------------------------------------------------
			}
			//
			return 2;
		}
	}
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		Has_LotPre2 = 0;
		Has_LotPost2 = 0;
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
			//
			Has_LotPost[Chamber] = 0;
			//
			if ( os >= 0 ) {
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( os , Chamber ) > MUF_00_NOTUSE ) {
					//
					if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( os , Chamber , oc , rcpname_post ) ) {
						//
						if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POSTC\t[PM%d][%d,%d][%s]\n" , Chamber - PM1 + 1 , os , *op , rcpname_post );
	//------------------------------------------------------------------------------------------------------------------
						}
						//
						Has_LotPost[Chamber] = 1;
						//
						if ( SCHEDULER_CONTROL_LOTPREPOST_APPEND( 2 , 0 , os , *op , Chamber , rcpname_post ) ) {
							//
							if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POSTC2\t[PM%d][%d,%d][%s]\n" , Chamber - PM1 + 1 , os , *op , rcpname_post );
	//------------------------------------------------------------------------------------------------------------------
							}
							//
							Has_LotPost[Chamber] = 2;
							//
							Has_LotPost2++;
							//
						}
						//
					}
				}
			}
			//
			Has_LotPre[Chamber]  = 0;
			//
			if ( ns >= 0 ) { // 2009.07.16
				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( ns , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( ns , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
					//
					if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( FALSE , ns , Chamber , nc , rcpname_pre , rcpname_post , 0 ) ) {
						//
						if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PREC \t[PM%d][OLD=%d,%d][NEW=%d,%d][%s][%s][i=%d]\n" , Chamber - PM1 + 1 , os , *op , ns , *np , rcpname_pre , rcpname_post , i );
	//------------------------------------------------------------------------------------------------------------------
						}
						//
						Has_LotPre[Chamber] = 1;
						//
						if ( !SCHEDULER_CONTROL_LOTPREPOST_APPEND( 2 , 0 , os , *op , Chamber , rcpname_pre ) ) {
							//
							if ( SCHEDULER_CONTROL_A0SUB4_Append_Another_Has_LotPreModule( ns , *np , Chamber ) ) {
								//
								if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PREC1\t[PM%d][%d,%d][%s][%s][i=%d]\n" , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post , i );
	//------------------------------------------------------------------------------------------------------------------
								}
								//
								Has_LotPre[Chamber] = 2;
								//
								Has_LotPre2++;
							}
							else {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) {
									//
									if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PREC2\t[PM%d][%d,%d][%s][%s][i=%d]\n" , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post , i );
	//------------------------------------------------------------------------------------------------------------------
									}
									//
									Has_LotPre[Chamber] = 2;
									//
									Has_LotPre2++;
								}
								else {
									//
									if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PREC3\t[PM%d][%d,%d][%s][%s][i=%d][Has_LotPre2=%d]\n" , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post , i , Has_LotPre2 );
	//------------------------------------------------------------------------------------------------------------------
									}
									//
								}
							}
							//
						}
						else {
							if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PREC4\t[PM%d][LP=%d][%d,%d][%s][%s][i=%d]\n" , Chamber - PM1 + 1 , Has_LotPost[Chamber] , ns , *np , rcpname_pre , rcpname_post , i );
	//------------------------------------------------------------------------------------------------------------------
							}
							//
							Has_LotPre[Chamber] = 2;
							//
							Has_LotPre2++;
						}
						//
					}
					//
				}
			}
			//
		}
		//
		if ( ( Has_LotPost2 > 0 ) || ( Has_LotPre2 > 0 ) || ( difflogskip == 0 ) ) {
			break;
		}
		//
	}
	//
//	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
//		if ( ( Has_LotPost[Chamber] != 0 ) && ( Has_LotPre[Chamber] != 0 ) ) return -5;
//	}
	//
/*
	Has_LotPost2 = 0; // 2011.01.28
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( os , Chamber ) > MUF_00_NOTUSE ) {
			//
			if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( os , Chamber , oc , rcpname_post ) ) {
				//
				Has_LotPost2++; // 2011.01.28
				//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST \t[PM%d][%d,%d][%s]\n" , Chamber - PM1 + 1 , os , *op , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
				//
				SCHEDULER_CONTROL_LOTPREPOST_APPEND( 0 , 1 , os , *op , Chamber , rcpname_post );
				//
				SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , os , Chamber , oc , "" );
				//
			}
		}
		//
	}
	*/
	//
	/*
	if ( Has_LotPost2 > 0 ) { // 2011.01.28
		if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST-SKIP 1 \t[%d][OLD=%d,%d]\n" , Has_LotPost2 , os , *op );
//------------------------------------------------------------------------------------------------------------------
		}
		return 2;
	}
	else { // 2011.01.28
		if ( SCHEDULER_CONTROL_LOTPREPOST_APPEND( 2 , -1 , os , *op , -1 , "LOTPOSTCHECK" ) ) { // 2010.12.17
			if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST-SKIP 2 \t[%d][OLD=%d,%d]\n" , Has_LotPost2 , os , *op );
//------------------------------------------------------------------------------------------------------------------
			}
			return 2; // 2011.01.28
		}
	}
	*/
	//

	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Has_OldRun[i] = 0;
		Has_NewRun[i] = 0;
	}
	//
	if ( os >= 0 ) {
		//
		for ( i = 0 ; i < _SCH_CLUSTER_Get_PathRange( os , *op ) ; i++ ) {
			//
			for ( sp = 0 ; sp < MAX_PM_CHAMBER_DEPTH ; sp++ ) {
				//
				Chamber = _SCH_CLUSTER_Get_PathProcessChamber( os , *op , i , sp );
				//
				if ( Chamber > 0 ) {
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) ) {
						Has_OldRun[Chamber]++;
					}
				}
				//
			}
		}
	}
	if ( ns >= 0 ) {
		//
		for ( i = 0 ; i < _SCH_CLUSTER_Get_PathRange( ns , *np ) ; i++ ) {
			//
			for ( sp = 0 ; sp < MAX_PM_CHAMBER_DEPTH ; sp++ ) {
				//
				Chamber = _SCH_CLUSTER_Get_PathProcessChamber( ns , *np , i , sp );
				//
				if ( Chamber > 0 ) {
					if ( ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) && ( _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Chamber ) < 2 ) ) {
						Has_NewRun[Chamber]++;
					}
				}
				//
			}
		}
	}
	//
	AllDiff = TRUE;
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) { // 2011.01.18
//		if ( ( Has_LotPost[Chamber] > 0 ) && ( ( Has_LotPre[Chamber] > 0 ) || ( Has_NewRun[Chamber] > 0 ) ) ) {
		if ( ( Has_OldRun[Chamber] > 0 ) && ( Has_NewRun[Chamber] > 0 ) ) {
			AllDiff = FALSE;
			break;
		}
	}
	//
	if ( ns < 0 ) AllDiff = FALSE; // 2011.02.22
	//
	if ( AllDiff ) {
		//
		Has_LotPost2 = 0; // 2011.01.28
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( os , Chamber ) > MUF_00_NOTUSE ) {
				//
				if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( os , Chamber , oc , rcpname_post ) ) {
					//
					Has_LotPost2++; // 2011.01.28
					//
					if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST \t[PM%d][%d,%d][%s]\n" , Chamber - PM1 + 1 , os , *op , rcpname_post );
	//------------------------------------------------------------------------------------------------------------------
					}
					//
					SCHEDULER_CONTROL_LOTPREPOST_APPEND( 0 , 1 , os , *op , Chamber , rcpname_post );
					//
					SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , os , Chamber , oc , "" );
					//
				}
				//
			}
			//
		}
		//
	}
	else {
		//
		switch ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() % 4 ) {
		case 1 :
		case 3 :
			//
//			if ( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( os , *op , -1 , -1 ) != 1 ) return 3; // 2015.01.22
			if ( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , os , *op , -1 , -1 ) != 1 ) return 3; // 2015.01.22
			//
			break;
		}
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) { // 2011.01.18
			if ( Has_LotPost[Chamber] == 2 ) {
				if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE-SKIP 1 \t[PM%d][OLD=%d,%d]\n" , Chamber - PM1 + 1 , os , *op );
	//------------------------------------------------------------------------------------------------------------------
				}
				return 4;
			}
			//
		}
		//
		Has_LotPost2 = 0;
		//
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE = os; // 2011.01.25
		xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT   = *op; // 2011.01.25
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
			//
			xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[Chamber-PM1] = 0; // 2011.01.25
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( os , Chamber ) > MUF_00_NOTUSE ) {
				//
				if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( os , Chamber , oc , rcpname_post ) ) {
					//
					Has_LotPost2++; // 2011.01.28
					//
					if ( difflogskip == 0 ) {
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST \t[PM%d][%d,%d][%s]\n" , Chamber - PM1 + 1 , os , *op , rcpname_post );
	//------------------------------------------------------------------------------------------------------------------
					}
					//
					xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[Chamber-PM1] = 1; // 2011.01.25
					//
					SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( os , Chamber , oc , rcpname_post , 12 ); // 2012.07.26
					//
					_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( os ,
						Chamber ,
						os + 1 , 0 , 0 ,
						rcpname_post ,
						12 ,
						0 , "" , 0 ,
						1 , 1498 );
					//
					SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , os , Chamber , oc , "" );
					//
				}
			}
			//
		}
	}
	//
//	if ( ns < 0 ) return 0; // 2011.02.22
	//
	if ( Has_LotPost2 > 0 ) { // 2011.01.28
		if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-POST-SKIP 1 \t[%d][OLD=%d,%d]\n" , Has_LotPost2 , os , *op );
//------------------------------------------------------------------------------------------------------------------
		}
		return 5;
	}
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) { // 2011.01.18
		//
		if ( Has_LotPre[Chamber] == 2 ) {
			if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE-SKIP 2 \t[PM%d][NEW=%d,%d]\n" , Chamber - PM1 + 1 , ns , *np );
//------------------------------------------------------------------------------------------------------------------
			}
			return 6;
		}
	}
	//
	sp = 0;
	//
	xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE = ns; // 2011.01.25
	xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT   = *np; // 2011.01.25
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[Chamber-PM1] = 0; // 2011.01.25
		//
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( ns , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( ns , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
			//
			if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( TRUE , ns , Chamber , nc , rcpname_pre , rcpname_post , 0 ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE  \t[RUN][lpmd=%d][PM%d][%d,%d][%s][%s]\n" , Has_LotPre[Chamber] , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
				//
				if ( !SCHEDULER_CONTROL_LOTPREPOST_APPEND( 2 , 1 , os , *op , Chamber , rcpname_pre ) ) {
					//
					if ( SCHEDULER_CONTROL_A0SUB4_Append_Another_Has_LotPreModule( ns , *np , Chamber ) ) {
						//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE1 \t[RUN][lpmd=%d][PM%d][%d,%d][%s][%s]\n" , Has_LotPre[Chamber] , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_LOTPREPOST_APPEND( 1 , 1 , ns , *np , Chamber , rcpname_pre );
					}
					else {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) > 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE2 \t[RUN][lpmd=%d][PM%d][%d,%d][%s][%s]\n" , Has_LotPre[Chamber] , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
							//
							SCHEDULER_CONTROL_LOTPREPOST_APPEND( 1 , 1 , ns , *np , Chamber , rcpname_pre );
						}
						else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE3 \t[RUN][lpmd=%d][PM%d][%d,%d][%s][%s]\n" , Has_LotPre[Chamber] , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
							//
							xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[Chamber-PM1] = 1; // 2011.01.25
							//
							SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( ns , Chamber , nc , rcpname_pre , 11 ); // 2012.07.26
							//
							_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( ns ,
								Chamber ,
								ns + 1 , 0 , 0 ,
								rcpname_pre ,
								11 ,
								0 , "" , 0 ,
								1 , 1499 );
							//
							sp++;
							//
						}
					}
					//
				}
				else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-PRE4 \t[RUN][lpmd=%d][PM%d][%d,%d][%s][%s]\n" , Has_LotPre[Chamber] , Chamber - PM1 + 1 , ns , *np , rcpname_pre , rcpname_post );
//------------------------------------------------------------------------------------------------------------------
				}
				//
			}
			//
			SCHEDULER_CONTROL_LOTPOST_REGIST( 0 , ns , Chamber , nc , rcpname_post );
			//
		}
		//
	}
	//
	if ( sp == 0 ) return 0;
	//
	switch ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() % 4 ) {
	case 1 :
	case 3 :
		return 1;
		break;
	}
	//
	return 0;
}

int SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_NotMixedCheck( int side , int clindx , int newside , int newclindx ) {
	int Chamber;
	char rcpname[256];
	char rcpname2[256];
	BOOL lpost = FALSE;
	//
//	return FALSE; // Testing 2011.01.27
	//
	switch ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() % 4 ) {
	case 1 :
	case 3 :
		return FALSE;
		break;
	}
	//
	if ( ( side < 0 ) || ( newside < 0 ) ) return FALSE;
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
			if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( side , Chamber , clindx , rcpname ) ) {
				//
				lpost = TRUE;
				//
				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
					if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( FALSE , newside , Chamber , newclindx , rcpname , rcpname2 , 0 ) ) {
						return FALSE;
					}
				}
			}
		}
	}
	//
	return lpost;
}

int SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_YesCheck( int side , int clindx , int newside , int newclindx , int ch ) {
	int Chamber;
	char rcpname[256];
	char rcpname2[256];
	int lpost , lpre; // 2009.06.11
	//
	lpost = FALSE; // 2009.06.11
	lpre  = FALSE; // 2009.06.11
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( ( ch != -1 ) && ( ch != Chamber ) ) continue; // 2011.04.29
		//
		if ( side >= 0 ) { // 2009.07.16
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
//				if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( side , Chamber , clindx , rcpname ) ) return TRUE; // 2009.06.11
				if ( SCHEDULER_CONTROL_LOTPOST_EXTRACT( side , Chamber , clindx , rcpname ) ) lpost = TRUE; // 2009.06.11
			}
		}
		//
		if ( newside >= 0 ) { // 2009.07.16
			if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
//				if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( FALSE , newside , Chamber , newclindx , rcpname , rcpname2 , FALSE ) ) return TRUE; // 2009.06.11
				if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( FALSE , newside , Chamber , newclindx , rcpname , rcpname2 , 0 ) ) lpre = TRUE; // 2009.06.11
			}
		}
		//
		if ( lpost && lpre ) return 3; // 2009.06.11
		//
	}
//	return FALSE; // 2009.06.11
	if      (  lpost &&  lpre ) return 3; // 2009.06.11
	else if (  lpost && !lpre ) return 1; // 2009.06.11
	else if ( !lpost &&  lpre ) return 2; // 2009.06.11
	return 0; // 2009.06.11
}

int SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_Disable_to_Enable( int oldside , int oldclindx , int side , int clindx ) { // 2011.04.01
	int Chamber;
	char rcpname_pre[256];
	char rcpname_post[256];
	//
	if ( ( oldside < 0 ) || ( side < 0 ) ) return TRUE;
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( oldside , Chamber ) == MUF_99_USE_to_DISABLE ) {
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) {

				if ( SCHEDULER_CONTROL_LOTPRE_EXTRACT( FALSE , side , Chamber , clindx , rcpname_pre , rcpname_post , 0 ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOT_PRE_Disable_to_Enable\t[PM%d][S=%d][P=%d][C=%d][R=%s]\n" , Chamber - PM1 + 1 , side , -1 , clindx , rcpname_pre );
//------------------------------------------------------------------------------------------------------------------
					//
					SCHEDULER_CONTROL_EXTRACT_RECIPE_COPY( side , Chamber , clindx , rcpname_pre , 11 ); // 2012.07.26
					//
					_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( side ,
						Chamber ,
						side + 1 , 0 , 0 ,
						rcpname_pre ,
						11 ,
						0 , "" , 0 ,
						1 , 1405 );
				}
			}
		}
	}
	//
	return TRUE;
}

void SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_MakeSkip( int side , int clindx , int newside , int newclindx , int mode , BOOL res ) {
	int Chamber;
	char rcpname[256];
	char rcpname2[256];
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
		if ( side >= 0 ) { // 2009.07.16
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
				if ( ( mode == 0 ) || ( mode == 2 ) ) SCHEDULER_CONTROL_LOTPOST_REMOVE( side , Chamber , clindx );
			}
		}
		//
		if ( newside >= 0 ) { // 2009.07.16
			if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( newside , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
				if ( ( mode == 0 ) || ( mode == 1 ) ) {
					//
					strcpy( rcpname , "" ); // 2011.02.22
					strcpy( rcpname2 , "" ); // 2011.03.17
					//
					SCHEDULER_CONTROL_LOTPRE_EXTRACT( TRUE , newside , Chamber , newclindx , rcpname , rcpname2 , res ? -2 : -1 );
					//
					if ( rcpname[0] != 0 ) SCHEDULER_CONTROL_LOTPRE_SKIP_APPEND( newside , Chamber , newclindx , rcpname ); // 2011.02.22
					//
					if ( !res ) SCHEDULER_CONTROL_LOTPOST_REGIST( 1 , newside , Chamber , newclindx , rcpname2 );
				}
			}
		}
	}
}


int SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_SkipCheck( int LotPost_side , int LotPost_clidx , int LotPre_side , int LotPre_clidx , int lpres , int pos ) { // 2009.06.11
	int nlpres , mode , res , LotPost_pt , LotPre_pt;
	CLUSTERStepTemplate4 LotPost_CLUSTER_INFO4;
	CLUSTERStepTemplate4 LotPre_CLUSTER_INFO4;
	//
	if ( lpres == 0 ) return FALSE;
	//
	if ( !USER_EXIST_SP_N0S4_LOTPOSTPRESKIP() ) return TRUE;
	//
	if ( LotPost_side < 0 ) {
		LotPost_pt = -1;
	}
	else {
		LotPost_pt = SCHEDULER_CONTROL_LOTPREPOST_GET_PT( LotPost_side , LotPost_clidx );
	}
	//
	if ( LotPre_side < 0 ) {
		LotPre_pt = -1;
	}
	else {
		LotPre_pt = SCHEDULER_CONTROL_LOTPREPOST_GET_PT( LotPre_side , LotPre_clidx );
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPRESKIP\t[OS=%d][OC=%d][OP=%d][S=%d][C=%d][P=%d][L=%d][POS=%d]\n" , LotPost_side , LotPost_clidx , LotPost_pt , LotPre_side , LotPre_clidx , LotPre_pt , lpres , pos );
//------------------------------------------------------------------------------------------------------------------
	if ( ( LotPost_pt == -1 ) && ( LotPre_pt == -1 ) ) {
		nlpres = USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( LotPost_side , LotPost_clidx , LotPre_side , LotPre_clidx , lpres ,
			NULL , 0 , "" , "" , "" , "" ,
			NULL , 0 , "" , "" , "" , ""
			);
	}
	else if ( LotPre_pt == -1 ) {
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( LotPost_side , LotPost_pt , &G_CLUSTER_INFO_A0SUB4_LPOST , &LotPost_CLUSTER_INFO4 );
		//
		nlpres = USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( LotPost_side , LotPost_clidx , LotPre_side , LotPre_clidx , lpres ,
			&G_CLUSTER_INFO_A0SUB4_LPOST , LotPost_CLUSTER_INFO4.LOT_SPECIAL_DATA , LotPost_CLUSTER_INFO4.LOT_USER_DATA , LotPost_CLUSTER_INFO4.CLUSTER_USER_DATA , LotPost_CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( LotPost_side ) ,
			NULL , 0 , "" , "" , "" , ""
			);
	}
	else if ( LotPost_pt == -1 ) {
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( LotPre_side  , LotPre_pt  , &G_CLUSTER_INFO_A0SUB4_LPRE , &LotPre_CLUSTER_INFO4 );
		//
		nlpres = USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( LotPost_side , LotPost_clidx , LotPre_side , LotPre_clidx , lpres ,
			NULL , 0 , "" , "" , "" , "" ,
			&G_CLUSTER_INFO_A0SUB4_LPRE , LotPre_CLUSTER_INFO4.LOT_SPECIAL_DATA  , LotPre_CLUSTER_INFO4.LOT_USER_DATA  , LotPre_CLUSTER_INFO4.CLUSTER_USER_DATA  , LotPre_CLUSTER_INFO4.RECIPE_TUNE_DATA  , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( LotPre_side )
			);
	}
	else {
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( LotPost_side , LotPost_pt , &G_CLUSTER_INFO_A0SUB4_LPOST   , &LotPost_CLUSTER_INFO4 );
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( LotPre_side  , LotPre_pt  , &G_CLUSTER_INFO_A0SUB4_LPRE , &LotPre_CLUSTER_INFO4 );
		//
		nlpres = USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( LotPost_side , LotPost_clidx , LotPre_side , LotPre_clidx , lpres ,
			&G_CLUSTER_INFO_A0SUB4_LPOST , LotPost_CLUSTER_INFO4.LOT_SPECIAL_DATA , LotPost_CLUSTER_INFO4.LOT_USER_DATA , LotPost_CLUSTER_INFO4.CLUSTER_USER_DATA , LotPost_CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( LotPost_side ) ,
			&G_CLUSTER_INFO_A0SUB4_LPRE  , LotPre_CLUSTER_INFO4.LOT_SPECIAL_DATA  , LotPre_CLUSTER_INFO4.LOT_USER_DATA  , LotPre_CLUSTER_INFO4.CLUSTER_USER_DATA  , LotPre_CLUSTER_INFO4.RECIPE_TUNE_DATA  , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( LotPre_side )
			);
	}
	//
	mode = -1;
	res = TRUE;
	//
	if      ( nlpres == 2 ) { // pre
		if      ( lpres == 3 ) { res = TRUE; mode = 2; } 
		else if ( lpres == 2 ) { res = TRUE; mode = -1; } 
		else                   { res = FALSE; mode = 0; } 
	}
	else if ( nlpres == 1 ) { // post
		if      ( lpres == 3 ) { res = TRUE; mode = 1; } 
		else if ( lpres == 1 ) { res = TRUE; mode = -1; } 
		else                   { res = FALSE; mode = 0; } 
	}
	else if ( nlpres != 3 ) { // x
		res = FALSE;
		mode = 0;
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPRESKIP\t[nlpres=%d][mode=%d][res=%d][POS=%d]\n" , nlpres , mode , res , pos );
//------------------------------------------------------------------------------------------------------------------
	if ( mode != -1 ) SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_MakeSkip( LotPost_side , LotPost_clidx , LotPre_side , LotPre_clidx , mode , res );
	return res;
}


BOOL SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( int side , int clindx , int newside , int newclindx , int *diff , int pos , BOOL OneS_Check , int difflogskip ) { // 2009.06.28
	int lpres2 , op , np;
	//
	*diff = 0; // 2010.12.17
	//
	if ( difflogskip == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-START\t[os,oc=%d,%d][ns,nc=%d,%d][pos=%d][1S=%d][DFL=%d]\n" , side , clindx , newside , newclindx , pos , OneS_Check , difflogskip );
//------------------------------------------------------------------------------------------------------------------
	}
	//
	lpres2 = SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_Diff_Remap( side , clindx , &op , newside , newclindx , &np , OneS_Check , difflogskip ); // 2010.12.17
	//
	if      ( lpres2 == 0 ) { // 2010.12.17
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-END\t[Res=%d][os,op=%d,%d(%d)][ns,np=%d,%d(%d)][1S=%d][DFL=%d]\n" , lpres2 , side , op , clindx , newside , np , newclindx , OneS_Check , difflogskip );
//------------------------------------------------------------------------------------------------------------------
		*diff = 1;
		return FALSE;
	}
	else if ( lpres2 == 1 ) { // 2011.01.18
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-END\t[Res=%d][os,op=%d,%d(%d)][ns,np=%d,%d(%d)][1S=%d][DFL=%d]\n" , lpres2 , side , op , clindx , newside , np , newclindx , OneS_Check , difflogskip );
//------------------------------------------------------------------------------------------------------------------
		*diff = 2;
		return FALSE;
	}
	else if ( ( lpres2 >= 2 ) && ( lpres2 <= 9 ) ) { // 2011.01.19
		if ( difflogskip != lpres2 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-END\t[Res=%d][os,op=%d,%d(%d)][ns,np=%d,%d(%d)][1S=%d][DFL=%d]\n" , lpres2 , side , op , clindx , newside , np , newclindx , OneS_Check , difflogskip );
//------------------------------------------------------------------------------------------------------------------
		}
		//
		*diff = 1 - lpres2;
		//
		return FALSE;
	}
	else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-LOTPOSTPREDIFF-END\t[Res=%d][os,op=%d,%d(%d)][ns,np=%d,%d(%d)][1S=%d][DFL=%d]\n" , lpres2 , side , op , clindx , newside , np , newclindx , OneS_Check , difflogskip );
//------------------------------------------------------------------------------------------------------------------
	}
	//
	lpres2 = SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_YesCheck( side , clindx , newside , newclindx , -1 );
	//
	if ( lpres2 == 0 ) {
		if ( side < 0 ) return TRUE; // 2009.07.22
		return FALSE;
	}
	//
	return SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_SkipCheck( side , clindx , newside , newclindx , lpres2 , pos ); // 2009.06.11
}



void SCHEDULER_CONTROL_A0SUB4_PreMain_Check( int side , int clidx , int pos ) {
	int Chamber;
	//
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
			if ( SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 0 , side , Chamber , -1 , clidx , FALSE ) ) {
				if ( USER_RECIPE_SP_N0S4_PREMAINSKIP( side , Chamber , clidx ) ) {
					xinside_A0SUB4_PREMAINSKIP[side] = TRUE; // 2008.11.22
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PREMAINSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[TRUE]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 0 , side , Chamber , -1 , clidx , TRUE );
				}
				else {
					xinside_A0SUB4_PREMAINSKIP[side] = FALSE; // 2008.11.22
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PREMAINSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[FALSE]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
				}
			}
		}
	}
}


void SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( int side , int clidx , int pos ) {
	int Chamber , mode , res;
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
			//
			if ( SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 1 , side , Chamber , -1 , clidx , FALSE ) ) {
				if ( SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 2 , side , Chamber , -1 , clidx , FALSE ) ) {
					mode = 3;
				}
				else {
					mode = 1;
				}
			}
			else {
				if ( SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 2 , side , Chamber , -1 , clidx , FALSE ) ) {
					mode = 2;
				}
				else {
					mode = 0;
				}
			}
			//
			if ( mode != 0 ) {
				//
				res = USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP( side , Chamber , clidx , mode );
				//
				if      ( res == 3 ) { // all skip
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-WFRPREPOSTSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[3]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
					if ( ( mode == 1 ) || ( mode == 3 ) ) {
						xinside_A0SUB4_WFRPRESKIP[side] = TRUE; // 2009.06.28
						SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 1 , side , Chamber , -1 , clidx , TRUE );
					}
					else {
						xinside_A0SUB4_WFRPRESKIP[side] = FALSE; // 2009.06.28
					}
					if ( ( mode == 2 ) || ( mode == 3 ) ) {
						xinside_A0SUB4_WFRPOSTSKIP[side] = TRUE; // 2009.06.28
						SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 2 , side , Chamber , -1 , clidx , TRUE );
					}
					else {
						xinside_A0SUB4_WFRPOSTSKIP[side] = FALSE; // 2009.06.28
					}
				}
				else if ( res == 2 ) { // post skip
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-WFRPREPOSTSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[2]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
					if ( ( mode == 2 ) || ( mode == 3 ) ) {
						xinside_A0SUB4_WFRPOSTSKIP[side] = TRUE; // 2009.06.28
						SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 2 , side , Chamber , -1 , clidx , TRUE );
					}
					else {
						xinside_A0SUB4_WFRPOSTSKIP[side] = FALSE; // 2009.06.28
					}
				}
				else if ( res == 1 ) { // pre skip
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-WFRPREPOSTSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[1]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
					if ( ( mode == 1 ) || ( mode == 3 ) ) {
						xinside_A0SUB4_WFRPRESKIP[side] = TRUE; // 2009.06.28
						SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 1 , side , Chamber , -1 , clidx , TRUE );
					}
					else {
						xinside_A0SUB4_WFRPRESKIP[side] = FALSE; // 2009.06.28
					}
				}
				else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-WFRPREPOSTSKIP\t[PM%d][S=%d][P=%d][C=%d][POS=%d]=>[0]\n" , Chamber - PM1 + 1 , side , -1 , clidx , pos );
//------------------------------------------------------------------------------------------------------------------
				}
			}
		}
	}
}


//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
int SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_RUN( BOOL premode , int side , int pt0 , int clidx , int opt ) {
	int pt , aside, aclidx;
//	CLUSTERStepTemplate2 CLUSTER_INFO; // 2008.07.24
	CLUSTERStepTemplate4 CLUSTER_INFO4;
	//
	if ( pt0 == -1 ) { // 2008.07.12
		pt = SCHEDULER_CONTROL_LOTPREPOST_GET_PT( side , clidx );
	} // 2008.07.12
	else { // 2008.07.12
		pt = pt0; // 2008.07.12
	} // 2008.07.12
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-USER_FUNCTION_RUN\t[P=%d][S=%d][P=%d][C=%d][O=%d]\n" , premode , side , pt , clidx , opt );
//------------------------------------------------------------------------------------------------------------------
	if ( pt == -1 ) {
		return USER_RECIPE_SP_N0S4_USERFUNCTION( TRUE , premode , side , pt , clidx , NULL , SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Act_A0SUB4( &aside , &aclidx ) , "" , "" , "" , "" );
	}
	else {
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( side , pt , &G_CLUSTER_INFO_A0SUB4_USER , &CLUSTER_INFO4 );
		return USER_RECIPE_SP_N0S4_USERFUNCTION( TRUE , premode , side , pt , clidx , &G_CLUSTER_INFO_A0SUB4_USER , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) );
	}
}

int SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( BOOL premode , int side , int clidx , int pos ) {
	int aside , aclidx , res;
	res = USER_RECIPE_SP_N0S4_USERFUNCTION( FALSE , premode , side , -1 , clidx , NULL , SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Act_A0SUB4( &aside , &aclidx ) , "" , "" , "" , "" );
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-USER_FUNCTION_STS\t[P=%d][S=%d][P=%d][C=%d][R=%d][POS=%d]\n" , premode , side , -1 , clidx , res , pos );
//------------------------------------------------------------------------------------------------------------------
	return res;
}

//=======================================================================================
//=======================================================================================
BOOL SCHEDULER_CONTROL_WAIT_USER_PRE_FUNCTION_A0SUB4( int side , int clidx ) { // 2007.11.20
	int res;
	//
	while ( TRUE ) {
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return FALSE;
		}
		//================================================================================================
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( TRUE , side , clidx , 101 );
		if ( res == SYS_SUCCESS ) {
			return TRUE;
		}
		if ( res == SYS_ABORTED ) {
			return FALSE;
		}
		if ( res == SYS_ERROR   ) {
			break;
		}
		Sleep(250);
	}
	return FALSE;
}

//=======================================================================================
//=======================================================================================
BOOL SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( int side , int pt , int clidx , int opt ) {
	int res;
	//
	SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , clidx , 101 ); // 2007.03.07
	//
	SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , clidx , 101 ); // 2009.06.11
	//
	_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function Start (%d:%d:%d:%d)%cWHPMUSERPRESTART %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
	//
	while ( TRUE ) {
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_RUN( TRUE , side , pt , clidx , opt );
		if ( res != SYS_SUCCESS ) {
			_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function Fail (%d:%d:%d:%d)%cWHPMUSERPREFAIL %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
			return FALSE;
		}
		while ( TRUE ) {
			//================================================================================================
			// 2007.10.23
			//================================================================================================
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function Aborted(%d:%d:%d:%d)%cWHPMUSERPREABORTED %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return FALSE;
			}
			//================================================================================================
			res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( TRUE , side , clidx , 102 );
			if ( res == SYS_SUCCESS ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function Success (%d:%d:%d:%d)%cWHPMUSERPRESUCCESS %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return TRUE;
			}
			if ( res == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function Abort (%d:%d:%d:%d)%cWHPMUSERPREABORT %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return FALSE;
			}
			if ( res == SYS_ERROR   ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Pre Function ReStart (%d:%d:%d:%d)%cWHPMUSERPRERESTART %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				break;
			}
			Sleep(250);
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( int side , int pt , int clidx , int rside , int opt ) {
	int res;
	//
	_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function Start (%d:%d:%d:%d)%cWHPMUSERPOSTSTART %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
	//
	while ( TRUE ) {
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_RUN( FALSE , side , pt , clidx , opt );
		if ( res != SYS_SUCCESS ) {
			_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function Fail (%d:%d:%d:%d)%cWHPMUSERPOSTFAIL %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
			return FALSE;
		}
		while ( TRUE ) {
			//================================================================================================
			// 2007.10.23
			//================================================================================================
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( rside ) ) || ( _SCH_SYSTEM_USING_GET( rside ) <= 0 ) ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function Aborted (%d:%d:%d:%d)%cWHPMUSERPOSTABORTED %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return FALSE;
			}
			//================================================================================================
			res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( FALSE , side , clidx , 103 );
			if ( res == SYS_SUCCESS ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function Success (%d:%d:%d:%d)%cWHPMUSERPOSTSUCCESS %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return TRUE;
			}
			if ( res == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function Abort (%d:%d:%d:%d)%cWHPMUSERPOSTABORT %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				return FALSE;
			}
			if ( res == SYS_ERROR   ) {
				_SCH_LOG_LOT_PRINTF( side , "PM Scheduling User Post Function ReStart (%d:%d:%d:%d)%cWHPMUSERPOSTRESTART %d:%d:%d:%d\n" , side , pt , clidx , opt , 9 , side , pt , clidx , opt ); // 2007.10.23
				break;
			}
			Sleep(250);
		}
	}
	return FALSE;
}

//=======================================================================================
BOOL SCHEDULER_CONTROL_FREE_CHECKA0SUB4( int currside , int fmmode ); // 2010.09.07
//=======================================================================================
int SCHEDULER_CONTROL_END_CHECK_SUB_A0SUB4_SUB( int side , int mode , int old_side , int old_clidx , int old_order ) {
	int dt;
	BOOL diff;

	//============================================================================================================
	// 2007.11.20
	//============================================================================================================
	SCHEDULER_CONTROL_WAIT_USER_PRE_FUNCTION_A0SUB4( old_side , old_clidx );
	//============================================================================================================
	while( TRUE ) {
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return -1;
		}
		// Lot Pre End Check of old_side
//		dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ); // 2015.01.22
		dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( TRUE , old_side , -1 , old_clidx , -1 ); // 2015.01.22
		if      ( dt ==  1 ) break;
		else if ( dt == -1 ) break;
		Sleep(1);
	}

//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] LOTPOST\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
	SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( side , -1 );
	//
	if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( old_side , old_clidx , -1 , -1 , &diff , 101 , FALSE , 0 ) ) { // 2009.07.16
		//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] LOTPOST2\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
		SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , FALSE , mode , 101 );
		//
		while( TRUE ) {
			//================================================================================================
			if ( !_SCH_SYSTEM_USING_STARTING_ONLY( old_side ) ) { // 2010.03.10
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] LOTPOST-CHECKSKIP\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
				return 1;
			}
			//================================================================================================
			// 2007.10.23
			//================================================================================================
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
				return -1;
			}
			//================================================================================================
			// Lot Post End Check of old_side
//			dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ); // 2015.01.22
			dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( TRUE , old_side , -1 , old_clidx , -1 ); // 2015.01.22
			if      ( dt ==  1 ) break;
			else if ( dt == -1 ) return -1;
			Sleep(1);
		}
		//
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] USERPOST\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
	//====================================================================
	// User Post Function (old_side,old_clidx)
	//====================================================================
	EnterCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2008.07.24
	//
	if ( !SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( old_side , -1 , old_clidx , side , 1001 ) ) {
		LeaveCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2008.07.24
		return -1;
	}
	//
	LeaveCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2008.07.24
	//====================================================================
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] END\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
	return 0;
}


BOOL SCHEDULER_CONTROL_END_CHECK_SUB_A0SUB4( int side , int mode , int old_side , int old_clidx , int old_order ) {
	int Res;
	//
	xinside_A0SUB4_ENDPART2_RUN[old_side] = 1; // 2010.02.23

	Res = SCHEDULER_CONTROL_END_CHECK_SUB_A0SUB4_SUB( side , mode , old_side , old_clidx , old_order );
	
	switch( Res ) { // 2010.03.10
	case 0 :
		xinside_A0SUB4_ENDPART2_RUN[old_side] = 0; // 2010.02.23
		return TRUE;
	case 1 :
		xinside_A0SUB4_ENDPART2_RUN[old_side] = 2; // 2010.03.10
		return TRUE;
	default :
		xinside_A0SUB4_ENDPART2_RUN[old_side] = 0;
		return FALSE;
	}
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_CHECK_HAS_LOT_POST( int side , int ch ) {
	int old_clidx , old_side , old_order;
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-CHECK_HAS_LOT_POST\t[S=%d][C=%d] CHECKING\n" , side , ch );
//------------------------------------------------------------------------------------------------------------------
	old_clidx = SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Index_A0SUB4( &old_side , &old_order );
	//
	if ( old_clidx == -1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-CHECK_HAS_LOT_POST\t[S=%d][C=%d] RETURN 1\n" , side , ch );
//------------------------------------------------------------------------------------------------------------------
		return FALSE;
	}
	//
	if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_YesCheck( side , old_clidx , -1 , -1 , ch ) == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-CHECK_HAS_LOT_POST\t[S=%d] RETURN 2\n" , side );
//------------------------------------------------------------------------------------------------------------------
		return FALSE;
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-CHECK_HAS_LOT_POST\t[S=%d] RETURN 3\n" , side );
//------------------------------------------------------------------------------------------------------------------
	return TRUE;
}

// New 202 Trouble




BOOL SCHEDULER_CONTROL_CHECK_RUNNING_LOT_POST( int side , int ch ) { // 2010.10.20
	int i , Chamber;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
			//
			if ( ( ch != -1 ) && ( ch != Chamber ) ) continue; // 2011.04.29
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) > MUF_00_NOTUSE ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( Chamber ) == PROCESS_INDICATOR_POST ) return TRUE;
			}
		}
		//
		if ( i == 0 ) Sleep(100);
	}
	return FALSE;
}



int SCHEDULER_CONTROL_MULTIRUN_END_WAITING_POST_A0SUB4( int side , int ch ) { // 2010.10.05
	int dt;
	int old_clidx , old_side , old_order;
	//
	//------------------------------------------------------------------------------------------------------------------
	if ( !SCHEDULER_CONTROL_CHECK_HAS_LOT_POST( side , ch ) ) {
		//
		if ( !SCHEDULER_CONTROL_CHECK_RUNNING_LOT_POST( side , ch ) ) { // 2010.10.20
			//
			return 1;
			//
		}
		//
	}
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][C=%d] MULTIRUN_END_WAITING_POST\n" , side , ch );
	//------------------------------------------------------------------------------------------------------------------
	//
	if ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() >= 4 ) { // 2011.02.22
		Sleep(250);
	}
	else {
		Sleep(1000);
	}
	//
	//------------------------------------------------------------------------------------------------------------------
	old_clidx = SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Index_A0SUB4( &old_side , &old_order );
	//============================================================================================================
	SCHEDULER_CONTROL_WAIT_USER_PRE_FUNCTION_A0SUB4( old_side , old_clidx );
	//============================================================================================================
	if ( _SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() >= 4 ) { // 2011.02.22
		return 0;
	}
	//============================================================================================================
	while( TRUE ) {
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return -1;
		}
		// Lot Pre End Check of old_side
//		dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , ch ); // 2015.01.22
		dt = SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( TRUE , old_side , -1 , old_clidx , ch ); // 2015.01.22
		if      ( dt ==  1 ) break;
		else if ( dt == -1 ) return -1;
		//
		Sleep(100);
	}
	//============================================================================================================
	return 0;
}




BOOL SCHEDULER_CONTROL_END_WAIT_A0SUB4( int side , int mode , int ch ) { // 2010.09.14
	int i , s = -1 , x;
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] CHECKING\n" , side , mode , ch );
//------------------------------------------------------------------------------------------------------------------
	if ( !SCHEDULER_CONTROL_CHECK_HAS_LOT_POST( side , ch ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] RETURN 1\n" , side , mode , ch );
//------------------------------------------------------------------------------------------------------------------
		return TRUE;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i == side ) continue;
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( _SCH_SYSTEM_RUNORDER_GET( i ) > _SCH_SYSTEM_RUNORDER_GET( side ) ) continue;
		//
		if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( _SCH_MODULE_Chk_FM_Free_Status( i ) ) continue;
		}
		else {
			if ( _SCH_MODULE_Chk_TM_Free_Status( i ) ) continue;
		}
		//
		s = i;
		//
		break;
	}
	//
	if ( s == -1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] RETURN 3-1\n" , side , mode , ch );
//------------------------------------------------------------------------------------------------------------------
		if ( !_SCH_SYSTEM_USING_STARTING_ONLY( side ) ) { // 2010.10.25
			SCHEDULER_CONTROL_MULTIRUN_END_WAITING_POST_A0SUB4( side , ch );
		}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] RETURN 3-2\n" , side , mode , ch );
//------------------------------------------------------------------------------------------------------------------
		return TRUE;
	}
	//
	for ( i = PM1 ; i < AL ; i++ ) {
		//
		if ( ( ch != -1 ) && ( ch != i ) ) continue; // 2011.04.29
		//
		x = _SCH_MODULE_Get_Mdl_Use_Flag( side , i );
		if ( ( x == MUF_01_USE ) || ( x == MUF_81_USE_to_PREND_RANDONLY ) || ( x >= MUF_90_USE_to_XDEC_FROM ) ) {
			if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
				//
//printf( "[%d,6:%d]\n" , side , i );
//				_SCH_MODULE_Set_Mdl_Use_Flag( s , i , MUF_99_USE_to_DISABLE ); // 2010.09.20
				_SCH_MODULE_Set_Mdl_Use_Flag( s , i , x ); // 2010.09.20
				_SCH_MODULE_Inc_Mdl_Run_Flag( i );
				//
				xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE  = s; // 2010.09.20
				xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER = xinside_A0SUB4_LOT_ORDER_INDEX[s];  // 2010.09.20
				//
				xinside_A0SUB4_LOTPOST_ORGSIDE[ i - PM1 ] = side; // 2010.09.26
				//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] SETTING [S=%d][O=%d]\n" , side , mode , ch , xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE , xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER );
//------------------------------------------------------------------------------------------------------------------
			}
		}
	}
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_WAIT_CM\t[S=%d][M=%d][C=%d] RETURN 4\n" , side , mode , ch );
//------------------------------------------------------------------------------------------------------------------
	return TRUE;
}


BOOL SCHEDULER_CONTROL_END_CHECK_A0SUB4( int side , int mode ) { // 2010.02.23
	int res;
	int old_clidx , old_side , old_order;
	//
//	if ( !_SCH_SYSTEM_USING_STARTING_ONLY( side ) ) return TRUE; // 2008.05.20
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d] CHECKING\n" , side , mode );
//------------------------------------------------------------------------------------------------------------------
	while ( TRUE ) { // 2008.05.21
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) {
			return FALSE;
		}
		if ( !_SCH_SYSTEM_USING_STARTING_ONLY( side ) ) {
/*
// 2010.03.14
			if ( !xinside_A0SUB4_ENDPART_RUN[side] ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d] RETURN 1\n" , side , mode );
//------------------------------------------------------------------------------------------------------------------
				return TRUE;
			}
			//
			if ( mode == 1 ) { // 2008.07.12
//				_SCH_SYSTEM_LEAVE_TM_CRITICAL( 0 ); // 2008.07.18
				//
//				_SCH_SYSTEM_ENTER_TM_CRITICAL( 0 ); // 2008.07.18
				//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d] RETURN 2\n" , side , mode );
//------------------------------------------------------------------------------------------------------------------
				return TRUE; // 2008.07.18
			}
			else {

			}
			//
*/

			SCHEDULER_CONTROL_MULTIRUN_END_WAITING_POST_A0SUB4( side , -1 ); // 2010.10.05

			return TRUE;
		}
		else {
			break;
		}
		Sleep(1); // 2008.05.20
	}
	//
	old_clidx = SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Index_A0SUB4( &old_side , &old_order );
	//
	if ( old_clidx == -1  ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d] RETURN 4\n" , side , mode );
//------------------------------------------------------------------------------------------------------------------
		return TRUE;
	}
	if ( old_side != side ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d][OS=%d] RETURN 5\n" , side , mode , old_side );
//------------------------------------------------------------------------------------------------------------------
		return TRUE;
	}
	//
	if ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[ side ] ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][M=%d][OD=%d,%d] RETURN 6\n" , side , mode , old_order , xinside_A0SUB4_LOT_ORDER_INDEX[ side ] );
//------------------------------------------------------------------------------------------------------------------
		return TRUE; // 2008.04.17
	}
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END_PART_CM\t[S=%d][OS=%d][OC=%d][OO=%d] STARTING\n" , side , old_side , old_clidx , old_order );
//------------------------------------------------------------------------------------------------------------------
	//
	res = SCHEDULER_CONTROL_END_CHECK_SUB_A0SUB4( side , mode , old_side , old_clidx , old_order );
	//
	return res;
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_END2_WAIT( int side , BOOL pre ) { // 2010.02.23
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
		if ( pre ) { // 2010.02.23
			if ( xinside_A0SUB4_ENDPART2_RUN[side] == 2 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-END2_WAIT\t[S=%d]PRE\n" , side );
//------------------------------------------------------------------------------------------------------------------
				xinside_A0SUB4_ENDPART2_RUN[side] = 0;
				return TRUE; // 2010.03.10
			}
		}
		else {
			if ( xinside_A0SUB4_ENDPART2_RUN[side] == 1 ) return TRUE;
		}
	}
	return FALSE;
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_FREE_CHECKA0SUB4( int currside , int fmmode ) { // 2010.09.07
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
//		if ( currside != i ) { // 2010.10.05
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
				if ( fmmode ) {
					if ( !_SCH_MODULE_Chk_FM_Free_Status( i ) ) return FALSE;
				}
				else {
					if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return FALSE;
				}
			}
//		} // 2010.10.05
	}
	return TRUE;
}
//
int Last_1s_Mode = FALSE;
//=======================================================================================
int SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4_SUB( int side , int pt , int mode0 ) {
	//================================================================================================
	int old_clidx , old_side , old_order , res , cur_clidx , rtag , act_data , act_side , act_clidx , i , mode;
	BOOL diff , specnewpre1s;
	//================================================================================================
//	CLUSTERStepTemplate2 CLUSTER_INFO; // 2008.07.24
	//================================================================================================
	CLUSTERStepTemplate4 CLUSTER_INFO4;
	//================================================================================================
	if ( mode0 == -1 ) { // 2011.01.27
		mode = -1;
		specnewpre1s = Last_1s_Mode;
	}
	else if ( mode0 == 10 ) { // 2011.01.27
		mode = 0;
		specnewpre1s = TRUE;
		Last_1s_Mode = TRUE;
	}
	else {
		mode = mode0;
		specnewpre1s = FALSE;
		Last_1s_Mode = FALSE;
	}
	//
	SCHEDULER_DUMMY_WAFER_PRE_POST_CUT( side , pt ); // 2010.01.26
	//
	old_clidx = SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Index_A0SUB4( &old_side , &old_order );
	act_data  = SCHEDULER_CONTROL_Get_Last_GlobalLot_Cluster_Act_A0SUB4( &act_side , &act_clidx ); // 2007.11.05
	cur_clidx = _SCH_CLUSTER_Get_ClusterIndex( side , pt );
	//================================================================================================
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FM  A0SUB4 START [pt=%d,mode=%d,cur_clidx=%d][old:clidx=%d,side=%d,order=%d][act:data=%d,side=%d,clidx=%d]\n" , pt , mode , cur_clidx , old_clidx , old_side , old_order , act_data , act_side , act_clidx );
	//----------------------------------------------------------------------
	if ( mode == 3 ) {
		//
		xinside_A0SUB4_PICKFROMCM_RUN[side] = FALSE; // 2008.04.17
		//
		SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( side , pt , &G_CLUSTER_INFO_A0SUB4 , &CLUSTER_INFO4 );
//		switch( USER_RECIPE_SP_N0S4_PICKFROMCM( FALSE , side , pt , old_clidx , &CLUSTER_INFO , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) { // 2008.04.25
//		switch( USER_RECIPE_SP_N0S4_PICKFROMCM( ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , old_clidx , &CLUSTER_INFO , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) { // 2008.05.16
		switch( USER_RECIPE_SP_N0S4_PICKFROMCM( ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx , &G_CLUSTER_INFO_A0SUB4 , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) { // 2008.05.16
		case 0 : // Reject
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[REJECT:0:-121]\n" , ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
			return -121;
			break;
		case 1 : // Separate(Lot Post & Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-122]\n" , ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
			xinside_A0SUB4_PICKFROMCM_RUN[side]		= TRUE; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_PT[side]		= pt; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_RESULT[side]	= 1; // 2008.04.17
			return -122;
			break;
		case 2 : // Separate(Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-123]\n" , ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
			xinside_A0SUB4_PICKFROMCM_RUN[side]		= TRUE; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_PT[side]		= pt; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_RESULT[side]	= 2; // 2008.04.17
			return -123;
			break;
		case 3 : // Separate(Lot Post - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-124]\n" , ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
			xinside_A0SUB4_PICKFROMCM_RUN[side]		= TRUE; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_PT[side]		= pt; // 2008.04.17
			xinside_A0SUB4_PICKFROMCM_RESULT[side]	= 3; // 2008.04.17
			return -124;
			break;
		default : // continue
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[CONTINUE:121]\n" , ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
			if ( ( old_side != side ) || ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) { // 2008.04.25
				//
				SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 102 );// 2008.04.25
				//
				SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 102 ); // 2009.06.11
				//
				if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 2 , ( mode == 1 ) , 101 ) == -1 ) { // 2008.04.25
					_SCH_SYSTEM_MODE_ABORT_SET( side );
					return -125;
				}
				//
			}
			return 121;
			break;
		}
	}
	//================================================================================================
	if ( old_clidx == -1 ) {
		// First Start
		if ( act_data == 0 ) {
//			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( side , -1 , -1 , -1 ) ) { // 2015.01.22
			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , side , -1 , -1 , -1 ) ) { // 2015.01.22
			case 1 : // finish
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 5 , -1 , -1 ); // Goto User Pre
				return -1;
				break;
			case 0 :
				return -2;
				break;
			case -1 :
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				//
				return -3;
				break;
			}
			return -4;
		}
		else if ( act_data == 5 ) { // Run User Pre & Lot Pre
			//====================================================================
			// Lot Pre Function (side,-1)
			//====================================================================
//			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , pt , -1 , 101 ) ) { // 2008.05.28
			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , pt , cur_clidx , 101 ) ) { // 2008.05.28
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -5;
			}
			//====================================================================
			if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( -1 , -1 , side , cur_clidx , &diff , 102 , specnewpre1s , 0 ) ) { // 2009.07.16
//				if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , -1 , 0 , ( mode == 1 ) ) == -1 ) { // 2008.05.28
				if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 0 , ( mode == 1 ) , 102 ) == -1 ) { // 2008.05.28
					_SCH_SYSTEM_MODE_ABORT_SET( side );
					return -6;
				}
			}
			//
			if ( xinside_A0SUB4_LOT_PRE_CHECK[side] == 1 ) { // 2008.05.29
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d]:(%d,%d)=>Goto = 18 , Res = -7\n" , side , pt , cur_clidx , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
				//====================================================================
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 18 , side , cur_clidx ); // Goto Idle
				//====================================================================
				return -7; // 2008.05.29
			}
			else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d]:(%d,%d)=>Goto = 19 , Res = 1\n" , side , pt , cur_clidx , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
				//====================================================================
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 19 , side , cur_clidx ); // Goto Idle
				//====================================================================
				return 1; // 2008.05.29
			}
//			return 1; // 2008.05.29
		}
		else {
			return 2;
		}
	}
	//================================================================================================
	if ( act_data == 0 ) {
//		if ( old_side == side ) { // 2008.04.25
		//===========================================================================================
		if ( mode == 1 ) return 999; // 2008.05.29
		//===========================================================================================
		if ( SCHEDULER_CONTROL_END2_WAIT( old_side , TRUE ) ) { // 2010.03.10
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 3 , -1 , -1 ); // Goto Lot Post Check
			return -198;
		}
		//===========================================================================================
		res = SCHEDULER_CONTROL_A0SUB4_1S_Properly_RunCheck( old_side , old_clidx , side , cur_clidx , mode ); // 2009.10.14
		if ( res < 0 ) { // 2009.10.14
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d]:(%d,%d)=>Res = -10 (%d)\n" , side , pt , cur_clidx , old_side , old_clidx , res );
//------------------------------------------------------------------------------------------------------------------
			return -10;
		}
		//
		if ( ( old_side == side ) && ( old_order == xinside_A0SUB4_LOT_ORDER_INDEX[side] ) ) { // 2008.04.25
			if ( old_clidx != cur_clidx ) { // 2008.02.29 // 2008.03.19
//			if ( old_clidx < cur_clidx ) { // 2008.02.29 // 2008.03.19
				//
				if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( old_side , old_clidx , side , cur_clidx , &diff , 103 , specnewpre1s , xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d]:(%d,%d)=>Goto = 1 , Res = -11\n" , side , pt , cur_clidx , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_NotMixedCheck( old_side , old_clidx , side , cur_clidx ) ) { // 2011.01.27
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 2 , -1 , -1 ); // Goto Lot Post
					}
					else {
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post
					}
					return -11;
				}
				else {
//					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 43 , -1 , -1 ); // Goto User Post // 2010.01.17 // 2010.02.10 // 2010.03.17
					if      ( diff == 1 ) { // 2010.12.17
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -12\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 31 , -1 , -1 ); // Goto User Post // 2010.12.17
					}
					else if ( diff == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -12\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 21 , -1 , -1 ); // Goto User Post // 2010.01.17 // 2010.02.10 // 2010.03.17
					}
					else if ( diff == 2 ) { // 2011.01.18
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -12\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 41 , -1 , -1 ); // Goto User Post // 2011.01.18
					}
					else {
						if ( xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -12\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						}
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 1 - diff; // 2011.01.27
						//
					}
					//
					return -12;
				}
			}
			else { // 2008.02.29 // 2008.03.19
//			else if ( old_clidx == cur_clidx ) { // 2008.02.29 // 2008.03.19
				//===========================================================================================
//				if ( mode == 1 ) return 11; // 2007.10.04 // 2008.05.29
				//===========================================================================================
				// 2008.03.26
				//===========================================================================================
				if ( ( xinside_A0SUB4_PICKFROMCM_RUN[side] ) && ( xinside_A0SUB4_PICKFROMCM_PT[side] == pt ) ) { // 2008.04.17
					//
					xinside_A0SUB4_PICKFROMCM_RUN[side] = FALSE;
					//
					switch( xinside_A0SUB4_PICKFROMCM_RESULT[side] ) {
					case 1 : // Separate(Lot Post & Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-22]\n" , 11 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 11 , -1 , -1 ); // Goto Lot Post
						return -22;
						break;
					case 2 : // Separate(Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-23]\n" , 14 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
//						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 13 , -1 , -1 ); // Goto User Post // 2008.05.16
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto Lot PreF // 2008.05.16
						return -23;
						break;
					case 3 : // Separate(Lot Post - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-24]\n" , 15 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 15 , -1 , -1 ); // Goto Lot Post
						return -24;
						break;
					}
				}
				//===========================================================================================
				SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( side , pt , &G_CLUSTER_INFO_A0SUB4 , &CLUSTER_INFO4 );
				switch( USER_RECIPE_SP_N0S4_PICKFROMCM( FALSE , side , pt , cur_clidx , &G_CLUSTER_INFO_A0SUB4 , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) {
				case 0 : // Reject
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[REJECT:0:-21]\n" , FALSE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
					return -21;
					break;
				case 1 : // Separate(Lot Post & Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-22]\n" , FALSE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 11 , -1 , -1 ); // Goto Lot Post
					return -22;
					break;
				case 2 : // Separate(Lot Pre - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-23]\n" , FALSE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
//					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 13 , -1 , -1 ); // Goto User Post // 2008.05.16
					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto Lot PreF // 2008.05.16
					return -23;
					break;
				case 3 : // Separate(Lot Post - force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-24]\n" , FALSE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 15 , -1 , -1 ); // Goto Lot Post
					return -24;
					break;
				default : // OK
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[CONTINUE:21]\n" , FALSE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto Finish
					//
//					if ( old_order != xinside_A0SUB4_LOT_ORDER_INDEX[side] ) { // 2008.04.17 // 2008.04.25
//						SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx ); // 2007.04.17 // 2008.04.25
//					} // 2008.04.25
					//
					return 21;
					break;
				}
			}
//			else { // 2008.02.29 // 2008.03.19
//				return 12; // 2008.03.19
//			} // 2008.03.19
		}
		else {
			if ( old_clidx != cur_clidx ) { // 2008.02.29 // 2008.03.19
				//
				if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( old_side , old_clidx , side , cur_clidx , &diff , 104 , specnewpre1s , xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d]:(%d,%d)=>Goto = 1 , Res = -31\n" , side , pt , cur_clidx , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_NotMixedCheck( old_side , old_clidx , side , cur_clidx ) ) { // 2011.01.27
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 2 , -1 , -1 ); // Goto Lot Post // 2007.11.05
					}
					else {
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post // 2007.11.05
					}
					//
					_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 ); // 2018.01.16
					//
					return -31; // 2007.11.05
				} // 2007.11.05
				else { // 2007.11.05
//					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 43 , -1 , -1 ); // Goto User Post // 2010.01.19 // 2010.02.10 // 2010.03.17
					if      ( diff == 1 ) { // 2010.12.17
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -32\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 31 , -1 , -1 ); // Goto User Post 2010.12.17
					}
					else if ( diff == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -32\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 21 , -1 , -1 ); // Goto User Post // 2010.01.19 // 2010.02.10 // 2010.03.17
					}
					else if ( diff == 2 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -32\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 0; // 2011.01.27
						//
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 41 , -1 , -1 ); // Goto User Post // 2011.01.18
					}
					else {
						if ( xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-GOTO_ACTRUN\t[S=%d][P=%d][C=%d][diff=%d]:(%d,%d)=>Goto = 21,31,41 , Res = -32\n" , side , pt , cur_clidx , diff , old_side , old_clidx );
//------------------------------------------------------------------------------------------------------------------
						}
						//
						xinside_A0SUB4_LOT_PRE_DIFF_LOG_SKIP = 1 - diff; // 2011.01.27
						//
					}
					//
					_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 ); // 2018.01.16
					//
					return -32; // 2007.11.05
				} // 2007.11.05
			}
			else { // 2008.02.29 // 2008.03.19
//			else if ( old_clidx == cur_clidx ) { // 2008.02.29 // 2008.03.19
//				if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_RunCheck( old_side , old_clidx , side , cur_clidx ) ) { // 2009.07.28
				if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_YesCheck( old_side , old_clidx , side , cur_clidx , -1 ) != 0 ) { // 2009.07.28
					//
//					if ( mode == 1 ) return 41; // 2007.10.04 // 2008.05.29
					//
					//===========================================================================================
					// 2008.03.26
					//===========================================================================================
					if ( ( xinside_A0SUB4_PICKFROMCM_RUN[side] ) && ( xinside_A0SUB4_PICKFROMCM_PT[side] == pt ) ) {
						//
						xinside_A0SUB4_PICKFROMCM_RUN[side] = FALSE;
						//
						switch( xinside_A0SUB4_PICKFROMCM_RESULT[side] ) {
						case 1 : // Separate(Lot Post & Lot Pre - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-42]\n" , 1 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_NotMixedCheck( old_side , old_clidx , side , cur_clidx ) ) { // 2011.01.27
								SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 2 , -1 , -1 ); // Goto Lot Post
							}
							else {
								SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post
							}
							return -42;
							break;
						case 2 : // Separate(Lot Pre - Force) // 2008.05.16
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-43]\n" , 14 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto Lot PreF
							//
							if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 1 , 2 , ( mode == 1 ) , 103 ) == -1 ) { // 2008.05.23
								_SCH_SYSTEM_MODE_ABORT_SET( side );
								return -143;
							}
							return -43;
							break;
						case 3 : // Separate(Lot Post - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-44]\n" , 6 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 6 , -1 , -1 ); // Goto Lot Post
							return -44;
							break;
						}
					}
					//===========================================================================================
					SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( side , pt , &G_CLUSTER_INFO_A0SUB4 , &CLUSTER_INFO4 );
					switch( USER_RECIPE_SP_N0S4_PICKFROMCM( TRUE , side , pt , cur_clidx , &G_CLUSTER_INFO_A0SUB4 , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) {
					case 0 : // Reject
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[REJECT:0:-41]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						return -41;
						break;
					case 1 : // Separate(Lot Post & Lot Pre - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-42]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_NotMixedCheck( old_side , old_clidx , side , cur_clidx ) ) { // 2011.01.27
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 2 , -1 , -1 ); // Goto Lot Post
						}
						else {
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post
						}
						return -42;
						break;
					case 2 : // Separate(Lot Pre - Force) // 2008.05.16
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-43]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto User Post // 2008.05.16
						//
						if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 1 , 2 , ( mode == 1 ) , 104 ) == -1 ) { // 2008.05.23
							_SCH_SYSTEM_MODE_ABORT_SET( side );
							return -143;
						}
						return -43; // 2008.05.16
						break; // 2008.05.16
					case 3 : // Separate(Lot Post - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-44]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 6 , -1 , -1 ); // Goto Lot Post
						return -44;
						break;
					default : // Continue
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[CONTINUE:-45]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
//						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 21 , -1 , -1 ); // Goto User Post // 2008.05.22
//						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 41 , -1 , -1 ); // Goto User Post // 2008.05.22 // 2011.01.18
//						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 81 , -1 , -1 ); // Goto User Post // 2008.05.22 // 2011.01.18 // 2011.04.01

						if ( !SCHEDULER_CONTROL_A0SUB4_LotPost_LotPre_Disable_to_Enable( old_side , old_clidx , side , cur_clidx ) ) { // 2011.04.01
							_SCH_SYSTEM_MODE_ABORT_SET( side );
							return -145;
						}
						else {
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 81 , -1 , -1 ); // Goto User Post // 2008.05.22 // 2011.01.18
						}
						return -45;
						break;
					}
				}
				else {
					//===========================================================================================
					// 2008.05.20
					//===========================================================================================
					if ( mode == 1 ) return 42;
					//===========================================================================================
					if ( ( xinside_A0SUB4_PICKFROMCM_RUN[side] ) && ( xinside_A0SUB4_PICKFROMCM_PT[side] == pt ) ) {
						//
						xinside_A0SUB4_PICKFROMCM_RUN[side] = FALSE;
						//
						switch( xinside_A0SUB4_PICKFROMCM_RESULT[side] ) {
						case 1 : // Separate(Lot Post & Lot Pre - Normal)
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-52]\n" , 1 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							return -52;
							break;
						case 2 : // Separate(Lot Pre - Force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPRE:2:-53]\n" , 14 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto Lot PreF
							//
							if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 1 , 2 , ( mode == 1 ) , 105 ) == -1 ) { // 2008.05.23
								_SCH_SYSTEM_MODE_ABORT_SET( side );
								return -153;
							}
							return -53;
							break;
						case 3 : // Separate(Lot Post - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM[X]\t[A=%d][S=%d][P=%d][C=%d]=>[LOTPOST:3:-54]\n" , 6 , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
							SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 6 , -1 , -1 ); // Goto Lot Post
							return -54;
							break;
						}
					}
					//===========================================================================================
					SCHEDULER_CONTROL_RunData_Make_Verification_Info_A0SUB4( side , pt , &G_CLUSTER_INFO_A0SUB4 , &CLUSTER_INFO4 );
					switch( USER_RECIPE_SP_N0S4_PICKFROMCM( TRUE , side , pt , cur_clidx , &G_CLUSTER_INFO_A0SUB4 , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ) ) {
					case 0 : // Reject
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[REJECT:0:-51]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						return -51;
						break;
					case 1 : // Separate(Lot Post & Lot Pre - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST&LOTPRE:1:-52]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 1 , -1 , -1 ); // Goto Lot Post
						return -52;
						break;
					case 2 : // Separate(Lot Pre - Force)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPRE:1:-53]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto Lot PreF
						//
						if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 1 , 2 , ( mode == 1 ) , 106 ) == -1 ) { // 2008.05.23
							_SCH_SYSTEM_MODE_ABORT_SET( side );
							return -153;
						}
						return -53;
						break;
					case 3 : // Separate(Lot Post - Normal)
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[LOTPOST:1:-54]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 6 , -1 , -1 ); // Goto Lot Post
						return -54;
						break;
					default : // Continue
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "A0S4-PICKFROM_CM\t[D=%d][S=%d][P=%d][C=%d]=>[CONTINUE:55]\n" , TRUE , side , pt , cur_clidx );
//------------------------------------------------------------------------------------------------------------------
						SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 103 );// 2008.05.23
						//
						SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 103 ); // 2009.06.11
						//
						return 55;
						break;
					}
					//===========================================================================================
				}
			}
//			else { // 2008.02.29 // 2008.03.19
//				return 42; // 2008.03.19
//			} // 2008.03.19
		}
	}
	else if ( act_data == 18 ) { // 2008.05.29
		//====================================================================
		SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 19 , side , cur_clidx ); // Goto Idle
		//====================================================================
		return 51;
	}
	else if ( act_data == 19 ) { // 2007.11.05
		if ( ( act_side != -1 ) && ( act_clidx != -1 ) ) {
			if ( ( act_side != side ) || ( act_clidx != cur_clidx ) ) {
				SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( act_side , act_clidx );
				return -51;
			}
		}
	}
	//======================================================================================================
	if ( mode == 1 ) return 61; // 2007.10.04
	//======================================================================================================
	//======================================================================================================
	rtag = FALSE;
	if ( mode == -1 ) { // 2009.12.26
		//
		// 2010.09.07
//		if ( _SCH_PRM_GET_MODULE_MODE( FM ) ) {
//			if ( _SCH_MODULE_Chk_FM_Free_Status( old_side ) ) rtag = TRUE;
//		}
//		else {
//			if ( _SCH_MODULE_Chk_TM_Free_Status( old_side ) ) rtag = TRUE;
//		}
		if ( SCHEDULER_CONTROL_FREE_CHECKA0SUB4( side , _SCH_PRM_GET_MODULE_MODE( FM ) ) ) rtag = TRUE; // 2010.09.07
		//
		if ( rtag ) return 1001;
		return -1001;
	}
	else {
//		if ( ( mode == 0 ) && _SCH_MODULE_Chk_FM_Free_Status( old_side ) ) rtag = TRUE; // 2010.09.07
//		if ( ( mode != 0 ) && _SCH_MODULE_Chk_TM_Free_Status( old_side ) ) rtag = TRUE; // 2010.09.07
		if ( ( mode == 0 ) && SCHEDULER_CONTROL_FREE_CHECKA0SUB4( side , TRUE ) ) rtag = TRUE; // 2010.09.07
		if ( ( mode != 0 ) && SCHEDULER_CONTROL_FREE_CHECKA0SUB4( side , FALSE ) ) rtag = TRUE; // 2010.09.07
	}
	//
	if ( SCHEDULER_CONTROL_END2_WAIT( old_side , FALSE ) ) return -199; // 2010.02.23
	//
	if ( rtag ) {
		//===========================================================================
		// Current Lot Before Group Finish & Next Group Start
		//===========================================================================
		switch( act_data ) {
		case 1 : // Run Lot Post
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 3 , -1 , -1 ); // Goto Lot Post End Check
			//
			SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , FALSE , mode , 102 );
			//
			return -71;
			break;

		case 2 : // Run Lot Post but No Wait 2011.01.27 
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 4 , -1 , -1 ); // Run User Post
			//
			SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , FALSE , mode , 102 );
			//
			return -71;
			break;


		case 3 : // Goto Lot Post End Check
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
//			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			case 1 :
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 4 , -1 , -1 ); // Goto User Post
				break;
			case -1 :
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				break;
			}
			return -72;
			break;
		case 4 : // Run User Post
			//
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			//====================================================================
			// User Post Function (old_side,old_clidx)
			//====================================================================
			if ( !SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( old_side , -1 , old_clidx , side , 1002 ) ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -73;
			}
			//
			xinside_A0SUB4_ENDPART_RUN[old_side] = FALSE; // 2008.05.21
			//
			//====================================================================
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 5 , -1 , -1 ); // Goto User Pre & Lot Pre
			//====================================================================
			return -74;
			break;
		case 5 : // Run User Pre & Lot Pre
			//====================================================================
			// User Pre Function (side,cur_clidx)
			//====================================================================
			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , pt , cur_clidx , 102 ) ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -75;
			}
			//====================================================================
			// Lot Pre of side
			//====================================================================
			if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 0 , ( mode == 1 ) , 107 ) == -1 ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -175;
			}
			//====================================================================
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto Finish
			//
			return -76;
			break;

		case 6 : // Run Lot Post
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 7 , -1 , -1 ); // Goto Lot Post End Check
			//
			SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , FALSE , mode , 103 );
			//
			return -77;
			break;
		case 7 : // Goto Lot Post End Check
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
//			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			case 1 :
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 8 , -1 , -1 ); // Goto User Post
				break;
			case -1 :
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				break;
			}
			return -78;
			break;
		case 8 : // Run User Post & User Pre
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			//====================================================================
			// User Post Function (old_side,old_clidx)
			//====================================================================
			if ( !SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( old_side , -1 , old_clidx , side , 1003 ) ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -79;
			}
			xinside_A0SUB4_ENDPART_RUN[old_side] = FALSE; // 2008.05.21
			//
			//-----------------------------------------------------------------------
			// 2009.07.28
			//-----------------------------------------------------------------------
			SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 104 ); // 2009.07.28
			//
			SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 104 ); // 2009.07.28
			//
			if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 4 , ( mode == 1 ) , 108 ) == -1 ) { // 2009.07.28
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -180;
			}
			//-----------------------------------------------------------------------

			//====================================================================
			// User Pre Function (side,cur_clidx)
			//====================================================================
			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , pt , cur_clidx , 103 ) ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -80;
			}
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto Finish
			//
			return -81;
			break;
		//==========================================================================================================================================
		//==========================================================================================================================================
		//==========================================================================================================================================
		case 11 : // Run Lot Post(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 12 , -1 , -1 ); // Goto Lot Post End Check(force)
			//
			SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , TRUE , mode , 104 );
			//
			return -82;
			break;
		case 12 : // Goto Lot Post End Check(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
//			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			case 1 :
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 13 , -1 , -1 ); // Goto User Post(force)
				break;
			case -1 :
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				break;
			}
			return -83;
			break;
		case 13 : // Run User Post(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			//====================================================================
			// User Post Function (old_side,old_clidx)
			//====================================================================
//			if ( !SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( old_side , old_clidx , side ) ) {
//				_SCH_SYSTEM_MODE_ABORT_SET( side );
//				return -84;
//			}
			xinside_A0SUB4_ENDPART_RUN[old_side] = FALSE; // 2008.05.21
			//
			//====================================================================
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 14 , -1 , -1 ); // Goto User Pre & Lot Pre(force)
			//====================================================================
			return -85;
			break;
		case 14 : // Run User Pre & Lot Pre(force)
			//====================================================================
			// User Pre Function (side,cur_clidx)
			//====================================================================
//			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , cur_clidx ) ) {
//				_SCH_SYSTEM_MODE_ABORT_SET( side );
//				return -86;
//			}
			//====================================================================
			SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 105 ); // 2007.05.23
			//
			SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 105 ); // 2009.06.11
			//====================================================================
			// Lot Pre of old_side
			//====================================================================
			if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 1 , 1 , ( mode == 1 ) , 109 ) == -1 ) {
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -86;
			}
			//====================================================================
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto Finish
			//
			return -87;
			break;
		case 15 : // Run Lot Post(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 16 , -1 , -1 ); // Goto Lot Post End Check(force)
			//
			SCHEDULER_CONTROL_A0SUB4_LotPost_Process_Start_Lot( old_side , old_clidx , TRUE , mode , 105 );
			//
			return -88;
			break;
		case 16 : // Goto Lot Post End Check(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
//			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , old_side , -1 , old_clidx , -1 ) ) { // 2015.01.22
			case 1 :
				SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 17 , -1 , -1 ); // Goto User Post(force)
				break;
			case -1 :
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				break;
			}
			return -89;
			break;
		case 17 : // Run User Post & User Pre(force)
			xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
			//
			//====================================================================
			// User Post Function (old_side,old_clidx)
			//====================================================================
//			if ( !SCHEDULER_CONTROL_CHECK_USER_POST_FUNCTION_A0SUB4( old_side , old_clidx , side ) ) {
//				_SCH_SYSTEM_MODE_ABORT_SET( side );
//				return -90;
//			}
			xinside_A0SUB4_ENDPART_RUN[old_side] = FALSE; // 2008.05.21
			//====================================================================
			// User Pre Function (side,cur_clidx)
			//====================================================================
//			if ( !SCHEDULER_CONTROL_CHECK_USER_PRE_FUNCTION_A0SUB4( side , cur_clidx ) ) {
//				_SCH_SYSTEM_MODE_ABORT_SET( side );
//				return -91;
//			}
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto Finish
			//
			return -92;
			break;
		}
	}
	//=====================================================================================
	switch( act_data ) {
	case 21 : // User Post
	case 31 : // User Post // 2010.12.17
	case 41 : // User Post // 2011.01.18
		xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
		//
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_RUN( FALSE , old_side , -1 , old_clidx , 2100 + act_data );
		if ( res != SYS_SUCCESS ) {
			_SCH_SYSTEM_MODE_ABORT_SET( side );
			return -101;
		}
		SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data + 1 , -1 , -1 ); // Goto User Post Check // 2010.12.17
		return -102;
		break;
	case 22 : // User Post Check
	case 32 : // User Post Check // 2010.12.17
	case 42 : // User Post Check // 2011.01.18
		xinside_A0SUB4_ENDPART_RUN[old_side] = TRUE; // 2008.05.21
		//
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( FALSE , old_side , old_clidx , 2200 + act_data );
		if      ( res == SYS_SUCCESS ) {
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data + 1 , -1 , -1 ); // Goto User Pre // 2010.12.17
			//
			xinside_A0SUB4_ENDPART_RUN[old_side] = FALSE; // 2008.05.21
			//
		}
		else if ( res == SYS_ABORTED ) {
			_SCH_SYSTEM_MODE_ABORT_SET( side );
		}
		else if ( res == SYS_ERROR   ) {
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data - 1 , -1 , -1 ); // Goto User Post // 2010.12.17
		}
		return -103;
		break;
	case 23 : // User Pre
	case 33 : // User Pre // 2010.12.17
	case 43 : // User Pre // 2011.01.18
		//
		SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 2300 + act_data ); // 2007.03.07 // 2008.04.25
		//
		SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 2400 + act_data ); // 2009.06.11
		//
		if ( act_data == 23 ) { // 2010.12.17
	//		if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 2 , ( mode == 1 ) , 110 ) == -1 ) { // 2007.11.20 // 2010.03.17
			if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 5 , ( mode == 1 ) , 2500 + act_data ) == -1 ) { // 2007.11.20 // 2010.03.17
				_SCH_SYSTEM_MODE_ABORT_SET( side );
				return -1104;
			}
		}
		//
//		SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx ); // 2007.03.07 // 2008.04.25
		//
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_RUN( TRUE , side , pt , cur_clidx , 2600 + act_data );
		if ( res != SYS_SUCCESS ) {
			_SCH_SYSTEM_MODE_ABORT_SET( side );
			return -104;
		}
		SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data + 1 , side , cur_clidx ); // Goto User Pre Check
		return -105;
		break;
	case 24 : // User Pre Check
	case 34 : // User Pre Check // 2010.12.17
	case 44 : // User Pre Check // 2011.01.18
		if ( ( act_side != -1 ) && ( act_clidx != -1 ) ) { // 2007.11.05
			if ( ( act_side != side ) || ( act_clidx != cur_clidx ) ) {
				res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( TRUE , act_side , act_clidx , 2700 + act_data );
				if      ( res == SYS_SUCCESS ) {
					SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( act_side , act_clidx );
				}
				else if ( res == SYS_ABORTED ) {
					_SCH_SYSTEM_MODE_ABORT_SET( side );
				}
				else if ( res == SYS_ERROR   ) {
					SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data - 1 , -1 , -1 ); // Goto User Pre
				}
				return -106;
			}
		}
		res = SCHEDULER_A0SUB4_USER_PREPOST_FUNCTION_STATUS( TRUE , side , cur_clidx , 2800 + act_data );
		if      ( res == SYS_SUCCESS ) {
			//------------------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "A0S4-USER_PRE_CHECK\t[S=%d][P=%d][C=%d][DIF=%d,%d]\n" , side , pt , cur_clidx , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT );
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) _IO_CONSOLE_PRINTF( "A0S4-USER_PRE_CHECK\t[PM%d=%d]\n" , i-PM1+1 , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[i-PM1] );
			//------------------------------------------------------------------------------------------------------------------
			//
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( ( act_data == 44 ) ? 45 : 99 , side , cur_clidx ); // Goto Finish
			//
		}
		else if ( res == SYS_ABORTED ) {
			_SCH_SYSTEM_MODE_ABORT_SET( side );
		}
		else if ( res == SYS_ERROR   ) {
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( act_data - 1 , -1 , -1 ); // Goto User Pre
		}
		return -107;
		break;

	case 45 : // Lot Pre EndCheck // 2011.01.18
		//
//		switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( side , pt , -2 , -1 ) ) { // 2015.01.22
		switch( SCHEDULER_CONTROL_A0SUB4_LotPrePost_EndCheck( FALSE , side , pt , -2 , -1 ) ) { // 2015.01.22
		case 1 :
			SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , -1 , -1 ); // Goto Finish
			break;
		case -1 :
			_SCH_SYSTEM_MODE_ABORT_SET( side );
			break;
		}
		return -108;
		break;

//	case 41 : // Lot Post / Lot Pre // 2008.05.22 // 2011.01.18
	case 81 : // Lot Post / Lot Pre // 2008.05.22 // 2011.01.18
		//
		SCHEDULER_CONTROL_A0SUB4_PreMain_Check( side , cur_clidx , 107 );
		//
		SCHEDULER_CONTROL_A0SUB4_WfrPrePost_Check( side , cur_clidx , 107 ); // 2009.06.11
		//
		if ( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_Start_Lot( side , cur_clidx , 0 , 2 , ( mode == 1 ) , 111 ) == -1 ) {
			_SCH_SYSTEM_MODE_ABORT_SET( side );
			return -1104;
		}
		//
		SCHEDULER_CONTROL_Set_Last_GlobalLot_Cluster_Act_A0SUB4( 99 , side , cur_clidx ); // Goto User Pre Check
		return -105;
		break;

	case 99 : // User Finish
		if ( ( act_side != -1 ) && ( act_clidx != -1 ) ) { // 2007.11.05
			if ( ( act_side != side ) || ( act_clidx != cur_clidx ) ) {
				SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( act_side , act_clidx );
				return -108;
			}
		}
		return 100;
		break;


	default : // Running
		return -109;
		break;
	}
	return 110;
}

//=================================================================================================
//[Tag = 0][i=0]
// ------------------------------------------------------------------------------------------------
// System Used Running Count
// ------------------------------------------------------------------------------------------------
// 몇번 Wafer가 공급이 되었었는지를 의미
//=================================================================================================
//
//=================================================================================================
//[Tag = 0][i=10]
// ------------------------------------------------------------------------------------------------
// Total Loop Count
// ------------------------------------------------------------------------------------------------
//=================================================================================================
//
//=================================================================================================
//[Tag = 1][i=11]
// ------------------------------------------------------------------------------------------------
// Next Run Slot
// ------------------------------------------------------------------------------------------------
// 현재 Wafer 다음에 진행할 Wafer에 대해 직접 진행할 Slot을 Setting한다
//   0 일때는 기본 Flow Data상에서 현재 Wafer 다음의 것을 진행한다.
//=================================================================================================
//
//=================================================================================================
//[Tag = 2][i=12]
// ------------------------------------------------------------------------------------------------
// Waiting Until Style
// ------------------------------------------------------------------------------------------------
// Before_Style이 [Wait until Free]이면 현재 Wafer를 Pick하기전에
//                모든 Wafer(현재 Cassette로 부터 Pick되어 나와있는)가 Cassete로 돌아와야만
//                비로소 현재 Wafer를 Pick을 하게 됩
// After_Style이 [Next_Wait]이면 현재 Wafer가 Pick된후 다시 Cassete로 돌아와야만
//                비로소 현재 Wafer를 Pick을 하게 됩
// ------------------------------------------------------------------------------------------------
//      Data     Before_Style      After_Style
//
//      0        N/A               Next_Not_Wait
//      1        Wait until Free   Next_Not_Wait
//      2        N/A               Next_Wait
//      3        Wait until Free   Next_Wait
//=================================================================================================
//
//=================================================================================================
//[Tag = 3][i=13]
// ------------------------------------------------------------------------------------------------
// Resuing Style
// ------------------------------------------------------------------------------------------------
// 현재 Wafer를 처음 Pick하고 다시 Cassette로 돌아온 후 다시 Pick을 할 수 있게 할지를 정의
//
//    0 : Wafer를 처음 Pick하고 다시 Cassette로 돌아온 후 다시 Pick을 할 수 없음
//    1 : Wafer를 처음 Pick하고 다시 Cassette로 돌아온 후 다시 Pick을 할 수 있음
//    2 : Wafer를 처음 Pick하고 다시 Cassette로 돌아온 후 다시 Pick을 할 수 없음
//        (Cassette로 돌아온 후 Wafer의 Flow,Recipe Data가 모두 사라짐)
//
//    5 : System이 동작중에 Setting하는 값으로 Resuing Style이 1로 정의한 경우 그리고
//          이 Wafer에 대해 [Tag2]에서 After_Style이 [Next_Wait]로 Setting되어 있으면
//          Wafer가 Cassete에서 Pick되고 다시 Cassette로 돌아올때 까지 System이 5로 Setting함
//          다시 Cassette로 돌아오면 Resuing Style을 1로 Setting함
//    6 : System이 동작중에 Setting하는 값으로 Resuing Style이 2로 정의한 경우 그리고
//          이 Wafer에 대해 [Tag2]에서 After_Style이 [Next_Wait]로 Setting되어 있으면
//          Wafer가 Cassete에서 Pick되고 다시 Cassette로 돌아올때 까지 System이 6로 Setting함
//          다시 Cassette로 돌아오면 Data 영역에서 삭제된다.
// ------------------------------------------------------------------------------------------------
//      Data     Meaning
//
//      0        Next Not Use
//      1        Next Use
//      2        Next Remove
//
//      (5)      If return wafer then Change to 1
//      (6)      If return wafer then remove
//=================================================================================================
//
//=================================================================================================
//[Tag = 4][i=14]
// ------------------------------------------------------------------------------------------------
// Make Dummy Mode(LotPre / WfrPre / WfrPost / LotPost)
// ------------------------------------------------------------------------------------------------
// 현재 Wafer가 Dummy Wafer를 만들때 어떤 Style의 Dummy Wafer를 만들지를 나타냄
// ------------------------------------------------------------------------------------------------
//      Data          LotPre  WfrPre  WfrPost LotPost
//
//      0             X       X       X       X
//      1             O       X       X       X
//      2             X       O       X       X
//      3             O       O       X       X
//      4             X       X       O       X
//      5             O       X       O       X
//      6             X       O       O       X
//      7             O       O       O       X
//      8             X       X       X       O
//      9             O       X       X       O
//      10            X       O       X       O
//      11            O       O       X       O
//      12            X       X       O       O
//      13            O       X       O       O
//      14            X       O       O       O
//      15            O       O       O       O
//=================================================================================================
//
//=================================================================================================
//[Tag = 5][i=15]
// ------------------------------------------------------------------------------------------------
// Make Dummy Style for Tag2 in Lot PrePost
// ------------------------------------------------------------------------------------------------
// Tag4에서 LotPre나 LotPost Wafer를 만들 경우
// 새롭게 만들어지는 LotPre나 LotPost Wafer의 Tag2 Style을 정의한다.
// 10으로 나눈 몫은 LotPost Wafer의 Tag2 Style이고
// 10으로 나눈 나머지는 LotPre Wafer의 Tag2 Style이다
// ------------------------------------------------------------------------------------------------
//    Value % 10 : Lot Pre
//    Value / 10 : Lot Post
//=================================================================================================
//
//=================================================================================================
//[Tag = 7][i=21,2+l]
// ------------------------------------------------------------------------------------------------
// Processing Finish Check
// Format : 0000000
//          -        Area D
//           --      Area C
//             --    Area B
//               --  Area A
//
// Area A : Check PM
// Area B : Run Possible PM Multi XIndex
// Area C : Check PM Order
// Area D : Remain Multi Recipe
// ------------------------------------------------------------------------------------------------
// 현재 Wafer가 공정을 진행할때 Area A,C의 조건에 맞는 PM과 Order에서 진행을 하면
//   이후의 공정은 하지 않고 Cassette로 돌아온다.
//
//   Area A : Check할 PM을 의미하며 1부터 PM1을 의미한다.
//            Area A가 0 일때는 Check하지 않는다.
//   Area C : Area A가 0 이 아니고 Check할 PM에 대해 Order가 Area C와 같을 때에만 선택된다.
//            Area C가 0 일때는 모든 Order에 대해 선택된다.
//
//   Area B : Area A와 Area C에 의해 선택된 PM에서 정의된 Process Recipe로 Processing을 하려고 할때
//            Process Recipe가 Multi Recipe일 경우 Area B가 0일때는 모든 Multi Recipe가
//            Processing에 참여하지만 Area B가 0이 아닐 경우 Area B로 정의된 순서까지의
//            Process Recipe까지만 Processing을 하고 나머지는 하지 않는다.
//   Area D : Area B가 0 이 아닐때 Multi Recipe중 일부만 Processing에 참여하게 된다.
//            이때 Processing에 참여한 Process Recipe를 다음에 계속 사용하려면
//            Area D가 1로 Setting 되어야 한다. Area D가 0으로 Setting 된면 Processing을 한
//            Multi Recipe는 다음에 사용되지 않는다.
//=================================================================================================
//
//=================================================================================================
//[Tag = ?][i=16]
// ------------------------------------------------------------------------------------------------
// Wafer Type
// ------------------------------------------------------------------------------------------------
//=================================================================================================
//
//
void SCHEDULER_CONTROL_RUN_STYLE_SET( int side , int pointer , int lc , int mode , int TotalLoopCount , int Type , int NextSlot , int Tag2 , int Tag3 , int LotPrePostMode , int LotPrePostStyle , char *MatchPMString ) {
	// Type
	//			0 : Last Loop
	//			1 : Middle Loop
	//			2 : First Loop
	//			3 : First/Last Loop

	//			4 : Last Loop + First Wafer
	//			5 : Middle Loop + First Wafer
	//			6 : First Loop + First Wafer
	//			7 : First/Last Loop + First Wafer

	//			8 : Last Loop + Last Wafer
	//			9 : Middle Loop + Last Wafer
	//			10 : First Loop + Last Wafer
	//			11 : First/Last Loop + Last Wafer

	//			12 : Last Loop + First/Last Wafer
	//			13 : Middle Loop + First/Last Wafer
	//			14 : First Loop + First/Last Wafer
	//			15 : First/Last Loop + First/Last Wafer
	//
	int i , z , z2 , c;
	char buffer[32];
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer ,  9 , 2 , 0 );				// Dll Call Checking
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 10 , 2 , TotalLoopCount );	// Total Loop Count
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 10 , 3 , 0 );				// 현재 Call한 Dummy의 Mode
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 12 , 2 , Tag2 );				// Before Pick Wait Control
	_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , Tag3 );				// After Pick Wait Control
	_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , LotPrePostMode );	// Lot Pre/Post Use
	_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 2 , LotPrePostStyle );	// Lot Pre/Post Style
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 3 , 0 );				// 현재 Call한 Dummy의 Count1
	_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 4 , 0 );				// 현재 Call한 Dummy의 Count2
	_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 3 , 0 );				// N/A
	_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 4 , 0 );				// N/A
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 16 , 2 , Type );				// Wafer FlowControl Detail Style
	_SCH_CLUSTER_Set_PathRun( side , pointer , 17 , 2 , 0 );				// Reference Normal Wafer Pointer(Post Only)
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 16 , 3 , 0 );
	_SCH_CLUSTER_Set_PathRun( side , pointer , 16 , 4 , 0 );
	_SCH_CLUSTER_Set_PathRun( side , pointer , 17 , 3 , 0 );
	_SCH_CLUSTER_Set_PathRun( side , pointer , 17 , 4 , 0 );
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 18 , 2 , 0 );				// Waiting Reference Pointer(Post Only)
	_SCH_CLUSTER_Set_PathRun( side , pointer , 19 , 2 , 0 );				// Waiting Reference Module(Post Only)
	_SCH_CLUSTER_Set_PathRun( side , pointer , 19 , 3 , 0 );				// Waiting Reference Order
	//
	i = 0;
	z = 0;
	//
	while ( TRUE ) {
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( MatchPMString , ':' , buffer , z , 31 );
		if ( z == z2 ) break;
		c = atoi( buffer );
		//
		if ( c > 0 ) {
			//
			if ( ( i + 21 ) >= MAX_CLUSTER_DEPTH ) { // 2010.02.25
				_IO_CIM_PRINTF( "SCHEDULER_CONTROL_RUN_STYLE_SET MatchCondition Full [%d][%d][%d][%d]\n" , ( c / 1000000 ) , ( c % 1000000 ) / 10000 , ( c % 10000 ) / 100 , ( c % 100 ) / 1 );
			}
			else {
				// 0-00-00-00
				// 5  3  4  2
				// *------------ (5) Reuse
				//    *--------- (3) Check ClusterDepth
				//       *------ (4) Check MultiIndex
				//          *--- (2) Check PM
				//
				_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + i , 5 , ( c / 1000000 ) );
				_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + i , 3 , ( c % 1000000   ) / 10000 );
				_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + i , 4 , ( c % 10000     ) / 100 );
				_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + i , 2 , ( c % 100       ) / 1 );
				//
				i++;
			}
			//
		}
		//
		z = z2;
	}
	//
	_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 2 , i );				// Detail Retuning Info Count
	_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 3 , 0 );
	_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 4 , 0 );				// 2010.02.01
	//
	if ( NextSlot > 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			if ( _SCH_CLUSTER_Get_PathIn( side , i ) >= PM1 ) continue;
			if ( _SCH_CLUSTER_Get_SlotIn( side , i ) != NextSlot ) continue;
			//
			_SCH_CLUSTER_Set_PathRun( side , pointer , 11 , 2 , i + 1 );	// Next Go Pointer
			//
			return;
		}
	}
	_SCH_CLUSTER_Set_PathRun( side , pointer , 11 , 2 , 0 );				// Next Go Pointer
}


//
BOOL SCHEDULER_FILENAME_INIT = FALSE;
//
char *SCHEDULER_FILENAME_BUFFER1 = NULL;
char *SCHEDULER_FILENAME_BUFFER2 = NULL;
char *SCHEDULER_FILENAME_BUFFER3 = NULL;
char *SCHEDULER_FILENAME_BUFFER4 = NULL;
//

int SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4( int side , int pointer , int mode ) {
	int Next_Go_Slot , Tag2 , Tag3 , LotPrePostMode , LotPrePostStyle , lc , totalloop;
	char MatchPMString[1024];
	//----------------------------------------------------------------------------------------------
	if ( !USER_EXIST_SP_N0S4_FLOWCONTROL() ) return 1;
	//----------------------------------------------------------------------------------------------
	if ( !SCHEDULER_FILENAME_INIT ) {
		SCHEDULER_FILENAME_BUFFER1 = calloc( 25600 + 1 , sizeof( char ) );
		if ( SCHEDULER_FILENAME_BUFFER1 == NULL ) {
			_IO_CIM_PRINTF( "SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4 Memory Allocate Error\n" );
			return 1;
		}
		SCHEDULER_FILENAME_BUFFER2 = calloc( 25600 + 1 , sizeof( char ) );
		if ( SCHEDULER_FILENAME_BUFFER2 == NULL ) {
			free( SCHEDULER_FILENAME_BUFFER1 );
			_IO_CIM_PRINTF( "SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4 Memory Allocate Error\n" );
			return 1;
		}
		SCHEDULER_FILENAME_BUFFER3 = calloc( 25600 + 1 , sizeof( char ) );
		if ( SCHEDULER_FILENAME_BUFFER3 == NULL ) {
			free( SCHEDULER_FILENAME_BUFFER1 );
			free( SCHEDULER_FILENAME_BUFFER2 );
			_IO_CIM_PRINTF( "SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4 Memory Allocate Error\n" );
			return 1;
		}
		SCHEDULER_FILENAME_BUFFER4 = calloc( 25600 + 1 , sizeof( char ) );
		if ( SCHEDULER_FILENAME_BUFFER4 == NULL ) {
			free( SCHEDULER_FILENAME_BUFFER1 );
			free( SCHEDULER_FILENAME_BUFFER2 );
			free( SCHEDULER_FILENAME_BUFFER3 );
			_IO_CIM_PRINTF( "SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4 Memory Allocate Error\n" );
			return 1;
		}
		SCHEDULER_FILENAME_INIT = TRUE;
	}
	//----------------------------------------------------------------------------------------------
	strcpy( SCHEDULER_FILENAME_BUFFER1 , "" );
	strcpy( SCHEDULER_FILENAME_BUFFER2 , "" );
	strcpy( SCHEDULER_FILENAME_BUFFER3 , "" );
	strcpy( SCHEDULER_FILENAME_BUFFER4 , "" );
	//
	totalloop       = _SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 2 );
	Tag2            = _SCH_CLUSTER_Get_PathRun( side , pointer , 12 , 2 );
	Tag3            = _SCH_CLUSTER_Get_PathRun( side , pointer , 13 , 2 );
	LotPrePostMode  = _SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 );
	LotPrePostStyle = _SCH_CLUSTER_Get_PathRun( side , pointer , 15 , 2 );
	//
	strcpy( MatchPMString , "" );
	//
	lc = _SCH_CLUSTER_Get_PathRun( side , pointer , 0 , 2 );
	//----------------------------------------------------------------------------------------------
	Next_Go_Slot = USER_RECIPE_SP_N0S4_FLOWCONTROL( side , pointer , _SCH_CLUSTER_Get_PathIn( side , pointer ) , _SCH_CLUSTER_Get_SlotIn( side , pointer ) , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , lc , mode , &totalloop , &Tag2 , &Tag3 , &LotPrePostMode , &LotPrePostStyle , MatchPMString , SCHEDULER_FILENAME_BUFFER1 , SCHEDULER_FILENAME_BUFFER2 , SCHEDULER_FILENAME_BUFFER3 , SCHEDULER_FILENAME_BUFFER4 );
	//----------------------------------------------------------------------------------------------
	if ( Next_Go_Slot < 0 ) {
		_SCH_CLUSTER_Set_PathRun( side , pointer , 12 , 2 , Tag2 );
		_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 2 , LotPrePostStyle );
		return 0;
	}
	//----------------------------------------------------------------------------------------------
	if ( ( Next_Go_Slot % 100 ) > MAX_CASSETTE_SLOT_SIZE ) Next_Go_Slot = 0;
	//----------------------------------------------------------------------------------------------
	if ( ( Tag2 < 0 ) || ( Tag2 >  3 ) ) Tag2 = 0;
	if ( ( Tag3 < 0 ) || ( Tag3 >  2 ) ) Tag3 = 0;
	if ( ( LotPrePostMode < 0 ) || ( LotPrePostMode > 15 ) ) LotPrePostMode = 0;
	if ( ( ( LotPrePostStyle % 10 ) < 0 ) || ( ( LotPrePostStyle % 10 ) > 3 ) ) LotPrePostStyle = 0;
	if ( ( ( LotPrePostStyle / 10 ) < 0 ) || ( ( LotPrePostStyle / 10 ) > 3 ) ) LotPrePostStyle = 0;
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SET1\t[CurrentLc=%dth][Side=%d][Pt=%d][Mode=%d][TotalLoop=%d][NextSlot=%d][Tag2=%d][Tag3=%d][Offset=%d]\n" , lc + 1 , side , pointer , mode , totalloop , Next_Go_Slot , Tag2 , Tag3 , _SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) );
_IO_CONSOLE_PRINTF( "FLOWCTL-SET2\t[LotPrePostMode=%d][LotPrePostStyle=%d][MatchPMString=%s]\n" , LotPrePostMode , LotPrePostStyle , MatchPMString );
_IO_CONSOLE_PRINTF( "FLOWCTL-SET3\t[RECIPE=%s,%s,%s,%s]\n" , SCHEDULER_FILENAME_BUFFER1 , SCHEDULER_FILENAME_BUFFER2 , SCHEDULER_FILENAME_BUFFER3 , SCHEDULER_FILENAME_BUFFER4 );
//------------------------------------------------------------------------------------------------------------------
	SCHEDULER_CONTROL_RUN_STYLE_SET( side , pointer , lc , mode , totalloop , Next_Go_Slot / 100 , Next_Go_Slot % 100 , Tag2 , Tag3 , LotPrePostMode , LotPrePostStyle , MatchPMString );
	//
	_SCH_CLUSTER_Set_UserArea( side  , pointer , SCHEDULER_FILENAME_BUFFER1 );
	_SCH_CLUSTER_Set_UserArea2( side , pointer , SCHEDULER_FILENAME_BUFFER2 );
	_SCH_CLUSTER_Set_UserArea3( side , pointer , SCHEDULER_FILENAME_BUFFER3 );
	_SCH_CLUSTER_Set_UserArea4( side , pointer , SCHEDULER_FILENAME_BUFFER4 );
	//----------------------------------------------------------------------------------------------
	return 1;
}

/*
// 2011.06.13
BOOL SCHEDULER_CONTROL_PICK_FROM_FM_DUMMYWAITCHECK_A0SUB4( int side , int pt ) { // 2011.05.11
	int i , j;
	//
	if ( _SCH_SYSTEM_MODE_GET( side ) == 0 ) return TRUE;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( side == i ) continue;
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( _SCH_SYSTEM_RUNORDER_GET( i ) > _SCH_SYSTEM_RUNORDER_GET( side ) ) continue;
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			//
			if ( _SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_READY ) return FALSE;
			//
		}
		//
	}
	//
	return TRUE;
	//
}
*/

int SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( int side , int pt , int mode ) {
	int res;
	EnterCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2007.10.04
	//
	if ( mode == 101 ) { // 2008.11.20
		res = SCHEDULER_CONTROL_PICK_FROM_FM_FLAG_CALL_A0SUB4( side , pt , 1 );
		LeaveCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 );
		return res;
	}
	//----------------------------------------------------------------------
	res = 1;
	//----------------------------------------------------------------------
	if ( mode != 1 ) { // 2008.05.29
		switch( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_FinishCheck_Before_Pick( side , -1 , TRUE , ( mode == -1 ) ) ) { // 2008.05.28 // 2010.01.14
		case -1 :
			_SCH_SYSTEM_MODE_ABORT_SET( side );
			res = -1001;
			break;
		case 1 :
			res = -1002;
			break;
		}
	}
	if ( res > 0 ) {
		//
//		if ( xinside_A0SUB4_PREMAINSKIP[ side ] ) { // 2008.11.22 // 2009.06.28
//			if ( _SCH_CLUSTER_Get_PathIn( side , pt ) >= PM1 ) {
//				SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 0 , side , -1 , pt , -1 , TRUE );
//			}
//		}
		if ( mode != -1 ) { // 2010.01.14
			if ( _SCH_CLUSTER_Get_PathIn( side , pt ) >= PM1 ) { // 2009.06.28
				if ( xinside_A0SUB4_PREMAINSKIP[ side ] ) SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 0 , side , -1 , pt , -1 , TRUE );
				if ( xinside_A0SUB4_WFRPRESKIP[ side ]  ) SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 1 , side , -1 , pt , -1 , TRUE );
				if ( xinside_A0SUB4_WFRPOSTSKIP[ side ] ) SCHEDULER_CONTROL_PREMAIN_WFRPRE_WFRPOST_REMOVE( 2 , side , -1 , pt , -1 , TRUE );
			}
		}
		//
		//----------------------------------------------------------------------
		res = SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4_SUB( side , pt , mode );
		//----------------------------------------------------------------------
		if ( mode != 1 ) { // 2008.05.29
			if ( res > 0 ) {
				switch( SCHEDULER_CONTROL_A0SUB4_LotPre_Process_FinishCheck_Before_Pick( side , -1 , FALSE , ( mode == -1 ) ) ) { // 2008.05.28 // 2010.01.14
				case -1 :
					_SCH_SYSTEM_MODE_ABORT_SET( side );
					res = -1003;
					break;
				case 1 :
					res = -1004;
					break;
				}
			}
		}
		//----------------------------------------------------------------------
	}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( side , 0 , "FM  A0SUB4 RUNED [pt=%d,mode=%d,newact=%d,res=%d]\n" , pt , mode , xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT , res );
	//----------------------------------------------------------------------
	LeaveCriticalSection( &CR_FEM_PICK_SUBAL_STYLE_0 ); // 2007.10.04
	return res;
}

BOOL SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( int TMATM , int bch , int side , int pointer , int mode ) { // 2009.08.26
	int Res , i;
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() != 0 ) return TRUE;
	//
	Res = _SCH_CLUSTER_Get_Extra_Flag( side , pointer , 2 );
	//
	if ( Res == 0 ) return TRUE;
	//
	switch( mode ) {
	case BUFFER_INWAIT_STATUS :
		if ( ( Res == 1 ) || ( Res == 2 ) ) {
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_IN_MODE ) continue;
				//
				if ( i == bch ) return TRUE;
				//
				return FALSE;
			}
			//
			return FALSE;
		}
		else if ( ( Res == 3 ) || ( Res == 4 ) ) {
			//
			for ( i = ( BM1 + MAX_BM_CHAMBER_DEPTH - 1 ) ; i >= BM1 ; i-- ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_IN_MODE ) continue;
				//
				if ( i == bch ) return TRUE;
				//
				return FALSE;
			}
			//
			return FALSE;
		}
		break;
	case BUFFER_OUTWAIT_STATUS :
		if ( ( Res == 1 ) || ( Res == 3 ) ) {
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_OUT_MODE ) continue;
				//
				if ( i == bch ) return TRUE;
				//
				return FALSE;
			}
			//
			return FALSE;
		}
		else if ( ( Res == 2 ) || ( Res == 4 ) ) {
			//
			for ( i = ( BM1 + MAX_BM_CHAMBER_DEPTH - 1 ) ; i >= BM1 ; i-- ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != TMATM ) ) continue;
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
				if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( TMATM , i ) != BUFFER_OUT_MODE ) continue;
				//
				if ( i == bch ) return TRUE;
				//
				return FALSE;
			}
			//
			return FALSE;
		}
		break;
	}
	return TRUE;
}

int SCHEDULER_CONTROL_Remain_Under_1_Count_for_FM( BOOL all , int side , int *cs , int *cp ) {
	int pt , cnt = 0;
	//
	*cs = -1;
	*cp = -1;
	//
	pt = all ? 0 : _SCH_MODULE_Get_FM_DoPointer( side );
	//
	if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return 0;
	//
	if ( _SCH_CLUSTER_Get_PathRun( side , pt , 11 , 2 ) > 0 ) return -1;
	//
	*cs = side;
	*cp = pt;
	//
	while ( TRUE ) {
		//
		while ( _SCH_SUB_Run_Impossible_Condition( side , pt , -1 ) ) {
			//
			pt++;
			//
			if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return cnt;
			//
		}
		//
		if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return cnt;
		//
		if ( _SCH_CLUSTER_Get_PathRun( side , pt , 11 , 2 ) > 0 ) return -1;
		//
		cnt++;
		//
		pt++; // 2008.04.08
		//
		if ( cnt > 1 ) return -1;
		//
	}
	return -1;
}


int SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_SUB_OTHER_FINISH( int CHECKING_SIDE , int *othercount , int *rcs , int *rcp ) { // 2010.03.18
	int i , cnt , cs , cp , of;
	//
	*rcs = -1; // 2012.06.16
	*rcp = -1; // 2012.06.16
	//
	*othercount = 0;
	//
	of = 1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( CHECKING_SIDE != i ) { // 2008.03.26
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
				if ( !_SCH_MODULE_Chk_FM_Finish_Status( i ) ) {
					cnt = SCHEDULER_CONTROL_Remain_Under_1_Count_for_FM( FALSE , i , &cs , &cp );
					if ( cnt == -1 ) return -1;
					//
					*othercount = *othercount + cnt;
					//
					of = 0;
					//
					*rcs = cs; // 2012.06.16
					*rcp = cp; // 2012.06.16
					//
				}
				if ( _SCH_SYSTEM_MODE_LOOP_GET( i ) == 0 ) {
					return -1; // 2007.12.14
				}
			}
			else {
				if ( _SCH_SYSTEM_USING_STARTING( i ) ) {
					cnt = SCHEDULER_CONTROL_Remain_Under_1_Count_for_FM( TRUE , i , &cs , &cp );
					//
					if ( cnt == -1 ) return -1;
					//
					*othercount = *othercount + cnt;
					//
					of = 0;
					//
					*rcs = cs; // 2012.06.16
					*rcp = cp; // 2012.06.16
					//
				}
			}
		}
	}
	return of;
}

BOOL SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_AFTER_PICK_A0SUB4( int CHECKING_SIDE , int pt , int bmc ) {
	int i , j , m , w , s , lp , lp2 , cs , cp , othercount , outlastcheck , lastblocking;

/*
// 2008.02.25
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( !_SCH_MODULE_Chk_FM_Finish_Status( i ) ) return FALSE;
		if ( _SCH_SYSTEM_MODE_LOOP_GET( i ) == 0 ) return FALSE; // 2007.12.14
	}
*/
	//-----------------------------------------------------------------------------------------
	// 2008.02.25
	//-----------------------------------------------------------------------------------------
	/*
	// 2010.03.18
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( CHECKING_SIDE != i ) { // 2008.03.26
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) {
				if ( !_SCH_MODULE_Chk_FM_Finish_Status( i ) ) return FALSE;
				if ( _SCH_SYSTEM_MODE_LOOP_GET( i ) == 0 ) return FALSE; // 2007.12.14
			}
			else {
				if ( _SCH_SYSTEM_USING_STARTING( i ) ) return FALSE;
			}
		}
	}
	*/
	//-----------------------------------------------------------------------------------------
	// 2008.04.15
	//-----------------------------------------------------------------------------------------

	lp = 0;
	//
	while( TRUE ) { // 2008.04.15
		//-----------------------------------------------------------------------------------------
		// 2008.03.26
		//-----------------------------------------------------------------------------------------
		cs = -1;
		cp = -1;
		//
		//-----------------------------------------------------------------------------------------
		// 2010.03.18
		//-----------------------------------------------------------------------------------------
		lp2 = 0;
		othercount = 0;
		outlastcheck = 0;
		lastblocking = FM_Pick_Running_Blocking_style_0;
		//
		while( TRUE ) { // 2010.03.18
			//
//			m = SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_SUB_OTHER_FINISH( CHECKING_SIDE , &othercount ); // 2012.06.17
			//
			m = SCHEDULER_CONTROL_CHECK_SKIP_FMSIDE_SUB_OTHER_FINISH( CHECKING_SIDE , &othercount , &cs , &cp ); // 2012.06.17
			//
			if      ( m == 1 ) { // allfinish
				othercount = 0;
				break;
			}
			else if ( m == -1 ) { // many
				return FALSE;
			}
			//
			if ( lp2 == 0 ) {
				outlastcheck = othercount;
				lp2++;
			}
			else {
				if ( outlastcheck != othercount ) {
					lp2 = 0;
				}
				else {
					//
					lp2++;
					if ( lp2 > 3 ) {
						//
						if ( othercount > 1 ) return FALSE;
						//
						break;
						//
					}
				}
			}
			//
			Sleep(1);
			//
		}
		//
		//
//		w = FALSE; // 2008.03.26 // 2010.03.18
		w = ( othercount != 0 ); // 2008.03.26 // 2010.03.18
		//
		//
		if ( !_SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
			if ( SCHEDULER_CONTROL_Remain_Under_1_Count_for_FM( FALSE , CHECKING_SIDE , &cs , &cp ) != -1 ) { // 2008.03.26
				//
			if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2010.03.18
					lp = 0;
					continue;
				}
				//
				if ( w ) return FALSE; // 2010.03.18
				w = TRUE; // 2008.03.26
			} // 2008.03.26
			else { // 2008.03.26
				return FALSE;
			}
		}
		//
		if ( _SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 0 ) return FALSE; // 2007.12.14
		//-----------------------------------------------------------------------------------------
		// 2008.04.15
		//-----------------------------------------------------------------------------------------
		if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2010.03.18
			lp = 0;
			continue;
		}
		//
//		if ( ( _SCH_MODULE_Get_FM_WAFER( 0 ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( 1 ) <= 0 ) ) { // 2017.01.06
		if ( ( _SCH_MODULE_Get_FM_WAFER( 0 ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( 1 ) <= 0 ) && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2017.01.06
			if ( FM_Pick_Running_Blocking_style_0 != 0 ) {
				lp++;
				if ( lp > 3 ) {
					//
					if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2010.03.18
						lp = 0;
						continue;
					}
					//
					if ( w ) return FALSE;
					w = TRUE;
					//
					lp = 0;
				}
			}
			else {
//				if ( ( _SCH_MODULE_Get_FM_WAFER( 0 ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( 1 ) <= 0 ) ) { // 2017.01.06
				if ( ( _SCH_MODULE_Get_FM_WAFER( 0 ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( 1 ) <= 0 ) && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2017.01.06
					lp = 0;
				}
				else {
					lp = 1;
				}
			}
		}
		else {
			lp = 0; // 2008.04.15
			//-----------------------------------------------------------------------------------------
			// 2008.03.19
			//-----------------------------------------------------------------------------------------
		//	w = FALSE; // 2008.03.26
			for ( i = 0 ; i < 2 ; i++ ) {
				if ( _SCH_MODULE_Get_FM_WAFER( i ) > 0 ) {
					lp++; // 2008.04.14
					s = _SCH_MODULE_Get_FM_SIDE( i );
					j = _SCH_MODULE_Get_FM_POINTER( i );
					if ( _SCH_MODULE_Get_FM_WAFER( i ) > 0 ) {
						if ( ( j >= 0 ) && ( j < 100 ) ) {
							if ( _SCH_CLUSTER_Get_PathDo( s , j ) == 0 ) {
								//
								if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2010.03.18
									lp = 0;
									break;
								}
								//
								if ( w ) return FALSE;
								w = TRUE;
								//
								cs = s;
								cp = j;
								//
								break;
							}
						}
					}
				}
			}
			//
			// 2017.01.06
			//
			if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2017.01.20
				lp = 0;
				continue;
			}
			//
//			if ( i == 2 ) { // 2017.01.20
				//
				if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
					lp++;
					s = _SCH_MODULE_Get_MFAL_SIDE();
					j = _SCH_MODULE_Get_MFAL_POINTER();
					//
					if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
						if ( ( j >= 0 ) && ( j < 100 ) ) {
							if ( _SCH_CLUSTER_Get_PathDo( s , j ) == 0 ) {
								//
								if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) {
									lp = 0;
								}
								else {
									//
									if ( w ) return FALSE;
									w = TRUE;
									//
									cs = s;
									cp = j;
									//
								}
							}
						}
					}
				}
//			} // 2017.01.20
			//
			//
			//-----------------------------------------------------------------------------------------
			if ( lp == 0 ) lp = 1; // 2008.04.15
			else           lp = 0; // 2008.04.15
			//-----------------------------------------------------------------------------------------
		}
		//-----------------------------------------------------------------------------------------
		if ( lastblocking != FM_Pick_Running_Blocking_style_0 ) { // 2010.03.18
			lp = 0;
			continue;
		}
		//-----------------------------------------------------------------------------------------
		if ( lp == 0 ) break; // 2008.04.15
		//-----------------------------------------------------------------------------------------
		Sleep(1); // 2008.04.15
		//-----------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------


//_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "SKIP_FMSIDE CHECK-1 [w=%d][cs=%d][cp=%d]\n" , w , cs , cp );


//	if ( ( _SCH_MODULE_Get_FM_WAFER( 0 ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( 1 ) > 0 ) ) { // 2008.03.19
	if ( w ) { // 2008.03.19
		j = 0;
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( i == bmc ) continue;
			if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2009.02.13
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) continue;
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue;
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , -1 ) ) continue;
			if ( _SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) != _SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) continue;
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
			if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( i ) <= 0 ) continue;
			//
			if ( ( cs != -1 ) && ( cp != -1 ) ) { // 2009.08.26
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( cs , i ) == MUF_00_NOTUSE ) continue; // 2009.11.05
				//
				if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , i , cs , cp , BUFFER_INWAIT_STATUS ) ) continue;
			}
			//
			j++;
		}
		if ( j == 0 ) {
			if ( ( cs != -1 ) && ( cp != -1 ) ) { // 2009.08.26
				if ( SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , bmc , cs , cp , BUFFER_INWAIT_STATUS ) ) {
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
	}
	else { // 2010.02.01
		if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
			if ( pt >= 0 ) {
				if ( _SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , pt , 11 , 2 ) > 0 ) return FALSE;
			}
		}
	}
	//
	_SCH_PREPOST_Pre_End_Part( -1 , bmc );
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_TUNING_GET_MORE_APPEND_DATA_A0SUB4( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index ,
						char *AppendStr ) { // 2008.06.09
	//
	if ( mode != 2 ) return FALSE; // Pre
	//
	if ( ( TuneData_Module % 100 ) != ( ch - PM1 + 1 ) ) return FALSE; // same Module
	if ( ( TuneData_Module / 100 ) != 0 ) return FALSE; // main
	//
	sprintf( AppendStr , "%d:%s:%s:%d" , TuneData_Step , TuneData_Name , TuneData_Data , TuneData_Index + 2 );
	//
	return TRUE;
}

