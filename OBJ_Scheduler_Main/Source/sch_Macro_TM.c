#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "sch_Sub.h"
#include "sch_Prm.h" // 2018.09.20
#include "Sch_Multi_ALIC.h"
#include "iolog.h"
#include "Multijob.h"
#include "IO_Part_Log.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "FA_Handling.h"
#include "Robot_Handling.h"
#include "Timer_Handling.h"

#include "sch_estimate.h" // 2016.10.22

#include "INF_sch_CommonUser.h"
//================================================================================
extern char *_lot_printf_sub_string[][4];
extern char *_lot_printf_sub_string2[][4];
extern char *_lot_printf_sub_string3[][4];
extern char *_lot_printf_sub_string4[];
extern char *_lot_printf_sub_string_Dummy[];
extern char *MULMDLSTR[];
//================================================================================
BOOL SCHMULTI_CHECK_HANDOFF_OUT_CHECK( BOOL , int Side , int Pointer , int ch , int );
//================================================================================

extern int  _SCH_MACRO_TM_ERROR_PICK_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20	0:x 1:return ERROR 2:WaferReturn
extern int  _SCH_MACRO_TM_ERROR_PLACE_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20 0:x 1:return ERROR 2:WaferReturn

#define DEFINE_SCH_MACRO_TM_OPERATION( ZZZ ) int _SCH_MACRO_TM_OPERATION_##ZZZ##( int mode0 , int side , int pointer , int ch0 , int arm , int slot , int depth , int Switch , int incm , BOOL ForceOption , int PrePostPartSkip0 , int dide , int dointer ) { \
	int PrePostPartSkip; \
	int Function_Result; \
	int mode , log; \
	int dual , d1 , d2 , mdod , pathin , ch , ech , xdm1 , xdm2; \
	char MsgSltchar[16]; /* 2013.05.23 */ \
	int pd; /* 2015.06.03 */ \
	int errorcode; /* 2018.09.20 */ \
	\
\
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , pointer ) ) return SYS_ABORTED; /* 2012.07.12 */ /* 2016.11.04 */ \
\
	mode = mode0 % 10; \
	log  = ( mode0 / 10 ) * 10; \
	\
	/* if ( ( mode < MACRO_PICK ) || ( mode > MACRO_PLACE ) ) { */ /* 2013.01.11 */ \
	if ( ( mode < MACRO_PICK ) || ( mode > MACRO_PICKPLACE ) ) { /* 2013.01.11 */ \
		_IO_CIM_PRINTF( "_SCH_MACRO_TM_OPERATION_%d Failed because invalid mode(%d)\n" , ##ZZZ## , mode ); \
		/* return -1; */ /* 2016.11.04 */ \
		return SYS_ABORTED; /* 2016.11.04 */ \
	} \
	if ( PrePostPartSkip0 == -1 ) { \
		PrePostPartSkip = _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Switch , ( mode == MACRO_PICK ) ? 0 : 1 ); \
	} \
	else { \
		PrePostPartSkip = PrePostPartSkip0; \
	} \
	if ( ch0 >= 1000 ) { \
		ch = ch0 % 1000; \
		ech = ch0 / 1000; \
	} \
	else { \
		ch = ch0; \
		ech = ch0; \
	} \
	\
	SCH_TM_PICKPLACING_SET( ch , ( mode == MACRO_PICK ) ? 1 : 0 , ##ZZZ## , arm , side , pointer ); /* 2015.08.26 */ \
	\
	if ( Switch < 0 ) Switch = 0; \
	if ( Switch > 2 ) Switch = 0; \
	if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) { \
		/* xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , 1 ); */ /* 2009.09.18 */ /* 2017.09.11 */ \
		xdm1 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , 1 , 2 , side , pointer ); /* 2009.09.18 */ /* 2017.09.11 */ \
		if ( xdm1 >= 0 ) { /* 2009.01.21 */ \
			d1 = 1; \
			mdod = 0; \
			pathin = 0; \
		} \
		else { \
			d1 = 0; \
			mdod = _SCH_COMMON_GET_METHOD_ORDER( side , pointer ); \
			if ( incm == -1 ) { \
				pathin = _i_SCH_CLUSTER_Get_PathIn( side , pointer ); \
			} \
			else { \
				pathin = incm; \
			} \
		} \
	} \
	else { \
		xdm1 = 0; /* 2009.09.23 */ \
		d1 = 1; \
		mdod = 0; \
		pathin = 0; \
	} \
	\
	d2 = 0; \
	xdm2 = -1; /* 2009.09.23 */ \
	\
	if ( ( dide != -1 ) && ( dide != side ) ) { \
		if ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) > 0 ) ) { \
			dual = TRUE; \
			if ( ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) { \
				/* xdm2 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 ); */ /* 2009.09.18 */ /* 2017.09.11 */ \
				xdm2 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 , 2 , dide , dointer ); /* 2009.09.18 */ /* 2017.09.11 */ \
				if ( xdm2 >= 0 ) { /* 2009.01.21 */ \
					d2 = 1; /* 2007.11.05 */ \
				} /* 2007.11.05 */ \
				else { \
					d2 = 0; \
				} \
			} \
			else { \
				xdm2 = 0; /* 2009.09.23 */ \
				d2 = 1; \
			} \
		} \
		else { \
			dual = FALSE; \
		} \
	} \
	else { \
		if ( dide != -1 ) { /* 2007.11.05 */ \
			if ( ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) { /* 2007.11.05 */ \
				/* xdm2 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 ); */ /* 2009.09.18 */ /* 2017.09.11 */ \
				xdm2 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 , 2 , dide , dointer ); /* 2009.09.18 */ /* 2017.09.11 */ \
				if ( xdm2 >= 0 ) { /* 2009.01.21 */ \
					d2 = 1; /* 2007.11.05 */ \
				} /* 2007.11.05 */ \
				else { /* 2007.11.05 */ \
					d2 = 0; /* 2007.11.05 */ \
				} /* 2007.11.05 */ \
			} /* 2007.11.05 */ \
			else { /* 2007.11.05 */ \
				xdm2 = 0; /* 2009.09.23 */ \
				d2 = 1; /* 2007.11.05 */ \
			} /* 2007.11.05 */ \
		} /* 2007.11.05 */ \
		dual = FALSE; \
	} \
\
	/* 2016.10.22 */ \
\
	if      ( mode == MACRO_PICK ) { \
		EST_TM_PICK_START( ##ZZZ## ); \
		if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
			EST_CM_PICK_START( ch ); \
		} \
		else if ( ( ch >= PM1 ) && ( ch < AL ) ) { \
			EST_PM_PICK_START( ch ); \
		} \
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) { \
			EST_BM_PICK_START( ch , ##ZZZ## ); \
		} \
	} \
	else if ( mode == MACRO_PLACE ) { \
		EST_TM_PLACE_START( ##ZZZ## ); \
		if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
			EST_CM_PLACE_START( ch ); \
		} \
		else if ( ( ch >= PM1 ) && ( ch < AL ) ) { \
			EST_PM_PLACE_START( ch ); \
		} \
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) { \
			EST_BM_PLACE_START( ch , ##ZZZ## ); \
		} \
	} \
