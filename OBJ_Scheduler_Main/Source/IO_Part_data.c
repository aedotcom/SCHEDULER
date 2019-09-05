#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "System_tag.h"


int EXTEND_SOURCE_USE; // 2018.09.05

//------------------------------
//-------------------------------------------
int COMM;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int Address_FM_RB_Synch[MAX_FM_CHAMBER_DEPTH];
//
int Address_TM_RB_Synch[MAX_TM_CHAMBER_DEPTH];

int Address_FM_Finger_Use[MAX_FM_CHAMBER_DEPTH][MAX_FINGER_FM];
//
int Address_TM_Finger_Use[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];

int Address_TM_R_STATUS[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
//
int Address_TM_R_SOURCE[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];

int Address_TM_R2_STATUS[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
int Address_TM_R2_SOURCE[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];

int CURRENT_MAIN_CONTROL[MAX_CASSETTE_SIDE]; // 2008.09.23
int CURRENT_MAIN_IO_CONTROL[MAX_CASSETTE_SIDE]; // 2008.09.23

int CURRENT_MAIN_BM_MODE[MAX_CASSETTE_SIDE]; // 2010.02.19

int CURRENT_MAIN_BM_CONTROL[MAX_CASSETTE_SIDE]; // 2010.02.19
int CURRENT_MAIN_IO_BM_CONTROL[MAX_CASSETTE_SIDE]; // 2010.02.19

int	Address_CTRL_MainControl[MAX_CASSETTE_SIDE];
int	Address_CTRL_MainBMControl[MAX_CASSETTE_SIDE];
int	Address_CTRL_EndControl[MAX_CASSETTE_SIDE];
int	Address_CTRL_InPath[MAX_CASSETTE_SIDE];
int	Address_CTRL_OutPath[MAX_CASSETTE_SIDE];
int	Address_CTRL_StartSlot[MAX_CASSETTE_SIDE];
int	Address_CTRL_EndSlot[MAX_CASSETTE_SIDE];
int	Address_CTRL_LoopCount[MAX_CASSETTE_SIDE];
int	Address_CTRL_RecipeFile[MAX_CASSETTE_SIDE];
int	Address_CTRL_HandOff[MAX_CASSETTE_SIDE];

int	Address_CTRL_StartIndex[MAX_CASSETTE_SIDE]; // 2015.10.12
int	Address_CTRL_EndIndex[MAX_CASSETTE_SIDE]; // 2015.10.12

int	Address_CTRL_RecipeBMFile[MAX_CASSETTE_SIDE]; // 2010.03.10

int Address_Rb_Extension[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
int Address_Rb_Rotation[MAX_TM_CHAMBER_DEPTH];
int Address_Rb_UpDown[MAX_TM_CHAMBER_DEPTH];
int Address_Rb_Movement[MAX_TM_CHAMBER_DEPTH];

int Address_FM_Rb_Anim[MAX_FM_CHAMBER_DEPTH][6];

int Address_Cassette_Wafer_ID[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE];

int Address_JOB[MAX_CASSETTE_SIDE];
int Address_MID[MAX_CASSETTE_SIDE];

int Address_JID_READ[MAX_CASSETTE_SIDE];
int Address_MID_READ[MAX_CASSETTE_SIDE];
int Address_NID_READ[MAX_CASSETTE_SIDE];
int Address_WID_READ[MAX_CASSETTE_SIDE];
int Address_WID_READ2[MAX_CASSETTE_SIDE];

int Address_WID_NAME;
int Address_WID_NAME2;

int	Address_PM_WAFER_STATUS[MAX_PM_CHAMBER_DEPTH][MAX_PM_SLOT_DEPTH];
int	Address_PM_WAFER_SOURCE[MAX_PM_CHAMBER_DEPTH][MAX_PM_SLOT_DEPTH]; // 2007.05.02
int	Address_PM_WAFER_RESULT[MAX_PM_CHAMBER_DEPTH][MAX_PM_SLOT_DEPTH];
//
int	Address_TM_RB_RESULT[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
int	Address_TM_RB2_RESULT[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM]; // 2015.05.27
//
int	Address_FE_RB_AL_RESULT;
int	Address_FE_RB_AL2_RESULT;
int	Address_AL_CH_RESULT;
int	Address_IC_CH_RESULT;

int	Address_CTRL_RecipePrcsFile[MAX_CHAMBER]; // 2011.09.01

int	Address_MODULE_Flag_Status[MAX_CHAMBER]; // 2013.03.19

int Address_FA_AGV_STATUS[MAX_CHAMBER];
int Address_FA_MODULE_STATUS[MAX_CHAMBER];
int Address_FA_SWITCH_STATUS[MAX_CHAMBER];
int Address_FA_MAP_STATUS[MAX_CHAMBER];
int Address_FA_HOF_STATUS[MAX_CHAMBER];
int Address_FA_RUNTIME_IN_STATUS[MAX_CHAMBER];
int Address_FA_RUNTIME_OUT_STATUS[MAX_CHAMBER];
int Address_FA_SIDE_STATUS[MAX_CHAMBER];
int Address_FA_SCH_RUNTYPE_SET;
int Address_FA_CPJOB_LOGBASED_SET;
int Address_IO_MODULE_SIZE_STATUS[MAX_CHAMBER];
int Address_FM_ARM_STYLE_SET;
int Address_TM_ARM_STYLE_SET[MAX_TM_CHAMBER_DEPTH]; // 2014.11.20
int Address_FA_CARRIER_GROUP_SET;
int Address_FA_WAFER_SUPPLY_MODE_SET;
int Address_FA_LOT_SUPPLY_MODE_SET; // 2007.10.11
int Address_FA_CPJOB_MODE_SET; // 2008.09.29
int Address_FA_CLUSTER_INCLUDE_SET; // 2005.08.02
int Address_FA_COOLING_TIME_SET; // 2005.11.29
int Address_FA_COOLING_TIME_SET2; // 2011.06.01
int Address_FA_BM_SCHEDULING_FACTOR; // 2015.03.25
int Address_IO_DOUBLE_SIDE[MAX_CHAMBER]; // 2015.05.27

int CURRENT_MAIN_STATUS[MAX_CASSETTE_SIDE];
int CURRENT_MAIN_IO_STATUS[MAX_CASSETTE_SIDE]; // 2008.09.18
int Address_MAIN_STATUS[MAX_CASSETTE_SIDE];

int CURRENT_MAIN_BM_STATUS[MAX_CASSETTE_SIDE]; // 2010.02.19
int CURRENT_MAIN_IO_BM_STATUS[MAX_CASSETTE_SIDE]; // 2010.02.19
int Address_MAIN_BM_STATUS[MAX_CASSETTE_SIDE]; // 2010.02.19

//
int Address_BM_Wafer_Status[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];
int Address_BM_Wafer_Source[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];
int	Address_BM_Wafer_Result[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];
//

int Address_RUN_STATUS;
int Address_TR_CONTROL;
int Address_TR_STATUS[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1]; // 2006.08.30 // 2006.11.08

int Address_FIC_CH_STATUS[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE]; // 2007.07.02
int Address_FIC_CH_SOURCE[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE]; // 2007.07.02
int Address_FIC_CH_RESULT[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE]; // 2007.07.02
//
int Address_FIC_CH_TIMER[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE]; // 2005.12.01
//
int	Address_AL_Wafer_Status;
int	Address_IC_Wafer_Status;
int	Address_AL_Wafer_Source;
int	Address_IC_Wafer_Source;
//
int	Address_F_Wafer_Status[MAX_FM_CHAMBER_DEPTH][MAX_FINGER_FM];
int	Address_F_Wafer_Source[MAX_FM_CHAMBER_DEPTH][MAX_FINGER_FM];
//
int	Address_FM_AL_Wafer_Source;
int	Address_FM_AL_Wafer_Status;
//
int	Address_FM_AL2_Wafer_Source;
int	Address_FM_AL2_Wafer_Status;
//
int	Address_F_Wafer_Result[MAX_FM_CHAMBER_DEPTH][MAX_FINGER_FM];
//
int	Address_FAE_Wafer_Status[MAX_FM_CHAMBER_DEPTH][MAX_FM_A_EXTEND_FINGER]; // 2003.10.08
int	Address_FAE_Wafer_Result[MAX_FM_CHAMBER_DEPTH][MAX_FM_A_EXTEND_FINGER]; // 2003.10.08
int	Address_FAE_Wafer_Source[MAX_FM_CHAMBER_DEPTH][MAX_FM_A_EXTEND_FINGER]; // 2003.10.08

int	Address_FAM_Wafer_Status[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24
int	Address_FAM_Wafer_Result[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24
int	Address_FAM_Wafer_Source[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24

int	Address_FBM_Wafer_Status[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24
int	Address_FBM_Wafer_Result[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24
int	Address_FBM_Wafer_Source[MAX_FM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE]; // 2018.08.24

int Address_DummyProcess_LotFirst[MAX_CHAMBER];
int Address_DummyProcess_LotEnd[MAX_CHAMBER];
//
int Address_DummyProcess_Mode[MAX_CASSETTE_SLOT_SIZE];

int Address_DummyProcess_Count[MAX_CASSETTE_SLOT_SIZE][MAX_SDM_STYLE]; // 2008.08.07
int Address_DummyProcess_Recipe[3][MAX_CASSETTE_SLOT_SIZE][MAX_SDM_STYLE][3]; // 2004.11.17
//
int Address_CTC_Pause_Status; // 2004.06.29
//
int Address_IO_INTLKS_FOR_PICK[MAX_CHAMBER]; // 2007.01.23
int Address_IO_INTLKS_FOR_PLACE[MAX_CHAMBER]; // 2007.01.23

int Address_IO_INTLKS_FOR_BM_FM_PICK[MAX_FM_CHAMBER_DEPTH][MAX_CHAMBER]; // 2010.07.09
int Address_IO_INTLKS_FOR_BM_FM_PLACE[MAX_FM_CHAMBER_DEPTH][MAX_CHAMBER]; // 2010.07.09
//
int Address_IO_INTLKS_FOR_BM_TM_PICK[MAX_TM_CHAMBER_DEPTH][MAX_CHAMBER]; // 2010.07.09
int Address_IO_INTLKS_FOR_BM_TM_PLACE[MAX_TM_CHAMBER_DEPTH][MAX_CHAMBER]; // 2010.07.09

int Address_CHAMBER_INTLKS_FOR_PICKPLACE[2][MAX_TM_CHAMBER_DEPTH+1][MAX_CHAMBER]; // 2013.11.21



//
// 2018.09.05
int	Address_AL_Wafer_Source_E;
int	Address_IC_Wafer_Source_E;
int Address_CM_Wafer_Source_E[MAX_CASSETTE_SIDE];
int Address_BM_Wafer_Source_E[MAX_BM_CHAMBER_DEPTH][MAX_CASSETTE_SLOT_SIZE];
int Address_FIC_CH_SOURCE_E[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE];
int	Address_F_Wafer_Source_E[MAX_FM_CHAMBER_DEPTH][MAX_FINGER_FM];
int	Address_FM_AL_Wafer_Source_E;
int	Address_FM_AL2_Wafer_Source_E;
int	Address_FAE_Wafer_Source_E[MAX_FM_CHAMBER_DEPTH][MAX_FM_A_EXTEND_FINGER];
int Address_TM_R_SOURCE_E[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
int Address_TM_R2_SOURCE_E[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM];
int	Address_PM_WAFER_SOURCE_E[MAX_PM_CHAMBER_DEPTH][MAX_PM_SLOT_DEPTH];
//

//----------------------------------------------------------------------------
int    IO_ADD_READ_DIGITAL_NAME( int *address , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_D_IO , name );
	}
	if ( *address < 0 ) return 0;
	return _dREAD_DIGITAL( *address , &COMM );
}

BOOL IO_ADD_WRITE_DIGITAL_NAME( int *address , int data , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_D_IO , name );
	}
	if ( *address < 0 ) return FALSE;
	_dWRITE_DIGITAL( *address , data , &COMM );
	return TRUE;
}

double IO_ADD_READ_ANALOG_NAME( int *address , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_A_IO , name );
	}
	if ( *address < 0 ) return 0;
	return _dREAD_ANALOG( *address , &COMM );
}

BOOL  IO_ADD_WRITE_ANALOG_NAME( int *address , double data , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_A_IO , name );
	}
	if ( *address < 0 ) return FALSE;
	_dWRITE_ANALOG( *address , data , &COMM );
	return TRUE;
}

void   IO_ADD_READ_STRING_NAME( int *address , char *data , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_S_IO , name );
	}
	if ( *address < 0 ) { strcpy( data , "" ); return; }
	_dREAD_STRING( *address , data , &COMM );
}

BOOL   IO_ADD_WRITE_STRING_NAME( int *address , char *data , LPSTR list , ... ) {
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_S_IO , name );
	}
	if ( *address < 0 ) return FALSE;
	_dWRITE_STRING( *address , data , &COMM );
	return TRUE;
}

//----------------------------------------------------------------------------
int    IO_ADD_READ_DIGITAL( int address ) {
	if ( address < 0 ) return 0;
	return _dREAD_DIGITAL( address , &COMM );
}
void   IO_ADD_WRITE_DIGITAL( int address , int data ) {
	if ( address < 0 ) return;
	_dWRITE_DIGITAL( address , data , &COMM );
}

void   IO_ADD_WRITE_DIGITAL_TH( int address , int data ) {
	if ( address < 0 ) return;
//	_dWRITE_DIGITAL_TH( address , data ); // 2008.10.17
	_dWRITE_DIGITAL_TH2( address , data ); // 2008.10.17
}
//
double IO_ADD_READ_ANALOG( int address ) {
	if ( address < 0 ) return 0;
	return _dREAD_ANALOG( address , &COMM );
}
void   IO_ADD_WRITE_ANALOG( int address , double data ) {
	if ( address < 0 ) return;
	_dWRITE_ANALOG( address , data , &COMM );
}
//
void   IO_ADD_WRITE_STRING( int address , char *data ) {
	if ( address < 0 ) return;
	_dWRITE_STRING( address , data , &COMM );
}

void   IO_ADD_READ_STRING( int address , char *data ) {
	if ( address < 0 ) { strcpy( data , "" ); return; }
	_dREAD_STRING( address , data , &COMM );
}

/*
// 2011.01.06
BOOL   IO_ADD_APPEND_DIGITAL( int address , int count ) {
	int i;
	if ( address < 0 ) return FALSE;
	i = _dREAD_DIGITAL( address , &COMM ) + count;
//	_dWRITE_DIGITAL_TH( address , i ); // 2008.10.17
	_dWRITE_DIGITAL_TH2( address , i ); // 2008.10.17
	return TRUE;
}
*/
BOOL IO_ADD_WRITE_DIGITAL_TH_NAME( int *address , BOOL app , int count , LPSTR list , ... ) { // 2011.01.06
	int i;
	va_list va;
	char name[64+1];
	if ( *address == -2 ) {
		va_start( va , list );
		vsprintf( name , list , (LPSTR) va );
		va_end( va );
		*address = _FIND_FROM_STRING( _K_D_IO , name );
	}
	//
	if ( *address < 0 ) return FALSE;
	//
	if ( app ) {
		i = _dREAD_DIGITAL( *address , &COMM ) + count;
	}
	else {
		i = count;
	}
	//
	_dWRITE_DIGITAL_TH2( *address , i );
	return TRUE;
}


