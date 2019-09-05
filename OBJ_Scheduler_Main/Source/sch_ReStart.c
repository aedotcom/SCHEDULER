#include "default.h"

#include "EQ_Enum.h"

#include "Multijob.h"
#include "MR_Data.h"
#include "DLL_Interface.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "system_tag.h"
#include "sch_main.h"
#include "sch_cassmap.h"
#include "sch_sub.h"
#include "sch_Multi_ALIC.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prepost.h"
#include "sch_processmonitor.h"
#include "Multireg.h"
#include "FA_Handling.h"
#include "Gui_Handling.h"
#include "Errorcode.h"
#include "Resource.h"
#include "sch_HandOff.h"
#include "utility.h"

#include "INF_sch_CommonUser.h"

// RESTART MODE(controlmode)
//
//		EventString	MultiSideAction	AllData(+CM)
//	0	-			-				-
//	1	r			x				x
//	2	t			o				x
//	3	a			o				o
//===========================================================================================================================
int Get_Module_ID_From_String( int mode , char *mdlstr , int badch );
//===========================================================================================================================

char SCHEDULER_RESTART_FOLDER[64];
char SCHEDULER_RESTART_FILE[64];
char SCHEDULER_RESTART_BACKUP[64];
CRITICAL_SECTION CR_SCHEDULER_RESTART;

//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
void _iSCH_RESTART_GET_FILE_NAME( char *name , int cnt , int mdl , int slotarm ) {
	char nsub[32];
	//
	_snprintf( nsub , 31 , "%s_%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slotarm );
	_snprintf( name , cnt , SCHEDULER_RESTART_FILE , nsub );
	//
}

int SCH_RESTART_GET_IO_DATA( int ch , int sl , int *s , int *w , int *r ) {
	//
	*w = 0;
	//
	if      ( ch < PM1 ) {
		*s = ch - CM1;
		*w = sl;
		*r = WAFERRESULT_PRESENT;
	}
	else if ( ch == F_AL ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( ( sl == 1 ) ? F_AL : 1001 , 1 );
		*w = _i_SCH_IO_GET_MODULE_STATUS( ( sl == 1 ) ? F_AL : 1001 , 1 );
		*r = _i_SCH_IO_GET_MODULE_RESULT( ( sl == 1 ) ? F_AL : 1001 , 1 );
	}
	else if ( ch == F_IC ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( ch , sl );
		*w = _i_SCH_IO_GET_MODULE_STATUS( ch , sl );
		*r = _i_SCH_IO_GET_MODULE_RESULT( ch , sl );
	}
	else if ( ch == FM ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( 10000 , sl );
		*w = _i_SCH_IO_GET_MODULE_STATUS( 10000 , sl );
		*r = _i_SCH_IO_GET_MODULE_RESULT( 10000 , sl );
	}
	else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( ch , sl );
		*w = _i_SCH_IO_GET_MODULE_STATUS( ch , sl );
		*r = _i_SCH_IO_GET_MODULE_RESULT( ch , sl );
	}
	else if ( ( ch >= TM ) && ( ch < FM ) ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( 20000 + ch - TM , sl );
		*w = _i_SCH_IO_GET_MODULE_STATUS( 20000 + ch - TM , sl );
		*r = _i_SCH_IO_GET_MODULE_RESULT( 20000 + ch - TM , sl );
	}
	else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
		*s = _i_SCH_IO_GET_MODULE_SOURCE( ch , sl );
		*w = _i_SCH_IO_GET_MODULE_STATUS( ch , sl );
		*r = _i_SCH_IO_GET_MODULE_RESULT( ch , sl );
	}
	//
	return *w;
}

BOOL _iSCH_RESTART_PRCS_FILE_BACKUPUSE_SUB( BOOL backup , int side , int ch , int slot , char *group1 , char *group2 , char *recipe ) {
	char Imsi1[256];
	char Imsi2[256];
	char filenamesrc[256];
	char filenametrg[256];
	int errorcode;
	//
	if ( recipe == NULL ) return TRUE;
	if ( recipe[0] == 0 ) return TRUE;
	if ( recipe[0] == '*' ) return TRUE;
	if ( recipe[0] == '?' ) return TRUE;
	//
	switch( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ) {
	case 1 :
		sprintf( Imsi1 , "%d$%s$%s$%s" , side , group1 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , recipe );
		sprintf( Imsi2 , "%d$%s$%s$%s" , side , group2 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , recipe );
		break;
	case 2 :
	case 3 : // 2015.07.20
		sprintf( Imsi1 , "%d$%d$%s$%s$%s" , side , slot , group1 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , recipe );
		sprintf( Imsi2 , "%d$%d$%s$%s$%s" , side , slot , group2 , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , recipe );
		break;
	default :
		return TRUE;
		break;
	}
	//
	Change_Dirsep_to_Dollar( Imsi1 );
	Change_Dirsep_to_Dollar( Imsi2 );
	//
//	sprintf( filenamesrc , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Imsi1 ); // 2017.10.30
	sprintf( filenamesrc , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , Imsi1 ); // 2017.10.30
	//
	sprintf( filenametrg , "%s/Restart_%s.info" , SCHEDULER_RESTART_FOLDER , Imsi2 );
	//
	if ( backup ) {
		if ( !UTIL_CopyFile( ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 3 ) , filenamesrc , FALSE , filenametrg , FALSE , 2 , FALSE , &errorcode ) ) {
			_IO_CIM_PRINTF( "_iSCH_RESTART_PRCS_FILE_BACKUPUSE_SUB [B][%s]->[%s] Fail [%d] / side=%d,ch=%d,slot=%d,group=%s,%s,rcp=%s\n" , filenamesrc , filenametrg , errorcode , side , ch , slot , group1 , group2 , recipe );
			return FALSE;
		}
	}
	else {
		if ( !UTIL_CopyFile( FALSE , filenametrg , ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 3 ) , filenamesrc , FALSE , 2 , FALSE , &errorcode ) ) {
			_IO_CIM_PRINTF( "_iSCH_RESTART_PRCS_FILE_BACKUPUSE_SUB [U][%s]->[%s] Fail [%d] / side=%d,ch=%d,slot=%d,group=%s,%s,rcp=%s\n" , filenamesrc , filenametrg , errorcode , side , ch , slot , group1 , group2 , recipe );
			return FALSE;
		}
	}
	//
	return TRUE;
}


BOOL _iSCH_RESTART_PRCS_FILE_BACKUPUSE( BOOL backup , int side , int ch , int slot , char *group1 , char *group2 , char *recipe ) { // 2015.02.26
	//
	int z1 , z2;
	char Retdata[512];
	char FileName[512];
	char DirName1[512];
	char DirName2[512];
	//
	if ( recipe == NULL ) return TRUE;
	//
	z1 = 0;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( recipe , '|' , Retdata , z1 , 511 );
	//
	if ( z1 == z2 ) return TRUE;
	//
	do {
		//
		UTIL_EXTRACT_GROUP_FILE( group1 , Retdata , DirName1 , 511 , FileName , 511 );
		UTIL_EXTRACT_GROUP_FILE( group2 , Retdata , DirName2 , 511 , FileName , 511 );
		//
		_iSCH_RESTART_PRCS_FILE_BACKUPUSE_SUB( backup , side , ch , slot , DirName1 , DirName2 , FileName );
		//
		z1 = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( recipe , '|' , Retdata , z1 , 255 );
		//
		if ( z1 == z2 ) break;
		//
	}
	while( TRUE );
	//
	return TRUE;
}

