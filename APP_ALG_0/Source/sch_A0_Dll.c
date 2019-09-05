//===========================================================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
//===========================================================================================================================
#include "sch_A0_dll.h"
//===========================================================================================================================
typedef BOOL (*DLL_USER_RECIPE_SP_N101_SEPSUPPLY)	 ( int , int * ); // 2007.01.12
//
typedef void (*DLL_USER_RECIPE_SP_N0S2_VERIFICATION) ( int , int , BOOL , int , int , int , int , int , char * , char * , char * , char * , char * , char * , int ); // 2007.08.31
typedef void (*DLL_USER_RECIPE_SP_N0S2_AUTOALIGNING) ( int , int , int , int , int , int , int , char * , char * , char * , char * , char * , char * , int * ); // 2007.08.31
//
typedef int	 (*DLL_USER_RECIPE_SP_N0S4_USERFUNCTION) ( BOOL , BOOL , int , int , int , CLUSTERStepTemplate2 * , int , char * , char * , char * , char * ); // 2007.02.14
typedef int	 (*DLL_USER_RECIPE_SP_N0S4_PREMAINSKIP)  ( int , int , int ); // 2007.03.07
typedef int	 (*DLL_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP)( int , int , int , int ); // 2009.06.11
typedef int	 (*DLL_USER_RECIPE_SP_N0S4_PICKFROMCM)	 ( BOOL , int , int , int , CLUSTERStepTemplate2 * , int , char * , char * , char * , char * ); // 2007.02.14
typedef int  (*DLL_USER_RECIPE_SP_N0S4_FLOWCONTROL)  ( int , int , int , int , char * , int , int , int * , int * , int * , int * , int * , char * , char * , char * , char * , char * ); // 2008.11.23
//
typedef int  (*DLL_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE) ( int , int ); // 2007.05.03
//
typedef int	 (*DLL_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP) ( int , int , int , int , int , CLUSTERStepTemplate2 * , int , char * , char * , char * , char * , CLUSTERStepTemplate2 * , int , char * , char * , char * , char * ); // 2009.06.11

//===========================================================================================================================
DLL_USER_RECIPE_SP_N101_SEPSUPPLY				DRV_USER_RECIPE_SP_N101_SEPSUPPLY; // 2007.01.12
//
DLL_USER_RECIPE_SP_N0S2_VERIFICATION			DRV_USER_RECIPE_SP_N0S2_VERIFICATION; // 2007.08.31
DLL_USER_RECIPE_SP_N0S2_AUTOALIGNING			DRV_USER_RECIPE_SP_N0S2_AUTOALIGNING; // 2007.08.31
//
DLL_USER_RECIPE_SP_N0S4_USERFUNCTION			DRV_USER_RECIPE_SP_N0S4_USERFUNCTION; // 2007.02.14
DLL_USER_RECIPE_SP_N0S4_PREMAINSKIP				DRV_USER_RECIPE_SP_N0S4_PREMAINSKIP; // 2007.03.07
DLL_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP			DRV_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP; // 2009.06.11
DLL_USER_RECIPE_SP_N0S4_PICKFROMCM				DRV_USER_RECIPE_SP_N0S4_PICKFROMCM; // 2007.02.14
//
DLL_USER_RECIPE_SP_N0S4_FLOWCONTROL				DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL; // 2008.11.23
//
DLL_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE		DRV_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE; // 2007.05.03
//
DLL_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP			DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP; // 2009.06.11
//===========================================================================================================================

