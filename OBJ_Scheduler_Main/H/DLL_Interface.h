#ifndef DLL_INTERFACE_H
#define DLL_INTERFACE_H

//=========================================================================================
#include "lottype.h"
#include "sch_prm_cluster.h"
//=========================================================================================

//--------------------------------------------------------------------------------------------------------
// 2004.03.15
//--------------------------------------------------------------------------------------------------------
typedef void   (*TypeREM_COMMON_PRINTF_API)			( BOOL , char * , char * , LPSTR , ... );
typedef int    (*TypeREM_FIND_FROM_STRING_API)		( int , char * );
typedef int    (*TypeREM_dWRITE_FUNCTION_API)		( int , char * );
typedef int    (*TypeREM_dREAD_FUNCTION_API)		( int );
typedef void   (*TypeREM_dRUN_FUNCTION_ABORT_API)	( int );
typedef void   (*TypeREM_dWRITE_FUNCTION_EVENT_API)	( int , char * );
typedef int    (*TypeREM_dREAD_FUNCTION_EVENT_API)	( int );
//
typedef void   (*DLL_KNFS_SET_COMMON_PRINTF_API)			( TypeREM_COMMON_PRINTF_API );
typedef void   (*DLL_KNFS_SET_FIND_FROM_STRING_API)			( TypeREM_FIND_FROM_STRING_API );
typedef void   (*DLL_KNFS_SET_dWRITE_FUNCTION_API)			( TypeREM_dWRITE_FUNCTION_API );
typedef void   (*DLL_KNFS_SET_dREAD_FUNCTION_API)			( TypeREM_dREAD_FUNCTION_API );
typedef void   (*DLL_KNFS_SET_dRUN_FUNCTION_ABORT_API)		( TypeREM_dRUN_FUNCTION_ABORT_API );
typedef void   (*DLL_KNFS_SET_dWRITE_FUNCTION_EVENT_API)	( TypeREM_dWRITE_FUNCTION_EVENT_API );
typedef void   (*DLL_KNFS_SET_dREAD_FUNCTION_EVENT_API)		( TypeREM_dREAD_FUNCTION_EVENT_API );

//--------------------------------------------------------------------------------------------------------
// 2005.04.19
//--------------------------------------------------------------------------------------------------------

typedef void   (*TypeREM_dREAD_UPLOAD_MESSAGE_API) ( int , char * );
typedef BOOL   (*TypeREM_dWRITE_UPLOAD_MESSAGE_API) ( char * );

typedef void   (*TypeREM_dWRITE_DIGITAL_API) ( int , int , int * );
typedef int    (*TypeREM_dREAD_DIGITAL_API) ( int , int * );
typedef void   (*TypeREM_dWRITE_ANALOG_API) ( int , double , int * );
typedef double (*TypeREM_dREAD_ANALOG_API) ( int , int * );
typedef void   (*TypeREM_dWRITE_STRING_API) ( int , char * , int * );
typedef void   (*TypeREM_dREAD_STRING_API) ( int , char * , int * );

typedef void   (*DLL_KNFS_SET_dREAD_UPLOAD_MESSAGE_API)		( TypeREM_dREAD_UPLOAD_MESSAGE_API );
typedef void   (*DLL_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API)	( TypeREM_dWRITE_UPLOAD_MESSAGE_API );

typedef void   (*DLL_KNFS_SET_dWRITE_DIGITAL_API)		( TypeREM_dWRITE_DIGITAL_API );
typedef void   (*DLL_KNFS_SET_dREAD_DIGITAL_API)		( TypeREM_dREAD_DIGITAL_API );
typedef void   (*DLL_KNFS_SET_dWRITE_ANALOG_API)		( TypeREM_dWRITE_ANALOG_API );
typedef void   (*DLL_KNFS_SET_dREAD_ANALOGL_API)		( TypeREM_dREAD_ANALOG_API );
typedef void   (*DLL_KNFS_SET_dWRITE_STRING_API)		( TypeREM_dWRITE_STRING_API );
typedef void   (*DLL_KNFS_SET_dREAD_STRING_API)			( TypeREM_dREAD_STRING_API );

//--------------------------------------------------------------------------------------------------------
// 2008.03.04
//--------------------------------------------------------------------------------------------------------

