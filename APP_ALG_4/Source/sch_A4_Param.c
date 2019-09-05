//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================
int				FM_SchedulerWillGoBM_STYLE_4[ MAX_CASSETTE_SIDE ];

int				BUFFER_WhereLastPlaced_STYLE_4 = -1; // 2007.09.12

void SCHEDULER_Set_FM_WILL_GO_BM( int side , int data ) { FM_SchedulerWillGoBM_STYLE_4[side] = data; } // 2003.06.20
int  SCHEDULER_Get_FM_WILL_GO_BM( int side ) { return FM_SchedulerWillGoBM_STYLE_4[side]; } // 2003.06.20

void SCHEDULER_Set_BM_WhereLastPlaced( int data ) { BUFFER_WhereLastPlaced_STYLE_4 = data; } // 2007.09.12
int  SCHEDULER_Get_BM_WhereLastPlaced() { return BUFFER_WhereLastPlaced_STYLE_4; } // 2007.09.12


