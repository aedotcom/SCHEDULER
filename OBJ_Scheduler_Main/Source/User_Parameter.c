#include "default.h"

#include "EQ_Enum.h"

#include "system_tag.h"
#include "File_ReadInit.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "sch_processmonitor.h"
#include "system_tag.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
#include "MultiJob.h"
#include "MultiReg.h"
#include "Robot_Handling.h"
#include "resource.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Module Fixed Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MODE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MODE_for_SW , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_SERVICE_MODE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MODULE_GROUP , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_GROUP_HAS_SWITCH , MAX_TM_CHAMBER_DEPTH );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_STOCK , MAX_CHAMBER ); // 2006.01.05
//
MAKE_DEFAULT_INT_PARAMETER3( MODULE_PICK_GROUP , MAX_TM_CHAMBER_DEPTH + 2 , MAX_CHAMBER ); // 2006.01.04
MAKE_DEFAULT_INT_PARAMETER3( MODULE_PLACE_GROUP , MAX_TM_CHAMBER_DEPTH + 2 , MAX_CHAMBER ); // 2006.01.04
MAKE_DEFAULT_INT_PARAMETER2( MODULE_TMS_GROUP , MAX_CHAMBER ); // 2013.03.19

MAKE_DEFAULT_INT_PARAMETER2( MODULE_INTERFACE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MODULE_SIZE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MODULE_SIZE_CHANGE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER3( MODULE_BUFFER_MODE , MAX_TM_CHAMBER_DEPTH , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_BUFFER_FULLRUN , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MODULE_BUFFER_LASTORDER , MAX_CHAMBER ); // 2003.06.02

//MAKE_DEFAULT_INT_PARAMETER( MODULE_BUFFER_SWITCH_SWAPPING , BUFFER_SWITCH_BATCH_ALL ); // 2013.03.18
MAKE_DEFAULT_INT_PARAMETER2( MODULE_BUFFER_M_SWITCH_SWAPPING , MAX_TM_CHAMBER_DEPTH ); // 2013.03.18

MAKE_DEFAULT_INT_PARAMETER2( MODULE_DOUBLE_WHAT_SIDE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER( MODULE_DUMMY_BUFFER_CHAMBER , 0 );

MAKE_DEFAULT_INT_PARAMETER( MODULE_ALIGN_BUFFER_CHAMBER , 0 );

MAKE_DEFAULT_CHAR_PARAMETER2( MODULE_PROCESS_NAME , MAX_CHAMBER );

MAKE_DEFAULT_CHAR_PARAMETER2( MODULE_COMMSTATUS_IO_NAME , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_PREPRECV , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_PREPSEND , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_POSTRECV , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_POSTSEND , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_GATE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_MESSAGE_NOTUSE_CANCEL , MAX_CHAMBER );	// 2016.11.24

MAKE_DEFAULT_INT_PARAMETER( MODULE_COMMSTATUS_MODE , 0 ); // 2003.06.11
MAKE_DEFAULT_INT_PARAMETER( MODULE_COMMSTATUS2_MODE , 0 ); // 2012.07.21
MAKE_DEFAULT_INT_PARAMETER( MODULE_TRANSFER_CONTROL_MODE , 0 ); // 2003.06.25

MAKE_DEFAULT_INT_PARAMETER( MULTI_GROUP_AUTO_ARRANGE , 0 ); // 2006.02.08
MAKE_DEFAULT_INT_PARAMETER( PRCS_ZONE_RUN_TYPE , 0 ); // 2006.05.04

MAKE_DEFAULT_INT_PARAMETER( LOT_END_ESTIMATION , 0 ); // 2006.08.11

MAKE_DEFAULT_INT_PARAMETER( PICKPLACE_CANCEL_MESSAGE , 0 ); // 2008.02.01

MAKE_DEFAULT_INT_PARAMETER( TIMER_RESET_WHEN_NEW_START , 0 ); // 2008.04.17

MAKE_DEFAULT_INT_PARAMETER( CHECK_DIFFERENT_METHOD , 0 ); // 2008.07.09

MAKE_DEFAULT_INT_PARAMETER( CHECK_DIFFERENT_METHOD2 , 0 ); // 2018.08.22

MAKE_DEFAULT_INT_PARAMETER( LOG_ENCRIPTION , 0 ); // 2010.04.14

MAKE_DEFAULT_INT_PARAMETER( PROCESS_MESSAGE_STYLE , 0 ); // 2010.09.02

MAKE_DEFAULT_INT_PARAMETER( WAFER_ANIM_STATUS_CHECK , 0 ); // 2010.09.13

MAKE_DEFAULT_INT_PARAMETER( MAINT_MESSAGE_STYLE , 0 ); // 2010.09.21

MAKE_DEFAULT_INT_PARAMETER( EIL_INTERFACE , 0 ); // 2010.09.13
MAKE_DEFAULT_CHAR_PARAMETER( EIL_INTERFACE_JOBLOG ); // 2010.09.13
MAKE_DEFAULT_INT_PARAMETER2( EIL_INTERFACE_FLOW_USER_OPTION , 256 ) // 2012.09.03

MAKE_DEFAULT_INT_PARAMETER2( MFI_INTERFACE_FLOW_USER_OPTION , 256 ); // 2012.12.05
MAKE_DEFAULT_INT_PARAMETER2( MFI_INTERFACE_FLOW_USER_OPTIONF , 256 ); // 2013.08.01
MAKE_DEFAULT_CHAR_PARAMETER2( MFI_INTERFACE_FLOW_USER_OPTIONS , 256 ); // 2013.08.01

MAKE_DEFAULT_INT_PARAMETER( DUMMY_MESSAGE , 0 ); // 2010.11.30

MAKE_DEFAULT_INT_PARAMETER( MODULE_PICK_ORDERING_USE , 0 ); // 2010.10.15
MAKE_DEFAULT_INT_PARAMETER2( MODULE_PICK_ORDERING , MAX_TM_CHAMBER_DEPTH ) // 2010.10.15
MAKE_DEFAULT_INT_PARAMETER2( MODULE_PICK_ORDERING_SKIP , MAX_CHAMBER ) // 2012.08.23

MAKE_DEFAULT_INT_PARAMETER( MTLOUT_INTERFACE , 0 ); // 2011.05.20

MAKE_DEFAULT_INT_PARAMETER( MESSAGE_MAINTINTERFACE , 0 ); // 2013.04.25

MAKE_DEFAULT_INT_PARAMETER( USE_EXTEND_OPTION , 0 ) // 2013.06.20

MAKE_DEFAULT_INT_PARAMETER( OPTIMIZE_MODE , 0 ) // 2017.12.07
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Side Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( MODULE_SWITCH_BUFFER , MAX_CASSETTE_SIDE );
MAKE_DEFAULT_INT_PARAMETER2( MODULE_SWITCH_BUFFER_ACCESS_TYPE , MAX_CASSETTE_SIDE );

MAKE_DEFAULT_INT_PARAMETER( BATCH_SUPPLY_INTERRUPT , 0 ); // 2006.09.05

MAKE_DEFAULT_INT_PARAMETER( DIFF_LOT_NOTSUP_MODE , 0 ); // 2007.07.05

MAKE_DEFAULT_INT_PARAMETER( METHOD1_TO_1BM_USING_MODE , 0 ); // 2018.03.21

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Module Factor Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( Getting_Priority , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( Putting_Priority , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( PRCS_TIME_WAIT_RANGE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( PRCS_TIME_REMAIN_RANGE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( STATION_PICK_HANDLING , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( STATION_PLACE_HANDLING , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( ANIMATION_ROTATE_PREPARE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( Process_Run_In_Mode , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( Process_Run_In_Time , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( Process_Run_Out_Mode , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( Process_Run_Out_Time , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MAPPING_SKIP , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( AUTO_HANDOFF , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( PRE_RECEIVE_WITH_PROCESS_RECIPE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( NEXT_FREE_PICK_POSSIBLE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( PREV_FREE_PICK_POSSIBLE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( ERROR_OUT_CHAMBER_FOR_CASSETTE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK , MAX_CHAMBER );

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Offset Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER3( OFFSET_SLOT_FROM_CM , MAX_CASSETTE_SIDE+1 , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( OFFSET_SLOT_FROM_ALL , MAX_CHAMBER );

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Interlock Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER3( INTERLOCK_PM_RUN_FOR_CM , MAX_CASSETTE_SIDE+1 , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER3( INTERLOCK_TM_RUN_FOR_ALL , MAX_TM_CHAMBER_DEPTH , MAX_CHAMBER );

//MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_PM_PICK_DENY_FOR_MDL , MAX_CHAMBER ); // 2013.05.28
//MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_PM_PLACE_DENY_FOR_MDL , MAX_CHAMBER ); // 2013.05.28

MAKE_DEFAULT_INT_PARAMETER3( INTERLOCK_PM_M_PICK_DENY_FOR_MDL , MAX_TM_CHAMBER_DEPTH , MAX_CHAMBER ); // 2013.05.28
MAKE_DEFAULT_INT_PARAMETER3( INTERLOCK_PM_M_PLACE_DENY_FOR_MDL , MAX_TM_CHAMBER_DEPTH , MAX_CHAMBER ); // 2013.05.28

MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_FM_PICK_DENY_FOR_MDL , MAX_CHAMBER ); // 2006.09.26
MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_FM_PLACE_DENY_FOR_MDL , MAX_CHAMBER ); // 2006.09.26
MAKE_DEFAULT_INT_PARAMETER3( INTERLOCK_FM_ROBOT_FINGER , MAX_FINGER_FM , 2 );
//
MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM , MAX_CHAMBER ); // 2004.01.30 // None Pick Place

MAKE_DEFAULT_INT_PARAMETER2( INTERLOCK_TM_SINGLE_RUN , MAX_TM_CHAMBER_DEPTH ); // 2007.01.15

MAKE_DEFAULT_INT_PARAMETER( INTERLOCK_FM_BM_PLACE_SEPARATE , FALSE ); // 2007.11.15

MAKE_DEFAULT_INT_PARAMETER( INTERLOCK_TM_BM_OTHERGROUP_SWAP , FALSE ); // 2013.04.25

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Dummy Process Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER( DUMMY_PROCESS_CHAMBER , 0 );
MAKE_DEFAULT_INT_PARAMETER( DUMMY_PROCESS_SLOT , 1 );
MAKE_DEFAULT_INT_PARAMETER( DUMMY_PROCESS_MODE , 0 );
MAKE_DEFAULT_INT_PARAMETER( DUMMY_PROCESS_FIXEDCLUSTER , 0 );
MAKE_DEFAULT_INT_PARAMETER2( DUMMY_PROCESS_NOT_DEPAND_CHAMBER , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER( DUMMY_PROCESS_MULTI_LOT_ACCESS , 0 );

MAKE_DEFAULT_INT_PARAMETER2( SDUMMY_PROCESS_CHAMBER , MAX_SDUMMY_DEPTH );
MAKE_DEFAULT_INT_PARAMETER2( SDUMMY_PROCESS_SLOT , MAX_SDUMMY_DEPTH );

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Fixed Cluster Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_PRE_MODE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_PRE_IN_USE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_PRE_OUT_USE , MAX_CHAMBER );
MAKE_DEFAULT_CHAR_PARAMETER2( FIXCLUSTER_PRE_IN_RECIPE_NAME , MAX_CHAMBER );
MAKE_DEFAULT_CHAR_PARAMETER2( FIXCLUSTER_PRE_OUT_RECIPE_NAME , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_POST_MODE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_POST_IN_USE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FIXCLUSTER_POST_OUT_USE , MAX_CHAMBER );
MAKE_DEFAULT_CHAR_PARAMETER2( FIXCLUSTER_POST_IN_RECIPE_NAME , MAX_CHAMBER );
MAKE_DEFAULT_CHAR_PARAMETER2( FIXCLUSTER_POST_OUT_RECIPE_NAME , MAX_CHAMBER );

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Pre/Post Run Recipe Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_CHAR_PARAMETER2( READY_RECIPE_NAME , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( READY_RECIPE_USE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( READY_RECIPE_MINTIME , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( READY_RECIPE_TYPE , MAX_CHAMBER ); // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2( END_RECIPE_NAME , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( END_RECIPE_USE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( END_RECIPE_MINTIME , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER( READY_MULTIJOB_MODE , 0 );

//
// 2016.12.09
MAKE_DEFAULT_INT_PARAMETER2( SIM_TIME1 , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( SIM_TIME2 , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( SIM_TIME3 , MAX_CHAMBER );
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Fail Scenario Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( FAIL_PROCESS_SCENARIO , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( STOP_PROCESS_SCENARIO , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MRES_FAIL_SCENARIO , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MRES_ABORT_SCENARIO , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( MRES_SUCCESS_SCENARIO , MAX_CHAMBER ); // 2003.09.25

MAKE_DEFAULT_INT_PARAMETER( MRES_ABORT_ALL_SCENARIO , 0 );

MAKE_DEFAULT_INT_PARAMETER( DISABLE_MAKE_HOLE_GOTOSTOP , FALSE );

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Clear Scenario Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( CLSOUT_USE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( CLSOUT_DELAY , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( CLSOUT_MESSAGE_USE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER( CLSOUT_AL_TO_PM , 0 ) // 2005.12.16
MAKE_DEFAULT_INT_PARAMETER( CLSOUT_IC_TO_PM , 0 ) // 2005.12.16

MAKE_DEFAULT_INT_PARAMETER( CLSOUT_BM_SINGLE_ONLY , 0 )

MAKE_DEFAULT_INT_PARAMETER( MAINTINF_DB_USE , FALSE ) // 2012.05.04
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Robot Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER2( RB_ROBOT_ANIMATION , MAX_TM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER2( RB_FINGER_ARM_SEPERATE , MAX_TM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER3( RB_FINGER_ARM_STYLE , MAX_TM_CHAMBER_DEPTH , MAX_FINGER_TM );
MAKE_DEFAULT_INT_PARAMETER3( RB_FINGER_ARM_DISPLAY , MAX_TM_CHAMBER_DEPTH , MAX_FINGER_TM );

MAKE_DEFAULT_INT_PARAMETER2( RB_SYNCH_CHECK , MAX_TM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER2( RB_ROTATION_STYLE , MAX_TM_CHAMBER_DEPTH );

MAKE_DEFAULT_DOUBLE_PARAMETER2( RB_HOME_POSITION , MAX_TM_CHAMBER_DEPTH );

MAKE_DEFAULT_DOUBLE_PARAMETER5( RB_POSITION , MAX_TM_CHAMBER_DEPTH , MAX_FINGER_TM , MAX_CHAMBER , 6 );

MAKE_DEFAULT_DOUBLE_PARAMETER4( RB_RNG , MAX_TM_CHAMBER_DEPTH , MAX_FINGER_TM , 6 );

//

MAKE_DEFAULT_INT_PARAMETER2( RB_ROBOT_FM_ANIMATION , MAX_FM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER2( RB_FM_FINGER_ARM_SEPERATE , MAX_FM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER3( RB_FM_FINGER_ARM_STYLE , MAX_FM_CHAMBER_DEPTH , MAX_FINGER_FM );
MAKE_DEFAULT_INT_PARAMETER3( RB_FM_FINGER_ARM_DISPLAY , MAX_FM_CHAMBER_DEPTH , MAX_FINGER_FM );

MAKE_DEFAULT_INT_PARAMETER2( RB_FM_SYNCH_CHECK , MAX_FM_CHAMBER_DEPTH );

MAKE_DEFAULT_INT_PARAMETER2( RB_FM_ROTATION_STYLE , MAX_FM_CHAMBER_DEPTH );

MAKE_DEFAULT_DOUBLE_PARAMETER2( RB_FM_HOME_POSITION , MAX_FM_CHAMBER_DEPTH );

MAKE_DEFAULT_DOUBLE_PARAMETER5( RB_FM_POSITION , MAX_FM_CHAMBER_DEPTH , MAX_CHAMBER , MAX_CASSETTE_SLOT_SIZE + 1 , 6 );

MAKE_DEFAULT_DOUBLE_PARAMETER3( RB_FM_RNG , MAX_FM_CHAMBER_DEPTH , 6 );

MAKE_DEFAULT_INT_PARAMETER2( RB_FM_ARM_IS_DUAL_EXTEND_STYLE , MAX_FM_CHAMBER_DEPTH ) // 2003.01.15

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// FA Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER( FA_STARTEND_STATUS_SHOW , 0 );

MAKE_DEFAULT_INT_PARAMETER( FA_SINGLE_CASS_MULTI_RUN , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_LOADUNLOAD_SKIP , 0 );
MAKE_DEFAULT_INT_PARAMETER( FA_MAPPING_SKIP , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_NORMALUNLOAD_NOTUSE , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_LOAD_REQUEST , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_LOAD_COMPLETE , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_LOAD_REJECT , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_UNLOAD_REJECT , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_MAPPING_REQUEST , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_SYSTEM_MSGSKIP_MAPPING_REJECT , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_WINDOW_NORMAL_CHECK_SKIP , FALSE );
MAKE_DEFAULT_INT_PARAMETER( FA_WINDOW_ABORT_CHECK_SKIP , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_PROCESS_STEPCHANGE_CHECK_SKIP , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_EXPAND_MESSAGE_USE , FALSE );

MAKE_DEFAULT_INT_PARAMETER( FA_SEND_MESSAGE_DISPLAY , FALSE );

MAKE_DEFAULT_INT_PARAMETER2( FA_EXTEND_HANDOFF , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FA_MID_READ_POINT , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FA_JID_READ_POINT , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( FA_LOAD_COMPLETE_MESSAGE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FA_UNLOAD_COMPLETE_MESSAGE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( FA_MAPPING_AFTERLOAD , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER( FA_SUBSTRATE_WAFER_ID , 0 );

MAKE_DEFAULT_INT_PARAMETER( FA_REJECTMESSAGE_DISPLAY , 0 ); // 2004.06.16

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Fixed Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_CHAR_PARAMETER( DFIX_METHOD_NAME )

MAKE_DEFAULT_CHAR_PARAMETER( DFIX_SYSTEM_PATH )
MAKE_DEFAULT_CHAR_PARAMETER( DFIX_LOG_PATH )

MAKE_DEFAULT_CHAR_PARAMETER( DFIX_TMP_PATH ) // 2017.10.30

// 2012.08.22
//MAKE_DEFAULT_CHAR_PARAMETER( DFIX_MAIN_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER( DFIX_MAIN_RECIPE_PATHF )
MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_MAIN_RECIPE_PATH , MAX_CHAMBER )
MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_MAIN_RECIPE_PATHM , MAX_CHAMBER )

MAKE_DEFAULT_CHAR_PARAMETER( DFIX_LOT_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER( DFIX_CLUSTER_RECIPE_PATH )
MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_GROUP_RECIPE_PATH , MAX_CASSETTE_SIDE );

MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_PROCESS_RECIPE_PATHF , MAX_CHAMBER ); // 2012.08.22
MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_PROCESS_RECIPE_PATH , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER2( DFIX_PROCESS_RECIPE_TYPE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER2( DFIX_PROCESS_RECIPE_FILE , MAX_CHAMBER );

MAKE_DEFAULT_CHAR_PARAMETER2( DFIX_PROCESS_LOG_FUNCTION_NAME , MAX_CHAMBER );
MAKE_DEFAULT_CHAR_PARAMETER3( DFIX_PROCESS_FUNCTION_NAME , MAX_CASSETTE_SIDE , MAX_CHAMBER );

MAKE_DEFAULT_INT_PARAMETER( DFIX_RECIPE_LOCKING_MODE , FALSE )
MAKE_DEFAULT_INT_PARAMETER( DFIX_RECIPE_LOCKING , 0 )
MAKE_DEFAULT_INT_PARAMETER2( DFIX_RECIPE_LOCKING_SKIP , MAX_CASSETTE_SIDE ); // 2014.02.08

MAKE_DEFAULT_INT_PARAMETER( DFIX_MODULE_SW_CONTROL , FALSE )

MAKE_DEFAULT_INT_PARAMETER( DFIX_CASSETTE_READ_MODE , 0 ) // 2006.02.16

MAKE_DEFAULT_INT_PARAMETER( DFIX_CASSETTE_ABSENT_RUN_DATA , 0 ) // 2010.03.08

MAKE_DEFAULT_INT_PARAMETER( DFIX_MAX_CASSETTE_SLOT , 25 )

MAKE_DEFAULT_INT_PARAMETER( DFIX_IOINTLKS_USE , FALSE ) // 2007.01.23

MAKE_DEFAULT_INT_PARAMETER( DFIX_CHAMBER_INTLKS_IOUSE , FALSE ) // 2013.11.21

MAKE_DEFAULT_INT_PARAMETER( DFIX_MAX_FINGER_TM , 0 )

MAKE_DEFAULT_INT_PARAMETER2( DFIX_TM_M_DOUBLE_CONTROL , MAX_TM_CHAMBER_DEPTH ); // 2013.02.06

//MAKE_DEFAULT_INT_PARAMETER( DFIX_TM_DOUBLE_CONTROL , 0 ) // 2013.02.06

MAKE_DEFAULT_INT_PARAMETER( DFIX_FM_DOUBLE_CONTROL , 0 ) // 2007.06.21

MAKE_DEFAULT_INT_PARAMETER( DFIX_FIC_MULTI_CONTROL , 0 )
MAKE_DEFAULT_INT_PARAMETER( DFIX_FAL_MULTI_CONTROL , FALSE )

MAKE_DEFAULT_CHAR_PARAMETER( DFIX_FAL_MULTI_FNAME )
MAKE_DEFAULT_CHAR_PARAMETER( DFIX_FIC_MULTI_FNAME )

MAKE_DEFAULT_CHAR_PARAMETER( DFIX_CARR_AUTO_HANDLER )

MAKE_DEFAULT_INT_PARAMETER( DFIX_CONTROL_RECIPE_TYPE , 0 )
MAKE_DEFAULT_INT_PARAMETER( DFIX_CONTROL_FIXED_CASSETTE_IN , -1 )
MAKE_DEFAULT_INT_PARAMETER( DFIX_CONTROL_FIXED_CASSETTE_OUT , -1 )
MAKE_DEFAULT_INT_PARAMETER( DFIX_CONTROL_FIXED_SLOT_START , -1 )
MAKE_DEFAULT_INT_PARAMETER( DFIX_CONTROL_FIXED_SLOT_END , -1 )

MAKE_DEFAULT_INT_PARAMETER( DFIX_LOT_LOG_MODE , 0 )

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Default Maked Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER( DFIM_MAX_MODULE_GROUP , 0 )

MAKE_DEFAULT_INT_PARAMETER( DFIM_MAX_PM_COUNT , 1 )

MAKE_DEFAULT_INT_PARAMETER3( DFIM_SLOT_NOTUSE , MAX_CHAMBER , MAX_CASSETTE_SLOT_SIZE ); // 2016.11.14

MAKE_DEFAULT_INT_PARAMETER3( DFIM_SLOT_NOTUSE_DATA , MAX_CHAMBER , MAX_CASSETTE_SLOT_SIZE ); // 2018.12.06

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// System Utility Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
MAKE_DEFAULT_INT_PARAMETER( UTIL_CHECK_WAFER_SYNCH , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_MAPPING_WHEN_TMFREE , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_TM_GATE_SKIP_CONTROL , FALSE )

MAKE_DEFAULT_INT_PARAMETER( UTIL_TM_MOVE_SR_CONTROL , FALSE )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FIRST_MODULE_PUT_DELAY_TIME , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_SCH_START_OPTION , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FM_MOVE_SR_CONTROL , FALSE )

MAKE_DEFAULT_INT_PARAMETER( UTIL_LOOP_MAP_ALWAYS , FALSE )

MAKE_DEFAULT_INT_PARAMETER( UTIL_SCH_LOG_DIRECTORY_FORMAT , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN , FALSE )

MAKE_DEFAULT_INT_PARAMETER( UTIL_ABORT_MESSAGE_USE_POINT , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_CANCEL_MESSAGE_USE_POINT , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_END_MESSAGE_SEND_POINT , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_CASSETTE_SUPPLY_DEVIATION_TIME , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_CHAMBER_PUT_PR_CHECK , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_CHAMBER_GET_PR_CHECK , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FIC_MULTI_WAITTIME , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FIC_MULTI_WAITTIME2 , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FIC_MULTI_TIMEMODE , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_FAL_MULTI_WAITTIME , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_MESSAGE_USE_WHEN_SWITCH , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_CM_SUPPLY_MODE , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_PREPOST_PROCESS_DEPAND , 0 )

MAKE_DEFAULT_INT_PARAMETER( UTIL_START_PARALLEL_CHECK_SKIP , 0 ) // 2003.01.11

MAKE_DEFAULT_INT_PARAMETER( UTIL_WAFER_SUPPLY_MODE , 0 ) // 2003.06.11

MAKE_DEFAULT_INT_PARAMETER( UTIL_CLUSTER_INCLUDE_FOR_DISABLE , 0 ) // 2003.10.09

MAKE_DEFAULT_INT_PARAMETER( UTIL_BMEND_SKIP_WHEN_RUNNING , 0 ) // 2004.02.19

MAKE_DEFAULT_INT_PARAMETER( UTIL_PMREADY_SKIP_WHEN_RUNNING , 0 ) // 2006.03.28

MAKE_DEFAULT_INT_PARAMETER( UTIL_LOT_LOG_PRE_POST_PROCESS , 0 ) // 2004.05.11

MAKE_DEFAULT_INT_PARAMETER( UTIL_MESSAGE_USE_WHEN_ORDER , 0 ) // 2004.06.24

MAKE_DEFAULT_INT_PARAMETER( UTIL_SW_BM_SCHEDULING_FACTOR , 0 ) // 2004.08.14

MAKE_DEFAULT_INT_PARAMETER( UTIL_ADAPTIVE_PROGRESSING , 0 ) // 2008.02.29

MAKE_DEFAULT_INT_PARAMETER( SYSTEM_LOG_STYLE , 0 ) // 2006.02.07

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Multiple Check Parameter
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------

int Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( int Chamber , BOOL pmmode ) {
	switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
	case 0 :
		return FALSE;
	default :
		if ( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) ) {
			switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) ) {
			case 0 :
				return FALSE;
			case 1 : // Enable
				if ( _i_SCH_MODULE_GET_MOVE_MODE(Chamber) == 1 ) return FALSE;
				if ( pmmode ) return FALSE;
				return TRUE;
			case 2 :
				return FALSE;
			case 3 : // EnablePM
				if ( _i_SCH_MODULE_GET_MOVE_MODE(Chamber) == 1 ) return FALSE;
				if ( pmmode ) return TRUE;
				return FALSE;
				break;
			default :
				return FALSE;
			}
		}
		return FALSE;
	}
	return TRUE;
}
//----------------------------------------------------------------------------
int Get_RunPrm_MODULE_GET_SINGLE_CHAMBER( int pmmode ) {
	/*
	// 2012.02.01
	int i , j = -1;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() == 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i+PM1 ) ) {
				if ( j != -1 ) return -1;
				j = i+PM1;
			}
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i+PM1 ) ) {
				switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i+PM1 ) ) {
				case 1 : // Enable
				case 3 : // EnablePM
					if ( j != -1 ) return -1;
					j = i+PM1;
					break;
				}
			}
		}
	}
	return j;
	*/
	// 2012.02.01
	int i , j;
	//
	j = -1;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i+PM1 ) ) {
			//
			if ( j != -1 ) break;
			j = i+PM1;
			//
		}
	}
	//
	if ( i == MAX_PM_CHAMBER_DEPTH ) return j;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i+PM1 ) ) {
			//
			if ( !pmmode ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i+PM1 ) == 1 ) return i + PM1; // Enable
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i+PM1 ) == 3 ) return i + PM1; // EnablePM
			}
		}
	}
	//
	return -1;
}
//----------------------------------------------------------------------------
BOOL Get_RunPrm_MODULE_HAS_WAFER() {
	int i , j;
	for ( i = PM1 ; i <= IC ; i++ ) {
		switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
		case 0 :
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) return TRUE;
			}
			break;
		default :
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
				case 1 :
				case 3 :
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) return TRUE;
					break;
				}
			}
			break;
		}
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
		case 0 :
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j+1 ) > 0 ) return TRUE;
				}
			}
			break;
		default :
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
				case 1 :
				case 3 :
					for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j+1 ) > 0 ) return TRUE;
					}
					break;
				}
			}
			break;
		}
	}
	return FALSE;
}
//----------------------------------------------------------------------------
int Get_RunPrm_MODULE_START_ENABLE( int Chamber , int pmmode ) {
	switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
	case 0 :
		return( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) );
		break;
	default :
		if ( _i_SCH_PRM_GET_MODULE_MODE( Chamber ) ) {
			switch ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) ) {
			case 0 : // Disable
				switch( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() ) {
				case 0 : // N/A
					return FALSE;
					break;
				case 1 : // Disable
				case 4 : // Disable
				case 7 : // Disable
				case 10 : // Disable
					return TRUE;
					break;
				case 2 : // DisableHW
				case 5 : // DisableHW
				case 8 : // DisableHW
				case 11 : // DisableHW
					return FALSE;
					break;
				case 3 : // Disable,DisableHW
				case 6 : // Disable,DisableHW
				case 9 : // Disable,DisableHW
				case 12 : // Disable,DisableHW
					return TRUE;
					break;
				default :
					return FALSE;
					break;
				}
				break;
			case 1 : // Enable
				if ( pmmode ) return FALSE; // 2005.07.01
				return TRUE;
				break;
			case 2 : // DisableHW
				switch( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() ) {
				case 0 : // N/A
					return FALSE;
					break;
				case 1 : // Disable
				case 4 : // Disable
				case 7 : // Disable
				case 10 : // Disable
					return FALSE;
					break;
				case 2 : // DisableHW
				case 5 : // DisableHW
				case 8 : // DisableHW
				case 11 : // DisableHW
					return TRUE;
					break;
				case 3 : // Disable,DisableHW
				case 6 : // Disable,DisableHW
				case 9 : // Disable,DisableHW
				case 12 : // Disable,DisableHW
					return TRUE;
					break;
				default :
					return FALSE;
					break;
				}
				break;
			case 3 : // EnablePM
				if ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() == 0 ) { // 2005.07.18
					if ( !pmmode ) return FALSE; // 2005.07.01
				}
				return TRUE;
				break;
			default :
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
		break;
	}
	return TRUE;
}


