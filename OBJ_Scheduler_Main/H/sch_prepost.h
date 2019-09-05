#ifndef SCH_PREPOST_H
#define SCH_PREPOST_H

#define MAKE_DEFAULT_INT_PARAMETER3_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data , int data2 , int data3 ); \
	int  _i_SCH_PRM_GET_##ITEM##( int data , int data2 );

#define MAKE_DEFAULT_CHAR_PARAMETER3_HEADER( ITEM ) void _i_SCH_PRM_SET_##ITEM##( int data0 , int data , char *data2 ); \
	char *_i_SCH_PRM_GET_##ITEM##( int data0 , int data );

//----------------------------------------------------------------------------
void INIT_SCHEDULER_CONTROL_PREPOST_DATA( int , int );
//----------------------------------------------------------------------------
void Scheduler_Module_Use_Reference_Data_Refresh( int CHECKING_SIDE , int mode );
//int  Scheduler_HandOffIn_Part( int CHECKING_SIDE , int ch1 , int ch2 );

void Scheduler_LotPre_Data_Set( BOOL normal , int side , int ch , char *processname , int processtype ); // 2017.10.18
void Scheduler_LotPre_Data_Supply( int side , int pointer ); // 2017.10.19

int Scheduler_LotPre_Check_Setting_Part( BOOL rand , int CHECKING_SIDE , int ch ); // 2017.10.19

int  Scheduler_Begin_Part( int CHECKING_SIDE , char *errorstring );
int  Scheduler_Begin_Before_Wait_Part( int CHECKING_SIDE , char *errorstring ); // 2005.08.19
int  Scheduler_Begin_After_Wait_Part( int CHECKING_SIDE , char *errorstring , BOOL seqrerun ); // 2005.02.21
int  Scheduler_End_Part( int CHECKING_SIDE );
int  Scheduler_End_Post_Part( int CHECKING_SIDE );
void Scheduler_End_Part_Lock_Make( int ); // 2004.03.11
void Scheduler_End_Part_Lock_Clear( int ); // 2004.03.11
int  Scheduler_Begin_One( int CHECKING_SIDE , int ch );
int  Scheduler_Begin_Restart_Part( int CHECKING_SIDE , char *errorstring );

void Scheduler_Restart_Process_Part( int CHECKING_SIDE ); // 2011.12.15
void Scheduler_Restart_Process_Set( int CHECKING_SIDE , int pt , int ch ); // 2011.12.15

//int  Scheduler_HandOffOut_Part( int CHECKING_SIDE , int run , int *ch1 , int *ch2 );

void Scheduler_Randomize_After_Waiting_Part( int CHECKING_SIDE , BOOL pralskip ); // 2006.02.28

void Scheduler_All_Abort_Part( BOOL runstyle );

void Scheduler_End_Set_Delay_Time( DWORD timedata ); // 2003.08.04

MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_READY_RECIPE_STEP2   );

MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_READY_RECIPE_USE     );
MAKE_DEFAULT_CHAR_PARAMETER3_HEADER( inside_READY_RECIPE_NAME    );
MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_READY_RECIPE_MINTIME );

MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_READY_RECIPE_TYPE    ); // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( inside_READY_RECIPE_NAME_LAST ); // 2017.09.07
MAKE_DEFAULT_INT_PARAMETER2_HEADER( inside_READY_RECIPE_TYPE_LAST ); // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2_HEADER( inside_READY_RECIPE_INDX_LAST ); // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2_HEADER( inside_READY_RECIPE_NAME_OLD_LAST ); // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2_HEADER( inside_READY_RECIPE_TYPE_OLD_LAST ); // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2_HEADER( inside_READY_RECIPE_INDX_OLD_LAST ); // 2017.10.16

MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_END_RECIPE_USE       );
MAKE_DEFAULT_CHAR_PARAMETER3_HEADER( inside_END_RECIPE_NAME      );
MAKE_DEFAULT_INT_PARAMETER3_HEADER(  inside_END_RECIPE_MINTIME   );

//
// 2016.12.09
MAKE_DEFAULT_INT_PARAMETER3_HEADER( inside_SIM_TIME1 )
MAKE_DEFAULT_INT_PARAMETER3_HEADER( inside_SIM_TIME2 )
MAKE_DEFAULT_INT_PARAMETER3_HEADER( inside_SIM_TIME3 )

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int   _i_SCH_PREPOST_Get_inside_READY_RECIPE_USE( int data , int data2 );
char *_i_SCH_PREPOST_Get_inside_READY_RECIPE_NAME( int data0 , int data );
int   _i_SCH_PREPOST_Get_inside_END_RECIPE_USE( int data , int data2 );
char *_i_SCH_PREPOST_Get_inside_END_RECIPE_NAME( int data0 , int data );
int   _i_SCH_PREPOST_Get_inside_READY_RECIPE_TYPE( int data , int data2 ); // 2017.10.16
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_PREPOST_Randomize_End_Part( int side , int Chamber );
int  _i_SCH_PREPOST_Randomize_End_Part2( int side , int Chamber , BOOL );
void _i_SCH_PREPOST_LotPost_Part_for_All_BM_Return( int CHECKING_SIDE ); // 2007.04.06
BOOL _i_SCH_PREPOST_Pre_End_Part( int CHECKING_SIDE , int Chamber );
BOOL _i_SCH_PREPOST_Pre_End_Force_Part( int CHECKING_SIDE , int Chamber );
//------------------------------------------------------------------------------------------



#endif

