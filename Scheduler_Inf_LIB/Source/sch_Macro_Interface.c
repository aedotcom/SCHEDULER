#include "Macro.h"

#ifndef _REFRENCE

//===========================================================================================================================
// MACRO EQUIPMENT PART
//===========================================================================================================================

DEFINE_SCHEDULER_MACRO_VAR_ARG4		( int	,	FM_POSSIBLE_PICK_FROM_FM			, int , side , int * , cm , int * , pt , int , subcheck , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG2		( void	,	FM_SUPPOSING_PICK_FOR_OTHERSIDE		, int , side , int , count , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG9		( int	,	FM_PICK_DATA_FROM_FM				, int , fms , BOOL , doubletm , int , mode , int , CHECKING_SIDE , int * , FM_Slot , int * , FM_Slot2 , int * , FM_Pointer , int * , FM_Pointer2 , int * , retpointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG12	( int	,	FM_PICK_DATA_FROM_FM_DETAIL			, int , fms , BOOL , doubletm , int , mode , int , CHECKING_SIDE , int * , FM_Slot , int * , FM_Slot2 , int * , FM_Pointer , int * , FM_Pointer2 , int * , retpointer , int * , FM_HSide , int * , FM_HSide2 , int * , CP_Reject , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG5		( void	,	FM_SUPPLY_FIRSTLAST_CHECK			, int , side , int , pointer , int , firsttag , int , lasttag , int , cmstarteveryskip , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG2		( BOOL	,	FM_FINISHED_CHECK					, int , side , BOOL , supplycheck , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG3		( int	,	INTERRUPT_PART_CHECK				, int , side , BOOL , patial , int , tmthreadblockingcheck , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	FM_OPERATION						, int , fms , int , mode , int , ch1 , int , slot1 , int , mslot1 , int , side1 , int , pointer1 , int , ch2 , int , slot2 , int , mslot2 , int , side2 , int , pointer2 , int , fs , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG14	( void	,	FM_DATABASE_OPERATION				, int , fms , int , mode , int , och1 , int , slot1 , int , omslot1 , int , side1 , int , pointer1 , int , och2 , int , slot2 , int , omslot2 , int , side2 , int , pointer2 , int , mode1 , int , mode2 , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG10	( int	,	FM_PLACE_TO_CM						, int , CHECKING_SIDE , int , FM_TSlot0 , int , FM_Side , int , FM_Pointer , int , FM_TSlot20 , int , FM_Side2 , int , FM_Pointer2 , BOOL , errorout , BOOL , disappear , int , realslotoffset , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_0						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_1						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_2						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_3						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_4						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_5						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_6						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG13	( int	,	TM_OPERATION_7						, int , mode , int , side , int , pointer , int , ch , int , arm , int , slot , int , depth , int , Switch , int , incm , BOOL , ForceOption , int , PrePostPartSkip , int , dide , int , dointer , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_0					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_1					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_2					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_3					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_4					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_5					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_6					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	TM_MOVE_OPERATION_7					, int , side , int , mode , int , ch , int , arm , int , slot , int , depth , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG12	( BOOL	,	TM_DATABASE_OPERATION				, int , tms , int , mode , int , och , int , arm , int , slot1 , int , slot2 , int , side1 , int , pointer1 , int , side2 , int , pointer2 , int , order , int , type , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG1		( int	,	CHECK_PROCESSING					, int , ch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG1		( int	,	CHECK_PROCESSING_INFO				, int , ch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG4		( int	,	CHECK_PROCESSING_GET_REMAIN_TIME	, int , ch , int , wafer , int * , tdata , int , willch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	MAIN_PROCESS_PART_SIMPLE			, int , side , int , pointer , int , pathdo , int , ch , int , arm , int , optdata , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG7		( int	,	MAIN_PROCESS_PART_MULTIPLE			, int , side , int , pointer , int , pathdo , int , ch , int , arm , int , firstnotuse , int , optdata , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG8		( void	,	PROCESS_PART_TAG_SETTING			, int , ch , int , status , int , side , int , pointer , int , no1 , int , no2 , char * , recipe , int , last , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG12	( int	,	LOTPREPOST_PROCESS_OPERATION		, int , side , int , ch , int , incm , int , slot , int , arm , char * , recipe , int , modeid , int , puttime , char * , nextpm , int , mintime , int , mode , int , optdata , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG4		( int	,	PRE_PROCESS_OPERATION				, int , side , int , pt , int , arm , int , ch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG4		( int	,	POST_PROCESS_OPERATION				, int , side , int , pt , int , arm , int , ch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG3		( BOOL	,	HAS_PRE_PROCESS_OPERATION			, int , side , int , pt , int , ch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG7		( void	,	SPAWN_WAITING_BM_OPERATION			, int , mode , int , side , int , ch , int , option , BOOL , modeset , int , logwhere , int , logdata , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG3		( int	,	FM_POSSIBLE_PICK_FROM_CM			, int , side , int * , cm , int * , pt , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG1		( BOOL	,	FM_OPERATION_THREAD_WAITING			, int , fms , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG1		( int	,	FM_OPERATION_THREAD_RESULT			, int , fms , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG8		( BOOL	,	FM_OPERATION_MOVE_THREAD_STARTING	, int , fms , int , slot , int , side , int , pointer , int , pick_mdl , int , pick_slot , int , place_mdl , int , place_slot , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG8		( BOOL	,	FM_OPERATION_MOVE_STARTING			, int , fms , int , slot , int , side , int , pointer , int , pick_mdl , int , pick_slot , int , place_mdl , int , place_slot , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG11	( void	,	PLACE_TO_CM_POST_PART_with_FM		, int , FM_TSlot , int , FM_CO1 , int , FM_OSlot , int , FM_Side , int , FM_Pointer , int , FM_TSlot2 , int , FM_CO2 , int , FM_OSlot2 , int , FM_Side2 , int , FM_Pointer2 , BOOL , NormalMode , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG7		( int	,	RUN_WAITING_BM_OPERATION			, int , mode , int , side , int , ch , int , option , BOOL , modeset , int , logwhere , int , logdata , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG2		( void	,	OTHER_LOT_LOAD_WAIT					, int , side , unsigned int , timeoutsec , (void) );
DEFINE_SCHEDULER_MACRO_VAR_ARG5		( int	,	SPAWN_FM_MALIGNING					, int , side , int , Slot1 , int , Slot2 , int , SrcCas , int , swch , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG2		( int	,	CHECK_FM_MALIGNING					, int , side , BOOL , waitfinish , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	SPAWN_FM_MCOOLING					, int , side , int , icslot1 , int , icslot2 , int , Slot1 , int , Slot2 , int , SrcCas , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG3		( int	,	CHECK_FM_MCOOLING					, int , side , int , icslot , BOOL , waitfinish , return ); 
DEFINE_SCHEDULER_MACRO_VAR_ARG4		( int	,	SPAWN_TM_MALIGNING					, int , side , int , Finger , int , Slot , int , SrcCas , return );
DEFINE_SCHEDULER_MACRO_VAR_ARG2		( int	,	CHECK_TM_MALIGNING					, int , side , BOOL , waitfinish ,  return );
DEFINE_SCHEDULER_MACRO_VAR_ARG6		( int	,	FM_MOVE_OPERATION					, int , fms , int , side , int , mode , int , ch , int , slot1 , int , slot2 , return );
//----------------
typedef int (  *_API_DEFINE_MAIN_PROCESS ) ( int side , int pointer , int ch , int rch , int slot , int pslot , int arm , int incm , char *recipe , int tmruntime , char *NextPMStr , int mintime , int prctag1 , int prctag2 , int prctag3 , int dide , int dointer , int pmstrtopt , int multiwfr , int actionoption , int optdata );
DEFINE_SCHEDULER_MACRO_DEFINF_API( _API_DEFINE_MAIN_PROCESS , _API_MAIN_PROCESS , _SCH_INF_DLL_API_MAIN_PROCESS )
int _SCH_MACRO_MAIN_PROCESS( int side , int pointer , int ch , int rch , int slot , int pslot , int arm , int incm , char *recipe , int tmruntime , char *NextPMStr , int mintime , int prctag1 , int prctag2 , int prctag3 , int dide , int dointer , int pmstrtopt , int multiwfr , int actionoption , int optdata ) {
	return (*_API_MAIN_PROCESS)( side , pointer , ch , rch , slot , pslot , arm , incm , recipe , tmruntime , NextPMStr , mintime , prctag1 , prctag2 , prctag3 , dide , dointer , pmstrtopt , multiwfr , actionoption , optdata );
}
//----------------
typedef int (  *_API_DEFINE_FM_ALIC_OPERATION )( int fms , int Mode , int side , int chamber , int alic_slot1 , int alic_slot2 , int SrcCas , int swch , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int side1 , int side2 , int pointer , int pointer2 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( _API_DEFINE_FM_ALIC_OPERATION , _API_FM_ALIC_OPERATION , _SCH_INF_DLL_API_FM_ALIC_OPERATION )
int _SCH_MACRO_FM_ALIC_OPERATION ( int fms , int Mode , int side , int chamber , int alic_slot1 , int alic_slot2 , int SrcCas , int swch , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int side1 , int side2 , int pointer , int pointer2 ) {
	return	(*_API_FM_ALIC_OPERATION)( fms , Mode , side , chamber , alic_slot1 , alic_slot2 , SrcCas , swch , insidelog , sourcewaferidA , sourcewaferidB , side1 , side2 , pointer , pointer2 );
}
//----------------



//===========================================================================================================================
// CIMS INTERFACE PART
//===========================================================================================================================

typedef void (  *_API_DEFINE_IO_CIM_PRINTF ) ( LPSTR list , ... );
DEFINE_SCHEDULER_MACRO_DEFINF_API( _API_DEFINE_IO_CIM_PRINTF , _SCH_INF_CIM_CONTROL_IO_CIM_PRINTF_API , _SCH_INF_DLL_API_IO_CIM_PRINTF )
//----------------
typedef void (  *_API_DEFINE_IO_COMMON_PRINTF ) ( BOOL Display , char *idstr , char *SeparateSting , LPSTR list , ... );
DEFINE_SCHEDULER_MACRO_DEFINF_API( _API_DEFINE_IO_COMMON_PRINTF , _SCH_INF_CIM_CONTROL_IO_COMMON_PRINTF_API , _SCH_INF_DLL_API_IO_COMMON_PRINTF )
//----------------
typedef void (  *_API_DEFINE_IO_CONSOLE_PRINTF ) ( LPSTR list , ... );
DEFINE_SCHEDULER_MACRO_DEFINF_API( _API_DEFINE_IO_CONSOLE_PRINTF , _SCH_INF_CIM_CONTROL_IO_CONSOLE_PRINTF_API , _SCH_INF_DLL_API_IO_CONSOLE_PRINTF )
//----------------
DEFINE_SCHEDULER_MACRO_VDNx_ARG0	( BOOL	,	MANAGER_ABORT				, return );
DEFINE_SCHEDULER_MACRO_VDNx_ARG0	( void	,	SYSTEM_ALL_ABORT			, (void) );
//----------------
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( int	,	_FIND_FROM_STRING			, int , IO_Type , char * , Name , return );
//----------------
// Digital IO
//----------------
DEFINE_SCHEDULER_MACRO_VDN_ARG3		( void	,	_dWRITE_DIGITAL				, int , IO_Point , int , Write_Value, int * , Result_Status , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( int	,	_dREAD_DIGITAL				, int , IO_Point , int * , Result_Status , return );
//----------------
// Analog IO
//----------------
DEFINE_SCHEDULER_MACRO_VDN_ARG3		( void	,	_dWRITE_ANALOG				, int , IO_Point , double , Write_Value, int * , Result_Status , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( int	,	_dREAD_ANALOG				, int , IO_Point , int * , Result_Status , return );
//----------------
// String IO
//----------------
DEFINE_SCHEDULER_MACRO_VDN_ARG3		( void	,	_dWRITE_STRING				, int , IO_Point , char * , Write_Value, int * , Result_Status , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG3		( void	,	_dREAD_STRING				, int , IO_Point , char * , Read_Value , int * , Result_Status , (void) );
//----------------
// Function Management
//----------------
// <Client>
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( BOOL	,	_dRUN_SET_FUNCTION			, int , Function_Point , char * , Func_Param , return );
DEFINE_SCHEDULER_MACRO_VDN_ARG1		( MdlSts,	_dREAD_FUNCTION				, int , Function_Point , return );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( MdlSts,	_dRUN_FUNCTION				, int , Function_Point , char * , Func_Param , return );
DEFINE_SCHEDULER_MACRO_VDN_ARG1		( void	,	_dRUN_FUNCTION_ABORT			, int , Function_Point , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( void	,	_dRUN_SET_FUNCTION_FREE		, int , Function_Point , char * , Func_Param , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( MdlSts,	_dRUN_FUNCTION_FREE			, int , Function_Point , char * , Func_Param , return );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( int	,	_dWRITE_FUNCTION				, int , Function_Point , char * , Func_Param , return );
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( void	,	_dWRITE_FUNCTION_EVENT		, int , Function_Point , char * , Event , (void) );
DEFINE_SCHEDULER_MACRO_VDN_ARG1		( int	,	_dREAD_FUNCTION_EVENT		, int , Function_Point , return );
//
DEFINE_SCHEDULER_MACRO_VDN_ARG2		( void	,	_dREAD_UPLOAD_MESSAGE		, int , Function_Point , char * , Return_Message , (void) );
//
// <Server>
DEFINE_SCHEDULER_MACRO_VDN_ARG1		( BOOL	,	_dWRITE_UPLOAD_MESSAGE		, char * , Set_Message , return );
//----------------
#endif



//===========================================================================================================================
// SCHEDULER EXTERN INTERFACE PART
//===========================================================================================================================
typedef LPARAM (*TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A , _SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_A_API , _SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_A )
//----------------
typedef LPARAM (*TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B , _SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_B_API , _SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_B )
//----------------
typedef LPARAM (*TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C , _SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_C_API , _SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_C )
//----------------
typedef LPARAM (*TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 , LPARAM a4 , LPARAM a5 , LPARAM a6 , LPARAM a7 , LPARAM a8 , LPARAM a9 , LPARAM a10 , LPARAM a11 , LPARAM a12 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D , _SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_D_API , _SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_D )
//----------------
typedef LPARAM (*TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E)( int m , LPARAM a1 , LPARAM a2 , LPARAM a3 );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E , _SCH_INF_DLL_CONTROL_EXTERN_INTERFACE_E_API , _SCH_INF_DLL_API_CONTROL_EXTERN_INTERFACE_E )
//----------------


#ifndef _REFRENCE

//===========================================================================================================================
// SCHEDULER LOG INTERFACE PART
//===========================================================================================================================
typedef void   (*TypeREM_SCH_INF_LOG_CONTROL_LOT_PRINTF_API)			( int , LPSTR , ... );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_LOG_CONTROL_LOT_PRINTF_API , _SCH_INF_LOG_CONTROL_LOT_PRINTF_API , _SCH_INF_DLL_API_LOG_CONTROL_LOT_PRINTF )
//----------------
typedef void   (*TypeREM_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API)			( int , int , LPSTR , ... );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API , _SCH_INF_LOG_CONTROL_DEBUG_PRINTF_API , _SCH_INF_DLL_API_LOG_CONTROL_DEBUG_PRINTF )
//----------------
typedef void   (*TypeREM_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API)			( int , int , int , int , int , int , int , char * , char * );
DEFINE_SCHEDULER_MACRO_DEFINF_API( TypeREM_SCH_INF_LOG_CONTROL_TIMER_PRINTF_API , _SCH_INF_LOG_CONTROL_TIMER_PRINTF_API , _SCH_INF_DLL_API_LOG_CONTROL_TIMER_PRINTF )
//----------------

//===========================================================================================================================
// SCHEDULER USER FLOW INTERFACE PART(CALLBACK)
//===========================================================================================================================
DEFINE_SCHEDULER_MACRO_FDN_ARG2		( int	,	_SCH_INF_DLL_API_ENTER_CONTROL_FEM_PART			, int , CHECKING_SIDE, int , mode , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_FEM_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG2		( int	,	_SCH_INF_DLL_API_ENTER_CONTROL_TM_PART			, int , CHECKING_SIDE, int , mode , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_1_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_2_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_3_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_4_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_5_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_6_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_7_PART			, int , CHECKING_SIDE , return );
DEFINE_SCHEDULER_MACRO_FDN_ARG1		( BOOL	,	_SCH_INF_DLL_API_RUN_CONTROL_TM_8_PART			, int , CHECKING_SIDE , return );
//----------------
//DEFINE_SCHEDULER_MACRO_FDN_ARG4		( int	,	_SCH_INF_DLL_API_USER_CONTROL_FEM_PART			, int , side , int , pointer , int , mode , char * , data , return );
//DEFINE_SCHEDULER_MACRO_FDN_ARG4		( int	,	_SCH_INF_DLL_API_USER_CONTROL_TM_PART			, int , side , int , pointer , int , mode , char * , data , return );
//----------------
#endif
