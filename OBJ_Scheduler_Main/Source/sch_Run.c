#include "default.h"
#include <commctrl.h>

#include "EQ_Enum.h"

#include "system_tag.h"
#include "ioLog.h"
#include "User_Parameter.h"
#include "Gui_Handling.h"
#include "sch_main.h"
#include "sch_estimate.h"
#include "sch_prm.h"
#include "sch_prm_log.h"
#include "sch_CassetteResult.h"
#include "sch_HandOff.h"
#include "sch_prepost.h"
#include "sch_cassmap.h"
#include "sch_Interlock.h"
#include "sch_sub.h"
#include "Sch_Multi_ALIC.h"
#include "sch_processmonitor.h"
#include "sch_util.h"
#include "Robot_Handling.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "FA_Handling.h"
#include "File_ReadInit.h"
#include "MR_Data.h"
#include "Multijob.h"
#include "Multireg.h"
#include "lottype.h"
#include "DLL_Interface.h"
#include "Errorcode.h"
#include "RcpFile_Handling.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_ReStart.h"
#include "sch_sdm.h" // 2015.07.30

#include "INF_sch_CommonUser.h"

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "resource.h"


extern int EXTEND_SOURCE_USE;

void OPERATE_CONTROL( int side , int mode );

void SCHEDULER_CLUSTER_Set_DefaultUse_Data( int side ); // 2017.09.08

void _SCH_CASSETTE_Set_Count_Action( int s , BOOL ); // 2016.12.21 // 2018.10.10

extern int SYSTEM_WIN_UPDATE;

extern Scheduling_Path	_SCH_INF_CLUSTER_DATA_AREA[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];

void SCHEDULER_CONTROL_Make_Clear_Method_Data( int side );

char *SCHMULTI_CASSETTE_MID_GET( int side );
void SCHEDULER_Set_CARRIER_INDEX( int side , int data );
int SCHEDULER_Get_CARRIER_INDEX( int side );

void TIMER_MONITOR_END_For_Move( int tside , int c ); // 2012.03.20

int Scheduler_Run_Unload_Code( int side , int cm , BOOL famode , BOOL Manual , int mode , int p , int w );

void PROCESS_MONITOR_ReSet_For_Move( int tside , int tpointer , int sside , int spointer );
//void SCHEDULER_Make_CLUSTER_INDEX_END( int ); // 2008.02.26 // 2008.04.17

//------------------------------------------------------------------------------------------
BOOL SCHEDULER_SIDE_IN_POSSIBLE( int , int side ); // 2010.01.19
BOOL SCHEDULER_SIDE_OUT_POSSIBLE( BOOL , int side ); // 2010.01.19
int  SCHEDULER_SIDE_GET_CM_IN( int side ); // 2010.01.19
int  SCHEDULER_SIDE_GET_CM_OUT( int side ); // 2010.01.19
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char _In_RecipeFileName[MAX_CASSETTE_SIDE][256];
int  _In_SL_1[MAX_CASSETTE_SIDE];
int  _In_SL_2[MAX_CASSETTE_SIDE];
int  _In_Loop[MAX_CASSETTE_SIDE];
int  _In_Run_Finished_Check[MAX_CASSETTE_SIDE];
//
int  _In_Job_HandOff_In_Pre_Check[MAX_CASSETTE_SIDE];
int  _In_Job_HandOff_Out_Message_Skip[MAX_CASSETTE_SIDE];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char _in_IMSI_JOB_RECIPE[MAX_CASSETTE_SIDE][256];
char _in_IMSI_LOT_RECIPE[MAX_CASSETTE_SIDE][256];
char _in_IMSI_MID_RECIPE[MAX_CASSETTE_SIDE][256];
int  _in_IMSI_EI[MAX_CASSETTE_SIDE];
int  _in_IMSI_SS[MAX_CASSETTE_SIDE];
int  _in_IMSI_ES[MAX_CASSETTE_SIDE];
int  _in_IMSI_LC[MAX_CASSETTE_SIDE];
char _in_IMSI_ETC[MAX_CASSETTE_SIDE][256];
BOOL _in_IMSI_INFO_REGIST_RES[MAX_CASSETTE_SIDE];
int  _in_IMSI_PralSkip[MAX_CASSETTE_SIDE]; // 2009.08.05
//------------------------------------------------------------------------------------------
int  _in_HOT_LOT_CHECKING[MAX_CASSETTE_SIDE]; // 2011.06.13
int  _in_HOT_LOT_ORDERING_INVALID[MAX_CASSETTE_SIDE]; // 2011.06.13
//------------------------------------------------------------------------------------------
int  _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[MAX_CASSETTE_SIDE]; // 2014.10.14
//------------------------------------------------------------------------------------------
int  _in_NOCHECK_RUNNING_TYPE[MAX_CASSETTE_SIDE]; // 2017.10.10
//------------------------------------------------------------------------------------------
int  _in_UNLOAD_USER_TYPE[MAX_CASSETTE_SIDE]; // 2010.04.26
//
//			TIME_COMPLETE(TIMER_STOP)	COMPLETE_F(FA_COMPLETE)		UNLOAD_PART
//	0				O							O						O
//	1				O							O						X
//	2				O							X						X
//	3				X							O						X
//	4				X							X						X

//	5				O							X						O
//	6				X							O						O
//	7				X							X						O

// 2017.05.22
//	11				O							O						X	Mapped
//	12				O							X						X	Mapped
//	13				X							O						X	Mapped
//	14				X							X						X	Mapped

//------------------------------------------------------------------------------------------

void Scheduler_InfoInit() {	// 2009.07.16
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		strcpy( _In_RecipeFileName[i] , "" );
		_In_SL_1[i] = -1;
		_In_SL_2[i] = -1;
		_In_Loop[i] = -1;
	}
}
//------------------------------------------------------------------------------------------
void Scheduler_System_Log( int side , char *logdata , int infolog , int sd ) { // 2006.02.02
	char Buffer[128];
	if ( ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2006.02.02
		sprintf( Buffer , "%s/LOTLOG/%s/System.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) );
		SCHEDULING_DATA_STRUCTURE_LOG( Buffer , logdata , -1 , 0 , sd );
	}
	if ( infolog > 0 ) { // 2006.10.19
		sprintf( Buffer , "%s/LOTLOG/%s/Info.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) );
		SCHEDULING_DATA_STRUCTURE_LOG( Buffer , "" , side , infolog , sd );
	}
}
//===================================================================================================
//===================================================================================================
//------+-------------------+---------------+---------------+
//		|PRJOB FINISH STATUS| Mdl.Tag.Reset	| Mdl.Tag.Dec	|
//------+-------------------+---------------+---------------+
//	-3	|	SYS_ABORTED		|	ALL			|	CM			|
//	-2	|	SYS_ABORTED		|	ALL			|	ALL			|
//	-1	|	SYS_ERROR		|	X			|	X			|
//	 0	|	SYS_ERROR		|	ALL			|	ALL			|
//	 1	|	SYS_ERROR		|	ALL			|	CM			|
//	 2	|	SYS_ERROR		|	ALL			|	X			|
//------+-------------------+---------------+---------------+
//
//void Scheduler_Fail_Finish_Check( int CHECKING_SIDE , int mode ) { // 2016.06.10
void Scheduler_Fail_Finish_Check( int CHECKING_SIDE , int mode , BOOL jobqueueclear ) { // 2016.06.10
	//
	if ( jobqueueclear ) SCHMULTI_ALL_QUEUE_DELETE_CHECK(); // 2016.06.10
	//
	if      ( mode == -3 ) {
		SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( CHECKING_SIDE , SYS_ABORTED );
		Scheduler_Module_Use_Reference_Data_Refresh( CHECKING_SIDE , 1 );
	}
	else if ( mode == -2 ) {
		SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( CHECKING_SIDE , SYS_ABORTED );
		Scheduler_Module_Use_Reference_Data_Refresh( CHECKING_SIDE , 0 );
	}
	else {
		//
		switch ( _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() ) { // 2018.12.20
		case 2 :
		case 3 :
			SCHMULTI_ALL_QUEUE_DELETE_CHECK();
			SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( CHECKING_SIDE , SYS_ABORTED );
			break;
		default :
			SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( CHECKING_SIDE , SYS_ERROR );
			break;
		}
		//
		if ( mode != -1 ) Scheduler_Module_Use_Reference_Data_Refresh( CHECKING_SIDE , mode );
		//
	}
	SCHMULTI_ALL_FORCE_CHECK( CHECKING_SIDE );
}
//===================================================================================================
//===================================================================================================
int Scheduler_Info_Max_CassetteCount() {
	int i , c = 0;
	for ( i = CM1 ; i < PM1 ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) c = i;
	}
	return c;
}
//===================================================================================================
int Scheduler_InfoRegCheck( int CHECKING_SIDE , int EI , char *Lot , int SS , int ES , int LC ) {
	int MaxSlot , maxunuse;
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2010.10.12 // 2017.10.10
	if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
		if ( CHECKING_SIDE < 0 || CHECKING_SIDE >= MAX_CASSETTE_SIDE ) return ERROR_CASS_INPATH;
	}
	else {
		//
		MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , CHECKING_SIDE + 1 , CHECKING_SIDE + 1 , 3 , &maxunuse ); // 2010.12.17
		//
		if ( CHECKING_SIDE < 0 || CHECKING_SIDE >= Scheduler_Info_Max_CassetteCount() ) return ERROR_CASS_INPATH;
		if ( EI < 0 || EI >= Scheduler_Info_Max_CassetteCount() ) return ERROR_CASS_OUTPATH;
		if ( SS <= 0 || SS > MaxSlot ) return ERROR_CASS_STARTRANGE; // 2010.12.17
		if ( ES <= 0 || ES > MaxSlot ) return ERROR_CASS_ENDRANGE; // 2010.12.17
		if ( SS > ES ) return ERROR_CASS_RANGE;
		if ( strlen( Lot ) <= 0 ) return ERROR_RECIPE_SELECT;
	}
	return ERROR_NONE;
}
//===================================================================================================
void Scheduler_InfoRegistCheck_WM_INITDIALOG( HWND hdlg , int index ) {
	char Buffer[16];
	//
	_in_IMSI_INFO_REGIST_RES[ index ] = FALSE;
	sprintf( Buffer , "CM%d -> CM%d" , index + 1 , _in_IMSI_EI[index]+1 );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , Buffer );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE2 , _in_IMSI_JOB_RECIPE[index] );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE3 , _in_IMSI_LOT_RECIPE[index] );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE4 , _in_IMSI_MID_RECIPE[index] );
	sprintf( Buffer , "%d -> %d" , _in_IMSI_SS[index] , _in_IMSI_ES[index] );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE5 , Buffer );
	if ( _in_IMSI_LC[index] <= 1 )
		KWIN_Item_Hide( hdlg , IDC_STATIC_MESSAGE6 );
	else
		KWIN_Item_Int_Display( hdlg , IDC_STATIC_MESSAGE6 , _in_IMSI_LC[index] );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE7 , _in_IMSI_ETC[index] );
}
BOOL Scheduler_InfoRegistCheck_WM_COMMAND( HWND hdlg , int index , WPARAM wParam ) {
	switch( wParam ) {
	case IDOK :		_in_IMSI_INFO_REGIST_RES[ index ] = TRUE; EndDialog( hdlg , 0 );	return TRUE;
	case IDCANCEL :	_in_IMSI_INFO_REGIST_RES[ index ] = FALSE; EndDialog( hdlg , 0 );	return TRUE;
	}
	return TRUE;
}
//===================================================================================================
BOOL APIENTRY Scheduler_InfoRegistCheck_TH0_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_INITDIALOG:
		Scheduler_InfoRegistCheck_WM_INITDIALOG( hdlg , 0 );
		return TRUE;
	case WM_COMMAND :
		return Scheduler_InfoRegistCheck_WM_COMMAND( hdlg , 0 , wParam );
	}
	return FALSE;
}
BOOL APIENTRY Scheduler_InfoRegistCheck_TH1_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_INITDIALOG:
		Scheduler_InfoRegistCheck_WM_INITDIALOG( hdlg , 1 );
		return TRUE;
	case WM_COMMAND :
		return Scheduler_InfoRegistCheck_WM_COMMAND( hdlg , 1 , wParam );
	}
	return FALSE;
}
BOOL APIENTRY Scheduler_InfoRegistCheck_TH2_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_INITDIALOG:
		Scheduler_InfoRegistCheck_WM_INITDIALOG( hdlg , 2 );
		return TRUE;
	case WM_COMMAND :
		return Scheduler_InfoRegistCheck_WM_COMMAND( hdlg , 2 , wParam );
	}
	return FALSE;
}
BOOL APIENTRY Scheduler_InfoRegistCheck_TH3_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	switch ( msg ) {
	case WM_INITDIALOG:
		Scheduler_InfoRegistCheck_WM_INITDIALOG( hdlg , 3 );
		return TRUE;
	case WM_COMMAND :
		return Scheduler_InfoRegistCheck_WM_COMMAND( hdlg , 3 , wParam );
	}
	return FALSE;
}
//===================================================================================================
void Scheduler_InfoRegistCheck_TH( int CHECKING_SIDE ) {
	HWND hWnd;
	int id;
	hWnd = GetForegroundWindow(); // GETHINST(NULL) // 2002.06.17
	if      ( CHECKING_SIDE%100 == 0 )
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH0_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH0_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH0_Proc , (LPARAM) NULL ); // 2004.01.19
	else if ( CHECKING_SIDE%100 == 1 )
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH1_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH1_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH1_Proc , (LPARAM) NULL ); // 2004.01.19
	else if ( CHECKING_SIDE%100 == 2 )
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH2_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH2_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH2_Proc , (LPARAM) NULL ); // 2004.01.19
	else if ( CHECKING_SIDE%100 == 3 )
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH3_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH3_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_REGIST_PAD ) , hWnd , Scheduler_InfoRegistCheck_TH3_Proc , (LPARAM) NULL ); // 2004.01.19
	if ( _in_IMSI_INFO_REGIST_RES[ CHECKING_SIDE%100 ] ) {
		_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE%100 , _in_IMSI_JOB_RECIPE[CHECKING_SIDE%100] );
		_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE%100 , _in_IMSI_MID_RECIPE[CHECKING_SIDE%100] );
		id = Scheduler_InfoRegCheck( CHECKING_SIDE%100 , _in_IMSI_EI[CHECKING_SIDE%100] , _in_IMSI_LOT_RECIPE[CHECKING_SIDE%100] , _in_IMSI_SS[CHECKING_SIDE%100] , _in_IMSI_ES[CHECKING_SIDE%100] , _in_IMSI_LC[CHECKING_SIDE%100] );
		if ( CHECKING_SIDE >= 100 ) {
			if ( id == 0 ) 
				FA_ACCEPT_MESSAGE( CHECKING_SIDE%100 , FA_REGIST , 0 );
			else
				FA_REJECT_MESSAGE( CHECKING_SIDE%100 , FA_REGIST , id , "" );
		}
	}
	else {
		if ( CHECKING_SIDE >= 100 ) {
			FA_REJECT_MESSAGE( CHECKING_SIDE%100 , FA_REGIST , ERROR_BY_USER , "" );
		}
	}
	_endthread();
}
//===================================================================================================
int Scheduler_InfoRegistCheck( int CHECKING_SIDE , int EI , char *Job , char *Lot , char *Mid , int SS , int ES , int LC , char *etc , BOOL QA , int FA ) {
	int i;
	if ( QA ) {
		strncpy( _in_IMSI_JOB_RECIPE[CHECKING_SIDE] , Job , 255 );
		strncpy( _in_IMSI_LOT_RECIPE[CHECKING_SIDE] , Lot , 255 );
		strncpy( _in_IMSI_MID_RECIPE[CHECKING_SIDE] , Mid , 255 );
		_in_IMSI_EI[CHECKING_SIDE] = EI;
		_in_IMSI_SS[CHECKING_SIDE] = SS;
		_in_IMSI_ES[CHECKING_SIDE] = ES;
		_in_IMSI_LC[CHECKING_SIDE] = LC;
		strncpy( _in_IMSI_ETC[CHECKING_SIDE] , etc , 255 );
		if ( FA != 0 ) {
			if ( _beginthread( (void *) Scheduler_InfoRegistCheck_TH , 0 , (void *) ( CHECKING_SIDE + 100 ) ) == -1 ) {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_InfoRegistCheck_TH(1) %d\n" , ( CHECKING_SIDE + 100 ) );
				//----------------------------------------------------------------------------------------------------------------
				return -1;
			}
		}
		else {
			if ( _beginthread( (void *) Scheduler_InfoRegistCheck_TH , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_InfoRegistCheck_TH(2) %d\n" , CHECKING_SIDE );
				//----------------------------------------------------------------------------------------------------------------
				return -1;
			}
		}
		return 0;
	}
	else {
		_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Job );
		_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , Mid );
		i = Scheduler_InfoRegCheck( CHECKING_SIDE , EI , Lot , SS , ES , LC );
		if ( i == 0 ) return 9999;
		return i;
	}
}
//===================================================================================================
int Scheduler_InfoCheck( int CHECKING_SIDE , int FAMODE , int CPJOB , int PreCheking , BOOL minicheck_StartJ ) {
	int i , j , csi , cso , ss , cs , es , os , MaxSlot , maxunuse , carrblank;
	CASSETTETemplate CASSETTE;
	Scheduling_Regist	Scheduler_Reg;
	char sBuffer[128];

	carrblank = FALSE; // 2012.04.27

	if ( _SCH_COMMON_USER_RECIPE_CHECK( PreCheking , CHECKING_SIDE , FAMODE , CPJOB , minicheck_StartJ , &j ) ) { // 2012.01.12
		//=====================================================================================
		if ( PreCheking == 0 /* NORMAL_RUN */ ) {
			//
			_In_SL_1[CHECKING_SIDE] = 1;
			_In_SL_2[CHECKING_SIDE] = MAX_CASSETTE_SLOT_SIZE;
			//
			strcpy( _In_RecipeFileName[CHECKING_SIDE] , "" );
			//
			_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , "" );
			_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , "" );
			//
			_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , 0 ); // 2019.02.26
			//
			SYSTEM_IN_MODULE_SET( CHECKING_SIDE , CHECKING_SIDE + CM1 );
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , CHECKING_SIDE + CM1 );
			//
			if ( _i_SCH_IO_GET_LOOP_COUNT( CHECKING_SIDE ) <= 0 ) _i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , 1 );
		}
		//=====================================================================================
		return j;
	}
	//
	if ( !minicheck_StartJ && ( FAMODE == 1 ) ) {
		//
		if ( PreCheking == 2 /* BM_MODE_CHECK */ ) return TRUE; // 2010.02.20
		//
		if ( !_i_SCH_MULTIREG_GET_REGIST_DATA( CHECKING_SIDE , &Scheduler_Reg ) ) {
			return ERROR_RECIPE_SELECT;
		}
		//
		//==================================================================================
		//
		// JOB으로 START
		//
		//==================================================================================
		//
		if ( CPJOB == 1 ) {
			SYSTEM_RID_SET( CHECKING_SIDE , Scheduler_Reg.RunIndex );
			csi = Scheduler_Reg.CassIn + 1;
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_IN_PATH( CHECKING_SIDE , csi );
			cso = Scheduler_Reg.CassOut + 1;
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_OUT_PATH( CHECKING_SIDE , cso );
			//=====================================================================================
			SYSTEM_IN_MODULE_SET( CHECKING_SIDE , csi ); // 2010.01.19
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , cso ); // 2010.01.19
			//=====================================================================================
			if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					_i_SCH_CLUSTER_Set_User_PathIn( CHECKING_SIDE , i , csi );
					if ( _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
						return ERROR_CASS_INPATH;
					}
					//--------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
					//
					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2018.07.17
						if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) < 0 || _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
							return ERROR_CASS_OUTPATH;
						}
					}
					else {
						if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
							return ERROR_CASS_OUTPATH;
						}
					}
					//--------------------------------------------------------------------
				}
			}
			//
			//=====================================================================================
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				CASSETTE.CASSETTE_IN_ORDER[i] = -999999;
				CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
				CASSETTE.CASSETTE_OUT_SLOT[i] = 0; // 2004.10.06
			}
			ss = -1;
			es = -1;
			os = 1;
			j = SCHMULTI_SELECTDATA_GET_MTLCOUNT( Scheduler_Reg.RunIndex );
			for ( i = 0 ; i < j ; i++ ) {
				cs = SCHMULTI_SELECTDATA_GET_MTLINSLOT( Scheduler_Reg.RunIndex , i );
				if ( ( ss == -1 ) || ( ss > cs ) ) ss = cs;
				if ( ( es == -1 ) || ( es < cs ) ) es = cs;
				CASSETTE.CASSETTE_IN_ORDER[cs-1] = os; os++;
				//
//				CASSETTE.CASSETTE_OUT_SLOT[cs-1] = SCHMULTI_SELECTDATA_GET_MTLOUTSLOT( Scheduler_Reg.RunIndex , i ); // 2011.03.08
				//
				maxunuse = SCHMULTI_SELECTDATA_GET_MTLOUTSLOT( Scheduler_Reg.RunIndex , i ); // 2011.03.08
				//
				CASSETTE.CASSETTE_OUT_SLOT[cs-1] = maxunuse; // 2011.03.08
				//
				if ( ( es == -1 ) || ( es < maxunuse ) ) es = maxunuse; // 2011.03.08
				//
// 2011.05.21
				//
				maxunuse = SCHMULTI_GET_VALID_CASSETTE( SCHMULTI_SELECTDATA_GET_MTLOUTCASSETTE( Scheduler_Reg.RunIndex , i ) , SCHMULTI_SELECTDATA_GET_MTLOUTCARRIERID( Scheduler_Reg.RunIndex , i ) );
				//
				if ( maxunuse != -1 ) {
					_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , cs-1 , maxunuse );
					CASSETTE.CASSETTE_OUT_PATH[cs-1] = maxunuse - CM1;
				}
				else {
					if ( csi != cso ) {
						_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , cs-1 , csi );
						CASSETTE.CASSETTE_OUT_PATH[cs-1] = csi - CM1;
					}
				}
				//
// 2011.05.21

			}
			//
			MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , csi , cso , 4 , &maxunuse ); // 2010.12.17
			//
			if ( maxunuse ) { // 2010.12.22
				ss = 1;
				es = MaxSlot;
			}
			//
			_In_SL_1[CHECKING_SIDE] = ss;
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_START_SLOT( CHECKING_SIDE , _In_SL_1[CHECKING_SIDE] );
			if ( _In_SL_1[CHECKING_SIDE] <= 0 || _In_SL_1[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
				return ERROR_CASS_STARTRANGE;
			}
			//
			_In_SL_2[CHECKING_SIDE] = es;
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_END_SLOT( CHECKING_SIDE , _In_SL_2[CHECKING_SIDE] );
			if ( _In_SL_2[CHECKING_SIDE] <= 0 || _In_SL_2[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
				return ERROR_CASS_ENDRANGE;
			}
			//
			if ( _In_SL_1[CHECKING_SIDE] > _In_SL_2[CHECKING_SIDE] ) {
				return ERROR_CASS_RANGE;
			}
			//======
			strcpy( _In_RecipeFileName[CHECKING_SIDE] , Scheduler_Reg.LotName );
			//======
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_RECIPE_FILE( CHECKING_SIDE , Scheduler_Reg.LotName );
			//======
			if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , Scheduler_Reg.LoopCount );
			//======
			_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Scheduler_Reg.JobName );
			_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , Scheduler_Reg.MidName );
			//================================================================================
			//
			// 2019.02.26
			//
			if ( EXTEND_SOURCE_USE != 0 ) {
				_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , _i_SCH_IO_GET_MODULE_SOURCE_E( csi , -1 ) );
			}
			else {
				_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , 0 );
			}
			//
			//================================================================================
			if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( CHECKING_SIDE ) ) { // 2018.01.30
				if ( !USER_RECIPE_MANUAL_INOUT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &CASSETTE ) ) {
					return ERROR_CASS_OUTPATH;
				}
			}
			else {
				if ( !USER_RECIPE_INOUT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &CASSETTE ) ) {
					return ERROR_CASS_OUTPATH;
				}
			}
			//================================================================================
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( CASSETTE.CASSETTE_IN_ORDER[i] <= -999999 ) { // 2007.07.11
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , -999999 ); // 2007.07.11
				}
				else {
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , CASSETTE.CASSETTE_IN_ORDER[i] - 1 );
				}
				//
				_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_PATH[i] + 1 );
				_i_SCH_CLUSTER_Set_User_SlotOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_SLOT[i] );
				//
				//
				//
				//
				//
				//
				if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() >= 2 ) && ( csi != ( CASSETTE.CASSETTE_OUT_PATH[i] + 1 ) ) ) { // 2018.07.09
					//
					if ( BUTTON_GET_FLOW_STATUS_VAR( ( CASSETTE.CASSETTE_OUT_PATH[i] + 1 ) - CM1 ) != _MS_5_MAPPED ) {
						//
						_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , -100 - ( CASSETTE.CASSETTE_OUT_PATH[i] + 1 ) );
						//
					}
				}
				//
			}
		}
		//
		//==================================================================================
		//
		// FA(200mm Style)으로 START
		//
		//==================================================================================
		//
		else {
			//
			SYSTEM_RID_SET( CHECKING_SIDE , Scheduler_Reg.RunIndex );
			//
//			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
			if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
				//
				csi = Scheduler_Reg.CassIn + 1;
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_IN_PATH( CHECKING_SIDE , csi );
				cso = Scheduler_Reg.CassOut + 1;
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_OUT_PATH( CHECKING_SIDE , cso );
				//=====================================================================================
				SYSTEM_IN_MODULE_SET( CHECKING_SIDE , csi ); // 2010.01.19
				SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , cso ); // 2010.01.19
				//=====================================================================================
				if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						_i_SCH_CLUSTER_Set_User_PathIn( CHECKING_SIDE , i , csi );
						if ( _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
							return ERROR_CASS_INPATH;
						}
						//--------------------------------------------------------------------
						_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
						//
						if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2018.07.17
							if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) < 0 || _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
								return ERROR_CASS_OUTPATH;
							}
						}
						else {
							if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
								return ERROR_CASS_OUTPATH;
							}
						}
						//--------------------------------------------------------------------
					}
				}
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( cso ) ) { // 2011.10.13
					//
//					if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && // 2013.06.26
					if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() < 2 ) && // 2013.06.26 // 2018.06.29
						( ( _i_SCH_SYSTEM_USING_GET( cso - CM1 ) > 0 ) || ( ( _i_SCH_SYSTEM_USING_GET( cso - CM1 ) <= 0 ) && ( BUTTON_GET_FLOW_STATUS_VAR( cso - CM1 ) != _MS_5_MAPPED ) ) )
						) { // 2012.01.03
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( cso ) > 0 ) { // 2016.06.17
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , -100 - cso );
								//
							}
							//
						}
						//
					}
					else if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() >= 2 ) ) { // 2018.06.29
						if ( BUTTON_GET_FLOW_STATUS_VAR( cso - CM1 ) != _MS_5_MAPPED ) {
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , -100 - cso );
								//
							}
							//
						}
						else { // 2019.01.09
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
								//
							}
							//
						}
					}
					//
				}
				//=====================================================================================
				//
				MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , csi , cso , 5 , &maxunuse ); // 2010.12.17
				//
				//=====================================================================================
				_In_SL_1[CHECKING_SIDE] = Scheduler_Reg.SlotStart;
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_START_SLOT( CHECKING_SIDE , _In_SL_1[CHECKING_SIDE] );
				if ( _In_SL_1[CHECKING_SIDE] <= 0 || _In_SL_1[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
					return ERROR_CASS_STARTRANGE;
				}
				_In_SL_2[CHECKING_SIDE] = Scheduler_Reg.SlotEnd;
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_END_SLOT( CHECKING_SIDE , _In_SL_2[CHECKING_SIDE] );
				if ( _In_SL_2[CHECKING_SIDE] <= 0 || _In_SL_2[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
					return ERROR_CASS_ENDRANGE;
				}
				if ( _In_SL_1[CHECKING_SIDE] > _In_SL_2[CHECKING_SIDE] ) {
					return ERROR_CASS_RANGE;
				}
				//======
				strcpy( _In_RecipeFileName[CHECKING_SIDE] , Scheduler_Reg.LotName );
				//======
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_RECIPE_FILE( CHECKING_SIDE , Scheduler_Reg.LotName );
				//======
				if ( PreCheking == 0 /* NORMAL_RUN */ ) _i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , Scheduler_Reg.LoopCount );
				//======
				if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(CHECKING_SIDE) == 1 ) {
					if ( PreCheking == 0 /* NORMAL_RUN */ ) IO_GET_MID_NAME_FROM_READ( CHECKING_SIDE , Scheduler_Reg.MidName );
				}
				else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(CHECKING_SIDE) == 2 ) {
					strcpy( Scheduler_Reg.MidName , "" );
				}
				else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(CHECKING_SIDE) == 3 ) {
					if ( PreCheking == 0 /* NORMAL_RUN */ ) IO_GET_MID_NAME( CHECKING_SIDE , Scheduler_Reg.MidName );
				}

				_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Scheduler_Reg.JobName );
				_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , Scheduler_Reg.MidName );
				//================================================================================
				//
				// 2019.02.26
				//
				if ( EXTEND_SOURCE_USE != 0 ) {
					_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , _i_SCH_IO_GET_MODULE_SOURCE_E( csi , -1 ) );
				}
				else {
					_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , 0 );
				}
				//
				//================================================================================
				/*
				//
				// 2016.01.27
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
					CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
					CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
				}
				*/
				//
				// 2016.01.27
				//
				if ( csi == cso ) {
					switch( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() ) {
					case  4 : // Last
					case  5 :
					case  6 :
					case  7 :
						for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					default : // First
						for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					}
				}
				else {
					switch( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() ) {
					case  1 : // First.Rev
					case  5 :
						for ( i = 0 ; i < ( _In_SL_1[CHECKING_SIDE] - 1 ); i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						for ( i = ( _In_SL_1[CHECKING_SIDE] - 1 ) ; i < _In_SL_2[CHECKING_SIDE] ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = _In_SL_2[CHECKING_SIDE] + ( _In_SL_1[CHECKING_SIDE] - 1 ) - i;
						}
						for ( i = _In_SL_2[CHECKING_SIDE] ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					case  2 : // Last.Rev
					case  6 :
						for ( i = 0 ; i < ( _In_SL_1[CHECKING_SIDE] - 1 ); i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						for ( i = ( _In_SL_1[CHECKING_SIDE] - 1 ) ; i < _In_SL_2[CHECKING_SIDE] ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = _In_SL_2[CHECKING_SIDE] + ( _In_SL_1[CHECKING_SIDE] - 1 ) - i;
						}
						for ( i = _In_SL_2[CHECKING_SIDE] ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					case  3 : // Last
					case  7 :
						for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					default : // First
						for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
							CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
							CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
						}
						break;
					}
				}
				//
				if ( !USER_RECIPE_INOUT_DATA_READ( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , &CASSETTE ) ) {
					return ERROR_CASS_OUTPATH;
				}
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , CASSETTE.CASSETTE_IN_ORDER[i] - 1 );
					_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_PATH[i] + 1 );
					_i_SCH_CLUSTER_Set_User_SlotOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_SLOT[i] );
				}
			}
		}
	}
	else {
		//
		//==================================================================================
		//
		// FA사용 아니고 JOB도 아닌 GUI(Manual)로 START
		//
		//==================================================================================
		//
		/* // 2010.02.19
		SYSTEM_RID_SET( CHECKING_SIDE , 0 ); // 2010.02.20
		//
		i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_CASSETTE_IN();
		if ( i >= 0 ) {
			_i_SCH_IO_SET_IN_PATH( CHECKING_SIDE , i - CM1 + 1 );
		}
		i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_CASSETTE_OUT();
		if ( i >= 0 ) {
			_i_SCH_IO_SET_OUT_PATH( CHECKING_SIDE , i - CM1 + 1 );
		}
		i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_SLOT_START();
		if ( i >= 0 ) {
			_i_SCH_IO_SET_START_SLOT( CHECKING_SIDE , i );
		}
		i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_SLOT_END();
		if ( i >= 0 ) {
			_i_SCH_IO_SET_END_SLOT( CHECKING_SIDE , i );
		}
		//
		csi = _i_SCH_IO_GET_IN_PATH( CHECKING_SIDE );
		//
		if ( csi >= PM1 ) csi = csi - PM1 + BM1; // 2010.01.19
		//
		cso = _i_SCH_IO_GET_OUT_PATH( CHECKING_SIDE );
		//
		if ( cso >= PM1 ) cso = cso - PM1 + BM1; // 2010.01.19
		//=====================================================================================
		SYSTEM_IN_MODULE_SET( CHECKING_SIDE , csi ); // 2010.01.19
		SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , cso ); // 2010.01.19
		//=====================================================================================
		*/
		//
		_In_SL_1[CHECKING_SIDE] = 1; // 2013.04.01
		_In_SL_2[CHECKING_SIDE] = MAX_CASSETTE_SLOT_SIZE; // 2013.04.01
		//
		if ( PreCheking == 0 /* NORMAL_RUN */ ) {
			SYSTEM_RID_SET( CHECKING_SIDE , 0 ); // 2010.02.20
			//
			i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_CASSETTE_IN();
			if ( i >= 0 ) {
				_i_SCH_IO_SET_IN_PATH( CHECKING_SIDE , i - CM1 + 1 );
			}
			i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_CASSETTE_OUT();
			if ( i >= 0 ) {
				_i_SCH_IO_SET_OUT_PATH( CHECKING_SIDE , i - CM1 + 1 );
			}
			i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_SLOT_START();
			if ( i >= 0 ) {
				_i_SCH_IO_SET_START_SLOT( CHECKING_SIDE , i );
			}
			i = _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_SLOT_END();
			if ( i >= 0 ) {
				_i_SCH_IO_SET_END_SLOT( CHECKING_SIDE , i );
			}
			//
			csi = _i_SCH_IO_GET_IN_PATH( CHECKING_SIDE );
			//
			if ( csi >= PM1 ) csi = csi - PM1 + BM1; // 2010.01.19
			//
			if ( -99 == _i_SCH_PRM_GET_DFIX_CONTROL_FIXED_CASSETTE_OUT() ) { // 2019.03.03
				cso = csi;
			}
			else {
				//
				cso = _i_SCH_IO_GET_OUT_PATH( CHECKING_SIDE );
				//
				if ( cso >= PM1 ) cso = cso - PM1 + BM1; // 2010.01.19
				//
			}
			//=====================================================================================
			SYSTEM_IN_MODULE_SET( CHECKING_SIDE , csi ); // 2010.01.19
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , cso ); // 2010.01.19
			//=====================================================================================
			//
			if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 0 ) return ERROR_NONE; // 2011.05.12
			if ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 1 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 2 ) ) {
				//
				_In_SL_1[CHECKING_SIDE] = 1;
				_In_SL_2[CHECKING_SIDE] = MAX_CASSETTE_SLOT_SIZE;
				//
				return ERROR_NONE; // 2011.09.23
			}
			//
		}
		else if ( PreCheking == 2 /* BM_MODE_CHECK */ ) { // 2010.02.20
			csi = _i_SCH_IO_GET_IN_PATH( CHECKING_SIDE );
			//
			if ( csi >= PM1 ) {
				SYSTEM_IN_MODULE_SET( CHECKING_SIDE , csi );
				return FALSE;
			}
			//
			return TRUE;
		}
