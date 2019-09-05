#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"

//-------------------------------------------
BOOL			Global_Chamber_Pre_NotUse_Flag[MAX_CHAMBER]; // 2002.03.25
//
int 			Global_Chamber_FirstPre_Use_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2003.11.11
//
BOOL			Global_Chamber_FirstPre_Done_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2005.10.27
//
BOOL			Global_Chamber_FirstPre_NotUse_Flag[ MAX_CASSETTE_SIDE ][MAX_CASSETTE_SIDE]; // 2005.10.27
BOOL			Global_Chamber_FirstPre_NotUse2_Flag[ MAX_CASSETTE_SIDE ][MAX_CASSETTE_SIDE]; // 2007.09.07
//-------------------------------------------
int  SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag( int side , int ch ) { return Global_Chamber_FirstPre_Use_Flag[side][ch]; }
int  SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag( int side , int ch ) { return Global_Chamber_FirstPre_Done_Flag[side][ch]; }
//-------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// RunPre Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULER_GLOBAL_Set_Chamber_Pre_NotUse_Flag( int ch , int data ) { Global_Chamber_Pre_NotUse_Flag[ch] = data; } // 2002.03.25
int  SCHEDULER_GLOBAL_Get_Chamber_Pre_NotUse_Flag(  int ch ) { return Global_Chamber_Pre_NotUse_Flag[ch]; } // 2002.03.25
//------------------------------------------------------------------------------------------
void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( int side , int data ) { // 2003.11.11
	int i;
	if      ( data == -1 ) { // 2007.09.03
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			Global_Chamber_FirstPre_Done_Flag[side][i] = FALSE;
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Global_Chamber_FirstPre_NotUse_Flag[side][i] = FALSE;
			Global_Chamber_FirstPre_NotUse2_Flag[side][i] = FALSE; // 2007.09.07
		}
	}
	else if ( ( data == 0 ) || ( data == 1 ) ) {
		if ( data == 0 ) {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_NOTUSE;
			}
		}
		else {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( Global_Chamber_FirstPre_Use_Flag[side][i] ) {
				case FORCE_PRE_TAG_NOTUSE			:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST;		break;
				case FORCE_PRE_TAG_FIRST			:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST;		break;
				case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_FIRST_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_DONE_FIRST		:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST;		break;
				case FORCE_PRE_TAG_DONE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				default								:	Global_Chamber_FirstPre_Use_Flag[side][i] = FORCE_PRE_TAG_FIRST;		break;
				}
			}
		}
		//===============================================================
		// 2005.10.27
		//===============================================================
		if ( data == 1 ) {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				Global_Chamber_FirstPre_Done_Flag[side][i] = FALSE;
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				Global_Chamber_FirstPre_NotUse_Flag[side][i] = FALSE;
				Global_Chamber_FirstPre_NotUse2_Flag[side][i] = FALSE; // 2007.09.07
			}
		}
		//===============================================================
	}
	else if ( data == 2 ) {
		//===============================================================
		// 2006.11.15
		//===============================================================
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) { // 2006.11.17
				switch( Global_Chamber_FirstPre_Use_Flag[i][side] ) {
				case FORCE_PRE_TAG_NOTUSE			:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_AGAIN;		break;
				case FORCE_PRE_TAG_FIRST			:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_AGAIN;		break;
				case FORCE_PRE_TAG_FIRST_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_DONE_FIRST		:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				case FORCE_PRE_TAG_DONE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_AGAIN;		break;
				case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
				default								:	Global_Chamber_FirstPre_Use_Flag[i][side] = FORCE_PRE_TAG_AGAIN;		break;
				}
			}
		}
		//===============================================================
	}
	else if ( ( data == 3 ) || ( data == 4 ) ) {
		//===============================================================
		// 2007.02.22
		//===============================================================
		switch( Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] ) {
		case FORCE_PRE_TAG_NOTUSE			:
			if ( data == 4 ) Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST;
			break;
		case FORCE_PRE_TAG_FIRST			:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST;		break;
		case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_AGAIN;		break;
		case FORCE_PRE_TAG_FIRST_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
		case FORCE_PRE_TAG_DONE_FIRST		:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST;		break;
		case FORCE_PRE_TAG_DONE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_AGAIN;		break;
		case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
		default								:	Global_Chamber_FirstPre_Use_Flag[side/1000][side%1000] = FORCE_PRE_TAG_FIRST;		break;
		}
		//===============================================================
	}
}
//
void _i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( int side , int ch ) { // 2005.02.17
	switch( Global_Chamber_FirstPre_Use_Flag[side][ch] ) {
	case FORCE_PRE_TAG_NOTUSE			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST;		break;
	case FORCE_PRE_TAG_FIRST			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST;		break;
	case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
	case FORCE_PRE_TAG_FIRST_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
	case FORCE_PRE_TAG_DONE_FIRST		:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST;		break;
	case FORCE_PRE_TAG_DONE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
	case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST_AGAIN;	break;
	default								:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_FIRST;		break;
	}
}
//
void _i_SCH_PREPRCS_FirstRunPre_Clear_PathProcessData( int side , int ch ) { // 2005.12.20
	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_NOTUSE;
}
//
BOOL _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( int side , int ch ) { // 2003.11.11
//	return Global_Chamber_FirstPre_Use_Flag[side][ch]; // 2005.10.27
	//===============================================================
	// 2005.10.27
	//===============================================================
	int i , j;

	switch( Global_Chamber_FirstPre_Use_Flag[side][ch] ) {
	case FORCE_PRE_TAG_NOTUSE			:	return FALSE;	break;
	case FORCE_PRE_TAG_FIRST			:					break;
	case FORCE_PRE_TAG_AGAIN			:					break;
	case FORCE_PRE_TAG_FIRST_AGAIN		:					break;
	case FORCE_PRE_TAG_DONE_FIRST		:	return FALSE;	break;
	case FORCE_PRE_TAG_DONE_AGAIN		:	return FALSE;	break;
	case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:	return FALSE;	break;
	default								:	return FALSE;	break;
	}

//	if ( !Global_Chamber_FirstPre_NotUse_Flag[side][side] ) return TRUE; // 2007.09.07

	if ( Global_Chamber_FirstPre_NotUse_Flag[side][side] ) { // 2007.09.07
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != side ) {
				if ( Global_Chamber_FirstPre_NotUse_Flag[side][i] ) {
	//				if ( Global_Chamber_FirstPre_Done_Flag[side][ch] ) { // 2007.09.03
					if ( Global_Chamber_FirstPre_Done_Flag[i][ch] ) {
	//					return TRUE; // 2007.09.03
						return FALSE; // 2007.09.03
					}
				}
			}
		}
	}
	//===========================================================================
	// 2007.09.07
	//===========================================================================
	if ( Global_Chamber_FirstPre_NotUse2_Flag[side][side] ) {
		j = -1;
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != side ) {
				if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
					if ( j == -1 ) {
						j = i;
					}
					else {
						if ( _i_SCH_SYSTEM_RUNORDER_GET( j ) > _i_SCH_SYSTEM_RUNORDER_GET( i ) ) {
							j = i;
						}
					}
				}
			}
		}
		if ( j != -1 ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( i != side ) {
					if ( Global_Chamber_FirstPre_NotUse2_Flag[side][i] ) {
						if ( i == j ) return FALSE;
					}
				}
			}
		}
	}
	//===========================================================================