//==========================================================================================
void Set_RunPrm_Config_Change( int mode , int chamber , int data ) {
	switch( mode ) {
	case 0 : // MAPPING
		if ( data < 0 ) return;
		if ( data > 8 ) return;
		_i_SCH_PRM_SET_MAPPING_SKIP( chamber , data );
		break;
	case 1 : // HANDOFF
		if ( data < 0 ) return;
		if ( data > 5 ) return;
		_i_SCH_PRM_SET_AUTO_HANDOFF( chamber , data );
		break;
	case 2 : // RUNTIME_IN
		if ( data < 0    ) return;
		if ( data > 1000 ) return;
		_i_SCH_PRM_SET_Process_Run_In_Time( chamber , data );
		break;
	case 3 : // RUNTIME_OUT
		if ( data < 0    ) return;
		if ( data > 9999 ) return;
		_i_SCH_PRM_SET_Process_Run_Out_Time( chamber , data );
		break;
	case 4 : // DEFAULT_SIDE
		if ( data < 0 ) return;
		if ( data > 2 ) return;
		_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( chamber , data );
		break;
	case 5 : // START OPTION
		if ( data < 0 ) return;
		if ( data > 4 ) return;
		_i_SCH_PRM_SET_UTIL_SCH_START_OPTION( data );
		break;
	case 6 : // SWITCH OPTION
		if ( data < 0 ) return;
//		if ( data > 4 ) return; // 2003.11.28
		if ( data > 8 ) return; // 2003.11.28
		if ( ( chamber - 1 ) >= MAX_CASSETTE_SIDE ) return;
		if ( ( chamber - 1 ) < 0 ) return;
		_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER( chamber - 1 , data );
		break;
	case 7 : // CPJOB Log Based // 2002.12.30
		if ( data < 0 ) return;
//		if ( data > 1 ) return; // 2019.04.05
		if ( data > 3 ) return; // 2019.04.05
		PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( data );
		break;
	case 9 : // Arm Style // 2003.01.23
		/*
		//
		// 2018.12.07
		//------------------------------------------------------------------------------------------
		if ( data < 0 ) return;
		if ( data > 41 ) return;
		//------------------------------------------------------------------------------------------
		switch( data ) {
		//------------------------------------------------------------------------------------------
		case 0 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 1 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 3 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 1 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 4 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 2 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 5 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 3 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 6 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 4 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 7 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 5 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 8 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 6 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 9 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 7 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 10 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 8 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 11 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 9 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 33 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 1 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 34 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 2 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 35 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 3 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 36 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 4 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 37 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 5 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 38 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 6 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 39 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 7 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 40 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 8 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 41 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 9 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		}
		*/
		//
		// 2018.12.07
		//------------------------------------------------------------------------------------------
		if ( data < 0 ) return;
		if ( ( data % 100 ) > 41 ) return;
		//------------------------------------------------------------------------------------------
		switch( data / 100 ) {
		//------------------------------------------------------------------------------------------
		case 1 :
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( 0 , TRUE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( 0 , FALSE );
			break;
		}
		//------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------
		switch( data % 100 ) {
		//------------------------------------------------------------------------------------------
		case 0 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 1 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 0 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 3 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 1 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 4 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 2 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 5 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 3 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 6 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 4 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 7 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 5 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 8 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 6 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 9 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 7 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 10 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 8 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		case 11 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 9 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 33 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 1 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 34 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 2 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 35 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 3 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 36 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 4 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 37 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 5 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 38 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 6 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 39 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 7 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 40 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 8 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		case 41 :
			_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 , 9 );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( 0 , 1 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		}
		//
		//
		break;
	case 10 : // Carrier Group // 2003.02.05
		if ( data < 0 ) return;
		if ( data > 11 ) return;
		_i_SCH_PRM_SET_UTIL_START_PARALLEL_CHECK_SKIP( data ); // 2003.02.05
		break;
	case 11 : // Dummy Multi // 2003.02.14
		if ( data < 0 ) return;
		if ( data > 2 ) return;
		_i_SCH_PRM_SET_DUMMY_PROCESS_MULTI_LOT_ACCESS( data ); // 2003.02.14
		break;
	case 12 : // Wafer Supply // 2003.06.12
		if ( data < 0 ) return;
		if ( data > 7 ) return;
		_i_SCH_PRM_SET_UTIL_WAFER_SUPPLY_MODE( data ); // 2003.06.12
		break;
	case 13 : // Cluster Include // 2005.11.29
		if ( data < 0 ) return;
		if ( data > 12 ) return;
		_i_SCH_PRM_SET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE( data ); // 2003.06.12
		break;
	case 14 : // Cooling Time // 2005.11.29
//		if ( data < 0 ) return; // 2007.09.18
		if ( data < -20 ) return; // 2007.09.18
		_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME( data );
		break;
	case 15 : // Cooling Time Mode // 2005.12.01
		if ( data < 0 ) return;
//		if ( data > 4 ) return; // 2011.04.27
		if ( data > 9 ) return; // 2011.04.27
		_i_SCH_PRM_SET_UTIL_FIC_MULTI_TIMEMODE( data );
		break;
	case 16 : // NextPick_Free // 2006.07.21
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_NEXT_FREE_PICK_POSSIBLE( chamber , data );
		break;
	case 17 : // Get Pr // 2006.11.16
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_UTIL_CHAMBER_GET_PR_CHECK( data );
		break;
	case 18 : // Put Pr // 2006.11.16
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_UTIL_CHAMBER_PUT_PR_CHECK( data );
		break;
	case 19 : // CM Supply Mode // 2006.11.23
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_UTIL_CM_SUPPLY_MODE( data );
		break;
	case 20 : // Align_Buffer // 2007.04.04
		if ( ( chamber == F_IC ) || ( ( chamber >= BM1 ) && ( chamber < TM ) ) ) {
			if ( data == 0 ) {
				_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( 0 );
			}
			else {
				_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( chamber );
			}
		}
		else {
			_i_SCH_PRM_SET_MODULE_ALIGN_BUFFER_CHAMBER( 0 );
		}
		break;
	case 21 : // Lot Separate // 2007.10.11
		if ( data < 0 ) return;
		if ( data > 3 ) return;
		_i_SCH_PRM_SET_DIFF_LOT_NOTSUP_MODE( data );
		break;
	case 22 : // CPJOB Mode // 2008.09.29
		if ( data < 0 ) return;
		if ( data > 7 ) return;
		SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_SET( data );
		break;
	case 23 : // Cooling Time2 // 2011.04.27
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME2( data );
		break;

	case 24 : // Arm Style TM // 2014.11.20
		/*
		//
		// 2018.12.07
		//------------------------------------------------------------------------------------------
		if ( data < 0 ) return;
		if ( data > 2 ) return;
		//------------------------------------------------------------------------------------------
		if ( chamber < 0 ) return;
		if ( chamber > MAX_TM_CHAMBER_DEPTH ) return;
		//------------------------------------------------------------------------------------------
		switch( data ) {
		//------------------------------------------------------------------------------------------
		case 0 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			break;
		case 1 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		}
		*/
		//
		//
		// 2018.12.07
		//------------------------------------------------------------------------------------------
		if ( data < 0 ) return;
		if ( ( data % 100 ) > 24 ) return;
		//------------------------------------------------------------------------------------------
		if ( chamber < 0 ) return;
		if ( chamber > MAX_TM_CHAMBER_DEPTH ) return;
		//------------------------------------------------------------------------------------------
		switch( data / 100 ) {
		//------------------------------------------------------------------------------------------
		case 1 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( chamber , TRUE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( chamber , FALSE );
			break;
		}
		//------------------------------------------------------------------------------------------
		switch( data % 100 ) {
		//------------------------------------------------------------------------------------------
		case 0 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			break;
		case 1 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			break;
		case 2 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------
		case 10 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 11 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 12 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 13 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 14 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 15 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 16 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 17 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 18 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 19 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 20 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 21 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 22 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		case 23 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , TRUE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , FALSE );
			break;
		//------------------------------------------------------------------------------------------
		case 24 :
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 0 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 1 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 2 , FALSE );
			_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( chamber , 3 , TRUE );
			break;
		//------------------------------------------------------------------------------------------
		}
		break;

	case 28 : // Double Side // 2015.05.28
		if ( data < 0 ) return;
		_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( chamber , data );
		break;

	}
}
//----------------------------------------------------------------------------
void Set_RunPrm_IO_Setting( int mode ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) continue; // 2002.11.05
		if ( ( mode == -1 ) || ( mode == 0 ) ) { // 2002.12.30
			IO_MAP_STATUS_SET( i + CM1 , _i_SCH_PRM_GET_MAPPING_SKIP( i + CM1 ) );
		}
		if ( ( mode == -1 ) || ( mode == 1 ) ) { // 2002.12.30
			IO_HANDOFF_STATUS_SET( i + CM1 , _i_SCH_PRM_GET_AUTO_HANDOFF( i + CM1 ) );
		}
		if ( ( mode == -1 ) || ( mode == 6 ) ) { // 2002.12.30
			IO_MDL_SWITCH_SET( i + CM1 , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( i ) );
		}
	}
	for ( i = PM1 ; i < TM ; i++ ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2002.11.05
		if ( i == AL ) continue;
		if ( i == IC ) continue;
		if ( ( mode == -1 ) || ( mode == 2 ) ) { // 2002.12.30
			IO_RUNTIME_IN_STATUS_SET( i , _i_SCH_PRM_GET_Process_Run_In_Time( i ) );
		}
		if ( ( mode == -1 ) || ( mode == 3 ) ) { // 2002.12.30
			IO_RUNTIME_OUT_STATUS_SET( i , _i_SCH_PRM_GET_Process_Run_Out_Time( i ) );
		}
		if ( ( mode == -1 ) || ( mode == 4 ) ) { // 2002.12.30
			if ( i < BM1 ) {
				IO_PM_SIDE_SET( i , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) );
			}
		}
		if ( ( mode == -1 ) || ( mode == 8 ) ) { // 2002.12.30
			IO_MODULE_SIZE_SET( i , _i_SCH_PRM_GET_MODULE_SIZE( i ) ); // 2002.11.05
		}
		//
	}
	//
	if ( ( mode == -1 ) || ( mode == 8 ) ) { // 2007.08.14
		IO_MODULE_SIZE_SET( F_IC , _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ); // 2007.08.14
	}
	//
	if ( ( mode == -1 ) || ( mode == 5 ) ) { // 2002.12.30
		IO_SCH_RUNTYPE_SET( _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 7 ) ) { // 2002.12.30
		IO_CPJOB_LOGBASED_SET( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ); // 2002.12.30
	}
	//
	if ( ( mode == -1 ) || ( mode == 9 ) ) { // 2003.01.24
		if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) {
			if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ); // 2003.10.06
			else if (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 32 ); // 2006.03.21
			else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 62 ); // 2006.03.21
		}
		else {
			if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( 0 ); // 2003.10.06
			else if (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( 1 ); // 2006.03.21
			else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 0 ) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , 1 ) ) IO_FM_ARM_STYLE_SET( 2 ); // 2006.03.21
		}
	}
	//
	if ( ( mode == -1 ) || ( mode == 10 ) ) { // 2003.02.05
		IO_SCH_CARRIER_GROUP_SET( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() );
	}
	//
	// 11 is not support for IO
	//
	if ( ( mode == -1 ) || ( mode == 12 ) ) { // 2003.06.12
		IO_SCH_WAFER_SUPPLY_MODE_SET( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 13 ) ) { // 2005.08.02
		IO_SCH_CLUSTER_INCLUDE_SET( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 14 ) ) { // 2005.11.29
		IO_SCH_COOLING_TIME_SET( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() );
	}
	//
	// 15 is not support for IO
	//
	//
	// 16 is not support for IO
	//
	//
	// 17 is not support for IO
	//
	//
	// 18 is not support for IO
	//
	//
	// 19 is not support for IO
	//
	//
	// 20 is not support for IO
	//
	//
	if ( ( mode == -1 ) || ( mode == 21 ) ) { // 2007.10.15
		IO_SCH_LOT_SUPPLY_MODE_SET( _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 22 ) ) { // 2008.09.29
		IO_SCH_CPJOB_MODE_SET( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() );
	}
	//
	//
	if ( ( mode == -1 ) || ( mode == 23 ) ) { // 2011.06.01
		IO_SCH_COOLING_TIME_SET2( _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 24 ) ) { // 2014.11.20
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			if      (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) ) IO_TM_ARM_STYLE_SET( i , 0 );
			else if (  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) ) IO_TM_ARM_STYLE_SET( i , 1 );
			else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 0 ) &&  _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( i , 1 ) ) IO_TM_ARM_STYLE_SET( i , 2 );
		}
		//
	}
	//
	if ( ( mode == -1 ) || ( mode == 25 ) ) { // 2015.03.25
		IO_SCH_BM_SCHEDULING_FACTOR( _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() );
	}
	//
	if ( ( mode == -1 ) || ( mode == 28 ) ) { // 2015.05.28
		for ( i = CM1 ; i < AL ; i++ ) {
			IO_DOUBLE_SIDE_SET( i , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ); // 2015.05.28
		}
	}
}

