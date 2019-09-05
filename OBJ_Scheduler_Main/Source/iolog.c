#include "default.h"

#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "sch_main.h"
#include "sch_restart.h"
#include "Robot_Handling.h"
#include "IO_Part_data.h"
#include "FA_Handling.h"

#include "INF_sch_CommonUser.h"

int PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB();
BOOL SCHMULTI_MESSAGE_GET_ALL( int side , int pointer , char *data , int size );

//extern int SCH_MACRO_PROCESS_MOVE[MAX_CHAMBER];
//extern int SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[MAX_CHAMBER];

extern int SYSTEM_WIN_UPDATE;
//------------------------------------------------------------------------------------------
char SCHEDULER_DEBUG_DATA_FILE[64]; // 2006.02.24
char SCHEDULER_DEBUG_FLOW_FILE[64]; // 2006.02.24
//-----------------------------------------------------------------------------------

void _i_SCH_LOG_DEBUG_PRINTF( int M , int tm , LPSTR list , ... );

int  IO_DEBUG_MONITOR_TRGCOUNT  = 0; // 2008.04.18
int  IO_DEBUG_MONITOR_CURCOUNT  = 0; // 2008.04.18
int  IO_DEBUG_MONITOR_STATUS = 0;
int  IO_DEBUG_MONITOR_WAITING = 0; // 2009.03.02
int  IO_DEBUG_MONITOR_DATAVIEW = 0; // 2008.09.08
int  IO_DEBUG_MONITOR_GROUP = -1; // 2007.04.27
int  IO_DEBUG_MONITOR_PRINTF = 0; // 2007.04.27

char *IO_DEBUG_FILENAME = NULL; // 2005.03.15 // 2008.04.02

CRITICAL_SECTION CR_DEBUG;
CRITICAL_SECTION CR_LOTLOG;

//int MSG_LOG_POINTER = -1; // 2005.03.14
int TIM_LOG_POINTER = -1;
int LOT_LOG_POINTER[MAX_CASSETTE_SIDE];
int LOT_DIR_POINTER[MAX_CASSETTE_SIDE];
//
char LOT_LOG_FILE[MAX_CASSETTE_SIDE][256]; // 2008.10.15

//
#define MAX_dWRITE_FUNCTION_EVENT_TH_TRY	32 // 2013.10.30
//
int EVENT_SEND_TIM_LOG_POINTER_ASNCH; // 2013.10.30
int EVENT_SEND_LOT_LOG_POINTER_ASNCH[MAX_CASSETTE_SIDE]; // 2013.10.30