/*
// 2010.01.19
		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				_i_SCH_CLUSTER_Set_User_PathIn( CHECKING_SIDE , i , csi );
				if ( _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) { // 2010.01.11
					return ERROR_CASS_INPATH;
				}
				//-----------------------------------------------------------------
				_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
				if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) <= 0 || _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) > Scheduler_Info_Max_CassetteCount() ) {
					return ERROR_CASS_OUTPATH;
				}
				//-----------------------------------------------------------------
			}
		}
*/
		//
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
		if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
			//
			if ( _SCH_COMMON_USER_RECIPE_CHECK( 6 , CHECKING_SIDE , FAMODE , CPJOB , minicheck_StartJ , &j ) ) { // 2012.04.27 // 2012.09.06
				if ( j == ERROR_NONE ) {
					carrblank = TRUE;
					csi = 0;
				}
				else {
//					return ERROR_CASS_INPATH; // 2012.04.01 // 2012.09.06
					return j; // 2012.04.01 // 2012.09.06
				}
			}
			else {
				if ( csi <= 0 ) return ERROR_CASS_INPATH; // 2012.04.01
			}
			//
			if ( cso <= 0 ) cso = csi; // 2012.04.01
			//
			if ( carrblank ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					_i_SCH_CLUSTER_Set_User_PathIn( CHECKING_SIDE , i , 0 );
					_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , 0 );
				}
				_In_SL_1[CHECKING_SIDE] = 1;
				_In_SL_2[CHECKING_SIDE] = MAX_CASSETTE_SLOT_SIZE;
				//
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE( csi ) ) {
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						_i_SCH_CLUSTER_Set_User_PathIn( CHECKING_SIDE , i , csi );
						if ( _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , i ) <= 0 ) {
							return ERROR_CASS_INPATH;
						}
					}
				}
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( cso ) ) {
					//
//					if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && // 2013.06.26
					if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() < 2 ) && // 2013.06.26 // 2018.06.29
						( ( _i_SCH_SYSTEM_USING_GET( cso - CM1 ) > 0 ) || ( ( _i_SCH_SYSTEM_USING_GET( cso - CM1 ) <= 0 ) && ( BUTTON_GET_FLOW_STATUS_VAR( cso - CM1 ) != _MS_5_MAPPED ) ) )
						) { // 2012.01.03
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( cso ) > 0 ) { // 2016.06.17
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , -100 - cso );
								//
							}
							//
						}
						else { // 2016.06.17
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
								//
								if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) <= 0 ) {
									return ERROR_CASS_OUTPATH;
								}
							}
							//
						}
					}
					else if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) && ( csi != cso ) && ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() >= 2 ) ) { // 2018.06.29
						//
						if ( BUTTON_GET_FLOW_STATUS_VAR( cso - CM1 ) != _MS_5_MAPPED ) {
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , -100 - cso );
								//
							}
							//
						}
						else { // 2019.01.09
							//
							for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
								//
								_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
								//
							}
							//
						}
						//
					}
					else {
						//
						for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
							//
							_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , cso );
							//
							if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) <= 0 ) {
								return ERROR_CASS_OUTPATH;
							}
						}
					}
				}
				//=====================================================================================
				//
				MaxSlot = Scheduler_Get_Max_Slot( CHECKING_SIDE , csi , cso , 6 , &maxunuse ); // 2010.12.17
				//
				//=====================================================================================
				_In_SL_1[CHECKING_SIDE] = _i_SCH_IO_GET_START_SLOT( CHECKING_SIDE );
				//
				if ( csi >= BM1 ) {
					if ( _In_SL_1[CHECKING_SIDE] <= 0 || _In_SL_1[CHECKING_SIDE] > _i_SCH_PRM_GET_MODULE_SIZE( csi ) ) {
						return ERROR_CASS_STARTRANGE;
					}
				}
				else {
					if ( _In_SL_1[CHECKING_SIDE] <= 0 || _In_SL_1[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
						return ERROR_CASS_STARTRANGE;
					}
				}
				_In_SL_2[CHECKING_SIDE] = _i_SCH_IO_GET_END_SLOT( CHECKING_SIDE );
				//
				if ( csi >= BM1 ) {
					if ( _In_SL_2[CHECKING_SIDE] <= 0 || _In_SL_2[CHECKING_SIDE] > _i_SCH_PRM_GET_MODULE_SIZE( csi ) ) {
						return ERROR_CASS_ENDRANGE;
					}
				}
				else {
					if ( _In_SL_2[CHECKING_SIDE] <= 0 || _In_SL_2[CHECKING_SIDE] > MaxSlot ) { // 2010.12.17
						return ERROR_CASS_ENDRANGE;
					}
				}
				//
				if ( ( cso >= BM1 ) && ( csi != cso ) ) {
					if ( _In_SL_2[CHECKING_SIDE] <= 0 || _In_SL_2[CHECKING_SIDE] > _i_SCH_PRM_GET_MODULE_SIZE( cso ) ) {
						return ERROR_CASS_ENDRANGE;
					}
				}
				//
				if ( _In_SL_1[CHECKING_SIDE] > _In_SL_2[CHECKING_SIDE] ) {
					return ERROR_CASS_RANGE;
				}
			}
			//
	//		_i_SCH_IO_GET_RECIPE_FILE( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] ); // 2010.03.10
			_i_SCH_IO_GET_RECIPE_FILE( ( csi >= BM1 ) ? ( CHECKING_SIDE + 100 ) : CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] ); // 2010.03.10
			//
			if ( strlen( _In_RecipeFileName[CHECKING_SIDE] ) <= 0 ) {
				return ERROR_RECIPE_SELECT;
			}
		}
		else { // 2010.10.12
			strcpy( _In_RecipeFileName[CHECKING_SIDE] , "" );
		}
		//================================================================================
		if ( minicheck_StartJ ) return ERROR_NONE; // 2008.04.23
		//================================================================================
		if ( PreCheking == 2 /*BM_MODE_CHECK*/ ) return TRUE; // 2010.02.20
		//================================================================================
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) return ERROR_NONE; // 2010.10.12 // 2017.10.10
		if ( _SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) return ERROR_NONE; // 2010.10.12 // 2017.10.10
		//================================================================================

//		A->	A	A->	NotA
//------------------------
//		1	1	1	1
//		1	1	1	25
//		1	1	25	1
//		1	1	25	25
//		25	25	1	1
//		25	25	1	25
//		25	25	25	1
//		25	25	25	25
//		"First:First|First:First.Rev|First:Last.Rev|First:Last|Last:First|Last:First.Rev|Last:Last.Rev|Last:Last"
		//================================================================================
		if ( !carrblank ) { // 2012.04.27
			if ( csi == cso ) { // 2003.06.23
				switch( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() ) {
				case  4 : // Last
				case  5 :
				case  6 :
				case  7 :
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				default : // First
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				}
			}
			else {
				switch( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() ) {
				case  1 : // First.Rev
				case  5 :
					for ( i = 0 ; i < ( _In_SL_1[CHECKING_SIDE] - 1 ); i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					for ( i = ( _In_SL_1[CHECKING_SIDE] - 1 ) ; i < _In_SL_2[CHECKING_SIDE] ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = _In_SL_2[CHECKING_SIDE] + ( _In_SL_1[CHECKING_SIDE] - 1 ) - i;
					}
					for ( i = _In_SL_2[CHECKING_SIDE] ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				case  2 : // Last.Rev
				case  6 :
					for ( i = 0 ; i < ( _In_SL_1[CHECKING_SIDE] - 1 ); i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					for ( i = ( _In_SL_1[CHECKING_SIDE] - 1 ) ; i < _In_SL_2[CHECKING_SIDE] ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = _In_SL_2[CHECKING_SIDE] + ( _In_SL_1[CHECKING_SIDE] - 1 ) - i;
					}
					for ( i = _In_SL_2[CHECKING_SIDE] ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				case  3 : // Last
				case  7 :
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = ( MAX_CASSETTE_SLOT_SIZE - 1 ) - i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				default : // First
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						CASSETTE.CASSETTE_IN_ORDER[i] = i + 1;
						CASSETTE.CASSETTE_OUT_PATH[i] = _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , i ) - 1;
						CASSETTE.CASSETTE_OUT_SLOT[i] = i + 1;
					}
					break;
				}
			}
			if ( !USER_RECIPE_MANUAL_INOUT_DATA_READ( CHECKING_SIDE , CHECKING_SIDE , &CASSETTE ) ) {
				return ERROR_CASS_OUTPATH;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( CASSETTE.CASSETTE_IN_ORDER[i] <= -999999 ) { // 2007.07.11
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , -999999 ); // 2007.07.11
				}
				else {
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , CASSETTE.CASSETTE_IN_ORDER[i] - 1 );
				}
				_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_PATH[i] + 1 );
				_i_SCH_CLUSTER_Set_User_SlotOut( CHECKING_SIDE , i , CASSETTE.CASSETTE_OUT_SLOT[i] );
			}
			//
			//================================================================================
			//
			// 2019.02.26
			//
			if ( EXTEND_SOURCE_USE != 0 ) {
				_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , _i_SCH_IO_GET_MODULE_SOURCE_E( csi , -1 ) );
			}
			else {
				_i_SCH_SYSTEM_ESOURCE_ID_SET( CHECKING_SIDE , 0 );
			}
			//
			//================================================================================
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2013.04.01
				if ( ( csi >= CM1 ) && ( csi < PM1 ) ) { // 2019.02.26
					if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( csi - CM1 ) == 1 ) {
						IO_GET_MID_NAME_FROM_READ( csi - CM1 , sBuffer );
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , sBuffer );
					}
					else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( csi - CM1 ) == 2 ) {
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , "" );
					}
					else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(csi - CM1) == 3 ) {
						IO_GET_MID_NAME( csi - CM1 , sBuffer );
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , sBuffer );
					}
				}
			}
			else {
				if ( csi < PM1 ) { // 2010.01.19
					if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( CHECKING_SIDE ) == 1 ) {
						IO_GET_MID_NAME_FROM_READ( CHECKING_SIDE , sBuffer );
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , sBuffer );
					}
					else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( CHECKING_SIDE ) == 2 ) {
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , "" );
					}
					else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(CHECKING_SIDE) == 3 ) {
						IO_GET_MID_NAME( CHECKING_SIDE , sBuffer );
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , sBuffer );
					}
				}
			}
		}
		else {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , i );
				_i_SCH_CLUSTER_Set_User_PathOut( CHECKING_SIDE , i , 0 );
				_i_SCH_CLUSTER_Set_User_SlotOut( CHECKING_SIDE , i , i + 1 );
			}
		}
		//
	}
//	if ( _i_SCH_IO_GET_LOOP_COUNT( CHECKING_SIDE ) == 0 ) _i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , 1 ); // 2007.03.28
	if ( _i_SCH_IO_GET_LOOP_COUNT( CHECKING_SIDE ) <= 0 ) _i_SCH_IO_SET_LOOP_COUNT( CHECKING_SIDE , 1 ); // 2007.03.28
	LOG_MTL_SET_DONE_CARR_COUNT( CHECKING_SIDE , 0 );
	LOG_MTL_SET_DONE_WFR_COUNT( CHECKING_SIDE , 0 );
	//================================================================================
	// 2007.07.11
	//================================================================================
	if ( !carrblank && _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
		//
		ss = 0;
		cs = 0;
		es = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			os = _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , i );
			if ( os <= -999999 ) {
				if ( ss == 0 ) {
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , -999990 );
				}
				else {
					es++;
					cs++;
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , cs );
				}
			}
			else {
				ss = 1;
				//
				if ( ( os + es ) < cs ) return ERROR_CASS_RANGE;
				//
				cs = os + es;
				_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , cs );
			}
			//
		}
	}
	/*
	// 2009.04.30 updating return
	if ( _SCH_COMMON_EXTEND_FM_ARM_CROSS_FIXED( CHECKING_SIDE ) ) {
		//
		for ( j = 0 ; j < 2 ; j++ ) { // 2009.04.30
			ss = 0;
			cs = 0;
			es = 0;
			//
//			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2009.04.30
//			for ( i = MAX_CASSETTE_SLOT_SIZE - 1 ; i >= 0 ; i-- ) { // 2009.04.30
			//
			csi = 0;
			//
			i = ( j == 0 ) ? 0 : MAX_CASSETTE_SLOT_SIZE - 1;
			//
			while ( TRUE ) { // 2009.04.30
				os = _i_SCH_CLUSTER_Get_User_OrderIn( CHECKING_SIDE , i );
				if ( os <= -999999 ) {
					if ( ss == 0 ) {
						_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , -999990 );
					}
					else {
						es++;
						cs++;
						_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , cs );
					}
				}
				else {
					ss = 1;
					//
//					if ( ( os + es ) < cs ) return ERROR_CASS_RANGE; // 2009.04.30
					if ( ( os + es ) < cs ) { // 2009.04.30
						csi = 1;
						break;
					}
					//
					cs = os + es;
					_i_SCH_CLUSTER_Set_User_OrderIn( CHECKING_SIDE , i , cs );
				}
				//
				if ( j == 0 ) {
					i++;
					if ( i >= MAX_CASSETTE_SLOT_SIZE ) break;
				}
				else {
					i--;
					if ( i < 0 ) break;
				}
				//
			}
			//
			if ( csi == 0 ) {
				break;
			}
			else {
				if ( j == 1 ) return ERROR_CASS_RANGE;
			}
		}
	}
	*/
	//================================================================================
	return ERROR_NONE;
}
//===================================================================================================
BOOL Scheduler_Ordering_has_same_Path( int side , int side2 ) { // 2018.08.13
	int i;
	//
	if ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() < 9 ) return TRUE;
	//
	for ( i = PM1 ; i < AL ; i++ ) {
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) {
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side2 , i ) == MUF_01_USE ) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

int Scheduler_Ordering_Checking( int side , BOOL checkcancel ) {
	int  _In_cLoopm[MAX_CASSETTE_SIDE];
	//
	_In_cLoopm[side] = 0;
	while( _In_cLoopm[side] < MAX_CASSETTE_SIDE ) {
		if ( _In_cLoopm[side] != side ) {
			if ( _i_SCH_SYSTEM_USING_GET( _In_cLoopm[side] ) > 0 ) {
				if ( _i_SCH_SYSTEM_RUNORDER_GET( _In_cLoopm[side] ) < _i_SCH_SYSTEM_RUNORDER_GET( side ) ) {

//					if ( _i_SCH_SYSTEM_USING_GET( _In_cLoopm[side] ) <= _i_SCH_SYSTEM_USING_GET( side ) ) { // 2008.04.08
					if ( _i_SCH_SYSTEM_USING_GET( _In_cLoopm[side] ) < _i_SCH_SYSTEM_USING_GET( side ) ) { // 2008.04.08
						//
						if ( Scheduler_Ordering_has_same_Path( side , _In_cLoopm[side] ) ) { // 2018.08.13
							//
							_In_cLoopm[side] = -1;
							Sleep(100);
							//
						}
					}
					else if ( _i_SCH_SYSTEM_USING_GET( _In_cLoopm[side] ) == _i_SCH_SYSTEM_USING_GET( side ) ) { // 2008.04.08
						//
						if ( BUTTON_GET_FLOW_STATUS_VAR( _In_cLoopm[side] ) <= BUTTON_GET_FLOW_STATUS_VAR( side ) ) { // 2008.04.08
							//
							if ( Scheduler_Ordering_has_same_Path( side , _In_cLoopm[side] ) ) { // 2018.08.13
								//
								_In_cLoopm[side] = -1;
								Sleep(100);
							}
							//
						}
					}
				}
			}
		}
		_In_cLoopm[side]++;
		if ( checkcancel ) {
//			if ( _In_cLoopm[side] == 0 ) { // 2008.04.01
				if ( _i_SCH_SYSTEM_USING_GET( side ) >= 100 ) return 0;
//			} // 2008.04.01
		}
//		if ( SIGNAL_MODE_ABORT_GET( side ) ) return -1; // 2016.01.12
		if ( SIGNAL_MODE_ABORT_GET( side ) > 0 ) return -1; // 2016.01.12
	}
	return 1;
}

//===================================================================================================

int Scheduler_Run_Gui_And_Job_Code( int CHECKING_SIDE , BOOL mappingskip , int StartStyle , int movemode ) { // 2008.04.23
	int res;
	//
	// StartStyle
		// P = 100
		// S = 200
		// O = 300
		//
		// E = 10
		// N = 1000

	res = Scheduler_InfoCheck( CHECKING_SIDE , 0 , 0 , 0 , TRUE );
	if ( res != ERROR_NONE ) {
		ERROR_HANDLER( res , "" );
		return res;
	}
	//
	if ( _In_SL_2[CHECKING_SIDE] > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) _In_SL_2[CHECKING_SIDE] = _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT(); // 2013.04.02
	//
//	res = SCHMULTI_PROCESSJOB_DIRECT_START_INSERT( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) - 1 , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , "" , mappingskip , StartStyle ); // 2011.12.14
	res = SCHMULTI_PROCESSJOB_DIRECT_START_INSERT( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) - 1 , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , SCHMULTI_CASSETTE_MID_GET( CHECKING_SIDE ) , mappingskip , StartStyle , movemode ); // 2011.12.14
	//
	if ( res != 0 ) {
		ERROR_HANDLER( ERROR_JOB_PARAM_ERROR , "" );
		return res;
	}
	//
	return ERROR_NONE;
}

//===================================================================================================
extern CLUSTERStepTemplate		G_CLUSTER_RECIPE_INF; // 2008.07.24
//
extern CLUSTERStepTemplate2		G_CLUSTER_RECIPE; // 2008.07.24
//===================================================================================================
void ERROR_REPORTING( int CHECKING_SIDE , int tag , BOOL cruse , int sd ) { // 2010.03.26
	//=================================================================================================
	char Buffer[256];
	char Buffer2[256];
	//=================================================================================================
	if ( cruse ) EnterCriticalSection( &CR_MAIN );
	//==========================================================================
	sprintf( Buffer , "%s/LOTLOG/%s/Error.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
	//
	sprintf( Buffer2 , "ERROR_RESULT[%d]" , tag );
	//
	SCHEDULING_DATA_STRUCTURE_LOG( Buffer , Buffer2 , -1 , 0 , sd );
	SCHEDULING_DATA_STRUCTURE_JOB_LOG( Buffer , Buffer2 );
	//
	sprintf( Buffer , "%s/LOTLOG/%s/Error.jpg" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
	sprintf( Buffer2 , "%s/LOTLOG/%s/Error.cil" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
	KWIN_SCREEN_CAPTURE( GET_WINDOW_X_REFERENCE_POSITION() , GET_WINDOW_Y_REFERENCE_POSITION() , GET_WINDOW_XS_REFERENCE_POSITION() , GET_WINDOW_YS_REFERENCE_POSITION() , Buffer );
	rename( Buffer , Buffer2 );
	//==========================================================================
	_SCH_COMMON_SYSTEM_CONTROL_FAIL( TRUE , CHECKING_SIDE , tag , cruse , sd ); // 2017.07.26
	//==========================================================================
	if ( cruse ) LeaveCriticalSection( &CR_MAIN );
	//==========================================================================
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//int Scheduler_Run_Main_Operation_Code( int CHECKING_SIDE , BOOL FirstRun , int *Mode , char *directory , int CPJOB , BOOL *EndLock , BOOL *oncerun , char *errorstring ) { // 2009.04.13
//int Scheduler_Run_Main_Operation_Code( int CHECKING_SIDE , BOOL FirstRun , int *Mode , char *directory , int CPJOB , BOOL *oncerun , char *errorstring ) { // 2009.04.13 // 2011.05.19
int Scheduler_Run_Main_Operation_Code( int CHECKING_SIDE , BOOL FirstRun , int *Mode , int CPJOB , BOOL *oncerun , char *errorstring ) { // 2009.04.13 // 2011.05.19
	char Buffer[256];
//	char Buffer2[256];
	CASSETTEVerTemplate MAPPING_INFO;
	//
//	CLUSTERStepTemplate CLUSTER_INFO; // 2004.12.14 // 2008.07.24
//	CLUSTERStepTemplate2 CLUSTER_INFO2; // 2007.04.25 // 2008.07.24
	//
	BOOL Result[MAX_CASSETTE_SIDE];
	int rck[MAX_CASSETTE_SIDE];
	int loopcnt[MAX_CASSETTE_SIDE];
//	int pralskip[MAX_CASSETTE_SIDE]; // 2006.03.02 // 2009.08.05
	int adaptprcs[MAX_CASSETTE_SIDE]; // 2008.04.01
	int TMATM , MAPVER , CANCELED;
	//---------------------------------------
	int OrderIndex; // 2005.02.17
	//---------------------------------------
	CLUSTERStepTemplate4 CLUSTER_INFO4; // 2007.04.25
	//---------------------------------------
	int dummyint;
	int verres; // 2011.07.28
	//---------------------------------------
	int incm[MAX_CASSETTE_SIDE] , outcm[MAX_CASSETTE_SIDE];
//	int used; // 2011.05.07
	int curres; // 2012.01.12
	//==========================================================================================================================
	Scheduler_System_Log( CHECKING_SIDE , "START" , 0 , 0 ); // 2006.02.02
	//==========================================================================================================================
	_In_Run_Finished_Check[CHECKING_SIDE] = 0; // 2011.04.18
	//
	_In_Job_HandOff_In_Pre_Check[CHECKING_SIDE] = 0; // 2011.04.18
	_In_Job_HandOff_Out_Message_Skip[CHECKING_SIDE] = 0; // 2011.09.19
	//	
	CANCELED = FALSE; // 2008.03.13
	*Mode = 0;
//	*EndLock = FALSE; // 2009.04.13
	*oncerun = FALSE; // 2007.06.28
	//
	_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
	//==========================================================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 0 ); // 2015.07.30
	//===========================================================================
	Scheduler_Mapping_Buffer_Data_Reset( CHECKING_SIDE ); // 2005.03.02
	//===========================================================================
	//
/*
// 2008.04.03
	for ( Result[CHECKING_SIDE] = CM1 ; Result[CHECKING_SIDE] <= FM ; Result[CHECKING_SIDE]++ ) {
		if ( PROCESS_MONITOR_Get_Status( CHECKING_SIDE , Result[CHECKING_SIDE] ) < 0 ) { // 2002.08.27
			PROCESS_MONITOR_Set_Status( CHECKING_SIDE , Result[CHECKING_SIDE] , 0 );
		} // 2002.08.27
	}
*/
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) {
		//-----------------------------------------------------------
		ERROR_REPORTING( CHECKING_SIDE , 110 , TRUE , 0 ); // 2010.03.26
		//-----------------------------------------------------------
		Scheduler_Fail_Finish_Check( CHECKING_SIDE , -1 , FALSE ); // 2007.07.31
		//-----------------------------------------------------------
		return ERROR_SYSTEM_ERROR;
	}

	//===============================================================================================================
	// 2005.08.25
	//===============================================================================================================
//	Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 0 ); // 2010.07.29
	Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 1 ); // 2010.07.29
	//==============================================================================================
	if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) == 0 ) { // 2013.09.03
		loopcnt[ CHECKING_SIDE ] = _i_SCH_IO_GET_LOOP_COUNT( CHECKING_SIDE ); // 2003.12.04 // 2008.10.30
	}
	else {
		loopcnt[ CHECKING_SIDE ] = 1;
	}
	//==============================================================================================
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 4 );
	//-------------------------------------------
//	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
//	if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07 // 2010.01.19 // 2011.09.23
	if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) || ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) ) { // 2004.09.07 // 2010.01.19 // 2011.09.23
		EnterCriticalSection( &CR_MAIN );
		//
//		Result[CHECKING_SIDE] = Scheduler_CONTROL_RECIPE_SETTING( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , &TMATM , CPJOB , FALSE , errorstring , loopcnt[ CHECKING_SIDE ] ); // 2014.05.13
		Result[CHECKING_SIDE] = Scheduler_CONTROL_RECIPE_SETTING( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , &TMATM , CPJOB , FALSE , errorstring , &(loopcnt[ CHECKING_SIDE ]) ); // 2014.05.13
		//
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 1 ); // 2015.07.30
		//
		if ( Result[CHECKING_SIDE] == ERROR_NONE ) {
			if ( _SCH_COMMON_USER_RECIPE_CHECK( 111 , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2012.01.12
				Result[CHECKING_SIDE] = curres;
			}
		}
		//
		LeaveCriticalSection( &CR_MAIN );
		//
		if ( Result[CHECKING_SIDE] != ERROR_NONE ) {
			//==========================================================================================================================
			ERROR_REPORTING( CHECKING_SIDE , 120 , TRUE , Result[CHECKING_SIDE] ); // 2010.03.26
			//==========================================================================================================================
			Scheduler_System_Log( CHECKING_SIDE , "FAIL_RECIPE" , 0 , Result[CHECKING_SIDE] ); // 2009.04.17
			//==========================================================================================================================
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , 2 , ( ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED ) || ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED2 ) ) ); // 2007.07.31
			//-----------------------------------------------------------
			return Result[CHECKING_SIDE];
		}
	}
	else {
		EnterCriticalSection( &CR_MAIN );
		_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 0 );
		TMATM = 0;
		//
