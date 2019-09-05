#ifndef FA_HANDLING_H
#define FA_HANDLING_H

//-----------------------------------------------------------------------------------
#define ST_INDEX_LOAD_ENTER					0
#define ST_INDEX_LOAD_LOAD					1
#define ST_INDEX_LOAD_COMPLETE				2
#define ST_INDEX_LOAD_RUNNING				3
#define ST_INDEX_LOAD_CONFIRM_COMPLETE		4
#define ST_INDEX_LOAD_CONFIRM_ABORT			5
#define ST_INDEX_LOAD_CONFIRM_START			6
#define ST_INDEX_LOAD_AGAIN					7
#define ST_INDEX_LOAD_AGAIN2				8
#define ST_INDEX_LOAD_AGAIN2_CONF_ABORT		9
#define ST_INDEX_LOAD_AGAIN2_CONF_NO		10
#define ST_INDEX_LOAD_AGAIN2_CONF_YES		11

#define ST_INDEX_UNLOAD_ENTER				12
#define ST_INDEX_UNLOAD_UNLOAD				13
#define ST_INDEX_UNLOAD_UNLOADM				14
#define ST_INDEX_UNLOAD_COMPLETE			15
#define ST_INDEX_UNLOAD_RUNNING				16
#define ST_INDEX_UNLOAD_CONFIRM_COMPLETE	17
#define ST_INDEX_UNLOAD_CONFIRM_ABORT		18
#define ST_INDEX_UNLOAD_CONFIRM_LATER		19
#define ST_INDEX_UNLOAD_CONFIRM_START		20
#define ST_INDEX_UNLOAD_AGAIN				21
//-----------------------------------------------------------------------------------
//-------------------------------------------------------------
void _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( int Side , int pointer , int Type , int mode , int module , int where , int arm , int waferid , int srcwaferid );
//-------------------------------------------------------------
void FA_HANDLER_ENTER_CR();
void FA_HANDLER_LEAVE_CR();
//-------------------------------------------------------------
void FA_HANDLER_WINDOW_POS( HWND hdlg , int type );
//-------------------------------------------------------------
void INIT_FA_HANDLER_SETTING_DATA( int , int );
void FA_HANDLER_LOAD_INIT();
void FA_HANDLER_UNLOAD_INIT();
//-------------------------------------------------------------
void FA_HANDOFF_RES_SET( int id , int res );
int  FA_HANDOFF_RES_GET( int id );
//-------------------------------------------------------------
int  FA_SERVER_MODE_GET();
BOOL FA_SERVER_FUNCTION_SET( int data , char *name );
char *FA_SERVER_FUNCTION_NAME_GET();
//-------------------------------------------------------------
void FA_FUNCTION_INTERFACE_SET( int id );
int  FA_FUNCTION_INTERFACE_GET();
//-------------------------------------------------------------
//void FA_SIDE_TO_PORT_SET( int Side , int data ); // 2006.03.09 // 2011.08.16
//int  FA_SIDE_TO_PORT_GET( int Side ); // 2006.03.09
//char *FA_SIDE_TO_PORT_GETS( int Side , int Pt ); // 2011.08.16 // 2013.05.23
//char *FA_SIDE_TO_SLOT_GETS( int Side , int Pt ); // 2012.02.18 // 2013.05.23
void FA_SIDE_TO_PORT_GETS( int Side , int Pt , char *data ); // 2011.08.16 // 2013.05.23
//void FA_SIDE_TO_SLOT_GETS( int Side , int Pt , char *data ); // 2012.02.18 // 2013.05.23 // 2015.10.12
void FA_SIDE_TO_SLOT_GETS( int Side , int Pt , char *data , char *dataE ); // 2012.02.18 // 2013.05.23 // 2015.10.12
void FA_SIDE_TO_SLOT_GET_L( int Side , int Pt , char *data ); // 2012.02.18 // 2013.05.23
//-------------------------------------------------------------
BOOL FA_AGV_MODE_GET( int id );
BOOL FA_AGV_FUNCTION_SET( int id , int data , char *name );
char *FA_AGV_FUNCTION_NAME_GET( int id );
int  FA_AGV_FUNCTION_GET( int id );
//-------------------------------------------------------------
void FA_LOAD_CONTROLLER_RUN_STATUS_SET( int Side , int data );
int  FA_LOAD_CONTROLLER_RUN_STATUS_GET( int Side );
//-------------------------------------------------------------
void FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( int Side , int data );
int  FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( int Side );
//-------------------------------------------------------------
int  FA_UNLOAD_HANDOFF_AGAIN_CHECK( int CHECKING_SIDE );
//-------------------------------------------------------------
void FA_SEND_MESSAGE_TO_SERVER( int mode , int side , int pt , char *data ); // 2005.10.26
//
void FA_AGV_AUTOHANDLER_SEND_MESSAGE( int id );
//
void FA_MAPPING_SIGNAL_SET( int Side , int data );
int  FA_MAPPING_SIGNAL_GET( int Side );
void FA_MAPPING_CANCEL_SET( int Side , int data );
int  FA_MAPPING_CANCEL_GET( int Side );
void FA_LOADING_SIGNAL_SET( int Side , BOOL data );
BOOL FA_LOADING_SIGNAL_GET( int Side );
void FA_LOADREQ_SIGNAL_SET( int Side , BOOL data );
BOOL FA_LOADREQ_SIGNAL_GET( int Side );
void FA_UNLOADING_SIGNAL_SET( int Side , BOOL data );
BOOL FA_UNLOADING_SIGNAL_GET( int Side );
//-------------------------------------------------------------
void FA_ACCEPT_MESSAGE( int Side , int Type , int Data );
void FA_RESULT_MESSAGE( int Side , int Type , int Data );
void FA_REJECT_MESSAGE( int Side , int Type , int Data , char *strdata );
void FA_INFO_MESSAGE( int Type , int Data );
//-------------------------------------------------------------
void FA_LOAD_CONTROLLER_TH( int CHECKING_SIDE );
void FA_UNLOAD_CONTROLLER( int CHECKING_SIDE , BOOL Loop , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck );
//-------------------------------------------------------------
void FA_STARTEND_STATUS_CONTROLLER( BOOL Start , BOOL Timeout );
//-------------------------------------------------------------
void FA_TIME_START_MESSAGE( int Side );
void FA_TIME_END_MESSAGE( int Side , int id );
void FA_TIME_END2_MESSAGE( int Side , int pt , int id );
//void FA_TIME_FIRSTRUN_MESSAGE( int Side ); // 2012.09.25
void FA_TIME_FIRSTRUN_MESSAGE( int Side , int pt ); // 2012.09.25
//void FA_TIME_LASTRUN_MESSAGE( int Side ); // 2012.09.25
void FA_TIME_LASTRUN_MESSAGE( int Side , int pt ); // 2012.09.25
//void FA_TIME_PM_START_MESSAGE( int Side , int pointer , int module , int wfrid , char *recipe ); // 2014.02.04
void FA_TIME_PM_START_MESSAGE( int Side , int pointer , int module , int wfrid , int PathDo , char *recipe ); // 2014.02.04
void FA_TIME_PM_END_MESSAGE( int Side , int pointer , int module , int wfrid , int Result , int Next , char *recipe );
void FA_TIME_WID_MESSAGE( int Side , int pointer , int wfrid , char *data ); // 2003.01.15
//-------------------------------------------------------------
void FA_TIME_DUMMY_PM_START_MESSAGE( int Side , int pointer , int module , int srcoffset , int wfrid , char *recipe );
void FA_TIME_DUMMY_PM_END_MESSAGE( int Side , int pointer , int module , int srcoffset , int wfrid , int Result , int Next , char *recipe );
//-------------------------------------------------------------
//void MANUAL_UNLOAD_CONTROLLER( int cm1 , int cm2 , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck ); // 2012.07.10
void MANUAL_UNLOAD_CONTROLLER( int cm , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck ); // 2012.07.10
//-------------------------------------------------------------
void FA_MDL_STATUS_IO_SET( int ch );
void FA_AGV_STATUS_IO_SET( int ch );
void FA_RUN_STATUS_IO_SET( int *OldStatus );
//-------------------------------------------------------------
BOOL FA_MAIN_BUTTON_SET_CHECK( int side );
//

#endif