int EVENT_LOG_OLD[64]; // 2015.04.09
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//
void f_enc_printf( FILE *fpt , LPSTR list , ... ) {
	va_list va;
//	char TextBuffer[4096+1]; // 2013.01.09
	char TextBuffer[2555+2555+2555+64+256+1]; // 2013.01.09

	va_start( va , list );
//	_vsnprintf( TextBuffer , 4096 , list , (LPSTR) va ); // 2013.01.09
	_vsnprintf( TextBuffer , 2555+2555+2555+64+256 , list , (LPSTR) va ); // 2013.01.09
	va_end( va );
	//
	Make_Simple_Encryption( _i_SCH_PRM_GET_LOG_ENCRIPTION() , TextBuffer );
	//
	fprintf( fpt , "%s" , TextBuffer );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void IO_LOG_INIT() {
	int i;
//	MSG_LOG_POINTER    = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_MSGLOG" ); // 2005.03.14
	TIM_LOG_POINTER    = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_TIMER" );
	//
	EVENT_SEND_TIM_LOG_POINTER_ASNCH; // 2013.10.30
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		LOT_LOG_POINTER[i] = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_LOTLOG%d" , i + 1 );
		if ( i == 0 )	LOT_DIR_POINTER[i] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.LOG_DIRECTORY" );
		else			LOT_DIR_POINTER[i] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.LOG_DIRECTORY%d" , i + 1 );
		//
		EVENT_SEND_LOT_LOG_POINTER_ASNCH[i] = 0; // 2013.10.30
		//
	}
	//
	for ( i = 0 ; i < 64 ; i++ ) EVENT_LOG_OLD[i] = 0; // 2015.04.09
	//
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void IO_LOT_DIR_USER_MESSAGE( int M , char *data ) { // 2012.02.17
	if ( LOT_LOG_POINTER[M] < 0 ) return;
	_dWRITE_FUNCTION_EVENT_TH( LOT_LOG_POINTER[M] , data );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void IO_LOT_DIR_INITIAL( int M , char *directory , char *folder ) {
	int CommStatus;

	//-------------------------------------------------------------------------------------------
	if ( strlen( folder ) <= 0 ) {
		if ( strlen( directory ) <= 0 ) {
			_snprintf( LOT_LOG_FILE[M] , 255 , "lotlog\\lotdata.log" );
		}
		else {
			_snprintf( LOT_LOG_FILE[M] , 255 , "lotlog\\%s\\lotdata.log" , directory );
		}
	}
	else {
		if ( strlen( directory ) <= 0 ) {
			_snprintf( LOT_LOG_FILE[M] , 255 , "%s\\lotlog\\lotdata.log" , folder );
		}
		else {
			_snprintf( LOT_LOG_FILE[M] , 255 , "%s\\lotlog\\%s\\lotdata.log" , folder , directory );
		}
	}
	//-------------------------------------------------------------------------------------------
	if ( LOT_LOG_POINTER[M] >= 0 ) {
		if ( LOT_DIR_POINTER[M] >= 0 ) _dWRITE_STRING( LOT_DIR_POINTER[M] , directory , &CommStatus );
		//==================================================================================
		// 2006.02.16
		//==================================================================================
		_dWRITE_FUNCTIONF_EVENT( LOT_LOG_POINTER[M] , "$LOG_FOLDER %s" , folder );
		//
		while( TRUE ) {
			CommStatus = _dREAD_FUNCTION_EVENT( LOT_LOG_POINTER[M] );
			if ( CommStatus != SYS_RUNNING ) break;
//			Sleep(10); // 2008.05.09
			Sleep(1); // 2008.05.09
		}
		//==================================================================================
		// 2008.05.09
		//==================================================================================
		_dWRITE_FUNCTIONF_EVENT( LOT_LOG_POINTER[M] , "$APPEND_DATA %d" , M + 1 );
		//
		while( TRUE ) {
			CommStatus = _dREAD_FUNCTION_EVENT( LOT_LOG_POINTER[M] );
			if ( CommStatus != SYS_RUNNING ) break;
//			Sleep(10); // 2008.05.09
			Sleep(1); // 2008.05.09
		}
		//==================================================================================
		_dRUN_FUNCTION_FREE( LOT_LOG_POINTER[M] , directory );
		//==================================================================================
		//==================================================================================
		// 2010.04.14
		//==================================================================================
		_dWRITE_FUNCTIONF_EVENT( LOT_LOG_POINTER[M] , "$LOG_ENCRIPTION %d" , _i_SCH_PRM_GET_LOG_ENCRIPTION() );
		//
		while( TRUE ) {
			CommStatus = _dREAD_FUNCTION_EVENT( LOT_LOG_POINTER[M] );
			if ( CommStatus != SYS_RUNNING ) break;
			Sleep(1);
		}
		//==================================================================================
		_dRUN_FUNCTION_FREE( LOT_LOG_POINTER[M] , directory );
		//==================================================================================
	}
}
//------------------------------------------------------------------------------------------
BOOL over_char_and_Conv29( char *data , char *retbuf , int cnt , BOOL tag29check ) {
	int i = 0;
	int needconv = FALSE;
	//======================================================
	while( TRUE ) {
		if ( data[i] ==   0 ) break;
		if ( tag29check ) { // 2008.02.28
			if ( data[i] == '|' ) {
				needconv = TRUE; // 2007.11.28
				break;
			}
		}
		i++;
		if ( i > cnt ) {
			needconv = TRUE;
			break;
		}
	}
	//======================================================
	if ( !needconv ) return FALSE;
	//======================================================
	i = 0;
	while( TRUE ) {
		if ( tag29check ) { // 2008.02.28
			if ( data[i] == '|' ) {
				retbuf[i] = 29;
			}
			else {
				retbuf[i] = data[i];
			}
		}
		else {
			retbuf[i] = data[i];
		}
		if ( data[i] == 0 ) break;
		i++;
		if ( i > ( cnt - 4 ) ) {
			retbuf[i] = '.'; i++;
			retbuf[i] = '.'; i++;
			retbuf[i] = '.'; i++;
			retbuf[i] = 0;
			break;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
void IO_LOT_TIMER_USER_MESSAGE( char *data ) { // 2012.02.17
	if ( TIM_LOG_POINTER < 0 ) return;
	_dWRITE_FUNCTION_EVENT_TH( TIM_LOG_POINTER , data );
}
//
void SCH_LOG_TIMER_dWRITE_FUNCTION_EVENT_TH( char *data ) { // 2013.10.30
	///
	int addr , sts , x;
	//
	if ( EVENT_SEND_TIM_LOG_POINTER_ASNCH == 0 ) {
		//
		addr = TIM_LOG_POINTER;
		//
		_dDIGITAL_SHAPE( -1010 , &sts , &x , &x , &addr , data );
		//
		if ( sts != 1 ) {
			//
			_dWRITE_FUNCTION_EVENT_TH( TIM_LOG_POINTER , data );
			//
			EVENT_SEND_TIM_LOG_POINTER_ASNCH++;
			//
			/*
			{
			FILE *fpt;
			fpt = fopen( "log.txt" , "a" );
			fprintf( fpt , "[1] %s\n" , data );
			fclose(fpt);
			}
			*/
		}
		//
	}
	else {
		//
		_dWRITE_FUNCTION_EVENT_TH( TIM_LOG_POINTER , data );
		//
		EVENT_SEND_TIM_LOG_POINTER_ASNCH++;
		//
		/*
		{
		FILE *fpt;
		fpt = fopen( "log.txt" , "a" );
		fprintf( fpt , "[0] %s\n" , data );
		fclose(fpt);
		}
		*/
		//
		if ( EVENT_SEND_TIM_LOG_POINTER_ASNCH > MAX_dWRITE_FUNCTION_EVENT_TH_TRY ) EVENT_SEND_TIM_LOG_POINTER_ASNCH = 0;
		//
	}
}

void _i_SCH_LOG_TIMER_PRINTF( int side , int mode , int i1 , int i2 , int i3 , int i4 , int i5 , char *s1 , char *s2 ) {
//	char TextBuffer[1025]; // 2007.01.04
//	char buffer[128+1]; // 2007.11.28
//	char buffer[2048+1]; // 2007.11.28 // 2008.02.13
	char buffer[2555+1]; // 2007.11.28 // 2008.02.13
	char Send_Buffer[2555+64+1]; // 2008.02.28
	int sts; // 2010.11.26
	char MsgSltchar[16]; /* 2013.05.23 */
	char MsgSltcharE[16]; /* 2015.10.12 */
	char pmname[32]; /* 2016.04.26 */

	if ( side < 0 ) { // 2010.11.26
		while( TRUE ) {
			sts = _dREAD_FUNCTION_EVENT( TIM_LOG_POINTER );
			if ( sts != SYS_RUNNING ) return;
			Sleep(1);
		}
	}
	//
	if ( mode < 1000 ) { // 2011.06.15
		//
		if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
			switch( mode ) {
			case TIMER_START :
//				if ( i2 != 1 ) FA_TIME_START_MESSAGE( side ); // 2011.09.14 // 2012.02.16
				if ( i2 == 0 ) FA_TIME_START_MESSAGE( side ); // 2011.09.14 // 2012.02.16
				break;
			case TIMER_RESTART :
				break;
			case TIMER_STOP :
				if ( i2 != 0 ) { // 2010.04.27
					FA_TIME_END_MESSAGE( side , i1 );
				}
				break;
			case TIMER_STOP2 :
				break;
			case TIMER_FIRST :
//				FA_TIME_FIRSTRUN_MESSAGE( side ); // 2012.09.25
				FA_TIME_FIRSTRUN_MESSAGE( side , i1 ); // 2012.09.25
				break;
			case TIMER_LAST :
//				FA_TIME_LASTRUN_MESSAGE( side ); // 2012.09.25
				FA_TIME_LASTRUN_MESSAGE( side , i1 );
				break;
			case TIMER_CM_START :
				break;
			case TIMER_CM_END :
				break;
			case TIMER_PM_START :
	//			if ( over_char_and_Conv29( s1 , buffer , 2048 ) ) { // 2008.02.13
				if ( over_char_and_Conv29( s1 , buffer , 2555 , TRUE ) ) { // 2008.02.13
					if ( i4 < 0 ) {
//						FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , buffer ); // 2014.02.04
						FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , ( i5 < 0 ) ? i5 : i5 % 1000 , buffer ); // 2014.02.04
					}
					else {
						if      ( ( i4 % 100 ) == 4 ) { // Dummy // 2007.11.07
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 % 100 , buffer ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 % 100 , ( i5 < 0 ) ? i5 : i5 % 1000 , buffer ); // 2014.02.04
						}
						else if ( ( i4 % 100 ) == 5 ) { // DummyOnly // 2007.11.29
							//
							if ( _i_SCH_PRM_GET_DUMMY_MESSAGE() > 0 ) { // 2010.11.30
//								FA_TIME_DUMMY_PM_START_MESSAGE( side , i3 , i2 , i5 , i1 / 100 , buffer ); // 2014.02.04
								FA_TIME_DUMMY_PM_START_MESSAGE( side , i3 , i2 , ( i5 < 0 ) ? i5 : i5 / 1000 , i1 / 100 , buffer ); // 2014.02.04
							}
							//
						}
						else if ( ( i4 % 100 ) == 99 ) { // lowcut // 2007.11.29
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 / 100 , buffer ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 / 100 , ( i5 < 0 ) ? i5 : i5 % 1000 , buffer ); // 2014.02.04
						}
						else {
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , buffer ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , ( i5 < 0 ) ? i5 : i5 % 1000 , buffer ); // 2014.02.04
						}
					}
				}
				else {
					if ( i4 < 0 ) {
//						FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , s1 ); // 2014.02.04
						FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , ( i5 < 0 ) ? i5 : i5 % 1000 , s1 ); // 2014.02.04
					}
					else {
						if      ( ( i4 % 100 ) == 4 ) { // Dummy // 2007.11.07
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 % 100 , s1 ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 % 100 , ( i5 < 0 ) ? i5 : i5 % 1000 , s1 ); // 2014.02.04
						}
						else if ( ( i4 % 100 ) == 5 ) { // DummyOnly // 2007.11.29
							//
							if ( _i_SCH_PRM_GET_DUMMY_MESSAGE() > 0 ) { // 2010.11.30
//								FA_TIME_DUMMY_PM_START_MESSAGE( side , i3 , i2 , i5 , i1 / 100 , s1 ); // 2014.02.04
								FA_TIME_DUMMY_PM_START_MESSAGE( side , i3 , i2 , ( i5 < 0 ) ? i5 : i5 / 1000 , i1 / 100 , s1 ); // 2014.02.04
							}
							//
						}
						else if ( ( i4 % 100 ) == 99 ) { // lowcut // 2007.11.29
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 / 100 , s1 ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 / 100 , ( i5 < 0 ) ? i5 : i5 % 1000 , s1 ); // 2014.02.04
						}
						else {
//							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , s1 ); // 2014.02.04
							FA_TIME_PM_START_MESSAGE( side , i3 , i2 , i1 , ( i5 < 0 ) ? i5 : i5 % 1000 , s1 ); // 2014.02.04
						}
					}
				}
				break;
			case TIMER_PM_END :
				if ( over_char_and_Conv29( s1 , buffer , 2555 , TRUE ) ) { // 2008.02.13
	//				if      ( ( i4 / 1000 ) == 2 ) { // dummyonly // 2007.11.29 // 2010.11.30
					if      ( ( ( i4 / 1000 ) % 10 ) == 2 ) { // dummyonly // 2007.11.29 // 2010.11.30
						//
						if ( _i_SCH_PRM_GET_DUMMY_MESSAGE() > 0 ) { // 2010.11.30
							FA_TIME_DUMMY_PM_END_MESSAGE( side , i3 , i1 , ( ( i4 / 1000 ) / 10 ) , i2 , i4 % 1000 , i5 , buffer );
						}
						//
					}
	//				else if ( ( i4 / 1000 ) == 1 ) { // dummy + wfr // 2007.11.29 // 2010.11.30
					else if ( ( ( i4 / 1000 ) % 10 ) == 1 ) { // dummy + wfr // 2007.11.29 // 2010.11.30
						FA_TIME_PM_END_MESSAGE( side , i3 , i1 , i2 % 100 , i4 % 1000 , i5 , buffer );
					}
					else {
						FA_TIME_PM_END_MESSAGE( side , i3 , i1 , i2 , i4 , i5 , buffer );
					}
				}
				else {
	//				if      ( ( i4 / 1000 ) == 2 ) { // dummyonly // 2007.11.29 // 2010.11.30
					if      ( ( ( i4 / 1000 ) % 10 ) == 2 ) { // dummyonly // 2007.11.29 // 2010.11.30
						//
						if ( _i_SCH_PRM_GET_DUMMY_MESSAGE() > 0 ) { // 2010.11.30
							FA_TIME_DUMMY_PM_END_MESSAGE( side , i3 , i1 , ( ( i4 / 1000 ) / 10 ) , i2 , i4 % 1000 , i5 , s1 );
						}
						//
					}
	//				else if ( ( i4 / 1000 ) == 1 ) { // dummy + wfr // 2007.11.29 // 2010.11.30
					else if ( ( ( i4 / 1000 ) % 10 ) == 1 ) { // dummy + wfr // 2007.11.29 // 2010.11.30
						FA_TIME_PM_END_MESSAGE( side , i3 , i1 , i2 % 100 , i4 % 1000 , i5 , s1 );
					}
					else {
						FA_TIME_PM_END_MESSAGE( side , i3 , i1 , i2 , i4 , i5 , s1 );
					}
				}
				break;
			case TIMER_MID_SET :
				break;
			case TIMER_WID_SET :
				FA_TIME_WID_MESSAGE( side , i2 , i1 , s1 ); // 2003.01.15
				break;
			case TIMER_LOG_DEL :
				break;
			}
		}
	}