\
	if ( dual ) { \
		if ( ch < PM1 ) { \
			FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
			if ( xdm1 > 0 ) { /* 2009.09.18 */ \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
			} \
			else { \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
			} \
			FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
			if ( xdm2 > 0 ) { /* 2009.09.18 */ \
				_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
			} \
			else { \
				_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
			} \
		} \
		else { \
			FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
			if ( xdm1 > 0 ) { /* 2009.09.18 */ \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
			} \
			else { \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
			} \
			FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
			if ( xdm2 > 0 ) { /* 2009.09.18 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
			} \
			else { \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
			} \
		} \
	} \
	else { \
		if ( ch < PM1 ) { \
			FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
			if ( xdm1 > 0 ) { /* 2009.09.18 */ \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
			} \
			else { \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
			} \
		} \
		else { \
			if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
				else { \
					if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
			} \
			else { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s:%d%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
				} \
				else { \
					if ( ( d1 == 0 ) && ( ( slot / 100 ) > 0 ) ) { /* 2012.06.14 */ \
						if ( ( slot % 100 ) > 0 ) { \
							_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d,%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
						} \
						else { \
							_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
						} \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][0] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
					} \
				} \
			} \
		} \
	} \
	if ( mode == MACRO_PICK ) { \
		if ( dual ) { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
				} \
				else { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
			} \
			else { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
		else { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						if ( ( slot % 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
						} \
					} \
					if ( ( slot / 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
					} \
				} \
				else { \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth , slot ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot , ##ZZZ## + 1 , arm , depth , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
					} \
				} \
				if ( d1 != 0 ) { /* 2011.09.29 */ \
					if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
						_i_SCH_LOG_TIMER_PRINTF( side , 1011 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
					} \
				} \
			} \
			else { /* 2007.11.05 */ \
				if ( ( slot % 100 ) > 0 ) { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "GET" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
\
		errorcode = _SCH_MACRO_TM_ERROR_PICK_MODE[##ZZZ##]; /* 2018.09.20 */ \
\
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { /* 2008.02.15 */ \
			if ( ( ch < AL ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { \
				if      ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) > 0 ) ) { \
					Function_Result = EQUIP_PICK_FROM_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , TRUE , FALSE , errorcode ); \
				} \
				else if ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) <= 0 ) ) { \
					Function_Result = EQUIP_PICK_FROM_CHAMBER_##ZZZ##( side , ech + 1 , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
				} \
				else { \
					Function_Result = EQUIP_PICK_FROM_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
				} \
			} \
			else { \
				Function_Result = EQUIP_PICK_FROM_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
			} \
		} \
		else { \
			Function_Result = EQUIP_PICK_FROM_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
		} \
	} \
	else if ( mode == MACRO_PLACE ) { /* 2013.01.11 */ \
		if ( ( ch >= PM1 ) && ( ch < AL ) ) _i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 1 ); /* 2018.10.20 */ \
		if ( dual ) { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
				} \
				else { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
			} \
			else { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
		else { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						if ( ( slot % 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
						} \
					} \
					if ( ( slot / 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
					} \
				} \
				else { \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth , slot ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot , ##ZZZ## + 1 , arm , depth , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
					} \
				} \
				if ( d1 != 0 ) { /* 2011.09.29 */ \
					if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
						_i_SCH_LOG_TIMER_PRINTF( side , 1011 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
					} \
				} \
			} \
			else { /* 2007.11.05 */ \
				if ( ( slot % 100 ) > 0 ) { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "PUT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
\
		errorcode = _SCH_MACRO_TM_ERROR_PLACE_MODE[##ZZZ##]; /* 2018.09.20 */ \
\
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { /* 2008.02.15 */ \
			if ( ( ch < AL ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { \
				if      ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) > 0 ) ) { \
					Function_Result = EQUIP_PLACE_TO_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , TRUE , FALSE , errorcode ); \
				} \
				else if ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) <= 0 ) ) { \
					Function_Result = EQUIP_PLACE_TO_CHAMBER_##ZZZ##( side , ech + 1 , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
				} \
				else { \
					Function_Result = EQUIP_PLACE_TO_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
				} \
			} \
			else { \
				Function_Result = EQUIP_PLACE_TO_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
			} \
		} \
		else { \
			Function_Result = EQUIP_PLACE_TO_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE , errorcode ); \
		} \
	} \
	else { /* 2013.01.11 */ \
		if ( ( ch >= PM1 ) && ( ch < AL ) ) _i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 1 ); /* 2018.10.20 */ \
		if ( dual ) { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
				} \
				else { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
			} \
			else { /* 2007.11.05 */ \
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
				_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
		else { \
			if ( d2 == 0 ) { /* 2007.11.05 */ \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						if ( ( slot % 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
						} \
					} \
					if ( ( slot / 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1001 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) ); /* 2011.06.15 */ \
					} \
				} \
				else { \
					if ( d1 == 0 ) { /* 2007.12.04 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth , slot ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot , ##ZZZ## + 1 , arm , depth , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
					} \
				} \
				if ( d1 != 0 ) { /* 2011.09.29 */ \
					if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
						_i_SCH_LOG_TIMER_PRINTF( side , 1011 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
					} \
				} \
			} \
			else { /* 2007.11.05 */ \
				if ( ( slot % 100 ) > 0 ) { \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_RUNNING , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1001 , slot % 100 , ##ZZZ## + 1 , arm , depth % 100 , pointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); /* 2011.06.15 */ \
				} \
				if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
					_i_SCH_LOG_TIMER_PRINTF( dide , 1011 , slot / 100 , ##ZZZ## + 1 , arm , depth / 100 , dointer , "SWP" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) ); \
				} \
			} \
		} \
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { /* 2008.02.15 */ \
			if ( ( ch < AL ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) ) { \
				if      ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) > 0 ) ) { \
					Function_Result = EQUIP_SWAP_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , TRUE , FALSE ); \
				} \
				else if ( ( ( depth / 100 ) > 0 ) && ( ( depth % 100 ) <= 0 ) ) { \
					Function_Result = EQUIP_SWAP_CHAMBER_##ZZZ##( side , ech + 1 , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE ); \
				} \
				else { \
					Function_Result = EQUIP_SWAP_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE ); \
				} \
			} \
			else { \
				Function_Result = EQUIP_SWAP_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE ); \
			} \
		} \
		else { \
			Function_Result = EQUIP_SWAP_CHAMBER_##ZZZ##( side , ech , arm , slot , depth , Switch , pathin , ForceOption , PrePostPartSkip , mdod , FALSE , FALSE ); \
		} \
	} \
	if      ( Function_Result == SYS_ABORTED ) { \
		if ( dual ) { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
			} \
			else { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
			} \
		} \
		else { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
				} \
			} \
			else { \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
					if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
					} \
					FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
					if ( xdm2 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
					if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , xdm1, slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1, slot , MsgSltchar ); \
					} \
					else { \
						if ( ( d1 == 0 ) && ( ( slot / 100 ) > 0 ) ) { /* 2012.06.14 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d,%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
							else { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
						} \
						else { \
							_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][2] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
						} \
					} \
				} \
			} \
		} \
		if ( mode == MACRO_PICK ) { \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04*/ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
		else if ( mode == MACRO_PLACE ) { /* 2013.01.11 */ \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
		else { /* 2013.01.11 */ \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_FAIL , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
	} \
	else if ( Function_Result == SYS_ERROR ) { \
\
		SCH_TM_PICKPLACING_SET( ch , ( mode == MACRO_PICK ) ? 11 : 10 , ##ZZZ## , arm , side , pointer ); /* 2015.08.26 */ \
\
		if ( dual ) { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
			} \
			else { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
			} \
		} \
		else { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
				} \
			} \
			else { \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
					if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
					} \
					FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
					if ( xdm2 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
					if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
					} \
					else { \
						if ( ( d1 == 0 ) && ( ( slot / 100 ) > 0 ) ) { /* 2012.06.14 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d,%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
							else { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
						} \
						else { \
							_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][3] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
						} \
					} \
				} \
			} \
		} \
\
\
		if ( mode == MACRO_PLACE ) { /* 2015.06.03 */ \
			/* 2015.04.30 */ \
			if ( ( ch < BM1 ) && ( ch >= PM1 ) ) { \
				switch ( ( _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( ch ) / 4 ) ) { /* 2018.09.27 */ \
				case 1 : \
\
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
					_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE ); \
\
					pd = _i_SCH_CLUSTER_Get_PathDo( side , pointer ); /* 2015.06.03 */ \
					if ( ( pd > 0 ) && ( pd < PATHDO_WAFER_RETURN ) ) _i_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( side , pointer , pd-1 , ch ); /* 2015.06.03 */ \
\
					break; \
\
				case 2 : /* 2018.09.27 */ \
\
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
					_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE ); \
\
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 2 ); \
\
					break; \
				} \
			} \
\
			if ( errorcode != 0 ) { /* 2018.09.20 */ \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Placing Function Result has been Changed from User2[%d]\n" , ##ZZZ## + 1 , errorcode ); \
				switch( errorcode ) { \
				case 1 : \
				case 21 : \
					break; \
				case 2 : \
				case 22 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 2 ); \
					break; \
				case 3 : \
				case 23 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 1 ); \
					break; \
				case 11 : \
				case 31 : \
					_SCH_MACRO_TM_ERROR_PLACE_MODE[##ZZZ##] = 0; \
					break; \
				case 12 : \
				case 32 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 2 ); \
					_SCH_MACRO_TM_ERROR_PLACE_MODE[##ZZZ##] = 0; \
					break; \
				case 13 : \
				case 33 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 1 ); \
					_SCH_MACRO_TM_ERROR_PLACE_MODE[##ZZZ##] = 0; \
					break; \
				default : \
					_SCH_MACRO_TM_ERROR_PLACE_MODE[##ZZZ##] = 0; \
					break; \
				} \
			} \
\
		} \
		else if ( mode == MACRO_PICK ) { /* 2015.06.03 */ \
\
			if ( errorcode != 0 ) { /* 2018.09.20 */ \
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Picking Function Result has been Changed from User2[%d]\n" , ##ZZZ## + 1 , errorcode ); \
				switch( errorcode ) { \
				case 1 : \
				case 21 : \
					break; \
				case 2 : \
				case 22 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 2 ); \
					break; \
				case 3 : \
				case 23 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 1 ); \
					break; \
				case 11 : \
				case 31 : \
					_SCH_MACRO_TM_ERROR_PICK_MODE[##ZZZ##] = 0; \
					break; \
				case 12 : \
				case 32 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 2 ); \
					_SCH_MACRO_TM_ERROR_PICK_MODE[##ZZZ##] = 0; \
					break; \
				case 13 : \
				case 33 : \
					SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , 1 ); \
					_SCH_MACRO_TM_ERROR_PICK_MODE[##ZZZ##] = 0; \
					break; \
				default : \
					_SCH_MACRO_TM_ERROR_PICK_MODE[##ZZZ##] = 0; \
					break; \
				} \
			} \
\
		} \
\
	} \
	else { \
\
\
		if ( mode == MACRO_PLACE ) SCH_TM_PICKPLACING_SET( ch , 2 , ##ZZZ## , arm , side , pointer ); /* 2017.01.02 */ \
\
		/* 2016.10.22 */ \
\
		if      ( mode == MACRO_PICK ) { \
			EST_TM_PICK_END( ##ZZZ## ); \
			if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
				EST_CM_PICK_END( ch ); \
			} \
			else if ( ( ch >= PM1 ) && ( ch < AL ) ) { \
				EST_PM_PICK_END( ch ); \
			} \
			else if ( ( ch >= BM1 ) && ( ch < TM ) ) { \
				EST_BM_PICK_END( ch , ##ZZZ## ); \
			} \
		} \
		else if ( mode == MACRO_PLACE ) { \
			EST_TM_PLACE_END( ##ZZZ## ); \
			if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
				EST_CM_PICK_END( ch ); \
			} \
			else if ( ( ch >= PM1 ) && ( ch < AL ) ) { \
				EST_PM_PLACE_END( ch ); \
			} \
			else if ( ( ch >= BM1 ) && ( ch < TM ) ) { \
				EST_BM_PLACE_END( ch , ##ZZZ## ); \
			} \
		} \
\
		if ( dual ) { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%s%d)[F%c%s2]%cWHTM%d%s %s:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
				} \
			} \
			else { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
				} \
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
					else { \
						_i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
			} \
		} \
		else { \
			if ( ch < PM1 ) { \
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
 				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d-%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
				} \
				else { \
					_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%s%d)[F%c%s]%cWHTM%d%s %s:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
				} \
			} \
			else { \
				if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
	 				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot % 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot % 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , MsgSltchar ); \
					} \
					FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar ); \
	 				if ( xdm2 > 0 ) { /* 2009.09.18 */ \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , xdm2 , slot / 100 , MsgSltchar ); \
					} \
					else { \
						if ( ( slot / 100 ) > 0 ) _i_SCH_LOG_LOT_PRINTF( dide , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch + 1 ) , depth / 100 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d2 ] , slot / 100 , MsgSltchar ); \
					} \
				} \
				else { \
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar ); \
	 				if ( xdm1 > 0 ) { /* 2009.09.18 */ \
						_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d-%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d-%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , xdm1 , slot , MsgSltchar ); \
					} \
					else { \
						if ( ( d1 == 0 ) && ( ( slot / 100 ) > 0 ) ) { /* 2012.06.14 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d,%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%sD:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth % 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot % 100 , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
							else { \
								_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s2]%cWHTM%d%s %s:%d:%s%d:%c%sd:%d%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth / 100 , _lot_printf_sub_string_Dummy[ d1 ] , slot / 100 , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , log , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
							} \
						} \
						else { \
							_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s %s(%d:%s%d)[F%c%s]%cWHTM%d%s %s:%d:%s%d:%c%s%c%s%d%s\n" , ##ZZZ## + 1 , _lot_printf_sub_string[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , ##ZZZ## + 1 , _lot_printf_sub_string3[mode][1] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , depth , _lot_printf_sub_string_Dummy[ d1 ] , slot , arm + 'A' , _lot_printf_sub_string4[ Switch ] , 9 , _lot_printf_sub_string_Dummy[ d1 ] , slot , MsgSltchar ); \
						} \
					} \
				} \
			} \
		} \
		if ( mode == MACRO_PICK ) { \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch + 1 , 1 ); /* 2008.03.20 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 2 ); /* 2008.03.20 */ \
						\
						/* 2011.07.27 */ \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
						} \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , dide , dointer , ch , slot / 100 ) ) return SYS_ABORTED; \
						} \
						\
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
					_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
					\
					/* 2011.07.27 */ \
					if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
					} \
					\
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							_i_SCH_IO_MTL_ADD_PICK_COUNT( ch + 1 , 1 ); /* 2008.03.20 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { /* 2008.03.20 */ \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 2 ); /* 2008.03.20 */ \
							} \
							else { \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
							} \
							\
							/* 2011.07.27 */ \
							if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
								if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot ) ) return SYS_ABORTED; \
							} \
							\
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
						\
						/* 2011.07.27 */ \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
						} \
						\
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
		else if ( mode == MACRO_PLACE ) { /* 2013.01.11 */ \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch + 1 , ( ( ch + 1 < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
					_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch + 1 , ( ( ch + 1 < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { /* 2008.03.20 */ \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 2 ); /* 2008.03.20 */ \
							} \
							else { \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
							} \
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
		else { /* 2013.01.11 */ \
			if ( dual ) { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch + 1 , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch + 1 , ( ( ch + 1 < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
					} \
					else { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 2 ); /* 2008.03.20 */ \
						\
						/* 2011.07.27 */ \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
						} \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , dide , dointer , ch , slot / 100 ) ) return SYS_ABORTED; \
						} \
						\
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
					} \
				} \
				else { /* 2007.11.05 */ \
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
					_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
					_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
					\
					/* 2011.07.27 */ \
					if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
					} \
					\
					_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
				} \
			} \
			else { \
				if ( d2 == 0 ) { /* 2007.11.05 */ \
					if ( ( ch < BM1 ) && ( ch >= PM1 ) && ( ( ( ch - PM1 ) % 2 ) == 0 ) && ( dide != -1 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() > 0 ) ) { /* 2007.10.07 */ \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							if ( ( slot % 100 ) > 0 ) { \
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
								_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
							} \
						} \
						if ( ( slot / 100 ) > 0 ) { \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( dide , dointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch + 1 , ##ZZZ## + 1 , arm , depth / 100 , slot / 100 ); \
							_i_SCH_LOG_TIMER_PRINTF( dide , 1002 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							_i_SCH_IO_MTL_ADD_PICK_COUNT( ch + 1 , 1 ); /* 2008.03.20 */ \
							_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch + 1 , ( ( ch + 1 < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( dide , dointer ) != PATHDO_WAFER_RETURN ) ) , dide , 1 ); /* 2008.03.20 */ \
						} \
					} \
					else { \
						if ( d1 == 0 ) { /* 2007.12.04 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth , slot ); \
							_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { /* 2008.03.20 */ \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 2 ); /* 2008.03.20 */ \
							} \
							else { \
								_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
							} \
							\
							/* 2011.07.27 */ \
							if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
								if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot ) ) return SYS_ABORTED; \
							} \
							\
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { /* 2008.03.20 */ \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 2 ); /* 2008.03.20 */ \
							} \
							else { \
								_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
							} \
						} \
					} \
					if ( d1 != 0 ) { /* 2011.09.29 */ \
						if ( ( slot % 100 ) > 0 ) { /* 2011.09.29 */ \
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); /* 2014.02.04 */ \
							_i_SCH_LOG_TIMER_PRINTF( side , 1012 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); \
						} \
					} \
				} \
				else { /* 2007.11.05 */ \
					if ( ( slot % 100 ) > 0 ) { \
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_SWAP , FA_SUBST_SUCCESS , ch , ##ZZZ## + 1 , arm , depth % 100 , slot % 100 ); \
						_i_SCH_LOG_TIMER_PRINTF( side , 1002 , slot % 100 , -1 , pointer , -1 , -1 , "" , "" ); /* 2011.06.15 */ \
						_i_SCH_IO_MTL_ADD_PICK_COUNT( ch , 1 ); /* 2008.03.20 */ \
						\
						/* 2011.07.27 */ \
						if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { \
							if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side , pointer , ch , slot % 100 ) ) return SYS_ABORTED; \
						} \
						\
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch , ( ( ch < PM1 ) && ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) != PATHDO_WAFER_RETURN ) ) , side , 1 ); /* 2008.03.20 */ \
					} \
					if ( ( slot / 100 ) > 0 ) { /* 2011.09.29 */ \
						_i_SCH_LOG_TIMER_PRINTF( dide , 1012 , slot / 100 , -1 , dointer , -1 , -1 , "" , "" ); \
					} \
				} \
			} \
		} \
	} \
	return Function_Result; \
} \
int _SCH_MACRO_TM_MOVE_OPERATION_##ZZZ##( int side , int mode , int ch , int arm , int slot , int depth ) { \
	int i , s , p; \
