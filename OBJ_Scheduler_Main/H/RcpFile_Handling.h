#ifndef RCPFILE_HANDLING_H
#define RCPFILE_HANDLING_H

#include "lottype.h"

#define	SEP_LEFT_BLACKET			11
#define	SEP_NAME_DATA				13
#define	SEP_RIGHT_BLACKET			12

//BOOL RECIPE_FILE_LOT_DATA_READ( int , int , LOTStepTemplate * , LOTStepTemplate2 * , LOTStepTemplate3 * , char * , int *lsp , char * , int , char *pj ); // 2015.10.12
//BOOL RECIPE_FILE_LOT_DATA_READ( int , int , LOTStepTemplate * , LOTStepTemplate2 * , LOTStepTemplate3 * , char * , int *lsp , char * , int , char *pj , BOOL *hasSlotFix ); // 2015.10.12 // 2016.07.13
//BOOL RECIPE_FILE_LOT_DATA_READ( int , int , LOTStepTemplate * , LOTStepTemplate2 * , LOTStepTemplate3 * , LOTStepTemplate_Dummy_Ex * , char * , int *lsp , char * , int , char *pj , BOOL *hasSlotFix ); // 2015.10.12 // 2016.07.13 // 2016.12.10
BOOL RECIPE_FILE_LOT_DATA_READ( BOOL , BOOL * , int , int , LOTStepTemplate * , LOTStepTemplate2 * , LOTStepTemplate3 * , LOTStepTemplate_Dummy_Ex * , char * , int *lsp , char * , int , char *pj , BOOL *hasSlotFix ); // 2015.10.12 // 2016.07.13 // 2016.12.10
BOOL RECIPE_FILE_CLUSTER_DATA_READ( int , CLUSTERStepTemplate2 * , CLUSTERStepTemplate3 * , char *grpname , int slot , char * , int reggrp , int *selgrp , BOOL *mgf , char *pj );
BOOL RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( int Locking , int side , int Chamber , char *Filename , int Style , int Wafer , int ID , char *wfile , char *TuneData );
int  RECIPE_FILE_PROCESS_File_Check( int LockingT , int LockingS , int side , int pt , int Chamber , int slot , char *Recipe , int mode , int etc );
void RECIPE_FILE_PROCESS_File_Locking_Delete( int Locking , int side );

void RECIPE_FILE_PROCESS_File_Locking_AutoDelete(); // 2013.09.13


int  RECIPE_FILE_GET_MAP_CLUSTERINDEX( int slot );
BOOL RECIPE_FILE_CLUSTER_DATA_READ_FOR_MAP( int side , CLUSTERStepTemplate2 *CLUSTER_RECIPE , CLUSTERStepTemplate3 *CLUSTER_RECIPE_SPECIAL , int Slot , int *selgrp , BOOL *mgf );

#endif
