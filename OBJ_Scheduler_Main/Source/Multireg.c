#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "Multireg.h"

CRITICAL_SECTION CR_REGSC;

Scheduling_Regist	Scheduler_Reg[ MAX_CASSETTE_SIDE ][ MAX_REGISTER_SIZE ];

int				Scheduler_Reg_Size[ MAX_CASSETTE_SIDE ];
int				Scheduler_Reg_Index_Data = 0;
BOOL			Scheduler_Reg_Disable[ MAX_CASSETTE_SIDE ];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_REG_RESET_DATA( int apmode , int side ) {
	int i;
	if ( apmode == 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Scheduler_Reg_Size[ i ] = 0;
			Scheduler_Reg_Disable[ i ] = FALSE;
		}
		InitializeCriticalSection( &CR_REGSC );
	}
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		EnterCriticalSection( &CR_REGSC );
		Scheduler_Reg_Index_Data = 0;
		LeaveCriticalSection( &CR_REGSC );
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHEDULER_REG_GET_REGIST_INDEX() {
	int i;
	EnterCriticalSection( &CR_REGSC );
	i = Scheduler_Reg_Index_Data;
	Scheduler_Reg_Index_Data++;
	if ( Scheduler_Reg_Index_Data >= 1000 ) Scheduler_Reg_Index_Data = 0;
	LeaveCriticalSection( &CR_REGSC );
	return i;
}
//---------------------------------------------------------------------------------------
void _i_SCH_MULTIREG_DATA_RESET( int Side ) {
	EnterCriticalSection( &CR_REGSC );
	Scheduler_Reg_Size[ Side ] = 0;
	LeaveCriticalSection( &CR_REGSC );
}
//---------------------------------------------------------------------------------------
void SCHEDULER_REG_SET_DISABLE( int Side , BOOL data ) {
	EnterCriticalSection( &CR_REGSC );
	Scheduler_Reg_Disable[ Side ] = data;
	LeaveCriticalSection( &CR_REGSC );
}
//---------------------------------------------------------------------------------------
BOOL SCHEDULER_REG_GET_DISABLE( int Side ) {
	BOOL i;
	EnterCriticalSection( &CR_REGSC );
	i = Scheduler_Reg_Disable[ Side ];
	LeaveCriticalSection( &CR_REGSC );
	return i;
}
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIREG_HAS_REGIST_DATA( int Side ) {
	EnterCriticalSection( &CR_REGSC );
	if ( Scheduler_Reg_Size[Side] <= 0 ) {
		LeaveCriticalSection( &CR_REGSC );
		return FALSE;
	}
	LeaveCriticalSection( &CR_REGSC );
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIREG_GET_REGIST_DATA( int Side , Scheduling_Regist *READ_DATA ) {
	int i;
	EnterCriticalSection( &CR_REGSC );
	if ( Scheduler_Reg_Size[Side] <= 0 ) {
		LeaveCriticalSection( &CR_REGSC );
		return FALSE;
	}
	memcpy( READ_DATA , &(Scheduler_Reg[Side][0]) , sizeof( Scheduling_Regist ) );
	for ( i = 0 ; i < ( Scheduler_Reg_Size[Side] - 1 ); i++ ) {
		memcpy( &(Scheduler_Reg[Side][i]) , &(Scheduler_Reg[Side][i+1]) , sizeof( Scheduling_Regist ) );
	}
	Scheduler_Reg_Size[Side]--;
	if ( Scheduler_Reg_Size[Side] < 0 ) Scheduler_Reg_Size[Side] = 0;
	LeaveCriticalSection( &CR_REGSC );
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIREG_SET_REGIST_DATA( int Side , Scheduling_Regist *READ_DATA ) {
	EnterCriticalSection( &CR_REGSC );
	if ( Scheduler_Reg_Disable[ Side ] ) {
		LeaveCriticalSection( &CR_REGSC );
		return FALSE;
	}
	if ( Scheduler_Reg_Size[Side] >= MAX_REGISTER_SIZE ) {
		LeaveCriticalSection( &CR_REGSC );
		return FALSE;
	}
	memcpy( &(Scheduler_Reg[Side][Scheduler_Reg_Size[Side]]) , READ_DATA , sizeof( Scheduling_Regist ) );
	Scheduler_Reg_Size[Side]++;
	LeaveCriticalSection( &CR_REGSC );
	return TRUE;
}

