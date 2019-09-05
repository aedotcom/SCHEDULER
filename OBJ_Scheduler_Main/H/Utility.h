#ifndef UTILITY_H
#define UTILITY_H

void UTIL_EXTRACT_UPFOLDER( char *folder , char *res );
void UTIL_CHANGE_FILE_VALID_STYLE( char *data );
void UTIL_EXTRACT_GROUP_FILE( char *OrgGroup , char *OrgFile , char *RunGroup , int RunGroupCnt , char *RunFile , int RunFileCnt );
void UTIL_MAKE_DYNAMIC_GROUP_FILE( char *TargetFile , char *SourceFile , int count );
//
BOOL UTIL_Get_OpenSave_File_Select( HWND hWnd , BOOL , char *FileName , int count , char *initdir , char *filter , char *title );
void UTIL_Extract_Reset_String( char *source );
void UTIL_Copy_Multi_Recipe_Valid_String_Count( char *target , char *source , int count );
BOOL UTIL_CopyFile( BOOL lpExistingSHMEM , char *lpExistingFileName , BOOL lpNewSHMEM , char *lpNewFileName , BOOL bFailIfExists , int retrycount , BOOL hide , int *errorcode );

#endif

