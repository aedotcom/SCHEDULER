#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Dll_Interface.h"
#include "IOlog.h"
#include "IO_Part_data.h"
#include "Robot_Handling.h"
#include "FA_Handling.h"
#include "sch_prepost.h"
#include "sch_CassetteResult.h"
#include "sch_CassetteSupply.h"
#include "sch_main.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_preprcs.h"
#include "sch_sub.h"
#include "sch_multi_alic.h"
#include "sch_FMArm_multi.h"
#include "sch_sdm.h"
#include "sch_util.h"
#include "sch_Cassmap.h"
#include "sch_OneSelect.h"
#include "sch_processmonitor.h"
#include "sch_CommonUser.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "Multireg.h"
#include "Multijob.h"
#include "Errorcode.h"
#include "Utility.h"

#include "INF_sch_macro.h"


void EIL_CHANGE_STATUS_TO( int bm , int slot , int data );
void SCHMULTI_SET_LOG_MESSAGE( char *data );
void _EIL_LOG_LOT_PRINTF( int M , int Slot , LPSTR list , ... );
int _i_EIL_OPERATE_REQUEST_TM_TO_EIL( int bm );

//------------------------------------------------------------------------------------------
/*
1) System.cfg

	EIL_INTERFACE	1		: or Fixed TM Count

2) Event Message

	EIL_CONTROL BEGIN|CMx|JOBID|MID|PPID

	EIL_CONTROL REMOVE|BMx|SLOT

	EIL_CONTROL START|BMx

	EIL_CONTROL INSERT|BMx|SLOT|CMx|WAFERNO|CLUSTER_RECIPE|INFORMATION_FILE

		=> CallBack DLL

			__declspec(dllexport) BOOL USER_RECIPE_INFO_VERIFICATION_EIL( int side , int pt , int bmc , int bmslot , int scrmodule , int srcslot , char *clusterFile , char *informationfile );

		=> Information File

			//============================================================================================
			CONTROLJOB	{Data}									// {Data} = String
			PROCESSJOB	{Data}									// {Data} = String
			PPID		{Data}									// {Data} = String
			//============================================================================================
			PATHRANGE	{Data}									// {Data} = No
			PATHDATA	{Data1}	{Data2}	{Data3}	{Data4}	{Data4}	// {Data1} = ClusterDepth,No,from 1
																// {Data2} = PM Module
																// {Data3} = Main Recipe
																// {Data4} = Post Recipe
																// {Data5} = Pre Recipe
			//============================================================================================
			PATHIN		{Data}									// {Data} = CM/BM Module
			PATHOUT		{Data}									// {Data} = CM/BM Module
			SLOTIN		{Data}									// {Data} = No
			SLOTOUT		{Data}									// {Data} = No
			//============================================================================================
			PATHHSIDE	{Data}									// {Data} = No
			SWITCHINOUT	{Data}									// {Data} = No
			FAILOUT		{Data}									// {Data} = No
			BUFFER		{Data}									// {Data} = No
			STOCK		{Data}									// {Data} = No
			OPTIMIZE	{Data}									// {Data} = No
			EXTRA		{Data}									// {Data} = No
			//============================================================================================
			LOTSPECIAL	{Data}									// {Data} = No
			LOTUSERSPECIAL	{Data}								// {Data} = String
			CLUSTERUSERSPECIAL	{Data}							// {Data} = String
			CLUSTERTUNEDATA	{Data}								// {Data} = String
			//============================================================================================
			USERAREA1	{Data}									// {Data} = String
			USERAREA2	{Data}									// {Data} = String
			USERAREA3	{Data}									// {Data} = String
			USERAREA4	{Data}									// {Data} = String
			//============================================================================================
			PARALLEL	{Data}									// {Data} = String
			//============================================================================================

	EIL_CONTROL FINISH

<ERROR CODE>

	INSERT

		1 - BM 값이 잘못된 값입니다.
		2 - BM Slot값이 잘못된 값입니다.
		4 - SourceWafer 값이 1-MAX_CASSETTE_SLOT 사이가 아닙니다.
		5 - BM의 해당 Slot에 Method가 존재합니다.

		11 - BM이 사용하지 않는 BM입니다.
		12 - BM이 Dummy/Align용 BM입니다.
		13 - BM이 Enable상태가 아닙니다.
		14 - BM이 FM_SIDE상태가 아닙니다.
		15 - BM이 FM_SIDE로 동작중입니다.

		21 - Source 정보가 같은 Wafer가 이미 존재합니다.
		22 - 해당 Wafer를 inser할 내부 저장공간이 없습니다.

		31 - Cluster Recipe File을 읽는중에 File에 문제가 있습니다.
		32 - Cluster Recipe File을 읽는중에 USER_RECIPE_CLUSTER_DATA_READ DLL에 문제가 있습니다.(FA Mode)
		33 - Cluster Recipe File을 읽는중에 DLL의 Data를 적용하는데 문제가 있습니다.(FA Mode)
		34 - Cluster Recipe File을 읽는중에 USER_RECIPE_CLUSTER_DATA_READ DLL에 문제가 있습니다.
		35 - Cluster Recipe File을 읽는중에 DLL의 Data를 적용하는데 문제가 있습니다.
	  
		51 - Information File을 읽는중에 문제가 있습니다.
		61 - DLL Interface(Normal)를 통한 Data Setting중에 문제가 있습니다.
		71 - DLL Interface(MFI)를 통한 Data Setting중에 문제가 있습니다.
		81 - Lot Pre Fail


	START

		1 - BM 값이 잘못된 값입니다.

		11 - BM이 사용하지 않는 BM입니다.
		12 - BM이 Dummy/Align용 BM입니다.
		13 - BM이 Enable상태가 아닙니다.
		14 - BM이 FM_SIDE상태가 아닙니다.
		15 - BM이 FM_SIDE로 동작중입니다.


	REMOVE

		1 - BM 값이 잘못된 값입니다.
		2 - BM Slot값이 잘못된 값입니다.
		6 - BM의 해당 Slot에 Method가 존재하지 않습니다.
		7 - BM의 해당 Method의 PathRange값이 잘못되었습니다.

		11 - BM이 사용하지 않는 BM입니다.
		12 - BM이 Dummy/Align용 BM입니다.
		13 - BM이 Enable상태가 아닙니다.
		14 - BM이 FM_SIDE상태가 아닙니다.
		15 - BM이 FM_SIDE로 동작중입니다.

	BEGIN

		1 - 해당 SIDE(CM)이 Running이 아닐때
		2 - 해당 SIDE(CM)의 Wafer가 한장 이상이 진행중에 있을때

		11 - MAIN_CONTROL IO의 상태가 CTC_IDLE(0)일 때
		12 - MAIN_CONTROL IO의 상태가 CTC_IDLE(0)보다 작을때
		13 - MAIN_CONTROL IO의 상태가 CTC_DISABLE(5)일 때
		14 - MAIN_CONTROL IO의 상태가 CTC_ABORTING(4)일 때
		15 - MAIN_CONTROL IO의 상태가 CTC_WAIT_HAND_OFF(7)보다 크거나 같을때

	END

		1 - 해당 SIDE(CM)이 Running이 아닐때
		2 - 해당 SIDE(CM)의 Wafer가 한장 이상이 진행중에 있을때

		11 - MAIN_CONTROL IO의 상태가 CTC_IDLE(0)일 때
		12 - MAIN_CONTROL IO의 상태가 CTC_IDLE(0)보다 작을때
		13 - MAIN_CONTROL IO의 상태가 CTC_DISABLE(5)일 때
		14 - MAIN_CONTROL IO의 상태가 CTC_ABORTING(4)일 때
		15 - MAIN_CONTROL IO의 상태가 CTC_WAIT_HAND_OFF(7)보다 크거나 같을때


3) IO Status

	CTC.BMx_EIL_MODE
		0	:	N/A
		1	:	Disable
		2	:	to Go FEM Control Mode
		3	:	FEM Control Mode
		4	:	EQ Control Mode
		5	:	EQ Control Mode(Invalid)
		6	:	Wait Mode(Unknown)

	CTC.BMx_EIL_STATUSx
		0	: Unknown					:	?	:	?		:	???
		1	: Need Supply(Empty)		:	X	:	EIL		:	1.X-S
		2	: Remain					:	O	:	EIL		:	2.O-A
		3	: Need Remove(Complete)		:	O	:	EIL		:	3.O-RC
		4	: Need Remove(Interrupt)	:	O	:	EIL		:	4.O-RI
		5	: Need Remove(ReturnReq)	:	O	:	EIL		:	5.O-RQ
		6	: Occupy					:	O	:	EIL		:	6.O-OC
		7	: Removed(Empty)			:	X	:	EIL		:	7.X-R
		8	: Equip						:	?	:	EQUIP	:	8.EQP
		9	: Maint						:	?	:	EQUIP	:	9.DIS

	CTC.BMx_EIL_ERROR

		REMOVE , START , INSERT 의 Message일 경우 사용

	CTC.CMx_EIL_ERROR

		BEGIN , END 의 Message일 경우 사용


*/

char *EIL_Get_SLOT_FOLDER( int side , int pt );
//------------------------------------------------------------------------------------------
enum { _EIL_MODE_UNKNOWN , _EIL_MODE_DISABLE , _EIL_MODE_GOTO_FEM , _EIL_MODE_FEM , _EIL_MODE_EQUIP , _EIL_MODE_EQUIP_MFI , _EIL_MODE_WAIT };
//
enum { _EIL_STS_UNKNOWN , _EIL_STS_X_NEED_SUPPLY , _EIL_STS_O_REMAIN , _EIL_STS_O_NEED_REMOVE , _EIL_STS_O_NEED_REMOVE_INT , _EIL_STS_O_NEED_REMOVE_RTRQ , _EIL_STS_O_OCCUPY , _EIL_STS_X_REMOVED , _EIL_STS_EQUIP , _EIL_STS_MAINT };
//------------------------------------------------------------------------------------------
//
int Address_EIL_CM_ERROR[MAX_CASSETTE_SIDE];
int Address_EIL_BM_ERROR[MAX_BM_CHAMBER_DEPTH];
int Address_EIL_BM_MODE[MAX_BM_CHAMBER_DEPTH];
int Address_EIL_BM_STATUS[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];
int Address_EIL_BM_UNIQUEID[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];

int Address_EIL_TIM_LOG_POINTER;
int Address_EIL_JOB_LOG_POINTER;
int	Address_EIL_LOT_LOG_POINTER[MAX_CASSETTE_SIDE];
//----------------------------------------------------------------------------
time_t	_EIL_TIME_FOR_INSERT[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];

int		_EIL_SUPPLY_COUNT_INSERT[ MAX_CASSETTE_SIDE ];
int		_EIL_SUPPLY_COUNT_REMOVE[ MAX_CASSETTE_SIDE ];
//----------------------------------------------------------------------------
char	*_EIL_METHOD_RECIPE[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2011.06.15
//----------------------------------------------------------------------------
char	*_EIL_METHOD_FOLDER[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2011.05.21
char	_EIL_METHOD_LAST_FOLDER[256]; // 2011.05.21
char	_EIL_METHOD_LAST_PPID[256]; // 2011.07.15
//----------------------------------------------------------------------------
char	*_EIL_READY_RECIPE_NAME[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ][ MAX_CHAMBER ]; // 2011.07.28
//----------------------------------------------------------------------------
int		IO_ADD_READ_DIGITAL_NAME( int *address , LPSTR list , ... );
BOOL	IO_ADD_WRITE_DIGITAL_NAME( int *address , int data , LPSTR list , ... );
//----------------------------------------------------------------------------
int		BM_EIL_OPERATE_SIGNAL[ MAX_BM_CHAMBER_DEPTH ];
//----------------------------------------------------------------------------
int		BM_EIL_AUTO_TMSIDE[ MAX_BM_CHAMBER_DEPTH ]; // 2012.04.27
//----------------------------------------------------------------------------
int		PM_EIL_ENABLE_CONTROL[ MAX_CHAMBER ]; // 2011.07.25
//----------------------------------------------------------------------------
int		_EIL_WAFER_NUMBERING = 0; // 2012.03.23
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL	SCHEDULER_LOG_FOLDER( int CHECKING_SIDE , char *Buffer , char *recipefile );
//----------------------------------------------------------------------------
int		Get_Module_ID_From_String( int , char * , int );
//----------------------------------------------------------------------------
//IO_ADD_WRITE_DIGITAL_NAME
//IO_LOT_DIR_INITIAL
//LOG_MTL_SET_DONE_WFR_COUNT
//LOG_MTL_SET_DONE_CARR_COUNT
//USER_RECIPE_INFO_VERIFICATION_EIL
//_SCH_INF_DLL_INFO_VERIFICATION_EIL

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_EIL_FMMODE_GO_TMSIDE[MAX_CHAMBER];

int _EIL_FMMODE_TM_INTERFACE_FUNCTION = -2;


int _EIL_FMMODE_BM_MODE_LAST[MAX_CHAMBER]; 
int _EIL_FMMODE_BM_MODE_STOPPED[MAX_CHAMBER]; 

int _EIL_FMMODE_BM_TMSIDE_RUNNING[MAX_CHAMBER]; 
int _EIL_FMMODE_BM_TMSIDE_TIMER[MAX_CHAMBER]; 

#define _EIL_FMMODE_INSERT_MAX_COUNT	512

CRITICAL_SECTION CR_EIL_FMMODE_INSERT;

int _EIL_FMMODE_INSERT_GID = 1;
//
int _EIL_FMMODE_INSERT_COUNT;
int _EIL_FMMODE_INSERT_SIDE[_EIL_FMMODE_INSERT_MAX_COUNT];
int _EIL_FMMODE_INSERT_PT[_EIL_FMMODE_INSERT_MAX_COUNT];
int _EIL_FMMODE_INSERT_WFR[_EIL_FMMODE_INSERT_MAX_COUNT];
int _EIL_FMMODE_INSERT_USE[_EIL_FMMODE_INSERT_MAX_COUNT];
int _EIL_FMMODE_INSERT_ID[_EIL_FMMODE_INSERT_MAX_COUNT];


int _EIL_FMMODE_INSERT_SET_DATA( int s , int p , int w , int *id ) {
	//
	EnterCriticalSection( &CR_EIL_FMMODE_INSERT );
	//
	if ( w <= 0 ) {
		LeaveCriticalSection( &CR_EIL_FMMODE_INSERT );
		return 1;
	}
	//
	if ( _EIL_FMMODE_INSERT_COUNT >= _EIL_FMMODE_INSERT_MAX_COUNT ) {
		LeaveCriticalSection( &CR_EIL_FMMODE_INSERT );
		return 2;
	}
	//
	_EIL_FMMODE_INSERT_SIDE[_EIL_FMMODE_INSERT_COUNT] = s;
	_EIL_FMMODE_INSERT_PT[_EIL_FMMODE_INSERT_COUNT] = p;
	_EIL_FMMODE_INSERT_WFR[_EIL_FMMODE_INSERT_COUNT] = w;
	_EIL_FMMODE_INSERT_USE[_EIL_FMMODE_INSERT_COUNT] = 0;
	*id = _EIL_FMMODE_INSERT_GID;
	_EIL_FMMODE_INSERT_ID[_EIL_FMMODE_INSERT_COUNT] = *id;
	//
	_EIL_FMMODE_INSERT_GID++;
	//
	_EIL_FMMODE_INSERT_COUNT++;
	//
	LeaveCriticalSection( &CR_EIL_FMMODE_INSERT );
	//
	return 0;
}


int _EIL_FMMODE_INSERT_GET_DATA( BOOL remain , int id , int *s , int *p , int *w ) {
	int i;
	//
	EnterCriticalSection( &CR_EIL_FMMODE_INSERT );
	//
	if ( id <= 0 ) {
		while ( TRUE ) {
			//
			if ( _EIL_FMMODE_INSERT_COUNT <= 0 ) {
				LeaveCriticalSection( &CR_EIL_FMMODE_INSERT );
				return 1;
			}
			//
			*s = _EIL_FMMODE_INSERT_SIDE[0];
			*p = _EIL_FMMODE_INSERT_PT[0];
			*w = _EIL_FMMODE_INSERT_WFR[0];
			//
			if ( _i_SCH_SYSTEM_USING_RUNNING( *s ) ) break;
			//
			for ( i = 0 ; i < (_EIL_FMMODE_INSERT_COUNT-1) ; i++ ) {
				_EIL_FMMODE_INSERT_SIDE[i]  = _EIL_FMMODE_INSERT_SIDE[i+1];
				_EIL_FMMODE_INSERT_PT[i]  = _EIL_FMMODE_INSERT_PT[i+1];
				_EIL_FMMODE_INSERT_WFR[i] = _EIL_FMMODE_INSERT_WFR[i+1];
				_EIL_FMMODE_INSERT_USE[i] = _EIL_FMMODE_INSERT_USE[i+1];
				_EIL_FMMODE_INSERT_ID[i]  = _EIL_FMMODE_INSERT_ID[i+1];
			}
			//
			_EIL_FMMODE_INSERT_COUNT--;
			//
		}
		if ( remain ) {
			for ( i = 0 ; i < _EIL_FMMODE_INSERT_COUNT ; i++ ) {
				//
				if ( _EIL_FMMODE_INSERT_USE[i] ) continue;
				//
				*s = _EIL_FMMODE_INSERT_SIDE[i];
				//
				if ( !_i_SCH_SYSTEM_USING_RUNNING( *s ) ) continue;
				//
				*p = _EIL_FMMODE_INSERT_PT[i];
				*w = _EIL_FMMODE_INSERT_WFR[i];
				//
				_EIL_FMMODE_INSERT_USE[i] = 1;
				//
				break;
			}
		}
		else {
			//
			for ( i = 0 ; i < (_EIL_FMMODE_INSERT_COUNT-1) ; i++ ) {
				_EIL_FMMODE_INSERT_SIDE[i]  = _EIL_FMMODE_INSERT_SIDE[i+1];
				_EIL_FMMODE_INSERT_PT[i]  = _EIL_FMMODE_INSERT_PT[i+1];
				_EIL_FMMODE_INSERT_WFR[i] = _EIL_FMMODE_INSERT_WFR[i+1];
				_EIL_FMMODE_INSERT_USE[i] = _EIL_FMMODE_INSERT_USE[i+1];
				_EIL_FMMODE_INSERT_ID[i]  = _EIL_FMMODE_INSERT_ID[i+1];
			}
			//
			_EIL_FMMODE_INSERT_COUNT--;
		}
	}
	else {
		//
		for ( i = 0 ; i < _EIL_FMMODE_INSERT_COUNT ; i++ ) {
			//
			if ( _EIL_FMMODE_INSERT_ID[i] != id ) continue;
			//
			*s = _EIL_FMMODE_INSERT_SIDE[i];
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( *s ) ) return 2;
			//
			*p = _EIL_FMMODE_INSERT_PT[i];
			*w = _EIL_FMMODE_INSERT_WFR[i];
			//
			_EIL_FMMODE_INSERT_USE[i] = 1;
			//
			break;
		}
		//
		if ( !remain ) {
			//
			for ( ; i < (_EIL_FMMODE_INSERT_COUNT-1) ; i++ ) {
				_EIL_FMMODE_INSERT_SIDE[i]  = _EIL_FMMODE_INSERT_SIDE[i+1];
				_EIL_FMMODE_INSERT_PT[i]  = _EIL_FMMODE_INSERT_PT[i+1];
				_EIL_FMMODE_INSERT_WFR[i] = _EIL_FMMODE_INSERT_WFR[i+1];
				_EIL_FMMODE_INSERT_USE[i] = _EIL_FMMODE_INSERT_USE[i+1];
				_EIL_FMMODE_INSERT_ID[i]  = _EIL_FMMODE_INSERT_ID[i+1];
			}
			//
			_EIL_FMMODE_INSERT_COUNT--;
		}
	}
	//
	//
	LeaveCriticalSection( &CR_EIL_FMMODE_INSERT );
	//
	return 0;

}

int _EIL_FMMODE_INF_GET_BMALLSTATUS( int ch , int SlotSize , int *ModeData , int *SlotData , int *UniqueData ) {
	int i;
	//
	*ModeData = IO_ADD_READ_DIGITAL_NAME( &(Address_EIL_BM_MODE[ch-BM1]) , "CTC.BM%d_EIL_MODE" , ch - BM1 + 1 );
	//
	for ( i = 1 ; i <= SlotSize ; i++ ) {
		SlotData[i-1] = IO_ADD_READ_DIGITAL_NAME( &(Address_EIL_BM_STATUS[ch-BM1][i-1]) , "CTC.BM%d_EIL_STATUS%d" , ch - BM1 + 1 , i );
		UniqueData[i-1] = IO_ADD_READ_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[ch-BM1][i-1]) , "CTC.BM%d_EIL_UNIQUEID%d" , ch - BM1 + 1 , i );
	}
	//
	return TRUE;
	//
}

