//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================
#include "sch_A0_Param.h"
#include "sch_A0_Default.h"
//================================================================================

void USER_REVISION_HISTORY_AL0_PART( int mode , char *data , int count ) {
	//
	sprintf( data , "2017/01/04/1200(%s %s)" , __DATE__ , __TIME__ );	//
	//
//	strcpy( data , "2015/02/05/1200" );	// EJ:AL2 BUG UPDATE
//	strcpy( data , "2014/08/18/0900" );	// AT:TENDOM SINGLEBM FMTMSIDE BUG
//	strcpy( data , "2014/01/27/1200" );	// IP:MULTI PM , CA:TMS5
//	strcpy( data , "2013/12/22/1000" );	// AT:TENDOM DEADLOCK
//	strcpy( data , "2013/11/14/1000" );	// AT:DBLFM BMMODE SYNCH BUG
//	strcpy( data , "2013/10/21/1000" );	// TY:1S 1Arm STOP DEADLOCK
//	strcpy( data , "2013/04/01/1000" );	// CA:DUALTM
//	strcpy( data , "2013/03/11/1200" );	// CA:DUALTM
//	strcpy( data , "2013/01/09/1200" );	// CA:DUALTM_PATHTHRU
//	strcpy( data , "2012/11/20/1200" );	// IP:EILFlow
//	strcpy( data , "2012/08/30/1800" );	// IP:AllDisable
//	strcpy( data , "2012/08/10/1400" );	// CA:PT deadlock
//	strcpy( data , "2012/07/12/1200" );	// EU:Arm Intlks Flow Update
//	strcpy( data , "2012/01/31/0000" );	// AP:MFIC Bug
//	strcpy( data , "2012/01/22/0000" );	// AP:MFIC Bug
//	strcpy( data , "2011/11/30/1800" );	// IP:ALG2
//	strcpy( data , "2011/10/13/1800" );	// CC:MFIC
//	strcpy( data , "2011/07/20/1800" );	// AP,CC:MFIC PT,Concurrent
//	strcpy( data , "2011/07/04/1800" );	// TY:1S 1Arm Black Cluster Deadlock
//	strcpy( data , "2011/06/23/1200" );	// TY:1S 1Arm STOP Deadlock
//	strcpy( data , "2011/05/31/1800" );	// CA:1S FMSIDE
//	strcpy( data , "2011/05/12/1400" );	// CA:1S Dummy
//	strcpy( data , "2011/05/04/1800" );	// CA:1S FMSIDE
//	strcpy( data , "2011/04/12/1800" );	// CA:Dummy Bug
//	strcpy( data , "2011/04/08/1800" );	// CA:1S TMSIDE Time Pick Order
//	strcpy( data , "2011/04/01/1800" );	// CA:Lot Pre ENABLEN
//	strcpy( data , "2011/03/31/1200" );	// CA:Lot Pre Post NewSpec Debug
//	strcpy( data , "2011/03/24/1200" );	// CA:Lot Pre Post NewSpec Debug Critical
//	strcpy( data , "2011/03/21/1600" );	// SF:Abort
//	strcpy( data , "2011/03/17/1600" );	// CA:Lot Pre Post NewSpec Debug Critical
//	strcpy( data , "2011/03/09/1600" );	// CA:Lot Pre Post NewSpec
//	strcpy( data , "2011/03/03/1600" );	// CA:Lot Pre Post NewSpec
//	strcpy( data , "2011/02/23/1800" );	// SF:place bug
//	strcpy( data , "2011/02/22/1600" );	// EU:Delay
//	strcpy( data , "2011/02/14/1200" );	// CA,AT:No Pass
//	strcpy( data , "2011/02/08/0800" );	// AT:DBL Append
//	strcpy( data , "2011/02/01/1200" );	// CA:Lot Pre Post NewSpec
//	strcpy( data , "2011/01/18/1400" );	// SF:CM PickPossible
//	strcpy( data , "2011/01/14/1600" );	// AT:nopass
//	strcpy( data , "2011/01/07/1600" );	// CA:NOPassing
//	strcpy( data , "2010/12/30/1200" );	// CA:NOPassing
//	strcpy( data , "2010/12/17/1800" );	// AP:MS Disable
//	strcpy( data , "2010/12/09/1200" );	// AP:MS Disable
//	strcpy( data , "2010/11/08/1200" );	// CC:1S
//	strcpy( data , "2010/10/28/1400" );	// CC:1S
//	strcpy( data , "2010/10/25/1200" );	// CA:202
//	strcpy( data , "2010/10/06/1400" );	// CA:MAINT
//	strcpy( data , "2010/10/05/2300" );	// CA:MAINT
//	strcpy( data , "2010/09/29/2300" );	// CA:MAINT
//	strcpy( data , "2010/09/26/1200" );	// CA:202
//	strcpy( data , "2010/09/14/1200" );	// CA:202
//	strcpy( data , "2010/08/28/1200" );	// APTC:FM IC Deadlock
//	strcpy( data , "2010/08/05/1400" );	// CA:PT 19? : Maint Pick 2
//	strcpy( data , "2010/08/16/1200" );	// CA:Maint / Dummy Run
//	strcpy( data , "2010/08/02/1600" );	// CA:PT 19? : Maint Pick
//	strcpy( data , "2010/08/11/1200" );	// CA:Maint Wafer Post , Lot Post
//	strcpy( data , "2010/07/09/1600" );	// CA:PT 196 : FMSIDE Trouble
//	strcpy( data , "2010/07/08/1500" );	// CA:PT 197 + align ordering
//	strcpy( data , "2010/06/24/1500" );	// CA:PT 197
//	strcpy( data , "2010/06/17/1800" );	// CA:PT 197
//	strcpy( data , "2010/06/16/1800" );	// CA:1S 196
//	strcpy( data , "2010/05/12/1200" );	// CA:Batch BM Start
//	strcpy( data , "2010/05/04/2000" );	// CA:Batch BM Start
//	strcpy( data , "2010/04/23/1800" );	// IP:Parallel
//	strcpy( data , "2010/04/21/1200" );	// IP:Parallel
//	strcpy( data , "2010/04/10/1800" );	// IP:Parallel
//	strcpy( data , "2010/04/05/2000" );	// CA:1S
//	strcpy( data , "2010/02/04/2200" );	// CA:1S , Dummy Trouble
//	strcpy( data , "2010/02/01/1200" );	// CA:1S , Dummy Trouble
//	strcpy( data , "2010/01/17/1800" );	// CA:1S LotPost&Pre No Trouble
//	strcpy( data , "2009/12/26/1800" );	// CA:AppItem
//	strcpy( data , "2009/12/08/1200" );	// SE:SUPPLY INT Bug
//	strcpy( data , "2009/11/20/1200" );	// CA:PT 185
//	strcpy( data , "2009/11/05/1800" );	// CA:PT 187
//	strcpy( data , "2009/10/22/1200" );	// AT:Double FM Bug Update
//	strcpy( data , "2009/10/14/1800" );	// CA:1S Deny Update
//	strcpy( data , "2009/10/13/1800" );	// AT:1S FM Place+Pick Append in TMDouble
//	strcpy( data , "2009/10/06/1600" );	// CA:1S Deny Part Update
//	strcpy( data , "2009/09/23/1000" );	// CA:Dummy Extra Flag
//	strcpy( data , "2009/09/19/1000" );	// CA:BM Extra Flag
//	strcpy( data , "2009/08/27/1600" );	// CA:BM Extra Flag
//	strcpy( data , "2009/08/18/1800" );	// AT:Double FM Arm Interlock Bug Update
//	strcpy( data , "2009/08/16/1200" );	// AT:Double FM Arm Interlock Bug Update
//	strcpy( data , "2009/08/11/1800" );	// AT:Double FM Bug Update
//	strcpy( data , "2009/08/06/1800" );	// AT:Double FM Bug Update
//	strcpy( data , "2009/07/29/1800" );	// AT:Double FM Bug Update
//	strcpy( data , "2009/07/28/1200" );	// CA:Wfer/Lot PrePost Skip DLL Update
//	strcpy( data , "2009/07/24/1800" );	// PS:BatchAll BM1->2 LoopCount Bug Update
//	strcpy( data , "2009/07/23/1200" );	// AT:Double FM Bug Update
//	strcpy( data , "2009/07/17/1800" );	// CA:Wfer/Lot PrePost Skip DLL Update
//	strcpy( data , "2009/07/03/1200" );	// CA:BatchLoad Bug Append
//	strcpy( data , "2009/06/28/1200" );	// CA:Wfer/Lot PrePost Skip DLL Append
//	strcpy( data , "2009/05/21/1400" );	// SE:BatchAll Disappear Update
//	strcpy( data , "2009/04/26/1200" );	// SE:BatchAll Bug Update
//	strcpy( data , "2009/04/08/1600" );	// CA:169
//	strcpy( data , "2009/03/27/2200" );	// AT:Double FM - X23
//	strcpy( data , "2009/03/24/1700" );	// EJ:BM Separate Supply
//	strcpy( data , "2009/03/03/2000" );	// CA:PathThru Message Order bug update in FM
										// SE:BatchAll Bug Update
										// AT:Double FM Bug Update
//	strcpy( data , "2009/02/18/1200" );	// CA:PathThru Message Order bug update in FM
//	strcpy( data , "2009/02/16/1800" );	// AT:Double FM - Deadlock and Full Swap TM SIDE
//	strcpy( data , "2009/01/19/1200" );	// CA:NoPass - Optimize
//	strcpy( data , "2008/12/10/1800" );	// CA:BatchAll - BM Use Flag Bug
//	strcpy( data , "2008/11/24/2200" );	// SE:BatchAll Bug
//	strcpy( data , "2008/12/04/2200" );	// AT:Double FM - PathThru Multi Slot
//	strcpy( data , "2008/11/13/2200" );	// AT:Double FM - MiddleSwap Bug
//	strcpy( data , "2008/10/28/1200" );	// SE:BatchAll , FO:Switch
//	strcpy( data , "2008/10/02/1800" );	// AT:Double FM - FullSwap Append
//	strcpy( data , "2008/09/25/1800" );	// CA:149,150,151,152,153,154
//	strcpy( data , "2008/09/16/2000" );	// SE:BatchAll(First Wafer No BM info in Align)
//	strcpy( data , "2008/09/01/1800" );	// AP:MiddleDeadlock,Cool
//	strcpy( data , "2008/08/25/2000" );	// SE:BatchAll(FM,TM Wait)'
//	strcpy( data , "2008/08/22/2000" );	// CC:Singleswap
//	strcpy( data , "2008/08/12/1200" );	// SE:BatchAll
//	strcpy( data , "2008/08/07/2300" );	// CA:144
//	strcpy( data , "2008/07/28/1400" );	// PS:Randomize Spilit , ASG:FullSwap Waiting FMSIDE
//	strcpy( data , "2008/07/22/1800" );	// CA:144
//	strcpy( data , "2008/07/21/1200" );	// IP:Normal(Split)
//	strcpy( data , "2008/07/18/1600" );	// IP:DoubleTM
//	strcpy( data , "2008/07/18/1100" );	// CA:140
//	strcpy( data , "2008/07/16/1400" );	// CA:140
//	strcpy( data , "2008/07/13/2300" );	// AT:Double FM Update
//	strcpy( data , "2008/07/12/2200" );	// SE:Batch-Aligner-Separate
//	strcpy( data , "2008/07/12/1200" );	// CA:141,142
//	strcpy( data , "2008/07/07/1000" );	// AT:Double FM Append
//	strcpy( data , "2008/06/26/1600" );	// CA:BatchAll Type Last tag Set Bug Update
//	strcpy( data , "2008/06/25/2300" );	// SE:Inline Type Update
}