/*	int wfr; */ /* 2014.06.25 */ \
	char Buffer[16]; \
	char MsgSltchar[16]; /* 2013.05.23 */ \
\
/*	wfr = 0; */ /* 2014.06.25 */ \
/*	s = 0; */ /* 2014.06.25 */ \
/*	p = 0; */ /* 2014.06.25 */ \
	s = -1; /* 2014.06.25 */ \
	p = -1; /* 2014.06.25 */ \
\
/*	if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) { */ /* 2011.06.15 */ /* 2011.07.27 */ \
	if ( slot <= 0 ) { /* 2014.06.25 */ \
		if      ( mode == 0 ) { /* pick */ \
			if ( ( ch >= PM1 ) && ( ch < AL ) ) { \
				/* wfr = _i_SCH_MODULE_Get_PM_WAFER( ch , 0 );*/ /* 2014.06.25 */  \
				/* if ( wfr <= 0 ) { */ /* 2014.06.25 */ \
				slot = _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ); /* 2014.06.25 */  \
				if ( slot <= 0 ) { /* 2014.06.25 */ \
					for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) { \
						/* wfr = _i_SCH_MODULE_Get_PM_WAFER( ch , i ); */ /* 2014.06.25 */ \
						/* if ( wfr > 0 ) { */ /* 2014.06.25 */ \
						slot = _i_SCH_MODULE_Get_PM_WAFER( ch , i ); /* 2014.06.25 */ \
						if ( slot > 0 ) { /* 2014.06.25 */ \
							s = _i_SCH_MODULE_Get_PM_SIDE( ch , i ); \
							p = _i_SCH_MODULE_Get_PM_POINTER( ch , i ); \
							break; \
						} \
					} \
				} \
			} \
			else if ( ( ch >= BM1 ) && ( ch < TM ) ) { \
				/* if ( ( slot >= 1 ) && ( slot <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ) ) { */ /* 2014.06.25 */ \
				/* 	wfr = _i_SCH_MODULE_Get_BM_WAFER( ch , slot ); */ /* 2014.06.25 */ \
				/* } */ /* 2014.06.25 */ \
				/* if ( wfr <= 0 ) { */ /* 2014.06.25 */ \
					for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) { \
						/* wfr = _i_SCH_MODULE_Get_BM_WAFER( ch , i ); */ /* 2014.06.25 */ \
						/* if ( wfr > 0 ) { */ /* 2014.06.25 */ \
						slot = _i_SCH_MODULE_Get_BM_WAFER( ch , i ); /* 2014.06.25 */ \
						if ( slot > 0 ) { /* 2014.06.25 */ \
							s = _i_SCH_MODULE_Get_BM_SIDE( ch , i ); \
							p = _i_SCH_MODULE_Get_BM_POINTER( ch , i ); \
							break; \
						} \
					} \
				/* } */ /* 2014.06.25 */ \
			} \
		} \
		else if ( mode == 1 ) { /* place */ \
			if ( ( arm >= 0 ) && ( arm <= 1 ) ) { \
				/* wfr = _i_SCH_MODULE_Get_TM_WAFER( ##ZZZ## , arm ); */ /* 2014.06.25 */ \
				slot = _i_SCH_MODULE_Get_TM_WAFER( ##ZZZ## , arm ); /* 2014.06.25 */ \
				s = _i_SCH_MODULE_Get_TM_SIDE( ##ZZZ## , arm ); \
				p = _i_SCH_MODULE_Get_TM_POINTER( ##ZZZ## , arm ); \
			} \
			/* if ( wfr <= 0 ) { */ /* 2014.06.25 */ \
			if ( slot <= 0 ) { /* 2014.06.25 */ \
				for ( i = 0 ; i <= 1 ; i++ ) { \
					/* wfr = _i_SCH_MODULE_Get_TM_WAFER( ch , i ); */ /* 2014.06.25 */ \
					/* if ( wfr > 0 ) { */ /* 2014.06.25 */ \
					slot = _i_SCH_MODULE_Get_TM_WAFER( ch , i ); /* 2014.06.25 */ \
					if ( slot > 0 ) { /* 2014.06.25 */ \
						s = _i_SCH_MODULE_Get_TM_SIDE( ch , i ); \
						p = _i_SCH_MODULE_Get_TM_POINTER( ch , i ); \
						break; \
					} \
				} \
			} \
		} \
	} /* 2014.06.25 */ \
/*	} */ \
\
	/* if ( wfr <= 0 ) { */ /* 2014.06.25 */ \
	if ( slot <= 0 ) { /* 2014.06.25 */ \
		strcpy( Buffer , "" ); \
		slot = 1; /* 2014.06.25 */ \
	} \
	else { \
		/* FA_SIDE_TO_SLOT_GET_L( s , p , MsgSltchar ); */ /* 2014.06.25 */ \
		/* sprintf( Buffer , "\t%d%s" , wfr , MsgSltchar ); */ /* 2014.06.25 */ \
\
		if ( ( s >= 0 ) && ( p >= 0 ) ) { /* 2014.06.25 */ \
			FA_SIDE_TO_SLOT_GET_L( s , p , MsgSltchar ); \
			sprintf( Buffer , "\t%d%s" , slot , MsgSltchar ); \
		} \
		else { \
			sprintf( Buffer , "\t%d" , slot ); \
		} \
	} \
\
/*	_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Start to %s(%d:%d)[F%c]%cWHTM%dMOVESTART %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , depth , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); */ /* 2014.06.25 */ \
\
	_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Start to %s(%d:%d)[F%c]%cWHTM%dMOVESTART %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , slot , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); /* 2014.06.25 */ \
\
	if ( EQUIP_ROBOT_MOVE_CONTROL_##ZZZ##( mode , side , ch , arm , slot , depth ) != SYS_SUCCESS ) { \
/*	_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Fail to %s(%d:%d)[F%c]%cWHTM%dMOVEFAIL %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , depth , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); */ /* 2014.06.25 */  \
\
		_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Fail to %s(%d:%d)[F%c]%cWHTM%dMOVEFAIL %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , slot , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); /* 2014.06.25 */  \
\
		return SYS_ABORTED; \
	} \
