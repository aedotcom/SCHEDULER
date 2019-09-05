//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_Robot_Handling.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_param.h"
#include "sch_A0_sub.h"
#include "sch_A0_dll.h"
#include "sch_A0_sub_sp2.h"
//=======================================================================================
int	Address_TIM_LOG_POINTER_SP2 = -2;
//int	Address_LOT_LOG_POINTER_SP2[MAX_CASSETTE_SIDE] = { -2 , -2 , -2 , -2 }; // 2011.05.13
int	Address_LOT_LOG_POINTER_SP2[MAX_CASSETTE_SIDE]; // 2011.05.13
//=======================================================================================
int  SUB2_INDEX_ROTATE_COUNT = 6;

int	 SUB2_CASSETTE_APPEND_REG_INDEX;

int	 SUB2_CASSETTE_APPEND_OUT_INDEX;

int	 SUB2_CASSETTE_APPEND_CAS_INDEX; // 2007.07.23

int	 SUB2_CASSETTE_APPEND_CUR_INDEX; // 2006.04.04

int	 SUB2_CASSETTE_APPEND_FLAG[ MAX_CASSETTE_SLOT_SIZE ];

int	 SUB2_CASSETTE_APPEND_AUTOALIGN[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.09.04

int	 SUB2_CASSETTE_APPEND_IN_CM[ MAX_CASSETTE_SLOT_SIZE ];
int	 SUB2_CASSETTE_APPEND_IN_SLOT[ MAX_CASSETTE_SLOT_SIZE ];
//
int	 SUB2_CASSETTE_APPEND_OUT_CM[ MAX_CASSETTE_SLOT_SIZE ];
int	 SUB2_CASSETTE_APPEND_OUT_SLOT[ MAX_CASSETTE_SLOT_SIZE ];
//
int	 SUB2_CASSETTE_APPEND_ID[ MAX_CASSETTE_SLOT_SIZE ];
//
int	 SUB2_CASSETTE_APPEND_OPTIMIZE[ MAX_CASSETTE_SLOT_SIZE ];
//
//char *SUB2_CASSETTE_APPEND_RECIPE_PM[ MAX_CASSETTE_SLOT_SIZE ][ MAX_PM_CHAMBER_DEPTH ];// 2008.04.02 // 2011.11.29
char *SUB2_CASSETTE_APPEND_RECIPE_PM[ MAX_CASSETTE_SLOT_SIZE ][ MAX_PM_CHAMBER_DEPTH ][3];// 2008.04.02 // 2011.11.29
char SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ MAX_CASSETTE_SLOT_SIZE ][ MAX_PM_CHAMBER_DEPTH ]; // 2007.08.30
//
char *SUB2_CASSETTE_FOLDER_DATA[ MAX_CASSETTE_SLOT_SIZE ]; // 2011.09.07

int  SUB2_CASSETTE_FOLDER_MODE[ MAX_CASSETTE_SLOT_SIZE ]; // 2011.09.07
char *SUB2_CASSETTE_FOLDER[ MAX_CASSETTE_SLOT_SIZE ]; // 2008.04.02