//----------------------------------------------------------------------------
//
//
void IO_DATA_INIT_BEFORE_READFILE() {
	int i , j , k;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		CURRENT_MAIN_STATUS[i] = 0;
		CURRENT_MAIN_IO_STATUS[i] = 0;
		//
		CURRENT_MAIN_BM_STATUS[i] = 0;
		CURRENT_MAIN_IO_BM_STATUS[i] = 0;
		//
		CURRENT_MAIN_CONTROL[i] = 0;
		CURRENT_MAIN_IO_CONTROL[i] = 0;
		//
		CURRENT_MAIN_BM_CONTROL[i] = 0; // 2010.02.19
		CURRENT_MAIN_IO_BM_CONTROL[i] = 0; // 2010.02.19
	}
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2011.09.01
		Address_CTRL_RecipePrcsFile[i] = -2;
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		Address_CTRL_MainControl[i] = -2;
		//
		Address_CTRL_MainBMControl[i] = -2;
		//
		Address_CTRL_EndControl[i] = -2;
		//
		Address_CTRL_InPath[i]     = -2;
		//
		Address_CTRL_OutPath[i]    = -2;
		//
		Address_CTRL_StartSlot[i]  = -2;
		//
		Address_CTRL_EndSlot[i]    = -2;
		//
		Address_CTRL_StartIndex[i]  = -2; // 2015.10.12
		//
		Address_CTRL_EndIndex[i]    = -2; // 2015.10.12
		//
		Address_CTRL_LoopCount[i]  = -2;
		//
		Address_CTRL_RecipeFile[i] = -2;
		//
		Address_CTRL_RecipeBMFile[i] = -2; // 2010.03.10
		//
		Address_CTRL_HandOff[i] = -2;
		//----------------------------		
		Address_MAIN_STATUS[i] = -2;
		//
		Address_MAIN_BM_STATUS[i] = -2;
		//
		Address_JOB[i]         = -2;
		//
		Address_MID[i]         = -2;
		//
		Address_JID_READ[i]    = -2;
		//
		Address_MID_READ[i]    = -2;
		//
		Address_NID_READ[i]    = -2;
		//
		Address_WID_READ[i]    = -2;
		//
		Address_WID_READ2[i]   = -2;
		//
	}
	//
	Address_WID_NAME  = -2;
	Address_WID_NAME2 = -2;
	//
	for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < 6 ; j++ ) Address_FM_Rb_Anim[i][j] = -1;
		//
		Address_FM_Rb_Anim[i][RB_ANIM_EXTEND]		= -2; // 2004.06.08
		Address_FM_Rb_Anim[i][RB_ANIM_EXTEND2]		= -2; // 2004.06.08
		Address_FM_Rb_Anim[i][RB_ANIM_ROTATE]		= -2; // 2004.06.08
		Address_FM_Rb_Anim[i][RB_ANIM_UPDOWN]		= -2; // 2004.06.08
		Address_FM_Rb_Anim[i][RB_ANIM_MOVE]			= -2; // 2004.06.08
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) && ( j < _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) ) { // 2002.09.13
				Address_Rb_Extension[i][j]	= -2;
			}
			else {
				Address_Rb_Extension[i][j]	= -1;
			}
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) { // 2002.09.13
			Address_Rb_Rotation[i]	= -2;
			Address_Rb_UpDown[i]	= -2;
			Address_Rb_Movement[i]	= -2;
		}
		else {
			Address_Rb_Rotation[i]	= -1;
			Address_Rb_UpDown[i]	= -1;
			Address_Rb_Movement[i]	= -1;
		}
		//
	}
	Address_RUN_STATUS		= _FIND_FROM_STRING( _K_D_IO , "CTC.RUN_STATUS" );

	Address_TR_CONTROL		= -2;
//=============================================================================
// 2006.08.30
//=============================================================================
	Address_TR_STATUS[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH]	= -2;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) Address_TR_STATUS[i]	= -2;
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) Address_TR_STATUS[i+MAX_TM_CHAMBER_DEPTH]   = -2;
//=============================================================================
	for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
//			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2002.09.13 // 2019.02.21
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( EXTEND_SOURCE_USE != 0 ) ) { // 2002.09.13 // 2019.02.21
				Address_FM_Finger_Use[i][j]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.F%c%s_Finger_Use" , j + 'A' , MULMDLSTR[ i ] );
			}
			else {
				Address_FM_Finger_Use[i][j]	= -1;
			}
		}
		//
//		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2002.09.13 // 2019.02.21
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( EXTEND_SOURCE_USE != 0 ) ) { // 2002.09.13 // 2019.02.21
			Address_FM_RB_Synch[i]		= -2;
		}
		else {
			Address_FM_RB_Synch[i]		= -1;
		}
	}
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) && ( j < _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) ) { // 2002.09.13
//				Address_TM_Finger_Use[i][j]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.T%c%s_Finger_Use" , j + 'A' , MULMDLSTR[ i ] ); // 2013.03.09
				Address_TM_Finger_Use[i][j]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.T%c%s_Finger_Use" , j + 'A' , TMMDLSTR[ i ] ); // 2013.03.09
			}
			else {
				Address_TM_Finger_Use[i][j]	= -1;
			}
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) { // 2002.09.13
			Address_TM_RB_Synch[i]	= -2;
		}
		else {
			Address_TM_RB_Synch[i]	= -1;
		}
	}
	//
	Address_FA_AGV_STATUS[0]       = -2;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) { // 2002.09.13
			Address_FA_AGV_STATUS[CM1+i] = -2;
		}
		else {
			Address_FA_AGV_STATUS[CM1+i] = -1;
		}
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) { // 2003.10.31
//			Address_FA_MODULE_STATUS[TM+i] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_MDL_TM%s" , MULMDLSTR[ i ] ); // 2013.03.09
			Address_FA_MODULE_STATUS[TM+i] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_MDL_TM%s" , TMMDLSTR[ i ] ); // 2013.03.09
		}
		else {
			Address_FA_MODULE_STATUS[TM+i] = -1;
		}
	}
//	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2003.10.31 // 2018.12.14
		Address_FA_MODULE_STATUS[FM]   = _FIND_FROM_STRING( _K_D_IO , "CTC.FA_STATUS_MDL_FM" );
//	} // 2018.12.14
//	else { // 2018.12.14
//		Address_FA_MODULE_STATUS[FM]   = -1; // 2018.12.14
//	} // 2018.12.14
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) { // 2002.09.13
			Address_FA_MODULE_STATUS[CM1+i] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_MDL_CM%d" , i + 1 );
			Address_FA_SWITCH_STATUS[CM1+i] = -2;
		}
		else {
			Address_FA_MODULE_STATUS[CM1+i] = -1;
			Address_FA_SWITCH_STATUS[CM1+i] = -1;
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) { // 2002.09.13
			Address_FA_MODULE_STATUS[i+PM1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_MDL_PM%d" , i + 1 );
		}
		else {
			Address_FA_MODULE_STATUS[i+PM1] = -1;
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) { // 2002.09.13
			Address_FA_MODULE_STATUS[i+BM1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_MDL_BM%d" , i + 1 );
		}
		else {
			Address_FA_MODULE_STATUS[i+BM1] = -1;
		}
	}
	Address_FA_MODULE_STATUS[AL]   = _FIND_FROM_STRING( _K_D_IO , "CTC.FA_STATUS_MDL_AL" );
	Address_FA_MODULE_STATUS[IC]   = _FIND_FROM_STRING( _K_D_IO , "CTC.FA_STATUS_MDL_IC" );
	Address_FA_MODULE_STATUS[F_AL] = _FIND_FROM_STRING( _K_D_IO , "CTC.FA_STATUS_MDL_FAL" );
	Address_FA_MODULE_STATUS[F_IC] = _FIND_FROM_STRING( _K_D_IO , "CTC.FA_STATUS_MDL_FIC" );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) { // 2002.09.13
			Address_FA_MAP_STATUS[CM1+i] = -2;
			Address_FA_HOF_STATUS[CM1+i] = -2;
		}
		else {
			Address_FA_MAP_STATUS[CM1+i] = -1;
			Address_FA_HOF_STATUS[CM1+i] = -1;
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) { // 2002.09.13
			Address_FA_RUNTIME_IN_STATUS[i+PM1] = -2;
			//
			Address_FA_RUNTIME_OUT_STATUS[i+PM1] = -2;
			//
			Address_FA_SIDE_STATUS[i+PM1] = -2;
			//
			Address_IO_MODULE_SIZE_STATUS[i+PM1] = -2; // 2002.11.05
		}
		else {
			Address_FA_RUNTIME_IN_STATUS[i+PM1] = -1;
			Address_FA_RUNTIME_OUT_STATUS[i+PM1] = -1;
			Address_FA_SIDE_STATUS[i+PM1] = -1;
			Address_IO_MODULE_SIZE_STATUS[i+PM1] = -1; // 2002.11.05
		}
	}
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2013.03.19
		Address_MODULE_Flag_Status[i] = -2;
		//
		Address_IO_DOUBLE_SIDE[i] = -2; // 2015.05.27
		//
	}
	//
	Address_FA_SCH_RUNTYPE_SET        = -2;
	//
	Address_FA_CPJOB_LOGBASED_SET     = -2; // 2002.12.30
	//
	Address_FM_ARM_STYLE_SET	      = -2; // 2003.01.24
	//
	Address_FA_CARRIER_GROUP_SET      = -2; // 2003.02.05
	//
	Address_FA_WAFER_SUPPLY_MODE_SET  = -2; // 2003.06.12
	//
	Address_FA_LOT_SUPPLY_MODE_SET    = -2; // 2007.10.11
	//
	Address_FA_CLUSTER_INCLUDE_SET    = -2; // 2005.08.02
	//
	Address_FA_COOLING_TIME_SET       = -2; // 2005.11.29
	//
	Address_FA_COOLING_TIME_SET2      = -2; // 2011.06.01
	//
	Address_FA_BM_SCHEDULING_FACTOR	  = -2; // 2015.03.25
	//
	Address_FA_CPJOB_MODE_SET		  = -2; // 2008.09.29
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2014.11.20
		Address_TM_ARM_STYLE_SET[i] = -2;
	}
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) { // 2002.09.13
			Address_FA_RUNTIME_IN_STATUS[i+BM1] = -2;
			//
			Address_FA_RUNTIME_OUT_STATUS[i+BM1] = -2;
			//
			Address_IO_MODULE_SIZE_STATUS[i+BM1] = -2; // 2002.11.05
		}
		else {
			Address_FA_RUNTIME_IN_STATUS[i+BM1] = -1;
			Address_FA_RUNTIME_OUT_STATUS[i+BM1] = -1;
			Address_IO_MODULE_SIZE_STATUS[i+BM1] = -1; // 2002.11.05
		}
	}
	//================================================================================
	Address_IO_MODULE_SIZE_STATUS[F_IC] = -2; // 2007.08.14
	//================================================================================
	// 2002.09.13
	//================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		Address_DummyProcess_Mode[i] = -1;
		for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) { // 2004.11.17
			//
			Address_DummyProcess_Count[i][j] = -1; // 2008.08.06
			//
			for ( k = 0 ; k < 3 ; k++ ) {
				Address_DummyProcess_Recipe[k][i][j][0] = -1;
				Address_DummyProcess_Recipe[k][i][j][1] = -1;
				Address_DummyProcess_Recipe[k][i][j][2] = -1;
			}
		}
	}
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Address_DummyProcess_LotFirst[i] = -1;
		Address_DummyProcess_LotEnd[i] = -1;
	}
	if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() != 0 ) {
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			Address_DummyProcess_LotFirst[i + PM1] = -2;
			Address_DummyProcess_LotEnd[i + PM1]   = -2;
		}
		//
		for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
			//
			Address_DummyProcess_Mode[i]  = -2;
			//
			for ( j = 0 ; j < MAX_SDM_STYLE ; j++ ) { // 2004.11.17
				//
				Address_DummyProcess_Count[i][j] = -2; // 2008.08.06
				//
				for ( k = 0 ; k < 3 ; k++ ) {
					Address_DummyProcess_Recipe[k][i][j][0] = -2;
					Address_DummyProcess_Recipe[k][i][j][1] = -2;
					Address_DummyProcess_Recipe[k][i][j][2] = -2;
				}
			}
		}
	}
	//=================================================================================
	Address_CTC_Pause_Status = -2; // 2004.06.29
	//
	//==================================================================================
	// 2005.12.01
	//==================================================================================
	for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) Address_FIC_CH_TIMER[i] = -2;
	//==================================================================================
}
//
void IO_DATA_INIT_AFTER_READFILE() {
	int i , j;

	//============================================================================================
	// 2010.07.09
	//============================================================================================
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		for ( j = 0 ; j < MAX_FM_CHAMBER_DEPTH ; j++ ) {
			Address_IO_INTLKS_FOR_BM_FM_PICK[j][i] = -1;
			Address_IO_INTLKS_FOR_BM_FM_PLACE[j][i] = -1;
		}
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			Address_IO_INTLKS_FOR_BM_TM_PICK[j][i] = -1;
			Address_IO_INTLKS_FOR_BM_TM_PLACE[j][i] = -1;
		}
		//
		for ( j = 0 ; j <= MAX_TM_CHAMBER_DEPTH ; j++ ) {
			Address_CHAMBER_INTLKS_FOR_PICKPLACE[0][j][i] = -1;
			Address_CHAMBER_INTLKS_FOR_PICKPLACE[1][j][i] = -1;
		}
	}
	if ( _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) {
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_CHAMBER_DEPTH ; j++ ) {
				Address_IO_INTLKS_FOR_BM_FM_PICK[j][i+BM1] = -2;
				Address_IO_INTLKS_FOR_BM_FM_PLACE[j][i+BM1] = -2;
			}
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				Address_IO_INTLKS_FOR_BM_TM_PICK[j][i+BM1] = -2;
				Address_IO_INTLKS_FOR_BM_TM_PLACE[j][i+BM1] = -2;
			}
		}
	}
	//============================================================================================
	// 2013.11.21
	//============================================================================================
	if ( _i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE() ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			for ( j = 0 ; j <= MAX_TM_CHAMBER_DEPTH ; j++ ) {
				Address_CHAMBER_INTLKS_FOR_PICKPLACE[0][j][i] = -2;
				Address_CHAMBER_INTLKS_FOR_PICKPLACE[1][j][i] = -2;
			}
		}
	}
	else {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			for ( j = 0 ; j <= MAX_TM_CHAMBER_DEPTH ; j++ ) {
				Address_CHAMBER_INTLKS_FOR_PICKPLACE[0][j][i] = -1;
				Address_CHAMBER_INTLKS_FOR_PICKPLACE[1][j][i] = -1;
			}
		}
	}
	//============================================================================================
	// 2007.01.23
	//============================================================================================
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Address_IO_INTLKS_FOR_PICK[i] = -1;
		Address_IO_INTLKS_FOR_PLACE[i] = -1;
	}
	if ( _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_IO_INTLKS_FOR_PICK[i+CM1]  = -2;
			Address_IO_INTLKS_FOR_PLACE[i+CM1] = -2;
		}
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			Address_IO_INTLKS_FOR_PICK[i+PM1]  = -2;
			Address_IO_INTLKS_FOR_PLACE[i+PM1] = -2;
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			Address_IO_INTLKS_FOR_PICK[i+BM1]  = -2;
			Address_IO_INTLKS_FOR_PLACE[i+BM1] = -2;
		}
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			Address_IO_INTLKS_FOR_PICK[i+TM]  = -2;
			Address_IO_INTLKS_FOR_PLACE[i+TM] = -2;
		}
		Address_IO_INTLKS_FOR_PICK[FM]  = -2;
		Address_IO_INTLKS_FOR_PLACE[FM] = -2;
	}
	//============================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) { // 2002.09.13
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_Cassette_Wafer_ID[i][j] = -2;
			}
		}
		else {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_Cassette_Wafer_ID[i][j] = -1;
			}
		}
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) && ( j < _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) ) { // 2002.09.13
				Address_TM_R_STATUS[i][j]  = -2;
				Address_TM_R_SOURCE[i][j]  = -2;
				Address_TM_R2_STATUS[i][j] = -2;
				Address_TM_R2_SOURCE[i][j] = -2;
				Address_TM_RB_RESULT[i][j] = -2;
				Address_TM_RB2_RESULT[i][j] = -2; // 2015.05.27
			}
			else {
				Address_TM_R_STATUS[i][j]	= -1;
				Address_TM_R_SOURCE[i][j]	= -1;
				Address_TM_R2_STATUS[i][j]	= -1;
				Address_TM_R2_SOURCE[i][j]	= -1;
				Address_TM_RB_RESULT[i][j]	= -1;
				Address_TM_RB2_RESULT[i][j] = -1; // 2015.05.27
			}
		}
	}
	//------------------------------
	Address_F_Wafer_Result[0][0]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2005.10.07 for AL0/SUB2
	Address_F_Wafer_Result[0][1]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2005.10.07 for AL0/SUB2
	//------------------------------
