#ifndef SCH_PRM_FBTPM_H
#define SCH_PRM_FBTPM_H

//----------------------------------------------------------------------------
void INIT_SCHEDULER_PRM_FBTPM_DATA( int apmode , int side );
//----------------------------------------------------------------------------
void _i_SCH_MODULE_Init_FBTPMData( int side );
//------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_FM_SIDE_WAFER( int arm , int side , int wafer );
void _i_SCH_MODULE_Set_FM_SIDE( int arm , int side );
int  _i_SCH_MODULE_Get_FM_SIDE( int arm );
void _i_SCH_MODULE_Set_FM_WAFER( int arm , int wafer ); // 2013.01.11
int  _i_SCH_MODULE_Get_FM_WAFER( int arm );
int  _i_SCH_MODULE_Get_FM_WAFER_B( int arm );
void _i_SCH_MODULE_Set_FM_POINTER( int arm , int data );
int  _i_SCH_MODULE_Get_FM_POINTER( int arm );
void _i_SCH_MODULE_Set_FM_MODE( int arm , int data );
int  _i_SCH_MODULE_Get_FM_MODE( int arm );
void _i_SCH_MODULE_Set_FM_POINTER_MODE( int arm , int data , int mode );


void _i_SCH_MODULE_Set_BM_FULL_MODE( int ch , int Data );
int  _i_SCH_MODULE_Get_BM_FULL_MODE( int ch );
void _i_SCH_MODULE_Set_BM_SIDE_POINTER( int ch , int slot , int side , int pointer );
void _i_SCH_MODULE_Set_BM_SIDE( int ch , int slot , int side );
int  _i_SCH_MODULE_Get_BM_SIDE( int ch , int slot );
void _i_SCH_MODULE_Set_BM_POINTER( int ch , int slot , int pointer );
int  _i_SCH_MODULE_Get_BM_POINTER( int ch , int slot );
void _i_SCH_MODULE_Set_BM_WAFER_STATUS( int ch , int slot , int wafer , int status );
void _i_SCH_MODULE_Set_BM_STATUS( int ch , int slot , int status );
int  _i_SCH_MODULE_Get_BM_STATUS( int ch , int slot );
int  _i_SCH_MODULE_Get_BM_WAFER( int ch , int slot );
int  _i_SCH_MODULE_Get_BM_WAFER_B( int ch , int slot );

void _i_SCH_MODULE_Set_TM_Switch_Signal( int TMATM , int Data );
int  _i_SCH_MODULE_Get_TM_Switch_Signal( int TMATM );
void _i_SCH_MODULE_Set_TM_Move_Signal( int TMATM , int Data );
int  _i_SCH_MODULE_Get_TM_Move_Signal( int TMATM );

void _i_SCH_MODULE_Set_TM_SIDE_POINTER( int TMATM , int finger , int side , int pointer , int side2 , int pointer2 );
void _i_SCH_MODULE_Set_TM_SIDE_POINTER1( int TMATM , int finger , int side , int pointer );
void _i_SCH_MODULE_Set_TM_SIDE_POINTER2( int TMATM , int finger , int side2 , int pointer2 );

void _i_SCH_MODULE_Set_TM_SIDE( int TMATM , int finger , int side );
void _i_SCH_MODULE_Set_TM_POINTER( int TMATM , int finger , int pointer );
int  _i_SCH_MODULE_Get_TM_SIDE( int TMATM , int finger );
int  _i_SCH_MODULE_Get_TM_SIDE2( int TMATM , int finger );
int  _i_SCH_MODULE_Get_TM_POINTER( int TMATM , int finger );
int  _i_SCH_MODULE_Get_TM_POINTER2( int TMATM , int finger );
int  _i_SCH_MODULE_Get_TM_PATHORDER( int TMATM , int finger );
void _i_SCH_MODULE_Set_TM_PATHORDER( int TMATM , int finger , int data );
int  _i_SCH_MODULE_Get_TM_TYPE( int TMATM , int finger );
void _i_SCH_MODULE_Set_TM_TYPE( int TMATM , int finger , int data );
int  _i_SCH_MODULE_Get_TM_WAFER( int TMATM , int finger );
int  _i_SCH_MODULE_Get_TM_WAFER_B( int TMATM , int finger );
void _i_SCH_MODULE_Set_TM_WAFER( int TMATM , int finger , int data );
int  _i_SCH_MODULE_Get_TM_OUTCH( int TMATM , int finger );
void _i_SCH_MODULE_Set_TM_OUTCH( int TMATM , int finger , int data );

