#ifndef MULTIREG_H
#define MULTIREG_H

typedef struct {
	int  RunIndex;

	int  CassIn;
	int  CassOut;
	int  SlotStart;
	int  SlotEnd;
	char JobName[ 512 + 1 ]; // 2005.05.11
	char LotName[ 128 + 1 ];
	char MidName[ 128 + 1 ];
	int  LoopCount;
} Scheduling_Regist;

void INIT_SCHEDULER_REG_RESET_DATA( int apmode , int side );

int  SCHEDULER_REG_GET_REGIST_INDEX();

void SCHEDULER_REG_SET_DISABLE( int Side , BOOL data );
BOOL SCHEDULER_REG_GET_DISABLE( int Side );

//
void _i_SCH_MULTIREG_DATA_RESET( int Side );
BOOL _i_SCH_MULTIREG_HAS_REGIST_DATA( int Side );
BOOL _i_SCH_MULTIREG_GET_REGIST_DATA( int Side , Scheduling_Regist *READ_DATA );
BOOL _i_SCH_MULTIREG_SET_REGIST_DATA( int Side , Scheduling_Regist *READ_DATA );


#endif

