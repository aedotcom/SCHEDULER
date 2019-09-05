#ifndef SCH_A0_DLL_H
#define SCH_A0_DLL_H

#include "INF_lottype.h"

//=========================================================================================================

void INIT_SCHEDULER_AL0_PART_SET_DLL_INSTANCE( HINSTANCE dllpt , int );

//=========================================================================================================

BOOL USER_RECIPE_SP_N101_SEPSUPPLY( int side , int *count ); // 2007.01.12

//=========================================================================================================

void USER_RECIPE_SP_N0S2_VERIFICATION( int pos , int res , BOOL align , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe , char *pm5recipe , char *pm6recipe , int selpm );
//
void USER_RECIPE_SP_N0S2_AUTOALIGNING( int pos , int addr , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe  , char *pm5recipe  , char *pm6recipe , int *selpm );

//=========================================================================================================

int  USER_RECIPE_SP_N0S4_USERFUNCTION( BOOL runstyle , BOOL premode , int side , int pt , int clidx , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *ClusterTuneData , char *GrpUserData );
//
BOOL USER_RECIPE_SP_N0S4_PREMAINSKIP( int side , int ch , int clidx );
//
int  USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP( int side , int ch , int clidx , int mode );
//
int  USER_RECIPE_SP_N0S4_PICKFROMCM( BOOL diffside , int side , int pt , int clidx , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *ClusterTuneData , char *GrpUserData );
//
BOOL USER_EXIST_SP_N0S4_FLOWCONTROL();
int  USER_RECIPE_SP_N0S4_FLOWCONTROL( int side , int pointer , int pathin , int slotin , char * , int runcount , int mode , int *totalloop , int *i2 , int *i3 , int *i4 , int *i5 , char *i7 , char * , char * , char * , char * ); // 2008.11.23

BOOL USER_EXIST_SP_N0S4_LOTPOSTPRESKIP();
int  USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( int oldside , int oldclidx , int side , int clidx , int mode ,
										CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *ClusterTuneData , char *GrpUserData ,
										CLUSTERStepTemplate2 *CLUSTER_INFO2 , int LotSpecialData2 , char *LotUserData2 , char *ClusterUserData2 , char *ClusterTuneData2 , char *GrpUserData2 );
//=========================================================================================================

int  USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE( int bmindex , int bmslot ); // 2007.05.03

//=========================================================================================================

#endif