typedef void   (*TypeREM_CIM_PRINTF_API)					( LPSTR , ... );
typedef void   (*TypeREM_COMMON_PRINTF_API)					( BOOL , char * , char * , LPSTR , ... );
typedef void   (*TypeREM_CONSOLE_PRINTF_API)				( LPSTR , ... );
//
typedef BOOL   (*TypeREM_MANAGER_ABORT_API)					( void );
typedef void   (*TypeREM_SYSTEM_ALL_ABORT_API)				( void );

typedef BOOL   (*TypeREM_dRUN_SET_FUNCTION_API)				( int , char * );
typedef int    (*TypeREM_dRUN_FUNCTION_API)					( int , char * );
typedef void   (*TypeREM_dRUN_SET_FUNCTION_FREE_API)		( int , char * );
typedef int    (*TypeREM_dRUN_FUNCTION_FREE_API)			( int , char * );

typedef void   (*DLL_KNFS_SET_CIM_PRINTF_API)				( TypeREM_CIM_PRINTF_API );
typedef void   (*DLL_KNFS_SET_COMMON_PRINTF_API)			( TypeREM_COMMON_PRINTF_API );
typedef void   (*DLL_KNFS_SET_CONSOLE_PRINTF_API)			( TypeREM_CONSOLE_PRINTF_API );
//
typedef void   (*DLL_KNFS_SET_MANAGER_ABORT_API)			( TypeREM_MANAGER_ABORT_API );
typedef void   (*DLL_KNFS_SET_SYSTEM_ALL_ABORT_API)			( TypeREM_SYSTEM_ALL_ABORT_API );
typedef void   (*DLL_KNFS_SET_dRUN_SET_FUNCTION_API)		( TypeREM_dRUN_SET_FUNCTION_API );
typedef void   (*DLL_KNFS_SET_dRUN_FUNCTION_API)			( TypeREM_dRUN_FUNCTION_API );
typedef void   (*DLL_KNFS_SET_dRUN_SET_FUNCTION_FREE_API)	( TypeREM_dRUN_SET_FUNCTION_FREE_API );
typedef void   (*DLL_KNFS_SET_dRUN_FUNCTION_FREE_API)		( TypeREM_dRUN_FUNCTION_FREE_API );

//===================================================================================
typedef void   (*TypeREM_Main_Code_API)				( int );
//===================================================================================
HINSTANCE USER_RECIPE_GET_DLL_INSTANCE();
//
BOOL USER_RECIPE_DLL_LOAD( char *Filename , int mode );
//
BOOL USER_SCH_DLL_LOAD( char *Filename , char *Filename2 , int mode );
//===================================================================================
//===================================================================================
int  USER_RECIPE_USE_TAG_GET();
//
int  USER_RECIPE_DATA_MODE();
BOOL USER_RECIPE_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC );
BOOL USER_RECIPE_DEFAULT_DATA_REGIST( char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC , char *DispStr );
int  USER_RECIPE_PM_MODE( int REGID , int side , char *allname , char *lotname , char *grpname );
BOOL USER_RECIPE_INOUT_DATA_READ( int REGID , int side , CASSETTETemplate *OUT_CASSETTE );
BOOL USER_RECIPE_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *runname );
BOOL USER_RECIPE_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *runname ); // 2003.09.24
BOOL USER_RECIPE_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *name );
BOOL USER_RECIPE_CLUSTER_DATA_READ2( int REGID , int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , int Slot , char *name ); // 2012.06.18
BOOL USER_RECIPE_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO );

int  USER_RECIPE_MANUAL_DATA_MODE();
BOOL USER_RECIPE_MANUAL_DEFAULT_DATA_READ( int REGID , char *InfoStr , int *SC , int *EC , char *JobStr , char *LotStr , char *MidStr , int *SS , int *ES , int *LC );
int  USER_RECIPE_MANUAL_PM_MODE( int REGID , int side , char *allname , char *lotname , char *grpname );
BOOL USER_RECIPE_MANUAL_INOUT_DATA_READ( int REGID , int side , CASSETTETemplate *OUT_CASSETTE );
BOOL USER_RECIPE_MANUAL_LOT_DATA_READ( int REGID , int side , LOTStepTemplate *LOT_RECIPE , char *runname );
BOOL USER_RECIPE_MANUAL_LOT_DUMMY_DATA_READ( int REGID , int side , LOTStepTemplate2 *LOT_RECIPE_DUMMY , char *runname ); // 2003.09.24
BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ( int REGID , int side , CLUSTERStepTemplate *CLUSTER_RECIPE , int Slot , char *name );
BOOL USER_RECIPE_MANUAL_CLUSTER_DATA_READ2( int REGID , int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , int Slot , char *name ); // 2012.06.18
BOOL USER_RECIPE_MANUAL_MAPPING_VERIFICATION( int REGID , int side , int position , CASSETTEVerTemplate MAPPING_INFO );
//=========================================================================================
//=========================================================================================
BOOL USER_RECIPE_INFO_VERIFICATION( int REGID , int side , int loopcount , int id , CLUSTERStepTemplate CLUSTER_INFO ); // 2004.12.14
BOOL USER_RECIPE_INFO_VERIFICATION_EX( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData ); // 2005.01.25
BOOL USER_RECIPE_INFO_VERIFICATION_EX2( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate2 CLUSTER_INFO , int LotSpecialData , char *LotUserData , char *ClusterUserData , char *ClusterTuneData ); // 2005.01.25
BOOL USER_RECIPE_INFO_VERIFICATION_EX3( int REGID , int side , int pos , int loopcount , int orderindex , int id , CLUSTERStepTemplate2 *CLUSTER_INFO , int LotSpecialData , char *LotUserdata , char *ClusterUserdata , char *clusterTuneData ); // 2008.07.24
BOOL USER_RECIPE_INFO_VERIFICATION_EX4( int REGID , int side , int pos , int loopcount , int orderindex , int id , Scheduling_Path *CLUSTER_DATA ); // 2009.04.01
//
BOOL USER_RECIPE_INFO_VERIFICATION_API_EXIST(); // 2004.12.14
BOOL USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST(); // 2005.01.24
BOOL USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST(); // 2007.04.25
BOOL USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST(); // 2008.07.24
BOOL USER_RECIPE_INFO_VERIFICATION_EX4_API_EXIST(); // 2009.04.02

BOOL USER_RECIPE_INFO_VERIFICATION_EIL( int side , int pos , int bm , int slot , int cm , int wafer , char * , char *feedbackstr ); // 2010.09.16

void USER_RECIPE_INFO_FLOWING( int side , int mode , int smode , int precheck , int etc , char *etc2 ); // 2010.12.02
//
BOOL USER_RECIPE_START_MODE_CHANGE_API_EXIST(); // 2005.08.02
BOOL USER_RECIPE_START_MODE_CHANGE( int REGID , int side , int loopcount , int firstmode , int *startmode ); // 2005.08.02
BOOL USER_RECIPE_JOB_DEFAULT_MODE_CHANGE( BOOL runstyle , int REGID , int cassin , int cassout , char *jobname , char *lotname , char *midname , int *startslot , int *endslot , int *startmode , int *loopcount ); // 2007.03.27
//
BOOL USER_RECIPE_ACTION_AFTER_READY( int side ); // 2005.10.26

//=========================================================================================
//=========================================================================================
BOOL USER_RECIPE_TUNE_DATA_APPEND( int side , int mode , int ch , int slot , char *allrcp , char *Buffer ); // 2007.06.11

int  USER_RECIPE_DUMMY_OPERATE( int side , int pointer , int pathin , int slotin , int mode , int , int , int , int *bmrealindex , int *bmrealslot , char *clusterrecipe );

int  USER_RECIPE_DUMMY_INFO_VERIFICATION( int side , int pointer , int pathin , int slotin , int selpm , int param , char *clusterrecipe , Scheduling_Path *CLUSTER_DATA ); // 2009.05.20

int  USER_RECIPE_PROCESS_FINISH_FOR_DUMMY( int , int side , int pointer , int ch , int runorder , int PathRange , int cutorder , int cutxindex , int order , char *recipe , Scheduling_Path *CLUSTER_DATA ); // 2009.05.28

int  USER_RECIPE_DATA_PRE_APPEND( int side , int mode ); // 2010.05.08

int  USER_RECIPE_GET_MODULE_FROM_ALARM_INFO( int moduleindex , char *ALARM_MODULE , int ALARM_MODULE_INDEX , int ALARM_ID , char *ALARM_NAME , char *ALARM_MESSAGE , int ALARM_LEVEL , int ALARM_STYLE , int ALARM_HAPPEN , char *ALARM_HAPPEN_INFO , int *findex ); // 2012.03.08

