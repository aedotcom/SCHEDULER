#include "default.h"

#include "EQ_Enum.h"

#include "IOLog.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "RcpFile_Handling.h"
#include "system_tag.h"
#include "Utility.h"
#include "MultiJob.h"

//=========================================================================================
//=========================================================================================
/*


	HDD(ORG)
	SHMEM(ORG)
	
	HDD(TMP)
	HDD(TMP2)

	SHMEM(TMP)
	SHMEM(TMP2)
	===================================================================================================

	"+:\"	=	ID

	<ORG - FILE>

		LOCKING		0					1							2							3

		JOBINSERT	X					X							HDD(ORG)->HDD(TMP)[2-A]		HDD(ORG)->SHMEM(TMP)[3-A]

		JOBSELECT
		+LOTSTART	X					HDD(ORG)->HDD(TMP2)[1-B]	HDD(TMP)->HDD(TMP2)[2-B]	SHMEM(TMP)->SHMEM(TMP2)[3-B]

		READ		HDD(ORG)[0-C]		HDD(TMP2)[1-C]				HDD(TMP2)[2-C]				SHMEM(TMP2)[3-C]

	<ORG - SHMEM : +:\ ·Î ½ÃÀÛ >

		LOCKING		0					1							2							3

		JOBINSERT	X					X							SHMEM(ORG)->HDD(TMP)[12-A]	SHMEM(ORG)->SHMEM(TMP)[13-A]

		JOBSELECT
		+LOTSTART	X					SHMEM(ORG)->HDD(TMP2)[11-B]	HDD(TMP)->HDD(TMP2)[12-B]	SHMEM(TMP)->SHMEM(TMP2)[13-B]

		READ		SHMEM(ORG)[10-C]	HDD(TMP2)[11-C]				HDD(TMP2)[12-C]				SHMEM(TMP2)[13-C]

*/
//=========================================================================================
//=========================================================================================
//
BOOL EQUIP_RUNNING_CHECK_SUB( BOOL prcsfunconly );
//
BOOL RECIPE_LOCKING_AUTO_DELETE_CHECK = FALSE; // 2013.09.13
//
int SCHEDULER_GET_RECIPE_LOCKING( int side ); // 2014.02.08

/*

// Using for Other App

void GET_REAL_PROCESS_RECIPE_FILE( int Locking , int Chamber , int Side , int WferID , char *Group , char *FileName , char *RealFileName ) {
	//
	char Buffer[256];
	//
	if ( Locking != 0 ) {
		if ( Locking == 2 ) {
			if ( WferID > 0 ) {
				sprintf( Buffer , "%d$%d$%s$%s$%s" , Side , WferID , Group , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
			}
			else {
				sprintf( Buffer , "%d$%s$%s$%s" , Side , Group , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
			}
		}
		else {
			sprintf( Buffer , "%d$%s$%s$%s" , Side , Group , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
		}
		//
		Change_Dirsep_to_Dollar( Buffer );
		//
		sprintf( RealFileName , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Buffer );
		//
	}
	else {
		strcpy( RealFileName , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) );
		if ( RealFileName != NULL ) strcat( RealFileName , "\\" );
		strcat( RealFileName , Group );
		if ( RealFileName != NULL ) strcat( RealFileName , "\\" );
		strcat( RealFileName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) );
		if ( RealFileName != NULL ) strcat( RealFileName , "\\" );
		strcat( RealFileName , FileName );
	}
}
*/
//=========================================================================================
//=========================================================================================
// style // 2003.09.24
//----+------+-------+-------+------
// id |Recipe|Using  |Dummy  | Fail
//----+------+-------+-------+------
// 0  | O    | O     |     O | Fail => default
//----+------+-------+-------+------
// 1  | O    | X     |     O | Fail => 300mm Append
// 2  | X    | X     |     O | Fail
// 3  | O    | X     |     O | OK
// 4  | X    | X     |     O | OK
//----+------+-------+-------+------

// style // 2012.06.28
//----+------+-------+-------+------
// id |LOT   |Using  |Dummy  | Fail
//    |Recipe|       |       |
//----+------+-------+-------+------
// 0  | O    | O     |     O | Fail => default
//----+------+-------+-------+------
// 1  | O    | X     |     O | Fail
// 2  | O    | X     |     O | SKIP(OK)
//----+------+-------+-------+------


//
//
// 2016.12.10
//
//
int   LOT_MAP_USE[MAX_CASSETTE_SLOT_SIZE];
char *LOT_MAP_DATA[MAX_CASSETTE_SLOT_SIZE];

void RECIPE_FILE_LOT_DATA_READ_FOR_MAP_INIT() {
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		LOT_MAP_DATA[i] = NULL;
	}
	//
}
/*
void RECIPE_FILE_LOT_DATA_READ_FOR_MAP_PRINTF() {
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( LOT_MAP_USE[i] == 0 ) continue;
		//
		printf( "[%d] [%d] [%s]\n" , i , LOT_MAP_USE[i] , LOT_MAP_DATA[i] );
	}
	//
}
*/
int RECIPE_FILE_GET_MAP_CLUSTERINDEX( int slot ) {
	//
	if ( ( slot >= 1 ) && ( slot <= MAX_CASSETTE_SLOT_SIZE ) ) return LOT_MAP_USE[slot-1] % 1000;
	//
	return 0;
}


BOOL RECIPE_FILE_LOT_DATA_READ_FOR_MAP( int side , char *FileName , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate3 *LOT_RECIPE_SPECIAL ) {
	/*
	T	30

	1	P1	P3	P5	P7	P9	P11	P13

	2	P1	P3	P5	P7	P9	P11	P13
	-	P1	P3	P5	P7	P9	P11	P13

	3	P1	P3	P5	P7	P9	P11	P13

	4	C:3:"test"
	//

	=>  string format per slot = 1:P1:30 1:P2:30 C:3:"test"

	*/
	HFILE hFile;
	BOOL FileEnd = TRUE;
	int  ReadCnt , Line;
	int  lastslot , lptime , okc;
	int  s , l , m , x , pmindex , depth , prtime , docnt , mapuse;
	char c , Buffer[2048] , BufferA[1024] , Buffer2[512] , Buf_rec1[512] , Buf_rec2[512] , Temp[512] , chr_return[512];

	//
	okc = 0;
	//
	lptime = 0;
	//
	for ( l = 0 ; l < MAX_CASSETTE_SLOT_SIZE ; l++ ) LOT_MAP_USE[l] = 0;
	//
	//-----------------------------------------------------------------------------
	lastslot = -1;
	//-----------------------------------------------------------------------------
	//
	hFile = _lopen( FileName , OF_READ );
	//
	if ( hFile == -1 ) return FALSE;
	//
	for ( Line = 1 ; FileEnd ; Line++ ) {
		//
//		FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt ); // 2017.02.15
		FileEnd = H_Get_Line_String_From_File2( hFile , Buffer , 2040 , &ReadCnt ); // 2017.02.15
		//
		if ( ReadCnt > 0 ) {
			//
			if ( !Get_Data_From_String( Buffer , chr_return , 0 ) ) {
				_lclose( hFile );
				return FALSE;
			}
			//
			if      ( STRCMP_L( chr_return , "T" ) ) {
				//			
				if ( Get_Data_From_String( Buffer , chr_return , 1 ) ) {
					lptime = atoi( chr_return );
				}
				//
			}
			else {
				//
				if ( STRCMP_L( chr_return , "A" ) || STRCMP_L( chr_return , "*A" )  || STRCMP_L( chr_return , "A*" ) ) {
					//
					s = -9999;
					//
					if ( STRCMP_L( chr_return , "A" ) ) {
						m = 0;
					}
					else {
						m = 1;
					}
				}
				else {
					if ( STRCMP_L( chr_return , "-" ) ) {
						s = lastslot;
						m = -1;
					}
					else {
						//
						l = strlen( chr_return );
						//
						if ( l <= 0 ) {
							s = -1;
							m = -1;
						}
						else {
							if ( chr_return[0] == '*' ) {
								s = atoi( chr_return + 1 );
								m = 1;
							}
							else if ( chr_return[l-1] == '*' ) {
								chr_return[l-1] = 0;
								s = atoi( chr_return );
								m = 1;
							}
							else {
								s = atoi( chr_return );
								m = 0;
							}
						}
					}
				}
				//
				//---------------------------------------------
				//
				if ( ( s == -9999 ) || ( ( s >= 1 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) ) {
					//
					if ( s == -9999 ) {
						//
						depth = 0;
						//
						mapuse = 0;
						//
					}
					else {
						//
						lastslot = s;
						//
						depth = LOT_MAP_USE[s-1] / 1000;
						//
						mapuse = LOT_MAP_USE[s-1];
						//
						if ( LOT_MAP_DATA[ s-1 ] == NULL ) mapuse = 0;
						//
					}
					//
					docnt = 0;
					//
					strcpy( Buffer2 , "" );
					//
					for ( l = 1 ; l < ReadCnt ; l++ ) {
						//
						if ( Get_Data_From_String( Buffer , chr_return , l ) ) {
							//
							Extract_Reset_String( chr_return );
							//
							if        ( toupper( chr_return[0] ) == 'P' ) {
								c = 'P';
							}
							else if   ( toupper( chr_return[0] ) == 'T' ) {
								c = 'T';
							}
							else if   ( toupper( chr_return[0] ) == 'R' ) {
								c = 'R';
							}
							else if   ( toupper( chr_return[0] ) == 'C' ) {
								c = 'C';
							}
							else {
								c = 0;
							}
							//
							if ( c == 'C' ) {
								//
								if ( chr_return[1] == ':' ) {
									//
									STR_SEPERATE_CHAR( chr_return + 2 , ':' , Buf_rec1 , Buf_rec2 , 511 );
									//
									if ( s == -9999 ) {
										for ( x = 0 ; x < MAX_CASSETTE_SLOT_SIZE ; x++ ) {
											//
											LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[x] = atoi( Buf_rec1 );
											//
											strcpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[x] , Buf_rec2 );
											//
										}
									}
									else {
										//
										LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[s-1] = atoi( Buf_rec1 );
										//
										strcpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[s-1] , Buf_rec2 );
										//
									}
								}
							}
							else if ( c != 0 ) {
								//
								STR_SEPERATE_CHAR( chr_return + 1 , ':' , Buf_rec1 , Buf_rec2 , 511 );
								//
								pmindex = atoi( Buf_rec1 ) - 1;
								//
								if ( ( pmindex >= 0 ) && ( pmindex < MAX_PM_CHAMBER_DEPTH ) ) {
									if ( Buf_rec2[0] == 0 ) {
										prtime = lptime;
									}
									else {
										prtime = atoi( Buf_rec2 );
									}
									//
									if ( ( mapuse == 0 ) && ( docnt == 0 ) ) {
										sprintf( Temp , "%d:%c%d:T*%d" , depth + 1 , c , pmindex + 1 , prtime );
									}
									else {
										sprintf( Temp , " %d:%c%d:T*%d" , depth + 1 , c , pmindex + 1 , prtime );
									}
									//
									docnt++;
									//
									strcat( Buffer2 , Temp );
									//
								}
								//
							}
						}
					}
					//
					if ( Buffer2[0] != 0 ) {
						//
						okc++;
						//
						if ( docnt > 0 ) docnt = 1;
						//
						for ( x = 0 ; x < MAX_CASSETTE_SLOT_SIZE ; x++ ) {
							//
							if ( ( s != -9999 ) && ( x != (s-1) ) ) continue;
							//
							if ( mapuse == 0 ) {
								//
								LOT_MAP_USE[x] = ( docnt * 1000 ) + ( ( m == 1 ) ? 2 : 1 );
								//
								STR_MEM_MAKE_COPY2( &(LOT_MAP_DATA[x]) , Buffer2 );
								//
							}
							else {
								//
								switch( LOT_MAP_USE[x] % 1000 ) {
								case 1 :
									LOT_MAP_USE[x] = ( ( ( LOT_MAP_USE[x] / 1000 ) + docnt ) * 1000 ) + ( ( m == 1 ) ? 2 : 1 );
									break;
								case 2 :
									LOT_MAP_USE[x] = ( ( ( LOT_MAP_USE[x] / 1000 ) + docnt ) * 1000 ) + 2;
									break;
								default :
									LOT_MAP_USE[x] = ( ( ( LOT_MAP_USE[x] / 1000 ) + docnt ) * 1000 ) + ( ( m == 1 ) ? 2 : 1 );
									break;
								}
								//
								sprintf( BufferA , "%s%s" , LOT_MAP_DATA[x] , Buffer2 );
								STR_MEM_MAKE_COPY2( &(LOT_MAP_DATA[x]) , BufferA );
								//
							}
							//
						}
						//
					}
				}
			}
		}
	}
	_lclose( hFile );
	//
	for ( l = 0 ; l < MAX_CASSETTE_SLOT_SIZE ; l++ ) {
		//
		LOT_RECIPE->CLUSTER_USING[l] = ( LOT_MAP_USE[l] == 0 ) ? 1 : 0;
		//
		if ( LOT_RECIPE->CLUSTER_USING[l] == 1 ) {
			if ( LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[l] > 0 ) {
				LOT_RECIPE->CLUSTER_USING[l] = 0;
			}
		}
		//
	}
	//
