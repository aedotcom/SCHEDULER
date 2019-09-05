//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_system_tag.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_sch_prepost.h"
#include "INF_MultiJob.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A4_sub.h"
#include "sch_A4_sdm.h"
#include "sch_A4_subBM.h"
#include "sch_A4_init.h"
//================================================================================

CRITICAL_SECTION CR_SINGLEBM_STYLE_4;
CRITICAL_SECTION CR_MULTI_BM_STYLE_4;

//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
// Include Start
//###################################################################################################

extern int	SDM_4_LOTSPECIAL_TAG[ MAX_CHAMBER ]; // 2005.07.20

extern int	sch4_BM_PROCESS_MONITOR[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_CHECKING[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_MODE[ MAX_CHAMBER ];

void SCHEDULER_ALIGN_BUFFER_SET_RUN( int ); // 2007.04.26

void INIT_SCHEDULER_AL4_PART_DATA( int apmode , int side ) {
	int i;

	if ( apmode == 0 ) {
		InitializeCriticalSection( &CR_SINGLEBM_STYLE_4 );
		InitializeCriticalSection( &CR_MULTI_BM_STYLE_4 );
	}

	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//-----------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			sch4_BM_PROCESS_MONITOR[ i ] = FALSE;
			sch4_BM_PROCESS_CHECKING[ i ] = FALSE;
		}
		SCHEDULER_ALIGN_BUFFER_SET_RUN( 0 ); // 2007.04.26
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			SDM_4_LOTSPECIAL_TAG[i] = -1;
		}
	}
	//
	INIT_SCHEDULER_AL4_SUB_SDM_PART_DATA( apmode , side );
	//
	if ( apmode == -1 ) {
		DeleteCriticalSection( &CR_SINGLEBM_STYLE_4 );
		DeleteCriticalSection( &CR_MULTI_BM_STYLE_4 );
	}
}

//###################################################################################################
#else
//###################################################################################################

void INIT_SCHEDULER_AL4_PART_DATA( int apmode , int side ) {
}
//###################################################################################################
// Include End
//###################################################################################################
#endif
//###################################################################################################

