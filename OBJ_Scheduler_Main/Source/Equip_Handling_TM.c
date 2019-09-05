#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_log.h"
#include "sch_prm_FBTPM.h"
#include "Robot_Animation.h"
#include "Timer_Handling.h"
#include "Equip_Handling.h"
#include "Robot_Handling.h"
#include "sch_estimate.h"
#include "sch_sub.h"

extern int  _SCH_MACRO_TM_ERROR_PICK_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20	0:x 1:ERROR 2:+WaferReturn(+ReRun) 3:+WaferReturn 11:ERROR(onetime) 12:+WaferReturn(+ReRun)(onetime) 13:+WaferReturn(onetime)
extern int  _SCH_MACRO_TM_ERROR_PLACE_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20 0:x 1:ERROR 2:+WaferReturn(+ReRun) 3:+WaferReturn 11:ERROR(onetime) 12:+WaferReturn(+ReRun)(onetime) 13:+WaferReturn(onetime)

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Robot Move Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFINE_EQUIP_ROBOT_MOVE_CONTROL( ZZZ ) int EQUIP_ROBOT_MOVE_CONTROL_##ZZZ##( int Mode , int CHECKING_SIDE , int Chamber , int Finger , int Slot , int Depth ) { \
	int timex = 100; \
	int Mt , Mr; \
	BOOL TM_Run; \
	int  Rb_Run; \
	char ParamT[64] , Buffer[64]; \
	long Mon_Goal; \
	BOOL update; \
	int l_c; \
\
	long Sim_Goal; /* 2016.12.09 */ \
\
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , TRUE ); /* 2012.10.31 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_START\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d\n" , "EQUIP_ROBOT_MOVE_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth ); \
	} \
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(1) > 0 ) ) { \
		TM_Run = FALSE; \
		Sim_Goal = Clock_Goal_Get_Start_Time(); /* 2016.12.09 */ \
	} \
	else { \
		TM_Run = TRUE; \
	} \
	/* 2003.01.23 (Change like below) if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( Chamber ) && _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ) ) \
		Rb_Run = TRUE; \
	else \
		Rb_Run = FALSE; \ */ \
/*	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ); */ /* 2010.01.27 */ \
	if ( Chamber == AL || Chamber == IC || Chamber == F_AL || Chamber == F_IC ) { /* 2011.07.25 */ \
		Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ); \
	} \
	else { \
		if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( Chamber ) != 2 ) /* 2010.01.27 */ \
			Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ); \
		else \
			Rb_Run = -1; \
	} \
	switch ( Mode ) { \
	case 0 :	strcpy( ParamT , "MOVE_RECV" );	break; \
	case 1 :	strcpy( ParamT , "MOVE_SEND" );	break; \
	default :	strcpy( ParamT , "ROTATE_WAFER" );	break; \
	} \
	\
	if ( Chamber == AL || Chamber == IC || Chamber == F_AL || Chamber == F_IC ) { /* 2011.07.25 */ \
		sprintf( Buffer , " %c_%s %d" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Depth ); \
	} \
	else { \
		if      ( Chamber <  PM1 ) sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot ); \
		else if ( Chamber >= BM1 ) sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth ); \
		else                       sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Depth ); \
	} \
	strcat( ParamT , Buffer ); \
	if ( !ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( ##ZZZ## , RB_MODE_ROTATE , Chamber , Finger , Slot % 100 , Slot / 100 , Depth % 100 , Depth / 100 , ( _i_SCH_PRM_GET_RB_SYNCH_CHECK(##ZZZ##) && TM_Run ) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE(##ZZZ##) , FALSE , ( Rb_Run == 1 ) ) ) { \
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
		return SYS_ABORTED; \
	} \
	if ( Rb_Run == 1 ) { \
		if ( TM_Run ) { \
			if ( _dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) ) { \
				Mt = SYS_RUNNING; \
			} \
			else { \
				Mt = SYS_SUCCESS; \
			} \
		} \
		else { \
			Mt = SYS_SUCCESS; \
		} \
		while ( TRUE ) { \
			_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
			Mon_Goal = Clock_Goal_Get_Start_Time(); \
			if ( TM_Run && ( Mt == SYS_RUNNING ) ) { \
				Mt = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ); \
			} \
			if ( TM_Run ) { \
				if ( Rb_Run == 1 ) { \
					if ( Mt != SYS_ABORTED ) { /* 2005.11.08 */ \
						if ( Mt != SYS_RUNNING ) Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , TRUE , &update ); \
						else                     Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ); \
						if ( update ) _i_SCH_IO_MTL_SAVE(); \
						if ( Mr == RB_ROTATEDONE ) Rb_Run = 0; \
					} \
				} \
				else { \
					Mr = RB_ROTATEDONE; \
				} \
			} \
			else { \
				if ( _SCH_Get_SIM_TIME3( CHECKING_SIDE , TM + ##ZZZ## ) > 0 ) { \
					if ( Clock_Goal_Data_Check( &Sim_Goal , _SCH_Get_SIM_TIME3( CHECKING_SIDE , TM + ##ZZZ## ) ) ) { \
						Mt = SYS_SUCCESS; \
					} \
					else { \
						Mt = SYS_RUNNING; \
					} \
					if ( Rb_Run == 1 ) { \
						if ( Mt != SYS_ABORTED ) { /* 2005.11.08 */ \
							if ( Mt != SYS_RUNNING ) Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , TRUE , &update ); \
							else                     Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ); \
							if ( update ) _i_SCH_IO_MTL_SAVE(); \
							if ( Mr == RB_ROTATEDONE ) Rb_Run = 0; \
						} \
					} \
					else { \
						Mr = RB_ROTATEDONE; \
					} \
				} \
				else { \
					Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ); \
					if ( update ) _i_SCH_IO_MTL_SAVE(); \
				} \
			} \
			if ( Mt != SYS_RUNNING ) { \
				if ( Mt == SYS_ABORTED ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ABORTED; \
				} \
				else { \
					if ( Mr == RB_ROTATEDONE ) break; \
				} \
			} \
			l_c = 0; \
			do { \
				if ( MANAGER_ABORT() ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ABORTED; \
				} \
				if ( TM_Run && ( Mt == SYS_RUNNING ) ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						break; \
					} \
				} \
				switch( GET_RUN_LD_CONTROL(0) ) { \
				case 1 : \
					timex = 0; \
					break; \
				case 2 : \
					timex = 20; \
					break; \
				} \
				if ( ( l_c % 10 ) == 0 ) Sleep(1); \
				l_c++; \
			} \
			while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );\
		} \
	} \
	else if ( TM_Run ) { \
		if ( _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) == SYS_ABORTED ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
		while ( TRUE ) { \
			if ( RB_ROTATEDONE == ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , Rb_Run , FALSE , &update ) ) break; \
		} \
		_i_SCH_IO_MTL_SAVE(); \
	} \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_END\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d\n" , "EQUIP_ROBOT_MOVE_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth ); \
	} \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
	return SYS_SUCCESS; \
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Utility Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFINE_EQUIP_UTILITY_CONTROL( ZZZ ) int EQUIP_UTILITY_CONTROL_##ZZZ##( int Mode , int Finger , int Slot , int SourceCassette , int AvtiveSlot ) { \
	char Buffer[64]; \
	int Res; \
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_START\t%s_%d\tMode=%d,Finger=%d,Slot=%d,SourceCassette=%d,AvtiveSlot=%d\n" , "EQUIP_UTILITY_CONTROL" , ##ZZZ## , Mode , Finger , Slot , SourceCassette , AvtiveSlot ); \
	} \
	if ( Mode == 0 ) { \
		if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) )	return SYS_SUCCESS; \
		sprintf( Buffer , "ALIGN_WAFER %c_AL %d CM%d %d" , Finger - TA_STATION + 'A' , Slot , SourceCassette - CM1 + 1 , AvtiveSlot ); \
	} \
	else { \
		if ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) || ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) ) { /* 2010.12.22 */ \
			if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) )	return SYS_SUCCESS; \
			sprintf( Buffer , "ALIGN_WAFER %c_AL %d CM%d %d - OUT" , Finger - TA_STATION + 'A' , Slot , SourceCassette - CM1 + 1 , AvtiveSlot ); \
		} \
		else { \
			if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(6) > 0 ) )	return SYS_SUCCESS; \
			sprintf( Buffer , "COOL_WAFER %c_IC %d CM%d %d" , Finger - TA_STATION + 'A' , Slot , SourceCassette - CM1 + 1 , AvtiveSlot ); \
		} \
	} \
	Res = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , Buffer ); \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_END\t%s_%d\tMode=%d,Finger=%d,Slot=%d,SourceCassette=%d,AvtiveSlot=%d Res=%d\n" , "EQUIP_UTILITY_CONTROL" , ##ZZZ## , Mode , Finger , Slot , SourceCassette , AvtiveSlot , Res ); \
	} \
	return Res; \
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Gate Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFINE_EQUIP_GATE_CONTROL( ZZZ ) int EQUIP_GATE_CONTROL_##ZZZ##( int Mode , int Chamber , int Finger , int Slot , int Depth , int Switch , int Transfer , int SrcCas , int order , BOOL pmdual , BOOL MaintInfUse ) { \
	int Res; \
	char Param[64] , Buffer[64]; \
	char MsgAppchar[2]; /* 2013.04.26 */ \
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
	\
