#ifndef EQ_ENUM_H
#define EQ_ENUM_H

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
#include "INF_EQ_Enum.h"
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
extern char *MULMDLSTR[];
extern char *TMMDLSTR[];
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_TMRUN[MAX_TM_CHAMBER_DEPTH];
extern CRITICAL_SECTION CR_FEMRUN;
extern CRITICAL_SECTION CR_FEM_MAP_PICK_PLACE;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
#define USER_RECIPE_MAX_TAG			10	// 2015.07.09

#define MAX_FM_CHAMBER_DEPTH		2	// 2007.07.18

#define	MAX_REGISTER_SIZE			10
//
//#define MAX_FINGER				2	// 2009.06.02
#define MAX_FINGER_FM				2	// 2009.06.02
#define MAX_FINGER_TM				4	// 2009.06.02
//
#define MAX_FM_A_EXTEND_FINGER		8
//
//#define	MAX_PM_SLOT_DEPTH		5	// 2007.04.30
//#define	MAX_PM_SLOT_DEPTH			6	// 2007.04.30
/*
// 2012.01.10 -> go MFI Define
#define	MAX_PM_SLOT_DEPTH			26	// 2009.05.28
*/
//
#define MAX_INTERFACE_COUNT			100

#define	MAX_SDUMMY_DEPTH			MAX_PM_CHAMBER_DEPTH // 2002.05.09

//#define	MAX_SDM_STYLE			2 // 2004.11.17
#define	MAX_SDM_STYLE				10 // 2006.09.19

#define	SDM_DEFAULT_STYLE			0 // 2004.11.17

#define RUNORDER_START_VALUE		1000 // 2008.12.17
#define RUNORDER_INTERVAL_COUNT		1000 // 2006.06.27

#define	MAX_PRCS_SPAWN_STRING_SIZE	2048	// 2007.06.19

//#define	MAX_PROCESS_RECIPE_STRING_SIZE	64	// 2007.01.26
//#define	MAX_PROCESS_RECIPE_STRING_SIZE	128	// 2007.01.26
//#define	MAX_PROCESS_RECIPE_STRING_SIZE	256	// 2007.03.06

#define	MAX_LISTVIEW_DISPLAY_COUNT	500

//#define MAX_PM_STEP_COUNT			100			// 2007.10.25
#define MAX_PM_STEP_COUNT			1000		// 2007.10.25

//================================================================================================
//#define	COOLING_GAP_SAME_TIME_ZONE	0.1			// 2007.09.17
#define	COOLING_GAP_SAME_TIME_ZONE	0.5			// 2007.09.17
//================================================================================================

enum {
	CONTROL_RECIPE_ALL ,
	CONTROL_RECIPE_LOT_PROCESS ,
	CONTROL_RECIPE_CLUSTER ,
	CONTROL_RECIPE_PROCESS
};