void INIT_SCHEDULER_AL0_PART_SET_DLL_INSTANCE( HINSTANCE dllpt , int unload ) {
	//
	if ( !dllpt || unload ) {
		//
		DRV_USER_RECIPE_SP_N101_SEPSUPPLY			= FALSE; // 2007.01.12
		//
		DRV_USER_RECIPE_SP_N0S2_VERIFICATION		= FALSE; // 2007.08.31
		DRV_USER_RECIPE_SP_N0S2_AUTOALIGNING		= FALSE; // 2007.08.31
		//
		DRV_USER_RECIPE_SP_N0S4_USERFUNCTION		= FALSE; // 2007.02.14
		DRV_USER_RECIPE_SP_N0S4_PREMAINSKIP			= FALSE; // 2007.03.07
		DRV_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP		= FALSE; // 2009.06.11
		DRV_USER_RECIPE_SP_N0S4_PICKFROMCM			= FALSE; // 2007.02.14
		DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL			= FALSE; // 2008.11.23
		//
		DRV_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE	= FALSE; // 2007.05.03
		//
		DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP		= FALSE; // 2009.06.11
	}
	else {
		DRV_USER_RECIPE_SP_N101_SEPSUPPLY			= (DLL_USER_RECIPE_SP_N101_SEPSUPPLY)		GetProcAddress( dllpt , "USER_RECIPE_SP_N101_SEPSUPPLY" ); // 2007.01.12
		//
		DRV_USER_RECIPE_SP_N0S2_VERIFICATION		= (DLL_USER_RECIPE_SP_N0S2_VERIFICATION)	GetProcAddress( dllpt , "USER_RECIPE_SP_N0S2_VERIFICATION" ); // 2007.08.31
		DRV_USER_RECIPE_SP_N0S2_AUTOALIGNING		= (DLL_USER_RECIPE_SP_N0S2_AUTOALIGNING)	GetProcAddress( dllpt , "USER_RECIPE_SP_N0S2_AUTOALIGNING" ); // 2007.08.31
		//
		DRV_USER_RECIPE_SP_N0S4_USERFUNCTION		= (DLL_USER_RECIPE_SP_N0S4_USERFUNCTION)	GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_USERFUNCTION" ); // 2007.02.14
		DRV_USER_RECIPE_SP_N0S4_PREMAINSKIP			= (DLL_USER_RECIPE_SP_N0S4_PREMAINSKIP)		GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_PREMAINSKIP" ); // 2007.03.07
		DRV_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP		= (DLL_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP)	GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP" ); // 2009.06.11
		DRV_USER_RECIPE_SP_N0S4_PICKFROMCM			= (DLL_USER_RECIPE_SP_N0S4_PICKFROMCM)		GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_PICKFROMCM" ); // 2007.02.14
		DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL			= (DLL_USER_RECIPE_SP_N0S4_FLOWCONTROL)		GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_FLOWCONTROL" ); // 2008.11.23
		//
		DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP		= (DLL_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP)	GetProcAddress( dllpt , "USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP" ); // 2009.06.11
		//
		DRV_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE	= (DLL_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE)	GetProcAddress( dllpt , "USER_RECIPE_INVALID_DUMMY_OPERATE" ); // 2007.05.03
	}
}

//===========================================================================================================================
//=========================================================================================
// 2007.01.12
//=========================================================================================
BOOL USER_RECIPE_SP_N101_SEPSUPPLY( int side , int *count ) {
	if ( !DRV_USER_RECIPE_SP_N101_SEPSUPPLY ) return FALSE;
	return( (* DRV_USER_RECIPE_SP_N101_SEPSUPPLY ) ( side , count ) );
}

//=========================================================================================
// 2007.08.31
//=========================================================================================

void USER_RECIPE_SP_N0S2_VERIFICATION( int pos , int res , BOOL align , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe , char *pm5recipe , char *pm6recipe , int selpm ) {
	if ( !DRV_USER_RECIPE_SP_N0S2_VERIFICATION ) return;
	(* DRV_USER_RECIPE_SP_N0S2_VERIFICATION ) ( pos , res , align , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe , selpm );
}
//
void USER_RECIPE_SP_N0S2_AUTOALIGNING( int pos , int addr , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe  , char *pm5recipe  , char *pm6recipe , int *selpm ) {
	if ( !DRV_USER_RECIPE_SP_N0S2_AUTOALIGNING ) return;
	(* DRV_USER_RECIPE_SP_N0S2_AUTOALIGNING ) ( pos , addr , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe , selpm );
}

//=========================================================================================


int  USER_RECIPE_SP_N0S4_USERFUNCTION( BOOL runstyle , BOOL premode , int side , int pt , int clidx , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *clusterTuneData , char *grpUserData ) {
	if ( !DRV_USER_RECIPE_SP_N0S4_USERFUNCTION ) return SYS_SUCCESS;
	return( (* DRV_USER_RECIPE_SP_N0S4_USERFUNCTION ) ( runstyle , premode , side , pt , clidx , CLUSTER_INFO , LotSpecialData , LotUserData , ClusterUserData , clusterTuneData , grpUserData ) );
}

BOOL USER_RECIPE_SP_N0S4_PREMAINSKIP( int side , int ch , int clidx ) {
	if ( !DRV_USER_RECIPE_SP_N0S4_PREMAINSKIP ) return FALSE;
	return( (* DRV_USER_RECIPE_SP_N0S4_PREMAINSKIP ) ( side , ch , clidx ) );
}