//	RECIPE_FILE_LOT_DATA_READ_FOR_MAP_PRINTF();

	return ( okc > 0 );

}

//BOOL RECIPE_FILE_LOT_DATA_READ( int side , int style , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , char *grp , int *lsp , char *FileName , int slot , char *pjname ) { // 2015.10.12
//BOOL RECIPE_FILE_LOT_DATA_READ( int side , int style , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , char *grp , int *lsp , char *FileName , int slot , char *pjname , BOOL *hasSlotFix ) { // 2015.10.12 // 2016.07.13
//BOOL RECIPE_FILE_LOT_DATA_READ( int side , int style , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , LOTStepTemplate_Dummy_Ex *LOT_RECIPE_DUMMY_EX , char *grp , int *lsp , char *FileName , int slot , char *pjname , BOOL *hasSlotFix ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
BOOL RECIPE_FILE_LOT_DATA_READ( BOOL mapuse , BOOL *mapmode , int side , int style , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , LOTStepTemplate_Dummy_Ex *LOT_RECIPE_DUMMY_EX , char *grp , int *lsp , char *FileName , int slot , char *pjname , BOOL *hasSlotFix ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
	char RecipeFileName[256];
	HFILE hFile;
	BOOL StepEndFind;
	int		kill;
	int i;
	char  buffer[256];
	char  buffer2[16];
	
	char  cc_return;
	int   ii_return;
	float ff_return;
	char  str_return[1024+1];
	char  name_return[1024+1];
	char  type_return;

	char  Buf_rec1[256]; // 2004.09.24
	char  Buf_rec2[256]; // 2004.09.24

	int FileName_SHPT; // 2015.07.20
	//
	*hasSlotFix = FALSE; // 2015.10.12
	//
	//------------------------------------------------------
	// Initial
	//------------------------------------------------------
	strcpy( LOT_RECIPE->LOT_ID , "" );

	//------------------------------------------------------
	if ( style == 0 ) { // 2003.09.24 // 2004.09.24
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			strcpy( LOT_RECIPE->CLUSTER_RECIPE[i] , "" );
			strcpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , "" );
		}
	}
	//------------------------------------------------------
	if ( style == 0 ) { // 2003.09.24
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			LOT_RECIPE->CLUSTER_USING[i] = 0;
			//
			LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = 0; // 2004.11.16
			//
			*lsp = 0; // 2005.07.18
			//
			strcpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[i] , "" ); // 2005.01.24
			//
			LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[i] = 0; // 2011.12.08
			strcpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[i] , "" ); // 2011.12.08
			//
			LOT_RECIPE_SPECIAL->USER_SLOTFIX_MODE[i] = 0; // 2015.10.12
			LOT_RECIPE_SPECIAL->USER_SLOTFIX_SLOT[i] = 0; // 2015.10.12
			//
		}
	}
	//------------------------------------------------------
	LOT_RECIPE->CLUSTER_EXTRA_STATION = 0;
	LOT_RECIPE->CLUSTER_EXTRA_TIME    = 0;
	//------------------------------------------------------
	strcpy( LOT_RECIPE_DUMMY->CLUSTER_RECIPE , "" );
	LOT_RECIPE_DUMMY->CLUSTER_USING = 0;
	//------------------------------------------------------
	//
	// 2016.07.13
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		LOT_RECIPE_DUMMY_EX->CLUSTER_USING[i] = 0;
		//
		strcpy( LOT_RECIPE_DUMMY_EX->CLUSTER_RECIPE[i] , "" );
		//
	}
	//
	//------------------------------------------------------
	if ( FileName == NULL ) {
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		return FALSE;
	}
	if ( strcmp( FileName , "????" ) == 0 ) {
		//===============================================================================
		// 2006.11.09
		//===============================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			strcpy( LOT_RECIPE->CLUSTER_RECIPE[i] , "????" );
		}
		//===============================================================================
		return TRUE;
	}
	//
	// 2015.07.20
	FileName_SHPT = 0;
	if ( ( FileName[0] == '+' ) && ( ( FileName[1] == '\\' ) || ( FileName[1] == '/' ) ) )  FileName_SHPT = 2; // 2015.07.20
	//
	strcpy( Buf_rec1 , "" ); // 2004.09.24
	strcpy( Buf_rec2 , "" ); // 2004.09.24

	//============================================================================================================================
	// 2004.11.16
	//============================================================================================================================
	STR_SEPERATE_CHAR( FileName , ':' , Buf_rec1 , Buf_rec2 , 255 );
	i = strlen( Buf_rec2 );
	if ( i > 0 ) {
		if ( ( Buf_rec2[0] == '(' ) && ( Buf_rec2[i-1] == ')' ) ) {
			 Buf_rec2[i-1] = 0;
			i = 1;
		}
		else {
			i = 0;
		}
	}
	//============================================================================================================================
//	if ( ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08
//	if ( ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08 // 2015.07.20
	if ( ( SCHEDULER_GET_RECIPE_LOCKING( side ) >= 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08 // 2015.07.20
		if ( i == 1 ) {
			//
			SCHMULTI_RCPLOCKING_GET_FILENAME( 0 , 1 , pjname , Buf_rec2 + 1 , Buf_rec1 + FileName_SHPT , RecipeFileName , 255 );
			//
		}
		else {
			//
			if ( side == -1 ) { // 2011.09.20
				//
				SCHMULTI_RCPLOCKING_GET_FILENAME( 0 , 1 , pjname , SCHMULTI_DEFAULT_GROUP_GET() , FileName + FileName_SHPT , RecipeFileName , 255 );
				//
			}
			else {
				//
				SCHMULTI_RCPLOCKING_GET_FILENAME( 0 , 1 , pjname , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , FileName + FileName_SHPT , RecipeFileName , 255 );
				//
			}
			//
		}
		//
		// 2015.07.20
		//
		if ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 2 ) {
			FileName_SHPT = 0;
		}
		else {
			FileName_SHPT = 1;
		}
		//
	}
	else {
		if ( i == 1 ) { // 2004.11.16
			if ( Buf_rec1[FileName_SHPT] == '*' ) { // 2016.12.10
				strcpy( RecipeFileName , Buf_rec1 + FileName_SHPT + 1 );
			}
			else {
				strcpy( RecipeFileName , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				strcat( RecipeFileName , Buf_rec2 + 1 );
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				strcat( RecipeFileName , _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() );
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				strcat( RecipeFileName , Buf_rec1 + FileName_SHPT );
			}
		}
		else {
			if ( FileName[FileName_SHPT] == '*' ) { // 2016.12.10
				strcpy( RecipeFileName , FileName + FileName_SHPT + 1 );
			}
			else {
				strcpy( RecipeFileName , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				//
				if ( side == -1 ) { // 2011.09.20
					strcat( RecipeFileName , grp );
				}
				else {
					strcat( RecipeFileName , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) );
				}
				//
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				strcat( RecipeFileName , _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() );
				if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
				strcat( RecipeFileName , FileName + FileName_SHPT );
			}
		}
		//
	}
	//============================================================================================================================
	//
	read_mode_file_set( ( FileName_SHPT > 0 ) ? 2 : 0 ); // 2015.07.20 // default , file , shmem
	//
	hFile = read_open_file( RecipeFileName );
	//
	if ( hFile < 0 ) {
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		return FALSE;
	}

	strcpy( Buf_rec1 , "" ); // 2004.09.24
	strcpy( Buf_rec2 , "" ); // 2004.09.24

	// INDICATOR
	if ( !read_form( hFile , ""         , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		read_close_file( hFile );
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		//
		if ( mapuse ) { // 2016.12.10
			if ( RECIPE_FILE_LOT_DATA_READ_FOR_MAP( side , RecipeFileName , LOT_RECIPE , LOT_RECIPE_SPECIAL ) ) {
				*mapmode = TRUE;
				return TRUE;
			}
		}
		//
		return FALSE;
	}
	if ( !read_form( hFile , ""         , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		read_close_file( hFile );
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		//
		if ( mapuse ) { // 2016.12.10
			if ( RECIPE_FILE_LOT_DATA_READ_FOR_MAP( side , RecipeFileName , LOT_RECIPE , LOT_RECIPE_SPECIAL ) ) {
				*mapmode = TRUE;
				return TRUE;
			}
		}
		//
		return FALSE;
	}

	if ( !read_form_name_until( hFile , "__&&LOT_RECIPE_BEGIN&&__" ) ) { // 2007.09.18
		read_close_file( hFile );
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		return FALSE;
	}

/*
// 2007.09.18
	if ( !read_form( hFile , "__&&LOT_RECIPE_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_lclose( hFile );
//		if ( style >= 3 ) return TRUE; // 2012.06.28
		if ( style >= 2 ) return TRUE; // 2012.06.28
		return FALSE;
	}
*/

	StepEndFind = FALSE;
	kill = 0;
	while( !StepEndFind ) {
		if ( !read_return_form( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , str_return ) ) { read_close_file( hFile ); return FALSE; }
		switch ( type_return ) {
			case NAME_FORMAT :
								if ( STRCMP_L( name_return , "__&&LOT_RECIPE_END&&__" ) ) StepEndFind = TRUE;
								else	kill++;
								break;
			case INT_FORMAT  :	
								if      ( STRCMP_L( name_return , "CLUSTER_EXTRA_STATION" ) ) {
									LOT_RECIPE->CLUSTER_EXTRA_STATION= ii_return;
								}
								else if ( STRCMP_L( name_return , "CLUSTER_EXTRA_TIME"	) )	{
									LOT_RECIPE->CLUSTER_EXTRA_TIME	= ii_return;
								}
//								else if ( STRCMP_L( name_return , "CLUSTER_DUMMY_USING"	) )	{ // 2016.07.13
//									LOT_RECIPE_DUMMY->CLUSTER_USING	= ii_return;
//								}
								else if ( STRNCMP_L( name_return , "CLUSTER_DUMMY_USING" , 19 ) )	{ // 2016.07.13
									if ( name_return[19] == 0 ) {
										LOT_RECIPE_DUMMY->CLUSTER_USING	= ii_return;
									}
									else {
										i = atoi( name_return + 19 ) - 1;
										//
										if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
											LOT_RECIPE_DUMMY_EX->CLUSTER_USING[i] = ii_return;
										}
										//
									}
								}
								else if ( STRCMP_L( name_return , "SPECIAL_INSIDE_DATA"	) )	{ // 2004.11.16
//									if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
									//
									if ( ii_return >= 0 ) { // 2014.06.23
										//
										*lsp = ii_return; // 2005.07.18
										//
										if ( ( style == 0 ) || ( slot == -1 ) ) {
											for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
//												LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = ii_return; // 2014.06.23
												LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = ii_return+1; // 2014.06.23
											}
										}
										else {
//											LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[slot-1] = ii_return; // 2014.06.23
											LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[slot-1] = ii_return+1; // 2014.06.23
										}
									}
									//
//									}
								} // 2004.11.16
								else if ( STRNCMP_L( name_return , "SPECIAL_INSIDE_DATA" , 19 ) )	{ // 2012.04.21
									//
//									if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
									//
									if ( ii_return >= 0 ) { // 2014.06.23
										//
										i = atoi( name_return + 19 ) - 1;
										//
										if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
//											LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = ii_return; // 2014.06.23
											LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = ii_return+1; // 2014.06.23
										}
										//
									}
									//
//									}
								}
								else if ( STRNCMP_L( name_return , "USER_CONTROL_MODE" , 17 ) )	{ // 2011.12.08
									if ( name_return[17] == 0 ) {
//									if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
										//
										if ( ( style == 0 ) || ( slot == -1 ) ) {
											for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
												LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[i] = ii_return;
											}
										}
										else {
											LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[slot-1] = ii_return;
										}
//									}
									}
									else { // 2012.07.22
//										if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
											strncpy( buffer , name_return , 255 );
											buffer2[0] = name_return[17];
											buffer2[1] = name_return[18];
											buffer2[2] = 0;
											i = atoi( buffer2 ) - 1;
											if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
												if ( ( style == 0 ) || ( slot == -1 ) ) {
													LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[i] = ii_return;
												}
												else {
													if ( slot == ( i + 1 ) ) {
														LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[i] = ii_return;
													}
												}
											}
//										}

									}
								} // 2011.12.08
								else {
									//if ( style == 0 ) { // 2003.09.24 // 2004.09.24
//									if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
										strncpy( buffer , name_return , 255 );
										buffer2[0] = name_return[8];
										buffer2[1] = name_return[9];
										buffer2[2] = 0;
										i = atoi( buffer2 ) - 1;
										if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
											name_return[8] = ' ';
											name_return[9] = ' ';
											//
											if ( style == 0 ) { // 2004.09.24
												//
												if ( STRCMP_L( name_return , "CLUSTER_  _USING" ) ) {
													LOT_RECIPE->CLUSTER_USING[i]	= ii_return;
												}
												//
												else if ( STRCMP_L( name_return , "CLUSTER_  _SLOTFIX_MODE" ) ) { // 2015.10.12
													//
													LOT_RECIPE_SPECIAL->USER_SLOTFIX_MODE[i]	= ii_return; // 2015.10.12
													//
													if ( ii_return > 0 ) *hasSlotFix = TRUE; // 2015.10.12
													//
												}
												else if ( STRCMP_L( name_return , "CLUSTER_  _SLOTFIX_SLOT" ) ) { // 2015.10.12
													LOT_RECIPE_SPECIAL->USER_SLOTFIX_SLOT[i]	= ii_return; // 2015.10.12
												}
												//
												else {
													kill++;
												}
											}
											else if ( slot == -1 ) { // 2004.10.06
												if ( STRCMP_L( name_return , "CLUSTER_  _USING" ) ) {
													if ( ii_return == 1 ) {
														LOT_RECIPE->CLUSTER_USING[i]	= 1;
													}
												}
											}
											else { // 2004.09.24
												if ( slot == ( i + 1 ) ) {
													if ( STRCMP_L( name_return , "CLUSTER_  _USING" ) ) {
														if ( ii_return == 1 ) {
															LOT_RECIPE->CLUSTER_USING[i]	= 1;
														}
													}
													else
														kill++;
												}
											}
										}
										else {
											kill++;
										}