int  SUB2_ARMINTLKS_BUFFER_SIDE = -1; // 2007.09.06
int  SUB2_ARMINTLKS_BUFFER_PT; // 2007.09.06
//=======================================================================================
//=======================================================================================
BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_RECOVER_AFTER_PLACE_A0SUB2( int tms ) {
	int i;
//printf( "============================================================\n" );
//printf( "[REC] side=%d,pt=%d Bside=%d,Bpt=%d\n" , side , pt , SUB2_ARMINTLKS_BUFFER_SIDE , SUB2_ARMINTLKS_BUFFER_PT );
//printf( "============================================================\n" );
	if ( SUB2_ARMINTLKS_BUFFER_SIDE == -1 ) return FALSE;
	if ( tms != -1 ) {
		_SCH_CLUSTER_Set_SupplyID( SUB2_ARMINTLKS_BUFFER_SIDE , SUB2_ARMINTLKS_BUFFER_PT , -9998 );
		_SCH_CLUSTER_Set_PathDo( SUB2_ARMINTLKS_BUFFER_SIDE , SUB2_ARMINTLKS_BUFFER_PT , 2 );
		for ( i = 0 ; i < 2 ; i++ ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) > 0 ) {
				if ( _SCH_MODULE_Get_TM_SIDE( tms , i ) == SUB2_ARMINTLKS_BUFFER_SIDE ) {
					if ( _SCH_MODULE_Get_TM_POINTER( tms , i ) == SUB2_ARMINTLKS_BUFFER_PT ) {
						_SCH_MODULE_Set_TM_TYPE( tms , i , SCHEDULER_MOVING_OUT );
					}
				}
			}
		}
	}
	SUB2_ARMINTLKS_BUFFER_SIDE = -1;
	SUB2_ARMINTLKS_BUFFER_PT   = 0;
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_PICK_FROM_CM_A0SUB2( int armdata , int side , int pt ) {
//printf( "============================================================\n" );
//printf( "[ADD] side=%d,pt=%d Bside=%d,Bpt=%d\n" , side , pt , SUB2_ARMINTLKS_BUFFER_SIDE , SUB2_ARMINTLKS_BUFFER_PT );
//printf( "============================================================\n" );
	if ( SUB2_ARMINTLKS_BUFFER_SIDE != -1 ) return FALSE;
	SUB2_ARMINTLKS_BUFFER_SIDE = side;
	SUB2_ARMINTLKS_BUFFER_PT   = pt;
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_DATA_A0SUB2( int side , int pt ) {
	if ( SUB2_ARMINTLKS_BUFFER_SIDE != side ) return FALSE;
	if ( SUB2_ARMINTLKS_BUFFER_PT != pt ) return FALSE;
	return TRUE;
}
//=======================================================================================
void SCHEDULER_CONTROL_Reset_CASSETTE_DATA_A0SUB2( int side , int pt , BOOL ioalso ) { // 2005.10.07
	int i , s;
	//
	_SCH_MODULE_Set_TM_DoPointer( side , 0 ); // 2007.09.06
	//
	if ( pt == -1 ) {
		//
		if ( _SCH_SYSTEM_RESTART_GET( side ) != 0 ) return; // 2011.10.19
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			_SCH_CLUSTER_Set_PathDo( side , i , 0 );
			_SCH_CLUSTER_Set_PathStatus( side , i , SCHEDULER_READY );
			//
			_SCH_CLUSTER_Set_PathHSide( side , i , 0 );
			_SCH_CLUSTER_Set_Buffer( side , i , 0 );
			_SCH_CLUSTER_Set_PathRange( side , i , -1 );
			_SCH_CLUSTER_Set_Optimize( side , i , -1 ); // 2006.04.04
		}
		if ( ioalso ) {
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) { // 2008.04.02
				if ( _SCH_MODULE_GET_USE_ENABLE( CM1 + s , FALSE , -1 ) ) { // 2008.04.02
					for ( i = 0 ; i < _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; i++ ) {
//						_SCH_IO_SET_MODULE_STATUS( CM1     , i + 1 , CWM_ABSENT ); // 2008.04.02
//						_SCH_IO_SET_MODULE_STATUS( CM1 + 1 , i + 1 , CWM_ABSENT ); // 2008.04.02
						_SCH_IO_SET_MODULE_STATUS( CM1 + s , i + 1 , CWM_ABSENT ); // 2008.04.02
					}
				}
			}
		}
	}
	else {
		_SCH_CLUSTER_Set_PathDo( side , pt , 0 );
		_SCH_CLUSTER_Set_PathStatus( side , pt , SCHEDULER_READY );
		//
		_SCH_CLUSTER_Set_PathHSide( side , pt , 0 );
		_SCH_CLUSTER_Set_Buffer( side , pt , 0 );
		_SCH_CLUSTER_Set_PathRange( side , pt , -1 );
		_SCH_CLUSTER_Set_Optimize( side , pt , -1 ); // 2006.04.04
	}
}
//=======================================================================================
void SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB2( int apmode , int side ) {
	int i , j;
	//
	if ( apmode == 0 ) {
		Address_TIM_LOG_POINTER_SP2 = -2;
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			Address_LOT_LOG_POINTER_SP2[i] = -2;
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2008.04.02
			SUB2_CASSETTE_FOLDER[ i ] = NULL;
			SUB2_CASSETTE_FOLDER_DATA[ i ] = NULL; // 2011.09.07
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) { // 2008.04.02
//				SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ] = NULL;// 2008.04.02 // 2011.11.28
				SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][0] = NULL;// 2008.04.02 // 2011.11.28
				SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][1] = NULL;// 2008.04.02 // 2011.11.28
				SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][2] = NULL;// 2008.04.02 // 2011.11.28
			}
		}
		//
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) || ( apmode == 4 ) ) {
		//========================================================
		// 2007.09.06
		//========================================================
		SCHEDULER_CONTROL_Chk_ARMINLTKS_RECOVER_AFTER_PLACE_A0SUB2( -1 );
		//========================================================
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			SUB2_CASSETTE_APPEND_FLAG[ i ] = FALSE;
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
//				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][0]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][1]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][2]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
				//
				SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ i ][ j ] = FALSE; // 2007.08.31
			}
			STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_FOLDER[ i ]) , "" ); // 2008.04.02 // 2010.03.25
		}
		SUB2_CASSETTE_APPEND_REG_INDEX = -1; // 2005.11.25
		SUB2_CASSETTE_APPEND_OUT_INDEX = -1; // 2005.11.25
		SUB2_CASSETTE_APPEND_CUR_INDEX =  0; // 2006.04.04
		SUB2_CASSETTE_APPEND_CAS_INDEX =  0; // 2007.07.23
	}
}
//=======================================================================================
int SCHEDULER_CONTROL_Chk_AUTOALIGN_A0SUB2( int pos , int addr , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe , char *pm5recipe , char *pm6recipe ) {
	int selpm , i , j , k;
	int pc[MAX_PM_CHAMBER_DEPTH];
	int pd[MAX_PM_CHAMBER_DEPTH];
	//=======================================================================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		pc[i] = FALSE;
		pd[i] = 0;
	}
	//=======================================================================================================================
	selpm = 0;
	//=======================================================================================================================
	i = 0;
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 0 , FALSE , -1 ) ) {
		if ( strlen( pm1recipe ) > 0 ) {
			i++;
			selpm = 1;
			pc[0] = TRUE;
		}
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 1 , FALSE , -1 ) ) {
		if ( strlen( pm2recipe ) > 0 ) {
			i++;
			selpm = 2;
			pc[1] = TRUE;
		}
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 2 , FALSE , -1 ) ) {
		if ( strlen( pm3recipe ) > 0 ) {
			i++;
			selpm = 3;
			pc[2] = TRUE;
		}
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 3 , FALSE , -1 ) ) {
		if ( strlen( pm4recipe ) > 0 ) {
			i++;
			selpm = 4;
			pc[3] = TRUE;
		}
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 4 , FALSE , -1 ) ) {
		if ( strlen( pm5recipe ) > 0 ) {
			i++;
			selpm = 5;
			pc[4] = TRUE;
		}
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 5 , FALSE , -1 ) ) {
		if ( strlen( pm6recipe ) > 0 ) {
			i++;
			selpm = 6;
			pc[5] = TRUE;
		}
	}
	//=======================================================================================================================
	if ( i <= 1 ) return selpm;
	//=======================================================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( SUB2_CASSETTE_APPEND_FLAG[ i ] ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