//	else { // 2011.07.15 // 2011.07.16
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) return; // 2011.07.16
//	} // 2011.07.16
	//
	if ( TIM_LOG_POINTER < 0 ) return;
	//-------------------------------------------------
	// Append 2001.09.18
	if ( LOT_LOG_POINTER[side] < 0 ) return;
	//-------------------------------------------------
	//
	if (
		( mode >= 1000 ) ||
		( ( ( mode == TIMER_CM_START ) || ( mode == TIMER_CM_END ) ) && ( i2 >= PM1 ) ) // 2011.11.23
	) { // 2011.11.18
		//
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2011.11.18
			if ( !_SCH_COMMON_USE_EXTEND_OPTION( 1 , 0 ) ) return;
		}
		//
	}
	//
	switch( mode ) {
	case 1001 : // user start // 2011.06.15
		//
//		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
		if ( i2 > 1 ) {
			if ( i3 < 0 ) {
//				sprintf( Send_Buffer , "USER_START%d %d%s %s %s%d::%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i2 , i4 ); // 2015.10.12
				sprintf( Send_Buffer , "USER_START%d %s%d%s %s %s%d::%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , s2 , s1 , i2 , i4 ); // 2015.10.12
			}
			else {
//				sprintf( Send_Buffer , "USER_START%d %d%s %s %s%d:%c:%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i2 , i3 + 'A' , i4 ); // 2015.10.12
				sprintf( Send_Buffer , "USER_START%d %s%d%s %s %s%d:%c:%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , s2 , s1 , i2 , i3 + 'A' , i4 ); // 2015.10.12
			}
		}
		else {
			if ( i3 < 0 ) {
//				sprintf( Send_Buffer , "USER_START%d %d%s %s %s::%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i4 ); // 2015.10.12
				sprintf( Send_Buffer , "USER_START%d %s%d%s %s %s::%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , s2 , s1 , i4 ); // 2015.10.12
			}
			else {
//				sprintf( Send_Buffer , "USER_START%d %d%s %s %s:%c:%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i3 + 'A' , i4 ); // 2015.10.12
				sprintf( Send_Buffer , "USER_START%d %s%d%s %s %s:%c:%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , s2 , s1 , i3 + 'A' , i4 ); // 2015.10.12
			}
		}
		break;
	case 1002 : // user end // 2011.06.15
		//
//		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
//		sprintf( Send_Buffer , "USER_END%d %d%s" , side + 1 , i1 , MsgSltchar ); // 2015.10.12
		sprintf( Send_Buffer , "USER_END%d %s%d%s" , side + 1 , MsgSltcharE , i1 , MsgSltchar ); // 2015.10.12
		break;

	case 1003 :
		sprintf( Send_Buffer , "CM_START%d %d %s" , side + 1 , i1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) );
		break;

	case 1004 :
		sprintf( Send_Buffer , "CM_END%d %d *->%s" , side + 1 , i1 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) );
		break;

	case 1011 : // user start D // 2011.09.29
		//
//		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
		if ( i2 > 1 ) {
			if ( i3 < 0 ) {
				sprintf( Send_Buffer , "USER_START%d D%d%s %s %s%d::%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i2 , i4 );
			}
			else {
				sprintf( Send_Buffer , "USER_START%d D%d%s %s %s%d:%c:%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i2 , i3 + 'A' , i4 );
			}
		}
		else {
			if ( i3 < 0 ) {
				sprintf( Send_Buffer , "USER_START%d D%d%s %s %s::%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i4 );
			}
			else {
				sprintf( Send_Buffer , "USER_START%d D%d%s %s %s:%c:%d" , side + 1 , i1 , MsgSltchar , s2 , s1 , i3 + 'A' , i4 );
			}
		}
		break;
	case 1012 : // user end D // 2011.09.29
		//
//		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
		sprintf( Send_Buffer , "USER_END%d D%d%s" , side + 1 , i1 , MsgSltchar );
		break;

	case TIMER_START :
		//
		sprintf( Send_Buffer , "LOG_ENCRIPTION %d" , _i_SCH_PRM_GET_LOG_ENCRIPTION() ); // 2010.04.15
		//
//		_dWRITE_FUNCTION_EVENT_TH( TIM_LOG_POINTER , Send_Buffer ); // 2010.04.15 // 2013.10.30
		//
		SCH_LOG_TIMER_dWRITE_FUNCTION_EVENT_TH( Send_Buffer ); // 2013.10.30
		//

		if ( i1 == 0 ) { // 2005.09.22
			if ( i2 >= 100 ) {
				sts = ( i2 % 100 ) - 1;
				if ( ( sts >= 0 ) && ( sts < MAX_CASSETTE_SIDE ) ) { // 2012.02.16
//					sprintf( Send_Buffer , "STARD%d %s/LOTLOG/%s %d" , side + 1 , s1 , s2 , sts + 1 ); // 2014.02.13
					sprintf( Send_Buffer , "START%d %s/LOTLOG/%s %d" , side + 1 , s1 , s2 , sts + 1 ); // 2014.02.13
				}
				else {
					sprintf( Send_Buffer , "START%d %s/LOTLOG/%s" , side + 1 , s1 , s2 );
				}
			}
			else {
				sprintf( Send_Buffer , "START%d %s/LOTLOG/%s" , side + 1 , s1 , s2 );
			}
		}
		else { // 2005.09.22
			if ( i2 >= 100 ) {
				sts = ( i2 % 100 ) - 1;
				if ( ( sts >= 0 ) && ( sts < MAX_CASSETTE_SIDE ) ) {
					sprintf( Send_Buffer , "STARD%d %s/LOTLOG/%s %d" , side + 1 , s1 , s2 , sts + 1 );
				}
				else {
					sprintf( Send_Buffer , "STARD%d %s/LOTLOG/%s" , side + 1 , s1 , s2 );
				}
			}
			else {
				sprintf( Send_Buffer , "STARD%d %s/LOTLOG/%s" , side + 1 , s1 , s2 );
			}
		}
		break;
	case TIMER_RESTART :
		if ( i1 == 1 ) {
			sprintf( Send_Buffer , "RESTART%d %d %s/LOTLOG/%s" , side + 1 , i2 , s1 , s2 );
		}
		else {
			sprintf( Send_Buffer , "RESTART%d" , side + 1 );
		}
		break;
	case TIMER_STOP :
	case TIMER_STOP2 :
		sprintf( Send_Buffer , "STOP%d %d" , side + 1 , i1 );
		break;
	case TIMER_FIRST :
		sprintf( Send_Buffer , "FIRST%d" , side + 1 );
		break;
	case TIMER_LAST :
		sprintf( Send_Buffer , "LAST%d" , side + 1 );
		break;
	case TIMER_CM_START :
		//
//		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i5 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
		if ( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() / 2 ) == 1 ) { // 2019.04.05
			//
			SCHMULTI_MESSAGE_GET_ALL( side , i5 , buffer , 256 );
			//
			if ( buffer[0] == '|' ) {
				if ( i2 < PM1 ) {
					sprintf( Send_Buffer , "CM_START%d %s%d%s %s->%s:%d %c%s%c" , side + 1 , MsgSltcharE , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i3) , i4 , '"' , buffer + 1 , '"' ); // 2010.03.06 // 2015.10.12
				}
				else { // 2011.09.30
					sprintf( Send_Buffer , "CM_START%d D%d%s %s:%d %c%s%c" , side + 1 , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , i4 , '"' , buffer + 1 , '"' ); // 2013.03.11
				}
			}
			else {
				if ( i2 < PM1 ) {
					sprintf( Send_Buffer , "CM_START%d %s%d%s %s->%s:%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i3) , i4 ); // 2010.03.06 // 2015.10.12
				}
				else { // 2011.09.30
					sprintf( Send_Buffer , "CM_START%d D%d%s %s:%d" , side + 1 , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , i4 ); // 2013.03.11
				}
			}
		}
		else {
			if ( i2 < PM1 ) {
	//			sprintf( Send_Buffer , "CM_START%d %d%s %s->%s:%d" , side + 1 , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i3) , i4 ); // 2010.03.06 // 2015.10.12
				sprintf( Send_Buffer , "CM_START%d %s%d%s %s->%s:%d" , side + 1 , MsgSltcharE , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i3) , i4 ); // 2010.03.06 // 2015.10.12
			}
			else { // 2011.09.30
	//			sprintf( Send_Buffer , "CM_START%d D%d%s %s" , side + 1 , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) ); // 2013.03.11
				sprintf( Send_Buffer , "CM_START%d D%d%s %s:%d" , side + 1 , i1 , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i2) , i4 ); // 2013.03.11
			}
		}
		//
		break;
	case TIMER_CM_END :
		//
//		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
		if ( i2 < PM1 ) {
//			sprintf( Send_Buffer , "CM_END%d %d%s" , side + 1 , i1 , MsgSltchar ); // 2015.10.12
			sprintf( Send_Buffer , "CM_END%d %s%d%s" , side + 1 , MsgSltcharE , i1 , MsgSltchar ); // 2015.10.12
		}
		else { // 2011.09.30
			sprintf( Send_Buffer , "CM_END%d D%d%s" , side + 1 , i1 , MsgSltchar );
		}
		//
		break;
	case TIMER_PM_START :
		//
		SYSTEM_WIN_UPDATE = 1; // 2013.03.11
		//
//		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar ); // 2015.10.12
		FA_SIDE_TO_SLOT_GETS( side , i3 , MsgSltchar , MsgSltcharE ); // 2015.10.12
		//
/*
//
// 2016.04.26
//
//		if ( over_char_and_Conv29( s1 , buffer , 2048 ) ) { // 2008.02.13
//		if ( over_char_and_Conv29( s1 , buffer , 128 ) ) { // 2008.02.13 // 2008.02.28
		if ( over_char_and_Conv29( s1 , buffer , 2555 , FALSE ) ) { // 2008.02.13 // 2008.02.28
			if      ( i4 < 0 ) {
//				sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
				sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
			}
			else {
				if ( ( i4 / 100 ) > 0 ) { // 2013.12.05
					if      ( ( i4 % 100 ) <= 0 ) {
//						sprintf( Send_Buffer , "PM_START%d %d%s %d:%d %s" , side + 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d:%d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) {
//						sprintf( Send_Buffer , "PM_START%d %d%s %d:%d %s" , side + 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d:%d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d:%d %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer );
					}
					else {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d:%d %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , buffer );
					}
				}
				else {
					if      ( ( i4 % 100 ) <= 0 ) { // 2004.02.28
//						sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) { // 2007.11.29
//						sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) { // Dummy+ 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , i2 , buffer );
					}
					else { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , i2 , buffer );
					}
				}
			}
		}
		else {
			if      ( i4 < 0 ) {
//				sprintf( Send_Buffer , "PM_START%d %c%d%s %d %s" , side + 1 , i4 + 'A' - 1 , i1 , MsgSltchar , i2 , s1 ); // 2014.02.04
//				sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , s1 ); // 2014.02.04 // 2015.10.12
				sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , s1 ); // 2014.02.04 // 2015.10.12
			}
			else {
				if ( ( i4 / 100 ) > 0 ) { // 2013.12.05
					if      ( ( i4 % 100 ) <= 0 ) {
//						sprintf( Send_Buffer , "PM_START%d %d%s %d:%d %s" , side + 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d:%d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) {
//						sprintf( Send_Buffer , "PM_START%d %d%s %d:%d %s" , side + 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d:%d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d:%d %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 );
					}
					else {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d:%d %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , i2 , ( i4 / 100 ) , s1 );
					}
				}
				else {
					if      ( ( i4 % 100 ) <= 0 ) { // 2004.02.28
//						sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , s1 ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) { // 2007.11.29
//						sprintf( Send_Buffer , "PM_START%d %d%s %d %s" , side + 1 , i1 , MsgSltchar , i2 , s1 ); // 2015.10.12
						sprintf( Send_Buffer , "PM_START%d %s%d%s %d %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , i2 , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) { // DummyOnly 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , i2 , s1 );
					}
					else { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %c%d%s %d %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , i2 , s1 );
					}
				}
			}
		}
*/
		//
		// 2016.04.26
		//
		if ( s2[0] == 0 ) {
			if      ( i4 < 0 ) {
				sprintf( pmname , "%d" , i2 );
			}
			else {
				if ( ( i4 / 100 ) > 0 ) {
					sprintf( pmname , "%d:%d" , i2 , ( i4 / 100 ) );
				}
				else {
					sprintf( pmname , "%d" , i2 );
				}
			}
		}
		else {
			if      ( i4 < 0 ) {
				sprintf( pmname , "%d::%s" , i2 , s2 );
			}
			else {
				if ( ( i4 / 100 ) > 0 ) {
					sprintf( pmname , "%d:%d:%s" , i2 , ( i4 / 100 ) , s2 );
				}
				else {
					sprintf( pmname , "%d::%s" , i2 , s2 );
				}
			}
		}
		//
		//
		if ( over_char_and_Conv29( s1 , buffer , 2555 , FALSE ) ) { // 2008.02.13 // 2008.02.28
			if      ( i4 < 0 ) {
				sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , buffer ); // 2015.10.12
			}
			else {
				if ( ( i4 / 100 ) > 0 ) { // 2013.12.05
					if      ( ( i4 % 100 ) <= 0 ) {
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) {
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , pmname , buffer );
					}
					else {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , pmname , buffer );
					}
				}
				else {
					if      ( ( i4 % 100 ) <= 0 ) { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) { // 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , buffer ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) { // Dummy+ 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , pmname , buffer );
					}
					else { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , pmname , buffer );
					}
				}
			}
		}
		else {
			if      ( i4 < 0 ) {
				sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , s1 ); // 2014.02.04 // 2015.10.12
			}
			else {
				if ( ( i4 / 100 ) > 0 ) { // 2013.12.05
					if      ( ( i4 % 100 ) <= 0 ) {
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) {
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , pmname , s1 );
					}
					else {
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , pmname , s1 );
					}
				}
				else {
					if      ( ( i4 % 100 ) <= 0 ) { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 99 ) { // 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %s%d%s %s %s" , side + 1 , MsgSltcharE , i1 , MsgSltchar , pmname , s1 ); // 2015.10.12
					}
					else if ( ( i4 % 100 ) == 5 ) { // DummyOnly 2007.11.29
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( ( i4 % 100 ) - 1 ) + 'A' - 1 , i1 , MsgSltchar , pmname , s1 );
					}
					else { // 2004.02.28
						sprintf( Send_Buffer , "PM_START%d %c%d%s %s %s" , side + 1 , ( i4 % 100 ) + 'A' - 1 , i1 , MsgSltchar , pmname , s1 );
					}
				}
			}
		}
		break;
	case TIMER_PM_END :
		//
		SYSTEM_WIN_UPDATE = 1; // 2013.03.11
		//
		sprintf( Send_Buffer , "PM_END %d %d" , i1 , i4 % 1000 ); // 2007.11.29
		break;

	case TIMER_MID_SET :
		sprintf( Send_Buffer , "MID_SET%d %s" , side + 1 , s1 );
		break;
	case TIMER_WID_SET :
		sprintf( Send_Buffer , "WID_SET%d %d %s" , side + 1 , i1 , s1 );
		break;
	case TIMER_LOG_DEL :
		sprintf( Send_Buffer , "LOG_DEL%d %s/LOTLOG %s" , side + 1 , s1 , s2 );
		break;
	default :
		return;
		break;
	}
	//
