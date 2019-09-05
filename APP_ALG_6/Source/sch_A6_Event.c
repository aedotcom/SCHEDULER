//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================


int USER_EVENT_PRE_ANALYSIS_AL6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return 1;
}


extern int BM_WAIT_CHECK_6[MAX_CHAMBER]; // 2018.10.02

int USER_EVENT_ANALYSIS_AL6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	int bmc;
	//
	if ( STRCMP_L( RunMessage , "BM_WAITING" ) ) { // 2018.10.02
		if ( STRNCMP_L( ElseMessage , "BM" , 2 ) ) {
			//
			bmc = atoi( ElseMessage + 2 ) - 1 + BM1;
			//
			if ( ( bmc >= BM1 ) && ( bmc < TM ) ) {
				BM_WAIT_CHECK_6[bmc] = TRUE;

printf( "BM_WAITING BM%d\n" , bmc - BM1 + 1 );

			}
		}
		return 1;
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunMessage , "BM_NOTWAITING" ) ) { // 2018.10.02
		if ( STRNCMP_L( ElseMessage , "BM" , 2 ) ) {
			//
			bmc = atoi( ElseMessage + 2 ) - 1 + BM1;
			//
			if ( ( bmc >= BM1 ) && ( bmc < TM ) ) {
				BM_WAIT_CHECK_6[bmc] = FALSE;
printf( "BM_NOTWAITING BM%d\n" , bmc - BM1 + 1 );
			}
		}
		return 1;
	}
	//--------------------------------------------------------------------------------
	return -1;
}