//				if ( STR_MEM_SIZE( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ] ) > 0 ) { // 2008.04.02 // 2011.11.29
				if ( STR_MEM_SIZE( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][0] ) > 0 ) { // 2008.04.02 // 2011.11.29
					if ( !SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ i ][ k ] ) {
						pd[k]++;
					}
				}
			}
		}
	}
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_GET( i ) > 0 ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				if ( _SCH_CLUSTER_Get_PathRange( i , j ) >= 0 ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						selpm = _SCH_CLUSTER_Get_PathProcessChamber( i , j , 0 , k );
						if ( selpm > 0 ) {
							pd[selpm-PM1]++;
						}
					}
				}
			}
		}
	}
	//=======================================================================================================================
	selpm = 0;
	//=======================================================================================================================
	for ( i = 0 , j = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( !pc[i] ) continue;
		if ( ( selpm == 0 ) || ( pd[i] < j ) ) {
			selpm = i + 1;
			j = pd[i];
		}
	}
	//=======================================================================================================================
	USER_RECIPE_SP_N0S2_AUTOALIGNING( pos , addr , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe  , pm5recipe  , pm6recipe , &selpm );
	return selpm;
}
//=======================================================================================
int SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2( int pos , int mode , int opt , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe  , char *pm5recipe  , char *pm6recipe , char *sublogfolder , int normalside , int *selpm ) {
	int i , j , side = -1 , l , ss , es , res;
	//
	char rcp0[256];
	char rcp1[256];
	char rcp2[256];
	//
	// mode = 0 :
	// mode = 1 : End Check
	// mode = 2 : End Check + Auto Align
	//
	*selpm = 0; // 2007.08.31
	//
	if ( mode != 0 ) {
		if ( SIGNAL_MODE_APPEND_END_GET( incm ) == 2 ) return 1;
	}
	//
	if ( normalside == -1 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _SCH_SYSTEM_USING_GET( i ) > 0 ) {
				side = i;
				break;
			}
		}
		if ( side == -1 ) {
			_IO_CIM_PRINTF( "========================================================\n" );
			_IO_CIM_PRINTF( "APP REGIST - Regist Data Error 1 with (%d,%d) (%d,%d) %d\n" , incm , inslot , outcm , outslot , spid );
			_IO_CIM_PRINTF( "========================================================\n" );
			return 2;
		}
	}
	else {
		side = normalside;
	}
	//
	if ( ( incm <= 0 ) || ( incm >= PM1 ) ) {
		if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Error 2 with (%d,%d) (%d,%d) %d%cAPP_REGIST_ERROR 2\n" , incm , inslot , outcm , outslot , spid , 9 );
		return 3;
	}
	if ( ( outcm < 0 ) || ( outcm >= PM1 ) ) {
		if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Error 3 with (%d,%d) (%d,%d) %d%cAPP_REGIST_ERROR 3\n" , incm , inslot , outcm , outslot , spid , 9 );
		return 4;
	}
	//
	if ( ( inslot < -1 ) || ( inslot > _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) {
		if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Error 4 with (%d,%d) (%d,%d) %d%cAPP_REGIST_ERROR 4\n" , incm , inslot , outcm , outslot , spid , 9 );
		return 5;
	}
	if ( ( outslot < -1 ) || ( outslot > _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) {
		if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Error 5 with (%d,%d) (%d,%d) %d%cAPP_REGIST_ERROR 5\n" , incm , inslot , outcm , outslot , spid , 9 );
		return 6;
	}
	//=======================================================================================================================
//	if ( !_SCH_MODULE_GET_USE_ENABLE( incm , TRUE , -1 ) ) return 7;
//	if ( outcm > 0 ) {
//		if ( !_SCH_MODULE_GET_USE_ENABLE( outcm , TRUE , -1 ) ) return 8;
//	}
	//=================================================================================================
	i = 0;
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 0 , FALSE , -1 ) ) {
		if ( strlen( pm1recipe ) > 0 ) i++;
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 1 , FALSE , -1 ) ) {
		if ( strlen( pm2recipe ) > 0 ) i++;
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 2 , FALSE , -1 ) ) {
		if ( strlen( pm3recipe ) > 0 ) i++;
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 3 , FALSE , -1 ) ) {
		if ( strlen( pm4recipe ) > 0 ) i++;
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 4 , FALSE , -1 ) ) {
		if ( strlen( pm5recipe ) > 0 ) i++;
	}
	if ( _SCH_MODULE_GET_USE_ENABLE( PM1 + 5 , FALSE , -1 ) ) {
		if ( strlen( pm6recipe ) > 0 ) i++;
	}
	if ( i <= 0 ) return 9;
	//=======================================================================================================================
	for ( l = 0 ; l < 2 ; l++ ) {
		if ( l == 0 ) {
			ss = ( SUB2_CASSETTE_APPEND_REG_INDEX + 1 );
			es = MAX_CASSETTE_SLOT_SIZE;
		}
		else {
			ss = 0;
			es = ( SUB2_CASSETTE_APPEND_REG_INDEX + 1 );
		}
		for ( i = ss ; i < es ; i++ ) {
			if ( !SUB2_CASSETTE_APPEND_FLAG[ i ] ) {
				//
				SUB2_CASSETTE_APPEND_IN_CM[ i ] = incm;
				SUB2_CASSETTE_APPEND_IN_SLOT[ i ] = inslot;
				//
				SUB2_CASSETTE_APPEND_OUT_CM[ i ] = outcm;
				SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] = outslot;
				//
				SUB2_CASSETTE_APPEND_ID[ i ] = spid;
				//
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					//
//					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][0]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][1]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][2]) , "" ); // 2008.04.02 // 2010.03.25 // 2011.11.29

					SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ i ][ j ] = FALSE; // 2007.08.31
				}
				//=======================================================================================================================
				// 2007.08.31
				//=======================================================================================================================
				SUB2_CASSETTE_APPEND_AUTOALIGN[ i ] = 0;
				SUB2_CASSETTE_APPEND_OPTIMIZE[ i ] = opt; // 2007.09.28
				//=======================================================================================================================
				if ( mode == 2 ) {
					res = SCHEDULER_CONTROL_Chk_AUTOALIGN_A0SUB2( pos , i , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe );
					if ( res != 0 ) {
						SUB2_CASSETTE_APPEND_AUTOALIGN[ i ] = res;
						*selpm = res;
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( res != ( j + 1 ) ) {
								SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ i ][ j ] = TRUE;
							}
						}
					}
				}
				//=======================================================================================================================
				/*
				//
				2011.11.29
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 0 ]) , pm1recipe ); // 2008.04.02 // 2010.03.25
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 1 ]) , pm2recipe ); // 2008.04.02 // 2010.03.25
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 2 ]) , pm3recipe ); // 2008.04.02 // 2010.03.25
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 3 ]) , pm4recipe ); // 2008.04.02 // 2010.03.25
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 4 ]) , pm5recipe ); // 2008.04.02 // 2010.03.25
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ 5 ]) , pm6recipe ); // 2008.04.02 // 2010.03.25
				*/
				// 2011.11.29
				for ( j = 0 ; j < 6 ; j++ ) {
					if      ( j == 0 ) STR_SEPERATE_CHAR3( pm1recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					else if ( j == 1 ) STR_SEPERATE_CHAR3( pm2recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					else if ( j == 2 ) STR_SEPERATE_CHAR3( pm3recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					else if ( j == 3 ) STR_SEPERATE_CHAR3( pm4recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					else if ( j == 4 ) STR_SEPERATE_CHAR3( pm5recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					else if ( j == 5 ) STR_SEPERATE_CHAR3( pm6recipe , '>' , rcp0 , rcp1 , rcp2 , 255 );
					//
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][0]) , rcp0 );
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][1]) , rcp1 );
					STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ j ][2]) , rcp2 );
				}
				//=======================================================================================================================
				STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_FOLDER_DATA[ i ]) , sublogfolder ); // 2011.09.07
				//=======================================================================================================================
				SUB2_CASSETTE_APPEND_FLAG[ i ] = TRUE;
				//
				SUB2_CASSETTE_APPEND_REG_INDEX = i; // 2005.11.25
				//
				if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Success 1 index=%d with (%d,%d) (%d,%d) %d%cAPP_REGIST %d:%d:%d:%d:%d:%d\n" , i , incm , inslot , outcm , outslot , spid , 9 , i , incm , inslot , outcm , outslot , spid );
				//
				return 0;
			}
		}
	}
	if ( normalside == -1 ) _SCH_LOG_LOT_PRINTF( side , "APP REGIST - Regist Data Error 6 with (%d,%d) (%d,%d) %d%cAPP_REGIST_ERROR 6\n" , incm , inslot , outcm , outslot , spid , 9 );
	return 10;
}
//=======================================================================================
int SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2_PLUSDLL( int tord , int pos , int mode , int opt , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe  , char *pm5recipe  , char *pm6recipe , char *sublogfolder ) {
	int res , selpm;
	//====================================================================================================================
	if ( tord >= 0 ) SUB2_INDEX_ROTATE_COUNT = tord + 6;// 2008.01.17
	//====================================================================================================================
	res = SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2( pos , mode , opt , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe  , pm5recipe  , pm6recipe , sublogfolder , -1 , &selpm );
	//====================================================================================================================
	USER_RECIPE_SP_N0S2_VERIFICATION( pos , res , ( mode == 2 ) ? TRUE : FALSE , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe , selpm );
	//====================================================================================================================
	return res;
}
//=======================================================================================
void SCHEDULER_CONTROL_Chk_Local_Time_for_File_A0SUB2( char *buffer , int Sep ) {
	SYSTEMTIME		SysTime;
	GetLocalTime( &SysTime );
	if      ( Sep == 1 )
		sprintf( buffer , "%04d%02d%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	else if ( Sep == 2 )
		sprintf( buffer , "%04d/%02d/%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	else
		sprintf( buffer , "%04d%02d%02d_%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
}
//=======================================================================================
void SCHEDULER_CONTROL_Chk_LOT_TIMER_MAKE_SLOT_FOLDER( int M , int slot , char *data ) { // 2006.04.26
	char buffer[256];
	//--------------------------------------------------------
	if ( M < 0 || M >= MAX_CASSETTE_SIDE ) return;
	//--------------------------------------------------------
	if ( Address_LOT_LOG_POINTER_SP2[M] == -2 ) {
		sprintf( buffer , "SCHEDULER_LOTLOG%d" , M + 1 );
		Address_LOT_LOG_POINTER_SP2[M] = _FIND_FROM_STRING( _K_F_IO , buffer );
	}
	if ( Address_LOT_LOG_POINTER_SP2[M] < 0 ) return;
	sprintf( buffer , "$SLOT_FOLDER %d|%s" , slot , data );
	_dWRITE_FUNCTION_EVENT( Address_LOT_LOG_POINTER_SP2[M] , buffer );
	//--------------------------------------------------------
	if ( Address_TIM_LOG_POINTER_SP2 == -2 ) {
		Address_TIM_LOG_POINTER_SP2 = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_TIMER" );
	}
	if ( Address_TIM_LOG_POINTER_SP2 < 0 ) return;
	sprintf( buffer , "$SLOT_FOLDER%d %d %s" , M + 1 , slot , data );
	_dWRITE_FUNCTION_EVENT( Address_TIM_LOG_POINTER_SP2 , buffer );
}
//=======================================================================================
int SCHEDULER_CONTROL_Chk_CASSETTE_MAKE_SLOT_FOLDER_A0SUB2( int side , int index , int slot , BOOL make ) { // 2006.04.26
	char Buffer[256];
	if ( make ) {
		//
		if ( ( SUB2_CASSETTE_FOLDER_DATA[ index ] != NULL ) && ( SUB2_CASSETTE_FOLDER_DATA[ index ][0] != 0 ) ) { // 2011.09.07
			//
			strncpy( Buffer , SUB2_CASSETTE_FOLDER_DATA[ index ] , 255 );
			//
			SUB2_CASSETTE_FOLDER_MODE[ slot - 1 ] = 1;
			//
		}
		else {
			//
			SCHEDULER_CONTROL_Chk_Local_Time_for_File_A0SUB2( Buffer , _SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() / 16 );
			//
			SUB2_CASSETTE_FOLDER_MODE[ slot - 1 ] = 0;
		}
		//
		SCHEDULER_CONTROL_Chk_LOT_TIMER_MAKE_SLOT_FOLDER( side , slot , Buffer );
		//
		STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_FOLDER[ slot - 1 ]) , Buffer ); // 2008.04.02 // 2010.03.25
	}
	else {
		//
		SCHEDULER_CONTROL_Chk_LOT_TIMER_MAKE_SLOT_FOLDER( side , slot , "" );
		//
		SUB2_CASSETTE_FOLDER_MODE[ slot - 1 ] = 0; // 2011.09.07
		//
		STR_MEM_MAKE_COPY2( &(SUB2_CASSETTE_FOLDER[ slot - 1 ]) , "" ); // 2008.04.02 // 2010.03.25
	}
	return 0;
}
//=======================================================================================
int SCHEDULER_CONTROL_Chk_CASSETTE_APPEND_A0SUB2( int side ) {
	int i , j , k , l , sl , el , next , u[MAX_CASSETTE_SLOT_SIZE] , ns , js;

	//========================================================================
	// 2006.04.04
	//========================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) break;
	}
	if ( i == MAX_CASSETTE_SLOT_SIZE ) SUB2_CASSETTE_APPEND_CUR_INDEX = 0;

	//========================================================================
	for ( l = 0 ; l < 2 ; l++ ) {
		if ( l == 0 ) {
			sl = ( SUB2_CASSETTE_APPEND_OUT_INDEX + 1 );
			el = MAX_CASSETTE_SLOT_SIZE;
		}
		else {
			sl = 0;
			el = ( SUB2_CASSETTE_APPEND_OUT_INDEX + 1 );
		}
		for ( i = sl ; i < el ; i++ ) {
			//================================================================
			if ( !SUB2_CASSETTE_APPEND_FLAG[ i ] ) continue;
			if ( !_SCH_MODULE_GET_USE_ENABLE( SUB2_CASSETTE_APPEND_IN_CM[ i ] , TRUE , side ) ) continue;
			//================================================================
			next = FALSE;
			if ( SUB2_CASSETTE_APPEND_IN_SLOT[ i ] > 0 ) {
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					if ( _SCH_CLUSTER_Get_PathRange( side , j ) >= 0 ) {
						if ( _SCH_CLUSTER_Get_PathIn( side , j ) == SUB2_CASSETTE_APPEND_IN_CM[ i ] ) {
							if ( _SCH_CLUSTER_Get_SlotIn( side , j ) == SUB2_CASSETTE_APPEND_IN_SLOT[ i ] ) {
								next = TRUE;
								break;
							}
						}
					}
				}
			}
			//================================================================
	//		if ( next ) continue; // 2005.11.25
			if ( next ) {
				_SCH_LOG_LOT_PRINTF( side , "APP SELECT - Select Data Error 1 index=%d Pointer=%d with (%d,%d) (%d,%d) %d%cAPP_SELECT_ERROR 1\n" , i , l + 1 , SUB2_CASSETTE_APPEND_IN_CM[ i ] , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] , SUB2_CASSETTE_APPEND_OUT_CM[ i ] , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] , SUB2_CASSETTE_APPEND_ID[ i ] , 9 );
				return 1; // 2005.11.25
			}
			//================================================================
			js = -1;
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) u[j] = FALSE;
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				if ( _SCH_CLUSTER_Get_PathRange( side , j ) >= 0 ) {
	//					if ( _SCH_CLUSTER_Get_PathIn( side , j ) == SUB2_CASSETTE_APPEND_IN_CM[ i ] ) {
						u[ _SCH_CLUSTER_Get_SlotIn( side , j ) - 1 ] = TRUE;
	//					}
				}
				else {
					if ( js == -1 ) js = j;
				}
			}
			if ( js == -1 ) {
				_SCH_LOG_LOT_PRINTF( side , "APP SELECT - Select Data Error 2 index=%d Pointer=%d with (%d,%d) (%d,%d) %d%cAPP_SELECT_ERROR 2\n" , i , l + 1 , SUB2_CASSETTE_APPEND_IN_CM[ i ] , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] , SUB2_CASSETTE_APPEND_OUT_CM[ i ] , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] , SUB2_CASSETTE_APPEND_ID[ i ] , 9 );
				return 2;
			}
			//================================================================
			ns = -1;
			//----------------------------------
			// 2007.07.23
			//----------------------------------
			if ( SUB2_CASSETTE_APPEND_IN_SLOT[ i ] == 0 ) {
				ns = SUB2_CASSETTE_APPEND_CAS_INDEX + 1;
				if ( ns > SUB2_INDEX_ROTATE_COUNT ) ns = 1;
				if ( u[ns-1] ) ns = -1;
			}
			//----------------------------------
			if ( ns == -1 ) {
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					if ( !u[j] ) {
						ns = j + 1;
						break;
					}
				}
			}
			if ( ns == -1 ) {
				_SCH_LOG_LOT_PRINTF( side , "APP SELECT - Select Data Error 3 index=%d Pointer=%d with (%d,%d) (%d,%d) %d%cAPP_SELECT_ERROR 3\n" , i , l + 1 , SUB2_CASSETTE_APPEND_IN_CM[ i ] , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] , SUB2_CASSETTE_APPEND_OUT_CM[ i ] , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] , SUB2_CASSETTE_APPEND_ID[ i ] , 9 );
				return 3;
			}
			//================================================================
			if ( _SCH_CLUSTER_Get_PathRange( side , js ) >= 0 ) {
				_SCH_LOG_LOT_PRINTF( side , "APP SELECT - Select Data Error 4 index=%d Pointer=%d with (%d,%d) (%d,%d) %d%cAPP_SELECT_ERROR 4\n" , i , l + 1 , SUB2_CASSETTE_APPEND_IN_CM[ i ] , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] , SUB2_CASSETTE_APPEND_OUT_CM[ i ] , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] , SUB2_CASSETTE_APPEND_ID[ i ] , 9 );
				return 4;
			}
			//=============================================
			_SCH_CLUSTER_Set_PathIn( side , js , SUB2_CASSETTE_APPEND_IN_CM[ i ] );
			_SCH_CLUSTER_Set_User_PathIn( side , js , SUB2_CASSETTE_APPEND_IN_CM[ i ] );
			//=============================================
			if ( SUB2_CASSETTE_APPEND_IN_SLOT[ i ] > 0 ) {
				_SCH_CLUSTER_Set_SlotIn( side , js , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] );
			}
			else {
				_SCH_CLUSTER_Set_SlotIn( side , js , ns );
			}
			_SCH_CLUSTER_Set_User_OrderIn( side , js , SUB2_CASSETTE_APPEND_IN_SLOT[ i ] );
			//=============================================
			if ( SUB2_CASSETTE_APPEND_OUT_CM[ i ] == 0 ) {
				_SCH_CLUSTER_Set_PathOut( side , js , SUB2_CASSETTE_APPEND_IN_CM[ i ] );
				_SCH_CLUSTER_Set_PathHSide( side , js , TRUE );
			}
			else {
				_SCH_CLUSTER_Set_PathOut( side , js , SUB2_CASSETTE_APPEND_OUT_CM[ i ] );
				_SCH_CLUSTER_Set_PathHSide( side , js , FALSE );
			}
			_SCH_CLUSTER_Set_User_PathOut( side , js , SUB2_CASSETTE_APPEND_OUT_CM[ i ] );
			//=============================================
			if ( SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] > 0 ) {
				_SCH_CLUSTER_Set_SlotOut( side , js , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] );
			}
			else {
				_SCH_CLUSTER_Set_SlotOut( side , js , ns );
			}
			_SCH_CLUSTER_Set_User_SlotOut( side , js , SUB2_CASSETTE_APPEND_OUT_SLOT[ i ] );
			//=============================================
			_SCH_CLUSTER_Set_Buffer( side , js , ( SUB2_CASSETTE_APPEND_ID[ i ] * 10 ) + SUB2_CASSETTE_APPEND_AUTOALIGN[ i ] );
			if ( SUB2_CASSETTE_APPEND_OPTIMIZE[i] == -1 ) {
				_SCH_CLUSTER_Set_Optimize( side , js , SUB2_CASSETTE_APPEND_CUR_INDEX ); // 2006.04.04
			}
			else {
				_SCH_CLUSTER_Set_Optimize( side , js , SUB2_CASSETTE_APPEND_OPTIMIZE[i] ); // 2007.09.28
			}
			//=============================================
			for ( j = 0 ; j < MAX_CLUSTER_DEPTH ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					if ( j == 0 ) {
//						if ( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ] == NULL ) { // 2008.04.02 // 2011.11.29
						if ( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][0] == NULL ) { // 2008.04.02 // 2011.11.29
							_SCH_CLUSTER_Set_PathProcessData( side , js , j , k , 0 , "" , "" , "" ); // 2008.04.02
						}
						else {
//							if ( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][0] == 0 ) { // 2011.11.29
							if ( SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][0][0] == 0 ) { // 2011.11.29
								_SCH_CLUSTER_Set_PathProcessData( side , js , j , k , 0 , "" , "" , "" );
							}
							else {
//								_SCH_CLUSTER_Set_PathProcessData( side , js , j , k , k + PM1 , SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ] , "" , "" ); // 2011.11.29
								_SCH_CLUSTER_Set_PathProcessData( side , js , j , k , k + PM1 , SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][0] , SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][1] , SUB2_CASSETTE_APPEND_RECIPE_PM[ i ][ k ][2] ); // 2011.11.29
								//
								if ( SUB2_CASSETTE_APPEND_RECIPE_PM_DISABLE[ i ][ k ] ) { // 2007.08.31
									//
									_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , js , j , k ); // 2003.05.26
									//
								}
							}
						}
					}
					else {
						_SCH_CLUSTER_Set_PathProcessData( side , js , j , k , 0 , "" , "" , "" );
					}
				}
			}
			//=============================================
			_SCH_CLUSTER_Set_FailOut( side , js , 0 );
			_SCH_CLUSTER_Set_SwitchInOut( side , js , 0 );
			//=============================================
			_SCH_CLUSTER_Set_PathDo( side , js , 0 );
			_SCH_CLUSTER_Set_PathStatus( side , js , SCHEDULER_READY );
			//=============================================
			_SCH_CLUSTER_Set_PathRange( side , js , 1 );
			//=============================================
			//=============================================
			_SCH_MODULE_Set_FM_DoPointer( side , 0 );
			_SCH_MODULE_Set_TM_DoPointer( side , 0 );
			//=============================================
			//=============================================