//									}
								}
								break;

			case FLOAT_FORMAT :
			case DOUBLE_FORMAT :
								kill++;
								break;

			case STRING_FORMAT :
								if ( STRCMP_L( name_return , "LOT_ID" ) ) {
									strncpy( LOT_RECIPE->LOT_ID , str_return , 255 );
								}
//								else if ( STRCMP_L( name_return , "CLUSTER_DUMMY_RECIPE" ) ) { // 2016.07.13
//									strncpy( LOT_RECIPE_DUMMY->CLUSTER_RECIPE , str_return , 64 );
//								}
								else if ( STRNCMP_L( name_return , "CLUSTER_DUMMY_RECIPE" , 20 ) ) { // 2016.07.13
									if ( name_return[20] == 0 ) {
										strncpy( LOT_RECIPE_DUMMY->CLUSTER_RECIPE , str_return , 64 );
									}
									else {
										i = atoi( name_return + 20 ) - 1;
										//
										if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
											strncpy( LOT_RECIPE_DUMMY_EX->CLUSTER_RECIPE[i] , str_return , 64 );
										}
										//
									}
								}
								else if ( STRNCMP_L( name_return , "SPECIAL_USER_DATA" , 17 ) )	{ // 2005.01.24
									if ( name_return[17] == 0 ) {
//										if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
											if ( ( style == 0 ) || ( slot == -1 ) ) {
												for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
													strncpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[i] , str_return , 1024 );
												}
											}
											else {
												strncpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[slot-1] , str_return , 1024 );
											}
//										}
									}
									else {
//										if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
											strncpy( buffer , name_return , 255 );
											buffer2[0] = name_return[17];
											buffer2[1] = name_return[18];
											buffer2[2] = 0;
											i = atoi( buffer2 ) - 1;
											if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
												if ( ( style == 0 ) || ( slot == -1 ) ) {
													strncpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[i] , str_return , 1024 );
												}
												else {
													if ( slot == ( i + 1 ) ) {
														strncpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[i] , str_return , 1024 );
													}
												}
											}
//										}
									}
								} // 2005.01.24
								else if ( STRNCMP_L( name_return , "USER_CONTROL_DATA" , 17 ) )	{ // 2011.12.08
									if ( name_return[17] == 0 ) {
//										if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
											if ( ( style == 0 ) || ( slot == -1 ) ) {
												for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
													strncpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[i] , str_return , 256 );
												}
											}
											else {
												strncpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[slot-1] , str_return , 256 );
											}
//										}
									}
									else {
//										if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
											strncpy( buffer , name_return , 255 );
											buffer2[0] = name_return[17];
											buffer2[1] = name_return[18];
											buffer2[2] = 0;
											i = atoi( buffer2 ) - 1;
											if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
												if ( ( style == 0 ) || ( slot == -1 ) ) {
													strncpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[i] , str_return , 256 );
												}
												else {
													if ( slot == ( i + 1 ) ) {
														strncpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[i] , str_return , 256 );
													}
												}
											}
//										}
									}
								} // 2011.12.08
								else {
//									if ( ( style == 0 ) || ( style == 1 ) || ( style == 3 ) ) { // 2012.06.28
										strncpy( buffer , name_return , 255 );
										buffer2[0] = name_return[8];
										buffer2[1] = name_return[9];
										buffer2[2] = 0;
										i = atoi( buffer2 ) - 1;
										if ( ( i >= 0 ) && ( i < MAX_CASSETTE_SLOT_SIZE ) ) {
											if ( ( style == 0 ) || ( slot == -1 ) ) { // 2004.09.24
												name_return[8] = ' ';
												name_return[9] = ' ';
												if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE" ) )
													strncpy( LOT_RECIPE->CLUSTER_RECIPE[i] , str_return , 64 );
												else if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE2" ) )
													strncpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , str_return , 64 );
												else
													kill++;
											}
											else { // 2004.09.24
												if ( slot == ( i + 1 ) ) {
													name_return[8] = ' ';
													name_return[9] = ' ';
													if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE" ) ) {
														if ( strlen( str_return ) > 0 ) {
															strncpy( LOT_RECIPE->CLUSTER_RECIPE[i] , str_return , 64 );
														}
														else {
															strncpy( LOT_RECIPE->CLUSTER_RECIPE[i] , Buf_rec1 , 64 );
														}
													}
													else if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE2" ) ) {
														if ( strlen( str_return ) > 0 ) {
															strncpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , str_return , 64 );
														}
														else {
															strncpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , Buf_rec2 , 64 );
														}
													}
													else {
														kill++;
													}
												}
												else {
													if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE" ) ) {
														if ( strlen( str_return ) > 0 ) {
															strncpy( Buf_rec1 , str_return , 64 );
														}
													}
													else if ( STRCMP_L( name_return , "CLUSTER_  _RECIPE2" ) ) {
														if ( strlen( str_return ) > 0 ) {
															strncpy( Buf_rec2 , str_return , 64 );
														}
													}
												}
											}
										}
										else {
											kill++;
										}
//									}
								}
								break;
			default :			kill++;
								break;
		}
		if ( kill >= 100 )	{
			read_close_file( hFile );
//			if ( style >= 3 ) return TRUE; // 2012.06.28
			if ( style >= 2 ) return TRUE; // 2012.06.28
			return FALSE;
		}
	}
	read_close_file( hFile );

	if ( ( style == 0 ) || ( slot == -1 ) ) { // 2003.09.24 // 2004.09.24
		for ( i = 1 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( strcmp( LOT_RECIPE->CLUSTER_RECIPE[i]  , "" ) == 0 ) strcpy( LOT_RECIPE->CLUSTER_RECIPE[i]  , LOT_RECIPE->CLUSTER_RECIPE[i-1] );
			if ( strcmp( LOT_RECIPE->CLUSTER_RECIPE2[i] , "" ) == 0 ) strcpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , LOT_RECIPE->CLUSTER_RECIPE2[i-1] );
		}
	}
	//
	if ( strcmp( LOT_RECIPE_DUMMY->CLUSTER_RECIPE , "" ) == 0 ) LOT_RECIPE_DUMMY->CLUSTER_USING = 0;
	return TRUE;
}
//=========================================================================================
/*
// 2015.04.09
BOOL RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( int side , char *grpname , char *DirName , char *datastr , char *DirName2 , char *FileName , int cnt , char **target ) {
	int len;
	UTIL_EXTRACT_GROUP_FILE( DirName , datastr , DirName2 , cnt , FileName , cnt );
	//
	if ( grpname != NULL ) strcpy( DirName2 , grpname ); // 2009.06.16
	//
	if ( *target != NULL ) free( *target );
	//
	if ( FileName[0] == 0 ) {
		*target = NULL;
		return TRUE;
	}
	else {
		if ( !STRCMP_L( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , DirName2 ) ) {
			strncat( FileName , ":(" , cnt );
			strncat( FileName , DirName2 , cnt );
			strncat( FileName , ")" , cnt );
		}
		len = (signed) strlen( FileName );
		if ( len <= 0 ) {
			*target = NULL;
			return TRUE;
		}
		*target = calloc( len + 1 , sizeof( char ) );
		if ( *target == NULL ) {
			_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP Memory Allocate Error\n" );
			return FALSE;
		}
		strncpy( *target , FileName , len );
	}
	return TRUE;
}
*/

//
// 2015.04.09
//
BOOL RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( int side , char *grpname , char *DirName , char *datastr , char *DirName2 , char *FileName , int cnt , char **target ) {
//	int len; // 2015.04.09
	unsigned int len; // 2015.04.09
	//
	if ( side != -2 ) { // 2016.12.22
		//
		UTIL_EXTRACT_GROUP_FILE( DirName , datastr , DirName2 , cnt , FileName , cnt );
		//
		if ( grpname != NULL ) strcpy( DirName2 , grpname ); // 2009.06.16
	}
	//
	if ( FileName[0] == 0 ) {
		if ( *target != NULL ) strcpy( *target , "" );
		return TRUE;
	}
	else {
		//
		if ( side != -2 ) { // 2016.12.22
			//
			if ( side != -1 ) { // 2016.12.10
				if ( !STRCMP_L( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , DirName2 ) ) {
					strncat( FileName , ":(" , cnt );
					strncat( FileName , DirName2 , cnt );
					strncat( FileName , ")" , cnt );
				}
			}
			else { // 2016.12.22
				if ( DirName2[0] != 0 ) {
					strncat( FileName , ":(" , cnt );
					strncat( FileName , DirName2 , cnt );
					strncat( FileName , ")" , cnt );
				}
			}
			//
		}
		//
//		len = (signed) strlen( FileName ); // 2015.04.09
		len = strlen( FileName ); // 2015.04.09
		//
		if ( *target != NULL ) {
			//
			if ( len <= (_msize( *target )-1) ) {
				//
//				strncpy( *target , FileName , len ); // 2015.04.28
				strcpy( *target , FileName ); // 2015.04.28
				//
				return TRUE;
				//
			}
			//
			free( *target );
		}
		//
		if ( len <= 0 ) {
			*target = NULL;
			return TRUE;
		}
		//
		*target = calloc( len + 1 , sizeof( char ) );
		//
		if ( *target == NULL ) {
			_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP Memory Allocate Error\n" );
			return FALSE;
		}
		strncpy( *target , FileName , len );
	}
	return TRUE;
}