//	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2002.09.13 // 2018.12.13
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( EXTEND_SOURCE_USE != 0 ) ) { // 2002.09.13 // 2018.12.13
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Result[i][j] = -2;
			}
		}
		//
		// 2018.08.24
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_FAM_Wafer_Result[i][j] = -2;
				Address_FBM_Wafer_Result[i][j] = -2;
			}
		}
		//
		Address_FE_RB_AL_RESULT	 = -2;
		Address_FE_RB_AL2_RESULT = -2;
		//
		Address_F_Wafer_Result[1][0]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA2_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
		Address_F_Wafer_Result[1][1]	= _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB2_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
	}
	else {
		//
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Result[i][j] = -1;
			}
		}
		//
		// 2018.08.24
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_FAM_Wafer_Result[i][j] = -1;
				Address_FBM_Wafer_Result[i][j] = -1;
			}
		}
		//
		Address_FE_RB_AL_RESULT	= -1;
		Address_FE_RB_AL2_RESULT	= -1;
		//
		Address_F_Wafer_Result[1][0]	= -1; // 2007.06.08
		Address_F_Wafer_Result[1][1]	= -1; // 2007.06.08
	}
	//
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
		for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) { // 2007.07.02
			Address_FIC_CH_STATUS[i] = -2;
			Address_FIC_CH_SOURCE[i] = -2;
			Address_FIC_CH_RESULT[i] = -2;
		}
	}
	else {
		Address_FIC_CH_STATUS[0] = -2;
		Address_FIC_CH_SOURCE[0] = -2;
		Address_FIC_CH_RESULT[0] = -2;
		//
		for ( i = 1 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) { // 2007.07.02
			Address_FIC_CH_STATUS[i] = -1;
			Address_FIC_CH_SOURCE[i] = -1;
			Address_FIC_CH_RESULT[i] = -1;
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) { // 2002.09.13
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				Address_PM_WAFER_STATUS[i][j] = -2;
				Address_PM_WAFER_SOURCE[i][j] = -2;
				Address_PM_WAFER_RESULT[i][j] = -2;
			}
		}
		else {
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				Address_PM_WAFER_STATUS[i][j] = -1;
				Address_PM_WAFER_RESULT[i][j] = -1;
				Address_PM_WAFER_SOURCE[i][j] = -1;
			}
		}
	}
	//
	Address_AL_CH_RESULT = -2;
	Address_IC_CH_RESULT = -2;
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) { // 2002.09.13
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_BM_Wafer_Status[i][j] = -2;
				Address_BM_Wafer_Source[i][j] = -2;
				Address_BM_Wafer_Result[i][j] = -2;
			}
		}
		else {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_BM_Wafer_Status[i][j] = -1;
				Address_BM_Wafer_Source[i][j] = -1;
				Address_BM_Wafer_Result[i][j] = -1;
			}
		}
	}
	//===== Change Define to Here 2002.01.24
	Address_AL_Wafer_Status = -2;
	Address_IC_Wafer_Status = -2;
	Address_AL_Wafer_Source = -2;
	Address_IC_Wafer_Source = -2;
	//--------------------------------------------------------------------------------------
	Address_F_Wafer_Status[0][0] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2005.10.07 for AL0/SUB2
	Address_F_Wafer_Status[0][1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2005.10.07 for AL0/SUB2
	//
//	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2002.09.13 // 2018.12.13
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( EXTEND_SOURCE_USE != 0 ) ) { // 2002.09.13 // 2018.12.13
		Address_F_Wafer_Source[0][0]    = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		Address_F_Wafer_Source[0][1]    = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		//
		Address_FM_AL_Wafer_Source = -2;
		Address_FM_AL_Wafer_Status = -2;
		Address_FM_AL2_Wafer_Source = -2;
		Address_FM_AL2_Wafer_Status = -2;
		//-------------------
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Status[i][j] = -2;
				Address_FAE_Wafer_Source[i][j] = -2;
			}
		}
		//-------------------
		// 2018.08.24
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_FAM_Wafer_Status[i][j] = -2; // 2018.08.24
				Address_FAM_Wafer_Source[i][j] = -2; // 2018.08.24
				Address_FBM_Wafer_Status[i][j] = -2; // 2018.08.24
				Address_FBM_Wafer_Source[i][j] = -2; // 2018.08.24
			}
		}
		//-------------------
		Address_F_Wafer_Status[1][0] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA2_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
		Address_F_Wafer_Status[1][1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB2_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
		Address_F_Wafer_Source[1][0] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA2_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
		Address_F_Wafer_Source[1][1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FB2_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2007.06.08
	}
	else {
		Address_F_Wafer_Source[0][0] = -1;
		Address_F_Wafer_Source[0][1] = -1;
		//
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Status[i][j] = -1;
				Address_FAE_Wafer_Source[i][j] = -1;
			}
		}
		//-------------------
		// 2018.08.24
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_FAM_Wafer_Status[i][j] = -1; // 2018.08.24
				Address_FAM_Wafer_Source[i][j] = -1; // 2018.08.24
				Address_FBM_Wafer_Status[i][j] = -1; // 2018.08.24
				Address_FBM_Wafer_Source[i][j] = -1; // 2018.08.24
			}
		}
		//-------------------
		if ( _i_SCH_PRM_GET_MODULE_MODE( F_AL ) ) { // 2011.07.26
			Address_FM_AL_Wafer_Source = -2;
			Address_FM_AL_Wafer_Status = -2;
			Address_FM_AL2_Wafer_Source = -2;
			Address_FM_AL2_Wafer_Status = -2;
		}
		else {
			Address_FM_AL_Wafer_Source = -1;
			Address_FM_AL_Wafer_Status = -1;
			Address_FM_AL2_Wafer_Source = -1;
			Address_FM_AL2_Wafer_Status = -1;
		}
		//
		Address_F_Wafer_Status[1][0] = -1; // 2007.06.08
		Address_F_Wafer_Status[1][1] = -1; // 2007.06.08
		Address_F_Wafer_Source[1][0] = -1; // 2007.06.08
		Address_F_Wafer_Source[1][1] = -1; // 2007.06.08
	}
	//===== 2003.10.27
	//
	// 2018.09.04
	//
	//
	//
	//
	if ( EXTEND_SOURCE_USE != 0 ) {
		//-------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_CM_Wafer_Source_E[i] = -2;
		}
		//-------------------
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
				Address_F_Wafer_Source_E[i][j] = -2;
			}
		}
		//-------------------
		Address_FM_AL_Wafer_Source_E = -2;
		Address_FM_AL2_Wafer_Source_E = -2;
		//-------------------
		Address_AL_Wafer_Source_E = -2;
		Address_IC_Wafer_Source_E = -2;
		//-------------------
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Source_E[i][j] = -2;
			}
		}
		//-------------------
		for ( i = 0 ; i < (MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE) ; i++ ) {
			Address_FIC_CH_SOURCE_E[i] = -2;
		}
		//-------------------
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_BM_Wafer_Source_E[i][j] = -2;
			}
		}
		//-------------------
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				Address_TM_R_SOURCE_E[i][j] = -2;
				Address_TM_R2_SOURCE_E[i][j] = -2;
			}
		}
		//-------------------
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				Address_PM_WAFER_SOURCE_E[i][j] = -2;
			}
		}
		//-------------------
	}
	else {
		//-------------------
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_CM_Wafer_Source_E[i] = -1;
		}
		//-------------------
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
				Address_F_Wafer_Source_E[i][j] = -1;
			}
		}
		//-------------------
		Address_FM_AL_Wafer_Source_E = -1;
		Address_FM_AL2_Wafer_Source_E = -1;
		//-------------------
		Address_AL_Wafer_Source_E = -1;
		Address_IC_Wafer_Source_E = -1;
		//-------------------
		for ( i = 0 ; i < MAX_FM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FM_A_EXTEND_FINGER ; j++ ) {
				Address_FAE_Wafer_Source_E[i][j] = -1;
			}
		}
		//-------------------
		for ( i = 0 ; i < (MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE) ; i++ ) {
			Address_FIC_CH_SOURCE_E[i] = -1;
		}
		//-------------------
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				Address_BM_Wafer_Source_E[i][j] = -1;
			}
		}
		//-------------------
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_FINGER_TM ; j++ ) {
				Address_TM_R_SOURCE_E[i][j] = -1;
				Address_TM_R2_SOURCE_E[i][j] = -1;
			}
		}
		//-------------------
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				Address_PM_WAFER_SOURCE_E[i][j] = -1;
			}
		}
		//-------------------
	}
	//
	//
	//
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) {
			j = _FIND_FROM_STRINGF( _K_S_IO , "CTC.CM%d_MODULE_NAME" , i + 1 );
			IO_ADD_WRITE_STRING( j , _i_SCH_SYSTEM_GET_MODULE_NAME( i + CM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) {
			j = _FIND_FROM_STRINGF( _K_S_IO , "CTC.BM%d_MODULE_NAME" , i + 1 );
			IO_ADD_WRITE_STRING( j , _i_SCH_SYSTEM_GET_MODULE_NAME( i + BM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) {
			j = _FIND_FROM_STRINGF( _K_S_IO , "CTC.PM%d_MODULE_NAME" , i + 1 );
			IO_ADD_WRITE_STRING( j , _i_SCH_SYSTEM_GET_MODULE_NAME( i + PM1 ) );
		}
	}
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2007.12.11
		if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
			if ( i == 0 ) {
				j = _FIND_FROM_STRING( _K_S_IO , "CTC.TM_MODULE_NAME" );
			}
			else {
				j = _FIND_FROM_STRINGF( _K_S_IO , "CTC.TM%d_MODULE_NAME" , i + 1 );
			}
			IO_ADD_WRITE_STRING( j , _i_SCH_SYSTEM_GET_MODULE_NAME( i + TM ) );
		}
	}
	j = _FIND_FROM_STRING( _K_S_IO , "CTC.FM_MODULE_NAME" ); // 2007.12.11
	IO_ADD_WRITE_STRING( j , _i_SCH_SYSTEM_GET_MODULE_NAME( FM ) ); // 2007.12.11
}
//
//----------------------------------------------------------------------------
int _i_SCH_IO_GET_MODULE_STATUS( int Chamber , int Slot ) {
	int Res;
	//
	if      ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return 0;
		//
		if ( _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA() > 0 ) { // 2010.03.10
			Res = IO_ADD_READ_DIGITAL_NAME( &Address_Cassette_Wafer_ID[Chamber-CM1][Slot-1] , "CM%d.C%02d_%s" , Chamber-CM1+1 , Slot , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			if ( Res == _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA() ) {
				return CWM_ABSENT;
			}
			return Res;
		}
		else {
			return IO_ADD_READ_DIGITAL_NAME( &Address_Cassette_Wafer_ID[Chamber-CM1][Slot-1] , "CM%d.C%02d_%s" , Chamber-CM1+1 , Slot , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
	}
	else if ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_PM_WAFER_STATUS[Chamber-PM1][Slot-1] , "CTC.PM%d_%s_Status%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_BM_Wafer_Status[Chamber-BM1][Slot-1] , "CTC.BM%d_%s_Status%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == AL   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_AL_Wafer_Status , "CTC.AL_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_IC_Wafer_Status , "CTC.IC_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL_Wafer_Status , "CTC.FM_AL_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_STATUS[0] , "CTC.FM_IC_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_STATUS[Slot-1] , "CTC.FM_IC_%s_Status%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL2_Wafer_Status , "CTC.FM_AL2_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Status[Chamber-10000][Slot] );
		}
		else if ( Slot == TB_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Status[Chamber-10000][Slot] );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Status[Chamber-10000][Slot-2] , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Status[Chamber-10000][Slot-1001] , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Status[Chamber-10000][Slot-2001] , "CTC.FB%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_STATUS[Chamber-20000][Slot] , "CTC.T%c%s_%s_Status" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_STATUS[Chamber-20000][Slot] , "CTC.T%c%s_%s_Status" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
//		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_STATUS[Chamber-30000][Slot] , "CTC.T%c%s_%s_Status2" , Slot + 'A' , MULMDLSTR[ Chamber - 30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_STATUS[Chamber-30000][Slot] , "CTC.T%c%s_%s_Status2" , Slot + 'A' , TMMDLSTR[ Chamber - 30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	return 0;
}
//---------------------------------------------------------------------------
void _i_SCH_IO_SET_MODULE_STATUS( int Chamber , int Slot , int data ) {
	if      ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_Cassette_Wafer_ID[Chamber-CM1][Slot-1] , data , "CM%d.C%02d_%s" , Chamber-CM1+1 , Slot , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_PM_WAFER_STATUS[Chamber-PM1][Slot-1] , data , "CTC.PM%d_%s_Status%s" , Chamber - PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_BM_Wafer_Status[Chamber-BM1][Slot-1] , data , "CTC.BM%d_%s_Status%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == AL   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_AL_Wafer_Status , data , "CTC.AL_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IC_Wafer_Status , data , "CTC.IC_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL_Wafer_Status , data , "CTC.FM_AL_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_STATUS[0] , data , "CTC.FM_IC_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			return;
		}
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_STATUS[Slot-1] , data , "CTC.FM_IC_%s_Status%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL2_Wafer_Status , data , "CTC.FM_AL2_%s_Status" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[Chamber-10000][Slot] , data );
		}
		else if ( Slot == TB_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[Chamber-10000][Slot] , data );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Status[Chamber-10000][Slot-2] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Status[Chamber-10000][Slot-1001] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Status[Chamber-10000][Slot-2001] , data , "CTC.FB%s_%s_Status%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_STATUS[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Status" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); //2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_STATUS[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Status" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); //2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_STATUS[Chamber-30000][Slot] , data , "CTC.T%c%s_%s_Status2" , Slot + 'A' , MULMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_STATUS[Chamber-30000][Slot] , data , "CTC.T%c%s_%s_Status2" , Slot + 'A' , TMMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
}
//---------------------------------------------------------------------------
int _i_SCH_IO_GET_MODULE_SOURCE( int Chamber , int Slot ) {
	if      ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_PM_WAFER_SOURCE[Chamber-PM1][Slot-1] , "CTC.PM%d_%s_Source%s" , Chamber - PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_BM_Wafer_Source[Chamber-BM1][Slot-1] , "CTC.BM%d_%s_Source%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		return Chamber - CM1;
	}
	else if ( Chamber == AL   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_AL_Wafer_Source , "CTC.AL_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_IC_Wafer_Source , "CTC.IC_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL_Wafer_Source , "CTC.FM_AL_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_SOURCE[0] , "CTC.FM_IC_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_SOURCE[Slot-1] , "CTC.FM_IC_%s_Source%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL2_Wafer_Source , "CTC.FM_AL2_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Source[Chamber-10000][Slot] );
		}
		else if ( Slot == TB_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Source[Chamber-10000][Slot] );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Source[Chamber-10000][Slot-2] , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Source[Chamber-10000][Slot-1001] , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Source[Chamber-10000][Slot-2001] , "CTC.FB%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE[Chamber-20000][Slot] , "CTC.T%c%s_%s_Source" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE[Chamber-20000][Slot] , "CTC.T%c%s_%s_Source" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