/*	_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Success to %s(%d:%d)[F%c]%cWHTM%dMOVESUCCESS %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , depth , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); */ /* 2014.06.25 */  \
\
	_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling Move Success to %s(%d:%d)[F%c]%cWHTM%dMOVESUCCESS %s:%c:%d%s\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , depth , slot , arm + 'A' , 9 , ##ZZZ## + 1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , arm + 'A' , mode , Buffer ); /* 2014.06.25 */  \
\
	return SYS_SUCCESS; \
}


DEFINE_SCH_MACRO_TM_OPERATION( 0 );
DEFINE_SCH_MACRO_TM_OPERATION( 1 );
DEFINE_SCH_MACRO_TM_OPERATION( 2 );
DEFINE_SCH_MACRO_TM_OPERATION( 3 );
#ifndef PM_CHAMBER_12
DEFINE_SCH_MACRO_TM_OPERATION( 4 );
DEFINE_SCH_MACRO_TM_OPERATION( 5 );
#ifndef PM_CHAMBER_30
DEFINE_SCH_MACRO_TM_OPERATION( 6 );
DEFINE_SCH_MACRO_TM_OPERATION( 7 );
#endif
#endif





BOOL _SCH_MACRO_TM_DATABASE_OPERATION( int tms , int mode , int och , int arm , int slot1 , int slot2 , int side1 , int pointer1 , int side2 , int pointer2 , int order , int type ) {
	int curwfr1 , curwfr2 , res = TRUE , cs , cp , pmwfr1 , pmwfr2;
//	if      ( mode == MACRO_PICK ) { // 2017.03.09
	if      ( ( mode % 10 ) == MACRO_PICK ) { // 2017.03.09
		curwfr1 = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100;
		curwfr2 = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100;
		//
		if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			if ( ( curwfr1 > 0 ) || ( curwfr2 > 0 ) ) res = FALSE;
		}
		else if ( slot1 > 0 ) {
			if ( curwfr1 > 0 ) res = FALSE;
		}
		else if ( slot2 > 0 ) {
			if ( curwfr2 > 0 ) res = FALSE;
		}
		//
		if      ( ( och == IC ) || ( och == F_IC ) ) {
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , order );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , type );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			//
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , side2 , pointer2 );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFIC_WAFER( slot1 ) + ( _i_SCH_MODULE_Get_MFIC_WAFER( slot2 ) * 100 ) );
				//
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , 0 );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , 0 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , 0 , 0 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFIC_WAFER( slot1 ) );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , cs , cp );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( curwfr2 * 100 ) + _i_SCH_MODULE_Get_MFIC_WAFER( slot1 ) );
				}
				//
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , 0 );
				//
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side2 , pointer2 , 0 , 0 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFIC_WAFER( slot2 )* 100 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , side2 , pointer2 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_MFIC_WAFER( slot2 ) * 100 ) + curwfr1 );
				}
				//
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , 0 );
			}
		}
		else if ( ( och == AL ) || ( och == F_AL ) ) {
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , order );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , type );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			//
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , side2 , pointer2 );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFALS_WAFER( slot1 ) + ( _i_SCH_MODULE_Get_MFALS_WAFER( slot2 ) * 100 ) );
				//
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , 0 );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , 0 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , 0 , 0 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFALS_WAFER( slot1 ) );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , cs , cp );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( curwfr2 * 100 ) + _i_SCH_MODULE_Get_MFALS_WAFER( slot1 ) );
				}
				//
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , 0 );
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side2 , pointer2 , 0 , 0 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_MFALS_WAFER( slot2 )* 100 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , side2 , pointer2 );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_MFALS_WAFER( slot2 ) * 100 ) + curwfr1 );
				}
				//
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , 0 );
			}
		}
		else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , order );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , type );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			//
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , side2 , pointer2 );
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( slot2 * 100 ) + slot1 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , slot1 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , cs , cp );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( curwfr2 * 100 ) + slot1 );
				}
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side2 , pointer2 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( slot2 * 100 ) );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , side2 , pointer2 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( slot2 * 100 ) + curwfr1 );
				}
			}
		}
		else if ( ( och >= PM1 ) && ( och < AL ) ) {
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , order );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , type );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			//
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , side2 , pointer2 );
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) );
				_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , 0 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) % 100 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , cs , cp );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( curwfr2 * 100 ) + ( _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) % 100 ) );
				}
				_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , ( _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) / 100 ) * 100 );
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side2 , pointer2 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 ) / 100 ) * 100 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , side2 , pointer2 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( ( _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 ) / 100 ) * 100 ) + curwfr1 );
				}
				_i_SCH_MODULE_Set_PM_WAFER( och , slot2 - 1 , _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 ) % 100 );
			}
		}
		else if ( och >= BM1 ) {
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , order );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , type );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			//
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , side2 , pointer2 );
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_BM_WAFER( och , slot1 ) + ( _i_SCH_MODULE_Get_BM_WAFER( och , slot2 ) * 100 ) );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , 0 , -1 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , 0 , -1 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_BM_WAFER( och , slot1 ) );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side1 , pointer1 , cs , cp );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( curwfr2 * 100 ) + _i_SCH_MODULE_Get_BM_WAFER( och , slot1 ) );
				}
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , 0 , -1 );
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , side2 , pointer2 , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , _i_SCH_MODULE_Get_BM_WAFER( och , slot2 )* 100 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , side2 , pointer2 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_BM_WAFER( och , slot2 ) * 100 ) + curwfr1 );
				}
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , 0 , -1 );
			}
		}
	}