//=========================================================================================
BOOL read_return_form_MaxLimit( BOOL shmem , HFILE hFilePointer , char *ReturnNameIndicator , char *type_return , char *cc_return , int *ii_return , float *ff_return , double *dd_return , char *str_return ) {
	char  cc;
	int   ii;
	char  Reader[ 256 ];
	int   wbyte;

	if ( shmem ) {
		wbyte = _FILE2SM_READ( hFilePointer , Reader , strlen(BEGIN_DELEMETER) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		Reader[wbyte] = 0x00;
		if ( strcmp( Reader , BEGIN_DELEMETER ) != 0 ) return( FALSE );

		wbyte = _FILE2SM_READ( hFilePointer , &ii , sizeof(int) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		wbyte = _FILE2SM_READ( hFilePointer , Reader , ii );          Reader[wbyte] = 0x00;
		strcpy( ReturnNameIndicator , Reader );

		wbyte = _FILE2SM_READ( hFilePointer , Reader , sizeof(char) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		memcpy( &cc , Reader , sizeof(char) );
	//	if ( cc < NAME_FORMAT || cc > FILENAME_FORMAT ) return ( FALSE ); // 2006.05.15
	//	if ( cc < NAME_FORMAT || cc > FILENAME_X_FORMAT ) return ( FALSE ); // 2006.05.15 // 2008.05.30
		if ( cc < NAME_FORMAT || cc > DOUBLE_X_FORMAT ) return ( FALSE ); // 2006.05.15 // 2008.05.30
		*type_return = cc;

		switch ( cc ) {
			case NAME_FORMAT   :
			case NAME_X_FORMAT   :
									break;
			case CHAR_FORMAT   :
			case CHAR_X_FORMAT   :
									wbyte = _FILE2SM_READ( hFilePointer , cc_return , sizeof(char) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case INT_FORMAT    :
			case INT_X_FORMAT    :
									wbyte = _FILE2SM_READ( hFilePointer , ii_return , sizeof(int) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case FLOAT_FORMAT  :
			case FLOAT_X_FORMAT  :
									wbyte = _FILE2SM_READ( hFilePointer , ff_return , sizeof(float) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case DOUBLE_FORMAT  :
			case DOUBLE_X_FORMAT  :
									wbyte = _FILE2SM_READ( hFilePointer , dd_return , sizeof(double) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case STRING_FORMAT :
			case FILENAME_FORMAT :
			case STRING_X_FORMAT :
			case FILENAME_X_FORMAT :
									wbyte = _FILE2SM_READ( hFilePointer , &ii , sizeof(int) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									wbyte = _FILE2SM_READ( hFilePointer , str_return , ii );
									str_return[wbyte] = 0x00;
									break;
		}
		wbyte = _FILE2SM_READ( hFilePointer , Reader , strlen(END_DELEMETER) );
		//
	}
	else {
		wbyte = _lread( hFilePointer , Reader , strlen(BEGIN_DELEMETER) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		Reader[wbyte] = 0x00;
		if ( strcmp( Reader , BEGIN_DELEMETER ) != 0 ) return( FALSE );

		wbyte = _lread( hFilePointer , &ii , sizeof(int) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		wbyte = _lread( hFilePointer , Reader , ii );          Reader[wbyte] = 0x00;
		strcpy( ReturnNameIndicator , Reader );

		wbyte = _lread( hFilePointer , Reader , sizeof(char) );
		if ( wbyte <= 0 ) return FALSE; // 2002.05.03
		memcpy( &cc , Reader , sizeof(char) );
	//	if ( cc < NAME_FORMAT || cc > FILENAME_FORMAT ) return ( FALSE ); // 2006.05.15
	//	if ( cc < NAME_FORMAT || cc > FILENAME_X_FORMAT ) return ( FALSE ); // 2006.05.15 // 2008.05.30
		if ( cc < NAME_FORMAT || cc > DOUBLE_X_FORMAT ) return ( FALSE ); // 2006.05.15 // 2008.05.30
		*type_return = cc;

		switch ( cc ) {
			case NAME_FORMAT   :
			case NAME_X_FORMAT   :
									break;
			case CHAR_FORMAT   :
			case CHAR_X_FORMAT   :
									wbyte = _lread( hFilePointer , cc_return , sizeof(char) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case INT_FORMAT    :
			case INT_X_FORMAT    :
									wbyte = _lread( hFilePointer , ii_return , sizeof(int) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case FLOAT_FORMAT  :
			case FLOAT_X_FORMAT  :
									wbyte = _lread( hFilePointer , ff_return , sizeof(float) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case DOUBLE_FORMAT  :
			case DOUBLE_X_FORMAT  :
									wbyte = _lread( hFilePointer , dd_return , sizeof(double) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									break;
			case STRING_FORMAT :
			case FILENAME_FORMAT :
			case STRING_X_FORMAT :
			case FILENAME_X_FORMAT :
									wbyte = _lread( hFilePointer , &ii , sizeof(int) );
									if ( wbyte <= 0 ) return FALSE; // 2002.05.03
									wbyte = _lread( hFilePointer , str_return , ii );
									str_return[wbyte] = 0x00;
									break;
		}
		wbyte = _lread( hFilePointer , Reader , strlen(END_DELEMETER) );
		//
	}
	//
	if ( wbyte <= 0 ) return FALSE; // 2002.05.03
	Reader[wbyte] = 0x00;
	if ( strcmp( Reader , END_DELEMETER ) != 0 ) return( FALSE );
	return( TRUE );
}
//============================================================================
int RECIPE_ITEM_CLUSTER_CHECK( char *data , int *cldepth , int *pmdepth ) {
	int i;
	//
	if ( !STRNCMP_L( data , "MODULE_" , 7 ) ) return -1;
	//
	i = 7;
	*cldepth = 0;
	*pmdepth = 0;
	//
	while ( TRUE ) {
		if ( ( data[i] >= '0' ) && ( data[i] <= '9' ) ) {
			*cldepth = ( ( *cldepth ) * 10 ) + ( data[i] - '0' );
		}
		else if ( data[i] == 0 ) {
			return -1;
		}
		else if ( data[i] == '_' ) {
			i++;
			break;
		}
		else {
			return -1;
		}
		i++;
	}
	//
	if ( data[i] != 'P' ) return -1;
	i++;
	//
	while ( TRUE ) {
		if ( ( data[i] >= '0' ) && ( data[i] <= '9' ) ) {
			*pmdepth = ( ( *pmdepth ) * 10 ) + ( data[i] - '0' );
		}
		else if ( data[i] == 0 ) {
			return 0;
		}
		else if ( data[i] == '_' ) {
			i++;
			break;
		}
		else {
			return -1;
		}
		i++;
	}
	//
	if ( STRCMP_L( data + i , "IN_RECIPE" ) ) return 1;
	if ( STRCMP_L( data + i , "OUT_RECIPE" ) ) return 2;
	if ( STRCMP_L( data + i , "PR_RECIPE" ) ) return 3;
	return -1;
}
//=========================================================================================
BOOL RECIPE_FILE_CLUSTER_DATA_READ_FOR_MAP( int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , CLUSTERStepTemplate3 *CLUSTER_RECIPE_SPECIAL , int Slot , int *selgrp , BOOL *mgf ) {
	int  i , j , z , z2;
	int depth , pmindex , mode;

	char Buffer[256+1];
	char Buffer1[256+1];
	char Buffer2[256+1];
	char Buffer3[256+1];

	// Initial

	*mgf = FALSE;
	*selgrp = 0;
	//
	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE->MODULE[i][j] = 0;
			CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] = NULL;
		}
	}
	CLUSTER_RECIPE->EXTRA_STATION = 0;
	CLUSTER_RECIPE->EXTRA_TIME    = 0;
	CLUSTER_RECIPE->HANDLING_SIDE = 0;

	CLUSTER_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA = 0;
	//
	CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA = NULL;
	CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA = NULL;

	CLUSTER_RECIPE_SPECIAL->USER_CONTROL_MODE = 0;
	CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA = NULL;
	//
	if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return TRUE;
	//
	if ( LOT_MAP_USE[Slot-1] == 0 ) return TRUE;
	if ( LOT_MAP_DATA[Slot-1] == NULL ) return TRUE;
	//
	z = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( LOT_MAP_DATA[Slot-1] , ' ' , Buffer , z , 256 );
		//
		if ( z == z2 ) break;
		//
		STR_SEPERATE_CHAR3( Buffer , ':' , Buffer1 , Buffer2 , Buffer3 , 256 );
		//
		depth = atoi( Buffer1 ) - 1;
		//
		mode = Buffer2[0];
		//
		pmindex = atoi( Buffer2 + 1 ) - 1;
		//
		if ( ( depth >= 0 ) && ( depth < MAX_CLUSTER_DEPTH ) ) {
			//
			if ( ( pmindex >= 0 ) && ( pmindex < MAX_PM_CHAMBER_DEPTH ) ) {
				//
				switch( mode ) {
				case 'P' :
					//
					CLUSTER_RECIPE->MODULE[depth][pmindex] = 1;
					//
					RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( -2 , NULL , NULL , NULL , NULL , Buffer3 , 4096 , &(CLUSTER_RECIPE->MODULE_IN_RECIPE2[depth][pmindex]) );
					//
					break;
				case 'T' :
					//
					CLUSTER_RECIPE->MODULE[depth][pmindex] = 1;
					//
					RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( -2 , NULL , NULL , NULL , NULL , Buffer3 , 4096 , &(CLUSTER_RECIPE->MODULE_OUT_RECIPE2[depth][pmindex]) );
					//
					break;
				case 'R' :
					//
					CLUSTER_RECIPE->MODULE[depth][pmindex] = 1;
					//
					RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( -2 , NULL , NULL , NULL , NULL , Buffer3 , 4096 , &(CLUSTER_RECIPE->MODULE_PR_RECIPE2[depth][pmindex]) );
					//
					break;
				}
			}
		}
		//
		z = z2;
		//
	}
	//===================================================================
	return TRUE;
}
//
//
//
BOOL RECIPE_FILE_CLUSTER_DATA_READ( int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , CLUSTERStepTemplate3 *CLUSTER_RECIPE_SPECIAL , char *grpname , int Slot , char *RealFileName , int reggrp , int *selgrp , BOOL *mgf , char *pjname ) {
	char RecipeFileName[4096+1];
	char FileName[4096+1];
	char DirName[4096+1];
	char DirName2[4096+1]; // 2002.09.12
	char FileName2[4096+1]; // 2002.09.12
//	char GrpName[256]; // 2002.05.10

	int grprun , grpidx;

	HFILE hFile;
	BOOL StepEndFind;
	int  i , j , j2 , kill;
	
	int FileName_SHPT;

	char  cc_return;
	int   ii_return;
	float ff_return;
	double dd_return;
	char  str_return[4096+1];
	char  name_return[255+1];
	char  type_return;

//	char  Buffer[1024+1]; // 2009.05.14
	char *imsi;

	// Initial

	for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			CLUSTER_RECIPE->MODULE[i][j] = 0;
			CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] = NULL;
			CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] = NULL;
		}
	}
	CLUSTER_RECIPE->EXTRA_STATION = 0;
	CLUSTER_RECIPE->EXTRA_TIME    = 0;
	CLUSTER_RECIPE->HANDLING_SIDE = 0;

	CLUSTER_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA = 0; // 2014.06.23
	//
	CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA = NULL; // 2005.01.24 // 2007.06.01
	CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA = NULL; // 2007.06.01

	CLUSTER_RECIPE_SPECIAL->USER_CONTROL_MODE = 0; // 2011.12.08
	CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA = NULL; // 2011.12.08

	if ( RealFileName == NULL ) return FALSE;
	if ( strcmp( RealFileName , "????" ) == 0 ) return TRUE;
	//===================================================================
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) return TRUE; // 2012.09.12
	//===================================================================
	// 2006.07.03
	//===================================================================
	grpidx = 0;
	*selgrp = 0;
	if ( reggrp == -1 ) {
		grprun = 1;
	}
	else {
		grprun = 0;
	}
	//
	// 2015.07.20
	FileName_SHPT = 0;
	if ( ( RealFileName[0] == '+' ) && ( ( RealFileName[1] == '\\' ) || ( RealFileName[1] == '/' ) ) )  FileName_SHPT = 2; // 2015.07.20
	//
	//===================================================================
	if ( side == -1 ) { // 2011.09.20
		UTIL_EXTRACT_GROUP_FILE(  SCHMULTI_DEFAULT_GROUP_GET() , RealFileName + FileName_SHPT , DirName , 4096 , FileName , 4096 ); // 2002.05.10
	}
	else {
		//
		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , RealFileName + FileName_SHPT , DirName , 4096 , FileName , 4096 ); // 2002.05.10
		//
	}
	//
//	STR_SEPERATE_CHAR( RealFileName , ':' , FileName , DirName , 255 );
//	i = strlen( DirName );
//	if ( i <= 0 ) {
//		strcpy( GrpName , _i_SCH_PRM_GET_GROUP_RECIPE_PATH( side ) );
//	}
//	else {
//		DirName[ i - 1 ] = 0;
//		strcpy( GrpName , DirName + 1 );
//	}
	//
//	if ( ( side != -1 ) && ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08
//	if ( ( side != -1 ) && ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08 // 2015.07.20
	if ( ( side != -1 ) && ( SCHEDULER_GET_RECIPE_LOCKING( side ) >= 2 ) && ( pjname != NULL ) && ( pjname[0] != 0 ) ) { // 2011.09.20 // 2014.02.08 // 2015.07.20
		if ( grpname == NULL ) {
			//
			SCHMULTI_RCPLOCKING_GET_FILENAME( 1 , 1 , pjname , DirName , FileName , RecipeFileName , 4096 );
			//
		}
		else {
			//
			SCHMULTI_RCPLOCKING_GET_FILENAME( 1 , 1 , pjname , grpname , FileName , RecipeFileName , 4096 );
			//
		}
		//
		// 2015.07.20
		//
		if ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 2 ) {
			FileName_SHPT = 0;
		}
		else {
			FileName_SHPT = 1;
		}
		//
	}
	else {
		strcpy( RecipeFileName , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
	//	strcat( RecipeFileName , GrpName ); // 2002.05.10
		if ( grpname == NULL ) { // 2009.06.16
			strcat( RecipeFileName , DirName );
		}
		else { // 2009.06.16
			strcat( RecipeFileName , grpname );
		}
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
		strcat( RecipeFileName , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() );
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
		strcat( RecipeFileName , FileName );
		//
	}
	//
	//
	read_mode_file_set( ( FileName_SHPT > 0 ) ? 2 : 0 ); // 2015.07.20 // default , file , shmem
	//
	hFile = read_open_file( RecipeFileName );
	//
	if ( hFile == -1 ) {
		_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Open Error (%s)\n" , RecipeFileName );
		return FALSE;
	}

	// INDICATOR
	if ( !read_form( hFile , "" , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ INDICATOR Error 1 (%s)\n" , RecipeFileName );
		read_close_file( hFile );
		return FALSE;
	}
	if ( !read_form( hFile , "" , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
		_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ INDICATOR Error 2 (%s)\n" , RecipeFileName );
		read_close_file( hFile );
		return FALSE;
	}

	if ( !read_form_name_until( hFile , "__&&CLUSTER_RECIPE_BEGIN&&__" ) ) { // 2007.09.18
		_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ BEGIN Error (%s)\n" , RecipeFileName );
		read_close_file( hFile );
		return FALSE;
	}

/*
// 2007.09.18
	if ( !read_form( hFile , "__&&CLUSTER_RECIPE_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) { _lclose( hFile ); return FALSE; }
*/

	StepEndFind = FALSE;
	kill = 0;
	while( !StepEndFind ) {
		if ( !read_return_form_MaxLimit( ( FileName_SHPT > 0 ) , hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , &dd_return , str_return ) ) {
			_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Read Error (%s)\n" , RecipeFileName );
			read_close_file( hFile );
			return FALSE;
		}
		switch ( type_return ) {
			case NAME_FORMAT :
								if ( STRCMP_L( name_return , "__&&CLUSTER_RECIPE_END&&__" ) ) StepEndFind = TRUE;
								//
								else if ( STRCMP_L( name_return , "__&&GROUP_NEXT&&__"		) )	{
									//=======================================================================================
									// 2006.07.03
									//=======================================================================================
									*mgf = TRUE;
									//
									grpidx++;
									//
									if ( grpidx > 0 ) {
										if ( grprun == 1 ) {
											StepEndFind = TRUE;
											break;
										}
										else {
											if ( grpidx > reggrp ) {
												grprun = 1;
												*selgrp = grpidx;
												//----------------------------------------
												for ( i = 0 ; i < MAX_CLUSTER_DEPTH ; i++ ) {
													for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
														CLUSTER_RECIPE->MODULE[i][j] = 0;
														CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j] = NULL;
														CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j] = NULL;
														CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j] = NULL;
													}
												}
												//----------------------------------------
											}
											else {
												grprun = 2;
											}
										}
									}
									//=======================================================================================
								}
								else	kill++;
								break;

			case INT_FORMAT  :	
								if      ( STRCMP_L( name_return , "EXTRA_STATION"	) )	CLUSTER_RECIPE->EXTRA_STATION	= ii_return;
								else if ( STRCMP_L( name_return , "EXTRA_TIME"		) )	CLUSTER_RECIPE->EXTRA_TIME		= ii_return;
								else if ( STRCMP_L( name_return , "HANDLING_SIDE"	) )	CLUSTER_RECIPE->HANDLING_SIDE	= ii_return;
								//
								else if ( STRCMP_L( name_return , "SPECIAL_INSIDE_DATA"	) )	{ // 2014.06.23
									if ( ii_return >= 0 ) {
										CLUSTER_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA = ii_return+1;
									}
								}
								//
								else if ( STRCMP_L( name_return , "USER_CONTROL_MODE" ) )	CLUSTER_RECIPE_SPECIAL->USER_CONTROL_MODE	= ii_return; // 2011.12.08
								//
								else {
									if ( grprun != 2 ) { // 2006.07.03
										/*
										// 2009.05.14
										strcpy( Buffer , name_return );
										Buffer[7] = 0;
										if ( STRCMP_L( Buffer , "MODULE_" ) ) {
											Buffer[0] = name_return[7];
											Buffer[1] = name_return[8];
											Buffer[2] = 0;
											i = atoi( Buffer ) - 1;
											Buffer[0] = name_return[11];
											if ( name_return[12] == 0 ) {
												Buffer[1] = 0;
											}
											else {
												Buffer[1] = name_return[12];
												Buffer[2] = 0;
											}
											j = atoi( Buffer ) - 1;
											if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) ) {
												kill++;
											}
											else {
												CLUSTER_RECIPE->MODULE[i][j]	= ii_return;
											}
										}
										else {
											kill++;
										}
										*/
										// 2009.05.14
										if ( RECIPE_ITEM_CLUSTER_CHECK( name_return , &i , &j ) == 0 ) {
											i--;
											j--;
											//
											if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) ) {
												kill++;
											}
											else {
												CLUSTER_RECIPE->MODULE[i][j]	= ii_return;
											}
										}
										else {
											kill++;
										}
									}
								}
								break;

			case FLOAT_FORMAT :
			case DOUBLE_FORMAT :
								kill++;
								break;

			case STRING_FORMAT :
								if      ( STRCMP_L( name_return , "SPECIAL_USER_DATA" ) ) { // 2005.01.24
									//strncpy( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA , str_return , 4096 ); // 2005.01.24 // 2007.06.01
									//=================================================================================================
									// 2007.06.04
									//=================================================================================================
									j = strlen( str_return );
									if ( j > 0 ) {
										if ( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA != NULL ) {
											j2 = strlen( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA );
											imsi = calloc( j2 + 1 , sizeof( char ) );
											if ( imsi == NULL ) {
												_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 1\n" ); // 2007.10.23
												read_close_file( hFile );
												return FALSE;
											}
											strcpy( imsi , CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA );
											free( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA );
										}
										else {
											imsi = NULL;
											j2 = 0;
										}
										CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA = calloc( j + j2 + 1 , sizeof( char ) );
										if ( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA == NULL ) {
											_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 2\n" ); // 2007.10.23
											read_close_file( hFile );
											return FALSE;
										}
										if ( imsi == NULL ) {
											strcpy( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA , str_return );
										}
										else {
											strcpy( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA , imsi );
											strcat( CLUSTER_RECIPE_SPECIAL->SPECIAL_USER_DATA , str_return );
											free( imsi );
										}
									}
									//=================================================================================================
								}
								else if ( STRCMP_L( name_return , "USER_CONTROL_DATA" ) ) { // 2011.12.08
									j = strlen( str_return );
									if ( j > 0 ) {
										if ( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA != NULL ) {
											j2 = strlen( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA );
											imsi = calloc( j2 + 1 , sizeof( char ) );
											if ( imsi == NULL ) {
												_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 11\n" ); // 2007.10.23
												read_close_file( hFile );
												return FALSE;
											}
											strcpy( imsi , CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA );
											free( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA );
										}
										else {
											imsi = NULL;
											j2 = 0;
										}
										CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA = calloc( j + j2 + 1 , sizeof( char ) );
										if ( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA == NULL ) {
											_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 12\n" ); // 2007.10.23
											read_close_file( hFile );
											return FALSE;
										}
										if ( imsi == NULL ) {
											strcpy( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA , str_return );
										}
										else {
											strcpy( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA , imsi );
											strcat( CLUSTER_RECIPE_SPECIAL->USER_CONTROL_DATA , str_return );
											free( imsi );
										}
									}
									//=================================================================================================
								}
								else if ( STRCMP_L( name_return , "RECIPE_TUNE_DATA" ) ) { // 2007.06.01
									//strncpy( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA , str_return , 4096 ); // 2007.06.01
									//=================================================================================================
									// 2007.06.04
									//=================================================================================================
									j = strlen( str_return );
									if ( j > 0 ) {
										if ( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA != NULL ) {
											j2 = strlen( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA );
											imsi = calloc( j2 + 1 , sizeof( char ) );
											if ( imsi == NULL ) {
												_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 3\n" ); // 2007.10.23
												read_close_file( hFile );
												return FALSE;
											}
											strcpy( imsi , CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA );
											free( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA );
										}
										else {
											imsi = NULL;
											j2 = 0;
										}
										CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA = calloc( j + j2 + 2 , sizeof( char ) );
										if ( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA == NULL ) {
											_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Memory Allocate Error 4\n" ); // 2007.10.23
											read_close_file( hFile );
											return FALSE;
										}
										if ( imsi == NULL ) {
											strcpy( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA , str_return );
										}
										else {
											strcpy( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA , imsi );
											strcat( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA , "|" );
											strcat( CLUSTER_RECIPE_SPECIAL->RECIPE_TUNE_DATA , str_return );
											free( imsi );
										}
									}
									//=================================================================================================
								}
								else {
									if ( grprun != 2 ) { // 2006.07.03
										/*
										Buffer[0] = name_return[7];
										Buffer[1] = name_return[8];
										Buffer[2] = 0;
										i = atoi( Buffer ) - 1;
										Buffer[0] = name_return[11];
										if ( name_return[12] == '_' ) {
											Buffer[1] = 0;
										}
										else {
											Buffer[1] = name_return[12];
											Buffer[2] = 0;
										}
										j = atoi( Buffer ) - 1;
										if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) ) {
											kill++;
										}
										else {
											name_return[7] = ' ';
											name_return[8] = ' ';
											name_return[11] = ' ';
											if ( name_return[12] != '_' ) name_return[12] = ' ';
											if      ( STRCMP_L( name_return , "MODULE_  _P _PR_RECIPE" ) || STRCMP_L( name_return , "MODULE_  _P  _PR_RECIPE" ) ) {
												if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j]) ) ) {
													_lclose( hFile );
													return FALSE;
												}
											}
											else if ( STRCMP_L( name_return , "MODULE_  _P _IN_RECIPE" ) || STRCMP_L( name_return , "MODULE_  _P  _IN_RECIPE" ) ) {
												if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j]) ) ) {
													_lclose( hFile );
													return FALSE;
												}
											}
											else if ( STRCMP_L( name_return , "MODULE_  _P _OUT_RECIPE" ) || STRCMP_L( name_return , "MODULE_  _P  _OUT_RECIPE" ) ) {
												if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j]) ) ) {
													_lclose( hFile );
													return FALSE;
												}
											}
											else {
												kill++;
											}
										}
										*/
										// 2009.05.14
										j2 = RECIPE_ITEM_CLUSTER_CHECK( name_return , &i , &j );
										//
										if ( ( j2 == 1 ) || ( j2 == 2 ) || ( j2 == 3 ) ) {
											i--;
											j--;
											//
											if ( ( i < 0 ) || ( j < 0 ) || ( i >= MAX_CLUSTER_DEPTH ) || ( j >= MAX_PM_CHAMBER_DEPTH ) ) {
												kill++;
											}
											else {
												if      ( j2 == 3 ) {
													if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , grpname , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_PR_RECIPE2[i][j]) ) ) {
														_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ PROCESS_MEM_MAP Error 1\n" );
														read_close_file( hFile );
														return FALSE;
													}
												}
												else if ( j2 == 1 ) {
													if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , grpname , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_IN_RECIPE2[i][j]) ) ) {
														_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ PROCESS_MEM_MAP Error 2\n" );
														read_close_file( hFile );
														return FALSE;
													}
												}
												else if ( j2 == 2 ) {
													if ( !RECIPE_FILE_CLUSTER_PROCESS_MEM_MAP( side , grpname , DirName , str_return , DirName2 , FileName2 , 4096 , &(CLUSTER_RECIPE->MODULE_OUT_RECIPE2[i][j]) ) ) {
														_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ PROCESS_MEM_MAP Error 3\n" );
														read_close_file( hFile );
														return FALSE;
													}
												}
											}
										}
										else {
											kill++;
										}

									}
								}
								break;

			default :			kill++;
								break;
		}
