#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "sch_prm_Cluster.h" // 2015.07.30

//------------------------------------------------------------------------------------------

BOOL SDM_EVENT_INCREASE = FALSE; // 2015.01.30
//
int  SDM_LOTFIRST_CHAMBER_OPTION[ MAX_CHAMBER ];
int  SDM_LOTEND_CHAMBER_OPTION[ MAX_CHAMBER ];

int  SDM_RUN_CHAMBER[ MAX_CASSETTE_SLOT_SIZE ];
//
//char SDM_RUN_RECIPE[3][ MAX_CASSETTE_SLOT_SIZE ][ MAX_SDM_STYLE ][3][65]; // 2004.11.17 // 2008.04.02
char *SDM_RUN_RECIPE[3][ MAX_CASSETTE_SLOT_SIZE ][ MAX_SDM_STYLE ][3]; // 2004.11.17 // 2008.04.02
//
//int  SDM_RUN_USECOUNT[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.08.06
int  SDM_RUN_USECOUNT[ MAX_CASSETTE_SLOT_SIZE ][ MAX_SDM_STYLE ]; // 2008.08.06
//------------------------------------------------------------------------------------------
int _i_SCH_SDM_Get_LOTFIRST_OPTION( int ch ) {
	return SDM_LOTFIRST_CHAMBER_OPTION[ ch ];
}
//------------------------------------------------------------------------------------------
int _i_SCH_SDM_Get_LOTEND_OPTION( int ch ) {
	return SDM_LOTEND_CHAMBER_OPTION[ ch ];
}
//------------------------------------------------------------------------------------------
char *_i_SCH_SDM_Get_RECIPE( int wh , int slot , int style , int mode ) {
	if ( SDM_RUN_RECIPE[wh][slot][style][mode] == NULL ) return COMMON_BLANK_CHAR; // 2008.04.02
	return SDM_RUN_RECIPE[wh][slot][style][mode];
}
//------------------------------------------------------------------------------------------
int _i_SCH_SDM_Get_RUN_CHAMBER( int slot ) {
	return SDM_RUN_CHAMBER[ slot ];
}
//------------------------------------------------------------------------------------------
int _i_SCH_SDM_Get_RUN_USECOUNT( int slot ) {
	return SDM_RUN_USECOUNT[ slot ][0];
}
//------------------------------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Save_SDM_ins_DATA() {
	int i , j , k , l;
	FILE *fpt;
	char Filename[256];
	char buffer[8];
	char buffer2[8];

	if ( SYSTEM_LOGSKIP_STATUS() ) return; // 2004.05.21

	sprintf( Filename , "%s/SchDummy.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( ( fpt = fopen( Filename , "w" ) ) == NULL ) return;
	//
	if ( SDM_EVENT_INCREASE ) { // 2015.01.30
		//
		fprintf( fpt , "%d\tEVENT_INCREASE\tON\n" , 0 );
		//
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( SDM_LOTFIRST_CHAMBER_OPTION[ i + PM1 ] != 0 ) {
			fprintf( fpt , "%d\tLOTFIRST\t%d\n" , i + 1 , SDM_LOTFIRST_CHAMBER_OPTION[ i + PM1 ] );
		}
		if ( SDM_LOTEND_CHAMBER_OPTION[ i + PM1 ] != 0 ) {
			fprintf( fpt , "%d\tLOTEND\t%d\n" , i + 1 , SDM_LOTEND_CHAMBER_OPTION[ i + PM1 ] );
		}
	}
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( 0 != SDM_RUN_CHAMBER[ i ] ) {
			fprintf( fpt , "%d\tMODE\t%d\n" , i + 1 , SDM_RUN_CHAMBER[ i ] - PM1 + 1 );
		}
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) {
			for ( k = 0 ; k < 3 ; k++ ) {
				for ( l = 0 ; l < 3 ; l++ ) {
					//------------------------------------------------------------
					if      ( k == 0 ) sprintf( buffer , "RUN" );
					else if ( k == 1 ) sprintf( buffer , "PST" );
					else if ( k == 2 ) sprintf( buffer , "PRE" );
					//
					if      ( l == 1 ) strcat( buffer , "F" );
					else if ( l == 2 ) strcat( buffer , "E" );
					//
					if ( j > 0 ) {
						sprintf( buffer2 , "%d" , j + 1 );
						strcat( buffer , buffer2 );
					}
					//------------------------------------------------------------
					if ( NULL != SDM_RUN_RECIPE[k][ i ][j][l] ) { // 2008.04.02
						if ( 0 != SDM_RUN_RECIPE[k][ i ][j][l][0] ) {
							fprintf( fpt , "%d\t%s\t%c%s%c\n" , i + 1 , buffer , '"' , SDM_RUN_RECIPE[k][ i ][j][l] , '"' );
						}
					}
					//------------------------------------------------------------
				}
			}
		}
		if ( 0 != SDM_RUN_USECOUNT[ i ][0] ) {
			fprintf( fpt , "%d\tCOUNT\t%d\n" , i + 1 , SDM_RUN_USECOUNT[ i ][0] );
		}
	}
	fclose( fpt );
}
//
void SCHEDULER_CONTROL_Read_SDM_ins_DATA() {
	HFILE hFile;
	char Buffer[256];
	char chr_return[64];
	char Filename[256];
	int  ReadCnt , i , j , k , l , sty;
	BOOL FileEnd = TRUE;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		SDM_LOTFIRST_CHAMBER_OPTION[ i ] = 0;
		SDM_LOTEND_CHAMBER_OPTION[ i ] = 0;
	}
	//
