#ifndef SCH_PRM_CLUSTER_H
#define SCH_PRM_CLUSTER_H

//===================================================================================
// Scheduler Main Structure
//===================================================================================
typedef struct {
	int  PathRange;

//	int  PathProcess[ MAX_CLUSTER_DEPTH ][ MAX_PM_CHAMBER_DEPTH ]; // 2006.07.03
	char PathProcess[ MAX_CLUSTER_DEPTH ][ MAX_PM_CHAMBER_DEPTH ]; // 2006.07.03
	char *PathProcessParallel[ MAX_CLUSTER_DEPTH ]; // 2007.02.20
	char *PathProcessRecipe[ MAX_CLUSTER_DEPTH ][ MAX_PM_CHAMBER_DEPTH ][ 3 ];
	//
//	unsigned short PathRun[ MAX_CLUSTER_DEPTH ]; // 2006.07.13 // 2008.11.04
	int  PathRun[ MAX_CLUSTER_DEPTH ]; // 2006.07.13 // 2008.11.04

	int  PathStatus;
	int  PathDo;
	//
	int  PathIn;
	int  PathOut;
	//
	int  PathHSide;
	//
	int  SlotIn;
	int  SlotOut;
	//
	int  SwitchInOut;
	//
	int  FailOut;
	//
	int  CPJOB_CONTROL;
	int  CPJOB_PROCESS;
	//
	int  Buffer;
	//
	int  Stock; // 2006.04.13
	//
	int  Extra;	// 2006.02.06
	//
	int  LotSpecial; // 2004.11.16
	//
	long StartTime; // 2006.03.29
	long EndTime; // 2006.03.29
	//
	int  Optimize; // 2006.04.04
	//
	int  ClusterIndex; // 2006.07.04
	//
	int  SupplyID; // 2007.09.05
	//
	char *LotUserSpecial; // 2005.01.24
	char *ClusterUserSpecial; // 2005.01.24
	//
	char *ClusterTuneData; // 2007.06.01
	//
	char *UserArea; // 2008.01.21
	char *UserArea2; // 2009.02.02
	char *UserArea3; // 2009.02.02
	char *UserArea4; // 2009.02.02
	//
} Scheduling_Path;

typedef struct {
	char *EILInfo;
	int   EIL_UniqueID;
	//
	int   MtlOut;
	int   CarrierIndex;
	char *JobName;
	char *RecipeName;
	char *PPID;
	char *WaferID;
	char *MaterialID;
	//
	int   OutCarrierIndex; // 2012.04.01
	char *OutMaterialID; // 2012.04.01
	//
	int  UserControl_Mode; // 2011.12.08
	char *UserControl_Data; // 2011.12.08
	//
	int  DisableSkip; // 2012.04.14
	int  PrcsID; // 2013.05.06
	//
	int  UserDummy; // 2015.10.12
	//
	char *PathProcessDepth[ MAX_CLUSTER_DEPTH ]; // 2014.01.28
	//
	int  ClusterSpecial; // 2014.06.23
	//
	int  OrderMode; // 2016.08.25
	//
	int   MtlOutWait; // 2016.11.02
	//
} Scheduling_Info; // 2011.07.20

int  _i_SCH_CLUSTER_FlowControl( int side ); // 2009.02.03

