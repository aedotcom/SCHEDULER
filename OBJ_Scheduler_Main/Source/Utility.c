#include "default.h"

#include <conio.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <SYS/STAT.H>

#include <winuser.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>

#include "EQ_Enum.h"

#include "Lottype.h"

//================================================================================================================
//================================================================================================================
//================================================================================================================
void UTIL_Copy_Multi_Recipe_Valid_String_Count( char *target , char *source , int count ) {
	int i;
//
	i = 0;
	//
	while ( TRUE ) {
		//
		if ( i >= count ) break;
		//
		if      ( source[i] == 0 ) {
			break;
		}
		else if ( source[i] == '|' ) {
			break;
		}
		//
		target[i] = source[i];
		//
		i++;
		//
	}
	//
	target[i] = 0;
}
//================================================================================================================
//================================================================================================================
void UTIL_Extract_Reset_String( char *source ) {
	int i , l;
	l = strlen( source );
	//
	if ( ( source[0] == '"' ) && ( source[l-1] == '"' ) ) {
		for ( i = 0 ; i < ( l - 1 ) ; i++ ) {
			source[i] = source[i+1];
		}
		source[l-2] = 0;
	}
}

void UTIL_EXTRACT_UPFOLDER( char *folder , char *res ) {
	int i , l , c = 0;
	l = strlen( folder );
	for ( i = 0 ; i < l ; i++ ) {
		if      ( folder[i] == '/' ) c++;
		else if ( folder[i] == '\\' ) c++;
	}
	strcpy( res , ".." );
	for ( i = 0 ; i < c ; i++ ) {
		strcat( res , "\\.." );
	}
}
//================================================================================================================
void UTIL_CHANGE_FILE_VALID_STYLE( char *data ) {
	int i , l;
	l = strlen( data );
	for ( i = 0 ; i < l ; i++ ) {
		if      ( data[i] == ' ' ) data[i] = 29; // 2007.08.30
		else if ( data[i] == ':' ) data[i] = 28; // 2007.10.07
		else if ( data[i] == '/' ) data[i] = '\\';
	}
}
//================================================================================================================
void UTIL_EXTRACT_GROUP_FILE( char *OrgGroup , char *OrgFile , char *RunGroup , int RunGroupCnt , char *RunFile , int RunFileCnt ) {
	int i , l , m;
	l = strlen( OrgFile );
	m = strlen( OrgGroup ); // 2002.05.30
	for ( i = 0 ; i < l ; i++ ) {
		if ( OrgFile[i] == ':' ) break;
	}
	if ( i == l ) {
		if ( m <= 0 ) { // 2002.05.30
			strncpy( RunGroup , "." , RunGroupCnt );
		}
		else {
			strncpy( RunGroup , OrgGroup , RunGroupCnt );
		}
		strncpy( RunFile , OrgFile , RunFileCnt );
	}
	else {
		if ( OrgFile[i+2] == '*' ) {
			if ( m <= 0 ) { // 2002.05.30
				strncpy( RunGroup , OrgFile + i + 2 + 1 , RunGroupCnt );
				l = strlen( RunGroup );
				if ( RunGroup[l-1] == ')' ) RunGroup[l-1] = 0;
			}
			else {
				strncpy( RunGroup , OrgGroup , RunGroupCnt );
			}
		}
		else {
			strncpy( RunGroup , OrgFile + i + 2 , RunGroupCnt );
			l = strlen( RunGroup );
			if ( RunGroup[l-1] == ')' ) RunGroup[l-1] = 0;
		}
		strncpy( RunFile , OrgFile , RunFileCnt );
		if ( i < RunFileCnt ) RunFile[i] = 0;
	}
}
//================================================================================================================
void UTIL_MAKE_DYNAMIC_GROUP_FILE( char *TargetFile , char *SourceFile , int count ) {
	int i , j , k , l;
	l = strlen( SourceFile );
	if ( l >= ( count - 1 ) ) l = count - 1;
	for ( i = 0 , j = 0 , k = 0 ; i < l ; i++ , j++ ) {
		if ( k == 0 ) {
			if ( SourceFile[i] == ':' ) k = 1;
		}
		else if ( k == 1 ) {
			if ( SourceFile[i] == '(' ) k = 2;
		}
		TargetFile[j] = SourceFile[i];
		if ( k == 2 ) {
			j++;
			TargetFile[j] = '*';
			k = 3;
		}
	}
	TargetFile[j] = 0;
}
//================================================================================================================
BOOL UTIL_Get_OpenSave_File_Select( HWND hWnd , BOOL Savemode , char *FileName , int count , char *initdir , char *filter , char *title ) {
	int i , l;
	OPENFILENAME ofn;
	char Buffer1[MAX_PATH+1] , Buffer2[MAX_PATH+1]; 
	char szExtDefault[4] , szExtFilter[MAX_PATH+1];
	//
	strcpy( szExtDefault , "*" );
	strcpy( szExtFilter , filter );
	l = strlen( szExtFilter );
	szExtFilter[l+1] = 0;

	if ( strlen( initdir ) > 0 ) {
		sprintf( Buffer1 , "%s/%s" , initdir , filter );
	}
	else {
		strcpy( Buffer1 , filter );
	}
	//
	l = strlen( Buffer1 );
	for ( i = 0 ; i < l ; i++ ) {
		if ( Buffer1[i] == '/' ) Buffer1[i] = '\\';
	}
	//
	strcpy( Buffer2 , "" );
	//
	ZeroMemory( &ofn, sizeof(ofn) );
	//
	ofn.lStructSize		= sizeof( OPENFILENAME );
	ofn.hwndOwner		= hWnd;
	//
	ofn.lpstrFilter		= szExtFilter;
	ofn.nFilterIndex	= 1;

	ofn.nMaxFile		= MAX_PATH;

	ofn.lpstrFile		= Buffer1;	// Full Name

	ofn.lpstrFileTitle	= Buffer2;	// File Name Only
	ofn.nMaxFileTitle	= Buffer2 ? MAX_PATH : 0;

	ofn.lpstrDefExt		= szExtDefault;

	ofn.lpstrTitle		= title; // Tile Display

	if ( Savemode ) {
		ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		if ( GetSaveFileName( &ofn ) ) {
			strncpy( FileName , Buffer1 , count );
			return TRUE;
		}
	}
	else {
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if ( GetOpenFileName( &ofn ) ) {
			strncpy( FileName , Buffer1 , count );
			return TRUE;
		}
	}
	return FALSE;
}
//