//		if ( kill >= 100 )	{ read_close_file( hFile );	return FALSE; } // 2007.06.08
		if ( kill >= 10000 )	{
			_IO_CIM_PRINTF( "RECIPE_FILE_CLUSTER_DATA_READ Overflow Error (%s)\n" , RecipeFileName );
			read_close_file( hFile );
			return FALSE;
		} // 2007.06.08
	}
	read_close_file( hFile );
	//===================================================================
	return TRUE;
}
//
//
//
BOOL RECIPE_FILE_PROCESS_DATA_READ_And_File_Save_Sub( int Locking , int side , int Chamber , char *RealFileName , int style , int Wafer , int ID , char *wfile , char *TuneData , int index ) { // 2005.10.16
	HFILE hFile;
	char RecipeFileName[512+1];
	char DirName[512+1];
	char FileName[512+1];
	BOOL StepEndFind;
//	int i,kill,Step_Count,Max_Step_Count,l,s,x,x2; // 2007.10.25
	int i,kill,Step_Count,l,s,x,x2; // 2007.10.25
	
	char  cc_return;
	int   ii_return;
	float ff_return;
	double dd_return;
	char  str_return[1024+1];
	char  name_return[1024+1];
	char  type_return;
	FILE *fpt;

	int RealFile_SHPT;
	//
	//============================================================================================================================
	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21
	//============================================================================================================================
	if ( RealFileName == NULL ) return FALSE;
	//============================================================================================================================
	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) != 2 ) { // 2008.03.13
		fpt = fopen( wfile , "a" );
		if ( fpt == NULL ) {
			_IO_CIM_PRINTF( "%s Recipe File Save Error[%d]\n" , wfile , GetLastError() ); // 2009.03.25
			return FALSE;
		}
		//============================================================================================================================
		if ( index != 0 ) {
			f_enc_printf( fpt , "\n=============================================================================\n" );
			f_enc_printf( fpt , "MULTIPLE FILE APPEND with %d\n" , index + 1 );
			f_enc_printf( fpt , "=============================================================================\n" );
		}
	}
	//============================================================================================================================
	RealFile_SHPT = 0;
	if ( ( RealFileName[0] == '+' ) && ( ( RealFileName[1] == '\\' ) || ( RealFileName[1] == '/' ) ) )   RealFile_SHPT = 2; // 2015.07.20
	//
	UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , RealFileName + RealFile_SHPT , DirName , 512 , FileName , 512 ); // 2002.05.10
	if ( Locking != 0 ) {
		if ( Locking >= 2 ) { // 2012.04.12
			/*
			// 2012.07.26
			if      ( style == 0 ) {
				sprintf( str_return , "%d$0$%d$%s$%s$%s" , side , Wafer , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName );
			}
			else if ( style == 1 ) {
				sprintf( str_return , "%d$1$%d$%s$%s$%s" , side , Wafer , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName );
			}
			else if ( style == 2 ) {
				sprintf( str_return , "%d$2$%d$%s$%s$%s" , side , Wafer , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName );
			}
			else {
				sprintf( str_return , "%d$%s$%s$%s" , side , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName );
			}
			*/
			if ( ( style == 0 ) || ( style == 1 ) || ( style == 2 ) ) {
				sprintf( str_return , "%d$%d$%s$%s$%s" , side , Wafer , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
			}
			else {
				sprintf( str_return , "%d$%s$%s$%s" , side , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
			}
		}
		else {
			sprintf( str_return , "%d$%s$%s$%s" , side , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , FileName );
		}
		Change_Dirsep_to_Dollar( str_return );
//		sprintf( RecipeFileName , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , str_return ); // 2017.10.30
		sprintf( RecipeFileName , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , str_return ); // 2017.10.30
		//
		if ( Locking > 2 ) { // 2015.07.20
			RealFile_SHPT = 2;
		}
		else {
			RealFile_SHPT = 0;
		}
	}
	else {
		strcpy( RecipeFileName , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) );
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
		strcat( RecipeFileName , DirName );
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
		strcat( RecipeFileName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) );
		if ( RecipeFileName != NULL ) strcat( RecipeFileName , "\\" );
		strcat( RecipeFileName , FileName );
	}
	if ( RecipeFileName == NULL ) {
		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) != 2 ) { // 2008.03.13
			f_enc_printf( fpt , "ERROR%cOpen File Data Null Error 2(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
		}
		return FALSE;
	}
	//==============================================================================================================