//		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE[Chamber-30000][Slot] , "CTC.T%c%s_%s_Source2" , Slot + 'A' , MULMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE[Chamber-30000][Slot] , "CTC.T%c%s_%s_Source2" , Slot + 'A' , TMMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	return 0;
}
//---------------------------------------------------------------------------
void _i_SCH_IO_SET_MODULE_SOURCE( int Chamber , int Slot , int data ) {
	if      ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_PM_WAFER_SOURCE[Chamber-PM1][Slot-1] , data , "CTC.PM%d_%s_Source%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_BM_Wafer_Source[Chamber-BM1][Slot-1] , data , "CTC.BM%d_%s_Source%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) { // 2018.12.13
	}
	else if ( Chamber == AL   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_AL_Wafer_Source , data , "CTC.AL_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IC_Wafer_Source , data , "CTC.IC_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL_Wafer_Source , data , "CTC.FM_AL_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_SOURCE[0] , data , "CTC.FM_IC_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			return;
		}
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_SOURCE[Slot-1] , data , "CTC.FM_IC_%s_Source%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL2_Wafer_Source , data , "CTC.FM_AL2_%s_Source" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Source[Chamber-10000][Slot] , data );
		}
		else if ( Slot == TB_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Source[Chamber-10000][Slot] , data );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Source[Chamber-10000][Slot-2] , data , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Source[Chamber-10000][Slot-1001] , data , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Source[Chamber-10000][Slot-2001] , data , "CTC.FB%s_%s_Source%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Source" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Source" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE[Chamber-30000][Slot] , data , "CTC.T%c%s_%s_Source2" , Slot + 'A' , MULMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE[Chamber-30000][Slot] , data , "CTC.T%c%s_%s_Source2" , Slot + 'A' , TMMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
}
//---------------------------------------------------------------------------
int _i_SCH_IO_GET_MODULE_SOURCE_E( int Chamber , int Slot ) { // 2018.09.05
	if      ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_PM_WAFER_SOURCE_E[Chamber-PM1][Slot-1] , "CTC.PM%d_%s_SourceE%s" , Chamber - PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_BM_Wafer_Source_E[Chamber-BM1][Slot-1] , "CTC.BM%d_%s_SourceE%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_CM_Wafer_Source_E[Chamber-CM1] , "CTC.CM%d_%s_SourceE" , Chamber-CM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == AL   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_AL_Wafer_Source_E , "CTC.AL_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_IC_Wafer_Source_E , "CTC.IC_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL_Wafer_Source_E , "CTC.FM_AL_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_SOURCE_E[0] , "CTC.FM_IC_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_SOURCE_E[Slot-1] , "CTC.FM_IC_%s_SourceE%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		return IO_ADD_READ_DIGITAL_NAME( &Address_FM_AL2_Wafer_Source_E , "CTC.FM_AL2_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_F_Wafer_Source_E[Chamber-10000][Slot] , "CTC.FA%s_%s_SourceE" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else if ( Slot == TB_STATION ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_F_Wafer_Source_E[Chamber-10000][Slot] , "CTC.FB%s_%s_SourceE" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Source_E[Chamber-10000][Slot-2] , "CTC.FA%s_%s_SourceE%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE_E[Chamber-20000][Slot] , "CTC.T%c%s_%s_SourceE" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE_E[Chamber-30000][Slot] , "CTC.T%c%s_%s_SourceE2" , Slot + 'A' , TMMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	return 0;
}
//---------------------------------------------------------------------------
void _i_SCH_IO_SET_MODULE_SOURCE_E( int Chamber , int Slot , int data ) { // 2018.09.05
	if      ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_PM_WAFER_SOURCE_E[Chamber-PM1][Slot-1] , data , "CTC.PM%d_%s_SourceE%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_BM_Wafer_Source_E[Chamber-BM1][Slot-1] , data , "CTC.BM%d_%s_SourceE%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( ( Chamber >= CM1 ) && ( Chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_CM_Wafer_Source_E[Chamber-CM1] , data , "CTC.CM%d_%s_SourceE" , Chamber-CM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == AL   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_AL_Wafer_Source_E , data , "CTC.AL_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC   ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IC_Wafer_Source_E , data , "CTC.IC_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL_Wafer_Source_E , data , "CTC.FM_AL_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_SOURCE_E[0] , data , "CTC.FM_IC_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			return;
		}
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_SOURCE_E[Slot-1] , data , "CTC.FM_IC_%s_SourceE%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_AL2_Wafer_Source_E , data , "CTC.FM_AL2_%s_SourceE" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if      ( Slot == TA_STATION ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_F_Wafer_Source_E[Chamber-10000][Slot] , data , "CTC.FA%s_%s_SourceE" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else if ( Slot == TB_STATION ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_F_Wafer_Source_E[Chamber-10000][Slot] , data , "CTC.FB%s_%s_SourceE" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Source_E[Chamber-10000][Slot-2] , data , "CTC.FA%s_%s_SourceE%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE_E[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_SourceE" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE_E[Chamber-30000][Slot] , data , "CTC.T%c%s_%s_SourceE2" , Slot + 'A' , TMMDLSTR[ Chamber-30000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
}
//---------------------------------------------------------------------------
int _i_SCH_IO_GET_MODULE_RESULT( int Chamber , int Slot ) {
	if      ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return 0;
		//
		if ( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] == -2 ) Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_%s_Result%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
		//
		if ( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] < 0 ) { // 2008.05.21
			return IO_ADD_READ_DIGITAL_NAME( &Address_PM_WAFER_RESULT[Chamber-PM1][0] , "CTC.PM%d_%s_Result" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2008.05.21
		}
		else {
			return IO_ADD_READ_DIGITAL( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] );
		}
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_BM_Wafer_Result[Chamber-BM1][Slot-1] , "CTC.BM%d_%s_Result%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == AL ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_AL_CH_RESULT , "CTC.AL_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_IC_CH_RESULT , "CTC.IC_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FE_RB_AL_RESULT , "CTC.FM_AL_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == 1001 ) { // F_AL2
		return IO_ADD_READ_DIGITAL_NAME( &Address_FE_RB_AL2_RESULT , "CTC.FM_AL2_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_RESULT[0] , "CTC.FM_IC_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return 0;
		return IO_ADD_READ_DIGITAL_NAME( &Address_FIC_CH_RESULT[Slot-1] , "CTC.FM_IC_%s_Result%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot - 1 ] );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if ( Slot == TA_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Result[Chamber-10000][Slot] );
		}
		else if ( Slot == TB_STATION ) {
			return IO_ADD_READ_DIGITAL( Address_F_Wafer_Result[Chamber-10000][Slot] );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Result[Chamber-10000][Slot-2] , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Result[Chamber-10000][Slot-1001] , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Result[Chamber-10000][Slot-2001] , "CTC.FB%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[Chamber-20000][Slot] , "CTC.T%c%s_%s_Result" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[Chamber-20000][Slot] , "CTC.T%c%s_%s_Result" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
	}
	return 0;
}
//---------------------------------------------------------------------------
void _i_SCH_IO_SET_MODULE_RESULT( int Chamber , int Slot , int data ) {
	if ( ( Chamber >= PM1 ) && ( Chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_PM_SLOT_DEPTH ) ) return;
		if ( Slot == 1 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_PM_WAFER_RESULT[Chamber-PM1][0] , data , "CTC.PM%d_%s_Result" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else {
			//
			if ( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] == -2 ) Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_%s_Result%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
			//
			if ( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] >= 0 ) {
				IO_ADD_WRITE_DIGITAL( Address_PM_WAFER_RESULT[Chamber-PM1][Slot-1] , data );
			}
			else {
				IO_ADD_WRITE_DIGITAL_NAME( &Address_PM_WAFER_RESULT[Chamber-PM1][0] , data , "CTC.PM%d_%s_Result" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			}
		}
	}
	else if ( ( Chamber >= BM1 ) && ( Chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( Slot < 1 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_BM_Wafer_Result[Chamber-BM1][Slot-1] , data , "CTC.BM%d_%s_Result%s" , Chamber-BM1+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot-1 ] );
	}
	else if ( Chamber == AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_AL_CH_RESULT , data , "CTC.AL_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == IC ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IC_CH_RESULT , data , "CTC.IC_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FE_RB_AL_RESULT , data , "CTC.FM_AL_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == 1001 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FE_RB_AL2_RESULT , data , "CTC.FM_AL2_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Chamber == F_IC ) {
		if ( Slot == 0 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_RESULT[0] , data , "CTC.FM_IC_%s_Result" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
			return;
		}
		if ( ( Slot < 1 ) || ( Slot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FIC_CH_RESULT[Slot-1] , data , "CTC.FM_IC_%s_Result%s" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ Slot - 1 ] );
	}
	else if ( ( Chamber >= 10000 ) && ( Chamber < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ) ) { // FM
		if ( Slot == TA_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Result[Chamber-10000][Slot] , data );
		}
		else if ( Slot == TB_STATION ) {
			IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Result[Chamber-10000][Slot] , data );
		}
		else if ( ( Slot >= 2 ) && ( Slot < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Result[Chamber-10000][Slot-2] , data , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
		}
		else if ( ( Slot >= 1001 ) && ( Slot < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Result[Chamber-10000][Slot-1001] , data , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 1000 );
		}
		else if ( ( Slot >= 2001 ) && ( Slot < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Result[Chamber-10000][Slot-2001] , data , "CTC.FB%s_%s_Result%d" , MULMDLSTR[ Chamber-10000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot - 2000 );
		}
	}
	else if ( ( Chamber >= 20000 ) && ( Chamber < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Result" , Slot + 'A' , MULMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[Chamber-20000][Slot] , data , "CTC.T%c%s_%s_Result" , Slot + 'A' , TMMDLSTR[ Chamber-20000 ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	}
	else if ( ( Chamber >= 30000 ) && ( Chamber < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ) ) { // TM2
	}
}
//---------------------------------------------------------------------------
int WAFER_STATUS_IN_FM( int fms , int Finger ) {
	if      ( Finger == TA_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Status[fms][Finger] );
	}
	else if ( Finger == TB_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Status[fms][Finger] );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Status[fms][Finger-2] , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Status[fms][Finger-1001] , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Status[fms][Finger-2001] , "CTC.FB%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
	else return 0;
}
//
void WAFER_STATUS_SET_FM( int fms , int Finger , int data ) {
	int i;
	if      ( Finger == TA_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[fms][Finger] , data );
	}
	else if ( Finger == TB_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[fms][Finger] , data );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Status[fms][Finger-2] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Status[fms][Finger-1001] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Status[fms][Finger-2001] , data , "CTC.FB%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
	else if ( Finger == -1 ) { // 2007.08.10
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[fms][0] , data );
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Status[fms][1] , data );
	}
	else if ( Finger == -2 ) { // 2007.08.10
		for ( i = 0 ; i < MAX_FM_A_EXTEND_FINGER ; i++ ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Status[fms][i] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , i + 2 );
		}
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2018.08.24
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Status[fms][i] , data , "CTC.FA%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , i + 1 );
			IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Status[fms][i] , data , "CTC.FB%s_%s_Status%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , i + 1 );
		}
	}
}
//
int WAFER_SOURCE_IN_FM( int fms , int Finger ) {
	if      ( Finger == TA_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Source[fms][Finger] );
	}
	else if ( Finger == TB_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Source[fms][Finger] );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Source[fms][Finger-2] , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Source[fms][Finger-1001] , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Source[fms][Finger-2001] , "CTC.FB%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
	else return 0;
}
//
void WAFER_SOURCE_SET_FM( int fms , int Finger , int data ) {
	if      ( Finger == TA_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Source[fms][Finger] , data );
	}
	else if ( Finger == TB_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Source[fms][Finger] , data );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Source[fms][Finger-2] , data , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Source[fms][Finger-1001] , data , "CTC.FA%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Source[fms][Finger-2001] , data , "CTC.FB%s_%s_Source%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
}
//
int WAFER_SOURCE_IN_FM_E( int fms , int Finger ) {
	if      ( Finger == TA_STATION ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_F_Wafer_Source_E[fms][Finger] , "CTC.FA%s_%s_SourceE" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Finger == TB_STATION ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_F_Wafer_Source_E[fms][Finger] , "CTC.FB%s_%s_SourceE" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Source_E[fms][Finger-2] , "CTC.FA%s_%s_SourceE%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else return 0;
}
//
void WAFER_SOURCE_SET_FM_E( int fms , int Finger , int data ) {
	if      ( Finger == TA_STATION ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_F_Wafer_Source_E[fms][Finger] , data , "CTC.FA%s_%s_SourceE" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( Finger == TB_STATION ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_F_Wafer_Source_E[fms][Finger] , data , "CTC.FB%s_%s_SourceE" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Source_E[fms][Finger-2] , data , "CTC.FA%s_%s_SourceE%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
}
//
int WAFER_RESULT_IN_FM( int fms , int Finger ) {
	if ( Finger == TA_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Result[fms][Finger] );
	}
	else if ( Finger == TB_STATION ) {
		return IO_ADD_READ_DIGITAL( Address_F_Wafer_Result[fms][Finger] );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAE_Wafer_Result[fms][Finger-2] , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FAM_Wafer_Result[fms][Finger-1001] , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		return IO_ADD_READ_DIGITAL_NAME( &Address_FBM_Wafer_Result[fms][Finger-2001] , "CTC.FB%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
	return 0;
}
//
void WAFER_RESULT_SET_FM( int fms , int Finger , int data ) {
	if ( Finger == TA_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Result[fms][Finger] , data );
	}
	else if ( Finger == TB_STATION ) {
		IO_ADD_WRITE_DIGITAL( Address_F_Wafer_Result[fms][Finger] , data );
	}
	else if ( ( Finger >= 2 ) && ( Finger < ( MAX_FM_A_EXTEND_FINGER + 2 ) ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAE_Wafer_Result[fms][Finger-2] , data , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger );
	}
	else if ( ( Finger >= 1001 ) && ( Finger < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FAM_Wafer_Result[fms][Finger-1001] , data , "CTC.FA%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 1000 );
	}
	else if ( ( Finger >= 2001 ) && ( Finger < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2018.08.24
		IO_ADD_WRITE_DIGITAL_NAME( &Address_FBM_Wafer_Result[fms][Finger-2001] , data , "CTC.FB%s_%s_Result%d" , MULMDLSTR[ fms ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Finger - 2000 );
	}
}
//
//============================================================================================
//
int WAFER_STATUS_IN_TM( int ATM , int Finger ) {
//	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_STATUS[ATM][Finger] , "CTC.T%c%s_%s_Status" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_STATUS[ATM][Finger] , "CTC.T%c%s_%s_Status" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
int WAFER_STATUS_IN_TM2( int ATM , int Finger ) {
//	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_STATUS[ATM][Finger] , "CTC.T%c%s_%s_Status2" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_STATUS[ATM][Finger] , "CTC.T%c%s_%s_Status2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_STATUS_SET_TM( int ATM , int Finger , int data ) {
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_STATUS[ATM][Finger] , data , "CTC.T%c%s_%s_Status" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_STATUS[ATM][Finger] , data , "CTC.T%c%s_%s_Status" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_STATUS_SET_TM2( int ATM , int Finger , int data ) {
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_STATUS[ATM][Finger] , data , "CTC.T%c%s_%s_Status2" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_STATUS[ATM][Finger] , data , "CTC.T%c%s_%s_Status2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
int WAFER_SOURCE_IN_TM( int ATM , int Finger ) {
//	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE[ATM][Finger] , "CTC.T%c%s_%s_Source" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE[ATM][Finger] , "CTC.T%c%s_%s_Source" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
int WAFER_SOURCE_IN_TM2( int ATM , int Finger ) {
//	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE[ATM][Finger] , "CTC.T%c%s_%s_Source2" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE[ATM][Finger] , "CTC.T%c%s_%s_Source2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_SOURCE_SET_TM( int ATM , int Finger , int data ) {
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE[ATM][Finger] , data , "CTC.T%c%s_%s_Source" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE[ATM][Finger] , data , "CTC.T%c%s_%s_Source" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_SOURCE_SET_TM2( int ATM , int Finger , int data ) {
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE[ATM][Finger] , data , "CTC.T%c%s_%s_Source2" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE[ATM][Finger] , data , "CTC.T%c%s_%s_Source2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//

int WAFER_SOURCE_IN_TM_E( int ATM , int Finger ) { // 2018.09.05
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R_SOURCE_E[ATM][Finger] , "CTC.T%c%s_%s_SourceE" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
}
//
int WAFER_SOURCE_IN_TM2_E( int ATM , int Finger ) { // 2018.09.05
	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_R2_SOURCE_E[ATM][Finger] , "CTC.T%c%s_%s_SourceE2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_SOURCE_SET_TM_E( int ATM , int Finger , int data ) { // 2018.09.05
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R_SOURCE_E[ATM][Finger] , data , "CTC.T%c%s_%s_SourceE" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//
void WAFER_SOURCE_SET_TM2_E( int ATM , int Finger , int data ) { // 2018.09.05
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_R2_SOURCE_E[ATM][Finger] , data , "CTC.T%c%s_%s_SourceE2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
}
//




//int WAFER_RESULT_IN_TM( int ATM , int Finger ) { // 2004.01.05 // 2015.05.27
////	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , "CTC.T%c%s_%s_Result" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
//	return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
//}
//
//void WAFER_RESULT_SET_TM( int ATM , int Finger , int data ) { // 2015.05.27
////	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , data , "CTC.T%c%s_%s_Result" , Finger + 'A' , MULMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , data , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() ); // 2013.03.09
//}
//
//
int WAFER_RESULT_IN_TM( int ATM , int Finger , int slot ) { // 2015.05.27
	int Res;
	if ( slot <= 0 ) {
		return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else {
		Res = IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB2_RESULT[ATM][Finger] , "CTC.T%c%s_%s_Result2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( Address_TM_RB2_RESULT[ATM][Finger] < 0 ) {
			return IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
		else {
			return Res;
		}
	}
}
//
void WAFER_RESULT_SET_TM( int ATM , int Finger , int slot , int data ) { // 2015.05.27
	if ( slot <= 0 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , data , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
	}
	else {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB2_RESULT[ATM][Finger] , data , "CTC.T%c%s_%s_Result2" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		if ( Address_TM_RB2_RESULT[ATM][Finger] < 0 ) {
			IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_RESULT[ATM][Finger] , data , "CTC.T%c%s_%s_Result" , Finger + 'A' , TMMDLSTR[ ATM ] , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		}
	}
}
//
BOOL WAFER_SOURCE_IO_CHECK( int Chamber , int depth ) { // 2007.03.20
	//
	if ( Address_PM_WAFER_SOURCE[Chamber-PM1][ depth - 1 ] == -2 ) Address_PM_WAFER_SOURCE[Chamber-PM1][ depth - 1 ] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_%s_Source%s" , Chamber-PM1 + 1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , MULMDLSTR[ depth-1 ] );
	//
	if ( Address_PM_WAFER_SOURCE[Chamber-PM1][ depth - 1 ] < 0 ) return FALSE;
	return TRUE;
}
//

//----------------------------------------------------------------------------

BOOL WAFER_STATUS_IN_ALL_CASSETTE( int cass1 , int cass2 , char *String_Read_Buffer ) {
	char SendBuffer[512];
	char ImsiBuffer[256];
	char ReadBuffer[64];
	int fi , ni , ix , md;
	//
	strcpy( String_Read_Buffer , "" );
	strcpy( SendBuffer , "" );
	//
	if ( _i_SCH_PRM_GET_DFIX_CASSETTE_READ_MODE() == 0 ) { // 2006.02.16
		if ( cass1 != 0 ) {
			for ( ix = 0 ; ix < MAX_CASSETTE_SLOT_SIZE ; ix++ ) {
				ni = _i_SCH_IO_GET_MODULE_STATUS( cass1 , ix + 1 );
				if ( ( ni < 0 ) || ( ni >= 10 ) ) { // 2008.04.23
					sprintf( ImsiBuffer , "0" );
				}
				else {
					sprintf( ImsiBuffer , "%d" , ni );
				}
				strcat( String_Read_Buffer , ImsiBuffer );
			}
		}
		if ( cass2 != 0 ) {
			for ( ix = 0 ; ix < MAX_CASSETTE_SLOT_SIZE ; ix++ ) {
				ni = _i_SCH_IO_GET_MODULE_STATUS( cass2 , ix + 1 );
				if ( ( ni < 0 ) || ( ni >= 10 ) ) { // 2008.04.23
					sprintf( ImsiBuffer , "0" );
				}
				else {
					sprintf( ImsiBuffer , "%d" , ni );
				}
				strcat( String_Read_Buffer , ImsiBuffer );
			}
		}
		return TRUE;
	}
	else {
		if ( cass1 != 0 ) {
			for ( ix = 0 ; ix < MAX_CASSETTE_SLOT_SIZE ; ix++ ) {
				if ( Address_Cassette_Wafer_ID[cass1-CM1][ix] == -2 ) Address_Cassette_Wafer_ID[cass1-CM1][ix] = _FIND_FROM_STRINGF( _K_D_IO , "CM%d.C%02d_%s" , cass1-CM1+1 , ix+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
				//
				if ( Address_Cassette_Wafer_ID[cass1-CM1][ix] >= 0 ) {
					sprintf( ImsiBuffer , "D%d" , Address_Cassette_Wafer_ID[cass1-CM1][ix] );
					strcat( SendBuffer , ImsiBuffer );
				}
			}
		}
		if ( cass2 != 0 ) {
			for ( ix = 0 ; ix < MAX_CASSETTE_SLOT_SIZE ; ix++ ) {
				if ( Address_Cassette_Wafer_ID[cass2-CM1][ix] == -2 ) Address_Cassette_Wafer_ID[cass2-CM1][ix] = _FIND_FROM_STRINGF( _K_D_IO , "CM%d.C%02d_%s" , cass2-CM1+1 , ix+1 , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
				//
				if ( Address_Cassette_Wafer_ID[cass2-CM1][ix] >= 0 ) {
					sprintf( ImsiBuffer , "D%d" , Address_Cassette_Wafer_ID[cass2-CM1][ix] );
					strcat( SendBuffer , ImsiBuffer );
				}
			}
		}
		_dGROUP_IO_READING( SendBuffer );
		while( TRUE ) {
			if ( !WAIT_SECONDS( 0.25 ) ) return FALSE;
			if ( _dGROUP_IO_READING_REMAIN_COUNT() <= 0 ) break;
		}
		if ( _dGROUP_IO_READ_from_MEM_only_Data( SendBuffer , ImsiBuffer , 511 ) ) {
			fi = 0;
			ix = 0;
			if      ( cass1 != 0 ) md = 0;
			else if ( cass2 != 0 ) md = 1;
			else                   md = 2;
			while ( _dGROUP_IO_TRANSLATE_from_BUFFER_only_Data( ImsiBuffer , fi , ReadBuffer , &ni ) ) {
				fi = ni;
				if ( md == 0 ) {
					while( TRUE ) {
						if ( ix >= MAX_CASSETTE_SLOT_SIZE ) {
							md = 1;
							ix = 0;
							break;
						}
						if ( Address_Cassette_Wafer_ID[cass1-CM1][ix] >= 0 ) {
							sprintf( ImsiBuffer , "%c" , ReadBuffer[0] );
							strcat( String_Read_Buffer , ImsiBuffer );
							ix++;
							break;
						}
						else {
							strcat( String_Read_Buffer , "0" );
							ix++;
						}
					}
				}
				if ( md == 1 ) {
					while( TRUE ) {
						if ( ix >= MAX_CASSETTE_SLOT_SIZE ) {
							md = 2;
							ix = 0;
							break;
						}
						if ( Address_Cassette_Wafer_ID[cass2-CM1][ix] >= 0 ) {
							sprintf( ImsiBuffer , "%c" , ReadBuffer[0] );
							strcat( String_Read_Buffer , ImsiBuffer );
							ix++;
							break;
						}
						else {
							strcat( String_Read_Buffer , "0" );
							ix++;
						}
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void WAFER_TIMER_SET_FIC( int Slot , int data ) { // 2005.12.01
//	printf( "[G:%d:%d]" , Slot , data );
	if ( Address_FIC_CH_TIMER[Slot-1] == -2 ) {
		if ( Slot == 1 )	Address_FIC_CH_TIMER[Slot-1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FM_IC_%s_Timer" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() );
		else				Address_FIC_CH_TIMER[Slot-1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FM_IC_%s_Timer%d" , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , Slot );
	}
//	printf( "[H:%d:%d]" , Slot , data );
	IO_ADD_WRITE_DIGITAL( Address_FIC_CH_TIMER[Slot-1] , data );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int _i_SCH_IO_GET_IN_PATH( int index ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_InPath[index] , "CTC.IN_PATH%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_IN_PATH( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_InPath[index] , data , "CTC.IN_PATH%s" , MULMDLSTR[ index ] );
}
int _i_SCH_IO_GET_OUT_PATH( int index ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_OutPath[index] , "CTC.OUT_PATH%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_OUT_PATH( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_OutPath[index] , data , "CTC.OUT_PATH%s" , MULMDLSTR[ index ] );
}
//
int _i_SCH_IO_GET_START_SLOT( int index ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_StartSlot[index] , "CTC.START_SLOT%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_START_SLOT( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_StartSlot[index] , data , "CTC.START_SLOT%s" , MULMDLSTR[ index ] );
}
//
int _i_SCH_IO_GET_END_SLOT( int index ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_EndSlot[index] , "CTC.END_SLOT%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_END_SLOT( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_EndSlot[index] , data , "CTC.END_SLOT%s" , MULMDLSTR[ index ] );
}
//
int _i_SCH_IO_GET_START_INDEX( int index ) { // 2015.10.12
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_StartIndex[index] , "CTC.START_INDEX%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_START_INDEX( int index , int data ) { // 2015.10.12
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_StartIndex[index] , data , "CTC.START_INDEX%s" , MULMDLSTR[ index ] );
}
//
int _i_SCH_IO_GET_END_INDEX( int index ) { // 2015.10.12
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_EndIndex[index] , "CTC.END_INDEX%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_END_INDEX( int index , int data ) { // 2015.10.12
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_EndIndex[index] , data , "CTC.END_INDEX%s" , MULMDLSTR[ index ] );
}
//
int _i_SCH_IO_GET_LOOP_COUNT( int index ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_LoopCount[index] , "CTC.LOOP_COUNT%s" , MULMDLSTR[ index ] );
}
//
void _i_SCH_IO_SET_LOOP_COUNT( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_LoopCount[index] , data , "CTC.LOOP_COUNT%s" , MULMDLSTR[ index ] );
}
//---------------------------------------------------------------------------
int IO_GET_PAUSE_STATUS() {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTC_Pause_Status , "CTC.PAUSE_STATUS" );
}
//
void IO_SET_PAUSE_STATUS( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTC_Pause_Status , data , "CTC.PAUSE_STATUS" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
void _i_SCH_IO_GET_RECIPE_FILE( int index , char *data ) {
	if ( ( index / 100 ) > 0 ) { // 2010.03.10 bmstart
		IO_ADD_READ_STRING_NAME( &Address_CTRL_RecipeBMFile[index%100] , data , "CTC.RECIPE_BM_FILE%s" , MULMDLSTR[ index%100 ] ); // 2010.03.10
	}
	else {
		IO_ADD_READ_STRING_NAME( &Address_CTRL_RecipeFile[index] , data , "CTC.RECIPE_FILE%s" , MULMDLSTR[ index ] );
	}
}
//
void _i_SCH_IO_SET_RECIPE_FILE( int index , char *data ) {
	if ( ( index / 100 ) > 0 ) { // 2010.03.10 bmstart
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipeBMFile[index%100] , data , "CTC.RECIPE_BM_FILE%s" , MULMDLSTR[ index%100 ] );
	}
	else {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipeFile[index] , data , "CTC.RECIPE_FILE%s" , MULMDLSTR[ index ] );
	}
}
//
void _i_SCH_IO_SET_RECIPE_PRCS_FILE( int ch , char *data ) { // 2011.09.01
	if        ( ch == FM ) {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipePrcsFile[ch] , data , "CTC.RECIPE_PRCS_FM" );
	}
	else if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipePrcsFile[ch] , data , "CTC.RECIPE_PRCS_CM%d" , ch-CM1+1 );
	}
	else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipePrcsFile[ch] , data , "CTC.RECIPE_PRCS_PM%d" , ch-PM1+1 );
	}
	else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipePrcsFile[ch] , data , "CTC.RECIPE_PRCS_BM%d" , ch-BM1+1 );
	}
	else if   ( ch >= TM ) {
		IO_ADD_WRITE_STRING_NAME( &Address_CTRL_RecipePrcsFile[ch] , data , "CTC.RECIPE_PRCS_TM%d" , ch-TM+1 );
	}
}
//
void IO_SET_JID_NAME( int index , char *data ) {
	IO_ADD_WRITE_STRING_NAME( &Address_JOB[index] , data , "CTC.JOB_NAME%s" , MULMDLSTR[ index ] );
}
//
void IO_GET_JID_NAME( int index , char *data ) {
	IO_ADD_READ_STRING_NAME( &Address_JOB[index] , data , "CTC.JOB_NAME%s" , MULMDLSTR[ index ] );
}
//
void IO_SET_MID_NAME( int index , char *data ) {
	IO_ADD_WRITE_STRING_NAME( &Address_MID[index] , data , "CTC.MID_NAME%s" , MULMDLSTR[ index ] );
}
//
void IO_GET_MID_NAME( int index , char *data ) {
	IO_ADD_READ_STRING_NAME( &Address_MID[index] , data , "CTC.MID_NAME%s" , MULMDLSTR[ index ] );
}
//
void IO_SET_MID_NAME_FROM_READ( int index , char *data ) {
	IO_ADD_WRITE_STRING_NAME( &Address_MID_READ[index] , data , "CTC.MID_READ%s" , MULMDLSTR[ index ] );
}
//
void IO_GET_MID_NAME_FROM_READ( int index , char *data ) {
	IO_ADD_READ_STRING_NAME( &Address_MID_READ[index] , data , "CTC.MID_READ%s" , MULMDLSTR[ index ] );
}
//
void IO_GET_JID_NAME_FROM_READ( int index , char *data ) {
	IO_ADD_READ_STRING_NAME( &Address_JID_READ[index] , data , "CTC.JID_READ%s" , MULMDLSTR[ index ] );
}
//
void IO_CLEAR_JID_NAME_FROM_READ( int index ) {
	IO_ADD_WRITE_STRING_NAME( &Address_JID_READ[index] , "" , "CTC.JID_READ%s" , MULMDLSTR[ index ] );
}
//
void IO_CLEAR_MID_NAME_FROM_READ( int index ) {
	IO_ADD_WRITE_STRING_NAME( &Address_MID_READ[index] , "" , "CTC.MID_READ%s" , MULMDLSTR[ index ] );
}
//
void IO_GET_NID_NAME_FROM_READ( int index , char *data ) {
	IO_ADD_READ_STRING_NAME( &Address_NID_READ[index] , data , "CTC.NID_READ%s" , MULMDLSTR[ index ] );
}
//
BOOL _i_SCH_IO_GET_WID_NAME_FROM_READ( int index , int mode , char *data ) {
	if ( mode == 0 ) {
		if ( Address_WID_NAME        == -2 ) Address_WID_NAME        = _FIND_FROM_STRING( _K_S_IO , "CTC.WID_NAME" );
		if ( Address_WID_READ[index] == -2 ) Address_WID_READ[index] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.WID_READ%s" , MULMDLSTR[ index ] );
		//
		if ( Address_WID_NAME < 0 ) {
			if ( Address_WID_READ[index] < 0 ) {
				strcpy( data , "" );
				return FALSE;
			}
			else {
				_dREAD_STRING( Address_WID_READ[index] , data , &COMM );
				if ( strlen( data ) <= 0 ) return FALSE;
				_dWRITE_STRING_TH( Address_WID_READ[index] , "" );
			}
		}
		else {
			_dREAD_STRING( Address_WID_NAME , data , &COMM );
			if ( strlen( data ) <= 0 ) {
				if ( Address_WID_READ[index] < 0 ) {
					return FALSE;
				}
				else {
					_dREAD_STRING( Address_WID_READ[index] , data , &COMM );
					if ( strlen( data ) <= 0 ) return FALSE;
					_dWRITE_STRING_TH( Address_WID_READ[index] , "" );
				}
			}
			else {
				if ( Address_WID_READ[index] >= 0 ) {
					_dWRITE_STRING_TH( Address_WID_READ[index] , "" );
				}
			}
		}
	}
	else {
		if ( Address_WID_NAME2        == -2 ) Address_WID_NAME2        = _FIND_FROM_STRING( _K_S_IO , "CTC.WID_NAMEB" );
		if ( Address_WID_READ2[index] == -2 ) Address_WID_READ2[index] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.WID_READB%s" , MULMDLSTR[ index ] );
		//
		if ( Address_WID_NAME2 < 0 ) {
			if ( Address_WID_READ2[index] < 0 ) {
				strcpy( data , "" );
				return FALSE;
			}
			else {
				_dREAD_STRING( Address_WID_READ2[index] , data , &COMM );
				if ( strlen( data ) <= 0 ) return FALSE;
				_dWRITE_STRING_TH( Address_WID_READ2[index] , "" );
			}
		}
		else {
			_dREAD_STRING( Address_WID_NAME2 , data , &COMM );
			if ( strlen( data ) <= 0 ) {
				if ( Address_WID_READ2[index] < 0 ) {
					return FALSE;
				}
				else {
					_dREAD_STRING( Address_WID_READ2[index] , data , &COMM );
					if ( strlen( data ) <= 0 ) return FALSE;
					_dWRITE_STRING_TH( Address_WID_READ2[index] , "" );
				}
			}
			else {
				if ( Address_WID_READ2[index] >= 0 ) {
					_dWRITE_STRING_TH( Address_WID_READ2[index] , "" );
				}
			}
		}
	}
	return TRUE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _SCH_IO_SET_MAIN_BM_MODE( int index , int data ) { // 2010.02.19
	CURRENT_MAIN_BM_MODE[index] = data;
}

BOOL _SCH_IO_GET_MAIN_BM_MODE( int index ) { // 2010.02.19
	return CURRENT_MAIN_BM_MODE[index];
}

void _SCH_IO_SET_MAIN_BUTTON_ONLY( int index , int data ) { // 2008.09.23
	if ( CURRENT_MAIN_CONTROL[index] == 0 ) {
		if ( CURRENT_MAIN_IO_CONTROL[index] != -1 ) {
			CURRENT_MAIN_IO_CONTROL[index] = data;
			IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_MainControl[index] , data , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
		}
	}
}

void _i_SCH_IO_SET_MAIN_BUTTON( int index , int data ) {
	if ( index == MAX_CASSETTE_SIDE ) { // 2012.08.29
		BUTTON_SET_TR_CONTROL( data );
	}
	else {
		CURRENT_MAIN_CONTROL[index] = data;
		CURRENT_MAIN_IO_CONTROL[index] = -1;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_MainControl[index] , data , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
	}
}
int _i_SCH_IO_GET_MAIN_BUTTON( int index ) {
	if ( index == MAX_CASSETTE_SIDE ) { // 2012.08.29
		return BUTTON_GET_TR_CONTROL();
	}
	else {
		return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_MainControl[index] , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
	}
}
void _SCH_IO_SET_MAIN_BUTTON_MC( int index , int data ) {
	if ( CURRENT_MAIN_BM_MODE[index] ) { // 2010.02.19
		CURRENT_MAIN_BM_CONTROL[index] = data;
		CURRENT_MAIN_IO_BM_CONTROL[index] = -1;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_MainBMControl[index] , data , "CTC.MAIN_BM_CONTROL%s" , MULMDLSTR[ index ] );
	}
	else {
		CURRENT_MAIN_CONTROL[index] = data;
		CURRENT_MAIN_IO_CONTROL[index] = -1;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_MainControl[index] , data , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
	}
}
int _SCH_IO_GET_MAIN_BUTTON_MC( int index ) {
	if ( CURRENT_MAIN_BM_MODE[index] ) { // 2010.02.19
		return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_MainBMControl[index] , "CTC.MAIN_BM_CONTROL%s" , MULMDLSTR[ index ] );
	}
	else {
		return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_MainControl[index] , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
	}
}
//
void _SCH_IO_SET_MAIN_BUTTON_EQUAL( int index ) { // 2008.09.23
	if ( CURRENT_MAIN_IO_CONTROL[index] != -1 ) {
		CURRENT_MAIN_IO_CONTROL[index] = -1;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_MainControl[index] , CURRENT_MAIN_CONTROL[index] , "CTC.MAIN_CONTROL%s" , MULMDLSTR[ index ] );
	}
}
//--------------------------------------------------------
void BUTTON_SET_HANDOFF_CONTROL( int Chamber , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_HandOff[Chamber-CM1] , data , "CTC.CM%dH_CONTROL" , Chamber-CM1 + 1 );
}
int BUTTON_GET_HANDOFF_CONTROL( int Chamber ) {
	return IO_ADD_READ_DIGITAL_NAME( &Address_CTRL_HandOff[Chamber-CM1] , "CTC.CM%dH_CONTROL" , Chamber-CM1 + 1 );
}
void _i_SCH_IO_SET_END_BUTTON( int index , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_CTRL_EndControl[index] , data , "CTC.END_CONTROL%s" , MULMDLSTR[ index ] );
}
void BUTTON_SET_TR_CONTROL( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TR_CONTROL , data , "CTC.TR_CONTROL" );
}
int BUTTON_GET_TR_CONTROL() {
	return IO_ADD_READ_DIGITAL_NAME( &Address_TR_CONTROL , "CTC.TR_CONTROL" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BUTTON_SET_FLOW_STATUS( int index , int data ) {
	if ( data == 0 ) {
		CURRENT_MAIN_BM_STATUS[index] = data;
		CURRENT_MAIN_IO_BM_STATUS[index] = data;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_BM_STATUS[index] , data , "CTC.MAIN_BM_STATUS%s" , MULMDLSTR[ index ] );
		//
		CURRENT_MAIN_STATUS[index] = data;
		CURRENT_MAIN_IO_STATUS[index] = data;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_STATUS[index] , data , "CTC.MAIN_STATUS%s" , MULMDLSTR[ index ] );
	}
	else if ( ( data >= 7 ) && ( data <= 17 ) ) {
		if ( CURRENT_MAIN_BM_MODE[index] ) { // 2010.02.19
			CURRENT_MAIN_BM_STATUS[index] = data;
			CURRENT_MAIN_IO_BM_STATUS[index] = data;
			IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_BM_STATUS[index] , data , "CTC.MAIN_BM_STATUS%s" , MULMDLSTR[ index ] );
		}
		else {
			CURRENT_MAIN_STATUS[index] = data;
			CURRENT_MAIN_IO_STATUS[index] = data;
			IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_STATUS[index] , data , "CTC.MAIN_STATUS%s" , MULMDLSTR[ index ] );
		}
	}
	else { // cm
		CURRENT_MAIN_STATUS[index] = data;
		CURRENT_MAIN_IO_STATUS[index] = data;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_STATUS[index] , data , "CTC.MAIN_STATUS%s" , MULMDLSTR[ index ] );
	}
}
//---------------------------------------------------------------------------
int BUTTON_GET_FLOW_IO_STATUS( int index ) { // 2011.04.14
	return IO_ADD_READ_DIGITAL_NAME( &Address_MAIN_STATUS[index] , "CTC.MAIN_STATUS%s" , MULMDLSTR[ index ] );
}
//

void BUTTON_SET_FLOW_IO_STATUS( int index , int data ) {
	if ( CURRENT_MAIN_IO_STATUS[index] != data ) {
		CURRENT_MAIN_IO_STATUS[index] = data;
		IO_ADD_WRITE_DIGITAL_NAME( &Address_MAIN_STATUS[index] , data , "CTC.MAIN_STATUS%s" , MULMDLSTR[ index ] );
	}
}
//---------------------------------------------------------------------------
void BUTTON_SET_FLOW_STATUS_EQUAL( int index ) {
	if ( CURRENT_MAIN_IO_STATUS[index] != CURRENT_MAIN_STATUS[index] ) {
		BUTTON_SET_FLOW_STATUS( index , CURRENT_MAIN_STATUS[index] );
	}
}
//---------------------------------------------------------------------------
void BUTTON_SET_FLOW_STATUS_VAR( int index , int data ) {
	CURRENT_MAIN_STATUS[index] = data;
}
//---------------------------------------------------------------------------
int BUTTON_GET_FLOW_STATUS_VAR( int index ) {
	if ( ( index / 100 ) == 0 ) {
		return CURRENT_MAIN_STATUS[index];
	}
	else {
		if ( CURRENT_MAIN_BM_MODE[index%100] ) { // 2010.02.19
			return CURRENT_MAIN_BM_STATUS[index%100];
		}
		else {
			return CURRENT_MAIN_STATUS[index%100];
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ROBOT_SET_ARM_DISPLAY_STYLE( int ATM , int i , int data ) {
	IO_ADD_WRITE_DIGITAL( Address_TM_Finger_Use[ATM][i] , data );
}
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
void   ROBOT_SET_SYNCH_STATUS( int ATM , int data ) {
//	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_Synch[ATM] , data , "TM%s.RB_SYNCH" , MULMDLSTR[ ATM ] ); // 2013.03.09
	IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_RB_Synch[ATM] , data , "TM%s.RB_SYNCH" , TMMDLSTR[ ATM ] ); // 2013.03.09
}
int    ROBOT_GET_SYNCH_STATUS( int ATM ) {
	int res;
//	res = IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_Synch[ATM] , "TM%s.RB_SYNCH" , MULMDLSTR[ ATM ] ); // 2013.03.09
	res = IO_ADD_READ_DIGITAL_NAME( &Address_TM_RB_Synch[ATM] , "TM%s.RB_SYNCH" , TMMDLSTR[ ATM ] ); // 2013.03.09
	if ( Address_TM_RB_Synch[ATM] < 0 ) return 99;
	return res;
}
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
/*
// 2013.03.09
void   ROBOT_SET_EXTEND_POSITION( int ATM , int md , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Extension[ATM][md] , data , "CTC.Rb%s_Extension%s" , MULMDLSTR[ ATM ] , MULMDLSTR[ md ] ); }
double ROBOT_GET_EXTEND_POSITION( int ATM , int md ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Extension[ATM][md] , "CTC.Rb%s_Extension%s" , MULMDLSTR[ ATM ] , MULMDLSTR[ md ] ); }

void   ROBOT_SET_ROTATE_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Rotation[ATM] , data , "CTC.Rb%s_Rotation" , MULMDLSTR[ ATM ] ); }
double ROBOT_GET_ROTATE_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Rotation[ATM] , "CTC.Rb%s_Rotation" , MULMDLSTR[ ATM ] ); }

void   ROBOT_SET_UPDOWN_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_UpDown[ATM] , data , "CTC.Rb%s_UpDown" , MULMDLSTR[ ATM ] ); }
double ROBOT_GET_UPDOWN_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_UpDown[ATM] , "CTC.Rb%s_UpDown" , MULMDLSTR[ ATM ] ); }

void   ROBOT_SET_MOVE_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Movement[ATM] , data , "CTC.Rb%s_Movement" , MULMDLSTR[ ATM ] ); }
double ROBOT_GET_MOVE_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Movement[ATM] , "CTC.Rb%s_Movement" , MULMDLSTR[ ATM ] ); }
*/
// 2013.03.09
void   ROBOT_SET_EXTEND_POSITION( int ATM , int md , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Extension[ATM][md] , data , "CTC.Rb%s_Extension%s" , TMMDLSTR[ ATM ] , MULMDLSTR[ md ] ); }
double ROBOT_GET_EXTEND_POSITION( int ATM , int md ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Extension[ATM][md] , "CTC.Rb%s_Extension%s" , TMMDLSTR[ ATM ] , MULMDLSTR[ md ] ); }

void   ROBOT_SET_ROTATE_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Rotation[ATM] , data , "CTC.Rb%s_Rotation" , TMMDLSTR[ ATM ] ); }
double ROBOT_GET_ROTATE_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Rotation[ATM] , "CTC.Rb%s_Rotation" , TMMDLSTR[ ATM ] ); }