enum {
	//---------------------------------------------------
	//      Lot - Cluster       - Process		-	DisbleError
	//      Lot - Curr PM Wafer - Next_PM_MODE	-	Wait/Abort
	//---------------------------------------------------
	FAIL_LIKE_ABORT ,							//	0
	FAIL_LIKE_CONT_GO_USE ,						//	1
	FAIL_LIKE_CONT_GO_DISABLE ,					//	2
	FAIL_LIKE_CONT_OUT_USE ,					//	3
	FAIL_LIKE_CONT_OUT_DISABLE ,				//	4
	FAIL_LIKE_END_GO_USE ,						//	5
	FAIL_LIKE_END_GO_DISABLE ,					//	6
	FAIL_LIKE_END_OUT_USE ,						//	7
	FAIL_LIKE_END_OUT_DISABLE ,					//	8
	//
	FAIL_LIKE_CONT_STOP_DISABLE ,				//	9	// 2003.08.09
	FAIL_LIKE_CONT_GO_DISABLESA ,				//	10	// 2003.08.09
	FAIL_LIKE_CONT_OUT_DISABLESA ,				//	11	// 2003.08.09
	FAIL_LIKE_CONT_STOP_DISABLESA ,				//	12	// 2003.08.12
	FAIL_LIKE_CONT_STOP_DISABLEHW ,				//	13	// 2003.08.12
	FAIL_LIKE_CONT_STOP_DISABLEHWSA ,			//	14	// 2003.08.12
	FAIL_LIKE_CONT_GO_DISABLE_ABORT ,			//	15	// 2003.08.09
	FAIL_LIKE_CONT_OUT_DISABLE_ABORT ,			//	16	// 2003.08.09
	FAIL_LIKE_CONT_STOP_DISABLE_ABORT ,			//	17	// 2003.08.09
	FAIL_LIKE_CONT_GO_DISABLESA_ABORT ,			//	18	// 2003.08.09
	FAIL_LIKE_CONT_OUT_DISABLESA_ABORT ,		//	19	// 2003.08.09
	FAIL_LIKE_CONT_STOP_DISABLESA_ABORT ,		//	20	// 2003.08.12
	FAIL_LIKE_CONT_STOP_DISABLEHW_ABORT ,		//	21	// 2003.08.12
	FAIL_LIKE_CONT_STOP_DISABLEHWSA_ABORT ,		//	22	// 2003.08.12
	FAIL_LIKE_CONT_NOTUSE_DISABLE_ABORT ,		//	23	// 2003.08.13
	FAIL_LIKE_CONT_NOTUSE_DISABLESA_ABORT ,		//	24	// 2003.08.13
	FAIL_LIKE_CONT_NOTUSE_DISABLEHW_ABORT ,		//	25	// 2003.08.13
	FAIL_LIKE_CONT_NOTUSE_DISABLEHWSA_ABORT ,	//	26	// 2003.08.13
	//
	FAIL_LIKE_END_STOP_DISABLE ,				//	27	// 2003.08.09
	FAIL_LIKE_END_GO_DISABLESA ,				//	28	// 2003.08.09
	FAIL_LIKE_END_OUT_DISABLESA ,				//	29	// 2003.08.09
	FAIL_LIKE_END_STOP_DISABLESA ,				//	30	// 2003.08.12
	FAIL_LIKE_END_STOP_DISABLEHW ,				//	31	// 2003.08.12
	FAIL_LIKE_END_STOP_DISABLEHWSA ,			//	32	// 2003.08.12
	FAIL_LIKE_END_GO_DISABLE_ABORT ,			//	33	// 2003.08.09
	FAIL_LIKE_END_OUT_DISABLE_ABORT ,			//	34	// 2003.08.09
	FAIL_LIKE_END_STOP_DISABLE_ABORT ,			//	35	// 2003.08.09
	FAIL_LIKE_END_GO_DISABLESA_ABORT ,			//	36	// 2003.08.09
	FAIL_LIKE_END_OUT_DISABLESA_ABORT ,			//	37	// 2003.08.09
	FAIL_LIKE_END_STOP_DISABLESA_ABORT ,		//	38	// 2003.08.12
	FAIL_LIKE_END_STOP_DISABLEHW_ABORT ,		//	39	// 2003.08.12
	FAIL_LIKE_END_STOP_DISABLEHWSA_ABORT ,		//	40	// 2003.08.12
	FAIL_LIKE_END_NOTUSE_DISABLE_ABORT ,		//	41	// 2003.08.13
	FAIL_LIKE_END_NOTUSE_DISABLESA_ABORT ,		//	42	// 2003.08.13
	FAIL_LIKE_END_NOTUSE_DISABLEHW_ABORT ,		//	43	// 2003.08.13
	FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT ,	//	44	// 2003.08.13
	//
	FAIL_LIKE_CONT_OUT_DISABLEHW ,				//	45	// 2006.06.28
	FAIL_LIKE_END_OUT_DISABLEHW ,				//	46	// 2006.06.28
	//
	FAIL_LIKE_USER_STYLE_1 = 1000 ,						// 2006.02.03
};