int _EIL_FMMODE_INF_GET_BMMODESTATUS( int ch ) {
	//
	return IO_ADD_READ_DIGITAL_NAME( &(Address_EIL_BM_MODE[ch-BM1]) , "CTC.BM%d_EIL_MODE" , ch - BM1 + 1 );
	//
}


void _EIL_FMMODE_INIT( int apmode , int side ) {
	int i;
	//
	if ( apmode == 0 ) {
		//============================================================================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			InitializeCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
		}
		InitializeCriticalSection( &CR_EIL_FMMODE_INSERT );
		//============================================================================================================================
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//============================================================================================================================
		_EIL_FMMODE_INSERT_COUNT = 0;
		//============================================================================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			//
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			//
			_EIL_FMMODE_BM_MODE_LAST[i] = -1;
			//
			_EIL_FMMODE_BM_MODE_STOPPED[i] = 0;
		}
		//============================================================================================================================
	}
	//
	if ( apmode == 3 ) {
		if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION == -2 ) {
			_EIL_FMMODE_TM_INTERFACE_FUNCTION = _FIND_FROM_STRING( _K_F_IO + 1 , "EIL_TM_INTERFACE" );
		}
	}
	if ( apmode == 4 ) {
		//============================================================================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) _EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 0;
		//============================================================================================================================
	}
}

//WAIT - FEM               <ACT>         - GOTM,TM           - GOFEM
//
//       NEED_SUPPLY(X) - OCUPY(O)       - NEED_SUPPLY(X)
//                                       - OCUPY(O)
//                                       - NEED_REMOVE(O)
//
//                      - NEED_SUPPLY(X) - NEED_SUPPLY(X)
//                                       - NEED_REMOVE(O)
//
//       NEED_REMOVE(O) - REMOVE(X)      - NEED_REMOVE(O)
//                                       - REMOVE(X)
//
//       REMOVE(X)      - REMOVE(X)      - NEED_REMOVE(O)
//                                       - REMOVE(X)
//
//       


int _EIL_FMMODE_RUN_CONTROL_TM( int side ) {
	int i , j , s , p , w , wfrfail;
	int ModeData , SlotData[MAX_CASSETTE_SLOT_SIZE] , UniqueData[MAX_CASSETTE_SLOT_SIZE];
	int ModeData2 , SlotData2[MAX_CASSETTE_SLOT_SIZE] , UniqueData2[MAX_CASSETTE_SLOT_SIZE];
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_EIL_FMMODE_INF_GET_BMALLSTATUS( i , _i_SCH_PRM_GET_MODULE_SIZE( i ) , &ModeData , SlotData , UniqueData ) ) ModeData = 0;
		//
		if ( ModeData == _EIL_MODE_UNKNOWN ) {
			//
			_EIL_FMMODE_BM_MODE_STOPPED[i]++;
			//
			if ( _EIL_FMMODE_BM_MODE_STOPPED[i] > 64 ) return 0;
			//
			continue;
		}
		else {
			_EIL_FMMODE_BM_MODE_STOPPED[i] = 0;
		}
		//
		EnterCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
		//
		if ( _EIL_FMMODE_BM_TMSIDE_RUNNING[i] >= 2 ) {
			//
			if ( ModeData == _EIL_MODE_FEM ) {
				//
				if ( ( GetTickCount() - _EIL_FMMODE_BM_TMSIDE_TIMER[i] ) >= 15000 ) {
					_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
					_EIL_FMMODE_BM_MODE_LAST[i] = -1;
				}
				//
			}
			else if ( ModeData == _EIL_MODE_EQUIP ) {
				_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			}
			//
			LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
			//
			continue;
			//
		}
		//
		if ( ModeData > _EIL_MODE_UNKNOWN ) {
			//
			wfrfail = FALSE;
			//
			for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
				//
				switch( SlotData[j-1] ) {
				case _EIL_STS_UNKNOWN	: //Unknown					:	?	:	?		:	???
					wfrfail = TRUE;
					break;
				case _EIL_STS_X_NEED_SUPPLY	: //Need Supply(Empty)		:	X	:	EIL		:	1.X-S
					break;
				case _EIL_STS_O_REMAIN	: //Remain					:	O	:	EIL		:	2.O-A
					break;
				case _EIL_STS_O_NEED_REMOVE	: //Need Remove(Complete)	:	O	:	EIL		:	3.O-RC
					break;
				case _EIL_STS_O_NEED_REMOVE_INT	: //Need Remove(Interrupt)	:	O	:	EIL		:	4.O-RI
					break;
				case _EIL_STS_O_NEED_REMOVE_RTRQ	: //Need Remove(ReturnReq)	:	O	:	EIL		:	5.O-RQ
					break;
				case _EIL_STS_O_OCCUPY	: //Occupy					:	O	:	EIL		:	6.O-OC
					break;
				case _EIL_STS_X_REMOVED	: //Removed(Empty)			:	X	:	EIL		:	7.X-R
					break;
				case _EIL_STS_EQUIP	: //Equip					:	?	:	EQUIP	:	8.EQP
					wfrfail = TRUE;
					break;
				case _EIL_STS_MAINT	: //Maint					:	?	:	EQUIP	:	9.DIS
					wfrfail = TRUE;
					break;
				default :
					wfrfail = TRUE;
					break;
				}
				//
			}
			//
			if ( wfrfail ) ModeData = _EIL_MODE_UNKNOWN;
		}
		//
		if ( ModeData <= _EIL_MODE_UNKNOWN ) {
			if ( _EIL_FMMODE_BM_MODE_LAST[i] != ModeData ) {
				_IO_CONSOLE_PRINTF( "_EIL_FMMODE\tBM_MODE %s : %d => %d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _EIL_FMMODE_BM_MODE_LAST[i] , ModeData );
			}
			//
			_EIL_FMMODE_BM_MODE_LAST[i] = ModeData;
			//
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			//
			LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
			//
			continue;
			//
		}
		//
		if ( !_EIL_FMMODE_INF_GET_BMALLSTATUS( i , _i_SCH_PRM_GET_MODULE_SIZE( i ) , &ModeData2 , SlotData2 , UniqueData2 ) ) continue;
		//
		if ( ModeData != ModeData2 ) {
			LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
			continue;
		}
		//
		for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			if ( SlotData[j] != SlotData2[j] ) break;
		}
		if ( j != _i_SCH_PRM_GET_MODULE_SIZE( i ) ) {
			LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
			continue;
		}
		//
		for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			if ( UniqueData[j] != UniqueData2[j] ) break;
		}
		if ( j != _i_SCH_PRM_GET_MODULE_SIZE( i ) ) {
			LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
			continue;
		}
		//
		for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			//
			if ( ModeData == _EIL_MODE_FEM ) {
				//
				if ( _EIL_FMMODE_BM_MODE_LAST[i] != ModeData ) {
					//
					switch( SlotData[j-1] ) {
					case _EIL_STS_X_NEED_SUPPLY	: //Need Supply(Empty)		:	X	:	EIL		:	1.X-S
						//
						_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , BUFFER_READY_STATUS );
						//
						break;
					case _EIL_STS_O_REMAIN	: //Remain					:	O	:	EIL		:	2.O-A
						break;
					case _EIL_STS_O_NEED_REMOVE	: //Need Remove(Complete)	:	O	:	EIL		:	3.O-RC
						//
						if ( _EIL_FMMODE_INSERT_GET_DATA( FALSE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
						//
						_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_SUCCESS );
						_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
						_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
						//
						SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( s , p , TRUE , 0 );
						//
						_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
						_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						//
						break;
					case _EIL_STS_O_NEED_REMOVE_INT	: //Need Remove(Interrupt)	:	O	:	EIL		:	4.O-RI
						//
						if ( _EIL_FMMODE_INSERT_GET_DATA( FALSE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
						//
						_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_FAILURE );
						_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
						_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
						//
						SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( s , p , TRUE , 1 );
						//
						_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
						_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						break;
					case _EIL_STS_O_NEED_REMOVE_RTRQ	: //Need Remove(ReturnReq)	:	O	:	EIL		:	5.O-RQ
						//
						if ( _EIL_FMMODE_INSERT_GET_DATA( FALSE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
						//
						_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_PRESENT );
						_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
						_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
						//
						SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( s , p , TRUE , -2 );
						//
						_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
						_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						break;
					case _EIL_STS_O_OCCUPY	: //Occupy					:	O	:	EIL		:	6.O-OC
						//
						if ( _EIL_FMMODE_INSERT_GET_DATA( FALSE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
						//
						_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_PRESENT );
						_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
						_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
						//
						SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( s , p , TRUE , -2 );
						//
						_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
						_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_INWAIT_STATUS );
						break;
					case _EIL_STS_X_REMOVED	: //Removed(Empty)			:	X	:	EIL		:	7.X-R
						//
						_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
						_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , BUFFER_READY_STATUS );
						//
						break;
					}
					//
				}
				//
			}
			else if ( ModeData == _EIL_MODE_GOTO_FEM ) {
				//
				if ( _EIL_FMMODE_BM_MODE_LAST[i] != ModeData ) {
					//
					switch( SlotData[j-1] ) {
					case _EIL_STS_X_NEED_SUPPLY	: // Need Supply(Empty)		:	X	:	EIL		:	1.X-S
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
							_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , BUFFER_READY_STATUS );
						}
						break;
					case _EIL_STS_O_REMAIN	: //Remain					:	O	:	EIL		:	2.O-A
						break;
					case _EIL_STS_O_NEED_REMOVE	: //Need Remove(Complete)	:	O	:	EIL		:	3.O-RC
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_SUCCESS );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
							_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
							_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						}
						break;
					case _EIL_STS_O_NEED_REMOVE_INT	: //Need Remove(Interrupt)	:	O	:	EIL		:	4.O-RI
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_FAILURE );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
							_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
							_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						}
						break;
					case _EIL_STS_O_NEED_REMOVE_RTRQ	: //Need Remove(ReturnReq)	:	O	:	EIL		:	5.O-RQ
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_PRESENT );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
							_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
							_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_OUTWAIT_STATUS );
						}
						break;
					case _EIL_STS_O_OCCUPY	: //Occupy					:	O	:	EIL		:	6.O-OC
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_PRESENT );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
							_i_SCH_MODULE_Set_BM_SIDE( i , j , s );
							_i_SCH_MODULE_Set_BM_POINTER( i , j , p );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , w , BUFFER_INWAIT_STATUS );
						}
						break;
					case _EIL_STS_X_REMOVED	: //Removed(Empty)			:	X	:	EIL		:	7.X-R
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
							_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
							_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , BUFFER_READY_STATUS );
						}
						break;
					}
					//
				}
			}
			else if ( ModeData == _EIL_MODE_EQUIP ) {
				//
				if ( _EIL_FMMODE_BM_MODE_LAST[i] == _EIL_MODE_EQUIP ) {
					//
					switch( SlotData[j-1] ) {
					case _EIL_STS_X_NEED_SUPPLY	: // Need Supply(Empty)		:	X	:	EIL		:	1.X-S
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
							_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
						}
						break;
					case _EIL_STS_O_REMAIN	: //Remain					:	O	:	EIL		:	2.O-A
						break;
					case _EIL_STS_O_NEED_REMOVE	: //Need Remove(Complete)	:	O	:	EIL		:	3.O-RC
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_SUCCESS );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
						}
						break;
					case _EIL_STS_O_NEED_REMOVE_INT	: //Need Remove(Interrupt)	:	O	:	EIL		:	4.O-RI
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_FAILURE );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
						}
						break;
					case _EIL_STS_O_NEED_REMOVE_RTRQ	: //Need Remove(ReturnReq)	:	O	:	EIL		:	5.O-RQ
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							//
							if ( _EIL_FMMODE_INSERT_GET_DATA( TRUE , UniqueData[j-1] , &s , &p , &w ) != 0 ) break;
							//
							_i_SCH_IO_SET_MODULE_RESULT( i , j , WAFERRESULT_PRESENT );
							_i_SCH_IO_SET_MODULE_SOURCE( i , j , s );
							_i_SCH_IO_SET_MODULE_STATUS( i , j , w );
							//
						}
						break;
					case _EIL_STS_O_OCCUPY	: //Occupy					:	O	:	EIL		:	6.O-OC
						//
						// 2015.01.21
						//
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) <= 0 ) {
							if ( _i_SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) {
								EIL_CHANGE_STATUS_TO( i , j , _EIL_STS_X_NEED_SUPPLY );
							}
						}
						break;
					case _EIL_STS_X_REMOVED	: //Removed(Empty)			:	X	:	EIL		:	7.X-R
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
							_i_SCH_IO_SET_MODULE_STATUS( i , j , 0 );
						}
						break;
					}
					//
				}
			}
		}
		//
		if ( _EIL_FMMODE_BM_MODE_LAST[i] != ModeData ) {
			_IO_CONSOLE_PRINTF( "_EIL_FMMODE\tBM_MODE %s : %d => %d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _EIL_FMMODE_BM_MODE_LAST[i] , ModeData );
		}
		//
		_EIL_FMMODE_BM_MODE_LAST[i] = ModeData;
		//
		switch ( ModeData ) {
		case _EIL_MODE_UNKNOWN : // N/A
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			break;
		case _EIL_MODE_DISABLE : // Disable
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			break;
		case _EIL_MODE_GOTO_FEM : // to Go FEM Control Mode
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_FM_STATION );
			break;
		case _EIL_MODE_FEM : // FEM Control Mode
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_FM_STATION );
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 0;
			break;
		case _EIL_MODE_EQUIP : // EQ Control Mode
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			break;
		case _EIL_MODE_EQUIP_MFI : // EQ Control Mode(MFI)
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			break;
		case _EIL_MODE_WAIT : // Wait Mode(Unknown)
			_EIL_FMMODE_BM_TMSIDE_RUNNING[i] = 1;
			_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_TM_STATION );
			break;
		default :
			break;
		}
		//
		LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
		//
	}
	//
	return 1;

}




int _EIL_FMMODE_RUN_CONTROL_FM_to_TM_CONDITION( int bmc ) { // 2012.09.21
	int j;
	//
	if ( _i_SCH_MODULE_Get_FM_WAFER(0) > 0 ) return FALSE;
	if ( _i_SCH_MODULE_Get_FM_WAFER(1) > 0 ) return FALSE;
	if ( _i_SCH_MODULE_Get_MFALS_WAFER(1) > 0 ) return FALSE;
	if ( _i_SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) return FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
		if ( !_i_SCH_SYSTEM_USING_RUNNING( j ) ) continue;
		if ( !_i_SCH_MODULE_Chk_TM_Free_Status( j ) ) break;
	}
	//
	if ( j == MAX_CASSETTE_SIDE ) return FALSE;
	//
	if ( !_i_SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) return FALSE;
	//
	for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( bmc ) ; j++ ) {
		if ( _i_SCH_MODULE_Get_BM_WAFER( bmc , j + 1 ) > 0 ) break;
	}
	//
	if ( j != _i_SCH_PRM_GET_MODULE_SIZE( bmc ) ) return FALSE;
	//
	if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( 0 , bmc ) <= 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_GROUP( bmc ) > 0 ) {
			return FALSE;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , bmc ) != BUFFER_OUT_MODE ) {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , bmc ) != BUFFER_SWITCH_MODE ) {
					return FALSE;
				}
			}
		}
	}
	//
	if ( _i_SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_FM_STATION ) return FALSE;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) != 0 ) return FALSE;
	//
	return TRUE;
}

int _EIL_FMMODE_RUN_CONTROL_FM( int side ) { // 2012.09.21
	int i , selbm1 , selbm2 , rc;
	//
	selbm1 = 0;
	selbm2 = 0;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		EnterCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
	}
	//
	rc = 0;
	//
	for ( i = 0; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		rc = rc + ( _i_SCH_MODULE_Get_TM_OutCount( i ) - _i_SCH_MODULE_Get_TM_InCount( i ) );
		//
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _EIL_FMMODE_BM_TMSIDE_RUNNING[i] != 0 ) {
			selbm1 = 0;
			selbm2 = 0;
			break;
		}
		//
		if ( !_EIL_FMMODE_RUN_CONTROL_FM_to_TM_CONDITION( i ) ) {
			selbm1 = 0;
			selbm2 = 0;
			break;
		}
		//
		if      ( selbm1 == 0 ) selbm1 = i;
		else if ( selbm2 == 0 ) selbm2 = i;
		//
	}
	//