void   ROBOT_SET_UPDOWN_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_UpDown[ATM] , data , "CTC.Rb%s_UpDown" , TMMDLSTR[ ATM ] ); }
double ROBOT_GET_UPDOWN_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_UpDown[ATM] , "CTC.Rb%s_UpDown" , TMMDLSTR[ ATM ] ); }

void   ROBOT_SET_MOVE_POSITION( int ATM , double data ) { IO_ADD_WRITE_ANALOG_NAME( &Address_Rb_Movement[ATM] , data , "CTC.Rb%s_Movement" , TMMDLSTR[ ATM ] ); }
double ROBOT_GET_MOVE_POSITION( int ATM ) { return IO_ADD_READ_ANALOG_NAME( &Address_Rb_Movement[ATM] , "CTC.Rb%s_Movement" , TMMDLSTR[ ATM ] ); }
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
void ROBOT_FM_SET_ARM_DISPLAY_STYLE( int fms , int i , int data ) {
	IO_ADD_WRITE_DIGITAL( Address_FM_Finger_Use[fms][i] , data );
}
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
void   ROBOT_FM_SET_SYNCH_STATUS( int fms , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_RB_Synch[fms] , data , "FM%s.RB_SYNCH" , MULMDLSTR[ fms ] );
}
int    ROBOT_FM_GET_SYNCH_STATUS( int fms ) {
	int res;
	res = IO_ADD_READ_DIGITAL_NAME( &Address_FM_RB_Synch[fms] , "FM%s.RB_SYNCH" , MULMDLSTR[ fms ] );
	if ( Address_FM_RB_Synch[fms] < 0 ) return 99;
	return res;
}
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------

