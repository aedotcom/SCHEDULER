#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Equip_Handling.h"
#include "Robot_Handling.h"
#include "User_Parameter.h"
#include "FA_Handling.h"
#include "Sch_Main.h"
#include "Sch_Prm.h"
#include "sch_CassetteSupply.h"
#include "sch_PrePrcs.h"
#include "sch_prepost.h"
#include "sch_CassetteResult.h"
#include "sch_Sub.h"
#include "Sch_Multi_ALIC.h"
#include "sch_FMARM_Multi.h"
#include "iolog.h"
#include "Multijob.h"
#include "MR_Data.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "sch_processmonitor.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "DLL_Interface.h"

#include "INF_sch_macro.h"
#include "INF_sch_CommonUser.h"

//================================================================================
extern int  _in_HOT_LOT_ORDERING_INVALID[MAX_CASSETTE_SIDE]; // 2011.06.13
//================================================================================
extern int  _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[MAX_CASSETTE_SIDE]; // 2014.10.14
//================================================================================
int Get_Inside_Value_for_Common_Thread_Status( int side , int where ); // 2008.07.29
void Init_Inf_Dummy_PostWait(void);
void Remove_Inf_Dummy_PostWait( int i );
int Get_Inf_Dummy_PostWait( int side , int pointer , int *dm ) ;
void Set_Inf_Dummy_PostWait( int side , int pointer , int m , int v , char *name );
void _SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( int side , int pointer , int firsttag , int lasttag , int cmstarteveryskip );
int _SCH_inside_Get_Dummy_Muti_Wait_UsingPM( int side , int pointer , int ch , int *order );
int _SCH_inside_Get_Dummy_Use_OnePM( int side , int pointer , int ch );
int _SCH_inside_Get_DLL_Dummy( int postwait , int side , int pointer , int mode , int selpm , int selparam , int *dummychamber , int *dummyslot , char *SelectRecipe );
void _SCH_inside_ConvStr_29_Dummy( char *SelectRecipe ) ;
int _SCH_inside_Get_Dummy( int side , int pointer , int mode , int tag2 , int tag3 , int cmd , int curnotuse );
int _SCH_inside_Unused_Remap_Dummy( int side , int pointer , int *orgrange );
int _SCH_inside_Make_Dummy( int side , int pointer );
void _SCH_inside_Ordering_Invalid_OK( int side ) ;
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_FOR_LOG( int side );
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( int side , int *cm , int *pt , int subchk );
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_CM( int side , int *cm , int *pt ) ;
int _i_SCH_USER_POSSIBLE_PICK_FROM_FM( int side , int *pt );
int _i_SCH_USER_POSSIBLE_PICK_FROM_TM( int side , int *pt );
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( int side , int *cm , int *pt );
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( int side , int *cm , int *pt );
void _SCH_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE( int side , int count );
void _SCH_MACRO_OTHER_LOT_LOAD_WAIT( int side , unsigned int timeoutsec );
//================================================================================
extern int  _SCH_TAG_DUMMY_FLOW_CH;
extern int  _SCH_TAG_DUMMY_FLOW_SLOT;
extern Scheduling_Path	_SCH_INF_CLUSTER_DATA_AREA[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
//================================================================================
extern int Inf_Dummy_PostWait_Use[32];
extern int Inf_Dummy_PostWait_Mode[32];
extern int Inf_Dummy_PostWait_Value[32];
extern char *Inf_Dummy_PostWait_Name[32];
//================================================================================

void Init_Inf_Dummy_PostWait(void) {
	int i;
	for ( i = 0 ; i < 32 ; i++ ) {
		Inf_Dummy_PostWait_Use[i] = -1;
		Inf_Dummy_PostWait_Name[i] = NULL;
	}
}

void Remove_Inf_Dummy_PostWait( int i ) {
	Inf_Dummy_PostWait_Use[i] = -1;
}

int Get_Inf_Dummy_PostWait( int side , int pointer , int *dm ) {
	int i;
	for ( i = 0 ; i < 32 ; i++ ) {
		if ( Inf_Dummy_PostWait_Use[i] == -1 ) continue;
		if ( ( Inf_Dummy_PostWait_Use[i] / 1000 ) != side ) continue;
		if ( ( Inf_Dummy_PostWait_Use[i] % 1000 ) != pointer ) continue;
		*dm = Inf_Dummy_PostWait_Mode[i];
		return i;
	}
	return -1;
}

void Set_Inf_Dummy_PostWait( int side , int pointer , int m , int v , char *name ) {
	int i;
	for ( i = 0 ; i < 32 ; i++ ) {
		if ( Inf_Dummy_PostWait_Use[i] == -1 ) continue;
		if ( ( Inf_Dummy_PostWait_Use[i] / 1000 ) != side ) continue;
		if ( ( Inf_Dummy_PostWait_Use[i] % 1000 ) != pointer ) continue;
		Inf_Dummy_PostWait_Mode[i] = m;
		Inf_Dummy_PostWait_Value[i] = v;
		if ( !STR_MEM_MAKE_COPY2( &(Inf_Dummy_PostWait_Name[i] ) , name ) ) {
			_IO_CIM_PRINTF( "Set_Inf_Dummy_PostWait 1 Memory Allocate Error[%d,%d,%d,%d]\n" , side , pointer , m , v );
		}
		return;
	}
	for ( i = 0 ; i < 32 ; i++ ) {
		if ( Inf_Dummy_PostWait_Use[i] != -1 ) continue;
		Inf_Dummy_PostWait_Use[i] = ( side * 1000 ) + pointer;
		Inf_Dummy_PostWait_Mode[i] = m;
		Inf_Dummy_PostWait_Value[i] = v;
		if ( !STR_MEM_MAKE_COPY2( &(Inf_Dummy_PostWait_Name[i] ) , name ) ) {
			_IO_CIM_PRINTF( "Set_Inf_Dummy_PostWait 2 Memory Allocate Error[%d,%d,%d,%d]\n" , side , pointer , m , v );
		}
		return;
	}
}
//
// 2010.01.20
void _i_SCH_FLOW_CONTROL_CHECK_AFTER_PICK_Sub( int side , int pointer , BOOL picked ) {
	int i , mode , ich , isl , och , osl , wait , next;
	int waitctrlskip; // 2010.04.10
	//
	if ( picked ) {
		i = _i_SCH_CLUSTER_Get_PathRun( side , pointer ,  0 , 2 );
		_i_SCH_CLUSTER_Set_PathRun( side , pointer , 0 , 2 , i + 1 ); // Picked Count
	}
	//
	mode = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 13 , 2 );
	//
	ich  = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
	isl  = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-START\t[Side=%d][Pt=%d]\n" , side , pointer );
//------------------------------------------------------------------------------------------------------------------
	//======================================================================================================================================
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 9 , 2 ) == 1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-REQ.POSSIBLE\t[Side=%d][Pt=%d]\n" , side , pointer );
//------------------------------------------------------------------------------------------------------------------
		_i_SCH_CLUSTER_Set_PathRun( side , pointer , 9 , 2 , 0 );
	}
	//======================================================================================================================================
	switch ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 2 ) ) {
	case 20 : // lot pre
		i    = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 17 , 2 ) - 1;
		och  = _i_SCH_CLUSTER_Get_PathIn( side , i );
		osl  = _i_SCH_CLUSTER_Get_SlotIn( side , i );
		wait = _i_SCH_CLUSTER_Get_PathRun( side , i , 0 , 2 ) + 1;
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
		//
		_i_SCH_CLUSTER_Set_PathRun( side , i , 12 , 2 , 0 ); // 2009.12.29
		_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 2 , ( _i_SCH_CLUSTER_Get_PathRun( side , i , 15 , 2 ) / 10 ) * 10 ); // 2009.12.29
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-LOT.PRE\t[(Side=%d,Pt=%d)(%s:%d/%dth)][ORG=>(Pt=%d)(%s:%d)(lc=%dth)][%s][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 3 ) + 1 , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( och ) , osl , wait , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 2 ) ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	case 21 : // Wafer pre
		i    = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 17 , 2 ) - 1;
		och  = _i_SCH_CLUSTER_Get_PathIn( side , i );
		osl  = _i_SCH_CLUSTER_Get_SlotIn( side , i );
		wait = _i_SCH_CLUSTER_Get_PathRun( side , i , 0 , 2 ) + 1;
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-WFR.PRE\t[(Side=%d,Pt=%d)(%s:%d/%dth)][ORG=>(Pt=%d)(%s:%d)(lc=%dth)][%s][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 3 ) + 1 , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( och ) , osl , wait , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 2 ) ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	case 22 : // Wafer post
		i    = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 17 , 2 ) - 1;
		och  = _i_SCH_CLUSTER_Get_PathIn( side , i );
		osl  = _i_SCH_CLUSTER_Get_SlotIn( side , i );
		wait = _i_SCH_CLUSTER_Get_PathRun( side , i , 0 , 2 );
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-WFR.PST\t[(Side=%d,Pt=%d)(%s:%d/%dth)][ORG=>(Pt=%d)(%s:%d)(lc=%dth)][%s][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 3 ) + 1 , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( och ) , osl , wait , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 2 ) ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	case 23 : // lot post
		i    = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 17 , 2 ) - 1;
		och  = _i_SCH_CLUSTER_Get_PathIn( side , i );
		osl  = _i_SCH_CLUSTER_Get_SlotIn( side , i );
		wait = _i_SCH_CLUSTER_Get_PathRun( side , i , 0 , 2 );
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-LOT.PST\t[(Side=%d,Pt=%d)(%s:%d/%dth)][ORG=>(Pt=%d)(%s:%d)(lc=%dth)][%s][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 3 ) + 1 , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( och ) , osl , wait , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 2 ) ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	case 29 : // 2015.02.04 // Setting pos
		//
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
		//
		if ( next >= 0 ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				_i_SCH_MODULE_Set_TM_DoPointer( side , next );
			}
			else {
				_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , next );
			}
		}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RE.DOPT\t[(Side=%d,Pt=%d)(%s:29)][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	default : // normal
		if ( !_i_SCH_CLUSTER_FlowControl( side ) ) break;
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-NORMAL \t[(Side=%d,Pt=%d)(%s:%d)][STYLE=>%d(lc=%dth,Trg=%d)][NextPt=%d]\n" , side , pointer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 3 ) , i + 1 , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 2 ) , next );
//------------------------------------------------------------------------------------------------------------------
		break;
	}
	//======================================================================================================================================
	switch( mode ) { // 2008.10.31
	case 1 :
	case 2 :
	case 3 :
	case 4 :
		//
		waitctrlskip = FALSE; // 2010.04.10
		//
		switch ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 2 ) ) {
		case 20 : // lot pre
			wait = FALSE;
			break;
		default :
			wait = ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 12 , 2 ) % 4 ) / 2;
			break;
		}
		//
		next = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 ) - 1;
		//
		if ( next >= 0 ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				_i_SCH_MODULE_Set_TM_DoPointer( side , next );
			}
			else {
				_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , next );
			}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE1 [Side=%d,Pt=%d,Next=%d]\n" , side , pointer , next );
//------------------------------------------------------------------------------------------------------------------
			//
			if ( ( _i_SCH_CLUSTER_Get_PathStatus( side , next ) != SCHEDULER_READY ) && ( _i_SCH_CLUSTER_Get_PathStatus( side , next ) != SCHEDULER_CM_END ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-NOTRETURN\tNEXT1 [Side=%d,Pt=%d,Next=%d]\n" , side , pointer , next );
//------------------------------------------------------------------------------------------------------------------
				//
				switch ( _i_SCH_CLUSTER_Get_PathRun( side , next , 13 , 2 ) ) {
				case 1 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 5 ); break;
				case 2 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 6 ); break;
				case 3 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 7 ); break;
				case 4 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 8 ); break;
				case 9 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 5 ); break;
				case 10 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 6 ); break;
				case 11 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 7 ); break;
				case 12 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 8 ); break;
				}
				//
				if ( pointer == next ) waitctrlskip = TRUE; // 2010.04.10
				//
			}
		}
		else { // 2009.02.12
			//
			next = pointer;
			//
			while( TRUE ) {
				//
				next++;
				//
				if ( next >= MAX_CASSETTE_SLOT_SIZE ) break;
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , next ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_PathRun( side , next , 16 , 2 ) >= 20 ) continue;
				//
				if ( ( _i_SCH_CLUSTER_Get_PathStatus( side , next ) != SCHEDULER_READY ) && ( _i_SCH_CLUSTER_Get_PathStatus( side , next ) != SCHEDULER_CM_END ) ) {
					//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-NOTRETURN\tNEXT2 [Side=%d,Pt=%d,Next=%d][%d]\n" , side , pointer , next , _i_SCH_CLUSTER_Get_PathRun( side , next , 13 , 2 ) );
//------------------------------------------------------------------------------------------------------------------
					switch ( _i_SCH_CLUSTER_Get_PathRun( side , next , 13 , 2 ) ) {
					case 1 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 5 ); break;
					case 2 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 6 ); break;
					case 3 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 7 ); break;
					case 4 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 8 ); break;
					case 9 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 5 ); break;
					case 10 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 6 ); break;
					case 11 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 7 ); break;
					case 12 : _i_SCH_CLUSTER_Set_PathRun( side , next , 13 , 2 , 8 ); break;
					}
				}
				//
				break;
			}
			//
			if ( next >= MAX_CASSETTE_SLOT_SIZE ) { // 2009.05.27
				//
				if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					_i_SCH_MODULE_Set_TM_DoPointer( side , MAX_CASSETTE_SLOT_SIZE );
				}
				else {
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , MAX_CASSETTE_SLOT_SIZE );
				}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE2 [Side=%d,Pt=%d,Next=%d]\n" , side , pointer , next );
//------------------------------------------------------------------------------------------------------------------
				//
			}
			else { // 2009.12.29
				//
				if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					_i_SCH_MODULE_Set_TM_DoPointer( side , next );
				}
				else {
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , next );
				}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE3 [Side=%d,Pt=%d,Next=%d]\n" , side , pointer , next );
//------------------------------------------------------------------------------------------------------------------
				//
			}
			//
		}
		//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-WAITCHECK\tWAITCHECK [Side=%d,Pt=%d,Next=%d,WAIT=%d,MODE=%d,waitctrlskip=%d]\n" , side , pointer , next , wait , mode , waitctrlskip );
//------------------------------------------------------------------------------------------------------------------
		if ( !waitctrlskip ) { // 2010.04.10
			if ( wait ) { // Wait
				switch( mode ) {
				case 1 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 5 );	break;
				case 2 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 6 );	break;
				case 3 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 7 );	break;
				case 4 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 8 );	break;
				case 9 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 5 );	break;
				case 10 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 6 );	break;
				case 11 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 7 );	break;
				case 12 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 8 );	break;
				}
			}
			else { // 2009.12.29
				switch( mode ) {
				case 1 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 9 );	break;
				case 2 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 10 );	break;
				case 3 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 11 );	break;
				case 4 :	_i_SCH_CLUSTER_Set_PathRun( side , pointer , 13 , 2 , 12 );	break;
				}
			}
		}
		//=====================================================================================
		break;
	}
	//======================================================================================================================================
}