//	if ( selbm1 >= BM1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm1 , -1 , TRUE , 1 , 9999 ); // 2017.02.27
	if ( selbm1 >= BM1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm1 , -1 , TRUE , 1 , 8001 ); // 2017.02.27
	if ( rc > 1 ) {
//		if ( selbm2 >= BM1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm2 , -1 , TRUE , 1 , 9999 ); // 2017.02.27
		if ( selbm2 >= BM1 ) _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , selbm2 , -1 , TRUE , 1 , 8002 ); // 2017.02.27
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[i] );
	}
	return 1;
}


int _EIL_FMMODE_MACRO_CHECK_PROCESSING( int ch ) {
	//
	if ( _EIL_FMMODE_BM_TMSIDE_RUNNING[ch] == -1 ) return -1;
	if ( _EIL_FMMODE_BM_TMSIDE_RUNNING[ch] == 0 ) return 0;
	//
	return 1;
}



	
void _EIL_FMMODE_EQ_BEGIN_END_RUN( int ch , BOOL End , char *appendstring , int Disable ) {
	//
	if ( End ) {
		printf( "_EIL_FMMODE_EQ_BEGIN_END_RUN END %s %d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Disable );
	}
	else {
		//
		printf( "_EIL_FMMODE_EQ_BEGIN_END_RUN READY %s %d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , Disable );
		//
		if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			//
			switch ( _EIL_FMMODE_INF_GET_BMMODESTATUS( ch ) ) {
			case _EIL_MODE_WAIT :
			case _EIL_MODE_EQUIP :
				//-----------------------------------
				if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
					_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "SET_FORCE_MAKE_FMSIDE BM%d" , ch - BM1 + 1 );
				}
				//-----------------------------------
				break;
			}
		}
		//
	}
	//
}

int  _EIL_FMMODE_EQ_BEGIN_END_STATUS( int ch ) {
	//
	if ( _EIL_FMMODE_INF_GET_BMMODESTATUS( ch ) == _EIL_MODE_UNKNOWN ) return SYS_ABORTED;
	//
	return SYS_SUCCESS;
}




int _EIL_FMMODE_MACRO_SPAWN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata , char *elsestr ) {
	//
	if ( mode == BUFFER_FM_STATION ) {
		//
		if      ( logwhere >= 20000 ) { // 'B'
			if      ( ( logwhere - 20000 ) == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Reject FMSIDE(B)%cWHFMGOFMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 20000 ) == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Reject FMSIDE(B)%cWHTMGOFMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 20000 ) >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Reject FMSIDE(B)%cWHTM%dGOFMSIDE BM%d:%d,B%s\n" , ( logwhere - 20000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 20000 ) , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		else if ( logwhere >= 10000 ) { // 'S'
			if      ( ( logwhere - 10000 ) == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Reject FMSIDE(S)%cWHFMGOFMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 10000 ) == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Reject FMSIDE(S)%cWHTMGOFMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 10000 ) >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Reject FMSIDE(S)%cWHTM%dGOFMSIDE BM%d:%d,S%s\n" , ( logwhere - 10000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 10000 ) , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		else {
			if      ( logwhere == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Reject FMSIDE%cWHFMGOFMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( logwhere == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Reject FMSIDE%cWHTMGOFMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( logwhere >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Reject FMSIDE%cWHTM%dGOFMSIDE BM%d:%d%s\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		//
//		return -1;
		//
	}
	else if ( mode == BUFFER_TM_STATION ) {
		//-----------------------------------
		if      ( logwhere >= 20000 ) { // 'B'
			if      ( ( logwhere - 20000 ) == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Goto TMSIDE(B)%cWHFMGOTMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 20000 ) == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Goto TMSIDE(B)%cWHTMGOTMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 20000 ) >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Goto TMSIDE(B)%cWHTM%dGOTMSIDE BM%d:%d,B%s\n" , ( logwhere - 20000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 20000 ) , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		else if ( logwhere >= 10000 ) { // 'S'
			if      ( ( logwhere - 10000 ) == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Goto TMSIDE(S)%cWHFMGOTMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 10000 ) == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Goto TMSIDE(S)%cWHTMGOTMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( ( logwhere - 10000 ) >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Goto TMSIDE(S)%cWHTM%dGOTMSIDE BM%d:%d,S%s\n" , ( logwhere - 10000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 10000 ) , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		else {
			if      ( logwhere == 0 ) { // fm
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Goto TMSIDE%cWHFMGOTMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( logwhere == 1 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Goto TMSIDE%cWHTMGOTMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			}
			else if ( logwhere >= 2 ) { // tm
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Goto TMSIDE%cWHTM%dGOTMSIDE BM%d:%d%s\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata , elsestr );
			}
		}
		//-----------------------------------
		EnterCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[ch] );
		//
		_EIL_FMMODE_BM_TMSIDE_RUNNING[ch] = 2;
		_EIL_FMMODE_BM_TMSIDE_TIMER[ch] = GetTickCount();
		//
		LeaveCriticalSection( &CR_EIL_FMMODE_GO_TMSIDE[ch] );
		//-----------------------------------
		if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
			_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "EIL_CONTROL START|BM%d" , ch - BM1 + 1 );
		}
		//-----------------------------------
	}
	//
	return 1;
}

int  _EIL_FMMODE_MACRO_RUN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata ) {

	printf( "_EIL_FMMODE_MACRO_RUN_WAITING_BM_OPERATION %s %d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mode );

	return SYS_ABORTED;
}




BOOL _EIL_FMMODE_MACRO_FM_OPERATION( int fms , int mode , int ch1 , int slot1 , int tslot1 , int side1 , int pointer1 , int ch2 , int slot2 , int tslot2 , int side2 , int pointer2 ) {
	int id;
	char cj_name[256];
	char pj_name[256];
	//
	if ( ( slot1 > 0 ) && ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
		//
		if      ( mode == MACRO_PICK ) {
			//-----------------------------------
			_i_SCH_MODULE_Inc_TM_InCount( side1 );
			//-----------------------------------
			printf( "_EIL_FMMODE_MACRO_FM_OPERATION [REMOVE1] %s:%d(%d)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch1 ) , tslot1 , slot1 );
			//-----------------------------------
			if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
				_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "EIL_CONTROL REMOVE|BM%d|%d" , ch1 - BM1 + 1 , tslot1 );
			}
			//-----------------------------------
		}
		else if ( mode == MACRO_PLACE ) {
			//-----------------------------------
			_i_SCH_MODULE_Inc_TM_OutCount( side1 );
			//-----------------------------------
			printf( "_EIL_FMMODE_MACRO_FM_OPERATION [INSERT1] %s:%d(%d) [%s]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch1 ) , tslot1 , slot1 , _i_SCH_CLUSTER_Get_Ex_RecipeName( side1 , pointer1 ) );
			//-----------------------------------
			if ( _EIL_FMMODE_INSERT_SET_DATA( side1 , pointer1 , slot1 , &id ) != 0 ) return FALSE;
			//-----------------------------------
			if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
				//
				if ( !_i_SCH_MULTIJOB_GET_CONTROLJOB_NAME( side1 , pointer1 , cj_name , 255 ) ) strcpy( cj_name , "" );
				if ( !_i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( side1 , pointer1 , pj_name , 255 ) ) strcpy( pj_name , "" );
				//
				_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "EIL_CONTROL INSERT|BM%d|%d|CM%d|%d|%s|>%d|%s|%s|%s" , ch1 - BM1 + 1 , tslot1 , _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 ) - CM1 + 1 , slot1 ,
					_i_SCH_CLUSTER_Get_Ex_RecipeName( side1 , pointer1 ) ,
					id ,
					cj_name ,
					pj_name ,
					_i_SCH_CLUSTER_Get_Ex_PPID( side1 , pointer1 )
					);
			}
			//-----------------------------------
		}
		else {
			return FALSE;
		}
		//
	}
	//
	if ( ( slot2 > 0 ) && ( ch2 >= BM1 ) && ( ch2 < TM ) ) {
		//
		if      ( mode == MACRO_PICK ) {
			//-----------------------------------
			_i_SCH_MODULE_Inc_TM_InCount( side2 );
			//-----------------------------------
			printf( "_EIL_FMMODE_MACRO_FM_OPERATION [REMOVE2] %s:%d(%d)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch2 ) , tslot2 , slot2 );
			//-----------------------------------
			if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
				_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "EIL_CONTROL REMOVE|BM%d|%d" , ch2 - BM1 + 1 , tslot2 );
			}
			//-----------------------------------
		}
		else if ( mode == MACRO_PLACE ) {
			//-----------------------------------
			_i_SCH_MODULE_Inc_TM_OutCount( side2 );
			//-----------------------------------
			printf( "_EIL_FMMODE_MACRO_FM_OPERATION [INSERT2] %s:%d(%d) [%s]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch2 ) , tslot2 , slot2 , _i_SCH_CLUSTER_Get_Ex_RecipeName( side2 , pointer2 ) );
			//-----------------------------------
			if ( _EIL_FMMODE_INSERT_SET_DATA( side2 , pointer2 , slot2 , &id ) != 0 ) return FALSE;
			//-----------------------------------
			if ( _EIL_FMMODE_TM_INTERFACE_FUNCTION >= 0 ) {
				//
				if ( !_i_SCH_MULTIJOB_GET_CONTROLJOB_NAME( side2 , pointer2 , cj_name , 255 ) ) strcpy( cj_name , "" );
				if ( !_i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( side2 , pointer2 , pj_name , 255 ) ) strcpy( pj_name , "" );
				//
				_dWRITE_FUNCTIONF_EVENT( _EIL_FMMODE_TM_INTERFACE_FUNCTION , "EIL_CONTROL INSERT|BM%d|%d|CM%d|%d|%s|>%d|%s|%s|%s" , ch2 - BM1 + 1 , tslot2 , _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 ) - CM1 + 1 , slot2 ,
					_i_SCH_CLUSTER_Get_Ex_RecipeName( side2 , pointer2 ) ,
					id ,
					cj_name ,
					pj_name ,
					_i_SCH_CLUSTER_Get_Ex_PPID( side2 , pointer2 )
					);
			}
			//-----------------------------------
		}
		else {
			return FALSE;
		}
		//
	}
	//
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------



int _EIL_COMMON_ENTER_CONTROL_FEM( int side , int mode ) {
	return -1;
}


void _EIL_BEFORE_ENTER_CONTROL_TM( int side , int mode ) {
	int i , k;
	//
	if ( mode == 2 ) {
		//
		if ( _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( &k ) ) {
			//
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( ( k != -1 ) && ( k != i ) ) continue;
				//
				if ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
					//
					_i_SCH_SUB_Make_FORCE_FMSIDE( 1000 + i ); // 2011.09.07
					//
					continue;
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) != 0 ) {
					//
					continue;
					//
				}
				//
//				_i_SCH_SUB_Make_FORCE_FMSIDE( 0 ); // 2010.10.20
				_i_SCH_SUB_Make_FORCE_FMSIDE( 1000 + i ); // 2010.10.20
				//
//				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , -1 , TRUE , 1 , 9999 ); // 2017.20.27
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , -1 , TRUE , 1 , 8003 ); // 2017.20.27
			}
			//
		}
	}
}


void _EIL_BEFORE_RUN_CONTROL_TM_1( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_2( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_3( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_4( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_5( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_6( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_7( int side ) {
}

void _EIL_BEFORE_RUN_CONTROL_TM_8( int side ) {
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _EIL_IO_NOTUSE_STATUS() { // 2012.09.14
	int i;
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) return;
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i+BM1 ) ) continue;
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i]) , _EIL_MODE_UNKNOWN , "CTC.BM%d_EIL_MODE" , i + 1 );
		//
	}
	//
}


void INIT_SCHEDULER_EIL_DATA( int apmode , int side ) {
	int i , j , k;
	//
	_EIL_FMMODE_INIT( apmode , side );
	//
	if ( apmode == 0 ) {
		//============================================================================================================================
		//
		// 2011.05.12
		//
		Address_EIL_TIM_LOG_POINTER = -2;
		Address_EIL_JOB_LOG_POINTER = -2;
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_EIL_LOT_LOG_POINTER[i] = -2;
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_EIL_CM_ERROR[i] = -2;
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2011.05.13
				_EIL_METHOD_FOLDER[i][j] = NULL;
				_EIL_METHOD_RECIPE[i][j] = NULL;
			}
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			Address_EIL_BM_MODE[i] = -2;
			Address_EIL_BM_ERROR[i] = -2;
			//
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_EIL_BM_STATUS[i][j] = -2;
				Address_EIL_BM_UNIQUEID[i][j] = -2;
			}
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) PM_EIL_ENABLE_CONTROL[ i ] = 0; // 2011.07.25
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2011.07.28
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				for ( k = 0 ; k < MAX_CHAMBER ; k++ ) {
					_EIL_READY_RECIPE_NAME[ i ][ j ][ k ] = NULL;
				}
			}
		}
		//
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) || ( apmode == 4 ) ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				STR_MEM_MAKE_COPY2( &(_EIL_METHOD_FOLDER[ i ][ j ]) , "" );
				STR_MEM_MAKE_COPY2( &(_EIL_METHOD_RECIPE[ i ][ j ]) , "" );
			}
		}
		//
		strcpy( _EIL_METHOD_LAST_FOLDER , "" );
		strcpy( _EIL_METHOD_LAST_PPID , "" );
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) PM_EIL_ENABLE_CONTROL[ i ] = 0; // 2011.07.25
		//
		_EIL_WAFER_NUMBERING = 0; // 2012.03.23
		//
	}

	if ( ( apmode == 1 ) || ( apmode == 3 ) ) {
		_EIL_SUPPLY_COUNT_INSERT[ side ] = 0; // 2010.11.26
		_EIL_SUPPLY_COUNT_REMOVE[ side ] = 0; // 2010.11.26
	}
	//
	if ( apmode == 3 ) {
		//
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) return;
		//
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) {
			//
			SCHMULTI_RUNJOB_DELETE_TUNING_INFO( TRUE , -1 , -1 ); // 2011.06.28
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_CM_ERROR[i]) , 0 , "CTC.CM%d_EIL_ERROR" , i + 1 );
			}
			//
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				//
				BM_EIL_OPERATE_SIGNAL[i] = 0;
				BM_EIL_AUTO_TMSIDE[i] = FALSE; // 2012.04.27
				//
				if ( !_i_SCH_PRM_GET_MODULE_MODE( i+BM1 ) ) continue;
				//
				IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i]) , _EIL_MODE_UNKNOWN , "CTC.BM%d_EIL_MODE" , i + 1 );
				IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[i]) , 0 , "CTC.BM%d_EIL_ERROR" , i + 1 );
				//
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i+BM1 ) ; j++ ) {
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[i][j]) , 0 , "CTC.BM%d_EIL_STATUS%d" , i + 1 , j + 1 );
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[i][j]) , 0 , "CTC.BM%d_EIL_UNIQUEID%d" , i + 1 , j + 1 );
				}
				//
			}
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2011.07.28
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					for ( k = 0 ; k < MAX_CHAMBER ; k++ ) {
						STR_MEM_MAKE_COPY2( &(_EIL_READY_RECIPE_NAME[ i ][ j ][ k ]) , "" );
					}
				}
			}
			//
		}
		//
	}
}



BOOL EIL_Get_READY_RECIPE_NAME( int side , int pt , int ch , char *processname ) { // 2011.07.28
	if ( _EIL_READY_RECIPE_NAME[ side ][ pt ][ ch ] == NULL ) return FALSE;
	strcpy( processname , STR_MEM_GET_STR( _EIL_READY_RECIPE_NAME[ side ][ pt ][ ch ] ) );
	if ( processname[0] == 0 ) return FALSE;
	return  TRUE;
}