//#define	FAIL_LIKE_DEFAULT_LAST_ITEM		FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT	// 2006.06.28
#define	FAIL_LIKE_DEFAULT_LAST_ITEM		FAIL_LIKE_END_OUT_DISABLEHW	// 2006.06.28
#define	FAIL_LIKE_USER_LAST_ITEM		FAIL_LIKE_USER_STYLE_1

enum {
	MRES_LIKE_FAIL ,
	MRES_LIKE_SUCCESS ,
	MRES_LIKE_NOTRUN ,
	MRES_LIKE_SKIP ,
};

enum {
	RB_ANIM_ROTATE ,
	RB_ANIM_EXTEND ,
	RB_ANIM_RETRACT ,
	RB_ANIM_UP ,
	RB_ANIM_DOWN ,
	RB_ANIM_MOVE ,
};

#define	RB_ANIM_EXTEND2	RB_ANIM_RETRACT
#define	RB_ANIM_UPDOWN	RB_ANIM_UP

enum {
	MRES_SUCCESS_LIKE_SUCCESS ,
	MRES_SUCCESS_LIKE_NOTRUN ,
	MRES_SUCCESS_LIKE_SKIP ,
	MRES_SUCCESS_LIKE_FAIL ,
	MRES_SUCCESS_LIKE_ALLSKIP , // 2005.04.23
};

enum {
	STOP_LIKE_NONE ,
	STOP_LIKE_PM_NONE_GO_OUT ,
	STOP_LIKE_PM_ABORT_GO_NEXT ,
	STOP_LIKE_PM_ABORT_GO_OUT ,
	STOP_LIKE_NONE_ALLABORT_REMAINPRCS , // 2012.02.08
	STOP_LIKE_PM_NONE_GO_OUT_ALLABORT_REMAINPRCS , // 2012.02.08
	STOP_LIKE_PM_ABORT_GO_NEXT_ALLABORT_REMAINPRCS , // 2012.02.08
	STOP_LIKE_PM_ABORT_GO_OUT_ALLABORT_REMAINPRCS , // 2012.02.08
};

enum {
	FORCE_PRE_TAG_NOTUSE ,
	FORCE_PRE_TAG_FIRST ,
	FORCE_PRE_TAG_AGAIN ,
	FORCE_PRE_TAG_FIRST_AGAIN ,
	FORCE_PRE_TAG_DONE_FIRST ,
	FORCE_PRE_TAG_DONE_AGAIN ,
	FORCE_PRE_TAG_DONE_FIRST_AGAIN ,
};

enum {
	BUFFER_RUN_WAITING_FOR_TM ,
	BUFFER_RUN_WAITING_FOR_FM
};

//

enum {
	TR_PICK ,
	TR_PLACE ,
	TR_MOVE ,
	TR_CLEAR ,
	TR_EXTEND ,
	TR_UNKNOWN
};

//

enum {
	ROBOT_SYNCH_READY ,
	ROBOT_SYNCH_ROTATE_START ,
	ROBOT_SYNCH_EXTEND_START ,
	ROBOT_SYNCH_UPDATE ,
	ROBOT_SYNCH_RETRACT_START ,
	ROBOT_SYNCH_RESET_REVERSE1 ,
	ROBOT_SYNCH_RESET_REVERSE2 ,
	ROBOT_SYNCH_SUCCESS ,
	//
	ROBOT_SYNCH_ROTATE_START_W , // 2013.01.11
	ROBOT_SYNCH_EXTEND_START_W , // 2013.01.11
	ROBOT_SYNCH_UPDATE_W , // 2013.01.11
	ROBOT_SYNCH_RETRACT_START_W , // 2013.01.11
	//
	ROBOT_SYNCH_IGNORE , // 2016.02.18
	//
};