//	else if ( mode == MACRO_PLACE ) { // 2017.03.09
	else if ( ( mode % 10 ) == MACRO_PLACE ) { // 2017.03.09
		curwfr1 = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100;
		curwfr2 = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100;
		//
		if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			if ( ( curwfr1 <= 0 ) || ( curwfr2 <= 0 ) ) res = FALSE;
		}
		else if ( slot1 > 0 ) {
			if ( curwfr1 <= 0 ) res = FALSE;
		}
		else if ( slot2 > 0 ) {
			if ( curwfr2 <= 0 ) res = FALSE;
		}
		//
		if      ( ( och == IC ) || ( och == F_IC ) ) {
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				pmwfr1 = _i_SCH_MODULE_Get_MFIC_WAFER( slot1 );
				pmwfr2 = _i_SCH_MODULE_Get_MFIC_WAFER( slot2 );
				if ( ( pmwfr1 > 0 ) || ( pmwfr2 > 0 ) ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , curwfr1 );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , curwfr2 );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
			}
			else if ( slot1 > 0 ) {
				pmwfr1 = _i_SCH_MODULE_Get_MFIC_WAFER( slot1 );
				if ( pmwfr1 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , curwfr1 );
				//
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr2 * 100 );
				}
			}
			else if ( slot2 > 0 ) {
				pmwfr2 = _i_SCH_MODULE_Get_MFIC_WAFER( slot2 );
				if ( pmwfr2 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , curwfr2 );
				//
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr1 );
				}
			}
		}
		else if ( ( och == AL ) || ( och == F_AL ) ) {
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				pmwfr1 = _i_SCH_MODULE_Get_MFALS_WAFER( slot1 );
				pmwfr2 = _i_SCH_MODULE_Get_MFALS_WAFER( slot2 );
				if ( ( pmwfr1 > 0 ) || ( pmwfr2 > 0 ) ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot1 , side1 , pointer1 , order );
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot2 , side2 , pointer2 , order );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , curwfr1 );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , curwfr2 );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
			}
			else if ( slot1 > 0 ) {
				pmwfr1 = _i_SCH_MODULE_Get_MFALS_WAFER( slot1 );
				if ( pmwfr1 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot1 , side1 , pointer1 , order );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , curwfr1 );
				//
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr2 * 100 );
				}
			}
			else if ( slot2 > 0 ) {
				pmwfr2 = _i_SCH_MODULE_Get_MFALS_WAFER( slot2 );
				if ( pmwfr2 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot2 , side2 , pointer2 , order );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , curwfr2 );
				//
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr1 );
				}
			}
		}
		else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
			}
			else if ( slot1 > 0 ) {
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr2 * 100 );
				}
			}
			else if ( slot2 > 0 ) {
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr1 );
				}
			}
		}
		else if ( ( och >= PM1 ) && ( och < AL ) ) {
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				pmwfr1 = _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) % 100;
				pmwfr2 = _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) / 100;
				if ( ( pmwfr1 > 0 ) || ( pmwfr2 > 0 ) ) res = FALSE;
