//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_Equip_Handling.h"
//================================================================================

int USER_EVENT_PRE_ANALYSIS_AL4( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return 1;
}

int USER_EVENT_ANALYSIS_AL4( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return -1;
}
