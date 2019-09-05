#include "default.h"
#include <commctrl.h>

#include "EQ_Enum.h"

#include "system_tag.h"
#include "ioLog.h"
#include "sch_prm_FBTPM.h"
#include "sch_Multi_ALIC.h"
#include "sch_prm.h"
#include "Equip_Handling.h"
#include "DLL_Interface.h"
#include "User_Parameter.h"

//

//
int EQUIP_CHECK_STATUS_RUNNING( int Chamber ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( EQUIP_STATUS_PROCESS( i , Chamber ) == SYS_RUNNING ) return TRUE;
	}
	if ( EQUIP_READ_FUNCTION( Chamber ) == SYS_RUNNING ) return TRUE;
	//
//	if ( _EQUIP_RUNNING2_TAG[Chamber] ) return TRUE; // 2012.10.31
	if ( _i_SCH_SYSTEM_EQUIP_RUNNING_GET(Chamber) ) return TRUE; // 2012.10.31
	//
	return FALSE;
}


/*
0.Format

	       2         3   
	      9012345678901234
	 0 ~18
	 ----------------------------
	"%-19s <POST>      - [%s][%s]"
	 --+-- ------+----    -+  -+
	   |         |         |   |
	   |         |         |   +---> 4.추가정보
	   |         |         |
	   |         |         +-------> 3.기본정보
	   |         |
	   |         +-----------------> 2.Style정보 (11 char)
	   |
	   +---------------------------> 1.19 문자 Time 정보 (19 char)


1.19 문자 Time 정보

2.Style정보

	"<POST>     "	: Alarm Post
	"<CLEAR>    "	: Alarm Clear

	"<USER>     "	: Alarm User Message

	"<R=CLEAR>  "	: Alarm Recovery with CLEAR
	"<R=RETRY>  "	: Alarm Recovery with RETRY
	"<R=WAIT>   "	: Alarm Recovery with WAIT
	"<R=IGNORE> "	: Alarm Recovery with IGNORE
	"<R=DISABLE>"	: Alarm Recovery with DISABLE
	"<R=ENABLE> "	: Alarm Recovery with ENABLE
	"<R=STOP>   "	: Alarm Recovery with STOP
	"<R=ABORT>  "	: Alarm Recovery with ABORT

	"<N=CLEAR>  "	: Alarm Recovery with CLEAR(Not Clear)
	"<N=RETRY>  "	: Alarm Recovery with RETRY(Not Clear)
	"<N=WAIT>   "	: Alarm Recovery with WAIT(Not Clear)
	"<N=IGNORE> "	: Alarm Recovery with IGNORE(Not Clear)
	"<N=DISABLE>"	: Alarm Recovery with DISABLE(Not Clear)
	"<N=ENABLE> "	: Alarm Recovery with ENABLE(Not Clear)
	"<N=STOP>   "	: Alarm Recovery with STOP(Not Clear)
	"<N=ABORT>  "	: Alarm Recovery with ABORT(Not Clear)

3.기본정보

	a) Alarm Message 문자 길이가 98보다 작거나 같을 경우 - 고정 Format 사용

		[%-8s %-4d %-30s %-98s %d %-19s]
		 --+- --+- --+-- --+-- -+ --+--
		   |    |    |     |    |   |
		   |    |    |     |    |   +---> 19 문자 Time 정보 (19 char)
		   |    |    |     |    |
		   |    |    |     |    +-------> Alarm Level 0-9 (1 char)
		   |    |    |     |
		   |    |    |     +------------> Alarm Message (98 char)
		   |    |    |
		   |    |    +------------------> Alarm Name (30 char)
		   |    |
		   |    +-----------------------> Alarm No (4 char)
		   |
		   +----------------------------> Alarm Module (8 char) , "++++++++" 일 경우 Local

	b) Alarm Message 문자 길이가 98보다 클경우 - 가변 Format 사용

		["%s" "%s" "%s" "%s" "%s" "%s"]
		  -+   -+   -+   -+   -+   -+
		   |    |    |    |    |   |
		   |    |    |    |    |   +----> Time 정보
		   |    |    |    |    |
		   |    |    |    |    +--------> Alarm Level 0-9
		   |    |    |    |
		   |    |    |    +-------------> Alarm Message
		   |    |    |
		   |    |    +------------------> Alarm Name
		   |    |
		   |    +-----------------------> Alarm No
		   |
		   +----------------------------> Alarm Module , "" 일 경우 Local

4.추가정보

		_ALARM_EVENT_RECEIVE();
		_ALARM_EVENT_RECEIVE_OPTION( 1 ); API를  수행 하였을 경우 하기 정보를 얻을 수 있다.

		[M:%d|N:%d|S:%d|R|F|F:%s|A|A:%s|D|D:%s|V]
		 --+- --+- --+- + + --+- + --+- + --+- +
		   |    |    |  | |   |  |   |  |   |  |
		   |    |    |  | |   |  |   |  |   |  +--> Happen Position 8 : Value Interlock (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |   |  |   |  |   |
		   |    |    |  | |   |  |   |  |   +-----> Happen Position 7 : Setpoint Interlock while Write Digital with Function Name(Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |   |  |   |  |
		   |    |    |  | |   |  |   |  +---------> Happen Position 6 : Setpoint Interlock while Write Digital (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |   |  |   |
		   |    |    |  | |   |  |   +------------> Happen Position 5 : Setpoint Interlock while Write Analog with Function Name (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |   |  |
		   |    |    |  | |   |  +----------------> Happen Position 4 : Setpoint Interlock while Write Analog (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |   |
		   |    |    |  | |   +-------------------> Happen Position 3 : Function,Function Name is %s (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  | |
		   |    |    |  | +-----------------------> Happen Position 2 : Function,Function Name is Unknown (Happen Position 1~8 은 한가지만 사용)
		   |    |    |  |
		   |    |    |  +-------------------------> Happen Position 1 : Remote,Unknown (Happen Position 1~8 은 한가지만 사용)
		   |    |    |
		   |    |    +----------------------------> Alarm Style (1:Post,2:Clear,3:Recovery)
		   |    |
		   |    +---------------------------------> Alarm No
		   |
		   +--------------------------------------> Module Index , 0 일 경우 Local

*/