/*
// 2011.01.06
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		SDM_RUN_CHAMBER[ i ] = 0;
		IO_SDM_DUMMY_MODE( i , 0 );
		//
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) {
			for ( k = 0 ; k < 3 ; k++ ) {
				for ( l = 0 ; l < 3 ; l++ ) {
					//
					// Initialize
					//
//					strcpy( SDM_RUN_RECIPE[k][ i ][ j ][l] , "" ); // 2008.04.02
					SDM_RUN_RECIPE[k][ i ][ j ][l] = NULL; // 2008.04.02
					IO_SDM_DUMMY_RECIPE( k , i , j , l , "" );
					//
				}
			}
			SDM_RUN_USECOUNT[ i ][ j ] = 0; // 2008.08.06
			IO_SDM_DUMMY_COUNT( i , j , 0 ); // 2008.08.06
		}
//		SDM_RUN_USECOUNT[ i ] = 0; // 2008.08.06
//		IO_SDM_DUMMY_COUNT( i , 0 ); // 2008.08.06
	}
*/

// 2011.01.06
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		SDM_RUN_CHAMBER[ i ] = 0;
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) {
			for ( k = 0 ; k < 3 ; k++ ) {
				for ( l = 0 ; l < 3 ; l++ ) {
					SDM_RUN_RECIPE[k][ i ][ j ][l] = NULL; // 2008.04.02
				}
			}
			SDM_RUN_USECOUNT[ i ][ j ] = 0; // 2008.08.06
		}
	}
	//======
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( !IO_SDM_DUMMY_MODE( i , 0 ) ) break;
	}
	//======
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) {
			if ( !IO_SDM_DUMMY_COUNT( i , j , 0 ) ) break;
		}
		//
		if ( j != MAX_SDM_STYLE ) break;
	}
	//======
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) {
			//
			sty = 0;
			//
			for ( k = 0 ; k < 3 ; k++ ) {
				for ( l = 0 ; l < 3 ; l++ ) {
					if ( IO_SDM_DUMMY_RECIPE( k , i , j , l , "" ) ) sty = 1;
				}
			}
			//
			if ( sty == 0 ) break;
			//
		}
		//
		if ( j != MAX_SDM_STYLE ) break;
	}
	//======
	sprintf( Filename , "%s/SchDummy.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	hFile = _lopen( Filename , OF_READ );
	if ( hFile == -1 ) return;
	while ( FileEnd ) {
		//
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 250 , &ReadCnt ); // 2017.02.15
		//
		if ( ReadCnt > 2 ) {
			//
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) return;
			i = atoi( chr_return ) - 1;
			if ( !Get_Data_From_String( Buffer , chr_return , 1 ) ) return;
			//========================================================================
			// 2004.11.17
			//========================================================================
			j = strlen( chr_return );
			sty = 0;
			if ( ( chr_return[j-2] >= '1' ) && ( chr_return[j-2] <= '9' ) && ( chr_return[j-1] >= '0' ) && ( chr_return[j-1] <= '9' ) ) { // 2006.09.19
				sty = ( ( chr_return[j-2] - '0' ) * 10 ) + ( chr_return[j-1] - '0' ) - 1;
				if ( sty <  0             ) sty = 0;
				if ( sty >= MAX_SDM_STYLE ) sty = 0;
				chr_return[j-2] = 0;
			}
			else if ( ( chr_return[j-1] >= '1' ) && ( chr_return[j-1] <= '9' ) ) { // 2006.09.19
				sty = chr_return[j-1] - '0' - 1;
				if ( sty <  0             ) sty = 0;
				if ( sty >= MAX_SDM_STYLE ) sty = 0;
				chr_return[j-1] = 0;
			}
			//========================================================================
			if ( strcmp( chr_return , "EVENT_INCREASE" ) == 0 ) { // 2015.01.30
				//
				if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
				//
				if ( strcmp( chr_return , "ON" ) == 0 ) {
					SDM_EVENT_INCREASE = TRUE;
				}
				else {
					SDM_EVENT_INCREASE = FALSE;
				}
				//
			}
			else if ( strcmp( chr_return , "LOTFIRST" ) == 0 ) {
				if ( ( i >= 0 ) && ( i < MAX_PM_CHAMBER_DEPTH ) ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
					j = atoi( chr_return );
					if ( j != 0 ) {
						if ( j != SDM_LOTFIRST_CHAMBER_OPTION[ i + PM1 ] ) {
							SDM_LOTFIRST_CHAMBER_OPTION[ i + PM1 ] = j;
							IO_SDM_DUMMY_LOTFIRST( i + PM1 , j );
						}
					}
				}
			}
			else if ( strcmp( chr_return , "LOTEND" ) == 0 ) {
				if ( ( i >= 0 ) && ( i < MAX_PM_CHAMBER_DEPTH ) ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
					j = atoi( chr_return );
					if ( j != 0 ) {
						if ( j != SDM_LOTEND_CHAMBER_OPTION[ i + PM1 ] ) {
							SDM_LOTEND_CHAMBER_OPTION[ i + PM1 ] = j;
							IO_SDM_DUMMY_LOTEND( i + PM1 , j );
						}
					}
				}
			}
			else if ( strcmp( chr_return , "MODE" ) == 0 ) {
				if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
					j = atoi( chr_return );
					if ( j != 0 ) j = j + PM1 - 1;
					if ( j != SDM_RUN_CHAMBER[ i ] ) {
						SDM_RUN_CHAMBER[ i ] = j;
						IO_SDM_DUMMY_MODE( i , j );
					}
				}
			}
			else if ( strcmp( chr_return , "COUNT" ) == 0 ) {
				if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
					if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
					j = atoi( chr_return );
//					if ( j != SDM_RUN_USECOUNT[ i ] ) { // 2008.08.06
//						SDM_RUN_USECOUNT[ i ] = j; // 2008.08.06
//						IO_SDM_DUMMY_COUNT( i , j ); // 2008.08.06
//					}
					if ( j != SDM_RUN_USECOUNT[ i ][sty] ) { // 2008.08.06
						SDM_RUN_USECOUNT[ i ][sty] = j; // 2008.08.06
						IO_SDM_DUMMY_COUNT( i , sty , j ); // 2008.08.06
					}
				}
			}