// 2008.01.15
void _SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( int side , int pointer , int firsttag , int lasttag , int cmstarteveryskip ) {
	int mode , i;
	int ich , isl;
	int och , osl;
	int picked;
	char MsgSltchar[16]; /* 2013.05.23 */

	//	first Tag
	//
	//	0	:	TIMER_FIRST		STS_FIRST_IN	SCHEDULER_SUPPLY
	//	1	:	TIMER_FIRST		STS_FIRST_IN	SCHEDULER_STARTING
	//	2	:	TIMER_FIRST		STS_FIRST_IN	SCHEDULER_RUNNING
	//	3	:	TIMER_FIRST		STS_FIRST_IN	x
	//
	//	10	:	TIMER_FIRST		x				SCHEDULER_SUPPLY
	//	11	:	TIMER_FIRST		x				SCHEDULER_STARTING
	//	12	:	TIMER_FIRST		x				SCHEDULER_RUNNING
	//	13	:	TIMER_FIRST		x				x
	//
	//	20	:	x				STS_FIRST_IN	SCHEDULER_SUPPLY
	//	21	:	x				STS_FIRST_IN	SCHEDULER_STARTING
	//	22	:	x				STS_FIRST_IN	SCHEDULER_RUNNING
	//	23	:	x				STS_FIRST_IN	x
	//
	//	30	:	x				x				SCHEDULER_SUPPLY
	//	31	:	x				x				SCHEDULER_STARTING
	//	32	:	x				x				SCHEDULER_RUNNING
	//	33	:	x				x				x

	//	cmstarteveryskip
	//
	//	0	:	CM_START	EVERY_IN
	//	1	:	CM_START	x
	//	2	:	x			EVERY_IN

	//	last Tag
	//
	//	0	:	TIMER_LAST		_i_SCH_SYSTEM_LASTING_SET
	//	1	:	TIMER_LAST		_i_SCH_SUB_Set_Last_Status
	//	2	:	TIMER_LAST		x
	//
	//	10	:	x				_i_SCH_SYSTEM_LASTING_SET
	//	11	:	x				_i_SCH_SUB_Set_Last_Status
	//	12	:	x				x

	//-----------------------------------------------------------------------------------------------------------------------
	if ( side < 0 ) return;
	if ( side >= MAX_CASSETTE_SIDE ) return;
	if ( pointer < 0 ) return;
	if ( pointer >= MAX_CASSETTE_SLOT_SIZE ) return;
	//-----------------------------------------------------------------------------------------------------------------------
	//
	Scheduler_LotPre_Data_Supply( side , pointer ); // 2017.10.19
	//
	//-----------------------------------------------------------------------------------------------------------------------
	ich = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
	isl = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	och = _i_SCH_CLUSTER_Get_PathOut( side , pointer );
	osl = _i_SCH_CLUSTER_Get_SlotOut( side , pointer );
	//-----------------------------------------------------------------------------------------------------------------------
	i    = _i_SCH_CLUSTER_Get_PathRun( side , pointer ,  0 , 2 ); // 2008.11.04
	mode = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 13 , 2 );
	//-----------------------------------------------------------------------------------------------------------------------
	memset(MsgSltchar, 0x00, sizeof (MsgSltchar)) ;
	if ( ich < PM1 ) { // 2009.02.13
		//-----------------------------------------------------------------------------------------------------------------------
		if ( firsttag >= 0 ) {
			if ( i == 0 ) { // 2008.11.04
				if ( _i_SCH_SUB_Chk_First_Out_Status( side , pointer ) ) {
					if ( ( ( firsttag / 10 ) == 0 ) || ( ( firsttag / 10 ) == 1 ) ) {
//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_FIRST , -1 , -1 , -1 , -1 , -1 , "" , "" ); // 2012.09.25
						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_FIRST , pointer , -1 , -1 , -1 , -1 , "" , "" ); // 2012.09.25
						//
						FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
						//
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
							_i_SCH_LOG_LOT_PRINTF( side , "FM Producing First Wafer from %s(%d)%cWHFMFWPRODUCE %s:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ich ) , isl , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , 9 , isl , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( side , "TM Producing First Wafer from %s(%d)%cWHTMFWPRODUCE %s:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ich ) , isl , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , 9 , isl , MsgSltchar );
						}
					}
					if ( ( ( firsttag / 10 ) == 0 ) || ( ( firsttag / 10 ) == 2 ) ) {
						if ( ich < PM1 ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_FIRSTIN , isl , ( och <= 0 ) ? 0 : och , osl , 0 , 0 , isl ); // 2011.05.25
					}
				}
			}
		}
		//-----------------------------------------------------------------------------------------------------------------------
		if ( lasttag >= 0 ) {
			if ( ( i + 1 ) >= _i_SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 2 ) ) { // 2009.02.02
//				if ( ( mode == 0 ) || ( mode == 9 ) ) { // 2008.11.04 // 2010.03.17 // 2011.04.12(Checking) // 2011.05.10
					//==================================================================================================================================
					//==================================================================================================================================
					//==================================================================================================================================
					//==================================================================================================================================
					if ( _i_SCH_SUB_Chk_Last_Out_Status( side , pointer ) ) {
						if      ( ( lasttag % 10 ) == 0 ) {
							_i_SCH_SYSTEM_LASTING_SET( side , TRUE );
						}
						else if ( ( lasttag % 10 ) == 1 ) {
							_i_SCH_SUB_Set_Last_Status( side , 1 );
						}
					}
					//==================================================================================================================================
					//==================================================================================================================================
					//==================================================================================================================================
					//==================================================================================================================================
//				} // 2011.04.12(Checking)
			}
			//
			if ( i == 0 ) { // 2009.02.02
				if ( _i_SCH_SUB_Chk_Last_Out_Status( side , pointer ) ) {
					if ( ( lasttag / 10 ) == 0 ) {
//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_LAST , -1 , -1 , -1 , -1 , -1 , "" , "" ); // 2012.09.25
						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_LAST , pointer , -1 , -1 , -1 , -1 , "" , "" ); // 2012.09.25
						//
						FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
						//
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
							if ( ich < PM1 ) _i_SCH_LOG_LOT_PRINTF( side , "FM Producing Last Wafer from %s(%d)%cWHFMLWPRODUCE %s:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ich ) , isl , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , 9 , isl , MsgSltchar );
						}
						else {
							if ( ich < PM1 ) _i_SCH_LOG_LOT_PRINTF( side , "TM Producing Last Wafer from %s(%d)%cWHTMLWPRODUCE %s:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ich ) , isl , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ich ) , isl , 9 , isl , MsgSltchar );
						}
					}
				}
			}
		}
		//-----------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------
	if ( ( cmstarteveryskip == 0 ) || ( cmstarteveryskip == 1 ) ) {
//		if ( ich < PM1 ) _i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_START , isl , ich , ( och <= 0 ) ? 0 : och , osl , -1 , "" , "" ); // 2011.05.25 // 2011.09.30
//		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_START , isl , ich , ( och <= 0 ) ? 0 : och , osl , -1 , "" , "" ); // 2011.05.25 // 2011.09.30 // 2012.02.18
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_START , isl , ich , ( och <= 0 ) ? 0 : och , osl , pointer , "" , "" ); // 2011.05.25 // 2011.09.30 // 2012.02.18
		//
		_i_SCH_CLUSTER_Set_Ex_PrcsID( side , pointer , 0 ); // 2013.05.06
		//
	}
	if ( ( cmstarteveryskip == 0 ) || ( cmstarteveryskip == 2 ) ) {
		_i_SCH_CLUSTER_Make_SupplyID( side , pointer ); // 2007.09.05
		_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side , pointer , FA_SUBSTRATE_EVERYIN , isl , ( och <= 0 ) ? 0 : och , osl , 0 , 0 , isl ); // 2011.05.25
	}
	//-----------------------------------------------------------------------------------------------------------------------
	if ( firsttag >= 0 ) {
		if      ( ( firsttag % 10 ) == 1 ) _i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_STARTING );
		else if ( ( firsttag % 10 ) == 2 ) _i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RUNNING );
		else if ( ( firsttag % 10 ) == 0 ) _i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_SUPPLY );
	}
	//-----------------------------------------------------------------------------------------------------------------------
	SCHEDULER_CASSETTE_LAST_RESULT_INIT_AFTER_CM_PICK( ich , isl , side , pointer );
	//-----------------------------------------------------------------------------------------------------------------------
	picked = FALSE;
	//
	if ( ( cmstarteveryskip == 0 ) || ( cmstarteveryskip == 1 ) ) { // 2009.01.15
		picked = TRUE;
	}
	else {
		if ( firsttag >= 0 ) {
			if ( ( ( firsttag / 10 ) == 0 ) || ( ( firsttag / 10 ) == 1 ) ) { // 2009.01.15
				picked = TRUE;
			}
		}
	}
	//======================================================================================================================================
	_i_SCH_FLOW_CONTROL_CHECK_AFTER_PICK_Sub( side , pointer , picked );
	//======================================================================================================================================
}

extern int  _In_Run_Finished_Check[MAX_CASSETTE_SIDE];

BOOL _SCH_MACRO_FM_FINISHED_CHECK( int side , BOOL supplycheck ) {
	//
	EnterCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
		return TRUE;
	}
	//
	if ( SIGNAL_MODE_WAITR_CHECK( side ) ) {
		LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
		return FALSE;
	}
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.16
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( supplycheck ) {
				if ( _i_SCH_MODULE_Get_TM_OutCount( side ) <= 0 ) {
					LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
					return FALSE;
				}
			}
			if ( !_i_SCH_MODULE_Chk_TM_Finish_Status( side ) ) {
				LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
				return FALSE;
			}
			if ( !_i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
				LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
				return FALSE;
			}
		}
		else {
			if ( supplycheck ) {
				if ( _i_SCH_MODULE_Get_FM_OutCount( side ) <= 0 ) {
					LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
					return FALSE;
				}
			}
			if ( !_i_SCH_MODULE_Chk_FM_Finish_Status_FDHC( side ) ) {
				LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
				return FALSE;
			}
			if ( !_i_SCH_MODULE_Chk_FM_Free_Status( side ) ) {
				LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
				return FALSE;
			}
		}
	}
	else {
		if ( _i_SCH_SYSTEM_APPEND_END_GET( side ) == 0 ) {
			LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
			return FALSE;
		}
		if ( !_i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
			LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
			return FALSE;
		}
	}
	//
	if ( _SCH_COMMON_FINISHED_CHECK( side ) ) {
		_In_Run_Finished_Check[side] = TRUE; // 2011.04.19
		LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
		return TRUE; // 2009.08.04
	}
	//
	LeaveCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
	return FALSE; // 2009.08.04
//	return TRUE; // 2009.08.04
}

int _SCH_MACRO_INTERRUPT_PART_CHECK( int side , BOOL partial , int tmthreadblockingcheck ) {
	int res = 0;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	//
	if ( _i_SCH_SYSTEM_MODE_END_GET( side ) != 0 ) {
		if ( _i_SCH_SYSTEM_MODE_RESUME_GET( side ) ) {
			if ( partial ) return 0;
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) ||
				 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 4 ) ) {
				_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
				_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
				_i_SCH_SYSTEM_MODE_RESUME_SET( side , FALSE );
				res = 101;
			}
			else if ( ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 5 ) ||
					  ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 6 ) ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					if ( _i_SCH_MODULE_Chk_FM_Free_Status( side ) ) {
						_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop_FDHC( side , _i_SCH_PRM_GET_MODULE_MODE( FM ) );
						_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
						_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
						_i_SCH_SYSTEM_MODE_RESUME_SET( side , FALSE );
						res = 102;
					}
				}
				else {
					if ( _i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
						_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Stop_FDHC( side , _i_SCH_PRM_GET_MODULE_MODE( FM ) );
						_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
						_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
						_i_SCH_SYSTEM_MODE_RESUME_SET( side , FALSE );
						res = 103;
					}
				}
			}
			else {
				_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
				_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
				_i_SCH_SYSTEM_MODE_RESUME_SET( side , FALSE );
				res = 104;
			}
			_SCH_IO_SET_MAIN_BUTTON_MC( side , CTC_RUNNING );
		}
		else {
			if ( ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) ||
				 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 4 ) ) {

//				if ( tmthreadblockingcheck >= 0 ) EnterCriticalSection( &CR_TMRUN[tmthreadblockingcheck] ); // 2008.07.29
				//
				if ( tmthreadblockingcheck >= 0 ) {
					if ( Get_Inside_Value_for_Common_Thread_Status( side , tmthreadblockingcheck ) >= 1000 ) return 1001; // 2008.07.29
				}
				//
				if ( !_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_FDHC( side , _i_SCH_PRM_GET_MODULE_MODE( FM ) , -1 ) ) {

					if      ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) _i_SCH_SYSTEM_MODE_END_SET( side , 1 );
					else if ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 4 ) _i_SCH_SYSTEM_MODE_END_SET( side , 2 );
					res = 1;
				}
				else {

					if      ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) _i_SCH_SYSTEM_MODE_END_SET( side , 5 );
					else if ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 4 ) _i_SCH_SYSTEM_MODE_END_SET( side , 6 );
					res = 2;
				}

				_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );

//				if ( tmthreadblockingcheck >= 0 ) LeaveCriticalSection( &CR_TMRUN[tmthreadblockingcheck] ); // 2008.07.29

			}
			else if ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 7 ) {

//				if ( tmthreadblockingcheck >= 0 ) EnterCriticalSection( &CR_TMRUN[tmthreadblockingcheck] ); // 2008.07.29
				//
				if ( tmthreadblockingcheck >= 0 ) {
					if ( Get_Inside_Value_for_Common_Thread_Status( side , tmthreadblockingcheck ) >= 1000 ) return 1002; // 2008.07.29
				}
				//

				_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop_FDHC( side , _i_SCH_PRM_GET_MODULE_MODE( FM ) , -1 );
				_i_SCH_SYSTEM_MODE_END_SET( side , 1 );
				_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );

//				if ( tmthreadblockingcheck >= 0 ) LeaveCriticalSection( &CR_TMRUN[tmthreadblockingcheck] ); // 2008.07.29
				res = 3;
			}
			else {
				if ( partial ) return 0;
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					if ( _i_SCH_MODULE_Chk_FM_Free_Status( side ) ) {
						if ( ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 1 ) ||
							 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) ||
							 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 5 ) ) {
							_i_SCH_MODULE_Set_FM_End_Status_FDHC( side );
							_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
							_i_SCH_SYSTEM_MODE_LOOP_SET( side , 2 );
							_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
							res = 99;
						}
						else {
							if ( !_i_SCH_SYSTEM_MODE_WAITR_GET( side ) ) {
								_i_SCH_SYSTEM_MODE_WAITR_SET( side , TRUE );
								_SCH_IO_SET_MAIN_BUTTON_MC( side , CTC_PAUSED );
								res = 5;
							}
						}
					}
				}
				else {
					if ( _i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
						if ( ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 1 ) ||
							 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 3 ) ||
							 ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 5 ) ) {
							_i_SCH_MODULE_Set_TM_End_Status( side );
							_i_SCH_SYSTEM_MODE_END_SET( side , 0 );
							_i_SCH_SYSTEM_MODE_LOOP_SET( side , 2 );
							_i_SCH_SYSTEM_MODE_WAITR_SET( side , FALSE );
							res = 99;
						}
						else {
							if ( !_i_SCH_SYSTEM_MODE_WAITR_GET( side ) ) {
								_i_SCH_SYSTEM_MODE_WAITR_SET( side , TRUE );
								_SCH_IO_SET_MAIN_BUTTON_MC( side , CTC_PAUSED );
								res = 4;
							}
						}
					}
				}
			}
		}
	}
	return res;
}