void   ROBOT_FM_SET_POSITION( int fms , int mode , double data ) {
	if ( Address_FM_Rb_Anim[fms][mode] == -2 ) {
		if      ( mode == RB_ANIM_EXTEND )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Extension" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_EXTEND2 ) Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Extension2" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_ROTATE )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Rotation" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_UPDOWN )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_UpDown" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_MOVE )    Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Movement" , MULMDLSTR[ fms ] );
	}
	//
	IO_ADD_WRITE_ANALOG( Address_FM_Rb_Anim[fms][mode] , data );
}
double ROBOT_FM_GET_POSITION( int fms , int mode ) {
	if ( Address_FM_Rb_Anim[fms][mode] == -2 ) {
		if      ( mode == RB_ANIM_EXTEND )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Extension" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_EXTEND2 ) Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Extension2" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_ROTATE )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Rotation" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_UPDOWN )  Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_UpDown" , MULMDLSTR[ fms ] );
		else if ( mode == RB_ANIM_MOVE )    Address_FM_Rb_Anim[fms][mode] = _FIND_FROM_STRINGF( _K_A_IO , "CTC.FM%s_Rb_Movement" , MULMDLSTR[ fms ] );
	}
	//
	return IO_ADD_READ_ANALOG( Address_FM_Rb_Anim[fms][mode] );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_MDL_STATUS_SET( int chamber , int data ) {
	IO_ADD_WRITE_DIGITAL( Address_FA_MODULE_STATUS[chamber] , data );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_MDL_SWITCH_SET( int chamber , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_SWITCH_STATUS[chamber] , data , "CTC.FA_SWITCH_SIDE_CM%d" , chamber );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_AGV_STATUS_SET( int chamber , int data ) {
	if ( chamber == 0 ) IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_AGV_STATUS[chamber] , data , "CTC.FA_STATUS_AGV" );
	else                IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_AGV_STATUS[chamber] , data , "CTC.FA_STATUS_AGV_CM%d" , chamber );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_MAP_STATUS_SET( int chamber , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_MAP_STATUS[chamber] , data , "CTC.FA_STATUS_MAP_CM%d" , chamber );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_HANDOFF_STATUS_SET( int chamber , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_HOF_STATUS[chamber] , data , "CTC.FA_STATUS_HOF_CM%d" , chamber );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_RUNTIME_IN_STATUS_SET( int chamber , int data ) {
	if ( chamber < BM1 ) {
		if ( Address_FA_RUNTIME_IN_STATUS[chamber] == -2 ) {
			Address_FA_RUNTIME_IN_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_RUNTIME_IN_PM%d" , chamber - PM1 + 1 );
			if ( Address_FA_RUNTIME_IN_STATUS[chamber] < 0 ) {
				Address_FA_RUNTIME_IN_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_RUNTIME_IN_PM%d" , chamber - PM1 + 1 );
			}
		}
	}
	else {
		if ( Address_FA_RUNTIME_IN_STATUS[chamber] == -2 ) {
			Address_FA_RUNTIME_IN_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_RUNTIME_IN_BM%d" , chamber - BM1 + 1 );
			if ( Address_FA_RUNTIME_IN_STATUS[chamber] < 0 ) {
				Address_FA_RUNTIME_IN_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_RUNTIME_IN_BM%d" , chamber - BM1 + 1 );
			}
		}
	}
	//
	IO_ADD_WRITE_DIGITAL( Address_FA_RUNTIME_IN_STATUS[chamber] , data );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_RUNTIME_OUT_STATUS_SET( int chamber , int data ) {
	if ( chamber < BM1 ) {
		if ( Address_FA_RUNTIME_OUT_STATUS[chamber] == -2 ) {
			Address_FA_RUNTIME_OUT_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_RUNTIME_OUT_PM%d" , chamber - PM1 + 1 );
			if ( Address_FA_RUNTIME_OUT_STATUS[chamber] < 0 ) {
				Address_FA_RUNTIME_OUT_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_RUNTIME_OUT_PM%d" , chamber - PM1 + 1 );
			}
		}
	}
	else {
		if ( Address_FA_RUNTIME_OUT_STATUS[chamber] == -2 ) {
			Address_FA_RUNTIME_OUT_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_STATUS_RUNTIME_OUT_BM%d" , chamber - BM1 + 1 );
			if ( Address_FA_RUNTIME_OUT_STATUS[chamber] < 0 ) {
				Address_FA_RUNTIME_OUT_STATUS[chamber] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.FA_RUNTIME_OUT_BM%d" , chamber - BM1 + 1 );
			}
		}
	}
	IO_ADD_WRITE_DIGITAL( Address_FA_RUNTIME_OUT_STATUS[chamber] , data );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_SCH_RUNTYPE_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_SCH_RUNTYPE_SET , data , "CTC.FA_SCH_START_OPTION" );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_CPJOB_LOGBASED_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_CPJOB_LOGBASED_SET , data , "CTC.FA_CPJOB_LOGBASED" );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_PM_SIDE_SET( int chamber , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_SIDE_STATUS[chamber] , data , "CTC.FA_STATUS_SIDE_PM%d" , chamber - PM1 + 1 );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_RUN_STATUS_SET( int data ) {
	IO_ADD_WRITE_DIGITAL( Address_RUN_STATUS , data );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_TR_STATUS_SET( int index , int data ) {
	if      ( ( index >= 0 ) && ( index < MAX_TM_CHAMBER_DEPTH ) ) {
//		IO_ADD_WRITE_DIGITAL_NAME( &Address_TR_STATUS[index] , data , "CTC.TR%s_STATUS" , MULMDLSTR[ index ] ); // 2013.03.09
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TR_STATUS[index] , data , "CTC.TR%s_STATUS" , TMMDLSTR[ index ] ); // 2013.03.09
	}
	else if ( ( index >= MAX_TM_CHAMBER_DEPTH ) && ( index < MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH ) ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TR_STATUS[index] , data , "CTC.BR%d_STATUS" , index - MAX_TM_CHAMBER_DEPTH + 1 );
	}
	else if ( index == MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TR_STATUS[index] , data , "CTC.FR_STATUS" );
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_MODULE_SIZE_SET( int chamber , int data ) {
	if ( chamber == F_IC ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_MODULE_SIZE_STATUS[chamber] , data , "CTC.FA_MODULE_SIZE_FIC" );
	}
	else if ( chamber < BM1 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_MODULE_SIZE_STATUS[chamber] , data , "CTC.FA_MODULE_SIZE_PM%d" , chamber - PM1 + 1 );
	}
	else {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_MODULE_SIZE_STATUS[chamber] , data , "CTC.FA_MODULE_SIZE_BM%d" , chamber - BM1 + 1 );
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_FM_ARM_STYLE_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FM_ARM_STYLE_SET , data , "CTC.FM_ARM_USING_STYLE" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_TM_ARM_STYLE_SET( int tms , int data ) { // 2014.11.20
	if ( tms == 0 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_ARM_STYLE_SET[tms] , data , "CTC.TM_ARM_USING_STYLE" );
	}
	else {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_TM_ARM_STYLE_SET[tms] , data , "CTC.TM%d_ARM_USING_STYLE" , tms + 1 );
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_SCH_CARRIER_GROUP_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_CARRIER_GROUP_SET , data , "CTC.FA_CARRIER_GROUP" );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_SCH_WAFER_SUPPLY_MODE_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_WAFER_SUPPLY_MODE_SET , data , "CTC.FA_WAFER_SUPPLY_MODE" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_SCH_LOT_SUPPLY_MODE_SET( int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_LOT_SUPPLY_MODE_SET , data , "CTC.FA_LOT_SUPPLY_MODE" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_SCH_CPJOB_MODE_SET( int data ) { // 2008.09.29
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_CPJOB_MODE_SET , data , "CTC.FA_CPJOB_MODE" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_SCH_CLUSTER_INCLUDE_SET( int data ) { // 2005.08.02
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_CLUSTER_INCLUDE_SET , data , "CTC.FA_CLUSTER_INCLUDE" );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_SCH_COOLING_TIME_SET( int data ) { // 2005.11.29
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_COOLING_TIME_SET , data , "CTC.FA_COOLING_TIME" );
}
//
void IO_SCH_COOLING_TIME_SET2( int data ) { // 2011.06.01
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_COOLING_TIME_SET2 , data , "CTC.FA_COOLING_TIME2" );
}