//	return FALSE; // 2007.09.03
	return TRUE; // 2007.09.03
	//===============================================================
}
//
void _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( int side , int ch ) { // 2003.11.11
	// 2012.03.23
	/*
	// 2011.01.20
	int Res , i;
	//
	Res = Global_Chamber_FirstPre_Use_Flag[side][ch];
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		switch( Res ) {
		case FORCE_PRE_TAG_NOTUSE			:																			break;
		case FORCE_PRE_TAG_USE				:	Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_USE;		break;
		case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_AGAIN;		break;
		case FORCE_PRE_TAG_USE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_USE_AGAIN;	break;
		case FORCE_PRE_TAG_DONE_USE			:																			break;
		case FORCE_PRE_TAG_DONE_AGAIN		:																			break;
		case FORCE_PRE_TAG_DONE_USE_AGAIN	:																			break;
		default								:	Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_NOTUSE;			break;
		}
	}
	*/
	/*
	// ORIGINAL_AREA 2011.01.20
	switch( Global_Chamber_FirstPre_Use_Flag[side][ch] ) {
	case FORCE_PRE_TAG_NOTUSE			:																				break;
	case FORCE_PRE_TAG_USE				:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_USE;		break;
	case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_AGAIN;		break;
	case FORCE_PRE_TAG_USE_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_USE_AGAIN;	break;
	case FORCE_PRE_TAG_DONE_USE			:																				break;
	case FORCE_PRE_TAG_DONE_AGAIN		:																				break;
	case FORCE_PRE_TAG_DONE_USE_AGAIN	:																				break;
	default								:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_NOTUSE;			break;
	}
	*/
	// 2012.03.23
	/*
	switch( Global_Chamber_FirstPre_Use_Flag[side][ch] ) {
	case FORCE_PRE_TAG_NOTUSE			:																				break;
	case FORCE_PRE_TAG_FIRST			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_FIRST;		break;
	case FORCE_PRE_TAG_AGAIN			:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_AGAIN;		break;
	case FORCE_PRE_TAG_FIRST_AGAIN		:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_DONE_FIRST_AGAIN;	break;
	case FORCE_PRE_TAG_DONE_FIRST		:																				break;
	case FORCE_PRE_TAG_DONE_AGAIN		:																				break;
	case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:																				break;
	default								:	Global_Chamber_FirstPre_Use_Flag[side][ch] = FORCE_PRE_TAG_NOTUSE;			break;
	}
	*/
	// 2012.03.27
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		switch( Global_Chamber_FirstPre_Use_Flag[i][ch] ) {
		case FORCE_PRE_TAG_NOTUSE			:
			break;
		case FORCE_PRE_TAG_FIRST			:
			if ( i == side ) {
				Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_FIRST;
			}
			break;
		case FORCE_PRE_TAG_AGAIN			:
			Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_AGAIN;
			break;
		case FORCE_PRE_TAG_FIRST_AGAIN		:
			if ( i == side ) {
				Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_DONE_FIRST_AGAIN;
			}
			else {
				Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_FIRST;
			}
			break;
		case FORCE_PRE_TAG_DONE_FIRST		:
			break;
		case FORCE_PRE_TAG_DONE_AGAIN		:
			break;
		case FORCE_PRE_TAG_DONE_FIRST_AGAIN	:
			break;
		default								:
			Global_Chamber_FirstPre_Use_Flag[i][ch] = FORCE_PRE_TAG_NOTUSE;
			break;
		}
	}
	//===============================================================
	// 2005.10.27
	//===============================================================
	Global_Chamber_FirstPre_Done_Flag[side][ch] = TRUE;
	//===============================================================
}
//
void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse( int side , char *data ) { // 2005.10.27
	int i , j , l;
	if ( data[0] == 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Global_Chamber_FirstPre_NotUse_Flag[side][i] = FALSE;
		}
	}
	else {
		l = strlen( data );
		for ( i = 0 ; i < l ; i++ ) {
			j = data[i] - '0' - 1;
			if ( ( j >= 0 ) && ( j < MAX_CASSETTE_SIDE ) && ( j != side ) ) {
				Global_Chamber_FirstPre_NotUse_Flag[side][ j ] = TRUE;
				Global_Chamber_FirstPre_NotUse_Flag[side][ side ] = TRUE;
			}
		}
	}
}

void _i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse2( int side , char *data ) { // 2007.09.07
	int i , j , l;
	if ( data[0] == 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Global_Chamber_FirstPre_NotUse2_Flag[side][i] = FALSE;
		}
	}
	else {
		l = strlen( data );
		for ( i = 0 ; i < l ; i++ ) {
			j = data[i] - '0' - 1;
			if ( ( j >= 0 ) && ( j < MAX_CASSETTE_SIDE ) && ( j != side ) ) {
				Global_Chamber_FirstPre_NotUse2_Flag[side][ j ] = TRUE;
				Global_Chamber_FirstPre_NotUse2_Flag[side][ side ] = TRUE;
			}
		}
	}
}


void INIT_SCHEDULER_CONTROL_FIRSTRUNPRE_DATA( int apmode , int side ) {
	int i;
	if ( ( apmode == 2 ) || ( apmode == 4 ) ) {
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( side , 0 ); // 2007.09.05
	}
	else {
		if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
			if ( apmode == 0 ) {
				for ( i = 0 ; i < MAX_CHAMBER ; i ++ ) { // 2002.03.25
					SCHEDULER_GLOBAL_Set_Chamber_Pre_NotUse_Flag( i , FALSE );
				}
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( i , 0 );
			}
		}
	}
}