//
			else {
				k = -1;
				//
				if ( strcmp( chr_return , "PRE" ) == 0 ) {
					k = 2;
					l = 0;
				}
				else if ( strcmp( chr_return , "RUN" ) == 0 ) {
					k = 0;
					l = 0;
				}
				else if ( strcmp( chr_return , "PST" ) == 0 ) { // 2004.11.17
					k = 1;
					l = 0;
				}
				else if ( strcmp( chr_return , "PREF" ) == 0 ) {
					k = 2;
					l = 1;
				}
				else if ( strcmp( chr_return , "RUNF" ) == 0 ) {
					k = 0;
					l = 1;
				}
				else if ( strcmp( chr_return , "PSTF" ) == 0 ) { // 2004.11.17
					k = 1;
					l = 1;
				}
				else if ( strcmp( chr_return , "PREE" ) == 0 ) {
					k = 2;
					l = 2;
				}
				else if ( strcmp( chr_return , "RUNE" ) == 0 ) {
					k = 0;
					l = 2;
				}
				else if ( strcmp( chr_return , "PSTE" ) == 0 ) { // 2004.11.17
					k = 1;
					l = 2;
				}
				//
				if ( k != -1 ) {
					if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
						if ( !Get_Data_From_String( Buffer , chr_return , 2 ) ) return;
						j = strlen( chr_return );
						chr_return[ j - 1 ] = 0;

//						if ( !STRCMP_L( SDM_RUN_RECIPE[k][ i ][sty][ l ]  , chr_return + 1 ) ) { // 2008.04.02
//							strcpy( SDM_RUN_RECIPE[k][ i ][sty][ l ]  , chr_return + 1 ); // 2008.04.02
//							IO_SDM_DUMMY_RECIPE( k , i , sty , l , chr_return + 1 ); // 2008.04.02
//						} // 2008.04.02
						if ( !STR_MEM_MAKE_COPY2( &(SDM_RUN_RECIPE[k][ i ][sty][ l ]) , chr_return + 1 ) ) { // 2008.04.02 // 2010.03.25
							_IO_CIM_PRINTF( "SCHEDULER_CONTROL_Read_SDM_ins_DATA Memory Allocate Error\n" );
						}
						IO_SDM_DUMMY_RECIPE( k , i , sty , l , chr_return + 1 ); // 2008.04.02
					}

				}
			}