//=======================================================================================
int _SCH_inside_Get_Dummy_Muti_Wait_UsingPM( int side , int pointer , int ch , int *order ) {
	int l;
	//
	if ( 23 == _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 2 ) ) return FALSE;
	//
	for ( l = 0 ; l < _i_SCH_CLUSTER_Get_PathRun( side , pointer , 20 , 2 ) ; l++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 2 ) == ( ch - PM1 + 1 ) ) {
			*order = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 3 ) - _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 );
			return TRUE;
		}
	}
	//
	return FALSE;
}
//
int _SCH_inside_Get_Dummy_Use_OnePM( int side , int pointer , int ch ) {
	int i , l , m , order = -2 , f , o;
	//
	for ( l = 0 ; l < _i_SCH_CLUSTER_Get_PathRun( side , pointer , 20 , 2 ) ; l++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 2 ) == ( ch - PM1 + 1 ) ) {
			order = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 3 ) - 1 - _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 );
		}
	}
	//
	if ( order == -2 ) return TRUE;
	if ( order != -1 ) {
		if ( order !=  0 ) return FALSE;
	}
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		//
		if ( ( order != -1 ) && ( order != i ) ) continue;
		//
		f = 0;
		o = 0;
		//
		for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , i , l );
			if ( m > 0 ) {
				if ( m == ch ) f = 1;
				else           o = 1;
			}
		}
		//
		if ( f == 1 ) {
			if ( o == 0 ) {
				if ( order == 0 ) return TRUE;
			}
			return FALSE;
		}
	}
	//
	return TRUE;
}
//=======================================================================================
int _SCH_inside_Get_DLL_Dummy( int postwait , int side , int pointer , int mode , int selpm , int selparam , int *dummychamber , int *dummyslot , char *SelectRecipe ) {
	int x = 0 , s , sch , ch , ssl , i , dbmc;
	//
	if ( postwait != 1 ) {
		x = USER_RECIPE_DUMMY_OPERATE( side , pointer , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , mode , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 0 , 2 ) , selpm , selparam , dummychamber , dummyslot , SelectRecipe );
		// x =    0 : Dummy Used with Default Search
		//        1 : Dummy Used with This Parameter
		//        2 : Not Used Dummy
		//       -1 : Reject
		//
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-GET.DUMMY3\t[DLL][M=%d][PW=%d][Res=%d][DCH=%d][DSL=%d][prm=%d][PM%d][%s]\n" , mode , postwait , x , *dummychamber , *dummyslot , selparam , selpm == 0 ? 0 : selpm - PM1 + 1 , SelectRecipe );
//------------------------------------------------------------------------------------------------------------------
	if ( x == -1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Reject\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
	if ( x ==  2 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Not Used Dummy\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return 0;
	}
	else if ( x != 1 ) {
		//
		*dummychamber = -1;
		dbmc = 0;
		//
		for ( s = 0 ; s < 2 ; s++ ) {
			//
			if ( s == 0 ) sch = _SCH_TAG_DUMMY_FLOW_CH;
			else          sch = BM1;
			//
			for ( ch = sch ; ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ; ch++ ) {
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) continue;
				if ( !SCH_Inside_ThisIs_BM_OtherChamber( ch , 1 ) ) continue;
				//
				dbmc++;
				//
				if ( s == 0 ) {
					if ( ch == sch ) ssl = _SCH_TAG_DUMMY_FLOW_SLOT + 1;
					else             ssl = 1;
				}
				else {
					ssl = 1;
				}
				//
				for ( i = ssl ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					//
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , i ) <= 0 ) continue;
					//
					*dummychamber = ch;
					*dummyslot = i;
					break;
				}
				if ( *dummychamber != -1 ) break;
			}
			if ( *dummychamber != -1 ) break;
		}
		//
		if ( dbmc == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Not Using Dummy\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
			return 0;
		}
		//
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-GET.DUMMY4\t[DCH=%d][DSL=%d]\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
	if ( ( *dummychamber < BM1 ) || ( *dummychamber >= TM ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Invalid Chamber\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( *dummychamber , FALSE , -1 ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Disable\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
	if ( ( *dummyslot <= 0 ) || ( *dummyslot > MAX_CASSETTE_SLOT_SIZE ) ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Invalid Slot\n" ,  *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
	if ( _i_SCH_IO_GET_MODULE_STATUS( *dummychamber , *dummyslot ) <= 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Nothing Wafer\n" , *dummychamber , *dummyslot );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
	return 1;
}

void _SCH_inside_ConvStr_29_Dummy( char *SelectRecipe ) {
	int i , l;
	l = strlen( SelectRecipe );
	for ( i = 0 ; i < l ; i++ ) {
		if ( SelectRecipe[i] == 29 ) SelectRecipe[i] = ':';
	}
}

int _SCH_inside_Get_Dummy( int side , int pointer , int mode , int tag2 , int tag3 , int cmd , int curnotuse ) {
	int i , x , rcpcheck , ssl , Org_Main_Pointer , postwait , newskip = 0 , ml;
	int dummychamber , dummyslot;
	int selpm , selorder , selparam;
	//
	int z1 , z2;
	//
//	char RemainRecipe[5120]; // 2010.01.29
//	char RemainRecipe[6401]; // 2010.01.29 // 2010.02.03
	char *RemainRecipe; // 2010.01.29 // 2010.02.03
	//
	char tempstr[512];
	char tempstr2[512];
	char SelectRecipe[512];
	//
	char *PathProcessRecipe[ 3 ];
	//

//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-GET.DUMMY1\t[Side=%d][Pt=%d][Mode=%d][T2=%d][T3=%d][Cmd=%d][CUR=%d]\n" , side , pointer , mode , tag2 , tag3 , cmd , curnotuse );
_IO_CONSOLE_PRINTF( "FLOWCTL-GET.DUMMY2\t[RECIPE=%s,%s,%s,%s]\n" , _i_SCH_CLUSTER_Get_UserArea( side , pointer ) , _i_SCH_CLUSTER_Get_UserArea2( side , pointer ) , _i_SCH_CLUSTER_Get_UserArea3( side , pointer ) , _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) );
//------------------------------------------------------------------------------------------------------------------
	//
	ml = 0;
	//
	selpm = 0;
	selorder = -1;
	selparam = -1;
	//
	dummychamber = -1;
	dummyslot = -1;
	//
	strcpy( SelectRecipe , "" );
	//
	rcpcheck = -1;
	//
	switch( mode ) {
	case 0 : // Lot Pre
		if ( _i_SCH_CLUSTER_Get_UserArea( side , pointer ) != NULL ) {
			//
			ml = strlen( _i_SCH_CLUSTER_Get_UserArea( side , pointer ) );
			//
			RemainRecipe = calloc( ml + 1 , sizeof( char ) );
			//
			if ( RemainRecipe == NULL ) {
				_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 1 Memory Allocate Error\n" );
				return -1;
			}
			//
			STR_SEPERATE_CHAR( _i_SCH_CLUSTER_Get_UserArea( side , pointer ) , ':' , tempstr , RemainRecipe , ml );
			//
			STR_SEPERATE_CHAR( tempstr , '>' , SelectRecipe , tempstr2 , 511 );
			//
			selpm    = atoi( tempstr2 ) % 100;
			selorder = ( atoi( tempstr2 ) % 10000 ) / 100;
			selparam = atoi( tempstr2 ) / 10000;
			//
			rcpcheck = strlen( RemainRecipe );
			//
		}
		break;
		//
	case 1 : // Wafer Pre
		if ( _i_SCH_CLUSTER_Get_UserArea2( side , pointer ) != NULL ) {
			//
			ml = strlen( _i_SCH_CLUSTER_Get_UserArea2( side , pointer ) );
			//
			RemainRecipe = calloc( ml + 1 , sizeof( char ) );
			//
			if ( RemainRecipe == NULL ) {
				_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 2 Memory Allocate Error\n" );
				return -1;
			}
			//
			STR_SEPERATE_CHAR( _i_SCH_CLUSTER_Get_UserArea2( side , pointer ) , ':' , tempstr , RemainRecipe , ml );
			//
			STR_SEPERATE_CHAR( tempstr , '>' , SelectRecipe , tempstr2 , 511 );
			//
			selpm    = atoi( tempstr2 ) % 100;
			selorder = ( atoi( tempstr2 ) % 10000 ) / 100;
			selparam = atoi( tempstr2 ) / 10000;
			//
			rcpcheck = strlen( RemainRecipe );
			//
		}
		break;
		//
	case 2 : // Wafer Post
		if ( _i_SCH_CLUSTER_Get_UserArea3( side , pointer ) != NULL ) {
			//
			ml = strlen( _i_SCH_CLUSTER_Get_UserArea3( side , pointer ) );
			//
			RemainRecipe = calloc( ml + 1 , sizeof( char ) );
			//
			if ( RemainRecipe == NULL ) {
				_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 3 Memory Allocate Error\n" );
				return -1;
			}
			//
			STR_SEPERATE_CHAR( _i_SCH_CLUSTER_Get_UserArea3( side , pointer ) , ':' , tempstr , RemainRecipe , ml );
			//
			STR_SEPERATE_CHAR( tempstr , '>' , SelectRecipe , tempstr2 , 511 );
			//
			selpm    = atoi( tempstr2 ) % 100;
			selorder = ( atoi( tempstr2 ) % 10000 ) / 100;
			selparam = atoi( tempstr2 ) / 10000;
			//
			rcpcheck = strlen( RemainRecipe );
			//
		}
		break;
		//
	case 3 : // Lot Post
		if ( _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) != NULL ) {
			//
			ml = strlen( _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) );
			//
			RemainRecipe = calloc( ml + 1 , sizeof( char ) );
			//
			if ( RemainRecipe == NULL ) {
				_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 4 Memory Allocate Error\n" );
				return -1;
			}
			//
			STR_SEPERATE_CHAR( _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) , ':' , tempstr , RemainRecipe , ml );
			//
			STR_SEPERATE_CHAR( tempstr , '>' , SelectRecipe , tempstr2 , 511 );
			//
			selpm    = atoi( tempstr2 ) % 100;
			selorder = ( atoi( tempstr2 ) % 10000 ) / 100;
			selparam = atoi( tempstr2 ) / 10000;
			//
			rcpcheck = strlen( RemainRecipe );
			//
		}
		break;
		//
	}
	//
	_SCH_inside_ConvStr_29_Dummy( SelectRecipe );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( i == pointer ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) {
			//==========================================================================================================
			Org_Main_Pointer = 0;
			postwait = 0;
			//
			if ( mode > 1 ) { // post part
				Org_Main_Pointer = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 17 , 2 ) - 1;
				if ( Org_Main_Pointer < 0 ) Org_Main_Pointer = pointer;
				//
				if ( mode == 2 ) { // wafer post
					//
					if ( !_SCH_inside_Get_Dummy_Use_OnePM( side , Org_Main_Pointer , selpm + PM1 - 1 ) ) {
						//
						postwait = 1;
						//
						Set_Inf_Dummy_PostWait( side , i , mode , selparam , SelectRecipe );
						//
					}
					//
				}
				//
				else if ( mode == 3 ) { // lot post // 2009.06.03
					//
					if ( _SCH_inside_Get_Dummy_Muti_Wait_UsingPM( side , Org_Main_Pointer , selpm + PM1 - 1 , &selorder ) ) {
						//
						postwait = 1;
						//
						Set_Inf_Dummy_PostWait( side , i , mode , selparam , SelectRecipe );
						//
					}
					//
				}
				//
			}
			//==========================================================================================================
			x = _SCH_inside_Get_DLL_Dummy( postwait , side , pointer , mode , ( selpm == 0 ) ? 0 : selpm + PM1 - 1 , selparam , &dummychamber , &dummyslot , SelectRecipe );
			//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-DLL.DUMMY\t[AppPt=%d][DCH=%d,DSL=%d][PM%d][SO=%d][postwait=%d][Param=%d][%s][x=%d][rc=%d][ml=%d][ns=%d]\n" , i , dummychamber , dummyslot , selpm , selorder , postwait , selparam , SelectRecipe , x , rcpcheck , ml , newskip );
//------------------------------------------------------------------------------------------------------------------
			if      ( x == 0 ) { // 2010.01.19 Skip
				newskip = 1;
			}
			else if ( x != 1 ) {
				if ( rcpcheck >= 0 ) free( RemainRecipe );
				return x;
			}
			//
			if ( postwait == 0 ) {
				_SCH_TAG_DUMMY_FLOW_CH = dummychamber;
				_SCH_TAG_DUMMY_FLOW_SLOT = dummyslot;
			}
			//==========================================================================================================
/*
// 2010.02.02
			if ( rcpcheck >= 0 ) {
				switch( mode ) {
				case 0 : // Lot Pre
					_i_SCH_CLUSTER_Set_UserArea( side , pointer , RemainRecipe );
					break;
					//
				case 1 : // Wafer Pre
					_i_SCH_CLUSTER_Set_UserArea2( side , pointer , RemainRecipe );
					break;
					//
				case 2 : // Wafer Post
					_i_SCH_CLUSTER_Set_UserArea3( side , i , RemainRecipe );
					_i_SCH_CLUSTER_Set_UserArea4( side , i , _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) );
					//
					_i_SCH_CLUSTER_Set_UserArea3( side , pointer , NULL );
					_i_SCH_CLUSTER_Set_UserArea4( side , pointer , NULL );
					break;
					//
				case 3 : // Lot Post
					_i_SCH_CLUSTER_Set_UserArea4( side , i , RemainRecipe );
					//
					_i_SCH_CLUSTER_Set_UserArea4( side , pointer , NULL );
					break;
					//
				}
				//
				free( RemainRecipe );
				//
			}
*/
			//==========================================================================================================
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-APP.DUMMY\t[AppPt=%d][DCH=%d,DSL=%d][PM%d][SO=%d][postwait=%d][Param=%d][%s]\n" , i , dummychamber , dummyslot , selpm , selorder , postwait , selparam , SelectRecipe );
//------------------------------------------------------------------------------------------------------------------
			//==========================================================================================================
			if ( !newskip ) _i_SCH_CLUSTER_Copy_Cluster_Data( side , i , side , pointer , 0 ); // 2010.01.20
			//
			if ( rcpcheck >= 0 ) { // 2010.02.02
				switch( mode ) {
				case 0 : // Lot Pre
					_i_SCH_CLUSTER_Set_UserArea( side , pointer , RemainRecipe );
					break;
					//
				case 1 : // Wafer Pre
					_i_SCH_CLUSTER_Set_UserArea2( side , pointer , RemainRecipe );
					break;
					//
				case 2 : // Wafer Post
					//
					if ( newskip ) { // 2010.02.04
						_i_SCH_CLUSTER_Set_UserArea3( side , pointer , RemainRecipe );
					}
					else {
						_i_SCH_CLUSTER_Set_UserArea3( side , i , RemainRecipe );
						_i_SCH_CLUSTER_Set_UserArea4( side , i , _i_SCH_CLUSTER_Get_UserArea4( side , pointer ) );
						//
						_i_SCH_CLUSTER_Set_UserArea3( side , pointer , NULL );
						_i_SCH_CLUSTER_Set_UserArea4( side , pointer , NULL );
					}
					break;
					//
				case 3 : // Lot Post
					if ( newskip ) { // 2010.02.04
						_i_SCH_CLUSTER_Set_UserArea4( side , pointer , RemainRecipe );
					}
					else {
						_i_SCH_CLUSTER_Set_UserArea4( side , i , RemainRecipe );
						//
						_i_SCH_CLUSTER_Set_UserArea4( side , pointer , NULL );
					}
					break;
					//
				}
				//
				free( RemainRecipe );
				//
			}
			//
			if ( newskip ) {
				if ( rcpcheck > 0 ) return -1;
				return 0;
			}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-APP.DUMMY2\t[AppPt=%d][DCH=%d,DSL=%d][PM%d][SO=%d][postwait=%d][Param=%d][%s]\n" , i , dummychamber , dummyslot , selpm , selorder , postwait , selparam , SelectRecipe );
//------------------------------------------------------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PathIn( side , i , dummychamber );
			_i_SCH_CLUSTER_Set_PathOut( side , i , dummychamber );
			_i_SCH_CLUSTER_Set_SlotIn( side , i , dummyslot );
			_i_SCH_CLUSTER_Set_SlotOut( side , i , dummyslot );
			//
			if ( mode > 1 ) { // 2009.03.15 // post part
//				_i_SCH_CLUSTER_Set_ClusterIndex( side , i , _i_SCH_CLUSTER_Get_ClusterIndex( side , Org_Main_Pointer ) + 1 ); // 2010.01.26
				_i_SCH_CLUSTER_Set_ClusterIndex( side , i , _i_SCH_CLUSTER_Get_ClusterIndex( side , Org_Main_Pointer ) ); // 2010.01.26
			}
			//
			if ( SCHMULTI_MESSAGE_GET_ALL( side , pointer , tempstr , 511 ) ) { // 2010.11.30
				if ( tempstr[0] == '|' ) {
					_i_SCH_CLUSTER_Set_Ex_EILInfo( side , i , tempstr + 1 );
					//
					_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , i , -99 );
					_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , i , 0 );
				}
				else {
					_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , i , -1 );
					_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , i , -1 );
				}
			}
			else {
				_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , i , -1 );
				_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , i , -1 );
			}
			//==========================================================================================================
			if ( strlen( SelectRecipe ) > 0 ) {
				if ( selpm > 0 ) {
					//
					// 2009.01.20
					x = -1;
					//
					PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_IN] = NULL;
					PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] = NULL;
					PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] = NULL;
					//
					for ( z1 = 0 ; z1 < MAX_CLUSTER_DEPTH ; z1++ ) {
						for ( z2 = 0 ; z2 < MAX_PM_CHAMBER_DEPTH ; z2++ ) {
							if ( _SCH_INF_CLUSTER_DATA_AREA[side][pointer].PathProcess[z1][z2] == ( selpm + PM1 - 1 ) ) {
								if ( !STR_MEM_MAKE_COPY2( &(PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_IN] )  , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , z1 , z2 , SCHEDULER_PROCESS_RECIPE_IN ) ) ) { // 2010.03.25
									_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 1 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , pointer , i , z1 , z2 );
								}
								if ( !STR_MEM_MAKE_COPY2( &(PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , z1 , z2 , SCHEDULER_PROCESS_RECIPE_OUT ) ) ) { // 2010.03.25
									_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 2 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , pointer , i , z1 , z2 );
								}
								if ( !STR_MEM_MAKE_COPY2( &(PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] )  , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , z1 , z2 , SCHEDULER_PROCESS_RECIPE_PR ) ) ) { // 2010.03.25
									_IO_CIM_PRINTF( "_SCH_inside_Get_Dummy 3 Memory Allocate Error[%d,%d,%d,%d,%d]\n" , side , pointer , i , z1 , z2 );
								}
								x = (z1 * 1000)+z2;
								break;
							}
						}
						if ( x != -1 ) break;
					}
					//
					_i_SCH_CLUSTER_Clear_PathProcessData( side , i , TRUE );
					//
					if ( x != -1 ) { // 2010.01.26
						if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] != NULL )  _i_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , x / 1000 , x % 1000 , PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] );
						if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] != NULL ) _i_SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , x / 1000 , x % 1000 , PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] );
					}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RCP.DUMMY\t[AppPt=%d][DCH=%d,DSL=%d][PM%d][SO=%d][Param=%d][%s]\n" , i , dummychamber , dummyslot , selpm , selorder , selparam , SelectRecipe );