//				_i_SCH_MODULE_Set_PM_SIDE_POINTER( och , side1 , pointer1 , side2 , pointer2 ); // 2014.01.10
				_i_SCH_MODULE_Set_PM_SIDE( och , slot1 - 1 , side1 ); // 2014.01.10
				_i_SCH_MODULE_Set_PM_POINTER( och , slot1 - 1 , pointer1 ); // 2014.01.10
				_i_SCH_MODULE_Set_PM_SIDE( och , slot1 , side2 ); // 2014.01.10
				_i_SCH_MODULE_Set_PM_POINTER( och , slot1 , pointer2 ); // 2014.01.10
				_i_SCH_MODULE_Set_PM_PATHORDER( och , slot1 - 1 , order );
				_i_SCH_MODULE_Set_PM_STATUS( och , slot1 - 1 , type );
				_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , ( curwfr2 * 100 ) + curwfr1 );
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
			}
			else if ( slot1 > 0 ) {
				pmwfr1 = _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) % 100;
				pmwfr2 = _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 ) / 100;
				if ( pmwfr1 > 0 ) res = FALSE;
				if ( pmwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_PM_SIDE( och , slot1 - 1 , side1 );
					_i_SCH_MODULE_Set_PM_POINTER( och , slot1 - 1 , pointer1 );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot1 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot1 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , curwfr1 );
				}
				else {
					/*
					// 2014.01.10
					cs = _i_SCH_MODULE_Get_PM_SIDE( och , 0 );
					cp = _i_SCH_MODULE_Get_PM_POINTER( och , 0 );
					_i_SCH_MODULE_Set_PM_SIDE_POINTER( och , side1 , pointer1 , cs , cp );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot1 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot1 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , ( pmwfr2 * 100 ) + curwfr1 );
					*/
					//
					// 2014.01.10
					//
					if ( pmwfr1 <= 0 ) {
						cs = _i_SCH_MODULE_Get_PM_SIDE( och , slot1 - 1 );
						cp = _i_SCH_MODULE_Get_PM_POINTER( och , slot1 - 1 );
						_i_SCH_MODULE_Set_PM_SIDE( och , slot1 , cs );
						_i_SCH_MODULE_Set_PM_POINTER( och , slot1 , cp );
					}
					//
					_i_SCH_MODULE_Set_PM_SIDE( och , slot1 - 1 , side1 );
					_i_SCH_MODULE_Set_PM_POINTER( och , slot1 - 1 , pointer1 );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot1 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot1 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , ( pmwfr2 * 100 ) + curwfr1 );
					//
				}
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr2 * 100 );
				}
			}
			else if ( slot2 > 0 ) {
				pmwfr1 = _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 ) % 100;
				pmwfr2 = _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 ) / 100;
				if ( pmwfr2 > 0 ) res = FALSE;
				if ( pmwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_PM_SIDE( och , slot2 - 1 , side2 );
					_i_SCH_MODULE_Set_PM_POINTER( och , slot2 - 1 , pointer2 );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot2 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot2 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot2 - 1 , ( curwfr2 * 100 ) );
				}
				else {
					/*
					// 2014.01.10
					cs = _i_SCH_MODULE_Get_PM_SIDE( och , 0 );
					cp = _i_SCH_MODULE_Get_PM_POINTER( och , 0 );
					_i_SCH_MODULE_Set_PM_SIDE_POINTER( och , cs , cp , side2 , pointer2 );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot2 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot2 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot2 - 1 , ( curwfr2 * 100 ) + pmwfr1 );
					*/
					// 2014.01.10
					//
					_i_SCH_MODULE_Set_PM_SIDE( och , slot2 , side2 );
					_i_SCH_MODULE_Set_PM_POINTER( och , slot2 , pointer2 );
					_i_SCH_MODULE_Set_PM_PATHORDER( och , slot2 - 1 , order );
					_i_SCH_MODULE_Set_PM_STATUS( och , slot2 - 1 , type );
					_i_SCH_MODULE_Set_PM_WAFER( och , slot2 - 1 , ( curwfr2 * 100 ) + pmwfr1 );
					//

				}
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr1 );
				}
			}
		}
		else if ( och >= BM1 ) {
			if      ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
				pmwfr1 = _i_SCH_MODULE_Get_BM_WAFER( och , slot1 );
				pmwfr2 = _i_SCH_MODULE_Get_BM_WAFER( och , slot2 );
				if ( ( pmwfr1 > 0 ) || ( pmwfr2 > 0 ) ) res = FALSE;
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , curwfr1 , order );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , curwfr2 , order );
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
			}
			else if ( slot1 > 0 ) {
				pmwfr1 = _i_SCH_MODULE_Get_BM_WAFER( och , slot1 );
				if ( pmwfr1 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , curwfr1 , order );
				//
				if ( curwfr2 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					cs = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					cp = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , cs , cp , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr2 * 100 );
				}
			}
			else if ( slot2 > 0 ) {
				pmwfr2 = _i_SCH_MODULE_Get_BM_WAFER( och , slot2 );
				if ( pmwfr2 > 0 ) res = FALSE;
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , curwfr2 , order );
				//
				if ( curwfr1 <= 0 ) {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
				}
				else {
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , curwfr1 );
				}
			}
		}
	}
	return res;
}

int _SCH_MACRO_DATABASE_CHECK_DBINDEX( int *side , int *pt , int *wafer ) { // 2012.05.04
	if ( *wafer <= 0 )					{	*side = 0;	*pt = 0;	*wafer = 0;	return 1;	}
	if ( *side < 0 )					{	*side = 0;	*pt = 0;	*wafer = 0;	return 2;	}
	if ( *side >= MAX_CASSETTE_SIDE )	{	*side = 0;	*pt = 0;	*wafer = 0;	return 2;	}
	if ( *pt < 0 )						{	*side = 0;	*pt = 0;	*wafer = 0;	return 2;	}
	if ( *pt >= MAX_CASSETTE_SLOT_SIZE ){	*side = 0;	*pt = 0;	*wafer = 0;	return 2;	}
	if ( !_i_SCH_SYSTEM_USING_RUNNING( *side ) ){	*side = 0;	*pt = 0;	*wafer = 0;	return 3;	}
	return 0;
}
//
BOOL _SCH_MACRO_DATABASE_GET_DBINDEX( int ch , int slot , int *s , int *p ) {
	int i , j , c;
	//
	for ( c = 0 ; c < 2 ; c++ ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			if ( c == 0 ) {
				if ( ( ch - CM1 ) != i ) continue;
			}
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_READY ) {
					if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_CM_END ) {
						continue;
					}
				}
				if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != ch ) continue;
				if ( _i_SCH_CLUSTER_Get_SlotIn( i , j ) != slot ) continue;
				//
				*s = i;
				*p = j;
				//
				return TRUE;
			}
		}
	}
	//
	return FALSE;
}