/*	if ( Chamber == AL || Chamber == IC ) return SYS_SUCCESS; */ /* 2011.07.25 */ \
	\
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_START\t%s_%d\tMode=%d,Chamber=%d,Slot=%d,Depth=%d,Switch=%d,Transfer=%d,SrcCas=%d,order=%d\n" , "EQUIP_GATE_CONTROL" , ##ZZZ## , Mode , Chamber , Slot , Depth , Switch , Transfer , SrcCas , order ); \
	} \
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) && ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] >= 0 ) && ( GET_RUN_LD_CONTROL(1) <= 0 ) ) { \
		\
		if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
			if ( EQUIP_MESAGE_NOTUSE_GATE( MaintInfUse , TM + ##ZZZ## , Chamber ) ) return SYS_SUCCESS; /* 2013.04.26 */ \
		} \
		else { \
			if ( EQUIP_MESAGE_NOTUSE_GATE( MaintInfUse , TM + ##ZZZ## , -1 ) ) return SYS_SUCCESS; /* 2013.04.26 */ \
		} \
		\
		EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */ \
		\
		switch ( Mode ) { \
		case GATE_MODE_OPEN :		sprintf( Param , "OPEN_GATE%s" , MsgAppchar );		break; \
		case GATE_MODE_CLOSE :		sprintf( Param , "CLOSE_GATE%s" , MsgAppchar );		break; \
		} \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) { /* 2011.07.25 */ \
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( TM + ##ZZZ## ) == 2 ) { \
				sprintf( Buffer , " %c_%s %d" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM ( Chamber ) , Depth ); \
			} \
			else { \
				sprintf( Buffer , " %s %d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Depth ); \
			} \
		} \
		else { \
			if      ( Chamber <  PM1 ) { \
				if ( ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( TM + ##ZZZ## ) == 2 ) || ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( Chamber ) == 2 ) ) { \
					sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot ); \
				} \
				else { \
					sprintf( Buffer , " CM%d %d" , Chamber - CM1 + 1 , Slot ); \
				} \
			} \
			else if ( Chamber >= BM1 ) { \
				if ( ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( TM + ##ZZZ## ) == 2 ) || ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( Chamber ) == 2 ) ) { \
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
						sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
					} \
					else { \
						sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
					} \
				} \
				else { \
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
						sprintf( Buffer , " BM%d %d" , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
					} \
					else { \
						sprintf( Buffer , " BM%d %d" , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
					} \
				} \
			} \
			else { \
				if ( ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( TM + ##ZZZ## ) == 2 ) || ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( Chamber ) == 2 ) ) { \
					sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Depth ); \
				} \
				else { \
					sprintf( Buffer , " PM%d %d" , Chamber - PM1 + 1 , Depth ); \
				} \
			} \
		} \
		strcat( Param , Buffer ); \
		\
		if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() != 12 ) { /* 2018.04.06 */ \
			if      ( Switch == 1 ) strcat( Param , " SWITCH" ); \
			else if ( Switch == 2 ) strcat( Param , " SWITCH2" ); \
			else if ( Switch == 3 ) strcat( Param , " SWITCH3" ); \
			else if ( Switch == 4 ) strcat( Param , " SWITCH4" ); \
			else if ( Switch == 5 ) strcat( Param , " SWITCH5" ); \
			else if ( Switch == 6 ) strcat( Param , " SWITCH6" ); \
			else if ( Switch == 7 ) strcat( Param , " SWITCH7" ); \
			else if ( Switch == 8 ) strcat( Param , " SWITCH8" ); \
			else if ( Switch == 9 ) strcat( Param , " SWITCH9" ); \
		} \
		\
		EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , Param ); \
		if ( order > 0 ) { \
			sprintf( Buffer , " ORDER%d" , order ); \
			strcat( Param , Buffer ); \
		} \
		Res = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , Param ); \
	} \
	else { \
		Res = SYS_SUCCESS; \
	} \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_END\t%s_%d\tMode=%d,Chamber=%d,Slot=%d,Depth=%d,Switch=%d,Transfer=%d,SrcCas=%d,order=%d Res=%d\n" , "EQUIP_GATE_CONTROL" , ##ZZZ## , Mode , Chamber , Slot , Depth , Switch , Transfer , SrcCas , order , Res ); \
	} \
	return Res; \
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Interface Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFINE_EQUIP_INTERFACE_CONTROL( ZZZ ) int EQUIP_INTERFACE_CONTROL_##ZZZ##( int Mode , int Chamber , int Finger , int Slot , int Depth , int Switch , int Transfer , BOOL TMLikeCM , int SrcCas , int order , BOOL pmdual , BOOL MaintInfUse ) { \
	int timex = 100; \
	int Mp1 , Mp2 , Mt , Mr; \
	BOOL PM_Run1 , PM_Run2 , TM_Run; \
	int Rb_Run; \
	char ParamT[64] , ParamP[64] , Buffer[64]; \
	char MsgAppchar[2]; /* 2013.04.26 */ \
	long Mon_Goal; \
	BOOL update; \
	int l_c; \
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
\
	/* if ( Chamber == AL || Chamber == IC ) return SYS_SUCCESS; */ /* 2011.07.25 */ \
\
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_START\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,Transfer=%d,TMLikeCM=%d,SrcCas=%d,order=%d\n" , "EQUIP_INTERFACE_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth , Switch , Transfer , TMLikeCM , SrcCas , order ); \
	} \
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(1) > 0 ) ) TM_Run = FALSE; \
	else { \
		if ( Mode >= HO_PICK_PREPARE_MDL ) TM_Run = FALSE; \
		else                               TM_Run = TRUE; \
	} \
\
	if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) { /* 2011.07.25 */ \
		PM_Run1 = FALSE; \
		PM_Run2 = FALSE; \
		Rb_Run = -1; \
	} \
	else { \
		if ( Chamber >= BM1 ) { \
			if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(3) > 0 ) ) PM_Run1 = FALSE; \
			else																					             PM_Run1 = TRUE; \
\
			if ( pmdual ) { /* 2008.02.15 */ \
				if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber + 1 ) || ( Address_FUNCTION_INTERFACE[Chamber+1] < 0 ) || ( GET_RUN_LD_CONTROL(3) > 0 ) )	PM_Run2 = FALSE; \
				else																							            PM_Run2 = TRUE; \
			} \
			else { \
				PM_Run2 = FALSE; \
			} \
		} \
		else { \
			if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(2) > 0 ) ) PM_Run1 = FALSE; \
			else																					             PM_Run1 = TRUE; \
\
			if ( pmdual ) { /* 2008.02.15 */ \
				if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber + 1 ) || ( Address_FUNCTION_INTERFACE[Chamber+1] < 0 ) || ( GET_RUN_LD_CONTROL(2) > 0 ) )	PM_Run2 = FALSE; \
				else																							            PM_Run2 = TRUE; \
			} \
			else { \
				PM_Run2 = FALSE; \
			} \
		} \
\
		if ( ( Mode == HO_PICK_PREPARE ) || ( Mode == HO_PLACE_PREPARE ) ) { \
			/* if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( Chamber ) ) */ /* 2010.01.27 */ \
			if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( Chamber ) == 1 ) /* 2010.01.27 */ \
				Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ); \
			else \
				Rb_Run = -1; \
		} \
		else { \
			Rb_Run = -1; \
		} \
	} \
\
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */ \
\
	switch ( Mode ) { \
	case HO_PICK_PREPARE : \
		sprintf( ParamT , "PREPARE_RECV%s" , MsgAppchar ); \
		sprintf( ParamP , "PREPARE_SEND%s" , MsgAppchar ); \
		\
		if ( EQUIP_MESAGE_NOTUSE_PREPRECV( MaintInfUse , TM + ##ZZZ## , -1 ) ) TM_Run = FALSE; /* 2013.04.26 */ \
		\
		if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
			\
			if ( EQUIP_MESAGE_NOTUSE_PREPSEND( MaintInfUse , -1 , Chamber ) ) PM_Run1 = FALSE; /* 2013.04.26 */ \
			\
			if ( PM_Run2 ) { /* 2008.02.15 */ \
				\
				if ( EQUIP_MESAGE_NOTUSE_PREPSEND( MaintInfUse , -1 , Chamber + 1 ) ) PM_Run2 = FALSE; /* 2013.04.26 */ \
				\
			} \
		} \
		break; \
	case HO_PICK_POST : \
		sprintf( ParamT , "POST_RECV%s" , MsgAppchar ); \
		sprintf( ParamP , "POST_SEND%s" , MsgAppchar ); \
		\
		if ( EQUIP_MESAGE_NOTUSE_POSTRECV( MaintInfUse , TM + ##ZZZ## , -1 ) ) TM_Run = FALSE; /* 2013.04.26 */ \
		\
		if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
			\
			if ( EQUIP_MESAGE_NOTUSE_POSTSEND( MaintInfUse , -1 , Chamber ) ) PM_Run1 = FALSE; /* 2013.04.26 */ \
			\
			if ( PM_Run2 ) { /* 2008.02.15 */ \
				\
				if ( EQUIP_MESAGE_NOTUSE_POSTSEND( MaintInfUse , -1 , Chamber + 1 ) ) PM_Run2 = FALSE; /* 2013.04.26 */ \
				\
			} \
		} \
		break; \
	case HO_PLACE_PREPARE : \
		sprintf( ParamT , "PREPARE_SEND%s" , MsgAppchar ); \
		sprintf( ParamP , "PREPARE_RECV%s" , MsgAppchar ); \
		\
		if ( EQUIP_MESAGE_NOTUSE_PREPSEND( MaintInfUse , TM + ##ZZZ## , -1 ) ) TM_Run = FALSE; /* 2013.04.26 */ \
		\
		if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
			\
			if ( EQUIP_MESAGE_NOTUSE_PREPRECV( MaintInfUse , -1 , Chamber ) ) PM_Run1 = FALSE; /* 2013.04.26 */ \
			\
			if ( PM_Run2 ) { /* 2008.02.15 */ \
				\
				if ( EQUIP_MESAGE_NOTUSE_PREPRECV( MaintInfUse , -1 , Chamber + 1 ) ) PM_Run2 = FALSE; /* 2013.04.26 */ \
				\
			} \
		} \
		break; \
	case HO_PLACE_POST : \
		sprintf( ParamT , "POST_SEND%s" , MsgAppchar ); \
		sprintf( ParamP , "POST_RECV%s" , MsgAppchar ); \
		\
		if ( EQUIP_MESAGE_NOTUSE_POSTSEND( MaintInfUse , TM + ##ZZZ## , -1 ) ) TM_Run = FALSE; /* 2013.04.26 */ \
		\
		if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
			\
			if ( EQUIP_MESAGE_NOTUSE_POSTRECV( MaintInfUse , -1 , Chamber ) ) PM_Run1 = FALSE; /* 2013.04.26 */ \
			\
			if ( PM_Run2 ) { /* 2008.02.15 */ \
				\
				if ( EQUIP_MESAGE_NOTUSE_POSTRECV( MaintInfUse , -1 , Chamber + 1 ) ) PM_Run2 = FALSE; /* 2013.04.26 */ \
				\
			} \
		} \
		break; \
	case HO_PICK_PREPARE_MDL : \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) return SYS_SUCCESS; /* 2011.07.25 */ \
		sprintf( ParamP , "PREPARE_PICK%s" , MsgAppchar ); \
		break; \
	case HO_PICK_MTL_MDL : \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) return SYS_SUCCESS; /* 2011.07.25 */ \
		sprintf( ParamP , "WAFER_PICK%s" , MsgAppchar ); \
		break; \
	case HO_PLACE_PREPARE_MDL: \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) return SYS_SUCCESS; /* 2011.07.25 */ \
		sprintf( ParamP , "PREPARE_PLACE%s" , MsgAppchar ); \
		break; \
	case HO_PLACE_MTL_MDL : \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) return SYS_SUCCESS; /* 2011.07.25 */ \
		sprintf( ParamP , "WAFER_PLACE%s" , MsgAppchar ); \
		break; \
	} \