int _iSCH_RESTART_MAKE_INFO_SUB( int synch , int mdl , int slotarm , int side , int pointer , int wafer , int subinf1 , int subinf2 , int subinf3 , int *id ) {
	char FileName[256];
	int k , l , pmc , r , slot;
	int rsource , rstatus , rresult;
	//
	FILE *fpt;
	//
	if ( !_SCH_COMMON_USE_EXTEND_OPTION( 2 , 0 ) ) return 1;
	//
	if ( side < 0 ) return 2;
	if ( side >= MAX_CASSETTE_SIDE ) return 3;
	//
	if ( pointer < 0 ) return 4;
	if ( pointer >= MAX_CASSETTE_SLOT_SIZE ) return 5;
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( side , pointer ) < 0 ) return 6;
	//
	_iSCH_RESTART_GET_FILE_NAME( FileName , 255 , mdl , slotarm );
	//
	fpt = fopen( FileName , "w" );
	//
	if ( fpt == NULL ) return 7;
	//
	SCH_RESTART_GET_IO_DATA( mdl , slotarm , &rsource , &rstatus , &rresult );
	//
	(*id)++;
	//
	fprintf( fpt , "<START>	%d\n" , *id );
	//=======================================================
	fprintf( fpt , "	INFOSYNCH	%d\n" , synch );
	fprintf( fpt , "	MODULEINFO	%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) );
	fprintf( fpt , "	ARMSLOTINFO	%d\n" , slotarm );
	fprintf( fpt , "	SUBINFO1	%d\n" , subinf1 );
	fprintf( fpt , "	SUBINFO2	%d\n" , subinf2 );
	fprintf( fpt , "	SUBINFO3	%d\n" , subinf3 );
	//=======================================================
	fprintf( fpt , "	SIDE	%d\n" , side );
	fprintf( fpt , "	POINT	%d\n" , pointer );
	fprintf( fpt , "	WAFER	%d\n" , wafer );
	//=======================================================
	fprintf( fpt , "	SOURCE	%d\n" , rsource );
	fprintf( fpt , "	STATUS	%d\n" , rstatus );
	fprintf( fpt , "	RESULT	%d\n" , rresult );
	//=======================================================
	fprintf( fpt , "	S-LOTLOG	%c%s%c\n" , '"' , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , '"' );
	fprintf( fpt , "	S-GROUP	%c%s%c\n" , '"' , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , '"' );
	fprintf( fpt , "	S-FA	%d\n" , _i_SCH_SYSTEM_FA_GET( side ) );
	fprintf( fpt , "	S-MODE	%d\n" , _i_SCH_SYSTEM_MODE_GET( side ) );
	fprintf( fpt , "	S-END	%d\n" , _i_SCH_SYSTEM_MODE_END_GET( side ) );
	fprintf( fpt , "	S-PMMODE	%d\n" , _i_SCH_SYSTEM_PMMODE_GET( side ) );
	fprintf( fpt , "	S-MOVEMODE	%d\n" , _i_SCH_SYSTEM_MOVEMODE_GET( side ) ); // 2013.09.03
	fprintf( fpt , "	S-JOBID	%c%s%c\n" , '"' , _i_SCH_SYSTEM_GET_JOB_ID( side ) , '"' );
	fprintf( fpt , "	S-MID	%c%s%c\n" , '"' , _i_SCH_SYSTEM_GET_MID_ID( side ) , '"' );
	//=======================================================
	fprintf( fpt , "	PATHIN	%d\n" , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) );
	fprintf( fpt , "	SLOTIN	%d\n" , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) );
	fprintf( fpt , "	PATHOUT	%d\n" , _i_SCH_CLUSTER_Get_PathOut( side , pointer ) );
	fprintf( fpt , "	SLOTOUT	%d\n" , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) );
	//=======================================================
	fprintf( fpt , "	PATHDO	%d\n" , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) );
	fprintf( fpt , "	PATHSTATUS	%d\n" , _i_SCH_CLUSTER_Get_PathStatus( side , pointer ) );
	//=======================================================
	fprintf( fpt , "	CLUSTERINDEX	%d\n" , _i_SCH_CLUSTER_Get_ClusterIndex( side , pointer ) );
	fprintf( fpt , "	SUPPLYID	%d\n" , _i_SCH_CLUSTER_Get_SupplyID( side , pointer ) );
	fprintf( fpt , "	SWITCHINOUT	%d\n" , _i_SCH_CLUSTER_Get_SwitchInOut( side , pointer ) );
	fprintf( fpt , "	FAILOUT	%d\n" , _i_SCH_CLUSTER_Get_FailOut( side , pointer ) );
	fprintf( fpt , "	EXTRA	%d\n" , _i_SCH_CLUSTER_Get_Extra( side , pointer ) );
	fprintf( fpt , "	PATHHSIDE	%d\n" , _i_SCH_CLUSTER_Get_PathHSide( side , pointer ) );
	fprintf( fpt , "	BUFFER	%d\n" , _i_SCH_CLUSTER_Get_Buffer( side , pointer ) );
	fprintf( fpt , "	OPTIMIZE	%d\n" , _i_SCH_CLUSTER_Get_Optimize( side , pointer ) );
	fprintf( fpt , "	STOCK	%d\n" , _i_SCH_CLUSTER_Get_Stock( side , pointer ) );
	//=======================================================
	fprintf( fpt , "	CP_CONTROL	%d\n" , _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer ) );
	fprintf( fpt , "	CP_PROCESS	%d\n" , _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer ) );
	//=======================================================
	//
	if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) != NULL ) {
		fprintf( fpt , "	EILINFO	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_LotSpecial( side , pointer ) != 0 ) {
		fprintf( fpt , "	LOT_SPECIAL	%d\n" , _i_SCH_CLUSTER_Get_LotSpecial( side , pointer ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_LotUserSpecial( side , pointer ) != NULL ) {
		fprintf( fpt , "	LOTUSERSPECIAL	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_LotUserSpecial( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_ClusterSpecial( side , pointer ) != 0 ) { // 2014.06.23
		fprintf( fpt , "	CLUSTER_SPECIAL	%d\n" , _i_SCH_CLUSTER_Get_ClusterSpecial( side , pointer ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_ClusterUserSpecial( side , pointer ) != NULL ) {
		fprintf( fpt , "	CLUSTERUSERSPECIAL	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_ClusterUserSpecial( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_ClusterTuneData( side , pointer ) != NULL ) {
		fprintf( fpt , "	CLUSTERTUNE	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_ClusterTuneData( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) != 0 ) {
		fprintf( fpt , "	EX_MTLOUT	%d\n" , _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pointer ) != 0 ) {
		fprintf( fpt , "	EX_CARRINDEX	%d\n" , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pointer ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_JobName( side , pointer ) != NULL ) {
		fprintf( fpt , "	EX_JOBNAME	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_JobName( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_RecipeName( side , pointer ) != NULL ) {
		fprintf( fpt , "	EX_RECIPENAME	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_RecipeName( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_PPID( side , pointer ) != NULL ) {
		fprintf( fpt , "	EX_PPID	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_PPID( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pointer ) != NULL ) {
		fprintf( fpt , "	EX_MATERIALID	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pointer ) , '"' );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , pointer ) != 0 ) {
		fprintf( fpt , "	EX_USER_CONTROL_MODE	%d\n" , _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , pointer ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_Ex_UserControl_Data( side , pointer ) != NULL ) {
		fprintf( fpt , "	EX_USER_CONTROL_DATA	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_Ex_UserControl_Data( side , pointer ) , '"' );
	}
	//
	//=======================================================================================
	/*
	// 2013.07.24
	if ( ( mdl >= BM1 ) && ( mdl < TM ) ) {
		if ( _i_SCH_PRM_GET_MODULE_GROUP( mdl ) > 0 ) { // 2013.07.01
			k = _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
		}
		else {
			k = _i_SCH_CLUSTER_Get_PathDo( side , pointer );
		}
	}
	else if ( ( mdl >= TM ) && ( mdl < FM ) ) {
		 k = _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
	}
	else if ( ( mdl >= PM1 ) && ( mdl < AL ) ) {
		 k = _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1;
	}
	else {
		 k = 0;
	}
	//
	r = 0;
	//
	if ( k < 0 ) k = 0;
	//
	for ( ; k < MAX_CLUSTER_DEPTH ; k++ ) {
		//
		if ( k >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) continue;
		//
		f = 0;
		//
		fprintf( fpt , "	FLOW_START	%d\n" , r + 1 );
		//
		for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
			//
			pmc = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , k , l );
			//
			if ( pmc > 0 ) {
				//
				f++;
				//
				fprintf( fpt , "	PROCESS_E	%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) );
				fprintf( fpt , "	RECIPE_IN	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) , '"' );
				fprintf( fpt , "	RECIPE_OUT	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) , '"' );
				fprintf( fpt , "	RECIPE_PRE	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) , '"' );
			}
			else if ( pmc < 0 ) {
				//
				f++;
				//
				fprintf( fpt , "	PROCESS_D	%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) );
				fprintf( fpt , "	RECIPE_IN	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) , '"' );
				fprintf( fpt , "	RECIPE_OUT	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) , '"' );
				fprintf( fpt , "	RECIPE_PRE	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) , '"' );
			}
			//
		}
		//
		if ( f > 0 ) {
			//
			fprintf( fpt , "	PATHRUN	%d\n" , _i_SCH_CLUSTER_Get_PathRun( side , pointer , k , -1 ) );
			//
			if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pointer , k ) != NULL ) {
				//
				f = strlen( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pointer , k ) );
				fprintf( fpt , "	PATHPARALLEL	" );
				for ( l = 0 ; l < f ; l++ ) {
					if ( l != 0 ) fprintf( fpt , "," );
					fprintf( fpt , "%d" , _i_SCH_CLUSTER_Get_PathProcessParallel( side , pointer , k )[l] );
				}
				fprintf( fpt , "\n" );
				//
			}
			//
		}
		//
		fprintf( fpt , "	FLOW_END	%d\n" , r + 1 );
		//
		r++;
	}
	//
	fprintf( fpt , "	PATHRANGE	%d\n" , r );
	*/
	//=======================================================================================
	// 2013.07.24
	//
	r = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
	slot = _i_SCH_CLUSTER_Get_SlotIn( side , pointer ); // 2014.10.30
	//
	for ( k = 0 ; k < r ; k++ ) {
		//
		fprintf( fpt , "	FLOW_START	%d\n" , k + 1 );
		//
		for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
			//
			pmc = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , k , l );
			//
			if ( pmc > 0 ) {
				//
				fprintf( fpt , "	PROCESS_E	%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) );
				fprintf( fpt , "	RECIPE_IN	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) , '"' );
				fprintf( fpt , "	RECIPE_OUT	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) , '"' );
				fprintf( fpt , "	RECIPE_PRE	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) , '"' );
				//
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) ); // 2014.10.30
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) ); // 2014.10.30
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) ); // 2014.10.30
				//
			}
			else if ( pmc < 0 ) {
				//
				fprintf( fpt , "	PROCESS_D	%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) );
				fprintf( fpt , "	RECIPE_IN	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) , '"' );
				fprintf( fpt , "	RECIPE_OUT	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) , '"' );
				fprintf( fpt , "	RECIPE_PRE	%c%s%c\n" , '"' , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) , '"' );
				//
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , -pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 0 ) ); // 2014.10.30
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , -pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 1 ) ); // 2014.10.30
				_iSCH_RESTART_PRCS_FILE_BACKUPUSE( TRUE , side , -pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , k , l , 2 ) ); // 2014.10.30
				//
			}
			//
		}
		//
		fprintf( fpt , "	PATHRUN	%d\n" , _i_SCH_CLUSTER_Get_PathRun( side , pointer , k , -1 ) );
		//
		if ( _i_SCH_CLUSTER_Get_PathProcessParallel( side , pointer , k ) != NULL ) {
			//
			fprintf( fpt , "	PATHPARALLEL	" );
			for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
				if ( l != 0 ) fprintf( fpt , "," );
//				fprintf( fpt , "%d" , _i_SCH_CLUSTER_Get_PathProcessParallel( side , pointer , k )[l] ); // 2015.04.10
				fprintf( fpt , "%d" , _i_SCH_CLUSTER_Get_PathProcessParallel2( side , pointer , k , l ) ); // 2015.04.10
			}
			fprintf( fpt , "\n" );
			//
		}
		//
		if ( _i_SCH_CLUSTER_Get_PathProcessDepth( side , pointer , k ) != NULL ) { // 2014.01.28
			//
			fprintf( fpt , "	PATHDEPTH	" );
			for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
				if ( l != 0 ) fprintf( fpt , "," );
//				fprintf( fpt , "%d" , _i_SCH_CLUSTER_Get_PathProcessDepth( side , pointer , k )[l] ); // 2015.04.10
				fprintf( fpt , "%d" , _i_SCH_CLUSTER_Get_PathProcessDepth2( side , pointer , k , l ) ); // 2015.04.10
			}
			fprintf( fpt , "\n" );
			//
		}
		//
		fprintf( fpt , "	FLOW_END	%d\n" , k + 1 );
		//
	}
	//
	fprintf( fpt , "	PATHRANGE	%d\n" , r );
	//=======================================================================================
	fprintf( fpt , "<END>	%d\n" , *id );
	//
	fclose( fpt );
	//
	return 0;
}

void _iSCH_RESTART_MAKE_INFO( int synch , int mdl , int slotarm , int side , int pointer , int wafer , int subinf1 , int subinf2 , int subinf3 , int *id ) {
	FILE *fpt;
	char bufsub[32];
	char FileName[256];
	//
	if ( 0 == _iSCH_RESTART_MAKE_INFO_SUB( synch , mdl , slotarm , side , pointer , wafer , subinf1 , subinf2 , subinf3 , id ) ) {
		//
		sprintf( bufsub , "%d" , side );
		sprintf( FileName , SCHEDULER_RESTART_FILE , bufsub );
		//
		fpt = fopen( FileName , "a" );
		//
		if ( fpt == NULL ) return;
		//
		fprintf( fpt , "ID:%d	SYNCH:%d	MODULE:%s	SLOTARM:%d	SIDE:%d	POINTER:%d	WAFER:%d	S1:%d	S2:%d	S3:%d\n" , *id , synch , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) , slotarm , side , pointer , wafer , subinf1 , subinf2 , subinf3 );
		//=======================================================
		fclose( fpt );
		//=======================================================
	}
	//
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int SCH_RESTART_GET_DATA_INFO( int controlmode , int side , int pointer , int fmdl , int fslotarm , int smdl , int sslotarm , int rside , int rwfr ) {
	HFILE hFile;
	char Buffer[256];
	char chr_return1[512];
	char chr_return2[512];
	char group[256] = {0,};
	int  ReadCnt;
	BOOL FileEnd = TRUE;
	int  Line;
	//
	int fside , fpointer , wafer;
	int infosynch , moduleinfo , armslotinfo , subinf1 , subinf2 , subinf3;
	int flow_cldepth , flow_pmdepth;
	int wside , wwafer;
	int slot , pmc; // 2014.10.30
	//
	if ( !_SCH_COMMON_USE_EXTEND_OPTION( 2 , 0 ) ) return 1;
	//
	_iSCH_RESTART_GET_FILE_NAME( Buffer , 255 , fmdl , fslotarm );
	//
	hFile = _lopen( Buffer , OF_READ );
	//
	if ( hFile == -1 ) return 2;
	//
	flow_cldepth = -1;
	//
	for ( Line = 1 ; FileEnd ; Line++ ) {
		//
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		//
		if ( ReadCnt < 2 ) continue;
		//
		if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
			_lclose( hFile );
			return 3;
		}
		//
		if ( STRCMP_L( chr_return1 , "<START>" ) ) continue;
		if ( STRCMP_L( chr_return1 , "<END>" ) ) continue;
		//
		if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
			_lclose( hFile );
			return 4;
		}
		//
		UTIL_Extract_Reset_String( chr_return2 );
		//
		//=======================================================
		if      ( STRCMP_L( chr_return1 , "INFOSYNCH" ) ) {
			infosynch = atoi( chr_return2 );
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "MODULEINFO" ) ) {
			moduleinfo = Get_Module_ID_From_String( 0 , chr_return2 , -1 );
		}
		else if ( STRCMP_L( chr_return1 , "ARMSLOTINFO" ) ) {
			armslotinfo = atoi( chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "SUBINFO1" ) ) {
			subinf1 = atoi( chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "SUBINFO2" ) ) {
			subinf2 = atoi( chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "SUBINFO3" ) ) {
			subinf3 = atoi( chr_return2 );
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "S-LOTLOG" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-GROUP" ) ) {
			strcpy( group , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "S-FA" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-MODE" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-END" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-PMMODE" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-JOBID" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "S-MID" ) ) {
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "SIDE" ) ) {
			fside = atoi( chr_return2 );
			//
			if ( controlmode >= 2 ) { // 2012.07.21
				if ( fside != side ) {
					_lclose( hFile );
					return 99;
				}
			}
			//
		}
		else if ( STRCMP_L( chr_return1 , "POINT" ) ) {
			fpointer = atoi( chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "WAFER" ) ) {
			wafer = atoi( chr_return2 );
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "SOURCE" ) ) {
			wside = atoi( chr_return2 );
			//
			if ( rside >= 0 ) {
				if ( wside != rside ) {
					_lclose( hFile );
					return 8;
				}
			}
		}
		else if ( STRCMP_L( chr_return1 , "STATUS" ) ) {
			wwafer = atoi( chr_return2 );
			//
			if ( rwfr >= 0 ) {
				if ( wwafer != rwfr ) {
					_lclose( hFile );
					return 9;
				}
			}
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "PATHIN" ) ) {
			_i_SCH_CLUSTER_Set_PathIn( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "SLOTIN" ) ) {
			//
			slot = atoi( chr_return2 ); // 2014.10.30
			//
			_i_SCH_CLUSTER_Set_SlotIn( side , pointer , slot );
			//
		}
		else if ( STRCMP_L( chr_return1 , "PATHOUT" ) ) {
			_i_SCH_CLUSTER_Set_PathOut( side , pointer , atoi( chr_return2 ) );
			//
			//
		}
		else if ( STRCMP_L( chr_return1 , "SLOTOUT" ) ) {
			_i_SCH_CLUSTER_Set_SlotOut( side , pointer , atoi( chr_return2 ) );
			//
			//
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "PATHRANGE" ) ) {
			_i_SCH_CLUSTER_Set_PathRange( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "PATHDO" ) ) {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "PATHSTATUS" ) ) {
			_i_SCH_CLUSTER_Set_PathStatus( side , pointer , atoi( chr_return2 ) );
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "CLUSTERINDEX" ) ) {
			_i_SCH_CLUSTER_Set_ClusterIndex( side , pointer , atoi( chr_return2 ) );
			//
			//
		}
		else if ( STRCMP_L( chr_return1 , "SUPPLYID" ) ) {
			_i_SCH_CLUSTER_Set_SupplyID( side , pointer , atoi( chr_return2 ) );
			//
			//
		}
		else if ( STRCMP_L( chr_return1 , "SWITCHINOUT" ) ) {
			_i_SCH_CLUSTER_Set_SwitchInOut( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "FAILOUT" ) ) {
			_i_SCH_CLUSTER_Set_FailOut( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "EXTRA" ) ) {
			_i_SCH_CLUSTER_Set_Extra( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "PATHHSIDE" ) ) {
			_i_SCH_CLUSTER_Set_PathHSide( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "BUFFER" ) ) {
			_i_SCH_CLUSTER_Set_Buffer( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "OPTIMIZE" ) ) {
			_i_SCH_CLUSTER_Set_Optimize( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "STOCK" ) ) {
			_i_SCH_CLUSTER_Set_Stock( side , pointer , atoi( chr_return2 ) );
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "CP_CONTROL" ) ) {
			_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , pointer , -1 );
			//
			//
		}
		else if ( STRCMP_L( chr_return1 , "CP_PROCESS" ) ) {
			_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , pointer , -1 );
			//
		}
		//=======================================================
		else if ( STRCMP_L( chr_return1 , "EILINFO" ) ) {
			_i_SCH_CLUSTER_Set_Ex_EILInfo( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EILUNIQUEID" ) ) {
			_i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( side , pointer , atoi(chr_return2) );
		}
		else if ( STRCMP_L( chr_return1 , "LOT_SPECIAL" ) ) {
			_i_SCH_CLUSTER_Set_LotSpecial( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "LOTUSERSPECIAL" ) ) {
			_i_SCH_CLUSTER_Set_LotUserSpecial( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "CLUSTER_SPECIAL" ) ) { // 2014.06.23
			_i_SCH_CLUSTER_Set_ClusterSpecial( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "CLUSTERUSERSPECIAL" ) ) {
			_i_SCH_CLUSTER_Set_ClusterUserSpecial( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "CLUSTERTUNE" ) ) {
			_i_SCH_CLUSTER_Set_ClusterTuneData( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EX_MTLOUT" ) ) {
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "EX_CARRINDEX" ) ) {
			_i_SCH_CLUSTER_Set_Ex_CarrierIndex( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "EX_JOBNAME" ) ) {
			_i_SCH_CLUSTER_Set_Ex_JobName( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EX_PPID" ) ) {
			_i_SCH_CLUSTER_Set_Ex_PPID( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EX_RECIPENAME" ) ) {
			_i_SCH_CLUSTER_Set_Ex_RecipeName( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EX_MATERIALID" ) ) {
			_i_SCH_CLUSTER_Set_Ex_MaterialID( side , pointer , chr_return2 );
		}
		else if ( STRCMP_L( chr_return1 , "EX_USER_CONTROL_MODE" ) ) {
			_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( side , pointer , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "EX_USER_CONTROL_DATA" ) ) {
			_i_SCH_CLUSTER_Set_Ex_UserControl_Data( side , pointer , chr_return2 );
		}
		//
		//=======================================================================================
		else if ( STRCMP_L( chr_return1 , "FLOW_START" ) ) {
			flow_cldepth++;
			flow_pmdepth = -1;
		}
		else if ( STRCMP_L( chr_return1 , "FLOW_END" ) ) {
		}
		else if ( STRCMP_L( chr_return1 , "PROCESS_E" ) ) {
			//
			flow_pmdepth++;
			//
			pmc = Get_Module_ID_From_String( 0 , chr_return2 , -1 ); // 2014.10.30
			//
			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , flow_cldepth , flow_pmdepth , pmc );
		}
		else if ( STRCMP_L( chr_return1 , "PROCESS_D" ) ) {
			//
			flow_pmdepth++;
			//
			pmc = Get_Module_ID_From_String( 0 , chr_return2 , -1 ); // 2014.10.30
			//
//			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , flow_cldepth , flow_pmdepth , -Get_Module_ID_From_String( 0 , chr_return2 , -1 ) );
			_i_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , flow_cldepth , flow_pmdepth , pmc );
		}
		else if ( STRCMP_L( chr_return1 , "RECIPE_IN" ) ) {
			//
//			if ( group[0] == 0 ) {
				_i_SCH_CLUSTER_Set_PathProcessData_JustIn( side , pointer , flow_cldepth , flow_pmdepth , chr_return2 );
//			}
//			else {
//				sprintf( chr_return1 , "%s:(%s)" , chr_return2 , group );
//				_i_SCH_CLUSTER_Set_PathProcessData_JustIn( side , pointer , flow_cldepth , flow_pmdepth , chr_return1 );
//			}
			//
			_iSCH_RESTART_PRCS_FILE_BACKUPUSE( FALSE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , group , chr_return2 ); // 2014.10.30
			//
		}
		else if ( STRCMP_L( chr_return1 , "RECIPE_OUT" ) ) {
			//
//			if ( group[0] == 0 ) {
				_i_SCH_CLUSTER_Set_PathProcessData_JustPost( side , pointer , flow_cldepth , flow_pmdepth , chr_return2 );
//			}
//			else {
//				sprintf( chr_return1 , "%s:(%s)" , chr_return2 , group );
//				_i_SCH_CLUSTER_Set_PathProcessData_JustPost( side , pointer , flow_cldepth , flow_pmdepth , chr_return1 );
//			}
			//
			_iSCH_RESTART_PRCS_FILE_BACKUPUSE( FALSE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , group , chr_return2 ); // 2014.10.30
			//
		}
		else if ( STRCMP_L( chr_return1 , "RECIPE_PRE" ) ) {
			//
//			if ( group[0] == 0 ) {
				_i_SCH_CLUSTER_Set_PathProcessData_JustPre( side , pointer , flow_cldepth , flow_pmdepth , chr_return2 );
//			}
//			else {
//				sprintf( chr_return1 , "%s:(%s)" , chr_return2 , group );
//				_i_SCH_CLUSTER_Set_PathProcessData_JustPre( side , pointer , flow_cldepth , flow_pmdepth , chr_return1 );
//			}
			//
			_iSCH_RESTART_PRCS_FILE_BACKUPUSE( FALSE , side , pmc , slot , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , group , chr_return2 ); // 2014.10.30
			//
		}
		else if ( STRCMP_L( chr_return1 , "PATHRUN" ) ) {
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , flow_cldepth , -1 , atoi( chr_return2 ) );
		}
		else if ( STRCMP_L( chr_return1 , "PATHPARALLEL" ) ) {
			//
			// ??
			//
			_i_SCH_CLUSTER_Set_PathProcessParallel( side , pointer , flow_cldepth , "" );
		}
		else if ( STRCMP_L( chr_return1 , "PATHDEPTH" ) ) {
			//
			// ??
			//
			_i_SCH_CLUSTER_Set_PathProcessDepth( side , pointer , flow_cldepth , "" );
		}
		//=======================================================================================
	}
	//
	_lclose( hFile );
	//
	//---------------------------------------------------------------------------------------------
	if      ( smdl < PM1 ) {
		_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_READY );
		_i_SCH_CLUSTER_Set_PathDo( side , pointer , 0 );
	}
	else if ( smdl == F_AL ) {
		//
		_i_SCH_MODULE_Set_MFALS_SIDE( sslotarm , side );
		_i_SCH_MODULE_Set_MFALS_POINTER( sslotarm , pointer );
		_i_SCH_MODULE_Set_MFALS_BM( sslotarm , subinf1 );
		_i_SCH_MODULE_Set_MFALS_WAFER( sslotarm , wafer );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
	}
	else if ( smdl == F_IC ) {
		//
		_i_SCH_MODULE_Set_MFIC_SIDE( sslotarm , side );
		_i_SCH_MODULE_Set_MFIC_POINTER( sslotarm , pointer );
		_i_SCH_MODULE_Set_MFIC_WAFER( sslotarm , wafer );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
	}
	else if ( smdl == FM ) {
		//
		_i_SCH_MODULE_Set_FM_SIDE_WAFER( sslotarm , side , wafer );
		_i_SCH_MODULE_Set_FM_POINTER( sslotarm , pointer );
		_i_SCH_MODULE_Set_FM_MODE( sslotarm , subinf1 );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
	}
	else if ( ( smdl >= BM1 ) && ( smdl < TM ) ) {
		//
		_i_SCH_MODULE_Set_BM_SIDE( smdl , sslotarm , side );
		_i_SCH_MODULE_Set_BM_POINTER( smdl , sslotarm , pointer );
		_i_SCH_MODULE_Set_BM_WAFER_STATUS( smdl , sslotarm , wafer , subinf1 );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
		/*
		// 2013.07.24
		if ( ( fmdl >= PM1 ) && ( fmdl < AL ) ) {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , 1 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , 0 );
		}
		*/
		//
	}
	else if ( ( smdl >= TM ) && ( smdl < FM ) ) {
		//
		_i_SCH_MODULE_Set_TM_SIDE( smdl - TM , sslotarm , side );
		_i_SCH_MODULE_Set_TM_POINTER( smdl - TM , sslotarm , pointer );
		_i_SCH_MODULE_Set_TM_PATHORDER( smdl - TM , sslotarm , subinf1 );
		_i_SCH_MODULE_Set_TM_TYPE( smdl - TM , sslotarm , subinf2 );
		_i_SCH_MODULE_Set_TM_OUTCH( smdl - TM , sslotarm , subinf3 );
		_i_SCH_MODULE_Set_TM_WAFER( smdl - TM , sslotarm , wafer );
		//
		_i_SCH_MODULE_Inc_TM_OutCount( side );
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
		/*
		// 2013.07.24
		if ( ( fmdl >= PM1 ) && ( fmdl < AL ) ) {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , 2 );
		}
		else {
			_i_SCH_CLUSTER_Set_PathDo( side , pointer , 1 );
		}
		*/
		//
	}
	else if ( ( smdl >= PM1 ) && ( smdl < AL ) ) {
		//
		_i_SCH_MODULE_Set_PM_SIDE( smdl , sslotarm-1 , side );
		_i_SCH_MODULE_Set_PM_POINTER( smdl , sslotarm-1 , pointer );
		_i_SCH_MODULE_Set_PM_PATHORDER( smdl , sslotarm-1 , subinf1 );
		_i_SCH_MODULE_Set_PM_STATUS( smdl , sslotarm-1 , subinf2 );
		_i_SCH_MODULE_Set_PM_WAFER( smdl , sslotarm-1 , wafer );
		//
		_i_SCH_MODULE_Inc_TM_OutCount( side );
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_OutCount( side );
		//
		/*
		// 2013.07.24
		_i_SCH_CLUSTER_Set_PathDo( side , pointer , 1 );
		*/
		//
	}
	//---------------------------------------------------------------------------------------------
	return 0;
}