//			SUB2_CASSETTE_APPEND_FLAG[ i ] = FALSE; // 2011.09.07
			//=============================================
			//=============================================
			_SCH_CASSETTE_LAST_RESET( _SCH_CLUSTER_Get_PathIn( side , js ) , _SCH_CLUSTER_Get_SlotIn( side , js ) );
			_SCH_IO_SET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( side , js ) , _SCH_CLUSTER_Get_SlotIn( side , js ) , CWM_PRESENT );
			//
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , _SCH_CLUSTER_Get_PathIn( side , js ) - CM1 , 0 );
			_SCH_IO_SET_MODULE_STATUS( 10000 + 0 , _SCH_CLUSTER_Get_PathIn( side , js ) - CM1 , 0 );
			//=============================================
			SUB2_CASSETTE_APPEND_CAS_INDEX = ns; // 2007.07.23
			//=============================================
			SUB2_CASSETTE_APPEND_OUT_INDEX = i; // 2005.11.25
			SUB2_CASSETTE_APPEND_CUR_INDEX++; // 2006.04.04
			//=============================================
			SCHEDULER_CONTROL_Chk_CASSETTE_MAKE_SLOT_FOLDER_A0SUB2( side , i , _SCH_CLUSTER_Get_SlotIn( side , js ) , TRUE ); // 2006.04.26
			//=============================================
			SUB2_CASSETTE_APPEND_FLAG[ i ] = FALSE; // 2011.09.07
			//=============================================
			_SCH_LOG_LOT_PRINTF( side , "APP SELECT - Regist Data Selected 1 index=%d Pointer=%d with %s(%d)->%s(%d)(%d) %d%cAPP_SELECT CM%d:%d:CM%d:%d:%d:%d%c%d\n" , i , js , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_CLUSTER_Get_PathIn( side , js ) ) , _SCH_CLUSTER_Get_SlotIn( side , js ) , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_CLUSTER_Get_PathOut( side , js ) ) , _SCH_CLUSTER_Get_SlotOut( side , js ) , _SCH_CLUSTER_Get_PathHSide( side , js ) , _SCH_CLUSTER_Get_Buffer( side , js ) , 9 , _SCH_CLUSTER_Get_PathIn( side , js ) - CM1 + 1 , _SCH_CLUSTER_Get_SlotIn( side , js ) , _SCH_CLUSTER_Get_PathOut( side , js ) - CM1 + 1 , _SCH_CLUSTER_Get_SlotOut( side , js ) , _SCH_CLUSTER_Get_PathHSide( side , js ) , _SCH_CLUSTER_Get_Buffer( side , js ) , 9 , _SCH_CLUSTER_Get_SlotIn( side , js ) );
			//=============================================
			return 0;
		}
	}
	return 4;
}
//=======================================================================================
void SCHEDULER_CONTROL_Chk_CASSETTE_REFRESH_A0SUB2( int side ) { // 2007.11.21
	int i;
	//
	if ( _SCH_MODULE_Get_TM_DoPointer( side ) < MAX_CASSETTE_SLOT_SIZE ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( side , i ) == 0 ) {
				if ( _SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) {
					break;
				}
			}
		}
	}
	//
	if ( i == MAX_CASSETTE_SLOT_SIZE ) return;
	//
	_SCH_MODULE_Set_TM_DoPointer( side , 0 );
}
//=======================================================================================
void SCHEDULER_CONTROL_Chk_CASSETTE_CLEAR_A0SUB2( int side , int pt , BOOL normal ) {
	int c , s , w;

	if ( normal ) {
		c = _SCH_CLUSTER_Get_PathOut( side , pt );
		s = _SCH_CLUSTER_Get_SlotOut( side , pt );
		w = _SCH_IO_GET_MODULE_STATUS( c , s );
		//
		_SCH_LOG_LOT_PRINTF( side , "APP CLEAR  - Select Data Cleared Pointer=%d , %s(%d)->%s(%d)%cAPP_CLEAR CM%d:%d:CM%d:%d%c%d\n" , pt , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_CLUSTER_Get_PathIn( side , pt ) ) , _SCH_CLUSTER_Get_SlotIn( side , pt ) , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_CLUSTER_Get_PathOut( side , pt ) ) , _SCH_CLUSTER_Get_SlotOut( side , pt ) , 9 , _SCH_CLUSTER_Get_PathIn( side , pt ) - CM1 + 1 , _SCH_CLUSTER_Get_SlotIn( side , pt ) , _SCH_CLUSTER_Get_PathOut( side , pt ) - CM1 + 1 , _SCH_CLUSTER_Get_SlotOut( side , pt ) , 9 , _SCH_CLUSTER_Get_SlotIn( side , pt ) );
		//
		//=============================================
		SCHEDULER_CONTROL_Chk_CASSETTE_MAKE_SLOT_FOLDER_A0SUB2( side , -1 , _SCH_CLUSTER_Get_SlotIn( side , pt ) , FALSE ); // 2006.04.26
		//=============================================
		switch( w ) {
		case CWM_UNKNOWN : // Unknown
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , c - CM1 , 0 );
			break;
		case CWM_ABSENT : // Absent
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , c - CM1 , 0 );
			break;
		case CWM_PRESENT : // Present
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , c - CM1 , 0 );
			break;
		case CWM_PROCESS : // Process
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , c - CM1 , 1 );
			break;
		default : // fail
			_SCH_IO_SET_MODULE_RESULT( 10000 + 0 , c - CM1 , 2 );
			break;
		}
	}
	else {
		c = _SCH_CLUSTER_Get_PathIn( side , pt );
		s = _SCH_CLUSTER_Get_SlotIn( side , pt );
		//
		_SCH_IO_SET_MODULE_RESULT( 0 , c - CM1 , 0 );
	}
	//=============================================
	_SCH_IO_SET_MODULE_STATUS( 10000 + 0 , c - CM1 , s );
	//=============================================
	_SCH_IO_SET_MODULE_STATUS( c , s , CWM_ABSENT );
}
//=======================================================================================
void SCHEDULER_CONTROL_Chk_CASSETTE_DISABLE_A0SUB2( int side ) { // 2005.11.28
	int i , l , k;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			if ( l == 0 ) { // 2007.09.04
				if ( _SCH_MODULE_GET_USE_ENABLE( _SCH_CLUSTER_Get_PathIn( side , i ) , TRUE , side ) ) continue;
			}
			if ( _SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
			if ( _SCH_CLUSTER_Get_PathDo( side , i ) != 0 ) continue;
			//-------------------------------------------------------------------------------------------
			// DataBackup
			//-------------------------------------------------------------------------------------------
			if ( l != 0 ) {
				switch( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) { // 2007.12.20
				case 6 :
				case 7 :
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( _SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , k ) > 0 ) break;
					}
					if ( k != MAX_PM_CHAMBER_DEPTH ) continue;
					break;
				default :
					continue;
					break;
				}
			}
			//-------------------------------------------------------------------------------------------
			// Clear & Append
			//-------------------------------------------------------------------------------------------
			SCHEDULER_CONTROL_Return_CASSETTE_Operation_A0SUB2( side , i , ( l == 0 ) ? 1 : 2 , -1 );
			//-------------------------------------------------------------------------------------------
		}
	}
}

