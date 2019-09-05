#ifndef SCH_A6_INIT_H
#define SCH_A6_INIT_H


#define		MAX_SUPPLY_GROUP		5

enum { CYCLON_GROUP_BM_IDLE , CYCLON_GROUP_BM_SUPPLYING , CYCLON_GROUP_BM_SUPPLYING_DUMMY , CYCLON_GROUP_BM_SUPPLY_FINISH , CYCLON_GROUP_BM_SUPPLY_FINISH_BM , CYCLON_GROUP_BM_PROCESSING };


void INIT_SCHEDULER_AL6_PART_DATA( int , int );


void SCH_CYCLON2_STATUS_VIEW_SUB( char *filename , BOOL append , int data , int t , int d1 , int d2 );

int  SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE_SUB( int t , int s , int p , BOOL , int *jix );

int SCH_CYCLON2_Get_PM_SLOT_INFO( int pmc );
int SCH_CYCLON2_Get_PM_RESULT_INFO( int pmc );
int SCH_CYCLON2_Set_PM_MOVE_ACTION( int pmc , int data );
void SCH_CYCLON2_Set_PM_MOVE_RESULT( int pmc , int data );


int SCH_CYCLON2_GROUP_GET_LAST_SUPPLY();

BOOL SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( int t , int s , int p , BOOL , int * );

BOOL SCH_CYCLON2_GROUP_SET_SUPPLY( int t , int s , int p , int *gidx , BOOL nomore );

BOOL SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( int bm , int *maxdumcnt , int *s , int *p );


int SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( int t , int *Dummy_count , int s , int p );
int SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( int t , int side , BOOL );

int SCH_CYCLON2_GROUP_STOCK_POSSIBLE( int t , int pmc , int bmc , BOOL );
int SCH_CYCLON2_GROUP_PROCESS_POSSIBLE( int t , int pmc );

BOOL SCH_CYCLON2_GROUP_SET_PROCESS( int t , int pmc , int g );
BOOL SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_PM( int s0 , int p0 , int pmc );
BOOL SCH_CYCLON2_GROUP_CHECK_HAVE_SAME_IN_BM( int s0 , int p0 , int pmc );

BOOL SCH_CYCLON2_GROUP_GET_PM_SLOT_INFO( int pmc , int *data );

BOOL SCH_CYCLON2_GROUP_SET_SUPPLY_TM( int t , int s , int p );

BOOL SCH_CYCLON2_GROUP_GET_NEED_DUMMY( int t0 , int *t );


void SCH_CYCLON2_Set_MDL_SELECT( int tms , int pmc , int bms , int stockbm , int res );

BOOL SCH_CYCLON2_GROUP_RETURN( int s , int p );

void SCH_CYCLON2_GROUP_SUPPLY_STOP( int s , int p );

BOOL SCH_CYCLON2_GROUP_SET_SUPPLY_MORE_PM( int t , int s , int p , int pmc , int ss );

BOOL SCH_CYCLON2_GROUP_WAIT_ANOTHER_WAFER_IN_PM( int t , int side , int pt , int pmc , int ss ); // 2013.05.28

BOOL SCH_CYCLON2_GROUP_AFTER_PICK_PM( int t , int side , int pt , int pmc , int ss ); // 2013.05.28
BOOL SCH_CYCLON2_GROUP_UNMATCHED_COUNT_RUN_STATUS( int t ); // 2013.05.28

int SCH_CYCLON2_GROUP_COUNT( int t , int index ); // 2013.06.26

/*
// 2014.07.10
void SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( int tms , int bmc ); // 2013.10.30
int SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( int tms ); // 2013.10.30

void SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( int tms , int bmc , int side , int pt ); // 2014.06.11
int SCH_BM_SUPPLY_LOCK_GET2_FOR_AL6( int tms , int side , int pt ); // 2014.06.11
*/

// 2014.07.10

void SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( int bmc );
int SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( int bmc );

void SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( int bmc , int side , int pt );
int SCH_BM_SUPPLY_LOCK_GET2_FOR_AL6( int tms , int side , int pt );

void SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( int bmc );

#endif