int  _i_SCH_CLUSTER_Copy_Cluster_Data( int tside , int tpointer , int sside , int spointer , int level );
int  _i_SCH_CLUSTER_Get_PathIn( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathIn( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_PathOut( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_PathHSide( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathHSide( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_SlotIn( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_SlotIn( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_SlotOut( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_SlotOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_PathDo( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathDo( int side , int SchPointer , int data );
void _i_SCH_CLUSTER_Inc_PathDo( int side , int SchPointer );
void _i_SCH_CLUSTER_Dec_PathDo( int side , int SchPointer );
int  _i_SCH_CLUSTER_Get_PathStatus( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathStatus( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_PathRange( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathRange( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_PathOrder( int side , int SchPointer , int No , int ch );
int	 _i_SCH_CLUSTER_Get_PathRun( int side , int SchPointer , int No , int mode );
void _i_SCH_CLUSTER_Set_PathRun( int side , int SchPointer , int No , int mode , int data );
int	 _i_SCH_CLUSTER_Get_PathProcessChamber( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Set_PathProcessChamber( int side , int SchPointer , int No , int No2 , int data );
char *_i_SCH_CLUSTER_Get_PathProcessRecipe( int side , int SchPointer , int No , int No2 , int PSide );
char *_i_SCH_CLUSTER_Get_PathProcessRecipe2( int side , int SchPointer , int No , int ch , int PSide );
void _i_SCH_CLUSTER_Disable_PathProcessRecipe_MultiData( int side , int SchPointer , int No , int ch , int PSide , int cutindex );
BOOL _i_SCH_CLUSTER_PathProcessRecipe_MultiData_Unused( int side , int SchPointer , int No );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( int side , int SchPointer , int No , int ch );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( int side , int SchPointer , int No , int ch );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Enable( int side , int SchPointer , int No , int ch );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Disable( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Enable( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Set_PathProcessChamber_Delete( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Clear_PathProcessData( int side , int SchPointer , BOOL prcreset );
int  _i_SCH_CLUSTER_Check_PathProcessData_OnlyOnePM( int side , int SchPointer , int i , int pmc ); // 2009.02.02
void _i_SCH_CLUSTER_Set_PathProcessData( int side , int SchPointer , int No , int No2 , int chamber , char *r1 , char *r2 , char *r3 );
BOOL _i_SCH_CLUSTER_Set_PathProcessData2( int side , int SchPointer , int No , int chamber , char *r1 , char *r2 , char *r3 );
void _i_SCH_CLUSTER_Set_PathProcessData_JustIn( int side , int SchPointer , int No , int No2 , char *r );
void _i_SCH_CLUSTER_Set_PathProcessData_JustPost( int side , int SchPointer , int No , int No2 , char *r );
void _i_SCH_CLUSTER_Set_PathProcessData_JustPre( int side , int SchPointer , int No , int No2 , char *r );
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPre( int side , int SchPointer , int No , int No2 , BOOL OnlyuseSelectPM );
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPre( int side , int ch );
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPre( int side , int SchPointer , int No , int No2 , int ch , BOOL Action );
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPre_GlobalOnly( int ch );
BOOL _i_SCH_CLUSTER_Check_Already_Run_UsedPre( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( int side , int SchPointer , int No );
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPost( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Set_PathProcessData_SkipPost2( int side , int SchPointer , int No , int ch );
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPost( int side , int SchPointer , int No , int No2 );
BOOL _i_SCH_CLUSTER_Check_Possible_UsedPost2( int side , int SchPointer , int ch , int No , int *No2 );
void _i_SCH_CLUSTER_Set_PathProcessData_UsedPost_Restore( int side , int SchPointer , int No );
BOOL _i_SCH_CLUSTER_Check_HasProcessData_Post( int side , int SchPointer , int No , int No2 );
void _i_SCH_CLUSTER_Swap_PathProcessChamber( int side , int SchPointer , int No , int i1 , int i2 );
void _i_SCH_CLUSTER_Move_PathProcessChamber( int side , int SchPointer , int No , int No2 );
int  _i_SCH_CLUSTER_Get_Next_PM_String( int side , int SchPointer , int nextdo , char *NextPM , int maxsize );
int  _i_SCH_CLUSTER_Get_SwitchInOut( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_SwitchInOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_FailOut( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_FailOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_CPJOB_CONTROL( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_CPJOB_CONTROL( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_CPJOB_PROCESS( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_CPJOB_PROCESS( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_Buffer( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_Buffer( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_Stock( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_Stock( int side , int SchPointer , int data );
void _i_SCH_CLUSTER_Inc_Stock( int side , int SchPointer );
void _i_SCH_CLUSTER_Dec_Stock( int side , int SchPointer );
int  _i_SCH_CLUSTER_Get_Optimize( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_Optimize( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_SupplyID( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_SupplyID( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_ClusterIndex( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_ClusterIndex( int side , int SchPointer , int data );
void _i_SCH_CLUSTER_Dec_ClusterIndex( int side , int SchPointer );
int  _i_SCH_CLUSTER_Get_Extra( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_Extra( int side , int SchPointer , int data );
void _i_SCH_CLUSTER_Dec_Extra_Flag( int side , int SchPointer , int flag );
int  _i_SCH_CLUSTER_Set_Extra_Flag( int side , int SchPointer , int flag , int data );
int  _i_SCH_CLUSTER_Get_Extra_Flag( int side , int SchPointer , int flag );
long _i_SCH_CLUSTER_Get_StartTime( int side , int SchPointer );
long _i_SCH_CLUSTER_Get_EndTime( int side , int SchPointer );
void _i_SCH_CLUSTER_Do_StartTime( int side , int SchPointer );
void _i_SCH_CLUSTER_Do_EndTime( int side , int SchPointer );
void _i_SCH_CLUSTER_Init_AllTime( int side , int SchPointer );
int  _i_SCH_CLUSTER_Get_LotSpecial( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_LotSpecial( int side , int SchPointer , int data );
void _i_SCH_CLUSTER_Set_LotUserSpecial( int side , int SchPointer , char *data );
char *_i_SCH_CLUSTER_Get_LotUserSpecial( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_ClusterUserSpecial( int side , int SchPointer , char *data );
char *_i_SCH_CLUSTER_Get_ClusterUserSpecial( int side , int SchPointer );
int  _i_SCH_CLUSTER_Get_ClusterSpecial( int side , int SchPointer ); // 2014.06.23
void _i_SCH_CLUSTER_Set_ClusterSpecial( int side , int SchPointer , int data ); // 2014.06.23
void _i_SCH_CLUSTER_Set_ClusterTuneData( int side , int SchPointer , char *data );
char *_i_SCH_CLUSTER_Get_ClusterTuneData( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_UserArea( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_UserArea2( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_UserArea3( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_UserArea4( int side , int SchPointer , char *data );
char *_i_SCH_CLUSTER_Get_UserArea( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_UserArea2( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_UserArea3( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_UserArea4( int side , int SchPointer );
void _i_SCH_CLUSTER_Set_PathProcessParallel( int side , int SchPointer , int cldepth , char *data );
void _i_SCH_CLUSTER_Set_PathProcessParallelF( int side , int SchPointer , int cldepth , char *data );
void _i_SCH_CLUSTER_Set_PathProcessParallel2( int side , int SchPointer , int cldepth , int pindex , char data );
char *_i_SCH_CLUSTER_Get_PathProcessParallel( int side , int SchPointer , int cldepth );
char _i_SCH_CLUSTER_Get_PathProcessParallel2( int side , int SchPointer , int cldepth , int pindex );
void _i_SCH_CLUSTER_Set_PathProcessDepth( int side , int SchPointer , int cldepth , char *data );
void _i_SCH_CLUSTER_Set_PathProcessDepthF( int side , int SchPointer , int cldepth , char *data );
void _i_SCH_CLUSTER_Set_PathProcessDepth2( int side , int SchPointer , int cldepth , int pindex , char data );
char *_i_SCH_CLUSTER_Get_PathProcessDepth( int side , int SchPointer , int cldepth );
char _i_SCH_CLUSTER_Get_PathProcessDepth2( int side , int SchPointer , int cldepth , int pindex );
BOOL _i_SCH_CLUSTER_Chk_Parallel_Used_Disable( int side , int pt , int cldx , int ch );
void _i_SCH_CLUSTER_Set_Parallel_Used_Restore( int side , int pt , int cldx );
void _i_SCH_CLUSTER_After_Place_For_Parallel_Resetting( int side , int pt , int cldx , int ch );
BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer( int side , int pointer , int dotrg );
void _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( int side , int point );
BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_FDHC( int CHECKING_SIDE , BOOL FMMode , int slot );
BOOL _i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_sub( int CHECKING_SIDE , int *p , BOOL FMMode , BOOL , int slot , BOOL );
void _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop_FDHC( int CHECKING_SIDE , BOOL FMMode );
int  _i_SCH_CLUSTER_Select_OneChamber_and_OtherBuffering( int side , int pointer , int cldepth , int ch , int ch2 );
int  _i_SCH_CLUSTER_Select_OneChamber_Restore_OtherBuffering( int side , int pointer , int cldepth );
void _i_SCH_CLUSTER_Init_ClusterData( int side , BOOL switchmode );
//------------------------------------------------------------------------------------------
int  _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Event_FDHC( int pathin , int slotin , BOOL ); // 2012.02.07
int  _i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_From_Event_FDHC( int pathin , int slotin ); // 2012.12.05
int  _i_SCH_CLUSTER_Check_and_Set_Extra_Flag_From_Event_FDHC( int pathin , int slotin , int flag , int data ); // 2013.02.25
//------------------------------------------------------------------------------------------
// 2011.07.21
void _i_SCH_CLUSTER_Set_Ex_JobName( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_RecipeName( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_PPID( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_WaferID( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_MaterialID( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_MtlOut( int side , int SchPointer , int data ); // 2011.08.10
void _i_SCH_CLUSTER_Set_Ex_MtlOutWait( int side , int SchPointer , int data ); // 2016.11.02
void _i_SCH_CLUSTER_Set_Ex_CarrierIndex( int side , int SchPointer , int data ); // 2011.08.10
void _i_SCH_CLUSTER_Set_Ex_EILInfo( int side , int SchPointer , char *data );
void _i_SCH_CLUSTER_Set_Ex_OutMaterialID( int side , int SchPointer , char *data ); // 2012.04.01
void _i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( int side , int SchPointer , int data ); // 2012.04.01
void _i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( int side , int SchPointer , int data ); // 2012.09.13
void _i_SCH_CLUSTER_Set_Ex_PrcsID( int side , int SchPointer , int data ); // 2013.05.06
void _i_SCH_CLUSTER_Set_Ex_OrderMode( int side , int SchPointer , int data ); // 2016.08.25

int  _i_SCH_CLUSTER_Get_Ex_MtlOut( int side , int SchPointer ); // 2011.08.10
int  _i_SCH_CLUSTER_Get_Ex_MtlOutWait( int side , int SchPointer ); // 2016.11.02
int  _i_SCH_CLUSTER_Get_Ex_CarrierIndex( int side , int SchPointer ); // 2011.08.10
char *_i_SCH_CLUSTER_Get_Ex_JobName( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_RecipeName( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_PPID( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_WaferID( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_MaterialID( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_EILInfo( int side , int SchPointer );
char *_i_SCH_CLUSTER_Get_Ex_OutMaterialID( int side , int SchPointer ); // 2012.04.01
int  _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( int side , int SchPointer ); // 2012.04.01
int  _i_SCH_CLUSTER_Get_Ex_EIL_UniqueID( int side , int SchPointer ); // 2012.09.13
int  _i_SCH_CLUSTER_Get_Ex_PrcsID( int side , int SchPointer ); // 2013.05.06
int  _i_SCH_CLUSTER_Get_Ex_OrderMode( int side , int SchPointer ); // 2016.08.25

//------------------------------------------------------------------------------------------

void _i_SCH_CLUSTER_Set_Ex_UserControl_Mode( int side , int SchPointer , int data ); // 2011.12.08
void _i_SCH_CLUSTER_Set_Ex_UserControl_Data( int side , int SchPointer , char *data ); // 2011.12.08

int  _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( int side , int SchPointer ); // 2011.12.08
//
// ??
// 00 : NotUse
//  _---> 0 : Normal Check = TRUE / 1 : User Check = TRUE / 2 : Normal,User Check = TRUE
// _----> 0 : Sequential 1 : Default
char *_i_SCH_CLUSTER_Get_Ex_UserControl_Data( int side , int SchPointer ); // 2011.12.08

//------------------------------------------------------------------------------------------

void _i_SCH_CLUSTER_Set_Ex_DisableSkip( int side , int SchPointer , int data ); // 2012.04.14
int  _i_SCH_CLUSTER_Get_Ex_DisableSkip( int side , int SchPointer ); // 2012.04.14

//------------------------------------------------------------------------------------------

void _i_SCH_CLUSTER_Set_Ex_UserDummy( int side , int SchPointer , int data ); // 2015.10.12
int  _i_SCH_CLUSTER_Get_Ex_UserDummy( int side , int SchPointer ); // 2015.10.12

//------------------------------------------------------------------------------------------
void _i_SCH_CLUSTER_Set_User_PathIn( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_User_PathIn( int side , int SchPointer );
//
void _i_SCH_CLUSTER_Set_User_PathOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_User_PathOut( int side , int SchPointer );
//
void _i_SCH_CLUSTER_Set_User_SlotOut( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_User_SlotOut( int side , int SchPointer );
//
void _i_SCH_CLUSTER_Set_User_OrderIn( int side , int SchPointer , int data );
int  _i_SCH_CLUSTER_Get_User_OrderIn( int side , int SchPointer );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Inside Structure Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_CLUSTER_Make_SupplyID( int side , int SchPointer ); // 2007.09.05
void _i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( int side , int SchPointer , int ch ); // 2007.09.05
//
void SCHEDULER_CONTROL_Inf_Reset_SupplyID(); // 2007.09.05
void SCHEDULER_CONTROL_Inf_Mode_SupplyID( int side , int mode ); // 2008.12.18
//
void SCHEDULER_CONTROL_Check_OrderMode_SupplyID( int side ); // 2016.08.26
//
//------------------------------------------------------------------------------------------
void _SCH_CLUSTER_Parallel_Check_Curr_DisEna( int side , int pt , int ch , BOOL disable ); // 2010.04.10
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_PRM_CLUSTER_DATA( int apmode , int side );
//------------------------------------------------------------------------------------------

#endif

