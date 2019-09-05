#ifndef MULTIALIC_H
#define MULTIALIC_H


int  _i_SCH_MODULE_Get_MFIC_LAST_PLACED_SLOT();
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL _i_SCH_MODULE_Need_Do_Multi_FAL();
BOOL _i_SCH_MODULE_Need_Do_Multi_TAL();
BOOL _i_SCH_MODULE_Need_Do_Multi_FIC();

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi FIC Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_MFIC_SIDE_POINTER( int slot , int side , int pointer );
void _i_SCH_MODULE_Set_MFIC_SIDE( int slot , int side );
int  _i_SCH_MODULE_Get_MFIC_SIDE( int slot );
void _i_SCH_MODULE_Set_MFIC_POINTER( int slot , int pointer );
int  _i_SCH_MODULE_Get_MFIC_POINTER( int slot );
void _i_SCH_MODULE_Set_MFIC_WAFER( int slot , int wafer );
void _i_SCH_MODULE_Set_MFIC_WAFER_B( int slot , int wafer );
int  _i_SCH_MODULE_Get_MFIC_WAFER( int slot );
int  _i_SCH_MODULE_Get_MFIC_WAFER_B( int slot );

int  _i_SCH_MODULE_Get_MFIC_TIMECONTROL( int slot ); // 2011.05.19


int  _i_SCH_MODULE_Chk_MFIC_HAS_COUNT();
int  _i_SCH_MODULE_Chk_MFIC_FREE_COUNT();
int  _i_SCH_MODULE_Chk_MFIC_FREE2_COUNT( int s1 , int p1 , int s2 , int p2 ); // 2007.09.27
int  _i_SCH_MODULE_Chk_MFIC_FREE_SLOT( int *s1 , int *s2 , int refslot1 , int refslot2 );
int  _i_SCH_MODULE_Chk_MFIC_FREE_TYPE2_SLOT( int refslot , int offset , int *s1 , int *s2 );
int  _i_SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( int Placewaferside1 , int Placewaferpt1 , int Placewaferside2 , int Placewaferpt2 , int *s1 , int *s2 , int multiextarm );
int  _i_SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( BOOL sidenotfix , int side , int *side2 , int *s1 , int *s2 );
BOOL _i_SCH_MODULE_Chk_MFIC_MULTI_FREE( int sl , int count ); // 2007.08.02
BOOL _i_SCH_MODULE_Chk_MFIC_MULTI_FINISH( int sl , int count ); // 2007.08.02
int  _i_SCH_MODULE_Chk_MFIC_RUN_TIMER( int slot , int mode , time_t curtime );

void _i_SCH_MODULE_Set_MFIC_RUN_TIMER( int slot , int slot2  , int targettime , BOOL targetsetonly );
int  _i_SCH_MODULE_Get_MFIC_Completed_Wafer_FDHC( int side , int *side2 , int *slot , int *slot2 );
BOOL _i_SCH_MODULE_Chk_MFIC_Yes_for_Get_FDHC( int side );
BOOL _i_SCH_MODULE_Chk_MFIC_LOCKING( int side );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Multi FAL Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void _i_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( int side , int pointer , int );
void _i_SCH_MODULE_Set_MFAL_SIDE( int side );
int  _i_SCH_MODULE_Get_MFAL_SIDE();
void _i_SCH_MODULE_Set_MFAL_POINTER( int pointer );
int  _i_SCH_MODULE_Get_MFAL_POINTER();
int  _i_SCH_MODULE_Get_MFAL_BM();

void _i_SCH_MODULE_Set_MFAL_WAFER( int wafer );
int  _i_SCH_MODULE_Get_MFAL_WAFER();

void _i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( int slot , int side , int pointer , int bm );
void _i_SCH_MODULE_Set_MFALS_SIDE( int slot , int side );
void _i_SCH_MODULE_Set_MFALS_POINTER( int slot , int pointer );
void _i_SCH_MODULE_Set_MFALS_WAFER( int slot , int wafer );
void _i_SCH_MODULE_Set_MFALS_BM( int slot , int data );
void _i_SCH_MODULE_Set_MFALS_WAFER_B( int slot , int wafer );

int _i_SCH_MODULE_Get_MFALS_SIDE( int slot );
int _i_SCH_MODULE_Get_MFALS_POINTER( int slot );
int _i_SCH_MODULE_Get_MFALS_BM( int slot );
int _i_SCH_MODULE_Get_MFALS_WAFER( int slot );
int _i_SCH_MODULE_Get_MFALS_WAFER_B( int slot );


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#include "INF_sch_multi_alic.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_MFALIC_DATA( int , int ); // 2007.09.17

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Check_MFIC_Supply_Impossible( int side );

BOOL APIENTRY Gui_IDD_MCOOL_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );

//------------------------------------------------------------------------------------------

#endif