//	_dWRITE_FUNCTION_EVENT_TH( TIM_LOG_POINTER , Send_Buffer ); // 2013.10.30
	//
	SCH_LOG_TIMER_dWRITE_FUNCTION_EVENT_TH( Send_Buffer ); // 2013.10.30
	//
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//extern int				TM_OutCounter[ MAX_CASSETTE_SIDE ];
//extern int				TM_InCounter[ MAX_CASSETTE_SIDE ];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
void SCH_LOG_LOT_dWRITE_FUNCTION_EVENT_TH( int M , char *data ) { // 2013.10.30
	///
	int addr , sts , x;
	//
	if ( EVENT_SEND_LOT_LOG_POINTER_ASNCH[M] == 0 ) {
		//
		addr = LOT_LOG_POINTER[M];
		//
		_dDIGITAL_SHAPE( -1010 , &sts , &x , &x , &addr , data );
		//
		if ( sts != 1 ) {
			//
			_dWRITE_FUNCTION_EVENT_TH( LOT_LOG_POINTER[M] , data );
			//
			EVENT_SEND_LOT_LOG_POINTER_ASNCH[M]++;
		}
		//
	}
	else {
		//
		_dWRITE_FUNCTION_EVENT_TH( LOT_LOG_POINTER[M] , data );
		//
		EVENT_SEND_LOT_LOG_POINTER_ASNCH[M]++;
		//
		if ( EVENT_SEND_LOT_LOG_POINTER_ASNCH[M] > MAX_dWRITE_FUNCTION_EVENT_TH_TRY ) EVENT_SEND_LOT_LOG_POINTER_ASNCH[M] = 0;
		//
	}
}

