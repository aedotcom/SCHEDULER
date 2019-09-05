#include "default.h"

//========================================================================================================================================================

#include "EQ_Enum.h"

//========================================================================================================================================================
#include "equip_Handling.h"
#include "io_Part_data.h"
#include "io_Part_log.h"
#include "iolog.h"
#include "system_Tag.h"
#include "MultiJob.h"
#include "MultiReg.h"
#include "Robot_Handling.h"
#include "Timer_Handling.h"
#include "sch_CassetteResult.h"
#include "sch_CassetteSupply.h"
#include "sch_OneSelect.h"
#include "sch_PrePost.h"
#include "sch_PrePrcs.h"
#include "sch_main.h"
#include "sch_sdm.h"
#include "sch_sub.h"
#include "sch_EIL.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_Multi_ALIC.h"
#include "sch_FMArm_Multi.h"
#include "sch_prm.h"
#include "sch_Util.h"
#include "user_Parameter.h"

extern char CLUSTER_INDEX_LRECIPE[256]; // 2019.05.29
extern char CLUSTER_INDEX_CRECIPE[256]; // 2019.05.29

void SCHEDULER_Set_CLUSTER_INDEX_SELECT_VALUE( int lastseldata ); // 2019.05.29

extern int USER_FIXED_JOB_OUT_CASSETTE; // 2018.08.22

extern int SCHEDULER_SYSTEM_STATUS; // 2018.12.12


//========================================================================================================================================================
int _i_SCH_SUB_COPY_LOCKING_FILE_WHILE_RUNNING( int side , int pt , int ch , int wfr , char *filename , int mode , int etc ); // 2018.01.05
//========================================================================================================================================================
int _i_SCH_SUB_CHECK_PRE_END( int side , int tmside , int ch , BOOL lastcheckskip ); // 2018.11.29
//========================================================================================================================================================
//int _i_SCH_SUB_SET_PROCESSING( int Side0 , int ch , char *rcpname , int skiplotpreside ); // 2017.09.19 // 2018.06.22
int _i_SCH_SUB_SET_PROCESSING( int Side0 , int ch , char *rcpname , int skiplotpreside , int mode ); // 2017.09.19 // 2018.06.22
//========================================================================================================================================================
int _i_SCH_SYSTEM_PLACING_TH_GET( int ch );
//========================================================================================================================================================
int _SCH_COMMON_USER_FLOW_NOTIFICATION( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ); // 2017.01.02
//========================================================================================================================================================
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( int side , int *cm , int *pt ); // 2016.12.14
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( int side , int *cm , int *pt ); // 2017.03.16
//========================================================================================================================================================
void _i_SCH_COMMSTATUS_GET( int ch , int *c1 , int *c2 , int *c3 );
//========================================================================================================================================================
void FILE_PARAM_SM_SCHEDULER_SETTING2( int mode , int id ); // 2013.08.22
//========================================================================================================================================================

LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 ) {
	switch( m ) {
	case 101 :	_i_SCH_CLUSTER_Init_ClusterData( (int) a1 , (int) a2 );																	break;

	case 111 :	_i_SCH_CLUSTER_Set_PathIn( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 121 :	_i_SCH_CLUSTER_Set_PathOut( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 131 :	_i_SCH_CLUSTER_Set_SlotIn( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 141 :	_i_SCH_CLUSTER_Set_SlotOut( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 151 :	_i_SCH_CLUSTER_Set_PathDo( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 152 :	_i_SCH_CLUSTER_Inc_PathDo( (int) a1 , (int) a2 );																				break;
	case 153 :	_i_SCH_CLUSTER_Dec_PathDo( (int) a1 , (int) a2 );																				break;
	case 161 :	_i_SCH_CLUSTER_Set_PathStatus( (int) a1 , (int) a2 , (int) a3 );																break;
	case 171 :	_i_SCH_CLUSTER_Set_PathRange( (int) a1 , (int) a2 , (int) a3 );																break;
	case 181 :	_i_SCH_CLUSTER_Set_PathRun( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );											break;
	case 191 :	_i_SCH_CLUSTER_Set_ClusterIndex( (int) a1 , (int) a2 , (int) a3 );															break;
	case 192 :	_i_SCH_CLUSTER_Dec_ClusterIndex( (int) a1 , (int) a2 );																		break;
	case 201 :	_i_SCH_CLUSTER_Set_SupplyID( (int) a1 , (int) a2 , (int) a3 );																break;
	case 202 :	_i_SCH_CLUSTER_Make_SupplyID( (int) a1 , (int) a2 );																		break; // 2013.01.29
	case 211 :	_i_SCH_CLUSTER_Set_PathHSide( (int) a1 , (int) a2 , (int) a3 );																break; // 2018.11.22
	case 221 :	_i_SCH_CLUSTER_Set_SwitchInOut( (int) a1 , (int) a2 , (int) a3 );																break;
	case 231 :	_i_SCH_CLUSTER_Set_FailOut( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 241 :	_i_SCH_CLUSTER_Set_CPJOB_CONTROL( (int) a1 , (int) a2 , (int) a3 );															break;
	case 251 :	_i_SCH_CLUSTER_Set_CPJOB_PROCESS( (int) a1 , (int) a2 , (int) a3 );															break;
	case 261 :	_i_SCH_CLUSTER_Set_Extra( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 271 :	_i_SCH_CLUSTER_Set_Buffer( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 281 :	_i_SCH_CLUSTER_Set_Optimize( (int) a1 , (int) a2 , (int) a3 );																break;
	case 291 :	_i_SCH_CLUSTER_Set_Stock( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 292 :	_i_SCH_CLUSTER_Inc_Stock( (int) a1 , (int) a2 );																				break;
	case 293 :	_i_SCH_CLUSTER_Dec_Stock( (int) a1 , (int) a2 );																				break;
	case 301 :	_i_SCH_CLUSTER_Set_LotSpecial( (int) a1 , (int) a2 , (int) a3 );																break;
	case 311 :	_i_SCH_CLUSTER_Set_LotUserSpecial( (int) a1 , (int) a2 , (char *) a3 );														break;
	case 321 :	_i_SCH_CLUSTER_Set_ClusterUserSpecial( (int) a1 , (int) a2 , (char *) a3 );													break;
	case 322 :	_i_SCH_CLUSTER_Set_ClusterSpecial( (int) a1 , (int) a2 , (int) a3 );																break; // 2014.06.23
	case 331 :	_i_SCH_CLUSTER_Set_ClusterTuneData( (int) a1 , (int) a2 , (char *) a3 );														break;
	case 341 :	_i_SCH_CLUSTER_Set_UserArea( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 342 :	_i_SCH_CLUSTER_Set_UserArea2( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 343 :	_i_SCH_CLUSTER_Set_UserArea3( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 344 :	_i_SCH_CLUSTER_Set_UserArea4( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 411 :	_i_SCH_CLUSTER_Set_User_PathIn( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 421 :	_i_SCH_CLUSTER_Set_User_PathOut( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 431 :	_i_SCH_CLUSTER_Set_User_OrderIn( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 441 :	_i_SCH_CLUSTER_Set_User_SlotOut( (int) a1 , (int) a2 , (int) a3 );																	break;
	case 451 :	_i_SCH_CLUSTER_Init_AllTime( (int) a1 , (int) a2 );																	break;
	case 452 :	_i_SCH_CLUSTER_Do_StartTime( (int) a1 , (int) a2 );																	break;
	case 453 :	_i_SCH_CLUSTER_Do_EndTime( (int) a1 , (int) a2 );																	break;

		// 2011.08.23
	case 501 :	_i_SCH_CLUSTER_Set_Ex_MtlOut( (int) a1 , (int) a2 , (int) a3 );																		break;
	case 502 :	_i_SCH_CLUSTER_Set_Ex_CarrierIndex( (int) a1 , (int) a2 , (int) a3 );																break;
	case 503 :	_i_SCH_CLUSTER_Set_Ex_JobName( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 504 :	_i_SCH_CLUSTER_Set_Ex_PPID( (int) a1 , (int) a2 , (char *) a3 );																break;
	case 507 :	_i_SCH_CLUSTER_Set_Ex_MaterialID( (int) a1 , (int) a2 , (char *) a3 );															break;
	case 508 :	_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( (int) a1 , (int) a2 , (int) a3 );														break;
	case 509 :	_i_SCH_CLUSTER_Set_Ex_UserControl_Data( (int) a1 , (int) a2 , (char *) a3 );													break;
	case 510 :	_i_SCH_CLUSTER_Set_Ex_PrcsID( (int) a1 , (int) a2 , (int) a3 );																		break;
	case 511 :	_i_SCH_CLUSTER_Set_Ex_OrderMode( (int) a1 , (int) a2 , (int) a3 );																		break; // 2016.08.25
	case 512 :	_i_SCH_CLUSTER_Set_Ex_MtlOutWait( (int) a1 , (int) a2 , (int) a3 );																		break; // 2016.11.02

	case 1111 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathIn( (int) a1 , (int) a2 );															break;
	case 1121 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathOut( (int) a1 , (int) a2 );															break;
	case 1131 :	return (LPARAM) _i_SCH_CLUSTER_Get_SlotIn( (int) a1 , (int) a2 );															break;
	case 1141 :	return (LPARAM) _i_SCH_CLUSTER_Get_SlotOut( (int) a1 , (int) a2 );															break;
	case 1151 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathDo( (int) a1 , (int) a2 );															break;
	case 1161 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathStatus( (int) a1 , (int) a2 );														break;
	case 1171 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathRange( (int) a1 , (int) a2 );															break;
	case 1181 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathRun( (int) a1 , (int) a2 , (int) a3 , (int) a4 );										break;
	case 1191 :	return (LPARAM) _i_SCH_CLUSTER_Get_ClusterIndex( (int) a1 , (int) a2 );														break;
	case 1201 :	return (LPARAM) _i_SCH_CLUSTER_Get_SupplyID( (int) a1 , (int) a2 );															break;
//	case 1211 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathHSide( (int) a1 , (int) a2 );															break; // 2018.11.22
	case 1211 :
				if (a3 == 0) {
					return (LPARAM)_i_SCH_CLUSTER_Get_PathHSide((int)a1, (int)a2) % 1000000;
				}
				else {
					return (LPARAM)_i_SCH_CLUSTER_Get_PathHSide((int)a1, (int)a2);
				}
				break; // 2018.11.22
	case 1221 :	return (LPARAM) _i_SCH_CLUSTER_Get_SwitchInOut( (int) a1 , (int) a2 );														break;
	case 1231 :	return (LPARAM) _i_SCH_CLUSTER_Get_FailOut( (int) a1 , (int) a2 );															break;
	case 1241 :	return (LPARAM) _i_SCH_CLUSTER_Get_CPJOB_CONTROL( (int) a1 , (int) a2 );														break;
	case 1251 :	return (LPARAM) _i_SCH_CLUSTER_Get_CPJOB_PROCESS( (int) a1 , (int) a2 );														break;
	case 1261 :	return (LPARAM) _i_SCH_CLUSTER_Get_Extra( (int) a1 , (int) a2 );																break;
	case 1262 :	return (LPARAM) _i_SCH_CLUSTER_Get_Extra_Flag( (int) a1 , (int) a2 , (int) a3 );												break;

	case 1271 :	return (LPARAM) _i_SCH_CLUSTER_Get_Buffer( (int) a1 , (int) a2 );															break;
	case 1281 :	return (LPARAM) _i_SCH_CLUSTER_Get_Optimize( (int) a1 , (int) a2 );															break;
	case 1291 :	return (LPARAM) _i_SCH_CLUSTER_Get_Stock( (int) a1 , (int) a2 );																break;
	case 1301 :	return (LPARAM) _i_SCH_CLUSTER_Get_LotSpecial( (int) a1 , (int) a2 );														break;
	case 1311 :	return (LPARAM) _i_SCH_CLUSTER_Get_LotUserSpecial( (int) a1 , (int) a2 );											break;
	case 1321 :	return (LPARAM) _i_SCH_CLUSTER_Get_ClusterUserSpecial( (int) a1 , (int) a2 );										break;
	case 1322 :	return (LPARAM) _i_SCH_CLUSTER_Get_ClusterSpecial( (int) a1 , (int) a2 );											break; // 2014.06.23
	case 1331 :	return (LPARAM) _i_SCH_CLUSTER_Get_ClusterTuneData( (int) a1 , (int) a2 );											break;
	case 1341 :	return (LPARAM) _i_SCH_CLUSTER_Get_UserArea( (int) a1 , (int) a2 );															break;
	case 1342 :	return (LPARAM) _i_SCH_CLUSTER_Get_UserArea2( (int) a1 , (int) a2 );															break;
	case 1343 :	return (LPARAM) _i_SCH_CLUSTER_Get_UserArea3( (int) a1 , (int) a2 );															break;
	case 1344 :	return (LPARAM) _i_SCH_CLUSTER_Get_UserArea4( (int) a1 , (int) a2 );															break;
	case 1411 :	return (LPARAM) _i_SCH_CLUSTER_Get_User_PathIn( (int) a1 , (int) a2 );														break;
	case 1421 :	return (LPARAM) _i_SCH_CLUSTER_Get_User_PathOut( (int) a1 , (int) a2 );														break;
	case 1431 :	return (LPARAM) _i_SCH_CLUSTER_Get_User_OrderIn( (int) a1 , (int) a2 );														break;
	case 1441 :	return (LPARAM) _i_SCH_CLUSTER_Get_User_SlotOut( (int) a1 , (int) a2 );														break;
	case 1451 :	return (LPARAM) _i_SCH_CLUSTER_Get_StartTime( (int) a1 , (int) a2 );															break;
	case 1452 :	return (LPARAM) _i_SCH_CLUSTER_Get_EndTime( (int) a1 , (int) a2 );															break;

		// 2011.08.23
	case 1501 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_MtlOut( (int) a1 , (int) a2 );																		break;
	case 1502 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_CarrierIndex( (int) a1 , (int) a2 );																break;
	case 1503 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_JobName( (int) a1 , (int) a2 );																break;
	case 1504 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_PPID( (int) a1 , (int) a2 );																break;
	case 1507 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_MaterialID( (int) a1 , (int) a2 );															break;
	case 1508 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( (int) a1 , (int) a2 );														break;
	case 1509 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_UserControl_Data( (int) a1 , (int) a2 );														break;
	case 1510 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_PrcsID( (int) a1 , (int) a2 );																		break;
	case 1511 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_OrderMode( (int) a1 , (int) a2 );																		break; // 2016.08.25
	case 1512 :	return (LPARAM) _i_SCH_CLUSTER_Get_Ex_MtlOutWait( (int) a1 , (int) a2 );																		break; // 2016.11.02

	case 2000 :	_i_SCH_CLUSTER_Set_PathProcessChamber( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );								break;
	case 2001 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break;
	case 2002 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break;
	case 2003 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );									break;
	case 2004 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );									break;
	case 2005 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( (int) a1 , (int) a2 , (int) a3 , (int) a4 );									break;
	case 2006 :	_i_SCH_CLUSTER_Clear_PathProcessData( (int) a1 , (int) a2 , (int) a3 );														break;
	case 2007 :	_i_SCH_CLUSTER_Set_PathProcessData( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int)a5 , (char *)a6,(char *)a7,(char *)a8 );	break;
	case 2008 :	_i_SCH_CLUSTER_Set_PathProcessData_JustPost( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char *) a5 );						break;
	case 2009 :	_i_SCH_CLUSTER_Set_PathProcessData_JustPre( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char *) a5 );						break;
	case 2010 :	_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );							break;
	case 2011 :	_i_SCH_CLUSTER_Set_PathProcessData_SkipPre( (int) a1 , (int) a2 );															break;
	case 2012 :	_i_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( (int) a1 , (int) a2 , (int) a3 );											break;
	case 2014 :	_i_SCH_CLUSTER_Set_PathProcessData_SkipPost( (int) a1 , (int) a2 , (int) a3 , (int) a4 );										break;
	case 2015 :	_i_SCH_CLUSTER_Set_PathProcessData_SkipPost2( (int) a1 , (int) a2 , (int) a3 , (int) a4 );									break;
	case 2016 :	_i_SCH_CLUSTER_Set_PathProcessData_UsedPost_Restore( (int) a1 , (int) a2 , (int) a3 );										break;
	case 2017 :	_i_SCH_CLUSTER_Swap_PathProcessChamber( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );								break;
	case 2018 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Enable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break; // 2013.03.28

	case 2100 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathOrder( (int) a1 , (int) a2 , (int) a3 , (int) a4 );									break;
	case 2101 :	return (LPARAM) _i_SCH_CLUSTER_Get_Next_PM_String( (int) a1 , (int) a2 , (int) a3 , (char *) a4 , (int) a5 );					break;
	case 2102 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessChamber( (int) a1 , (int) a2 , (int) a3 , (int) a4 );							break;
	case 2103 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessRecipe( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );					break;
	case 2104 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessRecipe2( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );				break;
	case 2105 :	return (LPARAM) _i_SCH_CLUSTER_Check_Possible_UsedPre( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 );		break;
	case 2106 :	return (LPARAM) _i_SCH_CLUSTER_Check_Possible_UsedPre_GlobalOnly( (int) a1 );													break;
	case 2107 :	return (LPARAM) _i_SCH_CLUSTER_Check_Already_Run_UsedPre( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break;
	case 2108 :	return (LPARAM) _i_SCH_CLUSTER_Check_Possible_UsedPost( (int) a1 , (int) a2 , (int) a3 , (int) a4 );							break;
	case 2109 :	return (LPARAM) _i_SCH_CLUSTER_Check_Possible_UsedPost2( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int *) a5 );			break;
	case 2110 :	return (LPARAM) _i_SCH_CLUSTER_Check_HasProcessData_Post( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break;

	case 3001 :	return (LPARAM) _i_SCH_CLUSTER_Copy_Cluster_Data( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );						break;

	case 3011 :	_i_SCH_CLUSTER_Set_PathProcessParallel( (int) a1 , (int) a2 , (int) a3 , (char *) a4 );										break;
	case 3012 :	_i_SCH_CLUSTER_Set_PathProcessParallel2( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char) a5 );								break;
	case 3013 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessParallel( (int) a1 , (int) a2 , (int) a3 );										break;
	case 3014 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessParallel2( (int) a1 , (int) a2 , (int) a3 , (int) a4 );							break;

	case 3015 :	_i_SCH_CLUSTER_Set_PathProcessDepth( (int) a1 , (int) a2 , (int) a3 , (char *) a4 );										break;
	case 3016 :	_i_SCH_CLUSTER_Set_PathProcessDepth2( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char) a5 );								break;
	case 3017 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessDepth( (int) a1 , (int) a2 , (int) a3 );										break;
	case 3018 :	return (LPARAM) _i_SCH_CLUSTER_Get_PathProcessDepth2( (int) a1 , (int) a2 , (int) a3 , (int) a4 );							break;

	case 3021 :	return (LPARAM) _i_SCH_CLUSTER_Chk_Parallel_Used_Disable( (int) a1 , (int) a2 , (int) a3 , (int) a4 );						break;
	case 3022 :	_i_SCH_CLUSTER_Set_Parallel_Used_Restore( (int) a1 , (int) a2 , (int) a3 );													break;
	case 3023 :	_i_SCH_CLUSTER_After_Place_For_Parallel_Resetting( (int) a1 , (int) a2 , (int) a3 , (int) a4 );								break;

	case 3031 :	return (LPARAM) _i_SCH_CLUSTER_Check_and_Make_Return_Wafer( (int) a1 , (int) a2 , (int) a3 );									break;
	case 3032 :	_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( (int) a1 , (int) a2 );											break;
	case 3033 :	return (LPARAM) _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_FDHC( (int) a1 , (int) a2 , (int) a3 );						break;
	case 3034 :	_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop_FDHC( (int) a1 , (int) a2 );													break;

	case 3041 :	return (LPARAM) _i_SCH_CLUSTER_Select_OneChamber_and_OtherBuffering( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );	break;
	case 3042 :	return (LPARAM) _i_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( (int) a1 , (int) a2 , (int) a3 );					break;

#ifdef _SCH_MIF // 2017.03.24
	case 10 :	return (LPARAM)	1;	break;
#else
	case 10 :	return (LPARAM)	0;	break;
#endif

	case 11 :					_i_SCH_SYSTEM_USING_SET( (int) a1 , (int) a2 );	break;
	case 12 :	return (LPARAM)	_i_SCH_SYSTEM_USING_GET( (int) a1 );	break;
	case 13 :					_i_SCH_SYSTEM_TM_SET( (int) a1 , (int) a2 , (int) a3 );	break;
	case 14 :	return (LPARAM)	_i_SCH_SYSTEM_TM_GET( (int) a1 , (int) a2 );	break;
	case 15 :					_i_SCH_SYSTEM_LASTING_SET( (int) a1 , (int) a2 );	break;
	case 16 :	return (LPARAM)	_i_SCH_SYSTEM_LASTING_GET( (int) a1 );	break;
	case 17 :					_i_SCH_SYSTEM_USING2_SET( (int) a1 , (int) a2 );	break;
	case 18 :	return (LPARAM)	_i_SCH_SYSTEM_USING2_GET( (int) a1 );	break;
	case 19 :					_i_SCH_SYSTEM_FA_SET( (int) a1 , (int) a2 );	break;
	case 20 :	return (LPARAM)	_i_SCH_SYSTEM_FA_GET( (int) a1 );	break;
	case 21 :					_i_SCH_SYSTEM_CPJOB_SET( (int) a1 , (int) a2 );	break;
	case 22 :	return (LPARAM)	_i_SCH_SYSTEM_CPJOB_GET( (int) a1 );	break;
	case 23 :					_i_SCH_SYSTEM_CPJOB_ID_SET( (int) a1 , (int) a2 );	break;
	case 24 :	return (LPARAM)	_i_SCH_SYSTEM_CPJOB_ID_GET( (int) a1 );	break;
	case 25 :					_i_SCH_SYSTEM_MODE_SET( (int) a1 , (int) a2 );	break;
	case 26 :	return (LPARAM)	_i_SCH_SYSTEM_MODE_GET( (int) a1 );	break;
	case 27 :					_i_SCH_SYSTEM_RUNORDER_SET( (int) a1 , (int) a2 );	break;
	case 28 :	return (LPARAM)	_i_SCH_SYSTEM_RUNORDER_GET( (int) a1 );	break;
	case 29 :					_i_SCH_SYSTEM_RUNCHECK_SET( (int) a1 );	break;
	case 30 :	return (LPARAM)	_i_SCH_SYSTEM_RUNCHECK_GET();	break;
	case 31 :					_i_SCH_SYSTEM_RUNCHECK_FM_SET( (int) a1 );	break;
	case 32 :	return (LPARAM)	_i_SCH_SYSTEM_RUNCHECK_FM_GET();	break;
	case 33 :	return (LPARAM)	_i_SCH_SYSTEM_USING_RUNNING( (int) a1 );	break;
	case 34 :	return (LPARAM)	_i_SCH_SYSTEM_USING_ANOTHER_RUNNING( (int) a1 );	break;
	case 35 :	return (LPARAM)	_i_SCH_SYSTEM_USING_STARTING_ONLY( (int) a1 );	break;
	case 36 :	return (LPARAM)	_i_SCH_SYSTEM_USING_STARTING( (int) a1 );	break;
	case 37 :	return (LPARAM)	_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( (int) a1 );	break;
	case 38 :	return (LPARAM)	_i_SCH_SYSTEM_ABORT_PAUSE_CHECK( (int) a1 );	break;
	case 39 :	return (LPARAM)	_i_SCH_SYSTEM_MODE_END_GET( (int) a1 );	break;
	case 40 :					_i_SCH_SYSTEM_MODE_END_SET( (int) a1 , (int) a2);	break;
	case 41 :	return (LPARAM)	_i_SCH_SYSTEM_MODE_RESUME_GET( (int) a1 );	break;
	case 42 :					_i_SCH_SYSTEM_MODE_RESUME_SET( (int) a1 , (int) a2 );	break;
	case 43 :	return (LPARAM)	_i_SCH_SYSTEM_MODE_WAITR_GET( (int) a1 );	break;
	case 44 :					_i_SCH_SYSTEM_MODE_WAITR_SET( (int) a1 , (int) a2 );	break;
	case 45 :	return (LPARAM)	_i_SCH_SYSTEM_MODE_LOOP_GET( (int) a1 );	break;
	case 46 :					_i_SCH_SYSTEM_MODE_LOOP_SET( (int) a1 , (int) a2 );	break;
	case 47 :					_i_SCH_SYSTEM_MODE_ABORT_SET( (int) a1 );	break;
	case 48 :	return (LPARAM)	_i_SCH_SYSTEM_GET_MID_ID( (int) a1 );	break;
	case 49 :	return (LPARAM)	_i_SCH_SYSTEM_GET_JOB_ID( (int) a1 );	break;
	case 50 :	return (LPARAM)	_i_SCH_SYSTEM_GET_MODULE_NAME( (int) a1 );	break;
	case 51 :	return (LPARAM)	_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( (int) a1 );	break;

	case 52 :	return (LPARAM)	_i_SCH_SYSTEM_APPEND_END_GET( (int) a1 );	break; // 2010.09.13
	case 53 :					_i_SCH_SYSTEM_APPEND_END_SET( (int) a1 , (int) a2 );	break; // 2010.09.13
		//
	case 54 :	return (LPARAM)	_i_SCH_SYSTEM_PMMODE_GET( (int) a1 );	break; // 2010.11.11
	case 55 :					_i_SCH_SYSTEM_PMMODE_SET( (int) a1 , (int) a2);	break; // 2010.11.11
		//
	case 56 :					_i_SCH_SYSTEM_FLAG_RESET( (int) a1 );	break; // 2012.08.29

	//
	case 57 :	return (LPARAM)	_i_SCH_SYSTEM_PAUSE_GET( (int) a1 );	break;	// 2012.12.13
	case 58 :					_i_SCH_SYSTEM_PAUSE_SET( (int) a1 , (int) a2);	break;	// 2012.12.13

	case 59 :	return (LPARAM)	_i_SCH_SYSTEM_PAUSE_SW_GET();	break;	// 2012.12.13
	case 60 :					_i_SCH_SYSTEM_PAUSE_SW_SET( (int) a1 );	break;	// 2012.12.13

	//

	case 61 :
//				EnterCriticalSection( &CR_FEMRUN ); // 2011.04.28
				while( TRUE ) {
					if ( TryEnterCriticalSection( &CR_FEMRUN ) ) break;
					Sleep(1);
				}
				break;
	case 62 :
				LeaveCriticalSection( &CR_FEMRUN );
				break;
	case 63 :
//				EnterCriticalSection( &(CR_TMRUN[(int) a1 ]) ); // 2011.04.28
				while( TRUE ) {
					if ( TryEnterCriticalSection( &(CR_TMRUN[(int) a1 ]) ) ) break;
					Sleep(1);
				}
				break;
	case 64 :
				LeaveCriticalSection( &(CR_TMRUN[(int) a1 ]) );
				break;
		//
	case 65 :
				return (LPARAM)	TryEnterCriticalSection( &CR_FEMRUN );	break;
	case 66 :
				return (LPARAM)	TryEnterCriticalSection( &(CR_TMRUN[(int) a1 ]) );	break;

		//
//	case 67 :	return (LPARAM)	_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( (int) a1 );	break; // 2012.12.13 // 2016.11.04
	case 67 :	return (LPARAM)	_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( (int) a1 , (int) a2 );	break; // 2012.12.13 // 2016.11.04

		//
	case 68 :	return (LPARAM)	_i_SCH_SYSTEM_EQUIP_RUNNING_GET( (int) a1 );	break; // 2012.10.31
	case 69 :					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( (int) a1 , (int) a2 );	break; // 2012.10.31
		//

	case 71 :					_i_SCH_COMMSTATUS_GET( (int) a1 , (int *) a2 , (int *) a3 , (int *) a4 ); break; // 2010.10.05

	case 72 :	return (LPARAM)	_i_SCH_SYSTEM_PLACING_TH_GET( (int) a1 );	break; // 2017.01.02

	case 73 :	return (LPARAM)	_i_SCH_SYSTEM_HAS_USER_PROCESS_GET( (int) a1 );	break; // 2017.07.17

	case 81 :	return (LPARAM)	_i_SCH_SYSTEM_BLANK_GET( (int) a1 );	break;
	case 82 :					_i_SCH_SYSTEM_BLANK_SET( (int) a1 , (int) a2 );	break;

	case 83 :	return (LPARAM)	_i_SCH_SYSTEM_DBUPDATE_GET( (int) a1 );	break;
	case 84 :					_i_SCH_SYSTEM_DBUPDATE_SET( (int) a1 , (int) a2 );	break;

	case 85 :	return (LPARAM)	_i_SCH_SYSTEM_FINISH_GET( (int) a1 );	break;
	case 86 :					_i_SCH_SYSTEM_FINISH_SET( (int) a1 , (int) a2 );	break;

	case 87 :	return (LPARAM)	_i_SCH_SYSTEM_RESTART_GET( (int) a1 );	break; // 2011.10.17
	case 88 :					_i_SCH_SYSTEM_RESTART_SET( (int) a1 , (int) a2 );	break; // 2011.10.17

	case 90 :	return (LPARAM)	BUTTON_GET_FLOW_STATUS_VAR( (int) a1 );	break; // 2012.03.28

	case 91 :	_i_SCH_SYSTEM_INFO_LOG( (char *) a1 , (char *) a1 );	break; // 2013.02.20

	case 92 :	return (LPARAM)	_i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( (int) a1 );	break;
	case 93 :
								_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( (int) a1 , (char *) a2 );
								if ( (int) a3 ) IO_LOT_DIR_INITIAL( (int) a1 , (char *) a2 , _i_SCH_PRM_GET_DFIX_LOG_PATH() );
								break;

	case 94 :	return (LPARAM)	_i_SCH_SYSTEM_MOVEMODE_GET( (int) a1 );	break; // 2013.09.03
	case 95 :					_i_SCH_SYSTEM_MOVEMODE_SET( (int) a1 , (int) a2 );	break; // 2013.09.03
		//

	case 96 :	return (LPARAM)	SCHEDULER_SYSTEM_STATUS; // 2018.12.12

	case 97 :	return (LPARAM)	ALARM_STATUS( (int) a1 );	break; // 2016.04.29
	case 98 :					ALARM_POST( (int) a1 );	break; // 2016.04.29

	case 99 :	return (LPARAM)	GET_SYSTEM_NAME();	break; // 2012.04.22

	case 100 :	ERROR_HANDLER( (int) a1 , (char *) a2 );	break; // 2014.02.05

	case 4001 :					_i_SCH_SYSTEM_ESOURCE_ID_SET( (int) a1 , (int) a2 );	break;	// 2019.02.26
	case 4002 :	return (LPARAM)	_i_SCH_SYSTEM_ESOURCE_ID_GET( (int) a1 );	break;	// 2019.02.26

	case 4003 :	return (LPARAM)	CLUSTER_INDEX_LRECIPE;	break;	// 2019.05.29
	case 4004 :	return (LPARAM)	CLUSTER_INDEX_CRECIPE;	break;	// 2019.05.29
	case 4005 :					SCHEDULER_Set_CLUSTER_INDEX_SELECT_VALUE( (int) a1 );	break;	// 2019.05.29
		//
	}

	return (LPARAM) NULL;
}

//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================

LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 ) {
	switch( m ) {
	case 101 : _i_SCH_MODULE_Set_FM_SIDE_WAFER( (int) a1 , (int) a2 , (int) a3 );		break;
	case 102 : _i_SCH_MODULE_Set_FM_POINTER_MODE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 103 : _i_SCH_MODULE_Set_FM_SIDE( (int) a1 , (int) a2 );						break;
	case 104 : _i_SCH_MODULE_Set_FM_POINTER( (int) a1 , (int) a2 );					break;
	case 105 : _i_SCH_MODULE_Set_FM_MODE( (int) a1 , (int) a2 );						break;
	case 106 : _i_SCH_MODULE_Set_FM_Runinig_Flag( (int) a1 , (int) a2 );				break;
	case 107 : _i_SCH_MODULE_Set_FM_OutCount( (int) a1 , (int) a2 );					break;
	case 108 : _i_SCH_MODULE_Inc_FM_OutCount( (int) a1 );								break;
	case 109 : _i_SCH_MODULE_Set_FM_InCount( (int) a1 , (int) a2 );					break;
	case 110 : _i_SCH_MODULE_Inc_FM_InCount( (int) a1 );								break;
	case 111 : _i_SCH_MODULE_Set_FM_MidCount( (int) a1 );								break;
	case 112 : _i_SCH_MODULE_Set_FM_LastOutPointer( (int) a1 , (int) a2 );				break;
	case 113 : _i_SCH_MODULE_Set_FM_DoPointer_FDHC( (int) a1 , (int) a2 );					break;
	case 114 : _i_SCH_MODULE_Inc_FM_DoPointer_FDHC( (int) a1 );								break;
	case 115 : _i_SCH_MODULE_Set_FM_End_Status_FDHC( (int) a1 );							break;
	case 116 : _i_SCH_MODULE_Set_FM_HWait( (int) a1 , (int) a2 );						break;
	case 117 : _i_SCH_MODULE_Set_FM_SwWait( (int) a1 , (int) a2 );						break;
	case 118 : _i_SCH_MODULE_Set_FM_WAFER( (int) a1 , (int) a2 );					break; // 2013.01.11
	case 120 : _i_SCH_MODULE_Init_FBTPMData( (int) a1 );					break;

	case 151 : return (LPARAM) _i_SCH_MODULE_Get_FM_SIDE( (int) a1 );					break;
	case 152 : return (LPARAM) _i_SCH_MODULE_Get_FM_WAFER( (int) a1 );					break;
	case 153 : return (LPARAM) _i_SCH_MODULE_Get_FM_POINTER( (int) a1 );				break;
	case 154 : return (LPARAM) _i_SCH_MODULE_Get_FM_MODE( (int) a1 );					break;
	case 155 : return (LPARAM) _i_SCH_MODULE_Get_FM_Runinig_Flag( (int) a1 );			break;
	case 156 : return (LPARAM) _i_SCH_MODULE_Get_FM_OutCount( (int) a1 );				break;
	case 157 : return (LPARAM) _i_SCH_MODULE_Get_FM_InCount( (int) a1 );				break;
	case 158 : return (LPARAM) _i_SCH_MODULE_Get_FM_MidCount();						break;
	case 159 : return (LPARAM) _i_SCH_MODULE_Get_FM_LastOutPointer( (int) a1 );		break;
	case 160 : return (LPARAM) _i_SCH_MODULE_Chk_FM_Finish_Status_FDHC( (int) a1 );			break;
	case 161 : return (LPARAM) _i_SCH_MODULE_Chk_FM_Free_Status( (int) a1 );			break;
	case 162 : return (LPARAM) _i_SCH_MODULE_Get_FM_DoPointer_FDHC( (int) a1 );				break;
	case 163 : return (LPARAM) _i_SCH_MODULE_Get_FM_HWait( (int) a1 );					break;
	case 164 : return (LPARAM) _i_SCH_MODULE_Get_FM_SwWait( (int) a1 );				break;

	case 205 : _i_SCH_MODULE_Set_BM_FULL_MODE( (int) a1 , (int) a2 );	break;
	case 206 : _i_SCH_MODULE_Set_BM_SIDE_POINTER( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 207 : _i_SCH_MODULE_Set_BM_SIDE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 208 : _i_SCH_MODULE_Set_BM_POINTER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 209 : _i_SCH_MODULE_Set_BM_WAFER_STATUS( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 210 : _i_SCH_MODULE_Set_BM_STATUS( (int) a1 , (int) a2 , (int) a3 );	break;
	case 211 : _i_SCH_MODULE_Set_BM_MidCount( (int) a1 );	break;

	case 254 : return (LPARAM) _i_SCH_MODULE_Get_BM_FULL_MODE( (int) a1 );	break;
	case 255 : return (LPARAM) _i_SCH_MODULE_Get_BM_SIDE( (int) a1 , (int) a2 );	break;
	case 256 : return (LPARAM) _i_SCH_MODULE_Get_BM_POINTER( (int) a1 , (int) a2 );	break;
	case 257 : return (LPARAM) _i_SCH_MODULE_Get_BM_STATUS( (int) a1 , (int) a2 );	break;
	case 258 : return (LPARAM) _i_SCH_MODULE_Get_BM_WAFER( (int) a1 , (int) a2 );	break;
	case 259 : return (LPARAM) _i_SCH_MODULE_Get_BM_MidCount();	break;

	case 301 : _i_SCH_MODULE_Set_TM_DoPointer( (int) a1 , (int) a2 );	break;
	case 302 : _i_SCH_MODULE_Inc_TM_DoPointer( (int) a1 );	break;
	case 303 : _i_SCH_MODULE_Set_TM_InCount( (int) a1 , (int) a2 );	break;
	case 304 : _i_SCH_MODULE_Inc_TM_InCount( (int) a1 );	break;
	case 305 : _i_SCH_MODULE_Set_TM_OutCount( (int) a1 , (int) a2 );	break;
	case 306 : _i_SCH_MODULE_Inc_TM_OutCount( (int) a1 );	break;
	case 307 : _i_SCH_MODULE_Set_TM_End_Status( (int) a1 );	break;
	case 308 : _i_SCH_MODULE_Set_TM_Switch_Signal( (int) a1 , (int) a2 );	break;
	case 309 : _i_SCH_MODULE_Set_TM_Move_Signal( (int) a1 , (int) a2 );	break;
	case 311 : _i_SCH_MODULE_Set_TM_SIDE_POINTER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 );	break;
	case 312 : _i_SCH_MODULE_Set_TM_SIDE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 313 : _i_SCH_MODULE_Set_TM_POINTER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 314 : _i_SCH_MODULE_Set_TM_PATHORDER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 315 : _i_SCH_MODULE_Set_TM_TYPE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 316 : _i_SCH_MODULE_Set_TM_WAFER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 317 : _i_SCH_MODULE_Set_TM_OUTCH( (int) a1 , (int) a2 , (int) a3 );	break;
	case 318 : _i_SCH_MODULE_Set_TM_DoubleCount( (int) a1 , (int) a2 );	break;
	case 319 : _i_SCH_MODULE_Inc_TM_DoubleCount( (int) a1 );	break;
	case 320 : _i_SCH_MODULE_Dec_TM_DoubleCount( (int) a1 );	break;

	case 351 : return (LPARAM) _i_SCH_MODULE_Get_TM_DoPointer( (int) a1 );	break;
	case 352 : return (LPARAM) _i_SCH_MODULE_Get_TM_InCount( (int) a1 );	break;
	case 353 : return (LPARAM) _i_SCH_MODULE_Get_TM_OutCount( (int) a1 );	break;
	case 354 : return (LPARAM) _i_SCH_MODULE_Chk_TM_Finish_Status( (int) a1 );	break;
	case 355 : return (LPARAM) _i_SCH_MODULE_Chk_TM_Free_Status( (int) a1 );	break;
	case 356 : return (LPARAM) _i_SCH_MODULE_Chk_TM_Free_Status2( (int) a1 , (int) a2 );	break;
	case 357 : return (LPARAM) _i_SCH_MODULE_Get_TM_Switch_Signal( (int) a1 );	break;
	case 358 : return (LPARAM) _i_SCH_MODULE_Get_TM_Move_Signal( (int) a1 );	break;
	case 360 : return (LPARAM) _i_SCH_MODULE_Get_TM_SIDE( (int) a1 , (int) a2 );	break;
	case 361 : return (LPARAM) _i_SCH_MODULE_Get_TM_SIDE2( (int) a1 , (int) a2 );	break;
	case 362 : return (LPARAM) _i_SCH_MODULE_Get_TM_POINTER( (int) a1 , (int) a2 );	break;
	case 363 : return (LPARAM) _i_SCH_MODULE_Get_TM_POINTER2( (int) a1 , (int) a2 );	break;
	case 364 : return (LPARAM) _i_SCH_MODULE_Get_TM_PATHORDER( (int) a1 , (int) a2 );	break;
	case 365 : return (LPARAM) _i_SCH_MODULE_Get_TM_TYPE( (int) a1 , (int) a2 );	break;
	case 366 : return (LPARAM) _i_SCH_MODULE_Get_TM_WAFER( (int) a1 , (int) a2 );	break;
	case 367 : return (LPARAM) _i_SCH_MODULE_Get_TM_OUTCH( (int) a1 , (int) a2 );	break;
	case 368 : return (LPARAM) _i_SCH_MODULE_Get_TM_DoubleCount( (int) a1 );	break;

	case 401 : _i_SCH_MODULE_Set_PM_SIDE_POINTER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );	break;
	case 402 : _i_SCH_MODULE_Set_PM_SIDE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 403 : _i_SCH_MODULE_Set_PM_POINTER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 404 : _i_SCH_MODULE_Set_PM_PATHORDER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 405 : _i_SCH_MODULE_Set_PM_WAFER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 406 : _i_SCH_MODULE_Set_PM_STATUS( (int) a1 , (int) a2 , (int) a3 );	break;
	case 407 : _i_SCH_MODULE_Set_PM_PICKLOCK( (int) a1 , (int) a2 , (int) a3 );	break; // 2014.01.10

	case 451 : return (LPARAM) _i_SCH_MODULE_Get_PM_SIDE( (int) a1 , (int) a2 );	break;
	case 452 : return (LPARAM) _i_SCH_MODULE_Get_PM_POINTER( (int) a1 , (int) a2 );	break;
	case 453 : return (LPARAM) _i_SCH_MODULE_Get_PM_PATHORDER( (int) a1 , (int) a2 );	break;
	case 454 : return (LPARAM) _i_SCH_MODULE_Get_PM_WAFER( (int) a1 , (int) a2 );	break;
	case 455 : return (LPARAM) _i_SCH_MODULE_Get_PM_STATUS( (int) a1 , (int) a2 );	break;
	case 456 : return (LPARAM) _i_SCH_MODULE_Get_PM_PICKLOCK( (int) a1 , (int) a2 );	break; // 2014.01.10

	case 501 : _i_SCH_MODULE_Set_MFIC_SIDE_POINTER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 502 : _i_SCH_MODULE_Set_MFIC_SIDE( (int) a1 , (int) a2 );	break;
	case 503 : _i_SCH_MODULE_Set_MFIC_POINTER( (int) a1 , (int) a2 );	break;
	case 504 : _i_SCH_MODULE_Set_MFIC_WAFER( (int) a1 , (int) a2 );	break;
	case 505 : _i_SCH_MODULE_Set_MFIC_RUN_TIMER( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 506 : _i_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( (int) a1 , (int) a2 , (int) a3 );	break;
	case 507 : _i_SCH_MODULE_Set_MFAL_SIDE( (int) a1 );	break;
	case 508 : _i_SCH_MODULE_Set_MFAL_POINTER( (int) a1 );	break;
	case 509 : _i_SCH_MODULE_Set_MFAL_WAFER( (int) a1 );	break;
		//
	case 511 : _i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 512 : _i_SCH_MODULE_Set_MFALS_SIDE( (int) a1 , (int) a2 );	break;
	case 513 : _i_SCH_MODULE_Set_MFALS_POINTER( (int) a1 , (int) a2 );	break;
	case 514 : _i_SCH_MODULE_Set_MFALS_WAFER( (int) a1 , (int) a2 );	break; // 2008.08.29

	case 551 : return (LPARAM) _i_SCH_MODULE_Need_Do_Multi_FAL();	break;
	case 552 : return (LPARAM) _i_SCH_MODULE_Need_Do_Multi_TAL();	break;
	case 553 : return (LPARAM) _i_SCH_MODULE_Need_Do_Multi_FIC();	break;
	case 554 : return (LPARAM) _i_SCH_MODULE_Get_MFIC_SIDE( (int) a1 );	break;
	case 555 : return (LPARAM) _i_SCH_MODULE_Get_MFIC_POINTER( (int) a1 );	break;
	case 556 : return (LPARAM) _i_SCH_MODULE_Get_MFIC_WAFER( (int) a1 );	break;
	case 557 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_HAS_COUNT();	break;
	case 558 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_FREE_COUNT();	break;
	case 559 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_FREE2_COUNT( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 560 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( (int *) a1 , (int *) a2 , (int) a3 , (int) a4 );	break;
	case 561 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_FREE_TYPE2_SLOT( (int) a1 , (int) a2 , (int *) a3 , (int *) a4 );	break;
	case 562 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int *) a5 , (int *) a6 , (int) a7 );	break;
	case 563 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( (int) a1 , (int) a2 , (int *) a3 , (int *) a4 , (int *) a5 );	break;
	case 564 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_MULTI_FREE( (int) a1 , (int) a2 );	break;
	case 565 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_MULTI_FINISH( (int) a1 , (int) a2 );	break;
	case 566 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_RUN_TIMER( (int) a1 , (int) a2 , (time_t) a3 );	break;
	case 567 : return (LPARAM) _i_SCH_MODULE_Get_MFIC_Completed_Wafer_FDHC( (int) a1 , (int *) a2 , (int *) a3 , (int *) a4 );	break;
	case 568 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_Yes_for_Get_FDHC( (int) a1 );	break;
	case 569 : return (LPARAM) _i_SCH_MODULE_Chk_MFIC_LOCKING( (int) a1 );	break;
	case 570 : return (LPARAM) _i_SCH_MODULE_Get_MFAL_SIDE();	break;
	case 571 : return (LPARAM) _i_SCH_MODULE_Get_MFAL_POINTER();	break;
	case 572 : return (LPARAM) _i_SCH_MODULE_Get_MFAL_BM();	break;
	case 573 : return (LPARAM) _i_SCH_MODULE_Get_MFAL_WAFER();	break;
		//
	case 581 : return (LPARAM) _i_SCH_MODULE_Get_MFALS_SIDE( (int) a1 );	break;
	case 582 : return (LPARAM) _i_SCH_MODULE_Get_MFALS_POINTER( (int) a1 );	break;
	case 583 : return (LPARAM) _i_SCH_MODULE_Get_MFALS_BM( (int) a1 );	break;
	case 584 : return (LPARAM) _i_SCH_MODULE_Get_MFALS_WAFER( (int) a1 );	break; // 2008.08.29

	case 601 : _i_SCH_FMARMMULTI_DATA_Init();	break;
	case 602 : _i_SCH_FMARMMULTI_DATA_SHIFTING_A_AND_APPEND( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int *) a6 , (int *) a7 , (int *) a8 );	break;
	case 603 : _i_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( (int) a1 , (int) a2 , (int) a3 );	break;
	case 604 : _i_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( (int) a1 );	break;
	case 605 : _i_SCH_FMARMMULTI_FA_SUBSTRATE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 606 : _i_SCH_FMARMMULTI_PICK_FROM_CM_POST( (int) a1 , (int) a2 );	break;
	case 607 : _i_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( (int) a1 );	break;
	case 608 : _i_SCH_FMARMMULTI_DATA_SET_FROM_FIC( (int) a1 , (int) a2 , (int) a3 );	break;
	case 609 : _i_SCH_FMARMMULTI_DATA_SET_FROM_CM_FDHC( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 610 : _i_SCH_FMARMMULTI_DATA_SET_FROM_BM( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );	break;

	case 651 : return (LPARAM) _i_SCH_FMARMMULTI_DATA_Append( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 652 : return (LPARAM) _i_SCH_FMARMMULTI_DATA_Exist( (int) a1 , (int *) a2 , (int *) a3 , (int *) a4 , (int *) a5 );	break;

	case 701 : _i_SCH_MODULE_Set_Mdl_Use_Flag( (int) a1 , (int) a2 , (int) a3 );	break;
	case 702 : _i_SCH_MODULE_Set_Mdl_Spd_Flag( (int) a1 , (int) a2 , (int) a3 );	break;
	case 703 : _i_SCH_MODULE_Set_Mdl_Run_Flag( (int) a1 , (int) a2 );	break;
	case 704 : _i_SCH_MODULE_Dec_Mdl_Run_Flag( (int) a1 );	break;
	case 705 : _i_SCH_MODULE_Inc_Mdl_Run_Flag( (int) a1 );	break;

	case 706 : _i_SCH_MODULE_Set_Use_Interlock_Run( (int) a1 , (int) a2 , (int) a3 );	break;
	case 707 : _i_SCH_MODULE_Set_Use_Interlock_Map( (int) a1 );	break;
	case 708 : _i_SCH_SUB_Set_Last_Status( (int) a1 , (int) a2 );	break;
	case 709 : _i_SCH_SUB_Chk_GLOBAL_STOP_FDHC( (int) a1 );	break;
	case 710 : _i_SCH_SUB_Make_FORCE_FMSIDE( (int) a1 );	break;
	case 711 : _i_SCH_SUB_Make_ENABLE_TO_ENABLEN( (int) a1 );	break; // 2015.04.03

	case 751 : return (LPARAM) _i_SCH_MODULE_Get_Mdl_Use_Flag( (int) a1 , (int) a2 );	break;
	case 752 : return (LPARAM) _i_SCH_MODULE_Get_Mdl_Spd_Flag( (int) a1 , (int) a2 );	break;
	case 753 : return (LPARAM) _i_SCH_MODULE_Get_Mdl_Run_Flag( (int) a1 );	break;

	case 754 : return (LPARAM) _i_SCH_MODULE_FIRST_CONTROL_GET();	break;
	case 755 : return (LPARAM) _i_SCH_MODULE_FIRST_EXPEDITE_GET( (int) a1 );	break;
	case 756 : return (LPARAM) _i_SCH_MODULE_SECOND_EXPEDITE_GET( (int) a1 );	break;
	case 757 : return (LPARAM) _i_SCH_MODULE_Get_Use_Interlock_Run( (int) a1 , (int) a2 );	break;
	case 758 : return (LPARAM) _i_SCH_SUB_Chk_First_Out_Status( (int) a1 , (int) a2 );	break;
	case 759 : return (LPARAM) _i_SCH_SUB_Chk_Last_Out_Status( (int) a1 , (int) a2 );	break;
	case 760 : return (LPARAM) _i_SCH_SUB_Get_Last_Status( (int) a1 );	break;
	case 761 : return (LPARAM) _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( (int *) a1 );	break;

	case 801 : return (LPARAM) _i_SCH_MODULE_GET_USE_ENABLE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 802 : return (LPARAM) _i_SCH_MODULE_GROUP_FM_POSSIBLE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 803 : return (LPARAM) _i_SCH_MODULE_GROUP_TBM_POSSIBLE( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 804 : return (LPARAM) _i_SCH_MODULE_GROUP_TPM_POSSIBLE( (int) a1 , (int) a2 , (int) a3 );	break;

	case 805 : return (LPARAM) _i_SCH_MODULE_GET_MOVE_MODE( (int) a1 );	break; // 2013.09.03
	case 806 :				   _i_SCH_MODULE_SET_MOVE_MODE( (int) a1 , (int) a2 );	break; // 2013.09.03

	case 901 : return (LPARAM) _i_EIL_METHOD_REMOVE_TO_EIL( (int) a1 , (int) a2 , FALSE );	break;
	case 902 : return (LPARAM) _i_EIL_OPERATE_REQUEST_TM_TO_EIL( (int) a1 );	break;
	case 903 : return (LPARAM) _i_EIL_METHOD_INSERT_TO_EIL( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char *) a5 , (char *) a6 , FALSE );	break;
	case 904 : return (LPARAM) _i_EIL_SIDE_BEGIN( (int) a1 , (char *) a2 , (char *) a3 , (char *) a4 , (char *) a5 );	break;
	case 905 : return (LPARAM) _i_EIL_SIDE_END( (int) a1 , (char *) a2 );	break;
	case 906 :				   _i_EIL_SET_FINISH( (int) a1 );	break;

	}

	return (LPARAM) NULL;
}


//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================

LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 ) {
	switch( m ) {

	case 101 : return (LPARAM) _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( (int) a1 , (int) a2 , (int) a3 );		break;
	case 102 : return (LPARAM) _i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( (int) a1 , (int) a2 );		break;
	case 103 :                 _i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( (int) a1 , (int) a2 );		break;
	case 104 :                 _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 , (int) a9 );		break;
	case 105 : return (LPARAM) _i_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS( (int) a1 , (int) a2 , (int) a3 );		break;

	case 106 : return (LPARAM) _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( (int) a1 , (int) a2 , (char *) a3 , (int) a4 );		break; // 2013.01.16
	case 107 : return (LPARAM) _i_SCH_MULTIJOB_GET_CONTROLJOB_NAME( (int) a1 , (int) a2 , (char *) a3 , (int) a4 );		break; // 2013.01.16

	case 108 : return (LPARAM) _i_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( (int) a1 , (int) a2 );		break; // 2013.05.27

	case 109 :                 _i_SCH_MULTIJOB_SET_LOTRECIPE_READ( (int) a1 );		break;	// 2014.06.23
	case 110 : return (LPARAM) _i_SCH_MULTIJOB_GET_LOTRECIPE_READ();		break;	// 2014.06.23

	case 111 :                 _i_SCH_MULTIREG_DATA_RESET( (int) a1 );		break;
	case 112 : return (LPARAM) _i_SCH_MULTIREG_HAS_REGIST_DATA( (int) a1 );		break;
	case 113 : return (LPARAM) _i_SCH_MULTIREG_GET_REGIST_DATA( (int) a1 , (Scheduling_Regist *) a2 );		break;
	case 114 : return (LPARAM) _i_SCH_MULTIREG_SET_REGIST_DATA( (int) a1 , (Scheduling_Regist *) a2 );		break;

	case 115 : return (LPARAM) _i_SCH_MULTIJOB_SET_CJPJJOB_RESULT( (int) a1 , (int) a2 , (int) a3 , (int) a4 );		break; // 2018.11.14

	case 121 : return (LPARAM) _i_SCH_CASSETTE_LAST_RESULT_GET( (int) a1 , (int) a2 );		break;
	case 122 : return (LPARAM) _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( (int) a1 , (int) a2 );		break;
	case 123 :                 _i_SCH_CASSETTE_LAST_RESET( (int) a1 , (int) a2 );		break;
	case 124 : return (LPARAM) _i_SCH_CASSETTE_Chk_SKIP( (int) a1 );		break;
	case 125 : return (LPARAM) _i_SCH_CASSETTE_LAST_RESULT2_GET( (int) a1 , (int) a2 );		break;

	case 131 : return (LPARAM) _i_SCH_CASSETTE_Check_Side_Monitor( (int) a1 );		break;
	case 132 :                 _i_SCH_CASSETTE_Reset_Side_Monitor( (int) a1 , (int) a2 );		break;
	case 133 : return (LPARAM) _i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_FDHC( (int) a1 , (int) a2 , (int) a3 );		break;
	case 134 :                 _i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( (int) a1 , (int) a2 );		break;
	case 135 : return (LPARAM) _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( (int) a1 );		break;

	case 141 : return (LPARAM) _i_SCH_ONESELECT_Get_LastSelectPM();		break;
	case 142 :                 _i_SCH_ONESELECT_Set_LastSelectPM( (int) a1 );		break;
	case 143 :                 _i_SCH_ONESELECT_Reset_First_What( (int) a1 , (int) a2 , (int) a3 );		break;
	case 144 : return (LPARAM) _i_SCH_ONESELECT_Get_First_One_What( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int *) a6 );		break;

	case 151 : return (LPARAM) _i_SCH_PREPOST_Get_inside_READY_RECIPE_USE( (int) a1 , (int) a2 );		break;
	case 152 : return (LPARAM) _i_SCH_PREPOST_Get_inside_READY_RECIPE_NAME( (int) a1 , (int) a2 );		break;
	case 153 : return (LPARAM) _i_SCH_PREPOST_Get_inside_END_RECIPE_USE( (int) a1 , (int) a2 );		break;
	case 154 : return (LPARAM) _i_SCH_PREPOST_Get_inside_END_RECIPE_NAME( (int) a1 , (int) a2 );		break;
	case 155 : return (LPARAM) _i_SCH_PREPOST_Get_inside_READY_RECIPE_TYPE( (int) a1 , (int) a2 );		break; // 2017.10.16

	case 161 :                 _i_SCH_PREPOST_Randomize_End_Part( (int) a1 , (int) a2 );		break;
	case 165 : return (LPARAM) _i_SCH_PREPOST_Randomize_End_Part2( (int) a1 , (int) a2 , (BOOL) a3 );		break;
	case 162 :                 _i_SCH_PREPOST_LotPost_Part_for_All_BM_Return( (int) a1 );		break;
	case 163 : return (LPARAM) _i_SCH_PREPOST_Pre_End_Part( (int) a1 , (int) a2 );		break;
	case 164 : return (LPARAM) _i_SCH_PREPOST_Pre_End_Force_Part( (int) a1 , (int) a2 );		break;

	case 171 :                 _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( (int) a1 , (int) a2 );		break;
	case 172 :                 _i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( (int) a1 , (int) a2 );		break;
	case 173 :                 _i_SCH_PREPRCS_FirstRunPre_Clear_PathProcessData( (int) a1 , (int) a2 );		break;
	case 174 : return (LPARAM) _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( (int) a1 , (int) a2 );		break;
	case 175 :                 _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( (int) a1 , (int) a2 );		break;
	case 176 :                 _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse( (int) a1 , (char *) a2 );		break;
	case 177 :                 _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse2( (int) a1 , (char *) a2 );		break;

	case 181 :                 _i_SCH_SDM_Set_CHAMBER_INC_COUNT( (int) a1 );	break;
	case 182 : return (LPARAM) _i_SCH_SDM_Get_LOTFIRST_OPTION( (int) a1 );	break;
	case 183 : return (LPARAM) _i_SCH_SDM_Get_LOTEND_OPTION( (int) a1 );	break;
	case 184 : return (LPARAM) _i_SCH_SDM_Get_RECIPE( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 185 : return (LPARAM) _i_SCH_SDM_Get_RUN_CHAMBER( (int) a1 );	break;
	case 186 : return (LPARAM) _i_SCH_SDM_Get_RUN_USECOUNT( (int) a1 );	break;
	case 187 :                 _i_SCH_SDM_Set_CHAMBER_RESET_COUNT( (int) a1 );	break; // 2012.02.15
	
	case 201 : return (LPARAM) _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( (int) a1 , (int) a2 , (int) a3 );		break;
	case 202 : return (LPARAM) _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( (int) a1 , (int) a2 , (int) a3 );		break;
	case 203 :                 _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( (int) a1 , (int) a2 );		break;
	case 204 :                 _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( (int) a1 , (int) a2 );		break;
	case 205 : return (LPARAM) _i_SCH_SUB_PRECHECK_WAITING_FOR_THREAD_TM( (int) a1 , (int) a2 );		break;
	case 206 : return (LPARAM) _i_SCH_SUB_Main_End_Check_Finished( (int) a1 );		break;
	case 207 : return (LPARAM) _i_SCH_SUB_Check_ClusterIndex_CPJob_Same( (int) a1 , (int) a2 , (int) a3 , (int) a4 );		break;
	case 208 : return (LPARAM) _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( (int) a1 , (int) a2 , (int) a3 );		break;
	case 209 : 
		if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) (int) a1 ) == -1 ) {
			return (LPARAM) FALSE;
		}
		return (LPARAM) TRUE;
		break;
	case 210 :                 _i_SCH_SUB_Wait_Finish_Complete( (int) a1 , (int) a2 );		break;
	case 211 :                 _i_SCH_SUB_Reset_Scheduler_Data( (int) a1 );		break;
	case 212 : return (LPARAM) _i_SCH_SUB_Check_Complete_Return( (int) a1 );		break;
	case 213 : return (LPARAM) _i_SCH_SUB_Run_Impossible_Condition( (int) a1 , (int) a2 , (int) a3 );		break;
	case 214 : return (LPARAM) _i_SCH_SUB_Simple_Supply_Possible_FDHC( (int) a1 );		break;
	case 215 : return (LPARAM) _i_SCH_SUB_Remain_for_CM( (int) a1 , FALSE );		break;
	case 216 : return (LPARAM) _i_SCH_SUB_Remain_for_FM_FDHC( (int) a1 );		break;
	case 217 : return (LPARAM) _i_SCH_SUB_FM_Another_No_More_Supply( (int) a1 , (int) a2 , (int) a3 , (int) a4 );		break;
	case 218 : return (LPARAM) _i_SCH_SUB_FM_Current_No_More_Supply_FDHC( (int) a1 , (int *) a2 , (int) a3 , (int *) a4 , (int) a5 , (int) a6 );		break;
	case 219 : return (LPARAM) _i_SCH_SUB_GET_OUT_CM_AND_SLOT( (int) a1 , (int) a2 , (int) a3 , (int *) a4 , (int *) a5 );		break;
	case 220 :                 _i_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( (int) a1 , (int) a2 , (int *) a3 , (int *) a4 );		break;
	case 221 : return (LPARAM) _i_SCH_SUB_PLACE_CM_for_ERROROUT( (int) a1 , (int) a2 , (int *) a3 , (int *) a4 , (int) a5 , (int) a6 , (int *) a7 , (int *) a8 );		break;
	case 222 : return (LPARAM) _i_SCH_SUB_DISAPPEAR_OPERATION( (int) a1 , (int) a2 );		break;
	case 223 : return (LPARAM) _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( (int) a1 , (int) a2 );		break;
	case 224 : return (LPARAM) _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( (int) a1 , (int) a2 );		break;
	case 225 : return (LPARAM) _i_SCH_SUB_START_PARALLEL_SKIP_CHECK( (int) a1 , (int) a2 , (int) a3 , (int) a4 );		break;
	case 226 : return (LPARAM) _i_SCH_SUB_FM_Current_No_Way_Supply( (int) a1 , (int) a2 , (int) a3 );		break;	// 2008.07.28
	case 227 : return (LPARAM) _i_SCH_SUB_RECIPE_READ_OPERATION( (int) a1 );		break; // 2009.04.23
	case 228 : return (LPARAM) _i_SCH_SUB_FMTMSIDE_OPERATION( (int) a1 , (int) a2 ); break; // 2009.05.06
		//
	case 229 : return (LPARAM) _i_SCH_SUB_RECIPE_READ_CLUSTER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (char *) a6 );		break; // 2010.11.09
	case 230 : return (LPARAM) _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( (int) a1 , (int) a2 , (int) a3 , (int *) a4 , (int *) a5 , (BOOL *) a6 );		break; // 2012.04.06

	case 231 : return (LPARAM) _i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK( (int) a1 , (int) a2 );		break; // 2015.06.11

	case 232 : return (LPARAM) _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( (int) a1 , (int *) a2 , (int *) a3 );	break; // 2016.12.14
	case 233 : return (LPARAM) _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( (int) a1 , (int *) a2 , (int *) a3 );	break; // 2017.03.16

//	case 234 : return (LPARAM) _i_SCH_SUB_SET_PROCESSING( (int) a1 , (int ) a2 , (char *) a3 , (int ) a4 );	break; // 2017.09.19 // 2018.06.22
	case 234 : return (LPARAM) _i_SCH_SUB_SET_PROCESSING( (int) a1 , (int ) a2 , (char *) a3 , (int ) a4 , (int ) a5 );	break; // 2017.09.19 // 2018.06.22

	case 235 : return (LPARAM) _i_SCH_SUB_COPY_LOCKING_FILE_WHILE_RUNNING( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (char *) a5 , (int ) a6 , (int ) a7 );	break; // 2018.01.05

	case 236 : return (LPARAM) _i_SCH_SUB_CHECK_PRE_END( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break; // 2018.11.29

	case 241 :                 USER_FIXED_JOB_OUT_CASSETTE = (int) a1;		break; // 2018.08.22
	case 242 : return (LPARAM) USER_FIXED_JOB_OUT_CASSETTE;		break; // 2018.08.22

	//=============================================================================================================
	//=============================================================================================================
	//=============================================================================================================
	//=============================================================================================================

	case 500 :return (LPARAM) _SCH_COMMON_USER_FLOW_NOTIFICATION( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 , (int) a9 ); // 2017.01.02

	}

	return (LPARAM) NULL;
}

//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
int _SCH_PRM_GET_INTERLOCK_PICK_ARM_DENY_CHECK_FOR_MDL( int tms , int Chamber , int arm ) { // 2015.03.30
	int intv;
	//
	//	arm : A,B,C,D
	//	return :	0 : Impossible
	//				1 : Possible(All Use)
	//				2 : Possible(has Interlock)
	//				3 : Possible(All Use2)
	//
	//
	intv = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms , Chamber );
	//
	if ( arm < 0 ) return intv;
	//
	if ( intv <= 0 ) return 1;
	//
	if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) {
		if ( intv == 3 ) return 3;
		if ( intv >= 4 ) return 1;
	}
	//
	switch( arm ) {
	case TA_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 1 : // A
		case 5 : // AB
		case 6 : // AC
		case 7 : // AD
		case 11 : // ABC
		case 12 : // ABD
		case 13 : // ACD
			return 2;
		case 2 : // B
		case 3 : // C
		case 4 : // D
		case 8 : // BC
		case 9 : // BD
		case 10 : // CD
		case 14 : // BCD
			return 0;
		}
		break;
	case TB_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 2 : // B
		case 5 : // AB
		case 8 : // BC
		case 9 : // BD
		case 11 : // ABC
		case 12 : // ABD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 3 : // C
		case 4 : // D
		case 6 : // AC
		case 7 : // AD
		case 10 : // CD
		case 13 : // ACD
			return 0;
		}
		break;
	case TC_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 3 : // C
		case 6 : // AC
		case 8 : // BC
		case 10 : // CD
		case 11 : // ABC
		case 13 : // ACD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 2 : // B
		case 4 : // D
		case 5 : // AB
		case 7 : // AD
		case 9 : // BD
		case 12 : // ABD
			return 0;
		}
		break;
	case TD_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 4 : // D
		case 7 : // AD
		case 9 : // BD
		case 10 : // CD
		case 12 : // ABD
		case 13 : // ACD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 2 : // B
		case 3 : // C
		case 5 : // AB
		case 6 : // AC
		case 8 : // BC
		case 11 : // ABC
			return 0;
		}
		break;
	}
	//
	return 0;
	//
}