enum {
	ROTATION_STYLE_HOME_CW ,
	ROTATION_STYLE_HOME_CCW ,
	ROTATION_STYLE_CW ,
	ROTATION_STYLE_CCW ,
	ROTATION_STYLE_MINIMUM
};


enum {
	WAFERRESULT_PRESENT ,
	WAFERRESULT_SUCCESS ,
	WAFERRESULT_FAILURE ,
	WAFERRESULT_PROCESSING ,
	//
	WAFERRESULT_USER ,
};

enum {
	PRCS_RET_OPTION_NONE ,
	PRCS_RET_OPTION_POSTSKIP ,
	PRCS_RET_OPTION_PRESKIP ,
	PRCS_RET_OPTION_POSTPRESKIP ,
};

#define	TR_CHECKING_SIDE			MAX_CASSETTE_SIDE
// Don't Modify

typedef enum
{
	RB_MODE_HOME ,
	RB_MODE_PICK ,
	RB_MODE_PLACE ,
	RB_MODE_RETPIC ,
	RB_MODE_EXTPIC ,
	RB_MODE_RETPLC ,
	RB_MODE_EXTPLC ,
	RB_MODE_ROTATE ,
	RB_MODE_EXTEND ,
	RB_MODE_RETRACT ,
	RB_MODE_PICK_UPDATE ,
	RB_MODE_PLACE_UPDATE ,

	RB_MODE_MOVE ,
	RB_MODE_UP ,
	RB_MODE_DOWN ,

	RB_MODE_PICKPLACE ,	// 2013.01.11

} MODE_ROBOT_OBJECT;

typedef enum
{
	RB_HOMEDONE ,
	RB_PICKDONE ,
	RB_PLACEDONE ,
	RB_PICKPLACEDONE , // 2013.01.11
	RB_RETRACTDONE ,
	RB_ROTATEDONE ,
	RB_EXTENDDONE ,
	RB_UPDATEDONE ,
	RB_HOMING ,
	RB_STARTING ,
	RB_STARTING2 ,
	RB_RETRACTING ,
	RB_EXTENDING ,
	RB_ROTATING ,
	RB_UPPING ,
	RB_DOWNING ,
	RB_RETRACTING2 ,
	RB_UPDATING ,

	RB_RESETTING1 ,
	RB_RESETTING1_END ,
	RB_RESETTING2 ,
	RB_RESETTING2_END ,
	//
	RB_STARTING_W , // 2013.01.11
	RB_RETRACTING_W , // 2013.01.11
	RB_EXTENDING_W , // 2013.01.11
	RB_ROTATING_W , // 2013.01.11
	RB_UPPING_W , // 2013.01.11
	RB_DOWNING_W , // 2013.01.11
	RB_RETRACTING2_W , // 2013.01.11
	RB_UPDATING_W , // 2013.01.11

	RB_RESETTING1_W , // 2013.01.11
	RB_RESETTING2_W , // 2013.01.11
	//
	RB_WAITING ,
	RB_LASTING ,

	RB_UPPING2 , // 2014.11.04
	RB_DOWNING2 , // 2014.11.04
	RB_UPPING2_W , // 2014.11.04
	RB_DOWNING2_W , // 2014.11.04

	RB_MOVE_DONE ,
	RB_MOVING ,
} CONTROL_ROBOT_OBJECT;

typedef enum
{
	GATE_MODE_OPEN ,
	GATE_MODE_CLOSE
} MODE_GATE_OBJECT;

enum {
	DM_NOT_USE ,
	DM_DEFAULT ,
	DM_OPTION1 ,
	DM_OPTION2 ,
	DM_OPTION3 ,
	DM_DEFAULT_DMFIX ,
	DM_OPTION1_DMFIX ,
	DM_OPTION2_DMFIX ,
	DM_OPTION3_DMFIX ,
};