/*	if      ( Chamber <  PM1 ) sprintf( Buffer , " %d %c/%d/1/%d" , Slot , Finger - TA_STATION + 'A' , SrcCas , Slot ); */ /* 2006.07.03 */ \
/*	else if ( Chamber >= BM1 ) sprintf( Buffer , " %d %c/%d/1/%d" , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) , Finger - TA_STATION + 'A' , SrcCas , Slot ); */ /* 2006.07.03 */ \
/*	else                       sprintf( Buffer , " %d %c/%d/1/%d" , Depth , Finger - TA_STATION + 'A' , SrcCas , Slot ); */ /* 2006.07.03 */ \
	if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( Chamber != AL ) && ( Chamber != IC ) ) { /* 2011.07.25 */ \
		if      ( Chamber <  PM1 ) { \
			sprintf( Buffer , " %d %c/%d/%d/%d" , Slot , Finger - TA_STATION + 'A' , SrcCas , ##ZZZ## + 1 , Slot ); /* 2006.07.03 */ \
		} \
		else if ( Chamber >= BM1 ) { \
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
				sprintf( Buffer , " %d %c/%d/%d/%d" , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) , Finger - TA_STATION + 'A' , SrcCas , ##ZZZ## + 1 , Slot ); /* 2006.07.03 */ \
			} \
			else { \
				sprintf( Buffer , " %d %c/%d/%d/%d" , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) , Finger - TA_STATION + 'A' , SrcCas , ##ZZZ## + 1 , Slot ); /* 2006.07.03 */ \
			} \
		} \
		else { \
			sprintf( Buffer , " %d %c/%d/%d/%d" , Depth , Finger - TA_STATION + 'A' , SrcCas , ##ZZZ## + 1 , Slot ); /* 2006.07.03 */ \
		} \
		strcat( ParamP , Buffer ); \
		if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() != 12 ) { /* 2018.04.06 */ \
			if      ( Switch == 1 ) strcat( ParamP , " SWITCH" ); \
			else if ( Switch == 2 ) strcat( ParamP , " SWITCH2" ); \
			else if ( Switch == 3 ) strcat( ParamP , " SWITCH3" ); \
			else if ( Switch == 4 ) strcat( ParamP , " SWITCH4" ); \
			else if ( Switch == 5 ) strcat( ParamP , " SWITCH5" ); \
			else if ( Switch == 6 ) strcat( ParamP , " SWITCH6" ); \
			else if ( Switch == 7 ) strcat( ParamP , " SWITCH7" ); \
			else if ( Switch == 8 ) strcat( ParamP , " SWITCH8" ); \
			else if ( Switch == 9 ) strcat( ParamP , " SWITCH9" ); \
		} \
		\
		EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , ParamP ); \
		if ( order > 0 ) { \
			sprintf( Buffer , " ORDER%d" , order ); \
			strcat( ParamP , Buffer ); \
		} \
	} \
