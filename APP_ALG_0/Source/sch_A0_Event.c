//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
//================================================================================
#include "sch_A0_Param.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub_sp2.h" // 2005.10.07
#include "sch_A0_sub_sp3.h" // 2006.03.07
#include "sch_A0_sub_sp4.h" // 2006.03.07
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_sub_F_dbl1.h"
//================================================================================


int Get_LOT_ID_From_String_AL0( char *mdata , BOOL idalso ) {
	int i;
	if      ( STRNCMP_L( mdata , "PORT" , 4 ) ) {
		i = atoi( mdata + 4 ) - 1;
		if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
	}
	else if ( STRNCMP_L( mdata , "CM" , 2 ) ) {
		i = atoi( mdata + 2 ) - 1;
		if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
	}
	else {
		if ( idalso ) { // 2006.11.17
			if ( ( mdata[0] >= '1' ) && ( mdata[0] <= '9' ) ) {
				i = atoi( mdata ) - 1;
				if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
			}
			else {
				i = -1;
			}
		}
		else {
			i = -1;
		}
	}
	return i;
}

int USER_EVENT_PRE_ANALYSIS_AL0( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	return 1;
}

//void Test_XXXX();
int USER_EVENT_ANALYSIS_AL0( char *FullMessage , char *RunMessage , char *ElseMessage , char *ErrorMessage ) {
	int LoopMax , AlingMode , Module , OutSlot , InSlot , Spid , ENDINDEX;
	char ElseStr[513] , Else1Str[513] , Else2Str[513];
	char Pm1Str[256] , Pm2Str[256] , Pm3Str[256] , Pm4Str[256] , Pm5Str[256] , Pm6Str[256];
	char SublogStr[256]; // 2011.09.07
	//
//	if ( STRCMP_L( RunMessage , "XXXXXXXXXX" ) ) {
//		Test_XXXX();
//		return 1;
//	}

	if ( STRNCMP_L( RunMessage , "APPEND_METHOD_FOR_CM" , 20 ) ) { // 2005.10.07
		//===========
		//---------------------------------------------------
		// 2008.01.17
		//---------------------------------------------------
		if      ( RunMessage[20] == 0 ) {
			LoopMax = -1;
		}
		else {
			if ( ( RunMessage[20] >= '1' ) && ( RunMessage[20] <= '9' ) ) {
				LoopMax = RunMessage[20] - '0';
			}
			else {
				LoopMax = -1;
			}
		}
		//---------------------------------------------------
		STR_SEPERATE_CHAR( ElseMessage , '|' , Else1Str , ElseStr , 255 );		// incm
		//---------------------------------------------------
		STR_SEPERATE_CHAR( Else1Str , ':' , Pm1Str , Pm2Str , 255 );		// incm : AutoAlign
		//---------------------------------------------------
		Module = Get_LOT_ID_From_String_AL0( Pm1Str , FALSE ) + CM1;
		//---------------------------------------------------
		if ( STRCMP_L( Pm2Str , "ALIGN" ) ) AlingMode = 2;
		else                                AlingMode = 1;
		//===========
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );		// inslot
		InSlot = atoi( Else1Str );
		//===========
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );		// outcm
		ENDINDEX = -1;
		if      ( STRNCMP_L( Else1Str , "AUTO" , 4 ) ) {
			ENDINDEX = 0;
		}
		else if ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			ENDINDEX = atoi( Else1Str + 4 );
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			ENDINDEX = atoi( Else1Str + 2 );
		}
		//===========
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );		// outslot
		OutSlot = atoi( Else1Str );
		//===========
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );		// spid
		Spid = atoi( Else1Str );
		//===========
		STR_SEPERATE_CHAR( ElseStr , '|' , Pm1Str , Else2Str , 255 );		// PM1
		STR_SEPERATE_CHAR( Else2Str , '|' , Pm2Str , ElseStr , 255 );		// PM2
		STR_SEPERATE_CHAR( ElseStr , '|' , Pm3Str , Else2Str , 255 );		// PM3
		STR_SEPERATE_CHAR( Else2Str , '|' , Pm4Str , ElseStr , 255 );		// PM4
		STR_SEPERATE_CHAR( ElseStr , '|' , Pm5Str , Else2Str , 255 );		// PM5
		STR_SEPERATE_CHAR( Else2Str , '|' , Pm6Str , ElseStr , 255 );		// PM6
		STR_SEPERATE_CHAR( ElseStr , '|' , SublogStr , Else2Str , 255 );		// sub Log Folder // 2011.09.07
		//===========
		if ( SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2_PLUSDLL( LoopMax , 0 , AlingMode , -1 , Module , ENDINDEX , InSlot , OutSlot , Spid , Pm1Str , Pm2Str , Pm3Str , Pm4Str , Pm5Str , Pm6Str , SublogStr ) != 0 ) {
			//
			strcpy( ErrorMessage , "APPEND_METHOD_FOR_CM OPERATION ERROR(INVALID DATA or END NOTAPPEND)" );
			//
			return SYS_ABORTED;
		}
		//===========
		return 1;
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunMessage , "APPEND_END_FOR_CM" ) ) { // 2005.10.25 // 2005.12.01
		STR_SEPERATE_CHAR( ElseMessage , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		Module = Get_LOT_ID_From_String_AL0( Else1Str , TRUE );
		if ( ( Module < 0 ) || ( Module >= MAX_CASSETTE_SIDE ) ) {
			strcpy( ErrorMessage , "PORT ANALYSIS ERROR" );
			return 0;
		}
		//---------------------------------------------------
		if      ( STRCMP_L( Else2Str , "NOTAPPEND" ) ) {
			ENDINDEX = 2;
		}
		else if ( STRCMP_L( Else2Str , "CONTINUE" ) ) {
			ENDINDEX = 0;
		}
		else {
			ENDINDEX = 1;
		}
		//---------------------------------------------------
		if ( ENDINDEX == 0 ) {
			_SCH_IO_SET_END_BUTTON( Module , CTCE_IDLE );
		}
		else {
			_SCH_IO_SET_END_BUTTON( Module , CTCE_ENDING );
		}
		//---------------------------------------------------
		SIGNAL_MODE_APPEND_END_SET( Module , ENDINDEX );
		//---------------------------------------------------
		return 1;
	}
	else {
		return -1;
	}
}