void _i_SCH_MODULE_Set_PM_SIDE_POINTER( int chamber , int side , int pointer , int side2 , int pointer2 );
void _i_SCH_MODULE_Set_PM_SIDE( int chamber , int depth , int side );
void _i_SCH_MODULE_Set_PM_POINTER( int chamber , int depth , int pointer );
void _i_SCH_MODULE_Set_PM_PATHORDER( int chamber , int depth , int data );
int  _i_SCH_MODULE_Get_PM_SIDE( int chamber , int depth );
int  _i_SCH_MODULE_Get_PM_POINTER( int chamber , int depth );
int  _i_SCH_MODULE_Get_PM_PATHORDER( int chamber , int depth );
int  _i_SCH_MODULE_Get_PM_WAFER( int chamber , int depth );
int  _i_SCH_MODULE_Get_PM_WAFER_B( int chamber , int depth );
void _i_SCH_MODULE_Set_PM_WAFER( int chamber , int depth , int data );
int  _i_SCH_MODULE_Get_PM_STATUS( int chamber , int depth );
void _i_SCH_MODULE_Set_PM_STATUS( int chamber , int depth , int data );

int  _i_SCH_MODULE_Get_PM_PICKLOCK( int chamber , int depth ); // 2014.01.10
void _i_SCH_MODULE_Set_PM_PICKLOCK( int chamber , int depth , int data ); // 2014.01.10
//==============================================================================================================
void _i_SCH_MODULE_Set_FM_Runinig_Flag( int side , int data );
int	 _i_SCH_MODULE_Get_FM_Runinig_Flag( int side );
void _i_SCH_MODULE_Set_FM_OutCount( int side , int data );
void _i_SCH_MODULE_Inc_FM_OutCount( int side );
int  _i_SCH_MODULE_Get_FM_OutCount( int side );
void _i_SCH_MODULE_Set_FM_InCount( int side , int data );
void _i_SCH_MODULE_Inc_FM_InCount( int side );
int  _i_SCH_MODULE_Get_FM_InCount( int side );
void _i_SCH_MODULE_Set_FM_LastOutPointer( int side , int data );
int  _i_SCH_MODULE_Get_FM_LastOutPointer( int side );
int  _i_SCH_MODULE_Get_FM_SupplyCount( int side ); // 2014.12.22
//--------------------------------------------------------------------------------------------------------------------------
void _i_SCH_MODULE_Set_FM_DoPointer_FDHC( int side , int data );
void _i_SCH_MODULE_Inc_FM_DoPointer_FDHC( int side );
int  _i_SCH_MODULE_Get_FM_DoPointer_FDHC( int side );
void _i_SCH_MODULE_Set_FM_End_Status_FDHC( int side );
BOOL _i_SCH_MODULE_Chk_FM_Finish_Status_FDHC( int side );
//
void _i_SCH_MODULE_Enter_FM_DoPointer_Sub();
void _i_SCH_MODULE_Leave_FM_DoPointer_Sub();
void _i_SCH_MODULE_Set_FM_DoPointer_Sub( int side , int data );
void _i_SCH_MODULE_Inc_FM_DoPointer_Sub( int side );
int  _i_SCH_MODULE_Get_FM_DoPointer_Sub( int side );
void _i_SCH_MODULE_Set_FM_End_Status_Sub( int side );
BOOL _i_SCH_MODULE_Chk_FM_Finish_Status_Sub( int side );
//--------------------------------------------------------------------------------------------------------------------------
BOOL _i_SCH_MODULE_Chk_FM_Free_Status( int side );
void _i_SCH_MODULE_Set_FM_HWait( int side , BOOL data );
BOOL _i_SCH_MODULE_Get_FM_HWait( int side );
void _i_SCH_MODULE_Set_FM_SwWait( int side , int data );
int  _i_SCH_MODULE_Get_FM_SwWait( int side );
void _i_SCH_MODULE_Set_FM_MidCount( int data );
int  _i_SCH_MODULE_Get_FM_MidCount();