//------------------------------------------------------------------------------------------------------------------
//					if ( USER_RECIPE_DUMMY_INFO_VERIFICATION( side , i , dummychamber , dummyslot , selpm , selparam , SelectRecipe , &_SCH_INF_CLUSTER_DATA_AREA[ side ][ i ] ) == 0 ) { // 2009.05.20
					z1 = USER_RECIPE_DUMMY_INFO_VERIFICATION( side , i , dummychamber , dummyslot , selpm , selparam , SelectRecipe , &_SCH_INF_CLUSTER_DATA_AREA[ side ][ i ] );
					//
					if ( ( z1 % 2 ) == 0 ) { // 0,2,4,6,8,...
						//
						_i_SCH_CLUSTER_Set_PathRange( side , i , 1 );
						//
						_i_SCH_CLUSTER_Set_PathProcessData( side , i , 0 , 0 , selpm + PM1 - 1 , SelectRecipe , "" , "" );
						//
					}
					//
					if ( ( z1 / 2 ) == 0 ) { // 0,1
						if ( x != -1 ) { // 2010.01.26
							if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] != NULL )  _i_SCH_CLUSTER_Set_PathProcessData_JustPre( side , i , 0 , 0 , PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] );
							if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] != NULL ) _i_SCH_CLUSTER_Set_PathProcessData_JustPost( side , i , 0 , 0 , PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] );
						}
					}
					//
					if ( x != -1 ) { // 2010.01.26
						if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_IN] != NULL ) free( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_IN] );
						if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] != NULL ) free( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_PR] );
						if ( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] != NULL ) free( PathProcessRecipe[SCHEDULER_PROCESS_RECIPE_OUT] );
					}
					//
				}
				else {
					Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( 0 , -1 , 1 , side , i , dummychamber , dummyslot , SelectRecipe , NULL );
				}
			}
			//==========================================================================================================
			_i_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_READY );
			_i_SCH_CLUSTER_Set_PathDo( side , i , 0 );
			//==========================================================================================================
			if ( mode > 1 ) { // post part
				//==========================================================================================================
				x = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 );
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 11 , 2 , i + 1 );
				//
				//==========================================================================================================
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  0 , 2 , 0 ); // Picked Count
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  8 , 2 , 0 ); // Delete ClusterDepth
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  9 , 2 , 1 ); // Dll Call Checking
				//
				if ( ( _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 0 , 2 ) + 1 ) >= _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 10 , 2 ) ) { // Last
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 2 , 1 ); // Total Loop Count
				}
				else {
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 2 , 2 ); // Total Loop Count
				}
				//
				if ( x <= 0 ) {
					//
					ssl = _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 16 , 2 );
					//
					switch ( ssl ) {
					case 8 : // Last Loop + Last Wafer
					case 11 : // First/Last Loop + Last Wafer
					case 12 : // Last Loop + First/Last Wafer
					case 15 : // First/Last Loop + First/Last Wafer
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-LAST.CONFIRM\t[Side=%d][pt=%d][AppPt=%d][orgpt=%d][Type=%d]\n" , side , pointer , i , Org_Main_Pointer , ssl );
//------------------------------------------------------------------------------------------------------------------
						_i_SCH_CLUSTER_Set_PathRun( side , i , 11 , 2 , 0 ); // Next Go Pointer
						break;
					default :
						_i_SCH_CLUSTER_Set_PathRun( side , i , 11 , 2 , pointer + 2 ); // Next Go Pointer
						break;
					}
				}
				else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-GO.REMAP\t[Side=%d][pt=%d][AppPt=%d][orgpt=%d][X=%d]\n" , side , pointer , i , Org_Main_Pointer , x );
//------------------------------------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathRun( side , i , 11 , 2 , x ); // Next Go Pointer
				}
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i , 12 , 2 , tag2 );	// Before Pick Wait Control
				_i_SCH_CLUSTER_Set_PathRun( side , i , 13 , 2 , tag3 );	// After Pick Wait Control
				//
				if ( mode == 2 ) { // wafer post
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 2 , ( rcpcheck > 0 ) ? cmd : cmd - 4 ); // Lot Pre/Post Use
				}
				else {
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 2 , ( rcpcheck > 0 ) ? cmd : cmd - 8 ); // Lot Pre/Post Use
				}
				//
				x = _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 16 + 3 - mode , 4 );
				_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 16 + 3 - mode , 4 , x + 1 );
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 2 , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 15 , 2 ) ); // Lot Pre/Post Style
				_i_SCH_CLUSTER_Set_PathRun( side , i , 16 , 2 , mode + 20 ); // Wafer FlowControl Detail Style
				_i_SCH_CLUSTER_Set_PathRun( side , i , 16 , 3 , x );
				_i_SCH_CLUSTER_Set_PathRun( side , i , 17 , 2 , Org_Main_Pointer + 1 ); // Reference Normal Wafer Pointer
				//
				if ( postwait == 1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-POST.WAIT\t[Side=%d][pt=%d][AppPt=%d][orgpt=%d]\n" , side , pointer , i , Org_Main_Pointer );
//------------------------------------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathRun( side , i , 18 , 2 , Org_Main_Pointer + 1 );  // Waiting Reference Pointer
					_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 2 , selpm + PM1 - 1 ); // Waiting Reference Module
					_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 3 , selorder <= 0 ? 0 : selorder - _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 8 , 2 ) ); // Waiting Reference Order
				}
				else {
					_i_SCH_CLUSTER_Set_PathRun( side , i , 18 , 2 , 0 ); // Waiting Reference Pointer
					_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 2 , selpm + PM1 - 1 ); // Waiting Reference Module
					_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 3 , selorder <= 0 ? 0 : selorder - _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 8 , 2 ) ); // Waiting Reference Order
				}
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 2 , 0 ); // Detail Retuning Info Count
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 3 , 0 ); // 2010.02.01
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 4 , 0 ); // 2010.02.01
				//==========================================================================================================
				// 2010.02.10
				//==========================================================================================================
				// 10 , 3	���� Dummy�� ���� Mode
				//	---+--------+--------+---------+---------+-------------------------------------------+
				//	   | LotPre | WfrPre | WfrPost | LotPost | Pre Condition                             |
				//	---+--------+--------+---------+---------+-------------------------------------------+
				//	0  |    X   |    X   |    X    |    X    | LotPre(X) WfrPre(X) WfrPost(X) LotPost(X) |
				//	1  |    O   |    X   |    X    |    X    | LotPre(O)                                 |
				//	2  |    X   |    O   |    X    |    X    | LotPre(X) WfrPre(O)                       |
				//	3  |    O   |    O   |    X    |    X    | LotPre(O)                                 |
				//	4  |    X   |    X   |    O    |    X    |           WfrPre(X) WfrPost(O)            |
				//	5  |    O   |    X   |    O    |    X    | LotPre(O)                                 |
				//	6  |    X   |    O   |    O    |    X    | LotPre(X) WfrPre(O)                       |
				//	7  |    O   |    O   |    O    |    X    | LotPre(O)                                 |
				//	8  |    X   |    X   |    X    |    O    |                     WfrPost(X) LotPost(O) |
				//	9  |    O   |    X   |    X    |    O    | LotPre(O)                                 |
				//	10 |    X   |    O   |    X    |    O    | LotPre(X) WfrPre(O)                       |
				//	11 |    O   |    O   |    X    |    O    | LotPre(O)                                 |
				//	12 |    X   |    X   |    O    |    O    |           WfrPre(X) WfrPost(O)            |
				//	13 |    O   |    X   |    O    |    O    | LotPre(O)                                 |
				//	14 |    X   |    O   |    O    |    O    | LotPre(X) WfrPre(O)                       |
				//	15 |    O   |    O   |    O    |    O    | LotPre(O)                                 |
				//	---+--------+--------+---------+---------+-------------------------------------------+
				//
				if ( ( mode + 1 ) == _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 10 , 3 ) ) {
					//
					x = ( _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 14 , 4 ) * 100 ) + _i_SCH_CLUSTER_Get_PathRun( side , Org_Main_Pointer , 14 , 3 ) + 1;
					//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 14 , 3 , x % 100 );	// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 14 , 4 , x / 100 );	// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 15 , 3 , 0 );			//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 15 , 4 , 0 );			//
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 3 , x % 100 );					// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 4 , x / 100 );					// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 3 , 0 );						//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 4 , 0 );						//
				}
				else {
					//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 10 , 3 , mode + 1 );	// ���� Mode�� ������ Dummy�� Mode
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 14 , 3 , 0 );			// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 14 , 4 , 0 );			// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 15 , 3 , 0 );			//
					_i_SCH_CLUSTER_Set_PathRun( side , Org_Main_Pointer , 15 , 4 , 0 );			//
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 3 , 0 );						// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 4 , 0 );						// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 3 , 0 );						//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 4 , 0 );						//
				}
				//
				if ( mode == 2 ) { // wafer post
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 3 , ( rcpcheck > 0 ) ? cmd : cmd - 4 ); // ���� Dummy�� ���� Mode
				}
				else {
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 3 , ( rcpcheck > 0 ) ? cmd : cmd - 8 ); // ���� Dummy�� ���� Mode
				}
				//
				//==========================================================================================================
				if ( curnotuse == 1 ) {
					if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
						_i_SCH_MODULE_Set_TM_DoPointer( side , i );
					}
					else {
						_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , i );
					}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE11 [Side=%d,Pt=%d]\n" , side , i );
//------------------------------------------------------------------------------------------------------------------
					//
					_i_SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , side , pointer , 4 );
				}
				//==========================================================================================================
				return 1;
			}
			else {
				//==========================================================================================================
				if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					_i_SCH_MODULE_Set_TM_DoPointer( side , i );
				}
				else {
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , i );
				}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE12 [Side=%d,Pt=%d]\n" , side , i );