/*
// 2009.08.05
		for ( Result[CHECKING_SIDE] = 0 ; Result[CHECKING_SIDE] < _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; Result[CHECKING_SIDE]++ ) {
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( BM1 + Result[CHECKING_SIDE] ) > 0 ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , BM1 + Result[CHECKING_SIDE] , MUF_01_USE );
			}
			else {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , BM1 + Result[CHECKING_SIDE] , MUF_00_NOTUSE );
			}
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( TM + Result[CHECKING_SIDE] ) > 0 ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + Result[CHECKING_SIDE] , MUF_01_USE );
			}
			else {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , TM + Result[CHECKING_SIDE] , MUF_00_NOTUSE );
			}
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( PM1 + Result[CHECKING_SIDE] ) > 0 ) {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , PM1 + Result[CHECKING_SIDE] , MUF_01_USE );
			}
			else {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , PM1 + Result[CHECKING_SIDE] , MUF_00_NOTUSE );
			}
		}
*/
		// 2009.08.05
		for ( Result[CHECKING_SIDE] = PM1 ; Result[CHECKING_SIDE] < FM ; Result[CHECKING_SIDE]++ ) {
			if ( ( Result[CHECKING_SIDE] == AL ) || ( Result[CHECKING_SIDE] == IC ) ) continue;
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Result[CHECKING_SIDE] ) > 0 ) {
				//
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Result[CHECKING_SIDE] , MUF_01_USE );
				//
				if ( Result[CHECKING_SIDE] >= TM ) TMATM = Result[CHECKING_SIDE] - TM;
			}
			else {
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Result[CHECKING_SIDE] , MUF_00_NOTUSE );
			}
		}
		//
		_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , FM , MUF_01_USE );
		LeaveCriticalSection( &CR_MAIN );
	}
	//
	/*
	// 2011.06.10
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() != 0 ) { // 2011.05.21
		//
//		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 0 ) SYSTEM_IN_MODULE_SET( CHECKING_SIDE , -1 ); // 2011.05.30
		//
//		if ( CPJOB == 1 ) { // Job // 2011.05.30
			//
			EnterCriticalSection( &CR_MAIN );
			//
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , -1 );
			//
			LeaveCriticalSection( &CR_MAIN );
			//
//		} // 2011.05.30
		//
	}
	*/
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//
	strcpy( errorstring , "" ); // 2008.03.26
	//
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) {
		//-----------------------------------------------------------
		ERROR_REPORTING( CHECKING_SIDE , 130 , TRUE , 0 ); // 2010.03.26
		//-----------------------------------------------------------
		Scheduler_Fail_Finish_Check( CHECKING_SIDE , 2 , FALSE ); // 2007.07.31
		//-----------------------------------------------------------
		return ERROR_SYSTEM_ERROR;
	}
//	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 5 ); // 2008.03.21
	//-------------------------------------------
	//==========================================================================================================================
	Scheduler_System_Log( CHECKING_SIDE , "AFTER_RECIPE" , 0 , 0 ); // 2006.02.02
	//==========================================================================================================================
	// 2008.02.29
	//===============================================================================================================
/*
// 2008.04.01
	switch( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ) {
	case 1 :
	case 2 :
		while( TRUE ) {
			//================================================================================
			EnterCriticalSection( &CR_MAIN ); // 2008.03.21
			//================================================================================
			if ( Scheduler_Main_Waiting( CHECKING_SIDE , TRUE , &(pralskip[CHECKING_SIDE]) , 0 ) != SYS_ABORTED ) {
				//-----------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN ); // 2008.03.21
				//-----------------------------------------------------------
				break;
			}
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( CHECKING_SIDE ) ) {
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , 2 ); // 2007.07.31
				//-----------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN ); // 2008.03.21
				//-----------------------------------------------------------
				return ERROR_SYSTEM_ERROR;
			}
			if ( _i_SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , 2 ); // 2007.07.31
				//-----------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN ); // 2008.03.21
				//-----------------------------------------------------------
				return ERROR_SYSTEM_ERROR;
			}
			//================================================================================
			LeaveCriticalSection( &CR_MAIN ); // 2008.03.28
			//================================================================================
			Sleep(250);
		}
		break;
	}
*/
	//---------------------------------------------------------------------------------
	// 2008.04.01
	//---------------------------------------------------------------------------------
	switch( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ) {
	case 1 :
	case 2 :
	case 5 : // 2010.12.17
	case 6 : // 2010.12.17
		adaptprcs[CHECKING_SIDE] = TRUE;
		break;
	default :
		adaptprcs[CHECKING_SIDE] = FALSE;
		break;
	}
	//---------------------------------------------------------------------------------
	//===============================================================================================================
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 5 ); // 2008.03.21
	//===============================================================================================================
	// 2005.02.17
	//===============================================================================================================
	_In_Loop[CHECKING_SIDE] = 0; // 2009.07.24
	//===============================================================================================================
//	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2009.08.05 // 2010.01.19
	if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2009.08.05 // 2010.01.19
		if ( USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() ) {
			//================================================================================
			EnterCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
			OrderIndex = 0;
			for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
				SCHEDULER_CONTROL_RunData_Make_Verification_Info( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE_INF , &CLUSTER_INFO4 );
				if ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE != -1 ) OrderIndex++;
				//
				verres = USER_RECIPE_INFO_VERIFICATION_EX( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , G_CLUSTER_RECIPE_INF , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA );
				//
				if ( verres != 1 ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 140 , FALSE , verres ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , ( verres == -2 ) ); // 2007.07.31
					//-----------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
					//-----------------------------------------------------------
					if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
					if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
					return ERROR_SYSTEM_ERROR;
				}
			}
			//================================================================================
			LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
		}
		//===============================================================================================================
		if ( USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() ) {
			//================================================================================
			EnterCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
			OrderIndex = 0;
			for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
				SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
				if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) OrderIndex++;
				//
				verres = USER_RECIPE_INFO_VERIFICATION_EX2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
				//
				if ( verres != 1 ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 150 , FALSE , verres ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , ( verres == -2 ) ); // 2007.07.31
					//-----------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
					//-----------------------------------------------------------
					if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
					if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
					return ERROR_SYSTEM_ERROR;
				}
			}
			//================================================================================
			LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
		}
		//===============================================================================================================
		//===============================================================================================================
		if ( USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() ) {
			//================================================================================
			EnterCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
			OrderIndex = 0;
			for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
				SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
				if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) OrderIndex++;
				//
				verres = USER_RECIPE_INFO_VERIFICATION_EX3( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
				//
				if ( verres != 1 ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 160 , FALSE , verres ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , ( verres == -2 ) ); // 2007.07.31
					//-----------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
					//-----------------------------------------------------------
					if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
					if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
					return ERROR_SYSTEM_ERROR;
				}
			}
			//================================================================================
			LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
			//================================================================================
		}
	}
	//===============================================================================================================
	//===============================================================================================================
//	Scheduler_Regist_Lot_PrePost_Recipe( CHECKING_SIDE ); // 2005.02.17 // 2005.07.18
	//===============================================================================================================
	//===============================================================================================================
	BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_8_HANDOFFIN ); // 2013.06.08
	//
//	if ( FirstRun ) { // 2004.09.07
//	if ( FirstRun && _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07 // 2010.01.19
//	if ( FirstRun && SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07 // 2010.01.19 // 2013.04.11
	if ( FirstRun && ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) || ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) ) ) { // 2004.09.07 // 2010.01.19 // 2013.04.11
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
		if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.05.21
				//
				if ( Scheduler_HandOffIn_Ready_Part( CHECKING_SIDE , -1 ) == SYS_ABORTED ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 170 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
					//-----------------------------------------------------------
					return ERROR_HANDOFF_IN_FAILURE;
				}
				//
			}
			else {
				//
				if ( Scheduler_HandOffIn_Part( CHECKING_SIDE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) ) == SYS_ABORTED ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 170 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
					//-----------------------------------------------------------
					return ERROR_HANDOFF_IN_FAILURE;
				}
				//
			}
		}
		//
		EnterCriticalSection( &CR_MAIN );
	
		SCHEDULER_CONTROL_MapData_Make_Verification_Info( CHECKING_SIDE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , &MAPPING_INFO , 1 );

		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
			if ( !USER_RECIPE_MAPPING_VERIFICATION( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , MAPPING_INFO ) ) {
				//-----------------------------------------------------------
				ERROR_REPORTING( CHECKING_SIDE , 180 , FALSE , 0 ); // 2010.03.26
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
				//-----------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				//-----------------------------------------------------------
				return ERROR_CASS_MAP;
			}
		}
		else {
			if ( !USER_RECIPE_MANUAL_MAPPING_VERIFICATION( CHECKING_SIDE , CHECKING_SIDE , 0 , MAPPING_INFO ) ) {
				//-----------------------------------------------------------
				ERROR_REPORTING( CHECKING_SIDE , 190 , FALSE , 0 ); // 2010.03.26
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
				//-----------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				//-----------------------------------------------------------
				return ERROR_CASS_MAP;
			}
		}
		LeaveCriticalSection( &CR_MAIN );
	}
	//
	BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_9_BEGIN ); // 2013.06.08
	//
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) {
		//-----------------------------------------------------------
		ERROR_REPORTING( CHECKING_SIDE , 200 , TRUE , 0 ); // 2010.03.26
		//-----------------------------------------------------------
		Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
		//-----------------------------------------------------------
		return ERROR_SYSTEM_ERROR;
	}
	//===============================================================================================================
	// 2008.04.03
	//===============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) { // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) { // 2016.01.12
		//-----------------------------------------------------------
		ERROR_REPORTING( CHECKING_SIDE , 210 , TRUE , 0 ); // 2010.03.26
		//-----------------------------------------------------------
		Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , TRUE );
		//-----------------------------------------------------------
		return ERROR_SYSTEM_ABORTED2;
	}
	//==========================================================================================================================
//	SYSTEM_BEGIN_SET( CHECKING_SIDE , 0 ); // 2007.05.04 // 2015.07.30
	//===============================================================================================================
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 6 );
	//==========================================================================================================================
	SCHEDULING_LOT_PREDICT_TIME_CHECK( CHECKING_SIDE ); // 2006.09.19
	//==========================================================================================================================
	Scheduler_System_Log( CHECKING_SIDE , "AFTER_HANDOFFIN" , 0 , 0 ); // 2006.02.02
	//==========================================================================================================================
	if ( FirstRun ) {
		if ( !adaptprcs[CHECKING_SIDE] ) { // 2008.04.01
			*Mode = 1;
			//
			if ( SCHEDULER_SIDE_IN_POSSIBLE( 2 , CHECKING_SIDE ) ) { // 2005.09.22
				//
//				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 0 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , directory ); // 2011.05.19
				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 0 , _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2011.05.19
				//
			}
			else { // 2005.09.22
				//
//				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 1 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , directory ); // 2011.05.19
				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 1 , _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2011.05.19
				//
			}
			//================================================================================
			strcpy( errorstring , "" ); // 2008.04.14
			//================================================================================
			if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.04.20 // 2011.09.23
				//
				if ( Scheduler_Begin_Part( CHECKING_SIDE , errorstring ) == SYS_ABORTED ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 220 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE ); // 2007.07.31
					//-----------------------------------------------------------
					return ERROR_READY_FAILURE;
				}
			}
			else { // 2011.05.30
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) {
					SYSTEM_IN_MODULE_SET( CHECKING_SIDE , -1 );
				}
			}
			//================================================================================
			strcpy( errorstring , "" ); // 2008.04.14
			//================================================================================
		}
		//
		if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) {
		}
		else {
			if ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) > 0 ) { // 2013.06.20
				//
				if ( Scheduler_Begin_Restart_Part( CHECKING_SIDE , errorstring ) == SYS_ABORTED ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 220 , TRUE , 0 );
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE );
					//-----------------------------------------------------------
					return ERROR_READY_FAILURE;
				}
			}
		}
		//
	}
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) { // 2010.10.28
		//-----------------------------------------------------------
		ERROR_REPORTING( CHECKING_SIDE , 221 , TRUE , 0 );
		//-----------------------------------------------------------
		Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , FALSE );
		//-----------------------------------------------------------
		return ERROR_SYSTEM_ERROR;
	}
	//==========================================================================================================================
//	SYSTEM_BEGIN_SET( CHECKING_SIDE , 1 ); // 2007.05.04 // 2010.04.23
	//==========================================================================================================================
	//==========================================================================================================================
	Scheduler_System_Log( CHECKING_SIDE , "AFTER_BEGIN" , 0 , 0 ); // 2006.02.02
	//==========================================================================================================================
	CANCELED = FALSE; // 2008.03.13
	//
//	_i_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , FALSE ); // 2003.12.04
//	loopcnt[ CHECKING_SIDE ] = _i_SCH_IO_GET_LOOP_COUNT( CHECKING_SIDE ); // 2003.12.04 // 2008.10.30
	//
	if ( loopcnt[ CHECKING_SIDE ] <= 1 ) { // 2003.12.04
		_i_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 1 ); // 2003.12.04
	} // 2003.12.04
	else { // 2003.12.04
		_i_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 0 ); // 2003.12.04
	} // 2003.12.04
	//
	for ( _In_Loop[CHECKING_SIDE] = 0 ; _In_Loop[CHECKING_SIDE] < loopcnt[ CHECKING_SIDE ] ; _In_Loop[CHECKING_SIDE]++ ) {
		if ( _In_Loop[CHECKING_SIDE] != 0 ) {
			//==============================================================================================
			// 2005.06.15
			//==============================================================================================
			if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
				SYSTEM_MODE_FIRST_SET( CHECKING_SIDE , FALSE );
				_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , SYSTEM_MODE_FIRST_GET_BUFFER( CHECKING_SIDE ) );
			}
			//==============================================================================================
			// 2003.05.22
			//==============================================================================================
			_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING ); // 2003.05.26
			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 6 );
			//
			Result[CHECKING_SIDE] = _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE );
			for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SIDE ; rck[CHECKING_SIDE]++ ) {
				if ( rck[CHECKING_SIDE] != CHECKING_SIDE ) {
					if ( _i_SCH_SYSTEM_USING_GET( rck[CHECKING_SIDE] ) > 0 ) {
						if ( _i_SCH_SYSTEM_RUNORDER_GET( rck[CHECKING_SIDE] ) > Result[CHECKING_SIDE] ) {
							Result[CHECKING_SIDE] = _i_SCH_SYSTEM_RUNORDER_GET( rck[CHECKING_SIDE] );
							_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , Result[CHECKING_SIDE] + 1 );
						}
					}
				}
			}
			//==============================================================================================
			if ( ( _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() / 2 ) == 1 ) { // 2012.04.12
				//
				sprintf( Buffer , "%s\\[LOOP%d]" , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY_LOOP( CHECKING_SIDE ) , _In_Loop[CHECKING_SIDE] + 1 );
				//
				_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( CHECKING_SIDE , Buffer );
				//
				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_RESTART , 1 , _In_Loop[CHECKING_SIDE] + 1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
				IO_LOT_DIR_INITIAL( CHECKING_SIDE , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) , _i_SCH_PRM_GET_DFIX_LOG_PATH() );
				//
			}
			else {
				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_RESTART , -1 , -1 , -1 , -1 , -1 , "" , "" );
			}
			//==============================================================================================
		}
		else {
			//
			_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY_LOOP( CHECKING_SIDE , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
			//
		}
		//==============================================================================================
//		Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 1 ); // 2003.05.22 // 2010.07.29
		Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 2 ); // 2003.05.22 // 2010.07.29
		//==============================================================================================
//		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.11.12 // 2010.01.19
		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2005.11.12 // 2010.01.19
			if ( _In_Loop[CHECKING_SIDE] == 0 ) {
				//-------------------------------------------
/*
// 2008.04.01
				if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) {
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 ); // 2007.07.31
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ERROR;
				}
*/
				//-------------------------------------------
				switch( Scheduler_Ordering_Checking( CHECKING_SIDE , TRUE ) ) {
				case -1 : // aborted // 2008.03.21
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 230 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , TRUE ); // 2008.03.21
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ABORTED2; // 2008.03.21
					break;
				case 1 : // ok
					break;
				case 0 : // cancel
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 240 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , FALSE ); // 2008.03.21
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ERROR; // 2008.03.21
					break;
				}
				//-------------------------------------------
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 7 );
			}
			else {
				//-------------------------------------------
	//			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 107 ); // 2003.05.26
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 7 ); // 2003.05.26
				//-------------------------------------------
				_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE, FALSE ); // 2008.05.19 <-- 추가(PSK Debug)
				//-------------------------------------------
			}
		}
		else {
			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 7 ); // 2005.11.12
		}
		//
		BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_10_MAPIN ); // 2013.06.08
		//
//		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.11.12 // 2010.01.19
		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2005.11.12 // 2010.01.19
			//
			if ( ( _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() % 2 ) == 1 ) {
				if ( _In_Loop[CHECKING_SIDE] != 0 ) {
					//==========================================
					SYSTEM_BEGIN_SET( CHECKING_SIDE , 11 ); // 2018.08.14
					//==========================================
					*Mode = 1;
//					Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , FALSE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
					Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , FALSE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10

					EnterCriticalSection( &CR_MAIN );
					if ( Result[CHECKING_SIDE] == ERROR_NONE )
						Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 2 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
					LeaveCriticalSection( &CR_MAIN );

					if ( Result[CHECKING_SIDE] != ERROR_NONE ) {
						//-----------------------------------------------------------
						ERROR_REPORTING( CHECKING_SIDE , 250 , TRUE , 0 ); // 2010.03.26
						//-----------------------------------------------------------
						Scheduler_Fail_Finish_Check( CHECKING_SIDE , -2 , ( ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED ) || ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED2 ) ) ); // 2007.07.31
						//-----------------------------------------------------------
//						return ERROR_SYSTEM_ABORTED; // 2006.03.30
						return Result[CHECKING_SIDE]; // 2006.03.30
					}
				}
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 12 ); // 2018.08.14
				//==========================================
				*Mode = 1;
//				Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , TRUE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
				Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , TRUE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
				//
				EnterCriticalSection( &CR_MAIN );
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 13 ); // 2018.08.14
				//==========================================
				if ( _In_Loop[CHECKING_SIDE] == 0 ) {
					if ( Result[CHECKING_SIDE] == ERROR_NONE )
						Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 0 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
				}
				else {
					if ( Result[CHECKING_SIDE] == ERROR_NONE ) // Switch
						Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 1 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
				}
//				if ( _In_Loop[CHECKING_SIDE] != 0 ) { // 2006.03.30
//					if ( Result[CHECKING_SIDE] != ERROR_NONE ) Result[CHECKING_SIDE] = ERROR_SYSTEM_ABORTED; // 2006.03.30
//				} // 2006.03.30
				LeaveCriticalSection( &CR_MAIN );
			}
			else {
				if ( FirstRun ) {
					if ( _In_Loop[CHECKING_SIDE] == 0 ) {
						//==========================================
						SYSTEM_BEGIN_SET( CHECKING_SIDE , 21 ); // 2018.08.14
						//==========================================
						*Mode = 1;
//						Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , TRUE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
						Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , TRUE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
						//
						EnterCriticalSection( &CR_MAIN );
						//==========================================
						SYSTEM_BEGIN_SET( CHECKING_SIDE , 22 ); // 2018.08.14
						//==========================================
						if ( Result[CHECKING_SIDE] == ERROR_NONE )
							Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 0 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
						LeaveCriticalSection( &CR_MAIN );
					}
					else { // switch
						EnterCriticalSection( &CR_MAIN );
						//==========================================
						SYSTEM_BEGIN_SET( CHECKING_SIDE , 23 ); // 2018.08.14
						//==========================================
						Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 1 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
//						if ( Result[CHECKING_SIDE] != ERROR_NONE ) Result[CHECKING_SIDE] = ERROR_SYSTEM_ABORTED; // 2006.03.30
						LeaveCriticalSection( &CR_MAIN );
					}
				}
				else {
					EnterCriticalSection( &CR_MAIN );
					//==========================================
					SYSTEM_BEGIN_SET( CHECKING_SIDE , 24 ); // 2018.08.14
					//==========================================
					Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 0 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
//					if ( Result[CHECKING_SIDE] != ERROR_NONE ) Result[CHECKING_SIDE] = ERROR_SYSTEM_ABORTED; // 2006.03.30
					LeaveCriticalSection( &CR_MAIN );
				}
			}

			if ( Result[CHECKING_SIDE] != ERROR_NONE ) {
				//-----------------------------------------------------------
				ERROR_REPORTING( CHECKING_SIDE , 260 , TRUE , 0 ); // 2010.03.26
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED ) || ( Result[CHECKING_SIDE] == ERROR_SYSTEM_ABORTED2 ) ) ); // 2007.07.31
				//-----------------------------------------------------------
				return Result[CHECKING_SIDE];
			}

			if ( ( FirstRun ) && ( _In_Loop[CHECKING_SIDE] == 0 ) ) {
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 31 ); // 2018.08.14
				//==========================================
				EnterCriticalSection( &CR_MAIN );
				MAPVER = FALSE;
				if ( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) >= 0 ) { // 2002.03.27
					switch ( _i_SCH_PRM_GET_MAPPING_SKIP( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) ) ) {
					case 0 : case 2 : case 4 :
						MAPVER = TRUE;
						break;
					case 6 : case 7 : case 8 :
						if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) MAPVER = TRUE;
						break;
					}
				}
				if ( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) >= 0 ) { // 2002.03.27
					switch ( _i_SCH_PRM_GET_MAPPING_SKIP( SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) ) ) {
					case 0 : case 2 : case 4 :
						MAPVER = TRUE;
						break;
					case 6 : case 7 : case 8 :
						if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) MAPVER = TRUE;
						break;
					}
				}
				if ( MAPVER ) { // 2002.03.27
					//==========================================
					SYSTEM_BEGIN_SET( CHECKING_SIDE , 32 ); // 2018.08.14
					//==========================================
					SCHEDULER_CONTROL_MapData_Make_Verification_Info( CHECKING_SIDE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , &MAPPING_INFO , 2 );
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) {
						if ( !USER_RECIPE_MAPPING_VERIFICATION( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , MAPPING_INFO ) ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 270 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , FALSE ); // 2007.07.31
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							return ERROR_CASS_MAP;
						}
					}
					else {
						if ( !USER_RECIPE_MANUAL_MAPPING_VERIFICATION( CHECKING_SIDE , CHECKING_SIDE , 1 , MAPPING_INFO ) ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 280 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , FALSE ); // 2007.07.31
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							return ERROR_CASS_MAP;
						}
					}
				}
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 33 ); // 2018.08.14
				//==========================================
				//
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2013.04.01
				}
				else {
					if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(CHECKING_SIDE) == 2 ) {
						//
						IO_GET_MID_NAME_FROM_READ( CHECKING_SIDE , Buffer );
						_i_SCH_SYSTEM_SET_MID_ID( CHECKING_SIDE , Buffer );
						//
						IO_SET_MID_NAME( CHECKING_SIDE , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
						//
						_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_MID_SET , -1 , -1 , -1 , -1 , -1 , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) , "" );
					}
				}
				LeaveCriticalSection( &CR_MAIN );
			}
		}
		// 2012.10.11
		//
		EnterCriticalSection( &CR_MAIN ); // 2003.02.05
		//==========================================
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 41 ); // 2018.08.14
		//==========================================
		//
		if ( !_SCH_COMMON_USER_RECIPE_CHECK( 121 , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2012.01.12
			curres = ( Scheduler_CONTROL_RECIPE_SETTING_AFTER_MAPPING( CHECKING_SIDE , FALSE ) ) ? ERROR_NONE : ERROR_CASS_MAP; // 2013.01.17
		}
		else {
			if ( curres == ERROR_NONE ) {
				curres = ( Scheduler_CONTROL_RECIPE_SETTING_AFTER_MAPPING( CHECKING_SIDE , TRUE ) ) ? ERROR_NONE : ERROR_CASS_MAP; // 2013.01.17
			}
		}
		//
		if ( curres == ERROR_NONE ) {
			if ( !_SCH_COMMON_USER_RECIPE_CHECK( 131 , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) curres = ERROR_NONE; // 2012.01.12 // 2013.01.17
		}
		//
//		if ( !curres ) { // 2003.02.05 // 2013.01.17
		if ( curres != ERROR_NONE ) { // 2003.02.05 // 2013.01.17
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 290 , FALSE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( ( curres == ERROR_SYSTEM_ABORTED ) || ( curres == ERROR_SYSTEM_ABORTED2 ) ) ); // 2007.07.31
			//-----------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN ); // 2003.02.05
			//-----------------------------------------------------------
//			return ERROR_CASS_MAP; // 2013.01.17
			return curres; // 2013.01.17
		}
		LeaveCriticalSection( &CR_MAIN ); // 2003.02.05

		//==========================================================================================================================
//		Scheduler_System_Log( CHECKING_SIDE , "AFTER_MAPPING" , TRUE , 0 ); // 2006.02.02 // 2006.10.19 // 2008.04.08
		//==========================================================================================================================
		//==========================================
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 51 ); // 2018.08.14
		//==========================================

		//===============================================================================================================
		//===============================================================================================================
		// 2004.12.14 // 2005.08.17
		//===============================================================================================================
		//===============================================================================================================
//		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.11.12 // 2010.01.19
		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2005.11.12 // 2010.01.19
			//===============================================================================================================
			// 2008.04.08
			//===============================================================================================================
			if ( _In_Loop[CHECKING_SIDE] == 0 ) {
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 61 ); // 2018.08.14
				//==========================================
				switch( Scheduler_Ordering_Checking( CHECKING_SIDE , TRUE ) ) {
				case -1 : // aborted // 2008.03.21
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 300 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , TRUE ); // 2008.03.21
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ABORTED2; // 2008.03.21
					break;
				case 1 : // ok
					break;
				case 0 : // cancel
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 310 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , FALSE ); // 2008.04.01
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ERROR; // 2008.04.01
					break;
				}
			}
			//==========================================
			SYSTEM_BEGIN_SET( CHECKING_SIDE , 62 ); // 2018.08.14
			//==========================================
			//===============================================================================================================
//			if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() || USER_RECIPE_START_MODE_CHANGE_API_EXIST() ) { // 2005.11.22 // 2009.04.01
			if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX4_API_EXIST() || USER_RECIPE_START_MODE_CHANGE_API_EXIST() ) { // 2005.11.22 // 2009.04.01
/*
				//===============================================================================================================
				// 2008.03.21
				//===============================================================================================================
				if ( _In_Loop[CHECKING_SIDE] == 0 ) {
					switch( Scheduler_Ordering_Checking( CHECKING_SIDE , TRUE ) ) {
					case -1 : // aborted // 2008.03.21
						//-----------------------------------------------------------
						Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 ); // 2008.03.21
						//-----------------------------------------------------------
						return ERROR_SYSTEM_ABORTED2; // 2008.03.21
						break;
					case 1 : // ok
						break;
					case 0 : // cancel
						//-----------------------------------------------------------
						Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 ); // 2008.04.01
						//-----------------------------------------------------------
						return ERROR_SYSTEM_ERROR; // 2008.04.01
						break;
					}
				}
*/
				//================================================================================
				EnterCriticalSection( &CR_MAIN ); // 2005.11.22
				//================================================================================
				if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() ) {
					OrderIndex = 0;
					for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
						SCHEDULER_CONTROL_RunData_Make_Verification_Info( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE_INF , &CLUSTER_INFO4 );
						if ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE != -1 ) OrderIndex++;
						if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() ) {
							//
							//==========================================
							SYSTEM_BEGIN_SET( CHECKING_SIDE , 63 ); // 2018.08.14
							//==========================================
							//
							verres = USER_RECIPE_INFO_VERIFICATION( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , _In_Loop[CHECKING_SIDE] , rck[CHECKING_SIDE] , G_CLUSTER_RECIPE_INF );
							//
							if ( verres != 1 ) {
								//-----------------------------------------------------------
								ERROR_REPORTING( CHECKING_SIDE , 320 , FALSE , 0 ); // 2010.03.26
								//-----------------------------------------------------------
								Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) ); // 2007.07.31
								//-----------------------------------------------------------
								LeaveCriticalSection( &CR_MAIN );
								//-----------------------------------------------------------
								if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
								if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
								return ERROR_SYSTEM_ERROR;
							}
						}
						if ( USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() ) {
							//
							//==========================================
							SYSTEM_BEGIN_SET( CHECKING_SIDE , 64 ); // 2018.08.14
							//==========================================
							//
							verres = USER_RECIPE_INFO_VERIFICATION_EX( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , G_CLUSTER_RECIPE_INF , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA );
							//
							if ( verres != 1 ) {
								//-----------------------------------------------------------
								ERROR_REPORTING( CHECKING_SIDE , 330 , FALSE , 0 ); // 2010.03.26
								//-----------------------------------------------------------
								Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) ); // 2007.07.31
								//-----------------------------------------------------------
								LeaveCriticalSection( &CR_MAIN );
								//-----------------------------------------------------------
								if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
								if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
								return ERROR_SYSTEM_ERROR;
							}
						}
					}
				}
				//===============================================================================================================
				if ( USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() ) {
					OrderIndex = 0;
					for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
						SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
						if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) OrderIndex++;
						//
						//==========================================
						SYSTEM_BEGIN_SET( CHECKING_SIDE , 65 ); // 2018.08.14
						//==========================================

						verres = USER_RECIPE_INFO_VERIFICATION_EX2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
						//
						if ( verres != 1 ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 340 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) ); // 2007.07.31
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
							if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
							return ERROR_SYSTEM_ERROR;
						}
					}
				}
				//===============================================================================================================
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 66 ); // 2018.08.14
				//==========================================
				if ( USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() ) {
					OrderIndex = 0;
					for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
						SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
						if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) OrderIndex++;
						//
						verres = USER_RECIPE_INFO_VERIFICATION_EX3( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , _In_Loop[CHECKING_SIDE] , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : OrderIndex ) , rck[CHECKING_SIDE] , &G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
						//
						if ( verres != 1 ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 350 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) ); // 2007.07.31
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
							if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
							return ERROR_SYSTEM_ERROR;
						}
					}
				}
				//===============================================================================================================
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 67 ); // 2018.08.14
				//==========================================
				if ( USER_RECIPE_INFO_VERIFICATION_EX4_API_EXIST() ) { // 2009.04.01
					OrderIndex = 0;
					for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , rck[CHECKING_SIDE] ) >= 0 ) OrderIndex++;
						//
						verres = USER_RECIPE_INFO_VERIFICATION_EX4( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , _In_Loop[CHECKING_SIDE] , _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , rck[CHECKING_SIDE] ) < 0 ? 0 : OrderIndex , rck[CHECKING_SIDE] , &_SCH_INF_CLUSTER_DATA_AREA[ CHECKING_SIDE ][ rck[CHECKING_SIDE] ] );
						//
						if ( verres != 1 ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 360 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) );
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
							if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
							return ERROR_SYSTEM_ERROR;
						}
					}
					//
					OrderIndex = 0;
					for ( rck[CHECKING_SIDE] = 0 ; rck[CHECKING_SIDE] < MAX_CASSETTE_SLOT_SIZE ; rck[CHECKING_SIDE]++ ) {
						if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , rck[CHECKING_SIDE] ) >= 0 ) OrderIndex++;
						//
						verres = USER_RECIPE_INFO_VERIFICATION_EX4( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , _In_Loop[CHECKING_SIDE] , _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , rck[CHECKING_SIDE] ) < 0 ? 0 : OrderIndex , rck[CHECKING_SIDE] , &_SCH_INF_CLUSTER_DATA_AREA[ CHECKING_SIDE ][ rck[CHECKING_SIDE] ] );
						//
						if ( verres != 1 ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 370 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , ( verres == -2 ) );
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN );
							//-----------------------------------------------------------
							if ( verres == -2 ) return ERROR_SYSTEM_ABORTED2; // 2011.07.28
							if ( verres == -1 ) return ERROR_VERIFY_FAIL; // 2011.07.28
							return ERROR_SYSTEM_ERROR;
						}
					}
				}
//		} // 2005.11.22
		//=================================================================================================================
		// 2005.08.02
		//=================================================================================================================