//----------------------------------------------------------------------------
void Set_RunPrm_Config_Change_for_ChIntlks( int mode , int tms , int chamber , int data ) { // 2013.11.21
	int t , c;
	//
	if ( ( mode == -1 ) || ( mode == MACRO_PICK ) ) {
		//
		if ( ( tms == -1 ) || ( tms == 0 ) ) {
			//
			for ( c = CM1 ; c < MAX_CHAMBER ; c++ ) {
				//
				if ( ( chamber != -1 ) && ( chamber != c ) ) continue;
				//
				_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( c , data );
			}
			//
		}
		//
		if ( ( tms == -1 ) || ( ( tms > 0 ) && ( tms <= MAX_TM_CHAMBER_DEPTH ) ) ) {
			//
			for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
				//
				if ( ( tms != -1 ) && ( ( tms - 1 ) != t ) ) continue;
				//
				for ( c = CM1 ; c < MAX_CHAMBER ; c++ ) {
					//
					if ( ( chamber != -1 ) && ( chamber != c ) ) continue;
					//
					_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( t , c, data );
					//
				}
				//
			}
		}
	}
	//
	if ( ( mode == -1 ) || ( mode == MACRO_PLACE ) ) {
		//
		if ( ( tms == -1 ) || ( tms == 0 ) ) {
			//
			for ( c = CM1 ; c < MAX_CHAMBER ; c++ ) {
				//
				if ( ( chamber != -1 ) && ( chamber != c ) ) continue;
				//
				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( c , data );
			}
			//
		}
		//
		if ( ( tms == -1 ) || ( ( tms > 0 ) && ( tms <= MAX_TM_CHAMBER_DEPTH ) ) ) {
			//
			for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
				//
				if ( ( tms != -1 ) && ( ( tms - 1 ) != t ) ) continue;
				//
				for ( c = CM1 ; c < MAX_CHAMBER ; c++ ) {
					//
					if ( ( chamber != -1 ) && ( chamber != c ) ) continue;
					//
					_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( t , c, data );
					//
				}
				//
			}
		}
	}
}
//----------------------------------------------------------------------------
void Set_RunPrm_Config_Change_for_ChIntlks_IO() { // 2013.11.21
	int c , t;
	//
	if ( !_i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE() ) return;
	//
	for ( c = CM1 ; c < MAX_CHAMBER ; c++ ) {
		//
		_IO_SET_CHAMBER_INLTKS( MACRO_PICK  , -1 , c , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( c ) );
		_IO_SET_CHAMBER_INLTKS( MACRO_PLACE , -1 , c , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( c ) );
		//
		for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
			//
			_IO_SET_CHAMBER_INLTKS( MACRO_PICK  , t , c , _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( t , c ) );
			_IO_SET_CHAMBER_INLTKS( MACRO_PLACE , t , c , _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( t , c ) );
			//
		}
	}
	//
}
//----------------------------------------------------------------------------
BOOL Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( int Source , int Target ) {
	int i;
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Source , F_AL ) != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Target , F_AL ) ) return FALSE;
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Source , F_IC ) != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Target , F_IC ) ) return FALSE;
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Source , i + BM1 ) != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Target , i + BM1 ) ) return FALSE;
		}
	}
	else {
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Source , AL ) != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Target , AL ) ) return FALSE;
		if ( _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Source , IC ) != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( Target , IC ) ) return FALSE;
	}
	return TRUE;
}
//----------------------------------------------------------------------------
BOOL Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( int Source , int Target ) {
	if ( _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( Source - CM1 , Target ) ) return FALSE;
	return TRUE;
}
//----------------------------------------------------------------------------
int Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( int CurrSide , int ch , BOOL batch , BOOL seq ) { // 2007.05.04
	int i , m = 0;
	//================================================================================
//	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
	//================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != CurrSide ) {
//			if ( ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) && ( SYSTEM_BEGIN_GET( i ) == 1 ) ) { // 2015.07.30
			if ( ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) && ( SYSTEM_BEGIN_GET( i ) >= 2 ) ) { // 2015.07.30
				if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CurrSide ) ) {
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2009.09.24
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) <= MUF_99_USE_to_DISABLE ) ) { // 2009.09.24 // 2011.04.29
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_81_USE_to_PREND_RANDONLY ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2011.04.29 // 2011.06.24
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_81_USE_to_PREND_RANDONLY ) ) { // 2011.04.29 // 2011.06.24 // 2018.11.01
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2011.04.29 // 2011.06.24 // 2018.11.01
						//
						if ( _i_SCH_SYSTEM_PMMODE_GET( CurrSide ) != _i_SCH_SYSTEM_PMMODE_GET( i ) ) continue; // 2009.10.15
						if ( _i_SCH_SYSTEM_MOVEMODE_GET( CurrSide ) != _i_SCH_SYSTEM_MOVEMODE_GET( i ) ) continue; // 2013.09.03
						//
//						m++; // 2008.04.30
						//
						//----------------------------------------------------------------------------------------
						// 2008.04.30
						//----------------------------------------------------------------------------------------
						if ( seq ) { // 2011.06.24
							if ( ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( i ) < 110 ) ) m++;
						}
						else {
							if ( ( _i_SCH_SYSTEM_FA_GET( i ) == 1 ) && _i_SCH_MULTIREG_HAS_REGIST_DATA( i ) ) {
								m++;
							}
							else {
								if ( batch ) { // 2009.02.12
									if ( _i_SCH_MODULE_Get_TM_DoPointer( i ) <= _i_SCH_MODULE_Get_FM_LastOutPointer( i ) ) m++;
								}
								else {
									if ( !_i_SCH_SYSTEM_LASTING_GET( i ) ) m++;
								}
							}
						}
						//----------------------------------------------------------------------------------------
					}
				}
			}
		}
	}
	//================================================================================
//	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
	//================================================================================
	return m;
}
//============================================================================================================================

BOOL Get_RunPrm_RUNNING_CLUSTER() { // 2003.06.12
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) return TRUE;
	}
	return FALSE;
}

BOOL Get_RunPrm_RUNNING_TRANSFER() { // 2003.06.12
	if ( _i_SCH_SYSTEM_USING_GET( TR_CHECKING_SIDE ) > 0 ) return TRUE;
	return FALSE;
}

BOOL Get_RunPrm_RUNNING_TRANSFER_EXCEPT_AUTOMAINT() { // 2012.07.28
	if ( _i_SCH_SYSTEM_USING_GET( TR_CHECKING_SIDE ) > 0 ) {
		if ( _i_SCH_SYSTEM_USING_GET( TR_CHECKING_SIDE ) < 3 ) {
			return TRUE;
		}
	}
	return FALSE;
}