/*
// 2012.11.21
BOOL _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( BOOL optioncheck , int tms , int mode , int och , int arm , int Slot , int Depth ) {
	int s , p , w;
	//
	if ( optioncheck ) { // 2012.11.05
		if ( !_i_SCH_PRM_GET_MAINTINF_DB_USE() ) return TRUE; // 2012.05.04
	}
	//

	if      ( mode == MACRO_PICK ) {
		//----------------------------------------------------------------
		if      ( ( och == IC ) || ( och == F_IC ) ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
			//
			s = _i_SCH_MODULE_Get_MFIC_SIDE( Depth );
			p = _i_SCH_MODULE_Get_MFIC_POINTER( Depth );
			w = _i_SCH_MODULE_Get_MFIC_WAFER( Depth );
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
		}
		else if ( ( och == AL ) || ( och == F_AL ) ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			s = _i_SCH_MODULE_Get_MFALS_SIDE( Depth );
			p = _i_SCH_MODULE_Get_MFALS_POINTER( Depth );
			w = _i_SCH_MODULE_Get_MFALS_WAFER( Depth );
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
		}
		else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			//
			if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			if ( !_SCH_MACRO_DATABASE_GET_DBINDEX( och , Slot , &s , &p ) ) {
				s = 0; // 2012.05.04
				p = 0; // 2012.05.04
				w = 0; // 2012.05.04
			}
			else {
				//
				w = Slot;
				//
			}
			//
			if ( w > 0 ) {
				_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
				_i_SCH_MODULE_Inc_TM_OutCount( s ); // 2012.01.21
				_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING ); // 2012.05.04
			}
			//
		}
		else if ( ( och >= PM1 ) && ( och < AL ) ) {
			//
			if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH  ) ) return FALSE;
			//
			s = _i_SCH_MODULE_Get_PM_SIDE( och , Depth-1 );
			p = _i_SCH_MODULE_Get_PM_POINTER( och , Depth-1 );
			w = _i_SCH_MODULE_Get_PM_WAFER( och , Depth-1 );
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
			if ( w > 0 ) { // 2012.05.04
				_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
			}
			//
		}
		else if ( och >= BM1 ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			s = _i_SCH_MODULE_Get_BM_SIDE( och , Depth );
			p = _i_SCH_MODULE_Get_BM_POINTER( och , Depth );
			w = _i_SCH_MODULE_Get_BM_WAFER( och , Depth );
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
			if ( w > 0 ) { // 2012.05.04
				_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
			}
			//
		}
		else {
			return FALSE;
		}
		//
		//----------------------------------------------------------------
		if ( !optioncheck ) { // 2012.11.05
			if ( w <= 0 ) return FALSE;
		}
		//----------------------------------------------------------------
		if ( ( tms < 0 ) || ( tms >= MAX_TM_CHAMBER_DEPTH  ) ) return FALSE;
		if ( ( arm < 0 ) || ( arm >= MAX_FINGER_TM  ) ) return FALSE;
		//
//		if ( s != -1 ) { // 2012.05.04
			_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , 0 );
			_i_SCH_MODULE_Set_TM_TYPE( tms , arm , 0 );
			_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
			_i_SCH_MODULE_Set_TM_SIDE_POINTER( tms , arm , s , p , 0 , 0 );
			_i_SCH_MODULE_Set_TM_WAFER( tms , arm , w );
//		}
		//----------------------------------------------------------------
		if      ( ( och == IC ) || ( och == F_IC ) ) {
			_i_SCH_MODULE_Set_MFIC_WAFER( Depth , 0 );
		}
		else if ( ( och == AL ) || ( och == F_AL ) ) {
			_i_SCH_MODULE_Set_MFALS_WAFER( Depth , 0 );
		}
		else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			//
//			if ( s != -1 ) _i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING ); // 2012.05.04
			//
		}
		else if ( ( och >= PM1 ) && ( och < AL ) ) {
			_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , 0 );
		}
		else if ( och >= BM1 ) {
			_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , 0 , -1 );
		}
		else {
			return FALSE;
		}
		//----------------------------------------------------------------
	}
	else if ( mode == MACRO_PLACE ) {
		//----------------------------------------------------------------
		if ( ( tms < 0 ) || ( tms >= MAX_TM_CHAMBER_DEPTH  ) ) return FALSE;
		if ( ( arm < 0 ) || ( arm >= MAX_FINGER_TM  ) ) return FALSE;
		//
		s = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
		p = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
		w = _i_SCH_MODULE_Get_TM_WAFER( tms , arm );
		//
		_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
		//
		//----------------------------------------------------------------
		if ( !optioncheck ) { // 2012.11.05
			if ( w <= 0 ) return FALSE;
		}
		//----------------------------------------------------------------
		if      ( ( och == IC ) || ( och == F_IC ) ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
			//
			_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( Depth , s , p );
			_i_SCH_MODULE_Set_MFIC_WAFER( Depth , w );
		}
		else if ( ( och == AL ) || ( och == F_AL ) ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( Depth , s , p , 0 );
			_i_SCH_MODULE_Set_MFALS_WAFER( Depth , w );
		}
		else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			//
			if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			if ( w > 0 ) { // 2012.05.04
				//
				_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
				_i_SCH_MODULE_Inc_TM_InCount( s ); // 2012.01.21
				//
			}
			//
		}
		else if ( ( och >= PM1 ) && ( och < AL ) ) {
			//
			if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH  ) ) return FALSE;
			//
			_i_SCH_MODULE_Set_PM_SIDE( och , Depth-1 , s );
			_i_SCH_MODULE_Set_PM_POINTER( och , Depth-1 , p );
			_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , w );
		}
		else if ( och >= BM1 ) {
			//
			if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
			//
			_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , Depth , s , p );
			_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , w , 0 );
		}
		else {
			return FALSE;
		}
		//----------------------------------------------------------------
		_i_SCH_MODULE_Set_TM_WAFER( tms , arm , 0 );
		//----------------------------------------------------------------
	}
	else {
		return FALSE;
	}
	//
	return TRUE;
}
*/

/*
// 2013.01.11
// 2012.11.21
BOOL _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( BOOL optioncheck , int tms , int mode , int och , int arm , int Slot0 , int Depth0 ) {
	int i , s , p , w , Slot , Depth , Res;
	//
	if ( optioncheck ) { // 2012.11.05
		if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 1 ) return TRUE; // 2012.05.04
	}
	//
	if ( ( tms < 0 ) || ( tms >= MAX_TM_CHAMBER_DEPTH  ) ) return FALSE;
	if ( ( arm < 0 ) || ( arm >= MAX_FINGER_TM  ) ) return FALSE;
	//
	//
	Res = FALSE;
	//
	for ( i = 0 ;  i < 2 ; i++ ) {
		//
		if ( i == 0 ) {
			Slot  = Slot0 % 100;
			Depth = Depth0 % 100;
		}
		else {
			Slot  = Slot0 / 100;
			Depth = Depth0 / 100;
		}
		//
		if      ( mode == MACRO_PICK ) {
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) continue;
				//
				s = _i_SCH_MODULE_Get_MFIC_SIDE( Depth );
				p = _i_SCH_MODULE_Get_MFIC_POINTER( Depth );
				w = _i_SCH_MODULE_Get_MFIC_WAFER( Depth );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				s = _i_SCH_MODULE_Get_MFALS_SIDE( Depth );
				p = _i_SCH_MODULE_Get_MFALS_POINTER( Depth );
				w = _i_SCH_MODULE_Get_MFALS_WAFER( Depth );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
				//
				if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				if ( !_SCH_MACRO_DATABASE_GET_DBINDEX( och , Slot , &s , &p ) ) {
					s = 0; // 2012.05.04
					p = 0; // 2012.05.04
					w = 0; // 2012.05.04
				}
				else {
					//
					w = Slot;
					//
				}
				//
				if ( w > 0 ) {
					_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
					_i_SCH_MODULE_Inc_TM_OutCount( s ); // 2012.01.21
					_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING ); // 2012.05.04
				}
				//
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) {
				//
				if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH ) ) continue;
				//
				s = _i_SCH_MODULE_Get_PM_SIDE( och , Depth-1 );
				p = _i_SCH_MODULE_Get_PM_POINTER( och , Depth-1 );
				w = _i_SCH_MODULE_Get_PM_WAFER( och , Depth-1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
				if ( w > 0 ) { // 2012.05.04
					_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
				}
				//
			}
			else if ( och >= BM1 ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( och , Depth );
				p = _i_SCH_MODULE_Get_BM_POINTER( och , Depth );
				w = _i_SCH_MODULE_Get_BM_WAFER( och , Depth );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
				if ( w > 0 ) { // 2012.05.04
					_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
				}
				//
			}
			else {
				return FALSE;
			}
			//
			//----------------------------------------------------------------
			if ( w <= 0 ) continue;
			//----------------------------------------------------------------
			if ( i == 0 ) {
				_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , 0 );
				_i_SCH_MODULE_Set_TM_TYPE( tms , arm , 0 );
				_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
				_i_SCH_MODULE_Set_TM_SIDE_POINTER1( tms , arm , s , p );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) + w );
				//
			}
			else {
				_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , 0 );
				_i_SCH_MODULE_Set_TM_TYPE( tms , arm , 0 );
				_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
				_i_SCH_MODULE_Set_TM_SIDE_POINTER2( tms , arm , s , p );
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) + ( w * 100 ) );
				//
			}
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_WAFER( Depth , 0 );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				_i_SCH_MODULE_Set_MFALS_WAFER( Depth , 0 );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) {
				_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , 0 );
			}
			else if ( och >= BM1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , 0 , -1 );
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			//
			Res = TRUE;
			//
			//----------------------------------------------------------------
		}
		else if ( mode == MACRO_PLACE ) {
			//----------------------------------------------------------------
			if ( i == 0 ) {
				s = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
				p = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
				w = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100;
			}
			else {
				s = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
				p = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
				w = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100;
			}
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
			//----------------------------------------------------------------
			if ( w <= 0 ) continue;
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) continue;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( Depth , s , p );
				_i_SCH_MODULE_Set_MFIC_WAFER( Depth , w );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( Depth , s , p , 0 );
				_i_SCH_MODULE_Set_MFALS_WAFER( Depth , w );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
				//
				if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				if ( w > 0 ) { // 2012.05.04
					//
					_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
					_i_SCH_MODULE_Inc_TM_InCount( s ); // 2012.01.21
					//
				}
				//
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) {
				//
				if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH  ) ) continue;
				//
				_i_SCH_MODULE_Set_PM_SIDE( och , Depth-1 , s );
				_i_SCH_MODULE_Set_PM_POINTER( och , Depth-1 , p );
				_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , w );
			}
			else if ( och >= BM1 ) {
				//
				if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , Depth , s , p );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , w , 0 );
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			if ( i == 0 ) {
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) * 100 ) + 0 );
				//
			}
			else {
				//
				_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) + 0 );
				//
			}
			//----------------------------------------------------------------
			//
			Res = TRUE;
			//
			//----------------------------------------------------------------
		}
		else {
			return FALSE;
		}
	}
	//
	return Res;
}
*/