//------------------------------------------------------------------------------------------------------------------
				//==========================================================================================================
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  0 , 2 , 0 );			// Picked Count
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  8 , 2 , 0 );			// Delete ClusterDepth
				_i_SCH_CLUSTER_Set_PathRun( side , i ,  9 , 2 , 0 );			// Dll Call Checking
				_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 2 , 0 );			// Total Loop Count
				_i_SCH_CLUSTER_Set_PathRun( side , i , 11 , 2 , pointer + 1 );	// Next Go Pointer
				_i_SCH_CLUSTER_Set_PathRun( side , i , 12 , 2 , tag2 );			// Before Pick Wait Control
				_i_SCH_CLUSTER_Set_PathRun( side , i , 13 , 2 , tag3 );			// After Pick Wait Control
				_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 2 , 0 );			// Lot Pre/Post Use
				//
				x = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 + mode , 3 );
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 16 + mode , 3 , x + 1 );
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 2 , 0 );			// Lot Pre/Post Style
				_i_SCH_CLUSTER_Set_PathRun( side , i , 16 , 2 , mode + 20 );	// Wafer FlowControl Detail Style
				_i_SCH_CLUSTER_Set_PathRun( side , i , 16 , 3 , x );
				_i_SCH_CLUSTER_Set_PathRun( side , i , 17 , 2 , pointer + 1 );	// Reference Normal Wafer Pointer
				_i_SCH_CLUSTER_Set_PathRun( side , i , 18 , 2 , 0 );			// Waiting Reference Pointer
				_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 2 , selpm + PM1 - 1 ); // Waiting Reference Module
				_i_SCH_CLUSTER_Set_PathRun( side , i , 19 , 3 , selorder <= 0 ? 0 : selorder - _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) ); // Waiting Reference Order
				//
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 2 , 0 );			// Detail Retuning Info Count
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 3 , 0 );			// 2010.02.01
				_i_SCH_CLUSTER_Set_PathRun( side , i , 20 , 4 , 0 );			// 2010.02.01
				//==========================================================================================================
				// 2010.02.10
				//==========================================================================================================
				if ( ( mode + 1 ) == _i_SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 3 ) ) {
					//
					x = ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 4 ) * 100 ) + _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 3 ) + 1;
					//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 3 , x % 100 );				// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 4 , x / 100 );				// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 3 , 0 );						//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 4 , 0 );						//
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 3 , x % 100 );						// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 4 , x / 100 );						// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 3 , 0 );							//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 4 , 0 );							//
				}
				else {
					//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 10 , 3 , mode + 1 );				// ���� Mode�� ������ Dummy�� Mode
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 3 , 0 );						// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 4 , 0 );						// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 3 , 0 );						//
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 15 , 4 , 0 );						//
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 3 , 0 );							// ���� Mode�� ������ Dummy�� Count1
					_i_SCH_CLUSTER_Set_PathRun( side , i , 14 , 4 , 0 );							// ���� Mode�� ������ Dummy�� Count2
					//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 3 , 0 );							//
					_i_SCH_CLUSTER_Set_PathRun( side , i , 15 , 4 , 0 );							//
				}
				//
				if ( mode == 0 ) { // lot Pre
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 3 , ( rcpcheck > 0 ) ? cmd : cmd - 1 ); // ���� Dummy�� ���� Mode
				}
				else {
					_i_SCH_CLUSTER_Set_PathRun( side , i , 10 , 3 , ( rcpcheck > 0 ) ? cmd : cmd - 2 ); // ���� Dummy�� ���� Mode
				}
				//
				//==========================================================================================================
			}
			//==========================================================================================================
			if ( rcpcheck > 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-NEXT.YES\t[Side=%d][pt=%d][AppPt=%d]\n" , side , pointer , i );
//------------------------------------------------------------------------------------------------------------------
				return 99;
			}
			//==========================================================================================================
			return 1;
			//==========================================================================================================
		}
	}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-RETURN\t[DCH=%d][DSL=%d] <= Nothing to append Space\n" , dummychamber , dummyslot );
//------------------------------------------------------------------------------------------------------------------
	if ( rcpcheck >= 0 ) free( RemainRecipe );
	//
	return -1;
}


int _SCH_inside_Unused_Remap_Dummy( int side , int pointer , int *orgrange ) {
	int i , m , o , cl , rmc;
	//
	*orgrange = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
	rmc = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 );
	//
	m = 0;
	cl = 0;
	//
	while( TRUE ) {
		o = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
		//
		if ( m >= o ) break;
		//
		if ( _i_SCH_CLUSTER_PathProcessRecipe_MultiData_Unused( side , pointer , m ) ) {
			//
			cl++;
			//
			for ( i = m ; i < ( o - 1 ) ; i++ ) {
				_i_SCH_CLUSTER_Move_PathProcessChamber( side , pointer , i , i + 1 );
				//
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , i , 0 , _i_SCH_CLUSTER_Get_PathRun( side , pointer , i + 1 , 0 ) );
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , i + 1 , 0 , 0 );
			}
			//
			_i_SCH_CLUSTER_Set_PathRange( side , pointer , o - 1 );
		}
		else {
			m++;
		}
	}
	if ( cl > 0 ) _i_SCH_CLUSTER_Set_PathRun( side , pointer , 8 , 2 , rmc + cl );
	return cl;
}


int _SCH_inside_Make_Dummy( int side , int pointer ) {
	int s , m , o , ch , cl , res , checkch , checkorder , findch , curnotuse = 0;
	int dummychamber , dummyslot , dummyparam , dummymode;
	//==========================================================================================================
	//=======================================================================================================
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 9 , 2 ) == 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-REQUEST\t[Side=%d][Pt=%d]\n" , side , pointer );
//------------------------------------------------------------------------------------------------------------------
		_SCH_COMMON_CHECK_PICK_FROM_FM( side , pointer , 101 );
		//
		_i_SCH_CLUSTER_Set_PathRun( side , pointer , 9 , 2 , 1 );
		//=======================================================================================================
		// 2009.01.23
		//=======================================================================================================
		cl = _SCH_inside_Unused_Remap_Dummy( side , pointer , &res );
		//
		if ( cl > 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-UNUSING1\t[Side=%d][Pt=%d][UnusingCount=%d][NewRange=%d->%d][Offset=%d][cl=%d]\n" , side , pointer , cl , res , _i_SCH_CLUSTER_Get_PathRange( side , pointer ) , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) , cl );
//------------------------------------------------------------------------------------------------------------------
			//
			o = USER_RECIPE_PROCESS_FINISH_FOR_DUMMY( TRUE , side , pointer , -1 , res , _i_SCH_CLUSTER_Get_PathRange( side , pointer ) , cl , -1 , -1 , "" , &_SCH_INF_CLUSTER_DATA_AREA[ side ][ pointer ] ); // 2009.06.02
			//
			if ( o != 0 ) { // 2009.06.02
				cl = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 );
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 8 , 2 , cl - o );
			}
			//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-UNUSING1A\t[Side=%d][Pt=%d][UnusingCount=%d][NewRange=%d->%d][Offset=%d][o=%d]\n" , side , pointer , cl , res , _i_SCH_CLUSTER_Get_PathRange( side , pointer ) , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) , o );
//------------------------------------------------------------------------------------------------------------------
		}
		//
		//=======================================================================================================
	}
	//=======================================================================================================
	m = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 18 , 2 ) - 1;
	//
	if ( m >= 0 ) {
		checkch    = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 2 );
		checkorder = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 19 , 3 ) - 1;
		//
		findch = 0;
		//
		cl = _i_SCH_CLUSTER_Get_PathRange( side , m );
		//
		for ( o = 0 ; o < cl ; o++ ) {
			//
			if ( ( checkorder >= 0 ) && ( checkorder != o ) ) continue;
			//
			ch = _i_SCH_CLUSTER_Get_PathRun( side , m , o , 0 );
			//
			if ( ch == 0 ) continue;
			//
			findch = ch;
			//
			break;
		}
		//
		if ( findch > 0 ) {
			if ( findch != checkch ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-REMOVING\t[Side=%d][Pt=%d][mode=%d][OrgRunPt=%d]-[checkch=PM%d][checkorder=%d][findch=PM%d]\n" , side , pointer , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 ) , m , checkch - PM1 + 1 , checkorder , findch - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
				curnotuse = 1;
			}
			else {
				//==========================================================================================================
				dummychamber = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
				dummyslot    = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
				dummyparam   = Get_Inf_Dummy_PostWait( side , pointer , &dummymode );
				//
				if ( dummyparam == -1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SEL.POST1\t[Side=%d][Pt=%d][mode=%d][OrgRunPt=%d]-[checkch=PM%d][checkorder=%d][Param=-1][dm=%d]\n" , side , pointer , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 ) , m , checkch - PM1 + 1 , checkorder , dummymode );
//------------------------------------------------------------------------------------------------------------------
					res = _SCH_inside_Get_DLL_Dummy( 2 , side , pointer , dummymode , findch , -1 , &dummychamber , &dummyslot , "" );
				}
				else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SEL.POST2\t[Side=%d][Pt=%d][mode=%d][OrgRunPt=%d]-[checkch=PM%d][checkorder=%d][Param=%d][dm=%d][%s]\n" , side , pointer , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 ) , m , checkch - PM1 + 1 , checkorder , Inf_Dummy_PostWait_Value[dummyparam] , dummymode , STR_MEM_GET_STR( Inf_Dummy_PostWait_Name[dummyparam] ) );
//------------------------------------------------------------------------------------------------------------------
					res = _SCH_inside_Get_DLL_Dummy( 2 , side , pointer , dummymode , findch , Inf_Dummy_PostWait_Value[dummyparam] , &dummychamber , &dummyslot , STR_MEM_GET_STR( Inf_Dummy_PostWait_Name[dummyparam] ) );
					//
//					Remove_Inf_Dummy_PostWait( dummyparam ); // 2009.09.23
					if ( res != -1 ) Remove_Inf_Dummy_PostWait( dummyparam ); // 2009.09.23
				}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "FLOWCTL-SEL.POST3\t[Side=%d][Pt=%d][mode=%d][OrgRunPt=%d]-[checkch=PM%d][checkorder=%d][Param=%d][dm=%d][%s][Res=%d]\n" , side , pointer , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 ) , m , checkch - PM1 + 1 , checkorder , Inf_Dummy_PostWait_Value[dummyparam] , dummymode , STR_MEM_GET_STR( Inf_Dummy_PostWait_Name[dummyparam] ) , res );
//------------------------------------------------------------------------------------------------------------------
				//
				if ( res == -1 ) return 1;
				//
				if ( res == 0 ) {
					curnotuse = 1;
				}
				else {
					_SCH_TAG_DUMMY_FLOW_CH = dummychamber;
					_SCH_TAG_DUMMY_FLOW_SLOT = dummyslot;
					//
					_i_SCH_CLUSTER_Set_PathIn( side , pointer , dummychamber );
					_i_SCH_CLUSTER_Set_PathOut( side , pointer , dummychamber );
					_i_SCH_CLUSTER_Set_SlotIn( side , pointer , dummyslot );
					_i_SCH_CLUSTER_Set_SlotOut( side , pointer , dummyslot );
				}
				//==========================================================================================================
			}
			//==========================================================================================================
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 18 , 2 , 0 );
			//==========================================================================================================
		}
		else {
			return 2;
		}
	}
	//==========================================================================================================
	m = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 14 , 2 );
	o = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 15 , 2 );
	s = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 16 , 2 ); // 2009.12.29
	//==========================================================================================================
	switch ( m ) { // Lot Pre
	case 1 :
	case 3 :
	case 5 :
	case 7 :
	case 9 :
	case 11 :
	case 13 :
	case 15 :
		//
		res = _SCH_inside_Get_Dummy( side , pointer , 0 , o % 10 , 2 , m , curnotuse );
		//
		if ( ( res != -1 ) && ( res != 99 ) ) {
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , m - 1 );
		}
		//
		if ( res == 0 ) return 0;
		return 3;
		break;
	}
	//==========================================================================================================
	switch ( m ) { // Wafer Pre
	case 2 :
	case 3 :
	case 6 :
	case 7 :
	case 10 :
	case 11 :
	case 14 :
	case 15 :
		//
		res = _SCH_inside_Get_Dummy( side , pointer , 1 , 0 , 2 , m , curnotuse );
		//
		if ( ( res != -1 ) && ( res != 99 ) ) {
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , m - 2 );
		}
		//
		if ( res == 0 ) return 0;
		return 4;
		break;
	}
	//==========================================================================================================
	switch ( m ) { // Wafer Post
	case 4 :
	case 5 :
	case 6 :
	case 7 :
	case 12 :
	case 13 :
	case 14 :
	case 15 :
		//
		res = _SCH_inside_Get_Dummy( side , pointer , 2 , 0 , 2 , m , curnotuse );
		//
		if ( ( res != -1 ) && ( res != 99 ) ) {
//			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , m - 4 ); // 2009.02.02
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , 0 ); // 2009.02.02
		}
		//
		if ( res == 0 ) return 0;
		return 5;
		break;
	}
	//==========================================================================================================
	switch ( m ) { // Lot Post
	case 8 :
	case 9 :
	case 10 :
	case 11 :
	case 12 :
	case 13 :
	case 14 :
	case 15 :
		//
		res = _SCH_inside_Get_Dummy( side , pointer , 3 , o / 10 , 2 , m , curnotuse );
		//
		if ( ( res != -1 ) && ( res != 99 ) ) {
//			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , m - 8 ); // 2009.02.02
			_i_SCH_CLUSTER_Set_PathRun( side , pointer , 14 , 2 , 0 ); // 2009.02.02
		}
		//
		if ( res == 0 ) return 0;
		return 6;
		break;
	}
	//==========================================================================================================
	if ( ( m == 0 ) && ( curnotuse == 1 ) ) {
		res = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 11 , 2 );
		//
		if ( res > 0 ) {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				_i_SCH_MODULE_Set_TM_DoPointer( side , res - 1 );
			}
			else {
				_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , res - 1 );
			}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE13 [Side=%d,Pt=%d]\n" , side , res - 1 );
//------------------------------------------------------------------------------------------------------------------
		}
		else {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				_i_SCH_MODULE_Set_TM_DoPointer( side , pointer + 1 );
			}
			else {
				_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , pointer + 1 );
			}
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-SETDO\tCASE14 [Side=%d,Pt=%d]\n" , side , pointer + 1 );
//------------------------------------------------------------------------------------------------------------------
		}
		_i_SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , side , pointer , 4 );
		//
		return 7;
	}
	//==========================================================================================================
	return 0;
}

//=======================================================================================
void _SCH_inside_Ordering_Invalid_OK( int side ) { // 2011.06.13
	if ( _in_HOT_LOT_ORDERING_INVALID[side] != 0 ) {
		//
		Scheduler_Randomize_After_Waiting_Part( side , 1 );
		//
		_in_HOT_LOT_ORDERING_INVALID[side] = 0;
		//
		_SCH_IO_SET_MAIN_BUTTON_MC( side , CTC_RUNNING );
		//
		BUTTON_SET_FLOW_MTLOUT_STATUS( side , -1 , FALSE , _MS_12_RUNNING ); // 2013.06.08
		//
	}
	//
}

BOOL _SCH_inside_Ordering_Invalid_Check_PM_Valid( int pmc ) { // 2012.01.17
	//
	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
	case 9 :
	case 11 :
		if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( pmc ) == MRES_SUCCESS_LIKE_ALLSKIP ) return FALSE;
		break;
	}
	//
	return TRUE;
}


//BOOL _SCH_inside_Ordering_Invalid_Check( int side ) { // 2011.06.13 // 2016.01.08
BOOL _SCH_inside_Ordering_Invalid_Check( int side , BOOL wait_for_start ) { // 2011.06.13 // 2016.01.08
	int i , j , c;
	//
	if ( !wait_for_start ) { // 2016.01.08
		//
		if ( _i_SCH_SYSTEM_MODE_GET( side ) != 0 ) {
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( side == i ) continue;
				//
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				//
				if ( _i_SCH_SYSTEM_MODE_END_GET( i ) != 0 ) continue; // 2018.06.15

				if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) > _i_SCH_SYSTEM_RUNORDER_GET( side ) ) continue;
				//
				c = 0;
				//
				for ( j = PM1 ; j < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; j++ ) {
					//
					if ( !_SCH_inside_Ordering_Invalid_Check_PM_Valid( j ) ) continue; // 2012.01.17
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , j ) == MUF_01_USE ) {
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) == MUF_01_USE ) {
							//
							c = 1;
							break;
							//
						}
					}
					//
				}
				//
				if ( c == 1 ) {
					//
					for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
						//
						if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
						//
						if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_READY ) {
							//
							return TRUE;
						}
						//
					}
				}
				//
			}
		}
	}
	//
	if ( SCHEDULER_CONTROL_Check_MFIC_Supply_Impossible( side ) ) return TRUE; // 2011.07.20
	//
	return FALSE;
	//
}