//		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.11.12 // 2005.11.22
//				if ( _SCH_SUB_NOCHECK_RUNNING() <= 0 ) { // 2010.11.23 // 2017.10.10
				if ( !_i_SCH_PRM_GET_EIL_INTERFACE( CHECKING_SIDE ) ) { // 2010.11.23 // 2017.10.10
					//
					if ( USER_RECIPE_START_MODE_CHANGE_API_EXIST() ) {
						//==========================================
						SYSTEM_BEGIN_SET( CHECKING_SIDE , 68 ); // 2018.08.14
						//==========================================
						//======================================================================
						// 2007.06.28
						//======================================================================
						//MAPVER = _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE );
						//======================================================================
						if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
							MAPVER = SYSTEM_MODE_FIRST_GET_BUFFER( CHECKING_SIDE );
						}
						else {
							MAPVER = _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE );
						}
						//======================================================================
						SYSTEM_MODE_BUFFER_SET( CHECKING_SIDE , MAPVER ); // 2008.03.04
						//======================================================================
						if ( !USER_RECIPE_START_MODE_CHANGE( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , _In_Loop[CHECKING_SIDE] , SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) , &MAPVER ) ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 380 , FALSE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? -3 : -2 , FALSE ); // 2007.07.31
							//-----------------------------------------------------------
							LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
							//-----------------------------------------------------------
							return ERROR_SYSTEM_ERROR;
						}
						//
						if ( ( MAPVER >= 0 ) && ( MAPVER <= 3 ) ) {
							if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
								SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , MAPVER );
							}
							else {
								_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , MAPVER );
							}
						}
					}
				}
				else { // 2010.11.23
					if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
						SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , 0 );
					}
					else {
						_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 0 );
					}
				}
				//
				//================================================================================
				LeaveCriticalSection( &CR_MAIN ); // 2005.11.22
				//================================================================================
			} // 2005.11.22
			//-------------------------------------------------------------------------------------------------------------------
			if ( ( FirstRun ) && ( _In_Loop[CHECKING_SIDE] == 0 ) ) { // 2011.05.31
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 69 ); // 2018.08.14
				//==========================================
				//================================================================================
				EnterCriticalSection( &CR_MAIN ); // 2011.05.30
				//================================================================================
				MAPVER = Scheduler_CONTROL_CHECK_AFTER_MODE_SETTING( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) );
				if ( MAPVER != ERROR_NONE ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 381 , FALSE , 0 );
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? -3 : -2 , ( ( MAPVER == ERROR_SYSTEM_ABORTED ) || ( MAPVER == ERROR_SYSTEM_ABORTED2 ) ) );
					//-----------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN );
					//-----------------------------------------------------------
					return MAPVER;
				}
				//================================================================================
				LeaveCriticalSection( &CR_MAIN ); // 2011.05.30
				//================================================================================
			}
			//-------------------------------------------------------------------------------------------------------------------
		}
		//==========================================
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 70 ); // 2018.08.14
		//==========================================
		//===============================================================================================================
		BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_11_WAITING ); // 2013.06.08
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_MAPPING" , 1 , 0 ); // 2006.02.02 // 2006.10.19 // 2008.04.08
		//==========================================================================================================================
		// 2011.06.10
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.05.21
			//
			SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , -1 );
			//
		}
		//===============================================================================================================
		// 2006.10.11
		//===============================================================================================================
//		if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) { // 2016.01.12
		if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) { // 2016.01.12
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 390 , TRUE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 , TRUE ); // 2007.07.31
			//-----------------------------------------------------------
//			return ERROR_SYSTEM_ABORTED; // 2008.03.13
			return ERROR_SYSTEM_ABORTED2; // 2008.03.13
		}
/*
// 2008.04.08
		//===============================================================================================================
		// 2006.11.29
		//===============================================================================================================
		if ( _In_Loop[CHECKING_SIDE] == 0 ) {
			switch( Scheduler_Ordering_Checking( CHECKING_SIDE , FALSE ) ) {
			case -1 : // aborted // 2008.03.21
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , adaptprcs[CHECKING_SIDE] ? 1 : 0 ); // 2008.03.21
				//-----------------------------------------------------------
				return ERROR_SYSTEM_ABORTED2; // 2008.03.21
				break;
			case 1 : // ok
				break;
			case 0 : // cancel
				break;
			}
		}
*/
		//===============================================================================================================
//		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.04.20
		if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.04.20 // 2011.09.23
			//
			if ( _In_Loop[CHECKING_SIDE] == 0 ) { // 2006.04.05
				//===============================================================================================================
				// 2008.04.01
				//===============================================================================================================
				if ( FirstRun ) {
					//
					//==========================================
					SYSTEM_BEGIN_SET( CHECKING_SIDE , 71 ); // 2018.08.14
					//==========================================
					if ( adaptprcs[CHECKING_SIDE] ) { // 2008.04.01
						//
						// 2010.12.17
						if ( !SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Progressing(%d)....%cSCHWAITP %d:%d:%d::\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() );
						}
						//
						while( TRUE ) {
							//================================================================================
	//						EnterCriticalSection( &CR_MAIN ); // 2008.03.21 // 2009.05.28
							//================================================================================
							if ( Scheduler_Main_Waiting( CHECKING_SIDE , 2 , &(_in_IMSI_PralSkip[CHECKING_SIDE]) , 0 , &dummyint ) != SYS_ABORTED ) {
								//-----------------------------------------------------------
	//							LeaveCriticalSection( &CR_MAIN ); // 2008.03.21 // 2009.05.28
								//-----------------------------------------------------------
								break;
							}
							if ( !_i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( CHECKING_SIDE ) ) {
								//-----------------------------------------------------------
								ERROR_REPORTING( CHECKING_SIDE , 400 , TRUE , 0 ); // 2010.03.26
								//-----------------------------------------------------------
								Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
								//-----------------------------------------------------------
	//							LeaveCriticalSection( &CR_MAIN ); // 2008.03.21 // 2009.05.28
								//-----------------------------------------------------------
								return ERROR_SYSTEM_ERROR;
							}
							if ( _i_SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
								//-----------------------------------------------------------
								ERROR_REPORTING( CHECKING_SIDE , 410 , TRUE , 0 ); // 2010.03.26
								//-----------------------------------------------------------
								Scheduler_Fail_Finish_Check( CHECKING_SIDE , 1 , FALSE ); // 2007.07.31
								//-----------------------------------------------------------
	//							LeaveCriticalSection( &CR_MAIN ); // 2008.03.21 // 2009.05.28
								//-----------------------------------------------------------
								return ERROR_SYSTEM_ERROR;
							}
							//================================================================================
	//						LeaveCriticalSection( &CR_MAIN ); // 2008.03.28 // 2009.05.28
							//================================================================================
							Sleep(250);
						}
						//
						//-------------------------------------------
						if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) { // 2017.06.29
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 411 , TRUE , 0 );
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE );
							//-----------------------------------------------------------
							return ERROR_SYSTEM_ERROR;
						}
						//-------------------------------------------
						//
						if ( SCHEDULER_SIDE_IN_POSSIBLE( 2 , CHECKING_SIDE ) ) { // 2005.09.22
							//
//							_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 0 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , directory ); // 2011.05.19
							_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 0 , _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2011.05.19
							//
						}
						else { // 2005.09.22
							//
//							_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 1 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , directory ); // 2011.05.19
							_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_START , 1 , _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2011.05.19
							//
						}
						//================================================================================
						strcpy( errorstring , "" ); // 2008.04.14
						//================================================================================
						if ( Scheduler_Begin_Part( CHECKING_SIDE , errorstring ) == SYS_ABORTED ) {
							//-----------------------------------------------------------
							ERROR_REPORTING( CHECKING_SIDE , 420 , TRUE , 0 ); // 2010.03.26
							//-----------------------------------------------------------
							Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE ); // 2007.07.31
							//-----------------------------------------------------------
							return ERROR_READY_FAILURE;
						}
						//================================================================================
						strcpy( errorstring , "" ); // 2008.04.14
						//================================================================================
						adaptprcs[CHECKING_SIDE] = FALSE; // 2008.04.01
						//================================================================================
					}
				}
				//================================================================================
				strcpy( errorstring , "" ); // 2008.04.14
				//================================================================================
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 72 ); // 2018.08.14
				//==========================================
				if ( Scheduler_Begin_Before_Wait_Part( CHECKING_SIDE , errorstring ) == SYS_ABORTED ) { // 2005.08.19
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 430 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , TRUE ); // 2007.07.31
					//-----------------------------------------------------------
	//				return ERROR_SYSTEM_ABORTED; // 2008.03.13
					return ERROR_SYSTEM_ABORTED2; // 2008.03.13
				}
				//-------------------------------------------
				if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) { // 2010.10.28
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 431 , TRUE , 0 );
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE );
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ERROR;
				}
				//-------------------------------------------
				//================================================================================
				strcpy( errorstring , "" ); // 2008.04.14
				//================================================================================
			}
		}
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_BEGIN1" , 0 , 0 ); // 2006.02.02
		//==========================================================================================================================
		
//		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07
		if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) || SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07
			//==========================================
			SYSTEM_BEGIN_SET( CHECKING_SIDE , 73 ); // 2018.08.14
			//==========================================
//		if ( _In_Loop[CHECKING_SIDE] == 0 ) { // 2003.05.26
			//=================================================================
			// 2007.10.24
			//=================================================================
			Result[CHECKING_SIDE] = FALSE;
			//=================================================================
//			EnterCriticalSection( &CR_MAIN ); // 2009.05.11 // 2009.05.28
			//=================================================================
			switch( Scheduler_Main_Waiting( CHECKING_SIDE , 0 , &(_in_IMSI_PralSkip[CHECKING_SIDE]) , loopcnt[ CHECKING_SIDE ] , &dummyint ) ) {
			case SYS_ABORTED :
				//-----------------------------------------------------------
				ERROR_REPORTING( CHECKING_SIDE , 440 , TRUE , 0 ); // 2010.03.26
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , -2 , TRUE ); // 2007.07.31
				//-----------------------------------------------------------
				//=================================================================
//				LeaveCriticalSection( &CR_MAIN ); // 2009.05.11 // 2009.05.28
				//=================================================================
//				return ERROR_SYSTEM_ABORTED; // 2008.03.13
				return ERROR_SYSTEM_ABORTED2; // 2008.03.13
				break;
			case SYS_ERROR :
				//=================================================================
				CANCELED = TRUE; // 2008.03.13
				//=================================================================
				// 2007.10.24
				//=================================================================
				Result[CHECKING_SIDE] = TRUE;
				//=================================================================
				//-----------------------------------------------------------
//				Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 ); // 2007.07.31
				//-----------------------------------------------------------
//				return ERROR_SYSTEM_ERROR;
				break; // 2007.10.25
			}
//		} // 2003.05.26
			//=================================================================
			// 2007.10.24
			//=================================================================
			if ( Result[CHECKING_SIDE] ) {
				//=================================================================
//				LeaveCriticalSection( &CR_MAIN ); // 2009.05.11 // 2009.05.28
				//=================================================================
				break; // 2007.10.24
			}
			//=============================================================================
			Scheduler_Randomize_After_Waiting_Part( CHECKING_SIDE , _in_IMSI_PralSkip[CHECKING_SIDE] ); // 2006.02.28
			//=============================================================================
			//=================================================================
//			LeaveCriticalSection( &CR_MAIN ); // 2009.05.11 // 2009.05.28
			//=================================================================
		}
		else {
//			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2005.11.12
			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 10 ); // 2005.11.12
			//
			while( TRUE ) { // 2011.05.13
				//
//				if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 1 ) break; // 2012.02.16
				if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) ==   0 ) break; // 2012.02.16
				if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) >= 200 ) break; // 2012.02.16
				//
				Sleep(1);
			}
			//
		}
		//
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 2 ); // 2011.04.22
		//===============================================================================================================
		//===============================================================================================================
		// 2006.10.11
		//===============================================================================================================
//		if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) { // 2016.01.12
		if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) { // 2016.01.12
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 450 , TRUE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , TRUE ); // 2007.07.31
			//-----------------------------------------------------------
//			return ERROR_SYSTEM_ABORTED; // 2008.03.13
			return ERROR_SYSTEM_ABORTED2; // 2008.03.13
		}
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 3 ); // 2011.04.22
		//===============================================================================================================
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_WAITING" , 0 , 0 ); // 2006.02.02
		//==========================================================================================================================
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 4 ); // 2011.04.22
		//===============================================================================================================
		// 2004.02.16
		//==============================================================================================
		//==========================================
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 74 ); // 2018.08.14
		//==========================================
//		Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 2 ); // 2010.07.29
		Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 3 ); // 2010.07.29
		//==============================================================================================
		//===============================================================================================================
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 5 ); // 2011.04.22
		//===============================================================================================================
		//===============================================================================================================
		//===============================================================================================================
		_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_RUNNING );
		//
		BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_12_RUNNING ); // 2013.06.08
		//
		*Mode = 1;
		*oncerun = TRUE; // 2007.06.28
		//
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 6 ); // 2011.04.22
		//===============================================================================================================
		SYSTEM_RUN_TIMER_SET( CHECKING_SIDE ); // 2010.12.02
		//
		SCHEDULING_CHECK_INTERLOCK_PM_SET( CHECKING_SIDE ); // 2003.02.13
		//===============================================================================================================
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 7 ); // 2011.04.22
		//===============================================================================================================
//		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.04.20
		if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.04.20 // 2011.09.23
			//
			if ( _In_Loop[CHECKING_SIDE] == 0 ) { // 2006.04.05
				//==========================================
				SYSTEM_BEGIN_SET( CHECKING_SIDE , 75 ); // 2018.08.14
				//==========================================
				//================================================================================
				strcpy( errorstring , "" ); // 2008.04.14
				//================================================================================
				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 11 ); // 2011.04.22
				//===============================================================================================================
				if ( Scheduler_Begin_After_Wait_Part( CHECKING_SIDE , errorstring , FALSE ) == SYS_ABORTED ) { // 2005.02.21
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 460 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , TRUE ); // 2007.07.31
					//-----------------------------------------------------------
	//				return ERROR_SYSTEM_ABORTED; // 2008.03.13
					return ERROR_SYSTEM_ABORTED2; // 2008.03.13
				}
				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 12 ); // 2011.04.22
				//===============================================================================================================
				//-------------------------------------------
				if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) { // 2010.10.28
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 461 , TRUE , 0 );
					//-----------------------------------------------------------
					Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE );
					//-----------------------------------------------------------
					return ERROR_SYSTEM_ERROR;
				}
				//-------------------------------------------
				//================================================================================
				strcpy( errorstring , "" ); // 2008.04.14
				//================================================================================
			}
		}
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 21 ); // 2011.04.22
		//===============================================================================================================
		if ( ( FirstRun ) && ( _In_Loop[CHECKING_SIDE] == 0 ) ) SCHEDULER_CLUSTER_Set_DefaultUse_Data( CHECKING_SIDE ); // 2017.09.08
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_BEGIN2" , 0 , 0 ); // 2006.02.02
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 22 ); // 2011.12.14
		//===============================================================================================================
		//==========================================================================================================================
		Scheduler_Restart_Process_Part( CHECKING_SIDE ); // 2011.12.15
		//==========================================================================================================================
		SCHEDULER_CONTROL_Pre_Inc( CHECKING_SIDE ); // 2015.07.30
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 31 ); // 2011.04.22
		//===============================================================================================================
		//==========================================
		SYSTEM_BEGIN_SET( CHECKING_SIDE , 999 ); // 2018.08.14
		//==========================================
		switch( Scheduler_Main_Running( CHECKING_SIDE , TMATM ) ) {
		case SYS_ABORTED :
			//===============================================================================================================
			_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 201 ); // 2011.04.22
			//===============================================================================================================
			//=================================================================================================
			// 2007.07.31 Error Reporting
			//=================================================================================================
			ERROR_REPORTING( CHECKING_SIDE , 0 , TRUE , 0 ); // 2010.03.26
/*
// 2010.03.26
			EnterCriticalSection( &CR_MAIN );
			sprintf( Buffer , "%s/LOTLOG/%s/Error.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
			SCHEDULING_DATA_STRUCTURE_LOG( Buffer , "ABORT_RESULT" , -1 );
			SCHEDULING_DATA_STRUCTURE_JOB_LOG( Buffer , "ABORT_RESULT" );
			//
			sprintf( Buffer , "%s/LOTLOG/%s/Error.jpg" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
			sprintf( Buffer2 , "%s/LOTLOG/%s/Error.cil" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) );
			KWIN_SCREEN_CAPTURE( GET_WINDOW_X_REFERENCE_POSITION() , GET_WINDOW_Y_REFERENCE_POSITION() , GET_WINDOW_XS_REFERENCE_POSITION() , GET_WINDOW_YS_REFERENCE_POSITION() , Buffer );
			rename( Buffer , Buffer2 );
			//==========================================================================
//			KPLT_PROCESS_TIME_RESET(); // 2005.04.19 // 2008.04.02
//			KPLT_PROCESS_TIME_RESET( -1 ); // 2005.04.19 // 2008.04.02 // 2008.04.18
//			KPLT_PROCESS_TIME_INIT( -2 , 0 , 0 ); // 2008.04.18 // 2008.07.13
//			KPLT_PROCESS_TIME_INIT( -1 , 0 , 0 ); // 2008.04.18 // 2008.07.13 // 2010.03.26
			//==========================================================================
			LeaveCriticalSection( &CR_MAIN );
			//=================================================================================================
*/
			//=================================================================================================
			KPLT_PROCESS_TIME_INIT( -1 , 0 , 0 ); // 2008.04.18 // 2008.07.13 // 2010.03.26
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , -2 , TRUE ); // 2007.07.31
			//-----------------------------------------------------------
			return ERROR_SYSTEM_ABORTED;
			break;

		case SYS_ERROR   :
			//===============================================================================================================
			_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 202 ); // 2011.04.22
			//===============================================================================================================
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 1 , TRUE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , 0 , FALSE ); // 2007.07.31
			//-----------------------------------------------------------
			return ERROR_SYSTEM_ERROR;
			break;
		}
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_RUNNING" , 0 , 0 ); // 2006.02.02
		//==========================================================================================================================
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 211 ); // 2011.04.22
		//===============================================================================================================
		//===============================================================================================
//		if ( !_i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) break; // 2004.09.07 // 2009.01.19
//		if ( !SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) break; // 2004.09.07 // 2013.06.19
		if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) && !SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) break; // 2004.09.07 // 2013.06.19
		//===============================================================================================
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 212 ); // 2011.04.22
		//===============================================================================================================
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
			LOG_MTL_SET_DONE_CARR_COUNT( CHECKING_SIDE , _In_Loop[CHECKING_SIDE] + 1 ); // 2013.06.19
		}
		else {
			if ( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) == SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) ) {
				if ( SCHEDULER_SIDE_IN_POSSIBLE( 0 , CHECKING_SIDE ) ) { // 2010.05.04
					LOG_MTL_ADD_CASS_COUNT( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) );
				}
			}
			else {
				if ( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) >= 0 ) {
					if ( SCHEDULER_SIDE_IN_POSSIBLE( 0 , CHECKING_SIDE ) ) { // 2010.05.04
						LOG_MTL_ADD_CASS_COUNT( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) );
					}
				}
				if ( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) >= 0 ) {
					if ( SCHEDULER_SIDE_OUT_POSSIBLE( FALSE , CHECKING_SIDE ) ) { // 2010.05.04
						LOG_MTL_ADD_CASS_COUNT( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) );
					}
				}
			}
			//===============================================================================================================
			_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 213 ); // 2011.04.22
			//===============================================================================================================
			//
			LOG_MTL_SET_DONE_CARR_COUNT( CHECKING_SIDE , _In_Loop[CHECKING_SIDE] + 1 );
		}
		//
		if ( _i_SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) != 0 ) break;
	}
	//===============================================================================================================
	_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 301 ); // 2011.04.22
	//===============================================================================================================
	//-------------------------------------------
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 110 );
	//-------------------------------------------
	_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_STOP2 , 0 , -1 , -1 , -1 , -1 , "" , "" );
	//-----------------------
	//-----------------------
	if ( !_i_SCH_MULTIREG_HAS_REGIST_DATA( CHECKING_SIDE ) ) {
		*Mode = 1;
		//
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 311 ); // 2011.04.22
		//===============================================================================================================
		SCHEDULER_REG_SET_DISABLE( CHECKING_SIDE , TRUE );
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------
		SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( CHECKING_SIDE , CANCELED ? SYS_ERROR : SYS_SUCCESS );
		//------------------------------------------------------------------------------------
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 312 ); // 2011.04.22
		//===============================================================================================================
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------
		SCHMULTI_ALL_FORCE_CHECK( CHECKING_SIDE ); // 2004.10.14
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 313 ); // 2011.04.22
		//===============================================================================================================
		//------------------------------------------------------------------------------------
		//===============================================================================================
		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07
			//
			BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_13_MAPOUT ); // 2013.06.08
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) { // 2012.04.01
				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 321 ); // 2011.04.22
				//===============================================================================================================
	//			Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , FALSE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10
				Result[CHECKING_SIDE] = Scheduler_Mapping_Real_Part( CHECKING_SIDE , FALSE , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] ); // 2010.03.10

				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 322 ); // 2011.04.22
				//===============================================================================================================
				EnterCriticalSection( &CR_MAIN );
				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 323 ); // 2011.04.22
				//===============================================================================================================
				if ( Result[CHECKING_SIDE] == ERROR_NONE )
					Result[CHECKING_SIDE] = Scheduler_Mapping_Check_Part( CHECKING_SIDE , 2 , SYSTEM_IN_MODULE_GET(CHECKING_SIDE) , SYSTEM_OUT_MODULE_GET(CHECKING_SIDE) , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , CPJOB );
				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 324 ); // 2011.04.22
				//===============================================================================================================
				LeaveCriticalSection( &CR_MAIN );

				//===============================================================================================================
				_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 325 ); // 2011.04.22
				//===============================================================================================================
				if ( Result[CHECKING_SIDE] != ERROR_NONE ) {
					//-----------------------------------------------------------
					ERROR_REPORTING( CHECKING_SIDE , 500 , TRUE , 0 ); // 2010.03.26
					//-----------------------------------------------------------
					Scheduler_Module_Use_Reference_Data_Refresh( CHECKING_SIDE , 0 );
					//-----------------------------------------------------------
					return Result[CHECKING_SIDE];
				}
			}
		}
		//===============================================================================================

		//-------------------------------------------
//		*EndLock = TRUE; // 2004.03.11 // 2009.04.13
		//-------------------------------------------
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 331 ); // 2011.04.22
		//===============================================================================================================

		//================================================================================
		if ( Scheduler_End_Part( CHECKING_SIDE ) == SYS_ABORTED ) {
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 510 , TRUE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			return ERROR_END_FAILURE;
		}
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 332 ); // 2011.04.22
		//===============================================================================================================
		//==========================================================================================================================
		Scheduler_System_Log( CHECKING_SIDE , "AFTER_END" , 0 , 0 ); // 2006.02.02
		//==========================================================================================================================

		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 111 );
		//-------------------------------------------
		//-------------------------------------------
//		if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07 // 2010.01.11

		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.21
			//
			if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07 // 2010.01.11
				//
//				if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12 // 2017.10.10
				if ( !_SCH_SUB_NOCHECK_RUNNING( CHECKING_SIDE ) ) { // 2010.10.12 // 2017.10.10
					//
					BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_14_HANDOFFOUT ); // 2011.04.14
					//
					incm[CHECKING_SIDE]  = SYSTEM_IN_MODULE_GET( CHECKING_SIDE );
					outcm[CHECKING_SIDE] = SYSTEM_OUT_MODULE_GET( CHECKING_SIDE );
					//
					//===============================================================================================================
					_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 341 ); // 2011.04.22
					//===============================================================================================================
					if ( Scheduler_HandOffOut_Part( CHECKING_SIDE , ( ( ( _in_UNLOAD_USER_TYPE[ CHECKING_SIDE ] % 10 ) == 0 ) || ( ( _in_UNLOAD_USER_TYPE[ CHECKING_SIDE ] % 10 ) >= 5 ) ) ? TRUE : FALSE , &(incm[CHECKING_SIDE]) , &(outcm[CHECKING_SIDE]) ) == SYS_ABORTED ) { // 2010.04.26
						//-----------------------------------------------------------
						ERROR_REPORTING( CHECKING_SIDE , 520 , TRUE , 0 ); // 2010.03.26
						//-----------------------------------------------------------
						return ERROR_HANDOFF_OUT_FAILURE;
					}
					//
					//===============================================================================================================
					_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 342 ); // 2011.04.22
					//===============================================================================================================
					if ( SCHEDULER_SIDE_GET_CM_IN( CHECKING_SIDE ) >= 0 ) SYSTEM_IN_MODULE_SET( CHECKING_SIDE , incm[CHECKING_SIDE] );
					if ( SCHEDULER_SIDE_GET_CM_OUT( CHECKING_SIDE ) >= 0 ) SYSTEM_OUT_MODULE_SET( CHECKING_SIDE , outcm[CHECKING_SIDE] );
				}
			}
		}
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 351 ); // 2011.04.22
		//===============================================================================================================
		//==========================================================================================================================
		if ( Scheduler_End_Post_Part( CHECKING_SIDE ) == SYS_ABORTED ) {
			//-----------------------------------------------------------
			ERROR_REPORTING( CHECKING_SIDE , 530 , TRUE , 0 ); // 2010.03.26
			//-----------------------------------------------------------
			return ERROR_END_FAILURE;
		}
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 352 ); // 2011.04.22
		//===============================================================================================================
		//==========================================================================================================================
//		Scheduler_System_Log( CHECKING_SIDE , "FINISH" , FALSE , 0 ); // 2006.02.02 // 2012.11.09
		Scheduler_System_Log( CHECKING_SIDE , "FINISH" , 2 , 0 ); // 2006.02.02 // 2012.11.09
		//==========================================================================================================================
	}
	else {
		//===============================================================================================================
		_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 361 ); // 2011.04.22
		//===============================================================================================================
		//------------------------------------------------------------------------------------
		SCHMULTI_ALL_FORCE_CHECK( CHECKING_SIDE ); // 2004.10.14
		//------------------------------------------------------------------------------------
	}
	//===============================================================================================================
	_i_SCH_SYSTEM_RUN_TAG_SET( CHECKING_SIDE , 399 ); // 2011.04.22
	//===============================================================================================================
	if ( CANCELED ) return ERROR_SYSTEM_ERROR; // 2012.09.28
	//===============================================================================================================
	return ERROR_NONE;
}
//===================================================================================================
int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_START_FOR_OUTCASS( int side , int cm , BOOL check ) { // 2011.06.20
	int p , ps , po , f , log;
	//
	f = 0;
	//
	log = FALSE; // 2013.06.05
	//
	for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , p ) < 0 ) continue;
		//
		ps = _i_SCH_CLUSTER_Get_PathStatus( side , p );
		po = _i_SCH_CLUSTER_Get_PathOut( side , p );
		//
		if ( ps == SCHEDULER_CM_END ) continue;
		//
		if ( po > -100 ) continue;
		//
		if ( po <= -200 ) continue;
		//
		po = -po % 100;
		//
		if ( po != cm ) continue;
		//
		if ( check ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( cm , _i_SCH_CLUSTER_Get_SlotOut( side , p ) ) != CWM_ABSENT ) continue;
		}
		else {
			//
			log = TRUE; // 2013.06.05
			//
			SCHMRDATA_Data_Setting_for_outcm( cm , side , p , 0 ); // 2011.09.07
			//
			_i_SCH_CLUSTER_Set_PathOut( side , p , cm );
		}
		//
		f++;
	}
	//
	if ( log ) SCHMRDATA_Data_Setting_for_outcm( -2 , -1 , -1 , -1 ); // 2013.06.05
	//
	if ( f == 0 ) return FALSE;
	return TRUE;
}

//===================================================================================================
/*
// 2011.08.10
int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( int side , int cm , BOOL unload ) { // 2011.06.10
	int p , ps , po , f , l , pos;

	pos = BUTTON_GET_FLOW_STATUS_VAR( cm - CM1 );
	if ( pos != 12 ) return 0;
	//
	pos = -1;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		//
		f = 0;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , p ) < 0 ) continue;
			//
			ps = _i_SCH_CLUSTER_Get_PathStatus( side , p );
			po = _i_SCH_CLUSTER_Get_PathOut( side , p );
			//
			if ( ps == SCHEDULER_CM_END ) continue;
			//
			if ( l == 0 ) {
				//
				if ( po > -200 ) return 0;
				//
				if ( ( -po % 100 ) != cm ) return 0;
				//
				if ( ps == SCHEDULER_RETURN_REQUEST ) return 0;
				//
				if ( ps == SCHEDULER_READY ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( side , p ) == cm ) return 0;
					//
				}
				//
			}
			//
			po = -po / 100;
			//
			if ( l == 0 ) {
				if ( pos == -1 ) {
					pos = po;
				}
				else {
					if ( po < pos ) pos = po;
				}
				//
				f++;
				//
			}
			else {
				if ( po == pos ) {
					_i_SCH_CLUSTER_Set_PathOut( side , p , -100 - cm );
				}
			}
		}
		//
		if ( l == 0 ) {
			//
			if ( f == 0 ) return 0;
			//
			if ( pos == -1 ) return 0;
			//
		}
		//
	}
	//
	if ( unload ) {
		if ( Scheduler_Run_Unload_Code( side , cm , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 6 ) < 0 ) return -1;
	}
	//
	return 1;
	//
}
*/

void SCHMULTI_CHECK_HANDOFF_OUT_MULTI_SET_DATA_FOR_OUTCASS( int side ) { // 2011.08.16
	int p , ps , po;
	//
	for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , p ) < 0 ) continue;
		//
		ps = _i_SCH_CLUSTER_Get_PathStatus( side , p );
		po = _i_SCH_CLUSTER_Get_PathOut( side , p );
		//
		if ( ps == SCHEDULER_CM_END ) {
			//
			if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) ) { // 2011.08.10 // 2013.07.02
				//
				_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , _MOTAG_23_END /* END3 */ ); // 2011.08.10
				//
				return;
			}
		}
	}
}

