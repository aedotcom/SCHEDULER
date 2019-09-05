#ifndef SCH_A6_COMMONUSER_H
#define SCH_A6_COMMONUSER_H


//----------------------------------------------------------------------------
// LEVEL 1
//----------------------------------------------------------------------------
BOOL _SCH_COMMON_CONFIG_INITIALIZE_STYLE_6( int runalg , int subalg , int guialg , int maxpm );
void _SCH_COMMON_REVISION_HISTORY_STYLE_6( int mode , char *data , int count );
void _SCH_COMMON_INITIALIZE_PART_STYLE_6( int apmode , int side );
int  _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING_STYLE_6( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM );
void _SCH_COMMON_RUN_PRE_BM_CONDITION_STYLE_6( int side , int bmc , int bmmode );

//----------------------------------------------------------------------------
// LEVEL 2
//----------------------------------------------------------------------------
void _SCH_COMMON_RECIPE_DLL_INTERFACE_STYLE_6( HINSTANCE dllpt , int unload );
BOOL _SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG_STYLE_6( int chg_ALG_DATA_RUN );
BOOL _SCH_COMMON_PRE_PART_OPERATION_STYLE_6( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring );
BOOL _SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE_STYLE_6( int side , int WaitParam , int option );

//----------------------------------------------------------------------------
// LEVEL 3
//----------------------------------------------------------------------------
int  _SCH_COMMON_GUI_INTERFACE_STYLE_6( int style , int alg , int mode , int data , char *Message , char *list , int *retdata );
int  _SCH_COMMON_EVENT_PRE_ANALYSIS_STYLE_6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage );
int  _SCH_COMMON_EVENT_ANALYSIS_STYLE_6( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage );
void _SCH_COMMON_SYSTEM_DATA_LOG_STYLE_6( char *filename , int side );
int  _SCH_COMMON_TRANSFER_OPTION_STYLE_6( int mode , int data );

//----------------------------------------------------------------------------
// LEVEL 4
//----------------------------------------------------------------------------
BOOL _SCH_COMMON_FINISHED_CHECK_STYLE_6( int side );
BOOL _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO_STYLE_6( int mode , int side , int cm1 , int cm2 , int option );
BOOL _SCH_COMMON_GET_PROCESS_FILENAME_STYLE_6( char *logpath , char *lotdir , BOOL prcs , int ch , int side , int waferindex1 , int waferindex2 , int waferindex3 , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend );
void _SCH_COMMON_GET_PROCESS_INDEX_DATA_STYLE_6( int mode , int x , int *rmode , BOOL *nomore , int *xoffset );
BOOL _SCH_COMMON_SIDE_SUPPLY_STYLE_6( int side , BOOL EndCheck , int Supply_Style , int *Result );
BOOL _SCH_COMMON_FM_ARM_APLUSB_SWAPPING_STYLE_6();
BOOL _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL_STYLE_6();
BOOL _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL_STYLE_6( int side , int pt , int ch );
int  _SCH_COMMON_PM_2MODULE_SAME_BODY_STYLE_6();
int  _SCH_COMMON_BM_2MODULE_SAME_BODY_STYLE_6();
void _SCH_COMMON_BM_2MODULE_END_APPEND_STRING_STYLE_6( int side , int bmc , char *appstr );
int  _SCH_COMMON_GET_METHOD_ORDER_STYLE_6( int side , int pt );
int  _SCH_COMMON_CHECK_PICK_FROM_FM_STYLE_6( int side , int pt , int subchk );
BOOL _SCH_COMMON_CHECK_END_PART_STYLE_6( int side , int mode );
int  _SCH_COMMON_FAIL_SCENARIO_OPERATION_STYLE_6( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore );
BOOL _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED_STYLE_6( int side );
int  _SCH_COMMON_CHECK_WAITING_MODE_STYLE_6( int side , int *mode , int lc_precheck , int option );
BOOL _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA_STYLE_6( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index ,
						char *AppendStr );
BOOL _SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START_STYLE_6( int mode , int side , int ch , int slot , int option );
void _SCH_COMMON_ANIMATION_UPDATE_TM_STYLE_6( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ); // 2010.01.29
BOOL _SCH_COMMON_USER_DATABASE_REMAPPING_STYLE_6( int side , int mode , BOOL PreChecking , int ); // 2010.11.09
BOOL _SCH_COMMON_ANIMATION_SOURCE_UPDATE_STYLE_6( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ); // 2010.11.23
int  _SCH_COMMON_MTLOUT_DATABASE_MOVE_STYLE_6( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ); // 2011.05.13


int  _SCH_COMMON_USE_EXTEND_OPTION_STYLE_6( int mode , int sub ); // 2011.11.18

//
#endif