BOOL _SCH_inside_Randomize_Invalid_Check( int side , int pt ) { // 2011.02.14
	int p , j , m , pr;
	//
	pr = _i_SCH_CLUSTER_Get_PathRange( side , pt );
	//
	for ( p = 0 ; p < pr ; p++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , p , j );
			//
			if ( m > 0 ) break;
			//
			if ( m < 0 ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , -(m) ) != MUF_97_USE_to_SEQDIS_RAND ) {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , -(m) ) != MUF_98_USE_to_DISABLE_RAND ) {
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , -(m) ) != MUF_99_USE_to_DISABLE ) { // 2013.01.07
							break;
						}
					}
				}
			}
			//
		}
		//
		if ( j == MAX_PM_CHAMBER_DEPTH ) return TRUE;
		//
	}
	//
	return FALSE;
}

//=======================================================================================
BOOL SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS_IN( int side , int pointer ); // 2013.05.27

int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( int side , int *cm , int *pt , int subchk0 , BOOL usermodecheck ) { // 2006.11.15
	int i , slt , res;
	int lpchk; // 2018.05.25
	//
	//, ret_imp;
	//
	int subchk , subchkdll; // 2018.12.20
	//
	// 2018.12.20
	//
	if ( subchk0 <= 0 ) {
		subchk = subchk0;
		subchkdll = subchk0;
	}
	else {
		i = subchk0 / 1000;
		//
		if ( i == 1 ) {
			subchk = subchk0 % 1000;
			subchkdll = -1;
		}
		else {
			subchk = subchk0 % 1000;
			subchkdll = subchk;
		}
	}
	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		*pt = -1;
		return -1;
	}
	//======================================================================================================================================
	if ( !_i_SCH_SUB_Simple_Supply_Possible_Sub( side , usermodecheck ) ) {
		*pt = -2;
		return -2;
	}
	//
	//======================================================================================================================================
	if ( !usermodecheck ) {
//		if ( _SCH_inside_Ordering_Invalid_Check( side ) ) { // 2011.06.13 // 2016.01.08
		if ( _SCH_inside_Ordering_Invalid_Check( side , FALSE ) ) { // 2011.06.13 // 2016.01.08
			//
			_i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1  + 91000 ); // 2011.06.13
			//
			_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 11; // 2014.10.14
			//
			*pt = -91;
			return -91;
		}
	}
	//======================================================================================================================================
	//
	lpchk = 0; // 2018.05.25
	//
	while ( TRUE ) { // 2018.05.25
		//
		while ( TRUE ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2009.04.29
				if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , 0 , usermodecheck , FALSE ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 12; // 2014.10.14
					//
					*pt = -3;
					return -3;
				}
				if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 13; // 2014.10.14
					//
					*pt = -4;
					return -4;
				}
				if ( !_i_SCH_SUB_Remain_for_CM( side , FALSE ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 14; // 2014.10.14
					//
					*pt = -5;
					return -5;
				}
				//======================================================================================================================================
				/*
				// 2014.06.16
				*cm = _i_SCH_CLUSTER_Get_PathIn( side , _i_SCH_MODULE_Get_TM_DoPointer( side ) );
				slt = _i_SCH_CLUSTER_Get_SlotIn( side , _i_SCH_MODULE_Get_TM_DoPointer( side ) );
				*pt = _i_SCH_MODULE_Get_TM_DoPointer( side );
				*/
				//
				// 2014.06.16
				//
				*pt = _i_SCH_MODULE_Get_TM_DoPointer( side );
				//
				res = *pt;
				//
				if  ( _SCH_COMMON_GET_PT_AT_PICK_FROM_CM( side , &res , FALSE , 1 ) ) { // 2014.06.16
					//
					if ( ( res < 0 ) || ( res >= MAX_CASSETTE_SLOT_SIZE ) ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 15; // 2014.10.14
						//
						*pt = -81;
						return -81;
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( side , res ) < 0 ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 16; // 2014.10.14
						//
						*pt = -82;
						return -82;
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( side , res ) != SCHEDULER_READY ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 17; // 2014.10.14
						//
						*pt = -83;
						return -83;
					}
					//
					*pt = res;
					//
				}
				//
				*cm = _i_SCH_CLUSTER_Get_PathIn( side , *pt );
				slt = _i_SCH_CLUSTER_Get_SlotIn( side , *pt );
				//
				//======================================================================================================================================
			}
			else {
				if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , ( subchk == 1 ) ? -1 : 1 , usermodecheck , FALSE ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 18; // 2014.10.14
					//
					*pt = -6;
					return -6; // 2008.07.22 // 2008.08.25
				}
				if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 19; // 2014.10.14
					//
					*pt = -7;
					return -7;
				}
				if ( !_i_SCH_SUB_Remain_for_FM_Sub( side , FALSE ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 20; // 2014.10.14
					//
					*pt = -8;
					return -8;
				}
				//======================================================================================================================================
				// 2011.10.25
	//			*cm = _i_SCH_CLUSTER_Get_PathIn( side , _i_SCH_MODULE_Get_FM_DoPointer( side ) );
	//			slt = _i_SCH_CLUSTER_Get_SlotIn( side , _i_SCH_MODULE_Get_FM_DoPointer( side ) );
	//			*pt = _i_SCH_MODULE_Get_FM_DoPointer( side );
				//
				/*
				// 2014.06.16
				*pt = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
				*cm = _i_SCH_CLUSTER_Get_PathIn( side , *pt );
				slt = _i_SCH_CLUSTER_Get_SlotIn( side , *pt );
				*/
				//
				// 2014.06.16
				//
				*pt = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
				//
				res = *pt;
				//
				if  ( _SCH_COMMON_GET_PT_AT_PICK_FROM_CM( side , &res , TRUE , 1 ) ) { // 2014.06.16
					//
					if ( ( res < 0 ) || ( res >= MAX_CASSETTE_SLOT_SIZE ) ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 21; // 2014.10.14
						//
						*pt = -81;
						return -81;
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( side , res ) < 0 ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 22; // 2014.10.14
						//
						*pt = -82;
						return -82;
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( side , res ) != SCHEDULER_READY ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 23; // 2014.10.14
						//
						*pt = -83;
						return -83;
					}
					//
					*pt = res;
					//
				}
				//
				*cm = _i_SCH_CLUSTER_Get_PathIn( side , *pt );
				slt = _i_SCH_CLUSTER_Get_SlotIn( side , *pt );
				//======================================================================================================================================
			}
			//
			if ( !_i_SCH_CASSETTE_Chk_SKIP_WITH_POINTER( side , *pt ) ) break; // 2009.04.29
			//
		}
		//======================================================================================================================================
		res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , *pt );
		//
		if ( usermodecheck ) {
			//======================================================================================================================================
			if ( ( res % 10 ) == 0 ) {
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 24; // 2014.10.14
				//
				*pt = -92;
				return -92;
			}
			//======================================================================================================================================
	//		if ( _SCH_inside_Ordering_Invalid_Check( side ) ) { // 2011.06.13 // 2016.01.18
			if ( _SCH_inside_Ordering_Invalid_Check( side , FALSE ) ) { // 2011.06.13 // 2016.01.18
				_i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1  + 92000 ); // 2011.06.13
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 25; // 2014.10.14
				//
				*pt = -91;
				return -91;
			}
			//======================================================================================================================================
		}
		else {
			//======================================================================================================================================
			if ( ( res != 0 ) && ( ( res % 10 ) != 0 ) ) { // 2011.12.29
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 26; // 2014.10.14
				//
				*pt = -92;
				return -92;
			}
			//======================================================================================================================================
		}
		//
		if ( _SCH_inside_Randomize_Invalid_Check( side , *pt ) ) { // 2011.02.14
			_i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1  + 93000 ); // 2011.06.13
			//
			_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 27; // 2014.10.14
			//
			*pt = -21;
			return -21;
		}
		//======================================================================================================================================
		if ( !usermodecheck ) { // 2011.12.08
			//
			// 2015.10.12
			//
			if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , *pt ) != 0 ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) < PM1 ) {
					//
					switch ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , *pt ) ) ) {
					case CWM_PRESENT :
					case CWM_PROCESS :
					case CWM_FAILURE :
						break;
					default :
						//
						*pt = -31;
						return -31;
						break;
					}
					//
				}
				else if ( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) >= BM1 ) {
					//
					if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , *pt ) ) <= 0 ) {
						//
						*pt = -32;
						return -32;
					}
					//
				}
				//
			}
			//
			//======================================================================================================================================
			i = _SCH_COMMON_CHECK_PICK_FROM_FM( side , *pt , -101 );
			if ( i <= 0 ) {
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 28; // 2014.10.14
				//
				*pt = -22 + ( i * 100 );
				return -22 + ( i * 100 );
			}
			//======================================================================================================================================
			i = _SCH_inside_Make_Dummy( side , *pt );
			if ( i > 0 ) {
		//		*pt = -9 * ( i * -100 ); // 2010.03.10
		//		return -9 * ( i * -100 ); // 2010.03.10
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 29; // 2014.10.14
				//
				*pt = -9 + ( i * -100 ); // 2010.03.10
				return -9 + ( i * -100 ); // 2010.03.10
			}
			//======================================================================================================================================
		}
		//======================================================================================================================================
		if ( ( _i_SCH_CLUSTER_Get_PathRun( side , *pt , 12 , 2 ) % 2 ) == 1 ) { // 2008.10.26
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2009.04.29
				if ( !_i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 30; // 2014.10.14
					//
					*pt = -10;
					return -10;
				}
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( i == side ) continue;
					if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
					if ( !_i_SCH_MODULE_Chk_TM_Free_Status( i ) ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 31; // 2014.10.14
						//
						*pt = -11;
						return -11;
					}
				}
			}
			else {
				if ( !_i_SCH_MODULE_Chk_FM_Free_Status( side ) ) {
					//
					_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 32; // 2014.10.14
					//
					*pt = -12;
					return -12;
				}
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( i == side ) continue;
					if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
					if ( !_i_SCH_MODULE_Chk_FM_Free_Status( i ) ) {
						//
						_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 33; // 2014.10.14
						//
						*pt = -13;
						return -13;
					}
				}
			}
		}
		//======================================================================================================================================
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.09.01
			if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , *cm , G_PICK ) ) {
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 34; // 2014.10.14
				//
				*pt = -14;
				return -14; // 2007.02.01
			}
		}
		else {
			if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( *cm , G_PICK , 0 ) ) {
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 35; // 2014.10.14
				//
				*pt = -14;
				return -14; // 2007.02.01
			}
		}
		//======================================================================================================================================
		if ( !usermodecheck ) { // 2011.12.08
			//======================================================================================================================================
//			i = _SCH_COMMON_CHECK_PICK_FROM_FM( side , *pt , subchk ); // 2018.12.20
			i = _SCH_COMMON_CHECK_PICK_FROM_FM( side , *pt , subchkdll ); // 2018.12.20
			//
			if ( ( i == -9999 ) && ( lpchk == 0 ) ) { // 2018.05.25
				//
				lpchk = 1;
				continue;
				//
			}
			else if ( i <= 0 ) {
				//
				_SCH_IO_SET_MODULE_FLAG_DATA3( side + 1 , 4 , 5 , 6 ); // 2016.12.13
				//
				_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 36; // 2014.10.14
				//
				*pt = -15 + ( i * 100 );
				return -15 + ( i * 100 );
			}
			else { // 2016.12.13
				//
				_SCH_IO_SET_MODULE_FLAG_DATA3( side + 1 , 1 , 2 , 3 ); // 2016.12.13
				//
			}
			//======================================================================================================================================
		}
		//
		//
		break; // 2018.05.25
		//
	} // 2018.05.25
	//
	//======================================================================================================================================
	if ( !usermodecheck ) { // 2013.10.08
		if ( !SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS_IN( side , *pt ) ) { // 2013.05.27
			//
			_in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] = ( ( _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[side] % 1000000 ) * 100 ) + 37; // 2014.10.14
			//
			*pt = -16;
			return -16;
		}
	}

	//======================================================================================================================================
	_SCH_inside_Ordering_Invalid_OK( side ); // 2011.06.13
	//======================================================================================================================================
	return 1;
	//======================================================================================================================================
}
//=======================================================================================
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB_NOSUPPLYCHECK( int side , int *cm , int *pt , BOOL usermodecheck ) { // 2016.12.14
	int i , slt , res;
	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		*pt = -1;
		return -1;
	}
	//======================================================================================================================================
	if ( !_i_SCH_SUB_Simple_Supply_Possible_Sub( side , usermodecheck ) ) {
		*pt = -2;
		return -2;
	}
	//
	//======================================================================================================================================
	if ( !usermodecheck ) {
		//
		if ( _SCH_inside_Ordering_Invalid_Check( side , FALSE ) ) {
			//
			*pt = -91;
			return -91;
		}
	}
	//======================================================================================================================================
	while ( TRUE ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			//
			//======================================================================================================================================
			//
			if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) {
				*pt = -4;
				return -4;
			}
			if ( !_i_SCH_SUB_Remain_for_CM( side , FALSE ) ) {
				*pt = -5;
				return -5;
			}
			//
			//======================================================================================================================================
			//
			*pt = _i_SCH_MODULE_Get_TM_DoPointer( side );
			//
			res = *pt;
			//
			if  ( _SCH_COMMON_GET_PT_AT_PICK_FROM_CM( side , &res , FALSE , 1 ) ) {
				//
				if ( ( res < 0 ) || ( res >= MAX_CASSETTE_SLOT_SIZE ) ) {
					*pt = -81;
					return -81;
				}
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , res ) < 0 ) {
					*pt = -82;
					return -82;
				}
				//
				if ( _i_SCH_CLUSTER_Get_PathStatus( side , res ) != SCHEDULER_READY ) {
					*pt = -83;
					return -83;
				}
				//
				*pt = res;
				//
			}
			//
			*cm = _i_SCH_CLUSTER_Get_PathIn( side , *pt );
			slt = _i_SCH_CLUSTER_Get_SlotIn( side , *pt );
			//
			//======================================================================================================================================
			//
		}
		else {
			//
			//======================================================================================================================================
			//
			if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) {
				*pt = -7;
				return -7;
			}
			if ( !_i_SCH_SUB_Remain_for_FM_Sub( side , FALSE ) ) {
				*pt = -8;
				return -8;
			}
			//
			//======================================================================================================================================
			//
			*pt = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
			//
			res = *pt;
			//
			if  ( _SCH_COMMON_GET_PT_AT_PICK_FROM_CM( side , &res , TRUE , 1 ) ) {
				//
				if ( ( res < 0 ) || ( res >= MAX_CASSETTE_SLOT_SIZE ) ) {
					*pt = -81;
					return -81;
				}
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , res ) < 0 ) {
					*pt = -82;
					return -82;
				}
				//
				if ( _i_SCH_CLUSTER_Get_PathStatus( side , res ) != SCHEDULER_READY ) {
					*pt = -83;
					return -83;
				}
				//
				*pt = res;
				//
			}
			//
			*cm = _i_SCH_CLUSTER_Get_PathIn( side , *pt );
			slt = _i_SCH_CLUSTER_Get_SlotIn( side , *pt );
			//
			//======================================================================================================================================
			//
		}
		//
		if ( !_i_SCH_CASSETTE_Chk_SKIP_WITH_POINTER( side , *pt ) ) break;
		//
	}
	//======================================================================================================================================
	res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , *pt );
	//
	if ( usermodecheck ) {
		//======================================================================================================================================
		if ( ( res % 10 ) == 0 ) {
			*pt = -92;
			return -92;
		}
		//======================================================================================================================================
		if ( _SCH_inside_Ordering_Invalid_Check( side , FALSE ) ) {
			*pt = -91;
			return -91;
		}
		//======================================================================================================================================
	}
	else {
		//======================================================================================================================================
		if ( ( res != 0 ) && ( ( res % 10 ) != 0 ) ) {
			*pt = -92;
			return -92;
		}
		//======================================================================================================================================
	}
	//
	if ( _SCH_inside_Randomize_Invalid_Check( side , *pt ) ) {
		*pt = -21;
		return -21;
	}
	//======================================================================================================================================
	if ( !usermodecheck ) {
		if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , *pt ) != 0 ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) < PM1 ) {
				//
				switch ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , *pt ) ) ) {
				case CWM_PRESENT :
				case CWM_PROCESS :
				case CWM_FAILURE :
					break;
				default :
					//
					*pt = -31;
					return -31;
					break;
				}
				//
			}
			else if ( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) >= BM1 ) {
				//
				if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , *pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , *pt ) ) <= 0 ) {
					//
					*pt = -32;
					return -32;
				}
				//
			}
			//
		}
		//
	}
	//======================================================================================================================================
	if ( ( _i_SCH_CLUSTER_Get_PathRun( side , *pt , 12 , 2 ) % 2 ) == 1 ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( !_i_SCH_MODULE_Chk_TM_Free_Status( side ) ) {
				*pt = -10;
				return -10;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				if ( !_i_SCH_MODULE_Chk_TM_Free_Status( i ) ) {
					*pt = -11;
					return -11;
				}
			}
		}
		else {
			if ( !_i_SCH_MODULE_Chk_FM_Free_Status( side ) ) {
				*pt = -12;
				return -12;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				if ( !_i_SCH_MODULE_Chk_FM_Free_Status( i ) ) {
					*pt = -13;
					return -13;
				}
			}
		}
	}
	//
	//======================================================================================================================================
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , *cm , G_PICK ) ) {
			*pt = -14;
			return -14;
		}
	}
	else {
		if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( *cm , G_PICK , 0 ) ) {
			*pt = -14;
			return -14;
		}
	}
	//
	//======================================================================================================================================
	//
	if ( !usermodecheck ) {
		if ( !SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS_IN( side , *pt ) ) {
			*pt = -16;
			return -16;
		}
	}
	//
	//======================================================================================================================================
	//
	return 1;
	//======================================================================================================================================
}
//=======================================================================================
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_FOR_LOG( int side ) { // 2014.10.14
	int i , res , pt , cm , supx;
	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	//======================================================================================================================================
	if ( !_i_SCH_SUB_Simple_Supply_Possible_Sub( side , FALSE ) ) return 2;
	//======================================================================================================================================
//	if ( _SCH_inside_Ordering_Invalid_Check( side ) ) return 11; // 2016.01.08
	if ( _SCH_inside_Ordering_Invalid_Check( side , FALSE ) ) return 11; // 2016.01.08
	//======================================================================================================================================
	supx = 0;
	//
//	while ( TRUE ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , 0 , FALSE , TRUE ) ) return 12;
			if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) return 13;
