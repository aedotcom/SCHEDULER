#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"


//=======================================================================================
void SCHEDULING_CHECK_INTERLOCK_PM_SET( int side ) { // 2003.02.13
	int i , j;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) _i_SCH_MODULE_Set_Use_Interlock_Run( side , i + PM1 , FALSE );
	for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
		if ( j != side ) {
			if ( _i_SCH_SYSTEM_USING_GET( j ) >= 4 ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					if ( _i_SCH_MODULE_Get_Use_Interlock_Run( j , i + PM1 ) ) {
						_i_SCH_MODULE_Set_Use_Interlock_Run( side , i + PM1 , TRUE );
					}
				}
			}
		}
	}
}

//=======================================================================================