//=======================================================================================
void EIL_CHECK_ENABLE_CONTROL( int ch , BOOL pmmode , int mode ) { // 2011.07.25
	int i;
	//
	if ( pmmode ) return;
	//
	if ( ch == -1 ) {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			PM_EIL_ENABLE_CONTROL[i] = mode;
		}
	}
	else {
		if ( ( ch >= PM1 ) && ( ch < AL ) ) {
			PM_EIL_ENABLE_CONTROL[ch] = mode;
		}
	}
}
//=======================================================================================
void EIL_Chk_Local_Time_for_Time( time_t time_data , char *buffer , int Sep ) {
	struct tm *time_data2;
	time_data2 = localtime( &time_data );
	if      ( Sep == 1 )
		sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , time_data2->tm_year+1900 , time_data2->tm_mon + 1 , time_data2->tm_mday , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
	else
		sprintf( buffer , "%04d%02d%02d_%02d%02d%02d" , time_data2->tm_year+1900 , time_data2->tm_mon + 1 , time_data2->tm_mday , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
}
void EIL_Chk_Elapsed_Time( time_t time_data , char *buffer ) {
	time_t finish;
	long elapsed_time , day , hour , min , sec;
	time( &finish );
	elapsed_time = (long) difftime( finish , time_data );
	day  = ( elapsed_time         ) / 86400;
	hour = ( elapsed_time % 86400 ) / 3600;
	min  = ( elapsed_time % 3600  ) / 60;
	sec  = ( elapsed_time % 60    );
	if ( day > 0 ) sprintf( buffer , "%02d:%02d:%02d:%02d" , day , hour , min , sec );
	else           sprintf( buffer , "%02d:%02d:%02d" , hour , min , sec );
}

void EIL_Chk_Local_Time_for_File( char *buffer , int Sep ) {
	SYSTEMTIME		SysTime;
	GetLocalTime( &SysTime );
	if      ( Sep == 1 )
		sprintf( buffer , "%04d%02d%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	else if ( Sep == 2 )
		sprintf( buffer , "%04d/%02d/%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	else if ( Sep == 3 ) // 2011.06.15
		sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	else
		sprintf( buffer , "%04d%02d%02d_%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
}
//=======================================================================================
BOOL EIL_APPEND_JOB_EXIST() {
	if ( Address_EIL_JOB_LOG_POINTER == -2 ) {
		if ( STRCMP_L( _i_SCH_PRM_GET_EIL_INTERFACE_JOBLOG() , "" ) ) {
			Address_EIL_JOB_LOG_POINTER = -1;
		}
		else {
			Address_EIL_JOB_LOG_POINTER = _FIND_FROM_STRING( _K_F_IO + 1 , _i_SCH_PRM_GET_EIL_INTERFACE_JOBLOG() );
		}
	}
	if ( Address_EIL_JOB_LOG_POINTER < 0 ) return FALSE;
	//
	return TRUE;
}

void EIL_APPEND_JOB_LOG( int side , int pt , BOOL end ) {
	int pathIn , slot;
	char buffer[256];
	char buffer2[256];
	char JobName[256];
	char MidName[256];
	char LotRecipe[256];
	char starttime[256];
	char endtime[256];
	char elapsedtime[256];
	//--------------------------------------------------------
	int  scnt;
	char ed_cj[65] , ed_pj[65];
	//
	if ( !EIL_APPEND_JOB_EXIST() ) return;
	//
	pathIn = _i_SCH_CLUSTER_Get_PathIn( side , pt );
	if ( ( pathIn < CM1 ) || ( pathIn >= PM1 ) ) pathIn = 0;
	//
	slot = _i_SCH_CLUSTER_Get_SlotIn( side , pt );
	//
	scnt = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pt );
	//
	EIL_Chk_Local_Time_for_Time( _EIL_TIME_FOR_INSERT[side][pt] , JobName , 0 );
	sprintf( MidName   , "%s_%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_CLUSTER_Get_PathIn( side , pt ) ) , slot );
	sprintf( LotRecipe , "%s" , STR_MEM_GET_STR( _EIL_METHOD_RECIPE[side][pt] ) );
	//
	if ( scnt == -99 ) {
		//
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pt ) != NULL ) {
			//
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pt ) , '|' , ed_cj , ed_pj , buffer , buffer2 , 64 );
			//
			if ( ( ed_cj[0] != 0 ) || ( ed_pj[0] != 0 ) ) sprintf( JobName , "%s_%s" , ed_cj , ed_pj );
			if ( buffer[0] != 0 )                         sprintf( LotRecipe , "%s" , buffer );
//			if ( buffer2[0] != 0 )                        sprintf( MidName , "%s" , buffer ); // 2011.07.21
			if ( buffer2[0] != 0 )                        sprintf( MidName , "%s" , buffer2 ); // 2011.07.21
			//
		}
	}
	//
	EIL_Chk_Local_Time_for_Time( _EIL_TIME_FOR_INSERT[side][pt] , starttime , 1 );
	EIL_Chk_Local_Time_for_File( endtime , 3 );
	EIL_Chk_Elapsed_Time( _EIL_TIME_FOR_INSERT[side][pt] , elapsedtime );
	//
	sprintf( buffer , "APPENDLOG %d|%s|%s|%s|%d|%d|%d|1|%s|%s|%s|%s" , end ? -1 : 5 , JobName , MidName , LotRecipe , pathIn , slot , slot , starttime , endtime , elapsedtime , EIL_Get_SLOT_FOLDER( side , pt ) );
	//
//	_dWRITE_FUNCTION_EVENT( Address_EIL_JOB_LOG_POINTER , buffer ); // 2012.02.21
	_dWRITE_FUNCTION_EVENT_TH( Address_EIL_JOB_LOG_POINTER , buffer ); // 2012.02.21
	//--------------------------------------------------------
}

void EIL_LOT_TIMER_MAKE_SLOT_FOLDER( int side , int slot , char *data , char *wid ) {
	char buffer[256];
	//--------------------------------------------------------
	if ( side < 0 || side >= MAX_CASSETTE_SIDE ) return;
	//--------------------------------------------------------
	if ( Address_EIL_LOT_LOG_POINTER[side] == -2 ) {
		sprintf( buffer , "SCHEDULER_LOTLOG%d" , side + 1 );
		Address_EIL_LOT_LOG_POINTER[side] = _FIND_FROM_STRING( _K_F_IO + 1 , buffer );
	}
	if ( Address_EIL_LOT_LOG_POINTER[side] < 0 ) return;
	sprintf( buffer , "$SLOT_FOLDER %d|%s" , slot , data );
//	_dWRITE_FUNCTION_EVENT( Address_EIL_LOT_LOG_POINTER[side] , buffer ); // 2012.02.21
	_dWRITE_FUNCTION_EVENT_TH( Address_EIL_LOT_LOG_POINTER[side] , buffer ); // 2012.02.21
	//--------------------------------------------------------
	if ( Address_EIL_TIM_LOG_POINTER == -2 ) {
		Address_EIL_TIM_LOG_POINTER = _FIND_FROM_STRING( _K_F_IO + 1 , "SCHEDULER_TIMER" );
	}
	if ( Address_EIL_TIM_LOG_POINTER < 0 ) return;
	sprintf( buffer , "$SLOT_FOLDER%d %d %s/LOTLOG/%s" , side + 1 , slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() , data );
//	_dWRITE_FUNCTION_EVENT( Address_EIL_TIM_LOG_POINTER , buffer ); // 2012.02.21
	_dWRITE_FUNCTION_EVENT_TH( Address_EIL_TIM_LOG_POINTER , buffer ); // 2012.02.21
	//
	sprintf( buffer , "WID_SET%d %d %s" , side + 1 , slot , wid ); // 2011.07.12
//	_dWRITE_FUNCTION_EVENT( Address_EIL_TIM_LOG_POINTER , buffer ); // 2012.02.21
	_dWRITE_FUNCTION_EVENT_TH( Address_EIL_TIM_LOG_POINTER , buffer ); // 2012.02.21
	//
}
//=======================================================================================


void EIL_SCHEDULER_LOG_FOLDER( char *Buffer , char *recipefile , char *jobid , char *midid ) { // 2015.09.01
	int i , j;
	// 0
	// 1  JID		// 2  LOT		// 3  MID
	// 4  JID + LOT		// 5  JID + MID		// 6  LOT + JID
	// 7  LOT + MID		// 8  MID + JID		// 9  MID + LOT
	// 10 JID + LOT + MID		// 11 JID + MID + LOT		// 12 LOT + JID + MID
	// 13 LOT + MID + JID		// 14 MID + JID + LOT		// 15 MID + LOT + JID
	//
	EIL_Chk_Local_Time_for_File( Buffer , _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() / 16 );
	//
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() % 16 ) {
	case 1 : case 4 : case 5 : case 10 : case 11 :
		if ( strlen( jobid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , jobid );
		}
		break;
	case 2 : case 6 : case 7 : case 12 : case 13 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 3 : case 8 : case 9 : case 14 : case 15 :
		if ( strlen( midid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , midid );
		}
		break;
	}
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() ) {
	case 6 : case 8 : case 12 : case 14 :
		if ( strlen( jobid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , jobid );
		}
		break;
	case 4 : case 9 : case 10 : case 15 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 5 : case 7 : case 11 : case 13 :
		if ( strlen( midid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , midid );
		}
		break;
	}
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() ) {
	case 13 : case 15 :
		if ( strlen( jobid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , jobid );
		}
		break;
	case 11 : case 14 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 10 : case 12 :
		if ( strlen( midid ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , midid );
		}
		break;
	}
	//
	j = strlen( Buffer );
	for ( i = 0 ; i < j ; i++ ) {
		if      ( Buffer[i] == '*'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '|'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '"'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '?'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '<'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '>'  ) Buffer[i] = '-';
		else if ( Buffer[i] == ','  ) Buffer[i] = '-';
		else if ( Buffer[i] == ';'  ) Buffer[i] = '-';
		else if ( Buffer[i] == ':'  ) Buffer[i] = '-';
		else if ( Buffer[i] == '\'' ) Buffer[i] = '-';
	}
}


//int EIL_MAKE_SLOT_FOLDER( int side , int pt , int slot , char *wid , BOOL make ) { // 2011.05.13 // 2015.09.01
int EIL_MAKE_SLOT_FOLDER( int side , int pt , int slot , char *wid , char *recipefile , char *jobid , BOOL make ) { // 2011.05.13 // 2015.09.01
	char Buffer[256];
	//
	if ( make ) {
		//
		while( TRUE ) {
			//
//			EIL_Chk_Local_Time_for_File( Buffer , _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() / 16 ); // 2015.09.01
			//
			EIL_SCHEDULER_LOG_FOLDER( Buffer , recipefile , jobid , wid ); // 2015.09.01
			//
			if ( !STRCMP_L( Buffer , _EIL_METHOD_LAST_FOLDER ) ) break;
			//
//			Sleep(100); // 2012.02.21
			Sleep(1); // 2012.02.21
			//
		}
		//
		EIL_LOT_TIMER_MAKE_SLOT_FOLDER( side , slot , Buffer , wid );
		//
		STR_MEM_MAKE_COPY2( &(_EIL_METHOD_FOLDER[side][pt]) , Buffer );
		//
		strcpy( _EIL_METHOD_LAST_FOLDER , Buffer );
		//
	}
	else {
		EIL_LOT_TIMER_MAKE_SLOT_FOLDER( side , slot , "" , "" );
		//
		STR_MEM_MAKE_COPY2( &(_EIL_METHOD_FOLDER[side][pt]) , "" );
	}
	//
	return 0;
}
//=======================================================================================
char *EIL_Get_SLOT_FOLDER( int side , int pt ) {
	//
	if ( side < 0 ) return STR_MEM_GET_STR( NULL );
	if ( side >= MAX_CASSETTE_SIDE ) return STR_MEM_GET_STR( NULL );
	if ( pt < 0 ) return STR_MEM_GET_STR( NULL );
	if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return STR_MEM_GET_STR( NULL );
	//
	return STR_MEM_GET_STR( _EIL_METHOD_FOLDER[side][pt] );
}
//=======================================================================================
/*
int EIL_Get_One_Blank_Wafer_Slot( int s , BOOL tmside ) { // 2011.05.17
	int p;
	//
	if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) return -1;
	//
	if ( tmside ) {
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
			//
			return _i_SCH_CLUSTER_Get_SlotIn( s , p );
		}
	}
	else {
		for ( p = (MAX_CASSETTE_SLOT_SIZE-1) ; p >= 0 ; p-- ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
			//
			return _i_SCH_CLUSTER_Get_SlotIn( s , p );
		}
	}
	//
	return -1;
}
*/
int EIL_Get_One_Blank_Wafer_Slot( int cs , int *rs , BOOL tmside ) { // 2012.07.20
	int i , p;
	//
	if ( ( cs >= 0 ) && ( cs < MAX_CASSETTE_SIDE ) ) {
		if ( _i_SCH_SYSTEM_USING_RUNNING( cs ) ) {
			//
			*rs = cs;
			//
			if ( tmside ) {
				for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( cs , p ) < 0 ) continue;
					//
					return _i_SCH_CLUSTER_Get_SlotIn( cs , p );
				}
			}
			else {
				for ( p = (MAX_CASSETTE_SLOT_SIZE-1) ; p >= 0 ; p-- ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( cs , p ) < 0 ) continue;
					//
					return _i_SCH_CLUSTER_Get_SlotIn( cs , p );
				}
			}
			//
			return -1;
			//
		}
	}
	//
	*rs = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		*rs = i;
		//
		if ( tmside ) {
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , p ) < 0 ) continue;
				//
				return _i_SCH_CLUSTER_Get_SlotIn( i , p );
			}
		}
		else {
			for ( p = (MAX_CASSETTE_SLOT_SIZE-1) ; p >= 0 ; p-- ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , p ) < 0 ) continue;
				//
				return _i_SCH_CLUSTER_Get_SlotIn( i , p );
			}
		}
		//
	}
	//
	return -1;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL EIL_BM_IS_INVALID( int ch ) {
	if ( ( ch >= BM1 ) && ( ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ch == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE;
		if ( ch == _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE;
		//
		if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE ) return TRUE;
		if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE2 ) return TRUE;
	}
	return FALSE;
}

void EIL_CHANGE_STATUS_TO( int bm , int slot , int data ) {
	int i , ss , es , s , p , id;
	//
	if ( slot == -1 ) {
		ss = 1;
		es = _i_SCH_PRM_GET_MODULE_SIZE( bm );
	}
	else {
		ss = slot;
		es = slot;
	}
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( data == -1 ) {
			//
			if ( _i_SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , 0 , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
				//
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , bm ) == BUFFER_OUT_MODE ) {
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_X_REMOVED , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
				}
				else {
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_X_NEED_SUPPLY , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
				}
			}
			else {
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( bm , i );
				p = _i_SCH_MODULE_Get_BM_POINTER( bm , i );
				//
				id = _i_SCH_CLUSTER_Get_Ex_EIL_UniqueID( s , p );
				//
				if ( _i_SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
					if ( _i_SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RETURN_REQUEST ) {
						IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , id , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
						IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_O_NEED_REMOVE_RTRQ , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
					}
					else {
						IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , id , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
						IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_O_NEED_REMOVE_INT , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
					}
				}
				else if ( _i_SCH_CLUSTER_Get_PathDo( s , p ) == 0 ) {
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , id , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_O_REMAIN , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
				}
				else {
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , id , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
					IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , _EIL_STS_O_NEED_REMOVE , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
				}
				//
			}
		}
		else {
			if ( _i_SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				id = 0;
			}
			else {
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( bm , i );
				p = _i_SCH_MODULE_Get_BM_POINTER( bm , i );
				//
				id = _i_SCH_CLUSTER_Get_Ex_EIL_UniqueID( s , p );
			}
			IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_UNIQUEID[bm-BM1][i-1]) , id , "CTC.BM%d_EIL_UNIQUEID%d" , bm-BM1 + 1 , i );
			IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_STATUS[bm-BM1][i-1]) , data , "CTC.BM%d_EIL_STATUS%d" , bm-BM1 + 1 , i );
		}
	}
	//
}

int EIL_BM_USE_POSSIBLE( int bm , int prcchk ) {
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( bm ) ) return 1;
	if ( EIL_BM_IS_INVALID( bm ) ) return 2;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) return 3;
	if ( _i_SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_FM_STATION ) return 4;
	if      ( prcchk == 1 ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) != 0 ) return 5;
	}
	else if ( prcchk == 2 ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) < 0 ) return 5;
	}
	return 0;
}

int EIL_BM_TMSIDE_STATUS( int bm ) {
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( bm ) ) return 1;
	if ( EIL_BM_IS_INVALID( bm ) ) return 2;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) return 3;
	if ( _i_SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_TM_STATION ) return 4;
	return 0;
}

int EIL_BM_WAIT_STATUS( int bm ) {
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( bm ) ) return 1;
	if ( EIL_BM_IS_INVALID( bm ) ) return 2;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) return 3;
	if ( _i_SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_WAIT_STATION ) return 4;
	return 0;
}

int EIL_BM_DISABLE_STATUS( int bm ) {
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( bm ) ) return 1;
	if ( EIL_BM_IS_INVALID( bm ) ) return 2;
	if ( _i_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) return 3;
	return 0;
}

int EIL_SIDE_HAS_DATA( int side , BOOL datacheck ) {
	int i;
	//
	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) return 1;
	//
	if ( datacheck ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) return 2;
			//
		}
	}
	//
	return 0;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int _EIL_COMMON_RUN_CONTROL_FEM( int side ) {
	int i , Res;
	//
	//==============================================================================================================================================
	//==============================================================================================================================================
	//==============================================================================================================================================
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !BM_EIL_AUTO_TMSIDE[i-BM1] ) continue;
		//
		switch( EIL_BM_USE_POSSIBLE( i , 1 ) ) {
		case 0 :
			//
			BM_EIL_AUTO_TMSIDE[i-BM1] = FALSE;
			//
			break;
		case 5 :
			continue;
			break;
		default :
			BM_EIL_AUTO_TMSIDE[i-BM1] = FALSE;
			continue;
			break;
		}
		//
		Res = _i_EIL_OPERATE_REQUEST_TM_TO_EIL( i );
		//==========================================================================================
		if ( Res != 0 ) _EIL_LOG_LOT_PRINTF( side , 0 , "FM Handling Auto TMSIDE %s%cWHTMSIDE %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
		//==========================================================================================
		//
	}
	//==============================================================================================================================================
	//==============================================================================================================================================
	//==============================================================================================================================================
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		Res = EIL_BM_WAIT_STATUS( i );
		//
		if ( Res == 0 ) {
			//
			EIL_CHANGE_STATUS_TO( i , -1 , _EIL_STS_UNKNOWN );
			//
			IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , _EIL_MODE_WAIT , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 );
			//
			continue;
		}
		//
		Res = EIL_BM_TMSIDE_STATUS( i );
		//
		if ( Res == 0 ) {
			//
			EIL_CHANGE_STATUS_TO( i , -1 , _EIL_STS_EQUIP );
			//
			if ( IO_ADD_READ_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 ) != _EIL_MODE_EQUIP ) {
				IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , _EIL_MODE_EQUIP_MFI , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 );
			}
			//
			continue;
		}
		//
		Res = EIL_BM_DISABLE_STATUS( i );
		//
		if ( Res == 0 ) {
			//
			EIL_CHANGE_STATUS_TO( i , -1 , _EIL_STS_MAINT );
			//
			IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , _EIL_MODE_DISABLE , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 );
			//
			continue;
		}
		//
		Res = EIL_BM_USE_POSSIBLE( i , 0 );
		//
		if ( Res != 0 ) continue;
		//
		if ( BM_EIL_OPERATE_SIGNAL[i-BM1] == 2 ) continue;
		//
		BM_EIL_OPERATE_SIGNAL[i-BM1] = 1;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) != 0 ) {
			//
			EIL_CHANGE_STATUS_TO( i , -1 , -1 );
			//
			IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , _EIL_MODE_GOTO_FEM , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 );
			//
			continue;
		}
		//
		BM_EIL_OPERATE_SIGNAL[i-BM1] = 2;
		//
		EIL_CHANGE_STATUS_TO( i , -1 , -1 );
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[i-BM1]) , _EIL_MODE_FEM , "CTC.BM%d_EIL_MODE" , i - BM1 + 1 );
		//
	}
	//
	return 1;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_EIL_SIDE_LOTPRE_RUN( int CHECKING_SIDE , int ch , BOOL , int pointer , char * );