//			if ( !_i_SCH_SUB_Remain_for_CM( side ) ) return 14; // 2018.07.12
			if ( !_i_SCH_SUB_Remain_for_CM( side , TRUE ) ) return 14; // 2018.07.12
			//
			pt = _i_SCH_MODULE_Get_TM_DoPointer( side );
			//
		}
		else {
			if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , -1 , FALSE , TRUE ) ) {
				if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , 1 , FALSE , TRUE ) ) {
					return 18;
				}
				else {
					supx = 50;
				}
			}
			else {
				if ( !_i_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply_Sub( side , TRUE , 1 , FALSE , TRUE ) ) {
					supx = 50;
				}
			}
			//
			if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) return 19+supx;
//			if ( !_i_SCH_SUB_Remain_for_FM_Sub( side ) ) return 20+supx; // 2018.07.12
			if ( !_i_SCH_SUB_Remain_for_FM_Sub( side , TRUE ) ) return 20+supx; // 2018.07.12
			pt = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
		}
		//
		cm = _i_SCH_CLUSTER_Get_PathIn( side , pt );
		//
//	}
	//======================================================================================================================================
	res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , pt );
	if ( ( res != 0 ) && ( ( res % 10 ) != 0 ) ) return 26+supx;
	//======================================================================================================================================
	if ( _SCH_inside_Randomize_Invalid_Check( side , pt ) ) return 27+supx;
	//
	if ( ( _i_SCH_CLUSTER_Get_PathRun( side , pt , 12 , 2 ) % 2 ) == 1 ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( !_i_SCH_MODULE_Chk_TM_Free_Status( side ) ) return 30+supx;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				if ( !_i_SCH_MODULE_Chk_TM_Free_Status( i ) ) return 31+supx;
			}
		}
		else {
			if ( !_i_SCH_MODULE_Chk_FM_Free_Status( side ) ) return 32+supx;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i == side ) continue;
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				if ( !_i_SCH_MODULE_Chk_FM_Free_Status( i ) ) return 33+supx;
			}
		}
	}
	//======================================================================================================================================
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , cm , G_PICK ) ) return 34+supx;
	}
	else {
		if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( cm , G_PICK , 0 ) ) return 35+supx;
	}
	//======================================================================================================================================
	return 0;
	//======================================================================================================================================
}
//=======================================================================================
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( int side , int *cm , int *pt , int subchk ) { // 2006.11.15
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( side , cm , pt , subchk , FALSE );
	//
//	printf( "[side=%d,pt=%d,Res=%d]" , side , pt , Res );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	if ( Res > 0 ) return TRUE;
	return FALSE;
}

int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_CM( int side , int *cm , int *pt ) {
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( side , cm , pt , 0 , FALSE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
}

int _i_SCH_USER_POSSIBLE_PICK_FROM_FM( int side , int *pt ) { // 2011.12.08
	int Res , cm;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( side , &cm , pt , -1 , TRUE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	if ( Res > 0 ) return TRUE;
	return FALSE;
}

int _i_SCH_USER_POSSIBLE_PICK_FROM_TM( int side , int *pt ) { // 2012.04.18
	int Res , cm;
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( side , &cm , pt , -1 , TRUE );
	//
	if ( Res > 0 ) return TRUE;
	return FALSE;
}


int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( int side , int *cm , int *pt ) { // 2016.12.14
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB_NOSUPPLYCHECK( side , cm , pt , FALSE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	if ( Res > 0 ) return TRUE;
	return FALSE;
}

int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( int side , int *cm , int *pt ) { // 2017.03.16
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB_NOSUPPLYCHECK( side , cm , pt , TRUE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	//
	if ( Res > 0 ) return TRUE;
	return FALSE;
}

//
void _SCH_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE( int side , int count ) {
	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return;
	//======================================================================================================================================
	_i_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , count );
}

//
BOOL _SCH_MACRO_inside_Remain_for_MM( int side , int *curpt ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( *curpt >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	while ( _i_SCH_SUB_Run_Impossible_Condition( side , *curpt , -1 ) ) {
		(*curpt)++;
		if ( *curpt >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	}
	if ( *curpt >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	return TRUE;
}

void Scheduler_HSide_Remap( int side , int pt , int hside ); // 2018.11.22

int _SCH_MACRO_inside_CHECK_PICK_from_FM( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_HSide , int *FM_HSide2 , BOOL TM_Double , int Single_Mode0 , BOOL *cpreject , int *returnpointer ) {
	int fh , fh2 , op , op2;
	int single , xr;
	BOOL fmmode;
	int hwarm_a , hwarm_b , extarm , armstart;
	int Single_Mode , notinc , notarm , curpointer;
	//
	*FM_Slot = 0; // 2008.11.01
	*FM_Slot2 = 0; // 2008.11.01
	*cpreject = FALSE; // 2004.05.11
	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	//======================================================================================================================================
	if ( !_i_SCH_SUB_Check_Complete_Return( side ) ) return -2;
	//
	notarm = ( ( ( Single_Mode0 % 1000 ) / 100 ) == 1 );
	notinc = ( ( ( Single_Mode0 % 100 ) / 10 ) == 1 );
	Single_Mode = Single_Mode0 % 10;
	//
	armstart = Single_Mode0 / 1000;
	//
	fmmode = _i_SCH_PRM_GET_MODULE_MODE( FM ); // 2008.04.11
	//
	if ( fmmode ) { // 2008.04.11
		//
		armstart = 0;
		//
		if ( !_i_SCH_SUB_Remain_for_FM_Sub( side , FALSE ) ) return -3;
		//
		*returnpointer = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ); // 2004.05.11
		//
		if ( notarm ) {
			hwarm_a = TRUE;
			hwarm_b = TRUE;
		}
		else {
			hwarm_a = _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( armstart );
			hwarm_b = _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( armstart+1 );
		}
		//
		extarm  = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
	}
	else {
		//
		if ( armstart >= MAX_FINGER_TM ) return -3;
		//
		if ( !_i_SCH_SUB_Remain_for_CM( side , FALSE ) ) return -4;
		*returnpointer = _i_SCH_MODULE_Get_TM_DoPointer( side ); // 2004.05.11
		//
		if ( notarm ) {
			hwarm_a = TRUE;
			hwarm_b = TRUE;
		}
		else {
			hwarm_a = _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , armstart );
			//
			if ( ( armstart + 1 ) >= MAX_FINGER_TM ) {
				hwarm_b = FALSE;
			}
			else {
				hwarm_b = _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , armstart+1 );
			}
		}
		//
		extarm  = 0;
	}
	//
	curpointer = *returnpointer; // 2009.01.15
	//
	if ( ( _i_SCH_CLUSTER_Get_PathStatus( side , curpointer ) != SCHEDULER_READY ) && ( _i_SCH_CLUSTER_Get_PathStatus( side , curpointer ) != SCHEDULER_CM_END ) ) { // 2009.02.13
		if ( ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 13 , 2 ) >= 1 ) && ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 13 , 2 ) <= 8 ) ) {
			return -31;
		}
	}
	//
	if ( hwarm_a && hwarm_b ) { // 2008.11.01
		if ( curpointer < MAX_CASSETTE_SLOT_SIZE ) { // 2009.01.15
			if ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 18 , 2 ) != 0 ) hwarm_b = FALSE; // 2009.02.02
			if ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 11 , 2 ) != 0 ) hwarm_b = FALSE;
			if ( ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 12 , 2 ) % 2 ) == 1 ) hwarm_b = FALSE;
			if ( ( ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 12 , 2 ) % 4 ) / 2 ) == 1 ) hwarm_b = FALSE;
			if ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 14 , 2 ) != 0 ) hwarm_b = FALSE;
			//
			// 2016.11.04
			//
			switch ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , curpointer ) ) {
			case MR_PRE_1_REQUEST :
			case MR_PRE_2_REQUEST_and_WAIT :
				//
				hwarm_b = FALSE;
				//
				break;
			}
			//
		}
	}
	//
	if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , curpointer , 0 ) == 0 ) {
		*cpreject = TRUE; // 2004.05.11
		return -21;
	}
	//
	if ( !TM_Double ) {
		xr = 0; // 2006.02.07
		_i_SCH_MODULE_Set_FM_HWait( side , TRUE );
		*FM_HSide  = 0;
		*FM_HSide2 = 0;
		if ( hwarm_a && ( Single_Mode != 2 ) ) { // 2007.08.28
			if ( extarm > 0 ) { // 2006.02.07
				//=========================================================================
				if ( Single_Mode != 0 ) { // 2007.08.28
					xr = 1;
				}
				else {
					*FM_Pointer  = curpointer;
					*FM_Slot     = _i_SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
					//
					fh			 = _i_SCH_CLUSTER_Get_PathIn( side , *FM_Pointer );
					//
					op = 0;
					op2 = *FM_Pointer + 1;
					//
					while( TRUE ) {
						while ( _i_SCH_SUB_Run_Impossible_Condition( side , op2 , -1 ) ) {
							op2++;
							if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) break;
						}
						if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) {
							xr = 1;
							break;
						}
						if ( hwarm_b ) {
							if ( _i_SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
								xr = 2;
								break;
							}
							if ( ( *FM_Slot + op + 1 ) != ( _i_SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
								xr = 2;
								break;
							}
							op++;
							if ( op >= extarm ) break;
						}
						else { // 2006.04.25
							if ( ( ( _i_SCH_CLUSTER_Get_SlotIn( side , op2 ) ) > *FM_Slot ) &&
								( ( _i_SCH_CLUSTER_Get_SlotIn( side , op2 ) ) <= ( *FM_Slot + extarm ) ) ) {
								if ( _i_SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
									xr = 2;
									break;
								}
							}
						}
						op2++;
					}
				}
				//
				if ( xr != 0 ) {
					if ( hwarm_b ) {
						*FM_Pointer2  = curpointer;
						*FM_Slot2     = _i_SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
						curpointer++;
						*FM_Slot  = 0;
					}
					else {
						if ( xr == 2 ) {
							return -11;
						}
						else {
							curpointer++;
							*FM_Slot2 = 0;
						}
					}
				}
				else {
					curpointer++;
					*FM_Slot2 = 0;
				}
				//
				xr = 1;
				//=========================================================================
			}
			else {
				*FM_Pointer  = curpointer;
				*FM_Slot     = _i_SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
				curpointer++;
				*FM_Slot2 = 0;
				//
				if ( Single_Mode != 0 ) xr = 1; // 2007.08.28
				//
			}
		}
		else if ( hwarm_b && ( Single_Mode != 1 ) ) { // 2007.08.28
			*FM_Pointer2  = curpointer;
			*FM_Slot2     = _i_SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
			curpointer++;
			*FM_Slot  = 0;
			//
			if ( Single_Mode != 0 ) xr = 1; // 2007.08.28
			//
		}
		else {
			return -1;
		}
		//===============================================================================
		if ( !_SCH_MACRO_inside_Remain_for_MM( side , &curpointer ) ) {
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			return ( fmmode ) ? 2 : 3;
		}
		//===============================================================================
		if ( ( xr != 0 ) || !hwarm_a || !hwarm_b ) {
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			return 4; // 2006.02.07
		}
		//===============================================================================
		if ( Single_Mode != 0 ) {
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			return 5; // 2007.08.28
		}
		//===============================================================================
		if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , curpointer , 0 ) == 0 ) {
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			return 6;
		}
		//===============================================================================
		if ( *FM_Pointer == curpointer ) return 16; // 2009.02.13
		//===============================================================================
		if ( ( _i_SCH_CLUSTER_Get_PathStatus( side , curpointer ) != SCHEDULER_READY ) && ( _i_SCH_CLUSTER_Get_PathStatus( side , curpointer ) != SCHEDULER_CM_END ) ) { // 2009.02.13
			if ( ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 13 , 2 ) >= 1 ) && ( _i_SCH_CLUSTER_Get_PathRun( side , curpointer , 13 , 2 ) <= 8 ) ) {
				return 17;
			}
		}
		//===============================================================================
		if ( _i_SCH_CLUSTER_Get_PathIn( side , *FM_Pointer ) == _i_SCH_CLUSTER_Get_PathIn( side , curpointer ) ) {
			//
			*FM_Pointer2  = curpointer;
			//
			if ( hwarm_a && hwarm_b ) { // 2008.11.01
				//==========================================================================================================
				if ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 9 , 2 ) == 0 ) { // 2008.11.22
					_SCH_COMMON_CHECK_PICK_FROM_FM( side , *FM_Pointer2 , 101 );
					_i_SCH_CLUSTER_Set_PathRun( side , *FM_Pointer2 , 9 , 2 , 1 );
					//=======================================================================================================
					// 2009.02.01
					//=======================================================================================================
					op = _SCH_inside_Unused_Remap_Dummy( side , *FM_Pointer2 , &fh );
					//
					if ( op > 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-UNUSING2\t[S=%d][P=%d][UnusingCount=%d][NewRange=%d->%d][Offset=%d][op=%d]\n" , side , *FM_Pointer2 , op , fh , _i_SCH_CLUSTER_Get_PathRange( side , *FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 8 , 2 ) , op );
//------------------------------------------------------------------------------------------------------------------
						//
						op2 = USER_RECIPE_PROCESS_FINISH_FOR_DUMMY( TRUE , side , *FM_Pointer2 , -1 , fh , _i_SCH_CLUSTER_Get_PathRange( side , *FM_Pointer2 ) , op , -1 , -1 , "" , &_SCH_INF_CLUSTER_DATA_AREA[ side ][ *FM_Pointer2 ] ); // 2009.06.02
						//
						if ( op2 != 0 ) { // 2009.06.02
							op = _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 8 , 2 );
							_i_SCH_CLUSTER_Set_PathRun( side , *FM_Pointer2 , 8 , 2 , op - op2 );
						}
						//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-UNUSING2A\t[S=%d][P=%d][UnusingCount=%d][NewRange=%d->%d][Offset=%d][op2=%d]\n" , side , *FM_Pointer2 , op , fh , _i_SCH_CLUSTER_Get_PathRange( side , *FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 8 , 2 ) , op2 );
//------------------------------------------------------------------------------------------------------------------
					}
					//
					//=======================================================================================================
				}
				//==========================================================================================================
				if ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 18 , 2 ) != 0 ) hwarm_b = FALSE; // 2009.02.02
				//