//  LOT | DWafer| Result             | DmFix
//
// Default
//------+-------+--------------------+-------------------
//    O |    O  | Run(Dummy)         | Run(Dummy)
//------+-------+--------------------+-------------------
//    X |    X  | Run(Normal)        | Run(Normal)(DSkip)
//------+-------+--------------------+-------------------
//    O |    X  | Run(Normal)        | Run(Normal)(DSkip)
//------+-------+--------------------+-------------------
//    X |    O  | Run(Normal)(DSkip) | Run(Normal)(DSkip)
//---
// Option1
//------+-------+--------------------+-------------------
//    O |    X  | Run(Normal)        | Run(Normal)(DSkip)
//    X |    O  | RunX               | RunX
//---
// Option2
//------+-------+--------------------+-------------------
//    O |    X  | RunX               | RunX
//    X |    O  | Run(Normal)(DSkip) | Run(Normal)(DSkip)
//---
// Option3
//------+-------+--------------------+-------------------
//    O |    X  | RunX               | RunX
//    X |    O  | RunX               | RunX

typedef enum
{
	HO_PICK_PREPARE ,
	HO_PICK_POST ,
	HO_PLACE_PREPARE ,
	HO_PLACE_POST ,
	HO_PICK_PREPARE_MDL ,
	HO_PICK_MTL_MDL ,
	HO_PLACE_PREPARE_MDL ,
	HO_PLACE_MTL_MDL
} CONTROL_INTERFACE_OBJECT;


typedef enum	// 2017.09.14
{
//	0	Idle Loading Loaded LoadFail
//	4	Mapping Mapped MapFail
//	7	Start HandOffIn Begin MapIn
//	11	Waiting Running
//	13	MapOut HandOffOut Complete
//	16	Cancel Aborted
//	18	Unloading Unloading(C) Unloaded UnloadFail Unloaded(C)
	//
	_MS_0_IDLE ,
	_MS_1_LOADING ,
	_MS_2_LOADED ,
	_MS_3_LOADFAIL ,
	_MS_4_MAPPING ,
	_MS_5_MAPPED ,
	_MS_6_MAPFAIL ,
	_MS_7_START ,
	_MS_8_HANDOFFIN ,
	_MS_9_BEGIN ,
	_MS_10_MAPIN ,
	_MS_11_WAITING ,
	_MS_12_RUNNING ,
	_MS_13_MAPOUT ,
	_MS_14_HANDOFFOUT ,
	_MS_15_COMPLETE ,
	_MS_16_CANCEL ,
	_MS_17_ABORTED ,
	_MS_18_UNLOADING_C ,
	_MS_19_UNLOADING ,
	_MS_20_UNLOADED_C ,
	_MS_21_UNLOADED ,
	_MS_22_UNLOADFAIL ,
	//
} MAIN_STATUS_OBJECT;



#define	_MOTAG_10_NEED_UPDATE		10
#define	_MOTAG_12_NEED_IN_OUT		12
#define	_MOTAG_13_NEED_IN			13
#define	_MOTAG_14_NEED_OUT			14
#define	_MOTAG_21_END				21
#define	_MOTAG_22_END				22
#define	_MOTAG_23_END				23


extern CRITICAL_SECTION CR_WAIT;
extern CRITICAL_SECTION CR_MAIN;
extern CRITICAL_SECTION CR_MAINT_INF_TRANSFER; // 2006.08.30
extern CRITICAL_SECTION CR_PRE_BEGIN_END; // 2006.12.01
extern CRITICAL_SECTION CR_PRE_BEGIN_ONLY; // 2010.05.21
extern CRITICAL_SECTION CR_FINISH_CHECK; // 2011.04.19

extern CRITICAL_SECTION CR_TMRUN[MAX_TM_CHAMBER_DEPTH];
extern CRITICAL_SECTION CR_FEMRUN;

//================================================================================================
//================================================================================================
//================================================================================================

//===================================================================================================

#endif