//	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) == 2 ) { // 2008.03.13 // 2015.07.20
	if ( ( RealFile_SHPT > 0 ) || ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) == 2 ) ) { // 2008.03.13 // 2015.07.20
		//
		if ( index != 0 ) {
			sprintf( str_return , "%s_FILE[%d]" , wfile , index + 1 );
		}
		else {
			sprintf( str_return , "%s_FILE" , wfile );
		}
		//
		if ( RealFile_SHPT > 0 ) {
			if ( 0 != _FILE2SM_COPY_SM_to_FILE( RecipeFileName , str_return ) ) return FALSE;		// 0:OK 1:SmfileOpenFail 2:SmfileNotFind 3:SmfileUsing 4:RealFileCreateFail 5:Smfileclose 6:SmfileReadFail 7:RealfileWriteFail 8:LockFail(M) 9:LockFail
		}
		else {
			if ( !CopyFile( RecipeFileName , str_return , FALSE ) ) return FALSE; // 2015.07.20
		}
		//
		if ( index != 0 ) {
			sprintf( str_return , "%s_INFO[%d]" , wfile , index + 1 );
		}
		else {
			sprintf( str_return , "%s_INFO" , wfile );
		}
		//
		fpt = fopen( str_return , "a" );
		if ( fpt == NULL ) {
			_IO_CIM_PRINTF( "%s Recipe File Save Error[%d]\n" , str_return , GetLastError ); // 2009.03.25
			return FALSE;
		}
		//
		KPLT_Current_Time( str_return );
//		f_enc_printf( fpt , "RECIPE%c%s%c%s%c%s\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , 9 , str_return ); // 2011.05.13
//		f_enc_printf( fpt , "RECIPE%c%s%c%s%c%s%c%d,%d\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , 9 , str_return , 9 , _i_SCH_PRM_GET_Process_Run_In_Mode(Chamber) , _i_SCH_PRM_GET_Process_Run_Out_Mode(Chamber) ); // 2011.05.13 // 2019.01.19
		f_enc_printf( fpt , "RECIPE%c%s%c%s [%s] [%d,%d]\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , str_return , _i_SCH_PRM_GET_Process_Run_In_Mode(Chamber) , _i_SCH_PRM_GET_Process_Run_Out_Mode(Chamber) ); // 2011.05.13 // 2019.01.19
		f_enc_printf( fpt , "HEADER_BEGIN\n" );
		if ( Locking != 0 ) {
//			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s\tLOCKED\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName ); // 2014.04.20
//			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s\tLOCKED\t%c%s%c\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName , '"' , RecipeFileName , '"' ); // 2014.04.20 // 2019.01.19
			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s LOCKED %c%s%c\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName , '"' , RecipeFileName , '"' ); // 2014.04.20 // 2019.01.19
		}
		else {
			f_enc_printf( fpt , "%cFILENAME%c%s\n"   , 9 , 9 , RecipeFileName );
		}
		f_enc_printf( fpt , "%cCHAMBER%c%s\n" , 9 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) );
		f_enc_printf( fpt , "%cMETHODID%c%d\n"   , 9 , 9 , ID );
		if ( index != 0 ) f_enc_printf( fpt , "%cMULTIINDEX%c%d\n"   , 9 , 9 , index ); // 2008.03.14
	}
	else {
		//
		hFile = _lopen( RecipeFileName , OF_READ );
		//
		if ( hFile == -1 ) {
			f_enc_printf( fpt , "ERROR%cOpen File Name Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}

		KPLT_Current_Time( str_return );
//		f_enc_printf( fpt , "RECIPE%c%s%c%s%c%s\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , 9 , str_return ); // 2006.10.26 // 2011.05.13
//		f_enc_printf( fpt , "RECIPE%c%s%c%s%c%s%c%d,%d\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , 9 , str_return , 9 , _i_SCH_PRM_GET_Process_Run_In_Mode(Chamber) , _i_SCH_PRM_GET_Process_Run_Out_Mode(Chamber)  ); // 2006.10.26 // 2011.05.13 // 2019.01.19
		f_enc_printf( fpt , "RECIPE%c%s%c%s [%s] [%d,%d]\n" , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , FileName , str_return , _i_SCH_PRM_GET_Process_Run_In_Mode(Chamber) , _i_SCH_PRM_GET_Process_Run_Out_Mode(Chamber)  ); // 2006.10.26 // 2011.05.13 // 2019.01.19
		f_enc_printf( fpt , "HEADER_BEGIN\n" );
		if ( Locking != 0 ) {
//			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s\tLOCKED\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName ); // 2014.04.20
//			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s\tLOCKED\t%c%s%c\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName , '"' , RecipeFileName , '"' ); // 2014.04.20 // 2019.01.19
			f_enc_printf( fpt , "%cFILENAME%c%s/%s/%s/%s LOCKED %c%s%c\n" , 9 , 9 , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , FileName , '"' , RecipeFileName , '"' ); // 2014.04.20 // 2019.01.19
		}
		else {
			f_enc_printf( fpt , "%cFILENAME%c%s\n"   , 9 , 9 , RecipeFileName );
		}
		f_enc_printf( fpt , "%cCHAMBER%c%s\n" , 9 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) ); // 2006.10.26
		f_enc_printf( fpt , "%cMETHODID%c%d\n"   , 9 , 9 , ID );
		if ( index != 0 ) f_enc_printf( fpt , "%cMULTIINDEX%c%d\n"   , 9 , 9 , index ); // 2008.03.14
	}
	//
	//=====================================================================================================
	if ( TuneData != NULL ) { // 2007.06.08
		if ( TuneData[0] == '$' ) { // 2007.06.08
			kill = strlen( TuneData );
			s = 0;
			l = 0;
			x = 0; // 2007.06.08
			x2 = 0; // 2007.06.08
	//		for ( i = 0 ; i < kill ; i++ ) { // 2007.06.08
			for ( i = 1 ; i < kill ; i++ ) { // 2007.06.08
				if ( TuneData[i] == '|' ) {
					if ( l > 0 ) {
						//=========================================================
						// 2007.06.08
						//=========================================================
						if ( x2 > 0 ) {
							str_return[x2] = 0;
							x2 = atoi( str_return );
							//
							if ( ( x2 == -1 ) || ( index == x2 ) ) {
								name_return[l] = 0;
								f_enc_printf( fpt , "%cTUNEITEM(%d)%c%s\n" , 9 , s + 1 , 9 , name_return );
								s++;
							}
							//
						}
						//=========================================================
						else {
							name_return[l] = 0;
							f_enc_printf( fpt , "%cTUNEITEM(%d)%c%s\n" , 9 , s + 1 , 9 , name_return );
							s++;
						}
						//=========================================================
					}
					l = 0;
					x = 0; // 2007.06.08
					x2 = 0; // 2007.06.08
				}
				else {
//					if ( l <= 1000 ) { // 2007.10.19
					if ( l <= 256 ) { // 2007.10.19
						//========================================================================
						// 2007.06.08
						//========================================================================
						if ( TuneData[i] == ':' ) {
							x++;
						}
						else {
							if ( x == 3 ) {
								if ( x2 < 256 ) { // 2007.10.19
									str_return[x2] = TuneData[i];
									x2++;
								}
							}
						}
						//========================================================================
						name_return[l] = TuneData[i];
						l++;
					}
				}
			}
			if ( l > 0 ) {
				//=========================================================
				// 2007.06.08
				//=========================================================
				if ( x2 > 0 ) {
					str_return[x2] = 0;
					x2 = atoi( str_return );
					//
					if ( ( x2 == -1 ) || ( index == x2 ) ) {
						name_return[l] = 0;
						f_enc_printf( fpt , "%cTUNEITEM(%d)%c%s\n" , 9 , s + 1 , 9 , name_return );
						s++; // 2008.03.13
					}
					//
				}
				//=========================================================
				else {
					name_return[l] = 0;
					f_enc_printf( fpt , "%cTUNEITEM(%d)%c%s\n" , 9 , s + 1 , 9 , name_return );
					s++; // 2008.03.13
				}
				//=========================================================
			}
		}
	}
	//=====================================================================================================
//	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) == 2 ) { // 2008.03.13 // 2015.07.20
	if ( ( RealFile_SHPT > 0 ) || ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) == 2 ) ) { // 2008.03.13 // 2015.07.20
		fclose( fpt );
		return TRUE;
	}
	//=====================================================================================================
	// 2007.06.19
	//=====================================================================================================
	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( Chamber ) == 1 ) {
		s = 0;
		while( TRUE ) {
			kill = _lread( hFile , str_return , 512 );
			if ( kill <= 0 ) break;
			s = 1;
			for ( i = 0 ; i < kill ; i++ ) {
				switch( str_return[i] ) {
				case 0 :	f_enc_printf( fpt , "<NULL:%d>" , str_return[i] );	break;	//	null
				case 1 :	f_enc_printf( fpt , "<SOH:%d>" , str_return[i] );	break;	//	Start Of Heading
				case 2 :	f_enc_printf( fpt , "<STX:%d>" , str_return[i] );	break;	//	Start Of Text
				case 3 :	f_enc_printf( fpt , "<ETX:%d>" , str_return[i] );	break;	//	End Of Text
				case 4 :	f_enc_printf( fpt , "<EOT:%d>" , str_return[i] );	break;	//	End Of Transmit
				case 5 :	f_enc_printf( fpt , "<ENQ:%d>" , str_return[i] );	break;	//	Enquiry
				case 6 :	f_enc_printf( fpt , "<ACK:%d>" , str_return[i] );	break;	//	Acknowledge
				case 7 :	f_enc_printf( fpt , "<BEL:%d>" , str_return[i] );	break;	//	Bell
				case 8 :	f_enc_printf( fpt , "<BS:%d>" , str_return[i] );		break;	//	Back Space
//				case 9 :	f_enc_printf( fpt , "<HT:%d>" , str_return[i] );		break;	//	Horizontal Tab
//				case 10 :	f_enc_printf( fpt , "<LF:%d>" , str_return[i] );		break;	//	Line Feed

				case 11 :	f_enc_printf( fpt , "<VT:%d>" , str_return[i] );		break;	//	Vertical Tab
				case 12 :	f_enc_printf( fpt , "<FF:%d>" , str_return[i] );		break;	//	Form Feed
//				case 13 :	f_enc_printf( fpt , "<CR:%d>" , str_return[i] );		break;	//	Carriage Return
				case 14 :	f_enc_printf( fpt , "<SO:%d>" , str_return[i] );		break;	//	Shift Out
				case 15 :	f_enc_printf( fpt , "<SI:%d>" , str_return[i] );		break;	//	Shift In
				case 16 :	f_enc_printf( fpt , "<DLE:%d>" , str_return[i] );	break;	//	Data Link Escape
				case 17 :	f_enc_printf( fpt , "<DC1:%d>" , str_return[i] );	break;	//	Device Control 1
				case 18 :	f_enc_printf( fpt , "<DC2:%d>" , str_return[i] );	break;	//	Device Control 2
				case 19 :	f_enc_printf( fpt , "<DC3:%d>" , str_return[i] );	break;	//	Device Control 3
				case 20 :	f_enc_printf( fpt , "<DC4:%d>" , str_return[i] );	break;	//	Device Control 4

				case 21 :	f_enc_printf( fpt , "<NAK:%d>" , str_return[i] );	break;	//	Negative Acknowledge
				case 22 :	f_enc_printf( fpt , "<SYN:%d>" , str_return[i] );	break;	//	Syncronomous idle
				case 23 :	f_enc_printf( fpt , "<ETB:%d>" , str_return[i] );	break;	//	End Transmit Block
				case 24 :	f_enc_printf( fpt , "<CAN:%d>" , str_return[i] );	break;	//	Cancel
				case 25 :	f_enc_printf( fpt , "<EM:%d>" , str_return[i] );		break;	//	End of Medium
				case 26 :	f_enc_printf( fpt , "<SUB:%d>" , str_return[i] );	break;	//	Substitution
				case 27 :	f_enc_printf( fpt , "<ESC:%d>" , str_return[i] );	break;	//	Escape
				case 28 :	f_enc_printf( fpt , "<FS:%d>" , str_return[i] );		break;	//	File Separator
				case 29 :	f_enc_printf( fpt , "<GS:%d>" , str_return[i] );		break;	//	Group Separator
				case 30 :	f_enc_printf( fpt , "<RS:%d>" , str_return[i] );		break;	//	Record Separator
				case 31 :	f_enc_printf( fpt , "<US:%d>" , str_return[i] );		break;	//	Unit Separator

				default :	f_enc_printf( fpt , "%c" , str_return[i] );			break;
				}
			}
		}
		if ( s == 0 ) f_enc_printf( fpt , "ERROR%cFile Data is Nothing(%s)\n" , 9 , RecipeFileName );
	}
	//=====================================================================================================
	else {
		// INDICATOR
		if ( !read_form_name_check( hFile , "" ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cFile ID Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}
		if ( !read_form_name_check( hFile , "" ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cProcess ID Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}
//		Max_Step_Count = MAX_PM_STEP_COUNT; // 2007.10.25
		// HEADER
		if ( !read_form_name_check( hFile , "__&&HEADER_BEGIN&&__" ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cHeader Begin Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}
		if ( !read_form_until_Matched( hFile , "NumberOfSteps"        , INT_FORMAT    , &cc_return , &Step_Count , &ff_return , str_return ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cHeader Data Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}
		if ( !read_form_name_until( hFile , "__&&HEADER_END&&__" ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cHeader End Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}

		//
		f_enc_printf( fpt , "%cSTEP%c%d\n" , 9 , 9 , Step_Count );
		f_enc_printf( fpt , "HEADER_END\n" );

		f_enc_printf( fpt , "STEP_BEGIN%c%d\n" , 9 , 0 );
		if ( !read_form( hFile , "__&&STEP0_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
			_lclose( hFile );
			f_enc_printf( fpt , "ERROR%cStep 0 Header Error(%s)\n" , 9 , RecipeFileName );
			fclose( fpt );
			return FALSE;
		}

		StepEndFind = FALSE;
		kill = 0;
		while( !StepEndFind ) {
			kill++;
			if ( !read_return_form2( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , &dd_return , str_return ) ) {
				_lclose( hFile );
				f_enc_printf( fpt , "ERROR%cStep 0 Data Read Error(%s)\n" , 9 , RecipeFileName );
				fclose( fpt );
				return FALSE;
			}
			switch ( type_return ) {
				case NAME_FORMAT :	if ( strcmp( name_return , "__&&STEP0_END&&__" ) == 0 ) StepEndFind = TRUE;
									break;
				case CHAR_FORMAT :
									f_enc_printf( fpt , "%c%s%c%d\n" , 9 , name_return , 9 , cc_return );
									break;
				case INT_FORMAT	 :
									f_enc_printf( fpt , "%c%s%c%d\n" , 9 , name_return , 9 , ii_return );
									break;
				case FLOAT_FORMAT:
									f_enc_printf( fpt , "%c%s%c%lf\n" , 9 , name_return , 9 , ff_return );
									break;
				case DOUBLE_FORMAT:
									f_enc_printf( fpt , "%c%s%c%lf\n" , 9 , name_return , 9 , dd_return );
									break;
				case STRING_FORMAT:
				case FILENAME_FORMAT:
									f_enc_printf( fpt , "%c%s%c%s\n" , 9 , name_return , 9 , str_return );
									break;
			}
			if ( kill >= 1000 )	{
				_lclose( hFile );
				f_enc_printf( fpt , "ERROR%cStep 0 Data Error(%s)\n" , 9 , RecipeFileName );
				fclose( fpt );
				return FALSE;
			}
		}
		f_enc_printf( fpt , "STEP_END%c%d\n" , 9 , 0 );
		// STEP
//		for ( i = 0 ; ( i < Step_Count ) && ( i < Max_Step_Count ) ; i++ ) { // 2007.10.25
		for ( i = 0 ; ( i < Step_Count ) && ( i < MAX_PM_STEP_COUNT ) ; i++ ) { // 2007.10.25
			if ( !read_form( hFile , "__&&STEP_BEGIN&&__"   , NAME_FORMAT   , &cc_return , &ii_return           , &ff_return , str_return        ) ) {
				_lclose( hFile );
				f_enc_printf( fpt , "ERROR%cStep %d Header Error(%s)\n" , 9 , i + 1 , RecipeFileName );
				fclose( fpt );
				return FALSE;
			}
			f_enc_printf( fpt , "STEP_BEGIN%c%d\n" , 9 , i + 1 );
			// StepRead
			StepEndFind = FALSE;
			kill = 0;
			while( !StepEndFind ) {
				if ( !read_return_form2( hFile , name_return , &type_return , &cc_return , &ii_return , &ff_return , &dd_return , str_return ) ) {
					_lclose( hFile );
					f_enc_printf( fpt , "ERROR%cStep %d Data Read Error(%s)\n" , 9 , i + 1 , RecipeFileName );
					fclose( fpt );
					return FALSE;
				}
				switch (type_return) {
					case NAME_FORMAT :	if ( strcmp( name_return , "__&&STEP_END&&__" ) == 0 ) StepEndFind = TRUE;
										break;
					case CHAR_FORMAT :
										f_enc_printf( fpt , "%c%s%c%d\n" , 9 , name_return , 9 , cc_return );
										break;
					case INT_FORMAT	 :
										f_enc_printf( fpt , "%c%s%c%d\n" , 9 , name_return , 9 , ii_return );
										break;
					case FLOAT_FORMAT:
										f_enc_printf( fpt , "%c%s%c%lf\n" , 9 , name_return , 9 , ff_return );
										break;
					case DOUBLE_FORMAT:
										f_enc_printf( fpt , "%c%s%c%lf\n" , 9 , name_return , 9 , dd_return );
										break;
					case STRING_FORMAT:
					case FILENAME_FORMAT:
										f_enc_printf( fpt , "%c%s%c%s\n" , 9 , name_return , 9 , str_return );
										break;
				}
				if ( kill >= 1000 )	{
					_lclose( hFile );
					f_enc_printf( fpt , "ERROR%cStep %d Data Error(%s)\n" , 9 , i + 1 , RecipeFileName );
					fclose( fpt );
					return FALSE;
				}
			}
			f_enc_printf( fpt , "STEP_END%c%d\n" , 9 , i + 1 );
		}
	}
	//=====================================================================================================
	_lclose( hFile );
	fclose( fpt );
	//=====================================================================================================
	return TRUE;
}
//=
BOOL RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( int Locking , int side , int Chamber , char *RealFileName , int Style , int Wafer , int ID , char *wfile , char *TuneData ) { // 2005.10.16
	int z1 , z2 , x;
	char Retdata[512+1];
	//
	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) != 0 ) return TRUE; // 2007.03.07
	//
	x = 0;
	z1 = 0;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( RealFileName , '|' , Retdata , z1 , 512 );
	if ( z1 == z2 ) return FALSE;
	//
	do {
		if ( strlen( Retdata ) > 0 ) { // 2007.06.11
			if ( !RECIPE_FILE_PROCESS_DATA_READ_And_File_Save_Sub( Locking , side , Chamber , Retdata , Style , Wafer , ID , wfile , TuneData , x ) ) return FALSE;
		}
		x++;
		z1 = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( RealFileName , '|' , Retdata , z1 , 512 );
		if ( z1 == z2 ) break;
	}
	while( TRUE );
	return TRUE;
}
//=

/*
// for Customer Supply
void UTIL_REMAP_PROCESS_RECIPE_FILE( int Locking , int side , int Slot , char *LogPath , char *MainRecipePath , char *RunGroup , char *PMRecipePath , char *RecipeName , BOOL Normal , char *filename , int count ) { // 2013.08.21
	//
	char BufferN[511+1];
	//
	if ( Locking != 0 ) {
		//
		if ( Locking == 2 ) {
			if ( Normal ) {
				_snprintf( BufferN , 511 , "%d$%d$%s$%s$%s" , side , Slot , RunGroup , PMRecipePath , RecipeName );
			}
			else {
				_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , PMRecipePath , RecipeName );
			}
		}
		else {
			_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , PMRecipePath , RecipeName );
		}
		//
		Change_Dirsep_to_Dollar( BufferN );
		//
		_snprintf( filename , count , "%s/tmp/%s" , LogPath , BufferN );
		//
	}
	else {
		_snprintf( filename , count , "%s/%s/%s/%s" , MainRecipePath , RunGroup , PMRecipePath , RecipeName );
	}
}
void UTIL_FILE_GET_PROCESS_Real_File( int Locking , int side , int Chamber , int Slot , char *LogPath , char *MainRecipePath , char *RecipeGroup , char *PMRecipePath , char *RecipeName , BOOL Normal , char *RealFilename , int count ) {
	char RunGroup[255+1];
	char RunFile[255+1];
	//
//	UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , RecipeOrgName , RunGroup , 255 , RunFile , 255 );
//	UTIL_REMAP_PROCESS_RECIPE_FILE( Locking , side , Slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunFile , Normal , filename , count );
	//
	UTIL_EXTRACT_GROUP_FILE( RecipeGroup , RecipeName , RunGroup , 255 , RunFile , 255 );
	UTIL_REMAP_PROCESS_RECIPE_FILE( Locking , side , Slot , LogPath , MainRecipePath , RunGroup , PMRecipePath , RunFile , Normal , RealFilename , count );
	//
}
*/
int RECIPE_FILE_PROCESS_File_Check_Sub( int LockingTrg , int LockingSrc , int side , int pt , int Chamber , int Slot , char *Recipe , int mode , int etc , int index ) { // 2005.10.14
	char Buffer[511+1];
	char Buffer2[511+1];
	char BufferN[511+1];
	char RunGroup[255+1];
	char RunFile[255+1];
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	int errorcode; // 2014.09.15
	int Recipe_SHPT;
	BOOL SHMEM_Src;
	BOOL SHMEM_Trg;
	//
	if ( Recipe == NULL ) return 2;
	//
	// 2015.07.20
	Recipe_SHPT = 0;
	if ( ( Recipe[0] == '+' ) && ( ( Recipe[1] == '\\' ) || ( Recipe[1] == '/' ) ) ) Recipe_SHPT = 2; // 2015.07.20
	//
	if ( strlen( Recipe ) <= 0 ) return 2;
	if ( Recipe[0] == '?' ) return 2; // 2013.09.03
	if ( strlen( Recipe + Recipe_SHPT ) <= 0 ) return 2; // 2015.07.20
	if ( Recipe[Recipe_SHPT] == '?' ) return 2; // 2015.07.20
	//
	UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , Recipe + Recipe_SHPT , RunGroup , 255 , RunFile , 255 ); // 2002.05.10
	//
//	if ( ( Locking == 2 ) && ( pt != -1 ) ) { // 2011.09.20 // 2014.02.07
	if ( ( LockingSrc >= 2 ) && ( pt != -1 ) ) { // 2011.09.20 // 2014.02.07
		//
		strcpy( Buffer2 , SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( 1 , side , pt ) );
		//
		if ( Buffer2[0] == 0 ) {
			//
			_snprintf( Buffer , 511 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunFile );
			//
			SHMEM_Src = ( Recipe_SHPT > 0 ); // 2015.07.20
			//
		}
		else {
			//
			SCHMULTI_RCPLOCKING_GET_FILENAME( Chamber , 1 , Buffer2 , RunGroup , RunFile , Buffer , 511 );
			//
			SHMEM_Src = ( LockingSrc > 2 ); // 2015.07.20
			//
		}
		//
	}
	else {
		_snprintf( Buffer , 511 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Chamber ) , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunFile );
		//
		SHMEM_Src = ( Recipe_SHPT > 0 ); // 2015.07.20
		//
	}
	//
	//=============================================================================================================
	//=============================================================================================================
	//
	if ( SHMEM_Src ) { // 2015.07.20
		if ( _FILE2SM_STATUS( Buffer ) == 0 ) {
			return 1;
		}
	}
	else {
		hFind = FindFirstFile( Buffer , &fd );
		if ( hFind == INVALID_HANDLE_VALUE ) {
//			FindClose( hFind ); // 2016.09.01
//			return 1; // 2016.09.01
			//
			// 2016.09.01
			//
			Sleep(250);
			//
			hFind = FindFirstFile( Buffer , &fd );
			if ( hFind == INVALID_HANDLE_VALUE ) {
				//
				Sleep(250);
				//
				hFind = FindFirstFile( Buffer , &fd );
				if ( hFind == INVALID_HANDLE_VALUE ) {
					return 1;
				}
				//
			}
			//
		}
		FindClose( hFind );
	}
	//
	//=============================================================================================================
	//=============================================================================================================
	//
//	if ( Locking != 0 ) { // 2014.02.07
	if ( LockingTrg != 0 ) { // 2014.02.07
		//
		if ( LockingTrg <= 2 ) { // 2015.07.20
			//
//			sprintf( BufferN , "%s/tmp" , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
			sprintf( BufferN , "%s/tmp" , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
			//
			_mkdir( BufferN );
			//
			SetFileAttributes( BufferN , FILE_ATTRIBUTE_HIDDEN ); // 2014.02.07
			//
		}
		//
//		if ( Locking == 2 ) { // 2012.04.12 // 2014.02.07
		if ( LockingTrg >= 2 ) { // 2012.04.12 // 2014.02.07
			if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) {
				_snprintf( BufferN , 511 , "%d$%d$%s$%s$%s" , side , Slot , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunFile );
			}
			else {
				_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunFile );
			}
			//
			SHMEM_Trg = ( LockingTrg > 2 ); // 2015.07.20
			//
		}
		else {
			_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunFile );
			//
			SHMEM_Trg = FALSE; // 2015.07.20
			//
		}
		//
		Change_Dirsep_to_Dollar( BufferN );
		//
//		_snprintf( Buffer2 , 511 , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferN ); // 2017.10.30
		_snprintf( Buffer2 , 511 , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferN ); // 2017.10.30
		//
		/*
		// 2014.09.15
		if ( !CopyFile( Buffer , Buffer2 , FALSE ) ) {
			//
			DeleteFile( Buffer2 ); // 2013.09.13
			//
			if ( !CopyFile( Buffer , Buffer2 , FALSE ) ) { // 2013.09.13
				return 1;
			}
		}
		//
		SetFileAttributes( Buffer2 , FILE_ATTRIBUTE_HIDDEN ); // 2013.09.13
		//
		*/
		//
		// 2014.09.15
		//
		if ( !UTIL_CopyFile( SHMEM_Src , Buffer , SHMEM_Trg , Buffer2 , FALSE , 3 , TRUE , &errorcode ) ) { // 2014.09.15
			//
			_IO_CIM_PRINTF( "UTIL_CopyFile(%d:%s,%d:%s,%d) in RECIPE_FILE_PROCESS_File_Check_Sub\n" , SHMEM_Src , Buffer , SHMEM_Trg , Buffer2 , errorcode );
			//
			return 1;
			//
		}
		//
		//
		RECIPE_LOCKING_AUTO_DELETE_CHECK = TRUE; // 2013.09.13
		//
	}
	return 0;
}
//=
int RECIPE_FILE_PROCESS_File_Check( int LockingTrg , int LockingSrc , int side , int pt , int Chamber , int Slot , char *Recipe , int mode , int etc ) { // 2005.10.14
	int z1 , z2 , x , Res;
	int allres; // 2010.10.26
	char Retdata[511+1];
	//
//	_i_SCH_LOG_LOT_PRINTF( side , "RCPCHK [Locking=%d][pt=%d][Slot=%d][mode=%d]->[%s][%s]%cWHPMCOPY\n" , Locking , pt , Slot , mode , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , Recipe , 9 );
	//
	if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) != 0 ) return 0; // 2007.03.07
	//
	x = 0;
	z1 = 0;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( Recipe , '|' , Retdata , z1 , 511 );
	if ( z1 == z2 ) return 0;
	//
	allres = 2; // 2010.10.26
	//
	do {
		Res = RECIPE_FILE_PROCESS_File_Check_Sub( LockingTrg , LockingSrc , side , pt , Chamber , Slot , Retdata , mode , etc , x );
//		if ( Res != 0 ) return Res; // 2010.10.26
		if      ( Res == 1 ) { // 2010.10.26
			allres = 1; // 2010.10.26
		}
		else if ( Res == 0 ) { // 2010.10.26
			if ( allres == 2 ) allres = 0; // 2010.10.26
		}
		//
		x++;
		z1 = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( Recipe , '|' , Retdata , z1 , 511 );
		if ( z1 == z2 ) break;
	}
	while( TRUE );
	//