BOOL EIL_MODULE_STATUS_PM_SET( BOOL insert , int side , int pointer , char *ppid ) {
	int i , j , s , p , r , m , cs;
	int p_s[MAX_PM_CHAMBER_DEPTH];
	char buffer[256];

//	int ppc;
//	int p_d[MAX_PM_CHAMBER_DEPTH];
	//
/*
	ppc = FALSE;
	//
	for ( p = 0 ; p < MAX_PM_CHAMBER_DEPTH ; p++ ) p_d[p] = 0;
	//
	if ( insert ) {
		//
		if ( !STRCMP_L( _EIL_METHOD_LAST_PPID , "" ) ) {
			if ( !STRCMP_L( _EIL_METHOD_LAST_PPID , ppid ) ) {
				ppc = TRUE;
			}
		}
		//
		strcpy( _EIL_METHOD_LAST_PPID , ppid );
	}
*/
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		//
		cs = 0; // 2011.09.10
		//
		for ( p = 0 ; p < MAX_PM_CHAMBER_DEPTH ; p++ ) p_s[p] = 0;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			r = _i_SCH_CLUSTER_Get_PathRange( s , p );
			//
			if ( r < 0 ) continue;
			//
			for ( i = 0 ; i < r ; i++ ) {
				//
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					//
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , p , i , j );
					//
					if ( m < 0 ) m = -m;
					//
					if ( ( m >= PM1 ) && ( m < AL ) ) p_s[m-PM1]++;
					//
				}
			}
		}
		//--------------------------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------------------------
		for ( p = PM1 ; p < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; p++ ) {
			//
			switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , p ) ) {
			case MUF_01_USE :
			case MUF_81_USE_to_PREND_RANDONLY :
			case MUF_97_USE_to_SEQDIS_RAND :
			case MUF_98_USE_to_DISABLE_RAND :
			case MUF_99_USE_to_DISABLE :
				//
				if ( p_s[p-PM1] > 0 ) {
					//
//					if ( _i_SCH_MODULE_GET_USE_ENABLE( p , FALSE , -1 ) ) { // 2018.12.20
					if ( _i_SCH_MODULE_GET_USE_ENABLE( p , FALSE , -2 ) ) { // 2018.12.20
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , p , MUF_01_USE );
						//
						cs++; // 2011.09.10
						//
					}
					else {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , p , MUF_99_USE_to_DISABLE );
					}
					//
				}
				else {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( s , p , MUF_02_USE_to_END );
					_i_SCH_MODULE_Dec_Mdl_Run_Flag( p );
					//
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( p ) < 0 ) { // 2011.11.17
						_i_SCH_LOG_LOT_PRINTF( s , "Module %s Run Flag Status Error EIL-1%cMDLSTSERROR EIL-1:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( p ) , 9 , p );
						_i_SCH_MODULE_Set_Mdl_Run_Flag( p , 0 );
						//
						sprintf( buffer , "Module %s Run Flag Status Error EIL-1[%d]" , _i_SCH_SYSTEM_GET_MODULE_NAME( p ) , s );
						SCHMULTI_SET_LOG_MESSAGE( buffer );
						//
					}
					//
				}
				//
				break;
			case MUF_00_NOTUSE :
			case MUF_02_USE_to_END :
			case MUF_03_USE_to_END_DISABLE :
			case MUF_04_USE_to_PREND_EF_LP :
			case MUF_05_USE_to_PRENDNA_EF_LP :
			case MUF_07_USE_to_PREND_DECTAG :
			case MUF_08_USE_to_PRENDNA_DECTAG :
			case MUF_31_USE_to_NOTUSE_SEQMODE :
			case MUF_71_USE_to_PREND_EF_XLP :
				//
				if ( p_s[p-PM1] > 0 ) {
					//
//					if ( _i_SCH_MODULE_GET_USE_ENABLE( p , FALSE , -1 ) ) { // 2018.12.20
					if ( _i_SCH_MODULE_GET_USE_ENABLE( p , FALSE , -2 ) ) { // 2018.12.20
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , p , MUF_01_USE );
						//
						cs++; // 2011.09.10
						//
					}
					else {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , p , MUF_99_USE_to_DISABLE );
					}
					//
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( p );
					//
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( p ) == 1 ) { // 2011.07.11
						//
//						if ( s == side ) p_d[p-PM1] = 1;
						//
//						if ( _i_EIL_SIDE_LOTPRE_RUN( s , p , FALSE ) != 0 ) return FALSE; // 2011.07.18
						//
						if ( s == side ) {
							//
							if ( _i_EIL_SIDE_LOTPRE_RUN( side , p , FALSE , pointer , NULL ) != 0 ) return FALSE; // 2011.07.18
							//
						}
						//
					}
					//
				}
				//
				break;
			}
		}
		//
		// 2011.09.10
		SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , ( cs <= 0 ) ? 1 : cs );
		//
	}
	//
	/*
	if ( ppc ) { // 2011.07.15
		//
		for ( p = PM1 ; p < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; p++ ) {
			//
			if ( p_d[p-PM1] == 1 ) continue;
			//
			switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , p ) ) {
			case MUF_01_USE :
			case MUF_81_USE_to_PREND_RANDONLY :
			case MUF_97_USE_to_SEQDIS_RAND :
			case MUF_98_USE_to_DISABLE_RAND :
			case MUF_99_USE_to_DISABLE :
				//
				if ( _i_EIL_SIDE_LOTPRE_RUN( side , pointer , p , TRUE ) != 0 ) return FALSE;
				//
				break;
			}
		}
		//
	}
	*/
	//
	return TRUE;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


int EIL_METHOD_GET_INSERT_POS( int *side , int *pt , int *lusp , int incm , int wafer ) {
	//
	int s , l , i;
	int lsupid , lsups;
	//
	lsupid = -1;
	lsups = 0;
	//
	for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( l ) ) continue;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( l , i ) >= 0 ) {
				//
				if ( lsupid == -1 ) {
					lsupid = _i_SCH_CLUSTER_Get_SupplyID( l , i );
					lsups  = l;
				}
				else {
					if ( _i_SCH_CLUSTER_Get_SupplyID( l , i ) > lsupid ) {
						lsupid = _i_SCH_CLUSTER_Get_SupplyID( l , i );
						lsups  = l;
					}
				}
				//
				if ( incm >= CM1 ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( l , i ) == incm ) {
						if ( _i_SCH_CLUSTER_Get_SlotIn( l , i ) == wafer ) {
							return 1;
						}
					}
				}
				//
			}
		}
	}
	//
	*lusp = lsupid;
	//
	if ( ( (incm-1) >= 0 ) && ( (incm-1) < MAX_CASSETTE_SIDE ) ) {
		//
		if ( _i_SCH_SYSTEM_USING_RUNNING( incm-1 ) ) {
			//
			for ( i = ( MAX_CASSETTE_SLOT_SIZE - 1 ) ; i >= 0 ; i-- ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( incm-1 , i ) >= 0 ) break;
			}
			//
			i++;
			//
			if ( i < MAX_CASSETTE_SLOT_SIZE ) {
				//
				*side = incm-1;
				*pt = i;
				//
				return 0;
			}
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2010.10.25
				if ( _i_SCH_CLUSTER_Get_PathRange( incm-1 , i ) < 0 ) {
					//
					*side = incm-1;
					*pt = i;
					//
					return 0;
					//
				}
			}
			//
		}
	}
	//
	for ( s = 0 ; s < 4 ; s++ ) {
		//
		for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( l ) ) continue;
			//
//			if ( s == 0 ) {
			if ( ( s % 2 ) == 0 ) { // 2010.10.25
				if ( l != lsups ) continue;
			}
			else {
				if ( l == lsups ) continue;
			}
			//
			if ( s <= 1 ) {
				for ( i = ( MAX_CASSETTE_SLOT_SIZE - 1 ) ; i >= 0 ; i-- ) {
					if ( _i_SCH_CLUSTER_Get_PathRange( l , i ) >= 0 ) break;
				}
				//
				i++;
				//
				if ( i < MAX_CASSETTE_SLOT_SIZE ) {
					//
					*side = l;
					*pt = i;
					//
					return 0;
				}
			}
			else {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					if ( _i_SCH_CLUSTER_Get_PathRange( l , i ) < 0 ) {
						//
						*side = l;
						*pt = i;
						//
						return 0;
					}
				}
			}
			//
		}
		//
	}
	//
	return 2;
}


int EIL_MAKE_DATABASE( int side , int pt , int lastsp , int cm , int wafer ) {
	//=============================================
	_i_SCH_CLUSTER_Set_SupplyID( side , pt , lastsp + 10 );
	//=============================================
	return 0;
}

BOOL EIL_PRE_SETTING( int side , int pt , int bm , int slot , int incm0 , int wafer , char *clusterRecipefile , char *feedbackmsg , char *wid_control , char *ppid_control ) { // 2010.10.05
	//
	HFILE hFile;
	//
	int retindexs[256] , retindexsz[256];
	char Buffer[ 1024 + 1 ];
	int  ReadCnt , Line;
	BOOL FileEnd = TRUE;
	//
	char chr_return[ 256 + 1 ];
	char chr_return2[ 256 + 1 ];
	//
	char cj_control[ 256 + 1 ];
	char pj_control[ 256 + 1 ];
	//
	char pr_control1[ 256 + 1 ];
	char pr_control2[ 256 + 1 ];
	char pr_control3[ 256 + 1 ];

	int i , l , k , m;
	//
	/*
	// 2012.09.13
	if ( hFile == -1 ) {
		strcpy( ppid_control , "" );
		sprintf( wid_control , "CM%d_%d" , _i_SCH_CLUSTER_Get_PathIn( side , pt ) - CM1 + 1 , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) );
		return TRUE;
	}
	//
	strcpy( cj_control , "" );
	strcpy( pj_control , "" );
	strcpy( wid_control , "" );
	*/

	//==========================================================================================
	_i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( side , pt , 0 ); // 2012.09.13
	//==========================================================================================

	strcpy( cj_control , "" );
	strcpy( pj_control , "" );
	strcpy( ppid_control , "" );
	strcpy( wid_control , "" );
	//
	if ( feedbackmsg[0] == '>' ) {
		//
		STR_SEPERATE_CHAR5( feedbackmsg + 1 , '|' , chr_return , cj_control , pj_control , ppid_control , wid_control , 256 );
		//
		i = atoi( chr_return );
		//
		if ( i < 0 ) i = 0;
		//
		//==========================================================================================
		_i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( side , pt , i );
		//==========================================================================================
	}
	else {
		hFile = _lopen( feedbackmsg , OF_READ );
		//
		if ( hFile != -1 ) {
			//
			for ( Line = 1 ; FileEnd ; Line++ ) {
				//
				FileEnd = H_Get_Line_String_From_File2_Include_Index( hFile , Buffer , 1024 , &ReadCnt , retindexs , retindexsz );
				if ( ReadCnt >= 2 ) {
					//============================================================================================
					if ( !Get_Data_From_String_with_index( Buffer , chr_return , retindexs[0] , retindexsz[0] ) ) {
						_lclose( hFile );
						return TRUE;
					}
					//============================================================================================
					UTIL_Extract_Reset_String( chr_return );
					//============================================================================================
					if ( !Get_Data_From_String_with_index( Buffer , chr_return2 , retindexs[1] , retindexsz[1] ) ) {
						_lclose( hFile );
						return TRUE;
					}
					//============================================================================================
					UTIL_Extract_Reset_String( chr_return2 );
					//============================================================================================
					if      ( STRCMP_L( chr_return , "CONTROLJOB" ) ) {
						strcpy( cj_control , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "PROCESSJOB" ) ) {
						strcpy( pj_control , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "PPID" ) ) {
						strcpy( ppid_control , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "RECIPENAME" ) ) {
						strcpy( ppid_control , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "WAFERID" ) ) {
						strcpy( wid_control , chr_return2 );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "PATHRANGE" ) ) {
						_i_SCH_CLUSTER_Set_PathRange( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "PATHDATA" ) ) { // PATHDATA 1  PM1 test test2 test3
						if ( ReadCnt >= 4 ) {
							//
							l = atoi( chr_return2 ) - 1;
							//
							if ( ( l >= 0 ) && ( l < MAX_CLUSTER_DEPTH ) ) {
								//
								strcpy( pr_control1 , "" );
								strcpy( pr_control2 , "" );
								strcpy( pr_control3 , "" );
								//
								for ( i = 2 ; ( ( i < ReadCnt ) && ( i < 6 ) ) ; i++ ) {
									if ( !Get_Data_From_String_with_index( Buffer , chr_return2 , retindexs[i] , retindexsz[i] ) ) {
										_lclose( hFile );
										return TRUE;
									}
									//
									if      ( i == 2 ) {
										k = Get_Module_ID_From_String( 0 , chr_return2 , 0 );
									}
									else if ( i == 3 ) {
										UTIL_Extract_Reset_String( chr_return2 );
										strcpy( pr_control1 , chr_return2 );
									}
									else if ( i == 4 ) {
										UTIL_Extract_Reset_String( chr_return2 );
										strcpy( pr_control2 , chr_return2 );
									}
									else if ( i == 5 ) {
										UTIL_Extract_Reset_String( chr_return2 );
										strcpy( pr_control3 , chr_return2 );
									}
								}
								//
								if ( _i_SCH_PRM_GET_MODULE_MODE( k ) ) {
									if ( _i_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) {
										_i_SCH_CLUSTER_Set_PathProcessData2( side , pt , l , k , pr_control1 , pr_control2 , pr_control3 );
									}
									else {
										_i_SCH_CLUSTER_Set_PathProcessData2( side , pt , l , -k , pr_control1 , pr_control2 , pr_control3 );
									}
								}
								//
							}
						}
						//
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "PATHREMOVE" ) || STRCMP_L( chr_return , "PATHDISABLE" ) ) { // PATHREMOVE 1  PM1
						//
						if ( STRCMP_L( chr_return , "PATHREMOVE" ) ) i = 0; else i = 1;
						//
						if ( ReadCnt >= 3 ) {
							//
							l = atoi( chr_return2 ) - 1;
							//
							if ( ( l >= 0 ) && ( l < MAX_CLUSTER_DEPTH ) ) {
								//
								if ( !Get_Data_From_String_with_index( Buffer , chr_return2 , retindexs[2] , retindexsz[2] ) ) {
									_lclose( hFile );
									return TRUE;
								}
								//
								m = Get_Module_ID_From_String( 0 , chr_return2 , 0 );
								//
								if ( _i_SCH_PRM_GET_MODULE_MODE( m ) ) {
									//
									for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
										//
										if ( m == _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , l , k ) ) {
											//
											if ( i == 0 ) {
												_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pt , l , k );
											}
											else {
												_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , l , k );
											}
											break;
										}
										//
									}
								}
								//
							}
						}
						//
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "PATHIN" ) ) {
						i = Get_Module_ID_From_String( 0 , chr_return2 , 0 );
						//
						_i_SCH_CLUSTER_Set_PathIn( side , pt , i );
						_i_SCH_CLUSTER_Set_PathOut( side , pt , i );
					}
					else if ( STRCMP_L( chr_return , "PATHOUT" ) ) {
						i = Get_Module_ID_From_String( 0 , chr_return2 , 0 );
						//
						_i_SCH_CLUSTER_Set_PathOut( side , pt , i );
					}
					else if ( STRCMP_L( chr_return , "SLOTIN" ) ) {
						_i_SCH_CLUSTER_Set_SlotIn( side , pt , atoi( chr_return2 ) );
						_i_SCH_CLUSTER_Set_SlotOut( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "SLOTOUT" ) ) {
						_i_SCH_CLUSTER_Set_SlotOut( side , pt , atoi( chr_return2 ) );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "PATHHSIDE" ) ) {
						_i_SCH_CLUSTER_Set_PathHSide( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "SWITCHINOUT" ) ) {
						_i_SCH_CLUSTER_Set_SwitchInOut( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "FAILOUT" ) ) {
						_i_SCH_CLUSTER_Set_FailOut( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "BUFFER" ) ) {
						_i_SCH_CLUSTER_Set_Buffer( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "STOCK" ) ) {
						_i_SCH_CLUSTER_Set_Stock( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "OPTIMIZE" ) ) {
						_i_SCH_CLUSTER_Set_Optimize( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "EXTRA" ) ) {
						_i_SCH_CLUSTER_Set_Extra( side , pt , atoi( chr_return2 ) );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "LOTSPECIAL" ) ) {
						_i_SCH_CLUSTER_Set_LotSpecial( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "LOTUSERSPECIAL" ) ) {
						_i_SCH_CLUSTER_Set_LotUserSpecial( side , pt , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "CLUSTERSPECIAL" ) ) { // 2014.06.23
						_i_SCH_CLUSTER_Set_ClusterSpecial( side , pt , atoi( chr_return2 ) );
					}
					else if ( STRCMP_L( chr_return , "CLUSTERUSERSPECIAL" ) ) {
						_i_SCH_CLUSTER_Set_ClusterUserSpecial( side , pt , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "CLUSTERTUNEDATA" ) ) {
						_i_SCH_CLUSTER_Set_ClusterTuneData( side , pt , chr_return2 );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "USERAREA1" ) ) {
						_i_SCH_CLUSTER_Set_UserArea( side , pt , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "USERAREA2" ) ) {
						_i_SCH_CLUSTER_Set_UserArea2( side , pt , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "USERAREA3" ) ) {
						_i_SCH_CLUSTER_Set_UserArea3( side , pt , chr_return2 );
					}
					else if ( STRCMP_L( chr_return , "USERAREA4" ) ) {
						_i_SCH_CLUSTER_Set_UserArea4( side , pt , chr_return2 );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "PARALLEL" ) ) {
						_i_SCH_CLUSTER_Set_PathProcessParallel( side , pt , -1 , chr_return2 );
					}
					//============================================================================================
					else if ( STRCMP_L( chr_return , "DEPTH" ) ) {
						_i_SCH_CLUSTER_Set_PathProcessDepth( side , pt , -1 , chr_return2 );
					}
					//============================================================================================
					else {
						continue;
					}
					//============================================================================================
				}
			}
			//
			_lclose( hFile );
		}
	}
	//
	if ( wid_control[0] == 0 ) sprintf( wid_control , "CM%d_%d" , _i_SCH_CLUSTER_Get_PathIn( side , pt ) - CM1 + 1 , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) );
	//