BOOL UTIL_CopyFile( BOOL lpExistingSHMEM , char *lpExistingFileName , BOOL lpNewSHMEM , char *lpNewFileName , BOOL bFailIfExists , int retrycount , BOOL hide , int *errorcode ) { // 2014.09.15
	int i;
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	int sizefind;
	DWORD sizehigh , sizelow;
	int bRet;
	//
	//----------------------------------------------------------------------------------------------
	//
	*errorcode = 0;
	//
	/*
	//
	if ( !CopyFile( lpExistingFileName , lpNewFileName , bFailIfExists ) ) {
		//
		*errorcode = GetLastError();
		//
		DeleteFile( lpNewFileName );
		//
		if ( !CopyFile( lpExistingFileName , lpNewFileName , bFailIfExists ) ) {
			//
			*errorcode = GetLastError();
			//
			return FALSE;
		}
	}
	//
	if ( hide ) SetFileAttributes( lpNewFileName , FILE_ATTRIBUTE_HIDDEN );
	//
	return TRUE;
	//
	*/
	if      ( lpExistingSHMEM && lpNewSHMEM ) { // 2015.07.20
		//
		i = _FILE2SM_COPY_SM_to_SM( lpExistingFileName , lpNewFileName );	// 0:OK 1:SmSrcNameFail 2:SmTrgNameFail 3:SmSrcOpenFail 4:SmTrgCreateFail 5:SmSrcUsing 6:SmTrgUsing 7:Smfileclose 8:SmMemMappingFail 9:SmCopySizeFail 10:LockFail(M) 11:LockFail
		//
		if ( i != 0 ) {
			*errorcode = -30000 - i;
			return FALSE;
		}
		//
		return TRUE;
		//
	}
	else if ( lpExistingSHMEM && !lpNewSHMEM ) { // 2015.07.20
		//
		i = _FILE2SM_COPY_SM_to_FILE( lpExistingFileName , lpNewFileName );		// 0:OK 1:SmfileOpenFail 2:SmfileNotFind 3:SmfileUsing 4:RealFileCreateFail 5:Smfileclose 6:SmfileReadFail 7:RealfileWriteFail 8:LockFail(M) 9:LockFail
		//
		if ( i != 0 ) {
			*errorcode = -30000 - i;
			return FALSE;
		}
		//
		return TRUE;
		//
	}
	else if ( !lpExistingSHMEM && lpNewSHMEM ) { // 2015.07.20
		//
		i = _FILE2SM_COPY_FILE_to_SM( lpExistingFileName , lpNewFileName );		// 0:OK 1:RealfileOpenFail 2:SmfileNoSpace 3:SmfileUsing 4:Smfileclose 5:SmfileWriteFail 6:LockFail(M) 7:LockFail
		//
		if ( i != 0 ) {
			*errorcode = -30000 - i;
			return FALSE;
		}
		//
		return TRUE;
		//
	}
	else {
		//----------------------------------------------------------------------------------------------
		//
		hFind = FindFirstFile( lpExistingFileName , &fd );
		//
		if ( hFind == INVALID_HANDLE_VALUE ) {
			*errorcode = -10001;
			return FALSE;
		}
		//
		sizefind = 0;
		//
		bRet = TRUE;
		//
		while ( bRet ) {
			//
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) { // file
				//
				sizefind++;
				//
				sizehigh = fd.nFileSizeHigh;
				sizelow  = fd.nFileSizeLow;
				//
				break;
				//
			}
			//
			bRet = FindNextFile( hFind , &fd );
		}
		//
		FindClose( hFind );
		//
		if ( sizefind == 0 ) {
			*errorcode = -10002;
			return FALSE;
		}
		//
		for ( i = 0 ; i < retrycount ; i++ ) { 
			//
			if ( i != 0 ) Sleep(1);
			//
			SetFileAttributes( lpNewFileName , FILE_ATTRIBUTE_NORMAL ); // 2016.09.06
			//
			if ( !CopyFile( lpExistingFileName , lpNewFileName , bFailIfExists ) ) {
				//
				*errorcode = GetLastError();
				//
			}
			else {
				//==========================================================================
				//
				hFind = FindFirstFile( lpNewFileName , &fd );
				//
				if ( hFind == INVALID_HANDLE_VALUE ) {
					*errorcode = -20001;
					continue;
				}
				//
				sizefind = 0;
				//
				bRet = TRUE;
				//
				while ( bRet ) {
					//
					if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) { // file
						//
						if ( sizehigh != fd.nFileSizeHigh ) break;
						if ( sizelow != fd.nFileSizeLow ) break;
						//
						sizefind = 1;
						//
						break;
						//
					}
					//
					bRet = FindNextFile( hFind , &fd );
				}
				//
				FindClose( hFind );
				//
				//==========================================================================
				//
				if ( sizefind == 1 ) {
					//
					if ( hide ) SetFileAttributes( lpNewFileName , FILE_ATTRIBUTE_HIDDEN );
					//
					return TRUE;
					//
				}
				//
				*errorcode = -20002;
				//
			}
			//
			DeleteFile( lpNewFileName );
			//
		}
	}
	//
	return FALSE;
}