void IO_SCH_BM_SCHEDULING_FACTOR( int data ) { // 2015.03.25
	IO_ADD_WRITE_DIGITAL_NAME( &Address_FA_BM_SCHEDULING_FACTOR , data , "CTC.FA_BM_SCH_FACTOR" );
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void IO_DOUBLE_SIDE_SET( int chamber , int data ) { // 2015.05.27
	if      ( chamber == F_AL ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_DOUBLE_SIDE[chamber] , data , "CTC.FA_DOUBLE_SIDE_FAL" );
	}
	else if ( chamber == F_IC ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_DOUBLE_SIDE[chamber] , data , "CTC.FA_DOUBLE_SIDE_FIC" );
	}
	else if ( chamber < PM1 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_DOUBLE_SIDE[chamber] , data , "CTC.FA_DOUBLE_SIDE_CM%d" , chamber - CM1 + 1 );
	}
	else if ( chamber < BM1 ) {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_DOUBLE_SIDE[chamber] , data , "CTC.FA_DOUBLE_SIDE_PM%d" , chamber - PM1 + 1 );
	}
	else {
		IO_ADD_WRITE_DIGITAL_NAME( &Address_IO_DOUBLE_SIDE[chamber] , data , "CTC.FA_DOUBLE_SIDE_BM%d" , chamber - BM1 + 1 );
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IO_SDM_DUMMY_LOTFIRST( int ch , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_DummyProcess_LotFirst[ch] , data , "CTC.DummyPrcs_PM%d_LotFirst" , ch - PM1 + 1 );
}
void IO_SDM_DUMMY_LOTEND( int ch , int data ) {
	IO_ADD_WRITE_DIGITAL_NAME( &Address_DummyProcess_LotEnd[ch] , data , "CTC.DummyPrcs_PM%d_LotEnd" , ch - PM1 + 1 );
}
BOOL IO_SDM_DUMMY_MODE( int pt , int data ) {
	return IO_ADD_WRITE_DIGITAL_NAME( &Address_DummyProcess_Mode[pt%100] , data , "CTC.DummyPrcs_%d_Mode" , (pt%100) + 1 );
}
BOOL IO_SDM_DUMMY_COUNT( int pt , int style , int data ) {
	return IO_ADD_WRITE_DIGITAL_NAME( &Address_DummyProcess_Count[pt%100][style] , data , "CTC.DummyPrcs_%d_Count%s" , (pt%100) + 1 , MULMDLSTR[style] );
}
BOOL IO_SDM_DUMMY_RECIPE( int wh , int pt , int style , int id , char *data ) {
	//
	if ( Address_DummyProcess_Recipe[wh][pt%100][style][id] == -2 ) {
		if ( id == 0 ) {
			if      ( wh == 0 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_RunRecipe%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 1 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PostRecipe%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 2 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PreRecipe%s" , (pt%100) + 1 , MULMDLSTR[style] );
		}
		else if ( id == 1 ) {
			if      ( wh == 0 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_RunRecipeF%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 1 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PostRecipeF%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 2 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PreRecipeF%s" , (pt%100) + 1 , MULMDLSTR[style] );
		}
		else if ( id == 2 ) {
			if      ( wh == 0 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_RunRecipeE%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 1 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PostRecipeE%s" , (pt%100) + 1 , MULMDLSTR[style] );
			else if ( wh == 2 ) Address_DummyProcess_Recipe[wh][pt%100][style][id] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.DummyPrcs_%d_PreRecipeE%s" , (pt%100) + 1 , MULMDLSTR[style] );
		}
	}
	//
	if ( Address_DummyProcess_Recipe[wh][pt%100][style][id] < 0 ) return FALSE;
	//
	_dWRITE_STRING( Address_DummyProcess_Recipe[wh][pt%100][style][id] , data , &COMM );
	//
	return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void WAFER_MSET_CM( int cm , int data ) {
	int i;
	for ( i = 0 ; i < _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; i++ ) {
		_i_SCH_IO_SET_MODULE_STATUS( cm , i+1 , data );
	}
}

void WAFER_MSWAP_CM( int cm1 , int cm2 ) {
	int i , x1 , x2;
	for ( i = 0 ; i < _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; i++ ) {
		//
		x1 = _i_SCH_IO_GET_MODULE_STATUS( cm1 , i+1 );
		x2 = _i_SCH_IO_GET_MODULE_STATUS( cm2 , i+1 );
		//
		_i_SCH_IO_SET_MODULE_STATUS( cm1 , i+1 , x2 );
		_i_SCH_IO_SET_MODULE_STATUS( cm2 , i+1 , x1 );
		//
	}
}

void WAFER_MSET_BM( int bm , int data ) {
	int i , k;
	if ( bm == -1 ) {
		for ( k = 0 ; k < MAX_BM_CHAMBER_DEPTH ; k++ ) {
			for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( k + BM1 ) ; i++ ) {
				_i_SCH_IO_SET_MODULE_STATUS( k + BM1 , i+1 , data );
			}
		}
	}
	else {
		for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( bm ) ; i++ ) {
			_i_SCH_IO_SET_MODULE_STATUS( bm , i+1 , data );
		}
	}
}

void WAFER_MSET_PM( int pm , int data ) {
	int i , k;
	if ( pm == -1 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
				_i_SCH_IO_SET_MODULE_STATUS( k + PM1 , i+1 , data );
			}
		}
	}
	else {
		for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
			_i_SCH_IO_SET_MODULE_STATUS( pm , i+1 , data );
		}
	}
}

void WAFER_MSET_TM( int data ) {
	int i;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		WAFER_STATUS_SET_TM( i , 0 , data );
		WAFER_STATUS_SET_TM( i , 1 , data );
		WAFER_STATUS_SET_TM2( i , 0 , data );
		WAFER_STATUS_SET_TM2( i , 1 , data );
	}
	//
	_i_SCH_IO_SET_MODULE_STATUS( AL , 1 , data );
	_i_SCH_IO_SET_MODULE_STATUS( IC , 1 , data );
}

void WAFER_MSET_FM( int data ) {
	int i;
	WAFER_STATUS_SET_FM( 0 , -1 , data );
	WAFER_STATUS_SET_FM( 0 , -2 , data );
	//
	_i_SCH_IO_SET_MODULE_STATUS( F_AL , 1 , data );
	_i_SCH_IO_SET_MODULE_STATUS( F_IC , 1 , data );
	//
	_i_SCH_IO_SET_MODULE_STATUS( 1001 , 1 , data );
	//
	WAFER_STATUS_SET_FM( 1 , -1 , data );
	//
	for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) {
		_i_SCH_IO_SET_MODULE_STATUS( F_IC , i + 1 , data );
	}
}