//=======================================================================================
extern int ALG_DATA_SUB_STYLE_0;
//=======================================================================================
extern int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE;
extern int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT;
extern int xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[MAX_PM_CHAMBER_DEPTH];
//=======================================================================================
extern int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE;
extern int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT;
extern int xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[MAX_PM_CHAMBER_DEPTH];
//=======================================================================================
extern int xinside_A0SUB4_LOT_ORDER_ALL_INDEX;
extern int xinside_A0SUB4_LOT_ORDER_INDEX[MAX_CASSETTE_SIDE];
//
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE;
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER;
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX;
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT;
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTSIDE;
extern int	xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTINDEX;
//
extern BOOL xinside_A0SUB4_PICKFROMCM_RUN[MAX_CASSETTE_SIDE];
extern int  xinside_A0SUB4_PICKFROMCM_PT[MAX_CASSETTE_SIDE];
extern int  xinside_A0SUB4_PICKFROMCM_RESULT[MAX_CASSETTE_SIDE];
//=======================================================================================


void USER_SYSTEM_LOG_AL0( char *filename , int side ) {
	int i;
	FILE *fpt;

	if ( side != -1 ) return;

	fpt = fopen( filename , "a" );

	if ( fpt == NULL ) return;

	fprintf( fpt , "<USER DATA AREA>\n" );
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	fprintf( fpt , " BUFFER_SWITCH_MODE [%d]\n" , Get_Prm_MODULE_BUFFER_SWITCH_MODE() );
	fprintf( fpt , " BUFFER_SINGLE_MODE [%d]\n" , Get_Prm_MODULE_BUFFER_SINGLE_MODE() ); // 2014.10.15
	fprintf( fpt , " BUFFER_USE_MODE    [%d]\n" , Get_Prm_MODULE_BUFFER_USE_MODE() ); // 2014.10.15
	fprintf( fpt , " PATHTHRU_MODE      [%d]\n" , SCHEDULER_Get_PATHTHRU_MODE() ); // 2014.10.15
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		fprintf( fpt , " BMFlag:%d:Switch_What=%2d,Switch_7_Cross=%2d,BATCHALL_SWITCH_BM_ONLY=%2d\n" , i , SCHEDULER_CONTROL_Get_BM_Switch_WhatCh(i),SCHEDULER_CONTROL_Get_BM_Switch_CrossCh(i) , SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY( i )); // 2003.12.10
	}
	fprintf( fpt , " BM_Switch_Separate,Tag=%d,Side=%d,Count=%d,Sch=%d,Nch=%d,Scnt=%d\n" , SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSide() , SCHEDULER_CONTROL_Get_BM_Switch_SeparateCount() , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSch() , SCHEDULER_CONTROL_Get_BM_Switch_SeparateNch() , SCHEDULER_CONTROL_Get_BM_Switch_SeparateSCnt() );