int  USER_RECIPE_SP_N0S4_PICKFROMCM( BOOL diffside , int side , int pt , int clidx , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *clusterTuneData , char *grpUserData ) {
	if ( !DRV_USER_RECIPE_SP_N0S4_PICKFROMCM ) {
		if ( diffside ) return 1;
		else			return -1;
	}
	return( (* DRV_USER_RECIPE_SP_N0S4_PICKFROMCM ) ( diffside , side , pt , clidx , CLUSTER_INFO , LotSpecialData , LotUserData , ClusterUserData , clusterTuneData , grpUserData ) );
}

BOOL USER_EXIST_SP_N0S4_LOTPOSTPRESKIP() {
	if ( !DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP ) return FALSE;
	return TRUE;
}
//
int USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP( int side , int ch , int cluster_Index , int mode ) {
	// mode = 1 ( Has     WaferPre(O) , Has not WaferPost(X) )
		//    2 ( Has not WaferPre(X) , Has     WaferPost(O) )
		//    3 ( Has     WaferPre(O) , Has     WaferPost(O) )
	// return 0 : Not Skip
		//    1 : Skip WaferPre
		//    2 : Skip WaferPost
		//    3 : Skip WaferPre and WaferPost (Skip All)
	if ( !DRV_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP ) return 0;
	return( (* DRV_USER_RECIPE_SP_N0S4_WFRPREPOSTSKIP ) ( side , ch , cluster_Index , mode ) );
}

int  USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP( int LotPost_Side , int LotPost_Cluster_Index , int LotPre_Side , int LotPre_Cluster_Index , int mode ,
										CLUSTERStepTemplate2 *LotPost_CLUSTER_INFO , int LotPost_LotSpecialData , char *LotPost_LotUserData , char *LotPost_ClusterUserData , char *LotPost_ClusterTuneData , char *LotPost_GrpUserData ,
										CLUSTERStepTemplate2 *LotPre_CLUSTER_INFO , int LotPre_LotSpecialData , char *LotPre_LotUserData , char *LotPre_ClusterUserData , char *LotPre_ClusterTuneData , char *LotPre_GrpUserData ) {
	// mode = 1 ( Has     LotPost(O) , Has not LotPre(X) )
		//    2 ( Has not LotPost(X) , Has     LotPre(O) )
		//    3 ( Has     LotPost(O) , Has     LotPre(O) )
	// return 0 : Skip LotPost and LotPre(Skip All)
		//    1 : Skip LotPre
		//    2 : Skip LotPost
		//    3 : Not Skip
	if ( !DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP ) return 4;
	return( (* DRV_USER_RECIPE_SP_N0S4_LOTPOSTPRESKIP ) ( LotPost_Side , LotPost_Cluster_Index , LotPre_Side , LotPre_Cluster_Index , mode ,
		LotPost_CLUSTER_INFO , LotPost_LotSpecialData , LotPost_LotUserData , LotPost_ClusterUserData , LotPost_ClusterTuneData , LotPost_GrpUserData ,
		LotPre_CLUSTER_INFO , LotPre_LotSpecialData , LotPre_LotUserData , LotPre_ClusterUserData , LotPre_ClusterTuneData , LotPre_GrpUserData ) );
}

BOOL USER_EXIST_SP_N0S4_FLOWCONTROL() {
	if ( !DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL ) return FALSE;
	return TRUE;
}
//
int  USER_RECIPE_SP_N0S4_FLOWCONTROL( int side , int pointer , int pathin , int slotin , char *grpname , int runcount , int mode , int *TotalLoop , int *i2 , int *i3 , int *i4 , int *i5 , char *i7 , char *str1 , char *str2 , char *str3 , char *str4 ) {
	if ( !DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL ) return -1;
	return( (* DRV_USER_RECIPE_SP_N0S4_FLOWCONTROL ) ( side , pointer , pathin , slotin , grpname , runcount , mode , TotalLoop , i2 , i3 , i4 , i5 , i7 , str1 , str2 , str3 , str4 ) );
}
//===========================================================================================================================
//=========================================================================================
// 2007.05.03
//=========================================================================================
int  USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE( int bmindex , int bmslot ) {
	if ( !DRV_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE ) return 0;
	return( (* DRV_USER_RECIPE_SP_N0_INVALID_DUMMY_OPERATE ) ( bmindex , bmslot ) );
}