//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
//=========================================================================================================================================================
int _sch_Restart_Find_Module_IO( int fmode , int side , int pointer , int wafer , int *retch , int *retslot ) {
	int c , i , wsrc , wsts , wsre;
	//
	switch ( fmode ) {
	case F_AL :
		//
		for ( i = 1 ; i <= 2 ; i++ ) {
			//
			IO_WAFER_DATA_GET_TO_CHAMBER( ( i == 1 ) ? F_AL : 1001 , 1 , &wsrc , &wsts , &wsre );
			//
			if ( wsts != wafer ) continue;
			//
			if ( wsrc >= MAX_CASSETTE_SIDE ) {
				if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
			}
			else {
				if ( wsrc != side ) continue;
				//
				*retch = F_AL;
				*retslot = i;
				//
				return TRUE;
				//
			}
			//
		}
		//
		break;

	case F_IC :
		//
		for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; i++ ) {
			//
			wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , i );
			wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , i );
			//
			if ( wsts != wafer ) continue;
			//
			if ( wsrc >= MAX_CASSETTE_SIDE ) {
				if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
			}
			else {
				if ( wsrc != side ) continue;
			}
			//
			*retch = F_IC;
			*retslot = i;
			//
			return TRUE;
			//
		}
		//
		break;

	case BM1 :
		//
		for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( c ) ) continue;
			//
			if ( SCH_Inside_ThisIs_BM_OtherChamber( c , 0 ) ) continue;
			//
			for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( c ) ; i++ ) {
				//
				IO_WAFER_DATA_GET_TO_BM( c , i , &wsrc , &wsts , &wsre );
				//
				if ( wsts != wafer ) continue;
				//
				if ( wsrc >= MAX_CASSETTE_SIDE ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
				}
				else {
					if ( wsrc != side ) continue;
				}
				//
				*retch = c;
				*retslot = i;
				//
				return TRUE;
				//
			}
			//
		}
		//
		break;

	case FM :
		//
		for ( i = 0 ; i < MAX_FINGER_FM ; i++ ) {
			//
			wsrc = WAFER_SOURCE_IN_FM( 0 , i );
			wsts = WAFER_STATUS_IN_FM( 0 , i );
			//
			if ( wsts != wafer ) continue;
			//
			if ( wsrc >= MAX_CASSETTE_SIDE ) {
				if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
			}
			else {
				if ( wsrc != side ) continue;
			}
			//
			*retch = FM;
			*retslot = i;
			//
			return TRUE;
			//
		}
		//
		break;

	case TM :
		//
		for ( c = 0 ; c < MAX_TM_CHAMBER_DEPTH ; c++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( TM + c ) ) continue;
			//
			for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
				//
				if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
				//
				IO_WAFER_DATA_GET_TO_TM( c , i , 0 , &wsrc , &wsts , &wsre );
				//
				if ( wsts != wafer ) continue;
				//
				if ( wsrc >= MAX_CASSETTE_SIDE ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
				}
				else {
					if ( wsrc != side ) continue;
				}
				//
				*retch = TM + c;
				*retslot = i;
				//
				return TRUE;
				//
			}
		}
		//
		break;

	case PM1 :
		//
		for ( c = PM1 ; c < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; c++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( c ) ) continue;
			//
			for ( i = 1 ; i <= MAX_PM_SLOT_DEPTH ; i++ ) {
				//
				wsrc = _i_SCH_IO_GET_MODULE_SOURCE( c , i );
				wsts = _i_SCH_IO_GET_MODULE_STATUS( c , i );
				//
				if ( wsts != wafer ) continue;
				//
				if ( wsrc >= MAX_CASSETTE_SIDE ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) < BM1 ) continue;
				}
				else {
					if ( wsrc != side ) continue;
				}
				//
				*retch = c;
				*retslot = i;
				//
				return TRUE;
				//
			}
		}
		//
		break;
	}
	//
	return FALSE;
}