//	return 0; // 2010.10.26
	return allres; // 2010.10.26
}

void RECIPE_FILE_PROCESS_File_Locking_Delete( int Locking , int side ) {
	char FileName[512];
	char Buffer[256];
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	BOOL bRet = TRUE;
	//
	if ( Locking == 0 ) return;
	//
	if ( Locking >= 3 ) {
//		sprintf( Buffer , "%s/tmp/%d$*" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , side ); // 2017.10.30
		sprintf( Buffer , "%s/tmp/%d$*" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , side ); // 2017.10.30
		_FILE2SM_DELETE( Buffer , TRUE );
	}
	else {
//		sprintf( Buffer , "%s/tmp/%d$*.*" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , side ); // 2017.10.30
		sprintf( Buffer , "%s/tmp/%d$*.*" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , side ); // 2017.10.30
		hFind = FindFirstFile( Buffer , &fd );
		while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
//				sprintf( FileName , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , fd.cFileName ); // 2017.10.30
				sprintf( FileName , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , fd.cFileName ); // 2017.10.30
				//
//				DeleteFile( FileName ); // 2016.09.06
				//
				if ( !DeleteFile( FileName ) ) { // 2016.09.06
					SetFileAttributes( FileName , FILE_ATTRIBUTE_NORMAL ); // 2016.09.06
					DeleteFile( FileName ); // 2016.09.06
				}
				//
			}
			bRet = FindNextFile( hFind , &fd );
		}
		FindClose( hFind );
	}
}
//
void RECIPE_FILE_PROCESS_File_Locking_AutoDelete() { // 2013.09.13
	int i;
	//
	if ( !RECIPE_LOCKING_AUTO_DELETE_CHECK ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) return;
		if ( _SCH_SYSTEM_SIDE_CLOSING_GET( i ) ) return; // 2014.10.30
		//
	}
	//
	if ( EQUIP_RUNNING_CHECK_SUB( TRUE ) ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
//		if ( _i_SCH_SYSTEM_USING_GET( i ) <= 0 ) { // 2014.10.30
//			RECIPE_FILE_PROCESS_File_Locking_Delete( 1 , i ); // 2014.10.30
//		} // 2014.10.30
		//
		//
		if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) return; // 2014.10.30
		if ( _SCH_SYSTEM_SIDE_CLOSING_GET( i ) ) return; // 2014.10.30
		//
		RECIPE_FILE_PROCESS_File_Locking_Delete( 1 , i ); // 2014.10.30
		//
	}
	//
	RECIPE_LOCKING_AUTO_DELETE_CHECK = FALSE;
	//
}
//