int _ALARM_EVENT_RECEIVE_OPTION_DATA = 0; // 2012.03.08
//
enum { _FIND_ALARM_HP_UNKNOWN , _FIND_ALARM_HP_REMOTE , _FIND_ALARM_HP_FUNCTION ,
		_FIND_ALARM_MODE_HP_SAINTLKS , _FIND_ALARM_MODE_HP_SDINTLKS , _FIND_ALARM_MODE_HP_VINTLKS };

void ALARM_INFO_SET_EVENT_RECEIVE_OPTION( int data ) {
	if ( data < 0 ) {
		if ( _ALARM_EVENT_RECEIVE_OPTION_DATA == 0 ) _ALARM_EVENT_RECEIVE();
	}
	else {
		_ALARM_EVENT_RECEIVE_OPTION_DATA = data;
		//
		_ALARM_EVENT_RECEIVE();
		_ALARM_EVENT_RECEIVE_OPTION( _ALARM_EVENT_RECEIVE_OPTION_DATA );
	}
}


void Find_Extend_Data_From_Alarm( char *data0 , int *ALARM_MODULE_INDEX , int *ALARM_STYLE , int *ALARM_HAPPEN , char *ALARM_HAPPEN_INFO ) {
	int z , z2;
	//
	char Buffer[256];
	char data[256];
	//
	strcpy( data , data0 );
	//
	z = strlen( data );
	//
	if ( data[z-1] != ']' ) return;
	//
	data[z-1] = 0;
	//
	z = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		if      ( STRNCMP( Buffer , "M:" , 2 ) ) {
			*ALARM_MODULE_INDEX = atoi( Buffer + 2 );
		}
		else if ( STRNCMP( Buffer , "S:" , 2 ) ) {
			*ALARM_STYLE = atoi( Buffer + 2 );
		}
		else if ( STRCMP( Buffer , "R" ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_HP_REMOTE;
		}
		else if ( STRCMP( Buffer , "F" ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_HP_FUNCTION;
		}
		else if ( STRCMP( Buffer , "A" ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_MODE_HP_SAINTLKS;
		}
		else if ( STRCMP( Buffer , "D" ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_MODE_HP_SDINTLKS;
		}
		else if ( STRCMP( Buffer , "V" ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_MODE_HP_VINTLKS;
		}
		else if ( STRNCMP( Buffer , "F:" , 2 ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_HP_FUNCTION;
			strcpy( ALARM_HAPPEN_INFO , Buffer + 2 );
		}
		else if ( STRNCMP( Buffer , "A:" , 2 ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_MODE_HP_SAINTLKS;
			strcpy( ALARM_HAPPEN_INFO , Buffer + 2 );
		}
		else if ( STRNCMP( Buffer , "D:" , 2 ) ) {
			*ALARM_HAPPEN = _FIND_ALARM_MODE_HP_SDINTLKS;
			strcpy( ALARM_HAPPEN_INFO , Buffer + 2 );
		}
		//
		z = z2;
	}
	//
}

int Find_Module_Info_Using( int ch , BOOL fchk ) {
	int x;
	//
	for ( x = 0 ; x < MAX_CASSETTE_SIDE ; x++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( x ) ) continue;
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( x , ch ) == 0 ) continue;
		//
		if ( fchk ) {
			if ( !EQUIP_CHECK_STATUS_RUNNING( ch ) ) continue;
		}
		//
		return TRUE;
	}
	return FALSE;
}

int Find_Module_Info_From_Alarm( char *ALARM_MODULE , int ALARM_MODULE_INDEX , int ALARM_ID , char *ALARM_NAME , char *ALARM_MESSAGE , int ALARM_LEVEL , int ALARM_STYLE , int ALARM_HAPPEN , char *ALARM_HAPPEN_INFO , int *midx2 ) {
	int i , s , f = -1;
	//
	_IO_CONSOLE_PRINTF( "[ALARM2] [M=%s][MI=%d][ID=%d][NAME=%s][MSG=%s][L=%d][S=%d][H=%d][HI=%s]\n" , ALARM_MODULE , ALARM_MODULE_INDEX , ALARM_ID , ALARM_NAME , ALARM_MESSAGE , ALARM_LEVEL , ALARM_STYLE , ALARM_HAPPEN , ALARM_HAPPEN_INFO );
	//
	for ( s = 0 ; s < 6 ; s++ ) {
		//
		for ( i = CM1 ; i <= F_IC ; i++ ) {
			if      ( s == 0 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_MODULE , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) ) ) break;
			}
			else if ( s == 1 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_NAME , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) ) ) break;
			}
			else if ( s == 2 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_MODULE , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ) ) break;
			}
			else if ( s == 3 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_NAME , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ) ) break;
			}
			else if ( s == 4 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_HAPPEN_INFO , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) ) ) break;
			}
			else if ( s == 5 ) {
				if ( STRFIND_SUBSTRING_L( ALARM_HAPPEN_INFO , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ) ) break;
			}
		}
		//
		if ( i <= F_IC ) {
			//
			if ( Find_Module_Info_Using( i , FALSE ) ) {
				//
				if ( EQUIP_CHECK_STATUS_RUNNING( i ) ) {
					//
					f = i;
					//
					s = 6;
					//
					break;
				}
				//
				if ( f == -1 ) f = i;
				//
			}
			//
			if ( *midx2 == -1 ) *midx2 = i;
		}
		//
	}
	//
	s = f;
	//
	if ( USER_RECIPE_GET_MODULE_FROM_ALARM_INFO( f , ALARM_MODULE , ALARM_MODULE_INDEX , ALARM_ID , ALARM_NAME , ALARM_MESSAGE , ALARM_LEVEL , ALARM_STYLE , ALARM_HAPPEN , ALARM_HAPPEN_INFO , &s ) ) {
		f = s;
	}
	//
	return f;
}


