#include "INF_default.h"
#include "sch_A4_Main.h"


int _SCH_INF_DLL_API_ENTER_CONTROL_TM_PART( int CHECKING_SIDE, int mode ) {
	return USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_4( CHECKING_SIDE , mode );
}

BOOL _SCH_INF_DLL_API_RUN_CONTROL_TM_1_PART( int CHECKING_SIDE ) {
	return USER_DLL_SCH_INF_RUN_CONTROL_TM_1_STYLE_4( CHECKING_SIDE );
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_2_PART( int CHECKING_SIDE ) {
	return( USER_DLL_SCH_INF_RUN_CONTROL_TM_2_STYLE_4( CHECKING_SIDE ) );
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_3_PART( int CHECKING_SIDE ) {
	return( USER_DLL_SCH_INF_RUN_CONTROL_TM_3_STYLE_4( CHECKING_SIDE ) );
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_4_PART( int CHECKING_SIDE ) {
	return( USER_DLL_SCH_INF_RUN_CONTROL_TM_4_STYLE_4( CHECKING_SIDE ) );
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_5_PART( int CHECKING_SIDE ) {
#ifndef PM_CHAMBER_12
	return( USER_DLL_SCH_INF_RUN_CONTROL_TM_5_STYLE_4( CHECKING_SIDE ) );
#else
	return 0;
#endif
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_6_PART( int CHECKING_SIDE ) {
#ifndef PM_CHAMBER_12
	return( USER_DLL_SCH_INF_RUN_CONTROL_TM_6_STYLE_4( CHECKING_SIDE ) );
#else
	return 0;
#endif
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_7_PART( int CHECKING_SIDE ) {
#ifndef PM_CHAMBER_12
	#ifndef PM_CHAMBER_30
		return( USER_DLL_SCH_INF_RUN_CONTROL_TM_7_STYLE_4( CHECKING_SIDE ) );
	#else
		return 0;
	#endif
#else
	return 0;
#endif
}

int _SCH_INF_DLL_API_RUN_CONTROL_TM_8_PART( int CHECKING_SIDE ) {
#ifndef PM_CHAMBER_12
	#ifndef PM_CHAMBER_30
		return( USER_DLL_SCH_INF_RUN_CONTROL_TM_8_STYLE_4( CHECKING_SIDE ) );
	#else
		return 0;
	#endif
#else
	return 0;
#endif
}