extern int SYSTEM_WIN_R_TAG; // 2014.07.17

void _i_SCH_LOG_LOT_PRINTF( int M , LPSTR list , ... ) {
	FILE *fpt;
	va_list va;
	int i , al;
	char Buffer[32+1];
//	char TextBuffer[1024+1]; // 2013.01.09
	char TextBuffer[2555+2555+64+256+1]; // 2013.01.09
	SYSTEMTIME SysTime; // 2008.10.15
	//
	if ( M < 0 || M >= MAX_CASSETTE_SIDE ) return;
	if ( LOT_LOG_POINTER[M] < 0 ) return;
	//
	va_start( va , list );
//	_vsnprintf( TextBuffer , 1024 , list , (LPSTR) va ); // 2013.01.09
	_vsnprintf( TextBuffer , 2555+2555+64+256 , list , (LPSTR) va ); // 2013.01.09
	va_end( va );
	//
//	if ( _i_SCH_PRM_GET_DFIX_LOT_LOG_MODE() == 0 ) { // 2010.11.16
	if ( ( _i_SCH_PRM_GET_DFIX_LOT_LOG_MODE() % 2 ) == 0 ) { // 2010.11.16
		//	_dWRITE_FUNCTION_EVENT( LOT_LOG_POINTER[M] , TextBuffer ); // 2005.10.26
		//
//		_dWRITE_FUNCTION_EVENT_TH( LOT_LOG_POINTER[M] , TextBuffer ); // 2005.10.26 // 2013.10.30
		//
		SCH_LOG_LOT_dWRITE_FUNCTION_EVENT_TH( M , TextBuffer ); // 2013.10.30
		//
	}
	else {
		EnterCriticalSection( &CR_LOTLOG );
		//
		GetLocalTime( &SysTime );
		//
		switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
		case 1 : // %d y/m/d
			sprintf( Buffer , "%d/%d/%d %d:%d:%d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			break;
		case 2 : // %0?d y/m/d
			sprintf( Buffer , "%04d/%02d/%02d %02d:%02d:%02d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			break;
		case 3 : // %d y-m-d
			sprintf( Buffer , "%d-%d-%d %d:%d:%d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			break;
		case 4 : // %0?d y-m-d
			sprintf( Buffer , "%04d-%02d-%02d %02d:%02d:%02d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			break;
		case 5 : // %d y/m/d
			sprintf( Buffer , "%d/%d/%d %d:%d:%d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			break;
		case 6 : // %0?d y/m/d
			sprintf( Buffer , "%04d/%02d/%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			break;
		case 7 : // %d y-m-d
			sprintf( Buffer , "%d-%d-%d %d:%d:%d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			break;
		case 8 : // %0?d y-m-d
			sprintf( Buffer , "%04d-%02d-%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			break;
		default :
			sprintf( Buffer , "%d/%d/%d %d:%d:%d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , 
				SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			break;
		}
		//
		if ( STRNCMP_L( TextBuffer , "ALARM\t" , 6 ) ) al = 6;
		else                                           al = 0;
		//
		fpt = fopen( LOT_LOG_FILE[M] , "a" );
		if ( fpt != NULL ) {
			//
			if ( al == 6 ) {
				f_enc_printf( fpt , "%-19s\tALARM\t%s" , Buffer , TextBuffer + 6 );
			}
			else {
				f_enc_printf( fpt , "%-19s\tLOT%d\t%s" , Buffer , M + 1 , TextBuffer );
//				f_enc_printf( fpt , "%-19s\tLOT%d\t%s[%d,%d,%d,%d][%d,%d,%d,%d]" , Buffer , M + 1 , TextBuffer , TM_InCounter[0] , TM_InCounter[1] , TM_InCounter[2] , TM_InCounter[3] , TM_OutCounter[0] , TM_OutCounter[1] , TM_OutCounter[2] , TM_OutCounter[3] );
			}
			//
			fclose(fpt);
		}
		//
		i = al;
		while( TRUE ) {
			if      ( TextBuffer[i] == 0 ) {
				break;
			}
			else if ( TextBuffer[i] == 13 ) {
				TextBuffer[i] = 0;
				break;
			}
			else if ( TextBuffer[i] == 10 ) {
				TextBuffer[i] = 0;
				break;
			}
			else if ( TextBuffer[i] == 9 ) {
				TextBuffer[i] = 0;
				break;
			}
			i++;
		}
		if ( al == 6 ) {
			printf( "%-19s\tALARM\t%s\n" , Buffer , TextBuffer + al );
		}
		else {
			printf( "%-19s\tLOT%d\t%s\n" , Buffer , M + 1 , TextBuffer );
		}
		//
		LeaveCriticalSection( &CR_LOTLOG );
	}
	//===========================================================================================
	// 2005.11.30
	//===========================================================================================
//	if ( IO_DEBUG_MONITOR_STATUS != 0 ) { // 2008.09.19
	if ( ( IO_DEBUG_MONITOR_STATUS != 0 ) || ( IO_DEBUG_MONITOR_DATAVIEW != 0 ) ) { // 2008.09.19
		_i_SCH_LOG_DEBUG_PRINTF( M , -1 , "%s" , TextBuffer );
	}
	//===========================================================================================
	// 2014.07.17
	//===========================================================================================
	if ( SYSTEM_WIN_R_TAG != 0 ) _i_SCH_SYSTEM_INFO_LOG( "" , TextBuffer );
	//======================================================================================================================================================
	// 2008.09.30
	//======================================================================================================================================================
	if ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_LOG\tLOT_PRINTF\t%s" , M + 1 , TextBuffer );
	}
	//======================================================================================================================================================
	//===========================================================================================
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void IO_DEBUG_INIT() {
//	FILE *fpt; // 2006.02.28
//	char Buffer[64]; // 2014.10.31
	char Buffer[256]; // 2014.10.31
	//===================================================================================
	// 2006.02.24
	//===================================================================================
	_IO_GET_SYSTEM_FOLDER( Buffer , 63 ); // 2006.02.28
	sprintf( SCHEDULER_DEBUG_DATA_FILE , "%s/schdata.log" , Buffer );
	sprintf( SCHEDULER_DEBUG_FLOW_FILE , "%s/schdebug.log" , Buffer );
	//
	SCH_RESTART_SET_FOLDER( Buffer );
	//===================================================================================
	InitializeCriticalSection( &CR_DEBUG );
	InitializeCriticalSection( &CR_LOTLOG );

	if ( SYSTEM_LOGSKIP_STATUS() ) return; // 2004.05.21

	DeleteFile( SCHEDULER_DEBUG_DATA_FILE );
	DeleteFile( SCHEDULER_DEBUG_FLOW_FILE );

//	fpt = fopen( SCHEDULER_DEBUG_FLOW_FILE , "w" );
//	if ( fpt != NULL ) fclose(fpt);
}
//
BOOL IO_DEBUG_ENABLE( int group ) {
	//
	if ( IO_DEBUG_MONITOR_STATUS != 0 ) return FALSE; // 2008.04.18
	//
	IO_DEBUG_MONITOR_GROUP = group;
	IO_DEBUG_MONITOR_TRGCOUNT = 0; // 2008.04.18
	IO_DEBUG_MONITOR_CURCOUNT = 0; // 2008.04.18
	IO_DEBUG_MONITOR_STATUS = 1;
	//
	return TRUE;
}
//
void IO_DEBUG_WAITING( BOOL wait ) { // 2009.03.02
	IO_DEBUG_MONITOR_WAITING = wait;
}
//
void IO_DEBUG_DISABLE( BOOL alw ) {
//	if ( !IO_DEBUG_MONITOR_WAITING ) { // 2009.03.02 // 2015.06.11
	if ( alw || !IO_DEBUG_MONITOR_WAITING ) { // 2009.03.02 // 2015.06.11
		IO_DEBUG_MONITOR_STATUS = 0;
	}
}
//
BOOL IO_DEBUG_RUNNING() { // 2008.04.18
	if ( IO_DEBUG_MONITOR_STATUS == 0 ) return FALSE;
	return TRUE;
}
//
BOOL IO_DEBUG_FILE_ENABLE( int group , char *name , int trgcnt , BOOL simple ) { // 2005.03.15
	//
	if ( IO_DEBUG_MONITOR_STATUS != 0 ) return FALSE; // 2008.04.18
	//
	STR_MEM_MAKE_COPY2( &(IO_DEBUG_FILENAME) , name ); // 2008.04.02 // 2010.03.25
	//
	IO_DEBUG_MONITOR_GROUP = group;
	if ( name[0] == 0 ) {
		IO_DEBUG_MONITOR_TRGCOUNT = trgcnt; // 2008.04.18
		IO_DEBUG_MONITOR_CURCOUNT = 0; // 2008.04.18
//		IO_DEBUG_MONITOR_STATUS = 1; // 2015.06.11
		IO_DEBUG_MONITOR_STATUS = simple ? 11 : 1; // 2015.06.11
	}
	else {
		IO_DEBUG_MONITOR_TRGCOUNT = trgcnt; // 2008.04.18
		IO_DEBUG_MONITOR_CURCOUNT = 0; // 2008.04.18
//		IO_DEBUG_MONITOR_STATUS = 2; // 2015.06.11
		IO_DEBUG_MONITOR_STATUS = simple ? 12 : 2; // 2015.06.11
	}
	//
	return TRUE;
}
//
void _i_SCH_LOG_DEBUG_PRINTF( int M , int tm , LPSTR list , ... ) {
	FILE *fpt;
	va_list va;
//	struct tm Pres_Time; // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	TCHAR output[256]; // 2008.09.08

//	if ( IO_DEBUG_MONITOR_STATUS == 0 ) return; // 2008.09.08
	if ( ( IO_DEBUG_MONITOR_STATUS == 0 ) && ( IO_DEBUG_MONITOR_DATAVIEW == 0 ) ) return; // 2008.09.08
	//
	if ( IO_DEBUG_MONITOR_DATAVIEW != 0 ) { // 2008.09.08
		//
		va_start( va , list );
		//
		sprintf( output , "[%d]" , M );
		//
		if ( _vsnprintf( output + strlen( output ) , 252 , list , va ) < 0 ) {
			output[255] = '\0';
			output[254] = output[253] = output[252] = '.';
		}
		//
		OutputDebugString( output );
		//
		va_end( va );
		//
		if ( IO_DEBUG_MONITOR_STATUS == 0 ) return;
		//
	}
	//
	if ( ( IO_DEBUG_MONITOR_GROUP != -1 ) && ( tm != -1 ) ) {
		if      ( tm == 0 ) { // fm
			if ( IO_DEBUG_MONITOR_GROUP != 0 ) return;
		}
		else { // tm
			if ( IO_DEBUG_MONITOR_GROUP == 0 ) return;
		}
	}
	//
	if ( SYSTEM_LOGSKIP_STATUS() ) {
		IO_DEBUG_MONITOR_STATUS = 0; // 2008.04.18
		return; // 2008.04.17
	}
	//
	EnterCriticalSection( &CR_DEBUG );

//	_get-systime( &Pres_Time );
	GetLocalTime( &SysTime ); // 2006.10.02

	va_start( va , list );

//	printf( "%02d:%02d:%02d\t[%d]\t" , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , M ); // 2006.10.02
//	printf( "%02d:%02d:%02d\t[%d]\t" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , M ); // 2006.10.02 // 2007.04.27

//	if ( IO_DEBUG_MONITOR_STATUS == 1 ) { // 2008.04.18 // 2015.06.11
	if ( ( IO_DEBUG_MONITOR_STATUS == 1 ) || ( IO_DEBUG_MONITOR_STATUS == 11 ) ) { // 2008.04.18 // 2015.06.11
		if      ( IO_DEBUG_MONITOR_PRINTF == 0 ) {
			IO_DEBUG_MONITOR_PRINTF = 1;
		}
		else if ( IO_DEBUG_MONITOR_PRINTF == 1 ) {
			IO_DEBUG_MONITOR_PRINTF = 2;
		}
		else if ( IO_DEBUG_MONITOR_PRINTF == 2 ) {
			IO_DEBUG_MONITOR_PRINTF = 3;
		}
		else {
			IO_DEBUG_MONITOR_PRINTF = 0;
			printf( "#" ); // 2007.04.27
		}
	}

//	vprintf( list , (LPSTR) va ); // 2007.04.27

//	if ( !SYSTEM_LOGSKIP_STATUS() ) { // 2004.05.21 // 2008.04.17
//		if      ( IO_DEBUG_MONITOR_STATUS == 1 ) { // 2015.06.11
		if      ( ( IO_DEBUG_MONITOR_STATUS == 1 ) || ( IO_DEBUG_MONITOR_STATUS == 11 ) ) { // 2015.06.11
			fpt = fopen( SCHEDULER_DEBUG_FLOW_FILE , "a" );
		}
//		else if ( IO_DEBUG_MONITOR_STATUS == 2 ) { // 2015.06.11
		else if ( ( IO_DEBUG_MONITOR_STATUS == 2 ) || ( IO_DEBUG_MONITOR_STATUS == 12 ) ) { // 2015.06.11
			if ( IO_DEBUG_FILENAME == NULL ) { // 2008.04.02
				fpt = fopen( SCHEDULER_DEBUG_FLOW_FILE , "a" );
			}
			else {
				if ( IO_DEBUG_FILENAME[0] == 0 ) {
					fpt = fopen( SCHEDULER_DEBUG_FLOW_FILE , "a" );
				}
				else {
					fpt = fopen( IO_DEBUG_FILENAME , "a" );
				}
			}
		}
		else {
			fpt = NULL;
		}
		if ( fpt != NULL ) {
//			fprintf( fpt , "%02d:%02d:%02d\t[%d]\t" , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , M ); // 2006.10.02
//			fprintf( fpt , "%02d:%02d:%02d\t[%d]\t" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , M ); // 2006.10.02 // 2019.01.18
			fprintf( fpt , "%02d:%02d:%02d.%03d\t[%d]\t" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , M ); // 2006.10.02 // 2019.01.18

			vfprintf( fpt , list , (LPSTR) va );


//			{
//				int i;
//
//				fprintf( fpt , "\t\t\t" );
//				for ( i = PM1 ; i < TM ; i++ )
//					fprintf( fpt , "(%s:%d:%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , SCH_MACRO_PROCESS_MOVE[i] , SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] );
//				fprintf( fpt , "\n" );
//			}













			fclose(fpt);
		}
//	} // 2008.04.17
	va_end( va );
	//
	if ( IO_DEBUG_MONITOR_TRGCOUNT > 0 ) { // 2008.04.18
		IO_DEBUG_MONITOR_CURCOUNT++; // 2008.04.18
		if ( IO_DEBUG_MONITOR_CURCOUNT >= IO_DEBUG_MONITOR_TRGCOUNT ) { // 2008.04.18
			if ( !IO_DEBUG_MONITOR_WAITING ) { // 2009.03.02
				IO_DEBUG_MONITOR_STATUS = 0; // 2008.04.18
			}
		} // 2008.04.18
	} // 2008.04.18
	//
	LeaveCriticalSection( &CR_DEBUG );
}
//



///*
void xprintf( LPSTR list , ... ) { // 2006.01.16
	FILE *fpt;
	va_list va;
	SYSTEMTIME		SysTime; // 2006.10.02
	GetLocalTime( &SysTime ); // 2006.10.02
	va_start( va , list );
	if ( !SYSTEM_LOGSKIP_STATUS() ) {
		fpt = fopen( "history\\test.log" , "a" );
		if ( fpt != NULL ) {
			fprintf( fpt , "%02d:%02d:%02d\t" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			vfprintf( fpt , list , (LPSTR) va );
			fclose(fpt);
		}
	}
	va_end( va );
}
//*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
// 2005.03.14
void IO_MSG_PRINTF( LPSTR list , ... ) {
	va_list va;
	char TextBuffer[1024+1];
	if ( MSG_LOG_POINTER < 0 ) return;
	va_start( va , list );
	vsprintf( TextBuffer , list , (LPSTR) va );
	va_end( va );
	_dWRITE_FUNCTION_EVENT( MSG_LOG_POINTER , TextBuffer );
}
*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int IO_LOT_LOG_STATUS( int M ) {
	if ( LOT_LOG_POINTER[M] < 0 ) return 0; // 2008.10.15
	return 2; // 2008.10.15
//	return TRUE; // 2008.10.15
}

//

void _IO_MSGEVENT_PRINTF( char *SeparateSting , char *data ) {
	FILE *fpt;
	char FileName[64];
	SYSTEMTIME		SysTime;
	int dday , dm;
	//
	if ( SYSTEM_LOGSKIP_STATUS() ) return;
	//
	GetLocalTime( &SysTime );
	//
	dm = 0;
	dday = SysTime.wDay - 10;
	//
	if ( dday <= 0 ) {
		dday = dday + 31;
		dm = 1;
	}
	//
	if ( EVENT_LOG_OLD[ dday ] != 2 ) { // 2015.04.09
		//
		EVENT_LOG_OLD[ dday ] = 2;
		//
		sprintf( FileName , "%s\\LOTLOG\\SCHEVT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth - dm , dday );
		DeleteFile( FileName );
		//
		sprintf( FileName , "%s\\LOTLOG\\SCHEVT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth - dm , dday - 1 );
		DeleteFile( FileName );
		//
		sprintf( FileName , "%s\\LOTLOG\\SCHEVT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth - dm , dday - 2 );
		DeleteFile( FileName );
		//
		sprintf( FileName , "%s\\LOTLOG\\SCHEVT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth - dm , dday - 3 );
		DeleteFile( FileName );
		//
	}
	//
	EVENT_LOG_OLD[ SysTime.wDay ] = 1; // 2015.04.09
	//
	sprintf( FileName , "%s\\LOTLOG\\SCHEVT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth , SysTime.wDay );
	//
	fpt = fopen( FileName , "a" );
	//
	if ( fpt != NULL ) {
		fprintf( fpt , "%04d/%02d/%02d %02d:%02d:%02d.%03d\t%s\t%s\n" ,
			SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ,
			SeparateSting ,
			data );
		fclose( fpt );
	}
}


//------------------------------------------------------------------------------------------
void _IO_DLL_MESSAGE_PRINTF( char *SeparateSting , LPSTR list , ... ) {
	FILE *fpt;
	char FileName[64];
	SYSTEMTIME		SysTime;
	va_list va;
	char TextBuffer[1024+1];

	if ( SYSTEM_LOGSKIP_STATUS() ) return;

	GetLocalTime( &SysTime );
	sprintf( FileName , "%s\\LOTLOG\\SCHDLL-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth , SysTime.wDay );
	fpt = fopen( FileName , "a" );
	if ( fpt != NULL ) {
		va_start( va , list );
		_vsnprintf( TextBuffer , 1024 , list , (LPSTR) va );
		va_end( va );
		fprintf( fpt , "%04d/%02d/%02d %02d:%02d:%02d.%03d\t%s\t%s\n" ,
			SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ,
			SeparateSting ,
			TextBuffer );
		fclose( fpt );
	}
}