//	if ( ( cj_control[0] != 0 ) || ( pj_control[0] != 0 ) || ( ppid_control[0] != 0 ) || ( wid_control[0] != 0 ) ) { // 2012.09.13
	if ( ( cj_control[0] != 0 ) || ( pj_control[0] != 0 ) || ( ppid_control[0] != 0 ) ) { // 2012.09.13
		//
		sprintf( chr_return , "%s|%s|%s|%s" , cj_control , pj_control , ppid_control , wid_control );
		//
		k = -1;
		//
		for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( l ) ) continue;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( l , i ) >= 0 ) {
					//
					if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( l , i ) != -99 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( l , i ) != NULL ) {
						//
						if ( STRCMP_L( _i_SCH_CLUSTER_Get_Ex_EILInfo( l , i ) , chr_return ) ) {
							//
							k = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( l , i );
							//
							break;
						}
						//
					}
				}
				//
			}
			//
			if ( k != -1 ) break;
			//
		}
		//
		if ( k == -1 ) k = 0;
		//
		_i_SCH_CLUSTER_Set_Ex_EILInfo( side , pt , chr_return );
		//
		_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , pt , -99 );
		_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , pt , k );
		//
	}
	else { // 2011.09.21
		//
		_i_SCH_CLUSTER_Set_Ex_EILInfo( side , pt , NULL );
		//
		_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , pt , -1 );
		_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , pt , -1 );
	}
	//-----------------------------------------------------------------------
	_i_SCH_CLUSTER_Set_Ex_WaferID( side , pt , wid_control ); // 2012.09.13
	//-----------------------------------------------------------------------
	_i_SCH_CLUSTER_Set_Ex_UserControl_Mode( side , pt , 0 ); // 2011.12.08
	_i_SCH_CLUSTER_Set_Ex_UserControl_Data( side , pt , NULL ); // 2011.12.08
	//-----------------------------------------------------------------------
	_i_SCH_CLUSTER_Set_Ex_DisableSkip( side , pt , 0 ); // 2012.04.14
	//-----------------------------------------------------------------------
	_i_SCH_CLUSTER_Set_Ex_UserDummy( side , pt , 0 ); // 2015.10.12
	//-----------------------------------------------------------------------
	return TRUE;
}


void Scheduler_Regist_Lot_PrePost_Recipe( int side );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int  xinside_global_Run_Tag[MAX_CASSETTE_SIDE][MAX_CHAMBER];
extern int  xinside_global_Run2_Tag[MAX_CASSETTE_SIDE][MAX_CHAMBER];
extern int  xinside_global_PrcsRunCheck[MAX_CASSETTE_SIDE][MAX_CHAMBER];

//------------------------------------------------------------------------------------------
BOOL Scheduler_LotPre_Finished_Check_Part( int CHECKING_SIDE , int mode , char *errorstring );
//void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch ); // 2018.06.22
void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch , char *recipe , int mode ); // 2018.06.22
int _i_SCH_MODULE_GET_USE_ENABLE_Sub( int Chamber , BOOL Always , int side );
//BOOL Scheduler_LotPre_Check_Start_Part( int CHECKING_SIDE , int ch , int firstNormalmode , BOOL *maintag , BOOL *mintag , BOOL *plustag , BOOL *chinctag , int ); // 2015.05.27
BOOL Scheduler_LotPre_Check_Start_Part( int CHECKING_SIDE , int ch , BOOL otherlotcheck , int firstNormalmode , BOOL *maintag , BOOL *mintag , BOOL *plustag , BOOL *chinctag , int ); // 2015.05.27
void Scheduler_LotPost_Check_Start_Part( int CHECKING_SIDE , int ch , BOOL premode , BOOL *chinctag );
//------------------------------------------------------------------------------------------

void _EIL_LOG_LOT_PRINTF( int M , int Slot , LPSTR list , ... ) {
	//
	va_list va;
	char TextBuffer[512+1];
	//
	va_start( va , list );
	_vsnprintf( TextBuffer , 512 , list , (LPSTR) va );
	va_end( va );
	//
	if ( Slot <= 0 ) _i_SCH_LOG_LOT_PRINTF( M , "%s" , TextBuffer );
	else             _i_SCH_LOG_LOT_PRINTF( M , "%s\t%d" , TextBuffer , Slot );
	//
}


int _i_EIL_SIDE_LOTPOST_RUN( int CHECKING_SIDE ) {
	int i , inctag;
	int Run[MAX_CASSETTE_SIDE][MAX_CHAMBER];
	char buffer[256];
	//--------------------------------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		Run[CHECKING_SIDE][i] = 0;
		//
		switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) ) {
		case MUF_01_USE :
		case MUF_81_USE_to_PREND_RANDONLY :
		case MUF_97_USE_to_SEQDIS_RAND :
		case MUF_98_USE_to_DISABLE_RAND :
		case MUF_99_USE_to_DISABLE :
			//
			_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END );
			_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
			//
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) { // 2011.11.17
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error EIL-1%cMDLSTSERROR EIL-1:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
				_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
				//
				sprintf( buffer , "Module %s Run Flag Status Error EIL-2[%d]" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , CHECKING_SIDE );
				SCHMULTI_SET_LOG_MESSAGE( buffer );
				//
			}
			//
			Run[CHECKING_SIDE][i] = 1;
			//
			break;
		case MUF_00_NOTUSE :
			//
			break;
		case MUF_02_USE_to_END :
		case MUF_03_USE_to_END_DISABLE :
		case MUF_04_USE_to_PREND_EF_LP :
		case MUF_05_USE_to_PRENDNA_EF_LP :
		case MUF_07_USE_to_PREND_DECTAG :
		case MUF_08_USE_to_PRENDNA_DECTAG :
		case MUF_31_USE_to_NOTUSE_SEQMODE :
		case MUF_71_USE_to_PREND_EF_XLP :
			//
			Run[CHECKING_SIDE][i] = 1;
			//
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) { // 2011.05.31
		//-----------------------------
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 0 ) {
			Run[CHECKING_SIDE][i] = 0;
		}
		else { // 2011.07.18
			if ( Run[CHECKING_SIDE][i] != 0 ) {
				PROCESS_MONITOR_Reset_Last_ProcessJob_Data( i );
			}
		}
		//-----------------------------
		//--------------------------------------------------------------------------
		if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) {
			//
			if ( ( Run[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , i ) != 0 ) ) {
				//==============================================================================================================
				Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , i , FALSE , &inctag );
				if ( inctag ) i++;
				//==============================================================================================================
			}
		}
		//--------------------------------------------------------------------------
	}
	//--------------------------------------------------------------------------------------------------------
	return 0;
	//
}

int _i_EIL_SIDE_LOTPRE_RUN( int CHECKING_SIDE , int ch , BOOL middle , int pt , char *ppid ) {
	int i , Res , pms , pme;
	BOOL maintag , mintag , plustag , inctag , nextmode;
	int xinside0_BeforeLotUsed[MAX_CASSETTE_SIDE][MAX_CHAMBER];
	char errorstring[256];

	//================================================================================
	if ( ch == -1 ) {
		pms = PM1;
		pme = ( PM1 + MAX_PM_CHAMBER_DEPTH );
	}
	else {
		pms = ch;
		pme = pms + 1;
	}
	//================================================================================
	for ( i = pms ; i < pme ; i++ ) {
		//==============================================================================================
		if ( ch == -1 ) {
			//==============================================================================================
			xinside_global_Run_Tag[CHECKING_SIDE][i] = 0;
			//==============================================================================================
			xinside0_BeforeLotUsed[CHECKING_SIDE][i] = FALSE;
			//==============================================================================================
			if ( !middle ) PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , TRUE );
			//==============================================================================================
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) continue;
			//==============================================================================================
			Res = _i_SCH_MODULE_Get_Mdl_Run_Flag( i );
			//
			if ( Res == 1 ) xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;
			if ( Res >  1 ) xinside0_BeforeLotUsed[CHECKING_SIDE][i] = TRUE;
			//==============================================================================================
		}
		else {
			//==============================================================================================
			xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;
			//==============================================================================================
			xinside0_BeforeLotUsed[CHECKING_SIDE][i] = FALSE;
			//==============================================================================================
			if ( !middle ) PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , TRUE );
			//==============================================================================================
		}
		//==============================================================================================
		if ( !middle ) {
			if ( !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) _SCH_CLUSTER_Parallel_Check_Curr_DisEna( CHECKING_SIDE , -1 , i , TRUE );
		}
		//==============================================================================================
//		Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i ); // 2018.06.22
		Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i , NULL , 0 ); // 2018.06.22
		//==============================================================================================
	}
	//=============================================================================================================
	if ( ch == -1 ) {
		for ( i = pms ; i < pme ; i++ ) {
			//-----------------------------
			xinside_global_Run2_Tag[CHECKING_SIDE][i] = xinside_global_Run_Tag[CHECKING_SIDE][i];
			//-----------------------------
			xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 0;
			//--------------------------------------------------------------------------
			if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) continue;
			//--------------------------------------------------------------------------
			if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] ) {
				_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 2 );
			}
			else { // first
				switch( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE,i ) ) {
				case 10 :
					_i_SCH_PRM_SET_inside_READY_RECIPE_USE( CHECKING_SIDE , i , 1 );
					break;
				case 11 :
					_i_SCH_PRM_SET_inside_READY_RECIPE_USE( CHECKING_SIDE , i , 3 );
					break;
				}
				//================================================================================
			}
			//=====================================================================================================
			if ( ( xinside_global_Run2_Tag[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) != 0 ) ) {
				//============================================================================================================
//				if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , 2 , &maintag , &mintag , &plustag , &inctag , 111 ) ) { // 2015.05.27
				if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , FALSE , 2 , &maintag , &mintag , &plustag , &inctag , 111 ) ) { // 2015.05.27
					//
					if ( maintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 1;
					else			xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 2;
					//
					if ( mintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i-1] = 1;
					if ( plustag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i+1] = 1;
					//
					if ( inctag )	i++;
					//
					if ( pt < 0 ) {
						PROCESS_MONITOR_Set_Last_ProcessJob_PPID( i , CHECKING_SIDE , ppid ); // 2011.07.18
					}
					else {
						PROCESS_MONITOR_Set_Last_ProcessJob_Data( i , CHECKING_SIDE , pt ); // 2011.07.18
					}
					//
				}
				//============================================================================================================
			}
		}
		//================================================================================
		if ( !Scheduler_LotPre_Finished_Check_Part( CHECKING_SIDE , 0 , errorstring ) ) return 1;
		//================================================================================
		for ( i = pms ; i < pme ; i++ ) {
			//
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) == 2 ) {
				//================================================================================
				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 12 ) { // 2017.09.15
					//
				}
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 10 ) {
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 1 ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 201 ); // 2017.09.22
				}
				//================================================================================
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 5 ) {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 1 ); // 2017.09.22
						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 202 ); // 2017.09.22
					}
					else {
						_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , i );
					}
				}
				//================================================================================
				else {
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 0 ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 91 ); // 2017.09.22
				}
				//================================================================================
			}
		}
		//================================================================================
		//
		nextmode = 0;
		//
	}
	else { // 2011.07.11
		//-----------------------------
		xinside_global_PrcsRunCheck[CHECKING_SIDE][ch] = 0;
		//--------------------------------------------------------------------------
//		_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 1 ); // 2017.09.22
		_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 203 ); // 2017.09.22
		//
		if ( !middle ) nextmode = 0;
		else           nextmode = 3;
		//
	}
	//
	for ( i = pms ; i < pme ; i++ ) {
		//
		if ( xinside_global_PrcsRunCheck[CHECKING_SIDE][i] == 0 ) {
			//--------------------------------------------------------------------------
			if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) {
			//--------------------------------------------------------------------------
//				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) == 1 ) { // 2017.09.22
				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) >= 100 ) { // 2017.09.22
					//============================================================================================================
//					if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , nextmode , &maintag , &mintag , &plustag , &inctag , 112 ) ) { // 2015.05.27
					if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , FALSE , nextmode , &maintag , &mintag , &plustag , &inctag , 112 ) ) { // 2015.05.27
						if ( maintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 1;
						else			xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 2;
						if ( mintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i-1]	= 1;
						if ( plustag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i+1]	= 1;
						if ( inctag )	i++;
						//
						if ( pt < 0 ) {
							PROCESS_MONITOR_Set_Last_ProcessJob_PPID( i , CHECKING_SIDE , ppid ); // 2011.07.18
						}
						else {
							PROCESS_MONITOR_Set_Last_ProcessJob_Data( i , CHECKING_SIDE , pt ); // 2011.07.18
						}
						//
					}
					//============================================================================================================
				}
			}
		}
	}
	//=============================================================================================================
	if ( ch == -1 ) {
		if ( !Scheduler_LotPre_Finished_Check_Part( CHECKING_SIDE , 1 , errorstring ) ) return 2;
	}
	//=============================================================================================================
	return 0;
	//=============================================================================================================
}


int _i_EIL_SIDE_LOTPREUPDATE( int side ) {
	//
	_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( side , 0 );
	//
	//===============================================================================================
	EnterCriticalSection( &CR_MAIN );
	if ( !USER_RECIPE_ACTION_AFTER_READY( side ) ) {
		LeaveCriticalSection( &CR_MAIN );
		return 1;
	}
	Scheduler_Regist_Lot_PrePost_Recipe( side );
	LeaveCriticalSection( &CR_MAIN );
	//=============================================================================================================
	//
	return 0;
}
//
int _i_EIL_SIDE_BEGIN( int side , char *jobname , char *midname , char *ppid_recipename , char *elsestr ) {
	int old , z , z2 , c_lp;
	char Buffer[256];
	char ppid[256];
	//
	if ( _EIL_SUPPLY_COUNT_INSERT[ side ] > 0 ) {
		//
		old = _i_SCH_IO_GET_MAIN_BUTTON( side );
		//
		if ( old == CTC_IDLE          ) return 11;
		if ( old <  CTC_IDLE          ) return 12;
		if ( old == CTC_DISABLE       ) return 13;
		if ( old == CTC_ABORTING      ) return 14;
		if ( old >= CTC_WAIT_HAND_OFF ) return 15;
		//
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_STOP , PRJOB_RESULT_NORMAL , -1 , -1 , -1 , -1 , "" , "" ); // 2010.11.26
		//
		_i_SCH_LOG_TIMER_PRINTF( -1 , -1 , 0 , -1 , -1 , -1 , -1 , "" , "" );
		//
		_i_SCH_IO_SET_MAIN_BUTTON( side , CTC_DISABLE );
		//
//		_i_EIL_SIDE_LOTPREUPDATE( side ); // 2011.05.03
		//
		Sleep(2000);
		//
	}
	else {
		old = -1;
	}
	//
	EIL_CHECK_ENABLE_CONTROL( -1 , FALSE , 0 ); // 2011.07.25
	//
//	if ( strlen( jobname ) > 0 ) {
		_i_SCH_SYSTEM_SET_JOB_ID( side , jobname );
		IO_SET_JID_NAME( side , jobname );
//	}
//	else {
//	}
	//
//	if ( strlen( midname ) > 0 ) {
		_i_SCH_SYSTEM_SET_MID_ID( side , midname );
		IO_SET_MID_NAME( side , midname );
//	}
//	else {
//	}
	//
//	if ( strlen( ppid_recipename ) > 0 ) {
		_i_SCH_IO_SET_RECIPE_FILE( side , ppid_recipename );
//	}
//	else {
//	}
	//
	while (TRUE) {
		//=======================================================================================================
		if ( SCHEDULER_LOG_FOLDER( side , Buffer , ppid_recipename ) ) break;
		//=======================================================================================================
	}
	//
	_EIL_SUPPLY_COUNT_INSERT[ side ] = 0;
	_EIL_SUPPLY_COUNT_REMOVE[ side ] = 0;
	//
	strcpy( _EIL_METHOD_LAST_PPID , "" ); // 2011.07.15
	//
	_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( side , Buffer );
	IO_LOT_DIR_INITIAL( side , Buffer , _i_SCH_PRM_GET_DFIX_LOG_PATH() );
	//
	_i_SCH_LOG_TIMER_PRINTF( side , TIMER_START , 0 , 0 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Buffer );
	//
	_i_SCH_LOG_TIMER_PRINTF( -1 , -1 , 0 , -1 , -1 , -1 , -1 , "" , "" );
	//
	LOG_MTL_SET_DONE_WFR_COUNT( side , 0 );
	//
	if ( old != -1 ) {
		//
		LOG_MTL_SET_DONE_CARR_COUNT( side ,	LOG_MTL_GET_DONE_CARR_COUNT( side ) + 1 );
		//
		_i_SCH_IO_SET_MAIN_BUTTON( side , old );
	}
	//
	if ( strlen( ppid_recipename ) > 0 ) {
		//
		if ( STRNCMP_L( ppid_recipename , "<MANUAL>" , 8 ) ) {
			//
			strcpy( ppid , ppid_recipename + 8 );
			//
		}
		else {
			//
			strcpy( ppid , ppid_recipename );
			//
		}
		//
	}
	else {
		strcpy( ppid , "" );
	}
	//
	if ( strlen( ppid ) > 0 ) {
		//
		_i_SCH_SUB_RECIPE_READ_CLUSTER( side , -1 , 2 , 0 , 0 , ppid );
		//
	}
	//
	if ( _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( side ) <= 0 ) SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( side , 1 ); // 2011.09.10
	//
	c_lp = 0;
	//
	z = 0;
	//
	while ( TRUE ) { // 2011.05.12
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( elsestr , ':' , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		if ( STRCMP_L( Buffer , "LOTPRE" ) ) {
			//
			if ( c_lp == 0 ) {
				//
				c_lp = 1;
				//
				_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( side , 0 );
				//===============================================================================================
				EnterCriticalSection( &CR_MAIN );
				if ( !USER_RECIPE_ACTION_AFTER_READY( side ) ) {
					LeaveCriticalSection( &CR_MAIN );
					return 21;
				}
				Scheduler_Regist_Lot_PrePost_Recipe( side );
				LeaveCriticalSection( &CR_MAIN );
				//=============================================================================================================
				if ( strlen( ppid ) > 0 ) {
					if ( _i_EIL_SIDE_LOTPRE_RUN( side , -1 , FALSE , -1 , ppid ) != 0 ) return 22;
				}
				//
			}
			//
		}
		//
		z = z2;
	}
	//
	return 0;
}