void SCH_RESTART_MAKE_INFO() {
	int c , i , side , wafer , pointer , subinf1 , subinf2 , subinf3 , l , retch , retslot;
	//
	l = 0;
	//=========================================================================================================================================================
	for ( c = 0 ; c < MAX_CASSETTE_SIDE ; c++ ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( c , i ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( c , i ) != SCHEDULER_READY ) {
				if ( _i_SCH_CLUSTER_Get_PathStatus( c , i ) != SCHEDULER_SUPPLY ) {
					if ( _i_SCH_CLUSTER_Get_PathStatus( c , i ) != SCHEDULER_STARTING ) continue;
				}
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( c , i ) < CM1 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( c , i ) > PM1 ) continue;
			//
			side    = c;
			wafer   = _i_SCH_CLUSTER_Get_SlotIn( c , i );
			pointer = i;
			subinf1 = 0;
			subinf2 = 0;
			subinf3 = 0;
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( c , i ) , wafer ) == CWM_PRESENT ) {
				//
				_iSCH_RESTART_MAKE_INFO( 1 , _i_SCH_CLUSTER_Get_PathIn( c , i ) , wafer , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			else {
				//
				if ( _sch_Restart_Find_Module_IO( FM , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				_iSCH_RESTART_MAKE_INFO( 0 , _i_SCH_CLUSTER_Get_PathIn( c , i ) , wafer , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			//
		}
		//
	}
	//=========================================================================================================================================================
	for ( i = 0 ; i < MAX_FINGER_FM ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_FM_WAFER( i ) <= 0 ) continue;
		//
		side    = _i_SCH_MODULE_Get_FM_SIDE( i );
		wafer   = _i_SCH_MODULE_Get_FM_WAFER( i );
		pointer = _i_SCH_MODULE_Get_FM_POINTER( i );
		subinf1 = _i_SCH_MODULE_Get_FM_MODE( i );
		subinf2 = 0;
		subinf3 = 0;
		//
		if ( WAFER_STATUS_IN_FM( 0 , i ) > 0 ) {
			//
			_iSCH_RESTART_MAKE_INFO( 1 , FM , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		else {
			//
			if ( _sch_Restart_Find_Module_IO( F_AL , side , pointer , wafer , &retch , &retslot ) ) {
				_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				continue;
			}
			//
			if ( _sch_Restart_Find_Module_IO( F_IC , side , pointer , wafer , &retch , &retslot ) ) {
				_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				continue;
			}
			//
			if ( _sch_Restart_Find_Module_IO( BM1 , side , pointer , wafer , &retch , &retslot ) ) {
				_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				continue;
			}
			//
			_iSCH_RESTART_MAKE_INFO( 0 , FM , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		//
	}
	//=========================================================================================================================================================
	for ( i = 1 ; i <= 2 ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_MFALS_WAFER( i ) <= 0 ) continue;
		//
		side    = _i_SCH_MODULE_Get_MFALS_SIDE( i );
		wafer   = _i_SCH_MODULE_Get_MFALS_WAFER( i );
		pointer = _i_SCH_MODULE_Get_MFALS_POINTER( i );
		subinf1 = _i_SCH_MODULE_Get_MFALS_BM( i );
		subinf2 = 0;
		subinf3 = 0;
		//
		if ( _i_SCH_IO_GET_MODULE_STATUS( ( i == 1 ) ? F_AL : 1001 , 1 ) > 0 ) {
			//
			_iSCH_RESTART_MAKE_INFO( 1 , F_AL , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		else {
			//
			if ( _sch_Restart_Find_Module_IO( FM , side , pointer , wafer , &retch , &retslot ) ) {
				_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				continue;
			}
			//
			_iSCH_RESTART_MAKE_INFO( 0 , F_AL , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		//
	}
	//=========================================================================================================================================================
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_MFIC_WAFER( i ) <= 0 ) continue;
		//
		side    = _i_SCH_MODULE_Get_MFIC_SIDE( i );
		wafer   = _i_SCH_MODULE_Get_MFIC_WAFER( i );
		pointer = _i_SCH_MODULE_Get_MFIC_POINTER( i );
		subinf1 = 0;
		subinf2 = 0;
		subinf3 = 0;
		//
		if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , i ) > 0 ) {
			//
			_iSCH_RESTART_MAKE_INFO( 1 , F_IC , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		else {
			//
			if ( _sch_Restart_Find_Module_IO( FM , side , pointer , wafer , &retch , &retslot ) ) {
				_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				continue;
			}
			//
			_iSCH_RESTART_MAKE_INFO( 0 , F_IC , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
			//
		}
		//
	}
	//=========================================================================================================================================================
	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( c ) ) continue;
		//
		if ( SCH_Inside_ThisIs_BM_OtherChamber( c , 0 ) ) continue;
		//
		for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( c ) ; i++ ) {
			//
			if ( _i_SCH_MODULE_Get_BM_WAFER( c , i ) <= 0 ) continue;
			//
			side    = _i_SCH_MODULE_Get_BM_SIDE( c , i );
			wafer   = _i_SCH_MODULE_Get_BM_WAFER( c , i );
			pointer = _i_SCH_MODULE_Get_BM_POINTER( c , i );
			subinf1 = _i_SCH_MODULE_Get_BM_STATUS( c , i );
			subinf2 = 0;
			subinf3 = 0;
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( c , i ) > 0 ) {
				//
				_iSCH_RESTART_MAKE_INFO( 1 , c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			else {
				//
				if ( _sch_Restart_Find_Module_IO( FM , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				if ( _sch_Restart_Find_Module_IO( TM , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				_iSCH_RESTART_MAKE_INFO( 0 , c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			//
		}
	}
	//=========================================================================================================================================================
	for ( c = 0 ; c < MAX_TM_CHAMBER_DEPTH ; c++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( TM + c ) ) continue;
		//
		for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
			//
			if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
			//
			if ( _i_SCH_MODULE_Get_TM_WAFER( c , i ) <= 0 ) continue;
			//
			side    = _i_SCH_MODULE_Get_TM_SIDE( c , i );
			wafer   = _i_SCH_MODULE_Get_TM_WAFER( c , i );
			pointer = _i_SCH_MODULE_Get_TM_POINTER( c , i );
			subinf1 = _i_SCH_MODULE_Get_TM_PATHORDER( c , i );
			subinf2 = _i_SCH_MODULE_Get_TM_TYPE( c , i );
			subinf3 = _i_SCH_MODULE_Get_TM_OUTCH( c , i );
			//
			if ( WAFER_STATUS_IN_TM( c , i ) > 0 ) {
				//
				_iSCH_RESTART_MAKE_INFO( 1 , TM + c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			else {
				//
				if ( _sch_Restart_Find_Module_IO( BM1 , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				if ( _sch_Restart_Find_Module_IO( PM1 , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				_iSCH_RESTART_MAKE_INFO( 0 , TM + c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
		}
	}
	//=========================================================================================================================================================
	for ( c = PM1 ; c < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; c++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( c ) ) continue;
		//
		for ( i = 1 ; i <= MAX_PM_SLOT_DEPTH ; i++ ) {
			//
			if ( _i_SCH_MODULE_Get_PM_WAFER( c , i-1 ) <= 0 ) continue;
			//
			side    = _i_SCH_MODULE_Get_PM_SIDE( c , i-1 );
			wafer   = _i_SCH_MODULE_Get_PM_WAFER( c , i-1 );
			pointer = _i_SCH_MODULE_Get_PM_POINTER( c , i-1 );
			subinf1 = _i_SCH_MODULE_Get_PM_PATHORDER( c , i-1 );
			subinf2 = _i_SCH_MODULE_Get_PM_STATUS( c , i-1 );
			subinf3 = 0;
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( c , i ) > 0 ) {
				//
				_iSCH_RESTART_MAKE_INFO( 1 , c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
			else {
				//
				if ( _sch_Restart_Find_Module_IO( TM , side , pointer , wafer , &retch , &retslot ) ) {
					_iSCH_RESTART_MAKE_INFO( 2 , retch , retslot , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
					continue;
				}
				//
				_iSCH_RESTART_MAKE_INFO( 0 , c , i , side , pointer , wafer , subinf1 , subinf2 , subinf3 , &l );
				//
			}
		}
	}
}


//===========================================================================================================================
void SCH_RESTART_DELETE_INFO( BOOL cruse ) {
	//
	char FileName[512];
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	//
	if ( cruse ) EnterCriticalSection( &CR_SCHEDULER_RESTART );
	//
	sprintf( FileName , SCHEDULER_RESTART_FILE , "*" );
	hFind = FindFirstFile( FileName , &fd );
	//
	while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			//
			sprintf( FileName , "%s/%s" , SCHEDULER_RESTART_FOLDER , fd.cFileName );
			DeleteFile( FileName );
			//
		}
		bRet = FindNextFile( hFind , &fd );
	}
	//
	FindClose( hFind );
	//
	if ( cruse ) LeaveCriticalSection( &CR_SCHEDULER_RESTART );
	//
}

void SCH_RESTART_GET_DATA_INIT() {
	//
	char FileName[512];
	char FileNameT[512];
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	//
	EnterCriticalSection( &CR_SCHEDULER_RESTART );
	//
	sprintf( FileName , SCHEDULER_RESTART_BACKUP , "*" );
	hFind = FindFirstFile( FileName , &fd );
	//
	while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			//
			sprintf( FileName , "%s/%s" , SCHEDULER_RESTART_FOLDER , fd.cFileName );
			DeleteFile( FileName );
			//
		}
		bRet = FindNextFile( hFind , &fd );
	}
	//
	FindClose( hFind );
	//
	//-------------------------------------------------------------------------------------
	//
	sprintf( FileName , SCHEDULER_RESTART_FILE , "*" );
	hFind = FindFirstFile( FileName , &fd );
	//
	while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			//
			sprintf( FileName , "%s/%s" , SCHEDULER_RESTART_FOLDER , fd.cFileName );
			sprintf( FileNameT , "%s/%s.bak" , SCHEDULER_RESTART_FOLDER , fd.cFileName );
			//
			CopyFile( FileName , FileNameT , TRUE );
			//
		}
		bRet = FindNextFile( hFind , &fd );
	}
	//
	FindClose( hFind );
	//
	LeaveCriticalSection( &CR_SCHEDULER_RESTART );
	//
}

//===========================================================================================================================
void INIT_SCH_RESTART_CONTROL( int apmode , int side ) { // 2011.09.21
	//
	if ( apmode == 0 ) {
		//
		InitializeCriticalSection( &CR_SCHEDULER_RESTART );
		//
	}
	else if ( apmode == 4 ) {
		//
		if ( SYSTEM_LOGSKIP_STATUS() ) return;
		//
		EnterCriticalSection( &CR_SCHEDULER_RESTART );
		//
		SCH_RESTART_DELETE_INFO( FALSE );
		//
		SCH_RESTART_MAKE_INFO();
		//
		LeaveCriticalSection( &CR_SCHEDULER_RESTART );
		//
	}
	//
}


//===========================================================================================================================
void SCH_RESTART_SET_FOLDER( char *folder ) {
	//
	strcpy( SCHEDULER_RESTART_FOLDER , folder );
	//
	strcat( SCHEDULER_RESTART_FOLDER , "/Restart" );
	//
	if ( !Directory_Make_at_Sub( SCHEDULER_RESTART_FOLDER ) ) { // 2014.10.31
		strcpy( SCHEDULER_RESTART_FOLDER , folder );
	}
	//
//	sprintf( SCHEDULER_RESTART_FILE , "%s/Restart_%s.info" , folder , "%s" ); // 2014.10.31
//	sprintf( SCHEDULER_RESTART_BACKUP , "%s/Restart_%s.bak" , folder , "%s" ); // 2014.10.31
	//
	sprintf( SCHEDULER_RESTART_FILE , "%s/Restart_%s.info" , SCHEDULER_RESTART_FOLDER , "%s" ); // 2014.10.31
	sprintf( SCHEDULER_RESTART_BACKUP , "%s/Restart_%s.bak" , SCHEDULER_RESTART_FOLDER , "%s" ); // 2014.10.31
}


//===========================================================================================================================





int SCH_RESTART_GET_DATA_ALL_INFO( int controlmode , int side , int pt , int cmd , int cslarm , int wfr_side , int wfr_sts , int *rmd , int *rslarm ) {
	int i , j , Res , rs;
	//------------------------------------------------------------------------------
	Res = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , cmd , cslarm , cmd , cslarm , wfr_side , wfr_sts );
	//
	if ( Res == 99 ) return 99; // 2012.07.21
	//
	if ( Res == 0 ) {
		//
		*rmd = cmd;
		*rslarm = cslarm;
		//
		return 0;
	}
	//------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( CM1 + i ) ) continue;
		//
		for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , CM1 + i , j , cmd , cslarm , wfr_side , wfr_sts );
			//
			if ( rs == 99 ) return 99; // 2012.07.21
			//
			if ( rs == 0 ) {
				//
				*rmd = CM1 + i;
				*rslarm = j;
				//
				return 0;
			}
			//
		}
	}
	//------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( TM + i ) ) continue;
		//
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			//
			if ( j >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
			//
			rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , TM + i , j , cmd , cslarm , wfr_side , wfr_sts );
			//
			if ( rs == 99 ) return 99; // 2012.07.21
			//
			if ( rs == 0 ) {
				//
				*rmd = TM + i;
				*rslarm = j;
				//
				return 0;
			}
			//
		}
	}
	//------------------------------------------------------------------------------
	for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
		//
		rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , FM , j , cmd , cslarm , wfr_side , wfr_sts );
		//
		if ( rs == 99 ) return 99; // 2012.07.21
		//
		if ( rs == 0 ) {
			//
			*rmd = FM;
			*rslarm = j;
			//
			return 0;
		}
		//
	}
	//------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
		//
		for ( j = 1 ; j <= MAX_PM_SLOT_DEPTH ; j++ ) {
			//
			rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , i , j , cmd , cslarm , wfr_side , wfr_sts );
			//
			if ( rs == 99 ) return 99; // 2012.07.21
			//
			if ( rs == 0 ) {
				//
				*rmd = i;
				*rslarm = j;
				//
				return 0;
			}
			//
		}
		//
	}
	//------------------------------------------------------------------------------
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
		//
		if ( SCH_Inside_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			//
			rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , i , j , cmd , cslarm , wfr_side , wfr_sts );
			//
			if ( rs == 99 ) return 99; // 2012.07.21
			//
			if ( rs == 0 ) {
				//
				*rmd = i;
				*rslarm = j;
				//
				return 0;
			}
			//
		}
		//
	}
	//------------------------------------------------------------------------------
	for ( j = 1 ; j <= 2 ; j++ ) {
		//
		rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , F_AL , j , cmd , cslarm , wfr_side , wfr_sts );
		//
		if ( rs == 99 ) return 99; // 2012.07.21
		//
		if ( rs == 0 ) {
			//
			*rmd = F_AL;
			*rslarm = j;
			//
			return 0;
		}
	}
	//------------------------------------------------------------------------------
	for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; j++ ) {
		//
		rs = SCH_RESTART_GET_DATA_INFO( controlmode , side , pt , F_IC , j , cmd , cslarm , wfr_side , wfr_sts );
		//
		if ( rs == 99 ) return 99; // 2012.07.21
		//
		if ( rs == 0 ) {
			//
			*rmd = F_IC;
			*rslarm = j;
			//
			return 0;
		}
		//
	}
	//------------------------------------------------------------------------------
	return Res;
}





void SCH_RESTART_DATA_REMAP_AFTER_USER_PART( int side , int ftm ) { // 2013.06.19
	int i , pi , sl;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
		//
		pi = _i_SCH_CLUSTER_Get_PathIn( side , i );
		sl = _i_SCH_CLUSTER_Get_SlotIn( side , i );
		//
		if ( ( pi < CM1 ) || ( pi >= PM1 ) ) {
			if ( ( pi < BM1 ) || ( pi >= TM ) ) {
				continue;
			}
		}
		//
		if ( _i_SCH_IO_GET_MODULE_STATUS( pi , sl ) != CWM_PRESENT ) _i_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_CM_END );
		//
	}
}