char *SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( int slot ) {
//	return SUB2_CASSETTE_FOLDER[ slot - 1 ]; // 2010.03.25
	return STR_MEM_GET_STR( SUB2_CASSETTE_FOLDER[ slot - 1 ] ); // 2010.03.25
}


void SCHEDULER_CONTROL_Return_CASSETTE_Operation_A0SUB2( int side , int pt , int mode , int och ) {
	int incm;
	int outcm;
	int inslot;
	int outslot;
	int spid;
	int autoal;
	int opt;
	int s;
	//
	int normalmode;
	//
	char pm1recipe[65];
	char pm2recipe[65];
	char pm3recipe[65];
	char pm4recipe[65];
	char pm5recipe[65];
	char pm6recipe[65];

	char Logfolder[256]; // 2011.09.07

	if ( ( mode != 0 ) || ( _SCH_CLUSTER_Get_SupplyID( side , pt ) <= -9998 ) ) {
		if ( och < 0 ) {
			incm = _SCH_CLUSTER_Get_User_PathIn( side , pt );
		}
		else { // 2007.09.28
			incm = och;
		}
		//
		inslot  = _SCH_CLUSTER_Get_User_OrderIn( side , pt );
		outcm   = _SCH_CLUSTER_Get_User_PathOut( side , pt );
		outslot = _SCH_CLUSTER_Get_User_SlotOut( side , pt );
		//
		spid    = _SCH_CLUSTER_Get_Buffer( side , pt ) / 10;
		autoal  = _SCH_CLUSTER_Get_Buffer( side , pt ) % 10;
		opt		= _SCH_CLUSTER_Get_Optimize( side , pt );

		strncpy( pm1recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 0 , 0 ) , 64 );
		strncpy( pm2recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 1 , 0 ) , 64 );
		strncpy( pm3recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 2 , 0 ) , 64 );
		strncpy( pm4recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 3 , 0 ) , 64 );
		strncpy( pm5recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 4 , 0 ) , 64 );
		strncpy( pm6recipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pt , 0 , 5 , 0 ) , 64 );
		//
		if      ( ( mode == 0 ) && ( _SCH_CLUSTER_Get_SupplyID( side , pt ) == -9999 ) ) {
			normalmode = 2;
		}
		else if ( ( mode == 0 ) && ( _SCH_CLUSTER_Get_SupplyID( side , pt ) == -9998 ) ) {
			normalmode = 4;
		}
		else {
			normalmode = 3;
		}
	}
	else {
		normalmode = 0;
		opt = -1;
	}
	//
	s = _SCH_CLUSTER_Get_SlotIn( side , pt );
	if ( SUB2_CASSETTE_FOLDER_MODE[ s - 1 ] && ( SUB2_CASSETTE_FOLDER[ s - 1 ] != NULL ) && ( SUB2_CASSETTE_FOLDER[ s - 1 ][0] != 0 ) ) { // 2011.09.07
		strncpy( Logfolder , SUB2_CASSETTE_FOLDER[ s - 1 ] , 255 );
	}
	else {
		strcpy( Logfolder , "" );
	}
	//
	SCHEDULER_CONTROL_Chk_CASSETTE_CLEAR_A0SUB2( side , pt , ( normalmode == 0 ) );
	SCHEDULER_CONTROL_Reset_CASSETTE_DATA_A0SUB2( side , pt , TRUE );

	//-------------------------------------------------------------------------------------------
	// Append
	//-------------------------------------------------------------------------------------------
	if      ( mode == 1 ) {
		SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2_PLUSDLL( -1 , 1 , 0 , -1 , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe , Logfolder );
	}
	else if ( normalmode != 0 ) {
		SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2_PLUSDLL( -1 , normalmode , ( autoal == 0 ) ? 1 : 2 , opt , incm , outcm , inslot , outslot , spid , pm1recipe , pm2recipe , pm3recipe , pm4recipe , pm5recipe , pm6recipe , Logfolder );
	}
}


void SCHEDULER_CONTROL_Chk_DISABLE_DATABASE_SKIP_A0SUB2( int side , int pt ) { // 2011.01.18
	_SCH_MODULE_Inc_TM_DoPointer( side );
}