int _i_EIL_SIDE_END( int side , char *message ) {
	int old , Res;
	//
	if ( _EIL_SUPPLY_COUNT_INSERT[ side ] > 0 ) {
		//
		old = _i_SCH_IO_GET_MAIN_BUTTON( side );
		//
		if ( old == CTC_IDLE          ) return 11;
		if ( old <  CTC_IDLE          ) return 12;
		if ( old == CTC_DISABLE       ) return 13;
		if ( old == CTC_ABORTING      ) return 14;
		if ( old >= CTC_WAIT_HAND_OFF ) return 15;
		//
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_STOP , PRJOB_RESULT_NORMAL , -1 , -1 , -1 , -1 , "" , "" );
		//
		_i_SCH_LOG_TIMER_PRINTF( -1 , -1 , 0 , -1 , -1 , -1 , -1 , "" , "" );
		//
		_i_SCH_IO_SET_MAIN_BUTTON( side , CTC_DISABLE );
		//
		Res = _i_EIL_SIDE_LOTPOST_RUN( side );
		//
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( side , 0 ); // 2011.05.03
		//
		Sleep(2000);
		//
		_EIL_SUPPLY_COUNT_INSERT[ side ] = 0;
		_EIL_SUPPLY_COUNT_REMOVE[ side ] = 0;
		//
		LOG_MTL_SET_DONE_CARR_COUNT( side ,	LOG_MTL_GET_DONE_CARR_COUNT( side ) + 1 );
		//
		LOG_MTL_SET_DONE_WFR_COUNT( side , 0 );
		//
		_i_SCH_IO_SET_MAIN_BUTTON( side , old );
		//
		if ( Res != 0 ) return 21;
		//
	}
	//
	return 0;
}

int _EIL_INSERT_DATA_REMAP( int side , int pt ) {
	int i , k , m;
	//-----------------------------------------------------------------------------------
	if ( _i_SCH_CLUSTER_Get_PathRange( side , pt ) < 0 ) return 1;
	//-----------------------------------------------------------------------------------
	for ( i = 0 ; i < _i_SCH_CLUSTER_Get_PathRange( side , pt ) ; i++ ) {
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , i , k );
			//
			if ( m == 0 ) continue;
			//
			if ( m > 0 ) {
				switch( PM_EIL_ENABLE_CONTROL[m] ) {
				case 1 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , pt , i , k );	break;
				case 2 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pt , i , k );	break;
				}
			}
			else if ( m < 0 ) {
				switch( PM_EIL_ENABLE_CONTROL[-m] ) {
				case 2 :	_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pt , i , k );	break;
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2011.07.28
		//
		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( side , i ) <= 0 ) {
			//
			STR_MEM_MAKE_COPY2( &(_EIL_READY_RECIPE_NAME[ side ][ pt ][ i ]) , "" );
			//
		}
		else {
			//
			STR_MEM_MAKE_COPY2( &(_EIL_READY_RECIPE_NAME[ side ][ pt ][ i ]) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , i ) );
			//
		}
		//
	}
	//-----------------------------------------------------------------------------------
	return 0;
}




void _i_EIL_METHOD_SET_WAFER_NUMBERING( int data ) {
	_EIL_WAFER_NUMBERING = data;
}

int _i_EIL_METHOD_GET_WAFER_NUMBERING( int maxid ) {
	int x , f , l , i , newno;
	//
	newno = _EIL_WAFER_NUMBERING;
	//
	for ( x = 0 ; x <= maxid ; x++ ) {
		//
		newno++;
		if ( newno > maxid ) newno = 1;
		//
		f = FALSE;
		//
		for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( l ) ) continue;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( l , i ) >= 0 ) {
					//
					if ( _i_SCH_CLUSTER_Get_SlotIn( l , i ) == newno ) {
						f = TRUE;
						break;
					}
					//
				}
			}
		}
		//
		if ( !f ) return newno;
	}
	//
	return 0;
	//
}



void _i_EIL_METHOD_INSERT_BLANK( int CHECKING_SIDE , int Pt , int Chamber , int Slot ) {
	//-----------------------------------------------------------------------
	_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE , Pt , 0 );
	//
	_i_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , Pt , SCHEDULER_READY );
	_i_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , Pt , 0 );
	//-----------------------------------------------------------------------
	if ( Chamber >= 0 ) _i_SCH_CLUSTER_Set_PathIn( CHECKING_SIDE , Pt , Chamber );
	if ( Chamber >= 0 ) _i_SCH_CLUSTER_Set_PathOut( CHECKING_SIDE , Pt , Chamber );
	if ( Slot >= 0 ) _i_SCH_CLUSTER_Set_SlotIn( CHECKING_SIDE , Pt , Slot );
	if ( Slot >= 0 ) _i_SCH_CLUSTER_Set_SlotOut( CHECKING_SIDE , Pt , Slot );
	//
	_i_SCH_CLUSTER_Set_Buffer( CHECKING_SIDE , Pt , 0 );
	_i_SCH_CLUSTER_Set_Stock( CHECKING_SIDE , Pt , 0 );
	_i_SCH_CLUSTER_Set_PathHSide( CHECKING_SIDE , Pt , 0 );
	_i_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , Pt , 0 );
	_i_SCH_CLUSTER_Set_FailOut( CHECKING_SIDE , Pt , 0 );
	//
	_i_SCH_CLUSTER_Set_Ex_EILInfo( CHECKING_SIDE , Pt , NULL );
	_i_SCH_CLUSTER_Set_Ex_EIL_UniqueID( CHECKING_SIDE , Pt , 0 );
	//
	_i_SCH_CLUSTER_Set_Ex_PrcsID( CHECKING_SIDE , Pt , 0 ); // 2013.05.06
	//
	_i_SCH_CLUSTER_Set_CPJOB_CONTROL( CHECKING_SIDE , Pt , -1 );
	_i_SCH_CLUSTER_Set_CPJOB_PROCESS( CHECKING_SIDE , Pt , -1 );
	//-----------------------------------------------------------------------
}

int _i_EIL_METHOD_INSERT_TO_EIL( int bm , int slot , int incm0 , int wafer , char *clusterRecipefile , char *informationfile , BOOL prcsnotcheck ) {
	int i , Res;
	int side , pt , lastsp , incm , pos;
	char wid_control[ 256 + 1 ];
	char ppid_control[ 256 + 1 ];
	//==========================================================================================
	if ( ( bm < BM1 ) || ( bm >= TM ) ) return 1;
	if ( ( slot <= 0 ) || ( slot > _i_SCH_PRM_GET_MODULE_SIZE( bm ) ) ) return 2;
	//
	if ( wafer < 0 ) wafer = _i_EIL_METHOD_GET_WAFER_NUMBERING( -wafer );
	//
	if ( ( wafer == 0 ) || ( wafer > MAX_CASSETTE_SLOT_SIZE ) ) return 4;
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[S] BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , bm , slot , incm0 , wafer , clusterRecipefile );
	Res = EIL_BM_USE_POSSIBLE( bm , prcsnotcheck ? 2 : 1 );
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[2] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//
	if ( Res != 0 ) return ( Res + 10 );
	//==========================================================================================
	if ( _i_SCH_MODULE_Get_BM_WAFER( bm , slot ) > 0 ) return 5;
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[3] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	Res = EIL_METHOD_GET_INSERT_POS( &side , &pt , &lastsp , incm0 , wafer );
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[4] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//
	if ( Res != 0 ) return ( Res + 20 );
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[5] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_i_SCH_CLUSTER_Copy_Cluster_Data( -1 , -1 , side , pt , 4 );
	//==========================================================================================
	incm = incm0;
	//
	if ( incm <= 0 ) {
		//
		incm = CM1;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			//
			incm = CM1 + i;
			//
			break;
			//
		}
		//
	}
	//==========================================================================================
	if ( clusterRecipefile[0] != 0 ) {
		//
		if ( STRNCMP_L( clusterRecipefile , "<MANUAL>" , 8 ) ) { // 2011.04.15
			//
			pos = 8;
			//
		}
		else {
			//
			pos = 0;
			//
		}
		//
		Res = _i_SCH_SUB_RECIPE_READ_CLUSTER( side , pt , 0 , incm , wafer , clusterRecipefile + pos );
		//
		if ( Res != ERROR_NONE ) return ( Res + 30 );
		//
	}
	else {
		pos = 0;
		//
		_i_EIL_METHOD_INSERT_BLANK( side , pt , incm , wafer );
		//
	}
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[6] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//
	_i_SCH_CLUSTER_Set_Extra_Flag( side , pt , 3 , ( pos == 0 ) ? 0 : 1 ); // 2011.04.15
	//==========================================================================================
	Res = EIL_MAKE_DATABASE( side , pt , lastsp , incm , wafer );
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[7] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//
	if ( Res != 0 ) return ( Res + 100 );
	//==========================================================================================
	_i_SCH_CLUSTER_Set_Ex_RecipeName( side , pt , clusterRecipefile + pos ); // 2012.09.13
	//
	if ( !EIL_PRE_SETTING( side , pt , bm , slot , incm0 , wafer , clusterRecipefile + pos , informationfile , wid_control , ppid_control ) ) { // 2010.10.05
		return 51;
	}
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[8] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
	if ( !USER_RECIPE_INFO_VERIFICATION_EIL( side , pt , bm , slot , incm0 , wafer , clusterRecipefile + pos , informationfile ) ) { // 2010.09.16
		return 61;
	}
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[9] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
	if ( !_SCH_INF_DLL_INFO_VERIFICATION_EIL( side , pt , bm , slot , incm0 , wafer , clusterRecipefile + pos , informationfile ) ) { // 2010.09.16
		return 71;
	}
	//
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[A] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_EIL_INSERT_DATA_REMAP( side , pt );
	//
	//==========================================================================================
//	EIL_MAKE_SLOT_FOLDER( side , pt , wafer , wid_control , TRUE ); // 2011.05.13 // 2015.09.01 // 2015.09.01
	EIL_MAKE_SLOT_FOLDER( side , pt , wafer , wid_control , clusterRecipefile + pos , ppid_control , TRUE ); // 2011.05.13 // 2015.09.01 // 2015.09.01
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[B] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	if ( !EIL_MODULE_STATUS_PM_SET( TRUE , side , pt , ppid_control ) ) return 81;
	//==========================================================================================
//	_i_SCH_CLUSTER_Set_PathStatus( side , pt , SCHEDULER_SUPPLY ); // 2011.07.05 // 2011.10.20 FM_Nomore가 되지 않게 하기 위해 READY로 남겨둠
	//-----------------------------------------------------------------------------------------------------------------------
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[C] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
	_i_SCH_MODULE_Inc_FM_OutCount( side );
	_i_SCH_MODULE_Inc_TM_OutCount( side );
	//=============================================
	_i_SCH_MODULE_Set_FM_DoPointer_FDHC( side , 0 );
	_i_SCH_MODULE_Set_TM_DoPointer( side , 0 );
	//==========================================================================================
	_i_SCH_CASSETTE_LAST_RESET( incm , wafer );
	_i_SCH_CASSETTE_LAST_RESET2( side , pt ); // 2011.04.21
	//==========================================================================================
	SCHEDULER_CASSETTE_LAST_RESULT_INIT_AFTER_CM_PICK( incm , wafer , side , pt ); // 2011.08.25
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[D] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	if ( EIL_BM_IS_INVALID( _i_SCH_CLUSTER_Get_PathIn( side , pt ) ) ) {
		_i_SCH_IO_SET_MODULE_SOURCE( bm , slot , MAX_CASSETTE_SIDE + 1 );
	}
	else {
		_i_SCH_IO_SET_MODULE_SOURCE( bm , slot , _i_SCH_CLUSTER_Get_PathIn( side , pt ) - 1 );
	}
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[E] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_i_SCH_IO_SET_MODULE_RESULT( bm , slot , 0 );
	_i_SCH_IO_SET_MODULE_STATUS( bm , slot , wafer );
	//==========================================================================================
	_i_SCH_MODULE_Set_BM_SIDE( bm , slot , side );
	_i_SCH_MODULE_Set_BM_POINTER( bm , slot , pt );
	_i_SCH_MODULE_Set_BM_WAFER_STATUS( bm , slot , wafer , BUFFER_INWAIT_STATUS );
	//
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[F] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_SCH_CLUSTER_Parallel_Check_Curr_DisEna( side , pt , -1 , TRUE ); // 2011.06.27
	//
	//==========================================================================================
	EIL_CHANGE_STATUS_TO( bm , slot , _EIL_STS_O_OCCUPY );
	//==========================================================================================
//	if ( _EIL_SUPPLY_COUNT_INSERT[ side ] <= 0 ) _i_SCH_LOG_TIMER_PRINTF( side , TIMER_FIRST , -1 , -1 , -1 , -1 , -1 , "" , "" ); // 2010.11.26 // 2012.09.25
	if ( _EIL_SUPPLY_COUNT_INSERT[ side ] <= 0 ) _i_SCH_LOG_TIMER_PRINTF( side , TIMER_FIRST , pt , -1 , -1 , -1 , -1 , "" , "" ); // 2010.11.26 // 2012.09.25
	_EIL_SUPPLY_COUNT_INSERT[ side ]++; // 2010.11.26
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[G] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
//	_i_SCH_LOG_TIMER_PRINTF( side , 1003 , wafer , bm , -1 , -1 , -1 , "" , "" ); // 2011.06.15 // 2011.08.02
	_i_SCH_LOG_TIMER_PRINTF( side , 1003 , wafer , bm , slot , -1 , -1 , "" , "" ); // 2011.06.15 // 2011.08.02
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[H] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_i_SCH_LOG_TIMER_PRINTF( side , 1001 , wafer ,  1 , -1 , slot , -1 , "INSERT" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( bm ) ); // 2011.06.15
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[I] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_i_SCH_LOG_TIMER_PRINTF( side , 1002 , wafer , -1 , -1 , -1 , -1 , "" , "" ); // 2011.06.15
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[J] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[K] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	_EIL_LOG_LOT_PRINTF( side , wafer , "TM Handling Inserted (%s:%d) in %s:%d%cWHTMINSERT %d:%d:%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _i_SCH_CLUSTER_Get_PathIn( side , pt ) ) , wafer , _i_SCH_SYSTEM_GET_MODULE_NAME( bm ) , slot , 9 , side , pt , wafer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( bm ) , slot );
	//==========================================================================================
	time( &(_EIL_TIME_FOR_INSERT[side][pt]) ); // 2011.06.15
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[L] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
	STR_MEM_MAKE_COPY2( &(_EIL_METHOD_RECIPE[side][pt]) , clusterRecipefile + pos );
	//==========================================================================================
	EIL_APPEND_JOB_LOG( side , pt , FALSE );
//_IO_COMMON_PRINTF( 3 , "EIL" , "INSERT" , "[Z] RES=%d , BM=%d,Slot=%d,incm0=%d,Wafer=%d,File=%s\n" , Res , bm , slot , incm0 , wafer , clusterRecipefile );
	//==========================================================================================
	_i_EIL_METHOD_SET_WAFER_NUMBERING( wafer );
	//==========================================================================================
	return 0;
}



int _i_EIL_OPERATE_REQUEST_TM_TO_EIL( int bm ) {
	//
	int j , Res , side = 0;
	//==========================================================================================
	if ( ( bm < BM1 ) || ( bm >= TM ) ) return 1;
	//==========================================================================================
	//
	Res = EIL_BM_USE_POSSIBLE( bm , 1 );
	//
	if ( Res != 0 ) return ( Res + 10 );
	//
	for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( bm ) ; j++ ) {
		//
		if ( _i_SCH_MODULE_Get_BM_WAFER( bm , j ) <= 0 ) continue;
		side = _i_SCH_MODULE_Get_BM_SIDE( bm , j );
		//
		break;
		//
	}
	//
	IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_MODE[bm-BM1]) , _EIL_MODE_EQUIP , "CTC.BM%d_EIL_MODE" , bm-BM1 + 1 );
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bm , 1 , TRUE , 0 , 1234 );
	//
	BM_EIL_OPERATE_SIGNAL[bm-BM1] = 0;
	//
	return 0;
}