\
	if ( TMLikeCM ) { \
/*		if      ( Chamber <  PM1 ) sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot ); */ /* 2011.07.25 */ \
/*		else if ( Chamber >= BM1 ) sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Slot ); */ /* 2011.07.25 */ \
/*		else                       sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Slot ); */ /* 2011.07.25 */ \
		sprintf( Buffer , " %c_%s %d" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot ); /* 2011.07.25 */ \
	} \
	else { \
		if ( Chamber == F_AL || Chamber == F_IC || Chamber == AL || Chamber == IC ) { /* 2011.07.25 */ \
			sprintf( Buffer , " %c_%s %d" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Depth ); \
		} \
		else { \
			if      ( Chamber <  PM1 ) { \
				sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot ); \
			} \
			else if ( Chamber >= BM1 ) { \
				if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
					sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
				} \
				else { \
					sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
				} \
			} \
			else { \
				sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Depth ); \
			} \
		} \
	} \
	strcat( ParamT , Buffer ); \
	if ( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() != 12 ) { /* 2018.04.06 */ \
		if      ( Switch == 1 ) strcat( ParamT , " SWITCH" ); \
		else if ( Switch == 2 ) strcat( ParamT , " SWITCH2" ); \
		else if ( Switch == 3 ) strcat( ParamT , " SWITCH3" ); \
		else if ( Switch == 4 ) strcat( ParamT , " SWITCH4" ); \
		else if ( Switch == 5 ) strcat( ParamT , " SWITCH5" ); \
		else if ( Switch == 6 ) strcat( ParamT , " SWITCH6" ); \
		else if ( Switch == 7 ) strcat( ParamT , " SWITCH7" ); \
		else if ( Switch == 8 ) strcat( ParamT , " SWITCH8" ); \
		else if ( Switch == 9 ) strcat( ParamT , " SWITCH9" ); \
	} \
	\
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , ParamT ); \
	if ( order > 0 ) { \
		sprintf( Buffer , " ORDER%d" , order ); \
		strcat( ParamT , Buffer ); \
	} \
	if ( Rb_Run == 1 ) { \
\
		if ( !ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( ##ZZZ## , RB_MODE_ROTATE , Chamber , Finger , Slot % 100 , Slot / 100 , Depth % 100 , Depth / 100 , ( _i_SCH_PRM_GET_RB_SYNCH_CHECK(##ZZZ##) && TM_Run ) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE(##ZZZ##) , FALSE , TRUE ) ) return SYS_ABORTED; /* 2005.11.08 */ \
\
		if ( TM_Run ) { \
			Mt = SYS_RUNNING; \
			if      ( PM_Run1 && PM_Run2 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) ) Mt = SYS_ABORTED; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mp1 = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run1 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT , Address_FUNCTION_INTERFACE[Chamber] , ParamP ) ) { \
					Mt = SYS_ABORTED; \
					Mp1 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run2 ) { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mt = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) ) Mt = SYS_ABORTED; \
			} \
		} \
		else { \
			Mt = SYS_SUCCESS; \
			if      ( PM_Run1 && PM_Run2 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mp1 = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run1 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP ) ) Mp1 = SYS_ABORTED; \
			} \
			else if ( PM_Run2 ) { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) Mp2 = SYS_ABORTED; \
			} \
			else { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_SUCCESS; \
			} \
		} \
		while ( TRUE ) { \
			_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
			Mon_Goal = Clock_Goal_Get_Start_Time(); \
			if ( Mt == SYS_RUNNING ) { \
				Mt = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ); \
			} \
			if ( Mp1 == SYS_RUNNING ) { \
				Mp1 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] ); \
			} \
			if ( Mp2 == SYS_RUNNING ) { \
				Mp2 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber+1] ); \
			} \
			if ( TM_Run ) { \
				if ( Rb_Run ) { \
					if ( Mt == SYS_ABORTED ) { /* 2005.11.08 */ \
						Rb_Run = FALSE; \
					} \
					else { \
						if ( Mt != SYS_RUNNING ) Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , TRUE , &update ); \
						else                     Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ); \
						if ( update ) _i_SCH_IO_MTL_SAVE(); \
						if ( Mr == RB_ROTATEDONE ) Rb_Run = FALSE; \
					} \
				} \
				else { \
					Mr = RB_ROTATEDONE; \
				} \
			} \
			else { \
				Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ); \
				if ( update ) _i_SCH_IO_MTL_SAVE(); \
			} \
			if ( ( Mt != SYS_RUNNING ) && ( Mp1 != SYS_RUNNING ) && ( Mp2 != SYS_RUNNING ) ) { \
				if ( ( Mode == HO_PICK_PREPARE ) || ( Mode == HO_PLACE_PREPARE ) ) { /* 2005.11.24 */ \
					if ( ( Mt == SYS_ABORTED ) || ( Mp1 == SYS_ABORTED ) || ( Mp2 == SYS_ABORTED ) ) return SYS_ABORTED; \
					else if ( Mt  == SYS_ERROR ) return SYS_ERROR; /* 2005.11.24 */  \
					else if ( Mp1 == SYS_ERROR ) return SYS_ERROR; /* 2005.11.24 */  \
					else if ( Mp2 == SYS_ERROR ) return SYS_ERROR; /* 2008.02.15 */  \
					else { \
						if ( Mr == RB_ROTATEDONE ) break; \
					} \
				} /* 2005.11.24 */  \
				else { \
					if ( ( Mt == SYS_ABORTED ) || ( Mp1 == SYS_ABORTED ) || ( Mp2 == SYS_ABORTED ) ) return SYS_ABORTED; \
					else { \
						if ( Mr == RB_ROTATEDONE ) break; \
					} \
				} \
			} \
			l_c = 0; \
			do { \
\
				if ( MANAGER_ABORT() ) return SYS_ABORTED; \
\
				if ( ( Mt == SYS_RUNNING ) && ( Mp1 == SYS_RUNNING ) && ( Mp2 == SYS_RUNNING ) ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber ] ) != SYS_RUNNING ) { \
							if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber + 1 ] ) != SYS_RUNNING ) { \
								break; \
							} \
						} \
					} \
				} \
				else if ( ( Mt == SYS_RUNNING ) && ( Mp1 == SYS_RUNNING ) ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber ] ) != SYS_RUNNING ) { \
							break; \
						} \
					} \
				} \
				else if ( ( Mt == SYS_RUNNING ) && ( Mp2 == SYS_RUNNING ) ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber + 1 ] ) != SYS_RUNNING ) { \
							break; \
						} \
					} \
				} \
				else if ( ( Mp1 == SYS_RUNNING ) && ( Mp2 == SYS_RUNNING ) ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber ] ) != SYS_RUNNING ) { \
						if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber + 1 ] ) != SYS_RUNNING ) { \
							break; \
						} \
					} \
				} \
				else if ( Mt == SYS_RUNNING ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						break; \
					} \
				} \
				else if ( Mp1 == SYS_RUNNING ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber ] ) != SYS_RUNNING ) { \
						break; \
					} \
				} \
				else if ( Mp2 == SYS_RUNNING ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ Chamber + 1 ] ) != SYS_RUNNING ) { \
						break; \
					} \
				} \
				switch( GET_RUN_LD_CONTROL(0) ) { \
				case 1 : \
					timex = 0; \
					break; \
				case 2 : \
					timex = 20; \
					break; \
				} \
				if ( ( l_c % 10 ) == 0 ) Sleep(1); \
				l_c++; \
			} \
			while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && Rb_Run ); \
		} \
	} \
	else { \
\
		if ( Rb_Run != -1 ) { \
			if ( !ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( ##ZZZ## , RB_MODE_ROTATE , Chamber , Finger , Slot % 100 , Slot / 100 , Depth % 100 , Depth / 100 , ( _i_SCH_PRM_GET_RB_SYNCH_CHECK(##ZZZ##) && TM_Run ) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE(##ZZZ##) , FALSE , FALSE ) ) return SYS_ABORTED; \
		} \
\
		if ( TM_Run ) { \
			Mt = SYS_RUNNING; \
			if      ( PM_Run1 && PM_Run2 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) ) Mt = SYS_ABORTED; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mp1 = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run1 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT , Address_FUNCTION_INTERFACE[Chamber] , ParamP ) ) { \
					Mt = SYS_ABORTED; \
					Mp1 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run2 ) { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mt = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , ParamT ) ) Mt = SYS_ABORTED; \
			} \
		} \
		else { \
			Mt = SYS_SUCCESS; \
			if      ( PM_Run1 && PM_Run2 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP , Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) { \
					Mp1 = SYS_ABORTED; \
					Mp2 = SYS_ABORTED; \
				} \
			} \
			else if ( PM_Run1 ) { \
				Mp1 = SYS_RUNNING; \
				Mp2 = SYS_SUCCESS; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP ) ) Mp1 = SYS_ABORTED; \
			} \
			else if ( PM_Run2 ) { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_RUNNING; \
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber+1] , ParamP ) ) Mp2 = SYS_ABORTED; \
			} \
			else { \
				Mp1 = SYS_SUCCESS; \
				Mp2 = SYS_SUCCESS; \
			} \
		} \
		l_c = 0; \
		while ( TRUE ) { \
			if ( Mt == SYS_RUNNING ) { \
				Mt = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ); \
			} \
			if ( Mp1 == SYS_RUNNING ) { \
				Mp1 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] ); \
			} \
			if ( Mp2 == SYS_RUNNING ) { \
				Mp2 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber+1] ); \
			} \
			if ( ( Mt != SYS_RUNNING ) && ( Mp1 != SYS_RUNNING ) && ( Mp2 != SYS_RUNNING ) ) { \
				/* if ( ( Mode == HO_PICK_PREPARE ) || ( Mode == HO_PLACE_PREPARE ) ) { */ /* 2005.11.24 */ /* 2008.12.12 */ \
					if ( ( Mt == SYS_ABORTED ) || ( Mp1 == SYS_ABORTED ) || ( Mp2 == SYS_ABORTED ) ) return SYS_ABORTED; \
					else if ( Mt  == SYS_ERROR ) return SYS_ERROR; /* 2005.11.24 */  \
					else if ( Mp1 == SYS_ERROR ) return SYS_ERROR; /* 2005.11.24 */  \
					else if ( Mp2 == SYS_ERROR ) return SYS_ERROR; /* 2008.02.15 */  \
					else \
						break; \
				/* } */ /* 2005.11.24 */ /* 2008.12.12 */ \
				/* else { */ /* 2008.12.12 */ \
				/*	if ( ( Mt == SYS_ABORTED ) || ( Mp1 == SYS_ABORTED ) || ( Mp2 == SYS_ABORTED ) ) */ /* 2008.12.12 */ \
				/*		return SYS_ABORTED; */ /* 2008.12.12 */ \
				/*	else */ /* 2008.12.12 */ \
				/*		break; */ /* 2008.12.12 */ \
				/* } */ /* 2008.12.12 */ \
			} \
			if ( MANAGER_ABORT() ) return SYS_ABORTED; \
			if ( ( l_c % 10 ) == 0 ) Sleep(1); \
			l_c++; \
		} \
		if ( Rb_Run != -1 ) { \
			while ( TRUE ) { \
				if ( RB_ROTATEDONE == ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , Rb_Run , FALSE , &update ) ) break; \
			} \
			_i_SCH_IO_MTL_SAVE(); \
		} \
	} \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_END\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,Transfer=%d,TMLikeCM=%d,SrcCas=%d,order=%d\n" , "EQUIP_INTERFACE_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth , Switch , Transfer , TMLikeCM , SrcCas , order ); \
	} \
	return SYS_SUCCESS; \
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Robot Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  EQUIP_TM_ROBOT_FLAG[MAX_TM_CHAMBER_DEPTH]; // 2017.03.20

#define DEFINE_EQUIP_ROBOT_CONTROL( ZZZ ) int EQUIP_ROBOT_CONTROL_##ZZZ##( int Mode , int CHECKING_SIDE , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order , BOOL MaintInfUse ) { \
	Module_Status Ms; \
	int Mr , MRes , timex = 100 , RMode , RChamber , RSlot , RDepth; \
	char Param[256] , Buffer[16]; \
	char MsgAppchar[2]; /* 2013.04.26 */ \
	BOOL e; \
	BOOL TM_Run; \
	int Rb_Run; \
	long Mon_Goal; \
	BOOL update; \
	int l_c; \
\
	long Sim_Goal; /* 2016.12.09 */ \
	int  Sim_Trg; /* 2016.12.09 */ \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 1; /* 2017.03.20 */ \
\
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_START\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Transfer=%d,SrcCas=%d,order=%d\n" , "EQUIP_ROBOT_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth , Transfer , SrcCas , order ); \
	} \
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + ##ZZZ## ) || ( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] < 0 ) || ( GET_RUN_LD_CONTROL(1) > 0 ) ) { \
		TM_Run = FALSE; \