// 2011.08.10
int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( int side , int pt , int cm ) { // 2011.06.10
	int p , ps , po , f , l , pos , mset , log;

	pos = BUTTON_GET_FLOW_STATUS_VAR( cm - CM1 );
	if ( pos != _MS_12_RUNNING ) return 0;
	//
	mset = ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pt ) % 100 ) > 0 ) ? TRUE : FALSE; // 2011.08.10
	//
	log = FALSE; // 2013.06.05
	//
	pos = -1;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		//
		f = 0;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , p ) < 0 ) continue;
			//
			ps = _i_SCH_CLUSTER_Get_PathStatus( side , p );
			po = _i_SCH_CLUSTER_Get_PathOut( side , p );
			//
			if ( ps == SCHEDULER_CM_END ) {
				//
				if ( l != 0 ) {
					//
					if      ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) <= 0 ) { // 2011.08.10
					}
					else if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) ) { // 2011.08.10 // 2013.07.02
						//
						_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , _MOTAG_22_END /* END2 */ ); // 2011.08.10
						//
					}
					else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) < _MOTAG_10_NEED_UPDATE ) { // 2012.03.20
						//
						_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , _MOTAG_21_END /* END1 */ ); // 2011.08.10
						//
					}
					//
				}
				//
				continue;
			}
			//
			if ( l == 0 ) {
				//
				if ( po > -200 ) return 0;
				//
				if ( ( -po % 100 ) != cm ) return 0;
				//
				if ( ps == SCHEDULER_RETURN_REQUEST ) return 0;
				//
				if ( ps == SCHEDULER_READY ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( side , p ) == cm ) return 0;
					//
				}
				//
			}
			//
			po = -po / 100;
			//
			if ( l == 0 ) {
				if ( pos == -1 ) {
					pos = po;
				}
				else {
					if ( po < pos ) pos = po;
				}
				//
				f++;
				//
			}
			else {
				if ( po == pos ) {
					//
					log = TRUE; // 2013.06.05
					//
					SCHMRDATA_Data_Setting_for_outcm( -100 - cm , side , p , 0 ); // 2011.09.07
					//
					_i_SCH_CLUSTER_Set_PathOut( side , p , -100 - cm );
					//
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) > 0 ) { // 2011.08.10
						//
						// 2013.07.02
						if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) {
							//
							_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , 400 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
							//
						}
						else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) {
							if ( _i_SCH_CLUSTER_Get_PathStatus( side , p ) == SCHEDULER_READY ) {
								if ( _i_SCH_CLUSTER_Get_PathIn( side , p ) == cm ) {
									//
									_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , 400 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
									//
								}
							}
						}
						else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , p ) % 100 ) < _MOTAG_10_NEED_UPDATE ) {
							if ( _i_SCH_CLUSTER_Get_PathStatus( side , p ) == SCHEDULER_READY ) {
								if ( _i_SCH_CLUSTER_Get_PathIn( side , p ) != cm ) {
									_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , 400 + _MOTAG_14_NEED_OUT /* OUT */ );
								}
								else {
									_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , 400 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
								}
							}
							else {
								_i_SCH_CLUSTER_Set_Ex_MtlOut( side , p , 400 + _MOTAG_14_NEED_OUT /* OUT */ );
							}
						}
						//
					}
					//
				}
			}
		}
		//
		if ( l == 0 ) {
			//
			if ( f == 0 ) return 0;
			//
			if ( pos == -1 ) return 0;
			//
		}
		//
	}
	//
	if ( log ) SCHMRDATA_Data_Setting_for_outcm( -3 , -1 , -1 , -1 ); // 2013.06.05
	//
	if ( mset ) { // 2011.08.10
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tOUTSET : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [CID=%d][JID=%s][PPID=%s][MID=%s][C=%d]\n" , side , pt , _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) , _i_SCH_CLUSTER_Get_PathOut( side , pt ) , _i_SCH_CLUSTER_Get_SlotOut( side , pt ) , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pt ) , _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) , _i_SCH_CLUSTER_Get_Ex_PPID( side , pt ) , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) , LOG_MTL_GET_DONE_WFR_COUNT( side ) );
//------------------------------------------------------------------------------------------------------------------
		//
//		if ( _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) != NULL ) { // 2011.09.15
//			_i_SCH_SYSTEM_SET_JOB_ID( side , _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) );
//			IO_SET_JID_NAME( side , _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) );
//		}
		//
//		if ( _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) != NULL ) { // 2011.09.15
//			_i_SCH_SYSTEM_SET_MID_ID( side , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) );
//			IO_SET_MID_NAME( side , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) );
//		}
		//
//		if ( _i_SCH_CLUSTER_Get_Ex_PPID( side , pt ) != NULL ) {  // 2011.09.15
//			_i_SCH_IO_SET_RECIPE_FILE( side , _i_SCH_CLUSTER_Get_Ex_PPID( side , pt ) );
//		}
		//

	}
	else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tOUTSET(X) : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [CID=%d][C=%d]\n" , side , pt , _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) , _i_SCH_CLUSTER_Get_PathOut( side , pt ) , _i_SCH_CLUSTER_Get_SlotOut( side , pt ) , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pt ) , LOG_MTL_GET_DONE_WFR_COUNT( side ) );
//------------------------------------------------------------------------------------------------------------------
	}
	//
	l = LOG_MTL_GET_DONE_WFR_COUNT( side );
	//
	LOG_MTL_SET_DONE_WFR_COUNT( side , 0 );
	//
	TIMER_MONITOR_END_For_Move( side , l ); // 2012.03.20
	//
	FA_TIME_END2_MESSAGE( side , pt , PRJOB_RESULT_NORMAL ); // 2011.09.19
	//
	l = Scheduler_Run_Unload_Code( side , cm , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 6 , pt , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) );
	//
	if ( l < 0 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tFAIL : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [CID=%d][JID=%s][PPID=%s][MID=%s]\n" , side , pt , _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) , _i_SCH_CLUSTER_Get_PathOut( side , pt ) , _i_SCH_CLUSTER_Get_SlotOut( side , pt ) , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pt ) , _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) , _i_SCH_CLUSTER_Get_Ex_PPID( side , pt ) , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) );
//------------------------------------------------------------------------------------------------------------------
		return -1;
	}
	//
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "MTLOUT-MSG\tCOMPLETE : S=%d,P=%d,PI=(%d,%d),PO=(%d,%d) => [CID=%d][JID=%s][PPID=%s][MID=%s][%d]\n" , side , pt , _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) , _i_SCH_CLUSTER_Get_PathOut( side , pt ) , _i_SCH_CLUSTER_Get_SlotOut( side , pt ) , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pt ) , _i_SCH_CLUSTER_Get_Ex_JobName( side , pt ) , _i_SCH_CLUSTER_Get_Ex_PPID( side , pt ) , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) , l );
//------------------------------------------------------------------------------------------------------------------
	return 1;
	//
}

BOOL SCHMULTI_CHECK_HANDOFF_OUT_CHECK( BOOL outmode , int side , int pointer , int ch , int slot ) {
	int used;
	int i , ps , f;
//	int x;
	char Buffer[256];

	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.21
		//
		return TRUE;
		//
	}
	//
	if ( outmode ) { // Place
		//
		EnterCriticalSection( &CR_MAIN );
		//
		//--------------------------------------------------------------------------------------------------------
		sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [PLACING    ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
		//--------------------------------------------------------------------------------------------------------
		_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
		SCHMULTI_SET_LOG_MESSAGE( Buffer );
		//--------------------------------------------------------------------------------------------------------
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , ch ) != MUF_01_USE ) { // 2011.05.03
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		if ( !_i_SCH_SYSTEM_USING_STARTING( side ) ) { // 2011.06.09
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
//		f = SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( side , ch , FALSE ); // 2011.06.10 // 2011.08.10
		f = SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( side , pointer , ch ); // 2011.06.10 // 2011.08.10
		//
		if ( f == -1 ) {
			LeaveCriticalSection( &CR_MAIN );
			return FALSE;
		}
		if ( f == 1 ) {
			LeaveCriticalSection( &CR_MAIN );
//			return Scheduler_Run_Unload_Code( side , ch , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 7 ); // 2011.08.10
			return TRUE; // 2011.08.10
		}
		//
		f = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
//			if ( !_i_SCH_SYSTEM_USING_STARTING( side ) ) continue; // 2011.06.09
			//
			if ( i == pointer ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			//
			ps = _i_SCH_CLUSTER_Get_PathStatus( side , i );
			//
			if ( ps == SCHEDULER_CM_END ) continue;
			//
			f++; // 2011.06.09
			//
			if ( ( ps == SCHEDULER_READY ) || ( ps == SCHEDULER_RETURN_REQUEST ) ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == ch ) {
					//
					LeaveCriticalSection( &CR_MAIN );
					return TRUE;
				}
				//
			}
			//
			ps = _i_SCH_CLUSTER_Get_PathOut( side , i );
			//
			if ( ps == 0 ) { // 2011.06.14
				//
				SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , i , FALSE , &ps , FALSE );
				//
				ps = _i_SCH_CLUSTER_Get_PathOut( side , i );
			}
			//
			if ( ps >= CM1 ) {
				if ( ps == ch ) {
					//
					LeaveCriticalSection( &CR_MAIN );
					return TRUE;
				}
			}
			//
			if ( ps <= -100 ) {
				if ( ( -ps % 100 ) == ch ) {
					//
					LeaveCriticalSection( &CR_MAIN );
					return TRUE;
				}
			}
		}
		//
		if ( f == 0 ) { // 2011.06.09
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		_i_SCH_MODULE_Set_Mdl_Use_Flag( side , ch , MUF_02_USE_to_END ); // 2011.05.03
		_i_SCH_MODULE_Dec_Mdl_Run_Flag( ch ); // 2011.05.03
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) < 0 ) {
			_i_SCH_LOG_LOT_PRINTF( side , "Module %s Run Flag Status Error 101%cMDLSTSERROR 101:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch );
			_i_SCH_MODULE_Set_Mdl_Run_Flag( ch , 0 );
		}
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) > 0 ) {
			//--------------------------------------------------------------------------------------------------------
			sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [AFTER_PLACE] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
			//--------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
			SCHMULTI_SET_LOG_MESSAGE( Buffer );
			//--------------------------------------------------------------------------------------------------------
			//
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		if ( SYSTEM_IN_MODULE_GET( side ) == ch ) {
			//------------------------------------------------------------------------------------
			_In_Job_HandOff_In_Pre_Check[side] = 1;
			//------------------------------------------------------------------------------------
			SYSTEM_IN_MODULE_SET( side , -1 );
			//------------------------------------------------------------------------------------
		}
		else {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( !_i_SCH_SYSTEM_USING_STARTING( i ) ) continue;
				//
				if ( SYSTEM_IN_MODULE_GET( i ) != ch ) continue;
				//------------------------------------------------------------------------------------
				SYSTEM_IN_MODULE_SET( i , -1 );
				//------------------------------------------------------------------------------------
			}
		}
		//
		if ( SCHMULTI_CHECK_ANOTHER_OUT_CASS_USING( FALSE , TRUE , FALSE , ch , &used ) ) { // 2011.06.01
			//--------------------------------------------------------------------------------------------------------
			sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [AFTER_PL_OC] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
			//--------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
			SCHMULTI_SET_LOG_MESSAGE( Buffer );
			//--------------------------------------------------------------------------------------------------------
			Scheduler_HandOffOut_Skip_Set( ch ); // 2011.06.01
			//--------------------------------------------------------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		LeaveCriticalSection( &CR_MAIN );
		//
		_In_Job_HandOff_Out_Message_Skip[side] = 1; // 2011.09.19
		//
		FA_TIME_END2_MESSAGE( side , pointer , PRJOB_RESULT_NORMAL ); // 2011.09.19
		//
		if ( Scheduler_Run_Unload_Code( side , ch , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 4 , pointer , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ) < 0 ) return FALSE;
		//
	}
	else { // Pick
		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 2 ) return TRUE; // 2013.06.26
		//
		EnterCriticalSection( &CR_MAIN );
		//--------------------------------------------------------------------------------------------------------
		sprintf( Buffer , "MTLOUT UNLOAD CHECKED [PICKING    ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
		//--------------------------------------------------------------------------------------------------------
		_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
		SCHMULTI_SET_LOG_MESSAGE( Buffer );
		//--------------------------------------------------------------------------------------------------------

		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , ch ) != MUF_01_USE ) { // 2011.05.03
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
//		for ( x = 0 ; x < 2 ; x++ ) { // 2011.06.03
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
				//
				ps = _i_SCH_CLUSTER_Get_PathStatus( side , i );
				//
				if ( ps == SCHEDULER_CM_END ) continue;
				//
				if ( ps == SCHEDULER_RETURN_REQUEST ) {
					LeaveCriticalSection( &CR_MAIN );
					return TRUE;
				}
				//
				if ( i != pointer ) {
					//
					if ( ps == SCHEDULER_READY ) {
						//
						if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == ch ) {
							//
							LeaveCriticalSection( &CR_MAIN );
							return TRUE;
						}
						//
					}
				}
				//
				ps = _i_SCH_CLUSTER_Get_PathOut( side , i );
				//
				if ( ps == 0 ) { // 2011.06.14
					//
					SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , i , FALSE , &ps , FALSE );
					//
					ps = _i_SCH_CLUSTER_Get_PathOut( side , i );
				}
				//
				if ( ps == ch ) {
					//
					LeaveCriticalSection( &CR_MAIN );
					return TRUE;
				}
				//
			}
			//
//			Sleep(1); // 2011.06.03
//		} // 2011.06.03
		//
		//------------------------------------------------------------------------------------
		_In_Job_HandOff_In_Pre_Check[side] = 1;
		//------------------------------------------------------------------------------------
		SYSTEM_IN_MODULE_SET( side , -1 );
		//------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_STARTING( i ) ) continue;
			//
			if ( SYSTEM_IN_MODULE_GET( i ) != ch ) continue;
			//------------------------------------------------------------------------------------
			SYSTEM_IN_MODULE_SET( i , -1 );
			//------------------------------------------------------------------------------------
		}
		//
		_i_SCH_MODULE_Set_Mdl_Use_Flag( side , ch , MUF_02_USE_to_END ); // 2011.05.03
		_i_SCH_MODULE_Dec_Mdl_Run_Flag( ch ); // 2011.05.03
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) < 0 ) {
			_i_SCH_LOG_LOT_PRINTF( side , "Module %s Run Flag Status Error 102%cMDLSTSERROR 102:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch );
			_i_SCH_MODULE_Set_Mdl_Run_Flag( ch , 0 );
		}
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) > 0 ) {
			//--------------------------------------------------------------------------------------------------------
			sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [AFTER_PICK ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
			//--------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
			SCHMULTI_SET_LOG_MESSAGE( Buffer );
			//--------------------------------------------------------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		if ( SCHMULTI_CHECK_ANOTHER_OUT_CASS_USING( FALSE , FALSE , FALSE , ch , &used ) ) { // 2011.06.01
			//--------------------------------------------------------------------------------------------------------
			sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [AFTER_PI_OK] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , ch - CM1 + 1 , slot , side , pointer , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) );
			//--------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
			SCHMULTI_SET_LOG_MESSAGE( Buffer );
			//--------------------------------------------------------------------------------------------------------
			Scheduler_HandOffOut_Skip_Set( ch ); // 2011.06.01
			//--------------------------------------------------------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN );
			return TRUE;
		}
		//
		LeaveCriticalSection( &CR_MAIN );
		//
		_In_Job_HandOff_Out_Message_Skip[side] = 1; // 2011.09.19
		//
		FA_TIME_END2_MESSAGE( side , pointer , PRJOB_RESULT_NORMAL ); // 2011.09.19
		//
//		if ( Scheduler_Run_Unload_Code( side , ch , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 5 , pointer , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ) < 0 ) return FALSE; // 2016.07.15
		if ( Scheduler_Run_Unload_Code( side , ch , ( _i_SCH_SYSTEM_FA_GET( side ) == 1 ) , FALSE , 5 , pointer , ( pointer < 0 ) ? -1 : _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ) < 0 ) return FALSE; // 2016.07.15
		//
	}
	return TRUE;
}



//===================================================================================================
//===================================================================================================
void Scheduler_Run_Abort_But_Remain_Check_Code( int side ) { // 2004.03.18
	Sleep( 5000 );
	if ( PROCESS_MONITOR_STATUS_for_Abort_Remain( side ) ) ERROR_HANDLER( ERROR_ABORT_BUT_REMAIN_PROCESS , "" );
	_endthread();
}
//===================================================================================================
//===================================================================================================

BOOL Scheduler_Job_Current_Side_is_Old( int currside , int *neworder ); // 2017.10.31

void Scheduler_Run_Main_Handling_Pre_Part( int CHECKING_SIDE , int NormalMode , int DefaultStartMode ) {
	int i , j , k , chktag;
	//
	int  job_order_New; // 2017.10.31
	//-----------------------------------------------------------------------------------------
	// 2008.04.03
	//-----------------------------------------------------------------------------------------
	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( PROCESS_MONITOR_Get_Status( CHECKING_SIDE , i ) < 0 ) {
//			KPLT_PROCESS_TIME_RESET( i ); // 2008.04.18
			KPLT_PROCESS_TIME_INIT( i , -1 , -1 ); // 2008.04.18
			PROCESS_MONITOR_Set_Status( CHECKING_SIDE , i , 0 , FALSE , -1 );
		}
	}
	//-----------------------------------------------------------------------------------------
	_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , FALSE );
	//-----------------------------------------------------------------------------------------
	chktag = FALSE;
	//
	for ( i = 0 , j = -1 , k = -1 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != CHECKING_SIDE ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
				if ( j == -1 ) {
					j = i;
				}
				else {
					if ( NormalMode == 0 ) { // Normal
						if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) > _i_SCH_SYSTEM_RUNORDER_GET( j ) ) {
							k = j;
							j = i;
						}
						//
					}
					else { // expedite
						if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( j ) ) {
							k = j;
							j = i;
						}
						else {
							if ( k == -1 ) k = i;
						}
					}
				}
				chktag = TRUE;
			}
		}
	}
	if ( !chktag ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) _i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
		_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , RUNORDER_START_VALUE );
	}
	else {
		if      ( NormalMode == 0 ) { // Normal
			//
			if ( Scheduler_Job_Current_Side_is_Old( CHECKING_SIDE , &job_order_New ) ) { // 2017.10.31
				//
				_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , job_order_New );
				//
			}
			else {
				//
				_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , _i_SCH_SYSTEM_RUNORDER_GET( j ) + RUNORDER_INTERVAL_COUNT );
				//
			}
		}
		else if ( NormalMode == 1 ) { // expedite
			if ( k == -1 ) {
				_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , _i_SCH_SYSTEM_RUNORDER_GET( j ) + RUNORDER_INTERVAL_COUNT );
			}
			else {
				_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , ( _i_SCH_SYSTEM_RUNORDER_GET( j ) + _i_SCH_SYSTEM_RUNORDER_GET( k ) ) / 2 );
			}
		}
		else { // Hotrun
			_i_SCH_SYSTEM_RUNORDER_SET( CHECKING_SIDE , _i_SCH_SYSTEM_RUNORDER_GET( j ) / 2 );
		}
	}
	//------------------------------------------------------------------------
	SCHEDULER_CONTROL_Inf_Mode_SupplyID( CHECKING_SIDE , ( NormalMode == 2 ) ? 1 : 0 ); // 2008.12.18
	//------------------------------------------------------------------------
	SYSTEM_MODE_FIRST_SET( CHECKING_SIDE , !chktag );
	//------------------------------------------------------------------------
	if ( !chktag ) {
		_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 0 );
		switch( _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() ) {
		case 0 :
			SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , DefaultStartMode );
			break;
		case 1 :
			SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , 0 );
			break;
		case 2 :
			SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , 1 );
			break;
		case 3 :
			SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , 2 );
			break;
		case 4 :
			SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , 3 );
			break;
		}
	}
	else {
		switch( _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() ) {
		case 0 :
			_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , DefaultStartMode );
			break;
		case 1 :
			_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 0 );
			break;
		case 2 :
			_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 1 );
			break;
		case 3 :
			_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 2 );
			break;
		case 4 :
			_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , 3 );
			break;
		}
		SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) );
	}
	//------------------------------------------------------------------------
}

int SCHEDULER_GET_SIDE_CM( int side ) { // 2018.08.30
	int i , c;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	//
	if ( ( SYSTEM_IN_MODULE_GET(side) >= CM1 ) && ( SYSTEM_IN_MODULE_GET(side) < PM1 ) ) {
		return SYSTEM_IN_MODULE_GET(side) - CM1;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		c = _i_SCH_CLUSTER_Get_PathIn( side , i );
		//
		if ( ( c >= CM1 ) && ( c < PM1 ) ) return c - CM1;
		//
	}
	//
	return -1;
	//
}


BOOL SCHEDULER_SIDE_IN_POSSIBLE( int all , int side ) { // 2010.01.19
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	//
	if ( all != 2 ) { // 2011.05.28
		if ( _i_SCH_SYSTEM_BLANK_GET( side ) != 0 ) return FALSE; // 2011.04.20
		if ( _i_SCH_SYSTEM_RESTART_GET( side ) == 1 ) return FALSE; // 2011.09.23
		if ( _i_SCH_SYSTEM_RESTART_GET( side ) == 2 ) return FALSE; // 2012.07.21
	}
	//
	if ( ( SYSTEM_IN_MODULE_GET(side) >= CM1 ) && ( SYSTEM_IN_MODULE_GET(side) < PM1 ) ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( SYSTEM_IN_MODULE_GET(side) ) ) return TRUE;
	}
	else if ( SYSTEM_IN_MODULE_GET(side) >= BM1 ) {
		if ( all != 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( SYSTEM_IN_MODULE_GET(side) ) ) return TRUE;
		}
	}
	return FALSE;
}

BOOL SCHEDULER_SIDE_OUT_POSSIBLE( BOOL all , int side ) { // 2010.01.19
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	//
	if ( ( SYSTEM_OUT_MODULE_GET(side) >= CM1 ) && ( SYSTEM_OUT_MODULE_GET(side) < PM1 ) ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( SYSTEM_OUT_MODULE_GET(side) ) ) return TRUE;
	}
	else if ( SYSTEM_OUT_MODULE_GET(side) >= BM1 ) {
		if ( all ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( SYSTEM_OUT_MODULE_GET(side) ) ) return TRUE;
		}
	}
	return FALSE;
}

int  SCHEDULER_SIDE_GET_CM_IN( int side ) { // 2010.01.19
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	if ( ( SYSTEM_IN_MODULE_GET(side) < CM1 ) || ( SYSTEM_IN_MODULE_GET(side) >= PM1 ) ) return -1;
	return SYSTEM_IN_MODULE_GET(side);
}
int  SCHEDULER_SIDE_GET_CM_OUT( int side ) { // 2010.01.19
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	if ( ( SYSTEM_OUT_MODULE_GET(side) < CM1 ) || ( SYSTEM_OUT_MODULE_GET(side) >= PM1 ) ) return -1;
	return SYSTEM_OUT_MODULE_GET(side);
}

int SCHEDULER_CM_LOCKED_CHECK_FOR_UNLOAD( int cm ) { // 2012.04.01
	//
	int s , p;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) return 0;
	//
	return 0; // 2013.06.08

	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
			//
			switch( _i_SCH_CLUSTER_Get_PathStatus( s , p ) ) {
			case SCHEDULER_READY :
				if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) == cm ) return 1;
				break;
			case SCHEDULER_SUPPLY :
				if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) == cm ) return 11;
				break;
			case SCHEDULER_CM_END :
				if ( _i_SCH_CLUSTER_Get_PathOut( s , p ) == cm ) return 21;
				break;
			case SCHEDULER_RETURN_REQUEST :
				if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) == cm ) return 31;
				break;
			}
			//
		}
	}
	//
	return 0;
	//
}
//

BOOL SCHEDULER_LOG_FOLDER( int CHECKING_SIDE , char *Buffer , char *recipefile ) {
	int i , j;
	// 0
	// 1  JID		// 2  LOT		// 3  MID
	// 4  JID + LOT		// 5  JID + MID		// 6  LOT + JID
	// 7  LOT + MID		// 8  MID + JID		// 9  MID + LOT
	// 10 JID + LOT + MID		// 11 JID + MID + LOT		// 12 LOT + JID + MID
	// 13 LOT + MID + JID		// 14 MID + JID + LOT		// 15 MID + LOT + JID
	//
	KPLT_Local_Time_for_File( ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 1 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 5 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) , Buffer , _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() / 16 );
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() % 16 ) {
	case 1 : case 4 : case 5 : case 10 : case 11 :
		if ( strlen( _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) );
		}
		break;
	case 2 : case 6 : case 7 : case 12 : case 13 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 3 : case 8 : case 9 : case 14 : case 15 :
		if ( strlen( _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
		}
		break;
	}
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() ) {
	case 6 : case 8 : case 12 : case 14 :
		if ( strlen( _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) );
		}
		break;
	case 4 : case 9 : case 10 : case 15 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 5 : case 7 : case 11 : case 13 :
		if ( strlen( _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
		}
		break;
	}
	switch( _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() ) {
	case 13 : case 15 :
		if ( strlen( _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) );
		}
		break;
	case 11 : case 14 :
		if ( strlen( recipefile ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , recipefile );
		}
		break;
	case 10 : case 12 :
		if ( strlen( _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) ) > 0 ) {
			strcat( Buffer , "_" );
			strcat( Buffer , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
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
		else if ( Buffer[i] == ':'  ) Buffer[i] = '-'; // 2010.10.07
		else if ( Buffer[i] == '\'' ) Buffer[i] = '-';
	}

	j = 0;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != CHECKING_SIDE ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) >= 3 ) {
				if ( STRCMP_L( Buffer , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY(i) ) ) {
					j = 1;
					break;
				}
			}
		}
	}
	if ( j == 0 ) return TRUE;
	return FALSE;
}


void Scheduler_Run_Unload_FlagReset( int side , BOOL manual , BOOL cancel ) { // 2012.04.03
	int i , j;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( _i_SCH_SYSTEM_USING_GET( i ) < 5 ) continue;
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , side + CM1 ) != MUF_01_USE ) continue;
		//
		_i_SCH_MODULE_Set_Mdl_Use_Flag( i , side + CM1 , MUF_02_USE_to_END );
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( side + CM1 ) <= 0 ) {
			_i_SCH_LOG_LOT_PRINTF( i , "Module %s Run Flag Status Error 601%cMDLSTSERROR 601:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i+CM1 ) , 9 , i+CM1 );
		}
		else {
			_i_SCH_MODULE_Dec_Mdl_Run_Flag( side + CM1 );
		}
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_CM_END ) continue;
			//
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) != 2 ) {
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) != 3 ) { // 2013.06.12
					continue;
				}
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_READY ) { // 2013.05.27
				if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) != ( side + CM1 ) ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != ( side + CM1 ) ) {
						continue;
					}
				}
				// 2013.07.02
				if      ( _i_SCH_CLUSTER_Get_PathIn( i , j ) == ( side + CM1 ) ) {
					if      ( _i_SCH_CLUSTER_Get_PathIn( i , j ) == _i_SCH_CLUSTER_Get_PathOut( i , j ) ) {
						_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
					}
					else {
						if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) {
							_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
						}
						else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) < _MOTAG_10_NEED_UPDATE ) {
							_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_13_NEED_IN /* IN */ );
						}
					}
				}
				else if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) == ( side + CM1 ) ) {
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) {
						_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
					}
					else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) < _MOTAG_10_NEED_UPDATE ) {
						_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_14_NEED_OUT /* OUT */ );
					}
				}
			}
			else {
				if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) != ( side + CM1 ) ) continue;
				// 2013.07.02
				_i_SCH_CLUSTER_Set_Ex_MtlOut( i , j , 500 + _MOTAG_14_NEED_OUT /* OUT */ );
			}
			//
		}
		//
	}
	//
}

