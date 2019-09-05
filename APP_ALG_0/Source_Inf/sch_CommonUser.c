#include "INF_Scheduler.h"

#include "sch_A0_CommonUser.h"

//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 0
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
//dll 처음 로드 될때 한번만 수행 되는 함
void _SCH_COMMON_LOAD_INIT( BOOL Reload ) {
//
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 1
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_CONFIG_INITIALIZE( int runalg , int subalg , int guialg , int maxpm ) {
	return _SCH_COMMON_CONFIG_INITIALIZE_STYLE_0( runalg , subalg , guialg , maxpm );
}
//===========================================================================================================
void _SCH_COMMON_INITIALIZE_PART( int apmode , int side ) {
	_SCH_COMMON_INITIALIZE_PART_STYLE_0( apmode , side );
}
//===========================================================================================================
int _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	return _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_0( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM );
}
//===========================================================================================================
void _SCH_COMMON_RUN_PRE_BM_CONDITION( int side , int bmc , int bmmode ) {
	_SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_0( side , bmc , bmmode );
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 2
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
void _SCH_COMMON_RECIPE_DLL_INTERFACE( HINSTANCE dllpt , int unload ) {
	_SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_0( dllpt , unload );
}
//===========================================================================================================
BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG( int chg_ALG_DATA_RUN ) {
	return _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_0( chg_ALG_DATA_RUN );
}
//===========================================================================================================
BOOL _SCH_COMMON_PRE_PART_OPERATION( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
	return _SCH_COMMON_PRE_PART_OPERATION_STYLE_0( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring );
}
//===========================================================================================================
BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE( int side , int WaitParam , int option ) {
	return _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_0( side , WaitParam , option );
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 3
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
int  _SCH_COMMON_GUI_INTERFACE( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
	return _SCH_COMMON_GUI_INTERFACE_STYLE_0( style , alg , mode , data , Message , list , retdata );
}
//===============================================================================
int _SCH_COMMON_EVENT_PRE_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_0( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
int _SCH_COMMON_EVENT_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return _SCH_COMMON_EVENT_ANALYSIS_STYLE_0( FullMessage , RunMessage , ElseMessage , ErrorMessage );
}
//===============================================================================
void _SCH_COMMON_SYSTEM_DATA_LOG( char *filename , int side ) {
	_SCH_COMMON_SYSTEM_DATA_LOG_STYLE_0( filename , side );
}
//===============================================================================
int  _SCH_COMMON_TRANSFER_OPTION( int mode , int data ) {
	return _SCH_COMMON_TRANSFER_OPTION_STYLE_0( mode , data );
}
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
// LEVEL 4
//===========================================================================================================
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_FINISHED_CHECK( int side ) {
	return _SCH_COMMON_FINISHED_CHECK_STYLE_0( side );
}
//===========================================================================================================
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( int mode , int side , int ch1 , int ch2 , int option ) {
	return _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_0( mode , side , ch1 , ch2 , option );
}
//===========================================================================================================
BOOL _SCH_COMMON_GET_PROCESS_FILENAME( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	return _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_0( logpath , lotdir , prcs , ch , side , waferindex1 , waferindex2 , waferindex3 , filename , dummy , mode , notuse , highappend );
}
//===========================================================================================================
void _SCH_COMMON_GET_PROCESS_INDEX_DATA( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
	_SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_0( mode , x , rmode , nomore , xoffset );
}
//===========================================================================================================
BOOL _SCH_COMMON_SIDE_SUPPLY( int side , BOOL EndCheck , int Supply_Style , int *Result ) {
	return _SCH_COMMON_SIDE_SUPPLY_STYLE_0( side , EndCheck , Supply_Style , Result );
}
//===========================================================================================================
BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING() {
	return _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_0();
}
//===========================================================================================================
BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() {
	return _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_0();
}
//===========================================================================================================
BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL( int side , int pt , int ch ) {
	return _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_0( side , pt , ch );
}
//===========================================================================================================
int _SCH_COMMON_PM_2MODULE_SAME_BODY() { // 2007.10.07
	return _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_0();
}
//===========================================================================================================
int _SCH_COMMON_BM_2MODULE_SAME_BODY() {
	return _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_0();
}
//===========================================================================================================
void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING( int side , int bmc , char *appstr ) {
	_SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_0( side , bmc , appstr );
}
//===========================================================================================================
int  _SCH_COMMON_GET_METHOD_ORDER( int side , int pt ) {
	return _SCH_COMMON_GET_METHOD_ORDER_STYLE_0( side , pt );
}
//===========================================================================================================
int  _SCH_COMMON_CHECK_PICK_FROM_FM( int side , int pt , int subchk ) {
	return _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_0( side , pt , subchk );
}
//===========================================================================================================
BOOL _SCH_COMMON_CHECK_END_PART( int side , int mode ) {
	return _SCH_COMMON_CHECK_END_PART_STYLE_0( side , mode );
}
//===========================================================================================================
int _SCH_COMMON_FAIL_SCENARIO_OPERATION( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	return _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_0( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore );
}
//===========================================================================================================
BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( int side ) {
	return _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_0( side );
}
//===========================================================================================================
int  _SCH_COMMON_CHECK_WAITING_MODE( int side , int *mode , int lc_precheck , int option ) {
	return _SCH_COMMON_CHECK_WAITING_MODE_STYLE_0( side , mode , lc_precheck , option );
}
//===========================================================================================================
BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module ,
						int TuneData_Order ,
						int TuneData_Step ,
						char *TuneData_Name ,
						char *TuneData_Data ,
						int TuneData_Index ,
						char *AppendStr ) {
	return _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_0( mode , ch , curorder , pjindex , pjno ,
						TuneData_Module ,
						TuneData_Order ,
						TuneData_Step ,
						TuneData_Name ,
						TuneData_Data ,
						TuneData_Index ,
						AppendStr );
}
//===========================================================================================================

int _SCH_COMMON_USER_FLOW_NOTIFICATION( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ) { // 2015.05.29
	return -1;
}