\
		switch( Mode ) { \
		case RB_MODE_PICK :			Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , TM + ##ZZZ## );	break; \
		case RB_MODE_PLACE :		Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , TM + ##ZZZ## );	break; \
		case RB_MODE_PICKPLACE :	Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , TM + ##ZZZ## ) + _SCH_Get_SIM_TIME2( CHECKING_SIDE , TM + ##ZZZ## );	break; \
		case RB_MODE_ROTATE :		Sim_Trg = _SCH_Get_SIM_TIME3( CHECKING_SIDE , TM + ##ZZZ## );	break; \
		default :					Sim_Trg = 0;							break; \
		} \
\
		if ( Sim_Trg > 0 ) { \
			Sim_Goal = Clock_Goal_Get_Start_Time(); \
			timex = Sim_Trg / 20; \
			if ( timex <= 0 ) timex = 1; \
		} \
	} \
	else { \
		Sim_Trg = 0; /* 2017.03.24 */ \
		TM_Run = TRUE; \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 2; /* 2017.03.20 */ \
\
	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( ##ZZZ## ); \
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */ \
	switch( Mode ) { \
	case RB_MODE_PICK :		ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PICK , Finger , Chamber );	sprintf( Param , "PICK_WAFER%s" , MsgAppchar );		break; \
	case RB_MODE_PLACE :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PLACE , Finger , Chamber );	sprintf( Param , "PLACE_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_RETPIC :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PICK , Finger , Chamber );	sprintf( Param , "RETPIC_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_EXTPIC :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PICK , Finger , Chamber );	sprintf( Param , "EXTPIC_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_RETPLC :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PLACE , Finger , Chamber );	sprintf( Param , "RETPLC_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_EXTPLC :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PLACE , Finger , Chamber );	sprintf( Param , "EXTPLC_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_EXTEND :	sprintf( Param , "EXTEND_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_RETRACT :	sprintf( Param , "RETRACT_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_HOME :		sprintf( Param , "HOME_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_UP :		sprintf( Param , "UP_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_DOWN :		sprintf( Param , "DOWN_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_ROTATE :	sprintf( Param , "ROTATE_WAFER%s" , MsgAppchar );	break; \
	case RB_MODE_PICKPLACE :	ROBOT_LAST_ACTION_CONTROL( ##ZZZ## , MACRO_PLACE , _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( Finger ) , Chamber );	sprintf( Param , "SWAP_WAFER%s" , MsgAppchar );	break; /* 2013.01.11 */ \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 3; /* 2017.03.20 */ \
\
	RMode = Mode; \
	RChamber = Chamber; \
	RSlot    = Slot; \
	RDepth   = Depth; \
	if ( AnimChange ) { \
		RChamber = AnimChamber; \
		RSlot    = AnimSlot; \
		RDepth   = RSlot; \
		switch( Mode ) { \
		case RB_MODE_PICK :		RMode = RB_MODE_PLACE;	break; \
		case RB_MODE_PLACE :	RMode = RB_MODE_PICK;	break; \
		case RB_MODE_RETPIC :	RMode = RB_MODE_RETPLC;	break; \
		case RB_MODE_EXTPIC :	RMode = RB_MODE_EXTPLC;	break; \
		case RB_MODE_RETPLC :	RMode = RB_MODE_RETPIC;	break; \
		case RB_MODE_EXTPLC :	RMode = RB_MODE_EXTPIC;	break; \
		case RB_MODE_PICK_UPDATE  : RMode = RB_MODE_PLACE_UPDATE; break; \
		case RB_MODE_PLACE_UPDATE : RMode = RB_MODE_PICK_UPDATE; break; \
		case RB_MODE_EXTEND :	RMode = RB_MODE_EXTEND;		break; \
		case RB_MODE_RETRACT :	RMode = RB_MODE_RETRACT;	break; \
		case RB_MODE_HOME :		RMode = RB_MODE_HOME;		break; \
		case RB_MODE_UP :		RMode = RB_MODE_UP;			break; \
		case RB_MODE_DOWN :		RMode = RB_MODE_DOWN;		break; \
		case RB_MODE_ROTATE :	RMode = RB_MODE_ROTATE;		break; \
		case RB_MODE_PICKPLACE :		RMode = RB_MODE_PICKPLACE;	break; /* 2013.01.11 */ \
		} \
	} \
	switch( RMode ) { \
	case RB_MODE_PICK :		MRes = RB_PICKDONE;		break; \
	case RB_MODE_PLACE :	MRes = RB_PLACEDONE;	break; \
	case RB_MODE_RETPIC :	MRes = RB_RETRACTDONE;	break; \
	case RB_MODE_RETPLC :	MRes = RB_RETRACTDONE;	break; \
	case RB_MODE_RETRACT :	MRes = RB_RETRACTDONE;	break; \
	case RB_MODE_EXTPIC :	MRes = RB_EXTENDDONE;	break; \
	case RB_MODE_EXTPLC :	MRes = RB_EXTENDDONE;	break; \
	case RB_MODE_EXTEND :	MRes = RB_EXTENDDONE;	break; \
	case RB_MODE_HOME :		MRes = RB_HOMEDONE;		break; \
	case RB_MODE_UP :		MRes = RB_HOMEDONE;		break; \
	case RB_MODE_DOWN :		MRes = RB_HOMEDONE;		break; \
	case RB_MODE_ROTATE :	MRes = RB_ROTATEDONE;	break; \
	case RB_MODE_PICK_UPDATE  : MRes = RB_UPDATEDONE; TM_Run = FALSE; timex = 1; break; \
	case RB_MODE_PLACE_UPDATE : MRes = RB_UPDATEDONE; TM_Run = FALSE; timex = 1; break; \
	case RB_MODE_PICKPLACE :	MRes = RB_PICKPLACEDONE;	break; /* 2013.01.11 */ \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 4; /* 2017.03.20 */ \
\
	if ( !ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( ##ZZZ## , RMode , RChamber , Finger , RSlot % 100 , RSlot / 100 , RDepth % 100 , RDepth / 100 , ( _i_SCH_PRM_GET_RB_SYNCH_CHECK(##ZZZ##) && TM_Run ) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE(##ZZZ##) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED; \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 5; /* 2017.03.20 */ \
\
	if ( TM_Run ) { \
		if ( Mode == RB_MODE_PICKPLACE ) { /* 2013.01.11 */ \
			if ( Finger >= 100 ) { \
				sprintf( Buffer , " %c_%s:%c" , ( Finger % 100 ) - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( Finger ) - TA_STATION + 'A' ); \
			} \
			else { \
				sprintf( Buffer , " %c_%s" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) ); \
			} \
		} \
		else { \
			sprintf( Buffer , " %c_%s" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) ); \
		} \
		strcat( Param , Buffer ); \
		if      ( Chamber <  PM1 ) { \
			sprintf( Buffer , " %d" , RSlot ); \
		} \
		else if ( Chamber == F_AL ) { /* 2011.07.25 */ \
			sprintf( Buffer , " %d" , RDepth ); \
		} \
		else if ( Chamber == F_IC ) { /* 2011.07.25 */ \
			sprintf( Buffer , " %d" , RDepth ); \
		} \
		else if ( Chamber >= BM1 ) { \
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
				sprintf( Buffer , " %d" , RDepth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
			} \
			else { \
				sprintf( Buffer , " %d" , RDepth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) ); \
			} \
		} \
		else if ( Chamber ==  AL ) { \
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
				sprintf( Buffer , " %d" , RDepth ); \
			} \
			else { \
				sprintf( Buffer , " %d" , RDepth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , AL ) ); \
			} \
		} \
		else if ( Chamber ==  IC ) { \
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
				sprintf( Buffer , " %d" , RDepth ); \
			} \
			else { \
				sprintf( Buffer , " %d" , RDepth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , IC ) ); \
			} \
		} \
		else { \
			sprintf( Buffer , " %d" , RDepth ); \
		} \
		strcat( Param , Buffer ); \
		EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , Param ); \
		if ( order > 0 ) { \
			sprintf( Buffer , " ORDER%d" , order ); \
			strcat( Param , Buffer ); \
		} \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 6; /* 2017.03.20 */ \
\
	if ( Rb_Run == 1 ) { \
		if ( TM_Run ) \
			if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , Param ) ) return SYS_ABORTED; \
		e = FALSE; \
		Ms = SYS_SUCCESS; \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 7; /* 2017.03.20 */ \
\
		while ( TRUE ) { \
			_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */ \
			Mon_Goal = Clock_Goal_Get_Start_Time(); \
			if ( TM_Run ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 8; /* 2017.03.20 */ \
\
				Ms = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ); \
				if ( Ms == SYS_ABORTED ) break; \
				if ( Ms == SYS_ERROR ) { /* 2006.07.14 */ \
					/* Ms = SYS_ABORTED; */ /* 2006.07.14 2007.01.04 */ \
					break; /* 2006.07.14 */ \
				} /* 2006.07.14 */ \
				if ( Rb_Run == 1 ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 9; /* 2017.03.20 */ \
\
					if ( Ms == SYS_SUCCESS ) e = TRUE; \
					Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , e , &update ); \
					if ( update ) _i_SCH_IO_MTL_SAVE(); \
					if ( Mr == MRes ) { \
						if ( Ms != SYS_RUNNING ) break; \
						Rb_Run = 0; \
					} \
					\
				} \
				else { \
					if ( Ms != SYS_RUNNING ) break; \
				} \
			} \
			else { \
				if ( Sim_Trg > 0 ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 10; /* 2017.03.20 */ \
\
					if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) { \
						Ms = SYS_SUCCESS; \
					} \
					else { \
						Ms = SYS_RUNNING; \
					} \
					if ( Rb_Run == 1 ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 11; /* 2017.03.20 */ \
\
						if ( Ms == SYS_SUCCESS ) e = TRUE; \
						Mr = ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , e , &update ); \
						if ( update ) _i_SCH_IO_MTL_SAVE(); \
						if ( Mr == MRes ) { \
							if ( Ms != SYS_RUNNING ) break; \
							Rb_Run = 0; \
						} \
					} \
					else { \
						if ( Ms != SYS_RUNNING ) break; \
					} \
				} \
				else { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 12; /* 2017.03.20 */ \
\
					if ( ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 1 , FALSE , &update ) == MRes ) { \
						if ( update ) _i_SCH_IO_MTL_SAVE(); \
						break; \
					} \
					if ( update ) _i_SCH_IO_MTL_SAVE(); \
				} \
			} \
			l_c = 0; \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 21; /* 2017.03.20 */ \
\
			do { \
				if ( MANAGER_ABORT() ) return SYS_ABORTED; \
				if ( TM_Run ) { \
					if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] ) != SYS_RUNNING ) { \
						break; \
					} \
				} \
				switch( GET_RUN_LD_CONTROL(0) ) { \
				case 1 : \
					timex = 0; \
					break; \
				case 2 : \
					timex = 20; \
					break; \
				} \
				if ( ( l_c % 10 ) == 0 ) Sleep(1); \
				l_c++; \
			} \
			while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) ); \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 23; /* 2017.03.20 */ \
\
		} \
		if ( Ms == SYS_AVAILABLE ) Ms = SYS_ABORTED; \
	} \
	else if ( TM_Run ) { \
\
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 31; /* 2017.03.20 */ \
\
		if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { /* 2015.12.16 */ \
			while ( TRUE ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 32; /* 2017.03.20 */ \
\
				if ( MRes == ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 3 , FALSE , &update ) ) break; \
			} \
		} \