int Scheduler_Run_Unload_Code( int side , int cm , BOOL famode , BOOL Manual , int mode , int pointer , int wafer ) {
	//
	// mode
	//
	// 1 :	!MTLOUT	:	LOT Finish				:	Normal CM IN
	// 2 :	!MTLOUT	:	LOT Finish				:	Normal CM OUT
	// 3 :	MTLOUT	:	LOT Finish				:	Used CM Check for Nomore Use
	// 4 :	MTLOUT	:	After Place CM			:	CM Nomore Use
	// 5 :	MTLOUT	:	After Pick CM			:	CM Nomore Use
	// 6 :	MTLOUT	:	Sleep,After Place CM	:	(-200 보다 큰값 = 현재 CM에 돌아와야 할 Wafer)가 없을 때
	//
	int action = 1 , mrun;
	char Buffer[256];
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) return 0; // 2011.06.27
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.05.21
		//
		switch( mode ) {
		case 1 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [FINISH_IN :%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		case 2 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [FINISH_OUT:%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		case 3 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [FINISH_ETC:%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		case 4 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [AFT_PLACED:%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		case 5 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [AFT_PICKED:%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		case 6 :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [CHANGE_CM :%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		default :
			sprintf( Buffer , "==> MTLOUT UNLOAD => CM%d [UNKNOWN   :%d][S=%d] [MC=%d,%d,%d,%d(%d,%d,%d,%d)] [famode=%d][Manual=%d]" , cm - CM1 + 1 , mode , side , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) , famode , Manual );
			break;
		}
		//
		_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
		//
		SCHMULTI_SET_LOG_MESSAGE( Buffer );
		//
		//--------------------------------------------------------------------------------------------------------
		//
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) return TRUE; // 2011.06.27
		//
		//--------------------------------------------------------------------------------------------------------
		//
		BUTTON_SET_FLOW_MTLOUT_STATUS( side , cm - CM1 , FALSE , _MS_14_HANDOFFOUT ); // 2013.06.08
		//
		if ( Scheduler_HandOffOut_End_Part( side , cm , pointer , wafer ) == SYS_ABORTED ) {
			//
			BUTTON_SET_FLOW_MTLOUT_STATUS( side , cm - CM1 , FALSE , _MS_17_ABORTED ); // 2013.06.08
			//
			return -1;
		}
		//
		BUTTON_SET_FLOW_MTLOUT_STATUS( side , cm - CM1 , FALSE , _MS_15_COMPLETE ); // 2013.06.08
		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) { // 2012.04.04
			//
			BUTTON_SET_HANDOFF_CONTROL( cm , CTC_IDLE ); // 2011.05.27
			//
		}
		//--------------------------------------------------------------------------------------------------------
		LOG_MTL_ADD_CASS_COUNT( cm ); // 2013.06.19
		//--------------------------------------------------------------------------------------------------------
	}
	//
	/*
	// 2013.05.29
	//
	SCHMRDATA_Data_Setting_for_LoadUnload( cm , 10 ); // 2011.09.07
	//
	if ( famode ) {
		switch( _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() ) { // Use,AutoSkip,ManualSkip,AllSkip
		case 0 :
		case 2 :
			switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // Use,Skip,Use(Also Manual Use),Use(Also Manual Use-Loop Type),Use(Not Loop Type),Use(Not Loop Type/Also Manual Use),Use(Not Loop Type/Also Manual Use-Loop Type),{Use(Always-Manual)}
			case 0 :
			case 2 :
			case 3 :
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
				FA_UNLOAD_CONTROLLER( cm - CM1 , TRUE , Manual , FALSE , FALSE , TRUE );
				//
				action = 11;
				break;
			case 4 :
			case 5 :
			case 6 :
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
				FA_UNLOAD_CONTROLLER( cm - CM1 , FALSE , Manual , FALSE , FALSE , TRUE );
				//
				action = 12;
				break;
			case 7 : // 2011.04.20
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 ); // 2012.04.03
//				MANUAL_UNLOAD_CONTROLLER( cm , -1 , Manual , FALSE , FALSE , TRUE ); // 2012.07.10
				MANUAL_UNLOAD_CONTROLLER( cm , Manual , FALSE , FALSE , TRUE ); // 2012.07.10
				//
				action = 13;
				break;
			default :
				//
				action = 4;
				break;
			}
			break;
		default  :
			//
			action = 5;
			break;
		}
	}
	else {
		switch( _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() ) { // Use,AutoSkip,ManualSkip,AllSkip
		case 0 :
		case 1 :
			switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // Use,Skip,Use(Also Manual Use),Use(Also Manual Use-Loop Type),Use(Not Loop Type),Use(Not Loop Type/Also Manual Use),Use(Not Loop Type/Also Manual Use-Loop Type),{Use(Always-Manual)}
			case 2 :
			case 5 :
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
				FA_UNLOAD_CONTROLLER( cm - CM1 , FALSE , Manual , FALSE , FALSE , TRUE );
				//
				action = 14;
				break;
			case 3 :
			case 6 :
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
				FA_UNLOAD_CONTROLLER( cm - CM1 , TRUE , Manual , FALSE , FALSE , TRUE );
				//
				action = 15;
				break;
			default :
				FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
//				MANUAL_UNLOAD_CONTROLLER( cm , -1 , Manual , FALSE , FALSE , TRUE ); // 2012.07.10
				MANUAL_UNLOAD_CONTROLLER( cm , Manual , FALSE , FALSE , TRUE ); // 2012.07.10
				//
				action = 16;
				break;
			}
			break;
		default :
			//
			action = 6;
			break;
		}
	}
	*/
	//

	//==============================================================================================================================
	// 2013.05.29
	//
	mrun = -1;
	//
	if ( famode ) {
		switch( _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() ) { // Use,AutoSkip,ManualSkip,AllSkip
		case 0 :
		case 2 :
			switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // Use,Skip,Use(Also Manual Use),Use(Also Manual Use-Loop Type),Use(Not Loop Type),Use(Not Loop Type/Also Manual Use),Use(Not Loop Type/Also Manual Use-Loop Type),{Use(Always-Manual)}
			case 0 :
			case 2 :
			case 3 :
				//
				mrun = 1;			action = 11;
				break;
			case 4 :
			case 5 :
			case 6 :
				//
				mrun = 0;			action = 12;
				break;
			case 7 :
				//
				mrun = -1;			action = 13;
				break;
			default :
				//
				action = 4;
				break;
			}
			break;
		default  :
			//
			action = 5;
			break;
		}
	}
	else {
		switch( _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE() ) { // Use,AutoSkip,ManualSkip,AllSkip
		case 0 :
		case 1 :
			switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // Use,Skip,Use(Also Manual Use),Use(Also Manual Use-Loop Type),Use(Not Loop Type),Use(Not Loop Type/Also Manual Use),Use(Not Loop Type/Also Manual Use-Loop Type),{Use(Always-Manual)}
			case 2 :
			case 5 :
				//
				mrun = 0;			action = 14;
				break;
			case 3 :
			case 6 :
				//
				mrun = 1;			action = 15;
				break;
			default :
				//
				mrun = -1;			action = 16;
				break;
			}
			break;
		default :
			//
			action = 6;
			break;
		}
	}
	//
	if ( action >= 10 ) {
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( cm , 10 );
		//
		FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( cm - CM1 , 1 );
		//
		switch( mrun ) {
		case 0 :	FA_UNLOAD_CONTROLLER( cm - CM1 , FALSE , Manual , FALSE , FALSE , TRUE );	break;
		case 1 :	FA_UNLOAD_CONTROLLER( cm - CM1 , TRUE  , Manual , FALSE , FALSE , TRUE );	break;
		default :	MANUAL_UNLOAD_CONTROLLER( cm , Manual , FALSE , FALSE , TRUE );				break;
		}
	}
	//==============================================================================================================================
	/*
	// 2013.06.08
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.06.03
		if ( action < 10 ) {
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
			}
			else {
				BUTTON_SET_FLOW_STATUS( cm - CM1 , _MS_5_MAPPED );
			}
		}
	}
	*/
	//
	return action;
}


void SCHEDULER_CONTROL_Move_Data_Check_Clear( int side ) { // 2013.09.04
	int j , x;
	//
	// 2013.09.03
	//
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		//
		for ( x = 1 ; x <= MAX_PM_SLOT_DEPTH ; x++ ) {
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( j+PM1 , x ) > 0 ) break;
			//
		}
		//
		if ( x > MAX_PM_SLOT_DEPTH ) { // No Wafer
			//
			_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 0 );
			//
		}
		else {
			//
			if ( _i_SCH_SYSTEM_MOVEMODE_GET( side ) == 2 ) {
				//
				if ( _i_SCH_MODULE_GET_MOVE_MODE( j + PM1 ) == 2 ) {
					//
					_i_SCH_MODULE_SET_MOVE_MODE( j+PM1 , 1 );
					//
				}
				//
			}
			//
		}
	}
}

//===================================================================================================
void Scheduler_Run_Main_Handling_Code( int data ) {
	int	 i , j;
	int  Error;
//	char Buffer[128]; // 2004.05.19
//	char Buffer2[128]; // 2004.05.19
	char Buffer[256]; // 2004.05.19
//	char Buffer2[256]; // 2004.05.19 // 2004.09.03
	int cm[MAX_CASSETTE_SLOT_SIZE];
	int  CHECKING_SIDE , END_RUN;
//	long time_data;
//	BOOL CANCEL_MODE , FIRST_MODE , DIRDEL_MODE , RETURN_MODE , END_LOCK , ONCE_RUN; // 2009.04.13
	BOOL CANCEL_MODE , FIRST_MODE , DIRDEL_MODE , RETURN_MODE , ONCE_RUN; // 2009.04.13
	char ErrorappendString[256];

	//------------------------------------------
	CHECKING_SIDE = data % 10;
	//-----------------------------------------------------------------
	USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 1 , 0 , FALSE , 0 , "" ); // 2010.12.02
	//-----------------------------------------------------------------
	SYSTEM_PREPARE_SET( CHECKING_SIDE , 1 ); // 2008.04.23
	//-----------------------------------------------------------------
	_i_SCH_SYSTEM_FINISH_SET( CHECKING_SIDE , 0 ); // 2011.04.27
	//-----------------------------------------------------------------
	_i_SCH_SYSTEM_DBUPDATE_SET( CHECKING_SIDE , 0 ); // 2011.04.25
	//------------------------------------------
	SCHMULTI_AFTER_SELECT_CHECK_SET( 2 ); // 2004.06.26
	//------------------------------------------
	if ( _i_SCH_SYSTEM_MOVEMODE_GET( CHECKING_SIDE ) != 0 ) { // 2013.09.27
		_in_UNLOAD_USER_TYPE[CHECKING_SIDE] = 1; // 2010.04.26
	}
	else {
		_in_UNLOAD_USER_TYPE[CHECKING_SIDE] = 0; // 2010.04.26
	}
	//
	//------------------------------------------
	_in_HOT_LOT_ORDERING_INVALID[CHECKING_SIDE] = 0; // 2011.06.13
	//------------------------------------------
	if ( Scheduler_InfoCheck( CHECKING_SIDE , _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , 2 , FALSE ) ) { // 2010.02.19
		//
		_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , FALSE );
		//
	}
	else {
		//
		_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , TRUE );
		//
	}
	//
	if ( _i_SCH_IO_GET_MAIN_BUTTON( CHECKING_SIDE ) != CTC_IDLE ) {
		//
		if ( ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) || ( _i_SCH_IO_GET_MAIN_BUTTON( CHECKING_SIDE ) != CTC_WAIT_HAND_OFF ) ) { // 2014.03.04
			//
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) { // 2004.08.18 // FA
				j = 100;
			}
			else { // 2004.08.18 // Manual
				j = 1;
			}
			for ( i = 0 ; i < j ; i++ ) { // 2004.08.18
				//
				Sleep(100);
				//
				if ( _i_SCH_IO_GET_MAIN_BUTTON( CHECKING_SIDE ) == CTC_IDLE ) break;
				//
			}
			if ( i == j ) { // 2004.08.18
				//
	//			if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 1 ) { // 2012.01.03 // 2012.02.16
				if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2012.01.03 // 2012.02.16
					//
					MessageBeep(MB_ICONHAND);
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) FA_REJECT_MESSAGE( CHECKING_SIDE , FA_START , ERROR_JOB_RUN_ALREADY , "" );
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) ERROR_HANDLER( ERROR_JOB_RUN_ALREADY , "" );
					//
				}
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , -1 , FALSE ); // 2007.07.31
				//-----------------------------------------------------------
				//==========================================
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE ); // 2004.08.18
				//==========================================
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 0 ); // 2004.09.06
				//==========================================
				_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , FALSE ); // 2010.02.19
				//==========================================
				SYSTEM_PREPARE_SET( CHECKING_SIDE , 0 ); // 2008.04.23
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , -1 , FALSE , 0 , "" ); // 2010.12.02
				//-----------------------------------------------------------------
				_endthread(); // 2006.09.13
				//==========================================
				return;
			}
		}
	}
	//========================================================================================================
	// 2006.04.19
	//========================================================================================================
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2014.03.04
		if ( !_SCH_IO_GET_MAIN_BM_MODE( CHECKING_SIDE ) ) {
			if ( ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( CHECKING_SIDE ) > 0 ) || ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( CHECKING_SIDE ) > 0 ) ) {
				//
	//			if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 1 ) { // 2012.01.03 // 2012.02.16
				if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2012.01.03 // 2012.02.16
					//
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) FA_REJECT_MESSAGE( CHECKING_SIDE , FA_START , ERROR_JOB_RUN_ALREADY , "" );
					if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) ERROR_HANDLER( ERROR_JOB_RUN_ALREADY , "" );
					//
				}
				//-----------------------------------------------------------
				Scheduler_Fail_Finish_Check( CHECKING_SIDE , -1 , FALSE ); // 2007.07.31
				//-----------------------------------------------------------
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
				//==========================================
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 0 );
				//==========================================
				_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , FALSE ); // 2010.02.19
				//==========================================
				SYSTEM_PREPARE_SET( CHECKING_SIDE , 0 ); // 2008.04.23
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , -2 , FALSE , 0 , "" ); // 2010.12.02
				//-----------------------------------------------------------------
				_endthread(); // 2006.09.13
				//==========================================
				return;
			}
		}
	}
	//-----------------------------------------------------------------
	SCHEDULER_REG_SET_DISABLE( CHECKING_SIDE , FALSE );
	SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , FALSE );
	//-----------------------------------------------------------------
	_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_DISABLE );
	_i_SCH_IO_SET_END_BUTTON( CHECKING_SIDE , CTCE_IDLE );
	//
	//=============================================================================
	// 2005.12.14
	//=============================================================================
//	Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 99 ); // 2010.07.29
	Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , 0 ); // 2010.07.29
	//=============================================================================
	//
	EnterCriticalSection( &CR_MAIN );
	//
	//-----------------------------------------------------------------
	USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 1 , 1 , FALSE , 0 , "" ); // 2010.12.02
	//-----------------------------------------------------------------
	//
	SCHEDULER_CONTROL_Data_Initialize_Part( Get_RunPrm_RUNNING_CLUSTER() ? 1 : 3 , CHECKING_SIDE );
	//
	Scheduler_Run_Main_Handling_Pre_Part( CHECKING_SIDE , ( data % 100 ) / 10 , ( data % 1000 ) / 100 );

	//
	//-------------------------------------------
//	SCHEDULER_FIRST_MODULE_CONTROL_SET( !(data / 1000) ); // 2017.10.10
	//
	if ( (data / 1000) == 2 ) { // 2017.10.10
		//
		_in_NOCHECK_RUNNING_TYPE[CHECKING_SIDE] = 1;
		//
		SCHEDULER_FIRST_MODULE_CONTROL_SET( TRUE );
		//
	}
	else {
		//
		_in_NOCHECK_RUNNING_TYPE[CHECKING_SIDE] = 0;
		//
		SCHEDULER_FIRST_MODULE_CONTROL_SET( !(data / 1000) );
		//
	}
	//
	//-------------------------------------------
	FIRST_MODE = TRUE;
	//-------------------------------------------
	//==========================================
	SYSTEM_PREPARE_SET( CHECKING_SIDE , 0 ); // 2008.04.23
	//==========================================

	do {	
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 1 ); // 2008.04.23
		//-------------------------------------------
		if ( !FIRST_MODE ) {
			//--------------------------------------------
			Sleep(2000);
			//--------------------------------------------
		}
		//-------------------------------------------
		CANCEL_MODE = FALSE;
		//-------------------------------------------
//		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 1 ); // 2008.04.23
		//-------------------------------------------
		_i_SCH_SYSTEM_FLAG_RESET( CHECKING_SIDE );

		BUTTON_HANDLER_REMOTE( "START CTC%d" , CHECKING_SIDE + 1 );
		//-------------------------------------------
		if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) CANCEL_MODE = TRUE;
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 2 );
		//-------------------------------------------
		Error = Scheduler_InfoCheck( CHECKING_SIDE , _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , 0 , FALSE );
		if ( Error != ERROR_NONE ) {
			//==========================================
			_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
			//==========================================
			_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_IDLE );
			_i_SCH_IO_SET_END_BUTTON( CHECKING_SIDE , CTCE_IDLE );
			BUTTON_HANDLER_REMOTE( "STOP CTC%d" , CHECKING_SIDE + 1 );
			//-------------------------------------------
			_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 0 );
			//-------------------------------------------
			_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , FALSE );
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) FA_REJECT_MESSAGE( CHECKING_SIDE , FA_START , Error , "" );
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) ERROR_HANDLER( Error , "" );
			//============================================================
			// Append 2002.01.17
			//============================================================
			BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_17_ABORTED ); // 2013.06.08
			//============================================================
			//==========================================
			_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , FALSE ); // 2010.02.19
			//==========================================
			//-----------------------------------------------------------
			Scheduler_Fail_Finish_Check( CHECKING_SIDE , -1 , FALSE ); // 2007.07.31
			//-----------------------------------------------------------
			//-----------------------------------------------------------------
			USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , Error , FALSE , 0 , "" ); // 2010.12.02
			//-----------------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN );
			_endthread();
			return;
		}
		//-------------------------------------------
		// Move here 2001.09.11
		BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_7_START ); // 2013.06.08
		//------------------------------------------------------------------
		//-------------------------------------------
		if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) CANCEL_MODE = TRUE;
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 3 );
		//-------------------------------------------
//		for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) cm[i] = 0; // 2010.01.19
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) cm[i] = 0; // 2010.01.19
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i == CHECKING_SIDE ) {
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					if ( _i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , j ) >= CM1 ) {
						cm[_i_SCH_CLUSTER_Get_User_PathIn( CHECKING_SIDE , j )] = 1;
					}
					if ( _i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , j ) >= CM1 ) {
						cm[_i_SCH_CLUSTER_Get_User_PathOut( CHECKING_SIDE , j )] = 1;
					}
				}
			}
			else {
				if ( _i_SCH_SYSTEM_USING_GET( i ) > 4 ) {
					for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
						if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) >= CM1 ) {
							cm[_i_SCH_CLUSTER_Get_PathIn( i , j )] = 1;
						}
						if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) >= CM1 ) {
							cm[_i_SCH_CLUSTER_Get_PathOut( i , j )] = 1;
						}
					}
				}
			}
		}
		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2012.04.01
		}
		else {
			for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
				if ( cm[i] == 1 ) BUTTON_SET_HANDOFF_CONTROL( i , CTC_DISABLE );
			}
		}
		//
		if ( _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) != 1 ) {
			switch( _i_SCH_PRM_GET_FA_JID_READ_POINT(CHECKING_SIDE) ) {
			case 1 :
				if ( strlen( _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) ) <= 0 ) {
					IO_GET_JID_NAME_FROM_READ( CHECKING_SIDE , Buffer );
					_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Buffer );
				}
				break;
			case 2 :
				IO_GET_JID_NAME_FROM_READ( CHECKING_SIDE , Buffer );
				_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Buffer );
				break;
			case 3 :
				IO_GET_JID_NAME( CHECKING_SIDE , Buffer );
				_i_SCH_SYSTEM_SET_JOB_ID( CHECKING_SIDE , Buffer );
				break;
			}
		}
		//
		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.05.14
			//
			IO_SET_JID_NAME( CHECKING_SIDE , _i_SCH_SYSTEM_GET_JOB_ID( CHECKING_SIDE ) );
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2013.04.01
			}
			else {
				IO_SET_MID_NAME( CHECKING_SIDE , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) );
			}
			//
			while (TRUE) {
				//=======================================================================================================
				//=======================================================================================================
				//=======================================================================================================
				if ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 2 ) { // 2013.06.20
		//			if ( !_i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
					//
					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2014.12.02
						//
						if ( !SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2010.01.09
							j = 0;
							for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
								if ( i != CHECKING_SIDE ) {
									if ( _i_SCH_SYSTEM_USING_GET( i ) >= 3 ) {
										strcpy( Buffer , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY(i) );
										j = 1;
										break;
									}
								}
							}
							if ( j == 1 ) break;
						}
					}
				}
				//=======================================================================================================
				//=======================================================================================================
				//=======================================================================================================
				if ( SCHEDULER_LOG_FOLDER( CHECKING_SIDE , Buffer , _In_RecipeFileName[CHECKING_SIDE] ) ) break;
				//=======================================================================================================
				//=======================================================================================================
			}
		}

/*
// 2011.05.14
		_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( CHECKING_SIDE , Buffer );
//		IO_LOT_DIR_INITIAL( CHECKING_SIDE , Buffer2 ); // 2004.09.03
//		IO_LOT_DIR_INITIAL( CHECKING_SIDE , Buffer ); // 2004.09.03 // 2006.02.15
		IO_LOT_DIR_INITIAL( CHECKING_SIDE , Buffer , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2004.09.03 // 2006.02.15
		//
		if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( CHECKING_SIDE ) != 2 ) {
			_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_MID_SET , -1 , -1 , -1 , -1 , -1 , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) , "" );
		}
*/
		// 2011.05.14
		//
		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.05.14
			//
			_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( CHECKING_SIDE , Buffer );
			//
		}
		//
		IO_LOT_DIR_INITIAL( CHECKING_SIDE , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2004.09.03 // 2006.02.15
		//
//		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.05.14
		if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.04.20 // 2011.09.23
			//
			if ( _i_SCH_PRM_GET_FA_MID_READ_POINT( CHECKING_SIDE ) != 2 ) {
				_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_MID_SET , -1 , -1 , -1 , -1 , -1 , _i_SCH_SYSTEM_GET_MID_ID( CHECKING_SIDE ) , "" );
			}
			//
		}
		//-------------------------------------------
		_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_WAITING );
		_i_SCH_IO_SET_END_BUTTON( CHECKING_SIDE , CTCE_RUNNING );
		//-------------------------------------------
//		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) { // 2011.05.14
		if ( ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) == 0 ) && ( ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 0 ) || ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) == 3 ) ) ) { // 2011.09.23
			//
			if ( ( !CANCEL_MODE ) && ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) ) {
				switch( _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() ) {
				case 1 :
				case 3 : FA_STARTEND_STATUS_CONTROLLER( TRUE , FALSE ); break;
				case 4 :
				case 6 : FA_STARTEND_STATUS_CONTROLLER( TRUE , TRUE ); break;
				}
			}
			//
		}
		LeaveCriticalSection( &CR_MAIN );
		//
		DIRDEL_MODE = FALSE;
//		END_LOCK = FALSE; // 2004.03.10 // 2009.04.13
		//
		strcpy( ErrorappendString , "" ); // 2007.12.05
		//
		if ( !CANCEL_MODE ) {
			//
			SYSTEM_RUN_TIMER_SET( CHECKING_SIDE ); // 2010.12.02
			//
//			Error = Scheduler_Run_Main_Operation_Code( CHECKING_SIDE , FIRST_MODE , &RETURN_MODE , Buffer , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , &END_LOCK , &ONCE_RUN , ErrorappendString ); // 2009.04.13
//			Error = Scheduler_Run_Main_Operation_Code( CHECKING_SIDE , FIRST_MODE , &RETURN_MODE , Buffer , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , &ONCE_RUN , ErrorappendString ); // 2009.04.13 // 2011.05.19
			Error = Scheduler_Run_Main_Operation_Code( CHECKING_SIDE , FIRST_MODE , &RETURN_MODE , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , &ONCE_RUN , ErrorappendString ); // 2009.04.13 // 2011.05.19
			if ( !ONCE_RUN ) { // 2007.06.28
				if ( _i_SCH_IO_GET_MAIN_BUTTON( CHECKING_SIDE ) == CTC_WAITING ) {
					DIRDEL_MODE = TRUE;
				}
			}
			else { // 2010.12.02
				while( TRUE ) {
					if ( SYSTEM_RUN_TIMER_GET( CHECKING_SIDE ) >= 2000 ) break;
					Sleep(250);
				}
			}
		}
		else {
			Error = ERROR_SYSTEM_ERROR;
			DIRDEL_MODE = TRUE;
			RETURN_MODE = 1;
		}

		//================================================
		// 2008.02.26
		//================================================
//		SCHEDULER_Make_CLUSTER_INDEX_END( CHECKING_SIDE ); // 2008.02.26 // 2008.04.17
		//================================================
		if ( _i_SCH_SYSTEM_BLANK_GET( CHECKING_SIDE ) != 0 ) { // 2011.09.19
			SCHMRDATA_Data_Setting_for_Finishing( CHECKING_SIDE + CM1 );
		}
		//
		END_RUN = 0;

		if ( Error == ERROR_SYSTEM_ERROR ) {
			_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_STOP2 , 1 , -1 , -1 , -1 , -1 , "" , "" );
		}
		else if ( Error == ERROR_NONE ) {
			//-----------------------
			Scheduler_End_Part_Lock_Make( 1 ); // 2009.04.13
			//-----------------------
			if ( _In_Job_HandOff_Out_Message_Skip[CHECKING_SIDE] == 0 ) { // 2011.09.19
				if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) ) // 2004.03.22 // 2006.10.26
					_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_STOP , PRJOB_RESULT_ENDSTOP , ( ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) <= 2 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 5 ) ) ? -1 : 0 , -1 , -1 , -1 , "" , "" ); // 2004.03.22 // 2010.04.26
				else // 2004.03.22
					_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_STOP , PRJOB_RESULT_NORMAL  , ( ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) <= 2 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 5 ) ) ? -1 : 0 , -1 , -1 , -1 , "" , "" ); // 2004.03.22 // 2010.04.26
			}
			//-----------------------
			Scheduler_End_Part_Lock_Clear( 1 ); // 2004.03.11 // 2009.04.13
			//-----------------------
		}
		else {
			_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_STOP2 , 2 , -1 , -1 , -1 , -1 , "" , "" );
		}
		//==============================================================================
//		if ( END_LOCK ) Scheduler_End_Part_Lock_Clear(); // 2004.03.11 // 2009.04.13
		//==============================================================================
//		if ( Error == ERROR_SYSTEM_ABORTED ) { // 2008.03.13
		if ( ( Error == ERROR_SYSTEM_ABORTED ) || ( Error == ERROR_SYSTEM_ABORTED2 ) ) { // 2008.03.13
			//
			BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_17_ABORTED ); // 2013.06.08
			//
			if ( Error == ERROR_SYSTEM_ABORTED ) { // 2008.03.13
				//===========================================================================
				SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , TRUE );
				//===========================================================================
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i );
				//===========================================================================
			}
			else {
				if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					if ( _i_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
						SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , FALSE );
					}
					else {
						SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , TRUE );
						//===========================================================================
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i ); // 2006.04.28
						//===========================================================================
					}
				}
				else {
					if ( _i_SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
						SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , FALSE );
					}
					else {
						SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , TRUE );
						//===========================================================================
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i ); // 2006.04.28
						//===========================================================================
					}
				}
			}
			//===========================================================================
//			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i ); // 2006.04.28
			//===========================================================================
			END_RUN = 2;
			if ( Error == ERROR_SYSTEM_ABORTED ) { // 2008.03.13
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Aborted\tSCHABORTED\n" );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Aborted2\tSCHABORTED%d\n" , Error );
			}
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) FA_RESULT_MESSAGE( CHECKING_SIDE , FA_ABORTED , 0 );
			//
			//===========================================================================
			if ( PROCESS_MONITOR_STATUS_for_Abort_Remain( CHECKING_SIDE ) ) { // 2003.12.01
				if ( _beginthread( (void *) Scheduler_Run_Abort_But_Remain_Check_Code , 0 , (void * ) CHECKING_SIDE ) == -1 ) { // 2004.03.18
					//----------------------------------------------------------------------------------------------------------------
					// 2004.08.18
					//----------------------------------------------------------------------------------------------------------------
					_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Run_Abort_But_Remain_Check_Code %d\n" , CHECKING_SIDE );
					//----------------------------------------------------------------------------------------------------------------
				}
//				ERROR_HANDLER( ERROR_ABORT_BUT_REMAIN_PROCESS , "" ); // 2003.12.01 // 2004.03.18
			} // 2003.12.01
			//===========================================================================
			//
			OPERATE_CONTROL( CHECKING_SIDE , 4 );
			//
		}
		else if ( Error == ERROR_SYSTEM_ERROR ) {
			//
			BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_16_CANCEL ); // 2013.06.08
			//
			SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , FALSE );
			END_RUN = 1;
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Cancel\tSCHCANCEL\n" );
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) FA_RESULT_MESSAGE( CHECKING_SIDE , FA_CANCELED , 0 );
			//
			OPERATE_CONTROL( CHECKING_SIDE , 5 );
			//
		}
		else if ( Error != ERROR_NONE ) {
			//
			BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_17_ABORTED ); // 2013.06.08
			//
			SYSTEM_CANCEL_DISABLE_SET( CHECKING_SIDE , FALSE );
			END_RUN = 2;
//			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Fail\tSCHFAIL\n" ); // 2010.11.03
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Fail\tSCHFAIL:%d\n" , Error ); // 2010.11.03
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) {
				if ( Error == ERROR_READY_FAILURE ) {
					FA_RESULT_MESSAGE( CHECKING_SIDE , FA_ABORTED , 0 );
				}
				else if ( Error == ERROR_HANDOFF_IN_FAILURE ) {
					FA_RESULT_MESSAGE( CHECKING_SIDE , FA_ABORTED , 0 );
				}
				else if ( Error == ERROR_END_FAILURE ) {
					FA_RESULT_MESSAGE( CHECKING_SIDE , FA_ABORTED , 0 );
				}
				else if ( Error == ERROR_HANDOFF_OUT_FAILURE ) {
					FA_RESULT_MESSAGE( CHECKING_SIDE , FA_ABORTED , 0 );
				}
				else {
					FA_REJECT_MESSAGE( CHECKING_SIDE , FA_START , Error , "" );
				}
			}
			if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 1 ) {
				ERROR_HANDLER( Error , ErrorappendString );
			}
			else {
				//----------------------------------------------------------------
				if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) { // 2004.06.16
					ERROR_HANDLER( Error + 1000 , ErrorappendString );
				}
				//----------------------------------------------------------------
			}
			//
			OPERATE_CONTROL( CHECKING_SIDE , 5 );
			//
		}
		else {
			//===========================================================================
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 1 ) {
				if ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) > 0 ) {
					BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE ); // 2013.06.12
				}
				else {
					if ( Scheduler_HandOffOut_Skip_Get( CHECKING_SIDE + CM1 ) ) {
						BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_5_MAPPED ); // 2013.06.12
					}
					else {
						if      ( BUTTON_GET_FLOW_STATUS_VAR( CHECKING_SIDE ) == _MS_12_RUNNING ) { // 2011.05.30
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE ); // 2013.06.12
						}
						else if ( BUTTON_GET_FLOW_STATUS_VAR( CHECKING_SIDE ) == _MS_13_MAPOUT ) { // 2011.05.30
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE ); // 2013.06.12
						}
					}
				}
			}
			else if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2018.10.05
				//
				if      ( ( BUTTON_GET_FLOW_STATUS_VAR( CHECKING_SIDE ) == _MS_12_RUNNING ) || ( BUTTON_GET_FLOW_STATUS_VAR( CHECKING_SIDE ) == _MS_13_MAPOUT ) ) {
					if ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] / 10 ) == 1 ) {
						//
						if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) {
							if ( SCHMULTI_CASSETTE_VERIFY_MAKE_SET_AGAIN( CHECKING_SIDE ) ) {
								BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_5_MAPPED );
							}
							else {
								BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE );
							}
						}
						else {
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE );
						}
						//
					}
					else {
						BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE );
					}
				}
				//
			}
			else {
				if ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] / 10 ) == 1 ) { // 2017.05.22
					//
					if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) {
						if ( SCHMULTI_CASSETTE_VERIFY_MAKE_SET_AGAIN( CHECKING_SIDE ) ) {
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_5_MAPPED );
						}
						else {
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE );
						}
					}
					else {
						BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE );
					}
					//
				}
				else {
					BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , -1 , FALSE , _MS_15_COMPLETE ); // 2013.06.12
				}
			}
			//===========================================================================
			//
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Scheduler System Complete%cSCHCOMPLETE\n" , 9 );
			//
			//===========================================================================
			if ( SCHEDULER_CONTROL_Get_Disable_Disappear( CHECKING_SIDE ) ) { // 2003.12.10
				ERROR_HANDLER( ERROR_WAFER_DISAPPEAR , "" ); // 2003.12.10
			} // 2003.12.10
			//===========================================================================
			//
			OPERATE_CONTROL( CHECKING_SIDE , 3 );
			//
		}

		if ( Error != ERROR_NONE ) { // 2002.07.23
			if ( _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() % 2 ) {
				SCHEDULER_CASSETTE_LAST_RESULT_INCOMPLETE_FAIL( CHECKING_SIDE );
			}
		}
		EnterCriticalSection( &CR_MAIN );
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 120 );
		//-------------------------------------------
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.27
			//
			if ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) > 0 ) { // 2011.04.14
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
					for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) cm[i] = 0;
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						if ( i != CHECKING_SIDE ) {
							if ( _i_SCH_SYSTEM_USING_GET( i ) > 4 ) {
								for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
									if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) >= CM1 ) {
										cm[_i_SCH_CLUSTER_Get_PathIn( i , j )] = 1;
									}
									if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) >= CM1 ) {
										cm[_i_SCH_CLUSTER_Get_PathOut( i , j )] = 1;
									}
								}
							}
						}
					}
					for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
						if ( cm[i] == 0 ) BUTTON_SET_HANDOFF_CONTROL( i , CTC_IDLE );
					}
				}
			}
		}
		//
		BUTTON_HANDLER_REMOTE( "STOP CTC%d" , CHECKING_SIDE + 1 );
		//
		if ( END_RUN != 0 ) break;

		FIRST_MODE = FALSE;
		//-------------------------------------------
