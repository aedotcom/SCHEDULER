#ifndef SCH_MACRO_FLOW_H
#define SCH_MACRO_FLOW_H

//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>				// 2015.04.09

#define _WIN32_WINNT 0x0400		// 2010.08.05

#include <windows.h>
#include <winbase.h>
#include <time.h>
#include <direct.h>

extern int Inf_Dummy_PostWait_Use[32];
extern int Inf_Dummy_PostWait_Mode[32];
extern int Inf_Dummy_PostWait_Value[32];
extern char *Inf_Dummy_PostWait_Name[32];


void Init_Inf_Dummy_PostWait(void);
void Remove_Inf_Dummy_PostWait( int i );
int Get_Inf_Dummy_PostWait( int side , int pointer , int *dm ) ;
void Set_Inf_Dummy_PostWait( int side , int pointer , int m , int v , char *name );
void _SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( int side , int pointer , int firsttag , int lasttag , int cmstarteveryskip );
int _SCH_inside_Get_Dummy_Muti_Wait_UsingPM( int side , int pointer , int ch , int *order );
int _SCH_inside_Get_Dummy_Use_OnePM( int side , int pointer , int ch );
int _SCH_inside_Get_DLL_Dummy( int postwait , int side , int pointer , int mode , int selpm , int selparam , int *dummychamber , int *dummyslot , char *SelectRecipe );
void _SCH_inside_ConvStr_29_Dummy( char *SelectRecipe ) ;
int _SCH_inside_Get_Dummy( int side , int pointer , int mode , int tag2 , int tag3 , int cmd , int curnotuse );
int _SCH_inside_Unused_Remap_Dummy( int side , int pointer , int *orgrange );
int _SCH_inside_Make_Dummy( int side , int pointer );
void _SCH_inside_Ordering_Invalid_OK( int side ) ;
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_FOR_LOG( int side );
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( int side , int *cm , int *pt , int subchk );
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_CM( int side , int *cm , int *pt ) ;
int _i_SCH_USER_POSSIBLE_PICK_FROM_FM( int side , int *pt );
int _i_SCH_USER_POSSIBLE_PICK_FROM_TM( int side , int *pt );
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK( int side , int *cm , int *pt );
int _i_SCH_SUB_POSSIBLE_PICK_FROM_FM_NOSUPPLYCHECK_U( int side , int *cm , int *pt );
void _SCH_MACRO_FM_SUPPOSING_PICK_FOR_OTHERSIDE( int side , int count );
void _SCH_MACRO_OTHER_LOT_LOAD_WAIT( int side , unsigned int timeoutsec );
//-----------------------------------------------------------------------------------
//extern char COMMON_BLANK_CHAR[2]; // 2008.04.02
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#endif