\
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 33; /* 2017.03.20 */ \
\
		Ms = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[ TM + ##ZZZ## ] , Param ); \
\
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 34; /* 2017.03.20 */ \
\
		if ( Rb_Run == 4 ) { /* 2015.12.16 */ \
			ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( ##ZZZ## , RMode , RChamber , Finger , RSlot % 100 , RSlot / 100 , RDepth % 100 , RDepth / 100 , -1 , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE(##ZZZ##) , MaintInfUse , TRUE ); \
		} \
\
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 35; /* 2017.03.20 */ \
\
		if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { /* 2015.12.16 */ \
			while ( TRUE ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 36; /* 2017.03.20 */ \
\
				if ( MRes == ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , 2 , FALSE , &update ) ) break; \
			} \
		} \
\
		_i_SCH_IO_MTL_SAVE(); \
	} \
	else { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 37; /* 2017.03.20 */ \
\
		Ms = SYS_SUCCESS; \
		while ( TRUE ) { \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 38; /* 2017.03.20 */ \
\
			if ( MRes == ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( ##ZZZ## , Rb_Run , FALSE , &update ) ) break; \
		} \
		_i_SCH_IO_MTL_SAVE(); \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 39; /* 2017.03.20 */ \
\
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT_END\t%s_%d\tMode=%d,Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Transfer=%d,SrcCas=%d,order=%d Res=%d\n" , "EQUIP_ROBOT_CONTROL" , ##ZZZ## , Mode , Chamber , Finger , Slot , Depth , Transfer , SrcCas , order , Ms ); \
	} \
\
	EQUIP_TM_ROBOT_FLAG[##ZZZ##] = 0; /* 2017.03.20 */ \
\
	return( Ms ); \
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Pick From Chamber
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  EQUIP_TM_RUNNING_FLAG[MAX_TM_CHAMBER_DEPTH]; // 2017.03.20

void _iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( int ch , int data ); // 2012.09.26

#define DEFINE_EQUIP_PICK_FROM_CHAMBER( ZZZ ) int EQUIP_PICK_FROM_CHAMBER_##ZZZ##( int CHECKING_SIDE , int Chamber , int Finger , int Slot , int Depth , int Switch , int SrcCas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror ) { \
	int Transfer; \
	int Res; /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 1; /* 2017.03.20 */ \
\
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , TRUE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); /* 2012.10.31 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,frcalign=%d,PostPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , frcalign , PostPartSkip , order ); \
	} \
	KPLT_START_TM_PICK_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	_i_SCH_TIMER_SET_ROBOT_TIME_START( ##ZZZ## , Finger ); \
	Transfer = ( CHECKING_SIDE < TR_CHECKING_SIDE ) ? 0 : (CHECKING_SIDE - TR_CHECKING_SIDE + 1); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 2; /* 2017.03.20 */ \
\
	switch ( PostPartSkip ) { \
	case 0 : \
	case 1 : \
	case 2 : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 3; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 4; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_PREPARE , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 5; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 1 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 6; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 3 : \
			break; \
		case 5 : \
		case 7 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 7; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 11 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 8; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_OPEN , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); /* 2006.06.27 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 9; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 2 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 10; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2006.06.27 */ \
			} \
			break; \
		} \
		break; \
	case 3 : /* 2009.07.10 */ \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 3 : \
			break; \
		case 5 : \
		case 7 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 11; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 12; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_OPEN , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 13; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 2 ); \
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; \
			} \
			break; \
		} \
		break; \
	case 99 : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 14; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 15; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_PREPARE , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 16; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 1 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 17; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
		break; \
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 18; /* 2017.03.20 */ \
\
	if ( _i_SCH_PRM_GET_STATION_PICK_HANDLING( Chamber ) ) { \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 12 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 19; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PICK , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 20; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2008.12.12 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 21; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 22; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
	} \
	else { \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 12 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 23; /* 2017.03.20 */ \
\
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_PREPARE_MDL , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 24; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 25; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 26; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 30; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_EXTPIC , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 31; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 32; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 33; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 34; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 35; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_MTL_MDL , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 36; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 37; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 38; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 39; /* 2017.03.20 */ \
\
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 40; /* 2017.03.20 */ \
\
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PICK_UPDATE , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 41; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 42; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 43; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 44; /* 2017.03.20 */ \
\
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 45; /* 2017.03.20 */ \
\
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_RETPIC , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 46; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 47; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 48; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
\
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 60; /* 2017.03.20 */ \
\
	switch ( PostPartSkip ) { \
	case 0 : \
	case 3 : /* 2009.07.10 */ \
	case 4 : /* 2009.07.10 */ \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 2 : \
			break; \
		case 4 : \
			if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 61; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 62; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 63; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 64; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 65; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		case 8 : \
			if ( ( Transfer == 0 ) && ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 66; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 67; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 68; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 69; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		case 5 : \
		case 6 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 70; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 71; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 72; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 73; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 74; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 14 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 75; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_POST , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 76; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 5 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 77; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 78; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
		break; \
	case 1 : \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 2 : \
			break; \
		case 4 : \
			if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 79; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 80; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 81; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 82; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 83; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		case 8 : \
			if ( ( Transfer == 0 ) && ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 84; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 85; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 86; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 87; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		case 5 : \
		case 6 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 88; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 89; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 90; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 91; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 92; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2008.12.12 */ \
			} \
			break; \
		} \
		break; \
	case 99 : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 93; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 94; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_POST , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 95; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 5 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 96; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 97; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
		break; \
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 100; /* 2017.03.20 */ \
\
	if ( ( ##ZZZ## == 0 ) && ( (frcalign) || ( Chamber < PM1 ) || ( Chamber >= BM1 ) ) && _i_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) ) { \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 101; /* 2017.03.20 */ \
\
		if ( EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PLACE , CHECKING_SIDE , AL , Finger , Slot , 1 , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ) != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; /* 2007.01.04 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 102; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 103; /* 2017.03.20 */ \
\
		if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) { /* 2004.09.08 */ \
			if ( EQUIP_ALIGNING_SPAWN_FOR_TM_MAL( CHECKING_SIDE , Finger , Slot , SrcCas ) == SYS_ABORTED ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
		} \
		else { \
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
				if ( EQUIP_UTILITY_CONTROL_##ZZZ##( 0 , Finger , Slot , SrcCas , 1 ) == SYS_ABORTED ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ABORTED; \
				} \
			} \
			else { \
				if ( EQUIP_UTILITY_CONTROL_##ZZZ##( 0 , Finger , Slot , SrcCas , 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , AL ) ) == SYS_ABORTED ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ABORTED; \
				} \
			} \
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
			if ( EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PICK , CHECKING_SIDE , AL , Finger , Slot , 1 , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ) != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; /* 2007.01.04 */ \
			} \
		} \
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 104; /* 2017.03.20 */ \
\
	_i_SCH_TIMER_SET_ROBOT_TIME_END( ##ZZZ## , Finger ); \
	KPLT_END_TM_PICK_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,frcalign=%d,PostPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , frcalign , PostPartSkip , order ); \
	} \
\
	_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( -1 , FALSE ); /* 2012.11.09 */ \
\
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 0; /* 2017.03.20 */ \
\
	if ( retsyserror != 0 ) { /* 2018.09.27 */ \
		if ( retsyserror < 20 ) { /* 2018.09.27 */ \
			if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) { \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); \
				return SYS_ERROR; \
			} \
		} \
		else { \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICK , Chamber , Finger ); \
			return SYS_ERROR; \
		} \
	} \
\
	return SYS_SUCCESS; \
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Place to Chamber
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFINE_EQUIP_PLACE_TO_CHAMBER( ZZZ ) int EQUIP_PLACE_TO_CHAMBER_##ZZZ##( int CHECKING_SIDE , int Chamber , int Finger , int Slot , int Depth , int Switch , int SrcCas , BOOL frccool , int PrepPartSkip , int order , BOOL pmdual , BOOL MaintInfUse , int retsyserror ) { \
	int Transfer; \
	int Res; /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 1; /* 2017.03.20 */ \
\
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , TRUE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); /* 2012.10.31 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,PrepPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_TO_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , PrepPartSkip , order ); \
	} \
	KPLT_START_TM_PLACE_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	_i_SCH_TIMER_SET_ROBOT_TIME_START( ##ZZZ## , Finger ); \
	Transfer = ( CHECKING_SIDE < TR_CHECKING_SIDE ) ? 0 : (CHECKING_SIDE - TR_CHECKING_SIDE + 1); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 2; /* 2017.03.20 */ \
\
	if ( ( ##ZZZ## == 0 ) && ( (frccool) || ( Chamber < PM1 ) || ( Chamber >= BM1 ) ) && _i_SCH_MODULE_GET_USE_ENABLE( IC , FALSE , -1 ) ) { \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
		if ( EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PLACE , CHECKING_SIDE , IC , Finger , Slot , 1 , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ) != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; /* 2007.01.04 */ \
		} \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { /* 2006.11.23 */ \
			if ( EQUIP_UTILITY_CONTROL_##ZZZ##( 1 , Finger , Slot , SrcCas , 1 ) == SYS_ABORTED ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
		} \
		else { \
			if ( EQUIP_UTILITY_CONTROL_##ZZZ##( 1 , Finger , Slot , SrcCas , 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , IC ) ) == SYS_ABORTED ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
		} \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
		if ( EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PICK , CHECKING_SIDE , IC , Finger , Slot , 1 , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ) != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; /* 2007.01.04 */ \
		} \
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 3; /* 2017.03.20 */ \
\
	switch( PrepPartSkip ) { \
	case 0 : \
	case 3 : /* 2009.07.10 */ \
	case 4 : /* 2009.07.10 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 4; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 5; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_PREPARE , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 6; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 1 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 7; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 3 : \
			break; \
		case 5 : \
		case 7 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 8; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 11 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 9; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_OPEN , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); /* 2006.06.27 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 10; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 2 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 11; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2006.06.27 */ \
			} \
			break; \
		} \
		break; \
	case 1 : \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 3 : \
			break; \
		case 5 : \
		case 7 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 12; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 13; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_OPEN , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); /* 2006.06.27 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 14; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 2 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 15; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2006.06.27 */ \
			} \
			break; \
		} \
		break; \
	case 99 : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 16; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 17; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_PREPARE , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); /* 2005.11.24 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 18; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 1 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 19; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
		break; \
	} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 30; /* 2017.03.20 */ \