void IO_WAFER_DATA_SET_TO_BM( int ch , int slot , int ssrc , int ssts , int sres , int ssre ) { // 2007.07.25
	if ( ssrc >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE( ch , slot , ssrc );
	if ( ssre >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE_E( ch , slot , ssre );
	if ( sres >= 0 ) _i_SCH_IO_SET_MODULE_RESULT( ch , slot , sres );
	if ( ssts >= 0 ) _i_SCH_IO_SET_MODULE_STATUS( ch , slot , ssts );
}

void IO_WAFER_DATA_SET_TO_CHAMBER( int ch , int slot , int ssrc , int sslt , int sres , int ssre ) { // 2007.07.25
	if ( slot <= 0 ) {
		if ( ssrc >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE( ch , 1 , ssrc );
		if ( ssre >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE_E( ch , 1 , ssre );
		if ( sres >= 0 ) _i_SCH_IO_SET_MODULE_RESULT( ch , 1 , sres );
		if ( sslt >= 0 ) _i_SCH_IO_SET_MODULE_STATUS( ch , 1 , sslt );
	}
	else {
		if ( ssrc >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE( ch , 1 , ssrc );
		if ( ssre >= 0 ) _i_SCH_IO_SET_MODULE_SOURCE_E( ch , 1 , ssre );
		if ( sres >= 0 ) _i_SCH_IO_SET_MODULE_RESULT( ch , 1 , sres );
		if ( sslt >= 0 ) _i_SCH_IO_SET_MODULE_STATUS( ch , slot , sslt );
	}
}


void IO_WAFER_DATA_SET_TO_FM( int fms , int finger , int ssrc , int ssts , int sres , int ssre ) { // 2007.08.10
	if ( ssrc >= 0 ) WAFER_SOURCE_SET_FM( fms , finger , ssrc );
	if ( ssre >= 0 ) WAFER_SOURCE_SET_FM_E( fms , finger , ssre );
	if ( sres >= 0 ) WAFER_RESULT_SET_FM( fms , finger , sres );
	if ( ssts >= 0 ) WAFER_STATUS_SET_FM( fms , finger , ssts );
}

void IO_WAFER_DATA_SET_TO_TM( int tms , int finger , int sub , int ssrc , int ssts , int sres , int ssre ) { // 2007.08.10
	if ( sub == 0 ) {
		if ( ssrc >= 0 ) WAFER_SOURCE_SET_TM( tms , finger , ssrc );
		if ( ssre >= 0 ) WAFER_SOURCE_SET_TM_E( tms , finger , ssre );
//		if ( sres >= 0 ) WAFER_RESULT_SET_TM( tms , finger , sres ); // 2015.05.27
		if ( sres >= 0 ) WAFER_RESULT_SET_TM( tms , finger , 0 , sres ); // 2015.05.27
		if ( ssts >= 0 ) WAFER_STATUS_SET_TM( tms , finger , ssts );
	}
	else {
		if ( ssrc >= 0 ) WAFER_SOURCE_SET_TM2( tms , finger , ssrc );
		if ( ssre >= 0 ) WAFER_SOURCE_SET_TM2_E( tms , finger , ssre );
		if ( sres >= 0 ) WAFER_RESULT_SET_TM( tms , finger , 1 , sres ); // 2015.05.27
		if ( ssts >= 0 ) WAFER_STATUS_SET_TM2( tms , finger , ssts );
	}
}



void IO_WAFER_DATA_GET_TO_BM( int ch , int slot , int *ssrc , int *ssts , int *ssre ) { // 2007.08.10
	*ssrc = _i_SCH_IO_GET_MODULE_SOURCE( ch , slot );
	*ssre = _i_SCH_IO_GET_MODULE_SOURCE_E( ch , slot );
	*ssts = _i_SCH_IO_GET_MODULE_STATUS( ch , slot );
}


void IO_WAFER_DATA_GET_TO_CHAMBER( int ch , int slot , int *ssrc , int *ssts , int *ssre ) { // 2007.08.10
	*ssrc = _i_SCH_IO_GET_MODULE_SOURCE( ch , 1 );
	*ssre = _i_SCH_IO_GET_MODULE_SOURCE_E( ch , 1 );
	*ssts = _i_SCH_IO_GET_MODULE_STATUS( ch , slot );
}

void IO_WAFER_DATA_GET_TO_TM( int tms , int finger , int sub , int *ssrc , int *ssts , int *ssre ) { // 2007.08.10
	if ( sub == 0 ) {
		*ssrc = WAFER_SOURCE_IN_TM( tms , finger );
		*ssre = WAFER_SOURCE_IN_TM_E( tms , finger );
		*ssts = WAFER_STATUS_IN_TM( tms , finger );
	}
	else {
		*ssrc = WAFER_SOURCE_IN_TM2( tms , finger );
		*ssre = WAFER_SOURCE_IN_TM2_E( tms , finger );
		*ssts = WAFER_STATUS_IN_TM2( tms , finger );
	}
}

void IO_WAFER_DATA_GET_TO_FM( int fms , int finger , int *ssrc , int *ssts , int *ssre ) { // 2007.08.10
	*ssrc = WAFER_SOURCE_IN_FM( fms , finger );
	*ssre = WAFER_SOURCE_IN_FM_E( fms , finger );
	*ssts = WAFER_STATUS_IN_FM( fms , finger );
}


BOOL _SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( int ch , int bmpos ) {
	if ( !_i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) return TRUE;
	if ( Address_IO_INTLKS_FOR_PICK[ch] == -2 ) {
		if        ( ch == FM ) {
			Address_IO_INTLKS_FOR_PICK[FM]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_FM" );
		}
		else if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_CM%d" , ch-CM1+1 );
		}
		else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_PM%d" , ch-PM1+1 );
		}
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d" , ch-BM1+1 );
		}
		else if   ( ch >= TM ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_TM%d" , ch-TM+1 );
		}
	}
//	if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_PICK[ch] ) == 0 ) return TRUE; // 2010.07.09
	if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_PICK[ch] ) != 0 ) return FALSE; // 2010.07.09
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) { // 2010.07.09
		if ( bmpos <= -1 ) {
			if ( Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_FM" , ch-BM1+1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch] ) != 0 ) return FALSE;
		}
		else if ( bmpos == 0 ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_TM" , ch-BM1+1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch] ) != 0 ) return FALSE;
		}
		else if ( bmpos < MAX_TM_CHAMBER_DEPTH ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_TM%d" , ch-BM1+1 , bmpos + 1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch] ) != 0 ) return FALSE;
		}
	}
	//
//	return FALSE; // 2010.07.09
	return TRUE;
}

BOOL _SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( int ch , int bmpos ) {
	if ( !_i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) return TRUE;
	if ( Address_IO_INTLKS_FOR_PLACE[ch] == -2 ) {
		if        ( ch == FM ) {
			Address_IO_INTLKS_FOR_PLACE[FM]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_FM" );
		}
		else if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_CM%d" , ch-CM1+1 );
		}
		else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_PM%d" , ch-PM1+1 );
		}
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d" , ch-BM1+1 );
		}
		else if   ( ch >= TM ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_TM%d" , ch-TM+1 );
		}
	}
//	if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_PLACE[ch] ) == 0 ) return TRUE; // 2010.07.09
	if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_PLACE[ch] ) != 0 ) return FALSE; // 2010.07.09
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) { // 2010.07.09
		if ( bmpos <= -1 ) {
			if ( Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_FM" , ch-BM1+1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch] ) != 0 ) return FALSE;
		}
		else if ( bmpos == 0 ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_TM" , ch-BM1+1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch] ) != 0 ) return FALSE;
		}
		else if ( bmpos < MAX_TM_CHAMBER_DEPTH ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_TM%d" , ch-BM1+1 , bmpos + 1 );
			}
			if ( IO_ADD_READ_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch] ) != 0 ) return FALSE;
		}
	}
	//
//	return FALSE; // 2010.07.09
	return TRUE;
}

BOOL _SCH_IO_SET_INLTKS_FOR_PICK_POSSIBLE( int ch , int bmpos , int data ) { // 2012.11.27
	if ( !_i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) return FALSE;
	if ( Address_IO_INTLKS_FOR_PICK[ch] == -2 ) {
		if        ( ch == FM ) {
			Address_IO_INTLKS_FOR_PICK[FM]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_FM" );
		}
		else if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_CM%d" , ch-CM1+1 );
		}
		else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_PM%d" , ch-PM1+1 );
		}
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d" , ch-BM1+1 );
		}
		else if   ( ch >= TM ) {
			Address_IO_INTLKS_FOR_PICK[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_TM%d" , ch-TM+1 );
		}
	}
	//
	IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_PICK[ch] , !data );
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		if ( bmpos <= -1 ) {
			if ( Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_FM" , ch-BM1+1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_FM_PICK[0][ch] , !data );
		}
		else if ( bmpos == 0 ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_TM" , ch-BM1+1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PICK[0][ch] , !data );
		}
		else if ( bmpos < MAX_TM_CHAMBER_DEPTH ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PICK_BM%d_TM%d" , ch-BM1+1 , bmpos + 1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PICK[bmpos][ch] , !data );
		}
	}
	//
	return TRUE;
}

BOOL _SCH_IO_SET_INLTKS_FOR_PLACE_POSSIBLE( int ch , int bmpos , int data ) { // 2012.11.27
	if ( !_i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) return FALSE;
	if ( Address_IO_INTLKS_FOR_PLACE[ch] == -2 ) {
		if        ( ch == FM ) {
			Address_IO_INTLKS_FOR_PLACE[FM]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_FM" );
		}
		else if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_CM%d" , ch-CM1+1 );
		}
		else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_PM%d" , ch-PM1+1 );
		}
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d" , ch-BM1+1 );
		}
		else if   ( ch >= TM ) {
			Address_IO_INTLKS_FOR_PLACE[ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_TM%d" , ch-TM+1 );
		}
	}
	IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_PLACE[ch] , !data );
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		if ( bmpos <= -1 ) {
			if ( Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_FM" , ch-BM1+1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_FM_PLACE[0][ch] , !data );
		}
		else if ( bmpos == 0 ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_TM" , ch-BM1+1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PLACE[0][ch] , !data );
		}
		else if ( bmpos < MAX_TM_CHAMBER_DEPTH ) {
			if ( Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch] == -2 ) {
				Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.IOINTLKS_PLACE_BM%d_TM%d" , ch-BM1+1 , bmpos + 1 );
			}
			IO_ADD_WRITE_DIGITAL( Address_IO_INTLKS_FOR_BM_TM_PLACE[bmpos][ch] , !data );
		}
	}
	//
	return TRUE;
}


int SET_FLAG_DATA[MAX_CHAMBER]; // 2016.12.13

void _SCH_IO_SET_MODULE_FLAG_DATA( int Chamber , int data ) { // 2013.03.19
	//
	SET_FLAG_DATA[Chamber] = data; // 2016.12.13
	//
	if ( Address_MODULE_Flag_Status[Chamber] == -1 ) return;
	//
	IO_ADD_WRITE_DIGITAL_NAME( &Address_MODULE_Flag_Status[Chamber] , SET_FLAG_DATA[Chamber] , "CTC.%s_Module_Flag" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(Chamber) );
}

void _SCH_IO_SET_MODULE_FLAG_DATA3( int Chamber , int data , int data2 , int data3 ) { // 2013.03.19
	//
	if      ( SET_FLAG_DATA[Chamber] == data ) {
		SET_FLAG_DATA[Chamber] = data2;
	}
	else if ( SET_FLAG_DATA[Chamber] == data2 ) {
		SET_FLAG_DATA[Chamber] = data3;
	}
	else {
		SET_FLAG_DATA[Chamber] = data;
	}
	//
	if ( Address_MODULE_Flag_Status[Chamber] == -1 ) return;
	//
	IO_ADD_WRITE_DIGITAL_NAME( &Address_MODULE_Flag_Status[Chamber] , SET_FLAG_DATA[Chamber] , "CTC.%s_Module_Flag" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(Chamber) );
}


BOOL _IO_SET_CHAMBER_INLTKS( int mode , int tmatm , int ch , int data ) { // 2013.11.21
	int tms;
	char BufferP[8];
	char BufferT[8];
	//
	if      ( tmatm == -1 ) {
		tms = MAX_TM_CHAMBER_DEPTH;
		//
		strcpy( BufferT , "FM" );
	}
	else if ( ( tmatm >= 0 ) && ( tmatm < MAX_TM_CHAMBER_DEPTH ) ) {
		tms = tmatm;
		//
		if ( tmatm == 0 ) {
			strcpy( BufferT , "TM" );
		}
		else {
			sprintf( BufferT , "TM%d" , tmatm + 1 );
		}
	}
	else {
		return FALSE;
	}
	//
	if      ( mode == MACRO_PICK ) {
		strcpy( BufferP , "PICK" );
	}
	else if ( mode == MACRO_PLACE ) {
		strcpy( BufferP , "PLACE" );
	}
	else {
		return FALSE;
	}
	//
	if      ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
		return IO_ADD_WRITE_DIGITAL_NAME( &(Address_CHAMBER_INTLKS_FOR_PICKPLACE[mode][tms][ch]) , data , "CTC.FA_CHINTLKS_%s_%s_CM%d" , BufferT , BufferP , ch-CM1+1 );
	}
	else if ( ( ch >= PM1 ) && ( ch < BM1 ) ) {
		return IO_ADD_WRITE_DIGITAL_NAME( &(Address_CHAMBER_INTLKS_FOR_PICKPLACE[mode][tms][ch]) , data , "CTC.FA_CHINTLKS_%s_%s_PM%d" , BufferT , BufferP , ch-PM1+1 );
	}
	else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		return IO_ADD_WRITE_DIGITAL_NAME( &(Address_CHAMBER_INTLKS_FOR_PICKPLACE[mode][tms][ch]) , data , "CTC.FA_CHINTLKS_%s_%s_BM%d" , BufferT , BufferP , ch-BM1+1 );
	}
	else if   ( ( ch == AL ) || ( ch == F_AL ) ) {
		return IO_ADD_WRITE_DIGITAL_NAME( &(Address_CHAMBER_INTLKS_FOR_PICKPLACE[mode][tms][ch]) , data , "CTC.FA_CHINTLKS_%s_%s_AL" , BufferT , BufferP );
	}
	else if   ( ( ch == IC ) || ( ch == F_IC ) ) {
		return IO_ADD_WRITE_DIGITAL_NAME( &(Address_CHAMBER_INTLKS_FOR_PICKPLACE[mode][tms][ch]) , data , "CTC.FA_CHINTLKS_%s_%s_IC" , BufferT , BufferP );
	}
	else {
		return FALSE;
	}
	//
}



//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================
//=============================================================================================================================================================


/*
void _IO_GET_CURRENT_DATA_STRING( char *data ) {






for ( i = CM1 ; i < ( MAX_CASSETTE_SIDE + CM1 ) ; i++ ) {
	for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
xx source
xx result

	}
}

for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
	for ( j = 1 ; j <= MAX_PM_SLOT_DEPTH ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
}

for ( i = BM1 ; i < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ; i++ ) {
	for ( j = 1 ; j <= MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
}

	_i_SCH_IO_GET_MODULE_STATUS( AL , -1 );
	_i_SCH_IO_GET_MODULE_STATUS( IC , -1 );

	_i_SCH_IO_GET_MODULE_STATUS( F_AL , -1 );
	_i_SCH_IO_GET_MODULE_STATUS( 1001 , -1 ); // F_AL2

	for ( j = 1 ; j <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( F_IC , j );
	}






// FM

for ( i = 10000 ; i < ( 10000 + MAX_FM_CHAMBER_DEPTH ) ; i++ ) {
	for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
	for ( j = 2 ; j < ( MAX_FM_A_EXTEND_FINGER + 2 ) ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
	for ( j = 1001 ; j < ( 1001 + MAX_CASSETTE_SLOT_SIZE ) ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
	for ( j = 2001 ; j < ( 2001 + MAX_CASSETTE_SLOT_SIZE ) ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
}

// TM

for ( i = 20000 ; i < ( 20000 + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
	for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );
	}
}

for ( i = 30000 ; i < ( 30000 + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
	for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
		_i_SCH_IO_GET_MODULE_STATUS( i , j );

xx Result

	}
}

*/