//		if ( !_i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) break; // 2004.09.07 // 2010.01.19
		if ( !SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) break; // 2004.09.07 // 2010.01.19
		//-------------------------------------------
	}
	while( _i_SCH_MULTIREG_HAS_REGIST_DATA( CHECKING_SIDE ) && !SCHEDULER_REG_GET_DISABLE( CHECKING_SIDE ) );

	_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );

	//-------------------------------------------
	_SCH_SYSTEM_SIDE_CLOSING_SET( CHECKING_SIDE , TRUE ); // 2013.10.02
	//-------------------------------------------

	if ( END_RUN != 2 ) {
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 121 );
		//-------------------------------------------
		LeaveCriticalSection( &CR_MAIN );
		//-------------------------------------------
		if ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) != 0 ) {
			if ( END_RUN == 0 ) {
				if ( _In_Job_HandOff_Out_Message_Skip[CHECKING_SIDE] == 0 ) { // 2011.09.19
					if ( ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) <= 1 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 3 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 6 ) ) { // 2010.04.26
						if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) ) { // 2006.10.26
							FA_RESULT_MESSAGE( CHECKING_SIDE , FA_COMPLETE , PRJOB_RESULT_ENDSTOP ); // END or STOP
							//
							if ( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) >= 0 ) { // 2009.04.08 // 2010.01.19
								FA_RESULT_MESSAGE( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) - CM1 , FA_COMPLETE , PRJOB_RESULT_ENDSTOP ); // 2010.01.19
							}
						}
						else {
							FA_RESULT_MESSAGE( CHECKING_SIDE , FA_COMPLETE , PRJOB_RESULT_NORMAL );
							//
							if ( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) >= 0 ) { // 2009.04.08 // 2010.01.19
								FA_RESULT_MESSAGE( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) - CM1 , FA_COMPLETE , PRJOB_RESULT_NORMAL ); // 2010.01.19
							}
						}
					}
				}
			}
		}
		//-------------------------------------------
//printf( "[SIDE=%d] UNLOAD_PART => %d , %d , %d\n" , CHECKING_SIDE , SCHEDULER_SIDE_IN_POSSIBLE( TRUE , CHECKING_SIDE ) , _in_UNLOAD_USER_TYPE[CHECKING_SIDE] , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) );
		//
		if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07 // 2010.01.19
			//
			switch( _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() ) {
			case 2 :
			case 3 : FA_STARTEND_STATUS_CONTROLLER( FALSE , FALSE ); break;
			case 5 :
			case 6 : FA_STARTEND_STATUS_CONTROLLER( FALSE , TRUE ); break;
			}
		}
		//
		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
			//
			EnterCriticalSection( &CR_MAIN );
			//
			//--------------------------------------------------------------------------------------------------------
			sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [FINISHING  ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , 0 , 0 , CHECKING_SIDE , -1 , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 3 ) );
			//--------------------------------------------------------------------------------------------------------
			_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
			SCHMULTI_SET_LOG_MESSAGE( Buffer );
			//--------------------------------------------------------------------------------------------------------
			//
			for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) continue;
				//
				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END );
				_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
				//
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 103%cMDLSTSERROR 103:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
					_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
				}
				//
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 0 ) {
					//
					//--------------------------------------------------------------------------------------------------------
					sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [FINISHED   ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , i - CM1 + 1 , 0 , CHECKING_SIDE , -1 , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 3 ) );
					//--------------------------------------------------------------------------------------------------------
					_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
					SCHMULTI_SET_LOG_MESSAGE( Buffer );
					//--------------------------------------------------------------------------------------------------------
					continue;
				}
				//
				LeaveCriticalSection( &CR_MAIN );
				//
				j = Scheduler_Run_Unload_Code( CHECKING_SIDE , i , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , ( END_RUN == 1 ) , 3 , -1 , -1 );
				if ( j < 0 ) {
				}
				else if ( j >= 10 ) {
				}
				//
				EnterCriticalSection( &CR_MAIN );
			}
			//
			LeaveCriticalSection( &CR_MAIN );
			//
		}
		else {
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.05.21
				//
		//2011.05.03
				if ( ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 0 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) >= 5 ) ) { // 2010.04.26
					//
					EnterCriticalSection( &CR_MAIN );
					//
					//--------------------------------------------------------------------------------------------------------
					sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [FINISHING  ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , 0 , 0 , CHECKING_SIDE , -1 , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 3 ) );
					//--------------------------------------------------------------------------------------------------------
					_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
					SCHMULTI_SET_LOG_MESSAGE( Buffer );
					//--------------------------------------------------------------------------------------------------------
					//
					for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
						//
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) continue;
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END );
						_i_SCH_MODULE_Dec_Mdl_Run_Flag( i ); // 2011.05.03
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 103%cMDLSTSERROR 103:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
							_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
						}
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 0 ) {
							//
							//--------------------------------------------------------------------------------------------------------
							sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [FINISHED   ] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , i - CM1 + 1 , 0 , CHECKING_SIDE , -1 , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 3 ) );
							//--------------------------------------------------------------------------------------------------------
							_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
							SCHMULTI_SET_LOG_MESSAGE( Buffer );
							//--------------------------------------------------------------------------------------------------------
							if ( ( i - CM1 ) == CHECKING_SIDE ) { // 2011.06.15
								BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , i - CM1 , FALSE , _MS_5_MAPPED );
							}
							//--------------------------------------------------------------------------------------------------------
							continue;
						}
						//
						if ( SCHMULTI_CHECK_ANOTHER_OUT_CASS_USING( FALSE , TRUE , FALSE , i , &j ) ) { // 2011.06.01
							//
							//--------------------------------------------------------------------------------------------------------
							sprintf( Buffer , "==> MTLOUT UNLOAD CHECKED [FINISHED_OC] => CM%d:%d [S=%d,P=%d][CURRENT_USE=%d,%d,%d,%d(%d,%d,%d,%d)]" , i - CM1 + 1 , 0 , CHECKING_SIDE , -1 , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , CM1 + 3 ) );
							//--------------------------------------------------------------------------------------------------------
							_IO_CONSOLE_PRINTF( "%s\n" , Buffer );
							SCHMULTI_SET_LOG_MESSAGE( Buffer );
							//--------------------------------------------------------------------------------------------------------
							//
							BUTTON_SET_FLOW_MTLOUT_STATUS( CHECKING_SIDE , i - CM1 , FALSE , _MS_5_MAPPED ); // 2011.06.01
							//
							//--------------------------------------------------------------------------------------------------------
							Scheduler_HandOffOut_Skip_Set( i ); // 2011.06.01
							//--------------------------------------------------------------------------------------------------------
							//
							continue;
						}
						//
						LeaveCriticalSection( &CR_MAIN );
						//
						SCHMULTI_CHECK_HANDOFF_OUT_MULTI_SET_DATA_FOR_OUTCASS( CHECKING_SIDE ); // 2011.08.16
						//
						j = Scheduler_Run_Unload_Code( CHECKING_SIDE , i , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , ( END_RUN == 1 ) , 3 , -1 , -1 );
						if ( j < 0 ) {
						}
						else if ( j >= 10 ) {
						}
						//
						EnterCriticalSection( &CR_MAIN );
					}
					//
					LeaveCriticalSection( &CR_MAIN );
					//
				}
			}
			else { // 2011.05.21
				//
				if ( SCHEDULER_SIDE_IN_POSSIBLE( 1 , CHECKING_SIDE ) ) { // 2004.09.07 // 2010.01.19
					//
					if ( ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) == 0 ) || ( ( _in_UNLOAD_USER_TYPE[CHECKING_SIDE] % 10 ) >= 5 ) ) { // 2010.04.26
						if ( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) >= 0 ) { // 2010.01.19
							//
							Scheduler_Run_Unload_Code( CHECKING_SIDE , SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , ( END_RUN == 1 ) , 1 , -1 , -1 );
							//
						}
						//
						if ( ( SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) >= 0 ) && ( SCHEDULER_SIDE_GET_CM_IN(CHECKING_SIDE) != SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) ) ) { // 2010.01.19
							//
							Scheduler_Run_Unload_Code( CHECKING_SIDE , SCHEDULER_SIDE_GET_CM_OUT(CHECKING_SIDE) , ( _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) == 1 ) , ( END_RUN == 1 ) , 2 , -1 , -1 );
							//
						}
						//
					}
					//
				}
			}
			//
		}
		// 2011.03.21
		//
		EnterCriticalSection( &CR_MAIN );
	}
	//-------------------------------------------
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 122 );
	//-------------------------------------------
	//-------------------------------------------
	SCHEDULER_REG_SET_DISABLE( CHECKING_SIDE , FALSE );
	//-------------------------------------------
//	if ( DIRDEL_MODE ) { // 2008.12.12
//		_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_LOG_DEL , -1 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Buffer ); // 2008.12.12
//	} // 2008.12.12
//
	if ( ( _i_SCH_PRM_GET_DFIX_LOT_LOG_MODE() / 2 ) > 0 ) { // 2010.11.16
		if ( DIRDEL_MODE ) { // 2008.12.12
//			_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_LOG_DEL , -1 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Buffer ); // 2008.12.12 // 2011.05.19
			_i_SCH_LOG_TIMER_PRINTF( CHECKING_SIDE , TIMER_LOG_DEL , -1 , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2008.12.12 // 2011.05.19
		} // 2008.12.12
	}
	//------------------------------------------------------------------------------
//	RECIPE_FILE_PROCESS_File_Locking_Delete( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , CHECKING_SIDE ); // 2010.09.09
	//------------------------------------------------------------------------------
	_i_SCH_SYSTEM_BLANK_SET( CHECKING_SIDE , 0 ); // 2011.04.20
	//
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 0 );
	_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , FALSE );
	//-------------------------------------------
	if ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) > 0 ) IO_CLEAR_JID_NAME_FROM_READ( CHECKING_SIDE ); // 2011.05.14
	//-------------------------------------------
	if ( SYSTEM_IN_MODULE_GET(CHECKING_SIDE) > 0 ) IO_CLEAR_MID_NAME_FROM_READ( CHECKING_SIDE ); // 2011.04.14
	//-------------------------------------------
	SCHEDULER_CONTROL_Data_Initialize_Part( Get_RunPrm_RUNNING_CLUSTER() ? 2 : 4 , CHECKING_SIDE ); // 2007.09.06
	//------------------------------------------------------------------------------------
	_i_SCH_IO_SET_END_BUTTON( CHECKING_SIDE , CTCE_IDLE ); // 2006.02.16
	//---------------------------------------------------
	_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_IDLE ); // 2006.02.16
	//==========================================
	_SCH_IO_SET_MAIN_BM_MODE( CHECKING_SIDE , FALSE ); // 2010.02.19
	//==========================================
	_SCH_SYSTEM_SIDE_CLOSING_SET( CHECKING_SIDE , FALSE ); // 2013.10.02
	//-------------------------------------------
	SCHEDULER_CONTROL_Make_Clear_Method_Data( CHECKING_SIDE ); // 2012.04.25
	//-----------------------------------------------------------------
	SCHEDULER_CONTROL_Move_Data_Check_Clear( CHECKING_SIDE ); // 2013.09.04
	//-----------------------------------------------------------------
	USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , Error , FALSE , 0 , "" ); // 2010.12.02
	//-----------------------------------------------------------------
	LeaveCriticalSection( &CR_MAIN );
	_endthread();
}

//===================================================================================================
//===================================================================================================
//===================================================================================================
int Scheduler_Run_Main_Handling_Code_CheckOnly( int data , char *errorstring ) {
	int  Error , CHECKING_SIDE;
	int	 tm;
	int curres;
	int loopcnt; // 2014.05.13
	//
	CLUSTERStepTemplate4 CLUSTER_INFO4; // 2015.07.28
	//---------------------------------------

	//-----------------------------------------------------------------
	
//	CHECKING_SIDE = data % 10; // 2006.11.23

	//-----------------------------------------------------------------
	EnterCriticalSection( &CR_MAIN );

	//-----------------------------------------------------------------
	CHECKING_SIDE = data % 10; // 2006.11.23
	//-----------------------------------------------------------------
	USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 1 , 1 , TRUE , 0 , "" ); // 2010.12.02
	//-----------------------------------------------------------------

	strcpy( errorstring , "" );

	SCHEDULER_CONTROL_Data_Initialize_Part( Get_RunPrm_RUNNING_CLUSTER() ? 1 : 3 , CHECKING_SIDE );

	Scheduler_Run_Main_Handling_Pre_Part( CHECKING_SIDE , ( data % 100 ) / 10 , ( data % 1000 ) / 100 );

	if ( _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) == 0 ) {
		//==========================================
		_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
		//==========================================
		//-----------------------------------------------------------------
		USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 1000 , TRUE , 0 , "" ); // 2010.12.02
		//-----------------------------------------------------------------
		LeaveCriticalSection( &CR_MAIN );
		return 1000;
	}

	//-------------------------------------------
	Error = Scheduler_InfoCheck( CHECKING_SIDE , _i_SCH_SYSTEM_FA_GET( CHECKING_SIDE ) , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , 1 , FALSE );
	if ( Error != ERROR_NONE ) {
		//==========================================
		_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
		//==========================================
		//-----------------------------------------------------------------
		USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 2000 + Error , TRUE , 0 , "" ); // 2010.12.02
		//-----------------------------------------------------------------
		LeaveCriticalSection( &CR_MAIN );
		return 2000 + Error;
	}
	//
//	Error = Scheduler_CONTROL_RECIPE_SETTING( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , &tm , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , TRUE , errorstring , 1 ); // 2014.05.13
	//
	loopcnt = 1; // 2014.05.13
	//
	Error = Scheduler_CONTROL_RECIPE_SETTING( CHECKING_SIDE , _In_RecipeFileName[CHECKING_SIDE] , _In_SL_1[CHECKING_SIDE] , _In_SL_2[CHECKING_SIDE] , &tm , _i_SCH_SYSTEM_CPJOB_GET( CHECKING_SIDE ) , TRUE , errorstring , &loopcnt ); // 2014.05.13
	//
	if ( Error == ERROR_NONE ) {
		if ( _SCH_COMMON_USER_RECIPE_CHECK( 211 , CHECKING_SIDE , 0 , 0 , 0 , &curres ) ) { // 2012.01.12
			Error = curres;
		}
	}
	//
	if ( Error != ERROR_NONE ) {
		//==========================================
		_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
		//==========================================
		//-----------------------------------------------------------------
		USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 3000 + Error , TRUE , 0 , "" ); // 2010.12.02
		//-----------------------------------------------------------------
		LeaveCriticalSection( &CR_MAIN );
		return 3000 + Error;
	}
	//
	//
	// 2015.07.28
	//
	//
	//
	if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() || USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() ) {
		//
		Error = 0;
		//
		for ( loopcnt = 0 ; loopcnt < MAX_CASSETTE_SLOT_SIZE ; loopcnt++ ) {
			//
			SCHEDULER_CONTROL_RunData_Make_Verification_Info( CHECKING_SIDE , loopcnt , &G_CLUSTER_RECIPE_INF , &CLUSTER_INFO4 );
			//
			if ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE != -1 ) Error++;
			//
			if ( USER_RECIPE_INFO_VERIFICATION_API_EXIST() ) {
				//
				curres = USER_RECIPE_INFO_VERIFICATION( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , -1 , loopcnt , G_CLUSTER_RECIPE_INF );
				//
				if ( curres != 1 ) {
					//==========================================
					_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
					//==========================================
					//-----------------------------------------------------------------
					USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4001 , TRUE , 0 , "" );
					//-----------------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN );
					return 4001;
				}
			}
			if ( USER_RECIPE_INFO_VERIFICATION_EX_API_EXIST() ) {
				//
				curres = USER_RECIPE_INFO_VERIFICATION_EX( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , -1 , ( ( G_CLUSTER_RECIPE_INF.HANDLING_SIDE == -1 ) ? 0 : Error ) , loopcnt , G_CLUSTER_RECIPE_INF , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA );
				//
				if ( curres != 1 ) {
					//==========================================
					_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
					//==========================================
					//-----------------------------------------------------------------
					USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4002 , TRUE , 0 , "" );
					//-----------------------------------------------------------------
					LeaveCriticalSection( &CR_MAIN );
					return 4002;
				}
			}
		}
	}
	//===============================================================================================================
	if ( USER_RECIPE_INFO_VERIFICATION_EX2_API_EXIST() ) {
		//
		Error = 0;
		//
		for ( loopcnt = 0 ; loopcnt < MAX_CASSETTE_SLOT_SIZE ; loopcnt++ ) {
			//
			SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , loopcnt , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
			//
			if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) Error++;
			//
			curres = USER_RECIPE_INFO_VERIFICATION_EX2( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , -1 , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : Error ) , loopcnt , G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
			//
			if ( curres != 1 ) {
				//==========================================
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4003 , TRUE , 0 , "" );
				//-----------------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				return 4003;
			}
		}
	}
	//===============================================================================================================
	if ( USER_RECIPE_INFO_VERIFICATION_EX3_API_EXIST() ) {
		//
		Error = 0;
		//
		for ( loopcnt = 0 ; loopcnt < MAX_CASSETTE_SLOT_SIZE ; loopcnt++ ) {
			//
			SCHEDULER_CONTROL_RunData_Make_Verification_Info2( CHECKING_SIDE , loopcnt , &G_CLUSTER_RECIPE , &CLUSTER_INFO4 );
			//
			if ( G_CLUSTER_RECIPE.HANDLING_SIDE != -1 ) Error++;
			//
			curres = USER_RECIPE_INFO_VERIFICATION_EX3( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , -1 , ( ( G_CLUSTER_RECIPE.HANDLING_SIDE == -1 ) ? 0 : Error ) , loopcnt , &G_CLUSTER_RECIPE , CLUSTER_INFO4.LOT_SPECIAL_DATA , CLUSTER_INFO4.LOT_USER_DATA , CLUSTER_INFO4.CLUSTER_USER_DATA , CLUSTER_INFO4.RECIPE_TUNE_DATA );
			//
			if ( curres != 1 ) {
				//==========================================
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4004 , TRUE , 0 , "" );
				//-----------------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				return 4004;
			}
		}
	}
	//===============================================================================================================
	if ( USER_RECIPE_INFO_VERIFICATION_EX4_API_EXIST() ) {
		//
		Error = 0;
		//
		for ( loopcnt = 0 ; loopcnt < MAX_CASSETTE_SLOT_SIZE ; loopcnt++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , loopcnt ) >= 0 ) Error++;
			//
			curres = USER_RECIPE_INFO_VERIFICATION_EX4( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 0 , -1 , _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , loopcnt ) < 0 ? 0 : Error , loopcnt , &_SCH_INF_CLUSTER_DATA_AREA[ CHECKING_SIDE ][ loopcnt ] );
			//
			if ( curres != 1 ) {
				//==========================================
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4005 , TRUE , 0 , "" );
				//-----------------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				return 4005;
			}
		}
		//
		Error = 0;
		//
		for ( loopcnt = 0 ; loopcnt < MAX_CASSETTE_SLOT_SIZE ; loopcnt++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , loopcnt ) >= 0 ) Error++;
			//
			curres = USER_RECIPE_INFO_VERIFICATION_EX4( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , 1 , -1 , _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , loopcnt ) < 0 ? 0 : Error , loopcnt , &_SCH_INF_CLUSTER_DATA_AREA[ CHECKING_SIDE ][ loopcnt ] );
			//
			if ( curres != 1 ) {
				//==========================================
				_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
				//==========================================
				//-----------------------------------------------------------------
				USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 4006 , TRUE , 0 , "" );
				//-----------------------------------------------------------------
				LeaveCriticalSection( &CR_MAIN );
				return 4006;
			}
		}
	}
	//
	if ( USER_RECIPE_START_MODE_CHANGE_API_EXIST() ) {
		//
		if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
			curres = SYSTEM_MODE_FIRST_GET_BUFFER( CHECKING_SIDE );
		}
		else {
			curres = _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE );
		}
		//======================================================================
		SYSTEM_MODE_BUFFER_SET( CHECKING_SIDE , curres );
		//======================================================================
//		if ( !USER_RECIPE_START_MODE_CHANGE( SYSTEM_RID_GET( CHECKING_SIDE ) , CHECKING_SIDE , -1 , SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) , &curres ) ) { // 2015.08.06
		if ( !USER_RECIPE_START_MODE_CHANGE( CHECKING_SIDE , CHECKING_SIDE , -1 , SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) , &curres ) ) { // 2015.08.06
			//==========================================
			_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
			//==========================================
			//-----------------------------------------------------------------
			USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 5000 , TRUE , 0 , "" );
			//-----------------------------------------------------------------
			LeaveCriticalSection( &CR_MAIN );
			return 5000;
		}
		//
		if ( ( curres >= 0 ) && ( curres <= 3 ) ) {
			if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) {
				SYSTEM_MODE_FIRST_SET_BUFFER( CHECKING_SIDE , curres );
			}
			else {
				_i_SCH_SYSTEM_MODE_SET( CHECKING_SIDE , curres );
			}
		}
	}
	//
	//
	//
	//
	//
	//
	//==========================================
	_i_SCH_MULTIREG_DATA_RESET( CHECKING_SIDE );
	//==========================================
	//-----------------------------------------------------------------
	USER_RECIPE_INFO_FLOWING( CHECKING_SIDE , 2 , 0 , TRUE , 0 , "" ); // 2010.12.02
	//-----------------------------------------------------------------
	LeaveCriticalSection( &CR_MAIN );

	return 0;
}
//===================================================================================================
//===================================================================================================
//===================================================================================================



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_MULTIJOB;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_PathOutCheck( int side ) { // 2011.04.20
	int i , res , return_impossible;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) continue;
		//
		res = SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , i , FALSE , &return_impossible , FALSE );
		if ( res < 0 ) return res;
		//
	}
	//
	return 0;
}

void SCHEDULER_CONTROL_Make_Finish_Data( int side , int clear_wfr_Info ) {
	//
	if ( clear_wfr_Info != -1 ) {
		_i_SCH_SYSTEM_FINISH_SET( side , ( clear_wfr_Info == 0 ) ? 1 : 2 );
	}
	//
	_i_SCH_MODULE_Set_FM_End_Status_FDHC( side );
	_i_SCH_MODULE_Set_TM_End_Status( side );
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
		_i_SCH_MODULE_Set_TM_InCount( side , _i_SCH_MODULE_Get_TM_OutCount( side ) );
		_i_SCH_MODULE_Set_FM_InCount( side , _i_SCH_MODULE_Get_FM_OutCount( side ) );
	}
	else {
		_i_SCH_MODULE_Set_TM_InCount( side , _i_SCH_MODULE_Get_TM_OutCount( side ) );
	}
	//
}

//
void SCHEDULER_CONTROL_Make_Clear_Method_Data( int side ) {
	int i , j , x;
	//
	if ( _i_SCH_SYSTEM_FINISH_GET( side ) == 0 ) return;
	//
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		SCHEDULER_CONTROL_Make_Finish_Data( side , -1 );
		//
		if ( _i_SCH_SYSTEM_FINISH_GET( side ) != 1 ) continue;
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				//
				if ( _i_SCH_MODULE_Get_PM_WAFER( PM1 + i , j ) <= 0 ) continue;
				if ( _i_SCH_MODULE_Get_PM_SIDE( PM1 + i , j ) != side ) continue;
				//
				_i_SCH_MODULE_Set_PM_WAFER( PM1 + i , j , 0 );
				//
			}
		}
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				//
				if ( _i_SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) continue;
				if ( _i_SCH_MODULE_Get_TM_SIDE( i , j ) != side ) continue;
				//
				_i_SCH_MODULE_Set_TM_WAFER( i , j , 0 );
				//
			}
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			//
			for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
				//
				if ( _i_SCH_MODULE_Get_BM_WAFER( BM1 + i , j ) <= 0 ) continue;
				if ( _i_SCH_MODULE_Get_BM_SIDE( BM1 + i , j ) != side ) continue;
				//
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( BM1 + i , j , 0 , -1 );
				//
			}
		}
		//
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
			//
			if ( _i_SCH_MODULE_Get_FM_WAFER( j ) <= 0 ) continue;
			if ( _i_SCH_MODULE_Get_FM_SIDE( j ) != side ) continue;
			//
//			_i_SCH_MODULE_Set_FM_SIDE_WAFER( j , 0 , 0 ); // 2013.01.11
			_i_SCH_MODULE_Set_FM_WAFER( j , 0 ); // 2013.01.11
			//
		}
		//
		for ( j = 0 ; j < 2 ; j++ ) {
			if ( _i_SCH_MODULE_Get_MFALS_WAFER( j ) <= 0 ) continue;
			if ( _i_SCH_MODULE_Get_MFALS_SIDE( j ) != side ) continue;
			//
			_i_SCH_MODULE_Set_MFALS_WAFER( j , 0 );
			//
		}
		//
		for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) {
			if ( _i_SCH_MODULE_Get_MFIC_WAFER( j ) <= 0 ) continue;
			if ( _i_SCH_MODULE_Get_MFIC_SIDE( j ) != side ) continue;
			//
			_i_SCH_MODULE_Set_MFIC_WAFER( j , 0 );
			//
		}
		//
		if ( x == 0 ) Sleep(100);
		//
	}
	//
}

//
void TIMER_MONITOR_Prepare_For_Move( int tside , int sside ) { // 2012.03.20
	char buffer[256];
	//
	sprintf( buffer , "$MOVEPREP%d %d" , tside + 1 , sside + 1 );
	IO_LOT_TIMER_USER_MESSAGE( buffer );
	//
}
//
//
void TIMER_MONITOR_END_For_Move( int tside , int c ) { // 2012.03.20
	char buffer[256];
	//
	sprintf( buffer , "$STOPUNLOAD%d %d" , tside + 1 , c );
	IO_LOT_TIMER_USER_MESSAGE( buffer );
	//
}
//
void TIMER_MONITOR_ReSet_For_Move( int tside , int tp , int sside , int sp , BOOL notupdate ) { // 2012.02.16
	char buffer[256];
	char bufferx[64];
	char MsgSltchar[16]; /* 2013.05.23 */
	char MsgSltcharE[16]; /* 2015.10.12 */
	//
//	FA_SIDE_TO_SLOT_GETS( tside , tp , MsgSltchar ); // 2015.10.12
	FA_SIDE_TO_SLOT_GETS( tside , tp , MsgSltchar , MsgSltcharE ); // 2015.10.12
	//
	if ( notupdate ) {
		sprintf( buffer , "$SLOT_FOLDER %d%s|%s" , _i_SCH_CLUSTER_Get_SlotIn( tside , tp ) , MsgSltchar , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( sside ) );
	}
	else {
		sprintf( buffer , "$SLOT_FOLDER %d%s|*%s" , _i_SCH_CLUSTER_Get_SlotIn( tside , tp ) , MsgSltchar , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( sside ) );
	}
	IO_LOT_DIR_USER_MESSAGE( tside , buffer );
	//
//	strcpy( bufferx , FA_SIDE_TO_SLOT_GETxS( sside , sp ) );
	//
//	FA_SIDE_TO_SLOT_GETS( sside , sp , bufferx ); // 2015.10.12
	FA_SIDE_TO_SLOT_GETS( sside , sp , bufferx , MsgSltcharE ); // 2015.10.12
	//
	sprintf( buffer , "$MOVE_FOLDER%d %d%s %d%s %s/LOTLOG/%s" , tside + 1 , sside + 1 , bufferx , _i_SCH_CLUSTER_Get_SlotIn( tside , tp ) , MsgSltchar , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( sside ) );
	IO_LOT_TIMER_USER_MESSAGE( buffer );
	//
	PROCESS_MAKE_SLOT_FOLDER( tside , tp , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( sside ) ); // 2012.02.18
}
//
int SCHEDULER_CONTROL_Move_Data_from_to_Sub( int side , int tm ) { // 2011.04.18 // 2016.07.27
	char Buffer[256];
	int t , i , j , m , tside , spawnmode , HasNotMoved , LastTry , s , p , p2 , f , fm , fs , stc;
	int ptcount[MAX_CASSETTE_SIDE] , ptlist[MAX_CASSETTE_SIDE][256];
	Scheduling_Regist	Scheduler_Reg;


	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return 1; // 2012.04.01

	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		if ( tm == -1 ) return 1;
	}
	else {
		if ( tm != -1 ) return 1;
		//
		SCHEDULER_CONTROL_Make_Clear_Method_Data( side );
		//
	}
	//
	if ( _In_Job_HandOff_In_Pre_Check[side] == 0 ) {
		//
		m = SCHEDULER_CONTROL_PathOutCheck( side ); // 2011.04.20
		//
		if ( m < 0 ) { // 2011.04.20
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] NORMAL CHECK FAIL [%d]\n" , side , tm , m );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] NORMAL CHECK FAIL [%d]\n" , side , tm , m ); // 2016.07.15
	//
			return -100 + m; // 2011.05.03
		}
		//
		return 1;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_BM_END ) continue;
		//
		break;
	}
	if ( i == MAX_CASSETTE_SLOT_SIZE ) {
		return 2;
	}
	//
	LastTry = _In_Job_HandOff_In_Pre_Check[side];
	_In_Job_HandOff_In_Pre_Check[side] = 0;
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] START [%d]\n" , side , tm , LastTry );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] START [%d]\n" , side , tm , LastTry ); // 2016.07.15
	//
	for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
		//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR TM%d LOCKING\n" , side , tm , t );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR TM%d LOCKING\n" , side , tm , t ); // 2016.07.15
		//
		if ( tm != t ) EnterCriticalSection( &CR_TMRUN[t] );
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR TM%d LOCKED\n" , side , tm , t );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR TM%d LOCKED\n" , side , tm , t ); // 2016.07.15
	}
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR MJOB LOCKING\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR MJOB LOCKING\n" , side , tm ); // 2016.07.15
	EnterCriticalSection( &CR_MULTIJOB );
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR MJOB LOCKED + FINISH LOCKING\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR MJOB LOCKED + FINISH LOCKING\n" , side , tm ); // 2016.07.15
	EnterCriticalSection( &CR_FINISH_CHECK );
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR FINISH LOCKED\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR FINISH LOCKED\n" , side , tm ); // 2016.07.15
	//
	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2011.06.10
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CR BEGINEND LOCKED\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CR BEGINEND LOCKED\n" , side , tm ); // 2016.07.15

_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] MODE-RES1 [%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm ,
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );

_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] MODE-RES1 [%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm , // 2016.07.15
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );

	HasNotMoved = FALSE;
	//
	for ( tside = 0 ; tside < MAX_CASSETTE_SIDE ; tside++ ) ptcount[tside] = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
		//
		m = _i_SCH_CLUSTER_Get_PathOut( side , i );
		//
		if ( m < CM1 ) {
			//
			m = SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , i , FALSE , &j , FALSE );
			//
			if ( m < 0 ) {
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] OUT CHECK FAIL [%d]\n" , side , tm , m );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] OUT CHECK FAIL [%d]\n" , side , tm , m ); // 2016.07.15
	//
				LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2011.06.10
				//
				LeaveCriticalSection( &CR_FINISH_CHECK );
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				//
				for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
					if ( tm != t ) LeaveCriticalSection( &CR_TMRUN[t] );
				}
				return m; // 2011.05.03
			}
			//
			m = _i_SCH_CLUSTER_Get_PathOut( side , i );
			//
			if ( m <= -100 ) m = -m % 100; // 2011.06.09
			//
		}
		//
		if ( m < CM1 ) {
			//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] HasNotMoved NotPreparedOutCass [index=%d][m=%d]\n" , side , tm , i , m );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] HasNotMoved NotPreparedOutCass [index=%d][m=%d]\n" , side , tm , i , m ); // 2016.07.15
			//
			HasNotMoved = TRUE;
			continue;
		}
		//
		tside = m - CM1;
		//
		ptlist[tside][ptcount[tside]] = i;
		ptcount[tside]++;
		//
	}
	//
	for ( tside = 0 ; tside < MAX_CASSETTE_SIDE ; tside++ ) {
		//
		if ( tside == side ) continue;
		if ( ptcount[tside] == 0 ) continue;
		//
		if ( !_i_SCH_SYSTEM_USING_STARTING( tside ) ) {
			spawnmode = TRUE;
			//
			if ( !Scheduler_Main_Wait_Finish_Complete( tside , -1 ) ) { // 2012.01.29
				HasNotMoved = TRUE;
				//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] HasNotMoved Scheduler_Main_Wait_Finish_Complete Fail [tside=%d]\n" , side , tm , tside );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] HasNotMoved Scheduler_Main_Wait_Finish_Complete Fail [tside=%d]\n" , side , tm , tside ); // 2016.07.15
				//
				continue;
			}
			//
		}
		else {
			spawnmode = FALSE;
			//
			if ( _In_Run_Finished_Check[tside] ) {
				HasNotMoved = TRUE;
				//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] HasNotMoved Finished [tside=%d]\n" , side , tm , tside );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] HasNotMoved Finished [tside=%d]\n" , side , tm , tside ); // 2016.07.15
				//
				continue;
			}
		}
		//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK [tside=%d][spawn=%d]\n" , side , tm , tside , spawnmode );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CHECK [tside=%d][spawn=%d]\n" , side , tm , tside , spawnmode ); // 2016.07.15
		//
		if ( spawnmode ) {
			//
			stc = -1; // 2011.06.09
			//
			LeaveCriticalSection( &CR_MULTIJOB );
			//
			m = _i_SCH_SYSTEM_FA_GET(side);
			//
			if ( m != 1 ) {
				_i_SCH_IO_SET_IN_PATH( tside , tside + CM1 );
				_i_SCH_IO_SET_OUT_PATH( tside , _i_SCH_IO_GET_OUT_PATH(side) );
				_i_SCH_IO_SET_START_SLOT( tside , _i_SCH_IO_GET_START_SLOT(side) );
				_i_SCH_IO_SET_END_SLOT( tside , _i_SCH_IO_GET_END_SLOT(side) );
				//
				_i_SCH_IO_GET_RECIPE_FILE( side , Buffer );	_i_SCH_IO_SET_RECIPE_FILE( tside , Buffer );
				_i_SCH_IO_SET_LOOP_COUNT( tside , 1 );
				//
				_i_SCH_SYSTEM_FA_SET( tside , _i_SCH_SYSTEM_FA_GET(side) );
				_i_SCH_SYSTEM_CPJOB_SET( tside , _i_SCH_SYSTEM_CPJOB_GET(side) );
				_i_SCH_SYSTEM_CPJOB_ID_SET( tside , _i_SCH_SYSTEM_CPJOB_ID_GET(side) );
				//
				_i_SCH_SYSTEM_MODE_SET( tside , _i_SCH_SYSTEM_MODE_GET(side) );
				_i_SCH_SYSTEM_RUNORDER_SET( tside , _i_SCH_SYSTEM_RUNORDER_GET(side) );
				//
//				IO_GET_JID_NAME( side , Buffer );	IO_SET_JID_NAME( tside , Buffer ); // 2011.09.15
//				IO_GET_MID_NAME( side , Buffer );	IO_SET_MID_NAME( tside , Buffer ); // 2011.09.15
			}
			else {
				Scheduler_Reg.RunIndex  = SYSTEM_RID_GET( side );
				Scheduler_Reg.CassIn    = tside;
				Scheduler_Reg.CassOut   = _i_SCH_IO_GET_OUT_PATH(side) - CM1;
				Scheduler_Reg.SlotStart = _i_SCH_IO_GET_START_SLOT(side);
				Scheduler_Reg.SlotEnd   = _i_SCH_IO_GET_END_SLOT(side);
				Scheduler_Reg.LoopCount = 1;
				//
				IO_GET_JID_NAME( side , Buffer );				strcpy( Scheduler_Reg.JobName , Buffer );
				_i_SCH_IO_GET_RECIPE_FILE( side , Buffer );		strcpy( Scheduler_Reg.LotName , Buffer );
				IO_GET_MID_NAME( side , Buffer );				strcpy( Scheduler_Reg.MidName , Buffer );
				//
				if ( !_i_SCH_MULTIREG_SET_REGIST_DATA( tside , &Scheduler_Reg ) ) {
					//
			//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] HasNotMoved SpawnRegistFail [tside=%d]\n" , side , tm , tside );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] HasNotMoved SpawnRegistFail [tside=%d]\n" , side , tm , tside ); // 2016.07.15
			//
					HasNotMoved = TRUE;
					EnterCriticalSection( &CR_MULTIJOB );
					continue;
				}
				else {
					_i_SCH_SYSTEM_FA_SET( tside , _i_SCH_SYSTEM_FA_GET(side) );
					_i_SCH_SYSTEM_CPJOB_SET( tside , _i_SCH_SYSTEM_CPJOB_GET(side) );
					_i_SCH_SYSTEM_CPJOB_ID_SET( tside , _i_SCH_SYSTEM_CPJOB_ID_GET(side) );
					//
					_i_SCH_SYSTEM_MODE_SET( tside , _i_SCH_SYSTEM_MODE_GET(side) );
					_i_SCH_SYSTEM_RUNORDER_SET( tside , _i_SCH_SYSTEM_RUNORDER_GET(side) );
				}
			}
			//
			if ( ptcount[tside] > 0 ) {
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , ptlist[tside][0] ) % 100 ) <= 0 ) {
					SCHEDULER_Set_CARRIER_INDEX( tside , SCHEDULER_Get_CARRIER_INDEX( side ) ); // 2011.09.17
				}
				else {
					SCHEDULER_Set_CARRIER_INDEX( tside , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , ptlist[tside][0] ) ); // 2011.09.17
				}
			}
			else {
				SCHEDULER_Set_CARRIER_INDEX( tside , SCHEDULER_Get_CARRIER_INDEX( side ) ); // 2011.08.17
			}
			//
			//--------------------------------------------------------------------------------------
//			_i_SCH_SYSTEM_BLANK_SET( tside , 1 ); // 2012.02.17
			_i_SCH_SYSTEM_BLANK_SET( tside , 100 + (side+1) ); // 2012.02.17
			//--------------------------------------------------------------------------------------
			_i_SCH_SYSTEM_USING_SET( tside , 1 );
			//--------------------------------------------------------------------------------------
			_i_SCH_SYSTEM_RESTART_SET( tside , 0 ); // 2011.09.23
			//--------------------------------------------------------------------------------------
			_i_SCH_SUB_Reset_Scheduler_Data( tside );
			//--------------------------------------------------------------------------------------
			_i_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( tside , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) );
			//
			FILE_PARAM_SM_SCHEDULER_SETTING2( 1 , tside ); // 2013.08.22
			//
			//--------------------------------------------------------------------------------------
			_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( tside , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) );
			//--------------------------------------------------------------------------------------
			// 2012.03.16
			if ( _i_SCH_IO_GET_LOOP_COUNT( tside ) <= 0 ) _i_SCH_IO_SET_LOOP_COUNT( tside , 1 );
			//
			LOG_MTL_SET_DONE_CARR_COUNT( tside , 0 );
			LOG_MTL_SET_DONE_WFR_COUNT( tside , 0 );
			//--------------------------------------------------------------------------------------
			if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) tside ) == -1 ) {
				//
				HasNotMoved = TRUE;
				//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) THREAD FAIL\n" , side , tm , tside );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) THREAD FAIL\n" , side , tm , tside ); // 2016.07.15
				//
				EnterCriticalSection( &CR_MULTIJOB );
				continue;
			}
			//
			_SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_SET( tside , side ); // 2014.06.26
			//
			while( TRUE ) {
				//-------------------------------------------
				Sleep(100);
				//-------------------------------------------
				if ( _i_SCH_SYSTEM_USING_GET( tside ) >= 10 ) break;
				//-------------------------------------------
				if ( MANAGER_ABORT() ) {
					//
					if ( m != 0 ) _i_SCH_MULTIREG_DATA_RESET( tside );
					//
					m = -1;
					break;
				}
				//-------------------------------------------
				if ( _i_SCH_SYSTEM_USING_GET( tside ) <= 0 ) {
					//
					if ( m != 0 ) _i_SCH_MULTIREG_DATA_RESET( tside );
					//
					m = -2;
					break;
				}
				//-------------------------------------------
			}
			//
			if ( m < 0 ) {
				//
				HasNotMoved = TRUE;
				//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) WAIT FAIL(%d)\n" , side , tm , tside , m );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) WAIT FAIL(%d)\n" , side , tm , tside , m ); // 2016.07.15
				//
				//--------------------------------------------------------------------------------------
//				_i_SCH_SYSTEM_BLANK_SET( tside , 2 ); // 2012.02.17
				_i_SCH_SYSTEM_BLANK_SET( tside , 200 + (side+1) ); // 2012.02.17
				//--------------------------------------------------------------------------------------
				EnterCriticalSection( &CR_MULTIJOB );
				continue;
			}
			//
			_i_SCH_SYSTEM_LASTING_SET( tside , _i_SCH_SYSTEM_LASTING_GET(side) );
			_i_SCH_SUB_Set_Last_Status( tside , _i_SCH_SUB_Get_Last_Status(side) );
			//
			//============================================================================================================
			for ( i = CM1 ; i < PM1 ; i++ ) {
				//
				if ( tside == ( i - CM1 ) ) {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , MUF_01_USE );
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
				}
				else {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , MUF_00_NOTUSE );
				}
				//
			}
			for ( ; i <= FM ; i++ ) {
				//
				if ( ( i == AL ) || ( i == IC ) ) continue;
				//
				s = _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i );
				_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , s );
				//
				switch( s ) {
				case MUF_00_NOTUSE :
				case MUF_02_USE_to_END :
				case MUF_03_USE_to_END_DISABLE :
				case MUF_04_USE_to_PREND_EF_LP :
				case MUF_05_USE_to_PRENDNA_EF_LP :
				case MUF_07_USE_to_PREND_DECTAG :
				case MUF_08_USE_to_PRENDNA_DECTAG :
				case MUF_31_USE_to_NOTUSE_SEQMODE :
				case MUF_71_USE_to_PREND_EF_XLP :
					break;

				case MUF_01_USE :
				case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
				case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
				case MUF_98_USE_to_DISABLE_RAND ://	XDEC
				case MUF_99_USE_to_DISABLE ://	XDEC
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
					break;
				}
			}
			//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) COMPLETE\n" , side , tm , tside );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] CHECK-SPAWN(%d) COMPLETE\n" , side , tm , tside ); // 2016.07.15
			//
			EnterCriticalSection( &CR_MULTIJOB );
			//
		}
		else { // 2011.06.09
			//
			stc = -1;
			//
			for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( tside , p ) < 0 ) continue;
				//
				m = _i_SCH_CLUSTER_Get_PathOut( tside , p );
				//
				if ( m <= -100 ) {
					//
					m = ( -m ) / 100;
					//
					if ( m > stc ) stc = m;
					//
				}
			}
			//
			_i_SCH_SYSTEM_SET_LOTLOGDIRECTORY( tside , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) ); // 2012.03.16
			//
			TIMER_MONITOR_Prepare_For_Move( tside , side ); // 2012.03.20
			//
		}
		//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK-CNTMOVE1(%d) [Src=%d],[Trg=%d]\n" , side , tm , tside , LOG_MTL_GET_DONE_WFR_COUNT( side ) , LOG_MTL_GET_DONE_WFR_COUNT( tside ) );
		LOG_MTL_MOVE_DATA_COUNT( tside , side ); // 2012.03.20
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK-CNTMOVE2(%d) [Src=%d],[Trg=%d]\n" , side , tm , tside , LOG_MTL_GET_DONE_WFR_COUNT( side ) , LOG_MTL_GET_DONE_WFR_COUNT( tside ) );
		//

		
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] A\n" , side , tm , tside , ptcount[tside] );


		for ( p = 0 ; p < ptcount[tside] ; p++ ) {
			//
			//---------------------------------------------------------------------------------------------------------------
			for ( p2 = 0 ; p2 < MAX_CASSETTE_SLOT_SIZE ; p2++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( tside , p2 ) < 0 ) break;
			}
			//---------------------------------------------------------------------------------------------------------------
			if ( p2 == MAX_CASSETTE_SLOT_SIZE ) {
				for ( p2 = 0 ; p2 < MAX_CASSETTE_SLOT_SIZE ; p2++ ) {
					if ( _i_SCH_CLUSTER_Get_PathRange( tside , p2 ) < 0 ) break;
					if ( _i_SCH_CLUSTER_Get_PathStatus( tside , p2 ) == SCHEDULER_CM_END ) break;
				}
				//---------------------------------------------------------------------------------------------------------------
				if ( p2 == MAX_CASSETTE_SLOT_SIZE ) {
				//---------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] DATA-MOVE : [FAIL1] Src[%d,%d]\n" , side , tm , side , ptlist[tside][p] );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] DATA-MOVE : [FAIL1] Src[%d,%d]\n" , side , tm , side , ptlist[tside][p] ); // 2016.07.15
				//---------------------------------------------------------------------------------------------------------------
					HasNotMoved = TRUE;
					break;
				}
				//---------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] B (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			f = 0;
			//
			if ( f == 0 ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
						if ( _i_SCH_MODULE_Get_PM_WAFER( PM1 + i , j ) <= 0 ) continue;
						if ( _i_SCH_MODULE_Get_PM_SIDE( PM1 + i , j ) != side ) continue;
						if ( _i_SCH_MODULE_Get_PM_POINTER( PM1 + i , j ) != ptlist[tside][p] ) continue;
						//
						f = 1;
						fm = i;
						fs = j;
						//
						break;
					}
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] C (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( f == 0 ) {
				//
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					//
					for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
						//
						if ( _i_SCH_MODULE_Get_TM_WAFER( i , j ) <= 0 ) continue;
						//
						if ( _i_SCH_MODULE_Get_TM_SIDE( i , j ) != side ) continue;
						if ( _i_SCH_MODULE_Get_TM_POINTER( i , j ) != ptlist[tside][p] ) continue;
						//
						//
						f = 2;
						fm = i;
						fs = j;
						//
						break;
					}
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] D (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( f == 0 ) {
				//
				for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
					//
					for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
						//
						if ( _i_SCH_MODULE_Get_BM_WAFER( BM1 + i , j ) <= 0 ) continue;
						//
						if ( _i_SCH_MODULE_Get_BM_SIDE( BM1 + i , j ) != side ) continue;
						if ( _i_SCH_MODULE_Get_BM_POINTER( BM1 + i , j ) != ptlist[tside][p] ) continue;
						//
						f = 3;
						fm = i;
						fs = j;
						//
						break;
					}
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] E (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( f == 0 ) {
				//
				for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( j ) <= 0 ) continue;
					if ( _i_SCH_MODULE_Get_FM_SIDE( j ) != side ) continue;
					if ( _i_SCH_MODULE_Get_FM_POINTER( j ) != ptlist[tside][p] ) continue;
					//
					f = 4;
					fm = 0;
					fs = j;
					//
					break;
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] F (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( f == 0 ) {
				//
//				for ( j = 0 ; j < 2 ; j++ ) { // 2012.03.28
				for ( j = 1 ; j <= 2 ; j++ ) { // 2012.03.28
					if ( _i_SCH_MODULE_Get_MFALS_WAFER( j ) <= 0 ) continue;
					if ( _i_SCH_MODULE_Get_MFALS_SIDE( j ) != side ) continue;
					if ( _i_SCH_MODULE_Get_MFALS_POINTER( j ) != ptlist[tside][p] ) continue;
					//
					f = 5;
					fm = 0;
					fs = j;
					//
					break;
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] G (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( f == 0 ) {
				//
//				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) { // 2012.03.28
				for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; j++ ) { // 2012.03.28
					if ( _i_SCH_MODULE_Get_MFIC_WAFER( j ) <= 0 ) continue;
					if ( _i_SCH_MODULE_Get_MFIC_SIDE( j ) != side ) continue;
					if ( _i_SCH_MODULE_Get_MFIC_POINTER( j ) != ptlist[tside][p] ) continue;
					//
					f = 6;
					fm = 0;
					fs = j;
					//
					break;
				}
			}
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] H (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			//---------------------------------------------------------------------------------------------------------------
			if ( f == 0 ) { // 2011.05.13
				//
				i = FALSE;
				//
				if ( _SCH_COMMON_MTLOUT_DATABASE_MOVE( f , tside , p2 , side , ptlist[tside][p] , &i ) ) {
					if ( i ) f = 7;
					else     f = 8;
				}
				//
			}
			else {
				//
				if ( _SCH_COMMON_MTLOUT_DATABASE_MOVE( f , tside , p2 , side , ptlist[tside][p] , &i ) ) {
				}
				//
			}
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] I (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			//---------------------------------------------------------------------------------------------------------------
			if ( f == 0 ) {
				//---------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] DATA-MOVE : [FAIL2] Src[%d,%d]\n" , side , tm , side , ptlist[tside][p] );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] DATA-MOVE : [FAIL2] Src[%d,%d]\n" , side , tm , side , ptlist[tside][p] ); // 2016.07.15
				//---------------------------------------------------------------------------------------------------------------
				HasNotMoved = TRUE;
				break;
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] J (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , ptlist[tside][p] ) % 100 ) <= 0 ) {
				SCHEDULER_Set_CARRIER_INDEX( tside , SCHEDULER_Get_CARRIER_INDEX( side ) ); // 2011.09.17
			}
			else {
				SCHEDULER_Set_CARRIER_INDEX( tside , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , ptlist[tside][p] ) ); // 2011.09.17
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] K (%d,%d) (%d)\n" , side , tm , tside , ptcount[tside] , p , p2 , ptlist[tside][p] );
			//---------------------------------------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------------------------------------
			_i_SCH_CLUSTER_Copy_Cluster_Data( tside , p2 , side , ptlist[tside][p] , 1 );
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] L (%d,%d)\n" , side , tm , tside , ptcount[tside] , p , p2 );
			_i_SCH_CLUSTER_Set_PathStatus( side , ptlist[tside][p] , SCHEDULER_CM_END );
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] M (%d,%d) (%d)\n" , side , tm , tside , ptcount[tside] , p , p2 , f );
			//---------------------------------------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------------------------------------
			if      ( f == 1 ) { // pm
				_i_SCH_MODULE_Set_PM_SIDE( PM1 + fm , fs , tside );
				_i_SCH_MODULE_Set_PM_POINTER( PM1 + fm , fs , p2 );
				//
			}
			else if ( f == 2 ) { // tm
				_i_SCH_MODULE_Set_TM_SIDE( fm , fs , tside );
				_i_SCH_MODULE_Set_TM_POINTER( fm , fs , p2 );
			}
			else if ( f == 3 ) { // bm
				_i_SCH_MODULE_Set_BM_SIDE( BM1 + fm , fs , tside );
				_i_SCH_MODULE_Set_BM_POINTER( BM1 + fm , fs , p2 );
			}
			else if ( f == 4 ) { // fm
				_i_SCH_MODULE_Set_FM_SIDE( fs , tside );
				_i_SCH_MODULE_Set_FM_POINTER( fs , p2 );
			}
			else if ( f == 5 ) { // al
				_i_SCH_MODULE_Set_MFALS_SIDE( fs , tside );
				_i_SCH_MODULE_Set_MFALS_POINTER( fs , p2 );
			}
			else if ( f == 6 ) { // ic
				_i_SCH_MODULE_Set_MFIC_SIDE( fs , tside );
				_i_SCH_MODULE_Set_MFIC_POINTER( fs , p2 );
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] G (%d,%d) (%d)\n" , side , tm , tside , ptcount[tside] , p , p2 , f );
			if ( ( f == 1 ) || ( f == 2 ) || ( f == 7 ) ) { // pm , tm , user1
				//
				_i_SCH_MODULE_Inc_TM_OutCount( tside );
				//
				_i_SCH_MODULE_Inc_TM_InCount( side );
				//
			}
			else { // 2011.07.27
				if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					//
					_i_SCH_MODULE_Inc_TM_OutCount( tside );
					//
					_i_SCH_MODULE_Inc_TM_InCount( side );
					//
				}
			}
			//
//_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] CHECK(%d)[%d] H (%d,%d) (%d)\n" , side , tm , tside , ptcount[tside] , p , p2 , f );
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				//
				_i_SCH_MODULE_Inc_FM_OutCount( tside );
				//
				_i_SCH_MODULE_Inc_TM_DoubleCount( tside );
				//
				_i_SCH_MODULE_Inc_FM_InCount( side );
				//
				_i_SCH_MODULE_Dec_TM_DoubleCount( side );
			}
			else {
				//
				_i_SCH_MODULE_Inc_TM_DoubleCount( tside );
				//
				_i_SCH_MODULE_Dec_TM_DoubleCount( side );
			}
			//
			//---------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] DATA-MOVE : [%d] Src[%d,%d]->Trg[%d,%d]\n" , side , tm , f , side , ptlist[tside][p] , tside , p2 );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] DATA-MOVE : [%d] Src[%d,%d]->Trg[%d,%d]\n" , side , tm , f , side , ptlist[tside][p] , tside , p2 ); // 2016.07.15
			//---------------------------------------------------------------------------------------------------------------
//			PROCESS_MONITOR_ReSet_For_Move( tside , p2 , side , ptlist[tside][p] ); // 2012.03.16
			//
			_SCH_CASSETTE_ReSet_For_Move( tside , p2 , side , ptlist[tside][p] );
			//
			SCHMULTI_ReSet_For_Move( spawnmode , tside , p2 , side , ptlist[tside][p] );
			//
			SCHMRDATA_ReSet_For_Move( tside , p2 , side , ptlist[tside][p] );
			//
			TIMER_MONITOR_ReSet_For_Move( tside , p2 , side , ptlist[tside][p] , spawnmode ); // 2012.02.16
			//
			PROCESS_MONITOR_ReSet_For_Move( tside , p2 , side , ptlist[tside][p] ); // 2012.03.16
			//
			if ( stc != -1 ) { // 2011.06.09
				//
				m = _i_SCH_CLUSTER_Get_PathOut( tside , p2 );
				//
				if ( m <= -100 ) {
					//
					m = ( -m ) % 100;
					//
					SCHMRDATA_Data_Setting_for_outcm( - ( ( ( stc + 1 ) * 100 ) + m ) , tside , p2 , 1 ); // 2011.09.07
					_i_SCH_CLUSTER_Set_PathOut( tside , p2 , - ( ( ( stc + 1 ) * 100 ) + m ) );
					//
				}
			}
			else { // 2011.06.14
				//
				m = _i_SCH_CLUSTER_Get_PathOut( tside , p2 );
				//
				if ( m <= -100 ) {
					//
					m = ( -m ) % 100;
					//
					SCHMRDATA_Data_Setting_for_outcm( - ( 200 + m ) , tside , p2 , 1 ); // 2011.09.07
					_i_SCH_CLUSTER_Set_PathOut( tside , p2 , - ( 200 + m ) );
					//
				}
			}
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( tside ) ) _i_SCH_SYSTEM_USING_SET( tside , 9 ); // 2011.05.30
			//---------------------------------------------------------------------------------------------------------------
			_i_SCH_SYSTEM_DBUPDATE_SET( tside , 1 ); // 2011.04.25
			_i_SCH_SYSTEM_DBUPDATE_SET( side , 2 ); // 2011.04.25
			//---------------------------------------------------------------------------------------------------------------
		}
		//============================================================================================================
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] MODE-SET\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] MODE-SET\n" , side , tm ); // 2016.07.15
		//
		if ( spawnmode ) {
			//--------------------------------------------------------------------------------------
//			_i_SCH_SYSTEM_BLANK_SET( tside , 2 ); // 2012.02.16
			_i_SCH_SYSTEM_BLANK_SET( tside , 200 + ( side + 1 ) ); // 2012.02.16
			//--------------------------------------------------------------------------------------
		}
		else {
			//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] MODE-RES2 [%d][%d,%d,%d,%d]-[%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm ,
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
				   tside , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] MODE-RES2 [%d][%d,%d,%d,%d]-[%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm , // 2016.07.15
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
				   tside , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );
			//
			for ( i = CM1 ; i < PM1 ; i++ ) {
				//
				if ( tside != ( i - CM1 ) ) continue;
				//
				s = _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i );
				t = _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , i );
				//
				switch( s ) {
				case MUF_00_NOTUSE :
				case MUF_02_USE_to_END :
				case MUF_03_USE_to_END_DISABLE :
				case MUF_04_USE_to_PREND_EF_LP :
				case MUF_05_USE_to_PRENDNA_EF_LP :
				case MUF_07_USE_to_PREND_DECTAG :
				case MUF_08_USE_to_PRENDNA_DECTAG :
				case MUF_31_USE_to_NOTUSE_SEQMODE :
				case MUF_71_USE_to_PREND_EF_XLP :
					break;

				case MUF_01_USE :
					switch( t ) {
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
						_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , MUF_01_USE );
						_i_SCH_MODULE_Set_Mdl_Use_Flag( side , i , MUF_02_USE_to_END );
						//
						break;
						//
					case MUF_01_USE :
					case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
					case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
					case MUF_98_USE_to_DISABLE_RAND ://	XDEC
					case MUF_99_USE_to_DISABLE ://	XDEC
						break;
					}
					break;

				case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
				case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
				case MUF_98_USE_to_DISABLE_RAND ://	XDEC
				case MUF_99_USE_to_DISABLE ://	XDEC
					//
					switch( t ) {
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
						_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , MUF_01_USE );
						_i_SCH_MODULE_Set_Mdl_Use_Flag( side , i , MUF_02_USE_to_END );
						//
						break;
						//
					case MUF_01_USE :
					case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
					case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
					case MUF_98_USE_to_DISABLE_RAND ://	XDEC
					case MUF_99_USE_to_DISABLE ://	XDEC
						break;
					}
					break;
				}
			}
			//
			for ( ; i <= FM ; i++ ) {
				//
				if ( ( i == AL ) || ( i == IC ) ) continue;
				//
				s = _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i );
				t = _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , i );
				//
				switch( s ) {
				case MUF_00_NOTUSE :
				case MUF_02_USE_to_END :
				case MUF_03_USE_to_END_DISABLE :
				case MUF_04_USE_to_PREND_EF_LP :
				case MUF_05_USE_to_PRENDNA_EF_LP :
				case MUF_07_USE_to_PREND_DECTAG :
				case MUF_08_USE_to_PRENDNA_DECTAG :
				case MUF_31_USE_to_NOTUSE_SEQMODE :
				case MUF_71_USE_to_PREND_EF_XLP :
					break;

				case MUF_01_USE :
					switch( t ) {
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
						_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , MUF_01_USE );
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
						//
						break;
						//
					case MUF_01_USE :
					case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
					case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
					case MUF_98_USE_to_DISABLE_RAND ://	XDEC
					case MUF_99_USE_to_DISABLE ://	XDEC
						break;
					}
					break;

				case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
				case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
				case MUF_98_USE_to_DISABLE_RAND ://	XDEC
				case MUF_99_USE_to_DISABLE ://	XDEC
					//
					switch( t ) {
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
						_i_SCH_MODULE_Set_Mdl_Use_Flag( tside , i , s );
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
						//
						break;
						//
					case MUF_01_USE :
					case MUF_81_USE_to_PREND_RANDONLY ://	XDEC
					case MUF_97_USE_to_SEQDIS_RAND ://	XDEC
					case MUF_98_USE_to_DISABLE_RAND ://	XDEC
					case MUF_99_USE_to_DISABLE ://	XDEC
						break;
					}
					break;
				}
			}
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] MODE-RES3 [%d][%d,%d,%d,%d]-[%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm ,
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
				   tside , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] MODE-RES3 [%d][%d,%d,%d,%d]-[%d][%d,%d,%d,%d]:[%d,%d,%d,%d]\n" , side , tm , // 2016.07.15
				   side , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , CM1 + 3 ) ,
				   tside , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Use_Flag( tside , CM1 + 3 ) ,
					_i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 0 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 1 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 2 ) , _i_SCH_MODULE_Get_Mdl_Run_Flag( CM1 + 3 )
				   );

		}
		//
		_SCH_CASSETTE_Set_Count_Action( tside , FALSE ); // 2016.12.21 // 2017.01.23 // 2018.10.01
		//
	}
	//
	_i_SCH_SUB_Remain_for_FM_Sub( side , FALSE ); // 2012.03.27
	//
	_SCH_CASSETTE_Set_Count_Action( side , FALSE ); // 2016.12.21 // 2018.10.01
//	_SCH_CASSETTE_Set_Count_Action( tside ); // 2016.12.21 // 2017.01.23
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] LASTING\n" , side , tm );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] LASTING\n" , side , tm ); // 2016.07.15
	//
	if ( HasNotMoved ) _In_Job_HandOff_In_Pre_Check[side] = ( LastTry + 1 );
	//
	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2011.06.10

	LeaveCriticalSection( &CR_FINISH_CHECK );
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	//
	for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
		if ( tm != t ) LeaveCriticalSection( &CR_TMRUN[t] );
	}
	//
_IO_CONSOLE_PRINTF( "SDM\t[S=%d][T=%d] COMPLETE [%d]\n" , side , tm , HasNotMoved );
_i_SCH_LOG_DEBUG_PRINTF( side , tm , "SDM\t[S=%d][T=%d] COMPLETE [%d]\n" , side , tm , HasNotMoved ); // 2016.07.15
	//
	SYSTEM_WIN_UPDATE = 21; // 2016.07.15
	//
	return 0;
}





int SCHEDULER_CONTROL_Move_Data_from_to( int side , int tm ) { // 2016.07.27
	int i , Res;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return 1;
	//
	Res = 1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) {
			Res = SCHEDULER_CONTROL_Move_Data_from_to_Sub( i , tm );
		}
		else {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				if ( _In_Job_HandOff_In_Pre_Check[i] != 0 ) {
					Res = SCHEDULER_CONTROL_Move_Data_from_to_Sub( i , tm );
				}
			}
		}
		//
	}
	//
	return Res;
}