//				if ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 11 , 2 ) != 0 ) hwarm_b = FALSE; // 2018.01.05
				//
				if ( ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 12 , 2 ) % 2 ) == 1 ) hwarm_b = FALSE;
				if ( ( ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 12 , 2 ) % 4 ) / 2 ) == 1 ) hwarm_b = FALSE;
				if ( _i_SCH_CLUSTER_Get_PathRun( side , *FM_Pointer2 , 14 , 2 ) != 0 ) hwarm_b = FALSE;
				//==========================================================================================================
			}
			//
			if ( hwarm_b ) { // 2008.11.01
				//
				*FM_Slot2     = _i_SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
				//
				// 2018.11.22
				//
				//
				//=============================================================================================
				//
				if ( ( *FM_Slot2 > 0 ) && ( *FM_Slot2 > 0 ) ) {
					Scheduler_HSide_Remap( side , *FM_Pointer2 , _i_SCH_CLUSTER_Get_PathHSide( side , *FM_Pointer ) % 1000000 ); // 2018.11.22
				}
				//
				//=============================================================================================
				//
				//
				//
				curpointer++;
				//======================================================================================
				_SCH_MACRO_inside_Remain_for_MM( side , &curpointer );
				//======================================================================================
			}
		}
	}
	else {
		single = FALSE;
		op	= ( fmmode ? _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) : _i_SCH_MODULE_Get_TM_DoPointer( side ) );
		fh = _i_SCH_CLUSTER_Get_PathHSide( side , op ) % 1000000;
		//
		curpointer++;
		//
		/*
		//
		// 2018.11.22
		//
		if ( curpointer >= MAX_CASSETTE_SLOT_SIZE ) { // 2003.01.03
			single = TRUE; // 2003.01.03
		} // 2003.01.03
		else {
			while ( _i_SCH_SUB_Run_Impossible_Condition( side , curpointer , -1 ) ) {
				curpointer++;
				if ( curpointer >= MAX_CASSETTE_SLOT_SIZE ) {
					single = TRUE;
					break;
				}
				//
				if ( ( _i_SCH_CLUSTER_Get_PathHSide( side , curpointer ) % 1000000 ) != HANDLING_ALL_SIDE ) {
					single = TRUE;
				}
			}
		}
		//
		*/
		//
		//
		// 2018.11.22
		//
		//
		if ( curpointer >= MAX_CASSETTE_SLOT_SIZE ) { // 2003.01.03
			single = TRUE; // 2003.01.03
		} // 2003.01.03
		else {
			while ( _i_SCH_SUB_Run_Impossible_Condition( side , curpointer , -1 ) ) {
				curpointer++;
				if ( curpointer >= MAX_CASSETTE_SLOT_SIZE ) {
					single = TRUE;
					break;
				}
			}
			//
			if ( curpointer >= MAX_CASSETTE_SLOT_SIZE ) {
				single = TRUE;
			}
			else {
				//
				//=============================================================================================
				//
				Scheduler_HSide_Remap( side , curpointer , fh ); // 2018.11.22
				//
				//=============================================================================================
				//
			}
			//
		}
		//
		//=============================================================================================
		if ( ( Single_Mode == 1 ) || ( Single_Mode == 3 ) ) { // 2007.08.28
			*FM_Pointer		= op;
			*FM_Slot        = _i_SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide		= fh;
			*FM_Slot2		= 0;
			_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
			//
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			//
			return 11;
		}
		else if ( Single_Mode == 2 ) { // 2007.08.28
			*FM_Pointer2	= op;
			*FM_Slot2       = _i_SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide2		= fh;
			*FM_Slot		= 0;
			_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
			//
			if ( notinc ) {
				*returnpointer = curpointer;
			}
			else {
				if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
				else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
			}
			//
			return 12;
		}
		//=============================================================================================
		if ( curpointer < MAX_CASSETTE_SLOT_SIZE ) {
			op2 = curpointer;
			fh2 = _i_SCH_CLUSTER_Get_PathHSide( side , op2 ) % 1000000;
			if      ( fh2 == HANDLING_B_SIDE_ONLY ) {
				single = TRUE;
			}
			else if ( fh2 == HANDLING_A_SIDE_ONLY ) {
				single = TRUE;
			}
		}
		else {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , curpointer , 0 ) == 0 ) single = TRUE;
		}
		//
		if      ( fh == HANDLING_B_SIDE_ONLY ) {
			single = TRUE;
		}
		else if ( fh == HANDLING_A_SIDE_ONLY ) {
			single = TRUE;
		}
		/*
		//
		// 2018.11.22
		//
		else if ( fh == HANDLING_END_SIDE ) {
			if ( _i_SCH_MODULE_Get_FM_HWait( side ) ) {
				fh = HANDLING_B_SIDE_ONLY;
			}
			else {
				fh = HANDLING_A_SIDE_ONLY;
			}
			single = TRUE;
		}
		//
		*/
		//
		else {
			if ( _i_SCH_MODULE_Get_FM_HWait( side ) ) {
				fh = HANDLING_B_SIDE_ONLY;
				single = TRUE;
			}
			else {
				fh = HANDLING_A_SIDE_ONLY;
			}
		}
		//
		if ( hwarm_a && hwarm_b && !notarm ) {
			if ( fmmode && _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( armstart , _i_SCH_CLUSTER_Get_SlotIn( side , op ) ) ) {
				_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
				*FM_Pointer2	= op;
				*FM_Slot2       = _i_SCH_CLUSTER_Get_SlotIn( side , op );
				*FM_HSide2		= fh;
				if ( single ) {
					*FM_Slot	= 0;
				}
				else {
					if ( _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( armstart , _i_SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
						*FM_Slot		= 0;
						_i_SCH_MODULE_Set_FM_HWait( side , TRUE );
					}
					else {
						*FM_Pointer	= op2;
						*FM_Slot	= _i_SCH_CLUSTER_Get_SlotIn( side , op2 );
						*FM_HSide	= HANDLING_B_SIDE_ONLY;
						curpointer++;
					}
				}
			}
			else {
				_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
				*FM_Pointer		= op;
				*FM_Slot        = _i_SCH_CLUSTER_Get_SlotIn( side , op );
				*FM_HSide		= fh;
				if ( single ) {
					*FM_Slot2	= 0;
				}
				else {
					if ( fmmode && _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( armstart + 1 , _i_SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
						*FM_Slot2		= 0;
						_i_SCH_MODULE_Set_FM_HWait( side , TRUE );
					}
					else {
						*FM_Pointer2	= op2;
						*FM_Slot2	    = _i_SCH_CLUSTER_Get_SlotIn( side , op2 );
						*FM_HSide2		= HANDLING_B_SIDE_ONLY;
						curpointer++;
					}
				}
			}
		}
		else if ( hwarm_a ) {
			*FM_Pointer		= op;
			*FM_Slot        = _i_SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide		= fh;
			*FM_Slot2		= 0;
			if ( single ) 	_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
			else			_i_SCH_MODULE_Set_FM_HWait( side , TRUE );
		}
		else if ( hwarm_b ) {
			*FM_Pointer2	= op;
			*FM_Slot2	    = _i_SCH_CLUSTER_Get_SlotIn( side , op );
			*FM_HSide2		= fh;
			*FM_Slot		= 0;
			if ( single ) 	_i_SCH_MODULE_Set_FM_HWait( side , FALSE );
			else			_i_SCH_MODULE_Set_FM_HWait( side , TRUE );
		}
	}
	//
	if ( notinc ) {
		*returnpointer = curpointer;
	}
	else {
		if ( fmmode ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , curpointer );
		else          _i_SCH_MODULE_Set_TM_DoPointer( side , curpointer );
	}
	//
	return 1;
}


int _SCH_MACRO_FM_PICK_DATA_FROM_FM( int fms , BOOL doubletm , int mode , int CHECKING_SIDE , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *retpointer ) {
	// mode = 0	:	Arm All
	//        1	:	Arm A
	//        2	:	Arm B
	//        3	:	Arm A or B but Single
	//        10	:	Arm All
	//        11	:	Arm A
	//        12	:	Arm B
	//        13	:	Arm A or B but Single
	int Result , FM_HSide , FM_HSide2 , cpreject;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Result = _SCH_MACRO_inside_CHECK_PICK_from_FM( fms , CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_HSide , &FM_HSide2 , doubletm , mode , &cpreject , retpointer );
	//
	if ( Result >= 0 ) {
		if ( ( Result >= 0 ) && ( *FM_Slot > 0 ) ) {
			if ( !_i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , *FM_Pointer ) ) {
				if ( mode < 10 ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( CHECKING_SIDE , *retpointer );
				//
				_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
				//
				return -101;
			}
		}
		if ( ( Result >= 0 ) && ( *FM_Slot2 > 0 ) ) {
			if ( !_i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , *FM_Pointer2 ) ) {
				if ( mode < 10 ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( CHECKING_SIDE , *retpointer );
				//
				_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
				//
				return -102;
			}
		}
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Result;
}

int _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( int TMATM , BOOL TM_Double , int Single_Mode , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *returnpointer , int *FM_HSide , int *FM_HSide2 , BOOL *cpreject ) {
	int Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Res = _SCH_MACRO_inside_CHECK_PICK_from_FM( TMATM , side , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_HSide , FM_HSide2 , TM_Double , Single_Mode , cpreject , returnpointer );
	//
	// 2017.02.17
	if ( ( Res >= 0 ) && ( *FM_Slot > 0 ) ) {
		if ( !_i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( side , *FM_Pointer ) ) {
			if ( Single_Mode < 10 ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , *returnpointer );
			//
			_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
			//
			return -101;
		}
	}
	if ( ( Res >= 0 ) && ( *FM_Slot2 > 0 ) ) {
		if ( !_i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( side , *FM_Pointer2 ) ) {
			if ( Single_Mode < 10 ) _i_SCH_MODULE_Set_FM_DoPointer_Sub( side , *returnpointer );
			//
			_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
			//
			return -102;
		}
	}
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
}

void _SCH_MACRO_OTHER_LOT_LOAD_WAIT( int side , unsigned int timeoutsec ) { // 2007.09.06
//	int Result , offsettime , md; // 2007.11.30
	int Result , Result2;
	long time_start;

	//======================================================================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return;
	//======================================================================================================================================
	//================================================================================================
	// 2007.11.30
	//================================================================================================
	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) {
		if ( _i_SCH_SYSTEM_USING_GET( side ) <= 0 ) {
			if ( SCHMULTI_GET_AFTERSEL_WAITMODE( side ) ) { // 2007.10.02
				time_start = GetTickCount();
				while( TRUE ) {
					//----------------------------------------------------------------------
					_i_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP LOT_LOAD_WAIT 1 RESULT is [%d]\n" , SCHMULTI_AFTER_SELECT_CHECK_GET() );
					//----------------------------------------------------------------------
					if ( _i_SCH_SYSTEM_USING_GET( side ) > 0 ) break;
					if ( ( GetTickCount() - time_start ) >= ( 5 * 1000 ) ) break;
					Sleep(1);
				}
			}
		}
	}
/*
	//================================================================================================
	// 2007.10.02
	//================================================================================================
	if ( SCHMULTI_GET_AFTERSEL_WAITMODE() ) { // 2007.10.02
		offsettime = 7;
		md = 0;
		time_start = GetTickCount();
		while( TRUE ) {
			//----------------------------------------------------------------------
			_i_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP LOT_LOAD_WAIT 1 RESULT is [%d]\n" , SCHMULTI_AFTER_SELECT_CHECK_GET() );
//printf( "[A] FEM[%d] STEP LOT_LOAD_WAIT [%d] RESULT\n" , side , SCHMULTI_AFTER_SELECT_CHECK_GET() );
			//----------------------------------------------------------------------
			if ( !SCHMULTI_GET_AFTERSEL_WAITMODE() ) {
				md = 1;
				break;
			}
			if ( ( GetTickCount() - time_start ) >= ( 10 * 1000 ) ) break;
			Sleep(1);
		}
		if ( md == 1 ) {
			time_start = GetTickCount();
			while( TRUE ) {
				//----------------------------------------------------------------------
				_i_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP LOT_LOAD_WAIT 2 RESULT is [%d]\n" , SCHMULTI_AFTER_SELECT_CHECK_GET() );
				//----------------------------------------------------------------------
//printf( "[B] FEM[%d] STEP LOT_LOAD_WAIT [%d] RESULT\n" , side , SCHMULTI_AFTER_SELECT_CHECK_GET() );
				if ( ( GetTickCount() - time_start ) >= ( 3 * 1000 ) ) break;
				Sleep(1);
			}
		}
	}
	else {
		offsettime = 0;
	}
*/
	//================================================================================================
	if ( ( _i_SCH_SYSTEM_USING_GET( side ) > 0 ) && !_i_SCH_SYSTEM_USING_RUNNING( side ) ) {
		if ( Scheduler_Main_Waiting( side , 1 , &Result , 0 , &Result2 ) == SYS_SUCCESS ) {
			time_start = GetTickCount();
			while( TRUE ) {
				//----------------------------------------------------------------------
				_i_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP LOT_LOAD_WAIT 3 RESULT is [%d]\n" , SCHMULTI_AFTER_SELECT_CHECK_GET() );
				//----------------------------------------------------------------------
				if ( _i_SCH_SYSTEM_USING_RUNNING( side ) ) break;
				if ( ( GetTickCount() - time_start ) >= ( ( timeoutsec ) * 1000 ) ) break;
				Sleep(1);
			}
		}
		else {
			//----------------------------------------------------------------------
			_i_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP LOT_LOAD_WAIT 4 RESULT is [%d][%d][%d][%d]\n" , SCHMULTI_AFTER_SELECT_CHECK_GET() , _i_SCH_SYSTEM_USING_GET( side ) , Result , Result2 );
			//----------------------------------------------------------------------
		}
	}
	//================================================================================================
}