\
	if ( _i_SCH_PRM_GET_STATION_PLACE_HANDLING( Chamber ) ) { \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 31; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 12 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 32; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PLACE , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); /* 2007.01.04 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 33; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 34; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 35; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
	} \
	else { \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 36; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 12 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 37; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_PREPARE_MDL , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 38; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 39; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 40; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 41; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_EXTPLC , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 42; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 43; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 44; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 45; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 46; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_MTL_MDL , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 47; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 48; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 49; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 50; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 51; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PLACE_UPDATE , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 52; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 53; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 54; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 55; /* 2017.03.20 */ \
\
		Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_RETPLC , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 56; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 57; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 58; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
\
	} \
	switch( PrepPartSkip ) { \
	case 0 : \
	case 1 : \
	case 2 : \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 2 : \
			break; \
		case 4 : \
			if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 59; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 60; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 61; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 62; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 63; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2007.01.04 */ \
			} \
			break; \
		case 8 : \
			if ( ( Transfer == 0 ) && ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 64; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 65; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 66; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 67; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 68; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2007.01.04 */ \
			} \
			break; \
		case 5 : \
		case 6 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 69; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 70; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 71; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 72; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 73; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; /* 2007.01.04 */ \
			} \
			break; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 74; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 14 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 75; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_POST , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 76; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 5 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 77; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 78; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
		break; \
	case 3 : /* 2009.07.10 */ \
		switch( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { \
		case 1 : \
		case 2 : \
			break; \
		case 4 : \
			if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 79; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 80; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 81; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 82; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 83; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; \
			} \
			break; \
		case 8 : \
			if ( ( Transfer == 0 ) && ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) break; \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 84; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 85; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 86; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 87; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 88; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; \
			} \
			break; \
		case 5 : \
		case 6 : \
			if ( Transfer == 0 ) break; \
		default : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 89; /* 2017.03.20 */ \
\
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
				EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ABORTED; \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 90; /* 2017.03.20 */ \
\
			Res = EQUIP_GATE_CONTROL_##ZZZ##( GATE_MODE_CLOSE , Chamber , Finger , Slot , Depth , Switch , Transfer , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 91; /* 2017.03.20 */ \
\
			if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 4 ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 92; /* 2017.03.20 */ \
\
			if ( Res == SYS_ERROR   ) { \
				if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
					return SYS_ERROR; /* 2011.04.05 */ \
				} \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); \
			} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 93; /* 2017.03.20 */ \
\
			if ( Res != SYS_SUCCESS ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return Res; \
			} \
			break; \
		} \
		break; \
	case 99 : \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 94; /* 2017.03.20 */ \
\
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 13 ); /* 2007.12.14 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 95; /* 2017.03.20 */ \
\
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_POST , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 96; /* 2017.03.20 */ \
\
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 1 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 5 ); /* 2007.12.14 */ \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 97; /* 2017.03.20 */ \
\
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 98; /* 2017.03.20 */ \
\
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2007.01.04 */ \
		} \
		break; \
	} \
	_i_SCH_TIMER_SET_ROBOT_TIME_END( ##ZZZ## , Finger ); \
	KPLT_END_TM_PLACE_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,PrepPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_TO_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , PrepPartSkip , order ); \
	} \
\
	_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( -1 , FALSE ); /* 2012.11.09 */ \
\
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
\
\
	EQUIP_TM_RUNNING_FLAG[##ZZZ##] = 0; /* 2017.03.20 */ \
\
	if ( retsyserror != 0 ) { /* 2018.09.27 */ \
		if ( retsyserror < 20 ) { /* 2018.09.27 */ \
			if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) { \
				ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); \
				return SYS_ERROR; \
			} \
		} \
		else { \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PLACE , Chamber , Finger ); \
			return SYS_ERROR; \
		} \
	} \
	return SYS_SUCCESS; \
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Swap Chamber // 2013.01.11
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define DEFINE_EQUIP_SWAP_CHAMBER( ZZZ ) int EQUIP_SWAP_CHAMBER_##ZZZ##( int CHECKING_SIDE , int Chamber , int Finger , int Slot , int Depth , int Switch , int SrcCas , int frcaligncool , int PostPartSkip , int order , BOOL pmdual , BOOL MaintInfUse ) { \
	int Transfer; \
	int Res; /* 2005.11.24 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , TRUE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); /* 2012.10.31 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,frcaligncool=%d,PostPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SWAP_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , frcaligncool , PostPartSkip , order ); \
	} \
	KPLT_START_TM_PICK_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	_i_SCH_TIMER_SET_ROBOT_TIME_START( ##ZZZ## , Finger ); \
	Transfer = ( CHECKING_SIDE < TR_CHECKING_SIDE ) ? 0 : (CHECKING_SIDE - TR_CHECKING_SIDE + 1); \
	switch ( PostPartSkip ) { \
	case 0 : \
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ABORTED; \
		} \
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PICK_PREPARE , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); /* 2005.11.24 */ \
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 1 ); /* 2007.12.14 */ \
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2005.11.24 */ \
		} \
		break; \
	} \
\
\
	if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
		EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 12 ); /* 2007.12.14 */ \
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
		return SYS_ABORTED; \
	} \
	Res = EQUIP_ROBOT_CONTROL_##ZZZ##( RB_MODE_PICKPLACE , CHECKING_SIDE , Chamber , Finger , Slot , Depth , Transfer , FALSE , 0 , 0 , SrcCas , order , MaintInfUse ); \
	if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 3 ); /* 2008.12.12 */ \
	if ( Res == SYS_ERROR   ) { \
		if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return SYS_ERROR; /* 2011.04.05 */ \
		} \
		ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICKPLACE , Chamber , Finger ); /* 2008.12.12 */ \
	} \
	if ( Res != SYS_SUCCESS ) { \
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
		return Res; /* 2005.11.24 */ \
	} \
\
\
	switch ( PostPartSkip ) { \
	case 0 : \
		Res = EQUIP_INTERFACE_CONTROL_##ZZZ##( HO_PLACE_POST , Chamber , Finger , Slot , Depth , Switch , Transfer , FALSE , SrcCas , order , pmdual , MaintInfUse ); \
		if ( Res == SYS_ABORTED ) EQUIP_INTERFACE_PICKPLACE_CANCEL( ##ZZZ## , 0 , Chamber , Finger , Slot , Depth , Transfer , SrcCas , 5 ); /* 2007.12.14 */ \
		if ( Res == SYS_ERROR   ) { \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( TM + ##ZZZ## ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			if ( _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( Chamber ) == 0 ) { \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
				return SYS_ERROR; /* 2011.04.05 */ \
			} \
			ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( ##ZZZ## , RB_MODE_PICKPLACE , Chamber , Finger ); /* 2008.12.12 */ \
		} \
		if ( Res != SYS_SUCCESS ) { \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
			return Res; /* 2008.12.12 */ \
		} \
		break; \
	} \
\
	_i_SCH_TIMER_SET_ROBOT_TIME_END( ##ZZZ## , Finger ); \
	KPLT_END_TM_PICK_ROBOT_TIME( ##ZZZ## , Chamber >= BM1 ); \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) { \
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s_%d\t[%d] Chamber=%d,Finger=%d,Slot=%d,Depth=%d,Switch=%d,SrcCas=%d,frcaligncool=%d,PostPartSkip=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SWAP_CHAMBER" , ##ZZZ## , KPLT_GET_LOT_PROGRESS_TIME( TM + ##ZZZ## ) , Chamber , Finger , Slot , Depth , Switch , SrcCas , frcaligncool , PostPartSkip , order ); \
	} \
\
	_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( -1 , FALSE ); /* 2012.11.09 */ \
\
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); /* 2012.10.31 */ \
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+##ZZZ## , FALSE ); /* 2012.10.31 */ \
\
	return SYS_SUCCESS; \
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Pick From AL
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_SCH_EQ_PICK_FROM_TM_MAL( int CHECKING_SIDE , int Finger , int Slot , int SrcCas , int order ) { // 2004.09.08
	int Transfer;
	_EQUIP_RUNNING_TAG = TRUE; /* 2008.04.17 */
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM , TRUE ); /* 2012.10.31 */ \
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\tFinger=%d,Slot=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_TM_MAL" , Finger , Slot , SrcCas , order );
	}
	Transfer = ( CHECKING_SIDE < TR_CHECKING_SIDE ) ? 0 : (CHECKING_SIDE - TR_CHECKING_SIDE + 1);
	if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) {
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM , FALSE ); /* 2012.10.31 */ \
		return SYS_ABORTED;
	}
	if ( EQUIP_ROBOT_CONTROL_0( RB_MODE_PICK , CHECKING_SIDE , AL , Finger , Slot , 1 , Transfer , FALSE , 0 , 0 , SrcCas , order , FALSE ) != SYS_SUCCESS ) {
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM , FALSE ); /* 2012.10.31 */ \
		return SYS_ABORTED; /* 2007.01.04 */ 
	}
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\tFinger=%d,Slot=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_TM_MAL" , Finger , Slot , SrcCas , order );
	}
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM , FALSE ); /* 2012.10.31 */ \
	return SYS_SUCCESS;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// TM Multiple Making
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 0 );
DEFINE_EQUIP_UTILITY_CONTROL( 0 );
DEFINE_EQUIP_GATE_CONTROL( 0 );
DEFINE_EQUIP_INTERFACE_CONTROL( 0 );
DEFINE_EQUIP_ROBOT_CONTROL( 0 );
DEFINE_EQUIP_PICK_FROM_CHAMBER( 0 );
DEFINE_EQUIP_PLACE_TO_CHAMBER( 0 );
DEFINE_EQUIP_SWAP_CHAMBER( 0 );
//
DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 1 );
DEFINE_EQUIP_UTILITY_CONTROL( 1 );
DEFINE_EQUIP_GATE_CONTROL( 1 );
DEFINE_EQUIP_INTERFACE_CONTROL( 1 );
DEFINE_EQUIP_ROBOT_CONTROL( 1 );
DEFINE_EQUIP_PICK_FROM_CHAMBER( 1 );
DEFINE_EQUIP_PLACE_TO_CHAMBER( 1 );
DEFINE_EQUIP_SWAP_CHAMBER( 1 );
//
DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 2 );
DEFINE_EQUIP_UTILITY_CONTROL( 2 );
DEFINE_EQUIP_GATE_CONTROL( 2 );
DEFINE_EQUIP_INTERFACE_CONTROL( 2 );
DEFINE_EQUIP_ROBOT_CONTROL( 2 );
DEFINE_EQUIP_PICK_FROM_CHAMBER( 2 );
DEFINE_EQUIP_PLACE_TO_CHAMBER( 2 );
DEFINE_EQUIP_SWAP_CHAMBER( 2 );
//
DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 3 );
DEFINE_EQUIP_UTILITY_CONTROL( 3 );
DEFINE_EQUIP_GATE_CONTROL( 3 );
DEFINE_EQUIP_INTERFACE_CONTROL( 3 );
DEFINE_EQUIP_ROBOT_CONTROL( 3 );
DEFINE_EQUIP_PICK_FROM_CHAMBER( 3 );
DEFINE_EQUIP_PLACE_TO_CHAMBER( 3 );
DEFINE_EQUIP_SWAP_CHAMBER( 3 );
//
#ifndef PM_CHAMBER_12
	DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 4 );
	DEFINE_EQUIP_UTILITY_CONTROL( 4 );
	DEFINE_EQUIP_GATE_CONTROL( 4 );
	DEFINE_EQUIP_INTERFACE_CONTROL( 4 );
	DEFINE_EQUIP_ROBOT_CONTROL( 4 );
	DEFINE_EQUIP_PICK_FROM_CHAMBER( 4 );
	DEFINE_EQUIP_PLACE_TO_CHAMBER( 4 );
	DEFINE_EQUIP_SWAP_CHAMBER( 4 );
	//
	DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 5 );
	DEFINE_EQUIP_UTILITY_CONTROL( 5 );
	DEFINE_EQUIP_GATE_CONTROL( 5 );
	DEFINE_EQUIP_INTERFACE_CONTROL( 5 );
	DEFINE_EQUIP_ROBOT_CONTROL( 5 );
	DEFINE_EQUIP_PICK_FROM_CHAMBER( 5 );
	DEFINE_EQUIP_PLACE_TO_CHAMBER( 5 );
	DEFINE_EQUIP_SWAP_CHAMBER( 5 );
	//