int Find_Slot_Info_From_Module( int station1 , int *Use_Count , int *Lot_List , int *Slot_List ) {
	int i , i2 , j , x , s , f = FALSE , station2;
	//
	station2 = -1;
	//
	if      ( ( station1 >= CM1 ) && ( station1 < PM1 ) ) {
		if ( Find_Module_Info_Using( FM , TRUE ) ) {
			station2 = FM;
		}
	}
	else if ( ( station1 >= PM1 ) && ( station1 < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( Find_Module_Info_Using( TM + i , TRUE ) ) {
				station2 = TM + i;
				break;
			}
		}
	}
	else if ( ( station1 >= BM1 ) && ( station1 < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
		if ( Find_Module_Info_Using( FM , TRUE ) ) {
			station2 = FM;
		}
		else {
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				if ( Find_Module_Info_Using( TM + i , TRUE ) ) {
					station2 = TM + i;
					break;
				}
			}
		}
	}
	else if ( ( station1 >= TM ) && ( station1 < ( TM + MAX_TM_CHAMBER_DEPTH ) ) ) {
		//
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( Find_Module_Info_Using( i , TRUE ) ) {
				station2 = i;
				break;
			}
		}
		//
		if ( station2 == -1 ) {
			for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
				if ( Find_Module_Info_Using( i , TRUE ) ) {
					station2 = i;
					break;
				}
			}
		}
		//
	}
	else if ( station1 == FM ) {
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( Find_Module_Info_Using( i , TRUE ) ) {
				station2 = i;
				break;
			}
		}
		for ( i = CM1 ; i < PM1 ; i++ ) {
			if ( Find_Module_Info_Using( i , TRUE ) ) {
				station2 = i;
				break;
			}
		}
	}
	else if ( station1 == F_IC ) {
		if ( Find_Module_Info_Using( FM , TRUE ) ) {
			station2 = FM;
		}
	}
	else if ( station1 == F_AL ) {
		if ( Find_Module_Info_Using( FM , TRUE ) ) {
			station2 = FM;
		}
	}
	else {
		return FALSE;
	}
	//
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		if ( x == 0 ) s = station1;
		else          s = station2;
		//
		if ( ( x == 1 ) && f ) break;
		//
		if      ( ( s >= CM1 ) && ( s < PM1 ) ) {
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
				//
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != s ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathDo( i , j ) != 0 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_SUPPLY ) continue;
					//
					Lot_List[*Use_Count]  = i;
					Slot_List[*Use_Count] =  _i_SCH_CLUSTER_Get_SlotIn( i , j );
					(*Use_Count)++;
					//
					f = TRUE;
					//
					i = MAX_CASSETTE_SIDE;
					//
					break;
					//
				}
				//
			}
			//
		}
		else if ( ( s >= PM1 ) && ( s < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_PM_WAFER( s , i );
					}
					else {
						j = _i_SCH_MODULE_Get_PM_WAFER_B( s , i );
					}
					//
					if ( i == 0 ) {
						if ( ( j % 100 ) > 0 ) {
							//
							Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_PM_SIDE( s , i );
							Slot_List[*Use_Count] =  ( j % 100 );
							(*Use_Count)++;
							//
							f = TRUE;
							//
							if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
							//
						}
						//
						if ( ( j / 100 ) > 0 ) {
							//
							Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_PM_SIDE( s , i+1 );
							Slot_List[*Use_Count] =  ( j / 100 );
							(*Use_Count)++;
							//
							f = TRUE;
							//
							i++;
						}
						//
					}
					else {
						if ( j > 0 ) {
							//
							Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_PM_SIDE( s , i );
							Slot_List[*Use_Count] =  j;
							(*Use_Count)++;
							//
							f = TRUE;
						}
					}
				}
			}
			//
		}
		else if ( ( s >= BM1 ) && ( s < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_BM_WAFER( s , i+1 );
					}
					else {
						j = _i_SCH_MODULE_Get_BM_WAFER_B( s , i+1 );
					}
					//
					if ( j > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_BM_SIDE( s , i+1 );
						Slot_List[*Use_Count] =  j;
						(*Use_Count)++;
						//
						f = TRUE;
					}
					//
				}
				//
			}
			//
		}
		else if ( ( s >= TM ) && ( s < ( TM + MAX_TM_CHAMBER_DEPTH ) ) ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_TM_WAFER( s - TM , i );
					}
					else {
						j = _i_SCH_MODULE_Get_TM_WAFER_B( s - TM , i );
					}
					//
					if ( ( j % 100 ) > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_TM_SIDE( s - TM , i );
						Slot_List[*Use_Count] =  ( j % 100 );
						(*Use_Count)++;
						//
						f = TRUE;
						//
						if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
						//
					}
					if ( ( j / 100 ) > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_TM_SIDE2( s - TM , i );
						Slot_List[*Use_Count] =  ( j / 100 );
						(*Use_Count)++;
						//
						f = TRUE;
					}
				}
				//
			}
			//
		}
		else if ( s == FM ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < 2 ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_FM_WAFER( i );
					}
					else {
						j = _i_SCH_MODULE_Get_FM_WAFER_B( i );
					}
					//
					if ( j > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_FM_SIDE( i );
						Slot_List[*Use_Count] =  j;
						(*Use_Count)++;
						//
						f = TRUE;
					}
				}
			}
			//
		}
		else if ( s == F_IC ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < (MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE) ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_MFIC_WAFER( i+1 );
					}
					else {
						j = _i_SCH_MODULE_Get_MFIC_WAFER_B( i+1 );
					}
					//
					if ( j > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_MFIC_SIDE( i+1 );
						Slot_List[*Use_Count] =  j;
						(*Use_Count)++;
						//
						f = TRUE;
					}
					//
				}
			}
			//
		}
		else if ( s == F_AL ) {
			//
			for ( i2 = 0 ; i2 < 2 ; i2++ ) {
				//
				if ( f ) break;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( *Use_Count >= MAX_CASSETTE_SLOT_SIZE ) break;
					//
					if ( i2 == 0 ) {
						j = _i_SCH_MODULE_Get_MFALS_WAFER( i+1 );
					}
					else {
						j = _i_SCH_MODULE_Get_MFALS_WAFER_B( i+1 );
					}
					//
					if ( j > 0 ) {
						//
						Lot_List[*Use_Count]  = _i_SCH_MODULE_Get_MFALS_SIDE( i+1 );
						Slot_List[*Use_Count] =  j;
						(*Use_Count)++;
						//
						f = TRUE;
					}
				}
			}
			//
		}
	}
	//
	return f;
}