int _SCH_PRM_GET_INTERLOCK_PLACE_ARM_DENY_CHECK_FOR_MDL( int tms , int Chamber , int arm ) { // 2015.03.30
	int intv;
	//
	//	arm : A,B,C,D
	//	return :	0 : Impossible
	//				1 : Possible(All Use)
	//				2 : Possible(has Interlock)
	//				3 : Possible(All Use2)
	//
	//
	intv = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , Chamber );
	//
	if ( arm < 0 ) return intv;
	//
	if ( intv <= 0 ) return 1;
	//
	if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) {
		if ( intv == 3 ) return 3;
		if ( intv >= 4 ) return 1;
	}
	//
	switch( arm ) {
	case TA_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 1 : // A
		case 5 : // AB
		case 6 : // AC
		case 7 : // AD
		case 11 : // ABC
		case 12 : // ABD
		case 13 : // ACD
			return 2;
		case 2 : // B
		case 3 : // C
		case 4 : // D
		case 8 : // BC
		case 9 : // BD
		case 10 : // CD
		case 14 : // BCD
			return 0;
		}
		break;
	case TB_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 2 : // B
		case 5 : // AB
		case 8 : // BC
		case 9 : // BD
		case 11 : // ABC
		case 12 : // ABD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 3 : // C
		case 4 : // D
		case 6 : // AC
		case 7 : // AD
		case 10 : // CD
		case 13 : // ACD
			return 0;
		}
		break;
	case TC_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 3 : // C
		case 6 : // AC
		case 8 : // BC
		case 10 : // CD
		case 11 : // ABC
		case 13 : // ACD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 2 : // B
		case 4 : // D
		case 5 : // AB
		case 7 : // AD
		case 9 : // BD
		case 12 : // ABD
			return 0;
		}
		break;
	case TD_STATION : 
		switch( intv ) {
		case 0 : // AB(ALL)
			return 1;
		case 4 : // D
		case 7 : // AD
		case 9 : // BD
		case 10 : // CD
		case 12 : // ABD
		case 13 : // ACD
		case 14 : // BCD
			return 2;
		case 1 : // A
		case 2 : // B
		case 3 : // C
		case 5 : // AB
		case 6 : // AC
		case 8 : // BC
		case 11 : // ABC
			return 0;
		}
		break;
	}
	//
	return 0;
	//
}


LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 ) {
	switch( m ) {
	case 101 : return (LPARAM) _i_SCH_EQ_PROCESS_FUNCTION_STATUS	( (int) a1 , (int) a2 , (int) a3 );	break;
	case 102 : return (LPARAM) _i_SCH_EQ_INTERFACE_FUNCTION_STATUS( (int) a1 , (int) a2 );	break;
	case 103 : return (LPARAM) _i_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( (int) a1 , (int *) a2 );	break;
	case 104 : return (LPARAM) _i_SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( (int) a1 , (int *) a2 , (int *) a3 );	break;
	case 105 : return (LPARAM) _i_SCH_EQ_PROCESS_SKIPFORCE_RUN( (int) a1 , (int) a2 );	break;
	case 106 : return (LPARAM) _i_SCH_EQ_SPAWN_PROCESS( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (char *) a7 , (int) a8 , (int) a9 , (char *) a10 , (int) a11 , -1 );	break;
	case 107 : return (LPARAM) _i_SCH_EQ_SPAWN_EVENT_PROCESS( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (char *) a7 , (int) a8 , (int) a9 , (char *) a10 , (int) a11 , (int) a12 );	break;
	case 108 : return (LPARAM) _i_SCH_EQ_PICK_FROM_CHAMBER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 , (int) a9 , (int) a10 , (int) a11 , (int) a12 );	break;
	case 109 : return (LPARAM) _i_SCH_EQ_PLACE_TO_CHAMBER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 , (int) a9 , (int) a10 , (int) a11 , (int) a12 );	break;
	case 110 : return (LPARAM) _i_SCH_EQ_PICK_FROM_FMBM( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 );	break;
	case 111 : return (LPARAM) _i_SCH_EQ_PLACE_TO_FMBM( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 );	break;
	case 112 : return (LPARAM) _i_SCH_EQ_PICK_FROM_TM_MAL( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 );	break;
	case 113 :                 _i_SCH_EQ_BEGIN_END_RUN( (int) a1 , (int) a2 , (char *) a3 , (int) a4 );	break;
	case 114 : return (LPARAM) _i_SCH_EQ_ROBOT_FROM_FMBM( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 );	break;
	case 115 : return (LPARAM) _i_SCH_EQ_ROBOT_FROM_CHAMBER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 );	break;

	case 116 : return (LPARAM) _i_SCH_EQ_SWAP_CHAMBER( (int) a1 , (int) a2 , (int) a3 , (int) a4 , (int) a5 , (int) a6 , (int) a7 , (int) a8 , (int) a9 , (int) a10 , (int) a11 , (int) a12 );	break; // 2013.01.11

	case 121 :                 _i_SCH_IO_SET_MAIN_BUTTON( (int) a1 , (int) a2 );	break;
	case 122 :                 _i_SCH_IO_SET_END_BUTTON( (int) a1 , (int) a2 );	break;
	case 123 :                 _i_SCH_IO_SET_IN_PATH( (int) a1 , (int) a2 );	break;
	case 124 :                 _i_SCH_IO_SET_OUT_PATH( (int) a1 , (int) a2 );	break;
	case 125 :                 _i_SCH_IO_SET_START_SLOT( (int) a1 , (int) a2 );	break;
	case 126 :                 _i_SCH_IO_SET_END_SLOT( (int) a1 , (int) a2 );	break;
	case 127 :                 _i_SCH_IO_SET_LOOP_COUNT( (int) a1 , (int) a2 );	break;
	case 128 :                 _i_SCH_IO_GET_RECIPE_FILE( (int) a1 , (char *) a2 );	break;
	case 129 :                 _i_SCH_IO_SET_RECIPE_FILE( (int) a1 , (char *) a2 );	break;
	case 130 :                 _i_SCH_IO_SET_MODULE_STATUS( (int) a1 , (int) a2 , (int) a3 );	break;
	case 131 :                 _i_SCH_IO_SET_MODULE_SOURCE( (int) a1 , (int) a2 , (int) a3 );	break;
	case 132 :                 _i_SCH_IO_SET_MODULE_RESULT( (int) a1 , (int) a2 , (int) a3 );	break;
	case 133 : return (LPARAM) _i_SCH_IO_GET_MAIN_BUTTON( (int) a1 );	break;
	case 134 : return (LPARAM) _i_SCH_IO_GET_IN_PATH( (int) a1 );	break;
	case 135 : return (LPARAM) _i_SCH_IO_GET_OUT_PATH( (int) a1 );	break;
	case 136 : return (LPARAM) _i_SCH_IO_GET_START_SLOT( (int) a1 );	break;
	case 137 : return (LPARAM) _i_SCH_IO_GET_END_SLOT( (int) a1 );	break;
	case 138 : return (LPARAM) _i_SCH_IO_GET_LOOP_COUNT( (int) a1 );	break;
	case 139 : return (LPARAM) _i_SCH_IO_GET_WID_NAME_FROM_READ( (int) a1 , (int) a2 , (char *) a3 );	break;
	case 140 : return (LPARAM) _i_SCH_IO_GET_MODULE_STATUS( (int) a1 , (int) a2 );	break;
	case 141 : return (LPARAM) _i_SCH_IO_GET_MODULE_SOURCE( (int) a1 , (int) a2 );	break;
	case 142 : return (LPARAM) _i_SCH_IO_GET_MODULE_RESULT( (int) a1 , (int) a2 );	break;
	case 143 :                 _i_SCH_IO_SET_RECIPE_PRCS_FILE( (int) a1 , (char *) a2 );	break;

	case 144 : return (LPARAM) _SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( (int) a1 , (int) a2 );	break; // 2012.11.27
	case 145 : return (LPARAM) _SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( (int) a1 , (int) a2 );	break; // 2012.11.27
	case 146 : return (LPARAM) _SCH_IO_SET_INLTKS_FOR_PICK_POSSIBLE( (int) a1 , (int) a2 , (int) a3 );	break; // 2012.11.27
	case 147 : return (LPARAM) _SCH_IO_SET_INLTKS_FOR_PLACE_POSSIBLE( (int) a1 , (int) a2 , (int) a3 );	break; // 2012.11.27

	case 151 :                 _i_SCH_IO_MTL_ADD_PICK_COUNT( (int) a1 , (int) a2 );	break;
	case 152 :                 _i_SCH_IO_MTL_ADD_PLACE_COUNT( (int) a1 , (int) a2 , (int) a3 , (int) a4 );	break;
	case 153 :                 _i_SCH_IO_MTL_SAVE();	break;

	case 155 : return (LPARAM) BUTTON_GET_FLOW_STATUS_VAR( (int) a1 );	break; // 2018.07.19
	case 156 :
		BUTTON_SET_FLOW_STATUS_VAR( (int) a1 % 1000 , (int) a2 );
		if ( (int) a1 / 1000 > 0 ) BUTTON_SET_FLOW_STATUS_EQUAL( (int) a1 % 1000 );
		break;// 2018.12.12

	case 161 : return (LPARAM) _i_SCH_ROBOT_GET_FINGER_FREE_COUNT( (int) a1 );	break;
	case 162 : return (LPARAM) _i_SCH_ROBOT_GET_FAST_PICK_FINGER( (int) a1 , (int) a2 );	break;
	case 163 : return (LPARAM) _i_SCH_ROBOT_GET_FAST_PLACE_FINGER( (int) a1 , (int) a2 );	break;
	case 164 : return (LPARAM) _i_SCH_ROBOT_GET_FINGER_HW_USABLE( (int) a1 , (int) a2 );	break;
	case 165 : return (LPARAM) _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( (int) a1 );	break;
	case 166 : return (LPARAM) _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( (int) a1 );	break; // 2013.01.22

	case 171 :                 _i_SCH_TIMER_SET_PROCESS_TIME_START( (int) a1 , (int) a2 );	break;
	case 172 :                 _i_SCH_TIMER_SET_PROCESS_TIME_END(   (int) a1 , (int) a2 , (int) a3 );	break;
	case 173 :                 _i_SCH_TIMER_SET_PROCESS_TIME_SKIP(  (int) a1 , (int) a2 );	break;
	case 174 :                 _i_SCH_TIMER_SET_PROCESS_TIME_TARGET( (int) a1 , (int) a2 , (int) a3 );	break;
	case 175 :                 _i_SCH_TIMER_SET_ROBOT_TIME_START( (int) a1 , (int) a2 );	break;
	case 176 :                 _i_SCH_TIMER_SET_ROBOT_TIME_END( (int) a1 , (int) a2 );	break;

	case 177 : return (LPARAM) _i_SCH_TIMER_GET_PROCESS_TIME_START(   (int) a1 , (int) a2 , FALSE );	break;
	case 178 : return (LPARAM) _i_SCH_TIMER_GET_PROCESS_TIME_END(     (int) a1 , (int) a2 , FALSE );	break;
	case 179 : return (LPARAM) _i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( (int) a1 , (int) a2 );	break;
	case 180 : return (LPARAM) _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN(  (int) a1 , (int) a2 );	break;
	case 181 : return (LPARAM) _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( (int) a1 , (int) a2 );	break;
	case 182 : return (LPARAM) _i_SCH_TIMER_GET_ROBOT_TIME_REMAIN( (int) a1 , (int) a2 );	break;
	case 183 : return (LPARAM) _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( (int) a1 , (int) a2 );	break;

	case 191 :                 _i_SCH_IO_SET_MODULE_SOURCE_E( (int) a1 , (int) a2 , (int) a3 );	break; // 2018.09.05
	case 192 : return (LPARAM) _i_SCH_IO_GET_MODULE_SOURCE_E( (int) a1 , (int) a2 );	break; // 2018.09.05


	}
	return (LPARAM) NULL;
}