#ifndef PM_CHAMBER_30
	DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 6 );
	DEFINE_EQUIP_UTILITY_CONTROL( 6 );
	DEFINE_EQUIP_GATE_CONTROL( 6 );
	DEFINE_EQUIP_INTERFACE_CONTROL( 6 );
	DEFINE_EQUIP_ROBOT_CONTROL( 6 );
	DEFINE_EQUIP_PICK_FROM_CHAMBER( 6 );
	DEFINE_EQUIP_PLACE_TO_CHAMBER( 6 );
	DEFINE_EQUIP_SWAP_CHAMBER( 6 );
	//
	DEFINE_EQUIP_ROBOT_MOVE_CONTROL( 7 );
	DEFINE_EQUIP_UTILITY_CONTROL( 7 );
	DEFINE_EQUIP_GATE_CONTROL( 7 );
	DEFINE_EQUIP_INTERFACE_CONTROL( 7 );
	DEFINE_EQUIP_ROBOT_CONTROL( 7 );
	DEFINE_EQUIP_PICK_FROM_CHAMBER( 7 );
	DEFINE_EQUIP_PLACE_TO_CHAMBER( 7 );
	DEFINE_EQUIP_SWAP_CHAMBER( 7 );
	//
#endif
#endif

int EQUIP_ROBOT_CONTROL_MAINT_INF( int TMATM , int Mode , int Chamber , int Finger , int Slot , int Depth , int Transfer , BOOL AnimChange , int AnimChamber , int AnimSlot , int SrcCas , int order ) {
	int Res = SYS_ABORTED;
	//
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+TMATM , TRUE ); /* 2012.10.31 */
	//
	switch( TMATM ) {
	case 0 : Res = EQUIP_ROBOT_CONTROL_0( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
	case 1 : Res = EQUIP_ROBOT_CONTROL_1( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
	case 2 : Res = EQUIP_ROBOT_CONTROL_2( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
	case 3 : Res = EQUIP_ROBOT_CONTROL_3( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
#ifndef PM_CHAMBER_12
	case 4 : Res = EQUIP_ROBOT_CONTROL_4( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
	case 5 : Res = EQUIP_ROBOT_CONTROL_5( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
#ifndef PM_CHAMBER_30
	case 6 : Res = EQUIP_ROBOT_CONTROL_6( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
	case 7 : Res = EQUIP_ROBOT_CONTROL_7( Mode , -1 , Chamber , Finger , Slot , Depth , Transfer , AnimChange , AnimChamber , AnimSlot , SrcCas , order , TRUE );	break;
#endif
#endif
	}
	//
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( TM+TMATM , FALSE ); /* 2012.10.31 */
	//
	return Res;
}


int EQUIP_PICK_FROM_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , BOOL Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual ) {
	switch( TMATM ) {
	case 0 : return EQUIP_PICK_FROM_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
	case 1 : return EQUIP_PICK_FROM_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
	case 2 : return EQUIP_PICK_FROM_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
	case 3 : return EQUIP_PICK_FROM_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_PICK_FROM_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
	case 5 : return EQUIP_PICK_FROM_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_PICK_FROM_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
	case 7 : return EQUIP_PICK_FROM_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , TRUE , 0 );
#endif
#endif
	}
	return SYS_ABORTED;
}
//
int EQUIP_PLACE_TO_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual ) {
	switch( TMATM ) {
	case 0 : return EQUIP_PLACE_TO_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
	case 1 : return EQUIP_PLACE_TO_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
	case 2 : return EQUIP_PLACE_TO_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
	case 3 : return EQUIP_PLACE_TO_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_PLACE_TO_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
	case 5 : return EQUIP_PLACE_TO_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_PLACE_TO_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
	case 7 : return EQUIP_PLACE_TO_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE , 0 );
#endif
#endif
	}
	return SYS_ABORTED;
}

int EQUIP_SWAP_CHAMBER_MAINT_INF( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual ) { // 2013.01.11
	switch( TMATM ) {
	case 0 : return EQUIP_SWAP_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
	case 1 : return EQUIP_SWAP_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
	case 2 : return EQUIP_SWAP_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
	case 3 : return EQUIP_SWAP_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_SWAP_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
	case 5 : return EQUIP_SWAP_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_SWAP_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
	case 7 : return EQUIP_SWAP_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , TRUE );
#endif
#endif
	}
	return SYS_ABORTED;
}

int _i_SCH_EQ_PICK_FROM_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , BOOL Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order , BOOL pmdual ) {
	switch( TMATM ) {
	case 0 : return EQUIP_PICK_FROM_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
	case 1 : return EQUIP_PICK_FROM_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
	case 2 : return EQUIP_PICK_FROM_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
	case 3 : return EQUIP_PICK_FROM_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_PICK_FROM_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
	case 5 : return EQUIP_PICK_FROM_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_PICK_FROM_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
	case 7 : return EQUIP_PICK_FROM_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , pmdual , FALSE , 0 );
#endif
#endif
	}
	return SYS_ABORTED;
}
//
int _i_SCH_EQ_PLACE_TO_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual ) {
	switch( TMATM ) {
	case 0 : return EQUIP_PLACE_TO_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
	case 1 : return EQUIP_PLACE_TO_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
	case 2 : return EQUIP_PLACE_TO_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
	case 3 : return EQUIP_PLACE_TO_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_PLACE_TO_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
	case 5 : return EQUIP_PLACE_TO_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_PLACE_TO_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
	case 7 : return EQUIP_PLACE_TO_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE , 0 );
#endif
#endif
	}
	return SYS_ABORTED;
}

int _i_SCH_EQ_SWAP_CHAMBER( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order , BOOL pmdual ) { // 2013.01.11
	switch( TMATM ) {
	case 0 : return EQUIP_SWAP_CHAMBER_0( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
	case 1 : return EQUIP_SWAP_CHAMBER_1( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
	case 2 : return EQUIP_SWAP_CHAMBER_2( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
	case 3 : return EQUIP_SWAP_CHAMBER_3( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
#ifndef PM_CHAMBER_12
	case 4 : return EQUIP_SWAP_CHAMBER_4( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
	case 5 : return EQUIP_SWAP_CHAMBER_5( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
#ifndef PM_CHAMBER_30
	case 6 : return EQUIP_SWAP_CHAMBER_6( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
	case 7 : return EQUIP_SWAP_CHAMBER_7( Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , pmdual , FALSE );
#endif
#endif
	}
	return SYS_ABORTED;
}


int EQUIP_PICK_FROM_CHAMBER_TT( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , BOOL Switch , int Srccas , BOOL frcalign , int PostPartSkip , int order ) {
	return _i_SCH_EQ_PICK_FROM_CHAMBER( TMATM , Side , Chamber , Finger , Slot , Depth , Switch , Srccas , frcalign , PostPartSkip , order , FALSE );
}

int EQUIP_PLACE_TO_CHAMBER_TT( int TMATM , int Side , int Chamber , int Finger , int Slot , int Depth , int Switch , int Srccas , BOOL forcecool , int PrepPartSkip , int order ) {
	return _i_SCH_EQ_PLACE_TO_CHAMBER( TMATM , Side , Chamber , Finger , Slot , Depth , Switch , Srccas , forcecool , PrepPartSkip , order , FALSE );
}

int _i_SCH_EQ_ROBOT_FROM_CHAMBER( int tms , int mode , int CHECKING_SIDE , int Chamber , int Finger , int Slot ) { // 2012.04.18
	return EQUIP_ROBOT_CONTROL_MAINT_INF( tms , mode , Chamber , Finger , Slot , Slot , CHECKING_SIDE , FALSE , 0 , 0 , 0 , 0 );
}