BOOL _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( BOOL optioncheck , int tms , int mode0 , int och , int arm0 , int Slot0 , int Depth0 ) { // 2013.01.11
	int i , s , p , w , Slot , Depth , Res , mode;
	int l , le , arm;
	//
	if ( optioncheck ) { // 2012.11.05
		if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 1 ) return TRUE; // 2012.05.04
	}
	//
	if ( ( tms < 0 ) || ( tms >= MAX_TM_CHAMBER_DEPTH  ) ) return FALSE;
	//
//	if ( mode0 == MACRO_PICKPLACE ) { // 2017.03.09
	if ( ( mode0 % 10 ) == MACRO_PICKPLACE ) { // 2017.03.09
		le = 2;
	}
	else {
		if ( ( arm0 < 0 ) || ( arm0 >= MAX_FINGER_TM  ) ) return FALSE;
		le = 1;
	}
	//
	Res = FALSE;
	//
	for ( l = 0 ; l < le ; l++ ) {
		//
		if ( l == 0 ) {
			if ( le == 2 ) {
				mode = MACRO_PICK;
				arm = arm0 % 100;
			}
			else {
//				mode = mode0; // 2017.03.09
				mode = ( mode0 % 10 ); // 2017.03.09
				arm = arm0;
			}
		}
		else {
			mode = MACRO_PLACE;
			arm = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( arm0 );
		}
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( i == 0 ) {
				Slot  = Slot0 % 100;
				Depth = Depth0 % 100;
			}
			else {
				Slot  = Slot0 / 100;
				Depth = Depth0 / 100;
			}
			//
			if      ( mode == MACRO_PICK ) {
				//----------------------------------------------------------------
				if      ( ( och == IC ) || ( och == F_IC ) ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) continue;
					//
					s = _i_SCH_MODULE_Get_MFIC_SIDE( Depth );
					p = _i_SCH_MODULE_Get_MFIC_POINTER( Depth );
					w = _i_SCH_MODULE_Get_MFIC_WAFER( Depth );
					//
					_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
					//
				}
				else if ( ( och == AL ) || ( och == F_AL ) ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					s = _i_SCH_MODULE_Get_MFALS_SIDE( Depth );
					p = _i_SCH_MODULE_Get_MFALS_POINTER( Depth );
					w = _i_SCH_MODULE_Get_MFALS_WAFER( Depth );
					//
					_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
					//
				}
				else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
					//
					if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					if ( !_SCH_MACRO_DATABASE_GET_DBINDEX( och , Slot , &s , &p ) ) {
						s = 0; // 2012.05.04
						p = 0; // 2012.05.04
						w = 0; // 2012.05.04
					}
					else {
						//
						w = Slot;
						//
					}
					//
					if ( w > 0 ) {
						_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
						_i_SCH_MODULE_Inc_TM_OutCount( s ); // 2012.01.21
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING ); // 2012.05.04
					}
					//
				}
				else if ( ( och >= PM1 ) && ( och < AL ) ) {
					//
					if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH ) ) continue;
					//
					s = _i_SCH_MODULE_Get_PM_SIDE( och , Depth-1 );
					p = _i_SCH_MODULE_Get_PM_POINTER( och , Depth-1 );
					w = _i_SCH_MODULE_Get_PM_WAFER( och , Depth-1 );
					//
					_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
					//
					if ( w > 0 ) { // 2012.05.04
						_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
					}
					//
				}
				else if ( och >= BM1 ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					s = _i_SCH_MODULE_Get_BM_SIDE( och , Depth );
					p = _i_SCH_MODULE_Get_BM_POINTER( och , Depth );
					w = _i_SCH_MODULE_Get_BM_WAFER( och , Depth );
					//
					_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
					//
					if ( w > 0 ) { // 2012.05.04
						_i_SCH_CLUSTER_Inc_PathDo( s , p ); // 2012.01.18
					}
					//
				}
				else {
					return FALSE;
				}
				//
				//----------------------------------------------------------------
				if ( w <= 0 ) continue;
				//----------------------------------------------------------------
				if ( i == 0 ) {
					_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , 0 );
					_i_SCH_MODULE_Set_TM_TYPE( tms , arm , 0 );
					_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER1( tms , arm , s , p );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) + w );
					//
				}
				else {
					_i_SCH_MODULE_Set_TM_PATHORDER( tms , arm , 0 );
					_i_SCH_MODULE_Set_TM_TYPE( tms , arm , 0 );
					_i_SCH_MODULE_Set_TM_OUTCH( tms , arm , och );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER2( tms , arm , s , p );
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) + ( w * 100 ) );
					//
				}
				//----------------------------------------------------------------
				if      ( ( och == IC ) || ( och == F_IC ) ) {
					_i_SCH_MODULE_Set_MFIC_WAFER( Depth , 0 );
				}
				else if ( ( och == AL ) || ( och == F_AL ) ) {
					_i_SCH_MODULE_Set_MFALS_WAFER( Depth , 0 );
				}
				else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
				}
				else if ( ( och >= PM1 ) && ( och < AL ) ) {
					_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , 0 );
				}
				else if ( och >= BM1 ) {
					_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , 0 , -1 );
				}
				else {
					return FALSE;
				}
				//----------------------------------------------------------------
				//
				Res = TRUE;
				//
				//----------------------------------------------------------------
			}
			else if ( mode == MACRO_PLACE ) {
				//----------------------------------------------------------------
				if ( i == 0 ) {
					s = _i_SCH_MODULE_Get_TM_SIDE( tms , arm );
					p = _i_SCH_MODULE_Get_TM_POINTER( tms , arm );
					w = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100;
				}
				else {
					s = _i_SCH_MODULE_Get_TM_SIDE2( tms , arm );
					p = _i_SCH_MODULE_Get_TM_POINTER2( tms , arm );
					w = _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100;
				}
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
				//----------------------------------------------------------------
				if ( w <= 0 ) continue;
				//----------------------------------------------------------------
				if      ( ( och == IC ) || ( och == F_IC ) ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) continue;
					//
					_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( Depth , s , p );
					_i_SCH_MODULE_Set_MFIC_WAFER( Depth , w );
				}
				else if ( ( och == AL ) || ( och == F_AL ) ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( Depth , s , p , 0 );
					_i_SCH_MODULE_Set_MFALS_WAFER( Depth , w );
				}
				else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
					//
					if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					if ( w > 0 ) { // 2012.05.04
						//
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
						_i_SCH_MODULE_Inc_TM_InCount( s ); // 2012.01.21
						//
					}
					//
				}
				else if ( ( och >= PM1 ) && ( och < AL ) ) {
					//
					if ( ( Depth < 0 ) || ( Depth >= MAX_PM_SLOT_DEPTH  ) ) continue;
					//
					_i_SCH_MODULE_Set_PM_SIDE( och , Depth-1 , s );
					_i_SCH_MODULE_Set_PM_POINTER( och , Depth-1 , p );
					_i_SCH_MODULE_Set_PM_WAFER( och , Depth-1 , w );
				}
				else if ( och >= BM1 ) {
					//
					if ( ( Depth <= 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) continue;
					//
					_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , Depth , s , p );
					_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , Depth , w , 0 );
				}
				else {
					return FALSE;
				}
				//----------------------------------------------------------------
				if ( i == 0 ) {
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) / 100 ) * 100 ) + 0 );
					//
				}
				else {
					//
					_i_SCH_MODULE_Set_TM_WAFER( tms , arm , ( _i_SCH_MODULE_Get_TM_WAFER( tms , arm ) % 100 ) + 0 );
					//
				}
				//----------------------------------------------------------------
				//
				Res = TRUE;
				//
				//----------------------------------------------------------------
			}
			else {
				return FALSE;
			}
		}
	}
	//
	return Res;
}

BOOL _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( BOOL optioncheck , int ch , int mode ) { // 2012.11.21
	//
	if ( optioncheck ) {
		if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 1 ) return TRUE;
	}
	//
	if ( ( ch < BM1 ) || ( ch >= TM ) ) return FALSE;
	//
	_i_SCH_MODULE_Set_BM_FULL_MODE( ch , mode );
	//
	return TRUE;
}