void _i_SCH_MODULE_Set_BM_MidCount( int data );
int  _i_SCH_MODULE_Get_BM_MidCount();

void _i_SCH_MODULE_Set_TM_DoPointer( int side , int data );
void _i_SCH_MODULE_Inc_TM_DoPointer( int side );
int  _i_SCH_MODULE_Get_TM_DoPointer( int side );
void _i_SCH_MODULE_Set_TM_InCount( int side , int data );
void _i_SCH_MODULE_Inc_TM_InCount( int side );
int  _i_SCH_MODULE_Get_TM_InCount( int side );
void _i_SCH_MODULE_Set_TM_OutCount( int side , int data );
void _i_SCH_MODULE_Inc_TM_OutCount( int side );
int  _i_SCH_MODULE_Get_TM_OutCount( int side );
void _i_SCH_MODULE_Set_TM_End_Status( int side );
BOOL _i_SCH_MODULE_Chk_TM_Finish_Status( int side );
BOOL _i_SCH_MODULE_Chk_TM_Free_Status( int side );
BOOL _i_SCH_MODULE_Chk_TM_Free_Status2( int side , int check );
int  _i_SCH_MODULE_Get_TM_SupplyCount( int side ); // 2014.12.22


void _i_SCH_MODULE_Set_TM_DoubleCount( int side , int data );
void _i_SCH_MODULE_Inc_TM_DoubleCount( int side );
void _i_SCH_MODULE_Dec_TM_DoubleCount( int side );
int  _i_SCH_MODULE_Get_TM_DoubleCount( int side );

int  _i_SCH_MODULE_GET_USE_ENABLE( int Chamber , BOOL Always , int side );
BOOL _i_SCH_MODULE_GROUP_FM_POSSIBLE( int Chamber , int mode , int prcs );
BOOL _i_SCH_MODULE_GROUP_TBM_POSSIBLE( int grp , int Chamber , int mode , int mdcheck );
BOOL _i_SCH_MODULE_GROUP_TPM_POSSIBLE( int grp , int Chamber , int mode );


BOOL _i_SCH_MODULE_GROUP_FM_POSSIBLE_EX( int Chamber , int mode , int prcs , BOOL fmchk );
BOOL _i_SCH_MODULE_GROUP_TBM_POSSIBLE_EX( int grp , int Chamber , int mode , int mdcheck , BOOL tmchk );
BOOL _i_SCH_MODULE_GROUP_TPM_POSSIBLE_EX( int grp , int Chamber , int mode , BOOL tmchk );

int  _i_SCH_MODULE_GET_MOVE_MODE( int Chamber ); // 2013.09.03
void _i_SCH_MODULE_SET_MOVE_MODE( int Chamber , int data ); // 2013.09.03

//
// 2015.08.26
//
void SCH_TM_PICKPLACING_SET( int ch , int pick , int tms , int arm , int side , int pointer );
//BOOL SCH_TM_PICKPLACING_GET( int ch ); // 2015.09.18
int  SCH_TM_PICKPLACING_GET( int ch ); // 2015.09.18
//void SCH_TM_PICKPLACING_PRCS( int ch ); // 2016.03.22
void SCH_TM_PICKPLACING_PRCS( int ch , BOOL post ); // 2016.03.22


#endif

