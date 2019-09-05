#include "default.h"

#include "EQ_Enum.h"

#include "lottype.h"
#include "iolog.h"
#include "DLL_Interface.h"
#include "User_Parameter.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "INF_Sch_Macro.h"
//-----------------------------------------------------------------------------------
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
LPARAM _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
typedef LPARAM (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
typedef LPARAM (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
typedef LPARAM (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
typedef LPARAM (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
typedef LPARAM (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 );

typedef void   (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API)( DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A );
typedef void   (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API)( DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B );
typedef void   (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API)( DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C );
typedef void   (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API)( DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D );
typedef void   (*DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API)( DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
typedef void   (*DLL_SCH_INF_LOAD_INIT)( BOOL ); // 2008.01.18

typedef BOOL   (*DLL_SCH_INF_LOAD_SIZE)( int , int ); // 2018.08.22

typedef void   (*DLL_SCH_INF_REVISION_HISTORY)( int , char * , int ); // 2008.06.21
typedef void   (*DLL_SCH_INF_RUN_INIT)( int , int ); // 2008.01.18
typedef void   (*DLL_SCH_INF_SET_DLL_INSTANCE)( HINSTANCE , int ); // 2008.03.07
typedef void   (*DLL_SCH_INF_RUN_PRE_BM)( int , int , int ); // 2008.01.18
typedef BOOL   (*DLL_SCH_INF_RUN_MFIC_COMPLETE_MACHINE)( int , int , int ); // 2008.01.18
typedef int    (*DLL_SCH_INF_RUN_CONTROL)( int ); // 2008.01.18
typedef int    (*DLL_SCH_INF_ENTER_CONTROL)( int , int );
typedef int    (*DLL_SCH_INF_USER_CONTROL)( int , int , int , char * ); // 2011.12.08
typedef int    (*DLL_SCH_INF_RESTART_PRE_CONTROL)( int , int , int ); // 2012.09.17
typedef int    (*DLL_SCH_INF_GUI_INTERFACE)( int , int , int , int , char * , char * , int * );
typedef int    (*DLL_SCH_INF_EVENT_PRE_ANALYSIS)( char * , char * , char * , char * );
typedef int    (*DLL_SCH_INF_EVENT_ANALYSIS)( char * , char * , char * , char * );
typedef void   (*DLL_SCH_INF_SYSTEM_LOG)( char * , int );
typedef void   (*DLL_SCH_INF_SYSTEM_FAIL)( BOOL errorfinish , int side , int data1 , int data2 , int data3 ); // 2017.07.26
typedef int    (*DLL_SCH_INF_TRANSFER_OPTION)( int , int );
typedef int    (*DLL_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING)( int , int , char * , int * );
typedef BOOL   (*DLL_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG)( int );
typedef BOOL   (*DLL_SCH_INF_RUN_PRE_OPERATION)( int , int , int * , int * , int * , char * );

typedef BOOL   (*DLL_SCH_INF_FINISHED_CHECK)( int );
typedef BOOL   (*DLL_SCH_INF_CASSETTE_CHECK_INVALID_INFO)( int , int , int , int , int );
typedef BOOL   (*DLL_SCH_INF_GET_PROCESS_FILENAME)( char * , char * , int , int , int , int , int , int , char * , BOOL , int , BOOL , BOOL );
typedef BOOL   (*DLL_SCH_INF_GET_PROCESS_FILENAME2)( char * , char * , int , int , int , int , int * , int * , int * , char * , char * , BOOL * , int * , BOOL * , BOOL * , BOOL * );
typedef void   (*DLL_SCH_INF_GET_PROCESS_INDEX_DATA)( int , int , int * , BOOL * , int * );
typedef BOOL   (*DLL_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER)( BOOL , int , int , int , int , int , int , int , int , int , int , int , double , double , double , char * , char * , char * );
typedef BOOL   (*DLL_SCH_INF_SIDE_SUPPLY)( int , BOOL , int , int * );
typedef BOOL   (*DLL_SCH_INF_FM_ARM_APLUSB_SWAPPING)();
typedef BOOL   (*DLL_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL)( int , int , int );
typedef BOOL   (*DLL_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL)();
typedef int    (*DLL_SCH_INF_PM_2MODULE_SAME_BODY)();
typedef int    (*DLL_SCH_INF_BM_2MODULE_SAME_BODY)();
typedef void   (*DLL_SCH_INF_BM_2MODULE_END_APPEND_STRING)( int , int , char * );
typedef int    (*DLL_SCH_INF_GET_METHOD_ORDER)( int , int );
typedef int    (*DLL_SCH_INF_CHECK_PICK_FROM_FM)( int , int , int );
typedef BOOL   (*DLL_SCH_INF_GET_PT_AT_PICK_FROM_CM)( int , int * , BOOL , int ); // 2014.06.16
typedef void   (*DLL_SCH_INF_HANDOFF_ACTION)( int , int , int ); // 2014.06.27
typedef BOOL   (*DLL_SCH_INF_CHECK_END_PART)( int , int );
typedef int    (*DLL_SCH_INF_FAIL_SCENARIO_OPERATION)( int , int , BOOL , int , BOOL , int * , int * , int * , int * , BOOL * );
typedef BOOL   (*DLL_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED)( int );
typedef int    (*DLL_SCH_INF_CHECK_WAITING_MODE)( int , int * , int , int );
typedef BOOL   (*DLL_SCH_INF_TUNING_GET_MORE_APPEND_DATA)( int , int , int , int , int , int , int , int , char * , char * , int , char * );
typedef BOOL   (*DLL_SCH_INF_METHOD_CHECK_SKIP_WHEN_START)( int , int , int , int , int );
typedef void   (*DLL_SCH_INF_ANIMATION_UPDATE_TM)( int , int , BOOL , int , int , int , int , int ); // 2009.07.16
typedef BOOL   (*DLL_SCH_INF_USER_DATABASE_REMAPPING)( int , int , BOOL , int ); // 2010.11.09
typedef BOOL   (*DLL_SCH_INF_ANIMATION_SOURCE_UPDATE)( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ); // 2010.11.23
typedef int    (*DLL_SCH_INF_ANIMATION_RESULT_UPDATE)( int side , int pt , int data , int sub ); // 2012.03.15
typedef BOOL   (*DLL_SCH_INF_MTLOUT_DATABASE_MOVE)( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ); // 2011.05.13
typedef int    (*DLL_SCH_INF_USE_EXTEND_OPTION)( int mode , int sub ); // 2011.11.18
typedef int    (*DLL_SCH_INF_USER_RECIPE_CHECK)( int mode , int side , int data1 , int data2 , int data3 , int *res ); // 2012.01.12
typedef int    (*DLL_SCH_INF_USER_FLOW_NOTIFICATION)( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ); // 2015.05.29
typedef BOOL   (*DLL_SCH_INF_GET_UPLOAD_MESSAGE)( int mode , int ch , char *message ); // 2017.01.03
typedef BOOL   (*DLL_SCH_INF_GET_DUMMY_METHOD_INDEX)( int ch , int mode , int wh , int side , int pt , int *GetIndex ); // 2017.09.11
typedef BOOL   (*DLL_SCH_INF_FA_SEND_MESSAGE)( int mode , int side , int pt , char *orgmsg , char *newmessageBuff , int newmessageBuffSize ); // 2018.06.14
typedef BOOL   (*DLL_SCH_INF_FA_ORDER_MESSAGE)( int mode , int side , int pt ); // 2018.06.29

typedef void   (*TypeREM_SCH_INF_DLL_CONTROL_LOT_PRINTF_API)			( int , LPSTR , ... );
typedef void   (*TypeREM_SCH_INF_DLL_CONTROL_DEBUG_PRINTF_API)			( int , int , LPSTR , ... );
typedef void   (*TypeREM_SCH_INF_DLL_CONTROL_TIMER_PRINTF_API)			( int , int , int , int , int , int , int , char * , char * );

typedef void   (*TypeREM_SCH_INF_DLL_CIM_IO_CIM_PRINTF_API)		( LPSTR , ... );
typedef void   (*TypeREM_SCH_INF_DLL_CIM_IO_COMMON_PRINTF_API)	( BOOL , char * , char * , LPSTR , ... );
typedef void   (*TypeREM_SCH_INF_DLL_CIM_IO_CONSOLE_PRINTF_API)	( LPSTR , ... );

typedef void   (*DLL_SCH_INF_LOG_CONTROL_LOT_PRINTF_API)		( TypeREM_SCH_INF_DLL_CONTROL_LOT_PRINTF_API );
typedef void   (*DLL_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API)		( TypeREM_SCH_INF_DLL_CONTROL_DEBUG_PRINTF_API );
typedef void   (*DLL_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API)		( TypeREM_SCH_INF_DLL_CONTROL_TIMER_PRINTF_API );

typedef void   (*DLL_SCH_INF_CIM_CONTROL_IO_CIM_PRINTF_API)		( TypeREM_SCH_INF_DLL_CIM_IO_CIM_PRINTF_API );
typedef void   (*DLL_SCH_INF_CIM_CONTROL_IO_COMMON_PRINTF_API)	( TypeREM_SCH_INF_DLL_CIM_IO_COMMON_PRINTF_API );
typedef void   (*DLL_SCH_INF_CIM_CONTROL_IO_CONSOLE_PRINTF_API)	( TypeREM_SCH_INF_DLL_CIM_IO_CONSOLE_PRINTF_API );

typedef BOOL   (*DLL_SCH_RECIPE_INFO_VERIFICATION_EIL)( int , int , int , int , int , int , char * , char * ); // 2010.09.16


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
typedef BOOL   (*DLL_SCH_INF_CONFIG_INITIALIZE)		( int , int , int , int );

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

//======================
// Macro Start : Type
//======================

typedef int  ( *Type_MACRO_FM_POSSIBLE_PICK_FROM_FM )( int side , int *cm , int *pt , int subcheck );
typedef void ( *Type_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE )( int side , int count );
typedef int  ( *Type_MACRO_FM_PICK_DATA_FROM_FM )( int fms , BOOL doubletm , int mode , int CHECKING_SIDE , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *retpointer );
typedef int  ( *Type_MACRO_FM_PICK_DATA_FROM_FM_DETAIL )( int fms , BOOL doubletm , int mode , int CHECKING_SIDE , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *retpointer , int *FM_HSide , int *FM_HSide2 , BOOL *cpreject );
typedef void ( *Type_MACRO_FM_SUPPLY_FIRSTLAST_CHECK )( int side , int pointer , int firsttag , int lasttag , int cmstarteveryskip );
typedef BOOL ( *Type_MACRO_FM_FINISHED_CHECK )( int side , BOOL supplycheck );
typedef int  ( *Type_MACRO_INTERRUPT_PART_CHECK )( int side , BOOL patial , int tmthreadblockingcheck );
typedef int  ( *Type_MACRO_FM_OPERATION )( int fms , int mode , int ch1 , int slot1 , int mslot1 , int side1 , int pointer1 , int ch2 , int slot2 , int mslot2 , int side2 , int pointer2 , int fs );
typedef void ( *Type_MACRO_FM_DATABASE_OPERATION )( int fms , int mode , int och1 , int slot1 , int omslot1 , int side1 , int pointer1 , int och2 , int slot2 , int omslot2 , int side2 , int pointer2 , int mode1 , int mode2 );
typedef int  ( *Type_MACRO_FM_PLACE_TO_CM )( int CHECKING_SIDE , int FM_TSlot0 , int FM_Side , int FM_Pointer , int FM_TSlot20 , int FM_Side2 , int FM_Pointer2 , BOOL errorout , BOOL disappear , int realslotoffset ); // 2007.07.11
typedef int  ( *Type_MACRO_TM_OPERATION_0 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_1 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_2 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_3 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_4 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_5 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_6 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_OPERATION_7 )( int mode , int side , int pointer , int ch , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip , int dide , int dointer );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_0 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_1 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_2 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_3 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_4 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_5 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_6 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef int  ( *Type_MACRO_TM_MOVE_OPERATION_7 )( int side , int mode , int ch , int arm , int slot , int depth );
typedef BOOL ( *Type_MACRO_TM_DATABASE_OPERATION )( int tms , int mode , int och , int arm , int slot1 , int slot2 , int side1 , int pointer1 , int side2 , int pointer2 , int order , int type );
typedef int  ( *Type_MACRO_CHECK_PROCESSING )( int ch );
typedef int  ( *Type_MACRO_CHECK_PROCESSING_INFO )( int ch );
typedef int  ( *Type_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME )( int ch , int wafer , int *tdata , int willch );
typedef int  ( *Type_MACRO_MAIN_PROCESS )( int side , int pointer ,
								  int ch , int rch ,
								  int slot , int pslot , int arm , int incm ,
								  char *recipe , int tmruntime , char *NextPMStr , int mintime ,
								  int prctag1 , int prctag2 , int prctag3 ,
								  int dide , int dointer , int pmstrtopt , int multiwfr ,
								  int actionoption , int optdata );
typedef int  ( *Type_MACRO_MAIN_PROCESS_PART_SIMPLE )( int side , int pointer , int pathdo , int ch , int arm , int optdata );
typedef int  ( *Type_MACRO_MAIN_PROCESS_PART_MULTIPLE )( int side , int pointer , int pathdo , int ch , int arm , BOOL firstnotuse , int optdata );
typedef void ( *Type_MACRO_PROCESS_PART_TAG_SETTING )( int ch , int status , int side , int pointer , int no1 , int no2 , char *recipe , int last );
typedef int  ( *Type_MACRO_LOTPREPOST_PROCESS_OPERATION )( int side ,
								 int ch ,
								 int incm ,
								 int slot ,
								 int arm ,
								 char *recipe ,
								 int modeid ,
								 int puttime , char *nextpm , int mintime ,
								 int mode , int optdata );
typedef int  ( *Type_MACRO_PRE_PROCESS_OPERATION )( int side , int pt , int arm , int ch );
typedef int  ( *Type_MACRO_POST_PROCESS_OPERATION )( int side , int pt , int arm , int ch );
typedef BOOL ( *Type_MACRO_HAS_PRE_PROCESS_OPERATION )( int side , int pt , int ch );
typedef void ( *Type_MACRO_SPAWN_WAITING_BM_OPERATION )( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata );
typedef int  ( *Type_MACRO_FM_ALIC_OPERATION )( int fms , int Mode , int side , int chamber , int alic_slot1 , int alic_slot2 , int SrcCas , int swch , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int side1 , int side2 , int pointer , int pointer2 );
typedef int  ( *Type_MACRO_FM_POSSIBLE_PICK_FROM_CM )( int side , int *cm , int *pt ); // 2007.02.01
typedef BOOL ( *Type_MACRO_FM_OPERATION_THREAD_WAITING )( int fms );
typedef int  ( *Type_MACRO_FM_OPERATION_THREAD_RESULT )( int fms );
typedef BOOL ( *Type_MACRO_FM_OPERATION_MOVE_THREAD_STARTING )( int fms , int slot , int side , int pointer , int pick_mdl , int pick_slot , int place_mdl , int place_slot );
typedef BOOL ( *Type_MACRO_FM_OPERATION_MOVE_STARTING )( int fms , int slot , int side , int pointer , int pick_mdl , int pick_slot , int place_mdl , int place_slot );
typedef void ( *Type_MACRO_PLACE_TO_CM_POST_PART_with_FM )( int FM_TSlot , int FM_CO1 , int FM_OSlot , int FM_Side , int FM_Pointer , int FM_TSlot2 , int FM_CO2 , int FM_OSlot2 , int FM_Side2 , int FM_Pointer2 , BOOL NormalMode ); // 2007.07.11
typedef int  ( *Type_MACRO_RUN_WAITING_BM_OPERATION )(   int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata );
typedef void ( *Type_MACRO_OTHER_LOT_LOAD_WAIT )( int side , unsigned int timeoutsec ); // 2007.09.06
typedef int  ( *Type_MACRO_SPAWN_FM_MALIGNING )( int side , int Slot1 , int Slot2 , int SrcCas , int swch ); // 2008.01.16
typedef int  ( *Type_MACRO_CHECK_FM_MALIGNING )( int side , BOOL waitfinish ); // 2008.01.16
typedef int  ( *Type_MACRO_SPAWN_FM_MCOOLING )( int side , int icslot1 , int icslot2 , int Slot1 , int Slot2 , int SrcCas ); // 2008.02.15
typedef int  ( *Type_MACRO_CHECK_FM_MCOOLING )( int side , int icslot , BOOL waitfinish ); // 2008.02.15
typedef int  ( *Type_MACRO_SPAWN_TM_MALIGNING )( int CHECKING_SIDE , int Finger , int Slot , int SrcCas );
typedef int  ( *Type_MACRO_CHECK_TM_MALIGNING )( int side , BOOL waitfinish );
typedef int  ( *Type_MACRO_FM_MOVE_OPERATION )( int fms , int side , int mode , int ch , int slot1 , int slot2 );

//

typedef void ( *DLL_API_MACRO_FM_POSSIBLE_PICK_FROM_FM )( Type_MACRO_FM_POSSIBLE_PICK_FROM_FM );
typedef void ( *DLL_API_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE )( Type_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE );
typedef void ( *DLL_API_MACRO_FM_PICK_DATA_FROM_FM )( Type_MACRO_FM_PICK_DATA_FROM_FM );
typedef void ( *DLL_API_MACRO_FM_PICK_DATA_FROM_FM_DETAIL )( Type_MACRO_FM_PICK_DATA_FROM_FM_DETAIL );
typedef void ( *DLL_API_MACRO_FM_SUPPLY_FIRSTLAST_CHECK )( Type_MACRO_FM_SUPPLY_FIRSTLAST_CHECK );
typedef void ( *DLL_API_MACRO_FM_FINISHED_CHECK )( Type_MACRO_FM_FINISHED_CHECK );
typedef void ( *DLL_API_MACRO_INTERRUPT_PART_CHECK )( Type_MACRO_INTERRUPT_PART_CHECK );
typedef void ( *DLL_API_MACRO_FM_OPERATION )( Type_MACRO_FM_OPERATION );
typedef void ( *DLL_API_MACRO_FM_DATABASE_OPERATION )( Type_MACRO_FM_DATABASE_OPERATION );
typedef void ( *DLL_API_MACRO_FM_PLACE_TO_CM )( Type_MACRO_FM_PLACE_TO_CM );
typedef void ( *DLL_API_MACRO_TM_OPERATION_0 )( Type_MACRO_TM_OPERATION_0 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_1 )( Type_MACRO_TM_OPERATION_1 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_2 )( Type_MACRO_TM_OPERATION_2 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_3 )( Type_MACRO_TM_OPERATION_3 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_4 )( Type_MACRO_TM_OPERATION_4 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_5 )( Type_MACRO_TM_OPERATION_5 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_6 )( Type_MACRO_TM_OPERATION_6 );
typedef void ( *DLL_API_MACRO_TM_OPERATION_7 )( Type_MACRO_TM_OPERATION_7 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_0 )( Type_MACRO_TM_MOVE_OPERATION_0 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_1 )( Type_MACRO_TM_MOVE_OPERATION_1 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_2 )( Type_MACRO_TM_MOVE_OPERATION_2 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_3 )( Type_MACRO_TM_MOVE_OPERATION_3 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_4 )( Type_MACRO_TM_MOVE_OPERATION_4 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_5 )( Type_MACRO_TM_MOVE_OPERATION_5 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_6 )( Type_MACRO_TM_MOVE_OPERATION_6 );
typedef void ( *DLL_API_MACRO_TM_MOVE_OPERATION_7 )( Type_MACRO_TM_MOVE_OPERATION_7 );
typedef void ( *DLL_API_MACRO_TM_DATABASE_OPERATION )( Type_MACRO_TM_DATABASE_OPERATION );
typedef void ( *DLL_API_MACRO_CHECK_PROCESSING )( Type_MACRO_CHECK_PROCESSING );
typedef void ( *DLL_API_MACRO_CHECK_PROCESSING_INFO )( Type_MACRO_CHECK_PROCESSING_INFO );
typedef void ( *DLL_API_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME )( Type_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME );
typedef void ( *DLL_API_MACRO_MAIN_PROCESS )( Type_MACRO_MAIN_PROCESS );
typedef void ( *DLL_API_MACRO_MAIN_PROCESS_PART_SIMPLE )( Type_MACRO_MAIN_PROCESS_PART_SIMPLE );
typedef void ( *DLL_API_MACRO_MAIN_PROCESS_PART_MULTIPLE )( Type_MACRO_MAIN_PROCESS_PART_MULTIPLE );
typedef void ( *DLL_API_MACRO_PROCESS_PART_TAG_SETTING )( Type_MACRO_PROCESS_PART_TAG_SETTING );
typedef void ( *DLL_API_MACRO_LOTPREPOST_PROCESS_OPERATION )( Type_MACRO_LOTPREPOST_PROCESS_OPERATION );
typedef void ( *DLL_API_MACRO_PRE_PROCESS_OPERATION )( Type_MACRO_PRE_PROCESS_OPERATION );
typedef void ( *DLL_API_MACRO_POST_PROCESS_OPERATION )( Type_MACRO_POST_PROCESS_OPERATION );
typedef void ( *DLL_API_MACRO_HAS_PRE_PROCESS_OPERATION )( Type_MACRO_HAS_PRE_PROCESS_OPERATION );
typedef void ( *DLL_API_MACRO_SPAWN_WAITING_BM_OPERATION )( Type_MACRO_SPAWN_WAITING_BM_OPERATION );
typedef void ( *DLL_API_MACRO_FM_ALIC_OPERATION )( Type_MACRO_FM_ALIC_OPERATION );
typedef void ( *DLL_API_MACRO_FM_POSSIBLE_PICK_FROM_CM )( Type_MACRO_FM_POSSIBLE_PICK_FROM_CM );
typedef void ( *DLL_API_MACRO_FM_OPERATION_THREAD_WAITING )( Type_MACRO_FM_OPERATION_THREAD_WAITING );
typedef void ( *DLL_API_MACRO_FM_OPERATION_THREAD_RESULT )( Type_MACRO_FM_OPERATION_THREAD_RESULT );
typedef void ( *DLL_API_MACRO_FM_OPERATION_MOVE_THREAD_STARTING )( Type_MACRO_FM_OPERATION_MOVE_THREAD_STARTING );
typedef void ( *DLL_API_MACRO_FM_OPERATION_MOVE_STARTING )( Type_MACRO_FM_OPERATION_MOVE_STARTING );
typedef void ( *DLL_API_MACRO_PLACE_TO_CM_POST_PART_with_FM )( Type_MACRO_PLACE_TO_CM_POST_PART_with_FM );
typedef void ( *DLL_API_MACRO_RUN_WAITING_BM_OPERATION )( Type_MACRO_RUN_WAITING_BM_OPERATION );
typedef void ( *DLL_API_MACRO_OTHER_LOT_LOAD_WAIT )( Type_MACRO_OTHER_LOT_LOAD_WAIT );
typedef void ( *DLL_API_MACRO_SPAWN_FM_MALIGNING )( Type_MACRO_SPAWN_FM_MALIGNING );
typedef void ( *DLL_API_MACRO_CHECK_FM_MALIGNING )( Type_MACRO_CHECK_FM_MALIGNING );
typedef void ( *DLL_API_MACRO_SPAWN_FM_MCOOLING )( Type_MACRO_SPAWN_FM_MCOOLING );
typedef void ( *DLL_API_MACRO_CHECK_FM_MCOOLING )( Type_MACRO_CHECK_FM_MCOOLING );
typedef void ( *DLL_API_MACRO_SPAWN_TM_MALIGNING )( Type_MACRO_SPAWN_TM_MALIGNING );
typedef void ( *DLL_API_MACRO_CHECK_TM_MALIGNING )( Type_MACRO_CHECK_TM_MALIGNING );
typedef void ( *DLL_API_MACRO_FM_MOVE_OPERATION )( Type_MACRO_FM_MOVE_OPERATION );

//

DLL_API_MACRO_FM_POSSIBLE_PICK_FROM_FM  DRV_API_MACRO_FM_POSSIBLE_PICK_FROM_FM[2];
DLL_API_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE  DRV_API_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE[2];
DLL_API_MACRO_FM_PICK_DATA_FROM_FM  DRV_API_MACRO_FM_PICK_DATA_FROM_FM[2];
DLL_API_MACRO_FM_PICK_DATA_FROM_FM_DETAIL  DRV_API_MACRO_FM_PICK_DATA_FROM_FM_DETAIL[2];
DLL_API_MACRO_FM_SUPPLY_FIRSTLAST_CHECK  DRV_API_MACRO_FM_SUPPLY_FIRSTLAST_CHECK[2];
DLL_API_MACRO_FM_FINISHED_CHECK  DRV_API_MACRO_FM_FINISHED_CHECK[2];
DLL_API_MACRO_INTERRUPT_PART_CHECK  DRV_API_MACRO_INTERRUPT_PART_CHECK[2];
DLL_API_MACRO_FM_OPERATION  DRV_API_MACRO_FM_OPERATION[2];
DLL_API_MACRO_FM_DATABASE_OPERATION  DRV_API_MACRO_FM_DATABASE_OPERATION[2];
DLL_API_MACRO_FM_PLACE_TO_CM  DRV_API_MACRO_FM_PLACE_TO_CM[2];
DLL_API_MACRO_TM_OPERATION_0  DRV_API_MACRO_TM_OPERATION_0[2];
DLL_API_MACRO_TM_OPERATION_1  DRV_API_MACRO_TM_OPERATION_1[2];
DLL_API_MACRO_TM_OPERATION_2  DRV_API_MACRO_TM_OPERATION_2[2];
DLL_API_MACRO_TM_OPERATION_3  DRV_API_MACRO_TM_OPERATION_3[2];
DLL_API_MACRO_TM_OPERATION_4  DRV_API_MACRO_TM_OPERATION_4[2];
DLL_API_MACRO_TM_OPERATION_5  DRV_API_MACRO_TM_OPERATION_5[2];
DLL_API_MACRO_TM_OPERATION_6  DRV_API_MACRO_TM_OPERATION_6[2];
DLL_API_MACRO_TM_OPERATION_7  DRV_API_MACRO_TM_OPERATION_7[2];
DLL_API_MACRO_TM_MOVE_OPERATION_0  DRV_API_MACRO_TM_MOVE_OPERATION_0[2];
DLL_API_MACRO_TM_MOVE_OPERATION_1  DRV_API_MACRO_TM_MOVE_OPERATION_1[2];
DLL_API_MACRO_TM_MOVE_OPERATION_2  DRV_API_MACRO_TM_MOVE_OPERATION_2[2];
DLL_API_MACRO_TM_MOVE_OPERATION_3  DRV_API_MACRO_TM_MOVE_OPERATION_3[2];
DLL_API_MACRO_TM_MOVE_OPERATION_4  DRV_API_MACRO_TM_MOVE_OPERATION_4[2];
DLL_API_MACRO_TM_MOVE_OPERATION_5  DRV_API_MACRO_TM_MOVE_OPERATION_5[2];
DLL_API_MACRO_TM_MOVE_OPERATION_6  DRV_API_MACRO_TM_MOVE_OPERATION_6[2];
DLL_API_MACRO_TM_MOVE_OPERATION_7  DRV_API_MACRO_TM_MOVE_OPERATION_7[2];
DLL_API_MACRO_TM_DATABASE_OPERATION  DRV_API_MACRO_TM_DATABASE_OPERATION[2];
DLL_API_MACRO_CHECK_PROCESSING  DRV_API_MACRO_CHECK_PROCESSING[2];
DLL_API_MACRO_CHECK_PROCESSING_INFO  DRV_API_MACRO_CHECK_PROCESSING_INFO[2];
DLL_API_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME  DRV_API_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME[2];
DLL_API_MACRO_MAIN_PROCESS  DRV_API_MACRO_MAIN_PROCESS[2];
DLL_API_MACRO_MAIN_PROCESS_PART_SIMPLE  DRV_API_MACRO_MAIN_PROCESS_PART_SIMPLE[2];
DLL_API_MACRO_MAIN_PROCESS_PART_MULTIPLE  DRV_API_MACRO_MAIN_PROCESS_PART_MULTIPLE[2];
DLL_API_MACRO_PROCESS_PART_TAG_SETTING  DRV_API_MACRO_PROCESS_PART_TAG_SETTING[2];
DLL_API_MACRO_LOTPREPOST_PROCESS_OPERATION  DRV_API_MACRO_LOTPREPOST_PROCESS_OPERATION[2];
DLL_API_MACRO_PRE_PROCESS_OPERATION  DRV_API_MACRO_PRE_PROCESS_OPERATION[2];
DLL_API_MACRO_POST_PROCESS_OPERATION  DRV_API_MACRO_POST_PROCESS_OPERATION[2];
DLL_API_MACRO_HAS_PRE_PROCESS_OPERATION  DRV_API_MACRO_HAS_PRE_PROCESS_OPERATION[2];
DLL_API_MACRO_SPAWN_WAITING_BM_OPERATION  DRV_API_MACRO_SPAWN_WAITING_BM_OPERATION[2];
DLL_API_MACRO_FM_ALIC_OPERATION  DRV_API_MACRO_FM_ALIC_OPERATION[2];
DLL_API_MACRO_FM_POSSIBLE_PICK_FROM_CM  DRV_API_MACRO_FM_POSSIBLE_PICK_FROM_CM[2];
DLL_API_MACRO_FM_OPERATION_THREAD_WAITING  DRV_API_MACRO_FM_OPERATION_THREAD_WAITING[2];
DLL_API_MACRO_FM_OPERATION_THREAD_RESULT  DRV_API_MACRO_FM_OPERATION_THREAD_RESULT[2];
DLL_API_MACRO_FM_OPERATION_MOVE_THREAD_STARTING  DRV_API_MACRO_FM_OPERATION_MOVE_THREAD_STARTING[2];
DLL_API_MACRO_FM_OPERATION_MOVE_STARTING  DRV_API_MACRO_FM_OPERATION_MOVE_STARTING[2];
DLL_API_MACRO_PLACE_TO_CM_POST_PART_with_FM  DRV_API_MACRO_PLACE_TO_CM_POST_PART_with_FM[2];
DLL_API_MACRO_RUN_WAITING_BM_OPERATION  DRV_API_MACRO_RUN_WAITING_BM_OPERATION[2];
DLL_API_MACRO_OTHER_LOT_LOAD_WAIT  DRV_API_MACRO_OTHER_LOT_LOAD_WAIT[2];
DLL_API_MACRO_SPAWN_FM_MALIGNING  DRV_API_MACRO_SPAWN_FM_MALIGNING[2];
DLL_API_MACRO_CHECK_FM_MALIGNING  DRV_API_MACRO_CHECK_FM_MALIGNING[2];
DLL_API_MACRO_SPAWN_FM_MCOOLING  DRV_API_MACRO_SPAWN_FM_MCOOLING[2];
DLL_API_MACRO_CHECK_FM_MCOOLING  DRV_API_MACRO_CHECK_FM_MCOOLING[2];
DLL_API_MACRO_SPAWN_TM_MALIGNING  DRV_API_MACRO_SPAWN_TM_MALIGNING[2];
DLL_API_MACRO_CHECK_TM_MALIGNING  DRV_API_MACRO_CHECK_TM_MALIGNING[2];
DLL_API_MACRO_FM_MOVE_OPERATION  DRV_API_MACRO_FM_MOVE_OPERATION[2];

#define MAKE_MACRO_DLL_DEFINE( X , Name , APIName ) \
	sprintf( buffer , "_SCH_INF_DLL_API_%s" , ##APIName## ); \
	DRV_API_MACRO_##Name##[##X##] = (DLL_API_MACRO_##Name##) GetProcAddress( DRV_DLL_SCH_LOAD_POINT[##X##] , buffer ); \
	if ( DRV_API_MACRO_##Name##[##X##] ) (* DRV_API_MACRO_##Name##[##X##] ) ( _SCH_MACRO_##Name## );

//======================
// Macro End : Type
//======================




int DLL_USER_SCH_USE = 0;

HINSTANCE DRV_DLL_SCH_LOAD_POINT[2] = { NULL , NULL };

char *DRV_DLL_SCH_FILENAME[2] = { NULL , NULL }; // 2008.04.02

//--------------------------------------------------------------------------------------------------------
DLL_KNFS_SET_COMMON_PRINTF_API					DRV_SCHK_SET_COMMON_PRINTF_API[2];
DLL_KNFS_SET_FIND_FROM_STRING_API				DRV_SCHK_SET_FIND_FROM_STRING_API[2];
DLL_KNFS_SET_dWRITE_FUNCTION_API				DRV_SCHK_SET_dWRITE_FUNCTION_API[2];
DLL_KNFS_SET_dREAD_FUNCTION_API					DRV_SCHK_SET_dREAD_FUNCTION_API[2];
DLL_KNFS_SET_dRUN_FUNCTION_ABORT_API			DRV_SCHK_SET_dRUN_FUNCTION_ABORT_API[2];
DLL_KNFS_SET_dWRITE_FUNCTION_EVENT_API			DRV_SCHK_SET_dWRITE_FUNCTION_EVENT_API[2];
DLL_KNFS_SET_dREAD_FUNCTION_EVENT_API			DRV_SCHK_SET_dREAD_FUNCTION_EVENT_API[2];
//--------------------------------------------------------------------------------------------------------
DLL_KNFS_SET_dREAD_UPLOAD_MESSAGE_API			DRV_SCHK_SET_dREAD_UPLOAD_MESSAGE_API[2];
DLL_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API			DRV_SCHK_SET_dWRITE_UPLOAD_MESSAGE_API[2];
		
DLL_KNFS_SET_dWRITE_DIGITAL_API					DRV_SCHK_SET_dWRITE_DIGITAL_API[2];
DLL_KNFS_SET_dREAD_DIGITAL_API					DRV_SCHK_SET_dREAD_DIGITAL_API[2];
DLL_KNFS_SET_dWRITE_ANALOG_API					DRV_SCHK_SET_dWRITE_ANALOG_API[2];
DLL_KNFS_SET_dREAD_ANALOGL_API					DRV_SCHK_SET_dREAD_ANALOG_API[2];
DLL_KNFS_SET_dWRITE_STRING_API					DRV_SCHK_SET_dWRITE_STRING_API[2];
DLL_KNFS_SET_dREAD_STRING_API					DRV_SCHK_SET_dREAD_STRING_API[2];
//--------------------------------------------------------------------------------------------------------
DLL_KNFS_SET_CIM_PRINTF_API						DRV_KNFS_SET_CIM_PRINTF_API[2];
DLL_KNFS_SET_COMMON_PRINTF_API					DRV_KNFS_SET_COMMON_PRINTF_API[2];
DLL_KNFS_SET_CONSOLE_PRINTF_API					DRV_KNFS_SET_CONSOLE_PRINTF_API[2];
//
DLL_KNFS_SET_MANAGER_ABORT_API					DRV_KNFS_SET_MANAGER_ABORT_API[2];
DLL_KNFS_SET_SYSTEM_ALL_ABORT_API				DRV_KNFS_SET_SYSTEM_ALL_ABORT_API[2];
DLL_KNFS_SET_dRUN_SET_FUNCTION_API				DRV_KNFS_SET_dRUN_SET_FUNCTION_API[2];
DLL_KNFS_SET_dRUN_FUNCTION_API					DRV_KNFS_SET_dRUN_FUNCTION_API[2];
DLL_KNFS_SET_dRUN_SET_FUNCTION_FREE_API			DRV_KNFS_SET_dRUN_SET_FUNCTION_FREE_API[2];
DLL_KNFS_SET_dRUN_FUNCTION_FREE_API				DRV_KNFS_SET_dRUN_FUNCTION_FREE_API[2];
		
//--------------------------------------------------------------------------------------------------------
// 2008.01.18
//--------------------------------------------------------------------------------------------------------
DLL_SCH_INF_LOAD_INIT							DRV_SCH_INF_LOAD_INIT[2];

DLL_SCH_INF_LOAD_SIZE							DRV_SCH_INF_LOAD_SIZE[2]; // 2018.08.22

DLL_SCH_INF_REVISION_HISTORY					DRV_SCH_INF_REVISION_HISTORY[2];
DLL_SCH_INF_CONFIG_INITIALIZE					DRV_SCH_INF_CONFIG_INITIALIZE[2];
DLL_SCH_INF_RUN_INIT							DRV_SCH_INF_RUN_INIT[2];
DLL_SCH_INF_SET_DLL_INSTANCE					DRV_SCH_INF_SET_DLL_INSTANCE[2];
DLL_SCH_INF_RUN_PRE_BM							DRV_SCH_INF_RUN_PRE_BM[2];
DLL_SCH_INF_RUN_MFIC_COMPLETE_MACHINE			DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[2];
DLL_SCH_INF_GUI_INTERFACE						DRV_SCH_INF_GUI_INTERFACE[2];

DLL_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING	DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[2];
DLL_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG	DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[2];
DLL_SCH_INF_RUN_PRE_OPERATION					DRV_SCH_INF_RUN_PRE_OPERATION[2];

DLL_SCH_INF_EVENT_ANALYSIS						DRV_SCH_INF_EVENT_ANALYSIS[2];
DLL_SCH_INF_EVENT_PRE_ANALYSIS					DRV_SCH_INF_EVENT_PRE_ANALYSIS[2];
DLL_SCH_INF_SYSTEM_LOG							DRV_SCH_INF_SYSTEM_LOG[2];
DLL_SCH_INF_SYSTEM_FAIL							DRV_SCH_INF_SYSTEM_FAIL[2]; // 2017.07.26
DLL_SCH_INF_TRANSFER_OPTION						DRV_SCH_INF_TRANSFER_OPTION[2];

DLL_SCH_INF_FINISHED_CHECK						DRV_SCH_INF_FINISHED_CHECK[2];
DLL_SCH_INF_CASSETTE_CHECK_INVALID_INFO			DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[2];
DLL_SCH_INF_GET_PROCESS_FILENAME				DRV_SCH_INF_GET_PROCESS_FILENAME[2];
DLL_SCH_INF_GET_PROCESS_FILENAME2				DRV_SCH_INF_GET_PROCESS_FILENAME2[2];
DLL_SCH_INF_GET_PROCESS_INDEX_DATA				DRV_SCH_INF_GET_PROCESS_INDEX_DATA[2];
DLL_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER		DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[2];
DLL_SCH_INF_SIDE_SUPPLY							DRV_SCH_INF_SIDE_SUPPLY[2];
DLL_SCH_INF_FM_ARM_APLUSB_SWAPPING				DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[2];
DLL_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL		DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[2];
DLL_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL		DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[2];
DLL_SCH_INF_PM_2MODULE_SAME_BODY				DRV_SCH_INF_PM_2MODULE_SAME_BODY[2];
DLL_SCH_INF_BM_2MODULE_SAME_BODY				DRV_SCH_INF_BM_2MODULE_SAME_BODY[2];
DLL_SCH_INF_BM_2MODULE_END_APPEND_STRING		DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[2];
DLL_SCH_INF_GET_METHOD_ORDER					DRV_SCH_INF_GET_METHOD_ORDER[2];
DLL_SCH_INF_CHECK_PICK_FROM_FM					DRV_SCH_INF_CHECK_PICK_FROM_FM[2];
DLL_SCH_INF_GET_PT_AT_PICK_FROM_CM				DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[2]; // 2014.06.16
DLL_SCH_INF_HANDOFF_ACTION						DRV_SCH_INF_HANDOFF_ACTION[2]; // 2014.06.27
DLL_SCH_INF_CHECK_END_PART						DRV_SCH_INF_CHECK_END_PART[2];
DLL_SCH_INF_FAIL_SCENARIO_OPERATION				DRV_SCH_INF_FAIL_SCENARIO_OPERATION[2];
DLL_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED			DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[2];
DLL_SCH_INF_CHECK_WAITING_MODE					DRV_SCH_INF_CHECK_WAITING_MODE[2];
DLL_SCH_INF_TUNING_GET_MORE_APPEND_DATA			DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[2];
DLL_SCH_INF_METHOD_CHECK_SKIP_WHEN_START		DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[2];
DLL_SCH_INF_ANIMATION_UPDATE_TM					DRV_SCH_INF_ANIMATION_UPDATE_TM[2];
DLL_SCH_INF_USER_DATABASE_REMAPPING				DRV_SCH_INF_USER_DATABASE_REMAPPING[2]; // 2010.11.09
DLL_SCH_INF_ANIMATION_SOURCE_UPDATE				DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[2]; // 2010.11.23
DLL_SCH_INF_ANIMATION_RESULT_UPDATE				DRV_SCH_INF_ANIMATION_RESULT_UPDATE[2]; // 2012.03.15
DLL_SCH_INF_MTLOUT_DATABASE_MOVE				DRV_SCH_INF_MTLOUT_DATABASE_MOVE[2]; // 2011.05.13
DLL_SCH_INF_USE_EXTEND_OPTION					DRV_SCH_INF_USE_EXTEND_OPTION[2]; // 2011.05.13
DLL_SCH_INF_USER_RECIPE_CHECK					DRV_SCH_INF_USER_RECIPE_CHECK[2]; // 2012.01.12
DLL_SCH_INF_USER_FLOW_NOTIFICATION				DRV_SCH_INF_USER_FLOW_NOTIFICATION[2]; // 2015.05.29
DLL_SCH_INF_GET_UPLOAD_MESSAGE					DRV_SCH_INF_GET_UPLOAD_MESSAGE[2]; // 2017.01.03
DLL_SCH_INF_GET_DUMMY_METHOD_INDEX				DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[2]; // 2017.09.11
DLL_SCH_INF_FA_SEND_MESSAGE						DRV_SCH_INF_FA_SEND_MESSAGE[2]; // 2018.06.14
DLL_SCH_INF_FA_ORDER_MESSAGE					DRV_SCH_INF_FA_ORDER_MESSAGE[2]; // 2018.06.29

DLL_SCH_INF_ENTER_CONTROL						DRV_SCH_INF_ENTER_CONTROL_FEM[2];
DLL_SCH_INF_ENTER_CONTROL						DRV_SCH_INF_ENTER_CONTROL_TM[2];

DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_1[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_2[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_3[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_4[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_5[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_6[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_7[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_TM_8[2];
DLL_SCH_INF_RUN_CONTROL							DRV_SCH_INF_RUN_CONTROL_FEM[2];

DLL_SCH_INF_USER_CONTROL						DRV_SCH_INF_USER_CONTROL_FEM[2] = { FALSE , FALSE }; // 2011.12.08
DLL_SCH_INF_USER_CONTROL						DRV_SCH_INF_USER_CONTROL_TM[2] = { FALSE , FALSE }; // 2011.12.08

DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_FEM[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_1[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_2[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_3[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_4[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_5[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_6[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_7[2] = { FALSE , FALSE }; // 2012.09.17
DLL_SCH_INF_RESTART_PRE_CONTROL					DRV_SCH_INF_RESTART_PRE_CONTROL_TM_8[2] = { FALSE , FALSE }; // 2012.09.17

DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API	DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API[2];
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API	DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API[2];
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API	DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API[2];
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API	DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API[2];
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API	DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API[2];

DLL_SCH_INF_LOG_CONTROL_LOT_PRINTF_API			DRV_SCH_INF_LOG_CONTROL_LOT_PRINTF_API[2];
DLL_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API		DRV_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API[2];
DLL_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API		DRV_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API[2];
//
//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================
//
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A		_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API;
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B		_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API;
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C		_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API;
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D		_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API;
DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E		_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API;

TypeREM_SCH_INF_DLL_CONTROL_LOT_PRINTF_API		_SCH_INF_LOG_CONTROL_LOT_PRINTF_API;
TypeREM_SCH_INF_DLL_CONTROL_DEBUG_PRINTF_API	_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API;
TypeREM_SCH_INF_DLL_CONTROL_TIMER_PRINTF_API	_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API;

TypeREM_SCH_INF_DLL_CIM_IO_CIM_PRINTF_API		_SCH_INF_CIM_CONTROL_IO_CIM_PRINTF_API;
TypeREM_SCH_INF_DLL_CIM_IO_COMMON_PRINTF_API	_SCH_INF_CIM_CONTROL_IO_COMMON_PRINTF_API;
TypeREM_SCH_INF_DLL_CIM_IO_CONSOLE_PRINTF_API	_SCH_INF_CIM_CONTROL_IO_CONSOLE_PRINTF_API;

//=========================================================================================

DLL_SCH_RECIPE_INFO_VERIFICATION_EIL	DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[2]; // 2010.09.16

//=========================================================================================
void INTERFACE_API_INSIDE_SET(
							  DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A api_A ,
							  DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B api_B ,
							  DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C api_C ,
							  DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D api_D ,
							  DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E api_E ,
							  TypeREM_SCH_INF_DLL_CONTROL_LOT_PRINTF_API api_X1 ,
							  TypeREM_SCH_INF_DLL_CONTROL_DEBUG_PRINTF_API api_X2 ,
							  TypeREM_SCH_INF_DLL_CONTROL_TIMER_PRINTF_API api_X3 ,
							  TypeREM_SCH_INF_DLL_CIM_IO_CIM_PRINTF_API api_C1 ,
							  TypeREM_SCH_INF_DLL_CIM_IO_COMMON_PRINTF_API api_C2 ,
							  TypeREM_SCH_INF_DLL_CIM_IO_CONSOLE_PRINTF_API api_C3
							  ) {
	_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API = api_A;
	_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API = api_B;
	_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API = api_C;
	_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API = api_D;
	_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API = api_E;
	//
	_SCH_INF_LOG_CONTROL_LOT_PRINTF_API   = api_X1;
	_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API = api_X2;
	_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API = api_X3;
	//
	_SCH_INF_CIM_CONTROL_IO_CIM_PRINTF_API	  = api_C1;
	_SCH_INF_CIM_CONTROL_IO_COMMON_PRINTF_API = api_C2;
	_SCH_INF_CIM_CONTROL_IO_CONSOLE_PRINTF_API = api_C3;
}
//=========================================================================================
int USER_SCH_USE_TAG_GET() {
	return DLL_USER_SCH_USE;
	// -1	:	Not Use
	// 0	:	Not Loaded
	// 1	:	Loaded Enable
	// 2	:	Loaded Disable
}
//=========================================================================================
int USER_dREAD_FUNCTION( int i ) {
	return _dREAD_FUNCTION( i );
}

int USER_dRUN_FUNCTION( int i , char *data ) {
	return _dRUN_FUNCTION( i , data );
}

int USER_dRUN_FUNCTION_FREE( int i , char *data ) {
	return _dRUN_FUNCTION_FREE( i , data );
}

BOOL USER_SCH_DLL_LOAD( char *Filename , char *Filename2 , int mode ) {
	int x;
	char buffer[256];
	//
	// mode 
	// 0	:	Init
	// 1	:	Disable
	// 2	:	Enable
	// 3	:	Unload
	// 4	:	Load
	// 5	:	Load Disable
	//================================================================================================
	// 2008.01.18
	//================================================================================================
	if ( ( mode == 1 ) || ( mode == 2 ) ) {
		//
		if ( !DRV_DLL_SCH_LOAD_POINT[0] && !DRV_DLL_SCH_LOAD_POINT[1] ) return FALSE;
		//
		if ( DLL_USER_SCH_USE <= 0 ) return FALSE;
		//
		if ( mode == 1 ) DLL_USER_SCH_USE = 2;
		else             DLL_USER_SCH_USE = 1;
		//
		return TRUE;
	}
	else if ( mode == 3 ) {
		//
		if ( !DRV_DLL_SCH_LOAD_POINT[0] && !DRV_DLL_SCH_LOAD_POINT[1] ) return TRUE;
		//
		if ( DRV_SCH_INF_LOAD_INIT[0] ) (* DRV_SCH_INF_LOAD_INIT[0] ) ( -1 ); // 2012.04.10
		if ( DRV_SCH_INF_LOAD_INIT[1] ) (* DRV_SCH_INF_LOAD_INIT[1] ) ( -1 ); // 2016.12.14
		//
		DLL_USER_SCH_USE = 0;
		//
		if ( DRV_DLL_SCH_LOAD_POINT[0] ) {
			FreeLibrary( DRV_DLL_SCH_LOAD_POINT[0] );
			DRV_DLL_SCH_LOAD_POINT[0] = NULL;
		}
		//
		if ( DRV_DLL_SCH_LOAD_POINT[1] ) {
			FreeLibrary( DRV_DLL_SCH_LOAD_POINT[1] );
			DRV_DLL_SCH_LOAD_POINT[1] = NULL;
		}
		//
		return TRUE;
	}
	else if ( ( mode == 4 ) || ( mode == 5 ) ) {
		//
		if ( DRV_DLL_SCH_LOAD_POINT[0] || DRV_DLL_SCH_LOAD_POINT[1] ) return FALSE;
		//
		if ( ( strlen( Filename ) > 0 ) || ( strlen( Filename2 ) > 0 ) ) {
			STR_MEM_MAKE_COPY2( &(DRV_DLL_SCH_FILENAME[0]) , Filename ); // 2008.04.02 // 2010.03.25
			STR_MEM_MAKE_COPY2( &(DRV_DLL_SCH_FILENAME[1]) , Filename2 ); // 2008.04.02 // 2010.03.25
		}
		//
		if ( ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME[0] ) > 0 ) || ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME[1] ) > 0 ) ) { // 2008.04.03
			DLL_USER_SCH_USE = 0;
		}
		else {
			DLL_USER_SCH_USE = -1;
		}
		//
	}
	else if ( ( mode == 0 ) || ( mode == 99 ) ) {
		STR_MEM_MAKE_COPY2( &(DRV_DLL_SCH_FILENAME[0]) , Filename ); // 2008.04.02
		STR_MEM_MAKE_COPY2( &(DRV_DLL_SCH_FILENAME[1]) , Filename2 ); // 2008.04.02
	}
	//================================================================================================
	INTERFACE_API_INSIDE_SET(
							  _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A ,
							  _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B ,
							  _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C ,
							  _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D ,
							  _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E ,
							  _i_SCH_LOG_LOT_PRINTF ,
							  _i_SCH_LOG_DEBUG_PRINTF ,
							  _i_SCH_LOG_TIMER_PRINTF ,
							  _IO_CIM_PRINTF ,
							  _IO_COMMON_PRINTF ,
							  _IO_CONSOLE_PRINTF
							  );
	//================================================================================================
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		if ( DRV_DLL_SCH_FILENAME[x] == NULL ) {
			DRV_DLL_SCH_LOAD_POINT[x] = FALSE;
		}
		else {
			DRV_DLL_SCH_LOAD_POINT[x] = LoadLibrary( DRV_DLL_SCH_FILENAME[x] );
		}
		//================================================================================================
		if ( DRV_DLL_SCH_LOAD_POINT[x] ) {
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
//			if ( mode == 5 ) DLL_USER_SCH_USE = 2;
//			else             DLL_USER_SCH_USE = 1;
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			DRV_SCHK_SET_COMMON_PRINTF_API[x]			= (DLL_KNFS_SET_COMMON_PRINTF_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_IO_COMMON_PRINTF" );
			DRV_SCHK_SET_FIND_FROM_STRING_API[x]		= (DLL_KNFS_SET_FIND_FROM_STRING_API)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_FIND_FROM_STRING" );
			DRV_SCHK_SET_dWRITE_FUNCTION_API[x]			= (DLL_KNFS_SET_dWRITE_FUNCTION_API)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_FUNCTION" );
			DRV_SCHK_SET_dREAD_FUNCTION_API[x]			= (DLL_KNFS_SET_dREAD_FUNCTION_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_FUNCTION" );
			DRV_SCHK_SET_dRUN_FUNCTION_ABORT_API[x]		= (DLL_KNFS_SET_dRUN_FUNCTION_ABORT_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dRUN_FUNCTION_ABORT" );
			DRV_SCHK_SET_dWRITE_FUNCTION_EVENT_API[x]	= (DLL_KNFS_SET_dWRITE_FUNCTION_EVENT_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_FUNCTION_EVENT" );
			DRV_SCHK_SET_dREAD_FUNCTION_EVENT_API[x]	= (DLL_KNFS_SET_dREAD_FUNCTION_EVENT_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_FUNCTION_EVENT" );
			//
			if ( DRV_SCHK_SET_COMMON_PRINTF_API[x] ) (* DRV_SCHK_SET_COMMON_PRINTF_API[x] ) ( _IO_COMMON_PRINTF );
			//
			if ( DRV_SCHK_SET_FIND_FROM_STRING_API[x] ) (* DRV_SCHK_SET_FIND_FROM_STRING_API[x] ) ( _FIND_FROM_STRING );
			if ( DRV_SCHK_SET_dWRITE_FUNCTION_API[x] ) (* DRV_SCHK_SET_dWRITE_FUNCTION_API[x] ) ( _dWRITE_FUNCTION );
			if ( DRV_SCHK_SET_dREAD_FUNCTION_API[x] ) (* DRV_SCHK_SET_dREAD_FUNCTION_API[x] ) ( USER_dREAD_FUNCTION ); // 2013.03.18
			if ( DRV_SCHK_SET_dRUN_FUNCTION_ABORT_API[x] ) (* DRV_SCHK_SET_dRUN_FUNCTION_ABORT_API[x] ) ( _dRUN_FUNCTION_ABORT );
			if ( DRV_SCHK_SET_dWRITE_FUNCTION_EVENT_API[x] ) (* DRV_SCHK_SET_dWRITE_FUNCTION_EVENT_API[x] ) ( _dWRITE_FUNCTION_EVENT );
			if ( DRV_SCHK_SET_dREAD_FUNCTION_EVENT_API[x] ) (* DRV_SCHK_SET_dREAD_FUNCTION_EVENT_API[x] ) ( _dREAD_FUNCTION_EVENT );
			//---------------------------------------------------------------------------------------------------------
			DRV_SCHK_SET_dREAD_UPLOAD_MESSAGE_API[x]	= (DLL_KNFS_SET_dREAD_UPLOAD_MESSAGE_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_UPLOAD_MESSAGE" );
			DRV_SCHK_SET_dWRITE_UPLOAD_MESSAGE_API[x]	= (DLL_KNFS_SET_dWRITE_UPLOAD_MESSAGE_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_UPLOAD_MESSAGE" );

			DRV_SCHK_SET_dWRITE_DIGITAL_API[x]			= (DLL_KNFS_SET_dWRITE_DIGITAL_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_DIGITAL" );
			DRV_SCHK_SET_dREAD_DIGITAL_API[x]			= (DLL_KNFS_SET_dREAD_DIGITAL_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_DIGITAL" );
			DRV_SCHK_SET_dWRITE_ANALOG_API[x]			= (DLL_KNFS_SET_dWRITE_ANALOG_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_ANALOG" );
			DRV_SCHK_SET_dREAD_ANALOG_API[x]			= (DLL_KNFS_SET_dREAD_ANALOGL_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_ANALOG" );
			DRV_SCHK_SET_dWRITE_STRING_API[x]			= (DLL_KNFS_SET_dWRITE_STRING_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dWRITE_STRING" );
			DRV_SCHK_SET_dREAD_STRING_API[x]			= (DLL_KNFS_SET_dREAD_STRING_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dREAD_STRING" );
			//
			if ( DRV_SCHK_SET_dREAD_UPLOAD_MESSAGE_API[x] )		(* DRV_SCHK_SET_dREAD_UPLOAD_MESSAGE_API[x] ) ( _dREAD_UPLOAD_MESSAGE );
			if ( DRV_SCHK_SET_dWRITE_UPLOAD_MESSAGE_API[x] )	(* DRV_SCHK_SET_dWRITE_UPLOAD_MESSAGE_API[x] ) ( _dWRITE_UPLOAD_MESSAGE );
			//
			if ( DRV_SCHK_SET_dWRITE_DIGITAL_API[x] )	(* DRV_SCHK_SET_dWRITE_DIGITAL_API[x] ) ( _dWRITE_DIGITAL );
			if ( DRV_SCHK_SET_dREAD_DIGITAL_API[x] )	(* DRV_SCHK_SET_dREAD_DIGITAL_API[x] ) ( _dREAD_DIGITAL );
			if ( DRV_SCHK_SET_dWRITE_ANALOG_API[x] )	(* DRV_SCHK_SET_dWRITE_ANALOG_API[x] ) ( _dWRITE_ANALOG );
			if ( DRV_SCHK_SET_dREAD_ANALOG_API[x] )	(* DRV_SCHK_SET_dREAD_ANALOG_API[x] ) ( _dREAD_ANALOG );
			if ( DRV_SCHK_SET_dWRITE_STRING_API[x] )	(* DRV_SCHK_SET_dWRITE_STRING_API[x] ) ( _dWRITE_STRING );
			if ( DRV_SCHK_SET_dREAD_STRING_API[x] )	(* DRV_SCHK_SET_dREAD_STRING_API[x] ) ( _dREAD_STRING );
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			DRV_KNFS_SET_CIM_PRINTF_API[x]				= (DLL_KNFS_SET_CIM_PRINTF_API)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_IO_CIM_PRINTF" );
			DRV_KNFS_SET_COMMON_PRINTF_API[x]			= (DLL_KNFS_SET_COMMON_PRINTF_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_IO_COMMON_PRINTF" );
			DRV_KNFS_SET_CONSOLE_PRINTF_API[x]			= (DLL_KNFS_SET_CONSOLE_PRINTF_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_IO_CONSOLE_PRINTF" );
			//
			DRV_KNFS_SET_MANAGER_ABORT_API[x]			= (DLL_KNFS_SET_MANAGER_ABORT_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_MANAGER_ABORT" );
			DRV_KNFS_SET_SYSTEM_ALL_ABORT_API[x]		= (DLL_KNFS_SET_SYSTEM_ALL_ABORT_API)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_SYSTEM_ALL_ABORT" );
			DRV_KNFS_SET_dRUN_SET_FUNCTION_API[x]		= (DLL_KNFS_SET_dRUN_SET_FUNCTION_API)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dRUN_SET_FUNCTION" );
			DRV_KNFS_SET_dRUN_FUNCTION_API[x]			= (DLL_KNFS_SET_dRUN_FUNCTION_API)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dRUN_FUNCTION" );
			DRV_KNFS_SET_dRUN_SET_FUNCTION_FREE_API[x]	= (DLL_KNFS_SET_dRUN_SET_FUNCTION_FREE_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dRUN_SET_FUNCTION_FREE" );
			DRV_KNFS_SET_dRUN_FUNCTION_FREE_API[x]		= (DLL_KNFS_SET_dRUN_FUNCTION_FREE_API)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_dRUN_FUNCTION_FREE" );

			if ( DRV_KNFS_SET_CIM_PRINTF_API[x] )			(* DRV_KNFS_SET_CIM_PRINTF_API[x] ) ( _IO_CIM_PRINTF );
			if ( DRV_KNFS_SET_COMMON_PRINTF_API[x] )		(* DRV_KNFS_SET_COMMON_PRINTF_API[x] ) ( _IO_COMMON_PRINTF );
			if ( DRV_KNFS_SET_CONSOLE_PRINTF_API[x] )		(* DRV_KNFS_SET_CONSOLE_PRINTF_API[x] ) ( _IO_CONSOLE_PRINTF );
			//
			if ( DRV_KNFS_SET_MANAGER_ABORT_API[x] )		(* DRV_KNFS_SET_MANAGER_ABORT_API[x] ) ( MANAGER_ABORT );
			if ( DRV_KNFS_SET_SYSTEM_ALL_ABORT_API[x] )		(* DRV_KNFS_SET_SYSTEM_ALL_ABORT_API[x] ) ( SYSTEM_ALL_ABORT );
			if ( DRV_KNFS_SET_dRUN_SET_FUNCTION_API[x] )	(* DRV_KNFS_SET_dRUN_SET_FUNCTION_API[x] ) ( _dRUN_SET_FUNCTION );
			if ( DRV_KNFS_SET_dRUN_FUNCTION_API[x] )		(* DRV_KNFS_SET_dRUN_FUNCTION_API[x] ) ( USER_dRUN_FUNCTION ); // 2013.03.18
			if ( DRV_KNFS_SET_dRUN_SET_FUNCTION_FREE_API[x] )(* DRV_KNFS_SET_dRUN_SET_FUNCTION_FREE_API[x] ) ( _dRUN_SET_FUNCTION_FREE );
			if ( DRV_KNFS_SET_dRUN_FUNCTION_FREE_API[x] )	(* DRV_KNFS_SET_dRUN_FUNCTION_FREE_API[x] ) ( USER_dRUN_FUNCTION_FREE ); // 2013.03.18
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API[x]		= (DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_A" );
			DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API[x]		= (DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_B" );
			DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API[x]		= (DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_C" );
			DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API[x]		= (DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_D" );
			DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API[x]		= (DLL_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_E" );

			if ( DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API[x] ) (* DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API[x] ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_A );
			if ( DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API[x] ) (* DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API[x] ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_B );
			if ( DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API[x] ) (* DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API[x] ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_C );
			if ( DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API[x] ) (* DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API[x] ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_D );
			if ( DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API[x] ) (* DRV_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API[x] ) ( _SCH_INF_iDLL_CONTROL_EXTERN_INTERFACE_E );
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			DRV_SCH_INF_LOG_CONTROL_LOT_PRINTF_API[x]		= (DLL_SCH_INF_LOG_CONTROL_LOT_PRINTF_API)	    GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_LOG_CONTROL_LOT_PRINTF" );
			DRV_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API[x]		= (DLL_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_LOG_CONTROL_DEBUG_PRINTF" );
			DRV_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API[x]		= (DLL_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_LOG_CONTROL_TIMER_PRINTF" );

			if ( DRV_SCH_INF_LOG_CONTROL_LOT_PRINTF_API[x] )	(* DRV_SCH_INF_LOG_CONTROL_LOT_PRINTF_API[x] )   ( _i_SCH_LOG_LOT_PRINTF );
			if ( DRV_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API[x] )	(* DRV_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API[x] ) ( _i_SCH_LOG_DEBUG_PRINTF );
			if ( DRV_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API[x] )	(* DRV_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API[x] ) ( _i_SCH_LOG_TIMER_PRINTF );
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			DRV_SCH_INF_LOAD_INIT[x]							= (DLL_SCH_INF_LOAD_INIT)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_LOAD_INIT" );
			DRV_SCH_INF_CONFIG_INITIALIZE[x]					= (DLL_SCH_INF_CONFIG_INITIALIZE)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_CONFIG_INITIALIZE" );
			DRV_SCH_INF_SET_DLL_INSTANCE[x]						= (DLL_SCH_INF_SET_DLL_INSTANCE)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_RECIPE_DLL_INTERFACE" );
			DRV_SCH_INF_RUN_INIT[x]								= (DLL_SCH_INF_RUN_INIT)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_INITIALIZE_PART" );
			DRV_SCH_INF_REVISION_HISTORY[x]						= (DLL_SCH_INF_REVISION_HISTORY)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_REVISION_HISTORY" );
			DRV_SCH_INF_GUI_INTERFACE[x]						= (DLL_SCH_INF_GUI_INTERFACE)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GUI_INTERFACE" );
			DRV_SCH_INF_EVENT_ANALYSIS[x]						= (DLL_SCH_INF_EVENT_ANALYSIS)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_EVENT_ANALYSIS" );
			DRV_SCH_INF_EVENT_PRE_ANALYSIS[x]					= (DLL_SCH_INF_EVENT_PRE_ANALYSIS)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_EVENT_PRE_ANALYSIS" );
			DRV_SCH_INF_SYSTEM_LOG[x]							= (DLL_SCH_INF_SYSTEM_LOG)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_SYSTEM_DATA_LOG" );
			DRV_SCH_INF_SYSTEM_FAIL[x]							= (DLL_SCH_INF_SYSTEM_FAIL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_SYSTEM_CONTROL_FAIL" );
			DRV_SCH_INF_TRANSFER_OPTION[x]						= (DLL_SCH_INF_TRANSFER_OPTION)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_TRANSFER_OPTION" );
			DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[x]	= (DLL_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING" );
			DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[x]	= (DLL_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_READING_CONFIG_FILE_WHEN_CHANGE_ALG" );
			DRV_SCH_INF_RUN_PRE_OPERATION[x]					= (DLL_SCH_INF_RUN_PRE_OPERATION)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_PRE_PART_OPERATION" );
			DRV_SCH_INF_RUN_PRE_BM[x]							= (DLL_SCH_INF_RUN_PRE_BM)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_RUN_PRE_BM_CONDITION" );
			DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[x]			= (DLL_SCH_INF_RUN_MFIC_COMPLETE_MACHINE)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_RUN_MFIC_COMPLETE_MACHINE" );
																																					  
			DRV_SCH_INF_FINISHED_CHECK[x]						= (DLL_SCH_INF_FINISHED_CHECK)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_FINISHED_CHECK" );
			DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[x]			= (DLL_SCH_INF_CASSETTE_CHECK_INVALID_INFO)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_CASSETTE_CHECK_INVALID_INFO" );
			DRV_SCH_INF_GET_PROCESS_FILENAME[x]					= (DLL_SCH_INF_GET_PROCESS_FILENAME)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_PROCESS_FILENAME" );
			DRV_SCH_INF_GET_PROCESS_FILENAME2[x]				= (DLL_SCH_INF_GET_PROCESS_FILENAME2)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_PROCESS_FILENAME2" );
			DRV_SCH_INF_GET_PROCESS_INDEX_DATA[x]				= (DLL_SCH_INF_GET_PROCESS_INDEX_DATA)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_PROCESS_INDEX_DATA" );
			DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[x]			= (DLL_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_MESSAGE_CONTROL_FOR_USER" );

			DRV_SCH_INF_SIDE_SUPPLY[x]							= (DLL_SCH_INF_SIDE_SUPPLY)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_SIDE_SUPPLY" );
			DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[x]				= (DLL_SCH_INF_FM_ARM_APLUSB_SWAPPING)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_FM_ARM_APLUSB_SWAPPING" );
			DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[x]			= (DLL_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL" );
			DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[x]		= (DLL_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL" );
			DRV_SCH_INF_PM_2MODULE_SAME_BODY[x]					= (DLL_SCH_INF_PM_2MODULE_SAME_BODY)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_PM_2MODULE_SAME_BODY" );
			DRV_SCH_INF_BM_2MODULE_SAME_BODY[x]					= (DLL_SCH_INF_BM_2MODULE_SAME_BODY)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_BM_2MODULE_SAME_BODY" );
			DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[x]			= (DLL_SCH_INF_BM_2MODULE_END_APPEND_STRING)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_BM_2MODULE_END_APPEND_STRING" );
			DRV_SCH_INF_GET_METHOD_ORDER[x]						= (DLL_SCH_INF_GET_METHOD_ORDER)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_METHOD_ORDER" );
			DRV_SCH_INF_CHECK_PICK_FROM_FM[x]					= (DLL_SCH_INF_CHECK_PICK_FROM_FM)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_CHECK_PICK_FROM_FM" );
			DRV_SCH_INF_CHECK_END_PART[x]						= (DLL_SCH_INF_CHECK_END_PART)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_CHECK_END_PART" );
			DRV_SCH_INF_FAIL_SCENARIO_OPERATION[x]				= (DLL_SCH_INF_FAIL_SCENARIO_OPERATION)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_FAIL_SCENARIO_OPERATION" );
			DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[x]			= (DLL_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED" );
			DRV_SCH_INF_CHECK_WAITING_MODE[x]					= (DLL_SCH_INF_CHECK_WAITING_MODE)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_CHECK_WAITING_MODE" );
			DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[x]			= (DLL_SCH_INF_TUNING_GET_MORE_APPEND_DATA)			GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_TUNING_GET_MORE_APPEND_DATA" );
			DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[x]			= (DLL_SCH_INF_METHOD_CHECK_SKIP_WHEN_START)		GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_METHOD_CHECK_SKIP_WHEN_START" );
			DRV_SCH_INF_ANIMATION_UPDATE_TM[x]					= (DLL_SCH_INF_ANIMATION_UPDATE_TM)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_ANIMATION_UPDATE_TM" ); // 2009.07.16
			DRV_SCH_INF_USER_DATABASE_REMAPPING[x]				= (DLL_SCH_INF_USER_DATABASE_REMAPPING)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_USER_DATABASE_REMAPPING" ); // 2010.11.09
			DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[x]				= (DLL_SCH_INF_ANIMATION_SOURCE_UPDATE)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_ANIMATION_SOURCE_UPDATE" ); // 2010.11.23
			DRV_SCH_INF_ANIMATION_RESULT_UPDATE[x]				= (DLL_SCH_INF_ANIMATION_RESULT_UPDATE)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_ANIMATION_RESULT_UPDATE" ); // 2012.03.15
			DRV_SCH_INF_MTLOUT_DATABASE_MOVE[x]					= (DLL_SCH_INF_MTLOUT_DATABASE_MOVE)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_MTLOUT_DATABASE_MOVE" ); // 2011.05.13
			DRV_SCH_INF_USE_EXTEND_OPTION[x]					= (DLL_SCH_INF_USE_EXTEND_OPTION)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_USE_EXTEND_OPTION" ); // 2011.11.18
			DRV_SCH_INF_USER_RECIPE_CHECK[x]					= (DLL_SCH_INF_USER_RECIPE_CHECK)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_USER_RECIPE_CHECK" ); // 2012.01.12
			DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[x]				= (DLL_SCH_INF_GET_PT_AT_PICK_FROM_CM)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_PT_AT_PICK_FROM_CM" ); // 2014.06.16
			DRV_SCH_INF_HANDOFF_ACTION[x]						= (DLL_SCH_INF_HANDOFF_ACTION)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_HANDOFF_ACTION" ); // 2014.06.27
			DRV_SCH_INF_USER_FLOW_NOTIFICATION[x]				= (DLL_SCH_INF_USER_FLOW_NOTIFICATION)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_USER_FLOW_NOTIFICATION" ); // 2015.06.05
			DRV_SCH_INF_GET_UPLOAD_MESSAGE[x]					= (DLL_SCH_INF_GET_UPLOAD_MESSAGE)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_UPLOAD_MESSAGE" ); // 2017.01.03
			DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[x]				= (DLL_SCH_INF_GET_DUMMY_METHOD_INDEX)				GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_GET_DUMMY_METHOD_INDEX" ); // 2017.09.11
			DRV_SCH_INF_FA_SEND_MESSAGE[x]						= (DLL_SCH_INF_FA_SEND_MESSAGE)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_FA_SEND_MESSAGE" ); // 2018.06.14
			DRV_SCH_INF_FA_ORDER_MESSAGE[x]						= (DLL_SCH_INF_FA_ORDER_MESSAGE)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_FA_ORDER_MESSAGE" ); // 2018.06.29

			DRV_SCH_INF_ENTER_CONTROL_FEM[x]					= (DLL_SCH_INF_ENTER_CONTROL)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_ENTER_CONTROL_FEM_PART" );
			DRV_SCH_INF_ENTER_CONTROL_TM[x]						= (DLL_SCH_INF_ENTER_CONTROL)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_ENTER_CONTROL_TM_PART" );

			DRV_SCH_INF_RUN_CONTROL_TM_1[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_1_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_2[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_2_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_3[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_3_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_4[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_4_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_5[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_5_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_6[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_6_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_7[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_7_PART" );
			DRV_SCH_INF_RUN_CONTROL_TM_8[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_TM_8_PART" );
			DRV_SCH_INF_RUN_CONTROL_FEM[x]						= (DLL_SCH_INF_RUN_CONTROL)							GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_I_SCH_INF_DLL_API_RUN_CONTROL_FEM_PART" );

			DRV_SCH_INF_USER_CONTROL_FEM[x]						= (DLL_SCH_INF_USER_CONTROL)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_USER_CONTROL_FEM_PART" ); // 2011.12.08
			DRV_SCH_INF_USER_CONTROL_TM[x]						= (DLL_SCH_INF_USER_CONTROL)						GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_USER_CONTROL_TM_PART" ); // 2011.12.08

			DRV_SCH_INF_RESTART_PRE_CONTROL_FEM[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_FEM_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_1[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_1_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_2[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_2_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_3[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_3_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_4[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_4_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_5[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_5_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_6[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_6_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_7[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_7_PART" ); // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_8[x]				= (DLL_SCH_INF_RESTART_PRE_CONTROL)					GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_INF_DLL_API_RESTART_PRE_CONTROL_TM_8_PART" ); // 2012.09.17
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			MAKE_MACRO_DLL_DEFINE( x , FM_POSSIBLE_PICK_FROM_FM , "FM_POSSIBLE_PICK_FROM_FM" )
			MAKE_MACRO_DLL_DEFINE( x , FM_SUPPOSING_PICK_FOR_OTHERSIDE , "FM_SUPPOSING_PICK_FOR_OTHERSIDE" )
			MAKE_MACRO_DLL_DEFINE( x , FM_PICK_DATA_FROM_FM , "FM_PICK_DATA_FROM_FM" )
			MAKE_MACRO_DLL_DEFINE( x , FM_PICK_DATA_FROM_FM_DETAIL , "FM_PICK_DATA_FROM_FM_DETAIL" )
			MAKE_MACRO_DLL_DEFINE( x , FM_SUPPLY_FIRSTLAST_CHECK , "FM_SUPPLY_FIRSTLAST_CHECK" )
			MAKE_MACRO_DLL_DEFINE( x , FM_FINISHED_CHECK , "FM_FINISHED_CHECK" )
			MAKE_MACRO_DLL_DEFINE( x , INTERRUPT_PART_CHECK , "INTERRUPT_PART_CHECK" )
			MAKE_MACRO_DLL_DEFINE( x , FM_OPERATION , "FM_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , FM_DATABASE_OPERATION , "FM_DATABASE_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , FM_PLACE_TO_CM , "FM_PLACE_TO_CM" )

			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_0 , "TM_OPERATION_0" )
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_1 , "TM_OPERATION_1" )
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_2 , "TM_OPERATION_2" )
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_3 , "TM_OPERATION_3" )
	//
	#ifndef PM_CHAMBER_12
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_4 , "TM_OPERATION_4" )
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_5 , "TM_OPERATION_5" )
	#ifndef PM_CHAMBER_30
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_6 , "TM_OPERATION_6" )
			MAKE_MACRO_DLL_DEFINE( x , TM_OPERATION_7 , "TM_OPERATION_7" )
	#endif
	#endif
	//
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_0 , "TM_MOVE_OPERATION_0" )
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_1 , "TM_MOVE_OPERATION_1" )
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_2 , "TM_MOVE_OPERATION_2" )
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_3 , "TM_MOVE_OPERATION_3" )
	//
	#ifndef PM_CHAMBER_12
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_4 , "TM_MOVE_OPERATION_4" )
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_5 , "TM_MOVE_OPERATION_5" )
	#ifndef PM_CHAMBER_30
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_6 , "TM_MOVE_OPERATION_6" )
			MAKE_MACRO_DLL_DEFINE( x , TM_MOVE_OPERATION_7 , "TM_MOVE_OPERATION_7" )
	#endif
	#endif
	//
			MAKE_MACRO_DLL_DEFINE( x , TM_DATABASE_OPERATION , "TM_DATABASE_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_PROCESSING , "CHECK_PROCESSING" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_PROCESSING_INFO , "CHECK_PROCESSING_INFO" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_PROCESSING_GET_REMAIN_TIME , "CHECK_PROCESSING_GET_REMAIN_TIME" )
			MAKE_MACRO_DLL_DEFINE( x , MAIN_PROCESS , "MAIN_PROCESS" )
			MAKE_MACRO_DLL_DEFINE( x , MAIN_PROCESS_PART_SIMPLE , "MAIN_PROCESS_PART_SIMPLE" )
			MAKE_MACRO_DLL_DEFINE( x , MAIN_PROCESS_PART_MULTIPLE , "MAIN_PROCESS_PART_MULTIPLE" )
			MAKE_MACRO_DLL_DEFINE( x , PROCESS_PART_TAG_SETTING , "PROCESS_PART_TAG_SETTING" )
			MAKE_MACRO_DLL_DEFINE( x , LOTPREPOST_PROCESS_OPERATION , "LOTPREPOST_PROCESS_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , PRE_PROCESS_OPERATION , "PRE_PROCESS_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , POST_PROCESS_OPERATION , "POST_PROCESS_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , HAS_PRE_PROCESS_OPERATION , "HAS_PRE_PROCESS_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , SPAWN_WAITING_BM_OPERATION , "SPAWN_WAITING_BM_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , FM_ALIC_OPERATION , "FM_ALIC_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , FM_POSSIBLE_PICK_FROM_CM , "FM_POSSIBLE_PICK_FROM_CM" )
			MAKE_MACRO_DLL_DEFINE( x , FM_OPERATION_THREAD_WAITING , "FM_OPERATION_THREAD_WAITING" )
			MAKE_MACRO_DLL_DEFINE( x , FM_OPERATION_THREAD_RESULT , "FM_OPERATION_THREAD_RESULT" )
			MAKE_MACRO_DLL_DEFINE( x , FM_OPERATION_MOVE_THREAD_STARTING , "FM_OPERATION_MOVE_THREAD_STARTING" )
			MAKE_MACRO_DLL_DEFINE( x , FM_OPERATION_MOVE_STARTING , "FM_OPERATION_MOVE_STARTING" )
			MAKE_MACRO_DLL_DEFINE( x , PLACE_TO_CM_POST_PART_with_FM , "PLACE_TO_CM_POST_PART_with_FM" )
			MAKE_MACRO_DLL_DEFINE( x , RUN_WAITING_BM_OPERATION , "RUN_WAITING_BM_OPERATION" )
			MAKE_MACRO_DLL_DEFINE( x , OTHER_LOT_LOAD_WAIT , "OTHER_LOT_LOAD_WAIT" )
			MAKE_MACRO_DLL_DEFINE( x , SPAWN_FM_MALIGNING , "SPAWN_FM_MALIGNING" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_FM_MALIGNING , "CHECK_FM_MALIGNING" )
			MAKE_MACRO_DLL_DEFINE( x , SPAWN_FM_MCOOLING , "SPAWN_FM_MCOOLING" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_FM_MCOOLING , "CHECK_FM_MCOOLING" )
			MAKE_MACRO_DLL_DEFINE( x , SPAWN_TM_MALIGNING , "SPAWN_TM_MALIGNING" )
			MAKE_MACRO_DLL_DEFINE( x , CHECK_TM_MALIGNING , "CHECK_TM_MALIGNING" )
			MAKE_MACRO_DLL_DEFINE( x , FM_MOVE_OPERATION , "FM_MOVE_OPERATION" )
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			//
			DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[x]		= (DLL_SCH_RECIPE_INFO_VERIFICATION_EIL)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "USER_RECIPE_INFO_VERIFICATION_EIL" ); // 2010.09.15
			//
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			//
			DRV_SCH_INF_LOAD_SIZE[x]		= (DLL_SCH_INF_LOAD_SIZE)	GetProcAddress( DRV_DLL_SCH_LOAD_POINT[x] , "_SCH_COMMON_USER_SIZE_CHECK" ); // 2018.08.22
			//
			if ( DRV_SCH_INF_LOAD_SIZE[x] ) {
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 1 , MAX_CASSETTE_SIDE		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_CASSETTE_SIDE      in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 2 , MAX_CASSETTE_SLOT_SIZE	) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_CASSETTE_SLOT_SIZE in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 3 , MAX_PM_CHAMBER_DEPTH		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_PM_CHAMBER_DEPTH   in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 4 , MAX_BM_CHAMBER_DEPTH		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_BM_CHAMBER_DEPTH   in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 5 , MAX_TM_CHAMBER_DEPTH		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_TM_CHAMBER_DEPTH   in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 6 , MAX_CLUSTER_DEPTH		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_CLUSTER_DEPTH      in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
				if ( ! (* DRV_SCH_INF_LOAD_SIZE[x] ) ( 7 , MAX_PM_SLOT_DEPTH		) ) _IO_CIM_PRINTF( "[WARNING] SIZE CHECK FAIL for MAX_PM_SLOT_DEPTH      in DLL(%s)\n" , DRV_DLL_SCH_FILENAME[x] );
			}
			//
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
			if ( DRV_SCH_INF_LOAD_INIT[x] ) (* DRV_SCH_INF_LOAD_INIT[x] ) ( ( mode != 0 ) && ( mode != 99 ) );
			//-----------------------------------------------------------------------------------------------------------------------------------------------------
//			return TRUE;
			//---------------------------------------------------------------------------------------------------------
		}
		else {
			//==================================================================================================================
			DRV_SCH_INF_LOAD_INIT[x]							= FALSE; // 2008.01.18
			DRV_SCH_INF_LOAD_SIZE[x]							= FALSE; // 2018.08.22
			DRV_SCH_INF_CONFIG_INITIALIZE[x]					= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_INIT[x]								= FALSE; // 2008.01.18
			DRV_SCH_INF_REVISION_HISTORY[x]						= FALSE;
			DRV_SCH_INF_SET_DLL_INSTANCE[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[x]	= FALSE;
			DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[x]	= FALSE;
			DRV_SCH_INF_RUN_PRE_OPERATION[x]					= FALSE;
			DRV_SCH_INF_RUN_PRE_BM[x]							= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[x]			= FALSE; // 2008.01.18
			DRV_SCH_INF_GUI_INTERFACE[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_EVENT_ANALYSIS[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_EVENT_PRE_ANALYSIS[x]					= FALSE; // 2008.01.18
			DRV_SCH_INF_SYSTEM_LOG[x]							= FALSE; // 2008.01.18
			DRV_SCH_INF_SYSTEM_FAIL[x]							= FALSE; // 2017.07.26

			DRV_SCH_INF_ENTER_CONTROL_FEM[x]					= FALSE; // 2008.01.18
			DRV_SCH_INF_ENTER_CONTROL_TM[x]						= FALSE; // 2008.01.18

			DRV_SCH_INF_RUN_CONTROL_TM_1[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_2[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_3[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_4[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_5[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_6[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_7[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_TM_8[x]						= FALSE; // 2008.01.18
			DRV_SCH_INF_RUN_CONTROL_FEM[x]						= FALSE; // 2008.01.18

			DRV_SCH_INF_USER_CONTROL_FEM[x]						= FALSE; // 2011.12.08
			DRV_SCH_INF_USER_CONTROL_TM[x]						= FALSE; // 2011.12.08

			DRV_SCH_INF_RESTART_PRE_CONTROL_FEM[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_1[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_2[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_3[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_4[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_5[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_6[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_7[x]				= FALSE; // 2012.09.17
			DRV_SCH_INF_RESTART_PRE_CONTROL_TM_8[x]				= FALSE; // 2012.09.17

			DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[x]				= FALSE; // 2010.09.16

			//==================================================================================================================

			// 2017.01.03
			//
			DRV_SCH_INF_TRANSFER_OPTION[x]						= FALSE;

			DRV_SCH_INF_FINISHED_CHECK[x]						= FALSE;
			DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[x]			= FALSE;
			DRV_SCH_INF_GET_PROCESS_FILENAME[x]					= FALSE;
			DRV_SCH_INF_GET_PROCESS_FILENAME2[x]				= FALSE;
			DRV_SCH_INF_GET_PROCESS_INDEX_DATA[x]				= FALSE;
			DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[x]			= FALSE;
			DRV_SCH_INF_SIDE_SUPPLY[x]							= FALSE;
			DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[x]				= FALSE;
			DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[x]			= FALSE;
			DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[x]		= FALSE;
			DRV_SCH_INF_PM_2MODULE_SAME_BODY[x]					= FALSE;
			DRV_SCH_INF_BM_2MODULE_SAME_BODY[x]					= FALSE;
			DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[x]			= FALSE;
			DRV_SCH_INF_GET_METHOD_ORDER[x]						= FALSE;
			DRV_SCH_INF_CHECK_PICK_FROM_FM[x]					= FALSE;
			DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[x]				= FALSE;
			DRV_SCH_INF_HANDOFF_ACTION[x]						= FALSE;
			DRV_SCH_INF_CHECK_END_PART[x]						= FALSE;
			DRV_SCH_INF_FAIL_SCENARIO_OPERATION[x]				= FALSE;
			DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[x]			= FALSE;
			DRV_SCH_INF_CHECK_WAITING_MODE[x]					= FALSE;
			DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[x]			= FALSE;
			DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[x]			= FALSE;
			DRV_SCH_INF_ANIMATION_UPDATE_TM[x]					= FALSE;
			DRV_SCH_INF_USER_DATABASE_REMAPPING[x]				= FALSE;
			DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[x]				= FALSE;
			DRV_SCH_INF_ANIMATION_RESULT_UPDATE[x]				= FALSE;
			DRV_SCH_INF_MTLOUT_DATABASE_MOVE[x]					= FALSE;
			DRV_SCH_INF_USE_EXTEND_OPTION[x]					= FALSE;
			DRV_SCH_INF_USER_RECIPE_CHECK[x]					= FALSE;
			DRV_SCH_INF_USER_FLOW_NOTIFICATION[x]				= FALSE;
			DRV_SCH_INF_GET_UPLOAD_MESSAGE[x]					= FALSE;
			DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[x]				= FALSE;
			DRV_SCH_INF_FA_SEND_MESSAGE[x]						= FALSE;
			DRV_SCH_INF_FA_ORDER_MESSAGE[x]						= FALSE;

			//==================================================================================================================

			//
//			if ( mode == 99 ) {
//				DLL_USER_SCH_USE = -1;
//			}
//			else {
//				if ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME ) > 0 ) { // 2008.04.03
//					_IO_CIM_PRINTF( "[WARNING] SCHEDULING INTERFACE DLL(%s) Loading Fail\n" , DRV_DLL_SCH_FILENAME );
//					DLL_USER_SCH_USE = 0;
//				}
//				else {
//					DLL_USER_SCH_USE = -1;
//				}
//			}
			//
		}
		//

		//
	}
	//
	//==================================================================================================================
	//
	if ( DRV_DLL_SCH_LOAD_POINT[0] || DRV_DLL_SCH_LOAD_POINT[1] ) {
		//-----------------------------------------------------------------------------------------------------------------------------------------------------
		if ( mode == 5 ) DLL_USER_SCH_USE = 2;
		else             DLL_USER_SCH_USE = 1;
		//-----------------------------------------------------------------------------------------------------------------------------------------------------
		return TRUE;
	}
	else {
		//==================================================================================================================
		//
		if ( mode == 99 ) {
			DLL_USER_SCH_USE = -1;
		}
		else {
			if ( ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME[0] ) > 0 ) || ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME[1] ) > 0 ) ) { // 2008.04.03
				_IO_CIM_PRINTF( "[WARNING] SCHEDULING INTERFACE DLL(%s,%s) Loading Fail\n" , DRV_DLL_SCH_FILENAME[0] , DRV_DLL_SCH_FILENAME[1] );
				DLL_USER_SCH_USE = 0;
			}
			else {
				DLL_USER_SCH_USE = -1;
			}
		}
		//
		//==================================================================================================================
		//
	}
	//
	//==================================================================================================================
	//
	return FALSE;
}

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

BOOL _SCH_INF_DLL_CONTROL_CONFIG_INITIALIZE( int runalg , int subalg , int guialg , int maxpm ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_CONFIG_INITIALIZE[0] ) {
		Res1 = ( ( *DRV_SCH_INF_CONFIG_INITIALIZE[0] ) ( runalg , subalg , guialg , maxpm ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_CONFIG_INITIALIZE[1] ) {
		Res2 = ( ( *DRV_SCH_INF_CONFIG_INITIALIZE[1] ) ( runalg , subalg , guialg , maxpm ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}

void _SCH_INF_DLL_CONTROL_SET_INSTANCE( HINSTANCE dllpt , int unload ) {
	if ( DRV_SCH_INF_SET_DLL_INSTANCE[0] ) ( *DRV_SCH_INF_SET_DLL_INSTANCE[0] ) ( dllpt , unload );
	if ( DRV_SCH_INF_SET_DLL_INSTANCE[1] ) ( *DRV_SCH_INF_SET_DLL_INSTANCE[1] ) ( dllpt , unload );
}

void _SCH_INF_DLL_CONTROL_RUN_INIT( int apmode , int side ) {
	if ( DRV_SCH_INF_RUN_INIT[0] ) (*DRV_SCH_INF_RUN_INIT[0] ) ( apmode , side );
	if ( DRV_SCH_INF_RUN_INIT[1] ) (*DRV_SCH_INF_RUN_INIT[1] ) ( apmode , side );
}

/*
//
// 2017.01.05
void _SCH_INF_DLL_CONTROL_REVISION_HISTORY( int mode , char *data , int count ) {
	if ( DRV_SCH_INF_REVISION_HISTORY[0] ) (*DRV_SCH_INF_REVISION_HISTORY[0] ) ( mode , data , count );
	if ( DRV_SCH_INF_REVISION_HISTORY[1] ) (*DRV_SCH_INF_REVISION_HISTORY[1] ) ( mode , data , count );
}
*/

//
// 2017.01.05
void _SCH_INF_DLL_CONTROL_REVISION_HISTORY( int mode , char *data , int count ) {
	char Buffer[128];
	//
	strcpy( data , "" );
	//
	if ( DRV_SCH_INF_REVISION_HISTORY[0] ) {
		strcpy( Buffer , "" );		(*DRV_SCH_INF_REVISION_HISTORY[0] ) ( mode , Buffer , 64 );		if ( Buffer[0] != 0 ) strcat( data , Buffer );
	}
	if ( DRV_SCH_INF_REVISION_HISTORY[1] ) {
		strcpy( Buffer , "" );		(*DRV_SCH_INF_REVISION_HISTORY[1] ) ( mode , Buffer , 64 );		if ( Buffer[0] != 0 ) {		if ( data[0] != 0 ) strcat( data , "|" );		strcat( data , Buffer );		}
	}
}

int  _SCH_INF_DLL_CONTROL_RECIPE_ANALYSIS_BM_USEFLAG_SETTING( int CHECKING_SIDE , int lc_target_wafer_count , char *errorstring , int *TMATM ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[0] ) {
		Res1 = ( (*DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[0] ) ( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM ) );
	}
	else {
		Res1 = -1;
	}
	if ( DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[1] ) {
		Res2 = ( (*DRV_SCH_INF_RECIPE_ANALYSIS_BM_USEFLAG_SETTING[1] ) ( CHECKING_SIDE , lc_target_wafer_count , errorstring , TMATM ) );
	}
	else {
		Res2 = -1;
	}
	if ( Res1 != -1 ) return Res1;
	if ( Res2 != -1 ) return Res1;
	return -1;
}

BOOL _SCH_INF_DLL_CONTROL_READING_CONFIG_FILE_WHEN_CHANGE_ALG( int chg_ALG_DATA_RUN ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[0] ) {
		Res1 = ( (*DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[0] ) ( chg_ALG_DATA_RUN ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[1] ) {
		Res2 = ( (*DRV_SCH_INF_READING_CONFIG_FILE_WHEN_CHANGE_ALG[1] ) ( chg_ALG_DATA_RUN ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}

BOOL _SCH_INF_DLL_CONTROL_RUN_PRE_OPERATION( int CHECKING_SIDE , int mode , int *dataflag , int *dataflag2 , int *dataflag3 , char *errorstring ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_RUN_PRE_OPERATION[0] ) {
		Res1 = ( (*DRV_SCH_INF_RUN_PRE_OPERATION[0] ) ( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_RUN_PRE_OPERATION[1] ) {
		Res2 = ( (*DRV_SCH_INF_RUN_PRE_OPERATION[1] ) ( CHECKING_SIDE , mode , dataflag , dataflag2 , dataflag3 , errorstring ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}

void _SCH_INF_DLL_CONTROL_RUN_PRE_BM_CONDITION( int side , int bmc , int mode ) {
	if ( DRV_SCH_INF_RUN_PRE_BM[0] ) (*DRV_SCH_INF_RUN_PRE_BM[0] ) ( side , bmc , mode );
	if ( DRV_SCH_INF_RUN_PRE_BM[1] ) (*DRV_SCH_INF_RUN_PRE_BM[1] ) ( side , bmc , mode );
}

BOOL _SCH_INF_DLL_CONTROL_RUN_MFIC_COMPLETE_MACHINE( int side , int data , int option ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[0] ) {
		Res1 = ( (*DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[0] ) ( side , data , option ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[1] ) {
		Res2 = ( (*DRV_SCH_INF_RUN_MFIC_COMPLETE_MACHINE[1] ) ( side , data , option ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

int  _SCH_INF_DLL_CONTROL_GUI_INTERFACE( int style , int alg , int mode , int data , char *Message , char *list , int *retdata ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_GUI_INTERFACE[0] ) {
		Res1 = ( (*DRV_SCH_INF_GUI_INTERFACE[0] ) ( style , alg , mode , data , Message , list , retdata ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_GUI_INTERFACE[1] ) {
		Res2 = ( (*DRV_SCH_INF_GUI_INTERFACE[1] ) ( style , alg , mode , data , Message , list , retdata ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}

int  _SCH_INF_DLL_CONTROL_EVENT_PRE_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_EVENT_PRE_ANALYSIS[0] ) {
		Res1 = ( (*DRV_SCH_INF_EVENT_PRE_ANALYSIS[0] ) ( FullMessage , RunMessage , ElseMessage , ErrorMessage ) );
	}
	else {
		Res1 = 1;
	}
	if ( DRV_SCH_INF_EVENT_PRE_ANALYSIS[1] ) {
		Res2 = ( (*DRV_SCH_INF_EVENT_PRE_ANALYSIS[1] ) ( FullMessage , RunMessage , ElseMessage , ErrorMessage ) );
	}
	else {
		Res2 = 1;
	}
	if ( Res1 != 1 ) return Res1;
	if ( Res2 != 1 ) return Res2;
	return 1;
}

int  _SCH_INF_DLL_CONTROL_EVENT_ANALYSIS( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_EVENT_ANALYSIS[0] ) {
		Res1 = ( (*DRV_SCH_INF_EVENT_ANALYSIS[0] ) ( FullMessage , RunMessage , ElseMessage , ErrorMessage ) );
	}
	else {
		Res1 = -1;
	}
	if ( DRV_SCH_INF_EVENT_ANALYSIS[1] ) {
		Res2 = ( (*DRV_SCH_INF_EVENT_ANALYSIS[1] ) ( FullMessage , RunMessage , ElseMessage , ErrorMessage ) );
	}
	else {
		Res2 = -1;
	}
	if ( Res1 != -1 ) return Res1;
	if ( Res2 != -1 ) return Res2;
	return -1;
}

void  _SCH_INF_DLL_CONTROL_SYSTEM_LOG( char *filename , int side ) {
	if ( DRV_SCH_INF_SYSTEM_LOG[0] )	(*DRV_SCH_INF_SYSTEM_LOG[0] ) ( filename , side );
	if ( DRV_SCH_INF_SYSTEM_LOG[1] )	(*DRV_SCH_INF_SYSTEM_LOG[1] ) ( filename , side );
}

void  _SCH_INF_DLL_CONTROL_SYSTEM_FAIL( BOOL errorfinish , int side , int data1 , int data2 , int data3 ) { // 2017.07.26
	if ( DRV_SCH_INF_SYSTEM_FAIL[0] )	(*DRV_SCH_INF_SYSTEM_FAIL[0] ) ( errorfinish , side , data1 , data2 , data3 );
	if ( DRV_SCH_INF_SYSTEM_FAIL[1] )	(*DRV_SCH_INF_SYSTEM_FAIL[1] ) ( errorfinish , side , data1 , data2 , data3 );
}

int  _SCH_INF_DLL_CONTROL_TRANSFER_OPTION( int mode , int data ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_TRANSFER_OPTION[0] ) {
		Res1 = ( (*DRV_SCH_INF_TRANSFER_OPTION[0] ) ( mode , data ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_TRANSFER_OPTION[1] ) {
		Res2 = ( (*DRV_SCH_INF_TRANSFER_OPTION[1] ) ( mode , data ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}
//===========================================================================================================================



BOOL _SCH_INF_DLL_CONTROL_FINISHED_CHECK( int side ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_FINISHED_CHECK[0] ) {
		Res1 = ( (*DRV_SCH_INF_FINISHED_CHECK[0])( side ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_FINISHED_CHECK[1] ) {
		Res2 = ( (*DRV_SCH_INF_FINISHED_CHECK[1])( side ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}

BOOL _SCH_INF_DLL_CONTROL_CASSETTE_CHECK_INVALID_INFO( int mode , int side , int ch1 , int ch2 , int option ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[0] ) {
		Res1 = ( (*DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[0])( mode , side , ch1 , ch2 , option ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[1] ) {
		Res2 = ( (*DRV_SCH_INF_CASSETTE_CHECK_INVALID_INFO[1])( mode , side , ch1 , ch2 , option ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}


BOOL _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME( char *logpath , char *lotdir , int prcs , int ch , int side , int i , int j , int e , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_GET_PROCESS_FILENAME[0] ) {
		Res1 = ( (*DRV_SCH_INF_GET_PROCESS_FILENAME[0])( logpath , lotdir , prcs , ch , side , i , j , e , filename , dummy , mode , notuse , highappend ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_GET_PROCESS_FILENAME[1] ) {
		Res2 = ( (*DRV_SCH_INF_GET_PROCESS_FILENAME[1])( logpath , lotdir , prcs , ch , side , i , j , e , filename , dummy , mode , notuse , highappend ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

BOOL _SCH_INF_DLL_CONTROL_GET_PROCESS_FILENAME2( char *logpath , char *lotdir , int prcs , int ch , int side , int pt , int *i , int *j , int *e , char *filename , char *foldername , BOOL *dummy , int *mode , BOOL *notuse , BOOL *highappend , BOOL *filenotuse ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_GET_PROCESS_FILENAME2[0] ) {
		Res1 = ( (*DRV_SCH_INF_GET_PROCESS_FILENAME2[0])( logpath , lotdir , prcs , ch , side , pt , i , j , e , filename , foldername , dummy , mode , notuse , highappend , filenotuse ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_GET_PROCESS_FILENAME2[1] ) {
		Res2 = ( (*DRV_SCH_INF_GET_PROCESS_FILENAME2[1])( logpath , lotdir , prcs , ch , side , pt , i , j , e , filename , foldername , dummy , mode , notuse , highappend , filenotuse ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}


void _SCH_INF_DLL_CONTROL_GET_PROCESS_INDEX_DATA( int mode , int x , int *rmode , BOOL *nomore , int *xoffset ) {
	if ( DRV_SCH_INF_GET_PROCESS_INDEX_DATA[0] ) (*DRV_SCH_INF_GET_PROCESS_INDEX_DATA[0])( mode , x , rmode , nomore , xoffset );
	if ( DRV_SCH_INF_GET_PROCESS_INDEX_DATA[1] ) (*DRV_SCH_INF_GET_PROCESS_INDEX_DATA[1])( mode , x , rmode , nomore , xoffset );
}

int  _SCH_INF_DLL_CONTROL_MESSAGE_CONTROL_FOR_USER( BOOL postmode , int id , int i1 , int i2 , int i3 , int i4 , int i5 , int i6 , int i7 , int i8 , int i9 , int i10 , double d1 , double d2 , double d3 , char *s1 , char *s2 , char *s3 ) { // 2012.11.29
	int Res1 , Res2;
	if ( DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[0] ) {
		Res1 = ( (*DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[0])( postmode , id , i1 , i2 , i3 , i4 , i5 , i6 , i7 , i8 , i9 , i10 , d1 , d2 , d3 , s1 , s2 , s3 ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[1] ) {
		Res2 = ( (*DRV_SCH_INF_GET_MESSAGE_CONTROL_FOR_USER[1])( postmode , id , i1 , i2 , i3 , i4 , i5 , i6 , i7 , i8 , i9 , i10 , d1 , d2 , d3 , s1 , s2 , s3 ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}


BOOL _SCH_INF_DLL_CONTROL_SIDE_SUPPLY( int side , BOOL EndCheck , int sup_style , int *res ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_SIDE_SUPPLY[0] ) {
		Res1 = ( (*DRV_SCH_INF_SIDE_SUPPLY[0])( side , EndCheck , sup_style , res ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_SIDE_SUPPLY[1] ) {
		Res2 = ( (*DRV_SCH_INF_SIDE_SUPPLY[1])( side , EndCheck , sup_style , res ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}


BOOL _SCH_INF_DLL_CONTROL_FM_ARM_APLUSB_SWAPPING() {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[0] ) {
		Res1 = ( (*DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[0])() );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[1] ) {
		Res2 = ( (*DRV_SCH_INF_FM_ARM_APLUSB_SWAPPING[1])() );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}



BOOL _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_SKIP_CONTROL( int side , int pt , int ch ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[0] ) {
		Res1 = ( (*DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[0])( side , pt , ch ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[1] ) {
		Res2 = ( (*DRV_SCH_INF_PM_MULTI_ENABLE_SKIP_CONTROL[1])( side , pt , ch ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}


BOOL _SCH_INF_DLL_CONTROL_PM_MULTI_ENABLE_DISABLE_CONTROL() {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[0] ) {
		Res1 = ( (*DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[0])() );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[1] ) {
		Res2 = ( (*DRV_SCH_INF_PM_MULTI_ENABLE_DISABLE_CONTROL[1])() );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

int _SCH_INF_DLL_CONTROL_PM_2MODULE_SAME_BODY() { // 2007.10.07
	int Res1 , Res2;
	if ( DRV_SCH_INF_PM_2MODULE_SAME_BODY[0] ) {
		Res1 = ( (*DRV_SCH_INF_PM_2MODULE_SAME_BODY[0])() );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_PM_2MODULE_SAME_BODY[1] ) {
		Res2 = ( (*DRV_SCH_INF_PM_2MODULE_SAME_BODY[1])() );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}

int _SCH_INF_DLL_CONTROL_BM_2MODULE_SAME_BODY() {
	int Res1 , Res2;
	if ( DRV_SCH_INF_BM_2MODULE_SAME_BODY[0] ) {
		Res1 = ( (*DRV_SCH_INF_BM_2MODULE_SAME_BODY[0])() );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_BM_2MODULE_SAME_BODY[1] ) {
		Res2 = ( (*DRV_SCH_INF_BM_2MODULE_SAME_BODY[1])() );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}


void _SCH_INF_DLL_CONTROL_BM_2MODULE_END_APPEND_STRING( int side , int bmc , char *appstr ) {
	if ( DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[0] ) (*DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[0])( side , bmc , appstr );
	if ( DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[1] ) (*DRV_SCH_INF_BM_2MODULE_END_APPEND_STRING[1])( side , bmc , appstr );
}


int  _SCH_INF_DLL_CONTROL_GET_METHOD_ORDER( int side , int pt ) {
	int Res1 , Res2;
	//
	if ( DRV_SCH_INF_GET_METHOD_ORDER[0] || DRV_SCH_INF_GET_METHOD_ORDER[1] ) {
		if ( DRV_SCH_INF_GET_METHOD_ORDER[0] ) {
			Res1 = ( (*DRV_SCH_INF_GET_METHOD_ORDER[0])( side , pt ) );
		}
		else {
			Res1 = 0;
		}
		if ( DRV_SCH_INF_GET_METHOD_ORDER[1] ) {
			Res2 = ( (*DRV_SCH_INF_GET_METHOD_ORDER[1])( side , pt ) );
		}
		else {
			Res2 = 0;
		}
		//
		if ( Res1 != 0 ) return Res1;
		if ( Res2 != 0 ) return Res2;
		//
	}
	else {
		//
		if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() == 0 ) return 0; // 2009.06.24
		//
		if ( pt < MAX_CASSETTE_SLOT_SIZE ) {
			return _i_SCH_CLUSTER_Get_PathDo( side , pt );
		}
		//
	}
	return 0;
	//
}


int  _SCH_INF_DLL_CONTROL_CHECK_PICK_FROM_FM( int side , int pt , int subchk ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_CHECK_PICK_FROM_FM[0] ) {
		Res1 = ( (*DRV_SCH_INF_CHECK_PICK_FROM_FM[0])( side , pt , subchk ) );
	}
	else {
		Res1 = 1;
	}
	if ( DRV_SCH_INF_CHECK_PICK_FROM_FM[1] ) {
		Res2 = ( (*DRV_SCH_INF_CHECK_PICK_FROM_FM[1])( side , pt , subchk ) );
	}
	else {
		Res2 = 1;
	}
	if ( Res1 != 1 ) return Res1;
	if ( Res2 != 1 ) return Res2;
	return 1;
}

BOOL _SCH_INF_DLL_CONTROL_GET_PT_AT_PICK_FROM_CM( int side , int *pt , BOOL fmmode , int subchk ) { // 2014.06.16
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[0] ) {
		Res1 = ( (*DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[0])( side , pt , fmmode , subchk ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[1] ) {
		Res2 = ( (*DRV_SCH_INF_GET_PT_AT_PICK_FROM_CM[1])( side , pt , fmmode , subchk ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

void _SCH_INF_DLL_CONTROL_HANDOFF_ACTION( int cm , int mode , int subchk ) { // 2014.06.27
	if ( DRV_SCH_INF_HANDOFF_ACTION[0] ) ( (*DRV_SCH_INF_HANDOFF_ACTION[0])( cm , mode , subchk ) );
	if ( DRV_SCH_INF_HANDOFF_ACTION[1] ) ( (*DRV_SCH_INF_HANDOFF_ACTION[1])( cm , mode , subchk ) );
}

BOOL _SCH_INF_DLL_CONTROL_CHECK_END_PART( int side , int mode ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_CHECK_END_PART[0] ) {
		Res1 = ( (*DRV_SCH_INF_CHECK_END_PART[0])( side , mode ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_CHECK_END_PART[1] ) {
		Res2 = ( (*DRV_SCH_INF_CHECK_END_PART[1])( side , mode ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}


int _SCH_INF_DLL_CONTROL_FAIL_SCENARIO_OPERATION( int side , int ch , BOOL normal , int loopindex , BOOL BeforeDisableResult , int *DisableHWMode , int *CheckMode , int *AbortWaitTag , int *PrcsResult , BOOL *DoMore ) {
	int Res1 , Res2;
	if ( DRV_SCH_INF_FAIL_SCENARIO_OPERATION[0] || DRV_SCH_INF_FAIL_SCENARIO_OPERATION[1] ) {
		if ( DRV_SCH_INF_FAIL_SCENARIO_OPERATION[0] ) {
			Res1 = ( (*DRV_SCH_INF_FAIL_SCENARIO_OPERATION[0])( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore ) );
		}
		else {
			Res1 = 0;
		}
		if ( DRV_SCH_INF_FAIL_SCENARIO_OPERATION[1] ) {
			Res2 = ( (*DRV_SCH_INF_FAIL_SCENARIO_OPERATION[1])( side , ch , normal , loopindex , BeforeDisableResult , DisableHWMode , CheckMode , AbortWaitTag , PrcsResult , DoMore ) );
		}
		else {
			Res2 = 0;
		}
		//
		if ( Res1 != 0 ) return Res1;
		if ( Res2 != 0 ) return Res2;
		//
	}
	else {
		*DoMore = FALSE;
	}
	return 0;
}

BOOL _SCH_INF_DLL_CONTROL_EXTEND_FM_ARM_CROSS_FIXED( int side ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[0] ) {
		Res1 = ( (*DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[0])( side ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[1] ) {
		Res2 = ( (*DRV_SCH_INF_EXTEND_FM_ARM_CROSS_FIXED[1])( side ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

BOOL _SCH_INF_DLL_CONTROL_CHECK_WAITING_MODE( int side , int *mode , BOOL precheck , int lc ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_CHECK_WAITING_MODE[0] ) {
		Res1 = ( (*DRV_SCH_INF_CHECK_WAITING_MODE[0])( side , mode , precheck , lc ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_CHECK_WAITING_MODE[1] ) {
		Res2 = ( (*DRV_SCH_INF_CHECK_WAITING_MODE[1])( side , mode , precheck , lc ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

BOOL _SCH_INF_DLL_CONTROL_TUNING_GET_MORE_APPEND_DATA( int mode , int ch , int curorder , int pjindex , int pjno ,
													  int TuneData_Module , int TuneData_Order , int TuneData_Step , char *TuneData_Name , char *TuneData_Data , int TuneData_Index , char *AppendStr ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[0] ) {
		Res1 = ( (*DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[0])( mode , ch , curorder , pjindex , pjno ,
															TuneData_Module , TuneData_Order , TuneData_Step , TuneData_Name , TuneData_Data , TuneData_Index , AppendStr ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[1] ) {
		Res2 = ( (*DRV_SCH_INF_TUNING_GET_MORE_APPEND_DATA[1])( mode , ch , curorder , pjindex , pjno ,
															TuneData_Module , TuneData_Order , TuneData_Step , TuneData_Name , TuneData_Data , TuneData_Index , AppendStr ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

BOOL _SCH_INF_DLL_CONTROL_METHOD_CHECK_SKIP_WHEN_START( int mode , int side , int ch , int slot , int option ) {
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[0] ) {
		Res1 = ( (*DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[0])( mode , side , ch , slot , option ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[1] ) {
		Res2 = ( (*DRV_SCH_INF_METHOD_CHECK_SKIP_WHEN_START[1])( mode , side , ch , slot , option ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

void _SCH_INF_DLL_CONTROL_ANIMATION_UPDATE_TM( int TMSide , int Mode , BOOL Resetting , int Type , int Chamber , int Arm , int Slot , int Depth ) { // 2009.07.16
	if ( DRV_SCH_INF_ANIMATION_UPDATE_TM[0] ) (*DRV_SCH_INF_ANIMATION_UPDATE_TM[0])( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
	if ( DRV_SCH_INF_ANIMATION_UPDATE_TM[1] ) (*DRV_SCH_INF_ANIMATION_UPDATE_TM[1])( TMSide , Mode , Resetting , Type , Chamber , Arm , Slot , Depth );
}

BOOL _SCH_INF_DLL_INFO_VERIFICATION_EIL( int side , int pos , int bm , int slot , int cm , int wafer , char *clusterfile , char *feedbackstr ) { // 2010.09.16
	BOOL Res1 , Res2;
	if ( DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[0] ) {
		Res1 = ( (* DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[0] ) ( side , pos , bm , slot , cm , wafer , clusterfile , feedbackstr ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[1] ) {
		Res2 = ( (* DRV_SCH_RECIPE_INFO_VERIFICATION_EIL[1] ) ( side , pos , bm , slot , cm , wafer , clusterfile , feedbackstr ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}


BOOL _SCH_INF_DLL_CONTROL_USER_DATABASE_REMAPPING( int side , int mode , BOOL precheck , int option ) { // 2010.11.09
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_USER_DATABASE_REMAPPING[0] ) {
		Res1 = ( (* DRV_SCH_INF_USER_DATABASE_REMAPPING[0] ) ( side , mode , precheck , option ) );
	}
	else {
		Res1 = TRUE;
	}
	if ( DRV_SCH_INF_USER_DATABASE_REMAPPING[1] ) {
		Res2 = ( (* DRV_SCH_INF_USER_DATABASE_REMAPPING[1] ) ( side , mode , precheck , option ) );
	}
	else {
		Res2 = TRUE;
	}
	if ( !Res1 ) return FALSE;
	if ( !Res2 ) return FALSE;
	return TRUE;
}

BOOL _SCH_INF_DLL_CONTROL_ANIMATION_SOURCE_UPDATE( int when , int mode , int tmside , int station , int slot , int *srccass , int *srcslot ) { // 2010.11.23
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[0] ) {
		Res1 = ( (* DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[0] ) ( when , mode , tmside , station , slot , srccass , srcslot ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[1] ) {
		Res2 = ( (* DRV_SCH_INF_ANIMATION_SOURCE_UPDATE[1] ) ( when , mode , tmside , station , slot , srccass , srcslot ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return TRUE;
	if ( Res2 ) return TRUE;
	return FALSE;
}

int _SCH_INF_DLL_CONTROL_ANIMATION_RESULT_UPDATE( int side , int pt , int data , int sub ) { // 2012.03.15
	int Res1 , Res2;
	if ( DRV_SCH_INF_ANIMATION_RESULT_UPDATE[0] ) {
		Res1 = ( (* DRV_SCH_INF_ANIMATION_RESULT_UPDATE[0] ) ( side , pt , data , sub ) );
	}
	else {
		Res1 = data;
	}
	if ( DRV_SCH_INF_ANIMATION_RESULT_UPDATE[1] ) {
		Res2 = ( (* DRV_SCH_INF_ANIMATION_RESULT_UPDATE[1] ) ( side , pt , data , sub ) );
	}
	else {
		Res2 = data;
	}
	if ( Res1 != data ) return Res1;
	if ( Res2 != data ) return Res2;
	return data;
}

BOOL _SCH_INF_DLL_CONTROL_MTLOUT_DATABASE_MOVE( int mode , int tside , int tpoint , int sside , int spoint , BOOL *tmsupply ) { // 2011.05.13
	BOOL Res1 , Res2;
	if ( DRV_SCH_INF_MTLOUT_DATABASE_MOVE[0] ) {
		Res1 = ( (* DRV_SCH_INF_MTLOUT_DATABASE_MOVE[0] ) ( mode , tside , tpoint , sside , spoint , tmsupply ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_MTLOUT_DATABASE_MOVE[1] ) {
		Res2 = ( (* DRV_SCH_INF_MTLOUT_DATABASE_MOVE[1] ) ( mode , tside , tpoint , sside , spoint , tmsupply ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}

int  _SCH_INF_DLL_CONTROL_USE_EXTEND_OPTION( int mode , int sub ) { // 2011.11.18
	int Res1 , Res2;
	if ( DRV_SCH_INF_USE_EXTEND_OPTION[0] ) {
		Res1 = ( (* DRV_SCH_INF_USE_EXTEND_OPTION[0] ) ( mode , sub ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_USE_EXTEND_OPTION[1] ) {
		Res2 = ( (* DRV_SCH_INF_USE_EXTEND_OPTION[1] ) ( mode , sub ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}

int  _SCH_INF_DLL_CONTROL_USER_RECIPE_CHECK( int mode , int side , int data1 , int data2 , int data3 , int *res ) { // 2012.01.12
	int Res1 , Res2;
	if ( DRV_SCH_INF_USER_RECIPE_CHECK[0] ) {
		Res1 = ( (* DRV_SCH_INF_USER_RECIPE_CHECK[0] ) ( mode , side , data1 , data2 , data3 , res ) );
	}
	else {
		Res1 = 0;
	}
	if ( DRV_SCH_INF_USER_RECIPE_CHECK[1] ) {
		Res2 = ( (* DRV_SCH_INF_USER_RECIPE_CHECK[1] ) ( mode , side , data1 , data2 , data3 , res ) );
	}
	else {
		Res2 = 0;
	}
	if ( Res1 != 0 ) return Res1;
	if ( Res2 != 0 ) return Res2;
	return 0;
}

int _SCH_INF_DLL_CONTROL_USER_FLOW_NOTIFICATION( int mode , int usertag , int side , int pt , int opt1 , int opt2 , int opt3 , int opt4 , int opt5 ) { // 2015.05.29
	int Res1 , Res2;
	if ( DRV_SCH_INF_USER_FLOW_NOTIFICATION[0] ) {
		Res1 = ( (*DRV_SCH_INF_USER_FLOW_NOTIFICATION[0])( mode , usertag , side , pt , opt1 , opt2 , opt3 , opt4 , opt5 ) );
	}
	else {
		Res1 = -1;
	}
	if ( DRV_SCH_INF_USER_FLOW_NOTIFICATION[1] ) {
		Res2 = ( (*DRV_SCH_INF_USER_FLOW_NOTIFICATION[1])( mode , usertag , side , pt , opt1 , opt2 , opt3 , opt4 , opt5 ) );
	}
	else {
		Res2 = -1;
	}
	if ( Res1 != -1 ) return Res1;
	if ( Res2 != -1 ) return Res2;
	return -1;
}


BOOL _SCH_INF_DLL_CONTROL_GET_UPLOAD_MESSAGE( int mode , int ch , char *message ) { // 2017.01.03
	if ( DRV_SCH_INF_GET_UPLOAD_MESSAGE[0] ) {
		if ( (*DRV_SCH_INF_GET_UPLOAD_MESSAGE[0])( mode , ch , message ) ) {
			if ( message[0] != 0 ) return TRUE;
		}
	}
	if ( DRV_SCH_INF_GET_UPLOAD_MESSAGE[1] ) {
		if ( (*DRV_SCH_INF_GET_UPLOAD_MESSAGE[1])( mode , ch , message ) ) {
			if ( message[0] != 0 ) return TRUE;
		}
	}
	return FALSE;
}

BOOL _SCH_INF_DLL_CONTROL_GET_DUMMY_METHOD_INDEX( int ch , int mode , int wh , int side , int pt , int *GetIndex ) { // 2017.09.11
	if ( DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[0] ) {
		if ( (*DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[0])( ch , mode , wh , side , pt , GetIndex ) ) {
			return TRUE;
		}
	}
	if ( DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[1] ) {
		if ( (*DRV_SCH_INF_GET_DUMMY_METHOD_INDEX[1])( ch , mode , wh , side , pt , GetIndex ) ) {
			return TRUE;
		}
	}
	return FALSE;
}

// 2011.12.08

int USER_DLL_SCH_INF_HAS_USER_CONTROL_FEM() {
	if ( DLL_USER_SCH_USE != 1 ) return FALSE;
	if ( DRV_SCH_INF_USER_CONTROL_FEM[0] ) return TRUE;
	if ( DRV_SCH_INF_USER_CONTROL_FEM[1] ) return TRUE;
	return FALSE;
}
int USER_DLL_SCH_INF_HAS_USER_CONTROL_TM() {
	if ( DLL_USER_SCH_USE != 1 ) return FALSE;
	if ( DRV_SCH_INF_USER_CONTROL_TM[0] ) return TRUE;
	if ( DRV_SCH_INF_USER_CONTROL_TM[1] ) return TRUE;
	return FALSE;
}

int USER_DLL_SCH_INF_USER_CONTROL_FEM( int side , int pointer , int mode , char *data ) {
	int Res1 , Res2;
	if ( DLL_USER_SCH_USE != 1 ) return SYS_RUNNING;
	if ( DRV_SCH_INF_USER_CONTROL_FEM[0] ) {
		Res1 = ( (*DRV_SCH_INF_USER_CONTROL_FEM[0] ) ( side , pointer , mode , data ) );
		if ( Res1 == 0 ) return Res1;
		if ( Res1 == SYS_ABORTED ) return Res1;
	}
	else {
		Res1 = SYS_RUNNING;
	}
	if ( DRV_SCH_INF_USER_CONTROL_FEM[1] ) {
		Res2 = ( (*DRV_SCH_INF_USER_CONTROL_FEM[1] ) ( side , pointer , mode , data ) );
		if ( Res2 == 0 ) return Res2;
		if ( Res2 == SYS_ABORTED ) return Res2;
	}
	else {
		Res2 = SYS_RUNNING;
	}
	if ( Res1 != SYS_RUNNING ) return Res1;
	if ( Res2 != SYS_RUNNING ) return Res2;
	return SYS_RUNNING;
}

int USER_DLL_SCH_INF_USER_CONTROL_TM( int side , int pointer , int mode , char *data ) {
	int Res1 , Res2;
	if ( DLL_USER_SCH_USE != 1 ) return SYS_RUNNING;
	if ( DRV_SCH_INF_USER_CONTROL_TM[0] ) {
		Res1 = ( (*DRV_SCH_INF_USER_CONTROL_TM[0] ) ( side , pointer , mode , data ) );
		if ( Res1 == 0 ) return Res1;
		if ( Res1 == SYS_ABORTED ) return Res1;
	}
	else {
		Res1 = SYS_RUNNING;
	}
	if ( DRV_SCH_INF_USER_CONTROL_TM[1] ) {
		Res2 = ( (*DRV_SCH_INF_USER_CONTROL_TM[1] ) ( side , pointer , mode , data ) );
		if ( Res2 == 0 ) return Res2;
		if ( Res2 == SYS_ABORTED ) return Res2;
	}
	else {
		Res2 = SYS_RUNNING;
	}
	if ( Res1 != SYS_RUNNING ) return Res1;
	if ( Res2 != SYS_RUNNING ) return Res2;
	return SYS_RUNNING;
}


//===========================================================================================================================
//===========================================================================================================================
//===========================================================================================================================


BOOL _SCH_INF_DLL_CONTROL_FA_SEND_MESSAGE( int mode , int side , int pt  , char *orgmsg , char *newmessageBuff , int newmessageBuffSize ) { // 2018.06.14
	BOOL Res1 , Res2;
	//
	/*
	// mode
		0	외부 gateway
		1	내부 Message				Side,pt
		2	Accept Message				Type
		3	Result Message				Side,Type
		4	Reject Message				Side,Type
		5	내부 Message(Substrate)		Side,pt

		11	Processjob Reject
		12	Processjob Accept
		13	Processjob Notify
		21	Controljob Reject
		22	Controljob Accept
		23	Controljob Notify
		24	Controljob Verify

		31	Step Change

		99	Info
	*/

	if ( DRV_SCH_INF_FA_SEND_MESSAGE[0] ) {
		Res1 = ( (* DRV_SCH_INF_FA_SEND_MESSAGE[0] ) ( mode , side , pt , orgmsg , newmessageBuff , newmessageBuffSize ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_FA_SEND_MESSAGE[1] ) {
		Res2 = ( (* DRV_SCH_INF_FA_SEND_MESSAGE[1] ) ( mode , side , pt , orgmsg , newmessageBuff , newmessageBuffSize ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return Res1;
	if ( Res2 ) return Res2;
	return FALSE;
}



BOOL _SCH_INF_DLL_CONTROL_FA_ORDER_MESSAGE( int mode , int side , int pt ) { // 2018.06.29
	BOOL Res1 , Res2;
	//
	if ( DRV_SCH_INF_FA_ORDER_MESSAGE[0] ) {
		Res1 = ( (* DRV_SCH_INF_FA_ORDER_MESSAGE[0] ) ( mode , side , pt ) );
	}
	else {
		Res1 = FALSE;
	}
	if ( DRV_SCH_INF_FA_ORDER_MESSAGE[1] ) {
		Res2 = ( (* DRV_SCH_INF_FA_ORDER_MESSAGE[1] ) ( mode , side , pt ) );
	}
	else {
		Res2 = FALSE;
	}
	if ( Res1 ) return Res1;
	if ( Res2 ) return Res2;
	return FALSE;
}



// 2012.09.19

#define MAKE_MACRO_PRE_CONTROL( ZZZ ) int USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_##ZZZ##() { \
	if ( DLL_USER_SCH_USE != 1 ) return FALSE; \
	if ( DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[0] ) return TRUE; \
	if ( DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[1] ) return TRUE; \
	return FALSE; \
} \
int USER_DLL_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##( int side , int mode , int etc ) { \
	int Res1 , Res2; \
	if ( DLL_USER_SCH_USE != 1 ) return SYS_RUNNING; \
	if ( DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[0] ) { \
		Res1 = ( (*DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[0] ) ( side , mode , etc ) ); \
	} \
	else { \
		Res1 = SYS_RUNNING; \
	} \
	if ( DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[1] ) { \
		Res2 = ( (*DRV_SCH_INF_RESTART_PRE_CONTROL_##ZZZ##[1] ) ( side , mode , etc ) ); \
	} \
	else { \
		Res2 = SYS_RUNNING; \
	} \
	if ( Res1 != SYS_RUNNING ) return Res1; \
	if ( Res2 != SYS_RUNNING ) return Res2; \
	return SYS_RUNNING; \
}

MAKE_MACRO_PRE_CONTROL( FEM )
MAKE_MACRO_PRE_CONTROL( TM_1 )
MAKE_MACRO_PRE_CONTROL( TM_2 )
MAKE_MACRO_PRE_CONTROL( TM_3 )
MAKE_MACRO_PRE_CONTROL( TM_4 )
MAKE_MACRO_PRE_CONTROL( TM_5 )
MAKE_MACRO_PRE_CONTROL( TM_6 )
MAKE_MACRO_PRE_CONTROL( TM_7 )
MAKE_MACRO_PRE_CONTROL( TM_8 )

//===========================================================================================================================
//===========================================================================================================================
//===========================================================================================================================


#define MAKE_MACRO_ENTER_CONTROL( ZZZ ) int USER_DLL_SCH_INF_ENTER_CONTROL_##ZZZ##( int side , int mode ) { \
	int Res1 , Res2; \
	if ( DLL_USER_SCH_USE != 1 ) return -1; \
	if ( DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[0] && DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[1] ) { \
\
		Res1 = ( (*DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[0] ) ( side , mode ) ); \
		if ( Res1 == 0 ) return Res1; \
		if ( Res1 == SYS_ABORTED ) return Res1; \
		if ( Res1 == SYS_ERROR ) return Res1; \
\
		Res2 = ( (*DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[1] ) ( side , mode ) ); \
		if ( Res2 == 0 ) return Res2; \
		if ( Res2 == SYS_ABORTED ) return Res2; \
		if ( Res2 == SYS_ERROR ) return Res2; \
\
		if ( Res1 == SYS_SUCCESS ) return Res2; \
		if ( Res2 == SYS_SUCCESS ) return Res1; \
\
		if ( Res1 != -1 ) return Res1; \
		if ( Res2 != -1 ) return Res2; \
	} \
	else { \
		if ( DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[0] ) { \
			return ( (*DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[0] ) ( side , mode ) ); \
		} \
		if ( DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[1] ) { \
			return ( (*DRV_SCH_INF_ENTER_CONTROL_##ZZZ##[1] ) ( side , mode ) ); \
		} \
	} \
	return -1; \
}

#define MAKE_MACRO_RUN_CONTROL( ZZZ ) int USER_DLL_SCH_INF_RUN_CONTROL_##ZZZ##( int side ) { \
	int Res1 , Res2; \
	if ( DLL_USER_SCH_USE != 1 ) return SYS_RUNNING; \
	if ( DRV_SCH_INF_RUN_CONTROL_##ZZZ##[0] && DRV_SCH_INF_RUN_CONTROL_##ZZZ##[1] ) { \
\
		Res1 = ( (*DRV_SCH_INF_RUN_CONTROL_##ZZZ##[0] ) ( side ) ); \
		if ( Res1 == 0 ) return Res1; \
		if ( Res1 == SYS_ABORTED ) return Res1; \
		if ( Res1 == SYS_ERROR ) return Res1; \
\
		Res2 = ( (*DRV_SCH_INF_RUN_CONTROL_##ZZZ##[1] ) ( side ) ); \
		if ( Res2 == 0 ) return Res2; \
		if ( Res2 == SYS_ABORTED ) return Res2; \
		if ( Res2 == SYS_ERROR ) return Res2; \
\
		if ( Res1 == SYS_SUCCESS ) return Res2; \
		if ( Res2 == SYS_SUCCESS ) return Res1; \
\
		if ( Res1 != SYS_RUNNING ) return Res1; \
		if ( Res2 != SYS_RUNNING ) return Res2; \
\
	} \
	else { \
		if ( DRV_SCH_INF_RUN_CONTROL_##ZZZ##[0] ) { \
			return ( (*DRV_SCH_INF_RUN_CONTROL_##ZZZ##[0] ) ( side ) ); \
		} \
		if ( DRV_SCH_INF_RUN_CONTROL_##ZZZ##[1] ) { \
			return ( (*DRV_SCH_INF_RUN_CONTROL_##ZZZ##[1] ) ( side ) ); \
		} \
	} \
	return SYS_RUNNING; \
}



MAKE_MACRO_ENTER_CONTROL( FEM )
MAKE_MACRO_ENTER_CONTROL( TM )

MAKE_MACRO_RUN_CONTROL( FEM )
MAKE_MACRO_RUN_CONTROL( TM_1 )
MAKE_MACRO_RUN_CONTROL( TM_2 )
MAKE_MACRO_RUN_CONTROL( TM_3 )
MAKE_MACRO_RUN_CONTROL( TM_4 )
MAKE_MACRO_RUN_CONTROL( TM_5 )
MAKE_MACRO_RUN_CONTROL( TM_6 )
MAKE_MACRO_RUN_CONTROL( TM_7 )
MAKE_MACRO_RUN_CONTROL( TM_8 )