BOOL USER_RECIPE_LOOPCOUNT_CHANGE( int REGID , int side , int precheck , int *loopcount ); // 2014.05.13

BOOL USER_RECIPE_LOT_SLOTFIX_SETTING( int side , int pt , int PreCheck , int Mode , int *PathIn , int *SlotIn , int *PathOut , int *SlotOut ); // 2015.10.12

//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
int  USER_SCH_USE_TAG_GET();

//=========================================================================================
void _SCH_INF_DLL_CONTROL_REVISION_HISTORY( int mode , char *data , int count );
void _SCH_INF_DLL_CONTROL_RUN_PRE_BM_CONDITION( int side , int bmc , int mode );
BOOL _SCH_INF_DLL_CONTROL_RUN_MFIC_COMPLETE_MACHINE( int , int , int );
int  _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM );
BOOL _SCH_INF_DLL_CONTROL_READING_CONFIG_FILE_WHEN_CHANGE_ALG( int chg_ALG_DATA_RUN );
BOOL _SCH_INF_DLL_CONTROL_RUN_PRE_OPERATION( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring );

//=========================================================================================
BOOL _SCH_INF_DLL_CONTROL_CONFIG_INITIALIZE( int runalg , int subalg , int guialg , int maxpm );
void _SCH_INF_DLL_CONTROL_RUN_INIT( int apmode , int side );
void _SCH_INF_DLL_CONTROL_SET_INSTANCE( HINSTANCE dllpt , int );
int  _SCH_INF_DLL_CONTROL_GUI_INTERFACE( int , int , int , int , char *Message , char *list , int * );
int  _SCH_INF_DLL_CONTROL_EVENT_ANALYSIS( char * , char * , char * , char * );
int  _SCH_INF_DLL_CONTROL_EVENT_PRE_ANALYSIS( char * , char * , char * , char * );
void _SCH_INF_DLL_CONTROL_SYSTEM_LOG( char *filename , int side );
void _SCH_INF_DLL_CONTROL_SYSTEM_FAIL( BOOL errorfinish , int side , int data1 , int data2 , int data3 ); // 2017.07.26
int  _SCH_INF_DLL_CONTROL_TRANSFER_OPTION( int mode , int data );
//=========================================================================================
BOOL _SCH_INF_DLL_CONTROL_FINISHED_CHECK( int side );
BOOL _SCH_INF_DLL_CONTROL_CASSETTE_CHECK_INVALID_INFO( int mode , int side , int ch1 , int ch2 , int option );
BOOL _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME( char *logpath , char *lotdir , int prcs , int ch , int side , int i , int j , int e , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend );
BOOL _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME2( char *logpath , char *lotdir , int prcs , int ch , int side , int pt , int *i , int *j , int *e , char *filename , char *foldername , BOOL *dummy , int *mode , BOOL *notuse , BOOL *highappend , BOOL *filenotuse );
int  _SCH_INF_DLL_CONTROL_MESSAGE_CONTROL_FOR_USER( BOOL postmode , int id , int i1 , int i2 , int i3 , int i4 , int i5 , int i6 , int i7 , int i8 , int i9 , int i10 , double d1 , double d2 , double d3 , char *s1 , char *s2 , char *s3 ); // 2012.11.29
void _SCH_INF_DLL_CONTROL_GET_PROCESS_INDEX_DATA( int mode , int x , int *rmode , BOOL *nomore , int *xoffset );
BOOL _SCH_INF_DLL_CONTROL_SIDE_SUPPLY( int side , BOOL EndCheck , int sup_style , int *res );
BOOL _SCH_INF_DLL_CONTROL_FM_ARM_APLUSB_SWAPPING();
BOOL _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_SKIP_CONTROL( int side , int pt , int ch );
BOOL _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_DISABLE_CONTROL();
int  _SCH_INF_DLL_CONTROL_PM_2MODULE_SAME_BODY();
int  _SCH_INF_DLL_CONTROL_BM_2MODULE_SAME_BODY();
void _SCH_INF_DLL_CONTROL_BM_2MODULE_END_APPEND_STRING( int side , int bmc , char *appstr );
int  _SCH_INF_DLL_CONTROL_GET_METHOD_ORDER( int side , int pt );
int  _SCH_INF_DLL_CONTROL_CHECK_PICK_FROM_FM( int side , int pt , int subchk );
BOOL _SCH_INF_DLL_CONTROL_GET_PT_AT_PICK_FROM_CM( int side , int *pt , BOOL fmmode , int subchk ); // 2014.06.16
void _SCH_INF_DLL_CONTROL_HANDOFF_ACTION( int cm , int mode , int subchk ); // 2014.06.27
BOOL _SCH_INF_DLL_CONTROL_CHECK_END_PART( int side , int mode );
int  _SCH_INF_DLL_CONTROL_FAIL_SCENARIO_OPERATION( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore );
BOOL _SCH_INF_DLL_CONTROL_EXTEND_FM_ARM_CROSS_FIXED( int side );
BOOL _SCH_INF_DLL_CONTROL_CHECK_WAITING_MODE( int side , int *mode , BOOL precheck , int lc );
BOOL _SCH_INF_DLL_CONTROL_TUNING_GET_MORE_APPEND_DATA( int mode , int ch , int curorder , int pjindex , int pjno ,
						int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index , char *AppendStr );