//
		}
	}
	_lclose( hFile );
}


//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_LOTFIRST_MODULE( int ch , int data ) {
	if ( _i_SCH_SDM_Get_LOTFIRST_OPTION( ch ) != data ) {
		SDM_LOTFIRST_CHAMBER_OPTION[ ch ] = data;
		IO_SDM_DUMMY_LOTFIRST( ch , data );
		SCHEDULER_CONTROL_Save_SDM_ins_DATA();
	}
}
void SCHEDULER_CONTROL_Set_SDM_ins_LOTEND_MODULE( int ch , int data ) {
	if ( _i_SCH_SDM_Get_LOTEND_OPTION( ch ) != data ) {
		SDM_LOTEND_CHAMBER_OPTION[ ch ] = data;
		IO_SDM_DUMMY_LOTEND( ch , data );
		SCHEDULER_CONTROL_Save_SDM_ins_DATA();
	}
}
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_MODULE( int pointer , int data ) {
	if ( _i_SCH_SDM_Get_RUN_CHAMBER( pointer%100 ) != data ) {
		SDM_RUN_CHAMBER[ pointer%100 ] = data;
		IO_SDM_DUMMY_MODE( pointer%100 , data );
		SCHEDULER_CONTROL_Save_SDM_ins_DATA();
	}
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( int pointer , int style , int id , char *recipe ) {
//	strncpy( SDM_RUN_RECIPE[2][pointer%100][style][id] , recipe , 64 ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(SDM_RUN_RECIPE[2][pointer%100][style][id]) , recipe ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE Memory Allocate Error[%d,%d,%d]\n" , pointer , style , id );
	}
	IO_SDM_DUMMY_RECIPE( 2 , pointer%100 , style , id , _i_SCH_SDM_Get_RECIPE( 2 , pointer%100 , style , id ) );
	SCHEDULER_CONTROL_Save_SDM_ins_DATA();
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( int pointer , int style , int id , char *recipe ) {
//	strncpy( SDM_RUN_RECIPE[0][pointer%100][style][id] , recipe , 64 ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(SDM_RUN_RECIPE[0][pointer%100][style][id]) , recipe ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE Memory Allocate Error[%d,%d,%d]\n" , pointer , style , id );
	}
	IO_SDM_DUMMY_RECIPE( 0 , pointer%100 , style , id , _i_SCH_SDM_Get_RECIPE( 0 , pointer%100 , style , id ) );
	SCHEDULER_CONTROL_Save_SDM_ins_DATA();
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE( int pointer , int style , int id , char *recipe ) {
//	strncpy( SDM_RUN_RECIPE[1][pointer%100][style][id] , recipe , 64 ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(SDM_RUN_RECIPE[1][pointer%100][style][id]) , recipe ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE Memory Allocate Error[%d,%d,%d]\n" , pointer , style , id );
	}
	IO_SDM_DUMMY_RECIPE( 1 , pointer%100 , style , id , _i_SCH_SDM_Get_RECIPE( 1 , pointer%100 , style , id ) );
	SCHEDULER_CONTROL_Save_SDM_ins_DATA();
}
//------------------------------------------------------------------------------------------
void _i_SCH_SDM_Set_CHAMBER_RESET_COUNT( int pointer ) {
	int i;
// void SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RESET_COUNT( int pointer ) { // 2012.02.15
	if ( pointer == -1 ) { // 2013.02.15
		for ( i = 0 ; i <  MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_SDM_Get_RUN_USECOUNT( i ) != 0 ) {
				SDM_RUN_USECOUNT[ i ][0] = 0;
				IO_SDM_DUMMY_COUNT( i , 0 , _i_SCH_SDM_Get_RUN_USECOUNT( i ) );
			}
		}
		SCHEDULER_CONTROL_Save_SDM_ins_DATA();
	}
	else {
		if ( _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) != 0 ) {
			SDM_RUN_USECOUNT[ pointer%100 ][0] = 0;
			IO_SDM_DUMMY_COUNT( pointer%100 , 0 , _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) );
			SCHEDULER_CONTROL_Save_SDM_ins_DATA();
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
// 2015.01.30
void _i_SCH_SDM_Set_CHAMBER_INC_COUNT( int pointer ) {
	int c;
	c = _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) + 1;
	SDM_RUN_USECOUNT[ pointer%100 ][0] = c;
	IO_SDM_DUMMY_COUNT( pointer%100 , 0 , _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) );
	SCHEDULER_CONTROL_Save_SDM_ins_DATA();
}
*/
void _i_SCH_SDM_Set_CHAMBER_INC_COUNT_SUB( int pointer ) { // 2015.01.30
	int c;
	c = _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) + 1;
	SDM_RUN_USECOUNT[ pointer%100 ][0] = c;
	IO_SDM_DUMMY_COUNT( pointer%100 , 0 , _i_SCH_SDM_Get_RUN_USECOUNT( pointer%100 ) );
	SCHEDULER_CONTROL_Save_SDM_ins_DATA();
}

void _i_SCH_SDM_Set_CHAMBER_INC_COUNT( int pointer ) { // 2015.01.30
	if ( SDM_EVENT_INCREASE ) return;
	_i_SCH_SDM_Set_CHAMBER_INC_COUNT_SUB( pointer );
}

void _i_SCH_SDM_Set_MODE_INC_EVENT( int data ) { // 2015.01.30
	SDM_EVENT_INCREASE = ( data > 0 ) ? 1 : 0;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2015.07.30
//
void SCHEDULER_CONTROL_Pre_Inc( int side ) {
	int j , si;
	//
	if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) continue;
		//
		si = _i_SCH_CLUSTER_Get_SlotIn( side , j ) - 1;
		//
		if ( si >= 0 ) _i_SCH_SDM_Set_CHAMBER_INC_COUNT( si );
		//
	}
	//
}