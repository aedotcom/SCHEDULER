//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================
#include "sch_A6_Init.h"
//================================================================================

void USER_REVISION_HISTORY_AL6_PART( int mode , char *data , int count ) {
	sprintf( data , "2014/06/10/1200(%s %s)" , __DATE__ , __TIME__ );	//
//	strcpy( data , "2014/06/10/1200" );	// IP:MFI(1) Option Expand : Balace Ignore Supply
//	strcpy( data , "2014/01/27/1200" );	// IP:FORCE VENT/MFI(1) Option:Arm Separate Lot
//	strcpy( data , "2013/11/07/1200" );	// IP:DTM DISABLE CLOSE
//	strcpy( data , "2013/10/30/2000" );	// IP:DTM PRE FMSIDE at NONWAFER PROCESS TIME
//	strcpy( data , "2013/10/23/2000" );	// IP:DTM BM Place Order Bug when differ lot
//	strcpy( data , "2013/10/17/2000" );	// IP:DTM Parallel Disable Deadlock
//	strcpy( data , "2013/09/02/2000" );	// IP:Cyc Update
//	strcpy( data , "2013/04/04/2000" );	// IP:Cyc New Sch Append
//	strcpy( data , "2013/04/01/1000" );	// IP:Cyc New Sch Append
//	strcpy( data , "2013/02/15/1600" );	// IP:Cyc New Sch Append
//	strcpy( data , "2012/11/14/1600" );	// IP:Cyc Stop Bug
//	strcpy( data , "2012/07/13/1600" );	// IP:DTM Supply Bug
//	strcpy( data , "2012/03/08/1600" );	// IP:Cyc FMSIDE Double Bug Update
//	strcpy( data , "2012/02/16/2200" );	// IP:Cyc TMSIDE/FMSIDE Bug Update
//	strcpy( data , "2011/08/26/1200" );	// IP:Cyc Unload Bug Update
//	strcpy( data , "2011/04/21/1600" );	// IP:Cyc Finish
//	strcpy( data , "2011/03/03/1200" );	// IP:Cyc Stop Update
//	strcpy( data , "2010/10/22/1200" );	// IP:Cyc update
//	strcpy( data , "2010/10/07/1200" );	// IP:Cyc update
//	strcpy( data , "2010/09/29/1400" );	// IP:Cyc update
//	strcpy( data , "2010/09/14/1400" );	// IP:Cyc update
//	strcpy( data , "2010/09/01/1400" );	// IP:Cyc update
//	strcpy( data , "2010/08/05/1400" );	// IP:Cyc update
//	strcpy( data , "2010/07/28/1200" );	// IP:Cyc update
//	strcpy( data , "2010/07/02/1400" );	// IP:Cyc update
//	strcpy( data , "2010/05/27/1400" );	// IP:Cyc Loop update
//	strcpy( data , "2010/04/27/1400" );	// IP:Cyc Loop update
//	strcpy( data , "2009/07/09/1800" );	// IP:Cyc Disable/DisableHW Bug update
//	strcpy( data , "2009/03/10/0000" );	// PS:Diff lot A,B Pick update
//	strcpy( data , "2009/01/12/1800" );	// IP:Cyc Disable/DisableHW Bug update
//	strcpy( data , "2008/11/12/1200" );	// IP:Cyc Disable/DisableHW Bug update
//	strcpy( data , "2008/10/02/1800" );	// IP:Cyc Disable/DisableHW Part Append
//	strcpy( data , "2008/09/16/2000" );	// IP:Cyc Slot Select Part
//	strcpy( data , "2008/07/28/1400" );	// IP:Cyc Loading part
//	strcpy( data , "2008/07/13/2200" );	// AT:FM Wait Deadlock
//	strcpy( data , "2008/07/01/1500" );	// AT:Pause Wait Problem(2)
//	strcpy( data , "2008/06/25/2300" );	// AT:Pause Wait Problem
}




//================================================================================
extern BOOL MODULE_LAST_SWITCH_6[MAX_CHAMBER];
//================================================================================

void USER_SYSTEM_LOG_AL6( char *filename , int side ) {
	//
	int i;
	FILE *fpt;
	//
	if ( side == -1 ) {
		//
		fpt = fopen( filename , "a" );
		//
		if ( fpt != NULL ) {
			//
			fprintf( fpt , "<USER DATA AREA>\n" );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				//
				if ( MODULE_LAST_SWITCH_6[i] != 0 ) {
					fprintf( fpt , "LAST_SWITCH_6	%d		%d\n" , i , MODULE_LAST_SWITCH_6[i] );
				}
				//
			}
			//
			fclose( fpt );
		}
		//
	}
	//
	SCH_CYCLON2_STATUS_VIEW_SUB( filename , TRUE , side , 0 , 0 , 0 );
	//
}