int FIND_ALARM_INFO_FOR_LOT_SUB( char *list , int *Use_Count , int *Lot_List , int *Slot_List , int *Module_Index , int *Module_Index2 ) { // 2012.03.08
	//
	char	DISP_ALARM_MODULE[33];
	int		DISP_ALARM_ID;
	char	DISP_ALARM_NAME[65];
	char	DISP_ALARM_MESSAGE[257];
	char	DISP_ALARM_TIME[20];
	int		DISP_ALARM_LEVEL;
	//
	int		DISP_ALARM_STYLE;
	int		DISP_ALARM_MODULE_INDEX;
	int		DISP_ALARM_HAPPEN;
	char	DISP_ALARM_HAPPEN_INFO[256];
	//
	DISP_ALARM_MODULE_INDEX = -1;
	DISP_ALARM_STYLE = -1;
	DISP_ALARM_HAPPEN = -1;
	DISP_ALARM_HAPPEN_INFO[0] = 0;
	//
	if ( list[0] == '"' ) {
//		["%s" "%s" "%s" "%s" "%s" "%s"]
	}
	else {
//		[%-8s %-4d %-30s %-98s %d %-19s]
		//
		Ext_Str_From_Str( list ,   0 ,  8 , DISP_ALARM_MODULE );		STRTRUNCATE_SPACE( DISP_ALARM_MODULE );
		//
		DISP_ALARM_ID = Ext_Int_From_Str( list , 9 , 4 );
		//
		Ext_Str_From_Str( list ,  14 , 30 , DISP_ALARM_NAME );			STRTRUNCATE_SPACE( DISP_ALARM_NAME );
		//
		Ext_Str_From_Str( list ,  45 , 98 , DISP_ALARM_MESSAGE );		STRTRUNCATE_SPACE( DISP_ALARM_MESSAGE );
		//
		DISP_ALARM_LEVEL = Ext_Int_From_Str( list , 144 , 1 );
		//
		Ext_Str_From_Str( list , 146 , 19 , DISP_ALARM_TIME );		STRTRUNCATE_SPACE( DISP_ALARM_TIME );
		//
		if ( strcmp( DISP_ALARM_MODULE , "++++++++" ) == 0 ) strcpy( DISP_ALARM_MODULE , "" );
		if ( ( DISP_ALARM_LEVEL < 0 ) || ( DISP_ALARM_LEVEL >= 10 ) ) DISP_ALARM_LEVEL = 2;
		//
		if ( ( list[146+19] == ']' ) && ( list[146+20] == '[' ) ) {
			//
			Find_Extend_Data_From_Alarm( list + 146 + 21 , &DISP_ALARM_MODULE_INDEX , &DISP_ALARM_STYLE , &DISP_ALARM_HAPPEN , DISP_ALARM_HAPPEN_INFO );
			//
		}
		//
		*Module_Index = Find_Module_Info_From_Alarm( DISP_ALARM_MODULE , DISP_ALARM_MODULE_INDEX , DISP_ALARM_ID , DISP_ALARM_NAME , DISP_ALARM_MESSAGE , DISP_ALARM_LEVEL , DISP_ALARM_STYLE , DISP_ALARM_HAPPEN , DISP_ALARM_HAPPEN_INFO , Module_Index2 );
		//
	}
	//
	if ( *Module_Index == -1 ) return FALSE;
	//
	if ( !Find_Slot_Info_From_Module( *Module_Index , Use_Count , Lot_List , Slot_List ) ) return FALSE;
	//
	return TRUE;
	//	
}