int _i_EIL_METHOD_REMOVE_TO_EIL( int bm , int slot , BOOL prcsnotcheck ) {
	int Res , s , p , tr , wafer , pathin;
	//==========================================================================================
	if ( ( bm < BM1 ) || ( bm >= TM ) ) return 1;
	if ( ( slot <= 0 ) || ( slot > _i_SCH_PRM_GET_MODULE_SIZE( bm ) ) ) return 2;
	//==========================================================================================
	//
	Res = EIL_BM_USE_POSSIBLE( bm , prcsnotcheck ? 2 : 1 ); // 2012.05.08
	//
	if ( Res != 0 ) return ( Res + 10 );
	//
	if ( _i_SCH_MODULE_Get_BM_WAFER( bm , slot ) <= 0 ) return 6;
	//
	s = _i_SCH_MODULE_Get_BM_SIDE( bm , slot );
	p = _i_SCH_MODULE_Get_BM_POINTER( bm , slot );
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) return 7;
	//
	wafer  = _i_SCH_CLUSTER_Get_SlotIn( s , p );
	pathin = _i_SCH_CLUSTER_Get_PathIn( s , p );
	//=============================================
	tr = SCHMULTI_RUNJOB_DELETE_TUNING_INFO( FALSE , s , p ); // 2011.06.15
	//=============================================
	EIL_APPEND_JOB_LOG( s , p , TRUE );
	//==========================================================================================
	_i_SCH_CLUSTER_Set_PathRange( s , p , -1 );
	//==========================================================================================
	_i_SCH_IO_SET_MODULE_STATUS( bm , slot , 0 );
	//=============================================
	_i_SCH_MODULE_Set_BM_WAFER_STATUS( bm , slot , 0 , -1 );
	//=============================================
	_i_SCH_MODULE_Inc_FM_InCount( s );
	_i_SCH_MODULE_Inc_TM_InCount( s );
	//=============================================
	EIL_CHANGE_STATUS_TO( bm , slot , _EIL_STS_X_REMOVED );
	//==========================================================================================
	EIL_MODULE_STATUS_PM_SET( FALSE , s , p , "" );
	//==========================================================================================
	_EIL_SUPPLY_COUNT_REMOVE[ s ]++;
	//==========================================================================================
	LOG_MTL_SET_DONE_WFR_COUNT( s , _EIL_SUPPLY_COUNT_REMOVE[ s ] ); // 2010.11.26
	//==========================================================================================
	_i_SCH_LOG_TIMER_PRINTF( s , 1004 , wafer , bm , slot , -1 , -1 , "" , "" ); // 2011.06.15
	_i_SCH_LOG_TIMER_PRINTF( s , 1001 , wafer ,  1 , -1 , slot , -1 , "REMOVE" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( bm ) ); // 2011.06.15
	_i_SCH_LOG_TIMER_PRINTF( s , 1002 , wafer , -1 , -1 , -1 , -1 , "" , "" ); // 2011.06.15
	//==========================================================================================
	_EIL_LOG_LOT_PRINTF( s , wafer , "TM Handling Removed (%s:%d) in %s:%d%cWHTMREMOVE %d:%d:%d:%s:%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( pathin ) , wafer , _i_SCH_SYSTEM_GET_MODULE_NAME( bm ) , slot , 9 , s , p , wafer , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( bm ) , slot , tr );
	//==========================================================================================
	STR_MEM_MAKE_COPY2( &(_EIL_METHOD_RECIPE[s][p]) , "" );
	//==========================================================================================
	return 0;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void _i_EIL_SET_FINISH( int side ) {
	int i;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			_i_SCH_IO_SET_END_BUTTON( i , CTCE_ENDING ); // 2012.07.03
			_i_SCH_SYSTEM_APPEND_END_SET( i , TRUE );
		}
	}
	else {
		_i_SCH_IO_SET_END_BUTTON( side , CTCE_ENDING ); // 2012.07.03
		_i_SCH_SYSTEM_APPEND_END_SET( side , TRUE );
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_EIL_PM_INFO_TAG_Make_PM_Mdl_String( char *strbuffer ) {
	//
	// *:11221-111111:*:111111221-11
	// A1d21-111111:11221-111111:11221-111111:11221-111111
	//
	int side , i;
	//
	strcpy( strbuffer , "" );
	//
	for ( side = 0 ; side < MAX_CASSETTE_SIDE ; side++ ) {
		//
		if ( side != 0 ) strcat( strbuffer , "-" );
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) {
			strcat( strbuffer , "*" );
			continue;
		}
		//
		for ( i = PM1 ; i < ( PM1 + 6 ) ; i++ ) {
			switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) ) {
			case MUF_00_NOTUSE					: strcat( strbuffer , "-" ); break;	//							0
			case MUF_01_USE						: strcat( strbuffer , "1" ); break;	//	INC		CTC_InUse		1
			case MUF_02_USE_to_END				: strcat( strbuffer , "2" ); break;	//	DEC						2
			case MUF_03_USE_to_END_DISABLE		: strcat( strbuffer , "3" ); break;	//	DEC						3
			case MUF_04_USE_to_PREND_EF_LP		: strcat( strbuffer , "4" ); break;	//	DEC						4
			case MUF_05_USE_to_PRENDNA_EF_LP	: strcat( strbuffer , "5" ); break;	//	DEC						5
			case 6								: strcat( strbuffer , "6" ); break;
			case MUF_07_USE_to_PREND_DECTAG		: strcat( strbuffer , "7" ); break;	//	DEC						7
			case MUF_08_USE_to_PRENDNA_DECTAG	: strcat( strbuffer , "8" ); break;	//	DEC						8
			case 9								: strcat( strbuffer , "9" ); break;
			case 10								: strcat( strbuffer , "A" ); break;
			case 11								: strcat( strbuffer , "B" ); break;
			case MUF_31_USE_to_NOTUSE_SEQMODE	: strcat( strbuffer , "S" ); break;	//	DEC						S
			case MUF_71_USE_to_PREND_EF_XLP		: strcat( strbuffer , "P" ); break;	//	DEC		CTC_InUse		P
			case MUF_81_USE_to_PREND_RANDONLY	: strcat( strbuffer , "o" ); break;	//	XDEC	CTC_InUse		o
			case MUF_90_USE_to_XDEC_FROM		: strcat( strbuffer , "X" ); break;	//	N/A						X
			case MUF_97_USE_to_SEQDIS_RAND		: strcat( strbuffer , "s" ); break;	//	XDEC					s
			case MUF_98_USE_to_DISABLE_RAND		: strcat( strbuffer , "r" ); break;	//	XDEC					r
			case MUF_99_USE_to_DISABLE			: strcat( strbuffer , "d" ); break;	//	XDEC					d
			default								: strcat( strbuffer , "?" ); break;
			}
			switch( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) ) {
			case 0  : strcat( strbuffer , "-" ); break;
			case 1  : strcat( strbuffer , "1" ); break;
			case 2  : strcat( strbuffer , "2" ); break;
			case 3  : strcat( strbuffer , "3" ); break;
			case 4  : strcat( strbuffer , "4" ); break;
			case 5  : strcat( strbuffer , "5" ); break;
			case 6  : strcat( strbuffer , "6" ); break;
			case 7  : strcat( strbuffer , "7" ); break;
			case 8  : strcat( strbuffer , "8" ); break;
			case 9  : strcat( strbuffer , "9" ); break;
			case 10 : strcat( strbuffer , "A" ); break;
			case 11 : strcat( strbuffer , "B" ); break;
			case 12 : strcat( strbuffer , "C" ); break;
			case 13 : strcat( strbuffer , "D" ); break;
			case 14 : strcat( strbuffer , "E" ); break;
			case 15 : strcat( strbuffer , "F" ); break;
			default : strcat( strbuffer , "?" ); break;
			}
		}
	}
}




void _EIL_EVENT_DATA_LOG( char *eventall , int Res , char *mdldata_pre ) {
	char buffer[256];
	char strbuffer[256];
	//
	_i_EIL_PM_INFO_TAG_Make_PM_Mdl_String( strbuffer );
	//
	if ( STRCMP( mdldata_pre , strbuffer ) ) {
		_snprintf( buffer , 255 , "%s=>%d(%s)" , eventall , Res , strbuffer );
	}
	else {
		_snprintf( buffer , 255 , "%s=>%d(%s>%s)" , eventall , Res , mdldata_pre , strbuffer );
	}
	//
	SCHMULTI_SET_LOG_MESSAGE( buffer );
}

//
int Get_PMBM_ID_From_String( int mode , char *mdlstr , int badch , int *resch );

int _EIL_EVENT_RECEIVED_OPERATION( char *Command , int bm , int slot , char *message , char *eventall ) {
	char ElseStr1[256];
	char ElseStr2[256];
	char ElseStr3[256];
	char ElseStr4[256];
	char ElseStrE[256];
	char ElseStrA[256];
	//
	char strbuffer[256];
	//
	int Res , mdl , s;
	int modeR , modeT , modeBM[ MAX_BM_CHAMBER_DEPTH ]; // 2012.04.27
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) return 0;
	//
	_i_EIL_PM_INFO_TAG_Make_PM_Mdl_String( strbuffer );
	//
	if      ( STRCMP_L( Command , "REMOVE" ) || STRCMP_L( Command , "REMOVER" ) || STRCMP_L( Command , "REMOVET" ) || STRCMP_L( Command , "REMOVEA" ) ) { // EIL_CONTROL REMOVE|BM|SLOT , EIL_CONTROL REMOVE|BM|SLOT|REMOVE|BM|SLOT
		/*
		// 2012.03.16
		Res = _i_EIL_METHOD_REMOVE_TO_EIL( bm , slot );
		if ( ( bm >= BM1 ) && ( bm < TM ) ) IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[bm-BM1]) , Res , "CTC.BM%d_EIL_ERROR" , bm - BM1 + 1 );
		*/

		//
		// 2012.05.08
		modeR = FALSE;
		modeT = FALSE;
		//
		if      ( STRCMP_L( Command , "REMOVEA" ) ) {
			modeR = TRUE;
			modeT = TRUE;
		}
		else if ( STRCMP_L( Command , "REMOVET" ) ) {
			modeT = TRUE;
		}
		else if ( STRCMP_L( Command , "REMOVER" ) ) {
			modeR = TRUE;
		}
		//
		for ( s = 0 ; s < MAX_BM_CHAMBER_DEPTH ; s++ ) modeBM[s] = FALSE;
		//
		// 2012.03.16
		//
		strcpy( ElseStrA , message );
		//
		while ( TRUE ) {
			//=============================================================================================================================================
			Res = _i_EIL_METHOD_REMOVE_TO_EIL( bm , slot , modeR );
			if ( ( bm >= BM1 ) && ( bm < TM ) ) IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[bm-BM1]) , Res , "CTC.BM%d_EIL_ERROR" , bm - BM1 + 1 );
			//
			if ( Res != 0 ) break;
			//=============================================================================================================================================
			if ( modeT ) modeBM[bm-BM1] = TRUE; // 2012.05.08
			//=============================================================================================================================================
			STR_SEPERATE_CHAR( ElseStrA , '|' , ElseStr1 , ElseStr2 , 255 );
			//
			// 2012.05.08
			if      ( STRCMP_L( ElseStr1 , "REMOVE" ) ) {
				modeR = FALSE;
				modeT = FALSE;
			}
			else if ( STRCMP_L( ElseStr1 , "REMOVER" ) ) {
				modeR = TRUE;
				modeT = FALSE;
			}
			else if ( STRCMP_L( ElseStr1 , "REMOVET" ) ) {
				modeR = FALSE;
				modeT = TRUE;
			}
			else if ( STRCMP_L( ElseStr1 , "REMOVEA" ) ) {
				modeR = TRUE;
				modeT = TRUE;
			}
			else {
				break;
			}
			//
			STR_SEPERATE_CHAR( ElseStr2 , '|' , ElseStr1 , ElseStr3 , 255 );
			//
			Res = 0;
			switch( Get_PMBM_ID_From_String( 2 , ElseStr1 , 0 , &bm ) ) {
			case 0 :	Res = 1; break;
			case 2 :	Res = 1; break;
			}
			if ( Res != 0 ) break;
			//
			STR_SEPERATE_CHAR( ElseStr3 , '|' , ElseStr1 , ElseStrA , 255 );
			slot = atoi( ElseStr1 );
			//=============================================================================================================================================
		}
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		for ( s = 0 ; s < MAX_BM_CHAMBER_DEPTH ; s++ ) { // 2012.05.08
			if ( modeBM[s] ) {
				BM_EIL_AUTO_TMSIDE[ s ] = TRUE;
			}
		}
		//
		return Res;
	}
	else if ( STRCMP_L( Command , "INSERT" ) || STRCMP_L( Command , "INSERTR" ) || STRCMP_L( Command , "INSERTT" ) || STRCMP_L( Command , "INSERTA" ) ) { // EIL_CONTROL INSERT|BM|SLOT|MDL|WAFER|CL|INFOFILE , EIL_CONTROL INSERT|BM|SLOT|MDL|WAFER|CL|INFOFILE|INSERT|BM|SLOT|MDL|WAFER|CL|INFOFILE
		//
		// 2012.04.27
		modeR = FALSE;
		modeT = FALSE;
		//
		if      ( STRCMP_L( Command , "INSERTA" ) ) {
			modeR = TRUE;
			modeT = TRUE;
		}
		else if ( STRCMP_L( Command , "INSERTT" ) ) {
			modeT = TRUE;
		}
		else if ( STRCMP_L( Command , "INSERTR" ) ) {
			modeR = TRUE;
		}
		//
		for ( s = 0 ; s < MAX_BM_CHAMBER_DEPTH ; s++ ) modeBM[s] = FALSE;
		//
		/*
		// 2012.03.16
		STR_SEPERATE_CHAR( message  , '|' , ElseStr1 , ElseStrE , 255 );
		//
		mdl = Get_Module_ID_From_String( 0 , ElseStr1 , 0 );
		//
		STR_SEPERATE_CHAR( ElseStrE , '|' , ElseStr1 , ElseStr2 , 255 );	s = atoi(ElseStr1);
		STR_SEPERATE_CHAR( ElseStr2 , '|' , ElseStr1 , ElseStrE , 255 );
		Res = _i_EIL_METHOD_INSERT_TO_EIL( bm , slot , mdl , s , ElseStr1 , ElseStrE );
		if ( ( bm >= BM1 ) && ( bm < TM ) ) IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[bm-BM1]) , Res , "CTC.BM%d_EIL_ERROR" , bm - BM1 + 1 );
		*/
		// 2012.03.16
		//
		strcpy( ElseStrA , message );
		//
		while ( TRUE ) {
			//=============================================================================================================================================
			STR_SEPERATE_CHAR( ElseStrA  , '|' , ElseStr1 , ElseStrE , 255 );
			//
			mdl = Get_Module_ID_From_String( 0 , ElseStr1 , 0 );
			//
			STR_SEPERATE_CHAR( ElseStrE , '|' , ElseStr1 , ElseStr2 , 255 );	s = atoi(ElseStr1);
			STR_SEPERATE_CHAR( ElseStr2 , '|' , ElseStr1 , ElseStrE , 255 );
			STR_SEPERATE_CHAR( ElseStrE , '|' , ElseStr2 , ElseStr3 , 255 );
			//
			Res = _i_EIL_METHOD_INSERT_TO_EIL( bm , slot , mdl , s , ElseStr1 , ElseStr2 , modeR );
			if ( ( bm >= BM1 ) && ( bm < TM ) ) IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[bm-BM1]) , Res , "CTC.BM%d_EIL_ERROR" , bm - BM1 + 1 );
			//
			if ( Res != 0 ) break;
			//=============================================================================================================================================
			if ( modeT ) modeBM[bm-BM1] = TRUE; // 2012.04.27
			//=============================================================================================================================================
			STR_SEPERATE_CHAR( ElseStr3 , '|' , ElseStr1 , ElseStr2 , 255 );
			//
			if      ( STRCMP_L( ElseStr1 , "INSERT" ) ) {
				modeR = FALSE;
				modeT = FALSE;
			}
			else if ( STRCMP_L( ElseStr1 , "INSERTR" ) ) {
				modeR = TRUE;
				modeT = FALSE;
			}
			else if ( STRCMP_L( ElseStr1 , "INSERTT" ) ) {
				modeR = FALSE;
				modeT = TRUE;
			}
			else if ( STRCMP_L( ElseStr1 , "INSERTA" ) ) {
				modeR = TRUE;
				modeT = TRUE;
			}
			else {
				break;
			}
			//
			STR_SEPERATE_CHAR( ElseStr2 , '|' , ElseStr1 , ElseStr3 , 255 );
			//
			Res = 0;
			switch( Get_PMBM_ID_From_String( 2 , ElseStr1 , 0 , &bm ) ) {
			case 0 :	Res = 1; break;
			case 2 :	Res = 1; break;
			}
			if ( Res != 0 ) break;
			//
			STR_SEPERATE_CHAR( ElseStr3 , '|' , ElseStr1 , ElseStrA , 255 );
			slot = atoi( ElseStr1 );
			//=============================================================================================================================================
		}
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		for ( s = 0 ; s < MAX_BM_CHAMBER_DEPTH ; s++ ) { // 2012.04.27
			if ( modeBM[s] ) {
				BM_EIL_AUTO_TMSIDE[ s ] = TRUE;
			}
		}
		//
		return Res;
	}
	else if ( STRCMP_L( Command , "START" ) ) { // EIL_CONTROL START|BM
		Res = _i_EIL_OPERATE_REQUEST_TM_TO_EIL( bm );
		if ( ( bm >= BM1 ) && ( bm < TM ) ) IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_BM_ERROR[bm-BM1]) , Res , "CTC.BM%d_EIL_ERROR" , bm - BM1 + 1 );
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		return Res;
	}
	else if ( STRCMP_L( Command , "STOP" ) ) { // EIL_CONTROL STOP|CM|WAFERID
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( bm ) ) Res = 1;
		else                                      Res = 0;
		//
		if ( Res == 0 ) {
			//
			STR_SEPERATE_CHAR( message  , '|' , ElseStr1 , ElseStrE , 255 );
			//
//			if ( !_i_SCH_CLUSTER_Check_and_Make_Return_Wafer_from_Stop( bm , FALSE , atoi( ElseStr1 ) ) ) Res = 11; // 2011.07.21
			//
//			SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , bm + CM1 , atoi( ElseStr1 ) , TRUE ); // 2011.07.21 // 2013.11.28
			SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , bm + CM1 , atoi( ElseStr1 ) , 1 ); // 2011.07.21 // 2013.11.28
			//
		}
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_CM_ERROR[bm]) , Res , "CTC.CM%d_EIL_ERROR" , bm + 1 );
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		return Res;
		//
	}
	else if ( STRCMP_L( Command , "BEGIN" ) ) { // EIL_CONTROL BEGIN|CM|JOBID|MID|PPID|ELSESTR[LOTPRE|...]
		//
		Res = EIL_SIDE_HAS_DATA( bm , TRUE );
		//
		if ( Res == 0 ) {
			//
			STR_SEPERATE_CHAR( message  , '|' , ElseStr1 , ElseStrE , 255 );
			//
			STR_SEPERATE_CHAR( ElseStrE , '|' , ElseStr2 , ElseStr4 , 255 );

			STR_SEPERATE_CHAR( ElseStr4 , '|' , ElseStr3 , ElseStrE , 255 );
			//
			Res = _i_EIL_SIDE_BEGIN( bm , ElseStr1 , ElseStr2 , ElseStr3 , ElseStrE );
			//
		}
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_CM_ERROR[bm]) , Res , "CTC.CM%d_EIL_ERROR" , bm + 1 );
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		return Res;
		//
	}
	else if ( STRCMP_L( Command , "END" ) ) { // EIL_CONTROL END|CM
		//
		Res = EIL_SIDE_HAS_DATA( bm , TRUE );
		//
		if ( Res == 0 ) {
			//
			Res = _i_EIL_SIDE_END( bm , message );
			//
		}
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_CM_ERROR[bm]) , Res , "CTC.CM%d_EIL_ERROR" , bm + 1 );
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		return Res;
		//
	}
	else if ( STRCMP_L( Command , "LOTPREUPDATE" ) ) { // EIL_CONTROL LOTPREUPDATE
		//
		Res = EIL_SIDE_HAS_DATA( bm , FALSE );
		//
		if ( Res == 0 ) {
			//
			Res = _i_EIL_SIDE_LOTPREUPDATE( bm );
			//
		}
		//
		IO_ADD_WRITE_DIGITAL_NAME( &(Address_EIL_CM_ERROR[bm]) , Res , "CTC.CM%d_EIL_ERROR" , bm + 1 );
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
		return Res;
		//
	}
	else if ( STRCMP_L( Command , "FINISH" ) ) { // EIL_CONTROL FINISH
		//
		_i_EIL_SET_FINISH( -1 );
		//
		Res = 0;
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
	}
	else {
		//
		Res = -999999;
		//
		_EIL_EVENT_DATA_LOG( eventall , Res , strbuffer );
		//
	}
	return 0;
}