//	fprintf( fpt , " BM_Switch_Use=" ); // 2008.11.24
//	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2008.11.24
//		fprintf( fpt , "[%d]" , SCHEDULER_CONTROL_Get_BM_Switch_UseSide(i) ); // 2008.11.24
//	} // 2008.11.24
//	fprintf( fpt , "\n" ); // 2008.11.24

	// 2014.10.15
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	fprintf( fpt , " SIDE DATA(APPEND_END,FM_OUT_FIRST,BATCHALL_SWITCH_BM_ONLY) " );
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		fprintf( fpt , "[%d=%d,%d,%d]" , i , SIGNAL_MODE_APPEND_END_GET(i) , SCHEDULER_Get_FM_OUT_FIRST(i) , SCHEDULER_Get_BATCHALL_SWITCH_BM_ONLY(i) );
	}
	fprintf( fpt , "\n" );

	// 2014.10.15
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	fprintf( fpt , " TMDEPTH DATA(TM_MidCount,TM_Pick_BM_Signal,TM_LastMove_Signal,PM_MidCount,RunPrm_TM_INTERLOCK_USE) " );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		fprintf( fpt , "[%d=%d,%d,%d,%d,%d]" , i , SCHEDULER_Get_TM_MidCount(i) , SCHEDULER_Get_TM_Pick_BM_Signal(i) , SCHEDULER_Get_TM_LastMove_Signal(i) , SCHEDULER_Get_PM_MidCount(i) , Get_RunPrm_TM_INTERLOCK_USE(i) );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , " ------------------------------------------------------------------\n" );
	fprintf( fpt , " FULLY_RETURN_MODE[%d] " , SCHEDULER_Get_FULLY_RETURN_MODE() );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%d]" , SCHEDULER_Get_FULLY_RETURN_CH(i) );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , " ------------------------------------------------------------------\n" );
	fprintf( fpt , " MULTI_GROUP_PICK,PLACE " );
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		fprintf( fpt , "[%d,%d]" , SCHEDULER_Get_MULTI_GROUP_PICK(i) , SCHEDULER_Get_MULTI_GROUP_PLACE(i) );
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , " ------------------------------------------------------------------\n" );

	if ( ALG_DATA_SUB_STYLE_0 == A0_SUB_ALGORITHM_NO4 ) {
		//
		fprintf( fpt , "LOT_POST_DIFF_SPAWNING SIDE=%d , PT=%d MODULE=" , xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_SIDE , xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_PT );
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_LOT_POST_DIFF_SPAWNING_MODULE[i] );
		fprintf( fpt , "\n" );
		//
		fprintf( fpt , "LOT_PRE_DIFF_SPAWNING SIDE=%d , PT=%d MODULE=" , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_SIDE , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_PT );
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_LOT_PRE_DIFF_SPAWNING_MODULE[i] );
		fprintf( fpt , "\n" );
		//
		fprintf( fpt , "LOT_ORDER_ALL_INDEX=%d INDEX=" , xinside_A0SUB4_LOT_ORDER_ALL_INDEX );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_LOT_ORDER_INDEX[i] );
		fprintf( fpt , "\n" );
		//=======================================================================================
		fprintf( fpt , "PM_LAST_LOT CLUSTERSIDE=%d,CLUSTERORDER=%d,CLUSTERINDEX=%d,CLUSTERSIDEACT=%d,CLUSTERSIDEACTSIDE=%d,CLUSTERSIDEACTINDEX=%d\n" ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDE ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERORDER ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERINDEX ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACT ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTSIDE ,
			xinside_A0SUB4_PM_LAST_LOT_CLUSTERSIDEACTINDEX );
		//=======================================================================================
		fprintf( fpt , "PICKFROMCM_RUN =" );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_PICKFROMCM_RUN[i] );
		fprintf( fpt , "\n" );
		//
		fprintf( fpt , "PICKFROMCM_PT =" );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_PICKFROMCM_PT[i] );
		fprintf( fpt , "\n" );
		//
		fprintf( fpt , "PICKFROMCM_RESULT =" );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) fprintf( fpt , "[%d]" , xinside_A0SUB4_PICKFROMCM_RESULT[i] );
		fprintf( fpt , "\n" );
		//=======================================================================================
	}

	fclose( fpt );
}