int FIND_ALARM_INFO_FOR_LOT( char *data , int *Use_Count , int *Lot_List , int *Slot_List ) { // 2012.03.08
	int i , Res , mdlindex , mdlindex2;
	char Buffer[256];
	char Buffer2[32];
	//
	if ( _ALARM_EVENT_RECEIVE_OPTION_DATA == 0 ) return FALSE;
	//
	Res = FALSE;
	*Use_Count = 0;
	mdlindex = -1;
	mdlindex2 = -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		Lot_List[i] = -1;
		Slot_List[i] = 0;
	}
	//
	if ( data[34] == '[' ) Res = FIND_ALARM_INFO_FOR_LOT_SUB( data + 35 , Use_Count , Lot_List , Slot_List , &mdlindex , &mdlindex2 );
	//
	Buffer[0] = 0;
	//
	for ( i = 0 ; i < *Use_Count ; i++ ) {
		//
		sprintf( Buffer2 , "(%d,%d)" , Lot_List[i] , Slot_List[i] );
		//
		strcat( Buffer , Buffer2 );
	}
	//
	_IO_CONSOLE_PRINTF( "[ALARM] [%s][R=%d][MI=%d:%d][UC=%d][DATA=%s]\n" , data , Res , mdlindex , mdlindex2 , *Use_Count , Buffer );
	//
	return Res;
}
//
//
void ALARM_INFO_MANAGEMENT() {
	char Buffer[256];
	int i , s , Use_Count , Lot_List[MAX_CASSETTE_SLOT_SIZE] , Slot_List[MAX_CASSETTE_SLOT_SIZE];
	//
	while( _ALARM_RECV_BUFFER_READ( Buffer , 255 ) ) {
		//
		if ( !FIND_ALARM_INFO_FOR_LOT( Buffer , &Use_Count , Lot_List , Slot_List ) ) {
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( _i_SCH_SYSTEM_USING_GET( i ) > 4 ) {
					//
					_i_SCH_LOG_LOT_PRINTF( i , "ALARM\t%s\tALARM\n" , Buffer + 20 );
					//
				}
				//
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 1 ) { // 2014.06.26
					//
					s = _SCH_SYSTEM_OLD_SIDE_FOR_MTLOUT_GET( i );
					//
					if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) ) {
						//
						if ( _i_SCH_SYSTEM_USING_GET( s ) <= 4 ) {
							//
							_i_SCH_LOG_LOT_PRINTF( s , "ALARM\t%s\tALARM\n" , Buffer + 20 );
							//
						}
						//
					}
				}
				//
			}
			//
		}
		else {
			//
			for ( i = 0 ; i < Use_Count ; i++ ) {
				//
				if ( Lot_List[i] >= 0 ) {
					//
					if ( _i_SCH_SYSTEM_USING_GET( Lot_List[i] ) > 4 ) {
						//
						if ( Slot_List[i] > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( Lot_List[i] , "ALARM\t%s\tALARM\t%d\n" , Buffer + 20 , Slot_List[i] );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( Lot_List[i] , "ALARM\t%s\tALARM\n" , Buffer + 20 );
						}
					}
				}
				//
			}
			//
		}
		//
	}
}
//