BOOL _SCH_INF_DLL_CONTROL_METHOD_CHECK_SKIP_WHEN_START( int mode , int side , int ch , int slot , int option );
void _SCH_INF_DLL_CONTROL_ANIMATION_UPDATE_TM( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ); // 2009.07.16
//
BOOL _SCH_INF_DLL_INFO_VERIFICATION_EIL( int side , int pos , int bm , int slot , int cm , int wafer , char * , char *feedbackstr ); // 2010.09.16
//
BOOL _SCH_INF_DLL_CONTROL_USER_DATABASE_REMAPPING( int Side , int Mode , BOOL , int ); // 2010.11.09

BOOL _SCH_INF_DLL_CONTROL_ANIMATION_SOURCE_UPDATE( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ); // 2010.11.23
BOOL _SCH_INF_DLL_CONTROL_ANIMATION_RESULT_UPDATE( int side , int pt , int data , int sub ); // 2012.03.15
BOOL _SCH_INF_DLL_CONTROL_MTLOUT_DATABASE_MOVE( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ); // 2011.05.13

int  _SCH_INF_DLL_CONTROL_USE_EXTEND_OPTION( int mode , int sub ); // 2011.11.18
int  _SCH_INF_DLL_CONTROL_USER_RECIPE_CHECK( int mode , int side , int data1 , int data2 , int data3 , int *res ); // 2012.01.12

int  _SCH_INF_DLL_CONTROL_USER_FLOW_NOTIFICATION( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ); // 2015.05.29

BOOL _SCH_INF_DLL_CONTROL_GET_UPLOAD_MESSAGE( int mode , int ch , char *message ); // 2017.01.03

BOOL _SCH_INF_DLL_CONTROL_GET_DUMMY_METHOD_INDEX( int ch , int mode , int wh , int side , int pt , int *GetIndex ); // 2017.09.11

BOOL _SCH_INF_DLL_CONTROL_FA_SEND_MESSAGE( int mode , int side , int pt , char *orgmsg , char *newmessageBuff , int newmessageBuffSize ); // 2018.06.14

BOOL _SCH_INF_DLL_CONTROL_FA_ORDER_MESSAGE( int mode , int side , int pt ); // 2018.06.29

//=========================================================================================
// for 99
//=========================================================================================
int  USER_DLL_SCH_INF_ENTER_CONTROL_FEM( int side , int mode );
int  USER_DLL_SCH_INF_ENTER_CONTROL_TM( int side , int mode );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_1( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_2( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_3( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_4( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_5( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_6( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_7( int side );
int  USER_DLL_SCH_INF_RUN_CONTROL_TM_8( int side );

int  USER_DLL_SCH_INF_RUN_CONTROL_FEM( int side );

// 2011.12.08

int USER_DLL_SCH_INF_HAS_USER_CONTROL_FEM();
int USER_DLL_SCH_INF_HAS_USER_CONTROL_TM();

int USER_DLL_SCH_INF_USER_CONTROL_FEM( int side , int pointer , int mode , char *data );

int USER_DLL_SCH_INF_USER_CONTROL_TM( int side , int pointer , int mode , char *data );

// 2012.09.19

int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_FEM();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_1();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_2();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_3();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_4();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_5();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_6();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_7();
int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_8();

int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_FEM( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_1( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_2( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_3( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_4( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_5( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_6( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_7( int side , int mode , int etc );
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_8( int side , int mode , int etc );
//=========================================================================================

#endif