//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================

LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 ) {

	switch( m ) {
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MODE , 101 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_GROUP , 102 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_GROUP_HAS_SWITCH , 103 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_STOCK , 104 )
	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( MODULE_PICK_GROUP , 105 )
	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( MODULE_PLACE_GROUP , 106 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_SIZE , 107 )
	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( MODULE_BUFFER_MODE , 108 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_BUFFER_FULLRUN , 109 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_BUFFER_LASTORDER , 110 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MODULE_BUFFER_SWITCH_SWAPPING , 111 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_DOUBLE_WHAT_SIDE , 112 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MODULE_DUMMY_BUFFER_CHAMBER , 113 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MODULE_ALIGN_BUFFER_CHAMBER , 114 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_BUFFER_M_SWITCH_SWAPPING , 115 ) // 2013.03.06
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_TMS_GROUP , 116 ) // 2013.03.06
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_INTERFACE , 117 ) // 2018.08.31

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MULTI_GROUP_AUTO_ARRANGE , 121 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( PRCS_ZONE_RUN_TYPE , 122 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_SWITCH_BUFFER , 131 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_SWITCH_BUFFER_ACCESS_TYPE , 132 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( BATCH_SUPPLY_INTERRUPT , 141 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DIFF_LOT_NOTSUP_MODE , 142 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( METHOD1_TO_1BM_USING_MODE , 143 ) // 2018.03.21

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( Getting_Priority , 151 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( Putting_Priority , 152 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( PRCS_TIME_WAIT_RANGE , 161 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( PRCS_TIME_REMAIN_RANGE , 162 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( NEXT_FREE_PICK_POSSIBLE , 171 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( PREV_FREE_PICK_POSSIBLE , 172 )

	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( INTERLOCK_PM_RUN_FOR_CM , 181 )
	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( INTERLOCK_TM_RUN_FOR_ALL , 182 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_PM_PICK_DENY_FOR_MDL , 183 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_PM_PLACE_DENY_FOR_MDL , 184 )

	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( INTERLOCK_FM_ROBOT_FINGER , 185 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_FM_PICK_DENY_FOR_MDL , 186 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_FM_PLACE_DENY_FOR_MDL , 187 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM , 188 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( INTERLOCK_TM_SINGLE_RUN , 189 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( INTERLOCK_TM_BM_OTHERGROUP_SWAP , 190 ) // 2013.04.25

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DUMMY_PROCESS_CHAMBER , 191 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DUMMY_PROCESS_SLOT , 192 )

	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( INTERLOCK_PM_M_PICK_DENY_FOR_MDL , 193 ) // 2013.05.28
	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( INTERLOCK_PM_M_PLACE_DENY_FOR_MDL , 194 ) // 2013.05.28

	case 195 + 1000 : return (LPARAM) _SCH_PRM_GET_INTERLOCK_PICK_ARM_DENY_CHECK_FOR_MDL( (int) a1 , (int) a2 , (int) a3 ); break; // 2015.03.30

	case 196 + 1000 : return (LPARAM) _SCH_PRM_GET_INTERLOCK_PLACE_ARM_DENY_CHECK_FOR_MDL( (int) a1 , (int) a2 , (int) a3 ); break; // 2015.03.30

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( CLSOUT_BM_SINGLE_ONLY , 201 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MAINTINF_DB_USE , 202 )

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( RB_ROBOT_ANIMATION , 211 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( RB_ROBOT_FM_ANIMATION , 212 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( RB_FM_ARM_IS_DUAL_EXTEND_STYLE , 213 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_IOINTLKS_USE , 221 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_MODULE_SW_CONTROL , 222 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_MAX_CASSETTE_SLOT , 223 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_TM_DOUBLE_CONTROL , 224 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_FM_DOUBLE_CONTROL , 225 )

//	MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN( DFIX_GROUP_RECIPE_PATH , 226 )	// 2013.08.22
	MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN_FILE_PARAM_SM1( DFIX_GROUP_RECIPE_PATH , 226 )	// 2013.08.22

	MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN( DFIX_PROCESS_RECIPE_PATH , 227 )
	MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN( DFIX_PROCESS_RECIPE_PATHF , 228 ) // 2012.08.22
	MAKE_DEFAULT_CHAR_PARAMETER2_EXTERN( DFIX_MAIN_RECIPE_PATH , 229 ) // 2012.08.22

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( DFIX_TM_M_DOUBLE_CONTROL , 230 ) // 2013.02.06

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIM_MAX_PM_COUNT , 231 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_TM_MOVE_SR_CONTROL , 241 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_FIRST_MODULE_PUT_DELAY_TIME , 242 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_FM_MOVE_SR_CONTROL , 243 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_END_MESSAGE_SEND_POINT , 244 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_CASSETTE_SUPPLY_DEVIATION_TIME , 245 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_CHAMBER_PUT_PR_CHECK , 246 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_CHAMBER_GET_PR_CHECK , 247 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_SCH_LOG_DIRECTORY_FORMAT , 248 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_FIC_MULTI_WAITTIME , 249 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_FAL_MULTI_WAITTIME , 250 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_MESSAGE_USE_WHEN_SWITCH , 251 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_CM_SUPPLY_MODE , 252 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_LOT_LOG_PRE_POST_PROCESS , 253 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_MESSAGE_USE_WHEN_ORDER , 254 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_SW_BM_SCHEDULING_FACTOR , 255 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( SYSTEM_LOG_STYLE , 256 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_ADAPTIVE_PROGRESSING , 257 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( UTIL_START_PARALLEL_CHECK_SKIP , 258 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( EIL_INTERFACE , 259 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DUMMY_MESSAGE , 260 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MAINT_MESSAGE_STYLE , 261 )

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( MTLOUT_INTERFACE , 262 )

	// 2012.09.03
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( EIL_INTERFACE_FLOW_USER_OPTION , 263 )

	// 2012.12.05
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MFI_INTERFACE_FLOW_USER_OPTION , 264 )

	// 2018.05.25
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( OPTIMIZE_MODE , 265 )

	// 2012.07.26
	MAKE_DEFAULT_CHAR_PARAMETER_EXTERN( DFIX_SYSTEM_PATH , 270 )

	MAKE_DEFAULT_CHAR_PARAMETER_EXTERN( DFIX_LOG_PATH , 271 )
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_RECIPE_LOCKING , 272 )
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( DFIX_PROCESS_RECIPE_TYPE , 273 )

	// 2013.11.21
	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_CHAMBER_INTLKS_IOUSE , 274 )

	// 2014.01.22
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( FAIL_PROCESS_SCENARIO , 275 );

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( STOP_PROCESS_SCENARIO , 276 );

	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( DFIM_SLOT_NOTUSE , 277 );	// 2016.11.14

	MAKE_DEFAULT_CHAR_PARAMETER_EXTERN( DFIX_TMP_PATH , 278 ) // 2017.10.30

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( DFIX_FIC_MULTI_CONTROL , 279 ) // 2018.06.29

	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_PREPRECV , 280 ); // 2016.11.24
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_PREPSEND , 281 ); // 2016.11.24
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_POSTRECV , 282 ); // 2016.11.24
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_POSTSEND , 283 ); // 2016.11.24
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_GATE	   , 284 ); // 2016.11.24
	MAKE_DEFAULT_INT_PARAMETER2_EXTERN( MODULE_MESSAGE_NOTUSE_CANCEL   , 285 ); // 2016.11.24

	MAKE_DEFAULT_INT_PARAMETER3_EXTERN( DFIM_SLOT_NOTUSE_DATA , 286 );	// 2018.12.06

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( CHECK_DIFFERENT_METHOD , 291 ) // 2018.08.22

	MAKE_DEFAULT_INT_PARAMETER_EXTERN( CHECK_DIFFERENT_METHOD2 , 292 ) // 2018.08.22




	}
	return (LPARAM) NULL;
}

//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================

